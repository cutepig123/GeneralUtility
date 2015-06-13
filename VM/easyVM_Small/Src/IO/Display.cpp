//////////////////////////////////////////////////////////////////////////
// 基本思想：根据虚拟机内存中的显存来填Disp自己的缓冲区，判断是否有更新，再把缓冲区的内容显示到窗口上

//使用GID+的时候就定义_GDIP_
//#define _GDIP_

#include "StdAfx.h"
#include "Display.h"
#include "KeyBoard.h"
#include "RTC.h"
#include "SystemTimer.h"
#include <math.h>

#define TICK 54.9255


#define CharWidth ((float)rect.right/(*TextSolutionX))				//我真的不知道为什么窗口水平像素除以文本水平字符数不等于音字符宽度
#define CharHeigh ((float)rect.bottom/TextSolutionY)
#define MemCurCur (*(unsigned char *)(MemoryStart+TextMemory+(CurLine*(*TextSolutionX)+CurRow)*2))
#define MemCurCurP1 (*(unsigned char *)(MemoryStart+TextMemory+(CurLine*(*TextSolutionX)+CurRow)*2+1))
#define BitPlane(a) (*(unsigned char *)(DispGrapBuf+(a)*64000+DrawP/8))
//#define VideoRAM GrapBufStart								//尝试把VideoRAM和0xa0000重叠起来，而不另开空间


//刷新频率
#define DispHZ 50
#define TextMemory 0xB8000
#define GrapMemory 0xA0000


// Disp的内部寄存器
char *DispMode;					//当前的显示模式，参看 INT 10H，功能号00H
char *TextBufStart;				//文本模式显存的实际起始位置
char *GrapBufStart;				//图形模式显存的实际起始位置
char CurLine,CurRow,*CurPage;	//文本模式下当前的光标位置及页号
short *TextSolutionX,TextSolutionY,GrapSolutionX,GrapSolutionY,GrapSolutionColor;			//分辨率
RECT rect;						//当前窗口的大小
unsigned short CurVisCount;
double CurHight;				//光标占字符高的比例，最大为1
double CurDispHZ;				//光标每秒闪多少次，一亮一暗为两闪。
char CursorTop,CursorBottom;	//光标的顶线、底线
bool CursorVisible;				//光标是否可见
bool colorized;					//当前显示模式是否彩色

char DispCombinCode;			//显示组合码 为0x8
unsigned char GraphicsControlRegister[9];				//由端口3CE所选择，3CF所读写。图形控制器寄存器
unsigned char GraphicsControlRegisterPointer;			//由端口3CE填写。

int DispModeChanged();


//gdi+用到的两个变量
GdiplusStartupInput m_gdiplusStartupInput;
ULONG_PTR m_pGdiToken;

HINSTANCE hInstance,hInst;
char *szWindowClass="easyVM_Disp";
char *szTitle="easyVM";
DWORD ThreadID;
HWND hWnd;			//窗口句柄
HWND hMyWnd;		//窗口句柄
bool NeedRefresh;	//虚拟机的显存是否有更新
char DispTextBuf[4*1024];			//用于检查是否需要更新屏幕
char DispGrapBuf[256000];			//并不是整个Buf都要使用，具体要看ModeBufSize
char VideoRAM[256000];				//当对0xa0000－0xbffff这段范围操作的时候，其实是在操作显存

int ModeBufSize[0x14]={2048,2048,4096,4096,16000,16000,16000,4096,16000,32000,32000,0,0,32000,64000,28000,224000,38400,153600,64000};			//各种显示模式下所需要的显存的大小


// 对应于3CEh端口的OUT指令
// 图形模式正在开发当中
void IO_Write_03CE(DWORD data)
{
	GraphicsControlRegisterPointer=data;
}

void IO_Write_03CF(DWORD data)
{
	GraphicsControlRegister[GraphicsControlRegisterPointer]=data;
	unsigned char tuc;
}

void ReadVideoRam(unsigned short addr, unsigned short len)
{

}

void WriteVideoRam(unsigned short addr)
{
	unsigned char planbit;
	int i,j;
	for (i=0;i<4;i++)
	{
		VideoRAM[addr+i*64000]=*(GrapBufStart+addr+i*64000);
		if (GraphicsControlRegister[1] & (1<<i))
		{
			VideoRAM[addr+i*64000]&=0;
			if (GraphicsControlRegister[0] & (1<<i))
				planbit=0xf;
			else
				planbit=0;
			planbit+=planbit<<4;
			planbit&=GraphicsControlRegister[8];
			VideoRAM[addr+i*64000]|=planbit;
		}
	}
}

// 设置文本模式下的一些参数
void TextMode(short x, short y, bool color)
{
	colorized=color;
	(*TextSolutionX)=x;
	TextSolutionY=y;
	CurHight=0.08;
	CurDispHZ=0.4;
	memset(TextBufStart,0,ModeBufSize[*DispMode]);
}

int CurCurChanged()
{
	*(char *)(MemoryStart+0x450+(*CurPage)*2)=CurRow;
	*(char *)(MemoryStart+0x450+(*CurPage)*2+1)=CurLine;
	return 0;
}

int CurPageChanged()
{
	*(unsigned short *)(MemoryStart+0x44e)=(*CurPage)*ModeBufSize[(*DispMode)];
	return 0;
}

//设置图形模式下的一些参数
void VgaMode(unsigned short x, unsigned short y, unsigned short color)
{
	GrapSolutionX=x;
	GrapSolutionY=y;
	GrapSolutionColor=color;
	DispCombinCode=0x8;	
}

void ClearScreen()
{
	PAINTSTRUCT ps;	
	HDC hdc = BeginPaint(hMyWnd, &ps);
	Graphics    rgraphics(hdc);
	//SolidBrush	GrapBrush(Color(255, 0, 0, 0));
	rgraphics.Clear(Color(255,255,255,255));
	EndPaint(hMyWnd, &ps);
}

// 每当显示模式改变的时候，重新设置一些全局变量
int DispModeChanged()
{
	CurLine=0;CurRow=0;(*CurPage)=0;
	switch((*DispMode))
	{
	case 0:
		TextMode(40,25,false);
		break;
	case 1:
		TextMode(40,25,true);
		break;
	case 2:
		TextMode(80,25,false);
		break;
	case 3:
		TextMode(80,25,true);
		break;
	case 4:
		VgaMode(320,200,4);
		break;
	case 5:
		VgaMode(320,200,2);
		break;
	case 6:
		VgaMode(640,200,2);
		break;
	case 7:
		TextMode(80,25,false);
		break;
	case 8:
		VgaMode(160,200,16);
		break;
	case 9:
		VgaMode(320,200,16);
		break;
	case 0xa:
		VgaMode(640,200,4);
		break;
	case 0xd:
		VgaMode(320,200,16);
		break;
	case 0xe:
		VgaMode(640,200,16);
		break;
	case 0xf:
		VgaMode(640,350,2);
		break;
	case 0x10:
		VgaMode(640,350,16);
		break;
	case 0x11:
		VgaMode(640,480,2);
		break;
	case 0x12:
		VgaMode(640,480,16);
		break;
	case 0x13:
		VgaMode(320,200,256);
		break;		
	}
	*(short *)(MemoryStart+0x44c)=ModeBufSize[(*DispMode)];	
	InvalidateRect(hMyWnd,&rect,true);
	return 0;
}

void TextUpRollOneLine()
{	
	int i;
	memcpy(TextBufStart+(*CurPage)*(*TextSolutionX)*TextSolutionY*2,TextBufStart+(*CurPage)*(*TextSolutionX)*TextSolutionY*2+(*TextSolutionX)*2,(*TextSolutionX)*(TextSolutionY-1)*2);
	unsigned short lastAttrib=*(unsigned char *)(TextBufStart+(*CurPage)*(*TextSolutionX)*TextSolutionY*2+(*TextSolutionX)*(TextSolutionY-1)*2-1);
	unsigned short NewLineFill=lastAttrib*0x100+0x20;
	for (i=0;i<(*TextSolutionX);i+=2)
	{
		*(unsigned short *)((unsigned int)(TextBufStart+(*CurPage)*(*TextSolutionX)*TextSolutionY*2+(*TextSolutionX)*(TextSolutionY-1)*2)+i)=NewLineFill;		//黑底白字的空格	//4页，每页占2KB
	}
}

void Push_Char(unsigned char tempc)
{
	(*CurPage)=eCPU.bh;
	if (tempc==0x8)					//退格
	{
		CurRow--;
		if (CurRow<0)
		{
			CurRow=(char)(*TextSolutionX);
			CurLine--;
			if (CurLine<0)
			{
				CurLine=0;
			}
		}
		MemCurCur=' ';
	}
	else if (tempc==0xa)			//换行
	{
		while (CurRow<(*TextSolutionX))
		{
			MemCurCur=' ';
			MemCurCurP1=eCPU.bl;
			CurRow++;
		}
	}
	else
	{
		MemCurCur=tempc;
		MemCurCurP1=eCPU.bl;
		CurRow++;
	}
	if (CurRow>=(*TextSolutionX))
	{
		CurRow=0;
		CurLine++;
		if (CurLine>=TextSolutionY)
		{
			TextUpRollOneLine();
			CurLine--;
		}
	}
}

// 整个INT 10H中断，用一个In指令调用。
// 因为如果用3B0的话就要先改变DX的值，所以就用B0了。
void IO_Write_00B0()			
{
	int i,j,k;
	char tempc;
	unsigned short treg;
	switch(eCPU.ah)
	{
	case 0:
		(*DispMode)=eCPU.al;
		DispModeChanged();
		break;
	case 1:
		CursorTop=eCPU.ch&0xf;
		CursorBottom=eCPU.cl&0xf;
		CurHight=(double)(CursorBottom-CursorTop)/7;
		CursorVisible=((eCPU.ch&0x60)==0);
		break;
	case 2:
		(*CurPage)=eCPU.bh;
		CurLine=eCPU.dh;
		CurRow=eCPU.dl;
		CurCurChanged();
		break;
	case 3:
		eCPU.ch=CursorTop;
		eCPU.cl=CursorBottom;
		eCPU.dh=CurLine;
		eCPU.dl=CurRow;
		break;
	case 4:
		__asm nop
		break;
	case 5:
		(*CurPage)=eCPU.al;
		CurPageChanged();
		break;
	case 6:
		if (eCPU.al==0)
		{
			for (i=0;i<(*TextSolutionX)*TextSolutionY;i++)
			{
				*(unsigned char *)(MemoryStart+TextMemory+i*2)=0x20;
				*(unsigned char *)(MemoryStart+TextMemory+i*2+1)=eCPU.bh;
			}			
			break;
		}
		for (i=0;i<eCPU.dh-eCPU.ch;i++)
		{
			for (j=eCPU.cl;j<=eCPU.dl;j++)
			{
				*(short *)(MemoryStart+TextMemory+((i+eCPU.ch)*(*TextSolutionX)+j)*2)=*(short *)(MemoryStart+TextMemory+((i+eCPU.ch+eCPU.al)*(*TextSolutionX)+j)*2);
			}
		}
		for (i=0;i<eCPU.al;i++)
		{
			for (j=eCPU.cl;j<=eCPU.dl;j++)
			{
				*(char *)(MemoryStart+TextMemory+((i+eCPU.dh-eCPU.al+1)*(*TextSolutionX)+j)*2)=' ';
				*(char *)(MemoryStart+TextMemory+((i+eCPU.dh-eCPU.al+1)*(*TextSolutionX)+j)*2+1)=eCPU.bh;
			}
		}
		break;
	case 7:
		if (eCPU.al==0)
		{
			for (i=0;i<(*TextSolutionX)*TextSolutionY;i++)
			{
				*(unsigned char *)(MemoryStart+TextMemory+i*2)=0x20;
				*(unsigned char *)(MemoryStart+TextMemory+i*2+1)=eCPU.bh;
			}			
			break;
		}
		for (i=eCPU.dh-eCPU.ch-1;i>=0;i--)
		{
			for (j=eCPU.cl;j<=eCPU.dl;j++)
			{
				*(short *)(MemoryStart+TextMemory+((i+eCPU.ch+eCPU.al)*(*TextSolutionX)+j)*2)=*(short *)(MemoryStart+TextMemory+((i+eCPU.ch)*(*TextSolutionX)+j)*2);
			}
		}
		for (i=eCPU.al-1;i>=0;i--)
		{
			for (j=eCPU.cl;j<=eCPU.dl;j++)
			{
				*(char *)(MemoryStart+TextMemory+((i+eCPU.ch)*(*TextSolutionX)+j)*2)=' ';
				*(char *)(MemoryStart+TextMemory+((i+eCPU.ch)*(*TextSolutionX)+j)*2+1)=eCPU.bh;
			}
		}
		break;
	case 8:
		eCPU.ah=*(unsigned char *)(MemoryStart+TextMemory+(((*CurPage)*TextSolutionY+CurLine)*(*TextSolutionX)+CurRow)*2+1);
		eCPU.al=*(unsigned char *)(MemoryStart+TextMemory+(((*CurPage)*TextSolutionY+CurLine)*(*TextSolutionX)+CurRow)*2);
		break;
	case 9:
	case 0xa:
		for (i=0;i<eCPU.cx;i++)
		{
			if (eCPU.ah==9)
				MemCurCurP1=eCPU.bl;
			MemCurCur=eCPU.al;
			CurRow++;
			if (CurRow>=(*TextSolutionX))
			{
				CurRow=0;
				CurLine++;
			}
		}
		break;
	case 0xb:
	case 0xc:
	case 0xd:
		__asm nop
		break;
	case 0xe:
		treg=eCPU.bx;
		eCPU.bl=07;
		Push_Char(eCPU.al);
		eCPU.bx=treg;
		break;
	case 0xf:
		eCPU.ah=(unsigned char)(*TextSolutionX);
		eCPU.al=(*DispMode);
		eCPU.bh=(*CurPage);
		break;
	case 0x11:
		switch(eCPU.al)
		{
		case 0x30:
			switch(eCPU.bh)
			{
			case 0x00:
				eCPU.es=*(unsigned short *)(MemoryStart+0x1f*4+2);
				eCPU.bp=*(unsigned short *)(MemoryStart+0x1f*4);				
				break;
			}
			eCPU.cx=0x10;			//从Bochs跟出来的，不知具体意思
			eCPU.dl=TextSolutionY-1;
			break;
		}
		break;
	case 0x12:
		switch(eCPU.bl)
		{
		case 0x10:
			eCPU.bh=0;
			eCPU.bl=3;
			eCPU.cl=7;
			break;
		}
		break;
	case 0x13:
		i=(MemoryStart+(i=eCPU.es,i<<=4)+eCPU.bp);
		CurLine=eCPU.dh;
		CurRow=eCPU.dl;
		for (k=0;k<eCPU.cx;k++)
		{
			tempc=*(char *)i;
			i++;
			Push_Char(tempc);
		}
		break;
	case 0x1a:
		switch(eCPU.al)
		{
		case 00:
			eCPU.al=0x1a;
			eCPU.bl=DispCombinCode;
			eCPU.bh=0;
			break;
		case 01:
			break;
		}
		break;
	default:
		__asm nop
		break;
	}
	CurCurChanged();
}


//根据颜色（4位）属性得出颜色值（DWORD）
DWORD AttrToColor(unsigned char a, bool spark)
{
	DWORD ans=0,bit;
	int i;
	bool HeightLight;
	HeightLight=a&0x8;
	for (i=0;i<3;i++)				//红绿蓝
	{
		if (a&4)
		{
			if (HeightLight)
				bit=(0xff);
			else
				bit=(0xbf);
			if (spark)				//闪
				bit*=(float)abs(DispHZ-(CurVisCount/4%DispHZ*2))/DispHZ;
			ans|=bit<<(i*8);
		}
		a<<=1;		
	}
	return ans;
}

//这是GDI+版本
DWORD AttrToColorP(unsigned char a, bool spark)
{
	DWORD ans=0;
	int i;
	for (i=0;i<3;i++)				//红绿蓝
	{
		if (a&1)
			ans|=(0xbf<<(i*8));
		a>>=1;
	}
	if (a&1)						//高亮
		ans|=0xff404040;
	else
		ans|=0xff000000;
	return ans;
}

//本函数未使用
int OutHighAscii(HDC hdc, float *x, float *y, unsigned char *Txt, int *Txtlen, float CharWide)
{
	int		HighNum=0;	
	char	HighAscii[2];
	HighAscii[1]=0xcc;										//这样组合可以打印出扩展ASCII码
	while (Txt[0]>0x7f && Txtlen>0)
	{
		HighAscii[0]=Txt[0];
		TextOut(hdc,*x,*y,HighAscii,2);
		HighNum++;
		*x+=CharWide;
		Txt++;
		*Txtlen--;
	}
	return HighNum;
}

//本函数未使用
int OutLowAscii(HDC hdc, float *x, float *y, unsigned char *Txt, int *Txtlen, float CharWide)
{

	int		LowNum=0;	
	char	LowAscii[2];
	HFONT	hFont,holdfont;

	hFont=CreateFont((float)rect.bottom/TextSolutionY,0,0,0,FW_MEDIUM,   
		FALSE,FALSE,FALSE,DEFAULT_CHARSET,   
		OUT_RASTER_PRECIS,CLIP_DEFAULT_PRECIS,   
		VARIABLE_PITCH|PROOF_QUALITY,FF_DONTCARE,"Lucida Console");
	SelectObject(hdc,hFont); 

	LowAscii[1]=0x0;										//这样组合可以打印出扩展ASCII码
	while (Txt[0]<0x20  && Txt[0]!=0x0d && Txt[0]!=0x0a && Txt[0]!=0x00 && Txt[0]!=0x07 && Txtlen>0)
	{
		LowAscii[0]=Txt[0];
		holdfont=(HFONT)SelectObject(hdc,GetStockObject(OEM_FIXED_FONT));
		TextOut(hdc,*x,*y,LowAscii,1);
		SelectObject(hdc,holdfont);
		LowNum++;
		*x+=CharWide;
		(Txt)++;
		*Txtlen--;
	}
	return LowNum;	
}


// 把DispTextBuf的内容输出到屏幕上去
// 对于属性相同的连续的字符，是以字符串而不是字符的形式TextOut的
int TextBufOut()
{
	PAINTSTRUCT ps;	
	HDC hdc = BeginPaint(hMyWnd, &ps);
	Graphics    rgraphics(hdc);
	SolidBrush  brush(Color(255, 255, 255, 255));	
	FontFamily  fontFamily(L"Courier New");
#ifdef _GDIP_
	double		WHscale=0.54112554;									//Courier New字体的宽高比
#else
	double		WHscale=0.5;
#endif	
	Font        font(&fontFamily, (float)rect.bottom/TextSolutionY*0.882759, FontStyleBold, UnitPixel);			//0.882759=font.GetSize()/font.GetHeight(0.0);  这是对应于Courier New字体的值。	
	SolidBrush	CurBrush(Color(255, 255, 255, 255));

	Bitmap		bmpBuf(rect.right,rect.bottom);							//双缓冲
	Graphics	graphics(&bmpBuf);	

	PointF      pointF(0.0f, 0.0f);
	float		tempFX;
	float		CurX,CurY;

	wchar_t		ws[100];
	int			lao;
	
//////////////////////////////////////////////////////////////////////////
	HFONT   hFont;
	hFont=CreateFont((float)rect.bottom/TextSolutionY,0,0,0,FW_MEDIUM,   
		FALSE,FALSE,FALSE,DEFAULT_CHARSET,   
		OUT_RASTER_PRECIS,CLIP_DEFAULT_PRECIS,   
		VARIABLE_PITCH|PROOF_QUALITY,FF_DONTCARE,"Lucida Console");
	SelectObject(hdc,hFont);   
//////////////////////////////////////////////////////////////////////////
	
	
	//下面被注释掉的语句都是用GDI+时使用的。
	//graphics.Clear(Color(0,0,0));									//清除原先的内容。
	
	int pTextBufCur=0;
#ifdef _GDIP_
	unsigned short Txt[80*25+1]={L"梁一信"};
#else
	char Txt[80*25+1]={"梁一信"};
#endif
	pointF.Y=0;
	for (int i=0;i<TextSolutionY;i++)
	{		
		int TxtLen=0;
		int PTxtLen=0;		
		char CurCharAttr=DispTextBuf[i*(*TextSolutionX)*2+1];	//第一个字符的属性
		pointF.X=0;
		for (int j=0;j<(*TextSolutionX);j++)
		{
			if (CurCharAttr!=DispTextBuf[pTextBufCur+1])			//遇到不同属性，要输出字符串了
																									//不在串开关的LowAscii将不得不输出，所以每遇到LowAscii都要重起一串
			{
#ifdef _GDIP_
				pointF.X=(float)(PTxtLen)*font.GetHeight(0.0)*WHscale;//(rect.right/(*TextSolutionX));
				pointF.Y=font.GetHeight(0.0)*(i);
#else				
				GetCharWidthFloat(hdc,'A','A',&tempFX);
				pointF.X+=(float)(PTxtLen)*16*tempFX;
#endif
				PTxtLen=TxtLen;
				
#ifdef _GDIP_
				brush.SetColor(AttrToColorP(CurCharAttr&0x0f),CurCharAttr&0x80);
				graphics.DrawString(Txt,TxtLen,&font,pointF,&brush);
#else
				SetTextColor(hdc,AttrToColor(CurCharAttr&0x0f,CurCharAttr&0x80));
				SetBkColor(hdc,AttrToColor(((CurCharAttr&0xf0)>>4)&0x07,false)&0xffffff);				
// 				if (Txt[0]>=0xa9 && Txt[0]<=0xdf)					//只输出扩展Ascii里面的制表符。其实easyVM里应该有一个地方来设定是否可以输出中文
// 					OutHighAscii(hdc,&pointF.X,&pointF.Y,Txt,&TxtLen,(float)16*tempFX);				
// 				if ((unsigned char)Txt[0]<0x20 && Txt[0]!=0x00)									//输出箭头和心形等
// 				{
// 					lao=OutLowAscii(hdc,&pointF.X,&pointF.Y,(unsigned char *)Txt,&TxtLen,(float)16*tempFX);
// 					//pTextBufCur+=lao*2;
// 				}
// 				else
// 					lao=0;
				MultiByteToWideChar(437,0,Txt,TxtLen,ws,100);
				TextOutW(hdc,pointF.X,pointF.Y,ws,TxtLen);
#endif								
				TxtLen=0;
				CurCharAttr=DispTextBuf[pTextBufCur+1];
			}
			//else													//遇到相同属性，把它加到Txt后面就可以了
			{
#ifdef _GDIP_
				if (DispTextBuf[pTextBufCur+1]>0 && DispTextBuf[pTextBufCur+1]<0x80)	//ASCII码
#endif
				{				
					Txt[TxtLen]=DispTextBuf[pTextBufCur];
					Txt[++TxtLen]=0;
					pTextBufCur+=2;
				}
#ifdef _GDIP_
				else																//Unicode字符
				{
					Txt[TxtLen]=(unsigned char)DispTextBuf[pTextBufCur];
					Txt[TxtLen]<<=8;
					pTextBufCur+=2;
					j++;
					Txt[TxtLen]+=(unsigned char)DispTextBuf[pTextBufCur];
					Txt[++TxtLen]=0;
					pTextBufCur+=2;
				}
#endif
			}	
		}

#ifdef _GDIP_
		pointF.X=(float)(j-TxtLen)*font.GetHeight(0.0)*WHscale;
		pointF.Y=font.GetHeight(0.0)*(i);
#else
		GetCharWidthFloat(hdc,'A','A',&tempFX);
		pointF.X=(float)(j-TxtLen)*16*tempFX;
#endif
		
#ifdef _GDIP_
		brush.SetColor(AttrToColorP(CurCharAttr&0x0f,CurCharAttr&0x80));
		graphics.DrawString(Txt,TxtLen,&font,pointF,&brush);		
#else
		SetTextColor(hdc,AttrToColor(CurCharAttr&0x0f,CurCharAttr&0x80));
		SetBkColor(hdc,AttrToColor(((CurCharAttr&0xf0)>>4)&0x07,false)&0xffffff);		
// 		if (Txt[0]>=0xa9 && Txt[0]<=0xdf)					//只输出扩展Ascii里面的制表符。其实easyVM里应该有一个地方来设定是否可以输出中文
// 			OutHighAscii(hdc,&pointF.X,&pointF.Y,Txt,&TxtLen,(float)16*tempFX);
// 		if ((unsigned char)Txt[0]<0x20 && Txt[0]!=0x00)									//输出箭头和心形等
// 		{
// 			lao=OutLowAscii(hdc,&pointF.X,&pointF.Y,(unsigned char *)Txt,&TxtLen,(float)16*tempFX);
// 			//pTextBufCur+=lao*2;
// 		}
// 		else
// 			lao=0;
		MultiByteToWideChar(437,0,Txt,TxtLen,ws,100);
		TextOutW(hdc,pointF.X,pointF.Y,ws,TxtLen);
		pointF.Y+=(int)font.GetHeight(0.0);							//pointF.Y是浮点，TextOut的参数是整形，浮点误差的积累会产生黑线
#endif
	}
	if (CurVisCount/(unsigned short)(DispHZ*CurDispHZ)%2 && CursorVisible)			//显示光标
	{		
#ifdef _GDIP_
		CurX=(float)(CurRow+0.5)*font.GetHeight(0.0)*WHscale;
		CurY=font.GetHeight(0.0)*(CurLine+(1-CurHight)-0.1);		//把光标抬高0.1行比较好看		
#else
		GetCharWidthFloat(hdc,'A','A',&tempFX);
		CurX=(float)(CurRow)*16*tempFX;
		CurY=(int)(font.GetHeight(0.0))*(CurLine+(1-CurHight)-0.1);		//把光标抬高0.1行比较好看		
#endif		
		graphics.FillRectangle(&CurBrush,CurX+1,CurY,(font.GetHeight(0.0)*WHscale+1),font.GetHeight(0.0)*CurHight);
	}

	rgraphics.DrawImage(&bmpBuf,0,0);
	EndPaint(hMyWnd, &ps);
	
	return 0;
}

// 图形模式开发中......
int GrapBufOut()
{
	PAINTSTRUCT ps;	
	HDC hdc = BeginPaint(hMyWnd, &ps);
	Graphics    rgraphics(hdc);
	SolidBrush	GrapBrush(Color(255, 255, 255, 255));

	Bitmap		bmpBuf(rect.right,rect.bottom);							//双缓冲
	Graphics	graphics(&bmpBuf);	

	PointF      pointF(0.0f, 0.0f);
	

	int i,j,k,l;
	int DrawP=-1;
	unsigned char tdc;
// 	unsigned char ColorSolutionBits;
// 	ColorSolutionBits=log(GrapSolutionColor)/log(2);

	float GrapCellX,GrapCellY;
	GrapCellX=(float)rect.right/GrapSolutionX;
	GrapCellY=(float)rect.bottom/GrapSolutionY;
	pointF.X=0;pointF.Y=0;


	for (i=0;i<GrapSolutionY;i++)
	{
		pointF.X=0;
		for (j=0;j<GrapSolutionX;j++)
		{
			DrawP++;
			for (k=0;k<4;k++)
			{
				if (VideoRAM[DrawP/8+k*64000]!=DispGrapBuf[DrawP/8+k*64000])
					break;
			}
			if (k!=4)
			{
				for (k=0;k<4;k++)
				{
					//更新该位
					DispGrapBuf[DrawP/8+k*64000]&=~(1<<(7-DrawP%8));
					DispGrapBuf[DrawP/8+k*64000]|=VideoRAM[DrawP/8+k*64000]&(1<<(7-DrawP%8));
				}
			
// 				switch (GrapSolutionColor)
// 				{
// 				case 2:
// 					tdc=*(unsigned char *)(DispGrapBuf+DrawP/8);
// 					GrapBrush.SetColor(Color((tdc)&(1<<(DrawP%8))*0x40+0xbf,255,255,255));
// 					break;
// 				case 4:
// 					tdc=*(unsigned char *)(DispGrapBuf+DrawP/8);
// 					GrapBrush.SetColor(Color(((tdc)&(3<<(DrawP%8))>>6)*0x10+0xbf,255,255,255));		//4灰度
// 					break;
// 				case 16:
					tdc=0;
					for (k=3;k>=0;k--)
					{
						if ((BitPlane(k)&(0x1<<(7-DrawP%8))))
							tdc|=(1<<k);
					}
					GrapBrush.SetColor(AttrToColorP(tdc,false));					
// 					break;
// 				}
				graphics.FillRectangle(&GrapBrush,pointF.X,pointF.Y,GrapCellX,GrapCellY);				
			}
			pointF.X+=GrapCellX;
		}
		pointF.Y+=GrapCellY;
	}	
	rgraphics.DrawImage(&bmpBuf,0,0);
	EndPaint(hMyWnd, &ps);
	
	return 0;
}


// 负责把自己的缓冲区里的内容用一条Draw命令显示到窗口上。
VOID OnPaint()
{
	switch((*DispMode))
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 7:
		TextBufOut();
		break;
	case 0x4:
	case 0x5:
	case 0x6:
	case 0x8:
	case 0x9:
	case 0xa:
	case 0xd:
	case 0xe:
	case 0xf:
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:		
		GrapBufOut();
		break;
	}
}





// GUI的窗口函数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	RECT trect;
	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		break;
 	case WM_PAINT:		
 		OnPaint();		
 		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		exit(0);
		break;
	case WM_SIZE:				
		rect.right=lParam & 0xffff;
		rect.bottom=lParam / 0x10000;
		break;
	case WM_SIZING:
		//SetWindowPos(hWnd,NULL,rect.left,rect.top,rect.right,rect.right/2,NULL);
		break;
	case WM_KEYDOWN:			//Bochs和NTVDM都只处理了MakeCode
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		RecvKeyBoardMsg(message,wParam,lParam);
		break;
	case WM_SETFOCUS:
		RefreshKeyboardState();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}




// 在Win32 APP例程里拷过来的函数，无视……
ATOM MyRegisterClass()
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 
	
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(8);//(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

// 在Win32 APP例程里拷过来的函数，无视……
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; 

   hWnd = CreateWindow(szWindowClass, szTitle,WS_THICKFRAME| WS_OVERLAPPED| WS_CAPTION| WS_SYSMENU| WS_MINIMIZEBOX| WS_MAXIMIZEBOX ,
      CW_USEDEFAULT, 0, 888, 484, NULL, NULL, hInstance, NULL);			//这个高宽比是针对 "Courier New"

   if (!hWnd)
   {
      return FALSE;
   }
   hMyWnd=hWnd;

   ShowWindow(hWnd, SW_SHOW);
   UpdateWindow(hWnd);

   return TRUE;
}






// 刷新Disp自己的缓冲区（不是内存中的那个显示缓冲区）
// 是否需要刷新的判断就在这里做
VOID CALLBACK DispRefresh( HWND hWnd, UINT message, UINT idTimer, DWORD dwTime)
{
	int i,j;
	RECT trect;
	memcpy(&trect,&rect,sizeof(rect));
	trect.top=0;
	trect.bottom=1;

	CurVisCount++;
	if (*DispMode<0x4 || *DispMode==0x7)
	{
		//文本模式		
		if(memcmp(TextBufStart,DispTextBuf,ModeBufSize[(*DispMode)]) || CurVisCount%(unsigned short)(DispHZ*CurDispHZ)==0)		//每两秒闪一下
		{
			memcpy(DispTextBuf,TextBufStart,ModeBufSize[(*DispMode)]);
			InvalidateRect(hMyWnd,&rect,false);
		}		
	}
	else
	{
		//图形模式
// 		int BufOneLine=ModeBufSize[(*DispMode)]/GrapSolutionY;
// 		for (i=0;i<GrapSolutionY;i++)
		if (CurVisCount%(unsigned short)(DispHZ*CurDispHZ)==0)
			memset(DispGrapBuf,0,sizeof(DispGrapBuf));			//这里只是为了刷新
		
		if(memcmp(VideoRAM,DispGrapBuf,sizeof(DispGrapBuf)))		//这里借用CurVisCount来刷新
		{
			//memcpy(DispGrapBuf,VideoRAM,ModeBufSize[(*DispMode)]);
			//修改GispGrapBuf的动作在OnPaint－》GrapBufOut里进行，只修改和刷新改变了的部分
			InvalidateRect(hMyWnd,&rect,false);
		}				
	}
}

int OtherInitNeedMainWindow()
{
	SetTimer(NULL,NULL,TICK,(TIMERPROC)RTCTimerThread);				//这里会把TICK强制转换成54,虚拟机运行久了以后会造成时间超前
	SetTimer(NULL,2,TICK,(TIMERPROC)SysTimerThread);
	return 0;
}

// 创建窗口
int MakeWindow()
{
	MSG msg;
	//HACCEL hAccelTable;
	
	MyRegisterClass();
	InitInstance(hInstance,NULL);
	SetTimer(hMyWnd,1,1000/DispHZ,(TIMERPROC) DispRefresh);
	OtherInitNeedMainWindow();

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;	
}

// GUI结束，供easyVM调用
int DispShutdown()
{
	//卸载gdi+
    GdiplusShutdown(m_pGdiToken);
	return 0;
}

// 负责GUI的线程
DWORD WINAPI DispThread(LPVOID lpParam)
{
    //装载gdi+
    GdiplusStartup(&m_pGdiToken,&m_gdiplusStartupInput,NULL); 
	hInstance=GetModuleHandle(NULL);
	MakeWindow();	

	return 0;
}



// DispInit，供easyVM调用的入口
int DispInit()
{
	int i;
	//修改BIOS数据区，这个显示器支持80×25BW彩色显示
	*(unsigned short *)(MemoryStart+0x410) &= !0x0010;
	*(unsigned short *)(MemoryStart+0x410) |= 0x0020;	

	DispMode=(char *)(MemoryStart+0x449);
	TextSolutionX=(short *)(MemoryStart+0x44a);
	CurPage=(char *)(MemoryStart+0x462);

	//用一个新线程来负责GUI
	TextBufStart=(char *)(TextMemory+MemoryStart);
	GrapBufStart=(char *)(GrapMemory+MemoryStart);
	(*DispMode)=0x03;
	DispModeChanged();
	CursorTop=6;
	CursorBottom=7;
	CursorVisible=true;
	DispCombinCode=0x8;

	//清空显示缓冲区	
	for (i=0;i<1024*4;i+=2)
	{
		*(unsigned short *)(TextBufStart+i)=0x0720;		//黑底白字的空格	//4页，每页占2KB
	}

	CreateThread( NULL, 0, DispThread, NULL, 0, &ThreadID);
	OutTable[0x00b0]=(unsigned int)IO_Write_00B0;
	OutTable[0x03ce]=(unsigned int)IO_Write_03CE;
	OutTable[0x03cf]=(unsigned int)IO_Write_03CF;

	OtherInitNeedMainWindow();
	return 0;	
}
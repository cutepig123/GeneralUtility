//////////////////////////////////////////////////////////////////////////
// ����˼�룺����������ڴ��е��Դ�����Disp�Լ��Ļ��������ж��Ƿ��и��£��ٰѻ�������������ʾ��������

//ʹ��GID+��ʱ��Ͷ���_GDIP_
//#define _GDIP_

#include "StdAfx.h"
#include "Display.h"
#include "KeyBoard.h"
#include "RTC.h"
#include "SystemTimer.h"
#include <math.h>

#define TICK 54.9255


#define CharWidth ((float)rect.right/(*TextSolutionX))				//����Ĳ�֪��Ϊʲô����ˮƽ���س����ı�ˮƽ�ַ������������ַ����
#define CharHeigh ((float)rect.bottom/TextSolutionY)
#define MemCurCur (*(unsigned char *)(MemoryStart+TextMemory+(CurLine*(*TextSolutionX)+CurRow)*2))
#define MemCurCurP1 (*(unsigned char *)(MemoryStart+TextMemory+(CurLine*(*TextSolutionX)+CurRow)*2+1))
#define BitPlane(a) (*(unsigned char *)(DispGrapBuf+(a)*64000+DrawP/8))
//#define VideoRAM GrapBufStart								//���԰�VideoRAM��0xa0000�ص��������������ռ�


//ˢ��Ƶ��
#define DispHZ 50
#define TextMemory 0xB8000
#define GrapMemory 0xA0000


// Disp���ڲ��Ĵ���
char *DispMode;					//��ǰ����ʾģʽ���ο� INT 10H�����ܺ�00H
char *TextBufStart;				//�ı�ģʽ�Դ��ʵ����ʼλ��
char *GrapBufStart;				//ͼ��ģʽ�Դ��ʵ����ʼλ��
char CurLine,CurRow,*CurPage;	//�ı�ģʽ�µ�ǰ�Ĺ��λ�ü�ҳ��
short *TextSolutionX,TextSolutionY,GrapSolutionX,GrapSolutionY,GrapSolutionColor;			//�ֱ���
RECT rect;						//��ǰ���ڵĴ�С
unsigned short CurVisCount;
double CurHight;				//���ռ�ַ��ߵı��������Ϊ1
double CurDispHZ;				//���ÿ�������ٴΣ�һ��һ��Ϊ������
char CursorTop,CursorBottom;	//���Ķ��ߡ�����
bool CursorVisible;				//����Ƿ�ɼ�
bool colorized;					//��ǰ��ʾģʽ�Ƿ��ɫ

char DispCombinCode;			//��ʾ����� Ϊ0x8
unsigned char GraphicsControlRegister[9];				//�ɶ˿�3CE��ѡ��3CF����д��ͼ�ο������Ĵ���
unsigned char GraphicsControlRegisterPointer;			//�ɶ˿�3CE��д��

int DispModeChanged();


//gdi+�õ�����������
GdiplusStartupInput m_gdiplusStartupInput;
ULONG_PTR m_pGdiToken;

HINSTANCE hInstance,hInst;
char *szWindowClass="easyVM_Disp";
char *szTitle="easyVM";
DWORD ThreadID;
HWND hWnd;			//���ھ��
HWND hMyWnd;		//���ھ��
bool NeedRefresh;	//��������Դ��Ƿ��и���
char DispTextBuf[4*1024];			//���ڼ���Ƿ���Ҫ������Ļ
char DispGrapBuf[256000];			//����������Buf��Ҫʹ�ã�����Ҫ��ModeBufSize
char VideoRAM[256000];				//����0xa0000��0xbffff��η�Χ������ʱ����ʵ���ڲ����Դ�

int ModeBufSize[0x14]={2048,2048,4096,4096,16000,16000,16000,4096,16000,32000,32000,0,0,32000,64000,28000,224000,38400,153600,64000};			//������ʾģʽ������Ҫ���Դ�Ĵ�С


// ��Ӧ��3CEh�˿ڵ�OUTָ��
// ͼ��ģʽ���ڿ�������
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

// �����ı�ģʽ�µ�һЩ����
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

//����ͼ��ģʽ�µ�һЩ����
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

// ÿ����ʾģʽ�ı��ʱ����������һЩȫ�ֱ���
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
		*(unsigned short *)((unsigned int)(TextBufStart+(*CurPage)*(*TextSolutionX)*TextSolutionY*2+(*TextSolutionX)*(TextSolutionY-1)*2)+i)=NewLineFill;		//�ڵװ��ֵĿո�	//4ҳ��ÿҳռ2KB
	}
}

void Push_Char(unsigned char tempc)
{
	(*CurPage)=eCPU.bh;
	if (tempc==0x8)					//�˸�
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
	else if (tempc==0xa)			//����
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

// ����INT 10H�жϣ���һ��Inָ����á�
// ��Ϊ�����3B0�Ļ���Ҫ�ȸı�DX��ֵ�����Ծ���B0�ˡ�
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
			eCPU.cx=0x10;			//��Bochs�������ģ���֪������˼
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


//������ɫ��4λ�����Եó���ɫֵ��DWORD��
DWORD AttrToColor(unsigned char a, bool spark)
{
	DWORD ans=0,bit;
	int i;
	bool HeightLight;
	HeightLight=a&0x8;
	for (i=0;i<3;i++)				//������
	{
		if (a&4)
		{
			if (HeightLight)
				bit=(0xff);
			else
				bit=(0xbf);
			if (spark)				//��
				bit*=(float)abs(DispHZ-(CurVisCount/4%DispHZ*2))/DispHZ;
			ans|=bit<<(i*8);
		}
		a<<=1;		
	}
	return ans;
}

//����GDI+�汾
DWORD AttrToColorP(unsigned char a, bool spark)
{
	DWORD ans=0;
	int i;
	for (i=0;i<3;i++)				//������
	{
		if (a&1)
			ans|=(0xbf<<(i*8));
		a>>=1;
	}
	if (a&1)						//����
		ans|=0xff404040;
	else
		ans|=0xff000000;
	return ans;
}

//������δʹ��
int OutHighAscii(HDC hdc, float *x, float *y, unsigned char *Txt, int *Txtlen, float CharWide)
{
	int		HighNum=0;	
	char	HighAscii[2];
	HighAscii[1]=0xcc;										//������Ͽ��Դ�ӡ����չASCII��
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

//������δʹ��
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

	LowAscii[1]=0x0;										//������Ͽ��Դ�ӡ����չASCII��
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


// ��DispTextBuf�������������Ļ��ȥ
// ����������ͬ���������ַ��������ַ����������ַ�����ʽTextOut��
int TextBufOut()
{
	PAINTSTRUCT ps;	
	HDC hdc = BeginPaint(hMyWnd, &ps);
	Graphics    rgraphics(hdc);
	SolidBrush  brush(Color(255, 255, 255, 255));	
	FontFamily  fontFamily(L"Courier New");
#ifdef _GDIP_
	double		WHscale=0.54112554;									//Courier New����Ŀ�߱�
#else
	double		WHscale=0.5;
#endif	
	Font        font(&fontFamily, (float)rect.bottom/TextSolutionY*0.882759, FontStyleBold, UnitPixel);			//0.882759=font.GetSize()/font.GetHeight(0.0);  ���Ƕ�Ӧ��Courier New�����ֵ��	
	SolidBrush	CurBrush(Color(255, 255, 255, 255));

	Bitmap		bmpBuf(rect.right,rect.bottom);							//˫����
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
	
	
	//���汻ע�͵�����䶼����GDI+ʱʹ�õġ�
	//graphics.Clear(Color(0,0,0));									//���ԭ�ȵ����ݡ�
	
	int pTextBufCur=0;
#ifdef _GDIP_
	unsigned short Txt[80*25+1]={L"��һ��"};
#else
	char Txt[80*25+1]={"��һ��"};
#endif
	pointF.Y=0;
	for (int i=0;i<TextSolutionY;i++)
	{		
		int TxtLen=0;
		int PTxtLen=0;		
		char CurCharAttr=DispTextBuf[i*(*TextSolutionX)*2+1];	//��һ���ַ�������
		pointF.X=0;
		for (int j=0;j<(*TextSolutionX);j++)
		{
			if (CurCharAttr!=DispTextBuf[pTextBufCur+1])			//������ͬ���ԣ�Ҫ����ַ�����
																									//���ڴ����ص�LowAscii�����ò����������ÿ����LowAscii��Ҫ����һ��
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
// 				if (Txt[0]>=0xa9 && Txt[0]<=0xdf)					//ֻ�����չAscii������Ʊ������ʵeasyVM��Ӧ����һ���ط����趨�Ƿ�����������
// 					OutHighAscii(hdc,&pointF.X,&pointF.Y,Txt,&TxtLen,(float)16*tempFX);				
// 				if ((unsigned char)Txt[0]<0x20 && Txt[0]!=0x00)									//�����ͷ�����ε�
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
			//else													//������ͬ���ԣ������ӵ�Txt����Ϳ�����
			{
#ifdef _GDIP_
				if (DispTextBuf[pTextBufCur+1]>0 && DispTextBuf[pTextBufCur+1]<0x80)	//ASCII��
#endif
				{				
					Txt[TxtLen]=DispTextBuf[pTextBufCur];
					Txt[++TxtLen]=0;
					pTextBufCur+=2;
				}
#ifdef _GDIP_
				else																//Unicode�ַ�
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
// 		if (Txt[0]>=0xa9 && Txt[0]<=0xdf)					//ֻ�����չAscii������Ʊ������ʵeasyVM��Ӧ����һ���ط����趨�Ƿ�����������
// 			OutHighAscii(hdc,&pointF.X,&pointF.Y,Txt,&TxtLen,(float)16*tempFX);
// 		if ((unsigned char)Txt[0]<0x20 && Txt[0]!=0x00)									//�����ͷ�����ε�
// 		{
// 			lao=OutLowAscii(hdc,&pointF.X,&pointF.Y,(unsigned char *)Txt,&TxtLen,(float)16*tempFX);
// 			//pTextBufCur+=lao*2;
// 		}
// 		else
// 			lao=0;
		MultiByteToWideChar(437,0,Txt,TxtLen,ws,100);
		TextOutW(hdc,pointF.X,pointF.Y,ws,TxtLen);
		pointF.Y+=(int)font.GetHeight(0.0);							//pointF.Y�Ǹ��㣬TextOut�Ĳ��������Σ��������Ļ��ۻ��������
#endif
	}
	if (CurVisCount/(unsigned short)(DispHZ*CurDispHZ)%2 && CursorVisible)			//��ʾ���
	{		
#ifdef _GDIP_
		CurX=(float)(CurRow+0.5)*font.GetHeight(0.0)*WHscale;
		CurY=font.GetHeight(0.0)*(CurLine+(1-CurHight)-0.1);		//�ѹ��̧��0.1�бȽϺÿ�		
#else
		GetCharWidthFloat(hdc,'A','A',&tempFX);
		CurX=(float)(CurRow)*16*tempFX;
		CurY=(int)(font.GetHeight(0.0))*(CurLine+(1-CurHight)-0.1);		//�ѹ��̧��0.1�бȽϺÿ�		
#endif		
		graphics.FillRectangle(&CurBrush,CurX+1,CurY,(font.GetHeight(0.0)*WHscale+1),font.GetHeight(0.0)*CurHight);
	}

	rgraphics.DrawImage(&bmpBuf,0,0);
	EndPaint(hMyWnd, &ps);
	
	return 0;
}

// ͼ��ģʽ������......
int GrapBufOut()
{
	PAINTSTRUCT ps;	
	HDC hdc = BeginPaint(hMyWnd, &ps);
	Graphics    rgraphics(hdc);
	SolidBrush	GrapBrush(Color(255, 255, 255, 255));

	Bitmap		bmpBuf(rect.right,rect.bottom);							//˫����
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
					//���¸�λ
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
// 					GrapBrush.SetColor(Color(((tdc)&(3<<(DrawP%8))>>6)*0x10+0xbf,255,255,255));		//4�Ҷ�
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


// ������Լ��Ļ��������������һ��Draw������ʾ�������ϡ�
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





// GUI�Ĵ��ں���
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
	case WM_KEYDOWN:			//Bochs��NTVDM��ֻ������MakeCode
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




// ��Win32 APP�����������ĺ��������ӡ���
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

// ��Win32 APP�����������ĺ��������ӡ���
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; 

   hWnd = CreateWindow(szWindowClass, szTitle,WS_THICKFRAME| WS_OVERLAPPED| WS_CAPTION| WS_SYSMENU| WS_MINIMIZEBOX| WS_MAXIMIZEBOX ,
      CW_USEDEFAULT, 0, 888, 484, NULL, NULL, hInstance, NULL);			//����߿������� "Courier New"

   if (!hWnd)
   {
      return FALSE;
   }
   hMyWnd=hWnd;

   ShowWindow(hWnd, SW_SHOW);
   UpdateWindow(hWnd);

   return TRUE;
}






// ˢ��Disp�Լ��Ļ������������ڴ��е��Ǹ���ʾ��������
// �Ƿ���Ҫˢ�µ��жϾ���������
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
		//�ı�ģʽ		
		if(memcmp(TextBufStart,DispTextBuf,ModeBufSize[(*DispMode)]) || CurVisCount%(unsigned short)(DispHZ*CurDispHZ)==0)		//ÿ������һ��
		{
			memcpy(DispTextBuf,TextBufStart,ModeBufSize[(*DispMode)]);
			InvalidateRect(hMyWnd,&rect,false);
		}		
	}
	else
	{
		//ͼ��ģʽ
// 		int BufOneLine=ModeBufSize[(*DispMode)]/GrapSolutionY;
// 		for (i=0;i<GrapSolutionY;i++)
		if (CurVisCount%(unsigned short)(DispHZ*CurDispHZ)==0)
			memset(DispGrapBuf,0,sizeof(DispGrapBuf));			//����ֻ��Ϊ��ˢ��
		
		if(memcmp(VideoRAM,DispGrapBuf,sizeof(DispGrapBuf)))		//�������CurVisCount��ˢ��
		{
			//memcpy(DispGrapBuf,VideoRAM,ModeBufSize[(*DispMode)]);
			//�޸�GispGrapBuf�Ķ�����OnPaint����GrapBufOut����У�ֻ�޸ĺ�ˢ�¸ı��˵Ĳ���
			InvalidateRect(hMyWnd,&rect,false);
		}				
	}
}

int OtherInitNeedMainWindow()
{
	SetTimer(NULL,NULL,TICK,(TIMERPROC)RTCTimerThread);				//������TICKǿ��ת����54,��������о����Ժ�����ʱ�䳬ǰ
	SetTimer(NULL,2,TICK,(TIMERPROC)SysTimerThread);
	return 0;
}

// ��������
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

// GUI��������easyVM����
int DispShutdown()
{
	//ж��gdi+
    GdiplusShutdown(m_pGdiToken);
	return 0;
}

// ����GUI���߳�
DWORD WINAPI DispThread(LPVOID lpParam)
{
    //װ��gdi+
    GdiplusStartup(&m_pGdiToken,&m_gdiplusStartupInput,NULL); 
	hInstance=GetModuleHandle(NULL);
	MakeWindow();	

	return 0;
}



// DispInit����easyVM���õ����
int DispInit()
{
	int i;
	//�޸�BIOS�������������ʾ��֧��80��25BW��ɫ��ʾ
	*(unsigned short *)(MemoryStart+0x410) &= !0x0010;
	*(unsigned short *)(MemoryStart+0x410) |= 0x0020;	

	DispMode=(char *)(MemoryStart+0x449);
	TextSolutionX=(short *)(MemoryStart+0x44a);
	CurPage=(char *)(MemoryStart+0x462);

	//��һ�����߳�������GUI
	TextBufStart=(char *)(TextMemory+MemoryStart);
	GrapBufStart=(char *)(GrapMemory+MemoryStart);
	(*DispMode)=0x03;
	DispModeChanged();
	CursorTop=6;
	CursorBottom=7;
	CursorVisible=true;
	DispCombinCode=0x8;

	//�����ʾ������	
	for (i=0;i<1024*4;i+=2)
	{
		*(unsigned short *)(TextBufStart+i)=0x0720;		//�ڵװ��ֵĿո�	//4ҳ��ÿҳռ2KB
	}

	CreateThread( NULL, 0, DispThread, NULL, 0, &ThreadID);
	OutTable[0x00b0]=(unsigned int)IO_Write_00B0;
	OutTable[0x03ce]=(unsigned int)IO_Write_03CE;
	OutTable[0x03cf]=(unsigned int)IO_Write_03CF;

	OtherInitNeedMainWindow();
	return 0;	
}
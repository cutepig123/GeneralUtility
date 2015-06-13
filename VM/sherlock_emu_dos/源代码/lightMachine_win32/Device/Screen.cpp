//#include "stdafx.h"
#include "screen.h"
#include "VGACard.h"
#include <comdef.h> 
//黑
#define COLOR_BLACK			0
//蓝
#define COLOR_BLUE			RGB(0,0,255)
//绿
#define COLOR_GREEN			RGB(0,128,0)
//青
#define COLOR_CYAN			RGB(0,255,255)
//灰
#define COLOR_GRAY			RGB(128,128,128)
//浅蓝
#define COLOR_LIGHTBLUE		RGB(173,216,230)
//浅绿
#define COLOR_LIGHTGREEN	RGB(144,238,144)
//浅青
#define COLOR_LIGHTCYAN		RGB(224,255,255)
//红
#define COLOR_RED			RGB(255,0,0)
//品红~~
#define COLOR_MAGENTA		RGB(255,0,255)
//棕色
#define COLOR_BROWN			RGB(184,134,11)
//浅灰色。。灰白
#define COLOR_LIGHTGRAY		RGB(211,211,211)
//浅红
#define COLOR_LIGHTRED		RGB(255,99,71)
//浅品红~~
#define COLOR_LIGHTMAGENTA  RGB(255,102,183) 
//黄
#define COLOR_YELLOW		RGB(255,255,0)
//白
#define COLOR_WHITE			RGB(255,255,255)

const int MAX_CHAR_PER_ROW = 100;//一行最多有可能显示多少字符，其实只有八十

Screen::Screen()
{
	this->hardwareName = "Screen";
	this->hwnd = displayhWnd;
	this->hdc = ::GetDC(hwnd);
	this->bufferHDC = ::CreateCompatibleDC(NULL);
	this->bufferBitmap = NULL;
	this->clientHeight = 0;
	this->clientWidth = 0;
	this->cursor_flashTime = 1;
	this->lastVgaDataID = 1;//和VGA不同，使得第一次一定要重画
	lf.lfWidth = 8;
	lf.lfHeight = 8;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = 0;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = 0;
	lf.lfClipPrecision = 0;
	lf.lfQuality = 0;
	lf.lfPitchAndFamily = 0;

}

void Screen::setScreen_Widht_Height(int charWidth, int charHeight, bool isColor)
{
	RECT clientRect,windowRect;
	::GetClientRect(this->hwnd,&clientRect);
	::GetWindowRect(this->hwnd,&windowRect);
	int widthOffset = windowRect.right - windowRect.left - clientRect.right;
	int heightOffset = windowRect.bottom - windowRect.top - clientRect.bottom;//获取窗口和客户区大小，以决定窗口大小，从而决定客户区大小
	::MoveWindow(this->hwnd, 0, 0, pvga->charPerRow*charWidth+widthOffset, pvga->charPerCol*charHeight+heightOffset, SWP_NOMOVE);
	::GetClientRect(this->hwnd,&clientRect);
	this->clientHeight = clientRect.bottom - clientRect.top;
	this->clientWidth = clientRect.right - clientRect.left;
	lf.lfWidth = charWidth;
	lf.lfHeight = charHeight;
	hFont = ::CreateFontIndirect(&lf);
	hFont = (HFONT)SelectObject(this->bufferHDC, hFont);
	this->isColor = isColor;
	this->bufferBitmap = ::CreateCompatibleBitmap(hdc,//创建缓冲位图
												GetDeviceCaps(hdc, HORZRES),
												GetDeviceCaps(hdc, VERTRES));
	SelectObject(bufferHDC, this->bufferBitmap);
									
	SendMessage(hwnd, WM_PAINT,0,0);
}
void Screen::setCursor_type(unsigned int start, unsigned int end )
{
}
void Screen::setCursor_Position(unsigned int X_position, unsigned int Y_position)
{
}
void Screen::displayCharWithProperty(char charData, unsigned char pro)
{
}
void Screen::displayString(const char* str, int size,unsigned char pro, int x, int y)
{
	

	COLOR strBackcolor = this->property2BkColor(pro);
	COLOR strFrontcolor = this->property2FontColor(pro);
	

	SetTextColor(this->hdc, strFrontcolor);
	SetBkMode(this->hdc, OPAQUE);
	SetBkColor(this->hdc, strBackcolor);
	//TextOut(hdc,0,0,param,lstrlen(param));
	SendMessage(hwnd, WM_PAINT,0,0);
	//delete[] param;
}

void Screen::showAll()
{
	if(pvga->isReady==false)
		return;
	if( pvga->vgaDataID != this->lastVgaDataID)
	{
		this->lastVgaDataID = pvga->vgaDataID;
		HBRUSH hBrush;
		hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
		SelectObject(bufferHDC, hBrush);
		Rectangle(bufferHDC,0,0, lf.lfWidth*pvga->charPerRow, lf.lfHeight*pvga->charPerCol);
		char* pcharacter = (char*)(pvga->textMemAddr + (pvga->currentPage*pvga->charPerRow*pvga->charPerCol)*2);
		for(int i=0; i< pvga->charPerCol; i++)
		{
			for(int j=0; j< pvga->charPerRow; j++)
			{
				char achar=pvga->getChar(j, i);
				U1 char_property = pvga->getPro(j,i);
				if(achar == 0)
					continue;
				wchar_t unicodeChar;
				int ilen=	MultiByteToWideChar(437,0,&achar,1,&unicodeChar,1);//
				SetTextColor(this->bufferHDC, property2FontColor(pvga->getPro(j, i)));
				SetBkMode(this->bufferHDC, OPAQUE);
				SetBkColor(this->bufferHDC, property2BkColor(pvga->getPro(j, i)));
				TextOut(this->bufferHDC, j*lf.lfWidth, i*lf.lfHeight,(LPWSTR)&unicodeChar, 1);
			}
		}
	}
	::BitBlt(hdc,0,0,this->clientWidth, this->clientHeight,
		     bufferHDC, 0, 0,SRCCOPY);
	flashCursor();//闪烁光标

}
void Screen::flashCursor()
{
	unsigned int page = pvga->currentPage;
	HPEN hPen;
	int x1_cursor, y1_cursor, x2_cursor, y2_cursor;
	x1_cursor = pvga->cursor[page].x * lf.lfWidth;
	y1_cursor = (pvga->cursor[page].y) * lf.lfHeight + pvga->cursor[page].cursor_top+8;
	x2_cursor = (pvga->cursor[page].x + 1) * lf.lfWidth;
	y2_cursor = (pvga->cursor[page].y) * lf.lfHeight + pvga->cursor[page].cursor_bottom+8;
	if(startTimeCount%10 < this->cursor_flashTime)
	{	//光标变白
		hPen = (HPEN)CreatePen(PS_SOLID,2,RGB(255,255,255));
		SelectObject(hdc, hPen);
		Rectangle(hdc, x1_cursor, y1_cursor, x2_cursor, y2_cursor);
						   
	}
	else
	{//光标消失。。即贴上背景图片。。
		::BitBlt(hdc, x1_cursor, y1_cursor, x2_cursor, y2_cursor, bufferHDC,
			          x1_cursor, y1_cursor,SRCCOPY);
	}
	
	
}
void Screen::repaint()
{
	this->lastVgaDataID = pvga->vgaDataID + 1;
	showAll();//必定重画
}
COLOR Screen::property2FontColor(U1 pro)
{
	switch(pro & MASK_00001111)
	{
	case MASK_00000000:
		return COLOR_BLACK;
		break;
	case MASK_00000001:
		return COLOR_BLUE;
		break;
	case MASK_00000010:
		return COLOR_GREEN;
		break;
	case MASK_00000011:
		return COLOR_CYAN;
		break;
	case MASK_00000100:
		return COLOR_RED;
		break;
	case MASK_00000101:
		return COLOR_MAGENTA;
		break;
	case MASK_00000110:
		return COLOR_BROWN;
		break;
	case MASK_00000111:
		return COLOR_LIGHTGRAY;
		break;
	case MASK_00001000:
		return COLOR_GRAY;
		break;
	case MASK_00001001:
		return COLOR_LIGHTBLUE;
		break;
	case MASK_00001010:
		return COLOR_LIGHTGREEN;
		break;
	case MASK_00001011:
		return COLOR_LIGHTGRAY;
		break;
	case MASK_00001100:
		return COLOR_LIGHTRED;
		break;
	case MASK_00001101:
		return COLOR_LIGHTMAGENTA;
		break;
	case MASK_00001110:
		return COLOR_YELLOW;
		break;
	case MASK_00001111:
		return COLOR_WHITE;
		break;
		
	}
}
COLOR Screen::property2BkColor(U1 pro)
{
	switch((pro & MASK_01110000)>>4)
	{
	case MASK_00000000:
		return COLOR_BLACK;
		break;
	case MASK_00000001:
		return COLOR_BLUE;
		break;
	case MASK_00000010:
		return COLOR_GREEN;
		break;
	case MASK_00000011:
		return COLOR_CYAN;
		break;
	case MASK_00000100:
		return COLOR_RED;
		break;
	case MASK_00000101:
		return COLOR_MAGENTA;
		break;
	case MASK_00000110:
		return COLOR_BROWN;
		break;
	case MASK_00000111:
		return COLOR_LIGHTGRAY;
		break;
	
		
	}
}
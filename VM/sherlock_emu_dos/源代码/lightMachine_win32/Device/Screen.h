#ifndef _SCREEN_H
#define _SCREEN_H
#include<windows.h>
#include<string>
#include"../common.h"
#include "../Hardware.h"
class VGACard;
using namespace std;
//Screen 是系统相关的。。。win32
extern HWND displayhWnd;
extern unsigned int startTimeCount;
class Screen :public Hardware
{
public:
	Screen();
	void setScreen_Widht_Height(int charWidth, int charHeight, bool isColor);
	void setCursor_type(unsigned int start, unsigned int end );
	void setCursor_Position(unsigned int X_position, unsigned int Y_position);
	void displayCharWithProperty(char charData, unsigned char pro);
	void displayString(const char* str, int size,unsigned char pro, int x, int y);
	void showAll();
	void repaint();
	VGACard* pvga;
private:
	void flashCursor();
private:
	int cursor_flashTime;
	COLOR property2FontColor(U1 pro);
	COLOR property2BkColor(U1 pro);
	HWND hwnd;
	HDC  hdc, bufferHDC;//hdc是窗口的，bufferHDC是缓冲hdc
	HBITMAP bufferBitmap;//缓冲位图
	int clientWidth, clientHeight;
	HFONT hFont;
	LOGFONT lf;
	bool isColor;
	int lastVgaDataID;

};



#endif
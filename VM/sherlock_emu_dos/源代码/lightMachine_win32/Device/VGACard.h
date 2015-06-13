#ifndef _VGACARD_H
#define _VGACARD_H
#include "../MotherBoard.h"
#include "../Device.h"
#define MAX_PAGE 8
class Screen;
struct Cursor
{
	unsigned int x;
	unsigned int y;
	bool isVisual;
	unsigned int cursor_top, cursor_bottom;
	Cursor()
	{
		x=0;
		y=0;
		isVisual = true;
		cursor_top = 5;
		cursor_bottom = 0;
	}
};
class VGACard : public Device
{
public:
	VGACard()
	{
		this->hardwareName = "VGACard";
		this->isReady=false;
	}
	void init();
	vector<int> get_INT_shouldHandle();
	void linkScreen(Screen* pscreen);
private:
	void HandleINT(U1 INT_num);
	void setDispalyMode();
	void setCursor_type();
	void setCursor_position();
	void setDisplayPage();
	void screenUpScroller();
	void screenDownScroller();
	void displayCharWithProperty();
	void displayChar();
	void setPalette();
	void displayPixel();
	void displayString();
	void getVGAInfo();
	void getVGAStatus();
	void characterGenerate();

	void getCursor_type();
	void getCursor_position();
	void getCharWithProperty();
	void getPixel();


	
	void insertString(string text, U1 pro,  unsigned int x, unsigned y,
						 unsigned int page , bool isCursorSensitive = true);
						 
	//PropertyString getPropertyString(unsigned int x, unsigned int y);
private:
	void cursorBackOneStep(const unsigned int page);
	bool cursorToNextRow(const unsigned int page);
	void cursorToNext(const unsigned int page);
	void screenUpMoveOneLine();
	void clsScreen();
private:
	U1 vgaMode;
public:
	U1 getChar(unsigned int x, unsigned int y);
	U1 getPro(unsigned int x, unsigned int y);

	bool isReady;
	U1* textMemAddr;
	U1 currentPage;
	Cursor cursor[MAX_PAGE];
	Screen* pscreen;
	unsigned int charPerRow, charPerCol;
	int vgaDataID;

};


struct PropertyString
{
	string text;
	U1 pro;
};

#endif
#include "GUI.h"
extern HWND displayhWnd;
void MessageMaker::displayMessage(string msg)
{
	MessageBoxA(displayhWnd, &msg[0], "LM Message!",MB_OK);
}
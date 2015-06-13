#ifndef _KB_INTERFACE_H
#define _KB_INTERFACE_H
#include "../common.h"
#include "../Device.h"
#include <windows.h>
class PCKeyBoard;
class KBInterface:public Hardware
{//与设备有关的keyboard接口
public:
	KBInterface();
	~KBInterface();
	void handleKeyBoardChar( WPARAM wParam, LPARAM lParam);
	void handleKeyBoardPress(UINT message, WPARAM wParam, LPARAM lParam);
	void updateKBStatus(UINT message, WPARAM wParam, LPARAM lParam);
	void sendSetedKey();
	void sleepForAWhile();
	PCKeyBoard* pKB;
private:
	bool isShiftPressed();
	bool isAltPressed();
	bool isCtrlPressed();
};
#endif
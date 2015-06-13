//#include "stdafx.h"
#include "KBInterface.h"
#include "KeyBoard.h"
KBInterface::KBInterface()
{
	this->hardwareName ="KeyBoardInterface";
}
KBInterface::~KBInterface(){}
const int KEY_INSERT = 1<<7;
const int KEY_CAPSLOCK = 1<<6;
const int KEY_NUMLOCK = 1<<5;
const int KEY_SCROLLLOCK = 1<<4;
const int KEY_ALT = 1<<3;
const int KEY_CONTROL = 1<<2;
const int KEY_LEFT_SHIFT = 1<<1;
const int KEY_RIGHT_SHIFT = 1;

const int KEY_PAUSE = 1<<3;
const int KEY_SYSREQ = 1<<2;
const int KEY_LEFT_ALT = 1<<1;
const int KEY_LEFT_CONTROL = 1;
int count=0;

void KBInterface::handleKeyBoardChar( WPARAM wParam, LPARAM lParam)
{
	U2 scan_Ascii=(U2)(((lParam&0xffff0000)>>8)|wParam);
	while(false == this->pKB->receiveKBData(scan_Ascii))
	{
		Sleep(10);
	}
}

void KBInterface::handleKeyBoardPress(UINT message, WPARAM wParam, LPARAM lParam)
{
	U2 scan_Ascii=(U2)( (lParam&0xffff0000)>>8 );//其ASCII的值为0
	if(WM_SYSKEYDOWN == message || WM_KEYDOWN == message)
	{
		switch(wParam)
		{
		case VK_F1:
		case VK_F2:
		case VK_F3:
		case VK_F4:
		case VK_F5:
		case VK_F6:
		case VK_F7:
		case VK_F8:
		case VK_F9:
		case VK_F10:
		case VK_F11:
		case VK_F12://F1~~F12
			if(isShiftPressed())
				scan_Ascii += 0x1900;
			else if(isAltPressed())
				scan_Ascii += 0x2d00;
			else if (isCtrlPressed())
				scan_Ascii += 0x2300;
			this->pKB->receiveKBData(scan_Ascii);
			break;
		case VK_ESCAPE://ESC
		case VK_PRIOR://pageUP
		case VK_NEXT://pageDown
		case VK_END:
		case VK_HOME:
		case VK_LEFT:
		case VK_UP:
		case VK_RIGHT:
		case VK_DOWN:
			 this->pKB->receiveKBData(scan_Ascii);
		     break;
		default://剩下的字符可能是alt。。ctl与普通字符等，但是updateKBStatus会过滤掉普通字符
			this->updateKBStatus(message, wParam, lParam);
			
			if(wParam>= 0x41 && wParam<=0x41+'Z'-'A')
			{
				if(isAltPressed())
				{
					scan_Ascii = scan_Ascii;
					this->pKB->receiveKBData(scan_Ascii);
				}
				if(isCtrlPressed())
				{
					scan_Ascii += wParam-0x41 ;
					this->pKB->receiveKBData(scan_Ascii);
				}
				
				//如果不是按下ctrl，则是按下alt
			}
			break;
		}
		
	}
	else
	{//keyUp, sysKeyUp
		this->updateKBStatus(message, wParam, lParam);
	}
}
void KBInterface::updateKBStatus(UINT message, WPARAM wParam, LPARAM lParam)
{
	unsigned char keyPressingState = 0;
	unsigned char keyState = 0;
	if(WM_KEYDOWN == message || WM_SYSKEYDOWN == message)
	{
		switch(wParam)//如果是普通字符，则keyPressingState是0
		{
		case VK_INSERT:
			keyPressingState |= KEY_INSERT;
			break;
		case VK_CAPITAL:
			keyPressingState |= KEY_CAPSLOCK;
			break;
		case VK_NUMLOCK:
			keyPressingState |= KEY_NUMLOCK;
			break;
		case VK_SCROLL:
			keyPressingState |= KEY_SCROLLLOCK;
			break;
		case VK_PAUSE:
			keyPressingState |= KEY_PAUSE;
			break;
			//这里扫了Sys req 键的处理
		case VK_MENU:
			keyPressingState |= KEY_LEFT_ALT;//区别不了。。。就当做按下左边alt，也就是说按右边也会相应，但是不要紧~~
			break;
		case VK_CONTROL:
			keyPressingState |= KEY_LEFT_CONTROL;//同上
			break;
		}
		this->pKB->setKeyPressingStatus(keyPressingState);
	}
	else
	{//keyUp, sysKeyUp
		this->pKB->setKeyPressingStatus(0);//功能键全部都处于没有按下状态
	}
	//刷新功能键当前状态
	if (GetKeyState(VK_INSERT)<0)
		keyState |= KEY_INSERT;
	if (GetKeyState(VK_CAPITAL)<0)
		keyState |= KEY_CAPSLOCK;
	if (GetKeyState(VK_NUMLOCK)<0)
		keyState |= KEY_NUMLOCK;
	if (GetKeyState(VK_SCROLL)<0)
		keyState |= KEY_SCROLLLOCK;
	if (GetKeyState(VK_MENU)<0)
		keyState |= KEY_ALT;
	if (GetKeyState(VK_CONTROL)<0)
		keyState |= KEY_CONTROL;
	if (GetKeyState(VK_LSHIFT)<0)
		keyState |= KEY_LEFT_SHIFT;
	if (GetKeyState(VK_RSHIFT)<0)
		keyState |= KEY_RIGHT_SHIFT;
	this->pKB->setKeyStatus(keyState);
	


}
bool KBInterface::isShiftPressed()
{
	return !!((this->pKB->getKeyStatus()&KEY_LEFT_SHIFT)|(this->pKB->getKeyStatus()&KEY_RIGHT_SHIFT));
}
bool KBInterface::isAltPressed()
{
	return !!(this->pKB->getKeyStatus()&KEY_ALT);
}
bool KBInterface::isCtrlPressed()
{
	return !!(this->pKB->getKeyStatus()&KEY_CONTROL);
}

void KBInterface::sleepForAWhile()
{
	Sleep(1);
}


void KBInterface::sendSetedKey()
{//此为程序测试程序，使命已经完成
	unsigned int nnn = 0x1cffff;
	switch(++count)
	{
	case 1:
		handleKeyBoardChar(char(0xd),nnn);
	//	RecvKeyBoardMsg(WM_KEYUP,'/r',nnn);
		break;
	case 2:
		handleKeyBoardChar(char(0xd),nnn);
	//	RecvKeyBoardMsg(WM_KEYUP,'/r',nnn);
		break;
	case 3:
		nnn=0x12ffff;
		handleKeyBoardChar('e',nnn);
	//	RecvKeyBoardMsg(WM_KEYUP,'e',nnn);
		break;
	case 4:
		nnn=0x20ffff;
		handleKeyBoardChar('d',nnn);
	//	RecvKeyBoardMsg(WM_KEYUP,'d',nnn);
		break;
	case 5:
		nnn=0x17ffff;
		handleKeyBoardChar('i',nnn);
		break;
	//	RecvKeyBoardMsg(WM_KEYUP,'i',nnn);
	case 6:
		nnn=0x14ffff;
		handleKeyBoardChar('t',nnn);
		break;
	//	RecvKeyBoardMsg(WM_KEYUP,'t',nnn);
	case 7:
		nnn=0x34ffff;
		handleKeyBoardChar('.',nnn);
		break;
	//	RecvKeyBoardMsg(WM_KEYUP,'.',nnn);
	case 8:
		nnn=0x2effff;
		handleKeyBoardChar('c',nnn);
		break;
	//	RecvKeyBoardMsg(WM_KEYUP,'c',nnn);
	case 9:
		nnn=0x18ffff;
		handleKeyBoardChar('o',nnn);
		break;
	//	RecvKeyBoardMsg(WM_KEYUP,'o',nnn);
	case 10:
		nnn=0x32ffff;
		handleKeyBoardChar('m',nnn);
		break;
	//	RecvKeyBoardMsg(WM_KEYUP,'m',nnn);
	case 11:
		nnn = 0x1cffff;
		handleKeyBoardChar(char(0xd),nnn);
	}
}
#ifndef _KEYBOARD_H
#define _KEYBOARD_H
#include "../common.h"
#include "../Device.h"
#include "KBInterface.h"
class PCKeyBoard:public Device
{
public:
	PCKeyBoard();
	~PCKeyBoard();
	void init();
	vector<int> get_INT_shouldHandle();
	void HandleINT(U1 INT_num);
	void linkKBInterface(KBInterface* pKBface);
	bool receiveKBData(const U2 scan_Ascii);
	void setKeyPressingStatus(U1 state);
	void setKeyStatus(U1 state);
	U1 getKeyStatus();
	KBInterface* pKBface;
private:
	void readKBInPut();// 读取键盘输入ah=0
	void getKBStatus();//检查键盘状态ah=1
	void getKBShiftStatus();//获取shift标志状态ah=2
	void setKBInPutRate();//设置键盘输入速度ah=3
	void setKBVoice();//调整键盘敲击声ah=4
	void storeKBInPutData();//将键数据装入键盘缓冲区ah=5
	void getKB_ID();//获取键盘ID ah=A
	void advanceHeadPointer();
	void advanceTailPointer();
	bool isBufferEmpty();
	bool isBufferFull();

	U2 *headPointer, *tailPointer;//Address of Head Pointer, Address of Tail Pointer
	

};
#endif

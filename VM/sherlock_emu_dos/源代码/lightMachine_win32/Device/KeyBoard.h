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
	void readKBInPut();// ��ȡ��������ah=0
	void getKBStatus();//������״̬ah=1
	void getKBShiftStatus();//��ȡshift��־״̬ah=2
	void setKBInPutRate();//���ü��������ٶ�ah=3
	void setKBVoice();//���������û���ah=4
	void storeKBInPutData();//��������װ����̻�����ah=5
	void getKB_ID();//��ȡ����ID ah=A
	void advanceHeadPointer();
	void advanceTailPointer();
	bool isBufferEmpty();
	bool isBufferFull();

	U2 *headPointer, *tailPointer;//Address of Head Pointer, Address of Tail Pointer
	

};
#endif

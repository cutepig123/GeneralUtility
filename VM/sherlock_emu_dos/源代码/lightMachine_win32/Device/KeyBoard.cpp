//#include "stdafx.h"
#include "../common.h"
#include "KeyBoard.h"
#include <vector>
#include"../CPU/CentreProcessorUnit.h"
using namespace std;
const int kbBufferStartAddr = 0x41E;
const int kbBufferEndAddr   = 0x43D;
const int kbLogicAddrHeadPointer = 0x41A;
const int kbLogicAddrTailPointer = 0x41C;
const int kbBufferSize = (kbBufferEndAddr+1-kbBufferStartAddr);//��һ���ֽ�Ϊ��λ
const int kbStatusAddr = 0x417;//����״̬
const int kbKeyPressingStatusAddr = 0x418;//��������״̬
bool isKeyhandle=true;
PCKeyBoard::PCKeyBoard()
{
	this->hardwareName = "KeyBoard";
}
PCKeyBoard::~PCKeyBoard(){}
void PCKeyBoard::init()
{//���忴IBM ��P553ҳ
	
	headPointer = (U2*)(this->memStartAddr + kbLogicAddrHeadPointer);//0x41A��0x41B�洢��keyboard buffer��ͷ��ַ
	tailPointer = (U2*)(this->memStartAddr + kbLogicAddrTailPointer);//0x41C��0x41D�洢��keyboard buffer��β��ַ
}
void PCKeyBoard::linkKBInterface(KBInterface* pKBface)
{
	this->pKBface = pKBface;
	pKBface->pKB = this;
}
vector<int> PCKeyBoard::get_INT_shouldHandle()
{
	vector<int> INTarray;
	INTarray.push_back(0x16);
	return INTarray;

}
void PCKeyBoard::HandleINT(U1 INT_num)
{
	switch(pCPU->ah)
	{
	case 0x0:
	case 0x10:
		this->readKBInPut();
		break;
	case 0x1:
	case 0x11:
		this->getKBStatus();
		break;
	case 0x2:
		this->getKBShiftStatus();
		break;
	case 0x3:
		this->setKBInPutRate();
		break;
	case 0x4:
		this->setKBVoice();
		break;
	case 0x5:
		this->storeKBInPutData();
		break;
	case 0xa:
		this->getKB_ID();
		break;
	default:
		return;
	}
}
void PCKeyBoard::readKBInPut()
{// ��ȡ��������ah=0
	if(isBufferEmpty())
		return;//��Ϊbuffer��ѭ�����нṹ������Ⱦͱ�ʾΪ��
	else
	{
		pCPU->ax =*(U2*)(memStartAddr+*headPointer);
		advanceHeadPointer();//������������
		isKeyhandle=true;
	}
}
void PCKeyBoard::getKBStatus()
{//������״̬ah=1
	if(true==isKeyhandle)
	{
		isKeyhandle=false;
		//this->pKBface->sendSetedKey();
	}
	if(isBufferEmpty())
	{
		this->pKBface->sleepForAWhile();//��΢����������ʧ��ȡCPUռ���ʵĴ���½��������ȫ����ת����ע�ʹ����
		pCPU->setZF_Flag(true);
		return;//��Ϊbuffer��ѭ�����нṹ������Ⱦͱ�ʾΪ��
		
	}
	else
	{
		pCPU->ax =*(U2*)(memStartAddr+*headPointer);
		pCPU->setZF_Flag(false);
	}
	
}
void PCKeyBoard::getKBShiftStatus()
{//��ȡshift��־״̬ah=2
	pCPU->al = 0x20;
}
void PCKeyBoard::setKBInPutRate()
{//���ü��������ٶ�ah=3
}
void PCKeyBoard::setKBVoice()
{//���������û���ah=4
}
void PCKeyBoard::storeKBInPutData()
{//��������װ����̻�����ah=5
}
void PCKeyBoard::getKB_ID()
{//��ȡ����ID ah=A
}
bool PCKeyBoard::receiveKBData(const U2 scan_Ascii)
{
	if(isBufferFull())
		return false;//�������������ٴ�������false
	pCPU->isNeedHandleKeyPress = true;
	*(U2*)(memStartAddr+*tailPointer) = scan_Ascii;
	advanceTailPointer();
	return true;
}
bool PCKeyBoard::isBufferEmpty()
{
	return *headPointer ==  *tailPointer;
}
bool PCKeyBoard::isBufferFull()
{
	return (*headPointer == (U2)(kbBufferStartAddr + ((*tailPointer)+2-kbBufferStartAddr)%kbBufferSize));
}
void PCKeyBoard::advanceHeadPointer()
{// ѭ�����С���ͷָ���һ
	*headPointer = (U2)(kbBufferStartAddr + ((*headPointer+2)-kbBufferStartAddr)%kbBufferSize);
}
void PCKeyBoard::advanceTailPointer()
{
	*tailPointer = (U2)(kbBufferStartAddr + ((*tailPointer)+2-kbBufferStartAddr)%kbBufferSize);
}

void PCKeyBoard::setKeyPressingStatus(const U1 state)
{
	*(U1*)(kbKeyPressingStatusAddr+memStartAddr) = state;

}
void PCKeyBoard::setKeyStatus(const U1 state)
{
	*(U1*)(kbStatusAddr+memStartAddr) = state;
}
U1  PCKeyBoard::getKeyStatus()
{
	return *(U1*)(kbStatusAddr+memStartAddr);
}
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
const int kbBufferSize = (kbBufferEndAddr+1-kbBufferStartAddr);//以一个字节为单位
const int kbStatusAddr = 0x417;//按键状态
const int kbKeyPressingStatusAddr = 0x418;//按键按下状态
bool isKeyhandle=true;
PCKeyBoard::PCKeyBoard()
{
	this->hardwareName = "KeyBoard";
}
PCKeyBoard::~PCKeyBoard(){}
void PCKeyBoard::init()
{//具体看IBM 的P553页
	
	headPointer = (U2*)(this->memStartAddr + kbLogicAddrHeadPointer);//0x41A和0x41B存储了keyboard buffer的头地址
	tailPointer = (U2*)(this->memStartAddr + kbLogicAddrTailPointer);//0x41C和0x41D存储了keyboard buffer的尾地址
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
{// 读取键盘输入ah=0
	if(isBufferEmpty())
		return;//因为buffer是循环队列结构。。相等就表示为空
	else
	{
		pCPU->ax =*(U2*)(memStartAddr+*headPointer);
		advanceHeadPointer();//并且移走数据
		isKeyhandle=true;
	}
}
void PCKeyBoard::getKBStatus()
{//检查键盘状态ah=1
	if(true==isKeyhandle)
	{
		isKeyhandle=false;
		//this->pKBface->sendSetedKey();
	}
	if(isBufferEmpty())
	{
		this->pKBface->sleepForAWhile();//以微弱的性能损失换取CPU占用率的大幅下降，如果想全速运转，可注释此语句
		pCPU->setZF_Flag(true);
		return;//因为buffer是循环队列结构。。相等就表示为空
		
	}
	else
	{
		pCPU->ax =*(U2*)(memStartAddr+*headPointer);
		pCPU->setZF_Flag(false);
	}
	
}
void PCKeyBoard::getKBShiftStatus()
{//获取shift标志状态ah=2
	pCPU->al = 0x20;
}
void PCKeyBoard::setKBInPutRate()
{//设置键盘输入速度ah=3
}
void PCKeyBoard::setKBVoice()
{//调整键盘敲击声ah=4
}
void PCKeyBoard::storeKBInPutData()
{//将键数据装入键盘缓冲区ah=5
}
void PCKeyBoard::getKB_ID()
{//获取键盘ID ah=A
}
bool PCKeyBoard::receiveKBData(const U2 scan_Ascii)
{
	if(isBufferFull())
		return false;//队列已满，不再处理，返回false
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
{// 循环队列。。头指针加一
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
#ifndef _DEVICE_H
#define _DEVICE_H
#include"common.h"
#include<vector>
#include "Hardware.h"
class MotherBoard;
class CentreProcessorUnit;
using namespace std;
class Device:public Hardware
{
public:
	Device();
	~Device();
	void linkMotherBoard(MotherBoard* pMB );
	virtual void init()=0;//初始化是必须的
	virtual vector<int> get_INT_shouldHandle()=0;//得到这个这杯需要处理的INT 号
	virtual void HandleINT(U1 INT_num)=0;
	MotherBoard* pMB;
	CentreProcessorUnit* pCPU;

	U1* memStartAddr;

};
#endif
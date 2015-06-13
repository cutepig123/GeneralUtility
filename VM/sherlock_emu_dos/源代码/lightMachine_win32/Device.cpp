#include "stdafx.h"
#include"Device.h"
#include"MotherBoard.h"

Device::Device()
{
}
Device::~Device()
{
}
void Device::linkMotherBoard(MotherBoard* pMB )
{//motherboard会自动调用的。。。
	this->pMB = pMB;
	this->pCPU = pMB->pCPU;
	memStartAddr = pMB->getMemStartAddr();
	init();
}

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
{//motherboard���Զ����õġ�����
	this->pMB = pMB;
	this->pCPU = pMB->pCPU;
	memStartAddr = pMB->getMemStartAddr();
	init();
}

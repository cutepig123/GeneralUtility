#include "StdAfx.h"
#include "SystemTimer.h"

// �ӵ���IR8
#define IntTimer 0x00000001


void SysTimerThread()
{
	IRR |= IntTimer;			//ʵʱʱ���ж�
}

void IO_Write_0058()
{

}

void SystemTimerInit()
{
	//SetTimer(NULL,2,55,(TIMERPROC)SysTimerThread);
	OutTable[0x0058]=(unsigned int)IO_Write_0058;
}

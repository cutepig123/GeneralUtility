#include "StdAfx.h"
#include "SystemTimer.h"

// 接到到IR8
#define IntTimer 0x00000001


void SysTimerThread()
{
	IRR |= IntTimer;			//实时时钟中断
}

void IO_Write_0058()
{

}

void SystemTimerInit()
{
	//SetTimer(NULL,2,55,(TIMERPROC)SysTimerThread);
	OutTable[0x0058]=(unsigned int)IO_Write_0058;
}

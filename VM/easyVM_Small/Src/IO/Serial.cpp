#include "StdAfx.h"
#include "Serial.h"

void IO_Write_0014()
{

}

void SerialInit()
{
	OutTable[0x0014]=(unsigned int)IO_Write_0014;
}
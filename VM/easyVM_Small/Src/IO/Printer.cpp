#include "StdAfx.h"
#include "Printer.h"

void IO_Write_0017()
{

}

void PrinterInit()
{
	OutTable[0x0017]=(unsigned int)IO_Write_0017;
}
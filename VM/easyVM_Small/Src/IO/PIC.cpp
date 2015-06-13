#include "StdAfx.h"
#include "PIC.h"
#include "..\cpu\cpu.h"

//这个PIC的中断优先级为从0到31,只有这一种方式
//由IRR和IMR共同决定ISR，CPU只可以见到ISR，I/O设备只可以见到IRR。IMR通过I/O指令设置。

unsigned int IRR;
unsigned int IMR;
unsigned int ISR;


void IO_Write_0020()
{
	int tempC;
	int i;
	if (eCPU.al & 0x10)		// ICW1 ?
	{
	}
	else					// OCW2
	{
		tempC=eCPU.al & 0xe0;	// R、SL、EOI三位
		switch(tempC)
		{
		case 0x00:
		case 0x20:
			i=0;					// 把ISR中优先级最高的1恢复为0
			while (!(ISR & (1<<i)) && i<32)
				i++;
			if (i<32)
				ISR^=(1<<i);
			break;
		case 0x60:
			i=eCPU.al&0x07;			// 复位ISR中由L2L1L0指定的位
			ISR&=(1<<i);
			break;		
		}
	}
}

void IO_Read_0021()
{
	
}


// CPU询问是否有新的中断的函数
char InterruptRequest()
{
	ISR=IRR&(~IMR);
	int p=1;
	for (int i=0;i<32;i++)
	{
		if ((ISR & p))
		{
			return i;
		}
		p<<=1;
	}
	return -1;
}


// CPU响应中断
void InterruptRespond(char i)
{
	int p=1;
	while (i>0)
	{
		p<<=1;
		i--;
	}
	ISR &= !p;
	IRR &= !p;
}


void PICInit()
{
	OutTable[0x0020]=(unsigned int)IO_Write_0020;
	return;
}
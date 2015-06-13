#include "StdAfx.h"
#include "PIC.h"
#include "..\cpu\cpu.h"

//���PIC���ж����ȼ�Ϊ��0��31,ֻ����һ�ַ�ʽ
//��IRR��IMR��ͬ����ISR��CPUֻ���Լ���ISR��I/O�豸ֻ���Լ���IRR��IMRͨ��I/Oָ�����á�

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
		tempC=eCPU.al & 0xe0;	// R��SL��EOI��λ
		switch(tempC)
		{
		case 0x00:
		case 0x20:
			i=0;					// ��ISR�����ȼ���ߵ�1�ָ�Ϊ0
			while (!(ISR & (1<<i)) && i<32)
				i++;
			if (i<32)
				ISR^=(1<<i);
			break;
		case 0x60:
			i=eCPU.al&0x07;			// ��λISR����L2L1L0ָ����λ
			ISR&=(1<<i);
			break;		
		}
	}
}

void IO_Read_0021()
{
	
}


// CPUѯ���Ƿ����µ��жϵĺ���
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


// CPU��Ӧ�ж�
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
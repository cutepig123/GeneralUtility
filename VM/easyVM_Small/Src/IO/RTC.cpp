#include "StdAfx.h"
#include "../cpu/cpu.h"
#include "../cpu/Instruction.h"
#include "../easyVM.h"
#include "RTC.h"

#define TICK 54.9255

//依次是 厘秒、秒、分、时、日、月、年、世纪
unsigned char rTime[8];		//十六进制存放，方便运算
unsigned char BCDTIme[8];	//BDC码存放
unsigned int *RtcTick;
unsigned char *RtcMidNight;

char carry[7]={100,60,60,24,31,12,100};		//八个寄存器之间的进位关系

unsigned int RWpnt;
bool A20Gate;								//A20地址线，只是为了92端口有个操作，其实它总是打开的。

int t;

void IO_Write_0070()
{
	RWpnt=eCPU.al&0x7f;
}

void IO_Write_0071()
{

}

void IO_Read_0071()			//简化IO
{
	switch(RWpnt)
	{
	case 0xb:
		eCPU.al=0x2;
		break;
	case 0xf:
		eCPU.al=0;
		break;
	default:
		eCPU.al=0;
		break;
	}	
}

unsigned char BCDtoBin(unsigned char bcd)
{
	return (bcd & 0xf) + ((bcd >> 4) & 0xf) * 10;
}

// 这个IO函数用来模拟INT 1Ah，在INT 1Ah的中断服务程序里调用OUT 73h,al
void IO_Write_0073()
{
	switch(eCPU.ah)
	{
	case 0:
		eCPU.dx=(*RtcTick) & 0xffff;
		eCPU.cx=((*RtcTick) >> 16) & 0xffff;
		eCPU.al=(*RtcTick) / 8640000;			//每天有8640000个Tick
		break;
	case 1:
		(*RtcTick)=eCPU.cx;
		(*RtcTick)<<=16;
		(*RtcTick)+=eCPU.dx;
		break;
	case 2:
		eCPU.ch=BCDTIme[3];
		eCPU.cl=BCDTIme[2];
		eCPU.dh=BCDTIme[1];
		eCPU.dl=0;
		(*(unsigned short*)((t=eCPU.ss,t<<=4)+eCPU.sp+4+MemoryStart))&=~CF;		
		break;
	case 3:
		rTime[3]=BCDtoBin(eCPU.ch);
		rTime[2]=BCDtoBin(eCPU.cl);
		rTime[1]=BCDtoBin(eCPU.dh);
		break;
	case 4:
		eCPU.ch=BCDTIme[7];
		eCPU.cl=BCDTIme[6];
		eCPU.dh=BCDTIme[5];
		eCPU.dl=BCDTIme[4];
		(*(unsigned short*)((t=eCPU.ss,t<<=4)+eCPU.sp+4+MemoryStart))&=~CF;		
		break;
	case 5:
		rTime[7]=BCDtoBin(eCPU.ch);
		rTime[6]=BCDtoBin(eCPU.cl);
		rTime[5]=BCDtoBin(eCPU.dh);
		rTime[4]=BCDtoBin(eCPU.dl);
		break;
	case 6:
		(*(unsigned short*)((t=eCPU.ss,t<<=4)+eCPU.sp+4+MemoryStart))|=CF;		
		break;
	default:
		break;
	}
}

void IO_Write_0092()
{
	if (eCPU.al&0x02)	
		A20Gate=true;
	else
		A20Gate=false;
}

void IO_Read_0092()
{
	if (A20Gate)
		eCPU.al=2;
	else
		eCPU.al=0;
}


// INT 15的中断服务程序，有一小部分功能号已经在BIOS里实现
void IO_Write_00B1()
{
	switch(eCPU.ah)
	{
	case 0x24:
		switch(eCPU.al)
		{
		case 0x03:
			(*(unsigned short*)((t=eCPU.ss,t<<=4)+eCPU.sp+4+MemoryStart))&=~CF;
			eCPU.ah=0;
			eCPU.bx=3;
			break;
		}
		break;
	case 0x88:
		(*(unsigned short*)((t=eCPU.ss,t<<=4)+eCPU.sp+4+MemoryStart))&=~CF;
		if (MemorySize>16)		
			eCPU.ax=0x3c00;					
		else		
			eCPU.ax=MemorySize*1024-256;
		break;
	case 0xd8:
		(*(unsigned short*)((t=eCPU.ss,t<<=4)+eCPU.sp+4+MemoryStart))|=CF;
		eCPU.ah=0x86;
		break;
	}
}


void TranslToBCD()
{
	for (int i=0;i<8;i++)
	{
		BCDTIme[i]=rTime[i]/10;
		BCDTIme[i]<<=4;
		BCDTIme[i]+=rTime[i]%10;
	}
}

void RTCTimerThread()
{
	(*RtcTick)++;

	rTime[7]++;
	for (int i=0;i<7;i++)
	{
		if (rTime[i]>carry[i])
		{
			rTime[i+1]++;
			rTime[i]%=carry[i];
		}
	}
	switch(rTime[5])		//Month
	{
	case 2:
		if ((rTime[6]%4==0 && rTime[7]%4) || (rTime[6]==0 && rTime[7]%4==0))
		{
			rTime[5]+=rTime[4]/29;
			rTime[4]%=29;
		}
		else
		{
			rTime[5]+=rTime[4]/28;
			rTime[4]%=28;
		}
		break;
	case 4:
	case 6:
	case 9:
	case 11:
		rTime[5]+=rTime[4]/30;
		rTime[4]%=30;
		break;
	}
	TranslToBCD();
}

void RTCInit()
{
    SYSTEMTIME st;
    GetLocalTime(&st); 
	rTime[0]=st.wMilliseconds/10;
	rTime[1]=(unsigned char)st.wSecond;
	rTime[2]=(unsigned char)st.wMinute;
	rTime[3]=(unsigned char)st.wHour;
	rTime[4]=(unsigned char)st.wDay;
	rTime[5]=(unsigned char)st.wMonth;
	rTime[6]=st.wYear%100;
	rTime[7]=st.wYear/100;
	TranslToBCD();
	RtcTick=(unsigned int *)(MemoryStart+0x46c);
	RtcMidNight=(unsigned char *)(MemoryStart+0x470);
	(*RtcTick)=(unsigned int)(((((double)((st.wHour)*60)+st.wMinute)*60+st.wSecond)*1000+st.wMilliseconds)/TICK);
	if (st.wHour>=12)		
		(*RtcMidNight)=0;
	else
		(*RtcMidNight)=0;
	//SetTimer(NULL,NULL,10,(TIMERPROC)RTCTimerThread);	

	OutTable[0x0073]=(unsigned int)IO_Write_0073;
	OutTable[0x0070]=(unsigned int)IO_Write_0070;
	OutTable[0x0071]=(unsigned int)IO_Write_0071;
	OutTable[0x0092]=(unsigned int)IO_Write_0092;
	OutTable[0x00B1]=(unsigned int)IO_Write_00B1;
	InTable[0x0071]=(unsigned int)IO_Read_0071;
	InTable[0x0092]=(unsigned int)IO_Read_0092;
}

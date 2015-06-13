#include "StdAfx.h"
#include "DMA.h"
#include "../cpu/cpu.h"
#include "../cpu/Instruction.h"
#include "Floppy.h"
#include "HardDisk.h"
#include <memory.h>

//	I/O
//	0000(byte)	--	选择通道
//	0001(byte)	--	连接写四个字节的起始地址，再连续写四个字节的字节数，从低位到高位
//	0002(byte)	--	模式寄存器，可读写，在向其写数据后立即开始传送。


// 内部寄存器
char ChnReg[4][2][4];		//四通道的起始地址和字节数，四字节的寄存器
char CmdReg;				//命令寄存器
char ModReg;				//模式寄存器
char *CurChn;				//I/O 1通道 要写的寄存器的指针

unsigned int Src,Des,tAd;				//数据复制的起始地址，内部使用
unsigned int BlockLen;		//数据复制的字节数，内部使用

//先写一个端口0,用于选择通道
//再写四个端口1,用于设定内存块的起始地址
//再写四个端口1,用于设定要传输的字节数
void IO_Write_0000(WORD data)	//参数都是通过栈传递的，所以要用WORD
{
	CurChn=&(ChnReg[(BYTE)data][0][0]);
}

void IO_Write_0001(WORD data)
{
	*CurChn=(BYTE)data;
	CurChn++;
}

void IO_Write_0002(WORD data)	//参考课本 P261
{
	ModReg=(BYTE)data;
	char tChn=ModReg & 0x03;
	char tReadWrite=(ModReg >> 2) & 0x03;
	char tDec=(ModReg >> 5) & 0x01;
	char tMode=(ModReg >> 6) & 0x03;

	switch(tChn)
	{
	case 2:
		tAd=FloppyIOPoint;		//软盘占用DMA通道2
		break;
	case 3:
		tAd=HardDiskIOPoint;	//硬盘占用DMA通道3
		break;
	default:
		return;
	}

	switch(tReadWrite)
	{
	case 1:
		Des=(unsigned int)*(unsigned short *)(&ChnReg[tChn][0][2]);
		Des<<=4;														//先取段基地
		Des+=(unsigned int)*(unsigned short *)(&ChnReg[tChn][0][0]);		//再取段内偏移
		Des+=MemoryStart;
		Src=tAd;
		break;
	case 2:
		Src=(unsigned int)*(unsigned short *)(&ChnReg[tChn][0][2]);
		Src<<=4;														//先取段基地
		Src+=(unsigned int)*(unsigned short *)(&ChnReg[tChn][0][0]);		//再取段内偏移
		Src+=MemoryStart;
		Des=tAd;
		break;
	}

	memcpy((void *)Des,(const void *)Src,*(unsigned int *)(&ChnReg[tChn][1][0]));
	
	IRR|=0x40;				//PIC的中断号是从8开始的，所以0x40是Int e
}

//硬盘操作完了之后会产生INT e中断，这是int e的处理程序。
void IO_Write_005E()
{
	
}

//猜测，中断服务程序里对Flags寄存器的修改应该是修改栈里的flags
void IO_Write_0003()
{
	int t;
	switch(eCPU.ah)
	{
	case 0:
		(*(unsigned short*)((t=eCPU.ss,t<<=4)+eCPU.sp+4+MemoryStart)) &= ~CF;		
		eCPU.ah=0;
		break;
	case 1:
		if (eCPU.dl<0x80)		
			eCPU.ah=0;
		else
			eCPU.ah=0x80;
		break;
	case 2:
	case 3:
		if (FloppyInserted[eCPU.dl] && eCPU.dl <0x80 || eCPU.dl >=0x80)
		//if (1)
		{
			if (eCPU.dl<0x80)
			{
				IO_Write_03F0(0);
				IO_Write_03F1(eCPU.cl&0x3f);
				IO_Write_03F0(1);
				IO_Write_03F1(eCPU.dh);
				IO_Write_03F0(2);		
				IO_Write_03F1(eCPU.ch+((eCPU.cl&0xc0)<<2));
				IO_Write_03F0(3);
				IO_Write_03F1(eCPU.dl);
				IO_Write_0000(2);					//选择通道
			}
			else
			{
				IO_Write_03F6(0);
				IO_Write_03F7(eCPU.cl&0x3f);
				IO_Write_03F6(1);
				IO_Write_03F7(eCPU.dh);
				IO_Write_03F6(2);		
				IO_Write_03F7(eCPU.ch+((eCPU.cl&0xc0)<<2));
				IO_Write_03F6(3);
				IO_Write_03F7(eCPU.dl-0x80);
				IO_Write_0000(3);					//选择通道
			}
			IO_Write_0001(eCPU.bl);				//内存块起始地址
			IO_Write_0001(eCPU.bh);
			IO_Write_0001(eCPU.es&0xff);
			IO_Write_0001((eCPU.es&0xff00)>>8);
			IO_Write_0001(0);					//传输的字节数，＝扇区数*200h
			IO_Write_0001(eCPU.al*2);
			IO_Write_0001((eCPU.al&0x80)>>7);
			IO_Write_0001(0);
			if (eCPU.dl<0x80)
			{										//软盘
				if (eCPU.ah==2)
					IO_Write_0002(0x96);			//读磁盘
				else
					IO_Write_0002(0x9a);			//写磁盘
			}
			else
			{										//硬盘
				if (eCPU.ah==2)
					IO_Write_0002(0x97);			//读磁盘
				else
					IO_Write_0002(0x9b);			//写磁盘
			}
		}
		else
		{
			(*(unsigned short*)((t=eCPU.ss,t<<=4)+eCPU.sp+4+MemoryStart))|=CF;
			eCPU.ah=0x80;
		}
		break;	
	case 4:
		break;
	case 8:
		if (eCPU.dl<0x80)
		{
			(*(unsigned short*)((t=eCPU.ss,t<<=4)+eCPU.sp+4+MemoryStart))&=~CF;
			eCPU.ah=0;
			eCPU.bl=4;
			eCPU.cx=0x4f12;
			eCPU.dx=0x0102;
			eCPU.es=*(unsigned short*)(MemoryStart+0x1e*4+2);	//INT 1Eh,磁盘参数表
			eCPU.di=*(unsigned short*)(MemoryStart+0x1e*4);
		}
		else
		{
			if (*(unsigned char *)(MemoryStart+0x475))
			{												//有硬盘
				(*(unsigned short*)((t=eCPU.ss,t<<=4)+eCPU.sp+4+MemoryStart))&=~CF;
				eCPU.ah=0;
				eCPU.ch=(HdCnt[eCPU.dl-0x80][0]&0xff)-2;
				eCPU.cl=63+((HdCnt[eCPU.dl-0x80][0]&0x300)>>2);
				eCPU.dh=HdCnt[eCPU.dl-0x80][1]-1;
				eCPU.dl=*(unsigned char *)(MemoryStart+0x475);
// 				eCPU.es=*(unsigned short*)(MemoryStart+0x1e*4+2);	//INT 1Fh,磁盘参数表
// 				eCPU.di=*(unsigned short*)(MemoryStart+0x1e*4);				
			}
			else
			{												//无硬盘
				(*(unsigned short*)((t=eCPU.ss,t<<=4)+eCPU.sp+4+MemoryStart))|=CF;
				eCPU.ah=0x1;
			}
		}
		break;
	case 0x10:
		break;
	case 0x15:
		if (eCPU.dl<0x02)		//暂时只支持两个软盘
		{
			(*(unsigned short*)((t=eCPU.ss,t<<=4)+eCPU.sp+4+MemoryStart))&=~CF;
			eCPU.ah=1;
		}
		else
		{
			(*(unsigned short*)((t=eCPU.ss,t<<=4)+eCPU.sp+4+MemoryStart))&=~CF;
			eCPU.ah=0;
		}
		break;
	}
}

void DMAInit()
{
	//修改BIOS数据区，easyVM装有磁盘
	*(unsigned short *)(MemoryStart+0x410) |= 0x0001;
	OutTable[0x0000]=(unsigned int)IO_Write_0000;
	OutTable[0x0001]=(unsigned int)IO_Write_0001;
	OutTable[0x0002]=(unsigned int)IO_Write_0002;
	OutTable[0x0003]=(unsigned int)IO_Write_0003;
	OutTable[0x005e]=(unsigned int)IO_Write_005E;
}
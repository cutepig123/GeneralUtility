#include "StdAfx.h"
#include "DMA.h"
#include "../cpu/cpu.h"
#include "../cpu/Instruction.h"
#include "Floppy.h"
#include "HardDisk.h"
#include <memory.h>

//	I/O
//	0000(byte)	--	ѡ��ͨ��
//	0001(byte)	--	����д�ĸ��ֽڵ���ʼ��ַ��������д�ĸ��ֽڵ��ֽ������ӵ�λ����λ
//	0002(byte)	--	ģʽ�Ĵ������ɶ�д��������д���ݺ�������ʼ���͡�


// �ڲ��Ĵ���
char ChnReg[4][2][4];		//��ͨ������ʼ��ַ���ֽ��������ֽڵļĴ���
char CmdReg;				//����Ĵ���
char ModReg;				//ģʽ�Ĵ���
char *CurChn;				//I/O 1ͨ�� Ҫд�ļĴ�����ָ��

unsigned int Src,Des,tAd;				//���ݸ��Ƶ���ʼ��ַ���ڲ�ʹ��
unsigned int BlockLen;		//���ݸ��Ƶ��ֽ������ڲ�ʹ��

//��дһ���˿�0,����ѡ��ͨ��
//��д�ĸ��˿�1,�����趨�ڴ�����ʼ��ַ
//��д�ĸ��˿�1,�����趨Ҫ������ֽ���
void IO_Write_0000(WORD data)	//��������ͨ��ջ���ݵģ�����Ҫ��WORD
{
	CurChn=&(ChnReg[(BYTE)data][0][0]);
}

void IO_Write_0001(WORD data)
{
	*CurChn=(BYTE)data;
	CurChn++;
}

void IO_Write_0002(WORD data)	//�ο��α� P261
{
	ModReg=(BYTE)data;
	char tChn=ModReg & 0x03;
	char tReadWrite=(ModReg >> 2) & 0x03;
	char tDec=(ModReg >> 5) & 0x01;
	char tMode=(ModReg >> 6) & 0x03;

	switch(tChn)
	{
	case 2:
		tAd=FloppyIOPoint;		//����ռ��DMAͨ��2
		break;
	case 3:
		tAd=HardDiskIOPoint;	//Ӳ��ռ��DMAͨ��3
		break;
	default:
		return;
	}

	switch(tReadWrite)
	{
	case 1:
		Des=(unsigned int)*(unsigned short *)(&ChnReg[tChn][0][2]);
		Des<<=4;														//��ȡ�λ���
		Des+=(unsigned int)*(unsigned short *)(&ChnReg[tChn][0][0]);		//��ȡ����ƫ��
		Des+=MemoryStart;
		Src=tAd;
		break;
	case 2:
		Src=(unsigned int)*(unsigned short *)(&ChnReg[tChn][0][2]);
		Src<<=4;														//��ȡ�λ���
		Src+=(unsigned int)*(unsigned short *)(&ChnReg[tChn][0][0]);		//��ȡ����ƫ��
		Src+=MemoryStart;
		Des=tAd;
		break;
	}

	memcpy((void *)Des,(const void *)Src,*(unsigned int *)(&ChnReg[tChn][1][0]));
	
	IRR|=0x40;				//PIC���жϺ��Ǵ�8��ʼ�ģ�����0x40��Int e
}

//Ӳ�̲�������֮������INT e�жϣ�����int e�Ĵ������
void IO_Write_005E()
{
	
}

//�²⣬�жϷ���������Flags�Ĵ������޸�Ӧ�����޸�ջ���flags
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
				IO_Write_0000(2);					//ѡ��ͨ��
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
				IO_Write_0000(3);					//ѡ��ͨ��
			}
			IO_Write_0001(eCPU.bl);				//�ڴ����ʼ��ַ
			IO_Write_0001(eCPU.bh);
			IO_Write_0001(eCPU.es&0xff);
			IO_Write_0001((eCPU.es&0xff00)>>8);
			IO_Write_0001(0);					//������ֽ�������������*200h
			IO_Write_0001(eCPU.al*2);
			IO_Write_0001((eCPU.al&0x80)>>7);
			IO_Write_0001(0);
			if (eCPU.dl<0x80)
			{										//����
				if (eCPU.ah==2)
					IO_Write_0002(0x96);			//������
				else
					IO_Write_0002(0x9a);			//д����
			}
			else
			{										//Ӳ��
				if (eCPU.ah==2)
					IO_Write_0002(0x97);			//������
				else
					IO_Write_0002(0x9b);			//д����
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
			eCPU.es=*(unsigned short*)(MemoryStart+0x1e*4+2);	//INT 1Eh,���̲�����
			eCPU.di=*(unsigned short*)(MemoryStart+0x1e*4);
		}
		else
		{
			if (*(unsigned char *)(MemoryStart+0x475))
			{												//��Ӳ��
				(*(unsigned short*)((t=eCPU.ss,t<<=4)+eCPU.sp+4+MemoryStart))&=~CF;
				eCPU.ah=0;
				eCPU.ch=(HdCnt[eCPU.dl-0x80][0]&0xff)-2;
				eCPU.cl=63+((HdCnt[eCPU.dl-0x80][0]&0x300)>>2);
				eCPU.dh=HdCnt[eCPU.dl-0x80][1]-1;
				eCPU.dl=*(unsigned char *)(MemoryStart+0x475);
// 				eCPU.es=*(unsigned short*)(MemoryStart+0x1e*4+2);	//INT 1Fh,���̲�����
// 				eCPU.di=*(unsigned short*)(MemoryStart+0x1e*4);				
			}
			else
			{												//��Ӳ��
				(*(unsigned short*)((t=eCPU.ss,t<<=4)+eCPU.sp+4+MemoryStart))|=CF;
				eCPU.ah=0x1;
			}
		}
		break;
	case 0x10:
		break;
	case 0x15:
		if (eCPU.dl<0x02)		//��ʱֻ֧����������
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
	//�޸�BIOS��������easyVMװ�д���
	*(unsigned short *)(MemoryStart+0x410) |= 0x0001;
	OutTable[0x0000]=(unsigned int)IO_Write_0000;
	OutTable[0x0001]=(unsigned int)IO_Write_0001;
	OutTable[0x0002]=(unsigned int)IO_Write_0002;
	OutTable[0x0003]=(unsigned int)IO_Write_0003;
	OutTable[0x005e]=(unsigned int)IO_Write_005E;
}
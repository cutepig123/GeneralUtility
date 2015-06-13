//////////////////////////////////////////////////////////////////////////
// ����	��CPU.cpp
// ����	��ģ��CPU
// ����	��2008��4��20��
// ����	����һ��
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "cpu.h"
#include "Instruction.h"
#include "../easyVM.h"
#include "../io/PIC.h"
#include "../io/Display.h"
#include <stdio.h>
#include <math.h>

#define eIMS (evIP+MemoryStart)
#define EvSP (t=eCPU.ss,(t<<4)+eCPU.sp)

CPU eCPU;					//�������CPU��ʵ��
unsigned int evIP;			//evIP��ָ��ʱ��ָ��
unsigned int GlobINT;		//���з�INTָ��������ж�
bool bStartRecord=false;			//��ĳ��λ�ÿ�ʼ���������״̬
unsigned int InstructionCount=0;

void IO_NOP()		//δʵ�ֵ�IO��
{
#ifdef _VM_DEBUG_
	fprintf(fDebug,"********************** %4x:%4x%4x%3x ******************** IO Not supported\n",eCPU.cs,eCPU.ip,*(unsigned char *)(evIP+MemoryStart-1),*(unsigned char *)(evIP+MemoryStart));
#endif
	__asm mov eax,0xcccccccc				//Debug�汾������һ�䣬��ʹ��������cc������Release�汾��û���ˡ�
											//�˿ڷ���CC�պø�ʹĳЩ����������У�������������ȷ������Ӧ����ʵ���Ǹ��˿ڡ�����3DAh�˿�
	__asm nop
}

void IO_Write_00CD()		//δʵ�ֵ��ж�
{
#ifdef _VM_DEBUG_
	fprintf(fDebug,"********************** %4x:%4x%4x%3x ******************** Interrupt Not supported\n",eCPU.cs,eCPU.ip,*(unsigned char *)(evIP+MemoryStart-1),*(unsigned char *)(evIP+MemoryStart));
#endif
	__asm mov eax,0xcccccccc
	__asm nop
}

//////////////////////////////////////////////////////////////////////////
// eCPU�ĳ�ʼ��
int CPUInit()
{
#ifdef _VM_DEBUG_ 
	fDebug=fopen("Debug.txt","w");
#endif
	memset(&eCPU,0,sizeof(eCPU));
	SetupInstructionTable();
	eCPU.cs=0xf000;
	eCPU.ip=0xfff0;
	evIP=eCPU.cs;
	evIP<<=4;
	evIP+=eCPU.ip;

	for (int i=0;i<0x10000;i++)
	{
		InTable[i]=(unsigned int)IO_NOP;
		OutTable[i]=(unsigned int)IO_NOP;		
	}
	OutTable[0x00cd]=(unsigned int)IO_Write_00CD;
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// ִ��evIP��ָ�����һ��ָ��
int ExecIns()
{
	unsigned char OpC=*(unsigned char *)(evIP+MemoryStart);
	evIP++;	
	InstructionCount++;
#ifdef _VM_DEBUG_ 	
	if (eCPU.cs==0x8a00 && eCPU.ip==0x108)
		bStartRecord=true;
	
	//if (bStartRecord)	
	//if (eCPU.ip==0x98ee && eCPU.cs==0x19)
		fprintf(fDebug,"%4x:%4x%4x%3x%6x\n",eCPU.cs,eCPU.ip,OpC,*(unsigned char *)(evIP+MemoryStart),eCPU.bx);//*(unsigned short*)(MemoryStart+0x8000));
	fflush(fDebug);
	static unsigned short prg;
	static DWORD prm=0101;
	if (prg!=eCPU.cs)
	{
		prg=eCPU.cs;
		//fprintf(fDebug,"%4x:%4x%4x%3x%6x\n",eCPU.cs,eCPU.ip,OpC,*(unsigned char *)(evIP+MemoryStart),eCPU.flags);
	}
	if (eCPU.si==0x6d99)
	{
		__asm nop;
	}
	if (prm!=*(unsigned short*)(MemoryStart+0x11cd))
	{
		prm=*(unsigned short*)(MemoryStart+0x11cd);
		//fprintf(fDebug,"%4x:%4x%4x%3x%6x\n",eCPU.cs,eCPU.ip,OpC,*(unsigned char *)(evIP+MemoryStart),prm);
	}
#endif
	unsigned int InsFucAddr=InsTable[OpC];
	__asm call InsFucAddr;
	unsigned int tcs=eCPU.cs;
	eCPU.ip=(evIP - (tcs << 4)) % 0x10000;
	tmpDs=eCPU.ds;
	tmpSs=eCPU.ss;
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// ǰ��ĳ���жϷ������
void GoInt(int r)
{
	//LongCallNewIP(1);					//��ΪExcIns��������ִ��ָ�����޸�CS:IP�ģ��������е������CS:IP��δ��ִ�еģ����Բ�����LongCallNewIP�ˡ�
	unsigned int t;	
	eCPU.sp-=2;
	*(unsigned short*)( EvSP +MemoryStart)=eCPU.flags;
	eCPU.flags&=~IF;
	eCPU.flags&=~TF;
	eCPU.flags&=~AF;
	eCPU.sp-=2;
	*(unsigned short*)( EvSP +MemoryStart)=eCPU.cs;				//��ѹCS����ѹIP��������ҪѹCALLָ��֮���IP
	eCPU.sp-=2;
	*(unsigned short*)( EvSP +MemoryStart)=eCPU.ip;
	eCPU.ip=*(unsigned short *)((r)*4+MemoryStart);
	eCPU.cs=*(unsigned short *)((r)*4+2+MemoryStart);
	evIP=((t=eCPU.cs,t<<4))+eCPU.ip;
}


//////////////////////////////////////////////////////////////////////////
// ����Ƿ����ж����󣬲���������Ӧ����
int ExecInt()
{	
	char intR;
	unsigned int tmpGlobINT;
	unsigned int IntNo;
	if (eCPU.flags&TF)
	{
		GlobINT|=0x2;
		eCPU.flags&=~TF;
	}
	tmpGlobINT=GlobINT;
	IntNo=0;

	while (!(tmpGlobINT&1) && IntNo<8)		//��������7���жϣ��������ӵ�8259�ϵ��жϣ�
	{
		IntNo++;
	}

	if (IntNo==2)
	{
		GoInt(2);							//���������ж�
		GlobINT&=~0x2;
		return IntNo;
	}

	if (eCPU.flags&IF)
	{	
		if (IntNo<8)
		{
			GoInt(IntNo);
			GlobINT&=~(unsigned int)(pow(2,IntNo));
			return IntNo;
		}
		intR=InterruptRequest();
		if (intR<0)
			return -1;
		GoInt(intR+8);						//PIC��IR0���жϺ�8	
		InterruptRespond(intR);
		return intR+8;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////
// ����������ڴ����������VideoRAM
void RefreshVideoRAM()
{
	int i;
	static unsigned int preAddr8,preAddr16;
	unsigned int addr8=(unsigned int )rm8-MemoryStart;
	unsigned int addr16=(unsigned int )rm16-MemoryStart;
	if (addr8>=0xa0000 && addr8<=0xde800 && preAddr8!=addr8)
	{
		WriteVideoRam(addr8-0xa0000);
		preAddr8=addr8;
	}
	if (addr16>=0xa0000 && addr16<=0xde800)
	{
		for (i=0;i<4;i++)
		{
			WriteVideoRam(addr16-0xa0000+i);
		}		
		preAddr16=addr16;
	}
}

//////////////////////////////////////////////////////////////////////////
// CPU���У���һ��ѭ��
int CPURun()
{
	while (1)
	{
		ExecIns();		//ִ��ָ��
		RefreshVideoRAM();
		ExecInt();		//����ж�
#ifdef _VM_DEBUG_
		if (eCPU.ip==0xa6ba && eCPU.cs==0x1cd)
		{
			__asm nop							//���﷽���Լ����Գ���			
		}
		if (eCPU.ip==0x169 && eCPU.cs==0x8a00)
		{
			__asm nop							//���﷽���Լ����Գ���			
		}
		if (eCPU.ip==0x3d7 && eCPU.cs==0x1e10)
		{
			__asm nop							//���﷽���Լ����Գ���			
		}
#endif
	}	
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// easyVM�ػ�ʱ������
void CPUTerminate()
{
#ifdef _VM_DEBUG_ 
	fclose(fDebug);
#endif
}
//////////////////////////////////////////////////////////////////////////
// 名称	：CPU.cpp
// 功能	：模拟CPU
// 日期	：2008年4月20日
// 作者	：梁一信
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

CPU eCPU;					//虚拟机的CPU的实体
unsigned int evIP;			//evIP读指令时的指针
unsigned int GlobINT;		//所有非INT指令产生的中断
bool bStartRecord=false;			//从某个位置开始，进入调试状态
unsigned int InstructionCount=0;

void IO_NOP()		//未实现的IO口
{
#ifdef _VM_DEBUG_
	fprintf(fDebug,"********************** %4x:%4x%4x%3x ******************** IO Not supported\n",eCPU.cs,eCPU.ip,*(unsigned char *)(evIP+MemoryStart-1),*(unsigned char *)(evIP+MemoryStart));
#endif
	__asm mov eax,0xcccccccc				//Debug版本会有这一句，以使函数返回cc，但是Release版本则没有了。
											//端口返回CC刚好给使某些软件正常运行，这是运气。正确的做法应该是实现那个端口。例如3DAh端口
	__asm nop
}

void IO_Write_00CD()		//未实现的中断
{
#ifdef _VM_DEBUG_
	fprintf(fDebug,"********************** %4x:%4x%4x%3x ******************** Interrupt Not supported\n",eCPU.cs,eCPU.ip,*(unsigned char *)(evIP+MemoryStart-1),*(unsigned char *)(evIP+MemoryStart));
#endif
	__asm mov eax,0xcccccccc
	__asm nop
}

//////////////////////////////////////////////////////////////////////////
// eCPU的初始化
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
// 执行evIP所指向的那一条指令
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
// 前往某个中断服务程序
void GoInt(int r)
{
	//LongCallNewIP(1);					//因为ExcIns里面是先执行指令再修改CS:IP的，所以运行到这里的CS:IP是未被执行的，所以不用再LongCallNewIP了。
	unsigned int t;	
	eCPU.sp-=2;
	*(unsigned short*)( EvSP +MemoryStart)=eCPU.flags;
	eCPU.flags&=~IF;
	eCPU.flags&=~TF;
	eCPU.flags&=~AF;
	eCPU.sp-=2;
	*(unsigned short*)( EvSP +MemoryStart)=eCPU.cs;				//先压CS，再压IP，而且是要压CALL指令之后的IP
	eCPU.sp-=2;
	*(unsigned short*)( EvSP +MemoryStart)=eCPU.ip;
	eCPU.ip=*(unsigned short *)((r)*4+MemoryStart);
	eCPU.cs=*(unsigned short *)((r)*4+2+MemoryStart);
	evIP=((t=eCPU.cs,t<<4))+eCPU.ip;
}


//////////////////////////////////////////////////////////////////////////
// 检查是否有中断请求，并且作出相应处理
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

	while (!(tmpGlobINT&1) && IntNo<8)		//所有少于7的中断（即不连接到8259上的中断）
	{
		IntNo++;
	}

	if (IntNo==2)
	{
		GoInt(2);							//不可屏蔽中断
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
		GoInt(intR+8);						//PIC的IR0接中断号8	
		InterruptRespond(intR);
		return intR+8;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////
// 根据最近的内存操作来更新VideoRAM
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
// CPU运行，是一个循环
int CPURun()
{
	while (1)
	{
		ExecIns();		//执行指令
		RefreshVideoRAM();
		ExecInt();		//检查中断
#ifdef _VM_DEBUG_
		if (eCPU.ip==0xa6ba && eCPU.cs==0x1cd)
		{
			__asm nop							//这里方便自己调试程序			
		}
		if (eCPU.ip==0x169 && eCPU.cs==0x8a00)
		{
			__asm nop							//这里方便自己调试程序			
		}
		if (eCPU.ip==0x3d7 && eCPU.cs==0x1e10)
		{
			__asm nop							//这里方便自己调试程序			
		}
#endif
	}	
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// easyVM关机时被调用
void CPUTerminate()
{
#ifdef _VM_DEBUG_ 
	fclose(fDebug);
#endif
}
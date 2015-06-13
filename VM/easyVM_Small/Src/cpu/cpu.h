#ifndef _CPU_H
#define _CPU_H


extern unsigned int evIP;					//CS:IP����ʾ�����Ե�ַ��ֻ���ڲ�ʹ��
extern unsigned int MemoryStart;			//easyVM�������ڴ����ʼ��ַ
extern unsigned short tmpDs;				//eCPU.ds��һ����������ʱ�������ES֮���ָ��ǰ׺�����޸���������������ڵĶ�д���������������Ϊ׼�ģ�������eCPU.ds
extern unsigned short tmpSs;				//ͬ��
extern void LongCallNewIP(char OffsetByte);	//LongCall��ʱ�򣬰ѵ�ǰIP�޸ĳ���һ��ָ���IP���Ա�ѹջ
extern char VideoRAM[256000];
extern unsigned short *rm16;			//����Ѱַ�ֽڵ�ʱ����
extern unsigned char *rm8;				//


/*
8086CPU���ѷ���
//////////////////////////////////////////////////////////////////////////
// CPU�ṹ��
// ��Ҫʵ����8086�ļĴ���
// easyVMֻ��һ��CPU
struct CPU 
{
	union
	{
		struct  
		{
			unsigned char al,ah;
		};
		unsigned short ax;
	};
	union
	{
		struct  
		{
			unsigned char bl,bh;
		};
		unsigned short bx;
	};
	union
	{
		struct  
		{
			unsigned char cl,ch;
		};
		unsigned short cx;
	};
	union
	{
		struct  
		{
			unsigned char dl,dh;
		};
		unsigned short dx;
	};
	unsigned short sp;
	unsigned short bp;
	unsigned short si;
	unsigned short di;
	unsigned short ip;
	unsigned short flags;
	unsigned short cs;
	unsigned short ds;
	unsigned short es;
	unsigned short ss;
};
*/

struct ExtendedReal
{
	__int64 Fraction;
	short Exponent;
};
struct MMXI
{
	union
	{
		unsigned char mmxi8[8];
		unsigned short mmxi16[4];
		unsigned int mmxi32[2];
		unsigned __int64 mmxi64;
	};
};
struct MMXF
{
	double mmxf;
};

struct ExtendedReg 
{
	union
	{
		ExtendedReal er;
		MMXI mi;
		MMXF mf;
	};
};

//////////////////////////////////////////////////////////////////////////
// ����CPU�ṹ��
// ��Ҫʵ����80586�ļĴ���
// easyVMֻ��һ��CPU
// CPU��FPU����һ���ˡ�
struct CPU 
{
//////////////////////////////////////////////////////////////////////////
	union
	{
		union
		{
			struct  
			{
				unsigned char al,ah;
			};
			unsigned short ax;
		};
		unsigned int eax;
	};
	union
	{
		union
		{
			struct  
			{
				unsigned char cl,ch;
			};
			unsigned short cx;
		};
		unsigned int ecx;
	};
	union
	{
		union
		{
			struct  
			{
				unsigned char dl,dh;
			};
			unsigned short dx;
		};
		unsigned int edx;
	};
	union
	{
		union
		{
			struct  
			{
				unsigned char bl,bh;
			};
			unsigned short bx;
		};
		unsigned int ebx;
	};
	union
	{
		unsigned short sp;
		unsigned int esp;
	};	
	union
	{
		unsigned short bp;
		unsigned int ebp;
	};	
	union
	{
		unsigned short si;
		unsigned int esi;
	};	
	union
	{
		unsigned short di;
		unsigned int edi;
	};	
	union
	{
		unsigned short ip;
		unsigned int eip;
	};	
	union
	{
		unsigned short flags;
		unsigned int eflags;
	};	
	unsigned short cs;
	unsigned short ds;
	unsigned short es;
	unsigned short ss;
	unsigned short fs;
	unsigned short gs;
//////////////////////////////////////////////////////////////////////////
	unsigned int CR[4];
	unsigned int DR[8];
//////////////////////////////////////////////////////////////////////////	
	ExtendedReg FpuR[8];
	unsigned short FpuCR;
	unsigned short FpuSR;
	unsigned short FpuTR;
	struct FpuIP 
	{
		unsigned short seg;
		unsigned int off;
	}Fpuip;
	struct FpuDP						//����Instruction Pointer��Operand Pointer������ͬһ���ṹ�壬Ҳ�����������ѽṹ��Ķ���д����
	{
		unsigned short seg;
		unsigned int off;
	}Fpudp;
	unsigned short FpuOpcode;			//ʵ����Opcode��11��bit�ġ�
//////////////////////////////////////////////////////////////////////////
	struct  
	{
		union
		{
			float fp[4];
			unsigned int ip[4];
		};
	}xmm[8];
};


extern CPU eCPU;
extern bool bStartRecord;			//��ĳ��λ�ÿ�ʼ���������״̬

int CPUInit();
int CPURun();
void CPUTerminate();
int ExecIns();
int ExecInt();

#endif
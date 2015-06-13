#include "StdAfx.h"
#include "Floppy.h"




//	I/O
//	03F0(byte)	--	选择要操作的内部寄存器
//	03F1(byte)	--	读、写该寄存器


unsigned int FloppyIOPoint;		//当前读写的数据的指针，供DMA使用
//	FloppyIOPoint=（（（柱面*磁头数）+磁头）*单道扇区数+扇区-1）*512

//  注意：
//  柱面比磁头更大
//  扇区从1开始编号


// 内部寄存器
char FlpReg[4];		//当前读写扇区的指针。  扇区、磁头、柱面、驱动器
char FlpCnt[3]={80,2,18};		//1.44MB的软盘有80个柱面，2个盘面（磁头），18个扇区
char RegPointer=0;

HANDLE hFloppy[4];				//一个软盘控制器最多连接4个驱动器
HANDLE hMap[4];
unsigned int SizeFloppy[4];
char *pBufFloppy[4];
bool FloppyInserted[4];

void IO_Write_03F0(WORD data)	//参数都是通过栈传递的，所以要用WORD
{
	RegPointer=(BYTE)data;
}

void IO_Write_03F1(WORD data)
{
	FlpReg[RegPointer]=(BYTE)data;
	FloppyIOPoint=0;
	for (int i=0;i<3;i++)
	{
		FloppyIOPoint*=FlpCnt[i];
		FloppyIOPoint+=FlpReg[2-i];
	}
	FloppyIOPoint-=1;					//扇区从1开始编号，所以计算偏移的时候要先减1
	FloppyIOPoint*=512;					//每个扇区512个字节
	FloppyIOPoint+=(unsigned int)pBufFloppy[FlpReg[3]];
}

void FloppyInit()
{
	//修改BIOS数据区，easyVM有两个软驱
	*(unsigned short *)(MemoryStart+0x410) |= 0x0060;

	char t[80];
	DWORD tDword;

	GetCurrentDirectory(sizeof(t),t);
	strcat(t,"\\easyVM.ini");
	GetPrivateProfileString("IMG","A","",t,sizeof(t),t);
	tDword=GetLastError();
	if (t[0]!=0)	
	{
		hFloppy[0]=CreateFile(t, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL); 
		SizeFloppy[0]=GetFileSize(hFloppy[0],&tDword);
		hMap[0]=CreateFileMapping(hFloppy[0],NULL,PAGE_READWRITE,0,SizeFloppy[0],"FloppyAMapping");
		pBufFloppy[0]=(char *)MapViewOfFile(hMap[0],FILE_MAP_ALL_ACCESS,0,0,SizeFloppy[0]);
		FloppyInserted[0]=true;
	}
	else
		FloppyInserted[0]=false;

	GetCurrentDirectory(sizeof(t),t);
	strcat(t,"\\easyVM.ini");	
	GetPrivateProfileString("IMG","B","",t,sizeof(t),t);
	if (t[0]!=0)
	{
		hFloppy[1]=CreateFile(t, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL); 
		SizeFloppy[1]=GetFileSize(hFloppy[1],&tDword);
		hMap[1]=CreateFileMapping(hFloppy[1],NULL,PAGE_READWRITE,0,SizeFloppy[1],"FloppyBMapping");
		pBufFloppy[1]=(char *)MapViewOfFile(hMap[1],FILE_MAP_ALL_ACCESS,0,0,SizeFloppy[1]);
		FloppyInserted[1]=true;
	}
	else
		FloppyInserted[1]=false;
	
	OutTable[0x03f0]=(unsigned int)IO_Write_03F0;
	OutTable[0x03f1]=(unsigned int)IO_Write_03F1;
}


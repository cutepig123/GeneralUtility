#include "StdAfx.h"
#include "HardDisk.h"
#include <stdlib.h>


//  硬盘总是有16个磁头、每个磁道有63个扇区，柱面数随容量而变，每个柱面大约有0.5MB容量。

//	I/O
//	03F6(byte)	--	选择要操作的内部寄存器
//	03F7(byte)	--	读、写该寄存器


unsigned int HardDiskIOPoint;		//当前读写的数据的指针，供DMA使用
//	HardDiskIOPoint=（（（柱面*磁头数）+磁头）*单道扇区数+扇区-1）*512

//  注意：
//  柱面比磁头更大
//  扇区从1开始编号


// 内部寄存器
unsigned int HdReg[4];		//当前读写扇区的指针。  扇区、磁头、柱面、驱动器
char HdCnt[4][3]={
	{0,16,63},
	{0,16,63},
	{0,16,63},
	{0,16,63},
};		//easyVM的硬盘总是有16个盘面（磁头），63个扇区，柱面数依据硬盘容量而定。
char HdRegPointer=0;

HANDLE hHardDisk[4];				//最多有4个硬盘
HANDLE HdhMap[4];
unsigned int SizeHardDisk[4];
char *pBufHardDisk[4];

void IO_Write_03F6(WORD data)	//参数都是通过栈传递的，所以要用WORD
{
	HdRegPointer=(BYTE)data;
}

void IO_Write_03F7(WORD data)
{
	HdReg[HdRegPointer]=(BYTE)data;
	HardDiskIOPoint=0;
	for (int i=0;i<3;i++)
	{
		HardDiskIOPoint*=HdCnt[HdReg[3]][i];		//HdCnt[][0]总是和0相乘的
		HardDiskIOPoint+=HdReg[2-i];
	}
	HardDiskIOPoint-=1;					//扇区从1开始编号，所以计算偏移的时候要先减1
	HardDiskIOPoint*=512;					//每个扇区512个字节
	HardDiskIOPoint+=(unsigned int)pBufHardDisk[HdReg[3]];
}

void HardDiskInit()
{

	char t[80];
	char iniRet[80];
	DWORD tDword;
	GetCurrentDirectory(sizeof(t),t);
	strcat(t,"\\easyVM.ini");
	GetPrivateProfileString("IMG","C","",iniRet,sizeof(iniRet),t);
	tDword=GetLastError();
	if (iniRet[0]!=0)	
	{
		hHardDisk[0]=CreateFile(iniRet, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL); 
		SizeHardDisk[0]=GetFileSize(hHardDisk[0],&tDword);
		HdhMap[0]=CreateFileMapping(hHardDisk[0],NULL,PAGE_READWRITE,0,SizeHardDisk[0],"HardDiskAMapping");
		pBufHardDisk[0]=(char *)MapViewOfFile(HdhMap[0],FILE_MAP_ALL_ACCESS,0,0,SizeHardDisk[0]);
		//HdCnt[0][0]=SizeHardDisk[0]/512/63/16;
		*(unsigned char *)(MemoryStart+0x474)=0;			// 硬盘上一次操作状态，默认是1的
		*(unsigned char *)(MemoryStart+0x475)=1;			// 硬盘个数+1
	}
	GetPrivateProfileString("IMG","spt","",iniRet,sizeof(iniRet),t);
	HdCnt[0][2]=atoi(iniRet);
	GetPrivateProfileString("IMG","heads","",iniRet,sizeof(iniRet),t);
	HdCnt[0][1]=atoi(iniRet);
	GetPrivateProfileString("IMG","cylinders","",iniRet,sizeof(iniRet),t);
	HdCnt[0][0]=atoi(iniRet);
	

	OutTable[0x03f6]=(unsigned int)IO_Write_03F6;
	OutTable[0x03f7]=(unsigned int)IO_Write_03F7;
}



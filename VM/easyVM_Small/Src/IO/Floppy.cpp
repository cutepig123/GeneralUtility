#include "StdAfx.h"
#include "Floppy.h"




//	I/O
//	03F0(byte)	--	ѡ��Ҫ�������ڲ��Ĵ���
//	03F1(byte)	--	����д�üĴ���


unsigned int FloppyIOPoint;		//��ǰ��д�����ݵ�ָ�룬��DMAʹ��
//	FloppyIOPoint=����������*��ͷ����+��ͷ��*����������+����-1��*512

//  ע�⣺
//  ����ȴ�ͷ����
//  ������1��ʼ���


// �ڲ��Ĵ���
char FlpReg[4];		//��ǰ��д������ָ�롣  ��������ͷ�����桢������
char FlpCnt[3]={80,2,18};		//1.44MB��������80�����棬2�����棨��ͷ����18������
char RegPointer=0;

HANDLE hFloppy[4];				//һ�����̿������������4��������
HANDLE hMap[4];
unsigned int SizeFloppy[4];
char *pBufFloppy[4];
bool FloppyInserted[4];

void IO_Write_03F0(WORD data)	//��������ͨ��ջ���ݵģ�����Ҫ��WORD
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
	FloppyIOPoint-=1;					//������1��ʼ��ţ����Լ���ƫ�Ƶ�ʱ��Ҫ�ȼ�1
	FloppyIOPoint*=512;					//ÿ������512���ֽ�
	FloppyIOPoint+=(unsigned int)pBufFloppy[FlpReg[3]];
}

void FloppyInit()
{
	//�޸�BIOS��������easyVM����������
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


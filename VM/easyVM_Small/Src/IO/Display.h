#pragma comment(lib,"gdiplus.lib")
#include <comdef.h>//��ʼ��һ��com��
#ifndef ULONG_PTR
#define ULONG_PTR unsigned long*
#include "GdiPlus.h"
using namespace Gdiplus;
#endif


#include "../cpu/cpu.h"


extern unsigned int MemoryStart;			//�����ڴ����ʼ��ַ
extern unsigned int InTable[0x10000];		//inָ��Ҫ���õ�ģ�⺯���ĵ�ַ��
extern unsigned int OutTable[0x10000];		//outָ��Ҫ���õ�ģ�⺯���ĵ�ַ��




int DispInit();														//��ʾ��ϵͳ��ʼ��
void ReadVideoRam(unsigned short addr);			//���Դ�Ĵ�����
void WriteVideoRam(unsigned short addr);		//д�Դ�Ĵ�����

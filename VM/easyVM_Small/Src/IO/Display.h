#pragma comment(lib,"gdiplus.lib")
#include <comdef.h>//初始化一下com口
#ifndef ULONG_PTR
#define ULONG_PTR unsigned long*
#include "GdiPlus.h"
using namespace Gdiplus;
#endif


#include "../cpu/cpu.h"


extern unsigned int MemoryStart;			//虚拟内存的起始地址
extern unsigned int InTable[0x10000];		//in指令要调用的模拟函数的地址表
extern unsigned int OutTable[0x10000];		//out指令要调用的模拟函数的地址表




int DispInit();														//显示子系统初始化
void ReadVideoRam(unsigned short addr);			//读显存的处理函数
void WriteVideoRam(unsigned short addr);		//写显存的处理函数

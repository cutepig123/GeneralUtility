#ifndef _easyVM_h_
#define _easyVM_h_


#include <stdio.h>
// 去掉下面的注释，则easyVM以调试状态运行
//#define _VM_DEBUG_


extern unsigned int MemoryStart;			//虚拟内存的起始地址
extern unsigned int MemorySize;				//虚拟内存的大小
extern unsigned int InTable[0x10000];		//in指令要调用的模拟函数的地址表
extern unsigned int OutTable[0x10000];		//out指令要调用的模拟函数的地址表
extern unsigned int InsTable[0x100];		//指令表
extern FILE *fDebug;						//调试信息输出

int VMInit();
void eVMerror(char *err);
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

#endif
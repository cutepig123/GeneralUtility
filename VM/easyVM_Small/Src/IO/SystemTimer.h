extern unsigned int IRR;
extern unsigned int OutTable[0x10000];		//out指令要调用的模拟函数的地址表


void SysTimerThread();			//这个函数给Display去SetTimer，不过这不是一个低耦合的方法。
void SystemTimerInit();
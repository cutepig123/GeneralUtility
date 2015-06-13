extern unsigned int InTable[0x10000];		//in指令要调用的模拟函数的地址表
extern unsigned int OutTable[0x10000];		//out指令要调用的模拟函数的地址表

extern unsigned int IRR;
extern unsigned int ISR;

char InterruptRequest();
void InterruptRespond(char i);
void PICInit();



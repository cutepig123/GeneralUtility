//导入
extern unsigned int FloppyIOPoint;		//软驱当前读写的数据的指针
extern unsigned int HardDiskIOPoint;
extern unsigned int MemoryStart;			//虚拟内存的起始地址
extern unsigned int InTable[0x10000];		//in指令要调用的模拟函数的地址表
extern unsigned int OutTable[0x10000];		//out指令要调用的模拟函数的地址表
extern unsigned int IRR;					//PIC的IRR寄存器，用于让设备指出中断请求
extern bool FloppyInserted[4];
extern char HdCnt[4][3];

//导出
void DMAInit();
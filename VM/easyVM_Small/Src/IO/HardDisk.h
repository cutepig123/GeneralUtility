//导入
extern unsigned int InTable[0x10000];		//in指令要调用的模拟函数的地址表
extern unsigned int OutTable[0x10000];		//out指令要调用的模拟函数的地址表
extern unsigned int MemoryStart;

//导出
extern unsigned int HardDiskIOPoint;		//当前读写的数据的指针，供DMA使用

void HardDiskInit();
void IO_Write_03F6(WORD data);	//参数都是通过栈传递的，所以要用WORD
void IO_Write_03F7(WORD data);

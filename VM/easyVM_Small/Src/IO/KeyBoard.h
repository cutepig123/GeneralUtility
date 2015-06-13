extern unsigned int MemoryStart;			//虚拟内存的起始地址
extern unsigned int InTable[0x10000];		//in指令要调用的模拟函数的地址表
extern unsigned int OutTable[0x10000];		//out指令要调用的模拟函数的地址表
extern unsigned int IRR;


void RecvKeyBoardMsg(UINT msg, WPARAM wParam, LPARAM lParam);
void RefreshKeyboardState();
void KeyBoardInit();

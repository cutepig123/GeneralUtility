extern unsigned int MemoryStart;			//�����ڴ����ʼ��ַ
extern unsigned int InTable[0x10000];		//inָ��Ҫ���õ�ģ�⺯���ĵ�ַ��
extern unsigned int OutTable[0x10000];		//outָ��Ҫ���õ�ģ�⺯���ĵ�ַ��
extern unsigned int IRR;


void RecvKeyBoardMsg(UINT msg, WPARAM wParam, LPARAM lParam);
void RefreshKeyboardState();
void KeyBoardInit();

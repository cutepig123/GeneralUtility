extern unsigned int InTable[0x10000];		//inָ��Ҫ���õ�ģ�⺯���ĵ�ַ��
extern unsigned int OutTable[0x10000];		//outָ��Ҫ���õ�ģ�⺯���ĵ�ַ��

extern unsigned int IRR;
extern unsigned int ISR;

char InterruptRequest();
void InterruptRespond(char i);
void PICInit();



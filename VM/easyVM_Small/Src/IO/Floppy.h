//����
extern unsigned int InTable[0x10000];		//inָ��Ҫ���õ�ģ�⺯���ĵ�ַ��
extern unsigned int OutTable[0x10000];		//outָ��Ҫ���õ�ģ�⺯���ĵ�ַ��
extern unsigned int MemoryStart;

//����
extern unsigned int FloppyIOPoint;		//��ǰ��д�����ݵ�ָ�룬��DMAʹ��
extern bool FloppyInserted[4];


void FloppyInit();
void IO_Write_03F0(WORD data);	//��������ͨ��ջ���ݵģ�����Ҫ��WORD
void IO_Write_03F1(WORD data);

//����
extern unsigned int InTable[0x10000];		//inָ��Ҫ���õ�ģ�⺯���ĵ�ַ��
extern unsigned int OutTable[0x10000];		//outָ��Ҫ���õ�ģ�⺯���ĵ�ַ��
extern unsigned int MemoryStart;

//����
extern unsigned int HardDiskIOPoint;		//��ǰ��д�����ݵ�ָ�룬��DMAʹ��

void HardDiskInit();
void IO_Write_03F6(WORD data);	//��������ͨ��ջ���ݵģ�����Ҫ��WORD
void IO_Write_03F7(WORD data);

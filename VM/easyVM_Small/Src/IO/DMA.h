//����
extern unsigned int FloppyIOPoint;		//������ǰ��д�����ݵ�ָ��
extern unsigned int HardDiskIOPoint;
extern unsigned int MemoryStart;			//�����ڴ����ʼ��ַ
extern unsigned int InTable[0x10000];		//inָ��Ҫ���õ�ģ�⺯���ĵ�ַ��
extern unsigned int OutTable[0x10000];		//outָ��Ҫ���õ�ģ�⺯���ĵ�ַ��
extern unsigned int IRR;					//PIC��IRR�Ĵ������������豸ָ���ж�����
extern bool FloppyInserted[4];
extern char HdCnt[4][3];

//����
void DMAInit();
#ifndef _easyVM_h_
#define _easyVM_h_


#include <stdio.h>
// ȥ�������ע�ͣ���easyVM�Ե���״̬����
//#define _VM_DEBUG_


extern unsigned int MemoryStart;			//�����ڴ����ʼ��ַ
extern unsigned int MemorySize;				//�����ڴ�Ĵ�С
extern unsigned int InTable[0x10000];		//inָ��Ҫ���õ�ģ�⺯���ĵ�ַ��
extern unsigned int OutTable[0x10000];		//outָ��Ҫ���õ�ģ�⺯���ĵ�ַ��
extern unsigned int InsTable[0x100];		//ָ���
extern FILE *fDebug;						//������Ϣ���

int VMInit();
void eVMerror(char *err);
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

#endif
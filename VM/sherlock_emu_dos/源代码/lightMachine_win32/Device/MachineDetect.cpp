#include "MachineDetect.h"
#include"../CPU/CentreProcessorUnit.h"
MachineDetective::MachineDetective(void)
{
	this->hardwareName ="MachineDetective";
}
MachineDetective::~MachineDetective(void)
{
}
void MachineDetective::init()
{
}
vector<int> MachineDetective::get_INT_shouldHandle()
{
	vector<int> INTarray;
	INTarray.push_back(0x11);//�豸���
	INTarray.push_back(0x12);//�ڴ��������
	INTarray.push_back(0x14);//����ͨѶ
	INTarray.push_back(0x17);//��ӡ��
	return  INTarray;
}
void MachineDetective::HandleINT(U1 INT_num)
{
	switch(INT_num)
	{
	case 0x11://�豸���
		pCPU->ax = 0x61;
		break;
	case 0x12://�ڴ��������
		pCPU->ax = 0x27f;
		break;
	case 0x14://����ͨѶ

		break;
	case 0x17://��ӡ��
		break;
	}
}
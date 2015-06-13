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
	INTarray.push_back(0x11);//设备检测
	INTarray.push_back(0x12);//内存容量检测
	INTarray.push_back(0x14);//串口通讯
	INTarray.push_back(0x17);//打印机
	return  INTarray;
}
void MachineDetective::HandleINT(U1 INT_num)
{
	switch(INT_num)
	{
	case 0x11://设备检测
		pCPU->ax = 0x61;
		break;
	case 0x12://内存容量检测
		pCPU->ax = 0x27f;
		break;
	case 0x14://串口通讯

		break;
	case 0x17://打印机
		break;
	}
}
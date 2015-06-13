#include "stdafx.h"
#include"Motherboard.h"
#include"Device.h"
void  MotherBoard::boot()
{
	this->virBios.loadExecuteFile();
}
U1 MotherBoard::getMemData_byte(const U4 addr)const 
{
	return *(U1*)(memory + addr);
}
U2 MotherBoard::getMemData_word(const U4 addr)const 
{
	return *(U2*)(memory + addr);
}
void MotherBoard::writeMem_byte(const U4 addr,const  U1 data)
{
	*(U1*)(memory + addr) = data;
}
void MotherBoard::writeMem_word(const U4 addr, const U2 data)
{
	*(U2*)(memory + addr) = data;
}

void MotherBoard::addDevic(Device* pdevice)
{
	pdevice->linkMotherBoard(this);
	vector<int> vec_INT = pdevice->get_INT_shouldHandle();
	for(int i=0; i < vec_INT.size(); i++)
	{
		this->deviceArray[vec_INT.at(i)] = pdevice;//按需要处理的中断号来存储device
	}

}
void MotherBoard::handleINT(U1 INT_num)
{
	this->deviceArray[INT_num]->HandleINT(INT_num);//让外设自己来处理中断
}
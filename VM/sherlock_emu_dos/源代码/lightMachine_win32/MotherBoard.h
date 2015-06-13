#ifndef _MOTHERBOARD_H
#define _MOTHERBOARD_H
#include "Hardware.h"
#include "common.h"
#include "CPU/CentreProcessorUnit.h"
#include "VirtualBIOS.h"
#include <vector>
using namespace std;
class Device;
class CentreProcessorUnit;
class MotherBoard:public Hardware
{
public:
	MotherBoard():virBios(this)
	{
		this->hardwareName ="MotherBoard";
		memory = new U1[0xffff0+0xffff+1];//使指令能确定访问这个内存0~~~0xffff0+0xffff
		if(NULL == memory)
		{
			this->buildLog(Hardware::ErrorType::MemoryCanNotApply, "Can not apply memory in Motherboard!");
			this->showMessage("Can not apply memory in Motherboard!");
			return;
		}
		this->buildLog(Hardware::ErrorType::Success, "Apply memory in Motherboard have success!");
		memset(memory, 0,sizeof(memory));
		memset(memory+0xffff0+0xffff,0xff,1);
		memset((void*)deviceArray, 0, sizeof(deviceArray));
	}
	~MotherBoard()
	{
		delete[] memory;
	};
	void boot();
	void registerCPU();
	
	U1* getMemStartAddr()
	{
		return memory;
	}
	void addDevic(Device* pdevice);
	void handleINT(U1 INT_num);
	U1 getMemData_byte(const U4 addr)const ;
	U2 getMemData_word(const U4 addr)const ;
	void writeMem_byte(const U4 addr,const  U1 data);
	void writeMem_word(const U4 addr, const U2 data);

public:
	CentreProcessorUnit *pCPU;
	Device* deviceArray[256];
private:
	
	VirtualBIOS virBios;
	
	U1* memory;
};


#endif
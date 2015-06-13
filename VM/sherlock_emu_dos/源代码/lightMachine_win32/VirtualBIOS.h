#ifndef _VIRTUALBIOS_H
#define _VIRTUALBIOS_H
#include"common.h"
#include "Hardware.h"
class MotherBoard;

class VirtualBIOS:public Hardware
{
public:
	VirtualBIOS(MotherBoard* pMB);
	void init();
	

	FileError loadExecuteFile();
private:
	MotherBoard* pMB;

};

#endif
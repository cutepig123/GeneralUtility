#ifndef _MACHINE_DETECT_H
#define _MACHINE_DETECT_H
#include "../device.h"
#include "../common.h"

class MachineDetective:public Device
{
public:
	MachineDetective(void);
	~MachineDetective(void);
	void init();
	vector<int> get_INT_shouldHandle();
	void HandleINT(U1 INT_num);
};

#endif
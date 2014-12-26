// Framework codes
#pragma once
#include "AlgthmBase.h"

class Flow
{
	class Impl;
	Impl *impl;
public:
	Flow();
	~Flow();
short AddModule(const char *name, int *pid, void *p=0);

short ConnectModule(int mod1, int pinid1, int mod2, int pinid2);

short run(std::vector<PinTypeBase*>    *pvInPins = 0, std::vector<PinTypeBase*>    *pvOutPins = 0);

short io(const char *file, bool isSave);
};

// Framework codes
#pragma once
#include "export.h"
#include "AlgthmBase.h"

class FLOWCHARTBASE_XXPORTS Flow
{
	class Impl;
	Impl *impl;
public:
	Flow();
	~Flow();
short AddModule(const char *name, int *pid, void *p=0);

short ConnectModule(int mod1, int pinid1, int mod2, int pinid2);

short run(std::vector<std::shared_ptr<PinTypeBase> >    *pvInPins = 0,
	std::vector<std::shared_ptr<PinTypeBase> >    *pvOutPins = 0,
	int startId = -1, int endId = -1);

short io(const char *file, bool isSave);
};

FLOWCHARTBASE_XXPORTS void Flow_Init();
FLOWCHARTBASE_XXPORTS void Flow_Free();
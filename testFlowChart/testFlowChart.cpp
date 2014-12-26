// testFlowChart.cpp : Defines the entry point for the console application.
//

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "stdafx.h"
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <assert.h>
#include <memory>
#include "AlgthmBase.h"
#include "BasicAlgthm.h"
#include "Flow.h"



std::map<std::string, std::vector<std::shared_ptr<PinTypeBase> > > g_PinTypes;
std::map<std::string, std::vector<std::shared_ptr<AlgthmBase>> > g_algs;

short RegisterPinType(std::shared_ptr<PinTypeBase> type)
{
	g_PinTypes[type->m_type].push_back( type);
	return 0;
}

short RegisterAlgthm(const char *name, std::shared_ptr<AlgthmBase> pAlg)
{
	pAlg->m_name =name;
	g_algs[name].push_back( pAlg);
	return 0;
}

void ResetPool()
{
	for (auto it = g_PinTypes.begin(); it != g_PinTypes.end(); ++it)
		it->second.resize(1);

	for (auto it = g_algs.begin(); it != g_algs.end(); ++it)
		it->second.resize(1);
}

PinTypeBase* CreatePinInst(const char *name)
{
	auto itv = g_PinTypes.find(name);
	assert(itv != g_PinTypes.end());
	/*for (auto it = itv->second.begin(); it != itv->second.end(); ++it)
	{
	if (it->unique())
	return *it;
	}
	return itv->second[0]->Create();*/
	std::shared_ptr<PinTypeBase> t(itv->second[0]->Create());
	itv->second.push_back(t);
	return t.get();
}

AlgthmBase* CrteateAlgInst(const char *name)
{
	auto itv = g_algs.find(name);
	assert(itv != g_algs.end());
	/*for (auto it = itv->second.begin(); it != itv->second.end(); ++it)
	{
	if (it->unique())
	return *it;
	}
	return itv->second[0]->Create();*/
	std::shared_ptr<AlgthmBase> t(itv->second[0]->Create());
	itv->second.push_back(t);
	return t.get();
}


class AlgthmMyFlow :public AlgthmCommImpl<AlgthmMyFlow>
{
public:
	Flow flow;

	AlgthmMyFlow()
	{
		int mid_add, mid_start, mid_end;

		//设置开始结束pin的数量和类型
		m_vInPins.resize(2);
		m_vInPins[0].m_type = "double";
		m_vInPins[1].m_type = "double";

		m_vOutPins.resize(1);
		m_vOutPins[0].m_type = "double";

		flow.AddModule(ALG_NAME_START, &mid_start, &m_vInPins);
		flow.AddModule(ALG_NAME_END, &mid_end, &m_vOutPins);

		flow.AddModule("Alg_Add", &mid_add);

		flow.ConnectModule(mid_start, 0, mid_add, 0);
		flow.ConnectModule(mid_start, 1, mid_add, 1);
		flow.ConnectModule(mid_add, 0, mid_end, 0);
	}

	short run(MyAlgEnv *flowx)
	{
		flow.run(&flowx->vInPin, &flowx->vOutPin);

		return 0;
	}


};


// User codes
void test()
{
	RegisterPinType( std::shared_ptr<PinTypeBase>(new PinDouble));
	RegisterAlgthm("Alg_Add", std::shared_ptr<AlgthmBase>(new AlgthmAdd));
	RegisterAlgthm("AlgthmInDbl", std::shared_ptr<AlgthmBase>(new AlgthmInDbl));
	RegisterAlgthm("AlgthmOutDbl", std::shared_ptr<AlgthmBase>(new AlgthmOutDbl));
	RegisterAlgthm(ALG_NAME_START, std::shared_ptr<AlgthmBase>(new AlgthmStartEnd));
	RegisterAlgthm(ALG_NAME_END, std::shared_ptr<AlgthmBase>(new AlgthmStartEnd));
	RegisterAlgthm("MyFlowAlg", std::shared_ptr<AlgthmBase>(new AlgthmMyFlow));

	if (0)
	{
		Flow flow;

		int mid_add, mid_start, mid_start2, mid_end;

		flow.AddModule("AlgthmOutDbl", &mid_start);
		flow.AddModule("Alg_Add", &mid_add);
		flow.AddModule("AlgthmInDbl", &mid_end);

		flow.ConnectModule(mid_start, 0, mid_add, 0);
		flow.ConnectModule(mid_start, 0, mid_add, 1);
		flow.ConnectModule(mid_add, 0, mid_end, 0);
		flow.run();

	}

	if (0)
	{
		Flow flow;

		int mid_add, mid_start, mid_end;

		//设置开始结束pin的数量和类型
		std::vector<AlgthmBase::Pin>    vInPins;
		vInPins.resize(2);
		vInPins[0].m_type = "double";
		vInPins[1].m_type = "double";

		std::vector<AlgthmBase::Pin>    vOutPins;
		vOutPins.resize(1);
		vOutPins[0].m_type = "double";

		flow.AddModule(ALG_NAME_START, &mid_start, &vInPins);
		flow.AddModule(ALG_NAME_END, &mid_end, &vOutPins);

		flow.AddModule("Alg_Add", &mid_add);
		
		flow.ConnectModule(mid_start, 0, mid_add, 0);
		flow.ConnectModule(mid_start, 1, mid_add, 1);
		flow.ConnectModule(mid_add, 0, mid_end, 0);

		PinDouble aPinIn[2];
		aPinIn[0].m_d = 1;
		aPinIn[1].m_d = 2;
		std::vector<PinTypeBase*> vPinIn(2);
		vPinIn[0] = &aPinIn[0];
		vPinIn[1] = &aPinIn[1];

		PinDouble aPinOut[2];
		std::vector<PinTypeBase*> vPinOut(1);
		vPinOut[0] = &aPinOut[0];

		flow.run(&vPinIn, &vPinOut);

		printf("Output %f\n", aPinOut[0].m_d);

	}

	if (1)
	{
		Flow flow;
		int mid_add, mid_start, mid_end;
		flow.AddModule("AlgthmOutDbl", &mid_start);
		flow.AddModule("MyFlowAlg", &mid_add);
		flow.AddModule("AlgthmInDbl", &mid_end);

		flow.ConnectModule(mid_start, 0, mid_add, 0);
		flow.ConnectModule(mid_start, 0, mid_add, 1);
		flow.ConnectModule(mid_add, 0, mid_end, 0);
		flow.run();

		flow.io("filename", true);
	}

	{
		Flow flow;

		flow.io("filename", false);

		flow.run();
	}

	ResetPool();
}

int _tmain(int argc, _TCHAR* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

	new int;

	test();
	return 0;
}



// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "AlgthmAlign.h"
#include "AlgthmBasic.h"
#include <FlowChartBase/Flow.h>

static std::shared_ptr<system_initializer> g_sys;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


void My_Flow_Init()
{
	RegisterPinType(std::shared_ptr<PinTypeBase>(new PinAlignRec));
	RegisterPinType(std::shared_ptr<PinTypeBase>(new PinString));
	RegisterPinType(std::shared_ptr<PinTypeBase>(new PinRPoint));

	RegisterAlgthm("ALG_NAME_START", std::shared_ptr<AlgthmBase>(new AlgthmStartEnd));
	RegisterAlgthm("ALG_NAME_END", std::shared_ptr<AlgthmBase>(new AlgthmStartEnd));
	RegisterAlgthm("AlgthmAlignLearn", std::shared_ptr<AlgthmBase>(new AlgthmAlignLearn));
	RegisterAlgthm("AlgthmAlignSrch", std::shared_ptr<AlgthmBase>(new AlgthmAlignSrch));

	RegisterPinType(std::shared_ptr<PinTypeBase>(new PinDouble));

	RegisterAlgthm("Alg_Add", std::shared_ptr<AlgthmBase>(new AlgthmAdd));
	RegisterAlgthm("AlgthmInDbl", std::shared_ptr<AlgthmBase>(new AlgthmInDbl));
	RegisterAlgthm("AlgthmOutDbl", std::shared_ptr<AlgthmBase>(new AlgthmOutDbl));
	RegisterAlgthm("ALG_NAME_START", std::shared_ptr<AlgthmBase>(new AlgthmStartEnd));
	RegisterAlgthm("ALG_NAME_END", std::shared_ptr<AlgthmBase>(new AlgthmStartEnd));
	
	g_sys.reset(new system_initializer);
}


void My_Flow_Free()
{
	g_sys.reset();
}

void Flow_Test_align();
void Flow_Test_Basic();

void My_Flow_Test()
{
	Flow_Test_Basic(); 
	Flow_Test_align();
}

struct Flow_Ftns g_Ftns = {
	My_Flow_Init,
	My_Flow_Free,
	My_Flow_Test
};

/*extern "C" __declspec(dllexport)*/  Flow_Ftns* /*__stdcall*/ Flow_Main()
{
	return &g_Ftns;
}


// testFlowChart.cpp : Defines the entry point for the console application.
//

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "stdafx.h"
#include <windows.h>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <assert.h>
#include <memory>
#include <FlowChartBase/AlgthmBase.h>
#include <FlowChartBase/Flow.h>

#define	DEPANDS_ALIGN 0

#if DEPANDS_ALIGN
#include <FlowChart_Alg/AlgthmAlign.h>
#endif



int _tmain(int argc, _TCHAR* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

	char currDir[300];
	GetCurrentDirectory(300, currDir);
	printf("Current dir %s\n", currDir);

	Flow_Init();
	
	new int;

	Flow_Free();

	return 0;
}



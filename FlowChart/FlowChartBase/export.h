#ifdef FLOWCHARTBASE_EXPORTS
#define FLOWCHARTBASE_XXPORTS __declspec(dllexport) 
#else
#define FLOWCHARTBASE_XXPORTS __declspec(dllimport) 

	#pragma comment(lib, "FlowChartBase.lib")
	
#endif



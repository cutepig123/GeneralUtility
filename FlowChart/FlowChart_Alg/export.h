#ifdef FLOWCHART_ALG_EXPORTS
	#define FLOWCHART_ALG_XXPORTS __declspec(dllexport) 
#else
	#define FLOWCHART_ALG_XXPORTS __declspec(dllimport) 

	#pragma comment(lib, "FlowChart_Alg.lib")
	
#endif



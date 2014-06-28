#include "stdafx.h"
#include "windows.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>

#include <string>
#include <algorithm>

#define	MY_VERIFY(x)	if(!(x)) _tprintf(_T("Fail %s\n"), #x);

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
	char ** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
	{
		return *itr;
	}
	return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
	return std::find(begin, end, option) != end;
}

void help()
{
	printf(" -list				List all processes\n");
	printf(" -kill	<name>		Kill process by name\n");
	printf(" -suspres <name>	wait and suspend process ny name\n");
}

short GetProcessName(DWORD processID, TCHAR szProcessName[MAX_PATH]);
short MyKillProcess(DWORD processID);
short MySuspendProcess(DWORD processId);
short MyResumeProcess(DWORD processId);

// return num of matched processes
enum MyOp
{
	MyOp_Suspend,
	MyOp_Kill,
	MyOp_Resume,
	MyOp_List,
};

int TestProcess(MyOp op, char const *pname)
{
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	MY_VERIFY(EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded));

	cProcesses = cbNeeded / sizeof(DWORD);

	int nKill = 0;
	for (i = 0; i < cProcesses; i++)
	{
		TCHAR szProcessName[MAX_PATH];
		GetProcessName(aProcesses[i], szProcessName);

		if (op == MyOp_List)
			_tprintf(_T("%x:\t%s\n"), aProcesses[i], szProcessName);
		else
		if (_stricmp(szProcessName, pname) == 0)
		{
			if (op == MyOp_Suspend)
				MySuspendProcess(aProcesses[i]);
			if (op == MyOp_Kill)
				MyKillProcess(aProcesses[i]);
			if (op == MyOp_Resume)
				MyResumeProcess(aProcesses[i]);
			nKill++;
		}
	}
	
	return nKill;
}

int main(int argc, char *argv[])
{
	if (argc <= 1)
		help();

	if (cmdOptionExists(argv, argv + argc, "-list"))
	{
		TestProcess(MyOp_List, NULL);
	}
	
	char *killpname = getCmdOption(argv, argv + argc, "-kill");
	if (killpname)
	{
		int nKill =TestProcess(MyOp_Kill, killpname);
		printf("%d processes are killed\n", nKill);
	}

	char *susppname = getCmdOption(argv, argv + argc, "-suspres");
	if (susppname)
	{
		printf("Waiting process start to suspend...\n");

		int n = 0;
		while ((n =TestProcess(MyOp_Suspend, susppname))==0)
		{
			Sleep(100);
		}

		printf("%d processes suspended, pess any key to resume...\n",n);
		system("pause");

		n = TestProcess(MyOp_Resume, susppname);
		printf("%d processes resumed\n",n);
	}

	return 0;
}
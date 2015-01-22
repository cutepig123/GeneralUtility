// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "stdio.h"
#include "assert.h"
#include "AlgthmBase.h"
#include "Flow.h"

static HMODULE g_hModule = 0;

void FreePool();

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
	g_hModule = hModule;
	return TRUE;
}

typedef Flow_Ftns*  (/*__stdcall*/ *Flow_Main)();

static std::vector<Flow_Ftns>	g_vFlowFtns;

void LoadPlugin(const char *file)
{
	HMODULE hMod = LoadLibrary(file);
	assert(hMod);
	Flow_Main pProc = (Flow_Main) GetProcAddress(hMod, "Flow_Main");
	assert(pProc);
	Flow_Ftns* pFtn = (pProc)();
	assert(pFtn);
	g_vFlowFtns.push_back(*pFtn);
}

short ListPluginFiles(void (*pFtn)(const char *file))
{
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH];
	TCHAR szDirSrchFilt[MAX_PATH];
	TCHAR szFile[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	GetModuleFileNameA(g_hModule, szDir, MAX_PATH);
	char *p = strrchr(szDir, '\\');
	if (p)
		*p = 0;

	sprintf(szDirSrchFilt, "%s\\FlowChart_*.dll", szDir);

	// Find the first file in the directory.

	hFind = FindFirstFile(szDirSrchFilt, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		return dwError;
	}

	// List all the files in the directory with some info about them.

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//_tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
		}
		else
		{
			//filesize.LowPart = ffd.nFileSizeLow;
			//filesize.HighPart = ffd.nFileSizeHigh;
			//_tprintf(TEXT("  %s   %ld bytes\n"), ffd.cFileName, filesize.QuadPart);
			sprintf(szFile, "%s\\%s", szDir, ffd.cFileName);
			pFtn(szFile);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
	}

	FindClose(hFind);
	return dwError;
}

void Flow_InitPlugins()
{
	for (auto it = g_vFlowFtns.begin(); it != g_vFlowFtns.end(); ++it)
	{
		it->Flow_Init();
	}
}

void Flow_FreePlugins()
{
	for (auto it = g_vFlowFtns.begin(); it != g_vFlowFtns.end(); ++it)
	{
		it->Flow_Free();
	}
	g_vFlowFtns.clear();
}

void Flow_TestPlugins()
{
	for (auto it = g_vFlowFtns.begin(); it != g_vFlowFtns.end(); ++it)
	{
		it->Flow_Test();
	}
}

void Flow_LoadPlugins()
{
	ListPluginFiles(LoadPlugin);
	Flow_InitPlugins();
	Flow_TestPlugins();
}


void Flow_Init()
{
	Flow_LoadPlugins();
}

void Flow_Free()
{
	FreePool();
	Flow_FreePlugins();
}

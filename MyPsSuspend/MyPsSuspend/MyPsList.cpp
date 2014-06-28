// MyPsSuspend.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>

// http://msdn.microsoft.com/en-us/library/windows/desktop/ms682623%28v=vs.85%29.aspx
// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

short GetProcessName(DWORD processID, TCHAR szProcessName[MAX_PATH])
{
	_sntprintf(szProcessName, MAX_PATH, TEXT("<unknown>"));
	
	// Get a handle to the process.

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);

	// Get the process name.

	if (NULL != hProcess)
	{
		HMODULE hMod;
		DWORD cbNeeded;

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
			&cbNeeded))
		{
			GetModuleBaseName(hProcess, hMod, szProcessName, MAX_PATH);
		}
	}

	// Release the handle to the process.
	CloseHandle(hProcess);
	return 0;
}

#define	MY_VERIFY(x)	if(!(x)) _tprintf(_T("Fail %s\n"), #x);

short MyKillProcess(DWORD processID)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE, processID);

	MY_VERIFY(TerminateProcess(hProcess, 1));

	// Release the handle to the process.
	CloseHandle(hProcess);
	return 0;
}

// http://stackoverflow.com/questions/11010165/how-to-suspend-resume-a-process-in-windows
typedef LONG(NTAPI *NtSuspendProcess)(IN HANDLE ProcessHandle);

short MySuspendProcess(DWORD processId)
{
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

	NtSuspendProcess pfnNtSuspendProcess = (NtSuspendProcess)GetProcAddress(
		GetModuleHandle(_T("ntdll")), ("NtSuspendProcess"));
	MY_VERIFY(pfnNtSuspendProcess);

	pfnNtSuspendProcess(processHandle);
	CloseHandle(processHandle);

	return 0;
}

short MyResumeProcess(DWORD processId)
{
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

	NtSuspendProcess pfnNtResumeProcess = (NtSuspendProcess)GetProcAddress(
		GetModuleHandle(_T("ntdll")), ("NtResumeProcess"));
	MY_VERIFY(pfnNtResumeProcess);

	pfnNtResumeProcess(processHandle);
	CloseHandle(processHandle);

	return 0;
}

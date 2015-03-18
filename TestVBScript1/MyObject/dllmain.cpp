// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "MyObject_i.h"
#include "dllmain.h"

CMyObjectModule _AtlModule;

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	return _AtlModule.DllMain(dwReason, lpReserved); 
}


int CMyObjectModule::SayHi(void)
{
	::MessageBox(NULL, _T("Inside SayHi()"), _T(""), MB_SETFOREGROUND);
	return 0;
}

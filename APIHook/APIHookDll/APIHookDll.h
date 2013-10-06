// APIHookDll.h : main header file for the APIHookDll DLL
//

#pragma once

//#ifndef __AFXWIN_H__
//	#error "include 'stdafx.h' before including this file for PCH"
//#endif

#include "resource.h"		// main symbols


// CAPIHookDllApp
// See APIHookDll.cpp for the implementation of this class
//

//class CAPIHookDllApp : public CWinApp
//{
//public:
//	CAPIHookDllApp();
//
//// Overrides
//public:
//	virtual BOOL InitInstance();
//
//	DECLARE_MESSAGE_MAP()
//};

void __declspec(dllexport) WINAPI InstallHook(BOOL,DWORD); 
BOOL WINAPI UnInstallHook(); 
LRESULT CALLBACK GetMsgProC(int code,WPARAM wParam,LPARAM lParam); 
void WINAPI HookOneAPI(LPCTSTR pszCalleeModuleName,PROC pfnOriginApiAddress, 
        PROC pfnDummyFuncAddress,HMODULE hModCallerModule); 
BOOL WINAPI HookAllAPI(LPCTSTR pszCalleeModuleName,PROC pfnOriginApiAddress, 
        PROC pfnDummyFuncAddress,HMODULE hModCallerModule); 
BOOL WINAPI UnhookAllAPIHooks(LPCTSTR pszCalleeModuleName,PROC pfnOriginApiAddress, 
         PROC pfnDummyFuncAddress,HMODULE hModCallerModule); 
BOOL WINAPI H_TextOutA(HDC, int, int, LPCSTR, int); 
BOOL WINAPI H_TextOutW(HDC, int, int, LPCWSTR, int); 
BOOL WINAPI H_ExtTextOutA(HDC, int, int, UINT, CONST RECT *,LPCSTR, UINT, CONST INT *); 
BOOL WINAPI H_ExtTextOutW(HDC, int, int, UINT, CONST RECT *,LPCWSTR, UINT, CONST INT *); 

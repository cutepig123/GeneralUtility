
// stdafx.cpp : source file that includes just the standard includes
// MFCApplication1.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


CAppModule _Module;
HINSTANCE g_hInst = 0;

HINSTANCE	AfxFindResourceHandle(LPCTSTR lpszName, LPCTSTR lpszType)
{
	return g_hInst;
}


HICON CWinApp::LoadIcon(LPCTSTR lpszResourceName) const
{
	return ::LoadIcon(AfxFindResourceHandle(lpszResourceName,
		ATL_RT_GROUP_ICON), lpszResourceName);
}

 CWinApp::CWinApp()
{
	g_pstApp = this;
}

void  CWinApp::Init()
{
	//g_pstApp = this;
	InitInstance();
}


CWinApp * CWinApp::g_pstApp = 0;

CWinApp *AfxGetApp()
{
	return CWinApp::g_pstApp;
}
 

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int /*nCmdShow*/)
{
	g_hInst = hInstance;
	HRESULT hRes = ::CoInitialize(NULL);
	// If you are running on NT 4.0 or higher you can use the following call instead to 
	// make the EXE free threaded. This means that calls come in on a random RPC thread.
	//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = 0;
	// BLOCK: Run application
	AfxGetApp()->Init();

	_Module.Term();
	::CoUninitialize();

	return nRet;
}


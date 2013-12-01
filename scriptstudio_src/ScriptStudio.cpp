// ScriptStudio.cpp : main source file for ScriptStudio.exe
//

#include "stdafx.h"

#include "resource.h"

#include "MainFrm.h"


CAppModule _Module;


int Run(LPTSTR /*lpstrCmdLine = NULL*/, int /*nCmdShow*/)
{
   CMessageLoop theLoop;
   _Module.AddMessageLoop(&theLoop);

   CMainFrame wndMain;
   if( wndMain.CreateEx() == NULL ) {
      ATLTRACE(_T("Main window creation failed!\n"));
      return 0;
   }
   wndMain.ShowWindow(SW_MAXIMIZE);

   int nRet = theLoop.Run();

   _Module.RemoveMessageLoop();
   return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
   HRESULT hRes = ::CoInitialize(NULL);
   ATLASSERT(SUCCEEDED(hRes));

   ::DefWindowProc(NULL, 0, 0, 0L);

   AtlInitCommonControls(ICC_WIN95_CLASSES | ICC_COOL_CLASSES | ICC_BAR_CLASSES);

   hRes = _Module.Init(NULL, hInstance);
   ATLASSERT(SUCCEEDED(hRes));

   AtlAxWinInit();

   int nRet = Run(lpstrCmdLine, nCmdShow);

   _Module.Term();
   ::CoUninitialize();

   return nRet;
}

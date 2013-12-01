// BasScript.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "BasScript.h"
#include "ScriptEngine.h"


BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason,  LPVOID lpReserved)
{
   switch( dwReason ) {
   case DLL_PROCESS_ATTACH:
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
   case DLL_PROCESS_DETACH:
      ::DisableThreadLibraryCalls(hModule);
      break;
   }
   return TRUE;
}



BASSCRIPT_API CBaseScript* CreateScriptEngine(void)
{
   return new CScriptEngine();
}

BASSCRIPT_API void DestroyScriptEngine(CBaseScript* pEngine)
{
   if( pEngine != NULL ) delete pEngine;
}


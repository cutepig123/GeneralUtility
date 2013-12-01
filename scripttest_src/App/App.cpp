// App.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include "../BasScript/BasScript.h"

#ifdef _DEBUG
   #pragma comment(lib, "../BasScript/Debug/BasScript.lib")
#else
   #pragma comment(lib, "../BasScript/Release/BasScript.lib")
#endif

#include <comdef.h>


class Globals : public IScriptCallback
{
public:
   bool GetProperty(LPCWSTR pstrName, VARIANT* pvRet)
   {
      if( wcscmp(pstrName, L"now") == 0 ) {
         SYSTEMTIME stNow;
         ::GetLocalTime(&stNow);
         WCHAR szDate[64] = { 0 };
         ::GetDateFormatW(LOCALE_USER_DEFAULT, 0, &stNow, NULL, szDate, 63);
         _variant_t v = szDate;
         *pvRet = v.Detach();
         return true;
      }
      return false;
   }
   bool SetProperty(LPCWSTR pstrName, VARIANT vRet)
   {
      return false;
   }
   bool Method(LPCWSTR pstrName, VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
   {
      if( wcscmp(pstrName, L"tick") == 0 ) {
         _variant_t v = (long) ::GetTickCount();
         *pvRet = v.Detach();
         return true;
      }
      if( wcscmp(pstrName, L"print") == 0 ) {
         _variant_t v = pvArgs[0];
         v.ChangeType(VT_BSTR);
         ::OutputDebugStringW(v.bstrVal);
         return true;
      }
      return false;
   }
};

class App : public IScriptCallback
{
public:
   long a;
   bool GetProperty(LPCWSTR pstrName, VARIANT* pvValue)
   {
      if( wcscmp(pstrName, L"a") == 0 ) {
         _variant_t v = a;
         *pvValue = v.Detach();
         return true;
      }
      return false;
   }
   bool SetProperty(LPCWSTR pstrName, VARIANT vValue)
   {
      if( wcscmp(pstrName, L"a") == 0 ) {
         _variant_t v = vValue;
         v.ChangeType(VT_I4);
         a = v.lVal;
         return true;
      }
      return false;
   }
   bool Method(LPCWSTR pstrName, VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
   {
      if( wcscmp(pstrName, L"foo") == 0 ) {
         _variant_t v = a;
         v.ChangeType(VT_BSTR);
         *pvRet = v.Detach();
         return true;
      }
      return false;
   }
};

void GetAppFolder(char *acFilePath)
{
	GetModuleFileName(GetModuleHandle(NULL), acFilePath, MAX_PATH);
	char *p = strrchr(acFilePath, '\\');
	p[1] = 0;
}
int main(int argc, char* argv[])
{
   ATL::CAtlFile f;
   char code[1024 * 10] = { 0 };
   char acFilePath[MAX_PATH];

   GetAppFolder(acFilePath);
   strcat(acFilePath, "test.scr");
   f.Create(acFilePath, GENERIC_READ, 0, OPEN_EXISTING);
   f.Read(code, sizeof(code));
   f.Close();

   _variant_t val;
   App app; 
   Globals globals; 
   CBaseScript* pScript = CreateScriptEngine();
   pScript->RegisterGlobals(&globals);
   pScript->RegisterObject("App", &app);
   bool bRes = pScript->RunScript(code);
   bRes = pScript->Evaluate("return xyz", &val);
   DestroyScriptEngine(pScript);
	return 0;
}


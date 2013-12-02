
#include "stdafx.h"

#include "ScriptEngine.h"

#include <comdef.h>


bool CScriptEngine::GetProperty(LPCWSTR pstrName, VARIANT* pvValue)
{
   return false;
}

bool CScriptEngine::SetProperty(LPCWSTR pstrName, VARIANT vValue)
{
   return false;
}

bool CScriptEngine::Method(LPCWSTR pstrName, VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
{
   if( pstrName[0] == 'a' ) {
      if( wcscmp(pstrName, L"atoi") == 0 ) return __func_atoi(pvArgs, nArgs, pvRet);
   }
   if( pstrName[0] == 'e' ) {
      if( wcscmp(pstrName, L"eval") == 0 ) return __func_eval(pvArgs, nArgs, pvRet);
      if( wcscmp(pstrName, L"exit") == 0 ) return __func_exit(pvArgs, nArgs, pvRet);
   }
   if( pstrName[0] == 'f' ) {
      if( wcscmp(pstrName, L"fopen") == 0 ) return __func_fopen(pvArgs, nArgs, pvRet);
      if( wcscmp(pstrName, L"fclose") == 0 ) return __func_fclose(pvArgs, nArgs, pvRet);
      if( wcscmp(pstrName, L"fgets") == 0 ) return __func_fgets(pvArgs, nArgs, pvRet);
      if( wcscmp(pstrName, L"fputs") == 0 ) return __func_fputs(pvArgs, nArgs, pvRet);
      if( wcscmp(pstrName, L"feof") == 0 ) return __func_feof(pvArgs, nArgs, pvRet);
   }
   if( pstrName[0] == 's' ) {
      if( wcscmp(pstrName, L"strlen") == 0 ) return __func_strlen(pvArgs, nArgs, pvRet);
      if( wcscmp(pstrName, L"strstr") == 0 ) return __func_strstr(pvArgs, nArgs, pvRet);
      if( wcscmp(pstrName, L"strpos") == 0 ) return __func_strpos(pvArgs, nArgs, pvRet);
      if( wcscmp(pstrName, L"substr") == 0 ) return __func_substr(pvArgs, nArgs, pvRet);
      if( wcscmp(pstrName, L"strupper") == 0 ) return __func_strupper(pvArgs, nArgs, pvRet);
      if( wcscmp(pstrName, L"strlower") == 0 ) return __func_strlower(pvArgs, nArgs, pvRet);
      if( wcscmp(pstrName, L"sleep") == 0 ) return __func_sleep(pvArgs, nArgs, pvRet);
   }
   return false;
}


///////////////////////////////////////////////////////////////////////////////////////


bool CScriptEngine::__func_strlen(VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
{
   if( nArgs != 1 ) throw EXCEP_ARGUMENTS;
   _variant_t arg = pvArgs[0];
   arg.ChangeType(VT_BSTR);
   _variant_t res = (long) ::lstrlenW(arg.bstrVal);
   *pvRet = res.Detach();
   return true;
}

bool CScriptEngine::__func_strupper(VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
{
   if( nArgs != 1 ) throw EXCEP_ARGUMENTS;
   _variant_t arg = pvArgs[0];
   arg.ChangeType(VT_BSTR);
   int cch = ::SysStringLen(arg.bstrVal);
   if( ::CharUpperBuffW(arg.bstrVal, cch) == 0 ) ::CharUpperBuffA((char*)arg.bstrVal, cch * 2);
   *pvRet = arg.Detach();
   return true;
}

bool CScriptEngine::__func_strlower(VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
{
   if( nArgs != 1 ) throw EXCEP_ARGUMENTS;
   _variant_t arg = pvArgs[0];
   arg.ChangeType(VT_BSTR);
   int cch = ::SysStringLen(arg.bstrVal);
   if( ::CharLowerBuffW(arg.bstrVal, cch) == 0 ) ::CharLowerBuffA((char*)arg.bstrVal, cch * 2);
   *pvRet = arg.Detach();
   return true;
}

bool CScriptEngine::__func_atoi(VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
{
   if( nArgs != 1 ) throw EXCEP_ARGUMENTS;
   if( FAILED(::VariantChangeType(pvRet, &pvArgs[0], 0, VT_I4)) ) *pvRet = _variant_t(0L).Detach();   
   return true;
}

bool CScriptEngine::__func_substr(VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
{
   if( nArgs < 2 ) throw EXCEP_ARGUMENTS;
   if( pvArgs[0].vt != VT_BSTR || pvArgs[0].bstrVal == NULL ) throw EXCEP_ARGUMENTS;
   BSTR bstr = pvArgs[0].bstrVal;
   long cch = (long) ::SysStringLen(bstr);
   _variant_t start = pvArgs[1];
   _variant_t len = nArgs == 3 ? pvArgs[2] : _variant_t(cch);
   start.ChangeType(VT_I4);
   len.ChangeType(VT_I4);
   if( start.lVal >= cch ) return *pvRet = _variant_t(L"").Detach(), true;
   if( start.lVal + len.lVal > cch ) len.lVal -= start.lVal + len.lVal - cch;   
   BSTR bstrRes = ::SysAllocStringLen(bstr + start.lVal, len.lVal);
   _variant_t res = bstrRes;
   *pvRet = res.Detach();
   ::SysFreeString(bstrRes);
   return true;
}

bool CScriptEngine::__func_strstr(VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
{
   if( nArgs != 2 ) throw EXCEP_ARGUMENTS;
   if( pvArgs[0].vt != VT_BSTR || pvArgs[0].bstrVal == NULL ) throw EXCEP_ARGUMENTS;
   if( pvArgs[1].vt != VT_BSTR || pvArgs[1].bstrVal == NULL ) throw EXCEP_ARGUMENTS;
   LPCWSTR pwstr = wcsstr(pvArgs[0].bstrVal, pvArgs[1].bstrVal);
   if( pwstr == NULL ) *pvRet = _variant_t(L"").Detach();
   else *pvRet = _variant_t(pwstr).Detach();
   return true;
}

bool CScriptEngine::__func_strpos(VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
{
   if( nArgs != 2 ) throw EXCEP_ARGUMENTS;
   if( pvArgs[0].vt != VT_BSTR || pvArgs[0].bstrVal == NULL ) throw EXCEP_ARGUMENTS;
   if( pvArgs[1].vt != VT_BSTR || pvArgs[1].bstrVal == NULL ) throw EXCEP_ARGUMENTS;
   LPCWSTR pwstr = wcsstr(pvArgs[0].bstrVal, pvArgs[1].bstrVal);
   if( pwstr == NULL ) *pvRet = _variant_t(-1L).Detach();
   else *pvRet = _variant_t((long)(pwstr - pvArgs[0].bstrVal)).Detach();
   return true;
}

bool CScriptEngine::__func_fopen(VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
{
   if( nArgs != 2 ) throw EXCEP_ARGUMENTS;
   if( pvArgs[0].vt != VT_BSTR || pvArgs[0].bstrVal == NULL ) throw EXCEP_ARGUMENTS;
   if( pvArgs[1].vt != VT_BSTR || pvArgs[1].bstrVal == NULL ) throw EXCEP_ARGUMENTS;
   char szFilename[MAX_PATH + 1] = { 0 };
   ::WideCharToMultiByte(::GetACP(), 0, pvArgs[0].bstrVal, -1, szFilename, MAX_PATH, NULL, NULL);
   DWORD dwAccess = GENERIC_READ;
   DWORD dwShare = FILE_SHARE_READ;
   DWORD dwCreate = OPEN_EXISTING;
   if( wcsstr(pvArgs[1].bstrVal, L"w") != NULL ) {
      dwAccess = GENERIC_WRITE;
      dwShare = 0;
      dwCreate = CREATE_ALWAYS;
   }
   if( wcsstr(pvArgs[1].bstrVal, L"a") != NULL ) {
      dwAccess = GENERIC_WRITE;
      dwShare = 0;
      dwCreate = OPEN_ALWAYS;
   }
   HANDLE hFile = ::CreateFile(szFilename, dwAccess, dwShare, NULL, dwCreate, FILE_ATTRIBUTE_NORMAL, NULL);
   if( hFile != INVALID_HANDLE_VALUE && dwCreate == OPEN_ALWAYS ) ::SetFilePointer(hFile, 0, NULL, FILE_END);
   *pvRet = _variant_t((long)hFile).Detach();
   return true;
}

bool CScriptEngine::__func_fclose(VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
{
   if( nArgs != 1 ) throw EXCEP_ARGUMENTS;
   if( pvArgs[0].vt != VT_I4 ) throw EXCEP_ARGUMENTS;
   HANDLE hFile = (HANDLE) pvArgs[0].lVal;
   if( hFile == INVALID_HANDLE_VALUE ) return true;
   ::CloseHandle(hFile);
   return true;
}

bool CScriptEngine::__func_fgets(VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
{
   if( nArgs != 1 ) throw EXCEP_ARGUMENTS;
   if( pvArgs[0].vt != VT_I4 ) throw EXCEP_ARGUMENTS;
   HANDLE hFile = (HANDLE) pvArgs[0].lVal;
   if( hFile == INVALID_HANDLE_VALUE ) return true;
   char szBuffer[400] = { 0 };
   int nRead = 0;
   for( ; ; ) {
      char c = '\0';
      DWORD dwRead = 0;
      ::ReadFile(hFile, &c, 1, &dwRead, NULL);
      if( dwRead == 0 ) break;
      if( c == '\r' ) continue;
      if( c == '\n' ) break;
      szBuffer[nRead] = c;
      if( ++nRead >= sizeof(szBuffer) - 1 ) break;
   }
   *pvRet = _variant_t(szBuffer).Detach();
   return true;
}

bool CScriptEngine::__func_fputs(VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
{
   if( nArgs != 2 ) throw EXCEP_ARGUMENTS;
   if( pvArgs[0].vt != VT_I4 ) throw EXCEP_ARGUMENTS;
   if( pvArgs[1].vt != VT_BSTR || pvArgs[1].bstrVal == NULL ) throw EXCEP_ARGUMENTS;
   HANDLE hFile = (HANDLE) pvArgs[0].lVal;
   char szBuffer[405] = { 0 };
   ::WideCharToMultiByte(::GetACP(), 0, pvArgs[1].bstrVal, -1, szBuffer, sizeof(szBuffer) - 3, NULL, NULL);
   ::lstrcatA(szBuffer, "\r\n");
   DWORD dwWritten = 0;
   ::WriteFile(hFile, szBuffer, ::lstrlen(szBuffer), &dwWritten, NULL);
   *pvRet = _variant_t(dwWritten > 0).Detach();
   return true;
}

bool CScriptEngine::__func_feof(VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
{
   if( nArgs != 1 ) throw EXCEP_ARGUMENTS;
   if( pvArgs[0].vt != VT_I4 ) throw EXCEP_ARGUMENTS;
   HANDLE hFile = (HANDLE) pvArgs[0].lVal;   
   if( hFile == INVALID_HANDLE_VALUE ) *pvRet = _variant_t(true).Detach();
   else *pvRet = _variant_t(::GetFileSize(hFile, NULL) == ::SetFilePointer(hFile, 0, NULL, FILE_CURRENT)).Detach();
   return true;
}

bool CScriptEngine::__func_sleep(VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
{
   if( nArgs != 1 ) throw EXCEP_ARGUMENTS;
   _variant_t arg = pvArgs[0];
   arg.ChangeType(VT_I4);
   ::Sleep(arg.lVal);
   return true;
}

bool CScriptEngine::__func_eval(VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
{
   if( nArgs != 1 ) throw EXCEP_ARGUMENTS;
   if( pvArgs[0].vt != VT_BSTR || pvArgs[0].bstrVal == NULL ) throw EXCEP_ARGUMENTS;
   int iStack = __pushstack();
   tok.Assign(pvArgs[0].bstrVal, 1);
   t = tok.GetToken();
   __eval(pvRet);
   __popstack(iStack);
   return true;
}

bool CScriptEngine::__func_exit(VARIANT* pvArgs, int nArgs, VARIANT* pvRet)
{
   throw EXCEP_NOP;
   return true;
}


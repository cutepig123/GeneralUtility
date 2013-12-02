
#include "stdafx.h"

#include "ScriptEngine.h"


CScriptEngine::CScriptEngine() : 
   pstrStore(NULL), 
   pstrScript(NULL), 
   t(OP_END), 
   m_nVars(0), 
   m_nObjects(0),
   m_nStackLevel(0)
{
   ::CoInitialize(NULL);
   RegisterGlobals(this);
   Clear();
   Cancel();
}

CScriptEngine::~CScriptEngine()
{
   Clear();
   ::CoUninitialize();
}

void CScriptEngine::Clear()
{
   int i;
   for( i = 0; i < m_nVars; i++ ) ::VariantClear(&m_vars[i].vValue);
   for( i = 0; i < m_nStackLevel; i++ ) ::VariantClear(&m_Stack[i].vRet);
   m_nVars = 0;
   m_nFunctions = 0;
   m_nStackLevel = 0;
   m_bCancelled = false;
   ::ZeroMemory(m_szErrorMsg, sizeof(m_szErrorMsg));
   ::ZeroMemory(m_szErrorCode, sizeof(m_szErrorCode));
   ::ZeroMemory(m_szErrorLine, sizeof(m_szErrorLine));
   if( pstrStore != NULL ) free(pstrStore);
   pstrStore = NULL;
   pstrScript = NULL;
}

void CScriptEngine::Cancel()
{
   m_bCancelled = true;
}

bool CScriptEngine::RunScriptFromFile(LPCSTR pstrFilename)
{
   HANDLE hFile = ::CreateFile(pstrFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
   if( hFile == INVALID_HANDLE_VALUE ) return false;
   DWORD dwSize = ::GetFileSize(hFile, NULL);
   LPSTR pstrCode = (LPSTR) malloc(dwSize + 1);
   DWORD dwRead = 0;
   ::ReadFile(hFile, pstrCode, dwSize, &dwRead, NULL);
   ::CloseHandle(hFile);
   if( dwRead != dwSize ) {
      free(pstrCode);
      return false;
   }
   pstrCode[dwSize] = '\0';
   bool bRes = RunScript(pstrCode);
   free(pstrCode);
   return bRes;
}

bool CScriptEngine::RunScript(LPCSTR pstrCode)
{
   Clear();
   int cchLen = strlen(pstrCode) + 1;
   pstrStore = pstrScript = (LPWSTR) malloc((sizeof(WCHAR) * cchLen));
   if( pstrStore == NULL ) return false;
   pstrStore[0] = '\0';
   ::MultiByteToWideChar(::GetACP(), 0, pstrCode, cchLen, pstrStore, cchLen);
   __run(pstrStore);
   return strlen(m_szErrorMsg) == 0;
}

bool CScriptEngine::Evaluate(LPCSTR pstrCode, VARIANT* pRet)
{ 
   ::ZeroMemory(m_szErrorMsg, sizeof(m_szErrorMsg));
   int cchLen = strlen(pstrCode) + 1;
   LPWSTR pwstr = pstrScript = (LPWSTR) malloc((sizeof(WCHAR) * cchLen));
   if( pwstr == NULL ) return false;
   pwstr[0] = '\0';
   ::MultiByteToWideChar(::GetACP(), 0, pstrCode, cchLen, pwstr, cchLen);
   int iStack = __pushstack();
   __run(pwstr);
   ::VariantCopy(pRet, &m_Stack[iStack - 1].vRet);
   __popstack(iStack);
   free(pwstr);
   return strlen(m_szErrorMsg) == 0;
}

bool CScriptEngine::RegisterObject(LPCSTR pstrName, IScriptCallback* pCallback)
{
   if( m_nObjects >= MAX_OBJECTS ) return false;
   CALLBACKDEF& obj = m_Objects[m_nObjects];
   ::ZeroMemory(&obj, sizeof(obj));
   ::MultiByteToWideChar(::GetACP(), 0, pstrName, -1, obj.szName, (sizeof(obj.szName) / sizeof(WCHAR)) -1);
   obj.pCallback = pCallback;
   m_nObjects++;
   return true;
}

bool CScriptEngine::RegisterGlobals(IScriptCallback* pCallback)
{
   return RegisterObject("", pCallback);
}

void CScriptEngine::GetErrorMessage(int& iLineNum, LPSTR pstrMessage, int cchMessage, LPSTR pstrCode, int cchCode, LPSTR pstrLine, int cchLine)
{
   iLineNum = m_iErrorLineNum;
   if( pstrMessage != NULL ) strncpy(pstrMessage, m_szErrorMsg, cchMessage);
   if( pstrCode != NULL ) strncpy(pstrCode, m_szErrorCode, cchCode);
   if( pstrLine != NULL ) strncpy(pstrLine, m_szErrorLine, cchLine);
}

void CScriptEngine::_InterpretError(EXCEP e)
{
   struct {
      EXCEP e;
      LPCTSTR pstrMessage;
   } Errors[] = 
   {
      { EXCEP_SYNTAXERROR,   "Syntax error" },
      { EXCEP_TYPEMISMATCH,  "Type mismatch" },
      { EXCEP_NAMEEXPECTED,  "Name exptected" },
      { EXCEP_CODEOVERFLOW,  "Compiler limitation reached" },
      { EXCEP_PARAMOVERFLOW, "Too many parameters" },
      { EXCEP_STACKOVERFLOW, "Stack overflow" },
      { EXCEP_SCOPE,         "Invalid scope" },
      { EXCEP_NOTFOUND,      "Undefined" },
      { EXCEP_KEYWORD,       "Keyword expected" },
      { EXCEP_DUPLICATE,     "Duplicated entry" },
      { EXCEP_PAREN,         "Parens mixup" },
      { EXCEP_ARGUMENTS,     "Bad arguments" },
      { EXCEP_ERROR,         "Error" },
      { EXCEP_BREAK,         "Unscoped break statement" },
      { EXCEP_RETURN,        "Unscoped return statement" },
      { EXCEP_CANCELLED,     "Cancelled" }
   };
   for( int i = 0; i < sizeof(Errors)/sizeof(Errors[0]); i++ ) {
      if( Errors[i].e == e ) strcpy(m_szErrorMsg, Errors[i].pstrMessage);
   }
   m_iErrorLineNum = tok.m_iLineNo;
   LPCWSTR p = tok.m_pstr - wcslen(t.value);
   if( p < pstrScript ) p = pstrScript;
   ::WideCharToMultiByte(::GetACP(), 0, p, -1, m_szErrorCode, sizeof(m_szErrorCode), NULL, NULL);
   while( p > pstrScript && *(p - 1) != '\n' ) p--;
   LPCWSTR pLen = p;
   while( *pLen != '\0' && *pLen != '\n' && *pLen != '\r' ) pLen++;
   ::WideCharToMultiByte(::GetACP(), 0, p, -1, m_szErrorLine, min(pLen - p, sizeof(m_szErrorLine) - 1), NULL, NULL);
}


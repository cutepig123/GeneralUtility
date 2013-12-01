#if !defined(AFX_SCRIPTENGINE_H__20050805_F673_570F_6CD8_0080AD509054__INCLUDED_)
#define AFX_SCRIPTENGINE_H__20050805_F673_570F_6CD8_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BasScript.h"
#include "Tokenizer.h"


class CScriptEngine : public CBaseScript, public IScriptCallback
{
public:
   CScriptEngine();
   ~CScriptEngine();

   void Clear();
   void Cancel();
   bool RunScript(LPCSTR pstrCode);
   bool RunScriptFromFile(LPCSTR pstrFilename);

   bool Evaluate(LPCSTR pstrCode, VARIANT* pRet);

   bool RegisterObject(LPCSTR pstrName, IScriptCallback* pCallback);
   bool RegisterGlobals(IScriptCallback* pCallback);

   void GetErrorMessage(int& iLineNum, 
      LPSTR pstrMessage, int cchMessage, 
      LPSTR pstrCode, int cchCode, 
      LPSTR pstrLine, int cchLine);

   // IScriptCallback

   bool GetProperty(LPCWSTR pstrName, VARIANT* pvValue);
   bool SetProperty(LPCWSTR pstrName, VARIANT vValue);
   bool Method(LPCWSTR pstrName, VARIANT* pvArgs, int nArgs, VARIANT* pvRet);

private:
   enum { MAX_VARIABLES = 100 };
   enum { MAX_FUNCTIONS = 50 };
   enum { MAX_OBJECTS = 20 };
   enum { MAX_STACKDEPTH = 20 };
   enum { MAX_ARGUMENTS = 12 };

   typedef enum EXCEP
   {
      EXCEP_NOP = 0,
      EXCEP_SYNTAXERROR,
      EXCEP_TYPEMISMATCH,
      EXCEP_NAMEEXPECTED,
      EXCEP_CODEOVERFLOW,
      EXCEP_PARAMOVERFLOW,
      EXCEP_STACKOVERFLOW,
      EXCEP_SCOPE,
      EXCEP_NOTFOUND,
      EXCEP_KEYWORD,
      EXCEP_DUPLICATE,
      EXCEP_ERROR,
      EXCEP_PAREN,
      EXCEP_ARGUMENTS,
      EXCEP_BREAK,
      EXCEP_RETURN,
      EXCEP_CANCELLED,
   };

private:
   void _InterpretError(EXCEP e);

   void __level6(VARIANT* pVar);
   void __level5(VARIANT* pVar);
   void __level4(VARIANT* pVar);
   void __level3(VARIANT* pVar);
   void __level2(VARIANT* pVar);
   void __level1(VARIANT* pVar);
   void __level0(VARIANT* pVar);
   void __eval(VARIANT* pVar);

   void __forloop();
   void __whileloop();
   void __ifcondit();
   void __switchcondit();
   void __break();
   void __return();

   void __func_define();
   void __var_define();
   void __var_define(LPCWSTR pstrName, VARIANT& vValue);
   void __var_assign();
   void __var_retrieve(VARIANT* pVar);
   void __var_getargs(VARIANT* pArgs, int& nArgs);
   void __skipblock();
   void __popstack(int iPos);
   int __pushstack();
   
   void __block();
   void __statement();
   void __exec(OPTYPE Stop);
   void __run(LPCWSTR pstrCode);

   bool __func_strlen(VARIANT* pvArgs, int nArgs, VARIANT* pvRet);
   bool __func_strstr(VARIANT* pvArgs, int nArgs, VARIANT* pvRet);
   bool __func_strpos(VARIANT* pvArgs, int nArgs, VARIANT* pvRet);
   bool __func_strupper(VARIANT* pvArgs, int nArgs, VARIANT* pvRet);
   bool __func_strlower(VARIANT* pvArgs, int nArgs, VARIANT* pvRet);
   bool __func_atoi(VARIANT* pvArgs, int nArgs, VARIANT* pvRet);
   bool __func_substr(VARIANT* pvArgs, int nArgs, VARIANT* pvRet);
   bool __func_fopen(VARIANT* pvArgs, int nArgs, VARIANT* pvRet);
   bool __func_fclose(VARIANT* pvArgs, int nArgs, VARIANT* pvRet);
   bool __func_fgets(VARIANT* pvArgs, int nArgs, VARIANT* pvRet);
   bool __func_fputs(VARIANT* pvArgs, int nArgs, VARIANT* pvRet);
   bool __func_feof(VARIANT* pvArgs, int nArgs, VARIANT* pvRet);
   bool __func_sleep(VARIANT* pvArgs, int nArgs, VARIANT* pvRet);
   bool __func_eval(VARIANT* pvArgs, int nArgs, VARIANT* pvRet);
   bool __func_exit(VARIANT* pvArgs, int nArgs, VARIANT* pvRet);

private:
   LPWSTR pstrScript;
   LPWSTR pstrStore;
   CTokenizer tok;
   TOKEN t;
   char m_szErrorMsg[64];
   char m_szErrorCode[40];
   char m_szErrorLine[100];
   int m_iErrorLineNum;

   typedef struct VARIABLE
   {
      WCHAR szName[MAX_KEYWORD_LEN];
      VARIANT vValue;
   };
   VARIABLE m_vars[MAX_VARIABLES];
   int m_nVars;

   typedef struct CALLBACKDEF
   {
      WCHAR szName[MAX_KEYWORD_LEN];
      IScriptCallback* pCallback;
   };
   CALLBACKDEF m_Objects[MAX_OBJECTS];
   int m_nObjects;

   typedef struct FUNCTIONDEF
   {
      WCHAR szName[MAX_KEYWORD_LEN];
      LPCWSTR pstrCode;
      int iLineNo;
   };
   FUNCTIONDEF m_Functions[MAX_FUNCTIONS];
   int m_nFunctions;

   typedef struct STACKLEVEL
   {
      LPCWSTR pstrCode;
      int iLineNo;
      int iVarIndex;
      VARIANT vRet;
      OPTYPE t;
   };
   STACKLEVEL m_Stack[MAX_STACKDEPTH];
   int m_nStackLevel;

   volatile bool m_bCancelled;
};


#endif // !defined(AFX_SCRIPTENGINE_H__20050805_F673_570F_6CD8_0080AD509054__INCLUDED_)

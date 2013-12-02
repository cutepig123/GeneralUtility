#ifndef __BASSCRIPT_H__
#define __BASSCRIPT_H__


#ifdef BASSCRIPT_EXPORTS
   #define BASSCRIPT_API __declspec(dllexport)
#else
   #define BASSCRIPT_API __declspec(dllimport)
#endif


class IScriptCallback
{
public:
   virtual bool GetProperty(LPCWSTR pstrName, VARIANT* pvValue) = 0;
   virtual bool SetProperty(LPCWSTR pstrName, VARIANT vValue) = 0;
   virtual bool Method(LPCWSTR pstrName, VARIANT* pvArgs, int nArgs, VARIANT* pvRet) = 0;
};


class CBaseScript 
{
public:
   virtual void Clear() = 0;
   virtual void Cancel() = 0;
   virtual bool RunScript(LPCSTR pstrCode) = 0;
   virtual bool RunScriptFromFile(LPCSTR pstrFilename) = 0;

   virtual bool Evaluate(LPCSTR pstrCode, VARIANT* pRet) = 0;

   virtual bool RegisterObject(LPCSTR pstrName, IScriptCallback* pCallback) = 0;
   virtual bool RegisterGlobals(IScriptCallback* pCallback) = 0;

   virtual void GetErrorMessage(int& iLineNum, 
      LPSTR pstrMessage, int cchMessage, 
      LPSTR pstrCode, int cchCode, 
      LPSTR pstrLine, int cchLine) = 0;
};


BASSCRIPT_API CBaseScript* CreateScriptEngine(void);
BASSCRIPT_API void DestroyScriptEngine(CBaseScript*);


#endif  // __BASSCRIPT_H__

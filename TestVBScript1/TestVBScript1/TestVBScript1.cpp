// TestVBScript1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "stdafx.h"
#include <atlbase.h>
#include <activscp.h>

//http://stackoverflow.com/questions/7491868/how-to-load-call-a-vbscript-function-from-within-c
//http://support.microsoft.com/kb/223139/en-us
//https://kobyk.wordpress.com/2007/09/13/a-lightweight-approach-for-exposing-c-objects-to-a-hosted-active-scripting-engine/

#define CHECKHR(stmt) \
    { \
        HRESULT hr = S_OK; \
        if (FAILED(hr = (stmt))) { return hr; } \
    }

class CSimpleScriptSite :
    public IActiveScriptSite,
    public IActiveScriptSiteWindow
{
public:
    CSimpleScriptSite() : m_cRefCount(1), m_hWnd(NULL) { }

    // IUnknown

    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();
    STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject);

    // IActiveScriptSite

    STDMETHOD(GetLCID)(LCID *plcid){ *plcid = 0; return S_OK; }
    STDMETHOD(GetItemInfo)(LPCOLESTR pstrName, DWORD dwReturnMask, IUnknown **ppunkItem, ITypeInfo **ppti) 
	{ 
	  HRESULT hr =S_OK;

      // Is it expecting an ITypeInfo?
      if(ppti) {
         // Default to NULL.
         *ppti = NULL;
         
         // Return if asking about ITypeInfo... 
         if(dwReturnMask & SCRIPTINFO_ITYPEINFO)
		 {
			// Check for our object name...
            if (!_wcsicmp(L"myobject", pstrName)) {
               // Provide our object.
               *ppti = m_pType;
               // Addref our object...
               //m_pUnkScriptObject->AddRef();
            }
			//return TYPE_E_ELEMENTNOTFOUND;
		 }
          
      }
      
      // Is the engine passing an IUnknown buffer?
      if(ppunkItem) {
         // Default to NULL.
         *ppunkItem = NULL;
         
         // Is Script Engine looking for an IUnknown for our object?
         if(dwReturnMask & SCRIPTINFO_IUNKNOWN) {
            // Check for our object name...
            if (!_wcsicmp(L"myobject", pstrName)) {
               // Provide our object.
               *ppunkItem = m_pMyobj;
               // Addref our object...
               //m_pUnkScriptObject->AddRef();
            }
         }
      }
      
      return S_OK;
	}
    STDMETHOD(GetDocVersionString)(BSTR *pbstrVersion) { *pbstrVersion = SysAllocString(L"1.0"); return S_OK; }
    STDMETHOD(OnScriptTerminate)(const VARIANT *pvarResult, const EXCEPINFO *pexcepinfo) { return S_OK; }
    STDMETHOD(OnStateChange)(SCRIPTSTATE ssScriptState) { return S_OK; }
    STDMETHOD(OnScriptError)(IActiveScriptError *pIActiveScriptError) { 
		static BSTR pwcErrorText;
		 pIActiveScriptError->GetSourceLineText(&pwcErrorText);

      //assert(0);
      ::SysFreeString(pwcErrorText);
		return S_OK; 
	}
    STDMETHOD(OnEnterScript)(void) { return S_OK; }
    STDMETHOD(OnLeaveScript)(void) { return S_OK; }

    // IActiveScriptSiteWindow

    STDMETHOD(GetWindow)(HWND *phWnd) { *phWnd = m_hWnd; return S_OK; }
    STDMETHOD(EnableModeless)(BOOL fEnable) { return S_OK; }

    // Miscellaneous

    HRESULT SetWindow(HWND hWnd) 
	{ 
		m_hWnd = hWnd; 
	}

	ITypeInfo*	m_pType;
	IUnknown* m_pMyobj;
public:
    //CComPtr<IActiveScript> m_spIActiveScript;
    LONG                   m_cRefCount;
    HWND                   m_hWnd;
};

STDMETHODIMP_(ULONG) CSimpleScriptSite::AddRef()
{
    return InterlockedIncrement(&m_cRefCount);
}

STDMETHODIMP_(ULONG) CSimpleScriptSite::Release()
{
    if (!InterlockedDecrement(&m_cRefCount))
    {
       // delete this;
        return 0;
    }
    return m_cRefCount;
}

STDMETHODIMP CSimpleScriptSite::QueryInterface(REFIID riid, void **ppvObject)
{
    if (riid == IID_IUnknown || riid == IID_IActiveScriptSiteWindow)
    {
        *ppvObject = (IActiveScriptSiteWindow *) this;
        AddRef();
        return NOERROR;
    }
    if (riid == IID_IActiveScriptSite)
    {
        *ppvObject = (IActiveScriptSite *) this;
        AddRef();
        return NOERROR;
    }
    return E_NOINTERFACE;
}

class MyObject
{
public:
	virtual void __stdcall f(int i){
		printf("f\n");
	}
	virtual BOOL __stdcall g(float f){
		printf("g\n");
		return 0;
	}
static PARAMDATA f_paramData;
static PARAMDATA g_paramData;
static METHODDATA methodData[];
static INTERFACEDATA interfaceData;
};

PARAMDATA MyObject::f_paramData = {
OLESTR("i"), VT_I4
};
PARAMDATA MyObject::g_paramData = {
OLESTR("f"), VT_R4
};
METHODDATA MyObject::methodData[] = {
{ OLESTR("f"), &MyObject::f_paramData, 1, 0, CC_STDCALL, 1, DISPATCH_METHOD, VT_EMPTY },
{ OLESTR("g"), &MyObject::g_paramData, 2, 1, CC_STDCALL, 1, DISPATCH_METHOD, VT_BOOL }
};
INTERFACEDATA MyObject::interfaceData = {
MyObject::methodData,
sizeof(MyObject::methodData) / sizeof(METHODDATA)
};

int _tmain(int argc, _TCHAR* argv[])
{
    HRESULT hr = S_OK;
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

    // Initialize
    CSimpleScriptSite* pScriptSite = new CSimpleScriptSite();
    CComPtr<IActiveScript> spJScript;
    CComPtr<IActiveScriptParse> spJScriptParse;
    hr = spJScript.CoCreateInstance(OLESTR("JScript"));
    hr = spJScript->SetScriptSite(pScriptSite);
    hr = spJScript->QueryInterface(&spJScriptParse);
    hr = spJScriptParse->InitNew();
    CComPtr<IActiveScript> spVBScript;
    CComPtr<IActiveScriptParse> spVBScriptParse;
    hr = spVBScript.CoCreateInstance(OLESTR("VBScript"));
    hr = spVBScript->SetScriptSite(pScriptSite);
    hr = spVBScript->QueryInterface(&spVBScriptParse);
    hr = spVBScriptParse->InitNew();

    // Run some scripts
    CComVariant result;
    EXCEPINFO ei = { };
    hr = spJScriptParse->ParseScriptText(OLESTR("(new Date()).getTime()"), NULL, NULL, NULL, 0, 0, SCRIPTTEXT_ISEXPRESSION, &result, &ei);
    hr = spVBScriptParse->ParseScriptText(OLESTR("Now"), NULL, NULL, NULL, 0, 0, SCRIPTTEXT_ISEXPRESSION, &result, &ei);
    hr = spVBScriptParse->ParseScriptText(OLESTR("MsgBox \"Hello World! The current time is: \" & Now"), NULL, NULL, NULL, 0, 0, 0, &result, &ei);

	MyObject	myobj;
	CComPtr<ITypeInfo> pMyobjTypeInfo;
	hr = CreateDispTypeInfo(
		&MyObject::interfaceData,
		LOCALE_SYSTEM_DEFAULT,
		&pMyobjTypeInfo);
	CComPtr<IUnknown> pMyobj;
	hr = CreateStdDispatch(NULL, &myobj, pMyobjTypeInfo, &pMyobj);

	pScriptSite->m_pMyobj =pMyobj;
	pScriptSite->m_pType =pMyobjTypeInfo;

	hr = spVBScript->AddNamedItem(
		L"myobject",
		SCRIPTITEM_ISSOURCE | SCRIPTITEM_ISVISIBLE | SCRIPTITEM_ISPERSISTENT);
	hr = spVBScriptParse->ParseScriptText(OLESTR("myobject.g(1)"), NULL, NULL, NULL, 0, 0, SCRIPTTEXT_ISEXPRESSION, &result, &ei);

    // Cleanup
    spVBScriptParse = NULL;
    spVBScript = NULL;
    spJScriptParse = NULL;
    spJScript = NULL;
    pScriptSite->Release();
    pScriptSite = NULL;

    ::CoUninitialize();
    return 0;
}

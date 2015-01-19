// TestVBScript1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "stdafx.h"
#include <atlbase.h>
#include <activscp.h>

//http://stackoverflow.com/questions/7491868/how-to-load-call-a-vbscript-function-from-within-c

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
    STDMETHOD(GetItemInfo)(LPCOLESTR pstrName, DWORD dwReturnMask, IUnknown **ppiunkItem, ITypeInfo **ppti) 
	{ 
		return TYPE_E_ELEMENTNOTFOUND; 
	}
    STDMETHOD(GetDocVersionString)(BSTR *pbstrVersion) { *pbstrVersion = SysAllocString(L"1.0"); return S_OK; }
    STDMETHOD(OnScriptTerminate)(const VARIANT *pvarResult, const EXCEPINFO *pexcepinfo) { return S_OK; }
    STDMETHOD(OnStateChange)(SCRIPTSTATE ssScriptState) { return S_OK; }
    STDMETHOD(OnScriptError)(IActiveScriptError *pIActiveScriptError) { return S_OK; }
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
        delete this;
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

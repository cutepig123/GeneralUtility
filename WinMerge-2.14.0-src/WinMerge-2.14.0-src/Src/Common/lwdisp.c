/* File:	lwdisp.c - light weight dispatch API
 * Author:	Jochen Tucht 2003/01/09
 *			Copyright (C) 2003 herbert dahm datensysteme GmbH
 *
 * Purpose:	- Create windows scripting objects (scriptlets)
 *			- Invoke methods and access properties
 *			- Implement callback interfaces to be invoked by scriptlets
 *
 * Remarks:	requires Win32
 *			link with oleaut32, shlwapi 4.71
 *
 *			Features not supported by this API include:
 *			- type libraries
 *			- named arguments
 *
 * License:	THIS FILE CONTAINS FREE SOURCE CODE. IT IS PROVIDED *AS IS*, WITHOUT
 *			WARRANTY OF ANY KIND. YOU MAY USE IT AT YOUR OWN RISK, AS LONG AS
 *			YOU KEEP IT IN A SEPARATE FILE AND PRESERVE THIS COMMENT.
 *			CHANGES MUST BE RECORDED IN THE MODIFICATION HISTORY BELOW SO THERE
 *			IS EVIDENCE THAT THE FILE DIFFERS FROM EARLIER RELEASES. THE LEVEL
 *			OF DETAIL IS UP TO YOU. YOU MAY SET THE BY: ENTRY TO "NOBODY@ALL"
 *			IF YOU DON'T WANT TO EXPOSE YOUR NAME. SUBSEQUENT CHANGES MAY BE
 *			REFLECTED BY A SINGLE RECORD CARRYING THE DATE OF THE LATEST CHANGE.
 *

DATE:		BY:					DESCRIPTION:
==========	==================	================================================
2003/01/14	J.Tucht				provide a way to subclass the default LWDispVtbl
2003/03/16	J.Tucht				ensure BSTR arguments are writeable
2003/05/31	J.Tucht				registration of SCT and OCX no longer required
2003/08/05	J.Tucht				change some names for use with MFC
2003/08/31	J.Tucht				avoid wnsprintfW to get away with shlwapi < 5.0
2003/10/05	J.Tucht				allow calls from other threads through HWND
2003/11/04	J.Tucht				more explicit error messages, SEH
2003/11/06	NOBODY@ALL			incredible number of changes for unknown reasons
2003/11/18	Laoran				CreateDispatchBySource : avoid crash if loading dll fails
2003/11/18	Laoran				CreateDispatchBySource, cosmetic : move dll load&object creation after the CLSID search (= less indentations)
2004/01/08  Perry               Updated function comment preceding ReportError
2008/01/22  Kimmo               Changed map argument name to disp_map to not confuse VC6
*/
// ID line follows -- this is updated by SVN
// $Id: lwdisp.c 4938 2008-01-22 13:54:38Z kimmov $

//#define _WIN32_IE		0x0300
//#define _WIN32_WINNT	0x0400	

#define NONAMELESSUNION		// avoid warning C4201
#define CINTERFACE			// tell gcc this is "C"

struct IShellView;			// avoid MSC warning C4115
struct _RPC_ASYNC_STATE;	// avoid MSC warning C4115

#include <shlobj.h>
#include <shlwapi.h>
#include <tchar.h>
#include "lwdisp.h"
#include "dllproxy.h"

/**
* @brief Display or return error message string (from
number)
*
* @param style: if 0, return sysalloc'd string, else
msgbox
*
* Calls FormatMessage to get description of system string.
* If not found, makes a string containing raw number.
* If msgbox (style!=0), then function returns 0.
* If not msgbox (style==0), caller must LocalFree string.
*/
static LPTSTR NTAPI ReportError(HRESULT sc, UINT style)
{
	LPTCH pc = 0;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, sc,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
		(LPTCH)&pc, 0, NULL
	);
	if (pc == 0)
	{
		FormatMessage
		(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_STRING |
			FORMAT_MESSAGE_ARGUMENT_ARRAY,
			"Error 0x%1!lX!", 0,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
			(LPTCH)&pc, 0, (va_list *)&sc
		);
	}
	if (style)
	{
		MessageBox(0, pc, 0, style);
		LocalFree(pc);
		pc = 0;
	}
	return pc;
}

/**
 * @brief build a formatted message string 
 */
static LPTSTR FormatMessageFromString(LPCTSTR format, ...)
{
	LPTCH pc = 0;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_STRING |
		FORMAT_MESSAGE_ARGUMENT_ARRAY,
		format, 0,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
		(LPTCH)&pc, 0, (va_list *)(&format + 1)
	);
	return pc;
}

static void mycpyt2w(LPCTSTR tsz, wchar_t * wdest, int limit)
{
#ifdef _UNICODE
	wcsncpy(wdest, tsz, limit);
#else
	MultiByteToWideChar(CP_ACP, 0, tsz, -1, wdest, limit);
#endif
	// always terminate the string
	wdest[limit-1] = 0;
}

static void mycpyt2a(LPCTSTR tsz, char * adest, int limit)
{
#ifdef _UNICODE
	WideCharToMultiByte(CP_ACP, 0, tsz, -1, adest, limit, 0, 0);
#else
	strncpy(adest, tsz, limit);
#endif
	// always terminate the string
	adest[limit-1] = 0;
}

/**
 * 
 * @Note We can use this code with unregistered COM DLL
 * For VC++ DLL, we need a custom CComTypeInfoHolder as the default one search the registry
 * For VB DLL, instance can not be shared accross thread, one must be created for each thread
 *
 * Don't catch unknown errors in this function, because we want to catch
 * both C++ and C errors, and this is a C file.
 */
LPDISPATCH NTAPI CreateDispatchBySource(LPCTSTR source, LPCWSTR progid)
{
	void *pv = 0;
	SCODE sc;
	WCHAR wc[320];
	if (source == 0)
	{
		CLSID clsid;
		if SUCCEEDED(sc=CLSIDFromProgID(progid, &clsid))
		{
			sc=CoCreateInstance(&clsid, 0, CLSCTX_ALL, &IID_IDispatch, &pv);
		}
	}
	else if (PathMatchSpec(source, _T("*.ocx")) || PathMatchSpec(source, _T("*.dll")))
	{
		CLSID objectGUID = {0};
		BOOL bGUIDFound = FALSE;

		// search in the interface of the dll for the CLSID of progid
		ITypeLib *piTypeLib;
		mycpyt2w(source, wc, DIMOF(wc));
		if SUCCEEDED(sc=LoadTypeLib(wc, &piTypeLib))
		{
			UINT count = piTypeLib->lpVtbl->GetTypeInfoCount(piTypeLib);
			while (SUCCEEDED(sc) && !bGUIDFound && count--)
			{
				ITypeInfo *piTypeInfo;
				if SUCCEEDED(sc=piTypeLib->lpVtbl->GetTypeInfo(piTypeLib, count, &piTypeInfo))
				{
					TYPEATTR *pTypeAttr;
					if SUCCEEDED(sc=piTypeInfo->lpVtbl->GetTypeAttr(piTypeInfo, &pTypeAttr))
					{
						BSTR bstrName = 0;
						if SUCCEEDED(sc=piTypeInfo->lpVtbl->GetDocumentation(piTypeInfo, MEMBERID_NIL, &bstrName, 0, 0, 0))
						{
							if (pTypeAttr->typekind == TKIND_COCLASS && StrCmpIW(bstrName, progid) == 0)
							{
								memcpy(&objectGUID, &pTypeAttr->guid, sizeof(CLSID));
								bGUIDFound = TRUE;
							}
							SysFreeString(bstrName);
						}
						piTypeInfo->lpVtbl->ReleaseTypeAttr(piTypeInfo, pTypeAttr);
					}
					piTypeInfo->lpVtbl->Release(piTypeInfo);
				}
			}
			piTypeLib->lpVtbl->Release(piTypeLib);
		}
	
		if (bGUIDFound)
		{
			// we have found the CLSID, so this is really a COM dll for WinMerge
			// now try to load the dll and to create an instance of the object
			IClassFactory *piClassFactory;
			EXPORT_DLLPROXY
			(
				Dll, "",
				HRESULT(NTAPI*DllGetClassObject)(REFCLSID,REFIID,IClassFactory**);
			);
			mycpyt2a(source, Dll.SIG+strlen(Dll.SIG), sizeof(Dll.SIG)-strlen(Dll.SIG));
			if ((FARPROC)DLLPROXY(Dll)->DllGetClassObject != DllProxy_ModuleState.Unresolved)
				if SUCCEEDED(sc=DLLPROXY(Dll)->DllGetClassObject(&objectGUID, &IID_IClassFactory, &piClassFactory))
				{
					sc=piClassFactory->lpVtbl->CreateInstance(piClassFactory, 0, &IID_IDispatch, &pv);
				}
		}
		// don't display an error message if no interface (normal dll)
		if (PathMatchSpec(source, _T("*.dll")) && sc == TYPE_E_CANTLOADLIBRARY)
			sc = 0;
		// don't display an error message if the format is too old
		if (sc == TYPE_E_UNSUPFORMAT)
			sc = 0;
	}
	else 
	{
		BIND_OPTS bind_opts;
		// initialize to official defaults:
		bind_opts.cbStruct = sizeof bind_opts;
		bind_opts.grfFlags = 0;
		bind_opts.grfMode = STGM_READWRITE;
		bind_opts.dwTickCountDeadline = 0;
		// prepend appropriate moniker:
		if (PathIsContentType(source, _T("text/scriptlet")))
			mycpyt2w(_T("script:"), wc, DIMOF(wc));
		else
			mycpyt2w(_T(""), wc, DIMOF(wc));
		mycpyt2w(source, wc+wcslen(wc), DIMOF(wc)-wcslen(wc));

		// I observed that CoGetObject() may internally provoke an access
		// violation and succeed anyway. No idea how to avoid this.
		sc=CoGetObject(wc, &bind_opts, &IID_IDispatch, &pv);
		if (sc == E_NOINTERFACE)
		{
			// give it a second try after opening within associated application:
			SHELLEXECUTEINFO sein;
			sein.cbSize = sizeof sein;
			sein.hwnd = 0;
			// SEE_MASK_FLAG_DDEWAIT: wait until application is ready to listen
			sein.fMask = SEE_MASK_FLAG_DDEWAIT;
			sein.lpVerb = _T("open");
			sein.lpFile = source;
			sein.lpParameters = 0;
			sein.lpDirectory = _T(".");
			sein.nShow = SW_SHOWNORMAL;
			if (ShellExecuteEx(&sein))
			{
				sc=CoGetObject(wc, &bind_opts, &IID_IDispatch, &pv);
			}
		}
		// no error if the interface does not exist
		if (sc == MK_E_INTERMEDIATEINTERFACENOTSUPPORTED || sc == E_UNEXPECTED)
			sc = 0;
	}
	if FAILED(sc)
	{
		// get the error description
		LPTSTR errorText = ReportError(sc, 0);
		if (source)
		{
			// append the source name
			LPTSTR tmp;
			tmp = FormatMessageFromString(_T("%1\n%2"), errorText, source);
			LocalFree(errorText);
			errorText = tmp;
		}
		// report error
		MessageBox(0, errorText, 0, MB_ICONSTOP|MB_TASKMODAL);
		LocalFree(errorText);
		// no valid dispatch
		pv = 0;
	}
	return (LPDISPATCH)pv;
}

STDAPI invokeV(LPDISPATCH pi, VARIANT *ret, DISPID id, LPCCH op, VARIANT *argv)
{
	HRESULT sc = E_FAIL;
	DISPID idNamed = DISPID_PROPERTYPUT;
	WORD wFlags = HIBYTE((UINT_PTR)op);
	DISPPARAMS dispparams;
	UINT nArgErr = (UINT)-1;
	EXCEPINFO excepInfo = {0};
	dispparams.cArgs = LOBYTE((UINT_PTR)op);
	dispparams.cNamedArgs = 0;
	if (wFlags & (DISPATCH_PROPERTYPUT|DISPATCH_PROPERTYPUTREF))
	{
		dispparams.cNamedArgs = 1;
		dispparams.rgdispidNamedArgs = &idNamed;
	}
	dispparams.rgvarg = argv;
	if (pi)
	{
		sc = pi->lpVtbl->Invoke(pi, id, &IID_NULL, 0, wFlags, &dispparams,
			ret, &excepInfo, &nArgErr);
		if FAILED(sc)
		{
			if (excepInfo.pfnDeferredFillIn)
			{
				excepInfo.pfnDeferredFillIn(&excepInfo);
			}
			if (excepInfo.bstrDescription)
			{
				MessageBoxW(0, excepInfo.bstrDescription, excepInfo.bstrSource, MB_ICONSTOP|MB_TASKMODAL);
			}
			else
			{
				ReportError(sc, MB_ICONSTOP|MB_TASKMODAL);
			}
			SysFreeString(excepInfo.bstrDescription);
			SysFreeString(excepInfo.bstrSource);
			SysFreeString(excepInfo.bstrHelpFile);
		}
	}
	while (dispparams.cArgs--)
	{
		VariantClear(dispparams.rgvarg++);
	}
	return sc;
}

HRESULT invokeA(LPDISPATCH pi, VARIANT *ret, DISPID id, LPCCH op, VARIANT *argv)
{
	return invokeV(pi, ret, id, op, argv);
}
HRESULT invokeW(LPDISPATCH pi, VARIANT *ret, BSTR silent, LPCCH op, VARIANT *argv)
{
	DISPID id = DISPID_UNKNOWN;
	BSTR name = (BSTR)((UINT_PTR)silent & ~1);
	if (pi)
	{
		HRESULT sc = pi->lpVtbl->GetIDsOfNames(pi, &IID_NULL, &name, 1, 0, &id);
		if FAILED(sc)
		{
			if (!((UINT_PTR)silent & 1))
			{
				ReportError(sc, MB_ICONSTOP);
			}
			pi = 0;
		}
	}
	return invokeV(pi, ret, id, op, argv);
}

STDAPI ValidateArgs(VARIANT *argv, UINT argc, LPCCH pvt)
{
	VARTYPE vt;
	while ((vt = (BYTE)*pvt++) != 0)
	{
		if (argc == 0)
			return DISP_E_BADPARAMCOUNT;
		if (vt == VT_VARIANT)
		{
			VARIANT *ref = argv;
			while (V_VT(ref) == VT_VARIANT + VT_BYREF)
			{
				ref = V_VARIANTREF(ref);
			}
			vt = V_VT(ref);
		}
		if (vt != VT_VARIANT)
		{
			if (V_VT(argv) != vt)
			{
				HRESULT sc = VariantChangeType(argv, argv, 0, vt);
				if FAILED(sc)
					return sc;
			}
			else if (vt == VT_BSTR)
			{
				// Const strings passed in from Compiled VB may reside in r/o
				// memory. Since B2A() expects BSTR arguments to be writeable,
				// the following code copies short strings to writeable memory.
				// Long strings are assumed to be writeable anyway since they
				// are normally a result of some non-const expression.
				// This is a tradeoff between efficiency and safety.
				// A const string longer than 260 OLECHARs (520 bytes) will
				// provoke an access violation in B2A().
				char buffer[520];
				UINT length =  SysStringByteLen V_BSTR(argv);
				if (length <= sizeof buffer)
				{
					CopyMemory(buffer, V_BSTR(argv), length);
					SysFreeString V_BSTR(argv);
					V_BSTR(argv) = SysAllocStringByteLen(buffer, length);
				}
			}
		}
		--argc;
		++argv;
	}
	return argc ? DISP_E_BADPARAMCOUNT : S_OK;
}

/** 
 * @brief BSTR to PCH (ANSI) conversion
 * It needs BSTR to be wide (always the case in Windows) 
 *
 * @note THIS METHOD IS BUGGY : IT IS NOT CORRECT TO PUT A CHAR STRING IN A WCHAR BUFFER
 * IN FACT IN PLACE TRANSFORMATION OF WCHAR TO CHAR IS NOT CORRECT
 * the content of bcVal is changed
 */
/*
PCH NTAPI B2A(BSTR bcVal)
{
	static const char empty[] = {0};
	PCH pcVal = (PCH)empty;
	if (bcVal)
	{
		pcVal = ((PCH)bcVal) + 1;
		if (*(PCH)bcVal)
		{
			int cch = lstrlenW(bcVal) + 1;
			WideCharToMultiByte(CP_ACP, 0, bcVal, cch, pcVal, cch, 0, 0);
			// this is buggy : we need to set *bcVal = 0 (so we use pcVal = ((PCH)bcVal) + 2)
			*(PCH)bcVal = 0;
		}
	}
	return pcVal;
}
*/

// FIX B2A BEFORE UNCOMMENTING THIS ONE
/*
LPCTSTR NTAPI B2T(BSTR bcVal)
{
#ifdef _UNICODE
	return bcVal;
#else
	return B2A(bcVal);
#endif
}
*/

STDAPI LWDefProc(PVOID UNUSED_ARG(target), HRESULT UNUSED_ARG(sc),
	VARIANT *UNUSED_ARG(ret), VARIANT *UNUSED_ARG(argv), UINT UNUSED_ARG(argc),
	EXCEPINFO *UNUSED_ARG(e))
{
	return DISP_E_MEMBERNOTFOUND;
}

struct LWDispMap
{
	LWDispFxn pfnProc;
	int count;
	struct LWDispRef ref[1];
};

static STDMETHODIMP QueryInterface(struct LWDispatch *This,
					/* [in] */ REFIID riid,
					/* [iid_is][out] */ void **ppvObject)
{
	if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_IDispatch))
	{
		This -> lpVtbl -> AddRef(This);
		*ppvObject = This;
		return S_OK;
	}
	return E_NOINTERFACE;
}
        
static ULONG STDMETHODCALLTYPE AddRef(struct LWDispatch *This)
{
	return ++This->refc;
}        

static ULONG STDMETHODCALLTYPE Release(struct LWDispatch *This)
{
	ULONG Release = --This->refc;
	if (Release == 0)
	{
		free(This);
	}
	return Release;
}

static STDMETHODIMP GetTypeInfoCount(struct LWDispatch *UNUSED_ARG(This),
					/* [out] */ UINT *UNUSED_ARG(pctinfo))
{
	return E_NOTIMPL;
}

static STDMETHODIMP GetTypeInfo(struct LWDispatch *UNUSED_ARG(This),
					/* [in] */ UINT UNUSED_ARG(iTInfo),
					/* [in] */ LCID UNUSED_ARG(lcid),
					/* [out] */ ITypeInfo **UNUSED_ARG(ppTInfo))
{
	return E_NOTIMPL;
}

static STDMETHODIMP GetIDsOfNames(struct LWDispatch *This,
					/* [in] */ REFIID UNUSED_ARG(riid),
					/* [size_is][in] */ LPOLESTR *rgszNames,
					/* [in] */ UINT UNUSED_ARG(cNames),
					/* [in] */ LCID UNUSED_ARG(lcid),
					/* [size_is][out] */ DISPID *rgDispId)
{
	DISPID dispIdMember = 0;
	for ( ; dispIdMember < This->map->count ; ++dispIdMember)
	{
		if (StrCmpIW(This->map->ref[dispIdMember].pwcName, rgszNames[0]) == 0)
		{
			*rgDispId = dispIdMember;
			return S_OK;
		}
	}
	return DISP_E_MEMBERNOTFOUND;
}

static STDMETHODIMP Invoke(struct LWDispatch *This,
					/* [in] */ DISPID dispIdMember,
					/* [in] */ REFIID UNUSED_ARG(riid),
					/* [in] */ LCID UNUSED_ARG(lcid),
					/* [in] */ WORD wFlags,
					/* [out][in] */ DISPPARAMS *pDispParams,
					/* [out] */ VARIANT *pVarResult,
					/* [out] */ EXCEPINFO *pExcepInfo,
					/* [out] */ UINT *UNUSED_ARG(puArgErr))
{
	// provide a dummy result variable if pVarResult == 0
	VARIANT varEmpty;
	HRESULT sc = dispIdMember < 0 ? dispIdMember : (HRESULT)wFlags;
	if (pDispParams->cNamedArgs >
		(wFlags & (DISPATCH_PROPERTYPUT|DISPATCH_PROPERTYPUTREF) ? 1U : 0U))
		return DISP_E_NONAMEDARGS;
	VariantInit(&varEmpty);
	sc =
	(
		dispIdMember < 0 ? This->map->pfnProc :
		dispIdMember < This->map->count ? This->map->ref[dispIdMember].pfnProc :
		LWDefProc
	)
	(
		This->target, sc,
		pVarResult ? pVarResult : &varEmpty,
		pDispParams->rgvarg, pDispParams->cArgs,
		pExcepInfo
	);
	VariantClear(&varEmpty);
	return sc;
}

static const struct LWDispVtbl vtbl =
{
	QueryInterface,
	AddRef,
	Release,
	GetTypeInfoCount,
	GetTypeInfo,
	GetIDsOfNames,
	Invoke
};

const struct LWDispVtbl *NTAPI LWDispSubclass(struct LWDispVtbl *lpVtbl)
{
	*lpVtbl = vtbl;
	return &vtbl;
}

IDispatch *NTAPI LWDispatch(void *target, const void *disp_map,
	const struct LWDispVtbl *lpVtbl, struct LWDispatch *This)
{
	if (lpVtbl == 0)
		lpVtbl = &vtbl;
	if (This == 0)
		This = (struct LWDispatch *)malloc(sizeof(*This));
	This->lpVtbl = lpVtbl;
	This->target = target;
	This->map = (const struct LWDispMap *)disp_map;
	This->refc = 0;
	return (IDispatch *)This;
}

VARIANT NTAPI LWArg(LPDISPATCH pdispVal)
{
	VARIANT v;
	VariantInit(&v);
	if (pdispVal) (V_VAR(&v,DISPATCH) = pdispVal) -> lpVtbl -> AddRef(pdispVal);
	return v;
}

VARIANT NTAPI LWArgW(LPCWSTR wcVal)
{
	VARIANT v;
	VariantInit(&v);
	V_VAR(&v,BSTR) = SysAllocString(wcVal);
	return v;
}

VARIANT NTAPI LWArgA(LPCSTR cVal)
{
	UINT len = lstrlenA(cVal);
	VARIANT v;
	VariantInit(&v);
	V_VAR(&v,BSTR) = SysAllocStringLen(0, len);
	MultiByteToWideChar(CP_ACP, 0, cVal, -1, V_BSTR(&v), len);
	return v;
}

VARIANT NTAPI LWArgV(UINT vt, ...)
{
	VARIANT v;
	VariantInit(&v);
	V_VT(&v) = (VARTYPE)(vt & 0xF0FF);
	CopyMemory(&V_NONE(&v), &vt + 1, (vt & 0x0F00) >> 8);
	return v;
}

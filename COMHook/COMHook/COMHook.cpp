// COMHook.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

template <class INTERFACE>
HRESULT HookMethod(
	INTERFACE		*original,			// VTable need modified
	DWORD			vtableOffset,		// old function offset
	PVOID	const	proxyMethod,		// new function
	PVOID			*originalMethod		// old function
	)		
{
	HRESULT	hr = 0;
	PVOID	*originalVtable = *(PVOID**)original;
	DWORD	dwOld = 0;

	if (originalVtable[vtableOffset] == proxyMethod)
		return S_OK;

	CHK_BOOL_STS(::VirtualProtect(originalVtable+vtableOffset, sizeof(void*), PAGE_EXECUTE_READWRITE, &dwOld));

	*originalMethod = originalVtable[vtableOffset];
	originalVtable[vtableOffset] = proxyMethod;
Exit:
	return hr;
}

typedef HRESULT  (*FUN_INVOKE)(
	IDispatch	*pDispatch,
	_In_  DISPID dispIdMember,
	_In_  REFIID riid,
	_In_  LCID lcid,
	_In_  WORD wFlags,
	_In_  DISPPARAMS *pDispParams,
	_Out_opt_  VARIANT *pVarResult,
	_Out_opt_  EXCEPINFO *pExcepInfo,
	_Out_opt_  UINT *puArgErr);

FUN_INVOKE g_OrigInvoke = (FUN_INVOKE)0;

HRESULT STDMETHODCALLTYPE My_Invoke(
	IDispatch	*pDispatch,
	_In_  DISPID dispIdMember,
	_In_  REFIID riid,
	_In_  LCID lcid,
	_In_  WORD wFlags,
	_In_  DISPPARAMS *pDispParams,
	_Out_opt_  VARIANT *pVarResult,
	_Out_opt_  EXCEPINFO *pExcepInfo,
	_Out_opt_  UINT *puArgErr)
	{
	printf("My_Invoke enter...\n");
	HRESULT hr =g_OrigInvoke(pDispatch, dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
	printf("g_OrigInvoke return pVarResult %d\n", abs(pVarResult->boolVal));
	printf("My_Invoke exit with status %d...\n", hr);
	return hr;
	}


template <class T>
unsigned int MyFieldOffset(T func)
{
	union
	{
		T ptr;
		int i;
	};
	ptr = func;

	return i;
}

void TestFieldOffset()
{

}

int TestCOMHook()
{
	HRESULT		hr = 0;
	CLSID		ClassID;
	IDispatch	*pDispatch = 0;
	OLECHAR		*sMember = L"FileExists";
	DISPID		idFileExists;
	unsigned int puArgErr = 0;
	VARIANT		VarResult;
	EXCEPINFO	pExcepInfo;
	DISPPARAMS	pParams;
	BOOL		isCoInited = 0;
	VARIANT		Arguments[1];

	VariantInit(&Arguments[0]);
	VariantInit(&VarResult);
	//VariantInit(&pExcepInfo);
	memset(&pExcepInfo, 0, sizeof(pExcepInfo));
	memset(&pParams, 0, sizeof(DISPPARAMS));

	CHK_HR_STS(CoInitialize(NULL));
	isCoInited = 1;

	CHK_HR_STS(CLSIDFromProgID(OLESTR("Scripting.FileSystemObject"), &ClassID));
	CHK_HR_STS(CoCreateInstance(ClassID, NULL, CLSCTX_INPROC_SERVER, IID_IDispatch, (void **)&pDispatch));
	CHK_HR_STS(pDispatch->GetIDsOfNames(IID_NULL, &sMember, 1, LOCALE_SYSTEM_DEFAULT, &idFileExists));

	pParams.cArgs = 1;
	pParams.rgvarg = Arguments;
	pParams.cNamedArgs = 0;
	pParams.rgvarg[0].vt = VT_BSTR;
	pParams.rgvarg[0].bstrVal = SysAllocString(L"C:\\Test.txt");

	if (!pParams.rgvarg[0].bstrVal)
	{
		hr = -1; goto Exit;
	}
	CHK_HR_STS(pDispatch->Invoke( 
		idFileExists,
		IID_NULL,
		LOCALE_SYSTEM_DEFAULT,
		DISPATCH_METHOD,
		&pParams,
		&VarResult,
		&pExcepInfo,
		&puArgErr
		));

	printf("File Exists? %d\n", abs(VarResult.boolVal));

	unsigned int nFunOff;
	if (0)
	{
		unsigned int anFunOff[5];
		memset(anFunOff, 0, sizeof(anFunOff));
		anFunOff[0] = MyFieldOffset(&IDispatch::QueryInterface);
		anFunOff[1] = MyFieldOffset(&IDispatch::AddRef);
		nFunOff = MyFieldOffset(&IDispatch::Invoke);
	}
	else
		nFunOff = 6;
	
	CHK_HR_STS(HookMethod(pDispatch, nFunOff, &My_Invoke, (PVOID*)&g_OrigInvoke));

	CHK_HR_STS(pDispatch->Invoke(
		idFileExists,
		IID_NULL,
		LOCALE_SYSTEM_DEFAULT,
		DISPATCH_METHOD,
		&pParams,
		&VarResult,
		&pExcepInfo,
		&puArgErr
		));

	printf("File Exists? %d\n", abs(VarResult.boolVal));

Exit:
	if (pParams.rgvarg[0].bstrVal)
		SysFreeString(pParams.rgvarg[0].bstrVal);

	if (pDispatch)
		pDispatch->Release();

	if (isCoInited)
		CoUninitialize();

	return 0;
}



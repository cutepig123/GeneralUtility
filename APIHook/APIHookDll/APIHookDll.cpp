// APIHookDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "APIHookDll.h"
#include <stdio.h> 
#include <stdarg.h>
#include <assert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CAPIHookDllApp

//BEGIN_MESSAGE_MAP(CAPIHookDllApp, CWinApp)
//END_MESSAGE_MAP()
//
//
//// CAPIHookDllApp construction
//
//CAPIHookDllApp::CAPIHookDllApp()
//{
//	// TODO: add construction code here,
//	// Place all significant initialization in InitInstance
//}


// The one and only CAPIHookDllApp object
//
//CAPIHookDllApp theApp;
//
//
//// CAPIHookDllApp initialization
//
//BOOL CAPIHookDllApp::InitInstance()
//{
//	CWinApp::InitInstance();
//
//	return TRUE;
//}


//#include "stdafx.h"   
//#include "APIHook_Dll.h"   
#include <ImageHlp.h>   
#include <tlhelp32.h>   
#pragma comment(lib,"ImageHlp") //����ȫ�ֹ������ݶ�   
#pragma data_seg("Shared")   
HMODULE hmodDll=NULL;   
HHOOK hHook=NULL;   
#pragma data_seg()   
#pragma comment(linker,"/Section:Shared,rws") //����ȫ�ֹ������ݶε�����  

void TRACE(const char *fmt,...)
{
	va_list vl;
	char s[300];
	va_start(vl,fmt);
	vsprintf(s,fmt,vl);
	OutputDebugString(s);
	va_end(vl);
}
///////////////////////////////////// DllMain ���� /////////////////////////////////////////   
//dll����ڵ�   
BOOL APIENTRY DllMain( HMODULE hModule,    
	DWORD  ul_reason_for_call,    
	LPVOID lpReserved   
	)   
{   
	switch(ul_reason_for_call)   
	{   
	case DLL_PROCESS_ATTACH:   
		//if(sHook)     

	case DLL_PROCESS_DETACH:   
		UnInstallHook();   
		break;   
	}   
	hmodDll=hModule;   
	return TRUE;   
}

#define MY_ASSERT(x) if(!(x)) {assert(0);sts =-1; goto Exit;}
///////////////////////////////////// HookOneAPI ���� /////////////////////////////////////////   
//����IATת���Ĺؼ���������������壺   
//pszCalleeModuleName����Ҫhook��ģ����   
//pfnOriginApiAddress��Ҫ�滻���Լ�API�����ĵ�ַ   
//pfnDummyFuncAddress����Ҫhook��ģ�����ĵ�ַ   
//hModCallerModule������Ҫ���ҵ�ģ�����ƣ����û�б���ֵ��   
//     ���ᱻ��ֵΪö�ٵĳ������е��õ�ģ��   
void WINAPI HookOneAPI(LPCTSTR pszCalleeModuleName,PROC pfnOriginApiAddress,    
	PROC pfnDummyFuncAddress,HMODULE hModCallerModule)   
{   
	
	ULONG size;   
	short sts =0;
	//��ȡָ��PE�ļ��е�Import��IMAGE_DIRECTORY_DESCRIPTOR�����ָ��   
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)   
		ImageDirectoryEntryToData(hModCallerModule,TRUE,IMAGE_DIRECTORY_ENTRY_IMPORT,&size);   
	MY_ASSERT (pImportDesc != NULL) ;
	//���Ҽ�¼,������û��������Ҫ��DLL   
	for (;pImportDesc->Name;pImportDesc++)   
	{   
		LPSTR pszDllName = (LPSTR)((PBYTE)hModCallerModule+pImportDesc->Name);   
		if (lstrcmpiA(pszDllName,pszCalleeModuleName) == 0)   
			break;   
	}   
	MY_ASSERT (pImportDesc->Name != NULL) ;
	//Ѱ��������Ҫ�ĺ���   
	PIMAGE_THUNK_DATA pThunk =    
		(PIMAGE_THUNK_DATA)((PBYTE)hModCallerModule+pImportDesc->FirstThunk);//IAT   
	BOOL bFind=0;
	for (;pThunk->u1.Function;pThunk++)   
	{   
		//ppfn��¼����IAT������Ӧ�ĺ����ĵ�ַ   
		PROC * ppfn= (PROC *)&pThunk->u1.Function;     
		if (*ppfn == pfnOriginApiAddress)    
		{   
			//�����ַ��ͬ��Ҳ�����ҵ���������Ҫ�ĺ��������и�д������ָ������������ĺ���   
			MY_ASSERT(WriteProcessMemory(GetCurrentProcess(),ppfn,&(pfnDummyFuncAddress),   
				sizeof(pfnDummyFuncAddress),NULL));   
			bFind=1;
			break;
		}   
	} 
	MY_ASSERT(bFind);
Exit:
	TRACE("HookOneAPI %s %x->%x %x %d\n",pszCalleeModuleName,pfnOriginApiAddress, pfnDummyFuncAddress,hModCallerModule, sts);
}   
//�������ҹ��Ľ�����Ӧ�õ�dllģ���   
BOOL WINAPI HookAllAPI(LPCTSTR pszCalleeModuleName,PROC pfnOriginApiAddress,   
	PROC pfnDummyFuncAddress,HMODULE hModCallerModule)   
{   
	if (pszCalleeModuleName == NULL)   
	{   
		return FALSE;   
	}   
	if (pfnOriginApiAddress == NULL)   
	{   
		return FALSE;   
	}   
	//���û������Ҫ�ҹ���ģ�����ƣ�ö�ٱ��ҹ����̵��������õ�ģ�飬   
	//������Щģ����д���������Ӧ�������ƵĲ���   
	TRACE("HookAllAPI %s %x->%x\n", pszCalleeModuleName, pfnOriginApiAddress, pfnDummyFuncAddress);
	if (hModCallerModule == NULL)   
	{   
		MEMORY_BASIC_INFORMATION mInfo;   
		HMODULE hModHookDLL;   
		HANDLE hSnapshot;   
		MODULEENTRY32 me = {sizeof(MODULEENTRY32)};   
		//MODULEENTRY32:������һ����ָ��������Ӧ�õ�ģ���struct   
		VirtualQuery(HookOneAPI,&mInfo,sizeof(mInfo));   
		hModHookDLL=(HMODULE)mInfo.AllocationBase;   

		hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,0);   
		BOOL bOk = Module32First(hSnapshot,&me);   
		while (bOk)   
		{   
			if (me.hModule != hModHookDLL)   
			{   
				hModCallerModule = me.hModule;//��ֵ   
				//me.hModule:ָ��ǰ���ҹ����̵�ÿһ��ģ��    
				HookOneAPI(pszCalleeModuleName,pfnOriginApiAddress,   
					pfnDummyFuncAddress,hModCallerModule);   
			}   
			bOk = Module32Next(hSnapshot,&me);   
		}   
		return TRUE;     
	}   
	//����������ˣ����в���   
	else   
	{   
		HookOneAPI(pszCalleeModuleName,pfnOriginApiAddress,   
			pfnDummyFuncAddress,hModCallerModule);   
		return TRUE;   
	}   
	return FALSE;   
}
//////////////////////////////////// UnhookAllAPIHooks ���� /////////////////////////////////////   
//ͨ��ʹpfnDummyFuncAddress��pfnOriginApiAddress��ȵķ�����ȡ����IAT���޸�   
BOOL WINAPI UnhookAllAPIHooks(LPCTSTR pszCalleeModuleName,PROC pfnOriginApiAddress,   
	PROC pfnDummyFuncAddress,HMODULE hModCallerModule)   
{   
	PROC temp;   
	temp = pfnOriginApiAddress;   
	pfnOriginApiAddress = pfnDummyFuncAddress;   
	pfnDummyFuncAddress = temp;   
	return HookAllAPI(pszCalleeModuleName,pfnOriginApiAddress,   
		pfnDummyFuncAddress,hModCallerModule);   
} 
////////////////////////////////// GetMsgProc ���� ////////////////////////////////////////   
//�����ӳ̡������������ӳ̲�����ͬ��û��ʲô����������飬����������һ�������ӳ̣��γ�ѭ��   
LRESULT CALLBACK GetMsgProc(int code,WPARAM wParam,LPARAM lParam)   
{   
	return CallNextHookEx(hHook,code,wParam,lParam);   
}
//////////////////////////////////// InstallHook ���� /////////////////////////////////////   
//��װ��ж�ع��ӣ�BOOL IsHook�����Ǳ�־λ   
//��Ҫ���ĸ�API�������г�ʼ��   
//��������װ�Ĺ���������WH_GETMESSAGE   
void __declspec(dllexport) WINAPI InstallHook(BOOL IsHook,DWORD dwThreadId)   
{   
	if(IsHook)   
	{   
		hHook=SetWindowsHookEx(WH_GETMESSAGE,(HOOKPROC)GetMsgProc,hmodDll,dwThreadId);   

		//GetProcAddress(GetModuleHandle("GDI32.dll"),"ExtTextOutA")��ȡ��Ҫ���ĺ���������dll�еĵ�ַ   

		HookAllAPI("GDI32.dll",GetProcAddress(GetModuleHandle("GDI32.dll"),   
			"TextOutW"),(PROC)&H_TextOutW,NULL);   
		HookAllAPI("GDI32.dll",GetProcAddress(GetModuleHandle("GDI32.dll"),   
			"TextOutA"),(PROC)&H_TextOutA,NULL);   
	}   
	else   
	{   
		UnInstallHook();   
		UnhookAllAPIHooks("GDI32.dll",GetProcAddress(GetModuleHandle("GDI32.dll"),   
			"TextOutW"),(PROC)&H_TextOutW,NULL);   
		UnhookAllAPIHooks("GDI32.dll",GetProcAddress(GetModuleHandle("GDI32.dll"),   
			"TextOutA"),(PROC)&H_TextOutA,NULL);   
	}   
}

///////////////////////////////////// UnInstallHook ���� ////////////////////////////////////   
//ж�ع���   
BOOL WINAPI UnInstallHook()   
{   
	UnhookWindowsHookEx(hHook);   
	return TRUE;   
}   
///////////////////////////////////// H_TextOutA ���� /////////////////////////////////////////   
//���ǵ��滻����������������ʵ��������Ҫ���Ĺ���   
//��������������ʾһ���Ի���ָ�����滻���ĸ�����   
BOOL WINAPI H_TextOutA(HDC hdc,int nXStart,int nYStart,LPCSTR lpString,int cbString)   
{   
	//  FILE *stream=fopen("logfile.txt","a+t");   
	MessageBox(NULL,"TextOutA","APIHook_Dll ---rivershan",MB_OK);   
	TextOutA(hdc,nXStart,nYStart,lpString,cbString);//����ԭ���ĺ���������ʾ�ַ�   
	// fprintf(stream,lpString);   
	// fclose(stream);   
	return TRUE;   
}   
///////////////////////////////////// H_TextOutW ���� /////////////////////////////////////////   
//ͬ��   
BOOL WINAPI H_TextOutW(HDC hdc,int nXStart,int nYStart,LPCWSTR lpString,int cbString)   
{    
	MessageBox(NULL,"TextOutW","APIHook_Dll ---rivershan",MB_OK);   
	TextOutW(hdc,nXStart,nYStart,lpString,cbString);//����ԭ���ĺ���������ʾ�ַ�   
	return TRUE;   
}
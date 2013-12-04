// MyDetour.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "assert.h"

static char g_acPattern1[] ={0x8b, 0xff, 0x55, 0x8b, 0xec};
static struct
{
	char *acPatterns;
	int n;
}g_pacPatterns[] =
{
	{g_acPattern1, sizeof(g_acPattern1)}
};

#define	ORIGINAL_BKUP_SZ	20
#define	MY_JUMP_SZ			5

int ReplaceFunction(FARPROC pOriginal, void* pNew, char pOriginalBkup[ORIGINAL_BKUP_SZ])
{
	DWORD flOldProtect;
	BOOL bRet =VirtualProtect( pOriginalBkup,ORIGINAL_BKUP_SZ, PAGE_EXECUTE_READWRITE, &flOldProtect);
	assert(bRet);

	// check
	for( int i=0; i<sizeof(g_pacPatterns)/sizeof(g_pacPatterns[0]); i++)
	{
		if( memcmp( pOriginal, g_pacPatterns[i].acPatterns, g_pacPatterns[i].n) !=0 )
			continue;

		// step 1- update pOriginalBkup
	    // backup pOriginal to pOriginalBkup
		assert( g_pacPatterns[i].n + MY_JUMP_SZ <= ORIGINAL_BKUP_SZ );
		memcpy( pOriginalBkup, g_pacPatterns[i].acPatterns, g_pacPatterns[i].n );
		// add a jump cmd in pOriginalBkup
		INT_PTR pDest =(INT_PTR)pOriginal +g_pacPatterns[i].n;
		INT_PTR pSrc = (INT_PTR)pOriginalBkup + g_pacPatterns[i].n + MY_JUMP_SZ;
		char *pcOriginalBkup =(char*)pOriginalBkup +g_pacPatterns[i].n;
		pcOriginalBkup[0] =0xe9;
		*(DWORD*)(pcOriginalBkup+1) = DWORD( pDest - pSrc);

		// step 2- update pOriginal
		// -- change to jmp pNew in pOriginal
		BOOL bRet =VirtualProtect( pOriginal,g_pacPatterns[i].n, PAGE_EXECUTE_READWRITE, &flOldProtect);
		assert(bRet);
		
		char *pcOriginal =(char*)pOriginal;
		pcOriginal[0] =0xe9;
		*(DWORD*)(pcOriginal+1) = DWORD((INT_PTR)pNew - ((INT_PTR)pOriginal + MY_JUMP_SZ));

		return 0;
	}

	return -1;
}


char g_pOriginalBkup[ORIGINAL_BKUP_SZ];

typedef int  (WINAPI *MessageBoxAType)(
		HWND hWnd,
		LPCSTR lpText,
		LPCSTR lpCaption,
		UINT uType);

int
WINAPI
MyMessageBoxA(
     HWND hWnd,
     LPCSTR lpText,
     LPCSTR lpCaption,
     UINT uType)
{
	printf("This is MyMessage!\n");

	char acText[100];
	char acCap[100];

	sprintf(acText, "%s (Modified)", lpText );
	sprintf(acCap, "%s (Modified)", lpCaption );

	MessageBoxAType pMsgFtn =(MessageBoxAType)(void*)g_pOriginalBkup;
	pMsgFtn(NULL, acText, acCap, 0);

	return 0;
}

void Test1()
{
	HMODULE hMode =LoadLibrary("user32.dll");
	FARPROC pProc =GetProcAddress(hMode, "MessageBoxA");
	
	printf("Before hook\n");
	MessageBox(NULL, "Text", "Cap", 0);

	int nRet =ReplaceFunction(pProc, MyMessageBoxA, g_pOriginalBkup);
	assert(nRet==0);

	printf("After hook -- call MessageBox\n");
	MessageBox(NULL, "Text", "Cap", 0);
}

typedef int (__cdecl *printf_type)(_In_z_ _Printf_format_string_ const char * _Format, ...);

int __cdecl myprintf(_In_z_ _Printf_format_string_ const char * _Format, ...)
{
	printf_type pMsgFtn =(printf_type)(void*)g_pOriginalBkup;
	pMsgFtn( "Fmt: %s\n", _Format );
	return 0;
}

void Test2()
{
	printf("Before hook\n");

	int nRet =ReplaceFunction((FARPROC)printf, myprintf, g_pOriginalBkup);
	assert(nRet==0);
	
	printf("After hook %d\n", 1);
}

int _tmain(int argc, _TCHAR* argv[])
{
	
	Test1();
}


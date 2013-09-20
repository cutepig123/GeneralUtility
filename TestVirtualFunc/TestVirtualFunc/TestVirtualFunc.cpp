// TestVirtualFunc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <assert.h>

class A
{
public:
A(){}
~A(){}

virtual void f1(){ printf("Founction f1 called\n"); }
virtual void f2(){ printf("Founction f2 called\n"); }
virtual void f3(){ printf("Founction f3 called\n"); }

private:
int n;

};

void reset_f1(A *pA)
{
	long** pplVrtable= (long**)(pA); //取得虚函数表的指针
	*pplVrtable = *pplVrtable +1;//将虚函数表的指针指向虚函数表第二个值。
}

#define MY_ASSERT(x) if(!(x)) {assert(0);sts =-1; goto Exit;}

short EnableModifyMemory(void *p, int n)
{
	short sts =0;
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ::GetCurrentProcessId());

	MEMORY_BASIC_INFORMATION mbi = {0};

	MY_ASSERT (VirtualQueryEx(hProcess, (LPVOID)(p), &mbi, sizeof(mbi)) == sizeof(mbi));

	DWORD dwOldProtect = 0;
	MY_ASSERT(::VirtualProtectEx(hProcess, mbi.BaseAddress, n, PAGE_EXECUTE_READWRITE, &dwOldProtect));

Exit:
	CloseHandle(hProcess);
	return sts;
}
int _tmain(int argc, _TCHAR* argv[])
{
	short sts =0;
	A *a =new A;
	a->f1();

	MY_ASSERT(0==EnableModifyMemory(&a, 4));
	printf("After reset f1\n");
	reset_f1(a);
	a->f1();
Exit:
	return sts;
}


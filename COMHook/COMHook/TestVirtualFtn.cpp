#include "stdafx.h"

struct A
{
	virtual void f(){};
};

struct B:public A
{
	virtual void f(){};
};

unsigned int GetVirtualFunAddr(void *pObj, int nFuncIdx)
{
	unsigned int* pVtbl = *(unsigned int**)pObj;
	return pVtbl[nFuncIdx];
}

unsigned int GetVirtualFunTblAddr(void *pObj)
{
	unsigned int pVtbl = *(unsigned int*)pObj;
	return pVtbl;
}

void TestVirtFunAddr()
{
	int hr = 0;

	A a;
	unsigned int nAddrA = GetVirtualFunTblAddr(&a);

	A *pA = new A;
	unsigned int nAddrpA = GetVirtualFunTblAddr(pA);

	B b;
	unsigned int nAddrB = GetVirtualFunTblAddr(&b);

	CHK_BOOL_STS(nAddrA == nAddrpA);
	CHK_BOOL_STS(nAddrA != nAddrB);

Exit:
	;
}

// TestExplicitTemplateInstantialization.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "../DllProj/DllProj.h"

int _tmain(int argc, _TCHAR* argv[])
{
	TmplA<int> a;
	a.f();

	//int i=1;
	//TmplF(i);

	return 0;
}


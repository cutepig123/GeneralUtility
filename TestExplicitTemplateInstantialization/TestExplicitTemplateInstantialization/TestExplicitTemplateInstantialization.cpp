// TestExplicitTemplateInstantialization.cpp : 定义控制台应用程序的入口点。
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


// DllProj.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "DllProj.h"
#include "stdio.h"

// 这是导出变量的一个示例
DLLPROJ_API int nDllProj=0;

// 这是导出函数的一个示例。
DLLPROJ_API int fnDllProj(void)
{
	return 42;
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 DllProj.h
CDllProj::CDllProj()
{
	return;
}

// ---- TmplA<T> --------
template <class T>
void TmplA<T>::f()
{
	printf(__FUNCTION__ " called\n");
}

template 
class TmplA<int>;

// ---- TmplF<T> -------- 
template <class T> 
void TmplF(T)
{

}

template <>
void TmplF(int);

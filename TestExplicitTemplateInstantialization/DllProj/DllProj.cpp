// DllProj.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "DllProj.h"
#include "stdio.h"

// ���ǵ���������һ��ʾ��
DLLPROJ_API int nDllProj=0;

// ���ǵ���������һ��ʾ����
DLLPROJ_API int fnDllProj(void)
{
	return 42;
}

// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� DllProj.h
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

// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <assert.h>

#define	CHK_HR_STS(x)	hr =(x); if(!SUCCEEDED(hr)) {assert(!#x);goto Exit;}
#define	CHK_BOOL_STS(x)	 if(!(x)) {assert(!#x); hr=-1; goto Exit;}


// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�

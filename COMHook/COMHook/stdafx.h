// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <assert.h>

#define	CHK_HR_STS(x)	hr =(x); if(!SUCCEEDED(hr)) {assert(!#x);goto Exit;}
#define	CHK_BOOL_STS(x)	 if(!(x)) {assert(!#x); hr=-1; goto Exit;}


// TODO:  在此处引用程序需要的其他头文件

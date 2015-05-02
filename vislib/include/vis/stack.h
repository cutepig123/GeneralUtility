#pragma once
#include <vis/type.h>
#include <vis/export.h>

#define	STK_MODULE_ID	0x0100
#define	STK_ERR_MALLOC	-STK_MODULE_ID-0x01

VIS_XXPORT VIS_INT16	STK_Push();
VIS_XXPORT VIS_INT16	STK_Pop();

VIS_XXPORT VIS_INT16	STK_Malloc(VIS_UINT32 n, VIS_VOID** pp);

template <class T>
VIS_XXPORT VIS_INT16	STK_Malloc(VIS_SIZE const &sz, VIS_BUF_T<T> *p);
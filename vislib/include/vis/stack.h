#pragma once
#include <vis/type.h>
#include <vis/moduleid.h>
#include <vis/export.h>

#define	STK_MODULE_ID	MODULE_ID_STK
#define	STK_ERR_MALLOC	-STK_MODULE_ID-0x01

VIS_XXPORT VIS_INT16	STK_Push();
VIS_XXPORT VIS_INT16	STK_Pop();

VIS_XXPORT VIS_INT16	STK_Malloc(VIS_UINT32 n, VIS_VOID** pp);

template <class T>
VIS_XXPORT VIS_INT16	STK_Malloc(VIS_SIZE const &size, VIS_BUF_T<T> *ptr);
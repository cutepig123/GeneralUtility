#pragma once
#include <vis/type.h>
#include <vis/export.h>

#define	STK_MODULE_ID	0x0100
#define	STK_ERR_MALLOC	-STK_MODULE_ID-0x01

VIS_XXPORT VIS_VOID	*VIS_Malloc(VIS_UINT32 n);

#pragma once
#include <vis/type.h>
#include <vis/export.h>
#include <memory>

struct VIS_MAT_BUF_T_MyStruct;

template <class T>
struct VIS_MAT_BUF_T
{
	VIS_BUF_T<T> buf;

	std::shared_ptr<VIS_MAT_BUF_T_MyStruct>	m;
};

template <class T>
VIS_XXPORT VIS_INT16 VIS_LoadImage(const VIS_INT8 *pcPath, VIS_MAT_BUF_T<T> *pBuf);

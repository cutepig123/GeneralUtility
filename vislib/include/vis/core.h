#pragma once
#include <vis/type.h>
#include <vis/export.h>

VIS_XXPORT VIS_INT16 VIS_EdgeDetect(
	VIS_BUF_T<VIS_UINT8> const	&src_gray,
	VIS_BUF_T<VIS_INT16>		&abs_grad_x_thin,
	//VIS_BUF_T<VIS_INT16>		&grad_x,
	//VIS_BUF_T<VIS_INT16>		&grad_y,
	VIS_BUF_T<VIS_INT16>		&dir);

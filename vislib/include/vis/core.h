#pragma once
#include <vis/type.h>
#include <vis/export.h>

VIS_XXPORT VIS_INT16 VIS_EdgeDetect(
	VIS_BUF_T<VIS_UINT8> const	&src_gray,
	VIS_BUF_T<VIS_INT16>		&abs_grad_x_thin,
	//VIS_BUF_T<VIS_INT16>		&grad_x,
	//VIS_BUF_T<VIS_INT16>		&grad_y,
	VIS_BUF_T<VIS_INT16>		&dir);

template <class T>
VIS_XXPORT VIS_INT16 VIS_CreateChildBuf(
	VIS_BUF_T<T>	const	&src,
	VIS_POINT		const	&off,
	VIS_SIZE		const	&op,
	VIS_BUF_T<T>			&des)
{
	VIS_INT16	wSts = VIS_OK;
	VIS_CHK_ASSERT(off.x >= 0 && off.x + op.x <= src.sz.x);
	VIS_CHK_ASSERT(off.y >= 0 && off.y + op.y <= src.sz.y);

	des.p = src.p + src.step *off.y +off.x;
	des.step = src.step;
	des.sz = op;
Exit:
	return wSts;
}

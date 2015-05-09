#include <vis/asm.h>
#include <vis/opencv.h>
#include <vis/core.h>

inline IMG_WORD  DSP_Copy_rr(IMG_RBUF	const *prbSbuf,
					  IMG_COORD const *pcoSoff,
					  IMG_RBUF		  *prbDbuf,
					  IMG_COORD const *pcoDoff,
					  IMG_SIZE	const *pszOpsize)
{
	VIS_INT16 wSts = VIS_OK;
	cv::Mat matS, matD;

	IMG_RBUF	rbSRef, rbDRef;
	wSts = VIS_CreateChildBuf(*prbSbuf, *pcoSoff, *pszOpsize, rbSRef);
	VIS_CHK_STS;

	wSts = VIS_CreateChildBuf(*prbDbuf, *pcoDoff, *pszOpsize, rbDRef);
	VIS_CHK_STS;

	OPCV_ConvBufRef(&rbSRef, &matS);
	OPCV_ConvBufRef(&rbDRef, &matD);

	matD =matS;
Exit:
	return wSts;
}

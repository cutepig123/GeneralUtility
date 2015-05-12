#include <vis/asm.h>
#include <vis/opencv.h>
#include <vis/core.h>

IMG_WORD  DSP_Copy_rr(IMG_RBUF	const *prbSbuf,
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

template <class T>
IMG_WORD DSP_AddC_impl(
	VIS_BUF_T<T>			const	*prbSbuf,
	IMG_COORD				const	*pcoSoff,
	T						const	rConst,
	VIS_BUF_T<T>			*prbDbuf,
	IMG_COORD		const	*pcoDoff,
	IMG_SIZE		const	*pszOpsize)
{
	VIS_INT16 wSts = VIS_OK;
	cv::Mat matS, matD;

	VIS_BUF_T<T>	rbSRef, rbDRef;
	wSts = VIS_CreateChildBuf(*prbSbuf, *pcoSoff, *pszOpsize, rbSRef);
	VIS_CHK_STS;

	wSts = VIS_CreateChildBuf(*prbDbuf, *pcoDoff, *pszOpsize, rbDRef);
	VIS_CHK_STS;

	OPCV_ConvBufRef(&rbSRef, &matS);
	OPCV_ConvBufRef(&rbDRef, &matD);

	matD = matS +rConst;
Exit:
	return wSts;
}

template
VIS_XXPORT IMG_WORD DSP_AddC_impl(
	VIS_BUF_T<VIS_UINT8>			const	*prbSbuf,
	IMG_COORD				const	*pcoSoff,
	VIS_UINT8						const	rConst,
	VIS_BUF_T<VIS_UINT8>			*prbDbuf,
	IMG_COORD		const	*pcoDoff,
	IMG_SIZE		const	*pszOpsize);
	
template
VIS_XXPORT IMG_WORD DSP_AddC_impl(
	VIS_BUF_T<VIS_FLOAT32>			const	*prbSbuf,
	IMG_COORD				const	*pcoSoff,
	VIS_FLOAT32						const	rConst,
	VIS_BUF_T<VIS_FLOAT32>			*prbDbuf,
	IMG_COORD		const	*pcoDoff,
	IMG_SIZE		const	*pszOpsize);	
	
template <class T>	
IMG_WORD DSP_FillBuffer_impl(T	const rValue,
	VIS_BUF_T<T>		  *prbBuf,
	IMG_COORD const *pcoOff,
	IMG_SIZE	const *pszSize)
{
	VIS_INT16 wSts = VIS_OK;
	cv::Mat mat;

	VIS_BUF_T<T>	rbRef;
	wSts = VIS_CreateChildBuf(*prbBuf, *pcoOff, *pszSize, rbRef);
	VIS_CHK_STS;

	OPCV_ConvBufRef(&rbRef, &mat);

	mat = rValue;
Exit:
	return wSts;
}
	
template
VIS_XXPORT 	IMG_WORD DSP_FillBuffer_impl(VIS_FLOAT32	const rValue,
	VIS_BUF_T<VIS_FLOAT32>		  *prbBuf,
	IMG_COORD const *pcoOff,
	IMG_SIZE	const *pszSize);
	
template
VIS_XXPORT 	IMG_WORD DSP_FillBuffer_impl(VIS_UINT8	const rValue,
	VIS_BUF_T<VIS_UINT8>		  *prbBuf,
	IMG_COORD const *pcoOff,
	IMG_SIZE	const *pszSize);
	
template <class T, class T2>		
IMG_WORD  DSP_Copy_impl(VIS_BUF_T<T>	const *prbSbuf,
	IMG_COORD const *pcoSoff,
	VIS_BUF_T<T2>		  *prbDbuf,
	IMG_COORD const *pcoDoff,
	IMG_SIZE	const *pszOpsize)
{
	VIS_INT16 wSts = VIS_OK;
	cv::Mat matS, matD;

	VIS_BUF_T<T>	rbSRef;
	VIS_BUF_T<T2>	rbDRef;
	wSts = VIS_CreateChildBuf(*prbSbuf, *pcoSoff, *pszOpsize, rbSRef);
	VIS_CHK_STS;

	wSts = VIS_CreateChildBuf(*prbDbuf, *pcoDoff, *pszOpsize, rbDRef);
	VIS_CHK_STS;

	OPCV_ConvBufRef(&rbSRef, &matS);
	OPCV_ConvBufRef(&rbDRef, &matD);

	matD = matS;
Exit:
	return wSts;
}

template
VIS_XXPORT 	IMG_WORD  DSP_Copy_impl(VIS_BUF_T<VIS_FLOAT32>	const *prbSbuf,
	IMG_COORD const *pcoSoff,
	VIS_BUF_T<VIS_FLOAT32>		  *prbDbuf,
	IMG_COORD const *pcoDoff,
	IMG_SIZE	const *pszOpsize);
	
template
VIS_XXPORT 	IMG_WORD  DSP_Copy_impl(VIS_BUF_T<VIS_UINT8>	const *prbSbuf,
	IMG_COORD const *pcoSoff,
	VIS_BUF_T<VIS_FLOAT32>		  *prbDbuf,
	IMG_COORD const *pcoDoff,
	IMG_SIZE	const *pszOpsize);
	
template
VIS_XXPORT 	IMG_WORD  DSP_Copy_impl(VIS_BUF_T<VIS_FLOAT32>	const *prbSbuf,
	IMG_COORD const *pcoSoff,
	VIS_BUF_T<VIS_UINT8>		  *prbDbuf,
	IMG_COORD const *pcoDoff,
	IMG_SIZE	const *pszOpsize);
	
IMG_WORD	MAT_FitAXBInit(MAT_FITPARA		const	*pstPara0,
	MAT_FITPARA				*pstPara1)
{
	memset(pstPara1, 0, sizeof(*pstPara1));
	return OK;
}
	
#define   IPPAPI_IMPL( type,name,arg )        type __STDCALL name arg
 
template <class T>	
IPPAPI_IMPL(IppStatus, ippiAddC_C1IR_impl,  (T value, T* pSrcDst, int srcDstStep,
                                       IppiSize roiSize))
{
	VIS_INT16 wSts = VIS_OK;
	cv::Mat mat;

	VIS_BUF_T<T>	rbRef;
	rbRef.ptr =pSrcDst;
	rbRef.size.x =roiSize.width;
	rbRef.size.y =roiSize.height;
	rbRef.linestep =srcDstStep/sizeof(T);
	
	VIS_CHK_ASSERT( sizeof(T) *rbRef.linestep ==srcDstStep);
	
	OPCV_ConvBufRef(&rbRef, &mat);

	mat += value;
Exit:
	return (IppStatus)wSts;
}

template
VIS_XXPORT IPPAPI_IMPL(IppStatus, ippiAddC_C1IR_impl,  (Ipp32f value, Ipp32f* pSrcDst, int srcDstStep,
                                       IppiSize roiSize));
									   
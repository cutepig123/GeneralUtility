#include <vis/IO.h>
#include <vis/opencv.h>
#include <opencv2/highgui/highgui.hpp>

struct VIS_MAT_BUF_T_MyStruct
{
	cv::Mat		 container;
};

template <class T>
VIS_INT16 VIS_LoadImage(const VIS_INT8 *pcPath, VIS_MAT_BUF_T<T> *pBuf)
{
	VIS_INT16 wSts = VIS_OK;
	
	if (!pBuf->m)
		pBuf->m.reset(new VIS_MAT_BUF_T_MyStruct);

	pBuf->m->container = cv::imread((char*)pcPath);

	wSts = OPCV_ConvBufRef(&pBuf->m->container, &pBuf->buf);
	VIS_CHK_STS;
Exit:
	return wSts;
	
}

template
VIS_XXPORT VIS_INT16 VIS_LoadImage(VIS_INT8 const *pcPath, VIS_MAT_BUF_T<VIS_UINT8> *pBuf);

template
VIS_XXPORT VIS_INT16 VIS_LoadImage(VIS_INT8 const *pcPath, VIS_MAT_BUF_T<VIS_FLOAT32> *pBuf);

template <class T>
VIS_INT16 VIS_SaveImage(const VIS_INT8 *pcPath, const VIS_BUF_T<T> *pBuf)
{
	VIS_INT16 wSts = VIS_OK;

	cv::Mat m;
	wSts = OPCV_ConvBufRef(pBuf, &m);
	VIS_CHK_STS;

	//cv::imwrite((char*)pcPath, m);
Exit:
	return wSts;

}

template
VIS_XXPORT VIS_INT16 VIS_SaveImage(const VIS_INT8 *pcPath, const VIS_BUF_T<VIS_UINT8> *pBuf);

template
VIS_XXPORT VIS_INT16 VIS_SaveImage(const VIS_INT8 *pcPath, const VIS_BUF_T<VIS_FLOAT32> *pBuf);


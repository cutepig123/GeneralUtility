#include "opencv2/core.hpp"
#include <vis/opencv.h>


template <class T>
struct	OPENCV_TRAITS;

template <>
struct	OPENCV_TRAITS<VIS_UINT8>
{
	static const int nType =CV_8UC1;
};	

/*
template <class T>
VIS_INT16	OPCV_ConvBufRef(const CvMat* mat, VIS_BUF<T> *p)
{
	cvMat( int rows, int cols, int type, void* data CV_DEFAULT(NULL))
}
*/
//
//template <class T>
//VIS_INT16	OPCV_ConvBufRef(const VIS_BUF_T<T> *p, CvMat* mat)
//{
//	int rows =p->sz.y;
//	int cols =p->sz.x;
//	Mat m( rows, cols, typename OPENCV_TRAITS<T>::nType, p->p);
//	
//	swap(m, *mat);
//	return VIS_OK;
//}
//
//template <class T>
//VIS_INT16	OPCV_ConvBufRef(const CvMat* mat, VIS_BUF<T> *p)
//{
//	int rows =p->sz.y;
//	int cols =p->sz.x;
//	Mat m( rows, cols, OPENCV_TRAITS<T>::nType, p->p);
//	
//	swap(m, *mat);
//	return VIS_OK;
//}

template <class T>
VIS_INT16	OPCV_ConvBufRef(const cv::Mat* mat, VIS_BUF_T<T> *p)
{
	VIS_INT16 wSts =VIS_OK;
	VIS_CHK_ASSERT( OPENCV_TRAITS<T>::nType== mat->type() );

	p->sz.x= mat->size().width;
	p->sz.y= mat->size().height;
	p->step =mat->step1();
	p->p =(T*)mat->ptr();
Exit:
	return wSts;
}

//template <class T>
//VIS_INT16	OPCV_ConvBufRef(const IplImage* mat, VIS_BUF<T> *p);
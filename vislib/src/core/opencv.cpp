
#include <vis/opencv.h>

/*
template <class T>
VIS_INT16	OPCV_ConvBufRef(const CvMat* mat, VIS_BUF<T> *ptr)
{
	cvMat( int rows, int cols, int type, void* data CV_DEFAULT(NULL))
}
*/
//
//template <class T>
//VIS_INT16	OPCV_ConvBufRef(const VIS_BUF_T<T> *ptr, CvMat* mat)
//{
//	int rows =ptr->size.y;
//	int cols =ptr->size.x;
//	Mat m( rows, cols, typename OPENCV_TRAITS<T>::nType, ptr->ptr);
//	
//	swap(m, *mat);
//	return VIS_OK;
//}
//
//template <class T>
//VIS_INT16	OPCV_ConvBufRef(const CvMat* mat, VIS_BUF<T> *ptr)
//{
//	int rows =ptr->size.y;
//	int cols =ptr->size.x;
//	Mat m( rows, cols, OPENCV_TRAITS<T>::nType, ptr->ptr);
//	
//	swap(m, *mat);
//	return VIS_OK;
//}

template <class T>
VIS_INT16	OPCV_ConvBufRef(const cv::Mat* mat, VIS_BUF_T<T> *ptr)
{
	VIS_INT16 wSts =VIS_OK;
	VIS_CHK_ASSERT( OPENCV_TRAITS<T>::nType== mat->type() );

	ptr->size.x= mat->size().width;
	ptr->size.y= mat->size().height;
	ptr->linestep =mat->step1();
	ptr->ptr =(T*)mat->ptr();
Exit:
	return wSts;
}

template 
VIS_XXPORT VIS_INT16	OPCV_ConvBufRef(const cv::Mat* mat, VIS_BUF_T<VIS_UINT8> *ptr);

template
VIS_XXPORT VIS_INT16	OPCV_ConvBufRef(const cv::Mat* mat, VIS_BUF_T<VIS_INT16> *ptr);

template
VIS_XXPORT VIS_INT16	OPCV_ConvBufRef(const cv::Mat* mat, VIS_BUF_T<VIS_FLOAT32> *ptr);


template <class T>
VIS_INT16	OPCV_ConvBufRef(const VIS_BUF_T<T> *ptr, cv::Mat* mat)
{
	VIS_INT16 wSts = VIS_OK;
	
	*mat = cv::Mat(ptr->size.y, ptr->size.x, OPENCV_TRAITS<T>::nType, ptr->ptr, ptr->linestep);
	
//Exit:
	return wSts;
}

template
VIS_XXPORT VIS_INT16	OPCV_ConvBufRef(const VIS_BUF_T<VIS_UINT8> *ptr, cv::Mat* mat);

template
VIS_XXPORT VIS_INT16	OPCV_ConvBufRef(const VIS_BUF_T<VIS_INT16> *ptr, cv::Mat* mat);

template
VIS_XXPORT VIS_INT16	OPCV_ConvBufRef(const VIS_BUF_T<VIS_FLOAT32> *ptr, cv::Mat* mat);

//template <class T>
//VIS_INT16	OPCV_ConvBufRef(const IplImage* mat, VIS_BUF<T> *ptr);
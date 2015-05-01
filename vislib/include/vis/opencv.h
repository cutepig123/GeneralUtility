#pragma once
#include <vis/type.h>
#include <vis/export.h>

//#include <opencv2\core\core.hpp>
//#include <opencv2\core\mat.hpp>

//template <class T>
//VIS_INT16	OPCV_ConvBufRef(const CvMat* mat, VIS_BUF_T<T> *p);
//template <class T>
//VIS_INT16	OPCV_ConvBufRef(const Mat* mat, VIS_BUF_T<T> *p);
//template <class T>
//VIS_INT16	OPCV_ConvBufRef(const IplImage* mat, VIS_BUF_T<T> *p);

template <class T>
VIS_XXPORT VIS_INT16	OPCV_ConvBufRef(const cv::Mat* mat, VIS_BUF_T<T> *p);

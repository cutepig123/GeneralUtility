#pragma once
#include <vis/type.h>
#include <vis/export.h>
#include <opencv2/core.hpp>

template <class T>
struct	OPENCV_TRAITS;

template <>
struct	OPENCV_TRAITS<VIS_UINT8>
{
	static const int nType = CV_8UC1;
};

template <>
struct	OPENCV_TRAITS<VIS_INT16>
{
	static const int nType = CV_16SC1;
};

template <>
struct	OPENCV_TRAITS<VIS_FLOAT32>
{
	static const int nType = CV_32FC1;
};

//#include <opencv2\core\core.hpp>
//#include <opencv2\core\mat.hpp>

//template <class T>
//VIS_INT16	OPCV_ConvBufRef(const CvMat* mat, VIS_BUF_T<T> *ptr);
//template <class T>
//VIS_INT16	OPCV_ConvBufRef(const Mat* mat, VIS_BUF_T<T> *ptr);
//template <class T>
//VIS_INT16	OPCV_ConvBufRef(const IplImage* mat, VIS_BUF_T<T> *ptr);

template <class T>
VIS_XXPORT VIS_INT16	OPCV_ConvBufRef(const cv::Mat* mat, VIS_BUF_T<T> *ptr);


template <class T>
VIS_XXPORT VIS_INT16	OPCV_ConvBufRef(const VIS_BUF_T<T> *ptr, cv::Mat* mat);

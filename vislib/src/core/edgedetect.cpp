#include <vis/core.h>
#include <vis/stack.h>
#include <vis/IO.h>
#include <vis/opencv.h>
#include <vis/debug.h>
#include "opencv2/imgproc/imgproc.hpp"
#include <functional>
#include <opencv2/highgui/highgui.hpp>


// note:
// channel is diff with dims!

int MyMapShortImgToColor(cv::Mat const &src,
	std::function<int(short val, int x, int y, uchar *c1, uchar *c2, uchar *c3)> &ftn, cv::Mat &dst)
{
	dst = cv::Mat::zeros(src.rows, src.cols, CV_8UC3);
	//int sz[] = { src.cols, src.rows };
	//dst = Mat::zeros(3, sz, CV_8UC3);
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			const short* t = src.ptr<short>(i, j);
			uchar *d = dst.ptr<uchar>(i, j);
			ftn(*t, j, i, d, d + 1, d + 2);
		}
	}
	return 0;
}

struct	TRAITS_X_16S_Dummy_Max{};
struct	TRAITS_X_16S_Dummy_Min{};

template <class T>
struct TRAITS_X_16S_T
{
	static int N1(VIS_BUF_T<VIS_INT16> const &abs_grad_x)
	{
		return abs_grad_x.sz.y;
	}

	static int N2(VIS_BUF_T<VIS_INT16> const &abs_grad_x)
	{
		return abs_grad_x.sz.x;
	}

	static short* ptr(VIS_BUF_T<VIS_INT16> &abs_grad_x, int y, int x)
	{
		return abs_grad_x.p + y*abs_grad_x.step + x;
	}

	static const short* ptr(VIS_BUF_T<VIS_INT16> const &abs_grad_x, int y, int x)
	{
		return abs_grad_x.p + y*abs_grad_x.step + x;
	}

	// return index
	static int Max(const VIS_BUF_T<VIS_INT16> &abs_grad_x, int n1, int n2_s, int n2_e, T=T());
};

template <>
int TRAITS_X_16S_T<TRAITS_X_16S_Dummy_Max>::Max(const VIS_BUF_T<VIS_INT16> &abs_grad_x, int n1, int n2_s, int n2_e, TRAITS_X_16S_Dummy_Max)
{
	const short *c1 = ptr(abs_grad_x, n1, n2_s);
	const short *c2 = ptr(abs_grad_x, n1, n2_e);
	const short* pMax = std::max_element(c1, c2);
	
	//for(const short *c =c1; c<c2; c++)
	//	TRACE("%d ",*c);
	//TRACE("\n");
#if 1	
	int magrin =5;	// gray level
	double sum=0.0f;
	double n=0.0f;
	for(const short *c =c1; c<c2; c++)
	{
		int diff = *pMax -*c;
		assert(diff>=0);

		if (diff <= magrin)
		{
			double dif_ratio = 1.0 - (double)diff / magrin;
			sum += dif_ratio*(c - c1);
			n += dif_ratio;
		}
	}
	assert(n > 0);
	double dc=sum/n;
	int ret= (int)(dc+ 0.5f);
	assert( ret< n2_e-n2_s);
	return ret;
#else	
	return pMax - c1;
#endif
}

template <>
int TRAITS_X_16S_T<TRAITS_X_16S_Dummy_Min>::Max(const VIS_BUF_T<VIS_INT16> &abs_grad_x, int n1, int n2_s, int n2_e, TRAITS_X_16S_Dummy_Min)
{
	const short *c1 = ptr(abs_grad_x, n1, n2_s);
	const short *c2 = ptr(abs_grad_x, n1, n2_e);
	const short* pMax = std::min_element(c1, c2);
	
	//for(const short *c =c1; c<c2; c++)
	//	TRACE("%d ",*c);
	//TRACE("\n");
#if 1	
	int magrin =5;	// gray level
	double sum=0.0f;
	double n=0.0f;
	for(const short *c =c1; c<c2; c++)
	{
		int diff = *c -*pMax;
		assert(diff>=0);

		if (diff <= magrin)
		{
			double dif_ratio = 1.0 - (double)diff / magrin;
			sum += dif_ratio*(c - c1);
			n += dif_ratio;
		}
	}
	assert(n > 0);
	double dc=sum/n;
	int ret= (int)(dc+ 0.5f);
	assert( ret< n2_e-n2_s);
	return ret;
#else	
	return pMax - c1;
#endif
}

struct TRAITS_X_16S :public TRAITS_X_16S_T<TRAITS_X_16S_Dummy_Max>
{};
struct TRAITS_X_16S_Min :public TRAITS_X_16S_T<TRAITS_X_16S_Dummy_Min>
{
};

bool MyOp_Greate(int a, int b)
{
	return a>b;
}

bool MyOp_Less(int a, int b)
{
	return a<b;
}
//thres_low: treshold to treat as different edges
//thres_high: edges mag must be larger than this value
template <class TRAITS>
int MyScanEdge(VIS_BUF_T<VIS_INT16> const &abs_grad_x, VIS_BUF_T<VIS_INT16> &abs_grad_x_thin, int thres_low, int thres_high, bool(*OP)(int, int))
{
	assert(abs_grad_x.sz.x == abs_grad_x_thin.sz.x);
	assert(abs_grad_x.sz.y == abs_grad_x_thin.sz.y);

	for (int y = 0; y < TRAITS::N1(abs_grad_x); y++)
	{

#define	STEP_FIND_NON_ZERO	0
#define	STEP_FIND_ZERO	1
#define	STEP_SUMMARY	2

		int step = STEP_FIND_NON_ZERO;
		int p0 = -1, p1 = -1;

		for (int j = 0; j < TRAITS::N2(abs_grad_x); j++)
		{
			short t = *TRAITS::ptr(abs_grad_x, y, j);
			switch (step)
			{
			case STEP_FIND_NON_ZERO:
				if (OP(t, thres_low))
				{
					p0 = j;
					p1 = -1;
					step = STEP_FIND_ZERO;

					//TRACE("STEP_FIND_NON_ZERO ok y %d x %d\n", y, j);
				}
				break;
			case STEP_FIND_ZERO:
				if (!OP(t, thres_low))
				{
					p1 = j;
					step = STEP_SUMMARY;
					//TRACE("STEP_FIND_ZERO ok y %d x %d\n", y, j);
				}
				break;
			}

			if (j + 1 == TRAITS::N2(abs_grad_x) && STEP_FIND_ZERO == step)
			{
				p1 = TRAITS::N2(abs_grad_x);
				step = STEP_SUMMARY;
			}

			if (step == STEP_SUMMARY)
			{
				assert(p0 >= 0 && p1 > p0);

				int maxIdx = TRAITS::Max(abs_grad_x, y, p0, p1) + p0;

				int t = *TRAITS::ptr(abs_grad_x, y, maxIdx);

				if (OP(t, thres_high))
					*TRAITS::ptr(abs_grad_x_thin, y, maxIdx) = t;

				TRACE("STEP_SUMMARY ok y %d p0 %d p1 %d\n", y, p0, p1);
				step = STEP_FIND_NON_ZERO;
			}
		}
	}

	return 0;
}

int MyCvtTo8bit(cv::Mat const &src, cv::Mat &des)
{
	des = cv::Mat::zeros(src.rows, src.cols, CV_8U);

	assert(src.type() == CV_16S);
	for (int y = 0; y<src.rows; y++)
	{
		for (int x = 0; x<src.cols; x++)
		{
			const short *p = src.ptr<short>(y, x);
			unsigned char *pd = des.ptr<unsigned char>(y, x);
			*pd = std::min(abs(*p), 255);
		}
	}
	return 0;
}

VIS_INT16 VIS_EdgeDetect(
	VIS_BUF_T<VIS_UINT8> const	&src_gray,
	VIS_BUF_T<VIS_INT16>		&abs_grad_x_thin,
	VIS_BUF_T<VIS_INT16>		&dir,	// between -180~180
	bool log
	)
{
	assert(src_gray.sz.x == abs_grad_x_thin.sz.x && src_gray.sz.y == abs_grad_x_thin.sz.y);
	assert(src_gray.sz.x == dir.sz.x && src_gray.sz.y == dir.sz.y);

	VIS_INT16 wSts = 0;
	STK_Push();

	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	cv::Mat mat_src_gray;
	cv::Mat mat_grad_x;
	cv::Mat mat_grad_y;
	wSts =OPCV_ConvBufRef(&src_gray, &mat_src_gray);
	VIS_CHK_STS;

	cv::Sobel(mat_src_gray, mat_grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
	Sobel(mat_src_gray, mat_grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);

	VIS_BUF_T<VIS_INT16>		grad_x;
	VIS_BUF_T<VIS_INT16>		grad_y;
	wSts = OPCV_ConvBufRef(&mat_grad_x, &grad_x);
	VIS_CHK_STS;
	wSts = OPCV_ConvBufRef(&mat_grad_y, &grad_y);
	VIS_CHK_STS;

	if (log)
	{
		cv::Mat abs_grad_x;
		
		std::function<int(short val, int x, int y, uchar *c1, uchar *c2, uchar *c3)> f =
			[&mat_src_gray](short val, int x, int y, uchar *c1, uchar *c2, uchar *c3){
			if (val>0) *c1 = (uchar)std::max((short)255, val);
			else if (val < 0) *c2 = (uchar)std::max((short)255, (short)(-val));
			*c3 = *mat_src_gray.ptr<uchar>(y, x);
			return 0; };

		MyMapShortImgToColor(mat_grad_x, f, abs_grad_x);

		cv::imwrite("abs_grad_x.bmp", abs_grad_x);
	}
	
	MyScanEdge<TRAITS_X_16S>(grad_x, abs_grad_x_thin, 50, 50, MyOp_Greate);
	MyScanEdge<TRAITS_X_16S_Min>(grad_x, abs_grad_x_thin, -50, -50, MyOp_Less);

	for (VIS_UINT16 y = 0; y<src_gray.sz.y; y++)
	{
		for (VIS_UINT16 x = 0; x<src_gray.sz.x; x++)
		{
#define	PTR(dir,y,x)	(dir.p +y*dir.step + x)
			*PTR(dir, y, x) = (VIS_INT16)(atan2(*PTR(grad_y, y, x), *PTR(grad_x, y, x)) * 180 / 3.14159);
		}
	}

	if (log)
	{
		cv::Mat mat_abs_grad_x_thin;
		
		wSts = OPCV_ConvBufRef(&abs_grad_x_thin, &mat_abs_grad_x_thin);
		VIS_CHK_STS;

		cv::Mat abs_grad_x_thin_save;
		//MyCvtTo8bit(abs_grad_x_thin,abs_grad_x_thin_save);

		std::function<int(short val, int x, int y, uchar *c1, uchar *c2, uchar *c3)> f =
			[&mat_src_gray](short val, int x, int y, uchar *c1, uchar *c2, uchar *c3){
			if (val>0) *c1 = std::max((short)255, val);
			else if (val < 0) *c2 = std::max((short)255, (short)(-val));
			*c3 = *mat_src_gray.ptr<uchar>(y, x);
			return 0; };

		MyMapShortImgToColor(mat_abs_grad_x_thin, f, abs_grad_x_thin_save);

		imwrite("Mi_thin.bmp", abs_grad_x_thin_save);

		cv::Mat mat_dir;
		wSts = OPCV_ConvBufRef(&dir, &mat_dir);
		VIS_CHK_STS; 
		
		cv::Mat dir_save;
		MyCvtTo8bit(mat_dir, dir_save);
		cv::imwrite("dir_save.bmp", dir_save);
	}
Exit:
	STK_Pop();
	return wSts;
}

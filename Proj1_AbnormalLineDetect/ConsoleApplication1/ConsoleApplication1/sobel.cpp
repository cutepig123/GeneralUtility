#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2\imgproc\types_c.h"
#include "opencv2\highgui\highgui_c.h"
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

using namespace cv;

struct TRAITS_X_16S
{
	static int N1(Mat const &abs_grad_x)
	{
		return abs_grad_x.rows;
	}

	static int N2(Mat const &abs_grad_x)
	{
		return abs_grad_x.cols;
	}

	static short* ptr(Mat &abs_grad_x, int n1, int n2)
	{
		return abs_grad_x.ptr<short>(n1,n2);
	}

	static const short* ptr(Mat const &abs_grad_x, int n1, int n2)
	{
		return abs_grad_x.ptr<short>(n1, n2);
	}

	static int Max(const Mat &abs_grad_x, int n1, int n2_s, int n2_e)
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
};

struct TRAITS_X_16S_Min
{
	static int N1(Mat const &abs_grad_x)
	{
		return abs_grad_x.rows;
	}

	static int N2(Mat const &abs_grad_x)
	{
		return abs_grad_x.cols;
	}

	static short* ptr(Mat &abs_grad_x, int n1, int n2)
	{
		return abs_grad_x.ptr<short>(n1,n2);
	}

	static const short* ptr(Mat const &abs_grad_x, int n1, int n2)
	{
		return abs_grad_x.ptr<short>(n1, n2);
	}

	static int Max(const Mat &abs_grad_x, int n1, int n2_s, int n2_e)
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
int MyScanEdge(Mat const &abs_grad_x, Mat &abs_grad_x_thin, int thres_low, int thres_high, bool(*OP)(int,int))
{
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
				if ( OP(t, thres_low))
				{
					p0 = j;
					p1 = -1;
					step = STEP_FIND_ZERO;

					//TRACE("STEP_FIND_NON_ZERO ok y %d x %d\n", y, j);
				}
				break;
			case STEP_FIND_ZERO:
				if ( !OP(t, thres_low))
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

				int maxIdx = TRAITS::Max(abs_grad_x,y, p0, p1)+p0;
				
				int t = *TRAITS::ptr(abs_grad_x, y, maxIdx);

				if ( OP(t, thres_high))
					*TRAITS::ptr(abs_grad_x_thin, y, maxIdx) = t;

				TRACE("STEP_SUMMARY ok y %d p0 %d p1 %d\n", y, p0, p1);
				step = STEP_FIND_NON_ZERO;
			}
		}
	}

	return 0;
}

int MyCvtTo8bit(Mat const &src, Mat &des)
{
	des= Mat::zeros(src.rows, src.cols, CV_8U);
	
	assert( src.type() ==CV_16S);
	for(int y=0; y<src.rows; y++)
	{
		for(int x=0; x<src.cols; x++)
		{
			const short *p = src.ptr<short>(y, x);
			unsigned char *pd = des.ptr<unsigned char>(y, x);
			*pd = std::min(abs(*p), 255);
		}
	}
	return 0;
}

int MyEdgeDetect(Mat const &src_gray, Mat &abs_grad_x_thin, Mat &grad_x, Mat &grad_y, Mat &dir,bool log)
{
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	/// Generate grad_x and grad_y
	//Mat abs_grad_x, abs_grad_y;

	/// Gradient X
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	//convertScaleAbs(grad_x, abs_grad_x);
	
	if(log)
	{
		Mat abs_grad_x;
		convertScaleAbs(grad_x, abs_grad_x);
		imwrite("abs_grad_x.bmp", abs_grad_x);
	}
	
	/// Gradient Y
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
	//convertScaleAbs(grad_y, abs_grad_y);

	/// Total Gradient (approximate)
	//addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

	// scan to thin x
	//Mat abs_grad_x_thin;

	abs_grad_x_thin = Mat::zeros(src_gray.rows, src_gray.cols, CV_16S);

	MyScanEdge<TRAITS_X_16S>(grad_x, abs_grad_x_thin,50,50,MyOp_Greate);
	MyScanEdge<TRAITS_X_16S_Min>(grad_x, abs_grad_x_thin,-50,-50,MyOp_Less);

	dir = Mat::zeros(src_gray.rows, src_gray.cols, CV_16S);
	
	for(int y=0; y<src_gray.rows; y++)
	{
		for(int x=0; x<src_gray.cols; x++)
		{
			*dir.ptr<short>(y,x) =atan2( *grad_y.ptr<short>(y,x), *grad_x.ptr<short>(y,x) ) *180/3.14159;
		}
	}
	
	//abs_grad_x_thin.convertTo(abs_grad_x_thin, CV_8S);
	if(log)
	{
		Mat abs_grad_x_thin_save;
		MyCvtTo8bit(abs_grad_x_thin,abs_grad_x_thin_save);
		imwrite("Mi_thin.bmp", abs_grad_x_thin_save);
		
		Mat dir_save;
		MyCvtTo8bit(dir,dir_save);
		imwrite("dir_save.bmp", dir_save);
	}
	return 0;
}

//sign: 1: down, -1:up
int MyTraceEdge(Mat const& abs_grad_x_thin, Mat const& dir, int y_start, int x_start, int label, int sign, 
	Mat &edge_label, std::vector<Point2d> &xy)
{
	int max_dx =3;
	int max_dy =8;
	int max_ddir =10;	//degree
	
	int x=x_start;
	int y=y_start;
	*edge_label.ptr<short>(y,x) =label;
	
	xy.reserve(abs_grad_x_thin.rows);

	bool bFound =true;
	for( ; y >0 && y<abs_grad_x_thin.rows-1 && bFound; )
	{
		TRACE("y %d x %d\n", y,x);
		
		// srch for a reachable point from near to far
		bFound =false;
		short t_grad =*abs_grad_x_thin.ptr<short>(y,x);
		assert(t_grad);
		int cur_dir =*dir.ptr<short>(y,x);
		
		for(int dy= 1; dy <=max_dy; dy++)
		{
			int next_y = y+ sign*dy;
			
			if( next_y >0 && next_y<abs_grad_x_thin.rows-1 )
			{
				for(int dx= 0; dx<=max_dx; dx++)
				{
					int next_x =x +dx;
					if( next_x>=0 && next_x<abs_grad_x_thin.cols-1)
					{
						short t_dir =*dir.ptr<short>(next_y,next_x);
						short t_grad =*abs_grad_x_thin.ptr<short>(next_y,next_x);
						if (t_grad && abs(cur_dir - t_dir) <= max_ddir)
						{
							x =next_x;
							y =next_y;
							bFound =true;
							goto Next;
						}
					}
					
					next_x =x -dx;
					if( next_x>=0 && next_x<abs_grad_x_thin.cols-1)
					{
						short t_dir =*dir.ptr<short>(next_y,next_x);
						short t_grad =*abs_grad_x_thin.ptr<short>(next_y,next_x);
						if(t_grad && abs(cur_dir-t_dir))
						{
							x =next_x;
							y =next_y;
							bFound =true;
							goto Next;
						}
					}
				}
			}
		}
		
		//
	Next:
		if(bFound)
		{
			*edge_label.ptr<short>(y,x) =label;
			xy.push_back(Point2d(x, y));
		}
	}

	return 0;
}

//x = a *y + b
//
//->
//
//[y1 1] *  [a]		[x1]
//			[b] =
struct VertLine
{
	double a, b;
};

int MyFitVertLine(std::vector<Point2d> const &xy, VertLine &line)
{
	assert(!xy.empty());

	Mat A, b;

	A.create(xy.size(), 2, CV_64F);
	b.create(xy.size(), 1, CV_64F);
	for (size_t i = 0, n = xy.size(); i < n; i++)
	{
		// A
		*A.ptr<double>(i, 0) = xy[i].y;
		*A.ptr<double>(i, 1) = 1;

		// B
		*b.ptr<double>(i, 0) = xy[i].x;
	}

	Mat x;
	solve(A, b, x, DECOMP_NORMAL);

	line.a = *x.ptr<double>(0, 0);
	line.b = *x.ptr<double>(1, 0);

	//Mat A_t;
	//transpose(A, A_t);
	//Mat r = (A_t*A).inv()*(A_t*b);
	double x1=line.a *80 +line.b;
	double x2=line.a *480 +line.b;
	
	return 0;
}

int MySortEdgeByDir(Mat const& abs_grad_x_thin,Mat const& dir, Mat &edge_label,bool log )
{
	int y_start =abs_grad_x_thin.rows/2;
	
	// gen seeds
	edge_label = Mat::zeros(abs_grad_x_thin.rows, abs_grad_x_thin.cols, CV_16S);
	int nlabel =0;
	//std::vector<MySeed> vSeeds;
	//vSeeds.reserve(src_gray.cols);
	for (int x = 0; x<abs_grad_x_thin.cols; x++)
	{
		short t =*abs_grad_x_thin.ptr<short>(y_start,x);
		if(t)
		{
			/*MySeed seed;
			seed.x =x;
			seed.y =y_start;
			seed.label =nlabel+1;
			vSeeds.push_back(seed);*/
			nlabel++;

			std::vector<Point2d> xy;
			MyTraceEdge(abs_grad_x_thin, dir, y_start, x, nlabel, 1, edge_label, xy);
			MyTraceEdge(abs_grad_x_thin, dir, y_start, x, nlabel, -1, edge_label, xy);

			VertLine line;
			MyFitVertLine( xy, line);
			printf("line %d, ab %f %f\n", nlabel, line.a, line.b);
		}
	}
	
	if(log)
	{
		Mat edge_label_save;
		MyCvtTo8bit(edge_label,edge_label_save);
		imwrite("edge_label_save.bmp", edge_label_save);
	}
	
	return 0;
}

/** @function main */
int main_sobel(int argc, char** argv)
{
	Mat src, src_gray,grad_x,grad_y,dir, edge_label;
	Mat abs_grad_x_thin;
	char* window_name = "Sobel Demo - Simple Edge Detector";
	//int scale = 1;
	//int delta = 0;
	//int ddepth = CV_16S;
	bool log =true;
//	int c;

	/// Load an image
	src = imread("1.bmp");

	if (!src.data)
	{
		return -1;
	}

	//GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);

	/// Convert it to gray
	cvtColor(src, src_gray, CV_RGB2GRAY);

	/// Create window
	//namedWindow(window_name, CV_WINDOW_AUTOSIZE);

	MyEdgeDetect(src_gray, abs_grad_x_thin, grad_x, grad_y, dir, log);
	
	MySortEdgeByDir(abs_grad_x_thin,dir, edge_label, log );
	///// Generate grad_x and grad_y
	//Mat grad_x, grad_y;
	//Mat abs_grad_x, abs_grad_y;

	///// Gradient X
	////Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	//Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	//convertScaleAbs(grad_x, abs_grad_x);

	///// Gradient Y
	////Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	//Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
	//convertScaleAbs(grad_y, abs_grad_y);

	///// Total Gradient (approximate)
	//addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

	//imshow(window_name, grad);

	//imwrite("out.bmp", grad);

	//waitKey(0);

	return 0;
}
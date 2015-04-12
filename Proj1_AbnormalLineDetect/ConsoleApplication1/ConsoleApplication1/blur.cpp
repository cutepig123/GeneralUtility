// Refer to http://docs.opencv.org/doc/tutorials/imgproc/table_of_content_imgproc/table_of_content_imgproc.html#table-of-content-imgproc

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

/// Global Variables
int DELAY_CAPTION = 1500;
int DELAY_BLUR = 100;
int MAX_KERNEL_LENGTH = 31;

Mat src; Mat dst;
char window_name[] = "Filter Demo 1";

/// Function headers
int display_caption(char* caption);
int display_dst(int delay);

/**
* function main
*/
int main_blur(int argc, char** argv)
{
	system("pause");

	namedWindow(window_name);

	/// Load the source image
	src = imread("1.bmp", 1);

	if (display_caption("Original Image") != 0) { return 0; }

	dst = src.clone();
	if (display_dst(DELAY_CAPTION) != 0) { return 0; }

	/// Applying Homogeneous blur
	if (display_caption("Homogeneous Blur") != 0) { return 0; }

	for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
	{
		blur(src, dst, Size(i, i), Point(-1, -1));
		if (display_dst(DELAY_BLUR) != 0) { return 0; }
	}

	/// Applying Gaussian blur
	if (display_caption("Gaussian Blur") != 0) { return 0; }

	for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
	{
		GaussianBlur(src, dst, Size(i, i), 0, 0);
		if (display_dst(DELAY_BLUR) != 0) { return 0; }
	}

	/// Applying Median blur
	if (display_caption("Median Blur") != 0) { return 0; }

	for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
	{
		medianBlur(src, dst, i);
		if (display_dst(DELAY_BLUR) != 0) { return 0; }
	}

	/// Applying Bilateral Filter
	if (display_caption("Bilateral Blur") != 0) { return 0; }

	for (int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2)
	{
		bilateralFilter(src, dst, i, i * 2, i / 2);
		if (display_dst(DELAY_BLUR) != 0) { return 0; }
	}

	/// Wait until user press a key
	display_caption("End: Press a key!");

	waitKey(0);
	return 0;
}

#define CV_FONT_HERSHEY_COMPLEX 1

int display_caption(char* caption)
{
	dst = Mat::zeros(src.size(), src.type());
	putText(dst, caption,
		Point(src.cols / 4, src.rows / 2),
		CV_FONT_HERSHEY_COMPLEX, 1, Scalar(255, 255, 255));

	imshow(window_name, dst);
	int c = waitKey(DELAY_CAPTION);
	if (c >= 0) { return -1; }
	return 0;
}

int display_dst(int delay)
{
	imshow(window_name, dst);
	int c = waitKey(delay);
	if (c >= 0) { return -1; }
	return 0;
}
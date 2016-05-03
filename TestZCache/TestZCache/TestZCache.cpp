// TestZCache.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <assert.h>

template <class T>
struct ImageT
{
	int w, h, step;
	T *p;

	T *get(int y, int x)
	{
		assert(y >= 0 && y < h);
		assert(x >= 0 && x < w);
		assert(w <= step);
		return &p[step*y + x];
	}

	const T *get(int y, int x) const
	{
		assert(y >= 0 && y < h);
		assert(x >= 0 && x < w);
		assert(w <= step);
		return &p[step*y + x];
	}
};

typedef ImageT<unsigned char>	Image;
typedef ImageT<double>	ImageDouble;

int LogImage(const char *file, Image const *image)
{
	FILE *fp = fopen(file, "w");
	fprintf(fp, "P2\n");
	fprintf(fp, "%d %d\n", image->w, image->h);
	fprintf(fp, "255\n");
	for (int y = 0; y < image->h; y++)
	{
		unsigned char const *p = image->get(y, 0);
		for (int x = 0; x < image->w; x++)
		{
			fprintf(fp, "%d ", p[x]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	return 0;
}

struct Ball
{
	double x0, y0, z0, r;
};

int Draw(int n, Ball const *balls, unsigned char const *colors, Image *image, ImageDouble *zimage)
{
	for (int i = 0; i < n; i++)
	{
		Ball const &ball = balls[i];
		unsigned char color = colors[i];

		for (double theta = 0; theta <= 3.14; theta +=0.01)
		{
			double ss = sin(theta);
			double cs = cos(theta);
			for (double fai = 0; fai <= 2*3.14; fai += 0.01)
			{
				double x = ball.x0 + ball.r * ss * cos(fai);
				double y = ball.y0 + ball.r * ss * sin(fai);
				double z = ball.z0 + ball.r * cs;
				assert(x >= 0 && x < image->w);
				assert(y >= 0 && y < image->h);
				if (x >= 0 && x<image->w && y >= 0 && y<image->h && z< *zimage->get(y, x))
				{
					*zimage->get(y, x) = z;
					*image->get(y, x) = color;
					
				}
			}
		}
	}
	return 0;
}
int _tmain(int argc, _TCHAR* argv[])
{
#define	N 200
	Ball balls[] = {
		{ N / 3, N / 3, N / 3, N / 4 },
		{ N / 2, N / 2, N, N / 3 },
	};
	unsigned char colors[] = {100, 150};
	std::vector<unsigned char> vBuf(N*N, 0);
	std::vector<double> vZ(N*N, 1.0e20);
	Image image = { N, N, N, &vBuf[0] };
	ImageDouble zimage = { N, N, N, &vZ[0] };
	Draw(2, balls, colors, &image, &zimage);
	LogImage("1.pgm", &image);

	return 0;
}


/*
盒子里的气球
*/

#include <stdio.h>
#include <algorithm>
#include <assert.h>

struct Point
{
	float x, y;
};

struct Window
{
	Point coUL, coLR;
};

#define	MAX_BALL	6
struct Input
{
	size_t	N;	// Num ball
	Point	aCentre[MAX_BALL];
	Window	stWin;
};

// 到盒子边缘的距离
float DistToWin(Point const &pt, Window const &stWin)
{
	assert(pt.x >= stWin.coUL.x && pt.y >= stWin.coUL.y);
	assert(pt.x <= stWin.coLR.x && pt.y <= stWin.coLR.y);
	float rMin1 = std::min(pt.x - stWin.coUL.x, pt.y - stWin.coUL.y);
	float rMin2 = std::min(stWin.coLR.x - pt.x, stWin.coLR.y - pt.y);
	return std::min(rMin1, rMin2);
}

float DistOf2Pt(Point const &l, Point const &r)
{
	float dx = (l.x - r.x);
	float dy = (l.y - r.y);
	return sqrtf(dx*dx + dy*dy);
}

//计算放第i个ball的最大半径
float CalcR(Input const &in, size_t i, 
			float		const	aR[]	//size ==i
			)
{
	float minR = DistToWin(in.aCentre[i], in.stWin);
	
	for (size_t j = 0; j < in.N; j++)
	{
		if (j == i) continue;

		float R = DistOf2Pt(in.aCentre[i], in.aCentre[j]);
		if (j < i)
			R -= aR[j];
		minR = std::min(R, minR);
	}
	return minR;
}

#define	PI 1

// 返回分数
float Solve(Input	const	&in, 
			size_t			i,
			size_t			aStep[MAX_BALL],		// put ball procedure
			float			aR[MAX_BALL],			// 半径
			float			rArea
			)
{
	if (i >= in.N)
		return rArea;

	char aSelected[MAX_BALL];
	size_t aPossibleSel[MAX_BALL];
	size_t nPossSel = 0;
	// select all possible selections
	memset(aSelected, 0, sizeof(aSelected));
	for (size_t k = 0; k < i; k++)
	{
		assert(aStep[k] < MAX_BALL);
		aSelected[aStep[k]] = 1;
	}
	for (size_t k = 0; k < in.N; k++)
	{
		if (!aSelected[k])
			aPossibleSel[nPossSel++] = k;
	}

	// test all
	float rMax = 0.0f;
	size_t maxIdx = 0;
	float  maxR = 0;

	for (size_t k = 0; k < nPossSel; k++)
	{
		aStep[i]= aPossibleSel[k];
		aR[i] = CalcR(in, i, aR);

		float rAreak = Solve(in, i + 1, aStep, aR, rArea + PI*aR[i] * aR[i]);

		if (rMax < rAreak)
		{
			rMax = rAreak;
			maxIdx = k;
			maxR = aR[i];
		}
	}

	aStep[i] = aPossibleSel[maxIdx];
	aR[i] = maxR;

	return rMax;
}

int main()
{
	Input in;

	size_t			aStep[MAX_BALL];		// put ball procedure
	float			aR[MAX_BALL];			// 半径
	memset(aStep, 0, sizeof(aStep));
	memset(aR, 0, sizeof(aR));

	in.stWin.coUL.x = in.stWin.coUL.y = 0;
	in.stWin.coLR.x = in.stWin.coLR.y = 1;
	in.N = 2;
	in.aCentre[0].x = in.aCentre[0].y = 0.5;
	in.aCentre[1].x = in.aCentre[1].y = 0.95;

	float rArea = Solve(in, 0, aStep, aR, 0);

}

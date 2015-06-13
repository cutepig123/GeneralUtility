#pragma once

#include <vector>
#define	CHK_ERR if((wSts)<0) goto Exit

struct Size
{
	int x,y;
};

bool operator==(Size const &l, Size const &r)
{
	return l.x==r.x && l.y==r.y;
}

struct Matrix
{
	double *ptr;
	Size sz;
	int step;
};

#define GETELEM(m,y,x) (m).ptr[(y)*(m).step+(x)]


#include "StdAfx.h"
#include"common.h"
U2 byte2Word(const U1 byte[])
{
	U2 word;
	U1* p = (U1*)&word;
	p[0] = byte[1];
	p[1] = byte[0];
	return word;
}
U1 bool2Data(bool f)
{
	if(f)
		return 1;
	else
		return 0;
}
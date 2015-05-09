// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vis/stack.h>
#include <vis/IO.h>

int _tmain(int argc, _TCHAR* argv[])
{
	VIS_INT16 wSts = 0;
	STK_Push();
	VIS_SIZE size = { 10, 20 };
	//VIS_BUF	ubb;
	//STK_Malloc(size, &ubb);
	VIS_MAT_BUF_T<VIS_UINT8>	ubbS;
	wSts =VIS_LoadImage("1.bmp", &ubbS);
	VIS_CHK_STS;
Exit:

	STK_Pop();
	return 0;
}


#include <vis/stack.h>
#include <vis/IO.h>
#include <gtest/gtest.h>

TEST (LoadImage, OK)
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
}

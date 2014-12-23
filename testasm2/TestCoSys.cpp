#include <apt_ds.h>

void TestCosys()
{
	DS_COSYS	stCosys0, stCosys100;

	memset( &stCosys0, 0, sizeof(stCosys0));
	memset( &stCosys100, 0, sizeof(stCosys100));
	stCosys100.rcoOrigin.x =stCosys100.rcoOrigin.y =100;

	IMG_RCOORD arcoPts[NO_OF_CORNERS], arcoPtd[NO_OF_CORNERS];
	arcoPts[0].x =arcoPts[0].y =0;

	DS_TRANS_PAR stTransPar;
	memset( &stTransPar, 0, sizeof(stTransPar));
	DS_SetTransPar(&stCosys0, &stCosys100, &stTransPar);	
	DS_TransformNPt( arcoPts, &stTransPar, NO_OF_CORNERS, arcoPtd );
}

void TestCreateDIr()
{
	IMG_WORD wSts =DOS_CreateDirectoryEx("c:\\temp1");
	printf("%d\n", wSts);
	wSts =DOS_CreateDirectoryEx("c:\\temp1");
	printf("%d\n", wSts);
}


void TestCopDIr()
{
	DOS_RemoveDirectory("c:\\temp1");
	IMG_WORD wSts =DOS_CopyDirectory("c:\\temp\\**", "c:\\temp1");
	printf("%d\n", wSts);
	assert(0==wSts);
}

#include <apt_ds.h>
#include <algorithm>

bool g_isDebug=false;

const char* getOpt(int argc, char **argv, const char *opt)
{
	for(int i=1; i<argc;i++)
	{
		if( stricmp(argv[i], opt)==0 && i+1<argc)
			return argv[i+1];
	}
	return 0;
}

bool hasOpt(int argc, char **argv, const char *opt)
{
	for(int i=1; i<argc;i++)
	{
		if( stricmp(argv[i], opt)==0)
			return true;
	}
	return false;
}

void	InitSystem()
{
	SYS_InitModule();
	IO_InitModule(0);
	//FM_InitModule(20);
	//FM_SetMaxNumOfFile(10,10);
	MEM_OpenHeap();
}

void	FreeSystem()
{
	MEM_CloseHeap();
	//FM_FreeModule();
	IO_FreeModule();
	SYS_FreeModule();
}

IMG_WORD	LoadBmp_MEM(IMG_CHAR const *pcFile, IMG_UBBUF	&ubbS)
{
	IMG_COORD	coZ ={0,0};
	IMG_SIZE	szOp;
	IMG_WORD	wStatus =OK;
	
	wStatus =FILE_ImageSize( (IMG_UBYTE *)pcFile, &szOp );
	if (DSP_ERROR_STATUS(wStatus)) goto Exit;
	
	wStatus = MEM_AllocBufAtBank( &szOp, SYS_BANK1_8, (IMG_VVBUF*)&ubbS );
	if ( DSP_ERROR_STATUS(wStatus) ) goto Exit;	

	wStatus = FILE_LoadImage((IMG_UBYTE *)pcFile, &coZ, &szOp, &ubbS );
	if (DSP_ERROR_STATUS(wStatus)) goto Exit;
Exit:
	return wStatus;
}

IMG_WORD Test1();
IMG_WORD Test2();
void TestSpeed1(int *pid);
IMG_WORD TestMask_Opt();
void TestCosys();
void TestCopDIr();
void TestPU();

int  main(int argc, char **argv)
{
	InitSystem();

	//if( hasOpt(argc, argv, "-d") )
	//	g_isDebug =true;

	//const char *cid =getOpt(argc, argv, "-id");
	//int id = cid ? atoi(cid) : -1;

	//TestSpeed1( id>=0 ? &id : (int*)0 );
	//TestMask_Opt();
	//TestCosys();
	//TestCopDIr();
	TestPU();

	FreeSystem();
}

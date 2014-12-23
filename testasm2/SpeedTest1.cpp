#include <apt_ds.h>
#include <archive.h>
#include <archive_entry.h>
#include "inc.h"

#define	ASSERT(x)		if(!(x)) {printf("Error " #x "\n");assert(0);}

IMG_WORD	LoadBmp_MEM(IMG_CHAR const *pcFile, IMG_UBBUF	&ubbS);
IMG_WORD	FILE_SaveBmpToMem(IMG_UBBUF const *pubbSbuf, IMG_CHAR **ppcBuf, IMG_ULWORD *pulN);

int myarch_set_entry(const char *filename, size_t  len, struct archive_entry *entry);

#define	N	2000
int TestSaveManySmallFiles(IMG_UBBUF const *pubbS)
{
	int			wSts =0;
	IMG_COORD	coZ ={0,0};
	IMG_CHAR	acFile[100];

	for(IMG_ULWORD i=0; i<N; i++)
	{
		sprintf(acFile,"temp\\%d.bmp", i);
		wSts =FILE_SaveImage( pubbS, &coZ, &pubbS->size, (IMG_UBYTE*)acFile);
		ASSERT(wSts>=0);
	}
	return 0;
}

int TestSaveManySmallFiles2Des(IMG_UBBUF const *pubbS, const char *des[2])
{
	int			wSts =0;
	IMG_COORD	coZ ={0,0};
	IMG_CHAR	acFile[100];

	for(IMG_ULWORD i=0; i<N; i++)
	{
		sprintf(acFile,"%s\\temp\\%d.bmp", i%2==0?des[0]:des[1], i);
		wSts =FILE_SaveImage( pubbS, &coZ, &pubbS->size, (IMG_UBYTE*)acFile);
		ASSERT(wSts>=0);
	}
	return 0;
}


#define	MAX_THREAD	4
struct TestSaveManySmallFilesMTPar{
	IMG_UBBUF const *pubbS;
	int beginID;
	int endID;
	HANDLE hThread;
	HANDLE hEvent;
	HANDLE hEventDone;
	DWORD dwTID;
	const char *dst;
}g_mtPar[MAX_THREAD];

void Trace(const char *fmt,...)
{
	if(g_isDebug)
	{
		va_list l;
		va_start(l, fmt);
		char s[500];
		//vsprintf(s,fmt, l);
		vprintf(fmt, l);
		va_end(l);
	}
}

//#define Trace printf
//#define Trace

DWORD WINAPI TestSaveManySmallFilesMTCallback(void *pvPar)
{
	TestSaveManySmallFilesMTPar *pPar =(TestSaveManySmallFilesMTPar*)pvPar;

	while(1)
	{
		ASSERT( WAIT_OBJECT_0==WaitForSingleObject( pPar->hEvent, INFINITE) );

		Trace("Thread %d start (%d->%d)...\n", pPar->dwTID, pPar->beginID, pPar->endID);

		int			wSts =0;
		IMG_COORD	coZ ={0,0};
		IMG_CHAR	acFile[100];

		for(IMG_ULWORD i=pPar->beginID; i<pPar->endID; i++)
		{
			if(pPar->dst)
				sprintf(acFile,"%s\\temp\\%d.bmp", pPar->dst, i);
			else
				sprintf(acFile,"temp\\%d.bmp", i);
			Trace("Thread %d start des %s...\n", pPar->dwTID, acFile);
			wSts =FILE_SaveImage( pPar->pubbS, &coZ, &pPar->pubbS->size, (IMG_UBYTE*)acFile);
			ASSERT(wSts>=0);
		}

		ASSERT(1==SetEvent(pPar->hEventDone));
		Trace("Thread %d end (%d->%d)...\n", pPar->dwTID, pPar->beginID, pPar->endID);
	}
	return 0;
}

int TestSaveManySmallFilesMT_Init()
{
	for(int i=0;i <MAX_THREAD; i++)
	{
		memset( g_mtPar+i, 0, sizeof(g_mtPar[i]));
		
		g_mtPar[i].hEvent = CreateEvent(0,FALSE,FALSE,0);
		g_mtPar[i].hEventDone = CreateEvent(0,FALSE,FALSE,0);
		g_mtPar[i].hThread = CreateThread(0,0,TestSaveManySmallFilesMTCallback, g_mtPar+i,
			0, &g_mtPar[i].dwTID);
		
	}
	return OK;
}


int TestSaveManySmallFilesMT(IMG_UBBUF const *pubbS, int nThread, const char **des=0)
{
	int step =N/nThread;
	ASSERT( nThread*step ==N );

	for(int i=0; i<nThread; i++)
	{
		g_mtPar[i].pubbS =pubbS;
		g_mtPar[i].beginID =step*i;
		g_mtPar[i].endID =step*(1+i);
		if(des)
			g_mtPar[i].dst =des[i];
		ASSERT(TRUE==ResetEvent( g_mtPar[i].hEventDone));
		ASSERT(TRUE==SetEvent( g_mtPar[i].hEvent));
	}

	for(int i=0; i<nThread; i++)
	{
		ASSERT( WAIT_OBJECT_0==WaitForSingleObject( g_mtPar[i].hEventDone, INFINITE));
	}

	return 0;
}

int TestSaveToOneLargeFile(IMG_UBBUF const *pubbS, const char *des="temp2\\outname.zip")
{
	int			wSts =0;
	struct archive *a;
	struct archive_entry *entry;
	IMG_CHAR	acFile[100];

	a = archive_write_new();
	ASSERT(a);
	//ASSERT( 0==archive_write_set_compression_gzip(a));
	ASSERT( 0==archive_write_set_format_zip(a));
	
	ASSERT( 0==archive_write_open_filename(a, des));
	entry = archive_entry_new(); // Note 2
	for(IMG_ULWORD i=0; i<N; i++)
	{
		MEM_PushAllHeapStatus();

		IMG_CHAR	*pcData =NULL;
		IMG_ULWORD	ulN =0;
		wSts =FILE_SaveBmpToMem( pubbS, &pcData, &ulN);
		ASSERT(wSts==0);

		sprintf(acFile,"temp/%d.bmp", i);
		
		ASSERT( 0==myarch_set_entry( acFile, ulN, entry));
		ASSERT( 0==archive_write_header(a, entry));
		ASSERT( ulN==archive_write_data(a, pcData, ulN));

		MEM_PopAllHeapStatus();
	}
	archive_entry_free(entry);
	archive_write_close(a); // Note 4
	archive_write_finish(a); // Note 5

Exit:
	
	return 0;
}

int TestSaveToOneLargeFileAndCopyTocTemp(IMG_UBBUF const *pubbS)
{
	const char *des1="temp2\\outname.zip";
	ASSERT(0==TestSaveToOneLargeFile(pubbS, des1));
	ASSERT(0==DOS_CopyFile(des1, "c:\\temp\\temp.zip",1));
Exit:
	
	return 0;
}

struct SpeedTest
{
	IMG_ULWORD ulClick1;
	IMG_ULWORD ulClick2;
	
	explicit SpeedTest(const char *pc)
	{
		printf("%40s...",pc);
		SYS_GetTimeInUS(&ulClick1);
	}

	~SpeedTest()
	{
		SYS_GetTimeInUS(&ulClick2);
		printf(" %10lu us\n",ulClick2-ulClick1);
	}
};

void TestSpeed1(int *pid)
{
	MEM_PushAllHeapStatus();
	IMG_UBBUF	ubbS;
	int wSts =LoadBmp_MEM("1.bmp", ubbS);
	ASSERT(0==wSts);

	if(!pid || *pid==1)
	{
		SpeedTest sp1("TestSaveToOneLargeFile");
		TestSaveToOneLargeFile(&ubbS);
	}

	if(!pid || *pid==2)
	{
		SpeedTest sp1("TestSaveManySmallFiles");
		TestSaveManySmallFiles(&ubbS);
	}

	TestSaveManySmallFilesMT_Init();

	if(!pid || *pid==3)
	{
		SpeedTest sp1("TestSaveManySmallFilesMT 2Thread");;
		TestSaveManySmallFilesMT(&ubbS, 2);
	}

	if(!pid || *pid==4)
	{
		SpeedTest sp1("TestSaveManySmallFilesMT 4Thread");;
		TestSaveManySmallFilesMT(&ubbS, 4);
	}

	if(!pid || *pid==5)
	{
		SpeedTest sp1("TestSaveToOneLargeFileAndCopyToC:\\Temp");
		TestSaveToOneLargeFileAndCopyTocTemp(&ubbS);
	}
	
	if(!pid || *pid==6)
	{
		const char *pdes[]={"c:","r:"};
		SpeedTest sp1("TestSaveManySmallFilesMT 2Thread DiffDes");
		TestSaveManySmallFilesMT(&ubbS, 2, pdes);
	}

	if(!pid || *pid==7)
	{
		const char *pdes[]={"c:","r:"};
		SpeedTest sp1("TestSaveManySmallFiles2Des 1Thread DiffDes");
		TestSaveManySmallFiles2Des(&ubbS, pdes);
	}
	
	
Exit:
	MEM_PopAllHeapStatus();
}

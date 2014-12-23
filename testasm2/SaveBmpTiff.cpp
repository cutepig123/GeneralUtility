#include <apt_ds.h>

IMG_WORD	LoadBmp_MEM(IMG_CHAR const *pcFile, IMG_UBBUF	&ubbS);

struct	SAVE_FILE_TRAITS
{
	typedef	::FILE	FILE;

	static FILE *open(const char* file, const char *mode)
	{
		return ::fopen(file, mode);
	}

	static size_t write(const void *buf, size_t sz, size_t n, FILE *fp)
	{
		return ::fwrite(buf, sz, n, fp );
	}

	static bool error( FILE *fp)
	{
		return !!ferror(fp);
	}

	static int close(FILE *fp)
	{
		return fclose(fp);
	}
};

template <class SAVE_FILE_TRAITS>
IMG_WORD	MIL_SaveToBmpFileEx(IMG_UBBUF	const	*pubbSbuf,
								IMG_COORD	const	*pcoOffset,
								IMG_SIZE	const	*pszOpSize,
								IMG_WORD	const	wIsRawColor,
								IMG_REAL	const	*prWbCoeff,
								IMG_UBYTE	const	*pubBayerPattern,
								IMG_CHAR	const	*pubFileName,
								SAVE_FILE_TRAITS	*pstFile)
{
	SAVE_FILE_TRAITS::FILE	*pFilePtr = NULL;
	IMG_LWORD	lwData, i;
	IMG_WORD	wData, wStatus = OK;
	IMG_UBYTE	ubData;
	IMG_SIZE	szOpSize;

	if ( pcoOffset->x + pszOpSize->x > pubbSbuf->size.x ||
		 pcoOffset->y + pszOpSize->y > pubbSbuf->size.y )
		return ERR_INVALID_ARG;

	pFilePtr = pstFile->open(pubFileName, "wb");
	if ( pFilePtr == NULL )
		return FILE_ERR_OPENFILE;

	wStatus = MEM_PushAllHeapStatus();
	if ( wStatus != OK ) return wStatus;

	szOpSize = *pszOpSize;

	if ( pszOpSize->x % 4 != 0 )
	{
		szOpSize.x = szOpSize.x + (4 - pszOpSize->x % 4);
	}

//	header
	ubData = 'B';
	pstFile->write(&ubData, 1, 1, pFilePtr);
	ubData = 'M';
	pstFile->write(&ubData, 1, 1, pFilePtr);
	lwData = szOpSize.x * szOpSize.y + 1078;
	pstFile->write(&lwData, 4, 1, pFilePtr);
	wData = 0;
	if ( wIsRawColor == IMG_TRUE )
	{
		if ( prWbCoeff == NULL && pubBayerPattern == NULL )
			ubData = 1;
		else if ( prWbCoeff != NULL && pubBayerPattern == NULL )
			ubData = 3;
		else if ( prWbCoeff == NULL && pubBayerPattern != NULL )
			ubData = 5;
		else 
			ubData = 7;

		pstFile->write(&ubData, 1, 1, pFilePtr);
		ubData = 0;
		pstFile->write(&ubData, 1, 1, pFilePtr);
		pstFile->write(&wData, 2, 1, pFilePtr);
	}
	else
	{
		pstFile->write(&wData, 2, 1, pFilePtr);
		pstFile->write(&wData, 2, 1, pFilePtr);
	}
	lwData = 1078;
	pstFile->write(&lwData, 4, 1, pFilePtr);

//	Bitmap header
	lwData = 40;
	pstFile->write(&lwData, 4, 1, pFilePtr);
	lwData = pszOpSize->x;
	pstFile->write(&lwData, 4, 1, pFilePtr);
	lwData = pszOpSize->y;
	pstFile->write(&lwData, 4, 1, pFilePtr);

	wData = 1;
	pstFile->write(&wData, 2, 1, pFilePtr);
	wData = 8;
	pstFile->write(&wData, 2, 1, pFilePtr);
	lwData = 0;
	pstFile->write(&lwData, 4, 1, pFilePtr);

	lwData = szOpSize.x * szOpSize.y;
	pstFile->write(&lwData, 4, 1, pFilePtr);
	lwData = 0;
	pstFile->write(&lwData, 4, 1, pFilePtr);
	pstFile->write(&lwData, 4, 1, pFilePtr);
	pstFile->write(&lwData, 4, 1, pFilePtr);
	pstFile->write(&lwData, 4, 1, pFilePtr);


//	header
	for ( i = 0 ; i < 256 ; i++ )
	{
		lwData = i + (i << 8) + ( i << 16 );
		pstFile->write(&lwData, 4, 1, pFilePtr);
	}

//	Data
	//ubData = 0;

	{
		IMG_UBBUF	ubbNewSbuf;
		IMG_COORD	coOffset = {0,0};

		ubbNewSbuf.ptr = (IMG_UBYTE*)MEM_Malloc(szOpSize.x * szOpSize.y, SYS_BANK1_8);
		if (ubbNewSbuf.ptr == NULL)
		{
			wStatus = MEM_ERR_MALLOC;
			goto Exit;
		}

		ubbNewSbuf.size = *pszOpSize;
		ubbNewSbuf.linestep = szOpSize.x;

		wStatus = DSP_VerticalFlip(pubbSbuf,pcoOffset,&ubbNewSbuf,&coOffset,pszOpSize);
		if ( wStatus != OK )
			goto Exit;

		pstFile->write(ubbNewSbuf.ptr, 1, szOpSize.x * szOpSize.y, pFilePtr);
	}
	
	if ( prWbCoeff != NULL )
		for ( i = 0 ; i < 3 ; i++ )
			pstFile->write(&prWbCoeff[i], 4, 1, pFilePtr);

	if(pubBayerPattern)
	pstFile->write(pubBayerPattern, 1, 1, pFilePtr);	
	

Exit:
	pstFile->close(pFilePtr);
	MEM_PopAllHeapStatus();

	return wStatus;
}

struct	MEM_FILE
{
	struct MEM_FILE_TYPE
	{
		IMG_CHAR	*p;
		IMG_ULWORD	ulSz;
		IMG_ULWORD	ulCapacity;

		MEM_FILE_TYPE(){
			memset(this, 0, sizeof(*this));
		}
	};
	typedef	MEM_FILE_TYPE	FILE;

	MEM_FILE_TYPE	m_file;

	FILE *open(const char* file, const char *mode)
	{
		m_file.ulSz =0;
		return &m_file;
	}

	size_t write(const void *buf, size_t sz, size_t n, FILE *fp)
	{
		if( fp->p )
		{
			assert( fp->ulSz + sz*n <= fp->ulCapacity );
			memcpy( fp->p + fp->ulSz, buf, sz*n );
		}
		
		fp->ulSz += sz*n;
		return n;
	}

	bool error( FILE *fp)
	{
		return false;
	}

	int close(FILE *fp)
	{
		return 0;
	}
};

IMG_WORD	FILE_SaveBmpToMem(IMG_UBBUF const *pubbSbuf, IMG_CHAR **ppcBuf, IMG_ULWORD *pulN)
{
	MEM_FILE	memFile;
	IMG_COORD	coZ ={0,0};
	IMG_WORD	wSts =OK;

	// Query buffer size
	wSts =MIL_SaveToBmpFileEx( pubbSbuf, &coZ, &pubbSbuf->size,IMG_TRUE,NULL,NULL,"pcFile",&memFile);
	if( DSP_ERROR_STATUS(wSts)) goto Exit;

	IMG_ULWORD ulN =memFile.m_file.ulSz;
	memFile.m_file.ulCapacity =ulN;
	*ppcBuf =memFile.m_file.p = (IMG_CHAR*)MEM_Malloc(ulN, SYS_BANK1_8);
	if(!memFile.m_file.p)
	{
		wSts =MEM_ERR_MALLOC; goto Exit;
	}
	// Save to MEM
	wSts =MIL_SaveToBmpFileEx( pubbSbuf, &coZ, &pubbSbuf->size,IMG_TRUE,NULL,NULL,"pcFile",&memFile);
	if( DSP_ERROR_STATUS(wSts)) goto Exit;

	*pulN =memFile.m_file.ulSz;
Exit:
	return wSts;
}


IMG_WORD	FILE_SaveBmpToMemToFile(IMG_UBBUF const *pubbSbuf, IMG_CHAR const *pcFile)
{
	IMG_CHAR	*pcBuf =NULL;
	IMG_ULWORD  ulN =0;
	IMG_WORD	wSts =OK;

	MEM_PushAllHeapStatus();
	// Query buffer size
	wSts =FILE_SaveBmpToMem( pubbSbuf, &pcBuf, &ulN);
	if( DSP_ERROR_STATUS(wSts)) goto Exit;

	// Save to file
	FILE *fp =fopen(pcFile, "wb");
	fwrite( pcBuf, ulN, 1, fp);
	fclose(fp);
Exit:
	MEM_PopAllHeapStatus();
	return wSts;
}

IMG_WORD Test1()
{
	if(0)
	{
		IMG_UBBUF	const	*pubbSbuf;
		IMG_COORD	const	*pcoOffset;
		IMG_SIZE	const	*pszOpSize;
		IMG_WORD			wIsRawColor;
		IMG_REAL	const	*prWbCoeff;
		IMG_UBYTE	const	*pubBayerPattern;
		IMG_CHAR			*pubFileName;
		SAVE_FILE_TRAITS	sf;

		MIL_SaveToBmpFileEx( pubbSbuf, pcoOffset,pszOpSize,wIsRawColor,prWbCoeff,pubBayerPattern,pubFileName,&sf);
	}
	MEM_PushAllHeapStatus();

	IMG_UBBUF	ubbS;
	IMG_COORD	coZ ={0,0};
	IMG_WORD	wStatus =OK;
	IMG_CHAR	*pcFile ="S.bmp";
	
	wStatus =LoadBmp_MEM( pcFile, ubbS);
	if (DSP_ERROR_STATUS(wStatus)) goto Exit;

	wStatus =FILE_SaveBmpToMemToFile( &ubbS, "D.bmp");
	if (DSP_ERROR_STATUS(wStatus)) goto Exit;

	{
	SAVE_FILE_TRAITS	sf;
	wStatus =MIL_SaveToBmpFileEx( &ubbS, &coZ,&ubbS.size,IMG_TRUE,NULL,NULL,"D2.bmp",&sf);
	if (DSP_ERROR_STATUS(wStatus)) goto Exit;
	
	}
Exit:
	MEM_PopAllHeapStatus();
	return wStatus;
}


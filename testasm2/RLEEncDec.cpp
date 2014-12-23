#include <apt_ds.h>

#define	NEED_PACK_OPT_SZ	ITF_IsVersionOneEarlier((IMG_UBYTE *)"2.40", ITF_GetHostID()) == IMG_FALSE)

// Not pack header to save 5 byte space
#define	PACK_RLE_HDR	0

IMG_WORD	DS_PackMask_RLE(const IMG_UBBUF *pubbS, IMG_ULWORD **ppulData, IMG_LWORD *plwLength)
{
	IMG_WORD wStatus =OK;
	MEM_PushAllHeapStatus();

	IMG_COORD coZ ={0,0};
	IMG_LWBUF lwbufDest;
	IMG_ULWORD *pulInpData=0;

	if( ppulData != NULL )
		pulInpData = *ppulData;
	// 1 Pack header
	*plwLength = 0;
#if PACK_RLE_HDR
	DS_PACK_DATA_HEADER( DS_PackMask_RLE, pubbS, ppulData, plwLength);
#endif
	
	// 2 Pack buffer size
	PU_PackDspSize ( &pubbS->size, ppulData );
	*plwLength += PU_PSizeDspSize(&pubbS->size);

	if(pubbS->size.x && pubbS->size.y)
	{
		IMG_SIZE	szLwbuf;
		szLwbuf.x =pubbS->size.x +2;
		szLwbuf.y =pubbS->size.y;
		wStatus = MEM_AllocBufAtBank( &szLwbuf, SYS_BANK1_32, (IMG_VVBUF*)&lwbufDest );
		if(DSP_ERROR_STATUS(wStatus)) { SYS_Assert(0); goto Exit;}

		wStatus = DSP_RLEEncoding( pubbS, &coZ, &pubbS->size, &lwbufDest);
		if(DSP_ERROR_STATUS(wStatus)) { SYS_Assert(0); goto Exit;}
		
		// 3 Pack dummy data length
		IMG_ULWORD	*pulDataLength = ppulData ? (*ppulData) : NULL;
		IMG_ULWORD	ulDataLengthDummy =0, ulDataLength=0;
		PU_PackDspUlword( &ulDataLengthDummy, ppulData );
		*plwLength += PU_PSizeDspUlword();

		// 4 Pack data 
		for( IMG_UWORD y=0; y<pubbS->size.y; y++)
		{
			IMG_LWORD	*plwP =lwbufDest.ptr + lwbufDest.linestep *y +0;
			for( IMG_UWORD x=0; x<lwbufDest.size.x; x++, plwP++)
			{
				assert( *plwP >=0 && *plwP <=pubbS->size.x );

				//if(ppulData)
					//printf("Pack %x to addr %p\n", *plwP, *ppulData);

				PU_PackDspUword_dense( (IMG_UWORD*)plwP, (IMG_UWORD**)ppulData );
				ulDataLength += PU_PSizeDspUword_dense();

				if( *plwP==0 )
					break;
			}
		}

		*plwLength +=ulDataLength;

		// 5. Fix data length
		if( pulDataLength )
		{
			assert( ulDataLengthDummy== *pulDataLength );
			*pulDataLength =ulDataLength;
		}
	}
Exit:
	//if(pulInpData)
	//{
	//	printf("Packed data:\n");
	//	for(IMG_UBYTE *pubP=(IMG_UBYTE*)pulInpData; pubP <(IMG_UBYTE*)*ppulData; pubP++)
	//	{
	//		printf("%p: %x\n", pubP, *pubP);
	//	}
	//}
	DS_CHECK_PACK_POINTER( pulInpData, ppulData, *plwLength, DS_PackMask_RLE );
	

	MEM_PopAllHeapStatus();
	return wStatus;
}

IMG_WORD	DS_UnpackMask_RLE (	 IMG_ULWORD		const	**ppulData,
								 IMG_UBBUF				*pubbBuf )
{
	IMG_WORD wStatus =OK;
	IMG_COORD coZ ={0,0};

	MEM_PushAllHeapStatus();

	// 1
#if PACK_RLE_HDR
	wStatus = PU_UnpackDataHeader( ppulData, NULL );
	if(DSP_ERROR_STATUS(wStatus)) { SYS_Assert(0); goto Exit;}
#endif

	// 2
	IMG_SIZE	szImg;
	szImg.x =szImg.y =0;
	PU_UnpackDspSize( ppulData, &szImg );
	
	pubbBuf->size= szImg;
	pubbBuf->linestep =szImg.x;

	if(szImg.x && szImg.y)
	{
		pubbBuf->ptr =(IMG_UBYTE*)realloc( pubbBuf->ptr, szImg.x *szImg.y );
		if(!pubbBuf->ptr)
		{
			SYS_Assert(0); wStatus =ERR_STD_MALLOC; goto Exit;
		}
		memset( pubbBuf->ptr, 0, szImg.x *szImg.y);

		IMG_LWBUF	lwbufDest;
		IMG_SIZE	szLwbuf;
		szLwbuf.x =szImg.x +2;
		szLwbuf.y =szImg.y;

		wStatus = MEM_AllocBufAtBank( &szLwbuf, SYS_BANK1_32, (IMG_VVBUF*)&lwbufDest );
		if(DSP_ERROR_STATUS(wStatus)) { SYS_Assert(0); goto Exit;}
		memset( lwbufDest.ptr, 0, lwbufDest.linestep *lwbufDest.size.y *sizeof(IMG_LWORD));

		// 3
		IMG_ULWORD	ulDataLength=0;
		PU_UnpackDspUlword( ppulData, &ulDataLength );
		
		if(ulDataLength==0) goto Exit;

		// 4
		IMG_UWORD uwData=0;
		IMG_UWORD x=0, y=0;
		IMG_LWORD *plwData =lwbufDest.ptr;
		for( IMG_ULWORD i=0; i<ulDataLength; i++)
		{
			PU_UnpackDspUword_dense(  (const IMG_UWORD**)ppulData, &uwData );
			
			//printf("UnPack %x to addr %p\n", uwData, (*(const IMG_UWORD**)ppulData) -1);

			if(uwData ==0)
			{
				y++; x=0;
				assert( y<= pubbBuf->size.y );
				if( y==pubbBuf->size.y )	// last row
				{
					assert( (i+1)*PU_PSizeDspUword_dense() ==ulDataLength );
					break;
				}
				plwData =lwbufDest.ptr +lwbufDest.linestep *y;
			}
			else
			{
				x++;
				*plwData =uwData;
				plwData++;
			}
		}

		wStatus = DSP_RLEDecoding( &lwbufDest, pubbBuf, &coZ, &pubbBuf->size);
		if(DSP_ERROR_STATUS(wStatus)) { SYS_Assert(0); goto Exit;}
	}
	else
	{
		if(pubbBuf->ptr) 
			asm_free(pubbBuf->ptr);
		pubbBuf->ptr =NULL;
	}
Exit:
#if PACK_RLE_HDR
	PU_SkipToEnd( ppulData );
#endif

	MEM_PopAllHeapStatus();

	return wStatus;
}

#define	PACK_HDR_HACK_METHOD	1
#define	PACK_AUTOSELECT_ON		0

static
IMG_WORD	DS_PackMask_AutoSelect(const IMG_UBBUF *pubbS, IMG_ULWORD **ppulData, IMG_LWORD *plwLength)
{
	IMG_WORD wStatus =OK;
	IMG_ULWORD *pulInpData=0;

	if( ppulData != NULL )
		pulInpData = *ppulData;
	// 1 Pack header
	*plwLength = 0;
#if PACK_HDR_HACK_METHOD
	IMG_UBYTE *pstHdr = (IMG_UBYTE*)( ppulData ? (*ppulData) : NULL );
	PU_DATA_HEADER stHdrDummy;
	PU_InitDataHeader(&stHdrDummy);		
	PU_PackDataHeader(&stHdrDummy, ppulData);
	*plwLength = PU_PSizeDataHeader(NULL);
#else
	DS_PACK_DATA_HEADER( DS_PackMask_AutoSelect, pubbS, ppulData, plwLength);
#endif

	// 2 Compare data size of 2 methods
#if PACK_AUTOSELECT_ON
	IMG_UWORD	uwIsRLE =IMG_FALSE;
	IMG_LWORD	lwLength1 =0;
	wStatus =DS_PackMask_RLE( pubbS, 0, &lwLength1);
	if(DSP_ERROR_STATUS(wStatus)) { SYS_Assert(0); goto Exit;}

	IMG_LWORD lwLength2 =0;
	PU_PackDspUbbuf_deep( pubbS, 0, &lwLength2);

	printf("Src sz %d %d RLE sz %d raw sz %d\n", pubbS->size.x, pubbS->size.y, lwLength1, lwLength2);

	if( lwLength1< lwLength2)
		uwIsRLE =IMG_TRUE;
#else
	IMG_UWORD	uwIsRLE =IMG_TRUE;
#endif
	// 3 Pack data size
	PU_PackDspUword( &uwIsRLE, ppulData );
	*plwLength += PU_PSizeDspUword();

	// Pack data
	if( uwIsRLE)
	{
		IMG_LWORD	lwLength =0;

		wStatus =DS_PackMask_RLE( pubbS, ppulData, &lwLength);
		if(DSP_ERROR_STATUS(wStatus)) { SYS_Assert(0); goto Exit;}
		*plwLength += lwLength;
	}
	else
	{
		IMG_LWORD	lwLength =0;
		PU_PackDspUbbuf_deep( pubbS, ppulData, &lwLength);
		*plwLength += lwLength;
	}

#if PACK_HDR_HACK_METHOD
	if(pstHdr)
	{
		assert( *pstHdr == PU_PSizeDataHeader(&stHdrDummy) );
		*(IMG_LWORD*)(pstHdr+1) =*plwLength;
	}
#endif

Exit:
	DS_CHECK_PACK_POINTER( pulInpData, ppulData, *plwLength, DS_PackMask_AutoSelect );

	return wStatus;
}
// Optimization pack
IMG_WORD	DS_PackMask_Opt(IMG_UBYTE	ubIsOpt, const IMG_UBBUF *pubbS, IMG_ULWORD **ppulData, IMG_LWORD *plwLength)
{
	IMG_WORD wStatus =OK;
	if(ubIsOpt)
	{
		return DS_PackMask_AutoSelect( pubbS, ppulData, plwLength);
	}
	else
	{
		PU_PackDspUbbuf_deep( pubbS, ppulData, plwLength);
		return OK;
	}
//Exit:
	return wStatus;
}

IMG_WORD	DS_UnpackMask_Opt(IMG_UBYTE	ubIsOpt, IMG_ULWORD const **ppulData, IMG_UBBUF		 *pubbDbuf)
{
	if(ubIsOpt)
	{
		IMG_WORD wStatus  =OK;

		wStatus = PU_UnpackDataHeader( ppulData, NULL );
		//if(DSP_ERROR_STATUS(wStatus)) { SYS_Assert(0); goto Exit;}

		IMG_UWORD uwIsRLE =0;
		PU_UnpackDspUword( ppulData , &uwIsRLE );
		assert( uwIsRLE==0 || uwIsRLE==1);

		if(uwIsRLE)
			wStatus = DS_UnpackMask_RLE( ppulData, pubbDbuf );
		else
			wStatus =PU_UnpackDspUbbuf_deep( ppulData, pubbDbuf);

		PU_SkipToEnd( ppulData );
		return wStatus;
	}
	else
	{
		return PU_UnpackDspUbbuf_deep( ppulData, pubbDbuf);
	}
}

static
void PrintBuf(IMG_UBBUF const *pubbS)
{
	for(int y=0; y<pubbS->size.y; y++)
	{
		for(int x=0; x<pubbS->size.x; x++)
		{
			printf("%d ", pubbS->ptr[pubbS->linestep *y+x]);
		}
		printf("\n");
	}
}

static
IMG_WORD TestMask_Opt(int id, IMG_SIZE const *psz)
{
	IMG_WORD wStatus =OK;
	std::vector<IMG_ULWORD> vulData;

	MEM_PushAllHeapStatus();

	//1  malloc buffer
	IMG_UBBUF ubbufS;
	memset( &ubbufS, 0, sizeof(ubbufS));
	IMG_SIZE szImg ={rand()%20+1,rand()%20+1};
	if(psz) szImg =*psz;

	if( szImg.x && szImg.y)
	{
		wStatus = MEM_AllocBufAtBank( &szImg, SYS_BANK1_8, (IMG_VVBUF*)&ubbufS );
		if(DSP_ERROR_STATUS(wStatus)) { SYS_Assert(0); goto Exit;}
		
		if( id==0)
			memset( ubbufS.ptr, 0, ubbufS.linestep *ubbufS.size.y);
		else if( id==1)
			memset( ubbufS.ptr, 1, ubbufS.linestep *ubbufS.size.y);
		else
		{
			for( int i=0; i<ubbufS.linestep *ubbufS.size.y; i++)
				ubbufS.ptr[i] = (rand()*2 > RAND_MAX) ?1:0;
		}
	}

	printf("------------------\n");
	printf("Input:\n");
	PrintBuf(&ubbufS);
	// 2 pack
	IMG_LWORD lwLength=0;
	wStatus =DS_PackMask_Opt( 1, &ubbufS, NULL, &lwLength);
	if(DSP_ERROR_STATUS(wStatus)) { SYS_Assert(0); goto Exit;}

	assert(lwLength>0);
	
	vulData.resize (lwLength);
	IMG_ULWORD *pulDataS =&vulData[0];
	IMG_LWORD lwLength2=0;
	wStatus =DS_PackMask_Opt( 1, &ubbufS, &pulDataS, &lwLength2);
	if(DSP_ERROR_STATUS(wStatus)) { SYS_Assert(0); goto Exit;}
	assert( lwLength2==lwLength );

	// 3 unpack
	pulDataS =&vulData[0];
	IMG_UBBUF ubbufD;
	memset( &ubbufD, 0, sizeof(ubbufD));
	wStatus =DS_UnpackMask_Opt( 1, (const IMG_ULWORD **)&pulDataS, &ubbufD);
	if(DSP_ERROR_STATUS(wStatus)) { SYS_Assert(0); goto Exit;}
	assert( (char*)pulDataS - (char*)&vulData[0] ==lwLength );

	// 4 compare
	printf("Output:\n");
	PrintBuf(&ubbufD);

	assert( ubbufD.size.x == ubbufS.size.x );
	assert( ubbufD.size.y == ubbufS.size.y );

	for( int y=0; y<ubbufD.size.y; y++)
	{
		for( int x=0; x<ubbufD.size.x; x++)
		{
			assert( ubbufD.ptr [ ubbufD.linestep *y +x] ==  ubbufS.ptr [ ubbufS.linestep *y +x] );
		}
	}
Exit:

	MEM_PopAllHeapStatus();
	return wStatus;
}

IMG_WORD TestMask_Opt()
{
	for( int i=0; i<1000; i++)
		TestMask_Opt(i, 0);
	
	IMG_SIZE szOp={0,1};
	TestMask_Opt(1, &szOp);

	return 0;
}

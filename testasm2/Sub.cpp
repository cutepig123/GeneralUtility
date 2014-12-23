#include <dsp_lib.h>
//#include <ipp.h>
//#include <FILE_MGT.H>
#include <dsp_mem.H>
//#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <atceagle.h>

//#define	ITF_SOLAR3D_CHK_ERROR		if(DSP_ERROR_STATUS(wStatus)) {goto Exit;}

//IMG_WORD  DSP_Sub_rr(IMG_RBUF const *prbSbuf1,
//						 IMG_RBUF const *prbSbuf2,
//						IMG_REAL	rAdd,
//						 IMG_RBUF		 *prbDbuf)
//{
//	IMG_WORD wStatus = OK;
//	IppStatus stStatus;
//	IppiSize stRoiSize = {0,0};
//	
//	stRoiSize.height = prbSbuf1->size.y;
//	stRoiSize.width = prbSbuf1->size.x;
//	
//	stStatus = ippiSub_32f_C1R(prbSbuf2->ptr, prbSbuf2->linestep * sizeof(IMG_REAL), prbSbuf1->ptr, prbSbuf1->linestep* sizeof(IMG_REAL),
//							prbDbuf->ptr, prbDbuf->linestep* sizeof(IMG_REAL), stRoiSize);
//	if(stStatus != ippStsNoErr)
//	{
//		wStatus = ERR_INVALID_ARG; goto Exit;
//	}
//	
//	stStatus =ippiAddC_32f_C1IR(rAdd, prbDbuf->ptr, prbDbuf->linestep* sizeof(IMG_REAL), stRoiSize);
//	if(stStatus != ippStsNoErr)
//	{
//		wStatus = ERR_INVALID_ARG; goto Exit;
//	}
//
//	
//Exit:
//	return wStatus;
//}		
//
//short CalcThicknessBuf1(IMG_RBUF const *prbFg, IMG_UBBUF const *pubFgM, 
//		IMG_RBUF const *prbbg, IMG_UBBUF const *pubBgM, IMG_REAL rZOffset,
//	IMG_RBUF *prbThick, IMG_UBBUF  *pubThickM, 
//	IMG_RBUF  *prbTemp,
//	FILE *fpLog)
//{
//	short wStatus = OK;
//	IMG_COORD coZero = {0,0};
//
//	wStatus =  DSP_And_ububub( pubFgM, &coZero, pubBgM, &coZero,pubThickM,&coZero,&prbFg->size);
//	ITF_SOLAR3D_CHK_ERROR;
//
//	wStatus = DSP_Sub_rr( prbFg, prbbg,rZOffset, prbThick);
//	ITF_SOLAR3D_CHK_ERROR;
//
//	
//		IppiSize stRoiSize;
//	
//	stRoiSize.height = pubThickM->size.y;
//	stRoiSize.width = pubThickM->size.x;
//
//	if (ippStsNoErr !=ippiConvert_8u32f_C1R( pubThickM->ptr, pubThickM->linestep, 
//		prbTemp->ptr, prbTemp->linestep*sizeof(IMG_REAL), stRoiSize))
//	{
//		wStatus = ERR_INVALID_ARG; goto Exit;
//	}
//
//	if (ippStsNoErr !=ippiMul_32f_C1IR(prbTemp->ptr, prbTemp->linestep*sizeof(IMG_REAL), prbThick->ptr, prbThick->linestep* sizeof(IMG_REAL), stRoiSize))
//	{
//		wStatus = ERR_INVALID_ARG; goto Exit;
//	}
//Exit:
//	return wStatus;
//}
//
//// prbThick = prbFg - prbbg + rZOffset
//short CalcThicknessBuf2(IMG_RBUF const *prbFg, IMG_UBBUF const *pubFgM, 
//	IMG_RBUF const *prbbg, IMG_UBBUF const *pubBgM, IMG_REAL rZOffset,
//	IMG_RBUF *prbThick, IMG_UBBUF  *pubThickM, FILE *fpLog)
//{
//	short wStatus = OK;
//	
//	IMG_REAL *prFg = prbFg->ptr;
//	IMG_REAL *prBg = prbbg->ptr;
//	IMG_REAL *prThick = prbThick->ptr;
//	IMG_UBYTE *puFgM = pubFgM->ptr;
//	IMG_UBYTE *puBgM = pubBgM->ptr;
//	IMG_UBYTE *puThickM = pubThickM->ptr;
//
//	for(IMG_UWORD y=0; y<prbFg->size.y; y++)
//	{
//		for(IMG_UWORD x=0; x<prbFg->size.x; x++,prFg++,prBg++,prThick++,puFgM++,puBgM++,puThickM++)
//		{
//			if( *puFgM && *puBgM )
//			{
//				*puThickM = 1;
//				*prThick = *prFg - *prBg + rZOffset;
//				
//			}
//			else
//			{
//				*puThickM = 0;
//				*prThick = 0.0f;
//			}
//			
//		}
//		
//		prFg += prbFg->linestep - prbFg->size.x;
//		prBg += prbbg->linestep- prbFg->size.x;
//		prThick += prbThick->linestep- prbFg->size.x;
//		puFgM += pubFgM->linestep- prbFg->size.x;
//		puBgM += pubBgM->linestep- prbFg->size.x;
//		puThickM += pubThickM->linestep- prbFg->size.x;
//	}
//Exit:
//	return wStatus;
//}
//
//struct TimeLog
//{
//	DWORD a;
//	DWORD b;
//
//	TimeLog()
//	{
//		a=GetTickCount();
//	}
//
//	~TimeLog()
//	{
//		b=GetTickCount();
//		printf("Using time %ums\n", b-a);
//	}
//};
//
//IMG_RBUF arbB[4];
//IMG_UBBUF aubbB[4];
//
//DWORD WINAPI ThreadFunc1( void * pThis)
//{
//	int j = *(int*)pThis;
//
//	{
//		DWORD a=GetTickCount();
//		for( int i=0; i<20; i++ )
//		{
//			short wStatus = CalcThicknessBuf2( arbB, aubbB, arbB+1, aubbB+1, 100.0f,  arbB+2+j, aubbB+2+j, NULL);
//			SYS_Assert(wStatus==OK);
//		}
//		DWORD b=GetTickCount();
//		printf(" %d Using time %ums\n", j, b-a);
//	}
//
//	return 0;
//}
//
//void Test1()
//{
//	int i;	
//	
//	for( i=0; i<4; i++ )
//	{
//		arbB[i].linestep = arbB[i].size.x = 5000;
//		arbB[i].size.y = 500;
//		arbB[i].ptr = new IMG_REAL[arbB[i].linestep*arbB[i].size.y];
//		memset( arbB[i].ptr, 0, sizeof(IMG_REAL)*arbB[i].linestep*arbB[i].size.y);
//
//		aubbB[i].linestep = aubbB[i].size.x = 5000;
//		aubbB[i].size.y = 500;
//		aubbB[i].ptr = new IMG_UBYTE[aubbB[i].linestep*aubbB[i].size.y];
//		memset( aubbB[i].ptr, 1, sizeof(IMG_UBYTE)*arbB[i].linestep*arbB[i].size.y);
//	}
//
//	short wStatus = OK;
//
//	{
//		IMG_UWORD
//			ii= 0,
//			jj= 0;
//
//		IMG_RBUF
//			rbB_s,
//			rbB1_s,
//			rbT_s,
//			rbD_s;
//
//		IMG_UBBUF
//			ubbB_s,
//			ubbB1_s,
//			ubbD_s;
//
//		rbB_s.size.x= rbB_s.size.y= 250;
//		ubbB_s.size.x= ubbB_s.size.y= 250;
//		rbB1_s.size.x= rbB1_s.size.y= 250;
//		ubbB1_s.size.x= ubbB1_s.size.y= 250;
//		rbD_s.size.x= rbD_s.size.y= 250;
//		ubbD_s.size.x= ubbD_s.size.y= 250;
//		rbT_s.size.x= rbT_s.size.y= 250;
//
//		rbB_s.linestep= arbB[0].linestep;
//		rbB1_s.linestep= arbB[1].linestep;
//		rbD_s.linestep= arbB[2].linestep;
//
//		ubbB_s.linestep= aubbB[0].linestep;
//		ubbB1_s.linestep= aubbB[1].linestep;
//		ubbD_s.linestep= aubbB[2].linestep;
//
//		rbT_s.linestep= arbB[3].linestep;
//
//		TimeLog a;
//		for( i=0; i<20; i++ )
//		{
//			//wStatus = CalcThicknessBuf1( arbB, aubbB, arbB+1, aubbB+1, 100.0f,  arbB+2, aubbB+2, arbB+3,NULL);
//
//			rbB_s.ptr= arbB[0].ptr;
//			rbB1_s.ptr= arbB[1].ptr;
//			rbD_s.ptr= arbB[2].ptr;
//			ubbB_s.ptr= aubbB[0].ptr;
//			ubbB1_s.ptr= aubbB[1].ptr;
//			ubbD_s.ptr= aubbB[2].ptr;
//			rbT_s.ptr= arbB[3].ptr;
//
//			for (jj= 0; jj< 500; jj+= 250,
//				rbB_s.ptr+= (250*rbB_s.linestep- ii), rbB1_s.ptr+= (250*rbB1_s.linestep- ii), rbD_s.ptr+= (250*rbD_s.linestep- ii), rbT_s.ptr+= (250*rbT_s.linestep- ii),
//				ubbB_s.ptr+= (250*ubbB_s.linestep- ii), ubbB1_s.ptr+= (250*ubbB1_s.linestep- ii), ubbD_s.ptr+= (250*ubbD_s.linestep- ii))
//
//				for (ii= 0; ii< 5000; ii+= 250, rbB_s.ptr+= 250, rbB1_s.ptr+= 250, rbD_s.ptr+= 250, rbT_s.ptr+= 250,
//				rbB_s.ptr+= 250, ubbB1_s.ptr+= 250, ubbD_s.ptr+= 250)
//					wStatus = CalcThicknessBuf1( &rbB_s, &ubbB_s, &rbB1_s, &ubbB1_s, 100.0f,  &rbD_s, &ubbD_s, &rbT_s, NULL);
//					//wStatus = CalcThicknessBuf1( arbB, aubbB, arbB+1, aubbB+1, 100.0f,  arbB+2, aubbB+2, arbB+3,NULL);
//			
//			SYS_Assert(wStatus==OK);
//		}
//	}
//	
//	//ThreadFunc1("MainThread");
//
//	int aaa[2] = {0,1};
//
//	ThreadFunc1(aaa+1);
//
//	for( int i=0; i<5000*500; i++ )
//	{
//		assert( aubbB[2].ptr[i] == aubbB[3].ptr[i] );
//		assert( arbB[2].ptr[i] == arbB[3].ptr[i] );
//	}
//	//for( i=0; i<2; i++ )
//	//	::CreateThread(NULL, NULL, ThreadFunc1, &aaa[i], NULL, NULL);
//
//}

//int Test2()
//{
//	printf("Hello\n");
//	return 1;
//}

void Test3()
{
	IMG_REAL	arX[2]={1,2};
	IMG_REAL	arY[2]={1,2};
	IMG_REAL	arResult[2];
									
	IMG_WORD	w =DSP_Fast_Atan2_Table( arX,arY,arResult,2);
};

void main()
{
	SYS_InitModule();
	//FM_InitModule(20);
	//FM_SetMaxNumOfFile(10,10);
	MEM_OpenHeap();

	//Test1();
	//Test2();
	Test3();
	system("pause");

	MEM_CloseHeap();
	//FM_FreeModule();
	SYS_FreeModule();
}

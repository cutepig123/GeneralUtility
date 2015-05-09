#include	<vis/asm.h>
#include	<math.h>
#include	<memory.h>

IMG_WORD	DSP_INS_AverageVertical(
	IMG_RBUF	const	*prbSrc,		// input buffer
	IMG_UWORD	const	uwOffx,			// Offset x
	IMG_UWORD	const	uwStep,			// x linestep
	IMG_UWORD	const	uwHSz,			// window size (i.e. full kernel size), must be odd kernel
	IMG_RBUF			*prbDes			// filtered buffer
	);

//
//
//			Omega= [{1 cos(wx_i) -sin(wx_i)}_i= -uwNumPeriod: uwNumPeriod]
//
IMG_WORD	MAT_GetOmegaMatrix(
							   IMG_REAL		const	rPeriod,		// period of the targeted sin. wave
							   IMG_UWORD	const	uwHKernel,		// half kernel size
							   IMG_RBUF				*prbW			// User malloced omega buffer
							   )
{
	IMG_UWORD
		uwStep	= 0;

	IMG_LWORD
		i		= 0,
		lFKernel= (uwHKernel<< 1)+ 1;

	IMG_REAL
		rPhase	= 0.0f,
		rOmega	= 0.0f;

	IMG_REAL
		*prW	= NULL,							// real pointer for omega buffer
		*prWe	= NULL;							// end pointer

	if (NULL== prbW		|| NULL== prbW->ptr)
		return OK;

	if (1.0f> rPeriod	|| 1> uwHKernel		||
		3!= prbW->size.x|| 2*uwHKernel+1!= prbW->size.y)
		return MAT_INVALID_INPUT;

	prWe= prbW->ptr+ (prbW->size.y- 1)*prbW->linestep+ prbW->size.x;
	uwStep= prbW->linestep- prbW->size.x;

	rOmega= (IMG_REAL) (MAT_2PI/rPeriod);

	for (prW = prbW->ptr; prW< prWe; prW+= uwStep)
	{
		for (i= 0; i< lFKernel; i++)
		{
			rPhase	= rOmega*(i- (IMG_LWORD)uwHKernel);

			*(prW++)= 1.0f;
			*(prW++)= (IMG_REAL) cos(rPhase);
			*(prW++)= (IMG_REAL)-sin(rPhase);
		}
	}

	return OK;
}


IMG_WORD	MAT_GetPseudoInverseMatrix(
									   IMG_RBUF		const	*prbM,		// a matrix
									   IMG_RBUF				*prbiM		// User malloced omega buffer
									   )
{
	IMG_WORD
		wSts	= OK,
		wMEMSts	= OK;

	IMG_ULWORD
		ulMEM= 0;

	IMG_RBUF
		rbMTM,					// MTM
		rbiMTM;					// inverse of MTM

	IMG_REAL
		*prBuf= NULL;			// buffer pointer

	IppStatus
		ippSts	= ippStsNoErr;

	if (NULL== prbiM	|| NULL== prbiM->ptr)	return OK;

	if (NULL== prbM					|| NULL== prbM->ptr	||
		prbiM->size.x!= prbM->size.y|| prbiM->size.y!= prbM->size.x)
		return MAT_INVALID_INPUT;

	rbMTM.size.x	= rbMTM.size.y	= rbMTM.linestep= prbM->size.x;
	rbiMTM.size		= rbMTM.size;
	rbiMTM.linestep	= rbiMTM.size.x;

	wSts= MEM_PushAllHeapStatus();
	if (OK> wSts)	return wSts;

	ulMEM= (rbMTM.size.x*rbMTM.size.y);
	if (NULL== (rbMTM.ptr= (IMG_REAL *) MEM_Malloc(((ulMEM<< 1)+ rbMTM.size.x)*sizeof(IMG_REAL), SYS_BANK1_8)))
	{
		wSts= MEM_ERR_MALLOC;
		goto EXIT_MAT_GetPseudoInverseMatrix;
	}
	memset(rbMTM.ptr, 0, ((ulMEM<< 1)+ rbMTM.size.x)*sizeof(IMG_REAL));
	rbiMTM.ptr	= rbMTM.ptr +ulMEM;
	prBuf		= rbiMTM.ptr+ulMEM;

	// MTM
	ippSts= ippmMul_tm_32f(
		prbM->ptr, prbM->linestep*sizeof(IMG_REAL), sizeof(IMG_REAL), prbM->size.x, prbM->size.y,
		prbM->ptr, prbM->linestep*sizeof(IMG_REAL), sizeof(IMG_REAL), prbM->size.x, prbM->size.y,
		rbMTM.ptr, rbMTM.linestep*sizeof(IMG_REAL), sizeof(IMG_REAL));

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_MAT_GetPseudoInverseMatrix;
	}

	ippSts= ippmInvert_m_32f(
		rbMTM.ptr , rbMTM.linestep*sizeof(IMG_REAL) , sizeof(IMG_REAL), prBuf,
		rbiMTM.ptr, rbiMTM.linestep*sizeof(IMG_REAL), sizeof(IMG_REAL), rbMTM.size.x);

	// (MTM)i*MT
	ippSts= ippmMul_mt_32f(
		rbiMTM.ptr, rbiMTM.linestep*sizeof(IMG_REAL), sizeof(IMG_REAL), rbiMTM.size.x, rbiMTM.size.y,
		prbM->ptr , prbM->linestep*sizeof(IMG_REAL) , sizeof(IMG_REAL), prbM->size.x , prbM->size.y ,
		prbiM->ptr, prbiM->linestep*sizeof(IMG_REAL), sizeof(IMG_REAL));

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_MAT_GetPseudoInverseMatrix;
	}

EXIT_MAT_GetPseudoInverseMatrix:
	wMEMSts= MEM_PopAllHeapStatus();
	if (OK> wMEMSts) return wMEMSts;
	return wSts;
}

// Generate 3 type kernels
//		pprKernel[0]= head	kernel
//		pprKernel[1]= middle	"
//		pprKernel[2]= tail		"
//

IMG_WORD	DSP_GEN_SinFilterKernelSet(
									   IMG_REAL		const	rPeriod,		// period of the targeted sin. wave
									   IMG_UWORD	const	uwHKernel,		// half sin kernel size
									   IMG_UWORD	const	uwHSKernel,		// half smoothing kernel (must be smaller than uwHKernel)
									   IMG_REAL				**pprKernel		// output kernel: head, middle and tail
									   )
{
	IMG_WORD
		wMEMSts	= OK,
		wSts	= OK;

	IMG_UWORD
		i		= 0,
		j		= 0;

	IMG_ULWORD
		ulMEM	= 0;										// memory allocation size

	IMG_RBUF
		rbW,												// Omega matrix
		rbIW,												// pesudo inverse matrix
		rbH,												// Kernel matrix
		rbHs;												// Kernel matrix with smooth

	IMG_REAL
		*prH	= NULL;										// ptr to H

	IppStatus
		ippSts	= ippStsNoErr;

	if (uwHSKernel> uwHKernel)
		return DSP_ERR_INVALID_ARG;							// Smoothing must be smaller than sin. kernel

	// Form reconstruction kenerals
	rbW.size.x= 3;
	rbW.size.y=(IMG_UWORD) ((uwHKernel<< 1)+ 1);
	rbW.linestep= rbW.size.x;

	rbIW.size.x= rbIW.linestep= rbW.size.y;
	rbIW.size.y= rbW.size.x;

	rbH.size.y = rbH.size.x = rbH.linestep= rbW.size.y;			// Keneral size
	rbHs.size= rbH.size;
	rbHs.linestep= rbH.linestep;

	wSts= MEM_PushAllHeapStatus();
	if (wSts< OK)	return wSts;

	ulMEM= ((rbW.size.x*rbW.size.y)<< 1)+ (rbH.size.x*rbH.size.y)+ (rbHs.size.x*rbHs.size.y);
	if (NULL== (rbW.ptr= (IMG_REAL *) MEM_Malloc(ulMEM*sizeof(IMG_REAL), SYS_BANK1_8)))
	{
		wSts= MEM_ERR_MALLOC;
		goto EXIT_DSP_GEN_SinFilterKernelSet;
	}
	memset(rbW.ptr, 0, ulMEM*sizeof(IMG_REAL));
	rbIW.ptr= rbW.ptr +(rbW.size.x *rbW.size.y );
	rbH.ptr = rbIW.ptr+(rbIW.size.x*rbIW.size.y);
	rbHs.ptr= rbH.ptr +(rbH.size.x*rbH.size.y);

	wSts= MAT_GetOmegaMatrix(rPeriod, uwHKernel, &rbW);
	if (OK> wSts) goto EXIT_DSP_GEN_SinFilterKernelSet;

	wSts= MAT_GetPseudoInverseMatrix(&rbW, &rbIW);
	if (OK> wSts) goto EXIT_DSP_GEN_SinFilterKernelSet;

	ippSts=	ippmMul_mm_32f(
		rbW.ptr + 1				, rbW.linestep*sizeof(IMG_REAL)	, sizeof(IMG_REAL), rbW.size.x-1 , rbW.size.y,
		rbIW.ptr+ rbIW.linestep	, rbIW.linestep*sizeof(IMG_REAL), sizeof(IMG_REAL), rbIW.size.x, rbIW.size.y-1,
		rbH.ptr					, rbH.linestep*sizeof(IMG_REAL)	, sizeof(IMG_REAL));		// Generate 3 kenerls

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_GEN_SinFilterKernelSet;
	}

	prH= rbH.ptr;
	for (i= 0; i< rbH.size.y; i++, prH+= (rbH.linestep+ 1))	*prH-= 1.0f;
	wSts= DSP_INS_AverageVertical(&rbH, 0, 1, uwHSKernel, &rbHs);

	for (i= 0; i< 3; i++)
	{
		if (NULL== pprKernel[i])	continue;

		for (j= 0; j< rbW.size.y; j++)
			pprKernel[i][rbW.size.y- j- 1]= -rbHs.ptr[i*uwHKernel*rbHs.linestep+ j];
	}

EXIT_DSP_GEN_SinFilterKernelSet:
	wMEMSts= MEM_PopAllHeapStatus();
	if (OK> wMEMSts)	return wMEMSts;
	return wSts;
}

// Average filter
IMG_WORD	DSP_INS_AverageVertical(
									IMG_RBUF	const	*prbSrc,		// input buffer
									IMG_UWORD	const	uwOffx,			// Offset x
									IMG_UWORD	const	uwStep,			// x linestep
									IMG_UWORD	const	uwHSz,			// window size (i.e. full kernel size), must be odd kernel
									IMG_RBUF			*prbDes			// filtered buffer
									)
{
	IMG_WORD
		wMEMSts	= OK,
		wSts	= OK;

	IMG_UWORD
		i	= 0,
		uwFSz= (IMG_UWORD) ((uwHSz<< 1)+ 1);

	IMG_REAL
		rNorm	= 0.0f,			// filter value
		*prK	= NULL,
		*prKb	= NULL,
		*prS	= NULL,
		*prD	= NULL;

	IppStatus
		ippSts;

	IppiSize
		ippRoiSize;

	if (NULL== prbDes)	return OK;
	if (NULL== prbSrc					|| NULL== prbSrc->ptr||
		uwOffx> prbSrc->size.x			||
		prbDes->size.x!= prbSrc->size.x	|| prbDes->size.y!= prbSrc->size.y)	return DSP_ERR_INVALID_ARG;

	wMEMSts= MEM_PushAllHeapStatus();
	if (wMEMSts< OK)	return wMEMSts;

	if (NULL== (prK= (IMG_REAL *) MEM_Malloc((uwFSz+ uwHSz+ 1)*sizeof(IMG_REAL), SYS_BANK1_8)))
	{
		wSts= MEM_ERR_MALLOC;
		goto EXIT_DSP_INS_AverageVertical;
	}
	prKb= prK+ uwFSz;

	rNorm= 1.0f/(IMG_REAL)uwFSz;
	for (i= 0; i< uwFSz; i++)		prK[i]= rNorm;

	rNorm= 1.0f/(IMG_REAL)(1+uwHSz);
	for (i= 0; i< (1+uwHSz); i++)	prKb[i]= rNorm;

	ippRoiSize.width = 1;
	for (i= uwOffx; i< prbSrc->size.x; i+= uwStep)
	{
		prS= prbSrc->ptr+ i;
		prD= prbDes->ptr+ i;

		ippRoiSize.height= uwHSz+ 1;
		ippSts= ippiFilterColumn_32f_C1R(
			prS	, prbSrc->linestep*sizeof(IMG_REAL),
			prD	, prbDes->linestep*sizeof(IMG_REAL),
			ippRoiSize	, prKb, 1+uwHSz, uwHSz);
		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_INS_AverageVertical;
		}

		ippRoiSize.height= prbSrc->size.y- ((uwHSz+1)<<1);
		ippSts= ippiFilterColumn_32f_C1R(
			prS+ ((uwHSz+1)*prbSrc->linestep)	, prbSrc->linestep*sizeof(IMG_REAL),
			prD+ ((uwHSz+1)*prbDes->linestep)	, prbDes->linestep*sizeof(IMG_REAL),
			ippRoiSize	, prK , uwFSz, uwHSz);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_INS_AverageVertical;
		}

		ippRoiSize.height= uwHSz+ 1;
		ippSts= ippiFilterColumn_32f_C1R(
			prS+ ((prbSrc->size.y- uwHSz- 1)*prbSrc->linestep), prbSrc->linestep*sizeof(IMG_REAL),
			prD+ ((prbDes->size.y- uwHSz- 1)*prbSrc->linestep), prbDes->linestep*sizeof(IMG_REAL),
			ippRoiSize	, prKb, 1+uwHSz, 0);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_INS_AverageVertical;
		}
	}

EXIT_DSP_INS_AverageVertical:
	wMEMSts= MEM_PopAllHeapStatus();
	if (wMEMSts< OK)	return wMEMSts;
	return wSts;
}

IMG_WORD	DSP_INS_FilterWithLRBorder(
									   IMG_RBUF		const	*prbSrc,		// input buffer
									   IMG_COORD	const	coOff,			// offset
									   IMG_SIZE		const	szOp,			// opsize
									   IMG_UWORD	const	uwWsz,			// window size (i.e. full kernel size), must be odd kernel
									   IMG_REAL		const	**pprK,			// output kernel: head, middle and tail			
									   IMG_RBUF				*prbDes			// filtered buffer
									   )
{
	IMG_WORD
		wSts	= OK;

	IMG_UWORD
		uwHFx	= (uwWsz>> 1);

	IppStatus
		ippSts;

	IppiSize
		ippRoiSize;

	IMG_RBUF
		rbSrc;

	if (NULL== prbDes|| NULL== prbDes->ptr)	return OK;

	if (uwWsz< 1	||	NULL== prbSrc	||	NULL== prbSrc->ptr	||
		NULL== pprK	||	NULL== pprK[0]	||	NULL== pprK[1]		|| NULL== pprK[2]||
		prbDes->size.x!= szOp.x	||	prbDes->size.y!= szOp.y)
		return DSP_ERR_INVALID_ARG;

	rbSrc.size		= szOp;
	rbSrc.linestep	= prbSrc->linestep;
	rbSrc.ptr		= prbSrc->ptr+ coOff.y*prbSrc->linestep+ coOff.x;

	// Fill in the left bounderary
	ippRoiSize.width	= uwHFx;
	ippRoiSize.height	= rbSrc.size.y;

	ippSts= ippiFilterRow_32f_C1R(								
		rbSrc.ptr	,  rbSrc.linestep *sizeof(IMG_REAL),
		prbDes->ptr	, prbDes->linestep*sizeof(IMG_REAL),
		ippRoiSize	, pprK[0], uwWsz, uwWsz- 1);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_INS_FilterWithLRBorder;
	}

	// Fill in the middle part
	ippRoiSize.width	= rbSrc.size.x- uwWsz+ 1;
	ippRoiSize.height	= rbSrc.size.y;

	ippSts= ippiFilterRow_32f_C1R(
		rbSrc.ptr+ uwHFx	,  rbSrc.linestep *sizeof(IMG_REAL),
		prbDes->ptr+ uwHFx	, prbDes->linestep*sizeof(IMG_REAL),
		ippRoiSize			, pprK[1], uwWsz, uwHFx);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_INS_FilterWithLRBorder;
	}

	// Fill in the right boundary
	ippRoiSize.width	= uwHFx;
	ippRoiSize.height	= rbSrc.size.y;

	ippSts= ippiFilterRow_32f_C1R(								
		rbSrc.ptr  + rbSrc.size.x	- uwHFx,	rbSrc.linestep*sizeof(IMG_REAL),
		prbDes->ptr+ prbDes->size.x	- uwHFx,	prbDes->linestep*sizeof(IMG_REAL),
		ippRoiSize , pprK[2], uwWsz, 0);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_INS_FilterWithLRBorder;
	}

EXIT_DSP_INS_FilterWithLRBorder:
	return wSts;
}

IMG_WORD	DSP_INS_FilterWithLRBorder_ubb(
	IMG_UBBUF	const	*pubbSrc,			// input buffer
	IMG_COORD	const	coOff,				// offset
	IMG_SIZE	const	szOp,				// opsize
	IMG_REAL	const	rPeriod,			//		
	IMG_UBBUF			*pubbDes			// filtered buffer
	)
{
	IMG_WORD
		wSts	= OK,
		wMEMSts	= OK;

	IMG_UWORD
		uwHKernel= 0,
		uwHSKernel= 0,
		uwFK= 0;

	IMG_COORD
		coZ= {0, 0};

	IMG_UBBUF
		ubbSrc;

	IMG_RBUF
		rbSrc,
		rbDes;

	IppStatus
		ippSts= ippStsNoErr;

	IppiSize
		roiSize;

	IMG_REAL
		*aprKernel[3]= {NULL, NULL, NULL};

	if (NULL== pubbSrc || NULL== pubbSrc->ptr)	return DSP_ERR_INVALID_ARG;
	if (NULL== pubbDes || NULL== pubbDes->ptr)	return OK;


	wMEMSts= MEM_PushAllHeapStatus();
	if (OK!= wMEMSts) return wMEMSts;

	ubbSrc.size= szOp;
	ubbSrc.linestep= pubbSrc->linestep;
	ubbSrc.ptr = pubbSrc->ptr+ coOff.y*pubbSrc->linestep+ coOff.x;

	rbSrc.size= rbDes.size= szOp;
	rbSrc.linestep= rbDes.linestep= szOp.x;

	uwHKernel= (IMG_UWORD) rPeriod;
	uwHSKernel= 2;  				// uwHSKernel<= uwHKernel

	uwFK= (IMG_UWORD) ((uwHKernel<< 1) + 1);

	if (NULL==  (rbSrc.ptr= (IMG_REAL *) MEM_Malloc(sizeof(IMG_REAL)*(
		rbSrc.size.x*rbSrc.size.y+ 
		rbDes.size.x*rbDes.size.y+
		3*uwFK), SYS_BANK1_8)))
	{
		wSts= MEM_ERR_MALLOC;
		goto EXIT_DSP_INS_FilterWithLRBorder_ubb;
	}
	rbDes.ptr	= rbSrc.ptr		+ rbSrc.size.x*rbSrc.size.y;
	aprKernel[0]= rbDes.ptr		+ rbDes.size.x*rbDes.size.y;
	aprKernel[1]= aprKernel[0]	+ uwFK;
	aprKernel[2]= aprKernel[1]	+ uwFK;

	wSts= DSP_GEN_SinFilterKernelSet(rPeriod, uwHKernel, uwHSKernel, aprKernel);
	if (OK> wSts) goto EXIT_DSP_INS_FilterWithLRBorder_ubb;


	// Convert src to real buffer
	roiSize.width = rbSrc.size.x;
	roiSize.height= rbSrc.size.y;

	ippSts= ippiConvert_8u32f_C1R(ubbSrc.ptr, ubbSrc.linestep,
		rbSrc.ptr, rbSrc.linestep*sizeof(IMG_REAL), roiSize);

	wSts= DSP_INS_FilterWithLRBorder(&rbSrc, coZ, rbSrc.size, uwFK, (const IMG_REAL **) &aprKernel[0], &rbDes);
	if (OK> wSts) goto EXIT_DSP_INS_FilterWithLRBorder_ubb;

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_INS_FilterWithLRBorder_ubb;
	}

	roiSize.width = rbDes.size.x;
	roiSize.height= rbDes.size.y;

	ippSts= ippiConvert_32f8u_C1R(rbDes.ptr, rbDes.linestep*sizeof(IMG_REAL),
		pubbDes->ptr, pubbDes->linestep*sizeof(IMG_UBYTE), roiSize, ippRndNear);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_INS_FilterWithLRBorder_ubb;
	}

EXIT_DSP_INS_FilterWithLRBorder_ubb:
	if (OK!= (wMEMSts= MEM_PopAllHeapStatus()))	return wMEMSts;
	return wSts;
}

IMG_WORD	DSP_INS_FilterWithTBBorder(										// Top bottom filter
									   IMG_RBUF		const	*prbSrc,		// input buffer
									   IMG_COORD	const	coOff,			// offset
									   IMG_SIZE		const	szOp,			// opsize
									   IMG_UWORD	const	uwWsz,			// window size (i.e. full kernel size), must be odd kernel
									   IMG_REAL		const	**pprK,			// output kernel: head, middle and tail			
									   IMG_RBUF				*prbDes			// filtered buffer
									   )
{
	IMG_WORD
		wSts	= OK;

	IMG_UWORD
		uwHFy	= (uwWsz>> 1);

	IppStatus
		ippSts;

	IppiSize
		ippRoiSize;

	IMG_RBUF
		rbSrc;

	if (NULL== prbDes|| NULL== prbDes->ptr)	return OK;

	if (uwWsz< 1	||	NULL== prbSrc	||	NULL== prbSrc->ptr	||
		NULL== pprK	||	NULL== pprK[0]	||	NULL== pprK[1]		|| NULL== pprK[2]||
		prbDes->size.x!= szOp.x	||	prbDes->size.y!= szOp.y)
		return DSP_ERR_INVALID_ARG;

	rbSrc.size		= szOp;
	rbSrc.linestep	= prbSrc->linestep;
	rbSrc.ptr		= prbSrc->ptr+ coOff.y*prbSrc->linestep+ coOff.x;

	// Fill in the top bounderary
	ippRoiSize.width	= rbSrc.size.x;
	ippRoiSize.height	= uwHFy;

	ippSts= ippiFilterColumn_32f_C1R(								
		rbSrc.ptr	,  rbSrc.linestep *sizeof(IMG_REAL),
		prbDes->ptr	, prbDes->linestep*sizeof(IMG_REAL),
		ippRoiSize	, pprK[0], uwWsz, uwWsz- 1);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_INS_FilterWithTBBorder;
	}

	// Fill in the middle part
	ippRoiSize.width	= rbSrc.size.x;
	ippRoiSize.height	= rbSrc.size.y- uwWsz+ 1;

	ippSts= ippiFilterColumn_32f_C1R(
		rbSrc.ptr	+ uwHFy*rbSrc.linestep	,  rbSrc.linestep *sizeof(IMG_REAL),
		prbDes->ptr	+ uwHFy*prbDes->linestep, prbDes->linestep*sizeof(IMG_REAL),
		ippRoiSize							, pprK[1], uwWsz, uwHFy);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_INS_FilterWithTBBorder;
	}

	// Fill in the right boundary
	ippRoiSize.width	= rbSrc.size.x;
	ippRoiSize.height	= uwHFy;

	ippSts= ippiFilterColumn_32f_C1R(								
		rbSrc.ptr  + (rbSrc.size.y	- uwHFy)*rbSrc.linestep		, rbSrc.linestep  *sizeof(IMG_REAL),
		prbDes->ptr+ (prbDes->size.y- uwHFy)*prbDes->linestep	, prbDes->linestep*sizeof(IMG_REAL),
		ippRoiSize , pprK[2], uwWsz, 0);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_INS_FilterWithTBBorder;
	}

EXIT_DSP_INS_FilterWithTBBorder:
	return wSts;
}

IMG_WORD	DSP_DeRipples_rb(
							 IMG_RBUF		const	*prbSrc,		// source buffer
							 IMG_COORD		const	coOff,			// offset
							 IMG_SIZE		const	szOp,			// opsize
							 DSP_SIN_INFO	const	stInfo,			// sin wave information
							 IMG_RBUF				*prbDes			// destination buffer
							 )
{
	IMG_WORD
		wMEMSts	= OK,
		wSts	= OK;

	IMG_UWORD
		uwHK	= 0,
		uwFK	= 0;

	IMG_REAL
		rT= 0.0f;											// ripple period

	IMG_COORD
		coZ= {0, 0};

	IMG_REAL
		*aprK[3]= {NULL, NULL, NULL},
		**pprK	= NULL;

	if (NULL== prbDes || NULL== prbDes->ptr)	return OK;
	if (NULL== prbSrc || NULL== prbSrc->ptr)	return DSP_ERR_INVALID_ARG;

	if (fabs(stInfo.rfx)< 0.0001f && fabs(stInfo.rfy)< 0.0001f)		// no frequency
		return DSP_Copy_rr(prbSrc, &coOff, prbDes, &coZ, &szOp);

	rT		=	1.0f/((stInfo.rfx> stInfo.rfy)?stInfo.rfx: stInfo.rfy);
	if (rT< 1.0f)	return DSP_ERR_DIVIDED_BY_ZERO;			// frequency is so high to remove
	uwHK	= (IMG_UWORD) (0.5f*rT + 0.5f);

	if (OK!= (wMEMSts= MEM_PushAllHeapStatus()))	return wMEMSts;

	uwFK= (uwHK<< 1)+ 1;
	if (NULL== (aprK[0]= (IMG_REAL *) MEM_Malloc(3*uwFK*sizeof(IMG_REAL), SYS_BANK1_8)))
	{
		wSts= MEM_ERR_MALLOC;
		goto EXIT_DSP_DeRipples_rb;
	}
	memset(aprK[0], 0, 3*uwFK*sizeof(IMG_REAL));
	aprK[1]= aprK[0]+ uwFK;
	aprK[2]= aprK[1]+ uwFK;

	if (OK != (wSts = DSP_GEN_SinFilterKernelSet(rT, uwHK, 0, aprK)))
		goto EXIT_DSP_DeRipples_rb;

	pprK= &aprK[0];
	if (stInfo.rfx> stInfo.rfy)
	{
		if (	OK != (wSts= DSP_INS_FilterWithLRBorder(
			prbSrc, coOff, szOp, uwFK, (const IMG_REAL **) pprK, prbDes)))
			goto EXIT_DSP_DeRipples_rb;
	}
	else if (	OK != (wSts= DSP_INS_FilterWithTBBorder(
			prbSrc, coOff, szOp, uwFK, (const IMG_REAL **) pprK, prbDes)))
			goto EXIT_DSP_DeRipples_rb;

EXIT_DSP_DeRipples_rb:
	if (OK!= (wMEMSts= MEM_PopAllHeapStatus()))	return wMEMSts;
	return wSts;
}

IMG_WORD	DSP_DeRipples_ubb(
							  IMG_UBBUF		const	*pubbSrc,		// source buffer
							  IMG_COORD		const	coOff,			// offset
							  IMG_SIZE		const	szOp,			// opsize
							  DSP_SIN_INFO	const	stInfo,			// sin wave information
							  IMG_UBBUF				*pubbDes		// destination buffer
							  )
{
	IMG_WORD
		wSts	= OK,
		wMEMSts	= OK;

	IMG_COORD
		coZ= {0, 0};

	IMG_ULWORD
		ulMEM= 0;

	IMG_RBUF
		rbSrc,
		rbDes;

	if (NULL== pubbDes || NULL== pubbDes->ptr)
		return OK;

	if (NULL== pubbSrc || NULL== pubbSrc->ptr)
		return DSP_ERR_INVALID_ARG;

	rbSrc.size= rbDes.size= szOp;
	rbSrc.linestep= rbSrc.size.x;
	rbDes.linestep= rbDes.size.x;

	if (OK!= (wMEMSts= MEM_PushAllHeapStatus()))	return wMEMSts;

	ulMEM= szOp.x*szOp.y;
	if (NULL== (rbSrc.ptr= (IMG_REAL *) MEM_Malloc((ulMEM<< 1)*sizeof(IMG_REAL), SYS_BANK1_8)))
	{
		wSts= MEM_ERR_MALLOC;
		goto EXIT_DSP_DeRipples_ubb;
	}
	rbDes.ptr= rbSrc.ptr+ ulMEM;

	if (OK!= (wSts= DSP_Copy_ubr(pubbSrc, &coOff, &rbSrc, &coZ, &szOp)))
		goto EXIT_DSP_DeRipples_ubb;
	
	if (OK!= (wSts= DSP_DeRipples_rb(&rbSrc, coZ, szOp, stInfo, &rbDes)))
		goto  EXIT_DSP_DeRipples_ubb;

	if (OK!= (wSts= DSP_Copy_rub(&rbDes, &coZ, pubbDes, &coOff, &szOp)))
		goto EXIT_DSP_DeRipples_ubb;

EXIT_DSP_DeRipples_ubb:
	if (OK!= (wMEMSts= MEM_PopAllHeapStatus()))	return wMEMSts;
	return wSts;
}

IMG_WORD	DSP_GEN_SinModelReconstruct(									// image reconstruction by sin model
										IMG_RBUF	const	*prbImg,		// image buffer
										IMG_REAL	const	rPeriod,		// period of the targeted sin. wave
										IMG_UWORD	const	uwHKernel,		// half kernel size
										IMG_RBUF			*prbImgH,		// reconstructed image	(User malloc, can be null)
										IMG_RBUF			*prbAmpH		// ampitude buffer;		(User malloc, can be null)
										)
{
	IMG_WORD
		wMEMSts	= OK,
		wSts	= OK;

	IMG_ULWORD
		ulMEM	= 0;

	IMG_COORD
		coZ		= {0, 0};

	IMG_RBUF
		rbW,												// Omega matrix
		rbIW;												// pesudo inverse matrix

	IppStatus
		ippSts	= ippStsNoErr;

	IppiSize
		ippRoiSize;


	if (NULL== prbImgH && NULL== prbAmpH)	return OK;

	if (NULL== prbImg || 1.0f> rPeriod|| 1> uwHKernel)
		return DSP_ERR_INVALID_ARG;

	// Form reconstruction kenerals
	rbW.size.x= 3;
	rbW.size.y=(IMG_UWORD) ((uwHKernel<< 1)+ 1);
	rbW.linestep= rbW.size.x;

	rbIW.size.x= rbIW.linestep= rbW.size.y;
	rbIW.size.y= rbW.size.x;

	wSts= MEM_PushAllHeapStatus();
	if (wSts< OK)	return wSts;

	ulMEM= rbW.size.x*rbW.size.y;
	if (NULL== (rbW.ptr= (IMG_REAL *) MEM_Malloc((ulMEM<< 1)*sizeof(IMG_REAL), SYS_BANK1_8)))
	{
		wSts= MEM_ERR_MALLOC;
		goto EXIT_DSP_GEN_SinModelReconstruct;
	}
	memset(rbW.ptr, 0, ulMEM<< 1);
	rbIW.ptr= rbW.ptr+ ulMEM;

	wSts= MAT_GetOmegaMatrix(rPeriod, uwHKernel, &rbW);
	if (OK> wSts) goto EXIT_DSP_GEN_SinModelReconstruct;

	wSts= MAT_GetPseudoInverseMatrix(&rbW, &rbIW);
	if (OK> wSts) goto EXIT_DSP_GEN_SinModelReconstruct;

	if (NULL!= prbImgH)
	{
		IMG_RBUF
			rbH;											// H (estimation) matrix

		IMG_REAL
			*prh;											// Kernel operator

		if (prbImg->size.x!= prbImgH->size.x	||
			prbImg->size.y!= prbImgH->size.y)
		{
			wSts= DSP_ERR_INVALID_ARG;
			goto EXIT_DSP_GEN_SinModelReconstruct;
		}

		rbH.size.x= rbH.size.y= rbW.size.y;
		rbH.linestep= rbH.size.x;

		ulMEM= rbH.size.x* rbH.size.y;
		if (NULL == 
			(rbH.ptr= (IMG_REAL *) MEM_Malloc(sizeof(IMG_REAL)*ulMEM, SYS_BANK1_8)))
		{
			wSts= MEM_ERR_MALLOC;
			goto EXIT_DSP_GEN_SinModelReconstruct;
		}
		memset(rbH.ptr, 0, ulMEM);

		ippSts=	ippmMul_mm_32f(
			rbW.ptr , rbW.linestep*sizeof(IMG_REAL) , sizeof(IMG_REAL), rbW.size.x , rbW.size.y,
			rbIW.ptr, rbIW.linestep*sizeof(IMG_REAL), sizeof(IMG_REAL), rbIW.size.x, rbIW.size.y,
			rbH.ptr , rbH.linestep*sizeof(IMG_REAL) , sizeof(IMG_REAL));

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_GEN_SinModelReconstruct;
		}
		prh= rbH.ptr+ uwHKernel*rbH.linestep;


		// ! Warning: since IPP flip the keneral by default!! 
		// The current kernel is sysmetric and hence ignore the flip

		// Reconstruct image
		ippRoiSize.width	= prbImg->size.x- rbIW.size.x+ 1;
		ippRoiSize.height	= prbImg->size.y;
		ippSts= ippiFilterRow_32f_C1R(
			prbImg->ptr + uwHKernel, prbImg->linestep*sizeof(IMG_REAL),
			prbImgH->ptr+ uwHKernel, prbImgH->linestep*sizeof(IMG_REAL), ippRoiSize,
			prh, rbH.size.x, uwHKernel);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_GEN_SinModelReconstruct;
		}
	}

	if (NULL!= prbAmpH)
	{
		IMG_UWORD
			i= 0;

		IMG_RBUF
			rbB;							// B_i=0 is cos, B_i=1 is sin

		IMG_REAL
			*prS= NULL;

		if (prbImg->size.x!= prbAmpH->size.x	||
			prbImg->size.y!= prbAmpH->size.y)
		{
			wSts= DSP_ERR_INVALID_ARG;
			goto EXIT_DSP_GEN_SinModelReconstruct;
		}

		rbB.size.x	= prbAmpH->size.x;
		rbB.size.y	= prbAmpH->size.y;
		rbB.linestep= rbB.size.x;

		ulMEM= rbB.size.x*rbB.size.y;
		if (NULL== (rbB.ptr= (IMG_REAL *) MEM_Malloc(ulMEM*sizeof(IMG_REAL), SYS_BANK1_8)))
		{
			wSts= MEM_ERR_MALLOC;
			goto EXIT_DSP_GEN_SinModelReconstruct;
		}
		memset(rbB.ptr, 0, ulMEM*sizeof(IMG_REAL));

		// ! Warning: since IPP flip the keneral by default!! 
		// The current kernel is symetric or anti symetric and hence ignore the flip

		wSts= DSP_FillBuffer_r(0.0f, prbAmpH, &coZ, &prbAmpH->size);

		for (i= 0; i< 2; i++)
		{
			prS= rbIW.ptr+ (i+ 1)*rbIW.linestep;

			ippRoiSize.width	= prbImg->size.x- rbIW.size.x+ 1;
			ippRoiSize.height	= prbImg->size.y;
			ippSts= ippiFilterRow_32f_C1R(
				prbImg->ptr	+ uwHKernel, prbImg->linestep*sizeof(IMG_REAL),
				rbB.ptr		+ uwHKernel, rbB.linestep*sizeof(IMG_REAL), ippRoiSize,
				prS, rbIW.size.x, uwHKernel);

			if (ippStsNoErr!= ippSts)
			{
				wSts= DSP_ERR_FAIL_IN_IPPI;
				goto EXIT_DSP_GEN_SinModelReconstruct;
			}

			ippRoiSize.width	= prbImg->size.x;
			ippRoiSize.height	= prbImg->size.y;
			ippSts= ippiAddSquare_32f_C1IR(rbB.ptr, rbB.linestep*sizeof(IMG_REAL),
				prbAmpH->ptr, prbAmpH->linestep*sizeof(IMG_REAL), ippRoiSize);

			if (ippStsNoErr!= ippSts)
			{
				wSts= DSP_ERR_FAIL_IN_IPPI;
				goto EXIT_DSP_GEN_SinModelReconstruct;
			}
		}
	}

EXIT_DSP_GEN_SinModelReconstruct:
	wMEMSts= MEM_PopAllHeapStatus();
	if (OK> wMEMSts)	return wMEMSts;
	return wSts;
}

//	The following function is to find the probability if the sinusoidal pattaen
//	exist at each pixel location
IMG_WORD	DSP_CAL_SinParaIniti(
								 DSP_SIN_PARA	*pstPara
								 )
{
	if (NULL== pstPara)	return OK;
	memset(pstPara, 0, sizeof(DSP_SIN_PARA));
	return OK;
}

IMG_WORD	DSP_INS_IsSinusoidalExist(
									  IMG_UBBUF		const	*pubImg,		// input image buffer
									  IMG_COORD		const	coOffset,		// Offset
									  IMG_SIZE		const	szOpsize,		// Opsize
									  DSP_SIN_PARA	const	stPara,			// input Parameter
									  IMG_RBUF				*prbSc,		// 0: not sure freq. sin wave, 1: there is this pattern (User malloc, size = Opsize)
									  IMG_RBUF				*prbRe,			// Reconstructed image (User malloc, size = Opsize),	can be null!
									  IMG_RBUF				*prbAmp			// Amplitude image (User malloc, size = Opsize),		can be null!
									  )
{
	IMG_UBYTE
		ubFlip	= 0;

	IMG_WORD
		wSts	= OK,
		wMEMSts	= OK;

	IMG_UWORD
		i		= 0,
		j		= 0;

	IMG_ULWORD
		ulMEM	= 0;

	IMG_REAL
		rKw		= 0.0f;		// keneral weighting

	IMG_COORD
		coZ		= {0, 0},
		coOff	= {0, 0};

	IMG_SIZE
		szOp	= {0, 0};

	IMG_UBBUF
		ubbImg;

	IMG_RBUF
		rbImg,
		rbImgM,				// averaged image
		rbImgH,				// reconstructing image
		rbAmpH,				// reconstructed sin. wave amplitude
		rbSc;				// score buffer

	IMG_UWORD
		uwHKernel,			// half kernel size
		uwFKernel,
		uwFSKernel;			// full smoothing kernel size

	IMG_REAL
		*prSK	= NULL;

	IppStatus
		ippSts	= ippStsNoErr;

	IppiSize
		ippRoiSize;

	if (NULL== prbSc	||  NULL== prbSc->ptr||
		0== szOpsize.x	||	0== szOpsize.y)	return OK;				// Must malloc this pointer

	if (pubImg->size.x< coOffset.x+ szOpsize.x	||
		pubImg->size.y< coOffset.y+ szOpsize.y	||
		stPara.rAmplitude< 0.0f					|| stPara.rPeriod< 0.0f)
		return DSP_ERR_INVALID_ARG;

	if (stPara.rPeriod< 3)
		return DSP_ERR_INVALID_SIZE;

	wSts= MEM_PushAllHeapStatus();
	if (wSts< OK)	goto EXIT_DSP_INS_IsSinusoidalExist;

	uwFSKernel	= (IMG_UWORD) ((stPara.uwSKernel<< 1)+ 1);			// vertical kernel size
	uwHKernel	= (IMG_UWORD) (stPara.rPeriod+ 0.5f);				// Take 2 period for horizontal kernel size
	uwFKernel	= (IMG_UWORD) ((uwHKernel<< 1)+ 1);

	ulMEM= IMG_MAX(uwFSKernel, uwFKernel);
	if (NULL== 
		(prSK= (IMG_REAL *) MEM_Malloc(ulMEM*sizeof(IMG_REAL), SYS_BANK1_8)))
	{
		wSts= MEM_ERR_MALLOC;
		goto EXIT_DSP_INS_IsSinusoidalExist;
	}

	rKw= 1.0f/(IMG_REAL)uwFSKernel;
	for (i= 0; i< uwFSKernel; i++)
		prSK[i]= rKw;

	ubFlip= (((IMG_LWORD)(stPara.rOrientation+0.5f) % 180) >  45)? 1: 0;
	if (0== ubFlip)												// no flipping is needed
	{
		ubbImg			=*pubImg;								// direct copy
		rbSc			=*prbSc ;								// score buffer

		coOff			= coOffset;
		szOp			= szOpsize;
	}
	else
	{
		ubbImg.size.x	= pubImg->size.y;
		ubbImg.size.y	= pubImg->size.x;
		ubbImg.linestep	= ubbImg.size.x;

		rbSc.size.x		= prbSc->size.y;
		rbSc.size.y		= prbSc->size.x;
		rbSc.linestep	= rbSc.size.x;

		ulMEM= ubbImg.size.x* ubbImg.size.y;
		if (NULL== 	(ubbImg.ptr	= (IMG_UBYTE *) MEM_Malloc((ulMEM)*sizeof(IMG_UBYTE), SYS_BANK1_8))||
			NULL==	(rbSc.ptr	= (IMG_REAL  *) MEM_Malloc((rbSc.size.x*rbSc.size.y)*sizeof(IMG_REAL ), SYS_BANK1_8)))
		{
			wSts= MEM_ERR_MALLOC;
			goto EXIT_DSP_INS_IsSinusoidalExist;
		}

		ippRoiSize.width	= pubImg->size.x;
		ippRoiSize.height	= pubImg->size.y;
		ippSts= ippiTranspose_8u_C1R(pubImg->ptr, pubImg->linestep*sizeof(IMG_UBYTE),
			ubbImg.ptr, ubbImg.linestep*sizeof(IMG_UBYTE), ippRoiSize);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_INS_IsSinusoidalExist;
		}

		coOff.x			= coOffset.y;
		coOff.y			= coOffset.x;
		szOp.x			= szOpsize.y;
		szOp.y			= szOpsize.x;
	}
	rbImg.size		= rbImgM.size		= rbImgH.size		= rbAmpH.size		= szOp;
	rbImg.linestep	= rbImgM.linestep	= rbImgH.linestep	= rbAmpH.linestep	= szOp.x;

	ulMEM= szOp.x*szOp.y;
	if (NULL== 
		(rbImg.ptr= (IMG_REAL *) MEM_Malloc((ulMEM<<2)*sizeof(IMG_REAL), SYS_BANK1_8)))
	{
		wSts= MEM_ERR_MALLOC;
		goto EXIT_DSP_INS_IsSinusoidalExist;
	}
	rbImgM.ptr= rbImg.ptr + ulMEM;
	rbImgH.ptr= rbImgM.ptr+ ulMEM;
	rbAmpH.ptr= rbImgH.ptr+ ulMEM;

	// Convert ubyte buffer to real buffer,
	ippRoiSize.width	= rbImg.size.x;
	ippRoiSize.height	= rbImg.size.y;
	ippSts=	ippiConvert_8u32f_C1R(
		ubbImg.ptr+ coOff.y*ubbImg.linestep+ coOff.x, ubbImg.linestep*sizeof(IMG_UBYTE),
		rbImg.ptr, rbImg.linestep*sizeof(IMG_REAL), ippRoiSize);

	memcpy(rbImgM.ptr, rbImg.ptr, ulMEM*sizeof(IMG_REAL));
	memcpy(rbImgH.ptr, rbImg.ptr, ulMEM*sizeof(IMG_REAL));
	memset(rbAmpH.ptr, 0, ulMEM*sizeof(IMG_REAL));

	// Veritical filtering take rbImgM as a temp to get veritically filtered img
	ippRoiSize.width	= rbImg.size.x;
	ippRoiSize.height	= rbImg.size.y- uwFSKernel+ 1;
	ippSts= ippiFilterColumn_32f_C1R(
		rbImgM.ptr+ stPara.uwSKernel, rbImgM.linestep*sizeof(IMG_REAL),
		rbImg.ptr + stPara.uwSKernel, rbImg.linestep*sizeof(IMG_REAL) , ippRoiSize,
		prSK, uwFSKernel, stPara.uwSKernel);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_INS_IsSinusoidalExist;
	}

	wSts= DSP_GEN_SinModelReconstruct(
		&rbImg, stPara.rPeriod, uwHKernel,
		&rbImgH, &rbAmpH);

	if (OK!= wSts)
		goto EXIT_DSP_INS_IsSinusoidalExist;

	// Take row averaging as an refer image
	rKw= 1.0f/(IMG_REAL)uwFKernel;
	for (i= 0; i< uwFKernel; i++)
		prSK[i]= rKw;

	ippRoiSize.width	= rbImg.size.x- uwFKernel+ 1;
	ippRoiSize.height	= rbImg.size.y;
	ippSts= ippiFilterRow_32f_C1R(
		rbImg.ptr +uwHKernel , rbImg.linestep*sizeof(IMG_REAL),
		rbImgM.ptr+uwHKernel, rbImgM.linestep*sizeof(IMG_REAL), ippRoiSize,
		prSK, uwFKernel, uwHKernel);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_INS_IsSinusoidalExist;
	}

	// Computing the score
	{
		IMG_REAL
			rTmp	= 0.0f,		// temporary value
			rSST	= 0.0f,		// sum of total error
			rSSE	= 0.0f,		// sum of fitting error
			rAmp2	=  stPara.rAmplitude*stPara.rAmplitude;

		IMG_REAL
			m	= (IMG_REAL)uwFKernel- 3.0f,
			n	= 2.0f,
			rQ	= 0.0f,
			rQm	= 0.0f,
			rVar= 0.0f;

		IMG_REAL
			*prI	= rbImg.ptr	+ stPara.uwSKernel*rbImg.linestep	+ uwHKernel,		// Original image 
			*prM	= rbImgM.ptr+ stPara.uwSKernel*rbImgM.linestep	+ uwHKernel,		// Image by averaging
			*prH	= rbImgH.ptr+ stPara.uwSKernel*rbImgH.linestep	+ uwHKernel,		// Reconstructed image
			*prA	= rbAmpH.ptr+ stPara.uwSKernel*rbAmpH.linestep	+ uwHKernel,		// Amplitude buffer
			*prS	= rbSc.ptr	+ stPara.uwSKernel*rbSc.linestep	+ uwHKernel;		// Score buffer

		wSts= DSP_FillBuffer_r(0.0f, &rbSc, &coZ, &rbSc.size);

		rQm = m/(m-2);
		rVar= 2*m*m*(m+n-2)/(n*(m-2)*(m-2)*(m-4)*rQm*rQm);

		for (i= 0;	i< rbImg.size.y- uwFSKernel+ 1;	i++)
		{
			rSST= 0.0f;
			rSSE= 0.0f;

			for (j= 0; j< uwFKernel; j++)								// Get starting value of a row
			{
				rTmp = *(prH+ j- uwHKernel)- *(prI+ j- uwHKernel);
				rSSE+= rTmp*rTmp;

				rTmp = *(prM+ j- uwHKernel)- *(prI+ j- uwHKernel);
				rSST+= rTmp*rTmp;
			}
			if (prA[0]< rAmp2)		prS[0]= 0.0f;
			else if (rSST< rSSE)	prS[0]= 0.0f;
			else
			{
				rQ		= (m*(rSST- rSSE)+ 0.00001f)/(n*rSSE+ 0.00001f);
				rTmp	= (IMG_REAL) log(rQ/rQm);
				prS[0]	= 1.0f- (IMG_REAL) exp(-rTmp*rTmp)/rVar;
			}

			for (j= 1; j< rbImg.size.x- uwFKernel+ 1; j++)				// moving average
			{
				rTmp = *(prH+ j+ uwHKernel	)- *(prI+ j+ uwHKernel	);
				rSSE+= rTmp*rTmp;
				rTmp = *(prH+ j- uwHKernel-1)- *(prI+ j- uwHKernel-1);
				rSSE-= rTmp*rTmp;


				rTmp = *(prM+ j+ uwHKernel	)- *(prI+ j+ uwHKernel	);
				rSST+= rTmp*rTmp;
				rTmp = *(prM+ j- uwHKernel-1)- *(prI+ j- uwHKernel-1);
				rSST-= rTmp*rTmp;

				if (prA[j]< rAmp2)		prS[j]= 0.0f;
				else if (rSST< rSSE)	prS[j]= 0.0f;
				else
				{
					rQ		= (m*(rSST- rSSE)+ 0.00001f)/(n*rSSE+ 0.00001f);
					rTmp	= (IMG_REAL) log(rQ/rQm);
					prS[j]	= 1.0f- (IMG_REAL) exp(-rTmp*rTmp)/rVar;
				}
			}

			prI+= rbImg.linestep;
			prM+= rbImgM.linestep;
			prH+= rbImgH.linestep;
			prA+= rbAmpH.linestep;
			prS+= rbSc.linestep;
		}
	}

	if (0< ubFlip)												// flipping score is needed
	{
		ippRoiSize.width	= rbSc.size.x;
		ippRoiSize.height	= rbSc.size.y;
		ippSts= ippiTranspose_32s_C1R((Ipp32s *) rbSc.ptr, rbSc.linestep*sizeof(IMG_REAL),
			(Ipp32s *) prbSc->ptr, prbSc->linestep*sizeof(IMG_REAL), ippRoiSize);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_INS_IsSinusoidalExist;
		}

		coOff.x			= coOffset.y;
		coOff.y			= coOffset.x;
		szOp.x			= szOpsize.y;
		szOp.y			= szOpsize.x;
	}

	if (NULL!= prbRe && NULL!= prbRe->ptr)
	{
		ippRoiSize.width	= rbImgH.size.x;
		ippRoiSize.height	= rbImgH.size.y;

		if (0== ubFlip)
			ippSts= ippiCopy_32f_C1R(				rbImgH.ptr, rbImgH.linestep*sizeof(IMG_REAL),
			prbRe->ptr, prbRe->linestep*sizeof(IMG_REAL), ippRoiSize);
		else												// flipping image is needed
			ippSts= ippiTranspose_32s_C1R((Ipp32s *)rbImgH.ptr, rbImgH.linestep*sizeof(IMG_REAL),
			(Ipp32s *)prbRe->ptr, prbRe->linestep*sizeof(IMG_REAL), ippRoiSize);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_INS_IsSinusoidalExist;
		}
	}

	if (NULL!= prbAmp)
	{
		ippRoiSize.width	= rbAmpH.size.x;
		ippRoiSize.height	= rbAmpH.size.y;

		if (0== ubFlip)
			ippSts= ippiCopy_32f_C1R(				rbAmpH.ptr, rbAmpH.linestep*sizeof(IMG_REAL),
			prbAmp->ptr, prbAmp->linestep*sizeof(IMG_REAL), ippRoiSize);
		else												// flipping image is needed
			ippSts= ippiTranspose_32s_C1R((Ipp32s *)rbAmpH.ptr, rbAmpH.linestep*sizeof(IMG_REAL),
			(Ipp32s *)prbAmp->ptr , prbAmp->linestep*sizeof(IMG_REAL), ippRoiSize);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_INS_IsSinusoidalExist;
		}
	}

EXIT_DSP_INS_IsSinusoidalExist:
	if (OK!= (wMEMSts= MEM_PopAllHeapStatus()))	return wMEMSts;
	return wSts;
}

#define		DSP_ESP		(1e-5)
IMG_WORD	DSP_INS_FindConvexMaximum(
									  IMG_RBUF	const	*prbSc0,		// Surface buffer
									  IMG_COORD	const	coOff,			// Offset
									  IMG_SIZE	const	szOp,			// Offsize
									  IMG_RCOORD		*prcoCM,		// convex maximum
									  IMG_REAL			*prScore		// get poostion value
									  )
{
	IMG_UBYTE
		ubFilterX= 1,					// filter size x
		ubFilterY= 1;					// filter size y

	IMG_WORD
		wSts	= OK,
		wMEMSts	= OK;

	IMG_UWORD
		i		= 0,
		uwHFx	= 0,
		uwHFy	= 0,
		uwHF	= 0;

	IMG_ULWORD
		ulMEM= 0;

	IMG_REAL
		arAAF[3]= {0.3333f, 0.3334f, 0.3333f};

	IMG_COORD
		coZ= {0, 0};

	IMG_RBUF
		rbScp,
		rbTmp,
		rbSc;

	IppStatus 
		ippSts;

	IppiSize
		ippRoiSize,
		ippRoiSizeR;

	IppiRect
		ippROI;

	IMG_REAL
		*prF = NULL;

	IMG_RBUF
		*prbSc= NULL;

	if (NULL== prcoCM)	return OK;

	if (NULL== prbSc0|| NULL== prbSc0->ptr)
		return DSP_ERR_INVALID_ARG;

	prbSc= &rbScp;
	rbScp.linestep	= prbSc0->linestep;
	rbScp.ptr		= prbSc0->ptr+ (coOff.y*prbSc0->linestep+ coOff.x);
	rbScp.size.x	= szOp.x;
	rbScp.size.y	= szOp.y;

	//if (szOp.x> 20)	ubFilterX= 1;		// much larger than 2
	//if (szOp.y> 20)	ubFilterY= 1;		// much larger than 2
	ubFilterX= 1;							// much larger than 2
	ubFilterY= 1;							// much larger than 2

	wSts= MEM_PushAllHeapStatus();
	if (OK!= wSts)	return wSts;

	rbSc.size		= prbSc->size;
	rbSc.linestep	= rbSc.size.x;

	rbTmp.size		= prbSc->size;
	rbTmp.linestep	= rbTmp.size.x;

	ulMEM= rbSc.size.x*rbSc.size.y;
	if (NULL == (rbSc.ptr = (IMG_REAL *) MEM_Malloc(sizeof(IMG_REAL)*(ulMEM<< 1), SYS_BANK1_8)))
	{
		wSts= MEM_ERR_MALLOC;
		goto EXIT_DSP_INS_FindConvexMaximum;
	}
	rbTmp.ptr= rbSc.ptr+ ulMEM;

	ippRoiSize.width	= prbSc->size.x;
	ippRoiSize.height	= prbSc->size.y;
	ippSts= ippiCopy_32f_C1R(prbSc->ptr, prbSc->linestep*sizeof(IMG_REAL),
		rbTmp.ptr, rbTmp.linestep*sizeof(IMG_REAL), ippRoiSize);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_INS_FindConvexMaximum;
	}

	//if (0< ubFilterX)
	{
		ippRoiSize.width	= prbSc->size.x- (ubFilterX<< 1);
		ippRoiSize.height	= prbSc->size.y;
		ippSts= ippiFilterRow_32f_C1R(
			prbSc->ptr+ ubFilterX, prbSc->linestep*sizeof(IMG_REAL),
			rbTmp.ptr + ubFilterX,  rbTmp.linestep*sizeof(IMG_REAL),
			ippRoiSize,	arAAF, (ubFilterX<< 1)+ 1, ubFilterX);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_INS_FindConvexMaximum;
		}
	}

	ippRoiSize.width	= rbTmp.size.x;
	ippRoiSize.height	= rbTmp.size.y;
	ippSts= ippiCopy_32f_C1R(rbTmp.ptr, rbTmp.linestep*sizeof(IMG_REAL),
		rbSc.ptr, rbSc.linestep*sizeof(IMG_REAL), ippRoiSize);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_INS_FindConvexMaximum;
	}

	//if (0< ubFilterY)
	{
		ippRoiSize.width	= prbSc->size.x;
		ippRoiSize.height	= prbSc->size.y- (ubFilterY<< 1);
		ippSts= ippiFilterColumn_32f_C1R(
			rbTmp.ptr+ ubFilterY*rbTmp.linestep	, rbTmp.linestep*sizeof(IMG_REAL),
			rbSc.ptr + ubFilterY*rbSc.linestep	,  rbSc.linestep*sizeof(IMG_REAL),
			ippRoiSize,	arAAF, (ubFilterY<< 1)+ 1, ubFilterY);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_INS_FindConvexMaximum;
		}
	}

	if (OK!= (wSts= DSP_FillBuffer_r( 0.0f,	&rbTmp , &coZ, &rbTmp.size )))
		goto EXIT_DSP_INS_FindConvexMaximum;

	ippRoiSize.width	= rbSc.size.x;
	ippRoiSize.height	= rbSc.size.y;

	ippROI.x		= 0;
	ippROI.y		= 0;
	ippROI.width	= ippRoiSize.width;
	ippROI.height	= ippRoiSize.height;

	ippRoiSizeR.width = rbTmp.size.x>> 1;
	ippRoiSizeR.height= rbTmp.size.y>> 1;

	uwHFx= rbTmp.size.x>> 2;
	uwHFy= rbTmp.size.y>> 2;

	ippSts= ippiResize_32f_C1R(
		rbSc.ptr , ippRoiSize, rbSc.linestep*sizeof(IMG_REAL), ippROI,
		rbTmp.ptr+ uwHFy*rbTmp.linestep+ uwHFx, rbTmp.linestep*sizeof(IMG_REAL), ippRoiSizeR, 
		1.0f/(IMG_REAL)(ubFilterX<< 1), 1.0f/(IMG_REAL)(ubFilterY<< 1), IPPI_INTER_LINEAR);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_INS_FindConvexMaximum;
	}

	uwHF = IMG_MAX(uwHFx, uwHFy);
	ulMEM= (uwHF<< 1)+ 1;
	if (NULL== (prF= (IMG_REAL *) MEM_Malloc(ulMEM*sizeof(IMG_REAL), SYS_BANK1_8)))
	{
		wSts= MEM_ERR_MALLOC;
		goto EXIT_DSP_INS_FindConvexMaximum;
	}

	// find x- maximum first
	memset(prF, 0, ulMEM*sizeof(IMG_REAL));
	for (i= 0; i< uwHFx; i++)		prF[i]= (IMG_REAL) i;
	prF[i]= (IMG_REAL) uwHFx;

	uwHF= (IMG_UWORD) ((uwHFx<< 1)+ 1);
	for (i= uwHFx+ 1; i< uwHF; i++)	prF[i]= (IMG_REAL) (uwHF- i- 1);

	if (OK!= (wSts= DSP_FillBuffer_r( 0.0f,	&rbSc , &coZ, &rbSc.size )))
		goto EXIT_DSP_INS_FindConvexMaximum;

	ippSts= ippiFilterRow_32f_C1R(
		rbTmp.ptr+ uwHFy*rbTmp.linestep+ uwHFx, rbTmp.linestep*sizeof(IMG_REAL),
		rbSc.ptr + uwHFy*rbSc.linestep + uwHFx,  rbSc.linestep*sizeof(IMG_REAL),
		ippRoiSizeR, prF, uwHF, uwHFx);

	// then find y- maximum
	memset(prF, 0, ulMEM*sizeof(IMG_REAL));
	for (i= 0; i< uwHFy; i++)		prF[i]= (IMG_REAL) i;
	prF[i]= (IMG_REAL) uwHFy;

	uwHF= (IMG_UWORD)((uwHFy<< 1)+ 1);
	for (i= uwHFy+ 1; i< uwHF; i++)	prF[i]= (IMG_REAL) (uwHF- i- 1);

	if (OK!= (wSts= DSP_FillBuffer_r( 0.0f,	&rbTmp , &coZ, &rbTmp.size )))
		goto EXIT_DSP_INS_FindConvexMaximum;

	ippSts= ippiFilterColumn_32f_C1R(
		rbSc.ptr + uwHFy*rbSc.linestep + uwHFx,  rbSc.linestep*sizeof(IMG_REAL),
		rbTmp.ptr+ uwHFy*rbTmp.linestep+ uwHFx, rbTmp.linestep*sizeof(IMG_REAL),
		ippRoiSizeR, prF, uwHF, uwHFy);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_INS_FindConvexMaximum;
	}


	// find max. location
	{
		IMG_REAL
			rMax= 0.0f,
			rb	= 0.0f,
			r2a	= 0.0f,
			rD	= 0.0f;	// rAdjustment

		int
			iX	= 0,
			iY	= 0;

		ippSts= ippiMaxIndx_32f_C1R(
			rbTmp.ptr+ uwHFy*rbTmp.linestep+ uwHFx, rbTmp.linestep*sizeof(IMG_REAL),
			ippRoiSizeR, &rMax, &iX, &iY);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_INS_FindConvexMaximum;
		}

		prcoCM->x= (IMG_REAL) coOff.x;
		prcoCM->y= (IMG_REAL) coOff.y;

		prcoCM->x+= (IMG_REAL) (iX)*(int)(1<< ubFilterX);
		prcoCM->y+= (IMG_REAL) (iY)*(int)(1<< ubFilterY);
		// subpixel adjustment
		// X
		r2a=rbTmp.ptr[(uwHFy+ iY)*rbTmp.linestep+ (uwHFx+ iX+1)]+
			rbTmp.ptr[(uwHFy+ iY)*rbTmp.linestep+ (uwHFx+ iX-1)]- 
			rbTmp.ptr[(uwHFy+ iY)*rbTmp.linestep+ (uwHFx+ iX  )]*2.0f;

		rb =0.5f*(rbTmp.ptr[(uwHFy+ iY)*rbTmp.linestep+ (uwHFx+ iX+ 1)]-
			rbTmp.ptr[(uwHFy+ iY)*rbTmp.linestep+ (uwHFx+ iX- 1)]);

		rD= -rb/r2a;

		if (DSP_ESP< fabs(r2a)&& 1.0f> fabs(rD))
			prcoCM->x+= rD;

		// Y
		r2a=rbTmp.ptr[(uwHFy+ iY+1)*rbTmp.linestep+ (uwHFx+ iX)]+
			rbTmp.ptr[(uwHFy+ iY-1)*rbTmp.linestep+ (uwHFx+ iX)]- 
			rbTmp.ptr[(uwHFy+ iY  )*rbTmp.linestep+ (uwHFx+ iX)]*2.0f;

		rb =0.5f*(rbTmp.ptr[(uwHFy+ iY+ 1)*rbTmp.linestep+ (uwHFx+ iX)]-
			rbTmp.ptr[(uwHFy+ iY- 1)*rbTmp.linestep+ (uwHFx+ iX)]);

		rD= -rb/r2a;

		if (DSP_ESP< fabs(r2a)&& 1.0f> fabs(rD))
			prcoCM->y+= rD;
	}

	if (NULL!= prScore)
	{
		IMG_REAL
			rRx= 0.0f,
			rRy= 0.0f;

		IMG_COORD
			coCM= {0, 0};

		coCM.x= (IMG_WORD) prcoCM->x;
		coCM.y= (IMG_WORD) prcoCM->y;

		rRx= prcoCM->x- (IMG_REAL)coCM.x;
		rRy= prcoCM->y- (IMG_REAL)coCM.y;

		*prScore=
			(1.0f- rRx)*(1.0f- rRy)*prbSc0->ptr[(coCM.y   )*prbSc0->linestep+ (coCM.x   )]+
			(rRx -0.0f)*(1.0f- rRy)*prbSc0->ptr[(coCM.y   )*prbSc0->linestep+ (coCM.x+ 1)]+
			(rRx -0.0f)*(rRy -0.0f)*prbSc0->ptr[(coCM.y+ 1)*prbSc0->linestep+ (coCM.x+ 1)]+
			(1.0f- rRx)*(rRy -0.0f)*prbSc0->ptr[(coCM.y+ 1)*prbSc0->linestep+ (coCM.x   )];
	}

EXIT_DSP_INS_FindConvexMaximum:
	if (OK!= (wMEMSts= MEM_PopAllHeapStatus()))	return wMEMSts;
	return wSts;
}

IMG_VVOID	DSP_CAL_SinInfoIniti(
								 DSP_SININFO_PARA	*pstPara
								 )
{
	if (NULL== pstPara)	return;
	pstPara->rFreqRange		= 0.005f;
	pstPara->rThreshold		= 1.0f;
	pstPara->uwPreference	= 0;
	pstPara->rMinFreqX		= 0.0f;
	pstPara->rMinFreqY		= 0.0f;
	return;
}

IMG_WORD	DSP_CAL_SinInfo(
							IMG_UBBUF			const	*pubbSrc,
							IMG_COORD			const	coOff,
							IMG_SIZE			const	szOp,
							DSP_SININFO_PARA	const	stPara,
							DSP_SIN_INFO				*pstInfo
							)
{
	IMG_UBYTE
		ubPeakFound= 0;

	IMG_WORD
		wSts     = OK,
		wMEMSts         = OK;

	IMG_UWORD
		uwMinFFTX= 0,                         // max. FFT x limit
		uwMinFFTY= 0,                         // max. FFT y limit
		uwMaxFFTX= 0,                         // max. FFT x limit
		uwMaxFFTY= 0,                         // max. FFT y limit
		x= 0,
		y= 0,
		xp= 0,
		yp= 0;

	IMG_LWORD
		lwOrderX= 0,			// Order X
		lwOrderY= 0,			// Order Y
		lwOrder = 0;

	IMG_ULWORD
		ulMEM  = 0;

	IMG_REAL
		rFreqBx = 0,                              // frequency border
		rFreqBy = 0,                              // frequency border
		rTol       = 0.25f*stPara.rThreshold;

	IMG_RBUF
		rbSrc,					// real image buffer
		rbDst,
		rbAmp;					// real down sampling buffer

	IppStatus
		ippSts	= ippStsNoErr;

	IppiSize
		ippRoiSize;

//	IppiPoint
//		ippPeak= {0, 0};

	IppiFFTSpec_R_32f
		*pFSpec= NULL;

	IMG_REAL
		*prFreqAmp= NULL;


	if (NULL== pstInfo)	return OK;
	memset(pstInfo, 0, sizeof(DSP_SIN_INFO));
	if (NULL== pubbSrc        || NULL== pubbSrc->ptr) return DSP_ERR_INVALID_ARG;

	if (OK!= (wMEMSts= MEM_PushAllHeapStatus()))           return wMEMSts;

	ulMEM= szOp.x*szOp.y;
	rbSrc.size= szOp;
	rbSrc.linestep= rbSrc.size.x;

	rbDst.size= rbSrc.size;
	rbDst.linestep= rbDst.size.x;

	if (NULL== (rbSrc.ptr= (IMG_REAL *) MEM_Malloc((ulMEM<< 1)*sizeof(IMG_REAL), SYS_BANK1_8)))
	{
		wSts= MEM_ERR_MALLOC;
		goto EXIT_DSP_CAL_SinInfo;
	}
	rbDst.ptr= rbSrc.ptr+ ulMEM;

	// Convert ubyte buffer to real buffer,
	ippRoiSize.width            = szOp.x;
	ippRoiSize.height           = szOp.y;

	ippSts= ippiConvert_8u32f_C1R(
		pubbSrc->ptr+ coOff.y*pubbSrc->linestep+ coOff.x, pubbSrc->linestep*sizeof(IMG_UBYTE),
		rbSrc.ptr, rbSrc.linestep*sizeof(IMG_REAL), ippRoiSize);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_CAL_SinInfo;
	}

	// Do FFT and get magnitude
	{
		IMG_REAL
			rPeakValue= 0.0f;

		lwOrderX= (IMG_LWORD)(log((double)rbSrc.size.x)/log(2.0));
		lwOrderY= (IMG_LWORD)(log((double)rbSrc.size.y)/log(2.0));

		ippiFFTInitAlloc_R_32f(&pFSpec,	lwOrderX, lwOrderY,
			IPP_FFT_DIV_FWD_BY_N, ippAlgHintAccurate);

		ippSts = ippiFFTFwd_RToPack_32f_C1R(
			rbSrc.ptr, rbSrc.linestep*sizeof(IMG_REAL),
			rbDst.ptr, rbDst.linestep*sizeof(IMG_REAL),
			pFSpec, NULL);

		ippiFFTFree_R_32f( pFSpec );

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_CAL_SinInfo;
		}

		ippRoiSize.width            = szOp.x;
		ippRoiSize.height           = szOp.y;
		ippSts= ippiMagnitudePack_32f_C1R(
			rbDst.ptr, rbDst.linestep*sizeof(IMG_REAL),
			rbSrc.ptr, rbSrc.linestep*sizeof(IMG_REAL),
			ippRoiSize);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_CAL_SinInfo;
		}

		// Find local maximum / minimum
		// ==================== Rescaling  =================
		{
			IMG_UWORD
				xf= 0,
				yf= 0;

			IMG_LREAL
				lrTmp;
			
			IMG_SIZE
				szFFT	= {0, 0},
				szFZ	= {0, 0};		// full size

			IppiSize
				ippSSize= {0, 0};

			lwOrder = IMG_MIN(lwOrderX, lwOrderY);
			lwOrder	= IMG_MIN(lwOrder, 7);

			szFFT.x = (IMG_UWORD) (1<< (IMG_UWORD) lwOrderX);
			szFFT.y = (IMG_UWORD) (1<< (IMG_UWORD) lwOrderY);

			szFZ.x	= (IMG_UWORD) (1<< (IMG_UWORD)(lwOrderX- lwOrder));
			szFZ.y	= (IMG_UWORD) (1<< (IMG_UWORD)(lwOrderY- lwOrder));

			ippSSize.width = szFZ.x;
			ippSSize.height= szFZ.y;

			rbAmp.linestep= rbAmp.size.x= rbAmp.size.y= (IMG_UWORD) (1<< lwOrder);
			if (NULL== (rbAmp.ptr= (IMG_REAL *)MEM_Malloc(rbAmp.size.x*rbAmp.size.y*sizeof(IMG_REAL), SYS_BANK1_8)))
			{
				wSts= MEM_ERR_MALLOC;
				goto EXIT_DSP_CAL_SinInfo;
			}

			for (y= 0, yf= 0; y< (szFFT.y- szFZ.y); y+= szFZ.y, yf++)
			{
				for (x= 0, xf= 0; x< (szFFT.x- szFZ.x); x+= szFZ.x, xf++)
				{
					if (ippStsNoErr!= ippiSum_32f_C1R(
						rbSrc.ptr+ y*rbSrc.linestep+ x, rbSrc.linestep*sizeof(IMG_REAL),
						ippSSize, &lrTmp, ippAlgHintNone))
					{
						wSts= DSP_ERR_FAIL_IN_IPPI;
						goto EXIT_DSP_CAL_SinInfo;
					}
					*(rbAmp.ptr+ yf*rbAmp.linestep+ xf)= (IMG_REAL) lrTmp;
				}
			}
		}
		// ================== Rescale End  ================


		ippRoiSize.width	= (1<< (lwOrder- 1));
		ippRoiSize.height	= (1<< (lwOrder- 1));

		rFreqBx= (IMG_REAL) (stPara.rFreqRange*(IMG_REAL)(IMG_UWORD)(1<< lwOrder));
		rFreqBx= IMG_MAX(1.0f, rFreqBx);

		rFreqBy= (IMG_REAL) (stPara.rFreqRange*(IMG_REAL)(IMG_UWORD)(1<< lwOrder));
		rFreqBy= IMG_MAX(1.0f, rFreqBy);

		uwMaxFFTX= (IMG_UWORD)(ippRoiSize.width -1);
		uwMaxFFTY= (IMG_UWORD)(ippRoiSize.height-1);
		switch (stPara.uwPreference)
		{
		case DSP_SININFO_XDOMINATE:
			uwMaxFFTY= IMG_MIN(uwMaxFFTY, (IMG_UWORD)rFreqBy+ 1);
			break;
		case DSP_SININFO_YDOMINATE:
			uwMaxFFTX= IMG_MIN(uwMaxFFTX, (IMG_UWORD)rFreqBx+ 1);
			break;
		default:
			break;
		}

		uwMinFFTX= (IMG_UWORD) (stPara.rMinFreqX*(IMG_REAL)(IMG_LWORD)(1<< lwOrder)+ 0.5f);
		uwMinFFTY= (IMG_UWORD) (stPara.rMinFreqY*(IMG_REAL)(IMG_LWORD)(1<< lwOrder)+ 0.5f);

		if (uwMinFFTX> uwMaxFFTX || uwMinFFTY> uwMaxFFTY)
		{
			wSts= DSP_ERR_INVALID_ARG;
			goto EXIT_DSP_CAL_SinInfo;
		}

		prFreqAmp= rbAmp.ptr+ uwMinFFTY*rbAmp.linestep+ uwMinFFTX;
		for (y= uwMinFFTY; y< uwMaxFFTY           ; y++)
		{
			for (x= uwMinFFTX; x< uwMaxFFTX            ; x++, prFreqAmp++)
			{
				if ((0==		x		&&	rFreqBy		< y)		&&					// within bound	to skip low frequency term
					*prFreqAmp	> rTol	&&	*prFreqAmp	> rPeakValue&&					// value is large
					*prFreqAmp>=*(prFreqAmp+1)						&&					// local maximum along x
					*prFreqAmp>=*(prFreqAmp-rbAmp.linestep) &&	*prFreqAmp>=*(prFreqAmp+rbAmp.linestep))		// local maximum along y
				{
					rPeakValue= *prFreqAmp;
					xp= x;	yp= y;
					ubPeakFound++;
				}
				else if	((rFreqBx	< x	&&	0		   == y)		&&					// within bound	to skip low frequency term
					*prFreqAmp	> rTol	&&	*prFreqAmp	> rPeakValue&&					// value is large
					*prFreqAmp>=*(prFreqAmp-1) && *prFreqAmp>=*(prFreqAmp+1) &&			// local maximum along x
					*prFreqAmp>=*(prFreqAmp+rbAmp.linestep))							// local maximum along y
				{
					rPeakValue= *prFreqAmp;
					xp= x;	yp= y;
					ubPeakFound++;
				}
				else if ((rFreqBx	< x	||	rFreqBy		< y)		&&					// within bound	to skip low frequency term
					0	< x				&&	0<	y					&&
					*prFreqAmp	> rTol	&&	*prFreqAmp	> rPeakValue&&					// value is large
					*prFreqAmp>=*(prFreqAmp-1) && *prFreqAmp>=*(prFreqAmp+1) &&			// local maximum along x
					*prFreqAmp>=*(prFreqAmp-rbAmp.linestep) &&	*prFreqAmp>=*(prFreqAmp+rbAmp.linestep))		// local maximum along y
				{
					rPeakValue= *prFreqAmp;
					xp= x;	yp= y;
					ubPeakFound++;
				}
			}
			prFreqAmp+= (rbAmp.linestep- x+ uwMinFFTX);
		}
	}

	pstInfo->rA = rbAmp.ptr[0];

	if (0== ubPeakFound)	goto EXIT_DSP_CAL_SinInfo;
	{
		IMG_REAL
			r2a= 0.0f,
			rb = 0.0f,
			rD = 0.0f;

		if (0< xp)
		{
			rb = 0.5f*(rbAmp.ptr[yp*rbAmp.linestep+ xp+ 1]- rbAmp.ptr[yp*rbAmp.linestep+ xp- 1]);
			r2a=rbAmp.ptr[yp*rbAmp.linestep+ xp+ 1]+
				rbAmp.ptr[yp*rbAmp.linestep+ xp- 1]-
				rbAmp.ptr[yp*rbAmp.linestep+ xp]*2.0f;
		}

		rD= 0.0f;
		if (DSP_ESP< fabs(r2a))
		{
			rD= -rb/r2a;
			if (1.0f< fabs(rD)) rD= 0.0f;
		}

		pstInfo->rfx=  ((IMG_REAL) xp+ rD)/(IMG_REAL)(IMG_UWORD)(1<< lwOrder);

		if (0< yp)
		{
			rb = 0.5f*(
				rbAmp.ptr[(yp+1)*rbAmp.linestep+ xp]- 
				rbAmp.ptr[(yp-1)*rbAmp.linestep+ xp]);

			r2a=rbAmp.ptr[(yp+1)*rbAmp.linestep+ xp]+
				rbAmp.ptr[(yp-1)*rbAmp.linestep+ xp]-
				rbAmp.ptr[yp*rbAmp.linestep+ xp]*2.0f;
		}

		rD= 0.0f;
		if (DSP_ESP< fabs(r2a))
		{
			rD= -rb/r2a;
			if (1.0f< fabs(rD)) rD= 0.0f;
		}
		pstInfo->rfy= ((IMG_REAL) yp+ rD)/(IMG_REAL)(IMG_UWORD)(1<< lwOrder);
	}

	// Compute the amplitude
	{
		IMG_LREAL
			lrMean,
			lrStd;

		DSP_SIN_PARA
			stSinPara;

		IMG_UWORD
			uwKx = 0,
			uwKy = 0;

		stSinPara.uwSKernel = 1;
		stSinPara.rAmplitude= stPara.rThreshold;

		if (pstInfo->rfx>= pstInfo->rfy)
		{
			stSinPara.rOrientation= 0;
			stSinPara.rPeriod             = 1/pstInfo->rfx;
		}
		else
		{
			stSinPara.rOrientation= 90;
			stSinPara.rPeriod             = 1/pstInfo->rfy;
		}

		wSts= DSP_INS_IsSinusoidalExist(
			pubbSrc, coOff, szOp,
			stSinPara,
			&rbDst, NULL, &rbSrc);              // Use Src is the amplitude 

		if (OK!= wSts) goto EXIT_DSP_CAL_SinInfo;

		uwKx= (IMG_UWORD) (stSinPara.rPeriod+ 0.5f);
		uwKx= uwKx>> 1;
		uwKy= stSinPara.uwSKernel;

		ippRoiSize.width            = rbSrc.size.x- (uwKx<< 1);
		ippRoiSize.height           = rbSrc.size.y- (uwKy<< 1);

		ippSts= ippiMean_StdDev_32f_C1R(
			rbSrc.ptr+ (uwKy*rbSrc.linestep+ uwKx), rbSrc.linestep*sizeof(IMG_REAL),
			ippRoiSize, &lrMean, &lrStd);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_CAL_SinInfo;
		}

		pstInfo->rB                    = (IMG_REAL) sqrt(lrMean);
		pstInfo->rB_SD  = (IMG_REAL) (0.5*lrStd/pstInfo->rB);
	}

EXIT_DSP_CAL_SinInfo:
	if(OK!= (wMEMSts= MEM_PopAllHeapStatus()))  return wMEMSts;
	return wSts;
}

IMG_VVOID	DSP_CAL_PPSinInfoIniti(
								   DSP_PPSININFO_PARA	*pstPara
								   )
{
	if (NULL== pstPara)	return;

	pstPara->uwMultiPX= 5;
	pstPara->uwFilterY= 0;
}

IMG_WORD	DSP_CAL_PPSinInfo(
							  IMG_UBBUF				const	*pubbSrc,
							  IMG_COORD				const	coOff,
							  IMG_SIZE				const	szOp,
							  DSP_PPSININFO_PARA	const	stPara,
							  DSP_PPSIN_INFO_X				*pstInfo
							  )
{
	IMG_WORD
		wSts	= OK,
		wStsAXB	= OK,
		wMEMSts	= OK;

	IMG_UWORD
		uwMultiPX	= 0,
		uwFilterY	= 0,
		i			= 0,		// loop x
		j			= 0,		// loop y
		k			= 0,		// kernal search
		uwT		= 0,
		uwFx	= 0,							// full size x
		uwFy	= 0,							// full size y
		uwKs	= 0,							// search start
		uwKe	= 0,							// search end
		uwHFx	= 0,							// half size x
		uwHFy	= 0,							// half size y
		uwPeak	= 0,
		uwTrough= 0,
		uwPTc	= 0;							// peak trough count

	IMG_ULWORD
		ulCPeriod	= 0,						// count periods
		ulNPeak		= 0,						// number of peak in a row
		ulMEM		= 0;

	IMG_REAL
		r2a		= 0.0f,
		rb		= 0.0f,
		rPeakA	= REAL_MIN,						// Peak value
		rTroughA= REAL_MAX,						// Trough value
		rCurrent= 0.0f,
		rPTP	= 0.0f,
		arPTA[3]= {0.0f, 0.0f, 0.0f},			// Combined peak trough value
		arWx[3]	= {0.0f, 0.0f, 0.0f};

	IMG_RBUF
		rbA,
		rbB,
		rbBG,									// background intensity
		rbTmp,									// Temporary buffer		(sum y buffer)
		rbSrc;									// maximizaiton buffer	(sum x buffer)

	MAT_FITPARA
		stHPPara;

	MAT_HPLANE
		stHPResult;

	DSP_SININFO_PARA
		stParaP;							// Partitioned sin parameters

	DSP_SIN_INFO
		stInfoA;							// All sin. info

	IppStatus
		ippSts;

	IppiSize
		ippRoiSize;

	IMG_REAL
		*prA	= NULL,						// buffer A operation pointer
		*prB	= NULL,						// buffer B operation pointer
		*prK	= NULL;						// sin wave kernel

	if (NULL== pstInfo	|| NULL== pstInfo->pubValid)	return OK;

	if (NULL== pubbSrc	|| NULL== pubbSrc->ptr)
		return DSP_ERR_INVALID_ARG;

	wMEMSts= MEM_PushAllHeapStatus();
	if (OK!= wMEMSts)	return wMEMSts;

	// ================ Calculate overall frequency by FFT ==============
	DSP_CAL_SinInfoIniti(&stParaP);
	stParaP.rMinFreqY= 0.00f;
	stParaP.rMinFreqX= 0.025f;
	wSts=	DSP_CAL_SinInfo(
		pubbSrc, coOff, szOp, stParaP, &stInfoA);

	if (wSts< OK)	goto EXIT_DSP_CAL_PPSinInfo;

	if (stInfoA.rfx< 0.0001f)
	{
		wSts= DSP_ERR_DIVIDED_BY_ZERO;
		goto EXIT_DSP_CAL_PPSinInfo;
	}

	//=================  Define size for the operators ==================
	uwMultiPX= stPara.uwMultiPX;
	if (0== uwMultiPX)	uwMultiPX= 5;					// no force multipler

	uwFx = (IMG_UWORD) (uwMultiPX/stInfoA.rfx + 0.5f);	// use N periods search
	if (uwFx> (szOp.x/10))	uwFx= (szOp.x/10);			// kernel too large

	uwFx =(((uwFx-1)>> 1)<< 1)+ 1;						// to generate an odd kernel
	uwHFx= uwFx>> 1;									// half size


	uwFilterY= stPara.uwFilterY;					// no force Y filtering
	if (0== uwFilterY)
	{
		if (stInfoA.rfy< 0.00001f)					// no Y frequency
			uwFilterY= uwFx;						// filtering along Y;
		else
			uwFilterY= (IMG_UWORD) (0.1f/stInfoA.rfy);

		if (1		  > uwFilterY)	uwFilterY= 1;
		if (uwFx	  < uwFilterY)	uwFilterY= uwFx;
		if (szOp.y/10 < uwFilterY)	uwFilterY= (szOp.y/10);
	}

	uwFy = uwFilterY;
	uwFy =(((uwFy-1)>> 1)<< 1)+ 1;						// to generate an odd kernel
	uwHFy= uwFy>> 1;									// half size

	rbTmp.linestep	= rbTmp.size.x= szOp.x;
	rbTmp.size.y	= szOp.y;

	rbSrc.linestep	= rbSrc.size.x= szOp.x- uwFx;
	rbSrc.size.y	= rbTmp.size.y;

	// determine maximum memory needed for fitting AXB
	MAT_FitAXBInit(NULL,&stHPPara);
	stHPPara.NoiseML= 1.0f;
	stHPPara.TimeML = 100;

	stHPResult.M= 3;
	stHPResult.ptr=&arWx[0];

	// omega A, B for x directions
	rbA.linestep	= rbA.size.x= 3;
	rbA.size.y		= szOp.y;												// add 10% margins for frequency variation

	rbB.linestep	= rbB.size.x= 1;
	rbB.size.y		= rbA.size.y;

	ulMEM= 
		rbTmp.size.x*rbTmp.size.y+	rbSrc.size.x*rbSrc.size.y+				//	noise filtering
		uwFx+																//	kernal
		(rbA.size.x + rbB.size.x )*rbA.size.y;								//	AXB -x

	// ======================== Memory allocations ========================
	if (NULL== 	(prK= (IMG_REAL *) MEM_Malloc(ulMEM*sizeof(IMG_REAL), SYS_BANK1_8)))
	{
		wSts= MEM_ERR_MALLOC;
		goto EXIT_DSP_CAL_PPSinInfo;
	}
	rbTmp.ptr	= prK+ uwFx;
	rbSrc.ptr	= rbTmp.ptr	+ (rbTmp.size.x*rbTmp.size.y);

	rbA.ptr		= rbSrc.ptr	+ (rbSrc.size.x*rbSrc.size.y);
	rbB.ptr		= rbA.ptr	+ (rbA.size.x*rbA.size.y);
	// =====================================================================

	//============================= Filtering ==============================
	// First uwFy rows
	ippRoiSize.width = rbTmp.size.x;
	ippRoiSize.height= uwHFy;

	// 1. Upper part note offset uwHFy

	ippSts= ippiSumWindowColumn_8u32f_C1R(
		pubbSrc->ptr+ (coOff.y)*pubbSrc->linestep+ coOff.x,
		pubbSrc->linestep*sizeof(IMG_UBYTE),
		rbTmp.ptr,
		rbTmp.linestep*sizeof(IMG_REAL),
		ippRoiSize, uwFy, 0);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_CAL_PPSinInfo;
	}

	// 2. Middle part

	// note offset uwHFy
	ippRoiSize.width = rbTmp.size.x;
	ippRoiSize.height= szOp.y- uwFy+ 1;

	ippSts= ippiSumWindowColumn_8u32f_C1R(
		pubbSrc->ptr+ (coOff.y+ uwHFy)*pubbSrc->linestep+ coOff.x,
		pubbSrc->linestep*sizeof(IMG_UBYTE),
		rbTmp.ptr+ (uwHFy)*rbTmp.linestep,
		rbTmp.linestep*sizeof(IMG_REAL),
		ippRoiSize, uwFy, uwHFy);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_CAL_PPSinInfo;
	}

	// filter lower part
	ippRoiSize.width = rbTmp.size.x;
	ippRoiSize.height= uwHFy;


	ippSts= ippiSumWindowColumn_8u32f_C1R(
		pubbSrc->ptr+ (coOff.y+ szOp.y- uwHFy)*pubbSrc->linestep+ coOff.x,
		pubbSrc->linestep*sizeof(IMG_UBYTE),
		rbTmp.ptr+ (szOp.y- uwHFy)*rbTmp.linestep,
		rbTmp.linestep*sizeof(IMG_REAL),
		ippRoiSize, uwFy, uwFy- 1);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_CAL_PPSinInfo;
	}

	//====== Correlation with cos wave
	for (i= 0; i< uwFx; i++)
		prK[uwFx- i- 1]= (IMG_REAL) cos(MAT_2PI*stInfoA.rfx*i);

	ippRoiSize.width = rbSrc.size.x;
	ippRoiSize.height= rbSrc.size.y;

	ippSts= ippiFilterRow_32f_C1R(
		rbTmp.ptr+ uwHFx, rbTmp.linestep*sizeof(IMG_REAL),
		rbSrc.ptr		, rbSrc.linestep*sizeof(IMG_REAL),
		ippRoiSize, prK	, uwFx, uwHFx);

	if (ippStsNoErr!= ippSts)
	{
		wSts= DSP_ERR_FAIL_IN_IPPI;
		goto EXIT_DSP_CAL_PPSinInfo;
	}


	uwT	 = (IMG_UWORD) (1.0f/stInfoA.rfx+ 1.0f);	// First period length
	uwKs= 0;
	uwKe= uwT- 1;

	for (j= 0; j< szOp.y; j++)
	{
		ulCPeriod= 0;							// current period
		ulNPeak= 0;								// number of peak
		prA= rbA.ptr;
		prB= rbB.ptr;

		i= 0;									// reset to starting line
		while (i< (szOp.x- uwT))
		{
			rPeakA	= REAL_MIN;
			rTroughA= REAL_MAX;

			for (k= uwKs; k<= uwKe; k++)
			{
				rCurrent= rbSrc.ptr[j*rbSrc.linestep+ i+ k];

				// Search peak
				if (rCurrent> rPeakA)
				{
					rPeakA= rCurrent;
					uwPeak= k;
				}

				// Search trough
				if (rCurrent< rTroughA)
				{
					rTroughA= rCurrent;
					uwTrough= k;
				}
			}

			// combined peak, trough (virtual peak)
			rPTP	= 0.0f;
			uwPTc	= 0;
			if (uwKs< uwPeak && uwKe> uwPeak)
			{
				arPTA[0]= rbSrc.ptr[j*rbSrc.linestep+ i+ uwPeak- 1	];
				arPTA[1]= rbSrc.ptr[j*rbSrc.linestep+ i+ uwPeak		];
				arPTA[2]= rbSrc.ptr[j*rbSrc.linestep+ i+ uwPeak+ 1	];

				rPTP+= (IMG_REAL) uwPeak;
				uwPTc++;
			}

			if (uwKs< uwTrough && uwKe> uwTrough)
			{
				arPTA[0]=-rbSrc.ptr[j*rbSrc.linestep+ i+ uwTrough- 1];
				arPTA[1]=-rbSrc.ptr[j*rbSrc.linestep+ i+ uwTrough	 ];
				arPTA[2]=-rbSrc.ptr[j*rbSrc.linestep+ i+ uwTrough+ 1];

				rPTP+= (IMG_REAL) uwTrough;
				rPTP+= ((uwTrough> uwPeak)?(-0.5f):0.5f)/stInfoA.rfx;

				uwPTc++;
			}

			if (0!= uwPTc)											// no observable peak or trough
			{
				rPTP/= (IMG_REAL)uwPTc;

				r2a= (arPTA[0]+arPTA[2]- 2.0f*arPTA[1]);
				rb = (arPTA[2]-arPTA[0])*0.5f;

				if (fabs(r2a)> 0.0001 && fabs(rb/r2a)< 1.0001f)
					rPTP-= rb/r2a;

				rPTP+= i;

				*(prA++)= 1.0f;
				*(prA++)= rPTP;
				*(prA++)= rPTP*rPTP;

				*(prB++)= (IMG_REAL) (ulCPeriod*MAT_2PI);

				ulNPeak++;												// get 1 peak
				i= (IMG_UWORD) (rPTP+ 0.5f+ (uwT>> 1));					// next i
			}
			else
				i+= uwT;												// search next peak

			ulCPeriod++;												// update current period
		}										// <=== while k

		rbA.size.y= (IMG_UWORD) ulNPeak;								// update number of peak found
		rbB.size.y= (IMG_UWORD) rbA.size.y;

		// Compute wx, wxx by fitting phase a quadritic curve of phase
		wStsAXB= MAT_FitAXB1_T(NULL, NULL, &rbA, NULL, &rbB, NULL, NULL, NULL, NULL, NULL,
			NULL, NULL, &stHPPara, NULL, &stHPResult, NULL, 0 );	

		if (OK== wStsAXB)
		{
			pstInfo->pubValid[j]	= 1;
			if (NULL!= pstInfo->prPhi)	pstInfo->prPhi[j]= stHPResult.ptr[0];
			if (NULL!= pstInfo->prWx )	pstInfo->prWx[j] = stHPResult.ptr[1];
			if (NULL!= pstInfo->prWxx)	pstInfo->prWxx[j]= stHPResult.ptr[2];
		}
		else
			pstInfo->pubValid[j]	= 0;
	}											// <=== for y

	//======================= Wrapping Phi=========================
	if (NULL!= pstInfo->prPhi)
	{
		IMG_WORD
			wN2PI	= 0;						// number of 2PI

		IMG_UWORD
			uwN		= 0,						// number of valid
			j_l		= 0,						// last j value
			uwPhi_l	= 0;						// if last phi found

		IMG_REAL
			rSlope	= 0.0f,						// rate of change of phi_x along y
			rNorm	= 0.0f,						// normalization
			rPhi_l	= 0.0f;						// last phase;


		rSlope = (stInfoA.rfy< 0.0001f)?0.0f: stInfoA.rfy;				// ratio of change along Y
		rSlope*= (IMG_REAL) MAT_2PI;

		rNorm	= 0.0f;
		uwN		= 0;
		for (j= 0; j< szOp.y; j++)
		{
			if (1== pstInfo->pubValid[j])
			{
				if (1== uwPhi_l)
				{
					arPTA[0]= arPTA[2]= 0.0f;
					arPTA[1]= 1.0f;

					while (arPTA[1]> arPTA[0] || arPTA[1]> arPTA[2])
					{
						arPTA[0]= (IMG_REAL) fabs(fabs(pstInfo->prPhi[j]- rPhi_l- MAT_2PI)	- rSlope*(j- j_l));
						arPTA[1]= (IMG_REAL) fabs(fabs(pstInfo->prPhi[j]- rPhi_l)			- rSlope*(j- j_l));
						arPTA[2]= (IMG_REAL) fabs(fabs(pstInfo->prPhi[j]- rPhi_l+ MAT_2PI)	- rSlope*(j- j_l));

						if (arPTA[1]> arPTA[0] && arPTA[2]> arPTA[0])	pstInfo->prPhi[j]-= (IMG_REAL) MAT_2PI;
						if (arPTA[1]> arPTA[2] && arPTA[0]> arPTA[2])	pstInfo->prPhi[j]+= (IMG_REAL) MAT_2PI;
					}
				}
				else
					uwPhi_l = 1;

				j_l= j;
				rPhi_l= pstInfo->prPhi[j];

				rNorm+= pstInfo->prPhi[j];
				uwN++;
			}
		}
		if (0< uwN) rNorm/= uwN;

		wN2PI = (IMG_WORD) (rNorm/MAT_2PI+ ((rNorm> 0.0f)?0.5:-0.5));
		rNorm = (IMG_REAL) (wN2PI*MAT_2PI);

		// Normalization

		for (j= 0; j< szOp.y; j++)
			if (1== pstInfo->pubValid[j])	pstInfo->prPhi[j]-= rNorm;
	}

	if (NULL	!= pstInfo->rbA.ptr		&&				// need to return A buffer
		szOp.x	== pstInfo->rbA.size.x	&&
		szOp.y	== pstInfo->rbA.size.y	)
		rbBG= pstInfo->rbA;								// use the memory allocated by A
	else
	{
		rbBG.size= szOp;
		rbBG.linestep= rbBG.size.x;

		if (NULL== 
			(rbBG.ptr= (IMG_REAL *) MEM_Malloc(rbBG.size.x*rbBG.size.y*sizeof(IMG_REAL), SYS_BANK1_8)))
		{
			wSts= MEM_ERR_MALLOC;
			goto EXIT_DSP_CAL_PPSinInfo;
		}
	}

	{
		IMG_UWORD
			uwFilterOff= 0;

		IppiPoint 
			IppAnchor;

		IppiSize
			IppMaskSize;


		ippRoiSize.width = szOp.x;
		ippRoiSize.height= szOp.y;

		ippSts= ippiDivC_32f_C1R(
			rbTmp.ptr		, rbTmp.linestep*sizeof(IMG_REAL),
			(IMG_REAL) uwFy	,
			rbBG.ptr		, rbBG.linestep*sizeof(IMG_REAL),
			ippRoiSize);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_CAL_PPSinInfo;
		}

		uwFx = (IMG_UWORD) (2.0f/stInfoA.rfx + 0.5f);
		uwFx = (((uwFx- 1)>> 1)<< 1)+ 1;
		uwHFx= uwFx>> 1;

		IppMaskSize.height= 1;
		IppMaskSize.width = uwFx;

		ippRoiSize.height= szOp.y;
		IppAnchor.y= 0;

		for (i= 0; i< 2; i++)
		{
			for (j= 0; j< 3; j++)		// j= 0: upper, j= 1: middle, j= 2: lower
			{
				// note offset uwHFx
				switch (j)
				{
				case 0:								// upper
					uwFilterOff = 0;
					ippRoiSize.width = uwHFx;
					IppAnchor.x= 0;
					break;
				case 2:								// lower
					uwFilterOff = szOp.x- uwHFx;
					ippRoiSize.width = uwHFx;
					IppAnchor.x= uwFx- 1;
					break;
				default:							// middle
					uwFilterOff = uwHFx;
					ippRoiSize.width = szOp.x- uwFx+ 1;
					IppAnchor.x= uwHFx;
					break;
				}

//				if( ( uwFilterOff + IppMaskSize.width + IppMaskSize.width - IppAnchor.x -1) > rbBG.size.x) ||
//					( uwFilterOff < IppAnchor.x) ) 
//				if( uwFilterOff + IppMaskSize.width) > rbBG.size.x)
//				{
//					SYS_Assert(0); wSts= DSP_ERR_INVALID_SIZE;
//					goto EXIT_DSP_CAL_PPSinInfo;
//				}

				ippSts= ippiFilterBox_32f_C1IR(
					//rbTmp.ptr		,		rbTmp.linestep*sizeof(IMG_REAL),
					rbBG.ptr+ uwFilterOff,		pstInfo->rbA.linestep*sizeof(IMG_REAL),
					ippRoiSize, IppMaskSize, IppAnchor);

				if (ippStsNoErr!= ippSts)
				{
					wSts= DSP_ERR_FAIL_IN_IPPI;
					goto EXIT_DSP_CAL_PPSinInfo;
				}
			}
		}
	}

	if (NULL	!= pstInfo->rbB.ptr		&&				// need to return B buffer
		szOp.x	== pstInfo->rbB.size.x	&&
		szOp.y	== pstInfo->rbB.size.y	)
	{
		IMG_UWORD
			uwFilterOff= 0;

		IppiPoint 
			IppAnchor;

		IppiSize
			IppMaskSize;


		ippRoiSize.width = szOp.x;
		ippRoiSize.height= szOp.y;



		//
		//   ((A+ B*cos(phi))- A)^2=  0.5*B^2(cos(2*phi)+ 1)
		//
		ippSts= ippiDivC_32f_C1R(
			rbTmp.ptr		, rbTmp.linestep*sizeof(IMG_REAL),
			(IMG_REAL) uwFy	,
			pstInfo->rbB.ptr, pstInfo->rbB.linestep*sizeof(IMG_REAL),
			ippRoiSize);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_CAL_PPSinInfo;
		}

		ippSts= ippiSub_32f_C1IR(
			rbBG.ptr		, rbBG.linestep*sizeof(IMG_REAL),
			pstInfo->rbB.ptr, pstInfo->rbB.linestep*sizeof(IMG_REAL),
			ippRoiSize);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_CAL_PPSinInfo;
		}

		ippSts= ippiSqr_32f_C1IR(
			pstInfo->rbB.ptr, pstInfo->rbB.linestep*sizeof(IMG_REAL),
			ippRoiSize);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_CAL_PPSinInfo;
		}

		uwFx= (IMG_UWORD) (1.0f/stInfoA.rfx + 0.5f);
		uwFx = (((uwFx- 1)>> 1)<< 1)+ 1;
		uwHFx= uwFx>> 1;

		IppMaskSize.height= 1;
		IppMaskSize.width = uwFx;

		ippRoiSize.height= szOp.y;
		IppAnchor.y= 0;

		for (i= 0; i< 2; i++)
		{
			for (j= 0; j< 3; j++)		// j= 0: upper, j= 1: middle, j= 2: lower
			{
				// note offset uwHFx
				switch (j)
				{
				case 0:								// upper
					uwFilterOff = 0;
					ippRoiSize.width = uwHFx;
					IppAnchor.x= 0;
					break;
				case 2:								// lower
					uwFilterOff = szOp.x- uwHFx;
					ippRoiSize.width = uwHFx;
					IppAnchor.x= uwFx- 1;
					break;
				default:							// middle
					uwFilterOff = uwHFx;
					ippRoiSize.width = szOp.x- uwFx+ 1;
					IppAnchor.x= uwHFx;
					break;
				}

				ippSts= ippiFilterBox_32f_C1IR(
					//rbTmp.ptr		,		rbTmp.linestep*sizeof(IMG_REAL),
					pstInfo->rbB.ptr+ uwFilterOff,	pstInfo->rbB.linestep*sizeof(IMG_REAL),
					ippRoiSize, IppMaskSize, IppAnchor);

				if (ippStsNoErr!= ippSts)
				{
					wSts= DSP_ERR_FAIL_IN_IPPI;
					goto EXIT_DSP_CAL_PPSinInfo;
				}
			}
		}

		ippRoiSize.width = szOp.x;
		ippRoiSize.height= szOp.y;

		ippSts= ippiMulC_32f_C1IR(2.0f,	
			pstInfo->rbB.ptr, pstInfo->rbB.linestep*sizeof(IMG_REAL),
			ippRoiSize);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_CAL_PPSinInfo;
		}

		{ 
			IMG_COORD	coZero= {0,0};

			wSts =  DSP_AddC_rrr(&pstInfo->rbB, &coZero, 0.01f, &pstInfo->rbB, &coZero, &szOp);
			if( wSts != OK){ SYS_Assert(0);		goto EXIT_DSP_CAL_PPSinInfo; }		
		}

		ippSts= ippiSqrt_32f_C1IR(
			pstInfo->rbB.ptr, pstInfo->rbB.linestep*sizeof(IMG_REAL),
			ippRoiSize);

		if (ippStsNoErr!= ippSts)
		{
			wSts= DSP_ERR_FAIL_IN_IPPI;
			goto EXIT_DSP_CAL_PPSinInfo;
		}
	}

EXIT_DSP_CAL_PPSinInfo:
	if (OK!= (wMEMSts= MEM_PopAllHeapStatus()))	return wMEMSts;
	return wSts;
}

//==================  Confocal specific function ==================
//
//IMG_VVOID	DSP_INS_Confocal_Inti(
//								  DSP_CONFOCAL_INSPARA	*pstPara
//								  )
//{
//	if (NULL== pstPara)	return;
//	memset(pstPara, 0, sizeof(DSP_CONFOCAL_INSPARA));
//	pstPara->rThre= -1.0f;
//	return;
//}
//
//IMG_WORD	DSP_Ins_ConfocalPos(
//								IMG_UBBUF			const	*pubbS,				// Number of images
//								IMG_COORD			const	coOp,				// offset
//								IMG_SIZE			const	szOp,				// opsize
//								DSP_SIN_INFO		const	*pstInfo,			// SIN info
//								IMG_RCOORD					*prcoP,				// extract position
//								IMG_REAL					*prM,				// confocal magnitude (can be NULL)
//								IMG_REAL					*prCf				// confidence (can be NULL)
//								)
//{
//	IMG_WORD
//		wMEMSts	= OK,
//		wSts	= OK;
//
//	IMG_ULWORD
//		ulMEM	= 0;
//
//	IMG_COORD
//		coZ	= {0, 0};
//
//	IMG_REAL
//		rScore= 0.0f;
//
//	IMG_RBUF
//		rbS,
//		rbM;
//
//	DSP_SIN_PARA
//		stPara;
//
//	if (NULL== prcoP)	return OK;
//
//	if (NULL== pubbS|| NULL== pubbS->ptr||
//		NULL== pstInfo)
//		return DSP_ERR_INVALID_ARG;
//
//	wMEMSts= MEM_PushAllHeapStatus();
//	if (OK!= wMEMSts)	return wMEMSts;
//
//	rbS.size= rbM.size= szOp;
//	rbS.linestep= rbM.linestep= szOp.x;
//
//	ulMEM= ((szOp.x*szOp.y)<< 1);
//	if (NULL== (rbS.ptr= (IMG_REAL *) MEM_Malloc(ulMEM*sizeof(IMG_REAL), SYS_BANK1_8)))
//	{
//		wSts= MEM_ERR_MALLOC;
//		goto EXIT_DSP_Ins_ConfocalPos;
//	}
//	memset(rbS.ptr, 0, ulMEM*sizeof(IMG_REAL));
//	rbM.ptr= rbS.ptr+ (szOp.x*szOp.y);
//
//	memset(&stPara, 0, sizeof(DSP_SIN_PARA));
//	if (pstInfo->rfx> 0.0f && pstInfo->rfx> pstInfo->rfy)
//	{
//		stPara.rPeriod		= 1.0f/pstInfo->rfx;
//		stPara.uwSKernel	= (IMG_UWORD) (0.1f/((pstInfo->rfy> 0.0f)? pstInfo->rfy:0.02f));
//		stPara.uwSKernel	= IMG_MIN(stPara.uwSKernel, 5);
//		stPara.rOrientation = 0.0f;
//	}
//	else if (pstInfo->rfy> 0.0f)
//	{
//		stPara.rPeriod		= 1.0f/pstInfo->rfy;
//		stPara.uwSKernel	= (IMG_UWORD) (0.1f/((pstInfo->rfx> 0.0f)? pstInfo->rfx:0.02f));
//		stPara.uwSKernel	= IMG_MIN(stPara.uwSKernel, 5);
//		stPara.rOrientation =90.0f;
//	}
//	else
//	{
//		wSts= DSP_ERR_INVALID_ARG;
//		goto EXIT_DSP_Ins_ConfocalPos;
//	}
//	stPara.rAmplitude	= pstInfo->rB- 2.0f*pstInfo->rB_SD;
//	if (stPara.rAmplitude< pstInfo->rB_SD) stPara.rAmplitude= IMG_MAX(pstInfo->rB_SD, 1.0f);
//
//	wSts = 	DSP_INS_IsSinusoidalExist(
//		pubbS,	coOp, szOp, stPara,	&rbS, NULL,	&rbM);
//	if ( wSts < OK )	goto EXIT_DSP_Ins_ConfocalPos;
//
//	wSts= DSP_INS_FindConvexMaximum(
//		&rbS, coZ, rbS.size, prcoP, &rScore);
//	if ( wSts < OK )	goto EXIT_DSP_Ins_ConfocalPos;
//
//	if (NULL!= prCf)	*prCf= rScore;
//
//	if (NULL!= prM)
//	{
//		IMG_COORD
//			coP		= {0, 0};
//
//		IMG_RCOORD
//			rcodP	= {0.0f, 0.0f};
//
//		coP.x= (IMG_WORD) prcoP->x;
//		coP.y= (IMG_WORD) prcoP->y;
//
//		rcodP.x= prcoP->x- (IMG_REAL) coP.x;
//		rcodP.y= prcoP->y- (IMG_REAL) coP.y;
//
//		*prM=
//			(1.0f-	rcodP.x)*(1.0f-	rcodP.y)*rbM.ptr[(coP.y   )*rbM.linestep+ (coP.x   )]+ 
//			(		rcodP.x)*(1.0f-	rcodP.y)*rbM.ptr[(coP.y   )*rbM.linestep+ (coP.x+ 1)]+
//			(		rcodP.x)*(		rcodP.y)*rbM.ptr[(coP.y+ 1)*rbM.linestep+ (coP.x+ 1)]+
//			(1.0f-  rcodP.x)*(		rcodP.y)*rbM.ptr[(coP.y+ 1)*rbM.linestep+ (coP.x   )];
//
//		*prM= (IMG_REAL) sqrt(*prM);
//	}
//
//	prcoP->x+= (IMG_REAL) coOp.x;
//	prcoP->y+= (IMG_REAL) coOp.y;
//
//EXIT_DSP_Ins_ConfocalPos:
//	wMEMSts= MEM_PopAllHeapStatus();
//	if (OK!= wMEMSts)	return wMEMSts;
//	return wSts;
//}
//
//IMG_WORD	DSP_INS_Confocal(
//							 IMG_UBBUF				const	*pubbS,			// Number of images
//							 IMG_COORD				const	coOp,			// offset
//							 IMG_SIZE				const	szOp,			// opsize
//							 DSP_CONFOCAL_INFO		const	stCInfo,		// calibration record
//							 DSP_CONFOCAL_INSPARA	const	stCPara,		// inspectiion parameter
//							 DSP_CONFOCAL_RES				*pstCRes		// packeed result
//							 )
//{
//	IMG_WORD
//		wSts = OK;
//
//	IMG_RCOORD
//		rcoP = {0.0f, 0.0f};
//
//	DSP_SIN_INFO
//		stSInfo;
//
//	if (NULL== pstCRes)	goto EXIT_DSP_INS_Confocal;
//	memset(pstCRes, 0, sizeof(DSP_CONFOCAL_RES));
//
//	stSInfo= stCInfo.stSInfo;
//	if (stCPara.rThre> 0.0f)
//	{
//		stSInfo.rB		= stCPara.rThre;
//		stSInfo.rB_SD	= 0.0f;
//	}
//
//	wSts= DSP_Ins_ConfocalPos(pubbS, coOp, szOp, &stSInfo, &rcoP, &pstCRes->rAmp, &pstCRes->rConf);
//	if (wSts< OK)	goto EXIT_DSP_INS_Confocal;
//
//	pstCRes->rHeight= rcoP.x*stCInfo.arFxy[0]+ rcoP.y*stCInfo.arFxy[1]+ stCInfo.arFxy[2];
//	pstCRes->rcoP= rcoP;
//
//EXIT_DSP_INS_Confocal:
//	return wSts;
//}
//
//IMG_VVOID	DSP_CAL_Confocal_Inti(
//								  DSP_CONFOCAL_CALPARA	*pstPara
//								  )
//{
//	if (NULL== pstPara)	return;
//	memset(pstPara, 0, sizeof(DSP_CONFOCAL_CALPARA));
//	pstPara->rTol= 2.0f;
//	return;
//}
//
//IMG_WORD	DSP_CAL_Confocal(
//							 IMG_UWORD				const	uwN,			// Number of images
//							 IMG_UBBUF				const	*pubbS,			// Number of images
//							 IMG_COORD				const	coOp,			// offset
//							 IMG_SIZE				const	szOp,			// opsize
//							 IMG_REAL				const	*prHeight,		// Height index
//							 DSP_CONFOCAL_CALPARA	const	stCPara,
//							 DSP_CONFOCAL_INFO				*pstCInfo,
//							 DSP_CONFOCAL_RES				*pstCRes
//							 )
//{
//	IMG_WORD
//		wMEMSts= OK,
//		wPOSSts= OK,
//		wSts= OK;
//
//	IMG_UWORD
//		n	= 0,
//		N	= 0,
//		i	= 0,
//		j	= 0;
//
//	IMG_ULWORD
//		ulMEM= 0;
//
//	IMG_REAL
//		rM		= 0.0f,
//		rCf		= 0.0f,
//		rMSum	= 0.0f,					// sum
//		rM2Sum	= 0.0f;					// sum square
//
//	IMG_COORD
//		coP		= {0, 0},
//		coZ		= {0, 0};
//
//	IMG_RCOORD
//		rcoP = {0.0f, 0.0f};
//
//	IMG_UBBUF
//		ubbS;
//
//	IMG_RBUF
//		rbS,
//		rbA,
//		rbB;
//
//	MAT_FITPARA
//		stHPPara;
//
//	MAT_HPLANE
//		stHPResult;
//
//	DSP_SININFO_PARA
//		stPara;
//
//	IppStatus
//		ippSts	= ippStsNoErr;
//
//	IppiSize
//		ippRoiSize;
//
//	IMG_UBYTE
//		*pubS= NULL;
//
//	IMG_REAL
//		*prS= NULL,
//		*prA= NULL,
//		*prB= NULL;
//
//	// Take image average to get learn image
//	if (NULL== pstCInfo)	return OK;
//	if (0== uwN|| NULL== pubbS)	return DSP_ERR_INVALID_ARG;
//
//	wMEMSts= MEM_PushAllHeapStatus();
//	if (wMEMSts< OK)	return wMEMSts;
//
//	rbS.size= ubbS.size= szOp;
//	rbS.linestep= ubbS.linestep= szOp.x;
//
//	rbA.linestep= rbA.size.x= 3;
//	rbB.linestep= rbB.size.x= 1;
//	rbA.size.y= rbB.size.y= uwN+ 2;				// add 2 coefficient equal to 0 constraints
//
//	ulMEM= rbS.size.x*rbS.size.y+ rbA.size.x*rbA.size.y+ rbB.size.x*rbB.size.y;
//
//	if (NULL== (rbS.ptr	= (IMG_REAL		*) MEM_Malloc(ulMEM*sizeof(IMG_REAL ), SYS_BANK1_8))||
//		NULL== (ubbS.ptr= (IMG_UBYTE	*) MEM_Malloc(ulMEM*sizeof(IMG_UBYTE), SYS_BANK1_8)))
//	{
//		wMEMSts= MEM_ERR_MALLOC;
//		goto EXIT_DSP_CAL_Confocal;
//	}
//	memset(rbS.ptr , 0, rbS.size.x* rbS.size.y*sizeof(IMG_REAL ));
//	rbA.ptr	= rbS.ptr+ rbS.size.x*rbS.size.y;
//	rbB.ptr	= rbA.ptr+ rbA.size.x*rbA.size.y;
//
//	memset(ubbS.ptr, 0,ubbS.size.x*ubbS.size.y*sizeof(IMG_UBYTE));
//
//	// ======== Compute the average intensity of input image series ======== 
//	ippRoiSize.width	= szOp.x;
//	ippRoiSize.height	= szOp.y;
//	for (n= 0; n< uwN; n++)
//	{
//		ippSts= ippiAdd_8u32f_C1IR(
//			pubbS[n].ptr+ coOp.y*pubbS[n].linestep+ coOp.x, pubbS[n].linestep*sizeof(IMG_UBYTE),
//			rbS.ptr, rbS.linestep*sizeof(IMG_REAL), ippRoiSize);
//
//		if (ippStsNoErr!= ippSts)
//		{
//			wSts= DSP_ERR_FAIL_IN_IPPI;
//			goto EXIT_DSP_CAL_Confocal;
//		}
//	}
//
//	prS	= rbS.ptr;
//	pubS= ubbS.ptr;
//	for (j= 0; j< szOp.y; j++, pubS+= (ubbS.linestep- szOp.x), prS+= (rbS.linestep- szOp.x))
//		for (i= 0; i< szOp.x; i++)
//			*(pubS++)= (IMG_UBYTE) (*(prS++)/(IMG_REAL)uwN+ 0.5f);
//
//
//	memset(&stPara, 0, sizeof(DSP_SININFO_PARA));
//	DSP_CAL_SinInfoIniti(&stPara);
//	wSts=	DSP_CAL_SinInfo(
//		&ubbS, coZ, ubbS.size, stPara, &pstCInfo->stSInfo);
//	if (wSts< OK)	goto EXIT_DSP_CAL_Confocal;
//
//	if (pstCInfo->stSInfo.rfx< 0.0f && pstCInfo->stSInfo.rfy< 0.0f)
//	{
//		wSts= DSP_ERR_DIVIDED_BY_ZERO;
//		goto EXIT_DSP_CAL_Confocal;
//	}
//
//	N  = 0;
//
//	// Compute the mean B and B_SD
//	rMSum = 0.0f;
//	rM2Sum= 0.0f;
//	for (n= 0; n< uwN; n++)
//	{
//		wPOSSts= DSP_Ins_ConfocalPos(
//			pubbS+ n, coOp, szOp, &pstCInfo->stSInfo, &rcoP, &rM, &rCf);
//		if (OK> wPOSSts) continue;
//
//		N++;
//
//		rMSum += (rM);
//		rM2Sum+= (rM*rM);
//	}
//
//	if (N< 2)
//	{
//		wSts= DSP_ERR_DIVIDED_BY_ZERO;
//		goto EXIT_DSP_CAL_Confocal;
//	}
//
//	rMSum /= (IMG_REAL)N;
//	rM2Sum/= (IMG_REAL)N;
//
//	pstCInfo->stSInfo.rB	= rMSum;
//	pstCInfo->stSInfo.rB_SD	=(IMG_REAL)sqrt(rM2Sum- (rMSum)*(rMSum));
//	pstCInfo->stSInfo.rB_SD*=((IMG_REAL)N/((IMG_REAL)N- 1));
//
//	N  = 0;
//	prA= rbA.ptr;
//	prB= rbB.ptr;
//
//	pstCInfo->acoBoundRegion[0].x= (IMG_WORD) (coOp.x+ szOp.x);
//	pstCInfo->acoBoundRegion[0].y= (IMG_WORD) (coOp.y+ szOp.y);
//	pstCInfo->acoBoundRegion[1].x= coOp.x;
//	pstCInfo->acoBoundRegion[1].y= coOp.y;
//
//	for (n= 0; n< uwN; n++)
//	{
//		// Store Result position
//		if (NULL!= pstCRes)
//			memset(pstCRes+ n, 0, sizeof(DSP_CONFOCAL_RES));
//
//		wPOSSts= DSP_Ins_ConfocalPos(
//			pubbS+ n, coOp, szOp, &pstCInfo->stSInfo, &rcoP, &rM, &rCf);
//		if (OK> wPOSSts) continue;
//
//		// added by Tony
//		coP.x	= (IMG_WORD) (rcoP.x+ 0.5f);
//		coP.y	= (IMG_WORD) (rcoP.y+ 0.5f);
//		pstCInfo->acoBoundRegion[0].x = IMG_MIN(pstCInfo->acoBoundRegion[0].x, coP.x);
//		pstCInfo->acoBoundRegion[0].y = IMG_MIN(pstCInfo->acoBoundRegion[0].y, coP.y);
//		pstCInfo->acoBoundRegion[1].x = IMG_MAX(pstCInfo->acoBoundRegion[1].x, coP.x);
//		pstCInfo->acoBoundRegion[1].y = IMG_MAX(pstCInfo->acoBoundRegion[1].y, coP.y);
//
//		*(prA++)= rM*rcoP.x;
//		*(prA++)= rM*rcoP.y;
//		*(prA++)= rM*1.0f;
//
//		*(prB++)= rM*prHeight[n];
//		N++;
//
//		// Store Result position
//		if (NULL!= pstCRes)
//		{
//			pstCRes[n].rcoP		= rcoP;
//			pstCRes[n].rHeight	= prHeight[n];
//			pstCRes[n].rConf	= rCf;
//			pstCRes[n].rAmp		= rM;
//		}
//	}
//	if (N< 1 || N< rbA.size.x)
//	{
//		wSts= DSP_ERR_DIVIDED_BY_ZERO;
//		goto EXIT_DSP_CAL_Confocal;
//	}
//
//	*(prA++)= pstCInfo->stSInfo.rB;
//	*(prA++)= 0.0f;
//	*(prA++)= 0.0f;
//	*(prB++)= 0.0f;
//
//	*(prA++)= 0.0f;
//	*(prA++)= pstCInfo->stSInfo.rB;
//	*(prA++)= 0.0f;
//	*(prB++)= 0.0f;
//
//	rbA.size.y= rbB.size.y= N+ 2;
//
//	MAT_FitAXBInit(NULL,&stHPPara);
//	stHPPara.NoiseML= stCPara.rTol*rMSum/(IMG_REAL)N;		// 2 um error
//	stHPPara.TimeML = 100;									// ml iteration
//
//	stHPPara.NoiseRS= 2.0f*stHPPara.NoiseML;
//	stHPPara.TimeRS = 500;									// ml iteration
//
//	stHPResult.M	= rbA.size.x;
//	stHPResult.ptr	= &pstCInfo->arFxy[0];
//	memset(stHPResult.ptr, 0, stHPResult.M*sizeof(IMG_REAL));
//
//	wSts= MAT_FitAXB1_T(NULL, NULL, &rbA, NULL, &rbB, NULL, NULL, NULL, NULL, NULL,
//		NULL, NULL, &stHPPara, NULL, &stHPResult, NULL, 0 );
//	if (wSts< OK)	goto EXIT_DSP_CAL_Confocal;
//
//EXIT_DSP_CAL_Confocal:
//	wMEMSts= MEM_PopAllHeapStatus();
//	if (wMEMSts< OK)	return wMEMSts;
//	return wSts;
//}
//
//#define		DSP_PPRACT_ZERO	(1e-5)
//#define		DSP_PHASE_STEP	0.02f
//IMG_WORD	DSP_PhaseOffset(
//							IMG_UBBUF	const	*pubbS0,		// Image 1
//							IMG_UBBUF	const	*pubbS1,		// Image 2
//							IMG_COORD	const	coOff,			// Offset
//							IMG_SIZE	const	szOpsz,			// Op size
//							IMG_REAL			*prOff			// Output in radian
//							)
//{
//	IMG_WORD
//		wSts	= OK,
//		wMEMSts	= OK;
//
//	IMG_UWORD
//		i	= 0,
//		j	= 0,
//		uwStep0= 0,
//		uwStep1= 0;
//
//	IMG_LREAL
//		alrMean[3]	= {0.0, 0.0, 0.0},
//		alrStd[3]	= {0.0, 0.0, 0.0};
//		//lrRatio		= 0.0;
//
//	IMG_SIZE
//		szOp= szOpsz,
//		szHF= {0, 0};					// Half filter size for smoothing purpose
//
//	IMG_RBUF
//		rbDiff;
//
//	IMG_REAL
//		*prS0= NULL,
//		*prS1= NULL;
//
//	IMG_REAL
//		*prDiff	= NULL;
//
//	DSP_SININFO_PARA
//		stPara;
//
//	DSP_SIN_INFO
//		stInfo;
//
//	IMG_RBUF
//		rbS0,
//		rbS1;
//
//	IppStatus
//		ippSts;
//
//	IppiSize
//		ipproiSize;
//
//	if (NULL== prOff)	return OK;
//	if (NULL== pubbS0		|| NULL== pubbS1		||
//		NULL== pubbS0->ptr	|| NULL== pubbS1->ptr)
//		return DSP_ERR_INVALID_ARG;
//
//	if (OK> (wMEMSts= MEM_PushAllHeapStatus()))	return wMEMSts;
//
//	DSP_CAL_SinInfoIniti(&stPara);
//	wSts=	DSP_CAL_SinInfo(
//		pubbS0,	coOff,	szOpsz,
//		stPara, &stInfo);
//	if (wSts< OK)	goto EXIT_DSP_PhaseOffset;
//
//	if (stInfo.rfx< 0.00001f && stInfo.rfy< 0.00001f)
//		return DSP_ERR_DIVIDED_BY_ZERO;
//
//	// Do smoothing according to computed frequency
//	{
//		rbS0.size= szOpsz;
//		rbS1.size= szOpsz;
//
//		rbS0.linestep= rbS0.size.x;
//		rbS1.linestep= rbS1.size.x;
//
//		if (NULL== (rbS0.ptr= (IMG_REAL *) MEM_Malloc((szOpsz.x<< 1)*szOpsz.y*sizeof(IMG_REAL), SYS_BANK1_8)))
//		{
//			wSts= MEM_ERR_MALLOC;
//			goto EXIT_DSP_PhaseOffset;
//		}
//		memset(rbS0.ptr, 0, 2*szOpsz.x*szOpsz.y*sizeof(IMG_UBYTE));
//		rbS1.ptr= rbS0.ptr+ szOpsz.x*szOpsz.y;
//
//		if (0> stInfo.rfx && 0> stInfo.rfy)
//		{
//			wSts= DSP_ERR_DIVIDED_BY_ZERO;
//			goto EXIT_DSP_PhaseOffset;
//		}
//
//		if (stInfo.rfx> stInfo.rfy)
//		{
//			szHF.x= 0;
//			szHF.y= szOpsz.y/10;
//			szHF.y= IMG_MIN(szHF.y, 100);
//
//			if (stInfo.rfy> 0)
//				szHF.y= IMG_MIN(szHF.y, IMG_UWORD(0.1f/stInfo.rfy+ 0.5f));
//
//			ipproiSize.width	= szOpsz.x- (szHF.x<< 1);
//			ipproiSize.height	= szOpsz.y- (szHF.y<< 1);
//
//			ippSts= ippiSumWindowColumn_8u32f_C1R(
//				pubbS0->ptr	+ (coOff.y+ szHF.y)*pubbS0->linestep+ (coOff.x+ szHF.x), pubbS0->linestep*sizeof(IMG_UBYTE)	,
//				rbS0.ptr	+ (         szHF.y)*   rbS0.linestep+ (         szHF.x),    rbS0.linestep*sizeof(IMG_REAL)	,
//				ipproiSize, 1+ (szHF.y<< 1), szHF.y);
//
//			if (ippStsNoErr!= ippSts)
//			{
//				wSts= DSP_ERR_FAIL_IN_IPPI;
//				goto EXIT_DSP_PhaseOffset;
//			}
//
//			ippSts= ippiSumWindowColumn_8u32f_C1R(
//				pubbS1->ptr	+ (coOff.y+ szHF.y)*pubbS1->linestep+ (coOff.x+ szHF.x), pubbS1->linestep*sizeof(IMG_UBYTE)	,
//				rbS1.ptr	+ (         szHF.y)*   rbS1.linestep+ (         szHF.x),    rbS1.linestep*sizeof(IMG_REAL)	,
//				ipproiSize, 1+ (szHF.y<< 1), szHF.y);
//
//			if (ippStsNoErr!= ippSts)
//			{
//				wSts= DSP_ERR_FAIL_IN_IPPI;
//				goto EXIT_DSP_PhaseOffset;
//			}
//		}
//		else
//		{
//            szHF.y= 0;
//			szHF.x= szOpsz.x/10;
//			szHF.x= IMG_MIN(szHF.x, 100);
//
//			if (stInfo.rfx> 0)
//				szHF.x= IMG_MIN(szHF.x, IMG_UWORD(0.1f/stInfo.rfx+ 0.5f));
//
//			ipproiSize.width	= szOpsz.x- (szHF.x<< 1);
//			ipproiSize.height	= szOpsz.y- (szHF.y<< 1);
//
//			ippSts= ippiSumWindowRow_8u32f_C1R(
//				pubbS0->ptr	+ (coOff.y+ szHF.y)*pubbS0->linestep+ (coOff.x+ szHF.x), pubbS0->linestep*sizeof(IMG_UBYTE)	,
//				rbS0.ptr	+ (         szHF.y)*   rbS0.linestep+ (         szHF.x),    rbS0.linestep*sizeof(IMG_REAL)	,
//				ipproiSize, 1+ (szHF.x<< 1), szHF.x);
//
//			if (ippStsNoErr!= ippSts)
//			{
//				wSts= DSP_ERR_FAIL_IN_IPPI;
//				goto EXIT_DSP_PhaseOffset;
//			}
//
//			ippSts= ippiSumWindowRow_8u32f_C1R(
//				pubbS1->ptr	+ (coOff.y+ szHF.y)*pubbS1->linestep+ (coOff.x+ szHF.x), pubbS1->linestep*sizeof(IMG_UBYTE)	,
//				rbS1.ptr	+ (         szHF.y)*   rbS1.linestep+ (         szHF.x), rbS1.linestep*sizeof(IMG_REAL)		,
//				ipproiSize, 1+ (szHF.x<< 1), szHF.x);
//
//			if (ippStsNoErr!= ippSts)
//			{
//				wSts= DSP_ERR_FAIL_IN_IPPI;
//				goto EXIT_DSP_PhaseOffset;
//			}
//		}
//	}
//
//	if (stInfo.rfx> 0.0f)	szOp.x= (IMG_UWORD) (floor((szOpsz.x- 2*szHF.x)*stInfo.rfx)/stInfo.rfx);		// trunciate to integer period along x
//	if (0== szOp.x)	szOp.x= szOpsz.x;		// period x too large
//
//	if (stInfo.rfy> 0.0f)	szOp.y= (IMG_UWORD)	(floor((szOpsz.y- 2*szHF.y)*stInfo.rfy)/stInfo.rfy);		// trunciate to integer period along y
//	if (0== szOp.y)	szOp.y= szOpsz.y;		// period y too large
//
//	ipproiSize.width	= szOp.x- (szHF.x<< 1);
//	ipproiSize.height	= szOp.y- (szHF.y<< 1);
//			
//	ippSts= ippiMean_StdDev_32f_C1R(rbS0.ptr+ szHF.y*rbS0.linestep+ szHF.x,
//		rbS0.linestep*sizeof(IMG_REAL), ipproiSize, &alrMean[0], &alrStd[0]);
//
//	if (ippStsNoErr!= ippSts)
//	{
//		wSts= DSP_ERR_FAIL_IN_IPPI;
//		goto EXIT_DSP_PhaseOffset;
//	}
//
//	ippSts= ippiMean_StdDev_32f_C1R(rbS1.ptr+ szHF.y*rbS1.linestep+ szHF.x,
//		rbS1.linestep*sizeof(IMG_REAL), ipproiSize, &alrMean[1], &alrStd[1]);
//
//	if (ippStsNoErr!= ippSts)
//	{
//		wSts= DSP_ERR_FAIL_IN_IPPI;
//		goto EXIT_DSP_PhaseOffset;
//	}
//
//	if (fabs(alrStd[0])	< DSP_PPRACT_ZERO||
//		fabs(alrStd[1])	< DSP_PPRACT_ZERO||
//		fabs(alrMean[0])< DSP_PPRACT_ZERO||
//		fabs(alrMean[1])< DSP_PPRACT_ZERO)
//		return DSP_ERR_DIVIDED_BY_ZERO;
//
//	//lrRatio= (alrStd[0]*alrMean[1])/(alrStd[1]*alrMean[0]);
//	//lrRatio= 1.0f;
//	//if (fabs(1.0- lrRatio)> 0.05)      // check 5 percentage change
//	//{
//	//	wSts= DSP_ERR_DIVIDED_BY_ZERO;
//	//	goto EXIT_DSP_PhaseOffset;
//	//}
//
//	rbDiff.size= szOp;
//	rbDiff.linestep= rbDiff.size.x;
//	if (NULL== (rbDiff.ptr= (IMG_REAL *) MEM_Malloc(szOp.x*szOp.y*sizeof(IMG_REAL), SYS_BANK1_8)))
//	{
//		wSts= MEM_ERR_MALLOC;
//		goto EXIT_DSP_PhaseOffset;
//	}
//	memset(rbDiff.ptr, 0, szOp.x*szOp.y*sizeof(IMG_REAL));
//
//	prS0= rbS0.ptr;
//	prS1= rbS1.ptr;
//	prDiff= rbDiff.ptr;
//
//	uwStep0= rbS0.linestep- szOp.x;
//	uwStep1= rbS1.linestep- szOp.x;
//
//	for (j= 0; j< rbDiff.size.y; j++, prS0+= uwStep0, prS1+= uwStep1)
//		for (i= 0; i< rbDiff.size.x; i++, prDiff++, prS0++, prS1++)
//			*prDiff= (IMG_REAL) ((*prS0- alrMean[0])- (*prS1- alrMean[1])*alrStd[0]/alrStd[1]);
//
//	ippSts= ippiMean_StdDev_32f_C1R(rbDiff.ptr+ szHF.y*rbDiff.linestep+ szHF.x,
//		rbDiff.linestep*sizeof(IMG_REAL), ipproiSize, &alrMean[2], &alrStd[2]);
//
//	if (ippStsNoErr!= ippSts)
//	{
//		wSts= DSP_ERR_FAIL_IN_IPPI;
//		goto EXIT_DSP_PhaseOffset;
//	}
//
//	*prOff= (IMG_REAL) (2.0*asin(alrStd[2]/(2*alrStd[0])));
//	if (fabs(*prOff)< 0.00001f)	goto EXIT_DSP_PhaseOffset;		// Same images, no intensity difference
//
//	// Check offset sign 
//	{
//		IMG_UWORD
//			n		= 0,
//			n_f		= 0,						// best forward n
//			n_b		= 0,						// best backward n
//			N		= 0,						// number of shift
//			uwTh	= 0,						// half period
//			uwOff	= 0;						// unsigned offset in pixels
//
//		IMG_REAL
//			rT		= 0.0f,								// Period
//			rSign	= 1.0f,								// Sign of sift
//			rOff_l	= (IMG_REAL) fabs(*prOff)- 0.6f,	// lower bound
//			rOff_u	= (IMG_REAL) fabs(*prOff)+ 0.6f,	// upper bound	
//			rOff	= 0.0f;								// subpixel offset
//
//		IMG_LREAL
//			lr2b		= 0.0f,					// fitting value b
//			lr2a		= 0.0f,					// fitting value a
//			lrDiff_f = LREAL_MAX,				// forward difference
//			lrDiff_b = LREAL_MAX;				// backward differenc
//
//		IMG_COORD
//			coCtr	={0, 0};					// Image center
//
//		IMG_LREAL
//			*plrDiff	= NULL,					// phase diff
//			*plrDiff_f	= NULL,					// Discrepency forward
//			*plrDiff_b	= NULL;					// Discrepency backward
//
//		coCtr.x= szOp.x>> 1;
//		coCtr.y= szOp.y>> 1;
//
//		rOff_l= IMG_MAX(rOff_l, 0);
//		N= (IMG_UWORD) ((rOff_u- rOff_l)/DSP_PHASE_STEP+ 1.0f);
//
//		if (NULL== (plrDiff_f= (IMG_LREAL *) MEM_Malloc((N<< 1)*sizeof(IMG_LREAL), SYS_BANK1_8)))
//		{
//			wSts= MEM_ERR_MALLOC;
//			goto EXIT_DSP_PhaseOffset;
//		}
//		memset(plrDiff_f, 0, (N<< 1)*sizeof(IMG_LREAL));
//		plrDiff_b= plrDiff_f+ N;
//
//		if (stInfo.rfx> stInfo.rfy)
//		{
//			rT		= 1.0f/stInfo.rfx;
//			uwStep0 = 1;	uwStep1 = 1;
//			uwTh	= (IMG_UWORD)	(0.8f*(szOp.x>> 1)+ 0.5f);			// Half width to compare intensity series
//		}
//		else
//		{
//			rT		= 1.0f/stInfo.rfy;
//			uwStep0 = rbS0.linestep;		uwStep1 = rbS1.linestep;
//			uwTh	= (IMG_UWORD)	(0.8f*(szOp.y>> 1)+ 0.5f);			// Half width to compare intensity series
//		}
//		
//		for (n= 0; n< N; n++)
//		{
//
//			rOff	= (IMG_REAL) (rT*(rOff_l+ n*DSP_PHASE_STEP)/MAT_2PI);			// equivalent offset
//			uwOff	= (IMG_UWORD) rOff;
//			rOff	-= uwOff;
//
//			prS0= rbS0.ptr+ coCtr.y*rbS0.linestep+ coCtr.x- uwTh*uwStep0;
//			prS1= rbS1.ptr+ coCtr.y*rbS1.linestep+ coCtr.x- uwTh*uwStep1;
//
//			for (i= 0; i< (uwTh<< 1)+ 1; i++)
//			{
//				plrDiff_f[n]+= fabs(
//					(((1- rOff)*(*(prS0+ (i+0)*uwStep0+ uwOff))+(rOff- 0)*(*(prS0+ (i+1)*uwStep0+ uwOff)))- alrMean[0])/alrStd[0]-
//					(*(prS1+ i*uwStep1       )- alrMean[1])/alrStd[1]);
//
//				plrDiff_b[n]+= fabs(
//					(((1- rOff)*(*(prS0+ (i+0)*uwStep0- uwOff))+(rOff- 0)*(*(prS0+ (i-1)*uwStep0- uwOff)))- alrMean[0])/alrStd[0]-
//					(*(prS1+ i*uwStep1       )- alrMean[1])/alrStd[1]);
//
//			}
//
//			if (plrDiff_f[n]< lrDiff_f)
//			{
//				lrDiff_f= plrDiff_f[n];
//				n_f		= n;
//			}
//
//			if (plrDiff_b[n]< lrDiff_b)
//			{
//				lrDiff_b= plrDiff_b[n];
//				n_b		= n;
//			}
//		}
//
//		if (lrDiff_b< lrDiff_f)
//		{
//			plrDiff= plrDiff_b;
//			n= n_b;
//			rSign= -1.0f;
//		}
//		else
//		{
//			plrDiff= plrDiff_f;
//			n= n_f;
//			rSign=  1.0f;
//		}
//
//		*prOff= rSign*(rOff_l+ n*DSP_PHASE_STEP);			// coarse fitting
//
//		if (0== n && fabs(rOff_l)< (0.1f*DSP_PHASE_STEP))					// 
//		{
//			lr2b= plrDiff_f[1]- plrDiff_b[1];
//			lr2a= plrDiff_f[1]+ plrDiff_b[1]- 2.0*plrDiff_f[0];				// note b is -f
//
//			if (fabs(lr2a)> 0.0001)
//				*prOff-= (IMG_REAL) (0.5f*rSign*DSP_PHASE_STEP*lr2b/lr2a);
//		}
//		else if (0< n && n< N)		// do interpolation
//		{
//			lr2b= plrDiff[n+ 1]- plrDiff[n- 1];
//			lr2a= plrDiff[n+ 1]+ plrDiff[n- 1]- 2.0*plrDiff[n];
//
//			if (fabs(lr2a)> 0.0001)
//				*prOff-= (IMG_REAL) (0.5f*rSign*DSP_PHASE_STEP*lr2b/lr2a);
//		}
//		else
//		{
//			wSts= DSP_ERR_ALL_UNIMODAL;
//		}
//		*prOff= -(*prOff);
//	}
//
//EXIT_DSP_PhaseOffset:
//	if (OK!= (wMEMSts= MEM_PopAllHeapStatus()))	return wMEMSts;
//	return wSts;
//}
//

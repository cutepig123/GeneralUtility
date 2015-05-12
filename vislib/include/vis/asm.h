#pragma once
#include <vis/type.h>
#include <vis/stack.h>

typedef VIS_INT8		IMG_CHAR;
typedef VIS_INT16		IMG_WORD;
typedef VIS_INT32		IMG_LWORD;
typedef VIS_UINT8		IMG_UBYTE;
typedef VIS_UINT16		IMG_UWORD;
typedef VIS_UINT32		IMG_ULWORD;
typedef VIS_FLOAT32		IMG_REAL;
typedef VIS_FLOAT64		IMG_LREAL;
typedef void            IMG_VVOID;

typedef	VIS_SIZE		IMG_SIZE;
typedef	VIS_RSIZE		IMG_RSIZE;

typedef	VIS_POINT		IMG_COORD;
typedef	VIS_RPOINT		IMG_RCOORD;

typedef	VIS_BUF						IMG_UBBUF;
typedef	VIS_BUF_UINT16				IMG_WBUF;
typedef	VIS_RBUF					IMG_RBUF;
typedef	VIS_BUF_T<VIS_FLOAT64>		IMG_LRBUF;

#define SYS_BANK1_32    0x201
#define SYS_BANK1_16    0x001
#define SYS_BANK1_8     0x101

#define	MEM_PushAllHeapStatus	STK_Push
#define	MEM_PopAllHeapStatus	STK_Pop

#define	ASM_ERROR_CODE			-MODULE_ID_ASM
#define	MAT_INVALID_INPUT		ASM_ERROR_CODE -1
#define	DSP_ERR_FAIL_IN_IPPI	ASM_ERROR_CODE -2
#define	DSP_ERR_INVALID_ARG		ASM_ERROR_CODE -3
#define	MEM_ERR_MALLOC			ASM_ERROR_CODE -4
#define	DSP_ERR_DIVIDED_BY_ZERO ASM_ERROR_CODE -5
#define	DSP_ERR_INVALID_SIZE	ASM_ERROR_CODE -6
		
#define	OK	VIS_OK

#ifndef NULL
	#define NULL 0
#endif

#define	IMG_MAX(a,b)	((a)>(b)?(a):(b))
#define	IMG_MIN(a,b)	((a)<(b)?(a):(b))

#define	MAT_2PI	2*3.14159265f


#define BYTE_MAX        (0x7F)
#define BYTE_MIN        (-BYTE_MAX-1)
#define WORD_MAX        (0x7FFF)
#define WORD_MIN        (-WORD_MAX-1)
#define LWORD_MAX       (0x7FFFFFFF)
#define LWORD_MIN       (-LWORD_MAX-1)
#define XLWORD_MAX      ((IMG_REAL)(9.2E+18))
#define XLWORD_MIN      (-XLWORD_MAX-1)

#define UBYTE_MAX       (0xFF)
#define UBYTE_MIN       (0x0)
#define UWORD_MAX       (0xFFFF)
#define UWORD_MIN       (0x0)
#define ULWORD_MAX      (0xFFFFFFFF)
#define ULWORD_MIN      (0x0)
#define UXLWORD_MAX     ((IMG_REAL)(1.8E+19))
#define UXLWORD_MIN     (0x0)


#ifndef REAL_MAX // window has defined this macro somewhere
#define REAL_MAX        ((IMG_REAL)(3.4E+38))
#endif
#ifndef REAL_MIN // window has defined this macro somewhere
#define REAL_MIN        ((IMG_REAL)(-3.4E+38))
#endif

#define REAL_ABS_MIN    ((IMG_REAL)(1.1E-38))

#ifndef LREAL_MAX // window has defined this macro somewhere
#define LREAL_MAX       ((IMG_LREAL)(1.79E+308))
#endif
#ifndef LREAL_MIN // window has defined this macro somewhere
#define LREAL_MIN       ((IMG_LREAL)(-1.79E+308))
#endif

#define	SYS_Assert	assert

inline	IMG_WORD	DSP_MapSYSBank(IMG_UWORD uwBank)
{
	switch(uwBank)
	{
	case SYS_BANK1_8:
		return 1;
	case SYS_BANK1_16:
		return 2;
	case SYS_BANK1_32:
		return 4;
	default:
		assert(0);
		return 0;
	}
}

inline	IMG_VVOID*	MEM_Malloc(IMG_ULWORD ulSize, IMG_UWORD uwBank)
{
	VIS_VOID* ptr =0;
	IMG_ULWORD ulN = ulSize*DSP_MapSYSBank(uwBank);
	STK_Malloc( ulN, &ptr );
	return ptr;
}

template <class T>
VIS_XXPORT IMG_WORD DSP_AddC_impl(
	VIS_BUF_T<T>			const	*prbSbuf,
	IMG_COORD				const	*pcoSoff,
	T						const	rConst,
	VIS_BUF_T<T>			*prbDbuf,
	IMG_COORD		const	*pcoDoff,
	IMG_SIZE		const	*pszOpsize);

#define DSP_AddC_rrr DSP_AddC_impl

template <class T>
VIS_XXPORT IMG_WORD DSP_FillBuffer_impl(T	const rValue,
	VIS_BUF_T<T>		  *prbBuf,
	IMG_COORD const *pcoOff,
	IMG_SIZE	const *pszSize);

#define DSP_FillBuffer_r DSP_FillBuffer_impl

template <class T, class T2>
VIS_XXPORT IMG_WORD  DSP_Copy_impl(VIS_BUF_T<T>	const *prbSbuf,
	IMG_COORD const *pcoSoff,
	VIS_BUF_T<T2>		  *prbDbuf,
	IMG_COORD const *pcoDoff,
	IMG_SIZE	const *pszOpsize);

#define  DSP_Copy_rr DSP_Copy_impl
#define  DSP_Copy_ubr DSP_Copy_impl
#define  DSP_Copy_rub DSP_Copy_impl

#include <ippi.h>
#include <ippm.h>
#include <ippcv.h>
//#define	IppStatus	IMG_WORD
//#define	Ipp32f		IMG_REAL
//
//IppStatus ippmMul_tm_32f(const Ipp32f* pSrc, int srcStride1,
//	int srcStride2, int width, int height, Ipp32f* pDst, int dstStride1,
//	int dstStride2);
//
//IppStatus ippmInvert_m_32f(const Ipp32f* pSrc, int srcStride1, int srcStride2, Ipp32f* pBuffer, Ipp32f* pDst, int dstStride1, int dstStride2, int widthHeight);;
//
//IppStatus ippmMul_mt_32f(const Ipp32f* pSrc1, int src1Stride1, int src1Stride2, int src1Width, int src1Height, const Ipp32f* pSrc2, int src2Stride1, int src2Stride2, int src2Width, int src2Height, Ipp32f* pDst, int dstStride1, int dstStride2);
//IppStatus ippmMul_mm_32f(const Ipp32f* pSrc1, int src1Stride1, int src1Stride2, int src1Width, int src1Height, const Ipp32f* pSrc2, int src2Stride1, int src2Stride2, int src2Width, int src2Height, Ipp32f* pDst, int dstStride1, int dstStride2);
//


//============= Level 1. Basic sine boolean filter parameter =============
typedef struct
{
	IMG_REAL	rPeriod;
	IMG_REAL	rAmplitude;
	IMG_REAL	rOrientation;			// Support 0 or 90 degrees only
	IMG_UWORD	uwSKernel;				// half smoothing kernel
	IMG_UWORD	auwDummy[4];
	IMG_REAL	arDummy[4];
} DSP_SIN_PARA;

//============= Level 2. sine model parameter/information =============
//	DSP_SININFO_NOPREFENCE	:		unknown fringe orientation
//	DSP_SININFO_XDOMINATE	:		search X frequency only
//	DSP_SININFO_YDOMINATE	:		search Y frequency only

#define	DSP_SININFO_NOPREFERENCE	0
#define	DSP_SININFO_XDOMINATE		1
#define	DSP_SININFO_YDOMINATE		2

typedef struct
{
	IMG_REAL	rThreshold;		// Amplitude Threshold
	IMG_REAL	rFreqRange;		// Frequency range 
	IMG_UWORD	uwPreference;	// 
	IMG_REAL	rMinFreqX;
	IMG_REAL	rMinFreqY;
	IMG_REAL	arDummy[4];
}	DSP_SININFO_PARA;

// B+ A*cos(rfx*x+ Phi)
typedef struct
{
	IMG_REAL	rfx;					// frequnecy X
	IMG_REAL	rfy;					// frequency Y
	IMG_REAL	rB;						// Amplitude
	IMG_REAL	rA;						// Offset
	IMG_REAL	rB_SD;					// Variation in B
	IMG_REAL	arDummy[4];
}	DSP_SIN_INFO;


//============= Level 2.5 not uniform sine model parameter/information =============
typedef struct
{
	IMG_UWORD	uwMultiPX;		// force multiple period X
	IMG_UWORD	uwFilterY;		// force filtering along Y
}	DSP_PPSININFO_PARA;

typedef struct					// malloc size equal to opsz y
{
	IMG_UBYTE	*pubValid;		// warning user malloc ! return immediately if not malloc
	IMG_REAL	*prPhi;			// warning user malloc !
	IMG_REAL	*prWx;			// warning user malloc !
	IMG_REAL	*prWxx;			// warning user malloc !
	IMG_RBUF	rbA;			// warning user malloc !
	IMG_RBUF	rbB;			// warning user malloc !
}	DSP_PPSIN_INFO_X;

typedef struct {						// "vector ptr"."vector x" - q = 0
	IMG_UWORD	M;						// dimension of "vector ptr". fill in this when malloc
	IMG_REAL   *ptr;						// remember to malloc pointer "*ptr" !!!
	IMG_REAL	q;						// distance from plane to origin
}	MAT_HPLANE;

typedef	struct
{
	IMG_SIZE	size;
	IMG_UWORD	*rNum;
	IMG_UWORD	*cNum;
	IMG_UWORD	*rIndex;
	IMG_UWORD	*cIndex;
	IMG_REAL	*rValue;
	IMG_REAL	*cValue;
}	MAT_RSM;

typedef struct
{
	IMG_UWORD	uw0;
	IMG_UWORD	uw1;
	IMG_UWORD	uw2;
	IMG_UWORD	uw3;
	IMG_UWORD	*puw0;
	IMG_UWORD	*puw1;
	IMG_UWORD	*puw2;
	IMG_UWORD	*puw3;
	IMG_REAL	r0;
	IMG_REAL	r1;
	IMG_REAL	r2;
	IMG_REAL	r3;
	IMG_REAL	*pr0;
	IMG_REAL	*pr1;
	IMG_REAL	*pr2;
	IMG_REAL	*pr3;
}	MAT_OPTION;


typedef struct
{
	IMG_UWORD	SpaChol;				// 0 for dense,1 for sparse
	IMG_UWORD	SpaMask;				// 0 for none, 1 for build, 2 for apply
	IMG_UWORD	RSAXBI;					// 0 for disable,>1 for enable RS's AXB's inequality
	IMG_UWORD	RSFit;					// 0 for normal,  1 for user defined fittness
	IMG_UWORD	Parall;					// 0 for normal,  1 for parallel processing
	IMG_UWORD	EApprox;				// 0 for 1/(1+xx),1 for 1/(1+xx+xxxx)
	IMG_REAL	rLUConstrainStop;		// >0 for activation, if all weighting smaller than rLUConstrainStop, fitting stops
	IMG_UWORD	uwDummy3;
	IMG_UWORD	TimeRS;					// "=0":turn off RS
	IMG_UWORD	TimeML;					// "=0":turn off ML
	IMG_REAL	NoiseRS;				// "<0":turn off RS
	IMG_REAL	NoiseML;				// "<0":turn off ML,iteration for inequality only
	IMG_REAL	UConRat;				// U-constraint:ratio
	IMG_REAL	UConWid;				// U-constraint:width
	IMG_REAL	DeltaX;					// for all "MAT_FitAXB" functions
	IMG_REAL	Del_W0;					// for "MAT_FitAXB1" or newer
	IMG_REAL	Del_W1;					// for "MAT_FitAXB1" or newer
	IMG_REAL	NoiseM0;				// NoiseML (initial NoiseML, should be larger than NoiseML)
	IMG_REAL	NoiseMF;				// NoiseML (reducing factor, should be within [0,1])
	IMG_REAL	RSLogPF;				// for RS auto-iteration : Log(Prob(Failure))
	IMG_REAL	UConMax;
	//IMG_REAL(*UserFct)  (IMG_REAL			rInput,
	//	IMG_WORD			wFlag);
	//IMG_REAL(*UserFRS)  (MAT_RSM		const	*pstA,
	//	IMG_RBUF	const	*prbA,
	//	IMG_RBUF	const	*prbB,
	//	IMG_REAL	const	*prW,
	//	MAT_HPLANE	const	*pstHP,
	//	MAT_OPTION	const	*pstOpt);
}	MAT_FITPARA;

IMG_WORD	MAT_FitAXBInit(MAT_FITPARA		const	*pstPara0,
	MAT_FITPARA				*pstPara1);

IMG_WORD	MAT_FitAXB1_T(MAT_RSM					*pstSMM,
	MAT_RSM			const	*pstA,
	IMG_RBUF		const	*prbA,
	IMG_RBUF		const	*prbAT,
	IMG_RBUF		const	*prbB,
	MAT_RSM			const	*pstCi,
	IMG_RBUF		const	*prbCi,
	IMG_RBUF		const	*prbDi,
	IMG_LRBUF		const	*plrbCe,
	IMG_LRBUF		const	*plrbDe,	// *** linestep=1 *** //
	IMG_WORD		const	*pwM,
	IMG_REAL				*prW,
	MAT_FITPARA		const	*pstPara,
	MAT_HPLANE		const	*pstHP0,
	MAT_HPLANE				*pstHP1,
	IMG_REAL				*prScore,
	IMG_UWORD		const	uwDebug);

#include <dsp_lib.h>
#include "atceagle.h"

/*	
	2009-11-28
	JiaoRongmei

	Purpose:
		Function DSP_Fast_Atan2_Table implement atan2 by combine look up table and linear interpolation together.
	
	Impletation:
		This function first divide [0, 1] to 8 sections(i.e.[0, 0.125], [0.125, 0.25], ..., [0.875, 1]),
		and then construct 8 tables(see 2d array arTable in the program).
			Table[0]:	x	>=	0,	y	>=	0	and		|y|	>	|x|
			Table[1]:	x	>=	0,	y	>=	0	and		|y|	<=	|x|
			Table[2]:	x	<	0,	y	>=	0	and		|y|	>	|x|
			Table[3]:	x	<	0,	y	>=	0	and		|y|	<=	|x|
			Table[4]:	x	>=	0,	y	<	0	and		|y|	>	|x|
			Table[5]:	x	>=	0,	y	<	0	and		|y|	<=	|x|
			Table[6]:	x	<	0,	y	<	0	and		|y|	>	|x|
			Table[7]:	x	<	0,	y	<	0	and		|y|	<=	|x|

		For example:
			y = 2.5173, x = 3
			tmp = (2.5173/3)*8 = 0.8391*8 = 6.7128
			so,
			table = 1, position = 6, k = 0.7128
			result	= Table[table][position]*(1-k)+Table[table][position+1]*k
					= 36.86989764 * (1-0.7128) + 41.18592516 * 0.7128
					= 39.946
	Accuracy:
		0.08 degree
*/

static	IMG_VVOID	Library_atan(IMG_REAL	const *prX,
								 IMG_REAL	const *prY,
								 IMG_REAL	*prResult,
								 IMG_ULWORD	ulNum)
{
	IMG_ULWORD	i;
	for( i = 0; i < ulNum; i++ )
	{
		*(prResult+i) = (IMG_REAL)atan2(*(prY+i), *(prX+i));
		*(prResult+i) *= (IMG_REAL)57.295779511437;
	}
}

#ifdef		_WIN64

#define		asm_si		rsi
#define		asm_di		rdi
#define		asm_dx		rdx
#define		asm_ax		rax
#define		asm_bx		rbx

#else

#define		asm_si		esi
#define		asm_di		edi
#define		asm_dx		edx
#define		asm_ax		eax
#define		asm_bx		ebx

#endif

/*
_macro_definition
{
	Interpolation MACRO
		movdqa		xmm3, xmm6		
		cmpleps		xmm3, xmm5					//if( |y|<=|x| )	use table1, table3, table5 or talbe7
												//ele				use table0, table2, table4 or table6
		movdqa		xmm7, xmm6
		maxps		xmm7, xmm5					//xmm7 large_number
		minps		xmm6, xmm5					//xmm6 small_number		
		
		pxor		xmm0, xmm0		
		cmpeqps		xmm0, xmm7					//if( large_number == 0 )		large_number = 1e-7
		pslld		xmm1, 1						//compute table
		pand		xmm0, xmm4
		por			xmm7, xmm0
		psrld		xmm3, 31					//compute table
		divps		xmm6, xmm7
		por			xmm1, xmm3					//compute table
		pslld		xmm1, 6						//multiply the length of the row of the table to determin the row position
		
		movdq2q		mm7, xmm1
		mulps		xmm6, xmm2					//(small_number/large_number)*8 to compute position of the table
		psrldq		xmm1, 8

		cvttps2dq	xmm0, xmm6					//compute position
		movq2dq		xmm5, mm2
		cvtdq2ps	xmm3, xmm0
		pslld		xmm0, 2
		movdq2q		mm6, xmm0		
		punpcklqdq	xmm5, xmm5
		subps		xmm6, xmm3					//(least significant)k1, k2, k3, k4(most significant)
		psrldq		xmm0, 8						//k1, k2, k3 and k4 means k of number1, number2, number3 and number4
		movdqa		xmm3, xmm6
		subps		xmm5, xmm6					//(least significant)1-k1, 1-k2, 1-k3, 1-k4(most significant)
		paddd		mm7, mm6					//1-k1, 1-k2, 1-k3, 1-k4 means 1-k of number1, number2, number3 and number4

		shufps		xmm6, xmm5, 0x44
		movd		ebx, mm7
		pshufd		xmm6, xmm6, 0x072			//1-k1,k1,1-k2,k2
		add			asm_bx, asm_ax
		psrlq		mm7, 32
		shufps		xmm3, xmm5, 0x0ee
		movq		xmm5, qword ptr[asm_bx]		//Table[table][position] and Table[table][position+1] for number1
		movd		ebx, mm7
		add			asm_bx, asm_ax
		movq		xmm7, qword ptr[asm_bx]		//Table[table][position] and Table[table][position+1] for number2
		movdq2q		mm7, xmm1
		movlhps		xmm5, xmm7
		pshufd		xmm3, xmm3, 0x072			//1-k3,k3,1-k4,k4
		mulps		xmm6, xmm5
		pshufd		xmm5, xmm6, 0xf5
		movdq2q		mm6, xmm0
		addps		xmm6, xmm5
		paddd		mm7, mm6		

		movd		ebx, mm7
		add			asm_bx, asm_ax
		movq		xmm0, qword ptr[asm_bx]		//Table[table][position] and Table[table][position+1] for number3
		psrlq		mm7, 32
		movd		ebx, mm7
		add			asm_bx, asm_ax
		movq		xmm1, qword ptr[asm_bx]		//Table[table][position] and Table[table][position+1] for number4
		pshufd		xmm6, xmm6, 8
		movlhps		xmm0, xmm1
		mulps		xmm0, xmm3
		pshufd		xmm7, xmm0, 0x0f5
		addps		xmm0, xmm7
		pshufd		xmm0, xmm0, 8
		movlhps		xmm6, xmm0
ENDM
}
*/

//lint -esym(529,rSmall,r1f,r8f,ulRemainder,arTable)
IMG_WORD	DSP_Fast_Atan2_Table(	IMG_REAL	const *prX,
									IMG_REAL	const *prY,
									IMG_REAL	*prResult,			//degree
									IMG_ULWORD	const ulNum)
{
	IMG_WORD	wStatus = OK;
	IMG_ULWORD	ulRemainder=ulNum&3;
	IMG_REAL	rSmall = (IMG_REAL)1E-7, r8f=8, r1f=1;
	IMG_REAL	arTable[8][16] = /* the length of each row of the table is 64 bytes*/
	{	90.0f,	82.87498365f,	75.96375653f,	69.44395478f,	63.43494882f,	57.99461679f,	53.13010236f,	48.81407484f,	45.0f,		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f,	7.125016349f,	14.03624347f,	20.55604522f,	26.56505118f,	32.00538321f,	36.86989764f,	41.18592516f,	45.0f,		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		90.0f,	97.12501635f,	104.0362435f,	110.5560452f,	116.5650512f,	122.0053832f,	126.8698976f,	131.1859252f,	135.0f,		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		180.0f,	172.8749837f,	165.9637565f,	159.4439548f,	153.4349488f,	147.9946168f,	143.1301024f,	138.8140748f,	135.0f,		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-90.0f,	-82.87498365f,	-75.96375653f,	-69.44395478f,	-63.43494882f,	-57.99461679f,	-53.13010236f,	-48.81407484f,	-45.0f,		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f,	-7.125016349f,	-14.03624347f,	-20.55604522f,	-26.56505118f,	-32.00538321f,	-36.86989764f,	-41.18592516f,	-45.0f,		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-90.0f,	-97.12501635f,	-104.0362435f,	-110.5560452f,	-116.5650512f,	-122.0053832f,	-126.8698976f,	-131.1859252f,	-135.0f,	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		-180.0f,-172.8749837f,	-165.9637565f,	-159.4439548f,	-153.4349488f,	-147.9946168f,	-143.1301024f,	-138.8140748f,	-135.0f,	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	IMG_ULWORD	ulCPU_FEATURE;
	IMG_UBYTE	ubSSE_MMX_Support;
	
	if(prX == NULL || prY == NULL || prResult == NULL)
	{
		wStatus = DSP_ERR_INVALID_ARG;
		goto EXIT;
	}

	//get cpu feature
	DSP_Get_CPU_FEATURE(&ulCPU_FEATURE);
	//get sse support label
	DSP_Get_SSESupport(&ubSSE_MMX_Support);
	ubSSE_MMX_Support &= ATC_EAGLE_SSE;
	if  ( (ulCPU_FEATURE&ATC_EAGLE_CPU_FEATURE_FOR_SSE2) == 0 || ubSSE_MMX_Support == IMG_FALSE )
	{
		Library_atan(prX, prY, prResult, ulNum);
		goto EXIT;
	}	

	__asm
	{
		//--->Initialization
		mov			asm_si, prX
		mov			asm_dx, prY
		lea			asm_ax, arTable
		mov			asm_di, prResult
		movd		xmm4, rSmall
		pshufd		xmm4, xmm4, 0
		movd		xmm2, r8f					//r8f: 8 poins in [0~1], multiply 8 to compute position of table
		movd		mm2, r1f					//r1f for computing 1-k of interpolation
		pshufd		xmm2, xmm2, 0		
		punpckldq	mm2, mm2

		push		asm_bx

		mov			ecx, ulNum
		and			ecx, 0x0fffffffc
		je			loop_over

		sar			ecx, 2
		//<---Initialization

loop_s:											//--->batch computing
		movdqu		xmm6, [asm_dx]				//Y
		pxor		xmm7, xmm7
		add			asm_dx, 16
		movdqa		xmm1, xmm6
		cmpltps		xmm1, xmm7					//xmm1 store table information of number1, number2, number3 and number4
		movdqa		xmm0, xmm6					//-->abs of Y,	if( Y[i] < 0 )	Y[i]=Y[i]-Y[i]-Y[i]
		pand		xmm0, xmm1					//				else			Y[i]=Y[i]-0-0
		subps		xmm6, xmm0
		psrld		xmm1, 31
		subps		xmm6, xmm0					//<--abs of Y in xmm6
		pslld		xmm1, 1						//xmm1 for compute table

		movdqu		xmm5, [asm_si]				//X
		add			asm_si, 16
		movdqa		xmm3, xmm5					//xmm3 store table information of number1, number2, number3 and number4
		cmpltps		xmm3, xmm7
		movdqa		xmm0, xmm5					//-->abs of X
		pand		xmm0, xmm3
		subps		xmm5, xmm0
		psrld		xmm3, 31
		subps		xmm5, xmm0					//<--abs of X in xmm5
		por			xmm1, xmm3					//y<0 && x<0 table6 ~ table7, y<0 && x>=0 table4~talbe5
												//y>=0 && x<0 table2 ~ table3, y>=0 && x>=0 talbe0~table1
		//---->interpolation
		/*
			comupte table, position and k
			then result = Table[table][position]*(1-k)+Table[table][position+1]*k
		*/
		//_macro Interpolation
		movdqa		xmm3, xmm6		
		cmpleps		xmm3, xmm5					//if( |y|<=|x| )	use table1, table3, table5 or talbe7
												//ele				use table0, table2, table4 or table6
		movdqa		xmm7, xmm6
		maxps		xmm7, xmm5					//xmm7 large_number
		minps		xmm6, xmm5					//xmm6 small_number		
		
		pxor		xmm0, xmm0		
		cmpeqps		xmm0, xmm7					//if( large_number == 0 )		large_number = 1e-7
		pslld		xmm1, 1						//compute table
		pand		xmm0, xmm4
		por			xmm7, xmm0
		psrld		xmm3, 31					//compute table
		divps		xmm6, xmm7
		por			xmm1, xmm3					//compute table
		pslld		xmm1, 6						//multiply the length of the row of the table to determin the row position
		
		movdq2q		mm7, xmm1
		mulps		xmm6, xmm2					//(small_number/large_number)*8 to compute position of the table
		psrldq		xmm1, 8

		cvttps2dq	xmm0, xmm6					//compute position
		movq2dq		xmm5, mm2
		cvtdq2ps	xmm3, xmm0
		pslld		xmm0, 2
		movdq2q		mm6, xmm0		
		punpcklqdq	xmm5, xmm5
		subps		xmm6, xmm3					//(least significant)k1, k2, k3, k4(most significant)
		psrldq		xmm0, 8						//k1, k2, k3 and k4 means k of number1, number2, number3 and number4
		movdqa		xmm3, xmm6
		subps		xmm5, xmm6					//(least significant)1-k1, 1-k2, 1-k3, 1-k4(most significant)
		paddd		mm7, mm6					//1-k1, 1-k2, 1-k3, 1-k4 means 1-k of number1, number2, number3 and number4

		shufps		xmm6, xmm5, 0x44
		movd		ebx, mm7
		pshufd		xmm6, xmm6, 0x072			//1-k1,k1,1-k2,k2
		add			asm_bx, asm_ax
		psrlq		mm7, 32
		shufps		xmm3, xmm5, 0x0ee
		movq		xmm5, qword ptr[asm_bx]		//Table[table][position] and Table[table][position+1] for number1
		movd		ebx, mm7
		add			asm_bx, asm_ax
		movq		xmm7, qword ptr[asm_bx]		//Table[table][position] and Table[table][position+1] for number2
		movdq2q		mm7, xmm1
		movlhps		xmm5, xmm7
		pshufd		xmm3, xmm3, 0x072			//1-k3,k3,1-k4,k4
		mulps		xmm6, xmm5
		pshufd		xmm5, xmm6, 0xf5
		movdq2q		mm6, xmm0
		addps		xmm6, xmm5
		paddd		mm7, mm6		

		movd		ebx, mm7
		add			asm_bx, asm_ax
		movq		xmm0, qword ptr[asm_bx]		//Table[table][position] and Table[table][position+1] for number3
		psrlq		mm7, 32
		movd		ebx, mm7
		add			asm_bx, asm_ax
		movq		xmm1, qword ptr[asm_bx]		//Table[table][position] and Table[table][position+1] for number4
		pshufd		xmm6, xmm6, 8
		movlhps		xmm0, xmm1
		mulps		xmm0, xmm3
		pshufd		xmm7, xmm0, 0x0f5
		addps		xmm0, xmm7
		pshufd		xmm0, xmm0, 8
		movlhps		xmm6, xmm0
		// _endm
		//<----interpolation

		movdqu		[asm_di], xmm6
		add			asm_di, 16

		dec			ecx
		jne			loop_s						//<---batch computing

loop_over:
		mov			ecx, ulRemainder			//--->point by point computing
		cmp			ecx, 0
		je			prog_over

		cmp			ecx, 1
		je			last_y

		movq		xmm6, qword ptr[asm_dx]		
		add			asm_dx, 8
		sub			ecx, 2
		je			abs_y		

		movd		xmm1, [asm_dx]
		pslldq		xmm1, 8
		por			xmm6, xmm1
		jmp			abs_y

last_y:
		movd		xmm6, [asm_dx]

abs_y:		
		pxor		xmm7, xmm7
		movdqa		xmm1, xmm6
		cmpltps		xmm1, xmm7					//y<0 table4 ~ table7, y>=0 table0 ~ table3
		movdqa		xmm0, xmm6					//-->abs of Y
		pand		xmm0, xmm1
		subps		xmm6, xmm0
		psrld		xmm1, 31
		subps		xmm6, xmm0					//<--abs of Y in xmm6
		pslld		xmm1, 1						//table information in xmm1

		mov			ecx, ulRemainder
		cmp			ecx, 1
		je			last_x

		movq		xmm5, qword ptr[asm_si]
		add			asm_si, 8
		sub			ecx, 2
		je			abs_x

		movd		xmm3, [asm_si]		
		pslldq		xmm3, 8
		por			xmm5, xmm3		
		jmp			abs_x

last_x:
		movd		xmm5, [asm_si]

abs_x:
		movdqa		xmm3, xmm5
		cmpltps		xmm3, xmm7
		movdqa		xmm0, xmm5					//-->abs of X
		pand		xmm0, xmm3
		subps		xmm5, xmm0
		psrld		xmm3, 31
		subps		xmm5, xmm0					//<--abs of X in xmm5
		por			xmm1, xmm3

		//_macro Interpolation
		movdqa		xmm3, xmm6		
		cmpleps		xmm3, xmm5					//if( |y|<=|x| )	use table1, table3, table5 or talbe7
												//ele				use table0, table2, table4 or table6
		movdqa		xmm7, xmm6
		maxps		xmm7, xmm5					//xmm7 large_number
		minps		xmm6, xmm5					//xmm6 small_number		
		
		pxor		xmm0, xmm0		
		cmpeqps		xmm0, xmm7					//if( large_number == 0 )		large_number = 1e-7
		pslld		xmm1, 1						//compute table
		pand		xmm0, xmm4
		por			xmm7, xmm0
		psrld		xmm3, 31					//compute table
		divps		xmm6, xmm7
		por			xmm1, xmm3					//compute table
		pslld		xmm1, 6						//multiply the length of the row of the table to determin the row position
		
		movdq2q		mm7, xmm1
		mulps		xmm6, xmm2					//(small_number/large_number)*8 to compute position of the table
		psrldq		xmm1, 8

		cvttps2dq	xmm0, xmm6					//compute position
		movq2dq		xmm5, mm2
		cvtdq2ps	xmm3, xmm0
		pslld		xmm0, 2
		movdq2q		mm6, xmm0		
		punpcklqdq	xmm5, xmm5
		subps		xmm6, xmm3					//(least significant)k1, k2, k3, k4(most significant)
		psrldq		xmm0, 8						//k1, k2, k3 and k4 means k of number1, number2, number3 and number4
		movdqa		xmm3, xmm6
		subps		xmm5, xmm6					//(least significant)1-k1, 1-k2, 1-k3, 1-k4(most significant)
		paddd		mm7, mm6					//1-k1, 1-k2, 1-k3, 1-k4 means 1-k of number1, number2, number3 and number4

		shufps		xmm6, xmm5, 0x44
		movd		ebx, mm7
		pshufd		xmm6, xmm6, 0x072			//1-k1,k1,1-k2,k2
		add			asm_bx, asm_ax
		psrlq		mm7, 32
		shufps		xmm3, xmm5, 0x0ee
		movq		xmm5, qword ptr[asm_bx]		//Table[table][position] and Table[table][position+1] for number1
		movd		ebx, mm7
		add			asm_bx, asm_ax
		movq		xmm7, qword ptr[asm_bx]		//Table[table][position] and Table[table][position+1] for number2
		movdq2q		mm7, xmm1
		movlhps		xmm5, xmm7
		pshufd		xmm3, xmm3, 0x072			//1-k3,k3,1-k4,k4
		mulps		xmm6, xmm5
		pshufd		xmm5, xmm6, 0xf5
		movdq2q		mm6, xmm0
		addps		xmm6, xmm5
		paddd		mm7, mm6		

		movd		ebx, mm7
		add			asm_bx, asm_ax
		movq		xmm0, qword ptr[asm_bx]		//Table[table][position] and Table[table][position+1] for number3
		psrlq		mm7, 32
		movd		ebx, mm7
		add			asm_bx, asm_ax
		movq		xmm1, qword ptr[asm_bx]		//Table[table][position] and Table[table][position+1] for number4
		pshufd		xmm6, xmm6, 8
		movlhps		xmm0, xmm1
		mulps		xmm0, xmm3
		pshufd		xmm7, xmm0, 0x0f5
		addps		xmm0, xmm7
		pshufd		xmm0, xmm0, 8
		movlhps		xmm6, xmm0
		// _endm

		mov			ecx, ulRemainder
		cmp			ecx, 1
		je			last_one

		movq		qword ptr[asm_di], xmm6
		add			asm_di, 8
		psrldq		xmm6, 8
		sub			ecx, 2
		je			prog_over

last_one:
		movd		[asm_di], xmm6						//<---point by point computing

prog_over:

		pop			asm_bx
		emms
	}	

EXIT:
	return wStatus;
}







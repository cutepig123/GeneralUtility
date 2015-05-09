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

typedef	VIS_BUF			IMG_UBBUF;
typedef	VIS_BUF_UINT16	IMG_WBUF;
typedef	VIS_RBUF		IMG_RBUF;

#define SYS_BANK1_32    0x201
#define SYS_BANK1_16    0x001
#define SYS_BANK1_8     0x101

#define	MEM_PushAllHeapStatus	STK_Push
#define	MEM_PopAllHeapStatus	STK_Pop

#define	OK	VIS_OK

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
	VIS_VOID* p =0;
	STK_Malloc( ulSize*DSP_MapSYSBank(uwBank), *p );
	return p;
}


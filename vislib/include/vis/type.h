#pragma once
#include <assert.h>

typedef signed char         VIS_INT8;
typedef signed short        VIS_INT16;
typedef signed int          VIS_INT32;
typedef unsigned char       VIS_UINT8;
typedef unsigned short      VIS_UINT16;
typedef unsigned int        VIS_UINT32;
typedef float               VIS_FLOAT32;
typedef double              VIS_FLOAT64;
typedef void                VIS_VOID;

template <class T>
struct VIS_SIZE_T
{
	typedef	T	type;
	
    T     x;
    T     y;
};

typedef VIS_SIZE_T<VIS_UINT32>	VIS_SIZE;
typedef VIS_SIZE_T<VIS_FLOAT32>	VIS_RSIZE;

template <class T>
struct VIS_POINT_T
{
	typedef	T	type;
	
    T     x;
    T     y;
};

typedef VIS_POINT_T<VIS_UINT32>		VIS_POINT;
typedef VIS_POINT_T<VIS_FLOAT32>	VIS_RPOINT;

template <class T>
struct VIS_BUF_T
{
	typedef	T	type;
	
	T 			*ptr;
	VIS_SIZE 	size;
	VIS_UINT32 	linestep;
};

typedef VIS_BUF_T<VIS_UINT8>		VIS_BUF;
typedef VIS_BUF_T<VIS_UINT16>		VIS_BUF_UINT16;
typedef VIS_BUF_T<VIS_FLOAT32>		VIS_RBUF;

#define VIS_ERR_INV_ARG			-1

#define	VIS_OK					0
#define	VIS_ERR(x)				((x)<VIS_OK)
#define	VIS_WARN(x)				((x)>=VIS_OK)
#define	VIS_CHK_STS				if(VIS_ERR(wSts)) { assert(0); goto Exit; }
#define	VIS_CHK_ASSERT(x)		wSts =(x)?VIS_OK:VIS_ERR_INV_ARG; VIS_CHK_STS

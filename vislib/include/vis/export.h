#pragma once

#if defined( VISLIB_EXPORTS )
#define	VIS_XXPORT	__declspec(dllexport)
#else
#define	VIS_XXPORT	__declspec(dllimport)
#endif	//	VIS_EXPORT

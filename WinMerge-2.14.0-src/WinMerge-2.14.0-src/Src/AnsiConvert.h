/** 
 * @file  AnsiConvert.h
 *
 * @brief Declaration file for routines to convert from Unicode to ANSI.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: AnsiConvert.h 5554 2008-07-03 19:39:57Z kimmov $

#ifndef _ANSI_CONVERST_H_
#define _ANSI_CONVERT_H_

LPCSTR ansiconvert_SystemCP(LPCTSTR str);
LPCSTR ansiconvert_ThreadCP(LPCTSTR str);

#endif // _ANSI_CONVERT_H_

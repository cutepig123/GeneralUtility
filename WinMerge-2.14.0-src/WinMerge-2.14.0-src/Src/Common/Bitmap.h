/** 
 * @file  Bitmap.h
 *
 * @brief Declaration file for Bitmap helper functions.
 *
 */
// ID line follows -- this is updated by SVN
// $Id: Bitmap.h 5544 2008-06-30 01:16:38Z marcelgosselin $

#ifndef _BITMAP_H_
#define _BITMAP_H_

class CBitmap;
class CDC;
class CRect;

CBitmap *CopyRectToBitmap(CDC *pDC, const CRect & rect);
void DrawBitmap(CDC *pDC, int x, int y, CBitmap *pBitmap);
CBitmap *GetDarkenedBitmap(CDC *pDC, CBitmap *pBitmap);

#endif // _BITMAP_H_

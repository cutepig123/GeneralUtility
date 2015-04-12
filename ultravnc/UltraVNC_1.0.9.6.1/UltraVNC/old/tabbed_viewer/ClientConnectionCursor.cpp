//  Copyright (C) 2000 Const Kaplinsky. All Rights Reserved.
//
//  This file is part of the VNC system.
//
//  The VNC system is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
//  USA.
//
// TightVNC distribution homepage on the Web: http://www.tightvnc.com/
//
// If the source code for the VNC system is not available from the place 
// whence you received this file, check http://www.uk.research.att.com/vnc or contact
// the authors on vnc@uk.research.att.com for information on obtaining it.

// XCursor and RichCursor encodings
//
// Support for cursor shape updates for ClientConnection class.

#include "stdhdrs.h"
#include "vncviewer.h"
#include "ClientConnection.h"

void ClientConnection::ReadCursorShape(rfbFramebufferUpdateRectHeader *pfburh) {

	vnclog.Print(6, _T("Receiving cursor shape update, cursor %dx%d\n"),
				 (int)pfburh->r.w, (int)pfburh->r.h);

	int bytesPerRow = (pfburh->r.w + 7) / 8;
	int bytesMaskData = bytesPerRow * pfburh->r.h;
	int bytesSourceData =
		pfburh->r.w * pfburh->r.h * (m_myFormat.bitsPerPixel / 8);
	CheckBufferSize(bytesMaskData);

	SoftCursorFree();

	if (pfburh->r.w * pfburh->r.h == 0)
		return;

	// Ignore cursor shape updates if requested by user
	if (m_opts.m_ignoreShapeUpdates) {
		int bytesToSkip = (pfburh->encoding == rfbEncodingXCursor) ?
			(6 + 2 * bytesMaskData) : (bytesSourceData + bytesMaskData);
		CheckBufferSize(bytesToSkip);
		ReadExact(m_netbuf, bytesToSkip);
		return;
	}

	// Read cursor pixel data.

	rcSource = new COLORREF[pfburh->r.w * pfburh->r.h];
	rcSourcecolor = new Byte[pfburh->r.w * pfburh->r.h*m_myFormat.bitsPerPixel / 8];

	if (pfburh->encoding == rfbEncodingXCursor) {
		CARD8 xcolors[6];
		ReadExact((char *)xcolors, 6);
		COLORREF rcolors[2];
		rcolors[1] = PALETTERGB(xcolors[0], xcolors[1], xcolors[2]);
		rcolors[0] = PALETTERGB(xcolors[3], xcolors[4], xcolors[5]);

		ReadExact(m_netbuf, bytesMaskData);
		int x, y, n, b;
		int i = 0;
		for (y = 0; y < pfburh->r.h; y++) {
			for (x = 0; x < pfburh->r.w / 8; x++) {
				b = m_netbuf[y * bytesPerRow + x];
				for (n = 7; n >= 0; n--)
				{
					rcSource[i++] = rcolors[b >> n & 1];
				}

			}
			for (n = 7; n >= 8 - pfburh->r.w % 8; n--) {
					rcSource[i++] = rcolors[m_netbuf[y * bytesPerRow + x] >> n & 1];
			}
		}
	} else {
		// rfb.EncodingRichCursor
		CheckBufferSize(bytesSourceData);
		ReadExact(m_netbuf, bytesSourceData);
		SETUP_COLOR_SHORTCUTS;
		char *p = m_netbuf;
		for (int i = 0; i < pfburh->r.w * pfburh->r.h; i++) {
			switch (m_myFormat.bitsPerPixel) {
			case 8:
				rcSource[i] = COLOR_FROM_PIXEL8_ADDRESS(p);
				p++;
				break;
			case 16:
				rcSource[i] = COLOR_FROM_PIXEL16_ADDRESS(p);
				p += 2;
				break;
			case 32:
				rcSource[i] = COLOR_FROM_PIXEL32_ADDRESS(p);
				p += 4;
				break;
			}
		}
	}

	// Read and decode mask data.

	ReadExact(m_netbuf, bytesMaskData);

	rcMask = new bool[pfburh->r.w * pfburh->r.h];

	int x, y, n, b;
	int i = 0;
	for (y = 0; y < pfburh->r.h; y++) {
		for (x = 0; x < pfburh->r.w / 8; x++) {
			b = m_netbuf[y * bytesPerRow + x];
			for (n = 7; n >= 0; n--)
				rcMask[i++] = (b >> n & 1) != 0;
		}
		for (n = 7; n >= 8 - pfburh->r.w % 8; n--) {
			rcMask[i++] = (m_netbuf[y * bytesPerRow + x] >> n & 1) != 0;
		}
	}

	// Set remaining data associated with cursor.
	SETUP_COLOR_SHORTCUTS;
	BYTE *p = rcSourcecolor;
	WORD wDummy;
		for (int ii = 0; ii < pfburh->r.w * pfburh->r.h; ii++) {
			switch (m_myFormat.bitsPerPixel) {
			case 8:
					COLRORREF_TO_DIB(GetRValue(rcSource[ii]),GetGValue(rcSource[ii]),GetBValue(rcSource[ii]),wDummy);
					memcpy(p,(BYTE *)&wDummy,1);
					p++;
				break;
			case 16:
					//wDummy=RGB32_TO_RGB16(rcSource[ii]);
					COLRORREF_TO_DIB(GetRValue(rcSource[ii]),GetGValue(rcSource[ii]),GetBValue(rcSource[ii]),wDummy);
					memcpy(p,(BYTE *)&wDummy,2);
					p +=2;
				break;
			case 32:
					wDummy=0;
//					COLRORREF_TO_DIB(GetRValue(rcSource[ii]),GetGValue(rcSource[ii]),GetBValue(rcSource[ii]),wDummy);
					p[0]=GetBValue(rcSource[ii]);
					p[1]=GetGValue(rcSource[ii]);
					p[2]=GetRValue(rcSource[ii]);
					//memcpy(p,(BYTE *)&wDummy,4);
					p += 4;
				break;
			}
		}
	int a=m_myFormat.redShift;
	rcWidth = pfburh->r.w;
	rcHeight = pfburh->r.h;
	rcHotX = (pfburh->r.x < rcWidth) ? pfburh->r.x : rcWidth - 1;
	rcHotY = (pfburh->r.y < rcHeight) ? pfburh->r.y : rcHeight - 1;

	{
		omni_mutex_lock l(m_cursorMutex);
		m_SavedAreaCursor=new BYTE[rcWidth*rcHeight*m_myFormat.bitsPerPixel / 8];
	}

	SoftCursorSaveArea();
	SoftCursorDraw();

	rcCursorHidden = false;
	rcLockSet = false;

	prevCursorSet = true;
}

//
// SoftCursorLockArea(). This method should be used to prevent
// collisions between simultaneous framebuffer update operations and
// cursor drawing operations caused by movements of pointing device.
// The parameters denote a rectangle where mouse cursor should not
// be drawn. Every next call to this function expands locked area so
// previous locks remain active.
//

void ClientConnection::SoftCursorLockArea(int x, int y, int w, int h) {

	omni_mutex_lock l(m_cursorMutex);

	if (!prevCursorSet)
		return;

	if (!rcLockSet) {
		rcLockX = x;
		rcLockY = y;
		rcLockWidth = w;
		rcLockHeight = h;
		rcLockSet = true;
	} else {
		int newX = (x < rcLockX) ? x : rcLockX;
		int newY = (y < rcLockY) ? y : rcLockY;
		rcLockWidth = (x + w > rcLockX + rcLockWidth) ?
			(x + w - newX) : (rcLockX + rcLockWidth - newX);
		rcLockHeight = (y + h > rcLockY + rcLockHeight) ?
			(y + h - newY) : (rcLockY + rcLockHeight - newY);
		rcLockX = newX;
		rcLockY = newY;
	}

	if (!rcCursorHidden && SoftCursorInLockedArea() && SaveAreaSet) {
		SoftCursorRestoreArea();
		rcCursorHidden = true;
	}
}

//
// SoftCursorUnlockScreen(). This function discards all locks
// performed since previous SoftCursorUnlockScreen() call.
//

void ClientConnection::SoftCursorUnlockScreen() {

	omni_mutex_lock l(m_cursorMutex);

	if (!prevCursorSet)
		return;

	if (rcCursorHidden) {
		SoftCursorSaveArea();
		SoftCursorDraw();
		rcCursorHidden = false;
	}
	rcLockSet = false;
}

//
// SoftCursorMove(). Moves soft cursor in particular location. This
// function respects locking of screen areas so when the cursor is
// moved in the locked area, it becomes invisible until
// SoftCursorUnlockScreen() method is called.
//

void ClientConnection::SoftCursorMove(int x, int y) {
	omni_mutex_lock l(m_cursorMutex);

	if (prevCursorSet && !rcCursorHidden && SaveAreaSet) {
		SoftCursorRestoreArea();
		rcCursorHidden = true;
	}

	rcCursorX = x;
	rcCursorY = y;

	if ( prevCursorSet && !(rcLockSet && SoftCursorInLockedArea())) {
		SoftCursorSaveArea();
		SoftCursorDraw();
		rcCursorHidden = false;
	}
}

 //
 // Free all data associated with cursor.
 //

void ClientConnection::SoftCursorFree() {

	omni_mutex_lock l(m_cursorMutex);

	if (prevCursorSet) {
		if (!rcCursorHidden && SaveAreaSet)
			SoftCursorRestoreArea();
		if (m_SavedAreaCursor!=NULL)delete[] m_SavedAreaCursor;
		m_SavedAreaCursor=NULL;
		if (rcSource!=NULL)delete[] rcSource;
		rcSource=NULL;
		if (rcSourcecolor!=NULL)delete[] rcSourcecolor;
		rcSourcecolor=NULL;
		if (rcMask!=NULL)delete[] rcMask;
		rcMask=NULL;
		prevCursorSet = false;
		SaveAreaSet = false;
	}
}

//////////////////////////////////////////////////////////////////
//
// Low-level methods implementing software cursor functionality.
//

//
// Check if cursor is within locked part of screen.
//

bool ClientConnection::SoftCursorInLockedArea() {

    return (rcLockX < rcCursorX - rcHotX + rcWidth &&
			rcLockY < rcCursorY - rcHotY + rcHeight &&
			rcLockX + rcLockWidth > rcCursorX - rcHotX &&
			rcLockY + rcLockHeight > rcCursorY - rcHotY);
}

//
// Save screen data in memory buffer.
//

void ClientConnection::SoftCursorSaveArea() {
	omni_mutex_lock l(m_bitmapdcMutex);
	RECT r;
	SoftCursorToScreen(&r, NULL);
	int x = r.left;
	int y = r.top;
	int w = r.right - r.left;
	int h = r.bottom - r.top;
	ConvertAll_dest0_source(w, h, x, y,m_myFormat.bitsPerPixel / 8,m_SavedAreaCursor,(BYTE *)m_DIBbits,m_si.framebufferWidth);
	SaveAreaSet=true;
}

//
// Restore screen data saved in memory buffer.
//

void ClientConnection::SoftCursorRestoreArea() {
	omni_mutex_lock l(m_bitmapdcMutex);
	RECT r;
	SoftCursorToScreen(&r, NULL);
	int x = r.left;
	int y = r.top;
	int w = r.right - r.left;
	int h = r.bottom - r.top;
	ConvertAll(w, h, x, y,m_myFormat.bitsPerPixel / 8,m_SavedAreaCursor,(BYTE *)m_DIBbits,m_si.framebufferWidth);
	InvalidateScreenRect(&r);
}

//
// Draw cursor.
//

void ClientConnection::SoftCursorDraw() {

	int x, y, x0, y0;
	int offset;

	omni_mutex_lock l(m_bitmapdcMutex);
	SETUP_COLOR_SHORTCUTS;

	for (y = 0; y < rcHeight; y++) {
		y0 = rcCursorY - rcHotY + y;
		if (y0 >= 0 && y0 < m_si.framebufferHeight) {
			for (x = 0; x < rcWidth; x++) {
				x0 = rcCursorX - rcHotX + x;
				if (x0 >= 0 && x0 < m_si.framebufferWidth) {
					offset = y * rcWidth + x;
					if (rcMask[offset]) {
						ConvertAll(1, 1, x0, y0,m_myFormat.bitsPerPixel / 8,&rcSourcecolor[offset*m_myFormat.bitsPerPixel / 8],(BYTE *)m_DIBbits,m_si.framebufferWidth);
					}
				}
			}
		}
	}

	RECT r;
	SoftCursorToScreen(&r, NULL);
	InvalidateScreenRect(&r);
}

//
// Calculate position, size and offset for the part of cursor
// located inside framebuffer bounds.
//

void ClientConnection::SoftCursorToScreen(RECT *screenArea, POINT *cursorOffset) {

	int cx = 0, cy = 0;

	int x = rcCursorX - rcHotX;
	int y = rcCursorY - rcHotY;
	int w = rcWidth;
	int h = rcHeight;

	if (x < 0) {
		cx = -x;
		w -= cx;
		x = 0;
	} else if (x + w > m_si.framebufferWidth) {
		w = m_si.framebufferWidth - x;
	}
	if (y < 0) {
		cy = -y;
		h -= cy;
		y = 0;
	} else if (y + h > m_si.framebufferHeight) {
		h = m_si.framebufferHeight - y;
	}

	if (w < 0) {
		cx = 0; x = 0; w = 0;
	}
	if (h < 0) {
		cy = 0; y = 0; h = 0;
	}

	if (screenArea != NULL) {
		SetRect(screenArea, x, y, x + w, y + h);
	}
	if (cursorOffset != NULL) {
		cursorOffset->x = cx;
		cursorOffset->y = cy;
	}
}

void ClientConnection::InvalidateScreenRect(const RECT *pRect) {
	RECT rect;

	// If we're scaling, we transform the coordinates of the rectangle
	// received into the corresponding window coords, and invalidate
	// *that* region.

	if (m_opts.m_scaling) {
		// First, we adjust coords to avoid rounding down when scaling.
		int n = m_opts.m_scale_num;
		int v = m_opts.m_scale_num_v;
		int d = m_opts.m_scale_den;
		int left   = (pRect->left / d) * d;
		int top    = (pRect->top  / d) * d;
		int right  = (pRect->right  + d - 1) / d * d; // round up
		int bottom = (pRect->bottom + d - 1) / d * d; // round up

		// Then we scale the rectangle, which should now give whole numbers.
		rect.left   = (left   * v / d) - m_hScrollPos;
		rect.top    = (top    * n / d) - m_vScrollPos;
		rect.right  = (right  * v / d) - m_hScrollPos;
		rect.bottom = (bottom * n / d) - m_vScrollPos;
	} else {
		rect.left   = pRect->left   - m_hScrollPos;
		rect.top    = pRect->top    - m_vScrollPos;
		rect.right  = pRect->right  - m_hScrollPos;
		rect.bottom = pRect->bottom - m_vScrollPos;
	}
	InvalidateRect(m_hwnd, &rect, FALSE);
}

void ClientConnection::InvalidateRegion(const RECT *pRect,HRGN *prgn) {
	RECT rect;

	// If we're scaling, we transform the coordinates of the rectangle
	// received into the corresponding window coords, and invalidate
	// *that* region.

	if (m_opts.m_scaling) {
		// First, we adjust coords to avoid rounding down when scaling.
		int n = m_opts.m_scale_num;
		int v = m_opts.m_scale_num_v;
		int d = m_opts.m_scale_den;
		int left   = (pRect->left / d) * d;
		int top    = (pRect->top  / d) * d;
		int right  = (pRect->right  + d - 1) / d * d; // round up
		int bottom = (pRect->bottom + d - 1) / d * d; // round up

		// Then we scale the rectangle, which should now give whole numbers.
		rect.left   = (left   * v / d) - m_hScrollPos;
		rect.top    = (top    * n / d) - m_vScrollPos;
		rect.right  = (right  * v / d) - m_hScrollPos;
		rect.bottom = (bottom * n / d) - m_vScrollPos;
	} else {
		rect.left   = pRect->left   - m_hScrollPos;
		rect.top    = pRect->top    - m_vScrollPos;
		rect.right  = pRect->right  - m_hScrollPos;
		rect.bottom = pRect->bottom - m_vScrollPos;
	}
	HRGN tempregion = CreateRectRgnIndirect(&rect);
	CombineRgn(*prgn,*prgn,tempregion,RGN_OR);
	DeleteObject(tempregion);
}
/**
 *  @file SortHeaderCtrl.cpp
 *
 *  @brief Implementation of CSortHeaderCtrl
 */ 
// RCS ID line follows -- this is updated by CVS
// $Id: SortHeaderCtrl.cpp 7335 2010-10-21 21:19:24Z gerundt $
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "SortHeaderCtrl.h"

#ifndef HDF_SORTUP
#define HDF_SORTUP              0x0400
#define HDF_SORTDOWN            0x0200
#endif

/////////////////////////////////////////////////////////////////////////////
// CSortHeaderCtrl

IMPLEMENT_DYNAMIC(CSortHeaderCtrl, CHeaderCtrl)

BEGIN_MESSAGE_MAP(CSortHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CSortHeaderCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSortHeaderCtrl::CSortHeaderCtrl()
{
	m_bSortAsc	= TRUE;
	m_nSortCol	= -1;
}

CSortHeaderCtrl::~CSortHeaderCtrl()
{
}

int CSortHeaderCtrl::SetSortImage(int nCol, BOOL bAsc)
{
	int nPrevCol = m_nSortCol;

	m_nSortCol = nCol;
	m_bSortAsc = bAsc;

	HD_ITEM hditem;

	hditem.mask = HDI_FORMAT;

	// Clear HDF_SORTDOWN and HDF_SORTUP flag in all columns.
	int i;
	for (i = 0; i < this->GetItemCount(); i++)
	{
		GetItem( i, &hditem );
		if (hditem.fmt & (HDF_SORTDOWN | HDF_SORTUP))
		{
			hditem.fmt &= ~(HDF_SORTUP | HDF_SORTDOWN);
			SetItem( i, &hditem );
		}
	}

	GetItem( nCol, &hditem );
	if (SendMessage(CCM_GETVERSION) >= 6)
	{
		// We can use HDF_SORTUP and HDF_SORTDOWN flag to draw a sort arrow on the header.
		hditem.fmt &= ~(HDF_SORTUP | HDF_SORTDOWN);
		hditem.fmt |= bAsc ? HDF_SORTUP : HDF_SORTDOWN;
	}
	else
	{
		// Change the item to owner drawn
		hditem.fmt |= HDF_OWNERDRAW;
	}
	SetItem( nCol, &hditem );

	// Invalidate header control so that it gets redrawn
	Invalidate();

	return nPrevCol;
}

void CSortHeaderCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;

	dc.Attach( lpDrawItemStruct->hDC );

	// Get the column rect
	CRect rcLabel( lpDrawItemStruct->rcItem );

	// Save DC
	int nSavedDC = dc.SaveDC();

	// Set clipping region to limit drawing within column
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcLabel );
	dc.SelectObject( &rgn );
	rgn.DeleteObject();

	// Draw the background
	CBrush brush(::GetSysColor(COLOR_3DFACE));
	dc.FillRect(rcLabel, &brush);

	// Labels are offset by a certain amount	
	// This offset is related to the width of a space character
	int offset = dc.GetTextExtent(_T(" "), 1 ).cx*2;

	// Get the column text and format
	TCHAR buf[256];
	HD_ITEM hditem;

	hditem.mask = HDI_TEXT | HDI_FORMAT;
	hditem.pszText = buf;
	hditem.cchTextMax = 255;

	GetItem( lpDrawItemStruct->itemID, &hditem );

	// Determine format for drawing column label
	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP 
						| DT_VCENTER | DT_END_ELLIPSIS ;

	if( hditem.fmt & HDF_CENTER)
		uFormat |= DT_CENTER;
	else if( hditem.fmt & HDF_RIGHT)
		uFormat |= DT_RIGHT;
	else
		uFormat |= DT_LEFT;

	// Adjust the rect if the mouse button is pressed on it
	if( lpDrawItemStruct->itemState == ODS_SELECTED )
	{
		rcLabel.left++;
		rcLabel.top += 2;
		rcLabel.right++;
	}

	// Adjust the rect further if Sort arrow is to be displayed
	if( lpDrawItemStruct->itemID == (UINT)m_nSortCol )
	{
		rcLabel.right -= 3 * offset;
	}

	rcLabel.left += offset;
	rcLabel.right -= offset;

	// Draw column label
	if( rcLabel.left < rcLabel.right )
		dc.DrawText(buf,-1,rcLabel, uFormat);

	// Draw the Sort arrow
	if( lpDrawItemStruct->itemID == (UINT)m_nSortCol )
	{
		CRect rcIcon( lpDrawItemStruct->rcItem );

		// Set up pens to use for drawing the triangle
		CPen penLight(PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT));
		CPen penShadow(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		CPen *pOldPen = dc.SelectObject( &penLight );

		offset = (rcIcon.bottom - rcIcon.top) / 4;
		if (m_bSortAsc)
		{
			// Draw triangle pointing upwards
			dc.MoveTo( rcIcon.right - 2* offset, offset);
			dc.LineTo( rcIcon.right - offset, rcIcon.bottom - offset-1 );
			dc.LineTo( rcIcon.right - 3*offset-2, rcIcon.bottom - offset-1 );
			dc.MoveTo( rcIcon.right - 3*offset-1, rcIcon.bottom - offset-1 );
			dc.SelectObject( &penShadow );
			dc.LineTo( rcIcon.right - 2*offset, offset-1);
		}
		else
		{
			// Draw triangle pointing downwards
			dc.MoveTo( rcIcon.right - offset-1, offset);
			dc.LineTo( rcIcon.right - 2*offset-1, rcIcon.bottom - offset );
			dc.MoveTo( rcIcon.right - 2*offset-2, rcIcon.bottom - offset );
			dc.SelectObject( &penShadow );
			dc.LineTo( rcIcon.right - 3*offset-1, offset );
			dc.LineTo( rcIcon.right - offset-1, offset);
		}

		// Restore the pen
		dc.SelectObject( pOldPen );
	}

	// Restore dc
	dc.RestoreDC( nSavedDC );

	// Detach the dc before returning
	dc.Detach();
}


// MyClass.cpp : implementation file
//

#include "stdafx.h"
#include "OwnerDrawButton.h"
#include "MyClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyClass

CMyClass::CMyClass()
{
}

CMyClass::~CMyClass()
{
}


BEGIN_MESSAGE_MAP(CMyClass, CButton)
	//{{AFX_MSG_MAP(CMyClass)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyClass message handlers

void CMyClass::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);		//Get device context object
	CRect rt;
	rt = lpDrawItemStruct->rcItem;		//Get button rect

	dc.FillSolidRect(rt, RGB(0, 0, 255));		//Fill button with blue color

	UINT state = lpDrawItemStruct->itemState;	//Get state of the button
	if ( (state & ODS_SELECTED) )		// If it is pressed
	{
		dc.DrawEdge(rt,EDGE_SUNKEN,BF_RECT);    // Draw a sunken face
	}
	else
	{
		dc.DrawEdge(rt,EDGE_RAISED,BF_RECT);	// Draw a raised face
	}
	dc.SetTextColor(RGB(255,255,120));		// Set the color of the caption to be yellow
	CString strTemp;
	GetWindowText(strTemp);		// Get the caption which have been set
	dc.DrawText(strTemp,rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);		// Draw out the caption
	if ( (state & ODS_FOCUS ) )       // If the button is focused
	{
		// Draw a focus rect which indicates the user 
		// that the button is focused
		int iChange = 3;
		rt.top += iChange;
		rt.left += iChange;
		rt.right -= iChange;
		rt.bottom -= iChange;
		dc.DrawFocusRect(rt);
	}
	dc.Detach();
}

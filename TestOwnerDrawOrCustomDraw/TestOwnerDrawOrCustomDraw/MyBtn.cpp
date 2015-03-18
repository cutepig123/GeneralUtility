// MyBtn.cpp : implementation file
//

#include "stdafx.h"
#include "TestOwnerDrawOrCustomDraw.h"
#include "MyBtn.h"


// CMyBtn

IMPLEMENT_DYNAMIC(CMyBtn, CButton)

CMyBtn::CMyBtn()
{

}

CMyBtn::~CMyBtn()
{
}


BEGIN_MESSAGE_MAP(CMyBtn, CButton)
END_MESSAGE_MAP()



// CMyBtn message handlers




void CMyBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	 CDC dc;
	dc.Attach(lpDrawItemStruct->hDC); //Get device context object
	CRect rt;
	rt = lpDrawItemStruct->rcItem; //Get button rect
 
	dc.FillSolidRect(rt, RGB(0, 0, 255)); //Fill button with blue color
 
	UINT state = lpDrawItemStruct->itemState; //Get state of the button
	if ( (state & ODS_SELECTED) ) // If it is pressed
	{
	dc.DrawEdge(rt,EDGE_SUNKEN,BF_RECT); // Draw a sunken face
	}
	else
	{
	dc.DrawEdge(rt,EDGE_RAISED,BF_RECT); // Draw a raised face
	}
 
	dc.SetTextColor(RGB(255,255,120));
	// Set the color of the caption to be yellow
	CString strTemp;
	GetWindowText(strTemp);
	// Get the caption which have been set
	dc.DrawText(strTemp,rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	// Draw out the caption
	if ( (state & ODS_FOCUS ) ) // If the button is focused
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

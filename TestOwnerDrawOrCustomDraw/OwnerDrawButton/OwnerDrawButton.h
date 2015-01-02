// OwnerDrawButton.h : main header file for the OWNERDRAWBUTTON application
//

#if !defined(AFX_OWNERDRAWBUTTON_H__8D976B7E_947B_4D9C_BB8C_77D8342CA841__INCLUDED_)
#define AFX_OWNERDRAWBUTTON_H__8D976B7E_947B_4D9C_BB8C_77D8342CA841__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// COwnerDrawButtonApp:
// See OwnerDrawButton.cpp for the implementation of this class
//

class COwnerDrawButtonApp : public CWinApp
{
public:
	COwnerDrawButtonApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COwnerDrawButtonApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(COwnerDrawButtonApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OWNERDRAWBUTTON_H__8D976B7E_947B_4D9C_BB8C_77D8342CA841__INCLUDED_)

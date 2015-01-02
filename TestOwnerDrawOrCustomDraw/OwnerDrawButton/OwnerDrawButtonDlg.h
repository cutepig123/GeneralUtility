// OwnerDrawButtonDlg.h : header file
//

#if !defined(AFX_OWNERDRAWBUTTONDLG_H__B2091368_03DF_4F9B_A538_82B88458D80D__INCLUDED_)
#define AFX_OWNERDRAWBUTTONDLG_H__B2091368_03DF_4F9B_A538_82B88458D80D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// COwnerDrawButtonDlg dialog

#include "MyClass.h"

class COwnerDrawButtonDlg : public CDialog
{
// Construction
public:
	COwnerDrawButtonDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(COwnerDrawButtonDlg)
	enum { IDD = IDD_OWNERDRAWBUTTON_DIALOG };
	CMyClass	m_MyColoredButton;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COwnerDrawButtonDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(COwnerDrawButtonDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OWNERDRAWBUTTONDLG_H__B2091368_03DF_4F9B_A538_82B88458D80D__INCLUDED_)


// TestOwnerDrawOrCustomDrawDlg.h : header file
//

#pragma once
#include "mybtn.h"


// CTestOwnerDrawOrCustomDrawDlg dialog
class CTestOwnerDrawOrCustomDrawDlg : public CDialogEx
{
// Construction
public:
	CTestOwnerDrawOrCustomDrawDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTOWNERDRAWORCUSTOMDRAW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CMyBtn m_myBtn;
};

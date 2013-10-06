
// APIHookExeDlg.h : header file
//

#pragma once


// CAPIHookExeDlg dialog
class CAPIHookExeDlg : public CDialogEx
{
// Construction
public:
	CAPIHookExeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_APIHOOKEXE_DIALOG };

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
	afx_msg void OnBnClickedButton1Hook();
	afx_msg void OnBnClickedButton1Unhook2();
};

/** 
 * @file  TestFilterDlg.h
 *
 * @brief Declaration file for CTestFilterDlg class
 *
 */
// ID line follows -- this is updated by SVN
// $Id: TestFilterDlg.h 4634 2007-10-16 10:56:17Z kimmov $

#if !defined(AFX_TESTFILTERDLG_H__1261AD7A_512F_47BC_882F_2E2D41C5072F__INCLUDED_)
#define AFX_TESTFILTERDLG_H__1261AD7A_512F_47BC_882F_2E2D41C5072F__INCLUDED_
#pragma once

struct FileFilter;
class FileFilterMgr;

/**
 * @brief Dialog allowing user to test out file filter strings.
 * The user can type a text and test if selected file filter matches to it.
 */
class CTestFilterDlg : public CDialog
{
// Construction
public:
	CTestFilterDlg(CWnd* pParent, FileFilter * pFileFilter, FileFilterMgr *pFilterMgr);

// Implementation data
private:
	FileFilter * m_pFileFilter; /**< Selected file filter. */
	FileFilterMgr * m_pFileFilterMgr; /**< File filter manager. */

// Implementation methods
private:
	BOOL CheckText(CString text) const;
	void AppendResult(CString result);

// Dialog Data
	//{{AFX_DATA(CTestFilterDlg)
	enum { IDD = IDD_TEST_FILTER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


	// Generated message map functions
	//{{AFX_MSG(CTestFilterDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnTestBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTFILTERDLG_H__1261AD7A_512F_47BC_882F_2E2D41C5072F__INCLUDED_)

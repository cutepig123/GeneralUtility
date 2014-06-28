/** 
 * @file  DirCompProgressDlg.h
 *
 * @brief Declaration file for Directory compare statusdialog class
 */
// ID line follows -- this is updated by SVN
// $Id: DirCompProgressDlg.h 5444 2008-06-07 06:48:49Z kimmov $

#if !defined(AFX_DIRCOMPPROGRESSDLG_H__8F66C090_C232_429F_A4A2_18D43CCC6C38__INCLUDED_)
#define AFX_DIRCOMPPROGRESSDLG_H__8F66C090_C232_429F_A4A2_18D43CCC6C38__INCLUDED_

#include "CompareStats.h"

class CDirFrame;
class CDirDoc;

/////////////////////////////////////////////////////////////////////////////
// DirCompProgressDlg dialog

/**
 * @brief Class for directory compare statusdialog.
 * 
 * Implements non-modal dialog directory compares. We create this
 * modeless dialog when we start the compare and destroy it after
 * compare is ready. Dialog itself shows counts of total found items
 * and items compared so far. And nice progressbar for user to have some
 * idea how compare is going.
 *
 * Status updates are fired by periodic timer events. We have shared
 * datastructure with compare code. Compare code updates status information
 * to datastructure during compare. When timer event fires, dialog reads
 * that datastructure and updates the GUI.
 * 
 * @todo Now we update total count of items with same timer than we update
 * compared items count. Maybe we should use different timer and bigger
 * interval for total count updates?
 */
class DirCompProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(DirCompProgressDlg)

// Construction
public:
	DirCompProgressDlg(CWnd* pParent = NULL);   // standard constructor
	void SetCompareStat(CompareStats * pCompareStats);
	void StartUpdating();
	void EndUpdating();
	void CloseDialog();
	void SetDirDoc(CDirDoc *pDirDoc);

// Dialog Data
	//{{AFX_DATA(DirCompProgressDlg)
	enum { IDD = IDD_DIRCOMP_PROGRESS };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DirCompProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void ClearStat();

	// Generated message map functions
	//{{AFX_MSG(DirCompProgressDlg)
	afx_msg BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedComparisonStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CompareStats *m_pCompareStats; /**< Pointer to comparestats */
	CompareStats::CMP_STATE m_prevState; /**< Previous state for compare (to track changes) */
	BOOL m_bCompareReady; /**< Compare ready, waiting for closing? */
	CDirDoc * m_pDirDoc; /**< Pointer to dirdoc we are comparing */
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRCOMPPROGRESSDLG_H__8F66C090_C232_429F_A4A2_18D43CCC6C38__INCLUDED_)


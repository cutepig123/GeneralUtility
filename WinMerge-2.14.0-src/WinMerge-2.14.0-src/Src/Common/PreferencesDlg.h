/** 
 * @file  PreferencesDlg.h
 *
 * @brief Declaration of CPreferencesDlg class
 *
 * @note This code originates from AbstractSpoon / TodoList
 * (http://www.abstractspoon.com/) but is modified to use in
 * WinMerge.
 */
// ID line follows -- this is updated by SVN
// $Id: PreferencesDlg.h 7523 2011-05-22 14:40:23Z sdottaka $


#if !defined(AFX_PREFERENCESDLG_H__C3FCC72A_6C69_49A6_930D_D5C94EC31298__INCLUDED_)
#define AFX_PREFERENCESDLG_H__C3FCC72A_6C69_49A6_930D_D5C94EC31298__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PropGeneral.h"
#include "PropCompare.h"
#include "PropEditor.h"
#include "PropVss.h"
#include "PropRegistry.h"
#include "PropColors.h"
#include "PropTextColors.h"
#include "PropSyntaxColors.h"
#include "PropCodepage.h"
#include "PropArchive.h"
#include "PropBackups.h"
#include "PropShell.h"
#include "PropCompareFolder.h"

#include "PropertyPageHost.h"

class COptionsMgr;
class SyntaxColors;

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlg dialog

class CPreferencesDlg : public CDialog
{
// Construction
public:
	//CPreferencesDlg(UINT nMenuID = 0, CWnd* pParent = NULL);   // standard constructor
	CPreferencesDlg(COptionsMgr *optionsMgr, SyntaxColors *colors,
		UINT nMenuID = 0, CWnd* pParent = NULL);   // standard constructor
	virtual ~CPreferencesDlg();

	void SetSyntaxColors(SyntaxColors *pColors);

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesDlg)
	CTreeCtrl m_tcPages;
	//}}AFX_DATA

	CPropertyPageHost m_pphost;
	
	PropGeneral m_pageGeneral;
	PropCompare m_pageCompare;
	PropEditor m_pageEditor;
	PropVss m_pageVss;	
	PropRegistry m_pageSystem;
	PropCodepage m_pageCodepage;
	PropMergeColors m_pageMergeColors;
	PropTextColors m_pageTextColors;
	PropSyntaxColors m_pageSyntaxColors;
	PropArchive m_pageArchive;
	PropBackups m_pageBackups;
	PropShell m_pageShell;
	PropCompareFolder m_pageCompareFolder;

	SyntaxColors *m_pSyntaxColors;

	CMapPtrToPtr m_mapPP2HTI;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesDlg)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnHelpButton();
	afx_msg void OnImportButton();
	afx_msg void OnExportButton();
	afx_msg void OnSelchangedPages(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void AddPage(CPropertyPage* pPage, UINT nResourceID);
	void AddPage(CPropertyPage* pPage, LPCTSTR szPath);
	void AddPage(CPropertyPage* pPage, UINT nTopHeading, UINT nSubHeading);
	void SetActivePage(int nPage);
	CString GetItemPath(HTREEITEM hti);
	void ReadOptions(BOOL bUpdate = FALSE);
	void SaveOptions();
	void SafeUpdatePage(CPropertyPage* pPage, BOOL bSaveAndValidate);

private:
	COptionsMgr *m_pOptionsMgr;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESDLG_H__C3FCC72A_6C69_49A6_930D_D5C94EC31298__INCLUDED_)

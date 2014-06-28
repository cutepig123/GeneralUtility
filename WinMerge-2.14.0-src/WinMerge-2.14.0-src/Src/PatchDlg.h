/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  PatchDlg.h
 *
 * @brief Declaration file for patch creation dialog
 */
// ID line follows -- this is updated by SVN
// $Id: PatchDlg.h 7550 2011-06-26 13:52:49Z sdottaka $

#if !defined(AFX_PATCHDLG_H__AB3CE671_1328_11D7_B088_005004D9D386__INCLUDED_)
#define AFX_PATCHDLG_H__AB3CE671_1328_11D7_B088_005004D9D386__INCLUDED_

#include "resource.h"
#include "SuperComboBox.h"

struct PATCHFILES;

/////////////////////////////////////////////////////////////////////////////
// PatchDlg dialog

/** 
 * @brief Dialog class for Generate Patch -dialog.
 * This dialog allows user to select files from which to create a patch,
 * patch file's filename and several options related to patch.
 */
class CPatchDlg : public CDialog
{
// Construction
public:
	CPatchDlg(CWnd* pParent = NULL);   // standard constructor

	// Functions to add and get selected files (as PATCHFILEs)
	void AddItem(const PATCHFILES& pf);
	int GetItemCount();
	const PATCHFILES& GetItemAt(int position);
	void ClearItems();

// Dialog Data
	//{{AFX_DATA(CPatchDlg)
	enum { IDD = IDD_GENERATE_PATCH };
	CComboBox m_comboStyle;
	CSuperComboBox m_comboContext;
	BOOL m_caseSensitive;
	CSuperComboBox m_ctlFile1;
	CSuperComboBox m_ctlFile2;
	CSuperComboBox m_ctlResult;
	CString	m_file1;
	CString	m_file2;
	CString	m_fileResult;
	BOOL m_ignoreBlanks;
	int m_whitespaceCompare;
	BOOL m_appendFile;
	BOOL m_openToEditor;
	BOOL m_includeCmdLine;
	//}}AFX_DATA

	enum output_style m_outputStyle; /**< Patch style (context, unified etc.) */
	int m_contextLines; /**< How many context lines are added. */

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	std::vector<PATCHFILES> m_fileList; /**< Source files to create patch from */

	void ChangeFile(const CString &sFile, BOOL bLeft);
	void UpdateSettings();
	void LoadSettings();
	void SaveSettings();
	void Swap();

	// Generated message map functions
	//{{AFX_MSG(CPatchDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDiffBrowseFile1();
	afx_msg void OnDiffBrowseFile2();
	afx_msg void OnDiffBrowseResult();
	afx_msg void OnSelchangeFile1Combo();
	afx_msg void OnSelchangeFile2Combo();
	afx_msg void OnSelchangeResultCombo();
	afx_msg void OnSelchangeDiffStyle();
	afx_msg void OnDiffSwapFiles();
	afx_msg void OnDefaultSettings();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATCHDLG_H__AB3CE671_1328_11D7_B088_005004D9D386__INCLUDED_)

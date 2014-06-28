/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//    
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  PatchDlg.cpp
 *
 * @brief Implementation of Patch creation dialog
 */
// ID line follows -- this is updated by SVN
// $Id: PatchDlg.cpp 7550 2011-06-26 13:52:49Z sdottaka $

#include "StdAfx.h"
#include "Merge.h"
#include "PatchTool.h"
#include "PatchDlg.h"
#include "DIFF.H"
#include "coretools.h"
#include "paths.h"
#include "CompareOptions.h"
#include "FileOrFolderSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPatchDlg dialog

/** 
 * @brief Constructor, initializes members.
 */
CPatchDlg::CPatchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatchDlg::IDD, pParent)
	, m_caseSensitive(FALSE)
	, m_ignoreBlanks(0)
	, m_whitespaceCompare(0)
	, m_appendFile(FALSE)
	, m_openToEditor(FALSE)
	, m_includeCmdLine(FALSE)
	, m_outputStyle(OUTPUT_NORMAL)
	, m_contextLines(0)
{
}

/**
 * @brief Map dialog controls and class member variables.
 */
void CPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatchDlg)
	DDX_Control(pDX, IDC_DIFF_STYLE, m_comboStyle);
	DDX_Control(pDX, IDC_DIFF_CONTEXT, m_comboContext);
	DDX_Check(pDX, IDC_DIFF_CASESENSITIVE, m_caseSensitive);
	DDX_Check(pDX, IDC_DIFF_WHITESPACE_IGNOREBLANKS, m_ignoreBlanks);
	DDX_Radio(pDX, IDC_DIFF_WHITESPACE_COMPARE, m_whitespaceCompare);
	DDX_Check(pDX, IDC_DIFF_APPENDFILE, m_appendFile);
	DDX_Control(pDX, IDC_DIFF_FILE1, m_ctlFile1);
	DDX_Control(pDX, IDC_DIFF_FILE2, m_ctlFile2);
	DDX_Control(pDX, IDC_DIFF_FILERESULT, m_ctlResult);
	DDX_CBStringExact(pDX, IDC_DIFF_FILE1, m_file1);
	DDX_CBStringExact(pDX, IDC_DIFF_FILE2, m_file2);
	DDX_CBStringExact(pDX, IDC_DIFF_FILERESULT, m_fileResult);
	DDX_Check(pDX, IDC_DIFF_OPENTOEDITOR, m_openToEditor);
	DDX_Check(pDX, IDC_DIFF_INCLCMDLINE, m_includeCmdLine);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPatchDlg, CDialog)
	//{{AFX_MSG_MAP(CPatchDlg)
	ON_BN_CLICKED(IDC_DIFF_BROWSE_FILE1, OnDiffBrowseFile1)
	ON_BN_CLICKED(IDC_DIFF_BROWSE_FILE2, OnDiffBrowseFile2)
	ON_BN_CLICKED(IDC_DIFF_BROWSE_RESULT, OnDiffBrowseResult)
	ON_BN_CLICKED(IDC_DIFF_DEFAULTS, OnDefaultSettings)
	ON_CBN_SELCHANGE(IDC_DIFF_FILE1, OnSelchangeFile1Combo)
	ON_CBN_SELCHANGE(IDC_DIFF_FILE2, OnSelchangeFile2Combo)
	ON_CBN_SELCHANGE(IDC_DIFF_FILERESULT, OnSelchangeResultCombo)
	ON_CBN_SELCHANGE(IDC_DIFF_STYLE, OnSelchangeDiffStyle)
	ON_BN_CLICKED(IDC_DIFF_SWAPFILES, OnDiffSwapFiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatchDlg message handlers

/** 
 * @brief Called when dialog is closed with OK.
 * Check options and filenames given and close the dialog.
 */
void CPatchDlg::OnOK()
{
	UpdateData(TRUE);

	// There are two different cases: single files or
	// multiple files.  Multiple files are selected from DirView.
	// Only if single files selected, filenames are checked here.
	// Filenames read from Dirview must be valid ones.
	int selectCount = m_fileList.size();
	if (selectCount == 1)
	{
		BOOL file1Ok = (paths_DoesPathExist(m_file1) == IS_EXISTING_FILE);
		BOOL file2Ok = (paths_DoesPathExist(m_file2) == IS_EXISTING_FILE);

		if (!file1Ok || !file2Ok)
		{
			if (!file1Ok)
				LangMessageBox(IDS_DIFF_ITEM1NOTFOUND, MB_ICONSTOP);

			if (!file2Ok)
				LangMessageBox(IDS_DIFF_ITEM2NOTFOUND, MB_ICONSTOP);
			return;
		}
	}

	// Check that a result file was specified
	if (m_fileResult.IsEmpty())
	{
		LangMessageBox(IDS_MUST_SPECIFY_OUTPUT, MB_ICONSTOP);
		m_ctlResult.SetFocus();
		return;
	}
 
	// Check that result (patch) file is absolute path
	if (!paths_IsPathAbsolute((LPCTSTR)m_fileResult))
	{
		ResMsgBox1(IDS_PATH_NOT_ABSOLUTE, m_fileResult, MB_ICONSTOP);
		m_ctlResult.SetFocus();
		return;
	}
	
	BOOL fileExists = (paths_DoesPathExist(m_fileResult) == IS_EXISTING_FILE);

	// Result file already exists and append not selected
	if (fileExists && !m_appendFile)
	{
		if (LangMessageBox(IDS_DIFF_FILEOVERWRITE,
				MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN,
				IDS_DIFF_FILEOVERWRITE) != IDYES)
		{
			return;
		}
	}
	// else it's OK to write new file

	m_outputStyle = (enum output_style) m_comboStyle.GetCurSel();

	int contextSel = m_comboContext.GetCurSel();
	if (contextSel != CB_ERR)
	{
		TCHAR contextText[50] = _T("");
		m_comboContext.GetLBText(contextSel, contextText);
		m_contextLines = _ttoi(contextText);
	}
	else
		m_contextLines = 0;

	SaveSettings();

	// Save combobox history
	m_ctlResult.SaveState(_T("Files\\DiffFileResult"));
	m_comboContext.SaveState(_T("PatchCreator\\DiffContext"));
	// Don't save filenames if multiple file selected (as editbox reads
	// [X files selected])
	if (selectCount <= 1)
	{
		m_ctlFile1.SaveState(_T("Files\\DiffFile1"));
		m_ctlFile2.SaveState(_T("Files\\DiffFile2"));
	}
	
	CDialog::OnOK();
}

/** 
 * @brief Initialise dialog data.
 *
 * There are two cases for filename editboxes:
 * - if one file was added to list then we show that filename
 * - if multiple files were added we show text [X files selected]
 */
BOOL CPatchDlg::OnInitDialog()
{
	theApp.TranslateDialog(m_hWnd);
	CDialog::OnInitDialog();

	// Load combobox history
	m_ctlFile1.LoadState(_T("Files\\DiffFile1"));
	m_ctlFile2.LoadState(_T("Files\\DiffFile2"));
	m_comboContext.LoadState(_T("PatchCreator\\DiffContext"));
	m_ctlResult.LoadState(_T("Files\\DiffFileResult"));

	int count = m_fileList.size();

	// If one file added, show filenames on dialog
	if (count == 1)
	{
        const PATCHFILES& files = m_fileList.front();
		m_file1 = files.lfile.c_str();
		m_ctlFile1.SetWindowText(files.lfile.c_str());
		m_file2 = files.rfile.c_str();
		m_ctlFile2.SetWindowText(files.rfile.c_str());
	}
	else if (count > 1)	// Multiple files added, show number of files
	{
		CString msg;
		CString num;
		num.Format(_T("%d"), count);
		LangFormatString1(msg, IDS_DIFF_SELECTEDFILES, num);
		m_file1 = msg;
		m_file2 = msg;
	}
	UpdateData(FALSE);

	// Add patch styles to combobox
	m_comboStyle.AddString(theApp.LoadString(IDS_DIFF_NORMAL).c_str());
	m_comboStyle.AddString(theApp.LoadString(IDS_DIFF_CONTEXT).c_str());
	m_comboStyle.AddString(theApp.LoadString(IDS_DIFF_UNIFIED).c_str());

	m_outputStyle = OUTPUT_NORMAL;
	m_comboStyle.SetCurSel(0);

	// Add default context line counts to combobox if its empty
	if (m_comboContext.GetCount() == 0)
	{
		m_comboContext.AddString(_T("0"));
		m_comboContext.AddString(_T("1"));
		m_comboContext.AddString(_T("3"));
		m_comboContext.AddString(_T("5"));
		m_comboContext.AddString(_T("7"));
	}
	
	LoadSettings();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/** 
 * @brief Select the left file.
 */
void CPatchDlg::OnDiffBrowseFile1()
{
	CString s;
	CString folder;

	folder = m_file1;
	if (SelectFile(GetSafeHwnd(), s, folder, IDS_OPEN_TITLE, NULL, TRUE))
	{
		ChangeFile(s, TRUE);
		m_ctlFile1.SetWindowText(s);
	}
}

/** 
 * @brief Select the right file.
 */
void CPatchDlg::OnDiffBrowseFile2()
{
	CString s;
	CString folder;

	folder = m_file2;
	if (SelectFile(GetSafeHwnd(), s, folder, IDS_OPEN_TITLE, NULL, TRUE))
	{
		ChangeFile(s, FALSE);
		m_ctlFile2.SetWindowText(s);
	}
}

/** 
 * @brief Changes original file to patch.
 * This function sets new file for left/right file to create patch from.
 * @param [in] sFile New file for patch creation.
 * @param [in] bLeft If true left file is changed, otherwise right file.
 */
void CPatchDlg::ChangeFile(const CString &sFile, BOOL bLeft)
{
	PATCHFILES pf;
	int count = GetItemCount();

	if (count == 1)
	{
		pf = GetItemAt(0);
	}
	else if (count > 1)
	{
		if (bLeft)
			m_file1.Empty();
		else
			m_file2.Empty();
	}
	ClearItems();

	// Change file
	if (bLeft)
	{
		pf.lfile = sFile;
		m_file1 = sFile;
	}
	else
	{
		pf.rfile = sFile;
		m_file2 = sFile;
	}
	AddItem(pf);
}

/** 
 * @brief Select the patch file.
 */
void CPatchDlg::OnDiffBrowseResult()
{
	CString s;
	CString folder;

	folder = m_fileResult;
	if (SelectFile(GetSafeHwnd(), s, folder, IDS_SAVE_AS_TITLE, NULL, FALSE))
	{
		m_fileResult = s;
		m_ctlResult.SetWindowText(s);
	}
}

/** 
 * @brief Called when File1 combo selection is changed.
 */
void CPatchDlg::OnSelchangeFile1Combo() 
{
	int sel = m_ctlFile1.GetCurSel();
	if (sel != CB_ERR)
	{
		CString file;
		m_ctlFile1.GetLBText(sel, file);
		m_ctlFile1.SetWindowText(file);
		ChangeFile(file, TRUE);
		m_file1 = file;
	}
}

/** 
 * @brief Called when File2 combo selection is changed.
 */
void CPatchDlg::OnSelchangeFile2Combo() 
{
	int sel = m_ctlFile2.GetCurSel();
	if (sel != CB_ERR)
	{
		CString file;
		m_ctlFile2.GetLBText(sel, file);
		m_ctlFile2.SetWindowText(file);
		ChangeFile(file, FALSE);
		m_file2 = file;
	}
}

/** 
 * @brief Called when Result combo selection is changed.
 */
void CPatchDlg::OnSelchangeResultCombo() 
{
	int sel = m_ctlResult.GetCurSel();
	if (sel != CB_ERR)
	{
		m_ctlResult.GetLBText(sel, m_fileResult);
		m_ctlResult.SetWindowText(m_fileResult);
	}
}

/** 
 * @brief Called when diff style dropdown selection is changed.
 * Called when diff style dropdown selection is changed.
 * If the new selection is context patch or unified patch format then
 * enable context lines selection control. Otherwise context lines selection
 * is disabled.
 */
void CPatchDlg::OnSelchangeDiffStyle()
{
	int selection = -1;

	selection = m_comboStyle.GetCurSel();

	// Only context and unified formats allow context lines
	if ((selection == OUTPUT_CONTEXT) ||
			(selection == OUTPUT_UNIFIED))
	{
		m_comboContext.EnableWindow(TRUE);
		// 3 lines is default context for Difftools too
		m_comboContext.SetCurSel(2);
	}
	else
	{
		m_contextLines = 0;
		m_comboContext.SetCurSel(0);
		m_comboContext.EnableWindow(FALSE);
	}
}

/** 
 * @brief Swap filenames on file1 and file2.
 */
void CPatchDlg::OnDiffSwapFiles()
{
	CString file1;
	CString file2;
	PATCHFILES files;

	m_ctlFile1.GetWindowText(file1);
	m_ctlFile2.GetWindowText(file2);

	m_ctlFile1.SetWindowText(file2);
	m_ctlFile2.SetWindowText(file1);
	m_file1 = file2;
	m_file2 = file1;

	//  swapped files
	Swap();
}

/** 
 * @brief Add patch item to internal list.
 * @param [in] pf Patch item to add.
 */
void CPatchDlg::AddItem(const PATCHFILES& pf)
{
	m_fileList.push_back(pf);
}

/** 
 * @brief Returns amount of patch items in the internal list.
 * @return Count of patch items in the list.
 */
int CPatchDlg::GetItemCount()
{
	return m_fileList.size();
}

/** 
 * @brief Return item in the internal list at given position
 * @param [in] position Zero-based index of item to get
 * @return PATCHFILES from given position.
 */
const PATCHFILES& CPatchDlg::GetItemAt(int position)
{
	return m_fileList.at(position);
}

/** 
 * @brief Empties internal item list.
 */
void CPatchDlg::ClearItems()
{
	m_fileList.clear();
}

/** 
 * @brief Updates patch dialog settings from member variables.
 */
void CPatchDlg::UpdateSettings()
{
	UpdateData(FALSE);

	switch (m_outputStyle)
	{
	case DIFF_OUTPUT_NORMAL:
		m_comboStyle.SelectString(-1, theApp.LoadString(IDS_DIFF_NORMAL).c_str());
		break;
	case DIFF_OUTPUT_CONTEXT:
		m_comboStyle.SelectString(-1, theApp.LoadString(IDS_DIFF_CONTEXT).c_str());
		break;
	case DIFF_OUTPUT_UNIFIED:
		m_comboStyle.SelectString(-1, theApp.LoadString(IDS_DIFF_UNIFIED).c_str());
		break;
	}

	CString str;
	str.Format(_T("%d"), m_contextLines);
	m_comboContext.SelectString(-1, str);

	if (m_outputStyle == OUTPUT_CONTEXT || m_outputStyle == OUTPUT_UNIFIED)
		m_comboContext.EnableWindow(TRUE);
	else
		m_comboContext.EnableWindow(FALSE);
}

/** 
 * @brief Loads patch dialog settings from registry.
 */
void CPatchDlg::LoadSettings()
{
	int patchStyle = theApp.GetProfileInt(_T("PatchCreator"), _T("PatchStyle"), 0);
	if (patchStyle < DIFF_OUTPUT_NORMAL || patchStyle > DIFF_OUTPUT_UNIFIED)
		patchStyle = DIFF_OUTPUT_NORMAL;
	m_outputStyle = (enum output_style) patchStyle;
	
	m_contextLines = theApp.GetProfileInt(_T("PatchCreator"), _T("ContextLines"), 0);
	if (m_contextLines < 0 || m_contextLines > 50)
		m_contextLines = 0;

	m_caseSensitive = theApp.GetProfileInt(_T("PatchCreator"), _T("CaseSensitive"), TRUE);
	m_ignoreBlanks = theApp.GetProfileInt(_T("PatchCreator"), _T("IgnoreBlankLines"), FALSE);
	
	m_whitespaceCompare = theApp.GetProfileInt(_T("PatchCreator"), _T("Whitespace"), WHITESPACE_COMPARE_ALL);
	if (m_whitespaceCompare < WHITESPACE_COMPARE_ALL ||
		m_whitespaceCompare > WHITESPACE_IGNORE_ALL)
	{
		m_whitespaceCompare = WHITESPACE_COMPARE_ALL;
	}
	
	m_openToEditor = theApp.GetProfileInt(_T("PatchCreator"), _T("OpenToEditor"), FALSE);
	m_includeCmdLine = theApp.GetProfileInt(_T("PatchCreator"), _T("IncludeCmdLine"), FALSE);

	UpdateSettings();
}

/** 
 * @brief Saves patch dialog settings to registry.
 */
void CPatchDlg::SaveSettings()
{
	theApp.WriteProfileInt(_T("PatchCreator"), _T("PatchStyle"), m_outputStyle);
	theApp.WriteProfileInt(_T("PatchCreator"), _T("ContextLines"), m_contextLines);
	theApp.WriteProfileInt(_T("PatchCreator"), _T("CaseSensitive"), m_caseSensitive);
	theApp.WriteProfileInt(_T("PatchCreator"), _T("IgnoreBlankLines"), m_ignoreBlanks);
	theApp.WriteProfileInt(_T("PatchCreator"), _T("Whitespace"), m_whitespaceCompare);
	theApp.WriteProfileInt(_T("PatchCreator"), _T("OpenToEditor"), m_openToEditor);
	theApp.WriteProfileInt(_T("PatchCreator"), _T("IncludeCmdLine"), m_includeCmdLine);
}

/** 
 * @brief Resets patch dialog settings to defaults.
 */
void CPatchDlg::OnDefaultSettings()
{
	m_outputStyle = (enum output_style) DIFF_OUTPUT_NORMAL;
	m_contextLines = 0;
	m_caseSensitive = TRUE;
	m_ignoreBlanks = FALSE;
	m_whitespaceCompare = WHITESPACE_COMPARE_ALL;
	m_openToEditor = FALSE;
	m_includeCmdLine = FALSE;

	UpdateSettings();
}

/**
 * @brief Swap sides.
 */
void CPatchDlg::Swap()
{
	std::vector<PATCHFILES>::iterator iter = m_fileList.begin();
	std::vector<PATCHFILES>::const_iterator iterEnd = m_fileList.end();
	while (iter != iterEnd)
	{
		(*iter).swap_sides();
		++iter;
	}
}

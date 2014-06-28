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
 * @file  PropRegistry.cpp
 *
 * @brief PropRegistry implementation file
 */
// ID line follows -- this is updated by SVN
// $Id: PropRegistry.cpp 7501 2011-01-03 13:29:00Z gerundt $

#include "stdafx.h"
#include "resource.h"
#include "PropRegistry.h"
#include "RegKey.h"
#include "coretools.h"
#include "FileOrFolderSelect.h"
#include "Merge.h" // GetDefaultEditor()
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// registry dir to WinMerge
static LPCTSTR f_RegDir = _T("Software\\Thingamahoochie\\WinMerge");

// registry values
static LPCTSTR f_RegValueEnabled = _T("ContextMenuEnabled");
static LPCTSTR f_RegValuePath = _T("Executable");

PropRegistry::PropRegistry(COptionsMgr *optionsMgr)
: OptionsPanel(optionsMgr, PropRegistry::IDD)
, m_bUseRecycleBin(TRUE)
, m_tempFolderType(0)
{
}

void PropRegistry::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropRegistry)
	DDX_Text(pDX, IDC_EXT_EDITOR_PATH, m_strEditorPath);
	DDX_Check(pDX, IDC_USE_RECYCLE_BIN, m_bUseRecycleBin);
	DDX_Text(pDX, IDC_FILTER_USER_PATH, m_strUserFilterPath);
	DDX_Radio(pDX, IDC_TMPFOLDER_SYSTEM, m_tempFolderType);
	DDX_Text(pDX, IDC_TMPFOLDER_NAME, m_tempFolder);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PropRegistry, CDialog)
	//{{AFX_MSG_MAP(PropRegistry)
	ON_BN_CLICKED(IDC_EXT_EDITOR_BROWSE, OnBrowseEditor)
	ON_BN_CLICKED(IDC_FILTER_USER_BROWSE, OnBrowseFilterPath)
	ON_BN_CLICKED(IDC_TMPFOLDER_BROWSE, OnBrowseTmpFolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropRegistry::ReadOptions()
{
	m_strEditorPath = GetOptionsMgr()->GetString(OPT_EXT_EDITOR_CMD).c_str();
	m_bUseRecycleBin = GetOptionsMgr()->GetBool(OPT_USE_RECYCLE_BIN);
	m_strUserFilterPath = GetOptionsMgr()->GetString(OPT_FILTER_USERPATH).c_str();
	m_tempFolderType = GetOptionsMgr()->GetBool(OPT_USE_SYSTEM_TEMP_PATH) ? 0 : 1;
	m_tempFolder = GetOptionsMgr()->GetString(OPT_CUSTOM_TEMP_PATH).c_str();
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropRegistry::WriteOptions()
{
	CMergeApp *app = static_cast<CMergeApp*>(AfxGetApp());
	CString sDefaultEditor = app->GetDefaultEditor();

	GetOptionsMgr()->SaveOption(OPT_USE_RECYCLE_BIN, m_bUseRecycleBin == TRUE);

	CString sExtEditor = m_strEditorPath;
	sExtEditor.TrimLeft();
	sExtEditor.TrimRight();
	if (sExtEditor.IsEmpty())
		sExtEditor = sDefaultEditor;
	GetOptionsMgr()->SaveOption(OPT_EXT_EDITOR_CMD, sExtEditor);

	CString sFilterPath = m_strUserFilterPath;
	sFilterPath.TrimLeft();
	sFilterPath.TrimRight();
	GetOptionsMgr()->SaveOption(OPT_FILTER_USERPATH, sFilterPath);

	bool useSysTemp = m_tempFolderType == 0;
	GetOptionsMgr()->SaveOption(OPT_USE_SYSTEM_TEMP_PATH, useSysTemp);

	CString tempFolder = m_tempFolder;
	tempFolder.TrimLeft();
	tempFolder.TrimRight();
	GetOptionsMgr()->SaveOption(OPT_CUSTOM_TEMP_PATH, tempFolder);
}

BOOL PropRegistry::OnInitDialog()
{
	theApp.TranslateDialog(m_hWnd);
	CPropertyPage::OnInitDialog();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/// Open file browse dialog to locate editor
void PropRegistry::OnBrowseEditor()
{
	CString path;
	if (SelectFile(GetSafeHwnd(), path, NULL, IDS_OPEN_TITLE, IDS_PROGRAMFILES, TRUE))
	{
		SetDlgItemText(IDC_EXT_EDITOR_PATH, path);
	}
}

/// Open Folder selection dialog for user to select filter folder.
void PropRegistry::OnBrowseFilterPath()
{
	CString path;
	if (SelectFolder(path, m_strUserFilterPath, IDS_OPEN_TITLE, GetSafeHwnd()))
	{
		SetDlgItemText(IDC_FILTER_USER_PATH, path);
	}
}

/// Select temporary files folder.
void PropRegistry::OnBrowseTmpFolder()
{
	CString path;
	if (SelectFolder(path, m_tempFolder, NULL, GetSafeHwnd()))
	{
		SetDlgItemText(IDC_TMPFOLDER_NAME, path);
	}
}

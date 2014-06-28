/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  PropVss.cpp
 *
 * @brief VSS properties dialog implementation.
 */
// ID line follows -- this is updated by SVN
// $Id: PropVss.cpp 7501 2011-01-03 13:29:00Z gerundt $

#include "stdafx.h"
#include "FileOrFolderSelect.h"
#include "MainFrm.h" // VCS_* constants
#include "Merge.h"
#include "PropVss.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "OptionsPanel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * @brief Constructor.
 * @param [in] optionsMgr Pointer to options manager.
 */
PropVss::PropVss(COptionsMgr *optionsMgr) 
: OptionsPanel(optionsMgr, PropVss::IDD)
, m_nVerSys(-1)
{
}

void PropVss::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PropVss)
	DDX_Control(pDX, IDC_VSS_L1, m_ctlVssL1);
	DDX_Control(pDX, IDC_PATH_EDIT, m_ctlPath);
	DDX_Control(pDX, IDC_BROWSE_BUTTON, m_ctlBrowse);
	DDX_Text(pDX, IDC_PATH_EDIT, m_strPath);
	DDX_CBIndex(pDX, IDC_VER_SYS, m_nVerSys);
	DDX_Control(pDX, IDC_VER_SYS, m_ctlVerSys);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(PropVss, CPropertyPage)
	//{{AFX_MSG_MAP(PropVss)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, OnBrowseButton)
	ON_CBN_SELENDOK(IDC_VER_SYS, OnSelendokVerSys)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/** 
 * @brief Reads options values from storage to UI.
 */
void PropVss::ReadOptions()
{
	m_nVerSys = GetOptionsMgr()->GetInt(OPT_VCS_SYSTEM);
	m_strPath = GetOptionsMgr()->GetString(OPT_VSS_PATH).c_str();
}

/** 
 * @brief Writes options values from UI to storage.
 */
void PropVss::WriteOptions()
{
	GetOptionsMgr()->SaveOption(OPT_VCS_SYSTEM, (int)m_nVerSys);
	GetOptionsMgr()->SaveOption(OPT_VSS_PATH, m_strPath);
}

/**
 * @brief Called when Browse-button is selected.
 */
void PropVss::OnBrowseButton()
{
	CString s;
	if (SelectFile(GetSafeHwnd(), s))
	{
		m_strPath = s;
		m_ctlPath.SetWindowText(s);
	}
}

/**
 * @brief Initialized the dialog.
 * @return Always TRUE.
 */
BOOL PropVss::OnInitDialog() 
{
	theApp.TranslateDialog(m_hWnd);
	CPropertyPage::OnInitDialog();

	LoadVssOptionStrings();

	UpdateData(FALSE);
	OnSelendokVerSys();

	return TRUE;
}

/**
 * @brief Called when user has selected VSS version.
 */
void PropVss::OnSelendokVerSys() 
{
	UpdateData(TRUE);
	String tempStr = theApp.LoadString(m_nVerSys == VCS_CLEARCASE ? IDS_CC_CMD : IDS_VSS_CMD);
	m_ctlVssL1.SetWindowText(tempStr.c_str());
	m_ctlPath.EnableWindow(m_nVerSys == VCS_VSS4 || m_nVerSys == VCS_CLEARCASE);
	m_ctlVssL1.EnableWindow(m_nVerSys == VCS_VSS4 || m_nVerSys == VCS_CLEARCASE);
	m_ctlBrowse.EnableWindow(m_nVerSys == VCS_VSS4 || m_nVerSys == VCS_CLEARCASE);
}

/**
 * Load strings for supported source code control
 */
void PropVss::LoadVssOptionStrings()
{
	/*
	Must be in order to agree with enum in MainFrm.h
	VCS_NONE = 0,
	VCS_VSS4,
	VCS_VSS5,
	VCS_CLEARCASE,
	*/

	m_ctlVerSys.AddString(theApp.LoadString(IDS_VCS_NONE).c_str());
	m_ctlVerSys.AddString(theApp.LoadString(IDS_VCS_VSS4).c_str());
	m_ctlVerSys.AddString(theApp.LoadString(IDS_VCS_VSS5).c_str());
	m_ctlVerSys.AddString(theApp.LoadString(IDS_VCS_CLEARCASE).c_str());
}

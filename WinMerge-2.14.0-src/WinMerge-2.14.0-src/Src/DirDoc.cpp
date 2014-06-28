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
 * @file  DirDoc.cpp
 *
 * @brief Implementation file for CDirDoc
 *
 */
// ID line follows -- this is updated by SVN
// $Id: DirDoc.cpp 7436 2010-11-21 13:06:27Z gerundt $
//

#include "StdAfx.h"
#include <shlwapi.h>		// PathFindFileName()
#include "Merge.h"
#include "HexMergeDoc.h"
#include "UnicodeString.h"
#include "CompareStats.h"
#include "FilterList.h"
#include "Ucs2Utf8.h"
#include "DirDoc.h"
#include "DirFrame.h"
#include "MainFrm.h"
#include "coretools.h"
#include "LogFile.h"
#include "paths.h"
#include "WaitStatusCursor.h"
#include "7zCommon.h"
#include "OptionsDef.h"
#include "FileActionScript.h"
#include "LineFiltersList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirDoc

IMPLEMENT_DYNCREATE(CDirDoc, CDocument)

/**
 * @brief Constructor.
 */
CDirDoc::CDirDoc()
: m_pCtxt(NULL)
, m_pDirView(NULL)
, m_pCompareStats(NULL)
, m_bROLeft(FALSE)
, m_bRORight(FALSE)
, m_bRecursive(FALSE)
, m_statusCursor(NULL)
, m_bReuseCloses(FALSE)
, m_bMarkedRescan(FALSE)
, m_pTempPathContext(NULL)
{
}

/**
 * @brief Destructor.
 *
 * Clears document list and deleted possible archive-temp files.
 */
CDirDoc::~CDirDoc()
{
	delete m_pCtxt;
	delete m_pCompareStats;

	// Inform all of our merge docs that we're closing
	POSITION pos = m_MergeDocs.GetHeadPosition();
	while (pos)
	{
		CMergeDoc * pMergeDoc = m_MergeDocs.GetNext(pos);
		pMergeDoc->DirDocClosing(this);
	}
	pos = m_HexMergeDocs.GetHeadPosition();
	while (pos)
	{
		CHexMergeDoc * pHexMergeDoc = m_HexMergeDocs.GetNext(pos);
		pHexMergeDoc->DirDocClosing(this);
	}
	// Delete all temporary folders belonging to this document
	while (m_pTempPathContext)
	{
		m_pTempPathContext = m_pTempPathContext->DeleteHead();
	}
}

/**
 * @brief Callback we give our frame which allows us to control whether
 * or not it closes.
 */
static bool DocClosableCallback(void * param)
{
	CDirDoc * pDoc = reinterpret_cast<CDirDoc *>(param);
	return pDoc->CanFrameClose();
}

/**
 * @brief Checks if there are mergedocs associated with this dirdoc.
 */
bool CDirDoc::CanFrameClose()
{
	return m_MergeDocs.IsEmpty() && m_HexMergeDocs.IsEmpty();
}

/**
 * @brief Called when new dirdoc is created.
 */
BOOL CDirDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	GetMainView()->GetParentFrame()->SetClosableCallback(&DocClosableCallback, this);

	return TRUE;
}


BEGIN_MESSAGE_MAP(CDirDoc, CDocument)
	//{{AFX_MSG_MAP(CDirDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDirDoc serialization

void CDirDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDirDoc commands

/**
 * @brief Initialise directory compare for given paths.
 *
 * Initialises directory compare with paths given and recursive choice.
 * Previous compare context is first free'd.
 * @param [in] paths Paths to compare
 * @param [in] bRecursive If TRUE subdirectories are included to compare.
 */
void CDirDoc::InitCompare(const PathContext & paths, BOOL bRecursive, CTempPathContext *pTempPathContext)
{
	m_pDirView->DeleteAllDisplayItems();
	// Anything that can go wrong here will yield an exception.
	// Default implementation of operator new() never returns NULL.
	delete m_pCtxt;
	
	if (m_pCompareStats == NULL)
		m_pCompareStats = new CompareStats();

	m_pCtxt = new CDiffContext(paths.GetLeft().c_str(), paths.GetRight().c_str());

	if (pTempPathContext)
	{
		ApplyLeftDisplayRoot(pTempPathContext->m_strLeftDisplayRoot);
		ApplyRightDisplayRoot(pTempPathContext->m_strRightDisplayRoot);
		pTempPathContext->m_pParent = m_pTempPathContext;
		m_pTempPathContext = pTempPathContext;
		m_pTempPathContext->m_strLeftRoot = m_pCtxt->GetNormalizedLeft();
		m_pTempPathContext->m_strRightRoot = m_pCtxt->GetNormalizedRight();
	}
	
	m_bRecursive = bRecursive;
	// All plugin management is done by our plugin manager
	m_pCtxt->m_piPluginInfos = &m_pluginman;
}

/**
 * @brief Tell if user may use ".." and move to parents directory.
 * This function checks if current compare's parent folders should
 * be allowed to open. I.e. if current compare folders are:
 * - C:\Work\Project1 and
 * - C:\Work\Project2
 * we check if C:\Work and C:\Work should be allowed to opened.
 * For regular folders we allow opening if both folders exist.
 * @param [out] leftParent Left parent folder to open.
 * @param [out] rightParent Right parent folder to open.
 * @return Info if opening parent folders should be enabled:
 * - No : upward RESTRICTED
 * - ParentIsRegularPath : upward ENABLED
 * - ParentIsTempPath : upward ENABLED
 */
CDirDoc::AllowUpwardDirectory::ReturnCode
CDirDoc::AllowUpwardDirectory(String &leftParent, String &rightParent)
{
	const String & left = GetLeftBasePath();
	const String & right = GetRightBasePath();

	// If we have temp context it means we are comparing archives
	if (IsArchiveFolders())
	{
		LPCTSTR lname = PathFindFileName(left.c_str());
		LPCTSTR rname = PathFindFileName(right.c_str());
        String::size_type cchLeftRoot = m_pTempPathContext->m_strLeftRoot.length();

		if (left.length() <= cchLeftRoot)
		{
			if (m_pTempPathContext->m_pParent)
			{
				leftParent = m_pTempPathContext->m_pParent->m_strLeftRoot;
				rightParent = m_pTempPathContext->m_pParent->m_strRightRoot;
				if (GetPairComparability(leftParent.c_str(), rightParent.c_str()) != IS_EXISTING_DIR)
					return AllowUpwardDirectory::Never;
				return AllowUpwardDirectory::ParentIsTempPath;
			}
			leftParent = m_pTempPathContext->m_strLeftDisplayRoot;
			rightParent = m_pTempPathContext->m_strRightDisplayRoot;
			if (!m_pCtxt->m_piFilterGlobal->includeFile(leftParent.c_str(), rightParent.c_str()))
				return AllowUpwardDirectory::Never;
			if (lstrcmpi(lname, _T("ORIGINAL")) == 0 && lstrcmpi(rname, _T("ALTERED")) == 0)
			{
				leftParent = paths_GetParentPath(leftParent.c_str());
				rightParent = paths_GetParentPath(rightParent.c_str());
			}
			lname = PathFindFileName(leftParent.c_str());
			rname = PathFindFileName(rightParent.c_str());
			if (lstrcmpi(lname, rname) == 0)
			{
				leftParent = paths_GetParentPath(leftParent.c_str());
				rightParent = paths_GetParentPath(rightParent.c_str());
				if (GetPairComparability(leftParent.c_str(), rightParent.c_str()) != IS_EXISTING_DIR)
					return AllowUpwardDirectory::Never;
				return AllowUpwardDirectory::ParentIsTempPath;
			}
			return AllowUpwardDirectory::No;
		}
		rname = lname;
	}

	// If regular parent folders exist, allow opening them
	leftParent = paths_GetParentPath(left.c_str());
	rightParent = paths_GetParentPath(right.c_str());
	if (GetPairComparability(leftParent.c_str(), rightParent.c_str()) != IS_EXISTING_DIR)
		return AllowUpwardDirectory::Never;
	return AllowUpwardDirectory::ParentIsRegularPath;
}

/**
 * @brief Load line filters to the compare context.
 * Loads linefilters, converts them to UTF-8 and sets them for compare context.
 */
void CDirDoc::LoadLineFilterList()
{
	ASSERT(m_pCtxt);
	
	BOOL bFilters = GetOptionsMgr()->GetBool(OPT_LINEFILTER_ENABLED);
	String filters = GetMainFrame()->m_pLineFilters->GetAsString();
	if (!bFilters || filters.empty())
	{
		delete m_pCtxt->m_pFilterList;
		m_pCtxt->m_pFilterList = NULL;
		return;
	}

	if (m_pCtxt->m_pFilterList)
		m_pCtxt->m_pFilterList->RemoveAllFilters();
	else
		m_pCtxt->m_pFilterList = new FilterList();

	char * regexp_str;
	FilterList::EncodingType type;

	regexp_str = UCS2UTF8_ConvertToUtf8(filters.c_str());
	type = FilterList::ENC_UTF8;

	m_pCtxt->m_pFilterList->AddRegExp(regexp_str, type);

	UCS2UTF8_Dealloc(regexp_str);
}

/**
 * @brief Perform directory comparison again from scratch
 */
void CDirDoc::Rescan()
{
	if (!m_pCtxt)
		return;

	CDirFrame *pf = m_pDirView->GetParentFrame();

	// If we're already doing a rescan, bail out
	UINT threadState = m_diffThread.GetThreadState();
	if (threadState == CDiffThread::THREAD_COMPARING)
		return;

	m_statusCursor = new CustomStatusCursor(0, IDC_APPSTARTING, IDS_STATUS_RESCANNING);

	GetLog()->Write(CLogFile::LNOTICE, _T("Starting directory scan:\n\tLeft: %s\n\tRight: %s\n"),
			m_pCtxt->GetLeftPath().c_str(), m_pCtxt->GetRightPath().c_str());
	m_pCompareStats->Reset();
	m_pDirView->StartCompare(m_pCompareStats);

	// Don't clear if only scanning selected items
	if (!m_bMarkedRescan)
	{
		m_pDirView->DeleteAllDisplayItems();
		m_pCtxt->RemoveAll();
	}

	LoadLineFilterList();

	DIFFOPTIONS options = {0};
	options.nIgnoreWhitespace = GetOptionsMgr()->GetInt(OPT_CMP_IGNORE_WHITESPACE);
	options.bIgnoreBlankLines = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_BLANKLINES);
	options.bFilterCommentsLines = GetOptionsMgr()->GetBool(OPT_CMP_FILTER_COMMENTLINES);
	options.bIgnoreCase = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CASE);
	options.bIgnoreEol = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_EOL);

	m_pCtxt->CreateCompareOptions(GetOptionsMgr()->GetInt(OPT_CMP_METHOD), options);

	m_pCtxt->m_bGuessEncoding = GetOptionsMgr()->GetBool(OPT_CP_DETECT);
	m_pCtxt->m_bIgnoreSmallTimeDiff = GetOptionsMgr()->GetBool(OPT_IGNORE_SMALL_FILETIME);
	m_pCtxt->m_bStopAfterFirstDiff = GetOptionsMgr()->GetBool(OPT_CMP_STOP_AFTER_FIRST);
	m_pCtxt->m_nQuickCompareLimit = GetOptionsMgr()->GetInt(OPT_CMP_QUICK_LIMIT);
	m_pCtxt->m_bPluginsEnabled = GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED);
	m_pCtxt->m_pCompareStats = m_pCompareStats;

	// Set total items count since we don't collect items
	if (m_bMarkedRescan)
		m_pCompareStats->IncreaseTotalItems(m_pDirView->GetSelectedCount());

	UpdateHeaderPath(0);
	UpdateHeaderPath(1);
	// draw the headers as active ones
	pf->GetHeaderInterface()->SetActive(0, TRUE);
	pf->GetHeaderInterface()->SetActive(1, TRUE);

	// Make sure filters are up-to-date
	theApp.m_globalFileFilter.ReloadUpdatedFilters();
	m_pCtxt->m_piFilterGlobal = &theApp.m_globalFileFilter;

	// Show active filter name in statusbar
	pf->SetFilterStatusDisplay(theApp.m_globalFileFilter.GetFilterNameOrMask());

	// Folder names to compare are in the compare context
	m_diffThread.SetContext(m_pCtxt);
	m_diffThread.SetHwnd(m_pDirView->GetSafeHwnd());
	m_diffThread.SetMessageIDs(MSG_UI_UPDATE);
	m_diffThread.SetCompareSelected(!!m_bMarkedRescan);
	m_diffThread.CompareDirectories(m_pCtxt->GetNormalizedLeft(),
			m_pCtxt->GetNormalizedRight(), m_bRecursive);
	m_bMarkedRescan = FALSE;
}

/**
 * @brief Determines if the user wants to see given item.
 * This function determines what items to show and what items to hide. There
 * are lots of combinations, but basically we check if menuitem is enabled or
 * disabled and show/hide matching items. For non-recursive compare we never
 * hide folders as that would disable user browsing into them. And we even
 * don't really know if folders are identical or different as we haven't
 * compared them.
 * @param [in] di Item to check.
 * @return TRUE if item should be shown, FALSE if not.
 * @sa CDirDoc::Redisplay()
 */
BOOL CDirDoc::IsShowable(const DIFFITEM & di)
{
	if (di.customFlags1 & ViewCustomFlags::HIDDEN)
		return FALSE;

	if (di.diffcode.isResultFiltered())
	{
		// Treat SKIPPED as a 'super'-flag. If item is skipped and user
		// wants to see skipped items show item regardless of other flags
		return GetOptionsMgr()->GetBool(OPT_SHOW_SKIPPED);
	}

	if (di.diffcode.isDirectory())
	{
		// Subfolders in non-recursive compare can only be skipped or unique
		if (!m_bRecursive)
		{
			// left/right filters
			if (di.diffcode.isSideLeftOnly() &&	!GetOptionsMgr()->GetBool(OPT_SHOW_UNIQUE_LEFT))
				return FALSE;
			if (di.diffcode.isSideRightOnly() && !GetOptionsMgr()->GetBool(OPT_SHOW_UNIQUE_RIGHT))
				return FALSE;

			// result filters
			if (di.diffcode.isResultError() && !GetMainFrame()->m_bShowErrors)
				return FALSE;
		}
		else // recursive mode (including tree-mode)
		{
			// left/right filters
			if (di.diffcode.isSideLeftOnly() &&	!GetOptionsMgr()->GetBool(OPT_SHOW_UNIQUE_LEFT))
				return FALSE;
			if (di.diffcode.isSideRightOnly() && !GetOptionsMgr()->GetBool(OPT_SHOW_UNIQUE_RIGHT))
				return FALSE;

			// ONLY filter folders by result (identical/different) for tree-view.
			// In the tree-view we show subfolders with identical/different
			// status. The flat view only shows files inside folders. So if we
			// filter by status the files inside folder are filtered too and
			// users see files appearing/disappearing without clear logic.		
			if (GetOptionsMgr()->GetBool(OPT_TREE_MODE))
			{
				// result filters
				if (di.diffcode.isResultError() && !GetMainFrame()->m_bShowErrors)
					return FALSE;

				// result filters
				if (di.diffcode.isResultSame() && !GetOptionsMgr()->GetBool(OPT_SHOW_IDENTICAL))
					return FALSE;
				if (di.diffcode.isResultDiff() && !GetOptionsMgr()->GetBool(OPT_SHOW_DIFFERENT))
					return FALSE;
			}
		}
	}
	else
	{
		// left/right filters
		if (di.diffcode.isSideLeftOnly() && !GetOptionsMgr()->GetBool(OPT_SHOW_UNIQUE_LEFT))
			return FALSE;
		if (di.diffcode.isSideRightOnly() && !GetOptionsMgr()->GetBool(OPT_SHOW_UNIQUE_RIGHT))
			return FALSE;

		// file type filters
		if (di.diffcode.isBin() && !GetOptionsMgr()->GetBool(OPT_SHOW_BINARIES))
			return FALSE;

		// result filters
		if (di.diffcode.isResultSame() && !GetOptionsMgr()->GetBool(OPT_SHOW_IDENTICAL))
			return FALSE;
		if (di.diffcode.isResultError() && !GetMainFrame()->m_bShowErrors)
			return FALSE;
		if (di.diffcode.isResultDiff() && !GetOptionsMgr()->GetBool(OPT_SHOW_DIFFERENT))
			return FALSE;
	}
	return TRUE;
}

/**
 * @brief Empty & reload listview (of files & columns) with comparison results
 * @todo Better solution for special items ("..")?
 */
void CDirDoc::Redisplay()
{
	if (!m_pDirView)
		return;

	// Do not redisplay an empty CDirView
	// Not only does it not have results, but AddSpecialItems will crash
	// trying to dereference null context pointer to get to paths
	if (!HasDiffs())
		return;

	m_pDirView->Redisplay();
}

CDirView * CDirDoc::GetMainView()
{
	CDirView *pView = NULL;
	if (POSITION pos = GetFirstViewPosition())
	{
		pView = static_cast<CDirView*>(GetNextView(pos));
		ASSERT_KINDOF(CDirView, pView);
	}
	return pView;
}

/**
 * @brief Update in-memory diffitem status from disk.
 * @param [in] diffPos POSITION of item in UI list.
 * @param [in] bLeft If TRUE left-side item is updated.
 * @param [in] bRight If TRUE right-side item is updated.
 */
void CDirDoc::UpdateStatusFromDisk(UINT_PTR diffPos, BOOL bLeft, BOOL bRight)
{
	m_pCtxt->UpdateStatusFromDisk(diffPos, bLeft, bRight);
}

/**
 * @brief Update in-memory diffitem status from disk and update view.
 * @param [in] nIdx Index of item in UI list.
 * @param [in] bLeft If TRUE left-side item is updated.
 * @param [in] bRight If TRUE right-side item is updated.
 * @note Do not call this function from DirView code! This function
 * calls slow DirView functions to get item position and to update GUI.
 * Use UpdateStatusFromDisk() function instead.
 */
void CDirDoc::ReloadItemStatus(UINT nIdx, BOOL bLeft, BOOL bRight)
{
	// Get position of item in DiffContext
	UINT_PTR diffpos = m_pDirView->GetItemKey(nIdx);

	// in case just copied (into existence) or modified
	UpdateStatusFromDisk(diffpos, bLeft, bRight);

	// Update view
	m_pDirView->UpdateDiffItemStatus(nIdx);
}

void CDirDoc::InitStatusStrings()
{

}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CDirDoc::UpdateResources()
{
	if (m_pDirView)
		m_pDirView->UpdateResources();

	SetTitle(0);

	Redisplay();
}

/**
 * @brief Find the CDiffContext diffpos of an item from its left & right paths
 * @return POSITION to item, NULL if not found.
 * @note Filenames must be same, if they differ NULL is returned.
 */
UINT_PTR CDirDoc::FindItemFromPaths(LPCTSTR pathLeft, LPCTSTR pathRight)
{
	LPCTSTR file1 = paths_FindFileName(pathLeft);
	LPCTSTR file2 = paths_FindFileName(pathRight);

	// Filenames must be identical
	if (lstrcmpi(file1, file2) != 0)
		return NULL;

	String path1(pathLeft, file1 - pathLeft); // include trailing backslash
	String path2(pathRight, file2 - pathRight); // include trailing backslash

	// Path can contain (because of difftools?) '/' and '\'
	// so for comparing purposes, convert whole path to use '\\'
	replace_char(&*path1.begin(), '/', '\\');
	replace_char(&*path2.begin(), '/', '\\');

	String base1 = m_pCtxt->GetLeftPath(); // include trailing backslash
	if (path1.compare(0, base1.length(), base1.c_str()) != 0)
		return NULL;
	path1.erase(0, base1.length()); // turn into relative path
	if (String::size_type length = path1.length())
		path1.resize(length - 1); // remove trailing backslash

	String base2 = m_pCtxt->GetRightPath(); // include trailing backslash
	if (path2.compare(0, base2.length(), base2.c_str()) != 0)
		return NULL;
	path2.erase(0, base2.length()); // turn into relative path
	if (String::size_type length = path2.length())
		path2.resize(length - 1); // remove trailing backslash

	UINT_PTR pos = m_pCtxt->GetFirstDiffPosition();
	while (UINT_PTR currentPos = pos) // Save our current pos before getting next
	{
		const DIFFITEM &di = m_pCtxt->GetNextDiffPosition(pos);
		if (di.left.path == path1 &&
			di.right.path == path2 &&
			di.left.filename == file1 &&
			di.right.filename == file2)
		{
			return currentPos;
		}
	}
	return 0;
}

/**
 * @brief Stash away our view pointer.
 */
void CDirDoc::SetDirView(CDirView * newView)
{
	m_pDirView = newView;
	// MFC has a view list for us, so lets check against it
	POSITION pos = GetFirstViewPosition();
	CDirView * temp = static_cast<CDirView *>(GetNextView(pos));
	ASSERT(temp == m_pDirView); // verify that our stashed pointer is the same as MFC's
}

/**
 * @brief A new MergeDoc has been opened.
 */
void CDirDoc::AddMergeDoc(CMergeDoc * pMergeDoc)
{
	ASSERT(pMergeDoc);
	m_MergeDocs.AddTail(pMergeDoc);
}

/**
 * @brief A new HexMergeDoc has been opened.
 */
void CDirDoc::AddHexMergeDoc(CHexMergeDoc * pHexMergeDoc)
{
	ASSERT(pHexMergeDoc);
	m_HexMergeDocs.AddTail(pHexMergeDoc);
}

/**
 * @brief MergeDoc informs us it is closing.
 */
void CDirDoc::MergeDocClosing(CDocument * pMergeDoc)
{
	ASSERT(pMergeDoc);
	if (POSITION pos = m_MergeDocs.CPtrList::Find(pMergeDoc))
		m_MergeDocs.RemoveAt(pos);
	else if (POSITION pos = m_HexMergeDocs.CPtrList::Find(pMergeDoc))
		m_HexMergeDocs.RemoveAt(pos);
	else
		ASSERT(FALSE);

	// If dir compare is empty (no compare results) and we are not closing
	// because of reuse close also dir compare
	if (m_pDirView)
	{
		if (m_pCtxt == NULL && !m_bReuseCloses)
			m_pDirView->PostMessage(WM_COMMAND, ID_FILE_CLOSE);
	}
	else if (m_MergeDocs.GetCount() == 0 && m_HexMergeDocs.GetCount() == 0)
	{
		delete this;
	}
}

/**
 * @brief Close MergeDocs opened from DirDoc.
 *
 * Asks confirmation for docs containing unsaved data and then
 * closes MergeDocs.
 * @return TRUE if success, FALSE if user canceled or closing failed
 */
BOOL CDirDoc::CloseMergeDocs()
{
	POSITION pos = m_MergeDocs.GetHeadPosition();
	while (pos)
	{
		CMergeDoc * pMergeDoc = m_MergeDocs.GetNext(pos);
		if (!pMergeDoc->CloseNow())
			return FALSE;
	}
	pos = m_HexMergeDocs.GetHeadPosition();
	while (pos)
	{
		CHexMergeDoc * pHexMergeDoc = m_HexMergeDocs.GetNext(pos);
		if (!pHexMergeDoc->CloseNow())
			return FALSE;
	}
	return TRUE;
}

/**
 * @brief Prepare for reuse.
 *
 * Close all our merge docs (which gives them chance to save)
 * This may fail if user cancels a Save dialog
 * in which case this aborts and returns FALSE
 */
BOOL CDirDoc::ReusingDirDoc()
{
	m_bReuseCloses = TRUE;

	// Inform all of our merge docs that we're closing
	if (!CloseMergeDocs())
		return FALSE;

	m_bReuseCloses = FALSE;

	// clear diff display
	ASSERT(m_pDirView);
	m_pDirView->DeleteAllDisplayItems();

	// delete comparison parameters and results
	delete m_pCtxt;
	m_pCtxt = NULL;

	while (m_pTempPathContext)
	{
		m_pTempPathContext = m_pTempPathContext->DeleteHead();
	}

	return TRUE;
}

/**
 * @brief Obtain a merge doc to display a difference in files.
 * @param [out] pNew Set to TRUE if a new doc is created,
 * and FALSE if an existing one reused.
 * @return Pointer to CMergeDoc to use (new or existing). 
 */
CMergeDoc * CDirDoc::GetMergeDocForDiff(BOOL * pNew)
{
	CMergeDoc * pMergeDoc = 0;
	// policy -- use an existing merge doc if available
	const BOOL bMultiDocs = GetOptionsMgr()->GetBool(OPT_MULTIDOC_MERGEDOCS);
	if (!bMultiDocs && !m_MergeDocs.IsEmpty())
	{
		*pNew = FALSE;
		pMergeDoc = m_MergeDocs.GetHead();
	}
	else
	{
		// Create a new merge doc
		pMergeDoc = (CMergeDoc*)theApp.m_pDiffTemplate->OpenDocumentFile(NULL);
		AddMergeDoc(pMergeDoc);
		pMergeDoc->SetDirDoc(this);
		*pNew = TRUE;
	}
	return pMergeDoc;
}

/**
 * @brief Obtain a hex merge doc to display a difference in files.
 * @param [out] pNew Set to TRUE if a new doc is created,
 * and FALSE if an existing one reused.
 * @return Pointer to CHexMergeDoc to use (new or existing). 
 */
CHexMergeDoc * CDirDoc::GetHexMergeDocForDiff(BOOL * pNew)
{
	CHexMergeDoc * pHexMergeDoc = 0;
	// policy -- use an existing merge doc if available
	const BOOL bMultiDocs = GetOptionsMgr()->GetBool(OPT_MULTIDOC_MERGEDOCS);
	if (!bMultiDocs && !m_HexMergeDocs.IsEmpty())
	{
		*pNew = FALSE;
		pHexMergeDoc = m_HexMergeDocs.GetHead();
	}
	else
	{
		// Create a new merge doc
		pHexMergeDoc = (CHexMergeDoc*)theApp.m_pHexMergeTemplate->OpenDocumentFile(NULL);
		if (pHexMergeDoc)
		{
			AddHexMergeDoc(pHexMergeDoc);
			pHexMergeDoc->SetDirDoc(this);
		}
		*pNew = TRUE;
	}
	return pHexMergeDoc;
}

/**
 * @brief Update changed item's compare status
 * @param [in] paths Paths for files we update
 * @param [in] nDiffs Total amount of differences
 * @param [in] nTrivialDiffs Amount of ignored differences
 * @param [in] bIdentical TRUE if files became identical, FALSE otherwise.
 */
void CDirDoc::UpdateChangedItem(PathContext &paths,
	UINT nDiffs, UINT nTrivialDiffs, BOOL bIdentical)
{
	UINT_PTR pos = FindItemFromPaths(paths.GetLeft().c_str(), paths.GetRight().c_str());
	// If we failed files could have been swapped so lets try again
	if (!pos)
		pos = FindItemFromPaths(paths.GetRight().c_str(), paths.GetLeft().c_str());
	
	// Update status if paths were found for items.
	// Fail means we had unique items compared as 'renamed' items
	// so there really is not status to update.
	if (pos > 0)
	{
		int ind = m_pDirView->GetItemIndex(pos);

		// Figure out new status code
		UINT diffcode = (bIdentical ? DIFFCODE::SAME : DIFFCODE::DIFF);

		// Update both views and diff context memory
		SetDiffCompare(diffcode, ind);

		if (nDiffs != -1 && nTrivialDiffs != -1)
			SetDiffCounts(nDiffs, nTrivialDiffs, ind);
		ReloadItemStatus(ind, TRUE, TRUE);
	}
}

/**
 * @brief Cleans up after directory compare
 */
void CDirDoc::CompareReady()
{
	GetLog()->Write(CLogFile::LNOTICE, _T("Directory scan complete\n"));

	// finish the cursor (the hourglass/pointer combo) we had open during display
	delete m_statusCursor;
	m_statusCursor = NULL;

}

/**
 * @brief Refresh cached options.
 *
 * For compare speed, we have to cache some frequently needed options,
 * instead of getting option value every time from OptionsMgr. This
 * function must be called every time options are changed to OptionsMgr.
 */
void CDirDoc::RefreshOptions()
{
	if (m_pDirView)
		m_pDirView->RefreshOptions();
}

/**
 * @brief Set left/right side readonly-status
 * @param bLeft Select side to set (TRUE = left)
 * @param bReadOnly New status of selected side
 */
void CDirDoc::SetReadOnly(BOOL bLeft, BOOL bReadOnly)
{
	if (bLeft)
		m_bROLeft = bReadOnly;
	else
		m_bRORight = bReadOnly;
}

/**
 * @brief Return left/right side readonly-status
 * @param bLeft Select side to ask (TRUE = left)
 */
BOOL CDirDoc::GetReadOnly(BOOL bLeft) const
{
	if (bLeft)
		return m_bROLeft;
	else
		return m_bRORight;
}

/**
 * @brief Set side status of diffitem
 * @note This does not update UI - ReloadItemStatus() does
 * @sa CDirDoc::ReloadItemStatus()
 */
void CDirDoc::SetDiffSide(UINT diffcode, int idx)
{
	SetDiffStatus(diffcode, DIFFCODE::SIDEFLAGS, idx);
}

/**
 * @brief Set compare status of diffitem
 * @note This does not update UI - ReloadItemStatus() does
 * @sa CDirDoc::ReloadItemStatus()
 */
void CDirDoc::SetDiffCompare(UINT diffcode, int idx)
{
	SetDiffStatus(diffcode, DIFFCODE::COMPAREFLAGS, idx);
}

/**
 * @brief Set status for diffitem
 * @param diffcode New code
 * @param mask Defines allowed set of flags to change
 * @param idx Item's index to list in UI
 */
void CDirDoc::SetDiffStatus(UINT diffcode, UINT mask, int idx)
{
	// Get position of item in DiffContext 
	UINT_PTR diffpos = m_pDirView->GetItemKey(idx);

	// TODO: Why is the update broken into these pieces ?
	// Someone could figure out these pieces and probably simplify this.

	// Update DIFFITEM code (comparison result) to DiffContext
	m_pCtxt->SetDiffStatusCode(diffpos, diffcode, mask);

	// update DIFFITEM time (and other disk info), and tell views
}

/**
 * @brief Write path and filename to headerbar
 * @note SetText() does not repaint unchanged text
 */
void CDirDoc::UpdateHeaderPath(BOOL bLeft)
{
	CDirFrame *pf = m_pDirView->GetParentFrame();
	ASSERT(pf);
	int nPane = 0;
	String sText;

	if (bLeft)
	{
		if (!m_strLeftDesc.empty())
			sText = m_strLeftDesc;
		else
		{
			sText = m_pCtxt->GetLeftPath();
			ApplyLeftDisplayRoot(sText);
		}
		nPane = 0;
	}
	else
	{
		if (!m_strRightDesc.empty())
			sText = m_strRightDesc;
		else
		{
			sText = m_pCtxt->GetRightPath();
			ApplyRightDisplayRoot(sText);
		}
		nPane = 1;
	}

	pf->GetHeaderInterface()->SetText(nPane, sText.c_str());
}

/**
 * @brief virtual override called just before document is saved and closed
 */
BOOL CDirDoc::SaveModified() 
{
	// Do not allow closing if there is a thread running
	if (m_diffThread.GetThreadState() == CDiffThread::THREAD_COMPARING)
		return FALSE;
	
	return CDocument::SaveModified();
}

/**
 * @brief Send signal to thread to stop current scan
 *
 * @sa CDirCompStateBar::OnStop()
 */
void CDirDoc::AbortCurrentScan()
{
	GetLog()->Write(CLogFile::LNOTICE, _T("Dircompare aborted!"));
	m_diffThread.Abort();
}

/**
 * @brief Returns true if there is an active scan that hasn't been aborted.
 */
bool CDirDoc::IsCurrentScanAbortable() const
{
	return (m_diffThread.GetThreadState() == CDiffThread::THREAD_COMPARING 
		&& !m_diffThread.IsAborting());
}

/**
 * @brief Set directory description texts shown in headerbar
 */
void CDirDoc::SetDescriptions(const String &strLeftDesc, const String &strRightDesc)
{
	m_strLeftDesc = strLeftDesc;
	m_strRightDesc = strRightDesc;
}

/**
 * @brief Replace internal root by display root (left).
 * When we have a archive file open, this function converts physical folder
 * (that is in the temp folder where archive was extracted) to the virtual
 * path for showing. The virtual path is path to the archive file, archive
 * file name and folder inside the archive.
 * @param [in, out] sText Path to convert.
 */
void CDirDoc::ApplyLeftDisplayRoot(String &sText)
{
	if (m_pTempPathContext)
	{
		sText.erase(0, m_pTempPathContext->m_strLeftRoot.length());
		sText.insert(0, m_pTempPathContext->m_strLeftDisplayRoot.c_str());
	}
}

/**
 * @brief Replace internal root by display root (right).
 * When we have a archive file open, this function converts physical folder
 * (that is in the temp folder where archive was extracted) to the virtual
 * path for showing. The virtual path is path to the archive file, archive
 * file name and folder inside the archive.
 * @param [in, out] sText Path to convert.
 */
void CDirDoc::ApplyRightDisplayRoot(String &sText)
{
	if (m_pTempPathContext)
	{
		sText.erase(0, m_pTempPathContext->m_strRightRoot.length());
		sText.insert(0, m_pTempPathContext->m_strRightDisplayRoot.c_str());
	}
}

/**
 * @brief Store a plugin setting for specified file comparison
 */
void CDirDoc::SetPluginPrediffSetting(LPCTSTR filteredFilenames, int newsetting)
{
	m_pluginman.SetPrediffSetting(filteredFilenames, newsetting);
}

/**
 * @brief Store a plugin setting for specified file comparison
 */
void CDirDoc::SetPluginPrediffer(LPCTSTR filteredFilenames, const CString & prediffer)
{
	m_pluginman.SetPrediffer(filteredFilenames, prediffer);
}

/**
 * @brief Retrieve any cached plugin info for specified comparison
 */
void CDirDoc::FetchPluginInfos(LPCTSTR filteredFilenames, 
                               PackingInfo ** infoUnpacker, 
                               PrediffingInfo ** infoPrediffer)
{
	// This will manufacture the needed objects if not already cached
	IPluginInfos * piPluginInfos = &m_pluginman;
	piPluginInfos->FetchPluginInfos(filteredFilenames, infoUnpacker, infoPrediffer);
}

void CDirDoc::SetDiffCounts(UINT diffs, UINT ignored, int idx)
{
	// Get position of item in DiffContext 
	UINT_PTR diffpos = m_pDirView->GetItemKey(idx);

	// Update diff counts
	m_pCtxt->SetDiffCounts(diffpos, diffs, ignored);
}

/**
 * @brief Update results for FileActionItem.
 * This functions is called to update DIFFITEM after FileActionItem.
 * @param [in] act Action that was done.
 * @param [in] pos List position for DIFFITEM affected.
 */
void CDirDoc::UpdateDiffAfterOperation(const FileActionItem & act, UINT_PTR pos)
{
	ASSERT(pos != NULL);
	const DIFFITEM &di = GetDiffByKey(pos);

	// Use FileActionItem types for simplicity for now.
	// Better would be to use FileAction contained, since it is not
	// UI dependent.
	switch (act.UIResult)
	{
	case FileActionItem::UI_SYNC:
		SetDiffSide(DIFFCODE::BOTH, act.context);
		if (act.dirflag)
			SetDiffCompare(DIFFCODE::NOCMP, act.context);
		else
			SetDiffCompare(DIFFCODE::SAME, act.context);
		SetDiffCounts(0, 0, act.context);
		break;

	case FileActionItem::UI_DEL_LEFT:
		if (di.diffcode.isSideLeftOnly())
		{
			RemoveDiffByKey(pos);
		}
		else
		{
			SetDiffSide(DIFFCODE::RIGHT, act.context);
			SetDiffCompare(DIFFCODE::NOCMP, act.context);
		}
		break;

	case FileActionItem::UI_DEL_RIGHT:
		if (di.diffcode.isSideRightOnly())
		{
			RemoveDiffByKey(pos);
		}
		else
		{
			SetDiffSide(DIFFCODE::LEFT, act.context);
			SetDiffCompare(DIFFCODE::NOCMP, act.context);
		}
		break;

	case FileActionItem::UI_DEL_BOTH:
		RemoveDiffByKey(pos);
		break;
	}
}

/**
 * @brief Set document title to given string or items compared.
 * 
 * Formats and sets caption for directory compare window. Caption
 * has left- and right-side paths separated with '-'.
 *
 * @param [in] lpszTitle New title for window if. If this parameter
 * is not NULL we use this string, otherwise format caption from
 * actual paths.
 */
void CDirDoc::SetTitle(LPCTSTR lpszTitle)
{
	if (!m_pDirView)
		return;

	if (lpszTitle)
		CDocument::SetTitle(lpszTitle);
	else if (!m_pCtxt || m_pCtxt->GetLeftPath().empty() ||
		m_pCtxt->GetRightPath().empty())
	{
		String title = theApp.LoadString(IDS_DIRECTORY_WINDOW_TITLE);
		CDocument::SetTitle(title.c_str());
	}
	else
	{
		const TCHAR strSeparator[] = _T(" - ");
		String strPath = m_pCtxt->GetLeftPath();
		ApplyLeftDisplayRoot(strPath);
		String strTitle = PathFindFileName(strPath.c_str());
		strTitle += strSeparator;
		strPath = m_pCtxt->GetRightPath();
		ApplyRightDisplayRoot(strPath);
		strTitle += PathFindFileName(strPath.c_str());
		CDocument::SetTitle(strTitle.c_str());
	}	
}

/**
 * @brief Set item's view-flag.
 * @param [in] key Item fow which flag is set.
 * @param [in] flag Flag value to set.
 * @param [in] mask Mask for possible flag values.
 */
void CDirDoc::SetItemViewFlag(UINT_PTR key, UINT flag, UINT mask)
{
	UINT curFlags = m_pCtxt->GetCustomFlags1(key);
	curFlags &= ~mask; // Zero bits masked
	curFlags |= flag;
	m_pCtxt->SetCustomFlags1(key, curFlags);
}

/**
 * @brief Set all item's view-flag.
 * @param [in] flag Flag value to set.
 * @param [in] mask Mask for possible flag values.
 */
void CDirDoc::SetItemViewFlag(UINT flag, UINT mask)
{
	UINT_PTR pos = m_pCtxt->GetFirstDiffPosition();

	while (pos != NULL)
	{
		UINT curFlags = m_pCtxt->GetCustomFlags1(pos);
		curFlags &= ~mask; // Zero bits masked
		curFlags |= flag;
		m_pCtxt->SetCustomFlags1(pos, curFlags);
		m_pCtxt->GetNextDiffPosition(pos);
	}
}

/**
 * @brief Checks if current folders are opened from archive file.
 * @return true if we are inside archive, false otherwise.
 */
bool CDirDoc::IsArchiveFolders()
{
	if (m_pTempPathContext)
		return true;
	else
		return false;
}

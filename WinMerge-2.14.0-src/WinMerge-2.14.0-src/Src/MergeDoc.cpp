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
 * @file  MergeDoc.cpp
 *
 * @brief Implementation file for CMergeDoc
 *
 */
// ID line follows -- this is updated by SVN
// $Id: MergeDoc.cpp 7569 2011-10-24 06:29:38Z jtuc $

#include "StdAfx.h"
#include <shlwapi.h>		// PathCompactPathEx()
#include <io.h>
#include "UnicodeString.h"
#include "Merge.h"
#include "MainFrm.h"
#include "DiffTextBuffer.h"
#include "Environment.h"
#include "Ucs2Utf8.h"
#include "DiffContext.h"	// FILE_SAME
#include "MovedLines.h"
#include "GETOPT.H"
#include "FNMATCH.H"
#include "coretools.h"
#include "MergeEditView.h"
#include "MergeDiffDetailView.h"
#include "ChildFrm.h"
#include "DirDoc.h"
#include "files.h"
#include "WaitStatusCursor.h"
#include "FileTransform.h"
#include "unicoder.h"
#include "UniFile.h"
#include "OptionsDef.h"
#include "DiffFileInfo.h"
#include "SaveClosingDlg.h"
#include "DiffList.h"
#include "codepage.h"
#include "paths.h"
#include "OptionsMgr.h"
#include "ProjectFile.h"
#include "MergeLineFlags.h"
#include "FileOrFolderSelect.h"
#include "LineFiltersList.h"
#include "TempFile.h"
#include "MergeCmdLineInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using std::swap;

/** @brief Max len of path in caption. */
static const UINT CAPTION_PATH_MAX = 50;

/** @brief EOL types */
static LPCTSTR crlfs[] =
{
	_T ("\x0d\x0a"), //  DOS/Windows style
	_T ("\x0a"),     //  UNIX style
	_T ("\x0d")      //  Macintosh style
};

static void SaveBuffForDiff(CDiffTextBuffer & buf, CDiffTextBuffer & buf2, LPCTSTR filepath);
static void UnescapeControlChars(CString &s);

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc

IMPLEMENT_DYNCREATE(CMergeDoc, CDocument)

BEGIN_MESSAGE_MAP(CMergeDoc, CDocument)
	//{{AFX_MSG_MAP(CMergeDoc)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_LEFT, OnFileSaveLeft)
	ON_COMMAND(ID_FILE_SAVE_RIGHT, OnFileSaveRight)
	ON_COMMAND(ID_FILE_SAVEAS_LEFT, OnFileSaveAsLeft)
	ON_COMMAND(ID_FILE_SAVEAS_RIGHT, OnFileSaveAsRight)
	ON_UPDATE_COMMAND_UI(ID_STATUS_DIFFNUM, OnUpdateStatusNum)
	ON_COMMAND(ID_TOOLS_GENERATEREPORT, OnToolsGenerateReport)
	ON_COMMAND(ID_FILE_ENCODING, OnFileEncoding)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc construction/destruction


#pragma warning(disable:4355)

/**
 * @brief Constructor.
 */
CMergeDoc::CMergeDoc()
: m_bEnableRescan(true)
, m_nCurDiff(-1)
, m_pDirDoc(NULL)
, m_bMixedEol(false)
{
	DIFFOPTIONS options = {0};

	// COleDateTime m_LastRescan
	curUndo = undoTgt.begin();
	m_pView[MERGE_VIEW_LEFT] = NULL;
	m_pView[MERGE_VIEW_RIGHT] = NULL;
	m_pDetailView[0] = NULL;
	m_pDetailView[1] = NULL;
	m_pInfoUnpacker = new PackingInfo;
	m_nBufferType[0] = BUFFER_NORMAL;
	m_nBufferType[1] = BUFFER_NORMAL;
	m_bMergingMode = GetOptionsMgr()->GetBool(OPT_MERGE_MODE);
	m_bEditAfterRescan[0] = false;
	m_bEditAfterRescan[1] = false;
	m_ptBuf[0] = new CDiffTextBuffer(this, 0);
	m_ptBuf[1] = new CDiffTextBuffer(this, 1);
	m_pSaveFileInfo[0] = new DiffFileInfo();
	m_pSaveFileInfo[1] = new DiffFileInfo();
	m_pRescanFileInfo[0] = new DiffFileInfo();
	m_pRescanFileInfo[1] = new DiffFileInfo();

	m_diffWrapper.SetDetectMovedBlocks(GetOptionsMgr()->GetBool(OPT_CMP_MOVED_BLOCKS));
	options.nIgnoreWhitespace = GetOptionsMgr()->GetInt(OPT_CMP_IGNORE_WHITESPACE);
	options.bIgnoreBlankLines = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_BLANKLINES);
	options.bFilterCommentsLines = GetOptionsMgr()->GetBool(OPT_CMP_FILTER_COMMENTLINES);
	options.bIgnoreCase = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CASE);
	options.bIgnoreEol = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_EOL);

	m_diffWrapper.SetOptions(&options);
	m_diffWrapper.SetPrediffer(NULL);
}

#pragma warning(default:4355)

/**
 * @brief Destructor.
 *
 * Informs associated dirdoc that mergedoc is closing.
 */
CMergeDoc::~CMergeDoc()
{	
	if (m_pDirDoc)
	{
		m_pDirDoc->MergeDocClosing(this);
		m_pDirDoc = NULL;
	}

	delete m_pInfoUnpacker;
	delete m_pSaveFileInfo[0];
	delete m_pSaveFileInfo[1];
	delete m_pRescanFileInfo[0];
	delete m_pRescanFileInfo[1];
	delete m_ptBuf[0];
	delete m_ptBuf[1];
}

/**
 * @brief Deleted data associated with doc before closing.
 */
void CMergeDoc::DeleteContents ()
{
	CDocument::DeleteContents ();
	m_ptBuf[0]->FreeAll ();
	m_ptBuf[1]->FreeAll ();
	m_tempFiles[0].Delete();
	m_tempFiles[1].Delete();
}

void CMergeDoc::OnFileEvent (WPARAM /*wEvent*/, LPCTSTR /*pszPathName*/)
{
	/*if (!(theApp.m_dwFlags & EP_NOTIFY_CHANGES))
    return;
	MessageBeep (MB_ICONEXCLAMATION);
	CFrameWnd *pwndMain= (CFrameWnd*) theApp.GetMainWnd ();
	ASSERT (pwndMain);
	if (!pwndMain->IsWindowVisible ())
          ((CMainFrame*) pwndMain)->FlashUntilFocus ();
	if (wEvent & FE_MODIFIED)
  	{
  	  bool bReload = (theApp.m_dwFlags & EP_AUTO_RELOAD) != 0;
  	  if (!bReload)
  	    {
          CString sMsg;
          sMsg.Format (IDS_FILE_CHANGED, pszPathName);
  	      bReload = AfxMessageBox (sMsg, MB_YESNO|MB_ICONQUESTION) == IDYES;
  	    }
  	  if (bReload)
        {
	        POSITION pos = GetFirstViewPosition ();
          ASSERT (pos);
	        CEditPadView *pView;
          do
            {
	            pView = (CEditPadView*) GetNextView (pos);
              pView->PushCursor ();
            }
          while (pos);
          m_xTextBuffer.FreeAll ();
          m_xTextBuffer.LoadFromFile (pszPathName);
	        pos = GetFirstViewPosition ();
          ASSERT (pos);
          do
            {
	            pView = (CEditPadView*) GetNextView (pos);
              pView->PopCursor ();
              HWND hWnd = pView->GetSafeHwnd ();
              ::RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE|RDW_INTERNALPAINT|RDW_ERASE|RDW_ERASENOW|RDW_UPDATENOW|RDW_NOFRAME);
            }
          while (pos);
        }
    }
  else if (wEvent & FE_DELETED)
    {
      if (!(theApp.m_dwFlags & EP_AUTO_RELOAD))
        {
          CString sMsg;
          sMsg.Format (IDS_FILE_DELETED, pszPathName);
        	AfxMessageBox (sMsg, MB_OK|MB_ICONINFORMATION);
        }
    }*/
}

/**
 * @brief Called when new document is created.
 *
 * Initialises buffers.
 */
BOOL CMergeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	m_ptBuf[0]->InitNew();
	m_ptBuf[1]->InitNew();
	return TRUE;
}

/**
 * @brief Determines currently active view.
 * @return one of MERGEVIEW_INDEX_TYPE values or -1 in error.
 * @todo Detect location pane and return != 1 for it.
 */
int CMergeDoc::GetActiveMergeViewIndexType() const
{
	CMergeDoc * pThis = const_cast<CMergeDoc *>(this);
	// Get active view pointer
	CView * pActiveView = pThis->GetParentFrame()->GetActiveView();
	// Cast it to common base of all our views
	CCrystalTextView* curView = dynamic_cast<CCrystalTextView*> (pActiveView);
	// Now test it against all our views to see which it is
	if (curView == GetLeftView())
		return MERGEVIEW_LEFT;
	else if (curView == GetRightView())
		return MERGEVIEW_RIGHT;
	else if (curView == GetLeftDetailView())
		return MERGEVIEW_LEFT_DETAIL;
	else if (curView == GetRightDetailView())
		return MERGEVIEW_RIGHT_DETAIL;

	// This assert fired when location pane caused refresh.
	// We can't detect location pane activity, so disable the assert.
	//_RPTF0(_CRT_ERROR, "Invalid view pointer!");
	return -1;
}

/**
 * @brief Return active merge edit view (or left one if neither active)
 */
CMergeEditView * CMergeDoc::GetActiveMergeView()
{
	CView * pActiveView = GetParentFrame()->GetActiveView();
	CMergeEditView * pMergeEditView = dynamic_cast<CMergeEditView *>(pActiveView);
	if (!pMergeEditView)
		pMergeEditView = GetLeftView(); // default to left view (in case some location or detail view active)
	return pMergeEditView;
}

void CMergeDoc::SetUnpacker(PackingInfo * infoNewHandler)
{
	if (infoNewHandler)
	{
		*m_pInfoUnpacker = *infoNewHandler;
	}
}

void CMergeDoc::SetPrediffer(PrediffingInfo * infoPrediffer)
{
	m_diffWrapper.SetPrediffer(infoPrediffer);
}
void CMergeDoc::GetPrediffer(PrediffingInfo * infoPrediffer)
{
	m_diffWrapper.GetPrediffer(infoPrediffer);
}

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc serialization

void CMergeDoc::Serialize(CArchive& ar)
{
	ASSERT(0); // we do not use CDocument serialization
}

/////////////////////////////////////////////////////////////////////////////
// CMergeDoc commands

/**
 * @brief Save an editor text buffer to a file for prediffing (make UCS-2LE if appropriate)
 *
 * @note 
 * original file is Ansi : 
 *   buffer  -> save as Ansi -> Ansi plugins -> diffutils
 * original file is Unicode (UCS2-LE, UCS2-BE, UTF-8) :
 *   buffer  -> save as UCS2-LE -> Unicode plugins -> convert to UTF-8 -> diffutils
 * (the plugins are optional, not the conversion)
 * @todo Show SaveToFile() errors?
 */
static void SaveBuffForDiff(CDiffTextBuffer & buf, CDiffTextBuffer & buf2, LPCTSTR filepath)
{
	ASSERT(buf.m_nSourceEncoding == buf.m_nDefaultEncoding);  
	int orig_codepage = buf.getCodepage();
	ucr::UNICODESET orig_unicoding = buf.getUnicoding();

	// If file was in Unicode
	if ((orig_unicoding != ucr::NONE) || (buf2.getUnicoding() != ucr::NONE))
	{
	// we subvert the buffer's memory of the original file encoding
		buf.setUnicoding(ucr::UCS2LE);  // write as UCS-2LE (for preprocessing)
		buf.setCodepage(0); // should not matter
	}

	// and we don't repack the file
	PackingInfo * tempPacker = NULL;

	// write buffer out to temporary file
	String sError;
	int retVal = buf.SaveToFile(filepath, TRUE, sError, tempPacker,
		CRLF_STYLE_AUTOMATIC, FALSE);

	// restore memory of encoding of original file
	buf.setUnicoding(orig_unicoding);
	buf.setCodepage(orig_codepage);
}

/**
 * @brief Save files to temp files & compare again.
 *
 * @param bBinary [in,out] [in] If TRUE, compare two binary files
 * [out] If TRUE binary file was detected.
 * @param bIdentical [out] If TRUE files were identical
 * @param bForced [in] If TRUE, suppressing is ignored and rescan
 * is done always
 * @return Tells if rescan was successfully, was suppressed, or
 * error happened
 * If this code is OK, Rescan has detached the views temporarily
 * (positions of cursors have been lost)
 * @note Rescan() ALWAYS compares temp files. Actual user files are not
 * touched by Rescan().
 * @sa CDiffWrapper::RunFileDiff()
 */
int CMergeDoc::Rescan(BOOL &bBinary, BOOL &bIdentical,
		BOOL bForced /* =FALSE */)
{
	DIFFOPTIONS diffOptions = {0};
	DiffFileInfo fileInfo;
	BOOL diffSuccess;
	int nResult = RESCAN_OK;
	FileChange leftFileChanged = FileNoChange;
	FileChange rightFileChanged = FileNoChange;

	if (!bForced)
	{
		if (!m_bEnableRescan)
			return RESCAN_SUPPRESSED;
	}

	if (GetOptionsMgr()->GetBool(OPT_LINEFILTER_ENABLED))
	{
		String regexp = GetMainFrame()->m_pLineFilters->GetAsString();
		m_diffWrapper.SetFilterList(regexp.c_str());
	}
	else
	{
		m_diffWrapper.SetFilterList(_T(""));
	}

	// Check if files have been modified since last rescan
	// Ignore checking in case of scratchpads (empty filenames)
	if (!m_filePaths.GetLeft().empty())
	{
		leftFileChanged = IsFileChangedOnDisk(m_filePaths.GetLeft().c_str(),
				fileInfo, false, 0);
	}
	if (!m_filePaths.GetRight().empty())
	{
		rightFileChanged = IsFileChangedOnDisk(m_filePaths.GetRight().c_str(),
				fileInfo, false, 1);
	}
	m_LastRescan = COleDateTime::GetCurrentTime();

	if (leftFileChanged == FileRemoved)
	{
		CString msg;
		LangFormatString1(msg, IDS_FILE_DISAPPEARED, m_filePaths.GetLeft().c_str());
		AfxMessageBox(msg, MB_ICONWARNING);
		BOOL bSaveResult = FALSE;
		bool ok = DoSaveAs(m_filePaths.GetLeft().c_str(), bSaveResult, 0);
		if (!ok || !bSaveResult)
		{
			return RESCAN_FILE_ERR;
		}
	}
	if (rightFileChanged == FileRemoved)
	{
		CString msg;
		LangFormatString1(msg, IDS_FILE_DISAPPEARED, m_filePaths.GetRight().c_str());
		AfxMessageBox(msg);
		BOOL bSaveResult = FALSE;
		bool ok = DoSaveAs(m_filePaths.GetRight().c_str(), bSaveResult, 1);
		if (!ok || !bSaveResult)
		{
			return RESCAN_FILE_ERR;
		}
	}

	if (leftFileChanged == FileChanged)
	{
		CString msg;
		LangFormatString1(msg, IDS_FILECHANGED_RESCAN, m_filePaths.GetLeft().c_str());
		if (AfxMessageBox(msg, MB_YESNO | MB_ICONWARNING) == IDYES)
		{
			ReloadDoc(0);
			return RESCAN_OK;
		}
	}
	else if (rightFileChanged == FileChanged)
	{
		CString msg;
		LangFormatString1(msg, IDS_FILECHANGED_RESCAN, m_filePaths.GetRight().c_str());
		if (AfxMessageBox(msg, MB_YESNO | MB_ICONWARNING) == IDYES)
		{
			ReloadDoc(1);
			return RESCAN_OK;
		}
	}

	String temp1 = m_tempFiles[0].GetPath();
	if (temp1.empty())
	{
		temp1 = m_tempFiles[0].CreateFromFile(m_filePaths.GetLeft().c_str(),
				_T("lt_wmdoc"));
	}
	String temp2 = m_tempFiles[1].GetPath();
	if (temp2.empty())
	{
		temp2 = m_tempFiles[1].CreateFromFile(m_filePaths.GetRight().c_str(),
				_T("rt_wmdoc"));
	}
	if (temp1.empty() || temp2.empty())
		return RESCAN_TEMP_ERR;

	String tempPath = env_GetTempPath(NULL);
	m_ptBuf[0]->SetTempPath(tempPath);
	m_ptBuf[1]->SetTempPath(tempPath);

	// output buffers to temp files (in UTF-8 if TCHAR=wchar_t or buffer was Unicode)
	if (bBinary == FALSE)
	{
		SaveBuffForDiff(*m_ptBuf[0], *m_ptBuf[1], m_tempFiles[0].GetPath().c_str());
		SaveBuffForDiff(*m_ptBuf[1], *m_ptBuf[0], m_tempFiles[1].GetPath().c_str());
	}

	// Set up DiffWrapper
	m_diffWrapper.SetCreateDiffList(&m_diffList);
	m_diffWrapper.GetOptions(&diffOptions);
	
	// Clear diff list
	m_diffList.Clear();
	m_nCurDiff = -1;
	// Clear moved lines lists
	if (m_diffWrapper.GetDetectMovedBlocks())
		m_diffWrapper.GetMovedLines()->Clear();

	// Set paths for diffing and run diff
	m_diffWrapper.EnablePlugins(GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED));
	m_diffWrapper.SetPaths(m_tempFiles[0].GetPath(), m_tempFiles[1].GetPath(), TRUE);
	m_diffWrapper.SetCompareFiles(m_filePaths.GetLeft(), m_filePaths.GetRight());
	m_diffWrapper.SetCodepage(m_ptBuf[0]->m_encoding.m_unicoding ?
			CP_UTF8 : m_ptBuf[0]->m_encoding.m_codepage);
	diffSuccess = m_diffWrapper.RunFileDiff();

	// Read diff-status
	DIFFSTATUS status;
	m_diffWrapper.GetDiffStatus(&status);
	if (bBinary) // believe caller if we were told these are binaries
		status.bBinaries = TRUE;

	// If comparing whitespaces and
	// other file has EOL before EOF and other not...
	if (status.bLeftMissingNL != status.bRightMissingNL &&
		!diffOptions.nIgnoreWhitespace && !diffOptions.bIgnoreBlankLines)
	{
		// ..lasf DIFFRANGE of file which has EOL must be
		// fixed to contain last line too
		m_diffWrapper.FixLastDiffRange(m_ptBuf[0]->GetLineCount(), m_ptBuf[1]->GetLineCount(),
				status.bRightMissingNL);
	}

	// set identical/diff result as recorded by diffutils
	bIdentical = status.bIdentical;

	// Determine errors and binary file compares
	if (!diffSuccess)
		nResult = RESCAN_FILE_ERR;
	else if (status.bBinaries)
	{
		bBinary = TRUE;
	}
	else
	{
		// Now update views and buffers for ghost lines

		// Prevent displaying views during this update 
		// BTW, this solves the problem of double asserts
		// (during the display of an assert message box, a second assert in one of the 
		//  display functions happens, and hides the first assert)
		m_pView[MERGE_VIEW_LEFT]->DetachFromBuffer();
		m_pView[MERGE_VIEW_RIGHT]->DetachFromBuffer();
		m_pDetailView[0]->DetachFromBuffer();
		m_pDetailView[1]->DetachFromBuffer();

		// Remove blank lines and clear winmerge flags
		// this operation does not change the modified flag
		m_ptBuf[0]->prepareForRescan();
		m_ptBuf[1]->prepareForRescan();

		// Divide diff blocks to match lines.
		if (GetOptionsMgr()->GetBool(OPT_CMP_MATCH_SIMILAR_LINES))
			AdjustDiffBlocks();

		// Analyse diff-list (updating real line-numbers)
		// this operation does not change the modified flag
		PrimeTextBuffers();

		// Apply flags to lines that moved, to differentiate from appeared/disappeared lines
		if (m_diffWrapper.GetDetectMovedBlocks())
			FlagMovedLines(m_diffWrapper.GetMovedLines(), m_ptBuf[0], m_ptBuf[1]);
		
		// After PrimeTextBuffers() we know amount of real diffs
		// (m_nDiffs) and trivial diffs (m_nTrivialDiffs)

		// Identical files are also updated
		if (!m_diffList.HasSignificantDiffs())
			bIdentical = TRUE;

		// just apply some options to the views
		m_pView[MERGE_VIEW_LEFT]->PrimeListWithFile();
		m_pView[MERGE_VIEW_RIGHT]->PrimeListWithFile();
		m_pDetailView[0]->PrimeListWithFile();
		m_pDetailView[1]->PrimeListWithFile();

		// Now buffers data are valid
		m_pView[MERGE_VIEW_LEFT]->ReAttachToBuffer();
		m_pView[MERGE_VIEW_RIGHT]->ReAttachToBuffer();
		m_pDetailView[0]->ReAttachToBuffer();
		m_pDetailView[1]->ReAttachToBuffer();

		m_bEditAfterRescan[0] = false;
		m_bEditAfterRescan[1] = false;
	}

	GetParentFrame()->SetLastCompareResult(m_diffList.GetSignificantDiffs());

	m_pRescanFileInfo[0]->Update(m_filePaths.GetLeft().c_str());
	m_pRescanFileInfo[1]->Update(m_filePaths.GetRight().c_str());

	return nResult;
}

/** @brief Adjust all different lines that were detected as actually matching moved lines */
void CMergeDoc::FlagMovedLines(MovedLines * pMovedLines, CDiffTextBuffer * pBuffer1,
		CDiffTextBuffer * pBuffer2)
{
	int i;
	for (i = 0; i < pBuffer1->GetLineCount(); ++i)
	{
		int j = pMovedLines->LineInBlock(i, MovedLines::SIDE_LEFT);
		if (j != -1)
		{
			TRACE(_T("%d->%d\n"), i, j);
			ASSERT(j>=0);
			// We only flag lines that are already marked as being different
			int apparent = pBuffer1->ComputeApparentLine(i);
			if (pBuffer1->FlagIsSet(apparent, LF_DIFF))
			{
				pBuffer1->SetLineFlag(apparent, LF_MOVED, TRUE, FALSE, FALSE);
			}
		}
	}

	for (i=0; i<pBuffer2->GetLineCount(); ++i)
	{
		int j = pMovedLines->LineInBlock(i, MovedLines::SIDE_RIGHT);
		if (j != -1)
		{
			TRACE(_T("%d->%d\n"), i, j);
			ASSERT(j>=0);
			// We only flag lines that are already marked as being different
			int apparent = pBuffer2->ComputeApparentLine(i);
			if (pBuffer2->FlagIsSet(apparent, LF_DIFF))
			{
				pBuffer2->SetLineFlag(apparent, LF_MOVED, TRUE, FALSE, FALSE);
			}
		}
	}

	// todo: Need to record actual moved information
}

/**
 * @brief Prints (error) message by rescan status.
 *
 * @param nRescanResult [in] Resultcocode from rescan().
 * @param bIdentical [in] Were files identical?.
 * @sa CMergeDoc::Rescan()
 */
void CMergeDoc::ShowRescanError(int nRescanResult, BOOL bIdentical)
{
	// Rescan was suppressed, there is no sensible status
	if (nRescanResult == RESCAN_SUPPRESSED)
		return;

	String s;

	if (nRescanResult == RESCAN_FILE_ERR)
	{
		s = theApp.LoadString(IDS_FILEERROR);
		LogErrorString(s.c_str());
		AfxMessageBox(s.c_str(), MB_ICONSTOP);
		return;
	}

	if (nRescanResult == RESCAN_TEMP_ERR)
	{
		s = theApp.LoadString(IDS_TEMP_FILEERROR);
		LogErrorString(s.c_str());
		AfxMessageBox(s.c_str(), MB_ICONSTOP);
		return;
	}

	// Files are not binaries, but they are identical
	if (bIdentical)
	{
		if (!m_filePaths.GetLeft().empty() && !m_filePaths.GetRight().empty() && 
			m_filePaths.GetLeft() == m_filePaths.GetRight())
		{
			// compare file to itself, a custom message so user may hide the message in this case only
			s = theApp.LoadString(IDS_FILE_TO_ITSELF);
			AfxMessageBox(s.c_str(), MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN, IDS_FILE_TO_ITSELF);
		}
		else
		{
			UINT nFlags = MB_ICONINFORMATION | MB_DONT_DISPLAY_AGAIN;

			if (GetMainFrame()->m_bExitIfNoDiff == MergeCmdLineInfo::Exit)
			{
				// Show the "files are identical" for basic "exit no diff" flag
				// If user don't want to see the message one uses the quiet version
				// of the "exit no diff".
				nFlags &= ~MB_DONT_DISPLAY_AGAIN;
			}

			if (GetMainFrame()->m_bExitIfNoDiff != MergeCmdLineInfo::ExitQuiet)
				LangMessageBox(IDS_FILESSAME, nFlags);

			// Exit application if files are identical.
			if (GetMainFrame()->m_bExitIfNoDiff == MergeCmdLineInfo::Exit ||
				GetMainFrame()->m_bExitIfNoDiff == MergeCmdLineInfo::ExitQuiet)
			{
				GetMainFrame()->PostMessage(WM_COMMAND, ID_APP_EXIT);
			}
		}
	}
}

bool CMergeDoc::Undo()
{
	return false;
}

/**
 * @brief An instance of RescanSuppress prevents rescan during its lifetime
 * (or until its Clear method is called, which ends its effect).
 */
class RescanSuppress
{
public:
	RescanSuppress(CMergeDoc & doc) : m_doc(doc)
	{
		m_bSuppress = true;
		m_bPrev = doc.m_bEnableRescan;
		m_doc.m_bEnableRescan = false;
	}
	void Clear() 
	{
		if (m_bSuppress)
		{
			m_bSuppress = false;
			m_doc.m_bEnableRescan = m_bPrev;
		}
	}
	~RescanSuppress()
	{
		Clear();
	}
private:
	CMergeDoc & m_doc;
	bool m_bPrev;
	bool m_bSuppress;
};

/**
 * @brief Copy all diffs from one side to side.
 * @param [in] srcPane Source side from which diff is copied
 * @param [in] dstPane Destination side
 */
void CMergeDoc::CopyAllList(int srcPane, int dstPane)
{
	CopyMultipleList(srcPane, dstPane, 0, m_diffList.GetSize() - 1);
}

/**
 * @brief Copy range of diffs from one side to side.
 * This function copies given range of differences from side to another.
 * Ignored differences are skipped, and not copied.
 * @param [in] srcPane Source side from which diff is copied
 * @param [in] dstPane Destination side
 * @param [in] firstDiff First diff copied (0-based index)
 * @param [in] lastDiff Last diff copied (0-based index)
 */
void CMergeDoc::CopyMultipleList(int srcPane, int dstPane, int firstDiff, int lastDiff)
{
#ifdef _DEBUG
	if (firstDiff > lastDiff)
		_RPTF0(_CRT_ERROR, "Invalid diff range (firstDiff > lastDiff)!");
	if (firstDiff < 0)
		_RPTF0(_CRT_ERROR, "Invalid diff range (firstDiff < 0)!");
	if (lastDiff > m_diffList.GetSize() - 1)
		_RPTF0(_CRT_ERROR, "Invalid diff range (lastDiff < diffcount)!");
#endif

	lastDiff = min(m_diffList.GetSize() - 1, lastDiff);
	firstDiff = max(0, firstDiff);
	if (firstDiff > lastDiff)
		return;
	
	RescanSuppress suppressRescan(*this);

	// Note we don't care about m_nDiffs count to become zero,
	// because we don't rescan() so it does not change

	bool bGroupWithPrevious = false;
	if (!ListCopy(srcPane, dstPane, lastDiff, bGroupWithPrevious))
		return; // sync failure

	// copy from bottom up is more efficient
	for (int i = lastDiff - 1; i >= firstDiff; --i)
	{
		if (m_diffList.IsDiffSignificant(i))
		{
			// Group merge with previous (merge undo data to one action)
			bGroupWithPrevious = true;
			if (!ListCopy(srcPane, dstPane, i, bGroupWithPrevious))
				return; // sync failure
		}
	}

	suppressRescan.Clear(); // done suppress Rescan
	FlushAndRescan();
}

/**
 * @brief Sanity check difference.
 *
 * Checks that lines in difference are inside difference in both files.
 * If file is edited, lines added or removed diff lines get out of sync and
 * merging fails miserably.
 *
 * @param [in] dr Difference to check.
 * @return TRUE if difference lines match, FALSE otherwise.
 */
bool CMergeDoc::SanityCheckDiff(DIFFRANGE dr) const
{
	const int cd_dbegin = dr.dbegin0;
	const int cd_dend = dr.dend0;

	// Must ensure line number is in range before getting line flags
	if (cd_dend >= m_ptBuf[0]->GetLineCount())
		return false;
	DWORD dwLeftFlags = m_ptBuf[0]->GetLineFlags(cd_dend);

	// Must ensure line number is in range before getting line flags
	if (cd_dend >= m_ptBuf[1]->GetLineCount())
		return false;
	DWORD dwRightFlags = m_ptBuf[1]->GetLineFlags(cd_dend);

	// Optimization - check last line first so we don't need to
	// check whole diff for obvious cases
	if (!(dwLeftFlags & LF_WINMERGE_FLAGS) ||
		!(dwRightFlags & LF_WINMERGE_FLAGS))
	{
		return false;
	}

	for (int line = cd_dbegin; line < cd_dend; line++)
	{
		dwLeftFlags = m_ptBuf[0]->GetLineFlags(cd_dend);
		dwRightFlags = m_ptBuf[1]->GetLineFlags(cd_dend);
		if (!(dwLeftFlags & LF_WINMERGE_FLAGS) ||
			!(dwRightFlags & LF_WINMERGE_FLAGS))
		{
			return false;
		}
	}
	return true;
}

/**
 * @brief Copy selected (=current) difference from from side to side.
 * @param [in] srcPane Source side from which diff is copied
 * @param [in] dstPane Destination side
 * @param [in] nDiff Diff to copy, if -1 function determines it.
 * @param [in] bGroupWithPrevious Adds diff to same undo group with
 * @return true if ok, false if sync failure & need to abort copy
 * previous action (allows one undo for copy all)
 */
bool CMergeDoc::ListCopy(int srcPane, int dstPane, int nDiff /* = -1*/,
		bool bGroupWithPrevious /*= false*/)
{
	// suppress Rescan during this method
	// (Not only do we not want to rescan a lot of times, but
	// it will wreck the line status array to rescan as we merge)
	RescanSuppress suppressRescan(*this);

	// If diff-number not given, determine it from active view
	if (nDiff == -1)
	{
		nDiff = GetCurrentDiff();

		// No current diff, but maybe cursor is in diff?
		if (nDiff == -1 && (m_pView[srcPane]->IsCursorInDiff() ||
			m_pView[dstPane]->IsCursorInDiff()))
		{
			// Find out diff under cursor
			CPoint ptCursor;
			int nActiveViewIndexType = GetActiveMergeViewIndexType();
			if (nActiveViewIndexType == MERGEVIEW_LEFT || nActiveViewIndexType == MERGEVIEW_RIGHT)
				ptCursor = m_pView[nActiveViewIndexType]->GetCursorPos();
			else if (nActiveViewIndexType == MERGEVIEW_LEFT_DETAIL ||
					nActiveViewIndexType == MERGEVIEW_RIGHT_DETAIL)
			{
				ptCursor = m_pView[nActiveViewIndexType - MERGEVIEW_LEFT_DETAIL]->GetCursorPos();
			}
			nDiff = m_diffList.LineToDiff(ptCursor.y);
		}
	}

	if (nDiff != -1)
	{
		DIFFRANGE cd;
		VERIFY(m_diffList.GetDiff(nDiff, cd));
		CDiffTextBuffer& sbuf = *m_ptBuf[srcPane];
		CDiffTextBuffer& dbuf = *m_ptBuf[dstPane];
		BOOL bSrcWasMod = sbuf.IsModified();
		const int cd_dbegin = srcPane == 0 ? cd.dbegin0 : cd.dbegin1;
		const int cd_dend = srcPane == 0 ? cd.dend0 : cd.dend1;
		const int cd_blank = srcPane == 0 ? cd.blank0 : cd.blank1;
		bool bInSync = SanityCheckDiff(cd);

		if (!bInSync)
		{
			LangMessageBox(IDS_VIEWS_OUTOFSYNC, MB_ICONSTOP);
			return false; // abort copying
		}

		// If we remove whole diff from current view, we must fix cursor
		// position first. Normally we would move to end of previous line,
		// but we want to move to begin of that line for usability.
		if ((cd.op == OP_LEFTONLY && dstPane == 0) ||
			(cd.op == OP_RIGHTONLY && dstPane == 1))
		{
			CCrystalTextView * pCurView = m_pView[dstPane];
			CPoint currentPos = pCurView->GetCursorPos();
			currentPos.x = 0;
			if (currentPos.y > 0)
				currentPos.y--;
			pCurView->SetCursorPos(currentPos);
		}

		// if the current diff contains missing lines, remove them from both sides
		int limit = cd_dend;

		// curView is the view which is changed, so the opposite of the source view
		CCrystalTextView* dstView = m_pView[dstPane];

		dbuf.BeginUndoGroup(bGroupWithPrevious);
		if (cd_blank>=0)
		{
			// text was missing, so delete rest of lines on both sides
			// delete only on destination side since rescan will clear the other side
			if (cd_dend + 1 < dbuf.GetLineCount())
			{
				dbuf.DeleteText(dstView, cd_blank, 0, cd_dend+1, 0, CE_ACTION_MERGE);
			}
			else
			{
				// To removing EOL chars of last line, deletes from the end of the line (cd_blank - 1).
				ASSERT(cd_blank > 0);
				dbuf.DeleteText(dstView, cd_blank-1, dbuf.GetLineLength(cd_blank-1), cd_dend, dbuf.GetLineLength(cd_dend), CE_ACTION_MERGE);
			}

			limit=cd_blank-1;
			dbuf.FlushUndoGroup(dstView);
			dbuf.BeginUndoGroup(TRUE);
		}

		CString strLine;

		// copy the selected text over
		for (int i=cd_dbegin; i <= limit; i++)
		{
			// text exists on other side, so just replace
			sbuf.GetFullLine(i, strLine);
			dbuf.ReplaceFullLine(dstView, i, strLine, CE_ACTION_MERGE);
			dbuf.FlushUndoGroup(dstView);
			dbuf.BeginUndoGroup(TRUE);
		}
		dbuf.FlushUndoGroup(dstView);

		// remove the diff
		SetCurrentDiff(-1);

		// reset the mod status of the source view because we do make some
		// changes, but none that concern the source text
		sbuf.SetModified(bSrcWasMod);
	}

	suppressRescan.Clear(); // done suppress Rescan
	FlushAndRescan();
	return true;
}

/**
 * @brief Save file with new filename.
 *
 * This function is called by CMergeDoc::DoSave() or CMergeDoc::DoSAveAs()
 * to save file with new filename. CMergeDoc::DoSave() calls if saving with
 * normal filename fails, to let user choose another filename/location.
 * Also, if file is unnamed file (e.g. scratchpad) then it must be saved
 * using this function.
 * @param [in, out] strPath 
 * - [in] : Initial path shown to user
 * - [out] : Path to new filename if saving succeeds
 * @param [in, out] nSaveResult 
 * - [in] : Statuscode telling why we ended up here. Maybe the result of
 * previous save.
 * - [out] : Statuscode of this saving try
 * @param [in, out] sError Error string from lower level saving code
 * @param [in] nBuffer Buffer we are saving
 * @return FALSE as long as the user is not satisfied. Calling function
 * should not continue until TRUE is returned.
 * @sa CMergeDoc::DoSave()
 * @sa CMergeDoc::DoSaveAs()
 * @sa CMergeDoc::CDiffTextBuffer::SaveToFile()
 */
bool CMergeDoc::TrySaveAs(CString &strPath, int &nSaveResult, String & sError,
	int nBuffer, PackingInfo * pInfoTempUnpacker)
{
	CString s;
	CString strSavePath; // New path for next saving try
	UINT titleid = 0;
	bool result = true;
	int answer = IDOK; // Set default we use for scratchpads
	int nActiveViewIndexType = GetActiveMergeViewIndexType();

	if (nActiveViewIndexType == -1)
	{
		// We don't have valid view active, but still tried to save.
		// We don't know which file to save, so just cancel.
		// Possible origin in location pane?
		_RPTF0(_CRT_ERROR, "Save request from invalid view!");
		nSaveResult = SAVE_CANCELLED;
		return TRUE;
	}

	HWND parent = m_pView[nActiveViewIndexType]->GetSafeHwnd();

	// We shouldn't get here if saving is succeed before
	ASSERT(nSaveResult != SAVE_DONE);

	// Select message based on reason function called
	if (nSaveResult == SAVE_PACK_FAILED)
	{
		LangFormatString2(s, IDS_FILEPACK_FAILED_LEFT + nBuffer,
			strPath, pInfoTempUnpacker->pluginName.c_str());
		// replace the unpacker with a "do nothing" unpacker
		pInfoTempUnpacker->Initialize(PLUGIN_MANUAL);
	}
	else
	{
		LangFormatString2(s, IDS_FILESAVE_FAILED, strPath, sError.c_str());
	}

	// SAVE_NO_FILENAME is temporarily used for scratchpad.
	// So don't ask about saving in that case.
	if (nSaveResult != SAVE_NO_FILENAME)
		answer = AfxMessageBox(s, MB_OKCANCEL | MB_ICONWARNING);

	switch (answer)
	{
	case IDOK:
		if (nBuffer == 0)
			titleid = IDS_SAVE_LEFT_AS;
		else
			titleid = IDS_SAVE_RIGHT_AS;

		if (SelectFile(parent, s, strPath, titleid, NULL, FALSE))
		{
			CDiffTextBuffer *pBuffer = m_ptBuf[nBuffer];
			strSavePath = s;
			nSaveResult = pBuffer->SaveToFile(strSavePath, FALSE, sError,
				pInfoTempUnpacker);

			if (nSaveResult == SAVE_DONE)
			{
				// We are saving scratchpad (unnamed file)
				if (strPath.IsEmpty())
				{
					m_nBufferType[nBuffer] = BUFFER_UNNAMED_SAVED;
					m_strDesc[nBuffer].erase();
				}
					
				strPath = strSavePath;
				UpdateHeaderPath(nBuffer);
			}
			else
				result = false;
		}
		else
			nSaveResult = SAVE_CANCELLED;
		break;

	case IDCANCEL:
		nSaveResult = SAVE_CANCELLED;
		break;
	}
	return result;
}

/**
 * @brief Save file creating backups etc.
 *
 * Safe top-level file saving function. Checks validity of given path.
 * Creates backup file if wanted to. And if saving to given path fails,
 * allows user to select new location/name for file.
 * @param [in] szPath Path where to save including filename. Can be
 * empty/NULL if new file is created (scratchpad) without filename.
 * @param [out] bSaveSuccess Will contain information about save success with
 * the original name (to determine if file statuses should be changed)
 * @param [in] nBuffer Index (0-based) of buffer to save
 * @return Tells if caller can continue (no errors happened)
 * @note Return value does not tell if SAVING succeeded. Caller must
 * Check value of bSaveSuccess parameter after calling this function!
 * @note If CMainFrame::m_strSaveAsPath is non-empty, file is saved
 * to directory it points to. If m_strSaveAsPath contains filename,
 * that filename is used.
 * @sa CMergeDoc::TrySaveAs()
 * @sa CMainFrame::CheckSavePath()
 * @sa CMergeDoc::CDiffTextBuffer::SaveToFile()
 */
bool CMergeDoc::DoSave(LPCTSTR szPath, BOOL &bSaveSuccess, int nBuffer)
{
	DiffFileInfo fileInfo;
	CString strSavePath(szPath);
	FileChange fileChanged;
	BOOL bApplyToAll = FALSE;	
	int nRetVal = -1;

	fileChanged = IsFileChangedOnDisk(szPath, fileInfo, true, nBuffer);
	if (fileChanged == FileChanged)
	{
		CString msg;
		LangFormatString1(msg, IDS_FILECHANGED_ONDISK, szPath);
		if (AfxMessageBox(msg, MB_ICONWARNING | MB_YESNO) == IDNO)
		{
			bSaveSuccess = SAVE_CANCELLED;
			return true;
		}		
	}

	// use a temp packer
	// first copy the m_pInfoUnpacker
	// if an error arises during packing, change and take a "do nothing" packer
	PackingInfo infoTempUnpacker = *m_pInfoUnpacker;

	bSaveSuccess = FALSE;
	
	// Check third arg possibly given from command-line
	if (!GetMainFrame()->m_strSaveAsPath.IsEmpty())
	{
		if (paths_DoesPathExist(GetMainFrame()->m_strSaveAsPath) == IS_EXISTING_DIR)
		{
			// third arg was a directory, so get append the filename
			String sname;
			SplitFilename(szPath, 0, &sname, 0);
			strSavePath = GetMainFrame()->m_strSaveAsPath;
			if (GetMainFrame()->m_strSaveAsPath.Right(1) != _T('\\'))
				strSavePath += _T('\\');
			strSavePath += sname.c_str();
		}
		else
			strSavePath = GetMainFrame()->m_strSaveAsPath;	
	}

	nRetVal = GetMainFrame()->HandleReadonlySave(strSavePath, FALSE, bApplyToAll);
	if (nRetVal == IDCANCEL)
		return false;

	if (!GetMainFrame()->CreateBackup(FALSE, strSavePath))
		return false;

	// FALSE as long as the user is not satisfied
	// TRUE if saving succeeds, even with another filename, or if the user cancels
	bool result = false;
	// the error code from the latest save operation, 
	// or SAVE_DONE when the save succeeds
	// TODO: Shall we return this code in addition to bSaveSuccess ?
	int nSaveErrorCode = SAVE_DONE;
	CDiffTextBuffer *pBuffer = m_ptBuf[nBuffer];

	// Assume empty filename means Scratchpad (unnamed file)
	// Todo: This is not needed? - buffer type check should be enough
	if (strSavePath.IsEmpty())
		nSaveErrorCode = SAVE_NO_FILENAME;

	// Handle unnamed buffers
	if (m_nBufferType[nBuffer] == BUFFER_UNNAMED)
			nSaveErrorCode = SAVE_NO_FILENAME;

	String sError;
	if (nSaveErrorCode == SAVE_DONE)
		// We have a filename, just try to save
		nSaveErrorCode = pBuffer->SaveToFile(strSavePath, FALSE, sError, &infoTempUnpacker);

	if (nSaveErrorCode != SAVE_DONE)
	{
		// Saving failed, user may save to another location if wants to
		do
			result = TrySaveAs(strSavePath, nSaveErrorCode, sError, nBuffer, &infoTempUnpacker);
		while (!result);
	}

	// Saving succeeded with given/selected filename
	if (nSaveErrorCode == SAVE_DONE)
	{
		// Preserve file times if user wants to
		if (GetOptionsMgr()->GetBool(OPT_PRESERVE_FILETIMES))
		{
			fileInfo.SetFile((LPCTSTR)strSavePath);
			files_UpdateFileTime(fileInfo);
		}

		m_ptBuf[nBuffer]->SetModified(FALSE);
		m_pSaveFileInfo[nBuffer]->Update((LPCTSTR)strSavePath);
		m_pRescanFileInfo[nBuffer]->Update(m_filePaths.GetPath(nBuffer).c_str());
		m_filePaths.SetPath(nBuffer, strSavePath);
		UpdateHeaderPath(nBuffer);
		bSaveSuccess = TRUE;
		result = true;
	}
	else if (nSaveErrorCode == SAVE_CANCELLED)
	{
		// User cancelled current operation, lets do what user wanted to do
		result = false;
	}
	return result;
}

/**
 * @brief Save file with different filename.
 *
 * Safe top-level file saving function. Asks user to select filename
 * and path. Does not create backups.
 * @param [in] szPath Path where to save including filename. Can be
 * empty/NULL if new file is created (scratchpad) without filename.
 * @param [out] bSaveSuccess Will contain information about save success with
 * the original name (to determine if file statuses should be changed)
 * @param [in] nBuffer Index (0-based) of buffer to save
 * @return Tells if caller can continue (no errors happened)
 * @note Return value does not tell if SAVING succeeded. Caller must
 * Check value of bSaveSuccess parameter after calling this function!
 * @sa CMergeDoc::TrySaveAs()
 * @sa CMainFrame::CheckSavePath()
 * @sa CMergeDoc::CDiffTextBuffer::SaveToFile()
 */
bool CMergeDoc::DoSaveAs(LPCTSTR szPath, BOOL &bSaveSuccess, int nBuffer)
{
	CString strSavePath(szPath);

	// use a temp packer
	// first copy the m_pInfoUnpacker
	// if an error arises during packing, change and take a "do nothing" packer
	PackingInfo infoTempUnpacker = *m_pInfoUnpacker;

	bSaveSuccess = FALSE;
	// FALSE as long as the user is not satisfied
	// TRUE if saving succeeds, even with another filename, or if the user cancels
	bool result = false;
	// the error code from the latest save operation, 
	// or SAVE_DONE when the save succeeds
	// TODO: Shall we return this code in addition to bSaveSuccess ?
	int nSaveErrorCode = SAVE_DONE;

	// Use SAVE_NO_FILENAME to prevent asking about error
	nSaveErrorCode = SAVE_NO_FILENAME;

	// Loop until user succeeds saving or cancels
	String sError;
	do
		result = TrySaveAs(strSavePath, nSaveErrorCode, sError, nBuffer, &infoTempUnpacker);
	while (!result);

	// Saving succeeded with given/selected filename
	if (nSaveErrorCode == SAVE_DONE)
	{
		m_pSaveFileInfo[nBuffer]->Update((LPCTSTR)strSavePath);
		m_pRescanFileInfo[nBuffer]->Update(m_filePaths.GetPath(nBuffer).c_str());
		m_filePaths.SetPath(nBuffer, strSavePath);
		UpdateHeaderPath(nBuffer);
		bSaveSuccess = TRUE;
		result = true;
	}
	return result;
}

/**
 * @brief Get left->right info for a moved line (apparent line number)
 */
int CMergeDoc::RightLineInMovedBlock(int apparentLeftLine)
{
	if (!(m_ptBuf[0]->GetLineFlags(apparentLeftLine) & LF_MOVED))
		return -1;

	int realLeftLine = m_ptBuf[0]->ComputeRealLine(apparentLeftLine);
	int realRightLine = -1;
	if (m_diffWrapper.GetDetectMovedBlocks())
	{
		realRightLine = m_diffWrapper.GetMovedLines()->LineInBlock(realLeftLine,
				MovedLines::SIDE_LEFT);
	}
	if (realRightLine != -1)
		return m_ptBuf[1]->ComputeApparentLine(realRightLine);
	else
		return -1;
}

/**
 * @brief Get right->left info for a moved line (apparent line number)
 */
int CMergeDoc::LeftLineInMovedBlock(int apparentRightLine)
{
	if (!(m_ptBuf[1]->GetLineFlags(apparentRightLine) & LF_MOVED))
		return -1;

	int realRightLine = m_ptBuf[1]->ComputeRealLine(apparentRightLine);
	int realLeftLine = -1;
	if (m_diffWrapper.GetDetectMovedBlocks())
	{
		realLeftLine = m_diffWrapper.GetMovedLines()->LineInBlock(realRightLine,
				MovedLines::SIDE_RIGHT);
	}
	if (realLeftLine != -1)
		return m_ptBuf[0]->ComputeApparentLine(realLeftLine);
	else
		return -1;
}

/**
 * @brief Save modified documents.
 * This function asks if user wants to save modified documents. We also
 * allow user to cancel the closing.
 *
 * There is a special trick avoiding showing two save-dialogs, as MFC framework
 * sometimes calls this function twice. We use static counter for these calls
 * and if we already have saving in progress (counter == 1) we skip the new
 * saving dialog.
 *
 * @return TRUE if docs are closed, FALSE if closing is cancelled.
 */
BOOL CMergeDoc::SaveModified()
{
	static int counter;
	++counter;
	if (counter > 1)
		return FALSE;

	if (PromptAndSaveIfNeeded(TRUE))
	{
		counter = 0;
		return TRUE;
	}
	else
	{
		counter = 0;
		return FALSE;
	}
}

/**
 * @brief Sets the current difference.
 * @param [in] nDiff Difference to set as current difference.
 */
void CMergeDoc::SetCurrentDiff(int nDiff)
{
	if (nDiff >= 0 && nDiff <= m_diffList.LastSignificantDiff())
		m_nCurDiff = nDiff;
	else
		m_nCurDiff = -1;
}

/**
 * @brief Unescape control characters.
 * @param [in,out] s Line of text excluding eol chars.
 */
static void UnescapeControlChars(CString &s)
{
	LPTSTR p = s.LockBuffer();
	LPTSTR q = p;
	while ((*p = *q) != _T('\0'))
	{
		++q;
		// Is it the leadin character?
		if (*p == _T('\x0F'))
		{
			LPTSTR r = q;
			// Expect a hexadecimal number...
			long ordinal = (TCHAR)_tcstol(q, &r, 16);
			// ...followed by the leadout character.
			if (*r == _T('\\'))
			{
				*p = (TCHAR)ordinal;
				q = r + 1;
			}
		}
		++p;
	}
	s.ReleaseBuffer(p - s);
}

/**
 * @brief Take care of rescanning document.
 * 
 * Update view and restore cursor and scroll position after
 * rescanning document.
 * @param [in] bForced If TRUE rescan cannot be suppressed
 */
void CMergeDoc::FlushAndRescan(BOOL bForced /* =FALSE */)
{
	// Ignore suppressing when forced rescan
	if (!bForced)
		if (!m_bEnableRescan) return;

	WaitStatusCursor waitstatus(IDS_STATUS_RESCANNING);

	int nActiveViewIndexType = GetActiveMergeViewIndexType();

	// store cursors and hide caret
	m_pView[MERGE_VIEW_LEFT]->PushCursors();
	m_pView[MERGE_VIEW_RIGHT]->PushCursors();
	m_pDetailView[0]->PushCursors();
	m_pDetailView[1]->PushCursors();
	if (nActiveViewIndexType == MERGEVIEW_LEFT || nActiveViewIndexType == MERGEVIEW_RIGHT)
		m_pView[nActiveViewIndexType]->HideCursor();

	BOOL bBinary = FALSE;
	BOOL bIdentical = FALSE;
	int nRescanResult = Rescan(bBinary, bIdentical, bForced);

	// restore cursors and caret
	m_pView[MERGE_VIEW_LEFT]->PopCursors();
	m_pView[MERGE_VIEW_RIGHT]->PopCursors();
	m_pDetailView[0]->PopCursors();
	m_pDetailView[1]->PopCursors();
	if (nActiveViewIndexType == MERGEVIEW_LEFT || nActiveViewIndexType == MERGEVIEW_RIGHT)
		m_pView[nActiveViewIndexType]->ShowCursor();

	// because of ghostlines, m_nTopLine may differ just after Rescan
	// scroll both views to the same top line
	CMergeEditView * fixedView = m_pView[MERGE_VIEW_LEFT];
	if (nActiveViewIndexType == MERGEVIEW_LEFT || nActiveViewIndexType == MERGEVIEW_RIGHT)
		// only one view needs to scroll so do not scroll the active view
		fixedView = m_pView[nActiveViewIndexType];
	fixedView->UpdateSiblingScrollPos(FALSE);

	// make sure we see the cursor from the curent view
	if (nActiveViewIndexType == MERGEVIEW_LEFT || nActiveViewIndexType == MERGEVIEW_RIGHT)
		m_pView[nActiveViewIndexType]->EnsureVisible(m_pView[nActiveViewIndexType]->GetCursorPos());

	// scroll both diff views to the same top line
	CMergeDiffDetailView * fixedDetailView = m_pDetailView[0];
	if (nActiveViewIndexType == MERGEVIEW_LEFT_DETAIL || nActiveViewIndexType == MERGEVIEW_RIGHT_DETAIL)
		// only one view needs to scroll so do not scroll the active view
		fixedDetailView = m_pDetailView[nActiveViewIndexType - MERGEVIEW_LEFT_DETAIL];
	fixedDetailView->UpdateSiblingScrollPos(FALSE);

	// Refresh display
	UpdateAllViews(NULL);

	// Show possible error after updating screen
	if (nRescanResult != RESCAN_SUPPRESSED)
		ShowRescanError(nRescanResult, bIdentical);
	m_LastRescan = COleDateTime::GetCurrentTime();
}

/**
 * @brief Saves both files
 */
void CMergeDoc::OnFileSave() 
{
	// We will need to know if either of the originals actually changed
	// so we know whether to update the diff status
	bool bLChangedOriginal = false;
	bool bRChangedOriginal = false;

	if (m_ptBuf[0]->IsModified() && !m_ptBuf[0]->GetReadOnly())
	{
		// (why we don't use return value of DoSave)
		// DoSave will return TRUE if it wrote to something successfully
		// but we have to know if it overwrote the original file
		BOOL bSaveOriginal = FALSE;
		DoSave(m_filePaths.GetLeft().c_str(), bSaveOriginal, 0);
		if (bSaveOriginal)
			bLChangedOriginal = true;
	}

	if (m_ptBuf[1]->IsModified() && !m_ptBuf[1]->GetReadOnly())
	{
		// See comments above for left case
		BOOL bSaveOriginal = FALSE;
		DoSave(m_filePaths.GetRight().c_str(), bSaveOriginal, 1);
		if (bSaveOriginal)
			bRChangedOriginal = true;
	}

	// If either of the actual source files being compared was changed
	// we need to update status in the dir view
	if (bLChangedOriginal || bRChangedOriginal)
	{
		// If DirDoc contains diffs
		if (m_pDirDoc->HasDiffs())
		{
			if (m_bEditAfterRescan[0] || m_bEditAfterRescan[1])
				FlushAndRescan(FALSE);

			bool bIdentical = !m_diffList.HasSignificantDiffs(); // True if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_filePaths, m_diffList.GetSignificantDiffs(),
					m_nTrivialDiffs, bIdentical);
		}
	}
}

/**
 * @brief Saves left-side file
 */
void CMergeDoc::OnFileSaveLeft()
{
	BOOL bLSaveSuccess = FALSE;
	BOOL bLModified = FALSE;

	if (m_ptBuf[0]->IsModified() && !m_ptBuf[0]->GetReadOnly())
	{
		bLModified = TRUE;
		DoSave(m_filePaths.GetLeft().c_str(), bLSaveSuccess, 0);
	}

	// If file were modified and saving succeeded,
	// update status on dir view
	if (bLModified && bLSaveSuccess)
	{
		// If DirDoc contains compare results
		if (m_pDirDoc->HasDiffs())
		{
			if (m_bEditAfterRescan[0] || m_bEditAfterRescan[1])
				FlushAndRescan(FALSE);

			bool bIdentical = !m_diffList.HasSignificantDiffs(); // True if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_filePaths, m_diffList.GetSignificantDiffs(),
					m_nTrivialDiffs, bIdentical);
		}
	}
}

/**
 * @brief Saves right-side file
 */
void CMergeDoc::OnFileSaveRight()
{
	BOOL bRSaveSuccess = FALSE;
	BOOL bRModified = FALSE;

	if (m_ptBuf[1]->IsModified() && !m_ptBuf[1]->GetReadOnly())
	{
		bRModified = TRUE;
		DoSave(m_filePaths.GetRight().c_str(), bRSaveSuccess, 1);
	}

	// If file were modified and saving succeeded,
	// update status on dir view
	if (bRModified && bRSaveSuccess)
	{
		// If DirDoc contains compare results
		if (m_pDirDoc->HasDiffs())
		{
			if (m_bEditAfterRescan[0] || m_bEditAfterRescan[1])
				FlushAndRescan(false);

			bool bIdentical = !m_diffList.HasSignificantDiffs(); // True if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_filePaths, m_diffList.GetSignificantDiffs(),
					m_nTrivialDiffs, bIdentical);
		}
	}
}

/**
 * @brief Saves left-side file with name asked
 */
void CMergeDoc::OnFileSaveAsLeft()
{
	BOOL bSaveResult = FALSE;
	DoSaveAs(m_filePaths.GetLeft().c_str(), bSaveResult, 0);
}

/**
 * @brief Saves right-side file with name asked
 */
void CMergeDoc::OnFileSaveAsRight()
{
	BOOL bSaveResult = FALSE;
	DoSaveAs(m_filePaths.GetRight().c_str(), bSaveResult, 1);
}

/**
 * @brief Update diff-number pane text in file compare.
 * The diff number pane shows selected difference/amount of differences when
 * there is difference selected. If there is no difference selected, then
 * the panel shows amount of differences. If there are ignored differences,
 * those are not count into numbers.
 * @param [in] pCmdUI UI component to update.
 */
void CMergeDoc::OnUpdateStatusNum(CCmdUI* pCmdUI) 
{
	TCHAR sIdx[32];
	TCHAR sCnt[32];
	String s;
	const int nDiffs = m_diffList.GetSignificantDiffs();
	
	// Files are identical - show text "Identical"
	if (nDiffs <= 0)
		s = theApp.LoadString(IDS_IDENTICAL);
	
	// There are differences, but no selected diff
	// - show amount of diffs
	else if (GetCurrentDiff() < 0)
	{
		s = theApp.LoadString(nDiffs == 1 ? IDS_1_DIFF_FOUND : IDS_NO_DIFF_SEL_FMT);
		string_replace(s, _T("%1"), _itot(nDiffs, sCnt, 10));
	}
	
	// There are differences and diff selected
	// - show diff number and amount of diffs
	else
	{
		s = theApp.LoadString(IDS_DIFF_NUMBER_STATUS_FMT);
		const int signInd = m_diffList.GetSignificantIndex(GetCurrentDiff());
		string_replace(s, _T("%1"), _itot(signInd + 1, sIdx, 10));
		string_replace(s, _T("%2"), _itot(nDiffs, sCnt, 10));
	}
	pCmdUI->SetText(s.c_str());
}

/**
 * @brief Build the diff array and prepare buffers accordingly (insert ghost lines, set WinMerge flags)
 *
 * @note Buffers may have different length after PrimeTextBuffers. Indeed, no
 * synchronization is needed after the last line. So no ghost line will be created
 * to face an ignored difference in the last line (typically : 'ignore blank lines' 
 * + empty last line on one side).
 * If you fell that different length buffers are really strange, CHANGE FIRST
 * the last diff to take into account the empty last line.
 */
void CMergeDoc::PrimeTextBuffers()
{
	SetCurrentDiff(-1);
	m_nTrivialDiffs = 0;
	int nDiff;
	int nDiffCount = m_diffList.GetSize();

	// walk the diff list and calculate numbers of extra lines to add
	int LeftExtras = 0;   // extra lines added to left view
	int RightExtras = 0;   // extra lines added to right view
	m_diffList.GetExtraLinesCounts(LeftExtras, RightExtras);

	// resize m_aLines once for each view
	UINT lcount0 = m_ptBuf[0]->GetLineCount();
	UINT lcount1 = m_ptBuf[1]->GetLineCount();
	UINT lcount0new = lcount0 + LeftExtras;
	UINT lcount1new = lcount1 + RightExtras;
// this ASSERT may be false because of empty last line (see function's note)
//	ASSERT(lcount0new == lcount1new);
	m_ptBuf[0]->m_aLines.SetSize(lcount0new);
	m_ptBuf[1]->m_aLines.SetSize(lcount1new);

	// walk the diff list backward, move existing lines to proper place,
	// add ghost lines, and set flags
	for (nDiff = nDiffCount - 1; nDiff >= 0; nDiff --)
	{
		DIFFRANGE curDiff;
		VERIFY(m_diffList.GetDiff(nDiff, curDiff));

		// move matched lines after curDiff
		int nline0 = lcount0 - curDiff.end0 - 1; // #lines on left after current diff
		int nline1 = lcount1 - curDiff.end1 - 1; // #lines on right after current diff
		// Matched lines should really match...
		// But matched lines after last diff may differ because of empty last line (see function's note)
		if (nDiff < nDiffCount - 1)
			ASSERT(nline0 == nline1);
		// Move all lines after current diff down as far as needed
		// for any ghost lines we're about to insert
		m_ptBuf[0]->MoveLine(curDiff.end0+1, lcount0-1, lcount0new-nline0);
		m_ptBuf[1]->MoveLine(curDiff.end1+1, lcount1-1, lcount1new-nline1);
		lcount0new -= nline0;
		lcount1new -= nline1;
		lcount0 -= nline0;
		lcount1 -= nline1;

		// move unmatched lines and add ghost lines
		nline0 = curDiff.end0 - curDiff.begin0 + 1; // #lines in diff on left
		nline1 = curDiff.end1 - curDiff.begin1 + 1; // #lines in diff on right
		int nextra = nline0-nline1;
		int nextraAbs = (nextra >= 0) ? nextra : -nextra;

		if (nextra > 0) 
		{
			// more lines on the left
			m_ptBuf[0]->MoveLine(curDiff.begin0, curDiff.end0, lcount0new-nline0);
			m_ptBuf[1]->MoveLine(curDiff.begin1, curDiff.end1, lcount1new-nline0);
			m_ptBuf[1]->SetEmptyLine(lcount1new - nextraAbs, nextraAbs);
			for (int i = 1; i <= nextraAbs; i++)
				m_ptBuf[1]->SetLineFlag(lcount1new-i, LF_GHOST, TRUE, FALSE, FALSE);
			lcount0new -= nline0;
			lcount1new -= nline0;
		}
		else if (nextra < 0) 
		{
			// more lines on the right
			m_ptBuf[0]->MoveLine(curDiff.begin0, curDiff.end0, lcount0new-nline1);
			m_ptBuf[1]->MoveLine(curDiff.begin1, curDiff.end1, lcount1new-nline1);
			m_ptBuf[0]->SetEmptyLine(lcount0new - nextraAbs, nextraAbs);
			for (int i = 1; i <= nextraAbs; i++)
				m_ptBuf[0]->SetLineFlag(lcount0new-i, LF_GHOST, TRUE, FALSE, FALSE);
			lcount0new -= nline1;
			lcount1new -= nline1;
		}
		else 
		{
			// same number of lines
			m_ptBuf[0]->MoveLine(curDiff.begin0, curDiff.end0, lcount0new-nline0);
			m_ptBuf[1]->MoveLine(curDiff.begin1, curDiff.end1, lcount1new-nline1);
			lcount0new -= nline0;
			lcount1new -= nline1;
		}
		lcount0 -= nline0;
		lcount1 -= nline1;

		// set dbegin, dend, blank, and line flags
		curDiff.dbegin0 = lcount0new;
		curDiff.dbegin1 = lcount1new;

		switch (curDiff.op)
		{
		case OP_LEFTONLY:
			// set curdiff
			{
				// left side
				curDiff.dend0 = lcount0new+nline0-1;
				curDiff.blank0 = -1;
				// right side
				curDiff.dend1 = lcount1new+nline0-1;
				curDiff.blank1 = curDiff.dbegin1;
			}
			// flag lines
			{
				for (UINT i = curDiff.dbegin0 ; i <= curDiff.dend0; i++)
					m_ptBuf[0]->SetLineFlag(i, LF_DIFF, TRUE, FALSE, FALSE);
				// blanks are already inserted (and flagged) to compensate for diff on other side
			}
			break;
		case OP_RIGHTONLY:
			// set curdiff
			{
				// left side
				curDiff.dend0 = lcount0new+nline1-1;
				curDiff.blank0 = curDiff.dbegin0;
				// right side
				curDiff.dend1 = lcount1new+nline1-1;
				curDiff.blank1 = -1;
			}
			// flag lines
			{
				for (UINT i = curDiff.dbegin1 ; i <= curDiff.dend1 ; i++)
					m_ptBuf[1]->SetLineFlag(i, LF_DIFF, TRUE, FALSE, FALSE);
				// blanks are already inserted (and flagged) to compensate for diff on other side
			}
			break;
		case OP_TRIVIAL:
			++m_nTrivialDiffs;
			// fall through and handle as diff
		case OP_DIFF:
			// set curdiff
			{
				int nline = max(nline0, nline1);
				// left side
				curDiff.dend0 = lcount0new+nline-1;
				curDiff.blank0 = -1;
				// right side
				curDiff.dend1 = lcount1new+nline-1;
				curDiff.blank1 = -1;
				if (nline0 > nline1)
					// more lines on left, ghost lines on right side
					curDiff.blank1 = curDiff.dend1+1 - nextraAbs;
				else if (nline0 < nline1)
					// more lines on right, ghost lines on left side
					curDiff.blank0 = curDiff.dend0+1 - nextraAbs;
			}
			// flag lines
			{
				// left side
				UINT i=0;
				for (i = curDiff.dbegin0; i <= curDiff.dend0 ; i++)
				{
					if (curDiff.blank0 == -1 || (int)i < curDiff.blank0)
					{
						// set diff or trivial flag
						DWORD dflag = (curDiff.op == OP_DIFF) ? LF_DIFF : LF_TRIVIAL;
						m_ptBuf[0]->SetLineFlag(i, dflag, TRUE, FALSE, FALSE);
					}
					else
					{
						// ghost lines are already inserted (and flagged)
						// ghost lines opposite to trivial lines are ghost and trivial
						if (curDiff.op == OP_TRIVIAL)
							m_ptBuf[0]->SetLineFlag(i, LF_TRIVIAL, TRUE, FALSE, FALSE);
					}
				}
				// right side
				for (i = curDiff.dbegin1; i <= curDiff.dend1 ; i++)
				{
					if (curDiff.blank1 == -1 || (int)i < curDiff.blank1)
					{
						// set diff or trivial flag
						DWORD dflag = (curDiff.op == OP_DIFF) ? LF_DIFF : LF_TRIVIAL;
						m_ptBuf[1]->SetLineFlag(i, dflag, TRUE, FALSE, FALSE);
					}
					else
					{
						// ghost lines are already inserted (and flagged)
						// ghost lines opposite to trivial lines are ghost and trivial
						if (curDiff.op == OP_TRIVIAL)
							m_ptBuf[1]->SetLineFlag(i, LF_TRIVIAL, TRUE, FALSE, FALSE);
					}
				}
			}
			break;
		}           // switch (curDiff.op)
		VERIFY(m_diffList.SetDiff(nDiff, curDiff));
	}             // for (nDiff = nDiffCount; nDiff-- > 0; )

	m_diffList.ConstructSignificantChain();

	// Used to strip trivial diffs out of the diff chain
	// if m_nTrivialDiffs
	// via copying them all to a new chain, then copying only non-trivials back
	// but now we keep all diffs, including trivial diffs


	m_ptBuf[0]->FinishLoading();
	m_ptBuf[1]->FinishLoading();
}

/**
 * @brief Checks if file has changed since last update (save or rescan).
 * @param [in] szPath File to check
 * @param [in] dfi Previous fileinfo of file
 * @param [in] bSave If TRUE Compare to last save-info, else to rescan-info
 * @param [in] nBuffer Index (0-based) of buffer
 * @return TRUE if file is changed.
 */
CMergeDoc::FileChange CMergeDoc::IsFileChangedOnDisk(LPCTSTR szPath, DiffFileInfo &dfi,
	bool bSave, int nBuffer)
{
	DiffFileInfo *fileInfo = NULL;
	bool bFileChanged = false;
	bool bIgnoreSmallDiff = GetOptionsMgr()->GetBool(OPT_IGNORE_SMALL_FILETIME);
	int tolerance = 0;
	if (bIgnoreSmallDiff)
		tolerance = SmallTimeDiff; // From MainFrm.h

	if (bSave)
		fileInfo = m_pSaveFileInfo[nBuffer];
	else
		fileInfo = m_pRescanFileInfo[nBuffer];

	// We assume file existed, so disappearing means removal
	if (_taccess(szPath, 0) == -1)
		return FileRemoved;

	dfi.Update(szPath);

	__int64 timeDiff = dfi.mtime - fileInfo->mtime;
	timeDiff = _abs64(timeDiff);
	if ((timeDiff > tolerance) || (dfi.size != fileInfo->size))
	{
		bFileChanged = true;
	}

	if (bFileChanged)
		return FileChanged;
	else
		return FileNoChange;
}

/**
 * @brief Asks and then saves modified files.
 *
 * This function saves modified files. Dialog is shown for user to select
 * modified file(s) one wants to save or discard changed. Cancelling of
 * save operation is allowed unless denied by parameter. After successfully
 * save operation file statuses are updated to directory compare.
 * @param [in] bAllowCancel If FALSE "Cancel" button is disabled.
 * @return TRUE if user selected "OK" so next operation can be
 * executed. If FALSE user choosed "Cancel".
 * @note If filename is empty, we assume scratchpads are saved,
 * so instead of filename, description is shown.
 * @todo If we have filename and description for file, what should
 * we do after saving to different filename? Empty description?
 * @todo Parameter @p bAllowCancel is always true in callers - can be removed.
 */
bool CMergeDoc::PromptAndSaveIfNeeded(BOOL bAllowCancel)
{
	const BOOL bLModified = m_ptBuf[0]->IsModified();
	const BOOL bRModified = m_ptBuf[1]->IsModified();
	bool result = true;
	BOOL bLSaveSuccess = FALSE;
	BOOL bRSaveSuccess = FALSE;

	if (!bLModified && !bRModified) //Both files unmodified
		return true;

	SaveClosingDlg dlg;
	dlg.DoAskFor(bLModified, bRModified);
	if (!bAllowCancel)
		dlg.m_bDisableCancel = TRUE;
	if (!m_filePaths.GetLeft().empty())
		dlg.m_sLeftFile = m_filePaths.GetLeft().c_str();
	else
		dlg.m_sLeftFile = m_strDesc[0].c_str();
	if (!m_filePaths.GetRight().empty())
		dlg.m_sRightFile = m_filePaths.GetRight().c_str();
	else
		dlg.m_sRightFile = m_strDesc[1].c_str();

	if (dlg.DoModal() == IDOK)
	{
		if (bLModified && dlg.m_leftSave == SaveClosingDlg::SAVECLOSING_SAVE)
		{
			if (!DoSave(m_filePaths.GetLeft().c_str(), bLSaveSuccess, 0))
				result = false;
		}

		if (bRModified && dlg.m_rightSave == SaveClosingDlg::SAVECLOSING_SAVE)
		{
			if (!DoSave(m_filePaths.GetRight().c_str(), bRSaveSuccess, 1))
				result = false;
		}
	}
	else
	{	
		result = false;
	}

	// If file were modified and saving was successfull,
	// update status on dir view
	if ((bLModified && bLSaveSuccess) ||
		 (bRModified && bRSaveSuccess))
	{
		// If directory compare has results
		if (m_pDirDoc->HasDiffs())
		{
			if (m_bEditAfterRescan[0] || m_bEditAfterRescan[1])
				FlushAndRescan(false);

			bool bIdentical = !m_diffList.HasSignificantDiffs(); // True if status should be set to identical
			m_pDirDoc->UpdateChangedItem(m_filePaths, m_diffList.GetSignificantDiffs(),
					m_nTrivialDiffs, bIdentical);
		}
	}

	return result;
}

/** Rescan only if we did not Rescan during the last timeOutInSecond seconds*/
void CMergeDoc::RescanIfNeeded(float timeOutInSecond)
{
	// if we did not rescan during the request timeOut, Rescan
	// else we did Rescan after the request, so do nothing
	COleDateTimeSpan elapsed = COleDateTime::GetCurrentTime() - m_LastRescan;
	if (elapsed.GetTotalSeconds() >= timeOutInSecond)
		// (laoran 08-01-2003) maybe should be FlushAndRescan(TRUE) ??
		FlushAndRescan();
}

/**
 * @brief We have two child views (left & right), so we keep pointers directly
 * at them (the MFC view list doesn't have them both)
 */
void CMergeDoc::SetMergeViews(CMergeEditView * pLeft, CMergeEditView * pRight)
{
	ASSERT(pLeft && !m_pView[MERGE_VIEW_LEFT]);
	m_pView[MERGE_VIEW_LEFT] = pLeft;
	ASSERT(pRight && !m_pView[MERGE_VIEW_RIGHT]);
	m_pView[MERGE_VIEW_RIGHT] = pRight;
}

/**
 * @brief Someone is giving us pointers to our detail views
 */
void CMergeDoc::SetMergeDetailViews(CMergeDiffDetailView * pLeft, CMergeDiffDetailView * pRight)
{
	ASSERT(pLeft && !m_pDetailView[0]);
	m_pDetailView[0] = pLeft;
	ASSERT(pRight && !m_pDetailView[1]);
	m_pDetailView[1] = pRight;
}

/**
 * @brief DirDoc gives us its identity just after it creates us
 */
void CMergeDoc::SetDirDoc(CDirDoc * pDirDoc)
{
	ASSERT(pDirDoc && !m_pDirDoc);
	m_pDirDoc = pDirDoc;
}

/**
 * @brief Return pointer to parent frame
 */
CChildFrame * CMergeDoc::GetParentFrame() 
{
	return dynamic_cast<CChildFrame *>(m_pView[MERGE_VIEW_LEFT]->GetParentFrame()); 
}

/**
 * @brief DirDoc is closing
 */
void CMergeDoc::DirDocClosing(CDirDoc * pDirDoc)
{
	ASSERT(m_pDirDoc == pDirDoc);
	m_pDirDoc = 0;
	// TODO (Perry 2003-03-30): perhaps merge doc should close now ?
}

/**
 * @brief DirDoc commanding us to close
 */
BOOL CMergeDoc::CloseNow()
{
	// Allow user to cancel closing
	if (!PromptAndSaveIfNeeded(TRUE))
		return FALSE;

	GetParentFrame()->CloseNow();
	return TRUE;
}

/**
 * @brief Loads file to buffer and shows load-errors
 * @param [in] sFileName File to open
 * @param [in] nBuffer Index (0-based) of buffer to load
 * @param [out] readOnly whether file is read-only
 * @param [in] encoding encoding used
 * @return Tells if files were loaded successfully
 * @sa CMergeDoc::OpenDocs()
 **/
int CMergeDoc::LoadFile(CString sFileName, int nBuffer, BOOL & readOnly, const FileTextEncoding & encoding)
{
	CDiffTextBuffer *pBuf;
	CString sError;
	DWORD retVal = FileLoadResult::FRESULT_ERROR;

	pBuf = m_ptBuf[nBuffer];
	nBuffer == 0 ? m_filePaths.SetLeft(sFileName) : m_filePaths.SetRight(sFileName);

	CRLFSTYLE nCrlfStyle = CRLF_STYLE_AUTOMATIC;
	CString sOpenError;
	retVal = pBuf->LoadFromFile(sFileName, m_pInfoUnpacker,
		m_strBothFilenames.c_str(), readOnly, nCrlfStyle, encoding, sOpenError);

	// if CMergeDoc::CDiffTextBuffer::LoadFromFile failed,
	// it left the pBuf in a valid (but empty) state via a call to InitNew

	if (FileLoadResult::IsOkImpure(retVal))
	{
		// File loaded, and multiple EOL types in this file
		FileLoadResult::SetMainOk(retVal);

		// If mixed EOLs are not enabled, enable them for this doc.
		if (!GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL))
		{
			pBuf->SetMixedEOL(true);
		}
	}

	if (FileLoadResult::IsError(retVal))
	{
		// Error from Unifile/system
		if (!sOpenError.IsEmpty())
			LangFormatString2(sError, IDS_ERROR_FILEOPEN, sFileName, sOpenError);
		else
			LangFormatString1(sError, IDS_ERROR_FILE_NOT_FOUND, sFileName);
		AfxMessageBox(sError, MB_OK | MB_ICONSTOP);
	}
	else if (FileLoadResult::IsErrorUnpack(retVal))
	{
		LangFormatString1(sError, IDS_ERROR_FILE_NOT_UNPACKED, sFileName);
		AfxMessageBox(sError, MB_OK | MB_ICONSTOP);
	}
	return retVal;
}

/**
 * @brief Check if specified codepage number is valid for WinMerge Editor.
 * @param [in] cp Codepage number to check.
 * @return true if codepage is valid, false otherwise.
 */
bool CMergeDoc::IsValidCodepageForMergeEditor(unsigned cp) const
{
	if (!cp) // 0 is our signal value for invalid
		return false;
	// Codepage must be actually installed on system
	// for us to be able to use it
	// We accept whatever codepages that codepage module says are installed
	return isCodepageInstalled(cp);
}

/**
 * @brief Sanity check file's specified codepage.
 * This function checks if file's specified codepage is valid for WinMerge
 * editor and if not resets the codepage to default.
 * @param [in,out] fileinfo Class containing file's codepage.
 */
void CMergeDoc::SanityCheckCodepage(FileLocation & fileinfo)
{
	if (fileinfo.encoding.m_unicoding == ucr::NONE
		&& !IsValidCodepageForMergeEditor(fileinfo.encoding.m_codepage))
	{
		int cp = getDefaultCodepage();
		if (!IsValidCodepageForMergeEditor(cp))
			cp = CP_ACP;
		fileinfo.encoding.SetCodepage(cp);
	}
}

/**
 * @brief Loads one file from disk and updates file infos.
 * @param [in] index Index of file in internal buffers.
 * @param [in] filename File's name.
 * @param [in] readOnly Is file read-only?
 * @param [in] encoding File's encoding.
 * @return One of FileLoadResult values.
 */
DWORD CMergeDoc::LoadOneFile(int index, String filename, BOOL readOnly,
		const FileTextEncoding & encoding)
{
	DWORD loadSuccess = FileLoadResult::FRESULT_ERROR;;
	
	if (!filename.empty())
	{
		if (GetMainFrame()->m_strDescriptions[index].empty())
			m_nBufferType[index] = BUFFER_NORMAL;
		else
		{
			m_nBufferType[index] = BUFFER_NORMAL_NAMED;
			m_strDesc[index] = GetMainFrame()->m_strDescriptions[index];
			GetMainFrame()->m_strDescriptions[index].erase();
		}
		m_pSaveFileInfo[index]->Update(filename);
		m_pRescanFileInfo[index]->Update(filename);

		loadSuccess = LoadFile(filename.c_str(), index, readOnly, encoding);
	}
	else
	{
		m_nBufferType[index] = BUFFER_UNNAMED;
		m_ptBuf[index]->InitNew();
		m_strDesc[index] = GetMainFrame()->m_strDescriptions[index];
		m_ptBuf[index]->m_encoding = encoding;
		loadSuccess = FileLoadResult::FRESULT_OK;
	}
	return loadSuccess;
}

/**
 * @brief Loads files and does initial rescan.
 * @param filelocLeft [in] File to open to left side (path & encoding info)
 * @param fileLocRight [in] File to open to right side (path & encoding info)
 * @param bROLeft [in] Is left file read-only
 * @param bRORight [in] Is right file read-only
 * @return Success/Failure/Binary (failure) per typedef enum OpenDocsResult_TYPE
 * @todo Options are still read from CMainFrame, this will change
 * @sa CMainFrame::ShowMergeDoc()
 */
OPENRESULTS_TYPE CMergeDoc::OpenDocs(FileLocation filelocLeft, FileLocation filelocRight,
		BOOL bROLeft, BOOL bRORight)
{
	BOOL bIdentical = FALSE;
	int nRescanResult = RESCAN_OK;

	// Filter out invalid codepages, or editor will display all blank
	SanityCheckCodepage(filelocLeft);
	SanityCheckCodepage(filelocRight);

	// clear undo stack
	undoTgt.clear();
	curUndo = undoTgt.begin();

	// Prevent displaying views during LoadFile
	// Note : attach buffer again only if both loads succeed
	m_pView[MERGE_VIEW_LEFT]->DetachFromBuffer();
	m_pView[MERGE_VIEW_RIGHT]->DetachFromBuffer();
	m_pDetailView[0]->DetachFromBuffer();
	m_pDetailView[1]->DetachFromBuffer();

	// clear undo buffers
	m_ptBuf[0]->m_aUndoBuf.clear();
	m_ptBuf[1]->m_aUndoBuf.clear();

	// free the buffers
	m_ptBuf[0]->FreeAll();
	m_ptBuf[1]->FreeAll();

	String sLeftFile = filelocLeft.filepath;
	String sRightFile = filelocRight.filepath;

	// build the text being filtered, "|" separates files as it is forbidden in filenames
	m_strBothFilenames = sLeftFile + _T("|") + sRightFile;

	// Load files
	DWORD nLeftSuccess = LoadOneFile(0, sLeftFile.c_str(), bROLeft,
		filelocLeft.encoding);
	DWORD nRightSuccess = LoadOneFile(1, sRightFile.c_str(), bRORight,
		filelocRight.encoding);

	const BOOL bFiltersEnabled = GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED);

	// scratchpad : we don't call LoadFile, so
	// we need to initialize the unpacker as a "do nothing" one
	if (bFiltersEnabled && m_nBufferType[0] == BUFFER_UNNAMED &&
			m_nBufferType[1] == BUFFER_UNNAMED)
		m_pInfoUnpacker->Initialize(PLUGIN_MANUAL);

	// Bail out if either side failed
	if (!FileLoadResult::IsOk(nLeftSuccess) || !FileLoadResult::IsOk(nRightSuccess))
	{
		OPENRESULTS_TYPE retVal = OPENRESULTS_FAILED_MISC;
		CChildFrame *pFrame = GetParentFrame();
		if (pFrame)
		{
			// Use verify macro to trap possible error in debug.
			VERIFY(pFrame->DestroyWindow());
		}
		return retVal;
	}

	// Warn user if file load was lossy (bad encoding)
	if (FileLoadResult::IsLossy(nLeftSuccess) || FileLoadResult::IsLossy(nRightSuccess))
	{
		// TODO: It would be nice to report how many lines were lossy
		// we did calculate those numbers when we loaded the files, in the text stats

		int idres=0;
		if (FileLoadResult::IsLossy(nLeftSuccess) && FileLoadResult::IsLossy(nRightSuccess))
		{
			idres = IDS_LOSSY_TRANSCODING_BOTH;
		}
		else if (FileLoadResult::IsLossy(nLeftSuccess))
		{
			idres = IDS_LOSSY_TRANSCODING_LEFT;
		}
		else
		{
			idres = IDS_LOSSY_TRANSCODING_RIGHT;
		}
		LangMessageBox(idres, MB_ICONSTOP);
	}

	// Now buffers data are valid
	m_pView[MERGE_VIEW_LEFT]->AttachToBuffer();
	m_pView[MERGE_VIEW_RIGHT]->AttachToBuffer();
	m_pDetailView[0]->AttachToBuffer();
	m_pDetailView[1]->AttachToBuffer();

	// Currently there is only one set of syntax colors, which all documents & views share
	m_pView[MERGE_VIEW_LEFT]->SetColorContext(GetMainSyntaxColors());
	m_pView[MERGE_VIEW_RIGHT]->SetColorContext(GetMainSyntaxColors());
	m_pDetailView[0]->SetColorContext(GetMainSyntaxColors());
	m_pDetailView[1]->SetColorContext(GetMainSyntaxColors());

	// Set read-only statuses
	m_ptBuf[0]->SetReadOnly(bROLeft);
	m_ptBuf[1]->SetReadOnly(bRORight);

	// Check the EOL sensitivity option (do it before Rescan)
	DIFFOPTIONS diffOptions = {0};
	m_diffWrapper.GetOptions(&diffOptions);
	if (m_ptBuf[0]->GetCRLFMode() != m_ptBuf[1]->GetCRLFMode() &&
		!GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL) && !diffOptions.bIgnoreEol)
	{
		// Options and files not are not compatible :
		// Sensitive to EOL on, allow mixing EOL off, and files have a different EOL style.
		// All lines will differ, that is not very interesting and probably not wanted.
		// Propose to turn off the option 'sensitive to EOL'
		String s = theApp.LoadString(IDS_SUGGEST_IGNOREEOL);
		if (AfxMessageBox(s.c_str(), MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN | MB_IGNORE_IF_SILENCED, IDS_SUGGEST_IGNOREEOL) == IDYES)
		{
			diffOptions.bIgnoreEol = TRUE;
			m_diffWrapper.SetOptions(&diffOptions);
		}
	}

	// Define the prediffer
	PackingInfo * infoUnpacker = 0;
	PrediffingInfo * infoPrediffer = 0;
	if (bFiltersEnabled)
	{
		m_pDirDoc->FetchPluginInfos(m_strBothFilenames.c_str(), &infoUnpacker, &infoPrediffer);
		m_diffWrapper.SetPrediffer(infoPrediffer);
		m_diffWrapper.SetTextForAutomaticPrediff(m_strBothFilenames);
	}

	BOOL bBinary = FALSE;
	nRescanResult = Rescan(bBinary, bIdentical);

	// Open filed if rescan succeed and files are not binaries
	if (nRescanResult == RESCAN_OK)
	{
		// prepare the four views
		CMergeEditView * pLeft = GetLeftView();
		CMergeEditView * pRight = GetRightView();
		CMergeDiffDetailView * pLeftDetail = GetLeftDetailView();
		CMergeDiffDetailView * pRightDetail = GetRightDetailView();
		
		// set the document types
		// Warning : it is the first thing to do (must be done before UpdateView,
		// or any function that calls UpdateView, like SelectDiff)
		// Note: If option enabled, and another side type is not recognized,
		// we use recognized type for unrecognized side too.
		String sextL, sextR;
		if (bFiltersEnabled && m_pInfoUnpacker->textType.length())
		{
			sextL = sextR = m_pInfoUnpacker->textType;
		}
		else
		{
			sextL = GetFileExt(sLeftFile.c_str(), m_strDesc[0].c_str());
			sextR = GetFileExt(sRightFile.c_str(), m_strDesc[1].c_str());
		}
		
		BOOL syntaxHLEnabled = GetOptionsMgr()->GetBool(OPT_SYNTAX_HIGHLIGHT);
		BOOL bLeftTyped = FALSE;
		BOOL bRightTyped = FALSE;
		
		if (syntaxHLEnabled)
		{
			bLeftTyped = pLeft->SetTextType(sextL.c_str());
			pLeftDetail->SetTextType(sextL.c_str());
			bRightTyped = pRight->SetTextType(sextR.c_str());
			pRightDetail->SetTextType(sextR.c_str());
		}

		// If textypes of the files aren't recogzined by their extentions,
		// try to recognize them using their first lines 
		if (!bLeftTyped && !bRightTyped)
		{
			CString sFirstLine;
			m_ptBuf[0]->GetLine(0, sFirstLine);
			bLeftTyped = pLeft->SetTextTypeByContent(sFirstLine);
			m_ptBuf[1]->GetLine(0, sFirstLine);
			bRightTyped = pRight->SetTextTypeByContent(sFirstLine);
		}

		// If other side didn't have recognized texttype, apply recognized
		// type to unrecognized one. (comparing file.cpp and file.bak applies
		// cpp file type to .bak file.
		if (bLeftTyped != bRightTyped)
		{
			CCrystalTextView::TextDefinition *enuType;

			if (bLeftTyped)
			{
				enuType = pLeft->GetTextType(sextL.c_str());
				pRight->SetTextType(enuType);
				pRightDetail->SetTextType(enuType);
			}
			else
			{
				enuType = pRight->GetTextType(sextR.c_str());
				pLeft->SetTextType(enuType);
				pLeftDetail->SetTextType(enuType);
			}
		}

		// set the frame window header
		UpdateHeaderPath(0);
		UpdateHeaderPath(1);

		pLeft->DocumentsLoaded();
		pRight->DocumentsLoaded();
		pLeftDetail->DocumentsLoaded();
		pRightDetail->DocumentsLoaded();

		// Inform user that files are identical
		// Don't show message if new buffers created
		if ((TRUE == bIdentical) &&
			((m_nBufferType[0] == BUFFER_NORMAL) ||
			 (m_nBufferType[0] == BUFFER_NORMAL_NAMED) ||
			 (m_nBufferType[1] == BUFFER_NORMAL) ||
			 (m_nBufferType[1] == BUFFER_NORMAL_NAMED)))
		{
			ShowRescanError(nRescanResult, bIdentical);
		}

		// scroll to first diff
		if (GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST) &&
			m_diffList.HasSignificantDiffs())
		{
			int nDiff = m_diffList.FirstSignificantDiff();
			pLeft->SelectDiff(nDiff, true, false);
		}

		// Exit if files are identical should only work for the first
		// comparison and must be disabled afterward.
		GetMainFrame()->m_bExitIfNoDiff = MergeCmdLineInfo::Disabled;
	}
	else
	{
		// CMergeDoc::Rescan fails if files do not exist on both sides 
		// or the really arcane case that the temp files couldn't be created, 
		// which is too obscure to bother reporting if you can't write to 
		// your temp directory, doing nothing is graceful enough for that).
		ShowRescanError(nRescanResult, bIdentical);
		GetParentFrame()->DestroyWindow();
		return OPENRESULTS_FAILED_MISC;
	}

	// Force repaint of location pane to update it in case we had some warning
	// dialog visible and it got painted before files were loaded
	if (m_pView[MERGE_VIEW_LEFT])
		m_pView[MERGE_VIEW_LEFT]->RepaintLocationPane();

	return OPENRESULTS_SUCCESS;
}

/**
 * @brief Re-load a document.
 * This methods re-loads the file compare document. The re-loaded document is
 * one side of the file compare.
 * @param [in] index The document to re-load.
 * @return Open result code.
 */
OPENRESULTS_TYPE CMergeDoc::ReloadDoc(int index)
{
	BOOL bIdentical = FALSE;
	int nRescanResult = RESCAN_OK;

	// clear undo stack
	undoTgt.clear();
	curUndo = undoTgt.begin();

	const String path = m_filePaths.GetPath(index);
	const BOOL readOnly = m_ptBuf[index]->GetReadOnly();


	// Prevent displaying views during LoadFile
	// Note : attach buffer again only if both loads succeed
	// clear undo buffers
	// free the buffers
	if (index == 0)
	{
		m_pView[MERGE_VIEW_LEFT]->DetachFromBuffer();
		m_pDetailView[0]->DetachFromBuffer();
		m_ptBuf[0]->m_aUndoBuf.clear();
		m_ptBuf[0]->FreeAll();
	}
	else
	{
		m_pView[MERGE_VIEW_RIGHT]->DetachFromBuffer();
		m_pDetailView[1]->DetachFromBuffer();
		m_ptBuf[1]->m_aUndoBuf.clear();
		m_ptBuf[1]->FreeAll();
	}

	// Load files
	DWORD nLoadSuccess = LoadOneFile(index, path.c_str(), readOnly,
		m_ptBuf[index]->getEncoding());
	const BOOL bFiltersEnabled = GetOptionsMgr()->GetBool(OPT_PLUGINS_ENABLED);

	// Bail out if either side failed
	if (!FileLoadResult::IsOk(nLoadSuccess))
	{
		OPENRESULTS_TYPE retVal = OPENRESULTS_FAILED_MISC;
		CChildFrame *pFrame = GetParentFrame();
		if (pFrame)
		{
			// Use verify macro to trap possible error in debug.
			VERIFY(pFrame->DestroyWindow());
		}
		return retVal;
	}

	// Warn user if file load was lossy (bad encoding)
	if (FileLoadResult::IsLossy(nLoadSuccess))
	{
		// TODO: It would be nice to report how many lines were lossy
		// we did calculate those numbers when we loaded the files, in the text stats
		LangMessageBox(IDS_LOSSY_TRANSCODING_LEFT, MB_ICONSTOP);
	}

	// Now buffers data are valid
	if (index == 0)
	{
		m_pView[MERGE_VIEW_LEFT]->AttachToBuffer();
		m_pDetailView[0]->AttachToBuffer();
	}
	else
	{
		m_pView[MERGE_VIEW_RIGHT]->AttachToBuffer();
		m_pDetailView[1]->AttachToBuffer();
	}

	// Currently there is only one set of syntax colors, which all documents & views share
	m_pView[MERGE_VIEW_LEFT]->SetColorContext(GetMainSyntaxColors());
	m_pView[MERGE_VIEW_RIGHT]->SetColorContext(GetMainSyntaxColors());
	m_pDetailView[0]->SetColorContext(GetMainSyntaxColors());
	m_pDetailView[1]->SetColorContext(GetMainSyntaxColors());

	// Set read-only status
	m_ptBuf[index]->SetReadOnly(readOnly);

	// Check the EOL sensitivity option (do it before Rescan)
	DIFFOPTIONS diffOptions = {0};
	m_diffWrapper.GetOptions(&diffOptions);
	if (m_ptBuf[0]->GetCRLFMode() != m_ptBuf[1]->GetCRLFMode() &&
		!GetOptionsMgr()->GetBool(OPT_ALLOW_MIXED_EOL) && !diffOptions.bIgnoreEol)
	{
		// Options and files not are not compatible :
		// Sensitive to EOL on, allow mixing EOL off, and files have a different EOL style.
		// All lines will differ, that is not very interesting and probably not wanted.
		// Propose to turn off the option 'sensitive to EOL'
		String s = theApp.LoadString(IDS_SUGGEST_IGNOREEOL);
		if (AfxMessageBox(s.c_str(), MB_YESNO | MB_ICONWARNING | MB_DONT_ASK_AGAIN | MB_IGNORE_IF_SILENCED, IDS_SUGGEST_IGNOREEOL) == IDYES)
		{
			diffOptions.bIgnoreEol = TRUE;
			m_diffWrapper.SetOptions(&diffOptions);
		}
	}

	// Define the prediffer
	PackingInfo * infoUnpacker = 0;
	PrediffingInfo * infoPrediffer = 0;
	if (bFiltersEnabled)
	{
		m_pDirDoc->FetchPluginInfos(m_strBothFilenames.c_str(), &infoUnpacker, &infoPrediffer);
		m_diffWrapper.SetPrediffer(infoPrediffer);
		m_diffWrapper.SetTextForAutomaticPrediff(m_strBothFilenames);
	}

	BOOL bBinary = FALSE;
	nRescanResult = Rescan(bBinary, bIdentical, TRUE);

	// Open filed if rescan succeed and files are not binaries
	if (nRescanResult == RESCAN_OK)
	{
		// prepare the four views
		CMergeEditView * pLeft = GetLeftView();
		CMergeEditView * pRight = GetRightView();
		CMergeDiffDetailView * pLeftDetail = GetLeftDetailView();
		CMergeDiffDetailView * pRightDetail = GetRightDetailView();
		
		// set the document types
		// Warning : it is the first thing to do (must be done before UpdateView,
		// or any function that calls UpdateView, like SelectDiff)
		// Note: If option enabled, and another side type is not recognized,
		// we use recognized type for unrecognized side too.
		String sext;
		if (bFiltersEnabled && m_pInfoUnpacker->textType.length())
		{
			sext = m_pInfoUnpacker->textType;
		}
		else
		{
			sext = GetFileExt(path.c_str(), m_strDesc[index].c_str());
		}
		
		BOOL syntaxHLEnabled = GetOptionsMgr()->GetBool(OPT_SYNTAX_HIGHLIGHT);
		BOOL bLeftTyped = FALSE;
		BOOL bRightTyped = FALSE;
		
		if (syntaxHLEnabled)
		{
			if (index == 0)
			{
				bLeftTyped = pLeft->SetTextType(sext.c_str());
				pLeftDetail->SetTextType(sext.c_str());
			}
			else if (index == 1)
			{
				bRightTyped = pRight->SetTextType(sext.c_str());
				pRightDetail->SetTextType(sext.c_str());
			}
		}

		// set the frame window header
		UpdateHeaderPath(index);

		if (index == 0)
		{
			pLeft->DocumentsLoaded();
			pRight->DocumentsLoaded();
		}
		else if (index == 1)
		{
			pLeftDetail->DocumentsLoaded();
			pRightDetail->DocumentsLoaded();
		}

		// Inform user that files are identical
		// Don't show message if new buffers created
		if ((TRUE == bIdentical) &&
			((m_nBufferType[0] == BUFFER_NORMAL) ||
			 (m_nBufferType[0] == BUFFER_NORMAL_NAMED) ||
			 (m_nBufferType[1] == BUFFER_NORMAL) ||
			 (m_nBufferType[1] == BUFFER_NORMAL_NAMED)))
		{
			ShowRescanError(nRescanResult, bIdentical);
		}

		// scroll to first diff
		if (GetOptionsMgr()->GetBool(OPT_SCROLL_TO_FIRST) &&
			m_diffList.HasSignificantDiffs())
		{
			int nDiff = m_diffList.FirstSignificantDiff();
			pLeft->SelectDiff(nDiff, true, false);
		}

		// Exit if files are identical should only work for the first
		// comparison and must be disabled afterward.
		GetMainFrame()->m_bExitIfNoDiff = MergeCmdLineInfo::Disabled;
	}
	else
	{
		// CMergeDoc::Rescan fails if files do not exist on both sides 
		// or the really arcane case that the temp files couldn't be created, 
		// which is too obscure to bother reporting if you can't write to 
		// your temp directory, doing nothing is graceful enough for that).
		ShowRescanError(nRescanResult, bIdentical);
		GetParentFrame()->DestroyWindow();
		return OPENRESULTS_FAILED_MISC;
	}

	// Force repaint of location pane to update it in case we had some warning
	// dialog visible and it got painted before files were loaded
	if (m_pView[MERGE_VIEW_LEFT])
		m_pView[MERGE_VIEW_LEFT]->RepaintLocationPane();

	return OPENRESULTS_SUCCESS;
}

/**
 * @brief Refresh cached options.
 *
 * For compare speed, we have to cache some frequently needed options,
 * instead of getting option value every time from OptionsMgr. This
 * function must be called every time options are changed to OptionsMgr.
 */
void CMergeDoc::RefreshOptions()
{
	DIFFOPTIONS options = {0};
	
	m_diffWrapper.SetDetectMovedBlocks(GetOptionsMgr()->GetBool(OPT_CMP_MOVED_BLOCKS));
	options.nIgnoreWhitespace = GetOptionsMgr()->GetInt(OPT_CMP_IGNORE_WHITESPACE);
	options.bIgnoreBlankLines = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_BLANKLINES);
	options.bFilterCommentsLines = GetOptionsMgr()->GetBool(OPT_CMP_FILTER_COMMENTLINES);
	options.bIgnoreCase = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_CASE);
	options.bIgnoreEol = GetOptionsMgr()->GetBool(OPT_CMP_IGNORE_EOL);

	m_diffWrapper.SetOptions(&options);

	// Refresh view options
	m_pView[MERGE_VIEW_LEFT]->RefreshOptions();
	m_pView[MERGE_VIEW_RIGHT]->RefreshOptions();
}

/**
 * @brief Write path and filename to headerbar
 * @note SetText() does not repaint unchanged text
 */
void CMergeDoc::UpdateHeaderPath(int pane)
{
	CChildFrame *pf = GetParentFrame();
	ASSERT(pf);
	String sText;
	BOOL bChanges = FALSE;

	if (m_nBufferType[pane] == BUFFER_UNNAMED ||
		m_nBufferType[pane] == BUFFER_NORMAL_NAMED)
	{
		sText = m_strDesc[pane];
	}
	else
	{
		sText = m_filePaths.GetPath(pane);
		if (m_pDirDoc)
		{
			if (pane == 0)
				m_pDirDoc->ApplyLeftDisplayRoot(sText);
			else
				m_pDirDoc->ApplyRightDisplayRoot(sText);
		}
	}
	bChanges = m_ptBuf[pane]->IsModified();

	if (bChanges)
		sText.insert(0, _T("* "));

	pf->GetHeaderInterface()->SetText(pane, sText.c_str());

	SetTitle(NULL);
}

/**
 * @brief Paint differently the headerbar of the active view
 */
void CMergeDoc::UpdateHeaderActivity(int pane, bool bActivate)
{
	CChildFrame *pf = GetParentFrame();
	ASSERT(pf);
	pf->GetHeaderInterface()->SetActive(pane, bActivate);
}

/**
 * @brief Return if doc is in Merging/Editing mode
 */
bool CMergeDoc::GetMergingMode() const
{
	return m_bMergingMode;
}

/**
 * @brief Set doc to Merging/Editing mode
 */
void CMergeDoc::SetMergingMode(bool bMergingMode)
{
	m_bMergingMode = bMergingMode;
	GetOptionsMgr()->SaveOption(OPT_MERGE_MODE, m_bMergingMode);
}

/**
 * @brief Set detect/not detect Moved Blocks
 */
void CMergeDoc::SetDetectMovedBlocks(bool bDetectMovedBlocks)
{
	if (bDetectMovedBlocks == m_diffWrapper.GetDetectMovedBlocks())
		return;

	GetOptionsMgr()->SaveOption(OPT_CMP_MOVED_BLOCKS, bDetectMovedBlocks);
	m_diffWrapper.SetDetectMovedBlocks(bDetectMovedBlocks);
	FlushAndRescan();
}

/**
 * @brief Check if given buffer has mixed EOL style.
 * @param [in] nBuffer Buffer to check.
 * @return true if buffer's EOL style is mixed, false otherwise.
 */
bool CMergeDoc::IsMixedEOL(int nBuffer) const
{
	CDiffTextBuffer *pBuf = m_ptBuf[nBuffer];
	return pBuf->IsMixedEOL();
}

void CMergeDoc::SetEditedAfterRescan(int nBuffer)
{
	m_bEditAfterRescan[nBuffer] = true;
}

/**
 * @brief Update document filenames to title
 */
void CMergeDoc::SetTitle(LPCTSTR lpszTitle)
{
	const TCHAR pszSeparator[] = _T(" - ");
	String sTitle;

	if (lpszTitle)
		sTitle = lpszTitle;
	else
	{
		if (!m_strDesc[0].empty())
			sTitle += m_strDesc[0];
		else
		{
			String file;
			String ext;
			SplitFilename(m_filePaths.GetLeft().c_str(), NULL, &file, &ext);
			sTitle += file.c_str();
			if (!ext.empty())
			{
				sTitle += _T(".");
				sTitle += ext.c_str();
			}
		}

		sTitle += pszSeparator;

		if (!m_strDesc[1].empty())
			sTitle += m_strDesc[1];
		else
		{
			String file;
			String ext;
			SplitFilename(m_filePaths.GetRight().c_str(), NULL, &file, &ext);
			sTitle += file.c_str();
			if (!ext.empty())
			{
				sTitle += _T(".");
				sTitle += ext.c_str();
			}
		}
	}
	CDocument::SetTitle(sTitle.c_str());
}

/**
 * @brief Update any resources necessary after a GUI language change
 */
void CMergeDoc::UpdateResources()
{
	if (m_filePaths.GetLeft().empty() && m_filePaths.GetRight().empty())
	{
		m_strDesc[0] = theApp.LoadString(IDS_EMPTY_LEFT_FILE);
		m_strDesc[1] = theApp.LoadString(IDS_EMPTY_RIGHT_FILE);
	}
	UpdateHeaderPath(0);
	UpdateHeaderPath(1);

	GetParentFrame()->UpdateResources();
	GetLeftView()->UpdateResources();
	GetRightView()->UpdateResources();
}

// Return current word breaking break type setting (whitespace only or include punctuation)
bool CMergeDoc::GetBreakType() const
{
	BOOL breakType = GetOptionsMgr()->GetInt(OPT_BREAK_TYPE);
	return breakType > 0;
}

// Return true to do line diff colors at the byte level (false to do them at word level)
bool CMergeDoc::GetByteColoringOption() const
{
	// color at byte level if 'break_on_words' option not set
	BOOL breakWords = GetOptionsMgr()->GetBool(OPT_BREAK_ON_WORDS);
	return !breakWords;
}

/// Swap files and update views
void CMergeDoc::SwapFiles()
{
	// Swap views
	int nLeftViewId = m_pView[0]->GetDlgCtrlID();
	int nRightViewId = m_pView[1]->GetDlgCtrlID();
	m_pView[0]->SetDlgCtrlID(nRightViewId);
	m_pView[1]->SetDlgCtrlID(nLeftViewId);

	int nLeftDetailViewId = m_pDetailView[0]->GetDlgCtrlID();
	int nRightDetailViewId = m_pDetailView[1]->GetDlgCtrlID();
	m_pDetailView[0]->SetDlgCtrlID(nRightDetailViewId);
	m_pDetailView[1]->SetDlgCtrlID(nLeftDetailViewId);

	// Swap buffers and so on
	swap(m_ptBuf[0], m_ptBuf[1]);
	swap(m_pView[0], m_pView[1]);
	swap(m_pDetailView[0], m_pDetailView[1]);
	swap(m_pSaveFileInfo[0], m_pSaveFileInfo[1]);
	swap(m_pRescanFileInfo[0], m_pRescanFileInfo[1]);
	swap(m_nBufferType[0], m_nBufferType[1]);
	swap(m_bEditAfterRescan[0], m_bEditAfterRescan[1]);
	m_strDesc[0].swap(m_strDesc[1]);

	m_filePaths.Swap();
	m_diffList.Swap();

	m_ptBuf[0]->m_nThisPane = 0;
	m_pView[MERGE_VIEW_LEFT]->m_nThisPane = 0;
	m_pDetailView[0]->m_nThisPane = 0;
	m_ptBuf[1]->m_nThisPane = 1;
	m_pView[MERGE_VIEW_RIGHT]->m_nThisPane = 1;
	m_pDetailView[1]->m_nThisPane = 1;
	swap(m_pView[MERGE_VIEW_LEFT]->m_piMergeEditStatus, m_pView[MERGE_VIEW_RIGHT]->m_piMergeEditStatus);

	// Update views
	UpdateHeaderPath(0);
	UpdateHeaderPath(1);
	GetParentFrame()->UpdateSplitter();
	m_pView[MERGE_VIEW_LEFT]->UpdateStatusbar();
	m_pView[MERGE_VIEW_RIGHT]->UpdateStatusbar();

	UpdateAllViews(NULL);
}

/**
 * @brief Display encodings to user
 */
void CMergeDoc::OnFileEncoding()
{
	DoFileEncodingDialog();
}

// Return file extension either from file name or file description (if WinMerge is used as an
// external Rational ClearCase tool.
String CMergeDoc::GetFileExt(LPCTSTR sFileName, LPCTSTR sDescription)
{
	String sExt;
	SplitFilename(sFileName, NULL, NULL, &sExt);

	if (TRUE == GetMainFrame()->m_bClearCaseTool)
	{
		// If no extension found in real file name.
		if (sExt.empty())
		{
			SplitViewName(sFileName, NULL, NULL, &sExt);
		}
		// If no extension found in repository file name.
		if (TRUE == sExt.empty())
		{
			SplitViewName(sDescription, NULL, NULL, &sExt);
		}
	}
	return sExt;
}

/**
 * @brief Generate report from file compare results.
 */
void CMergeDoc::OnToolsGenerateReport()
{
	CString s, folder;

	if (!SelectFile(GetMainFrame()->GetSafeHwnd(), s, folder, IDS_SAVE_AS_TITLE, IDS_HTML_REPORT_FILES, FALSE, _T("htm")))
		return;

	// calculate HTML font size
	LOGFONT lf;
	CDC dc;
	dc.CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	m_pView[0]->GetFont(lf);
	int nFontSize = -MulDiv (lf.lfHeight, 72, dc.GetDeviceCaps (LOGPIXELSY));

	// create HTML report
	UniStdioFile file;
	if (!file.Open(s, _T("wt")))
	{
		String errMsg = GetSysError(GetLastError());
		ResMsgBox1(IDS_REPORT_ERROR, errMsg.c_str(), MB_OK | MB_ICONSTOP);
		return;
	}

	file.SetCodepage(CP_UTF8);

	String header = 
		Fmt(
		_T("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n")
		_T("\t\"http://www.w3.org/TR/html4/loose.dtd\">\n")
		_T("<html>\n")
		_T("<head>\n")
		_T("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n")
		_T("<title>WinMerge File Compare Report</title>\n")
		_T("<style type=\"text/css\">\n")
		_T("<!--\n")
		_T("td,th {font-size: %dpt;}\n")
		_T(".ln {text-align: right; background-color: lightgrey;}\n")
		_T(".title {color: white; background-color: blue; vertical-align: top;}\n")
		_T("%s")
		_T("-->\n")
		_T("</style>\n")
		_T("</head>\n")
		_T("<body>\n")
		_T("<table cellspacing=\"0\" cellpadding=\"0\" style=\"width: 100%%; margin: 0; border: none;\">\n")
		_T("<thead>\n")
		_T("<tr>\n"),
		nFontSize, m_pView[0]->GetHTMLStyles());
	file.WriteString(header);

	// Get paths
	// If archive, use archive path + folder + filename inside archive
	// If desc text given, use it
	String left = m_filePaths.GetPath(0);
	String right = m_filePaths.GetPath(1);
	if (m_pDirDoc->IsArchiveFolders())
	{
		m_pDirDoc->ApplyLeftDisplayRoot(left);
		m_pDirDoc->ApplyRightDisplayRoot(right);
	}
	else
	{
		if (!m_strDesc[0].empty())
			left = m_strDesc[0];
		if (!m_strDesc[1].empty())
			right = m_strDesc[1];
	}

	// left and right title
	int nBuffer;
	for (nBuffer = 0; nBuffer < 2; nBuffer++)
	{
		int nLineNumberColumnWidth = m_pView[nBuffer]->GetViewLineNumbers() ? 1 : 0;
		String data = Fmt(_T("<th class=\"title\" style=\"width:%d%%\"></th>"), 
			nLineNumberColumnWidth);
		file.WriteString(data);
		data = Fmt(_T("<th class=\"title\" style=\"width:%f%%\">"),
			(double)(100 - nLineNumberColumnWidth * 2) / 2);
		file.WriteString(data);
		if (nBuffer == 0)
			file.WriteString(left.c_str());
		else
			file.WriteString(right.c_str());
		file.WriteString(_T("</th>\n"));
	}
	file.WriteString(
		_T("</tr>\n")
		_T("</thead>\n")
		_T("<tbody>\n"));

	// write the body of the report
	int idx[2] = {0};
	int nLineCount[2] = {0};
	for (nBuffer = 0; nBuffer < 2; nBuffer++)
		nLineCount[nBuffer] = m_ptBuf[nBuffer]->GetLineCount();

	for (;;)
	{
		file.WriteString(_T("<tr valign=\"top\">\n"));
		for (nBuffer = 0; nBuffer < 2; nBuffer++)
		{
			if (idx[nBuffer] < nLineCount[nBuffer])
			{
				// line number
				DWORD dwFlags = m_ptBuf[nBuffer]->GetLineFlags(idx[nBuffer]);
				if (!(dwFlags & LF_GHOST) && m_pView[nBuffer]->GetViewLineNumbers())
				{
					String data = Fmt(_T("<td class=\"ln\">%d</td>"),
							m_ptBuf[nBuffer]->ComputeRealLine(idx[nBuffer]) + 1);
					file.WriteString(data);
				}
				else
					file.WriteString(_T("<td class=\"ln\"></td>"));
				// write a line on left/right side
				file.WriteString((LPCTSTR)m_pView[nBuffer]->GetHTMLLine(idx[nBuffer], _T("td")));
				idx[nBuffer]++;
			}
			else
				file.WriteString(_T("<td class=\"ln\"></td><td></td>"));
			file.WriteString(_T("\n"));
		}
		file.WriteString(_T("</tr>\n"));

		if (idx[0] >= nLineCount[0] && idx[1] >= nLineCount[1])
			break;
	}
	file.WriteString(
		_T("</tbody>\n")
		_T("</table>\n")
		_T("</body>\n")
		_T("</html>\n"));

	file.Close();

	LangMessageBox(IDS_REPORT_SUCCESS, MB_OK | MB_ICONINFORMATION);
}

/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
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
 * @file  MergeDoc.h
 *
 * @brief Declaration of CMergeDoc class
 */
// ID line follows -- this is updated by SVN
// $Id: MergeDoc.h 7494 2011-01-02 13:26:21Z gerundt $

#if !defined(AFX_MERGEDOC_H__BBCD4F90_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
#define AFX_MERGEDOC_H__BBCD4F90_34E4_11D1_BAA6_00A024706EDC__INCLUDED_

#include "DiffTextBuffer.h"
#include <vector>
#include "DiffWrapper.h"
#include "DiffList.h"
#include "stringdiffs.h"
#include "TempFile.h"
#include "PathContext.h"
#include "DiffFileInfo.h"

/**
 * @brief Additional action codes for WinMerge.
 * @note Reserve first 100 for CrystalEditor
 */
enum
{
	CE_ACTION_MERGE = 100, /**< Merging action */
};

/**
 * @brief Return statuses of file rescan
 */
enum
{
	RESCAN_OK = 0, /**< Rescan succeeded */
	RESCAN_SUPPRESSED, /**< Rescan not done - suppressed */
	RESCAN_FILE_ERR, /**< Error reading file */
	RESCAN_TEMP_ERR, /**< Error saving to temp file */
};

/**
 * @brief File saving statuses
 */
enum
{
	SAVE_DONE = 0, /**< Saving succeeded */  
	SAVE_FAILED, /**< Saving failed */  
	SAVE_PACK_FAILED, /**< Plugin failed to pack file */  
	SAVE_NO_FILENAME, /**< File has no filename */  
	SAVE_CANCELLED, /**< Saving was cancelled */  
};

/**
 * @brief Result of trying to open file in MergeEditView display
 */
typedef enum {
	OPENRESULTS_SUCCESS,       /**< Open succeeded */
	OPENRESULTS_FAILED_BINARY, /**< Open failed because one or both files were binary */
	OPENRESULTS_FAILED_MISC    /**< Open failed for some other reason */
} OPENRESULTS_TYPE;

enum MERGEVIEW_INDEX_TYPE
{
	MERGEVIEW_LEFT = 0,         /**< Left MergeView */
	MERGEVIEW_RIGHT,            /**< Right MergeView */
	MERGEVIEW_LEFT_DETAIL = 10, /**< Left DetailView */
	MERGEVIEW_RIGHT_DETAIL,     /**< Right DetailView */
};

/**
 * @brief Types for buffer. Buffer's type defines behavior
 * of buffer when saving etc.
 * 
 * Difference between BUFFER_NORMAL and BUFFER_NORMAL_NAMED is
 * that _NAMED has description text given and which is shown
 * instead of filename.
 *
 * BUFFER_UNNAMED is created empty buffer (scratchpad), it has
 * no filename, and default description is given for it. After
 * this buffer is saved it becomes _SAVED. It is not equal to
 * NORMAL_NAMED, since scratchpads don't have plugins etc.
 */
enum BUFFERTYPE
{
	BUFFER_NORMAL = 0, /**< Normal, file loaded from disk */
	BUFFER_NORMAL_NAMED, /**< Normal, description given */
	BUFFER_UNNAMED, /**< Empty, created buffer */
	BUFFER_UNNAMED_SAVED, /**< Empty buffer saved with filename */
};

struct DiffFileInfo;
class CMergeEditView;
class CMergeDiffDetailView;
class PackingInfo;
class PrediffingInfo;
class CChildFrame;
class CDirDoc;

/**
 * @brief Document class for merging two files
 */
class CMergeDoc : public CDocument
{
public:
	enum FileChange
	{
		FileNoChange,
		FileChanged,
		FileRemoved,
	};

// Attributes
public:
	CDiffTextBuffer *m_ptBuf[2]; /**< Left/Right side text buffer */

protected: // create from serialization only
	CMergeDoc();
	DECLARE_DYNCREATE(CMergeDoc)

	// Operations
public:	
	DiffFileInfo *m_pSaveFileInfo[2];
	DiffFileInfo *m_pRescanFileInfo[2];
	DiffList m_diffList;
	UINT m_nTrivialDiffs; /**< Amount of trivial (ignored) diffs */
	PathContext m_filePaths; /**< Filepaths for this document */
	/// String of concatenated filenames as text to apply plugins filter to
	String m_strBothFilenames;

	int GetActiveMergeViewIndexType() const;
	CMergeEditView * GetActiveMergeView();
	void UpdateHeaderPath(int pane);
	void UpdateHeaderActivity(int pane, bool bActivate);
	void RefreshOptions();
	void UpdateResources();
	OPENRESULTS_TYPE OpenDocs(FileLocation filelocLeft, FileLocation filelocRight,
		BOOL bROLeft, BOOL bRORight);
	OPENRESULTS_TYPE ReloadDoc(int index);
	void RescanIfNeeded(float timeOutInSecond);
	int Rescan(BOOL &bBinary, BOOL &bIdentical, BOOL bForced = FALSE);
	void ShowRescanError(int nRescanResult, BOOL bIdentical);
	void AddUndoAction(UINT nBegin, UINT nEnd, UINT nDiff, int nBlanks, BOOL bInsert, CMergeEditView *pList);
	bool Undo();
	void CopyAllList(int srcPane, int dstPane);
	void CopyMultipleList(int srcPane, int dstPane, int firstDiff, int lastDiff);
	bool SanityCheckDiff(DIFFRANGE dr) const;
	bool ListCopy(int srcPane, int dstPane, int nDiff = -1, bool bGroupWithPrevious = false);
	bool TrySaveAs(CString &strPath, int &nLastErrorCode, String & sError,
		int nBuffer, PackingInfo * pInfoTempUnpacker);
	bool DoSave(LPCTSTR szPath, BOOL &bSaveSuccess, int nBuffer);
	bool DoSaveAs(LPCTSTR szPath, BOOL &bSaveSuccess, int nBuffer);
	int RightLineInMovedBlock(int leftLine);
	int LeftLineInMovedBlock(int rightLine);
	void SetEditedAfterRescan(int nBuffer);

	void SetUnpacker(PackingInfo * infoUnpacker);
	void SetPrediffer(PrediffingInfo * infoPrediffer);
	void GetPrediffer(PrediffingInfo * infoPrediffer);
	void SetMergeViews(CMergeEditView * pLeft, CMergeEditView * pRight);
	void SetMergeDetailViews(CMergeDiffDetailView * pLeft, CMergeDiffDetailView * pRight);
	void SetDirDoc(CDirDoc * pDirDoc);
	void DirDocClosing(CDirDoc * pDirDoc);
	BOOL CloseNow();
	void SwapFiles();

	CMergeEditView * GetLeftView() const { return m_pView[MERGE_VIEW_LEFT]; }
	CMergeEditView * GetRightView() const { return m_pView[MERGE_VIEW_RIGHT]; }
	CMergeEditView * GetView(int pane) const { return m_pView[pane]; }
	CMergeDiffDetailView * GetLeftDetailView() const { return m_pDetailView[0]; }
	CMergeDiffDetailView * GetRightDetailView() const { return m_pDetailView[1]; }
	CMergeDiffDetailView * GetDetailView(int pane) const { return m_pDetailView[pane]; }
	CChildFrame * GetParentFrame();
	const FileTextEncoding & GetEncoding(int file) const
			{ return m_ptBuf[file]->getEncoding(); }

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL SaveModified();
	virtual void DeleteContents ();
	virtual void SetTitle(LPCTSTR lpszTitle);
	//}}AFX_VIRTUAL

// Implementation in MergeDocLineDiffs.cpp
public:
	typedef enum { BYTEDIFF, WORDDIFF } DIFFLEVEL;
	void Showlinediff(CMergeEditView * pView, DIFFLEVEL difflvl);
	void Showlinediff(CMergeDiffDetailView * pView, DIFFLEVEL difflvl);
	void GetWordDiffArray(int nLineIndex, std::vector<wdiff*> *pworddiffs);
private:
	void Computelinediff(CCrystalTextView * pView, CCrystalTextView * pOther, int line, CRect * rc1, CRect * rc2, DIFFLEVEL difflvl);
// End MergeDocLineDiffs.cpp

// Implementation in MergeDocEncoding.cpp
public:
	void DoFileEncodingDialog();
// End MergeDocEncoding.cpp

// Implementation
public:
	FileChange IsFileChangedOnDisk(LPCTSTR szPath, DiffFileInfo &dfi,
		bool bSave, int nBuffer);
	bool PromptAndSaveIfNeeded(BOOL bAllowCancel);
	std::vector<CMergeEditView*> undoTgt;
	std::vector<CMergeEditView*>::iterator curUndo;
	void FlushAndRescan(BOOL bForced = FALSE);
	void SetCurrentDiff(int nDiff);
	int GetCurrentDiff() { return m_nCurDiff; }
	virtual ~CMergeDoc();
	virtual void OnFileEvent (WPARAM wEvent, LPCTSTR pszPathName);
	bool GetMergingMode() const;
	void SetMergingMode(bool bMergingMode);
	void SetDetectMovedBlocks(bool bDetectMovedBlocks);
	bool IsMixedEOL(int nBuffer) const;

// implementation methods
private:
	bool GetBreakType() const;
	bool GetByteColoringOption() const;
	bool IsValidCodepageForMergeEditor(unsigned cp) const;
	void SanityCheckCodepage(FileLocation & fileinfo);
	DWORD LoadOneFile(int index, String filename, BOOL readOnly, const FileTextEncoding & encoding);
	int LoadFile(CString sFileName, int nBuffer, BOOL & readOnly, const FileTextEncoding & encoding);

// Implementation data
protected:
	int m_nCurDiff; /**< Selected diff, 0-based index, -1 if no diff selected */
	CMergeEditView * m_pView[MERGE_VIEW_COUNT]; /**< Pointer to left/right view */
	CMergeDiffDetailView * m_pDetailView[2];
	CDirDoc * m_pDirDoc;
	bool m_bEnableRescan; /**< Automatic rescan enabled/disabled */
	COleDateTime m_LastRescan; /**< Time of last rescan (for delaying) */ 
	CDiffWrapper m_diffWrapper;
	/// information about the file packer/unpacker
	PackingInfo * m_pInfoUnpacker;
	String m_strDesc[2]; /**< Left/right side description text */
	BUFFERTYPE m_nBufferType[2];
	bool m_bMergingMode; /**< Merging or Edit mode */
	bool m_bEditAfterRescan[2]; /**< Left/right doc edited after rescanning */
	TempFile m_tempFiles[2]; /**< Temp files for compared files */
	bool m_bMixedEol; /**< Does this document have mixed EOL style? */

// friend access
	friend class RescanSuppress;


// Generated message map functions
protected:
	//{{AFX_MSG(CMergeDoc)
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveLeft();
	afx_msg void OnFileSaveRight();
	afx_msg void OnFileSaveAsLeft();
	afx_msg void OnFileSaveAsRight();
	afx_msg void OnUpdateStatusNum(CCmdUI* pCmdUI);
	afx_msg void OnFileEncoding();
	afx_msg void OnToolsGenerateReport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void PrimeTextBuffers();
	void AdjustDiffBlocks();
	void AdjustDiffBlock(DiffMap & diffmap, const DIFFRANGE & diffrange, int lo0, int hi0, int lo1, int hi1);
	int GetMatchCost(const CString &Line0, const CString &Line1);
	void FlagMovedLines(MovedLines * pMovedLines, CDiffTextBuffer * pBuffer1,
		CDiffTextBuffer * pBuffer2);
	String GetFileExt(LPCTSTR sFileName, LPCTSTR sDescription);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGEDOC_H__BBCD4F90_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)

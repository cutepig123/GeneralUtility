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
 * @file  DirDoc.h
 *
 * @brief Declaration file for CDirDoc
 *
 */
// ID line follows -- this is updated by SVN
// $Id: DirDoc.h 6136 2008-12-01 17:04:25Z kimmov $

#if !defined(AFX_DIRDOC_H__0B17B4C1_356F_11D1_95CD_444553540000__INCLUDED_)
#define AFX_DIRDOC_H__0B17B4C1_356F_11D1_95CD_444553540000__INCLUDED_
#pragma once

#include "DiffThread.h"
#include "PluginManager.h"

class CDirView;
class CMergeDoc;
class CHexMergeDoc;
typedef CTypedPtrList<CPtrList, CMergeDoc *> MergeDocPtrList;
typedef CTypedPtrList<CPtrList, CHexMergeDoc *> HexMergeDocPtrList;
class DirDocFilterGlobal;
class DirDocFilterByExtension;
class CustomStatusCursor;
class CTempPathContext;
struct FileActionItem;

/////////////////////////////////////////////////////////////////////////////
// CDirDoc document

/**
 * @brief Class for folder compare data.
 * This class is "document" class for folder compare. It has compare context,
 * which in turn has a list of differences and other compare result data.
 * This class also has compare statistics which are updated during compare.
 * GUI calls this class to operate with results.
 */
class CDirDoc : public CDocument
{
protected:
	CDirDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDirDoc)

// Attributes
public:
	CTempPathContext *m_pTempPathContext;
// Operations
public:
	BOOL CloseMergeDocs();
	CDirView * GetMainView();
	CMergeDoc * GetMergeDocForDiff(BOOL * pNew);
	CHexMergeDoc * GetHexMergeDocForDiff(BOOL * pNew);
	BOOL ReusingDirDoc();
	bool CanFrameClose();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual void SetTitle(LPCTSTR lpszTitle);
	protected:
	virtual BOOL OnNewDocument();
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	void InitCompare(const PathContext & paths, BOOL bRecursive, CTempPathContext *);
	void Rescan();
	BOOL GetRecursive() const { return m_bRecursive; }
	BOOL GetReadOnly(BOOL bLeft) const;
	void SetReadOnly(BOOL bLeft, BOOL bReadOnly);
	BOOL HasDirView() { return m_pDirView != NULL; }
	void RefreshOptions();
	void CompareReady();
	void UpdateChangedItem(PathContext & paths,
		UINT nDiffs, UINT nTrivialDiffs, BOOL bIdentical);
	UINT_PTR FindItemFromPaths(LPCTSTR pathLeft, LPCTSTR pathRight);
	void SetDiffSide(UINT diffcode, int idx);
	void SetDiffCompare(UINT diffcode, int idx);
	void UpdateResources();
	void InitStatusStrings();
	void UpdateStatusFromDisk(UINT_PTR diffPos, BOOL bLeft, BOOL bRight);
	void ReloadItemStatus(UINT nIdx, BOOL bLeft, BOOL bRight);
	void Redisplay();
	virtual ~CDirDoc();
	void SetDirView( CDirView *newView ); // TODO Perry
	void AddMergeDoc(CMergeDoc * pMergeDoc);
	void AddHexMergeDoc(CHexMergeDoc * pHexMergeDoc);
	void MergeDocClosing(CDocument * pMergeDoc);
	CDiffThread m_diffThread;
	void SetDiffStatus(UINT diffcode, UINT mask, int idx);
	void SetDiffCounts(UINT diffs, UINT ignored, int idx);
	void UpdateDiffAfterOperation(const FileActionItem & act, UINT_PTR pos);
	void UpdateHeaderPath(BOOL bLeft);
	void AbortCurrentScan();
	bool IsCurrentScanAbortable() const;
	void SetDescriptions(const String &strLeftDesc, const String &strRightDesc);
	void ApplyLeftDisplayRoot(String &);
	void ApplyRightDisplayRoot(String &);

	void SetPluginPrediffSetting(LPCTSTR filteredFilenames, int newsetting);
	void SetPluginPrediffer(LPCTSTR filteredFilenames, const CString & prediffer);
	void FetchPluginInfos(LPCTSTR filteredFilenames, 
	                      PackingInfo ** infoUnpacker, PrediffingInfo ** infoPrediffer);
	BOOL IsShowable(const DIFFITEM & di);

	BOOL HasDiffs() const { return m_pCtxt != NULL; }
	const CDiffContext & GetDiffContext() const { return *m_pCtxt; }
	const DIFFITEM & GetDiffByKey(UINT_PTR key) const { return m_pCtxt->GetDiffAt(key); }
	DIFFITEM & GetDiffRefByKey(UINT_PTR key) { return m_pCtxt->GetDiffRefAt(key); }
	String GetLeftBasePath() const { return m_pCtxt->GetNormalizedLeft(); }
	String GetRightBasePath() const { return m_pCtxt->GetNormalizedRight(); }
	void RemoveDiffByKey(UINT_PTR key) { m_pCtxt->RemoveDiff(key); }
	void SetMarkedRescan() {m_bMarkedRescan = TRUE; }
	struct AllowUpwardDirectory
	{
		enum ReturnCode
		{
			Never,
			No,
			ParentIsRegularPath,
			ParentIsTempPath
		};
	};
	AllowUpwardDirectory::ReturnCode AllowUpwardDirectory(String &leftParent, String &rightParent);
	void SetItemViewFlag(UINT_PTR key, UINT flag, UINT mask);
	void SetItemViewFlag(UINT flag, UINT mask);
	const CompareStats * GetCompareStats() const { return m_pCompareStats; };
	bool IsArchiveFolders();

protected:
	void LoadLineFilterList();

	// Generated message map functions
	//{{AFX_MSG(CDirDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Implementation data
private:
	CDiffContext *m_pCtxt; /**< Pointer to compare results-data */
	CDirView *m_pDirView; /**< Pointer to GUI */
	CompareStats *m_pCompareStats; /**< Compare statistics */
	MergeDocPtrList m_MergeDocs; /**< List of file compares opened from this compare */
	HexMergeDocPtrList m_HexMergeDocs; /**< List of hex file compares opened from this compare */
	BOOL m_bROLeft; /**< Is left side read-only */
	BOOL m_bRORight; /**< Is right side read-only */
	BOOL m_bRecursive; /**< Is current compare recursive? */
	CustomStatusCursor * m_statusCursor;
	String m_strLeftDesc; /**< Left side desription text */
	String m_strRightDesc; /**< Left side desription text */
	PluginManager m_pluginman;
	BOOL m_bReuseCloses; /**< Are we closing because of reuse? */
	BOOL m_bMarkedRescan; /**< If TRUE next rescan scans only marked items */
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRDOC_H__0B17B4C1_356F_11D1_95CD_444553540000__INCLUDED_)

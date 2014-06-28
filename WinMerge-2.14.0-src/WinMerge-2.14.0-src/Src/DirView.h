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
 *  @file DirView.h
 *
 *  @brief Declaration of class CDirView
 */
//
// ID line follows -- this is updated by SVN
// $Id: DirView.h 7465 2010-12-07 19:29:29Z gerundt $

#if !defined(AFX_DirView_H__16E7C721_351C_11D1_95CD_444553540000__INCLUDED_)
#define AFX_DirView_H__16E7C721_351C_11D1_95CD_444553540000__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CDirView view
#include <afxcview.h>
#include "SortHeaderCtrl.h"
#include "UnicodeString.h"

class FileActionScript;

struct DIFFITEM;

typedef enum { eMain, eContext } eMenuType;

class CDirDoc;
class CDirFrame;

class PackingInfo;
class PathContext;
class DirCompProgressDlg;
class CompareStats;
struct DirColInfo;
class CLoadSaveCodepageDlg;
class CShellContextMenu;

struct ViewCustomFlags
{
	enum
	{
		// We use extra bits so that no valid values are 0
		// and each set of flags is in a different hex digit
		// to make debugging easier
		// These can always be packed down in the future
		INVALID_CODE = 0,
		VISIBILITY = 0x3, VISIBLE = 0x1, HIDDEN = 0x2, EXPANDED = 0x4
	};
};

/**
 * @brief Position value for special items (..) in directory compare view.
 */
const UINT_PTR SPECIAL_ITEM_POS = (UINT_PTR) - 1L;

/** Default column width in directory compare */
const UINT DefColumnWidth = 150;

/**
 * @brief Directory compare results view.
 *
 * Directory compare view is list-view based, so it shows one result (for
 * folder or file, commonly called as 'item') in one line. User can select
 * visible columns, re-order columns, sort by column etc.
 *
 * Actual data is stored in CDiffContext in CDirDoc. Dircompare items and
 * CDiffContext items are linked by storing POSITION of CDiffContext item
 * as CDirView listitem key.
 */
class CDirView : public CListView
{
	class DirItemEnumerator;
	friend DirItemEnumerator;
protected:
	CDirView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDirView)

// Attributes
public:
	CDirDoc* GetDocument(); // non-debug version is inline

	// const version, for const methods to be able to call
	const CDirDoc * GetDocument() const { return const_cast<CDirView *>(this)->GetDocument(); }

// Operations
public:
	CDirFrame * GetParentFrame();

	void StartCompare(CompareStats *pCompareStats);
	void Redisplay();
	void RedisplayChildren(UINT_PTR diffpos, int level, UINT &index, int &alldiffs);
	void UpdateResources();
	void LoadColumnHeaderItems();
	UINT_PTR GetItemKey(int idx) const;
	int GetItemIndex(UINT_PTR key);
	// for populating list
	void DeleteAllDisplayItems();
	void SetColumnWidths();

	void SortColumnsAppropriately();

	UINT GetSelectedCount() const;
	int GetFirstSelectedInd();
	DIFFITEM & GetNextSelectedInd(int &ind);
	DIFFITEM & GetItemAt(int ind);
	void AddParentFolderItem(BOOL bEnable);
	void RefreshOptions();

	LRESULT HandleMenuMessage(UINT message, WPARAM wParam, LPARAM lParam);

// Implementation types
private:
	typedef enum { SIDE_LEFT = 1, SIDE_RIGHT } SIDE_TYPE;

// Implementation in DirActions.cpp
private:
	BOOL GetSelectedDirNames(String& strLeft, String& strRight) const;
	BOOL GetSelectedFileNames(String& strLeft, String& strRight) const;
	String GetSelectedFileName(SIDE_TYPE stype) const;
	void GetItemFileNames(int sel, String& strLeft, String& strRight) const;
	void GetItemFileNames(int sel, PathContext * paths) const;
	void FormatEncodingDialogDisplays(CLoadSaveCodepageDlg * dlg);
	BOOL IsItemLeftOnly(int code);
	BOOL IsItemRightOnly(int code);
	BOOL IsItemCopyableToLeft(const DIFFITEM & di) const;
	BOOL IsItemCopyableToRight(const DIFFITEM & di) const;
	BOOL IsItemDeletableOnLeft(const DIFFITEM & di) const;
	BOOL IsItemDeletableOnRight(const DIFFITEM & di) const;
	BOOL IsItemDeletableOnBoth(const DIFFITEM & di) const;
	BOOL IsItemOpenable(const DIFFITEM & di) const;
	BOOL AreItemsOpenable(const DIFFITEM & di1, const DIFFITEM & di2) const;
	BOOL IsItemOpenableOnLeft(const DIFFITEM & di) const;
	BOOL IsItemOpenableOnRight(const DIFFITEM & di) const;
	BOOL IsItemOpenableOnLeftWith(const DIFFITEM & di) const;
	BOOL IsItemOpenableOnRightWith(const DIFFITEM & di) const;
	BOOL IsItemCopyableToOnLeft(const DIFFITEM & di) const;
	BOOL IsItemCopyableToOnRight(const DIFFITEM & di) const;
	void DoCopyLeftToRight();
	void DoCopyRightToLeft();
	void DoDelLeft();
	void DoDelRight();
	void DoDelBoth();
	void DoDelAll();
	void DoCopyLeftTo();
	void DoCopyRightTo();
	void DoMoveLeftTo();
	void DoMoveRightTo();
	void DoOpen(SIDE_TYPE stype);
	void DoOpenWith(SIDE_TYPE stype);
	void DoOpenWithEditor(SIDE_TYPE stype);
	void ApplyPluginPrediffSetting(int newsetting);
	void ConfirmAndPerformActions(FileActionScript & actions, int selCount);
	BOOL ConfirmActionList(const FileActionScript & actions, int selCount);
	void PerformActionList(FileActionScript & actions);
	void UpdateAfterFileScript(FileActionScript & actionList);
	UINT MarkSelectedForRescan();
	void DoFileEncodingDialog();
	void DoUpdateFileEncodingDialog(CCmdUI* pCmdUI);
	BOOL DoItemRename(LPCTSTR szNewItemName);
	BOOL RenameOnSameDir(LPCTSTR szOldFileName, LPCTSTR szNewFileName);
// End DirActions.cpp
	void ReflectGetdispinfo(NMLVDISPINFO *);

// Implementation in DirViewColHandler.cpp
public:
	void UpdateColumnNames();
	void SetColAlignments();
	// class CompareState is used to pass parameters to the PFNLVCOMPARE callback function.
	class CompareState
	{
	private:
		const CDirView *const pView;
		const CDiffContext *const pCtxt;
		const int sortCol;
		const bool bSortAscending;
	public:
		CompareState(const CDirView *, int sortCol, bool bSortAscending);
		static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	} friend;
	void UpdateDiffItemStatus(UINT nIdx);
private:
	void InitiateSort();
	void NameColumn(int id, int subitem);
	int AddNewItem(int i, UINT_PTR diffpos, int iImage, int iIndent);
	bool IsDefaultSortAscending(int col) const;
	int ColPhysToLog(int i) const { return m_invcolorder[i]; }
	int ColLogToPhys(int i) const { return m_colorder[i]; } /**< -1 if not displayed */
	String GetColDisplayName(int col) const;
	String GetColDescription(int col) const;
	int GetColLogCount() const;
	void LoadColumnOrders();
	void ValidateColumnOrdering();
	void ClearColumnOrders();
	void ResetColumnOrdering();
	void MoveColumn(int psrc, int pdest);
	CString GetColRegValueNameBase(int col) const;
	String ColGetTextToDisplay(const CDiffContext *pCtxt, int col, const DIFFITEM & di);
	int ColSort(const CDiffContext *pCtxt, int col, const DIFFITEM & ldi, const DIFFITEM &rdi) const;
// End DirViewCols.cpp

// Implementation in DirViewColItems.cpp
	int GetColDefaultOrder(int col) const;
	const DirColInfo * DirViewColItems_GetDirColInfo(int col) const;
	bool IsColName(int col) const;
	bool IsColLmTime(int col) const;
	bool IsColRmTime(int col) const;
	bool IsColStatus(int col) const;
	bool IsColStatusAbbr(int col) const;
// End DirViewColItems.cpp

private:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirView)
public:
	virtual void OnInitialUpdate();
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDirView();
	int GetFocusedItem();
	int GetFirstDifferentItem();
	int GetLastDifferentItem();
	int GetColImage(const DIFFITEM & di) const;
	int GetDefaultColImage() const;
	int AddSpecialItems();
	void GetCurrentColRegKeys(CStringArray & colKeys);
	void WarnContentsChanged(const CString & failedPath);
	void OpenSpecialItems(UINT_PTR pos1, UINT_PTR pos2);
	bool OpenOneItem(UINT_PTR pos1, DIFFITEM **di1, DIFFITEM **di2,
			String &path1, String &path2, int & sel1, bool & isDir);
	bool OpenTwoItems(UINT_PTR pos1, UINT_PTR pos2, DIFFITEM **di1, DIFFITEM **di2,
			String &path1, String &path2, int & sel1, int & sel2, bool & isDir);
	bool CreateFoldersPair(DIFFITEM & di, bool side1, String &newFolder);

// Implementation data
protected:
	CString GenerateReport();
	CSortHeaderCtrl m_ctlSortHeader;
	CImageList m_imageList;
	CImageList m_imageState;
	CListCtrl * m_pList;
	int m_numcols;
	int m_dispcols;
	std::vector<int> m_colorder; /**< colorder[logical#]=physical# */
	std::vector<int> m_invcolorder; /**< invcolorder[physical]=logical# */
	BOOL m_bEscCloses; /**< Cached value for option for ESC closing window */
	CFont m_font; /**< User-selected font */
	UINT m_nHiddenItems; /**< Count of items we have hidden */
	BOOL m_bTreeMode; /**< TRUE if tree mode is on*/
	DirCompProgressDlg * m_pCmpProgressDlg;
	clock_t m_compareStart; /**< Starting process time of the compare */
	BOOL m_bUserCancelEdit; /**< TRUE if the user cancels rename */
	String m_lastCopyFolder; /**< Last Copy To -target folder. */

	CShellContextMenu* m_pShellContextMenuLeft; /**< Shell context menu for group of left files */
	CShellContextMenu* m_pShellContextMenuRight; /**< Shell context menu for group of right files */
	HMENU m_hCurrentMenu; /**< Current shell context menu (either left or right) */

	// Generated message map functions
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	//{{AFX_MSG(CDirView)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDirCopyRightToLeft();
	afx_msg void OnCtxtDirCopyRightToLeft();
	afx_msg void OnUpdateDirCopyRightToLeft(CCmdUI* pCmdUI);
	afx_msg void OnDirCopyLeftToRight();
	afx_msg void OnCtxtDirCopyLeftToRight();
	afx_msg void OnUpdateDirCopyLeftToRight(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxtDirCopyRightToLeft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxtDirCopyLeftToRight(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirDelLeft();
	afx_msg void OnUpdateCtxtDirDelLeft(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirDelRight();
	afx_msg void OnUpdateCtxtDirDelRight(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirDelBoth();
	afx_msg void OnUpdateCtxtDirDelBoth(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirOpenLeft();
	afx_msg void OnUpdateCtxtDirOpenLeft(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirOpenLeftWith();
	afx_msg void OnUpdateCtxtDirOpenLeftWith(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirOpenRight();
	afx_msg void OnUpdateCtxtDirOpenRight(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirOpenRightWith();
	afx_msg void OnUpdateCtxtDirOpenRightWith(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirOpenRightWithEditor();
	afx_msg void OnUpdateCtxtDirOpenRightWithEditor(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirOpenLeftWithEditor();
	afx_msg void OnUpdateCtxtDirOpenLeftWithEditor(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirCopyLeftTo();
	afx_msg void OnUpdateCtxtDirCopyLeftTo(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirCopyRightTo();
	afx_msg void OnUpdateCtxtDirCopyRightTo(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFirstdiff();
	afx_msg void OnUpdateFirstdiff(CCmdUI* pCmdUI);
	afx_msg void OnLastdiff();
	afx_msg void OnUpdateLastdiff(CCmdUI* pCmdUI);
	afx_msg void OnNextdiff();
	afx_msg void OnUpdateNextdiff(CCmdUI* pCmdUI);
	afx_msg void OnPrevdiff();
	afx_msg void OnUpdatePrevdiff(CCmdUI* pCmdUI);
	afx_msg void OnCurdiff();
	afx_msg void OnUpdateCurdiff(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSave(CCmdUI* pCmdUI);
	afx_msg LRESULT OnUpdateUIMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRefresh();
	afx_msg void OnUpdateRefresh(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEditColumns();
	afx_msg void OnLeftReadOnly();
	afx_msg void OnUpdateLeftReadOnly(CCmdUI* pCmdUI);
	afx_msg void OnRightReadOnly();
	afx_msg void OnUpdateRightReadOnly(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatusLeftRO(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatusRightRO(CCmdUI* pCmdUI);
	afx_msg void OnCustomizeColumns();
	afx_msg void OnCtxtOpenWithUnpacker();
	afx_msg void OnUpdateCtxtOpenWithUnpacker(CCmdUI* pCmdUI);
	afx_msg void OnToolsGenerateReport();
	afx_msg void OnCtxtDirZipLeft();
	afx_msg void OnCtxtDirZipRight();
	afx_msg void OnCtxtDirZipBoth();
	afx_msg void OnCtxtDirZipBothDiffsOnly();
	afx_msg void OnUpdateCtxtDir(CCmdUI* pCmdUI);
	afx_msg void OnSelectAll();
	afx_msg void OnUpdateSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnPluginPredifferMode(UINT nID);
	afx_msg void OnUpdatePluginPredifferMode(CCmdUI* pCmdUI);
	afx_msg void OnCopyLeftPathnames();
	afx_msg void OnCopyRightPathnames();
	afx_msg void OnCopyBothPathnames();
	afx_msg void OnCopyFilenames();
	afx_msg void OnUpdateCopyFilenames(CCmdUI* pCmdUI);
	afx_msg void OnItemRename();
	afx_msg void OnUpdateItemRename(CCmdUI* pCmdUI);
	afx_msg void OnHideFilenames();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCtxtDirMoveLeftTo();
	afx_msg void OnUpdateCtxtDirMoveLeftTo(CCmdUI* pCmdUI);
	afx_msg void OnCtxtDirMoveRightTo();
	afx_msg void OnUpdateCtxtDirMoveRightTo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHideFilenames(CCmdUI* pCmdUI);
	afx_msg void OnDelete();
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnMarkedRescan();
	afx_msg void OnUpdateStatusNum(CCmdUI* pCmdUI);
	afx_msg void OnViewShowHiddenItems();
	afx_msg void OnUpdateViewShowHiddenItems(CCmdUI* pCmdUI);
	afx_msg void OnViewTreeMode();
	afx_msg void OnUpdateViewTreeMode(CCmdUI* pCmdUI);
	afx_msg void OnViewExpandAllSubdirs();
	afx_msg void OnUpdateViewExpandAllSubdirs(CCmdUI* pCmdUI);
	afx_msg void OnViewCollapseAllSubdirs();
	afx_msg void OnUpdateViewCollapseAllSubdirs(CCmdUI* pCmdUI);
	afx_msg void OnMergeCompare();
	afx_msg void OnMergeCompareXML();
	afx_msg void OnMergeCompareHex();
	afx_msg void OnUpdateMergeCompare(CCmdUI *pCmdUI);
	afx_msg void OnViewCompareStatistics();
	afx_msg void OnFileEncoding();
	afx_msg void OnUpdateFileEncoding(CCmdUI* pCmdUI);
	afx_msg void OnHelp();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPluginsList();
	afx_msg void OnExpandFolder();
	afx_msg void OnCollapseFolder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	BOOL OnHeaderBeginDrag(LPNMHEADER hdr, LRESULT* pResult);
	BOOL OnHeaderEndDrag(LPNMHEADER hdr, LRESULT* pResult);

private:
	void OpenSelection(PackingInfo * infoUnpacker = NULL);
	void OpenSelectionHex();
	bool GetSelectedItems(int * sel1, int * sel2);
	void OpenParentDirectory();
	void DoUpdateDirCopyRightToLeft(CCmdUI* pCmdUI, eMenuType menuType);
	void DoUpdateDirCopyLeftToRight(CCmdUI* pCmdUI, eMenuType menuType);
	void ModifyPopup(CMenu * pPopup, int nStringResource, int nMenuId, LPCTSTR szPath);
	void DoUpdateCtxtDirDelLeft(CCmdUI* pCmdUI);
	void DoUpdateCtxtDirDelRight(CCmdUI* pCmdUI);
	void DoUpdateCtxtDirDelBoth(CCmdUI* pCmdUI);
	void DoUpdateOpen(CCmdUI* pCmdUI);
	void DoUpdateOpenLeft(CCmdUI* pCmdUI);
	void DoUpdateOpenRight(CCmdUI* pCmdUI);
	void DoUpdateOpenLeftWith(CCmdUI* pCmdUI);
	void DoUpdateOpenRightWith(CCmdUI* pCmdUI);
	void DoUpdateDelete(CCmdUI* pCmdUI);
	void DoUpdateCopyFilenames(CCmdUI* pCmdUI);
	void DoUpdateCtxtDirCopyLeftTo(CCmdUI* pCmdUI);
	void DoUpdateCtxtDirCopyRightTo(CCmdUI* pCmdUI);
	void DoUpdateCtxtDirMoveLeftTo(CCmdUI* pCmdUI);
	void DoUpdateCtxtDirMoveRightTo(CCmdUI* pCmdUI);
	const DIFFITEM & GetDiffItem(int sel) const;
	DIFFITEM & GetDiffItemRef(int sel);
	int GetSingleSelectedItem() const;
	bool IsItemNavigableDiff(const DIFFITEM & di) const;
	void MoveFocus(int currentInd, int i, int selCount);
	void SaveColumnWidths();
	void SaveColumnOrders();
	void FixReordering();
	void HeaderContextMenu(CPoint point, int i);
	void ListContextMenu(CPoint point, int i);
	bool ListShellContextMenu(SIDE_TYPE side);
	CShellContextMenu* GetCorrespondingShellContextMenu(HMENU hMenu) const;
	void ReloadColumns();
	void ResetColumnWidths();
	BOOL IsLabelEdit();
	BOOL IsItemSelectedSpecial();
	void CollapseSubdir(int sel);
	void ExpandSubdir(int sel);
};


#ifndef _DEBUG  // debug version in DirView.cpp
inline CDirDoc* CDirView::GetDocument()
{ return (CDirDoc*)m_pDocument; }
#endif


String NumToStr(int n);


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DirView_H__16E7C721_351C_11D1_95CD_444553540000__INCLUDED_)


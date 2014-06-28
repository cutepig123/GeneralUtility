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
 * @file  MergeEditView.h
 *
 * @brief Declaration file for CMergeEditView
 *
 */
// ID line follows -- this is updated by SVN
// $Id: MergeEditView.h 6588 2009-03-23 20:59:52Z kimmov $

#if !defined(AFX_MERGEEDITVIEW_H__0CE31CFD_4BEE_4378_ADB4_B7C9F50A9F53__INCLUDED_)
#define AFX_MERGEEDITVIEW_H__0CE31CFD_4BEE_4378_ADB4_B7C9F50A9F53__INCLUDED_

/** 
 * @brief Color settings.
 */
struct COLORSETTINGS
{
	COLORREF	clrDiff;			/**< Difference color */
	COLORREF	clrSelDiff;			/**< Selected difference color */
	COLORREF	clrDiffDeleted;		/**< Difference deleted color */
	COLORREF	clrSelDiffDeleted;	/**< Selected difference deleted color */
	COLORREF	clrDiffText;		/**< Difference text color */
	COLORREF	clrSelDiffText;		/**< Selected difference text color */
	COLORREF	clrTrivial;			/**< Ignored difference color */
	COLORREF	clrTrivialDeleted;	/**< Ignored difference deleted color */
	COLORREF	clrTrivialText;		/**< Ignored difference text color */
	COLORREF	clrMoved;			/**< Moved block color */
	COLORREF	clrMovedDeleted;	/**< Moved block deleted color */
	COLORREF	clrMovedText;		/**< Moved block text color */
	COLORREF	clrSelMoved;		/**< Selected moved block color */
	COLORREF	clrSelMovedDeleted;	/**< Selected moved block deleted color */
	COLORREF	clrSelMovedText;	/**< Selected moved block text color */
	COLORREF	clrWordDiff;		/**< Word difference color */
	COLORREF	clrWordDiffText;	/**< Word difference text color */
	COLORREF	clrSelWordDiff;		/**< Selected word difference color */
	COLORREF	clrSelWordDiffText;	/**< Selected word difference text color */
};

/** 
 * @brief Non-diff lines shown above diff when scrolling to it
 */
const UINT CONTEXT_LINES_ABOVE = 5;

/** 
 * @brief Non-diff lines shown below diff when scrolling to it
 */
const UINT CONTEXT_LINES_BELOW = 3;


#define FLAG_RESCAN_WAITS_FOR_IDLE   1


/////////////////////////////////////////////////////////////////////////////
// CMergeEditView view
#include "edtlib.h"
#include "GhostTextView.h"

class IMergeEditStatus;
class CLocationView;
class CMergeDoc;
struct DIFFRANGE;

/**
This class is the base class for WinMerge editor panels.
It hooks the painting of ghost lines (GetLineColors), the shared
scrollbar (OnUpdateSibling...).
It offers the UI interface commands to work with diffs 

@todo
If we keep GetLineColors here, we should clear DIFF flag here
and not in CGhostTextBuffer (when insertText/deleteText). 
Small problem... This class doesn't derives from CGhostTextBuffer... 
We could define a new class which derives from CGhostTextBuffer to clear the DIFF flag.
and calls a virtual function for additional things to do on the flag.
Maybe in the future...
*/
class CMergeEditView : public CGhostTextView
{
protected:
	CMergeEditView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMergeEditView)
	CCrystalParser m_xParser; /**< Syntax parser used for syntax highlighting. */

// Attributes
public:
	/**
	 * Index of pane this view is attached to.
	 * This indicates the pane number the view is attached to. If we swap panes
	 * then these indexes are changed.
	 */
	int m_nThisPane;
	IMergeEditStatus * m_piMergeEditStatus; /**< interface to status bar */

protected:
	/**
	 * Are automatic rescans enabled?
	 * If automatic rescans are enabled then we rescan files after edit
	 * events, unless timer suppresses rescan. We suppress rescans within
	 * certain time from previous rescan.
	 */
	bool m_bAutomaticRescan;

private:
	/** 
	This flag is set when we receive an OnTimer command, and we want 
	to wait for theApp::OnIdle before processing it 
	*/
	BOOL fTimerWaitingForIdle;
	COLORSETTINGS m_cachedColors; /**< Cached color settings */

	/// active prediffer ID : helper to check the radio button
	int m_CurrentPredifferID;

	bool m_bCurrentLineIsDiff; /**< TRUE if cursor is in diff line */
	CLocationView * m_pLocationView; /**< Pointer to locationview */

// Operations
public:
	void RefreshOptions();
	bool EnableRescan(bool bEnable);
	bool IsReadOnly(int pane);
	void ShowDiff(bool bScroll, bool bSelectText);
	virtual void OnEditOperation(int nAction, LPCTSTR pszText);
	void UpdateLineLengths();
	bool IsLineInCurrentDiff(int nLine);
	void SelectNone();
	void SelectDiff(int nDiff, bool bScroll = true, bool bSelectText = true);
	virtual CCrystalTextBuffer *LocateTextBuffer ();
	void GetFullySelectedDiffs(int & firstDiff, int & lastDiff);
	CString GetSelectedText();
	CString GetLineText(int idx);
	CMergeDoc* GetDocument();
	void UpdateResources();
	BOOL IsModified() { return (LocateTextBuffer()->IsModified()); }
	void PrimeListWithFile();
	void SetStatusInterface(IMergeEditStatus * piMergeEditStatus);
	void SelectArea(const CPoint & ptStart, const CPoint & ptEnd) { SetSelection(ptStart, ptEnd); } // make public
	virtual void UpdateSiblingScrollPos (BOOL bHorz);
	virtual int GetAdditionalTextBlocks (int nLineIndex, TEXTBLOCK *pBuf);
	virtual COLORREF GetColor(int nColorIndex);
	virtual void GetLineColors (int nLineIndex, COLORREF & crBkgnd,
			COLORREF & crText, BOOL & bDrawWhitespace);
	virtual void GetLineColors2 (int nLineIndex, DWORD ignoreFlags
		, COLORREF & crBkgnd, COLORREF & crText, BOOL & bDrawWhitespace);
	void WMGoto() { OnWMGoto(); };
	void GotoLine(UINT nLine, bool bRealLine, int pane);
	int GetTopLine() { return m_nTopLine; }
	int GetScreenLines() { return CCrystalTextView::GetScreenLines(); }
	int GetTopSubLine() { return m_nTopSubLine; }
	int GetSubLines(int nLineIndex) { return CCrystalTextView::GetSubLines(nLineIndex); }
	virtual int GetSubLineCount() { return CCrystalTextView::GetSubLineCount(); }
	virtual int GetSubLineIndex(int nLineIndex) { return CCrystalTextView::GetSubLineIndex(nLineIndex); }
	virtual void GetLineBySubLine(int nSubLineIndex, int &nLine, int &nSubLine) {
		CCrystalTextView::GetLineBySubLine(nSubLineIndex, nLine, nSubLine);
	}
	virtual int GetEmptySubLines( int nLineIndex );
	virtual void InvalidateSubLineIndexCache( int nLineIndex );
	void RepaintLocationPane();
	bool SetPredifferByName(const CString & prediffer);
	void SetPredifferByMenu(UINT nID);
	void DocumentsLoaded();
	void SetLocationView(const CLocationView * pView = NULL);
	void UpdateLocationViewPosition(int nTopLine = -1, int nBottomLine = -1);
	virtual void RecalcPageLayouts(CDC * pdc, CPrintInfo * pInfo);
	virtual void GetPrintHeaderText(int nPageNum, CString & text);
	virtual void PrintHeader(CDC * pdc, int nPageNum);
	virtual void PrintFooter(CDC * pdc, int nPageNum);
	void UpdateStatusbar();

	// to customize the mergeview menu
	static HMENU createScriptsSubmenu(HMENU hMenu);
	HMENU createPrediffersSubmenu(HMENU hMenu);

	bool IsInitialized() const;
	bool IsCursorInDiff() const;
	bool IsDiffVisible(int nDiff);
	void ZoomText(short amount);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeEditView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnBeginPrinting (CDC * pDC, CPrintInfo * pInfo);
	virtual void OnEndPrinting (CDC * pDC, CPrintInfo * pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMergeEditView();
	virtual void OnUpdateSibling (CCrystalTextView * pUpdateSource, BOOL bHorz);
	virtual void OnUpdateCaret();
	bool MergeModeKeyDown(MSG* pMsg);
	int FindPrediffer(LPCTSTR prediffer) const;
	bool IsDiffVisible(const DIFFRANGE& diff, int nLinesBelow = 0);

	// Generated message map functions
protected:
	//{{AFX_MSG(CMergeEditView)
	afx_msg void OnCurdiff();
	afx_msg void OnUpdateCurdiff(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnFirstdiff();
	afx_msg void OnUpdateFirstdiff(CCmdUI* pCmdUI);
	afx_msg void OnLastdiff();
	afx_msg void OnUpdateLastdiff(CCmdUI* pCmdUI);
	afx_msg void OnNextdiff();
	afx_msg void OnUpdateNextdiff(CCmdUI* pCmdUI);
	afx_msg void OnPrevdiff();
	afx_msg void OnUpdatePrevdiff(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnAllLeft();
	afx_msg void OnUpdateAllLeft(CCmdUI* pCmdUI);
	afx_msg void OnAllRight();
	afx_msg void OnUpdateAllRight(CCmdUI* pCmdUI);
	afx_msg void OnL2r();
	afx_msg void OnUpdateL2r(CCmdUI* pCmdUI);
	afx_msg void OnR2l();
	afx_msg void OnUpdateR2l(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnUpdateFileSaveLeft(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveRight(CCmdUI* pCmdUI);
	afx_msg void OnRefresh();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnSelectLineDiff();
	afx_msg void OnUpdateSelectLineDiff(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateEditReplace(CCmdUI* pCmdUI);
	afx_msg void OnLeftReadOnly();
	afx_msg void OnUpdateLeftReadOnly(CCmdUI* pCmdUI);
	afx_msg void OnRightReadOnly();
	afx_msg void OnUpdateRightReadOnly(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatusLeftRO(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatusRightRO(CCmdUI* pCmdUI);
	afx_msg void OnConvertEolTo(UINT nID );
	afx_msg void OnUpdateConvertEolTo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatusLeftEOL(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatusRightEOL(CCmdUI* pCmdUI);
	afx_msg void OnL2RNext();
	afx_msg void OnUpdateL2RNext(CCmdUI* pCmdUI);
	afx_msg void OnR2LNext();
	afx_msg void OnUpdateR2LNext(CCmdUI* pCmdUI);
	afx_msg void OnChangePane();
	afx_msg void OnUpdateChangePane(CCmdUI* pCmdUI);
	afx_msg void OnWMGoto();
	afx_msg void OnUpdateWMGoto(CCmdUI* pCmdUI);
	afx_msg void OnUpdateScripts(CCmdUI* pCmdUI);
	afx_msg void OnScripts(UINT nID );
	afx_msg void OnUpdateNoPrediffer(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePrediffer(CCmdUI* pCmdUI);
	afx_msg void OnNoPrediffer();
	afx_msg void OnPrediffer(UINT nID );
	afx_msg void OnMergingMode();
	afx_msg void OnUpdateMergingMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMergingStatus(CCmdUI* pCmdUI);
	afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
	afx_msg void OnVScroll (UINT nSBCode, UINT nPos, CScrollBar * pScrollBar);
	afx_msg void OnEditCopyLineNumbers();
	afx_msg void OnUpdateEditCopyLinenumbers(CCmdUI* pCmdUI);
	afx_msg void OnViewLineDiffs();
	afx_msg void OnUpdateViewLineDiffs(CCmdUI* pCmdUI);
	afx_msg void OnViewLineNumbers();
	afx_msg void OnUpdateViewLineNumbers(CCmdUI* pCmdUI);
	afx_msg void OnViewWordWrap();
	afx_msg void OnUpdateViewWordWrap(CCmdUI* pCmdUI);
	afx_msg void OnOpenFile();
	afx_msg void OnOpenFileWith();
	afx_msg void OnOpenFileWithEditor();
	afx_msg void OnViewSwapPanes();
	afx_msg void OnUpdateViewSwapPanes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNoEditScripts(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHelp();
	afx_msg void OnViewMargin();
	afx_msg void OnUpdateViewMargin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewChangeScheme(CCmdUI *pCmdUI);
	afx_msg void OnChangeScheme(UINT nID);
	afx_msg void OnUpdateChangeScheme(CCmdUI* pCmdUI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnViewZoomNormal();
	afx_msg void OnUpdateStatusLeftEncoding(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatusRightEncoding(CCmdUI* pCmdUI);
	afx_msg void OnPluginsList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DiffView.cpp
inline CMergeDoc* CMergeEditView::GetDocument()
   { return (CMergeDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGEEDITVIEW_H__0CE31CFD_4BEE_4378_ADB4_B7C9F50A9F53__INCLUDED_)

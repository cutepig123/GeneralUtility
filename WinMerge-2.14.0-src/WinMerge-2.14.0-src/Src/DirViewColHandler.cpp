/** 
 * @file  DirViewColHandler.cpp
 *
 * @brief Methods of CDirView dealing with the listview (of file results)
 *
 * @date  Created: 2003-08-19
 */
// ID line follows -- this is updated by SVN
// $Id: DirViewColHandler.cpp 7337 2010-10-21 22:20:18Z gerundt $


#include "StdAfx.h"
#include "Merge.h"
#include "DirView.h"
#include "DirDoc.h"
#include "resource.h"
#include "coretools.h"
#include "DirViewColItems.h"
#include "DirColsDlg.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////


/**
 * @brief Get text for specified column.
 * This function retrieves the text for the specified colum. Text is
 * retrieved by using column-specific handler functions.
 * @param [in] pCtxt Compare context.
 * @param [in] col Column number.
 * @param [in] di Difference data.
 * @return Text for the specified column.
 */
String CDirView::ColGetTextToDisplay(const CDiffContext *pCtxt, int col,
		const DIFFITEM & di)
{
	// Custom properties have custom get functions
	const DirColInfo * pColInfo = DirViewColItems_GetDirColInfo(col);
	if (!pColInfo)
	{
		ASSERT(0); // fix caller, should not ask for nonexistent columns
		return _T("???");
	}
	ColGetFncPtrType fnc = pColInfo->getfnc;
	SIZE_T offset = pColInfo->offset;
	return (*fnc)(pCtxt, reinterpret_cast<const char *>(&di) + offset);
}

/**
 * @brief Sort two items on specified column.
 * This function determines order of two items in specified column. Order
 * is determined by column-specific functions.
 * @param [in] pCtxt Compare context.
 * @param [in] col Column number to sort.
 * @param [in] ldi Left difference item data.
 * @param [in] rdi Right difference item data.
 * @return Order of items.
 */
int CDirView::ColSort(const CDiffContext *pCtxt, int col, const DIFFITEM & ldi,
		const DIFFITEM & rdi) const
{
	// Custom properties have custom sort functions
	const DirColInfo * pColInfo = DirViewColItems_GetDirColInfo(col);
	if (!pColInfo)
	{
		ASSERT(0); // fix caller, should not ask for nonexistent columns
		return 0;
	}
	SIZE_T offset = pColInfo->offset;
	const void * arg1;
	const void * arg2;
	if (m_bTreeMode)
	{
		int lLevel = ldi.GetDepth();
		int rLevel = rdi.GetDepth();
		const DIFFITEM *lcur = &ldi, *rcur = &rdi;
		if (lLevel < rLevel)
		{
			for (; lLevel != rLevel; rLevel--)
				rcur = rcur->parent;
		}
		else if (rLevel < lLevel)
		{
			for (; lLevel != rLevel; lLevel--)
				lcur = lcur->parent;
		}
		while (lcur->parent != rcur->parent)
		{
			lcur = lcur->parent;
			rcur = rcur->parent;
		}
		arg1 = reinterpret_cast<const char *>(lcur) + offset;
		arg2 = reinterpret_cast<const char *>(rcur) + offset;
	}
	else
	{
		arg1 = reinterpret_cast<const char *>(&ldi) + offset;
		arg2 = reinterpret_cast<const char *>(&rdi) + offset;
	}
	if (ColSortFncPtrType fnc = pColInfo->sortfnc)
	{
		return (*fnc)(pCtxt, arg1, arg2);
	}
	if (ColGetFncPtrType fnc = pColInfo->getfnc)
	{
		String p = (*fnc)(pCtxt, arg1);
		String q = (*fnc)(pCtxt, arg2);
		return lstrcmpi(p.c_str(), q.c_str());
	}
	return 0;
}

/**
 * @brief return whether column normally sorts ascending (dates do not)
 */
bool CDirView::IsDefaultSortAscending(int col) const
{
	const DirColInfo * pColInfo = DirViewColItems_GetDirColInfo(col);
	if (!pColInfo)
	{
		ASSERT(0); // fix caller, should not ask for nonexistent columns
		return 0;
	}
	return pColInfo->defSortUp;
}

/// Assign column name, using string resource & current column ordering
void CDirView::NameColumn(int id, int subitem)
{
	int phys = ColLogToPhys(subitem);
	if (phys>=0)
	{
		String s = theApp.LoadString(id);
		LV_COLUMN lvc;
		lvc.mask = LVCF_TEXT;
		lvc.pszText = const_cast<LPTSTR>(s.c_str());
		m_pList->SetColumn(m_colorder[subitem], &lvc);
	}
}

/// Load column names from string table
void CDirView::UpdateColumnNames()
{
	for (int i=0; i<g_ncols; ++i)
	{
		const DirColInfo * col = DirViewColItems_GetDirColInfo(i);
		NameColumn(col->idName, i);
	}
}

/**
 * @brief Set alignment of columns.
 */
void CDirView::SetColAlignments()
{
	for (int i=0; i<g_ncols; ++i)
	{
		const DirColInfo * col = DirViewColItems_GetDirColInfo(i);
		LVCOLUMN lvc;
		lvc.mask = LVCF_FMT;
		lvc.fmt = col->alignment;
		m_pList->SetColumn(m_colorder[i], &lvc);
	}
}

CDirView::CompareState::CompareState(const CDirView *pView, int sortCol, bool bSortAscending)
: pView(pView)
, pCtxt(&pView->GetDocument()->GetDiffContext())
, sortCol(sortCol)
, bSortAscending(bSortAscending)
{
}

/// Compare two specified rows during a sort operation (windows callback)
int CALLBACK CDirView::CompareState::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CompareState *pThis = reinterpret_cast<CompareState*>(lParamSort);
	// Sort special items always first in dir view
	if (lParam1 == -1)
		return -1;
	if (lParam2 == -1)
		return 1;

	UINT_PTR diffposl = (UINT_PTR)lParam1;
	UINT_PTR diffposr = (UINT_PTR)lParam2;
	const DIFFITEM &ldi = pThis->pCtxt->GetDiffAt(diffposl);
	const DIFFITEM &rdi = pThis->pCtxt->GetDiffAt(diffposr);
	// compare 'left' and 'right' parameters as appropriate
	int retVal = pThis->pView->ColSort(pThis->pCtxt, pThis->sortCol, ldi, rdi);
	// return compare result, considering sort direction
	return pThis->bSortAscending ? retVal : -retVal;
}

/// Add new item to list view
int CDirView::AddNewItem(int i, UINT_PTR diffpos, int iImage, int iIndent)
{
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE | LVIF_INDENT;
	lvItem.iItem = i;
	lvItem.iIndent = iIndent;
	lvItem.iSubItem = 0;
	lvItem.pszText = LPSTR_TEXTCALLBACK;
	lvItem.lParam = (LPARAM)diffpos;
	lvItem.iImage = iImage;
	return GetListCtrl().InsertItem(&lvItem);
}

/**
 * @brief Update listview display of details for specified row
 * @note Customising shownd data should be done here
 */
void CDirView::UpdateDiffItemStatus(UINT nIdx)
{
	GetListCtrl().RedrawItems(nIdx, nIdx);
}

static String rgDispinfoText[2]; // used in function below

/**
 * @brief Allocate a text buffer to assign to NMLVDISPINFO::item::pszText
 * Quoting from SDK Docs:
 *	If the LVITEM structure is receiving item text, the pszText and cchTextMax
 *	members specify the address and size of a buffer. You can either copy text to
 *	the buffer or assign the address of a string to the pszText member. In the
 *	latter case, you must not change or delete the string until the corresponding
 *	item text is deleted or two additional LVN_GETDISPINFO messages have been sent.
 */
static LPTSTR NTAPI AllocDispinfoText(const String &s)
{
	static int i = 0;
	LPCTSTR pszText = (rgDispinfoText[i] = s).c_str();
	i ^= 1;
	return (LPTSTR)pszText;
}

/**
 * @brief Respond to LVN_GETDISPINFO message
 */
void CDirView::ReflectGetdispinfo(NMLVDISPINFO *pParam)
{
	int nIdx = pParam->item.iItem;
	int i = ColPhysToLog(pParam->item.iSubItem);
	UINT_PTR key = GetItemKey(nIdx);
	if (key == SPECIAL_ITEM_POS)
	{
		if (IsColName(i))
		{
			pParam->item.pszText = _T("..");
		}
		return;
	}
	if (!GetDocument()->HasDiffs())
		return;
	const CDiffContext &ctxt = GetDocument()->GetDiffContext();
	const DIFFITEM &di = GetDocument()->GetDiffRefByKey(key);
	if (pParam->item.mask & LVIF_TEXT)
	{
		String s = ColGetTextToDisplay(&ctxt, i, di);
		// Add '*' to newer time field
		if (di.left.mtime != 0 || di.right.mtime != 0)
		{
			if ((IsColLmTime(i) && di.left.mtime > di.right.mtime) ||
				(IsColRmTime(i) && di.left.mtime < di.right.mtime))
			{
				s.insert(0, _T("* "));
			}
		}
		pParam->item.pszText = AllocDispinfoText(s);
	}
	if (pParam->item.mask & LVIF_IMAGE)
	{
		pParam->item.iImage = GetColImage(di);
	}
}

/// store current column orders into registry
void CDirView::SaveColumnOrders()
{
	ASSERT(m_colorder.size() == m_numcols);
	ASSERT(m_invcolorder.size() == m_numcols);
    for (int i=0; i < m_numcols; i++)
	{
		CString RegName = GetColRegValueNameBase(i) + _T("_Order");
		int ord = m_colorder[i];
		theApp.WriteProfileInt(_T("DirView"), RegName, ord);
	}
}

/**
 * @brief Load column orders from registry
 */
void CDirView::LoadColumnOrders()
{
	ASSERT(m_numcols == -1);
	m_numcols = GetColLogCount();
	ClearColumnOrders();
	m_dispcols = 0;

	// Load column orders
	// Break out if one is missing
	// Break out & mark failure (m_dispcols == -1) if one is invalid
	int i=0;
	for (i=0; i<m_numcols; ++i)
	{
		CString RegName = GetColRegValueNameBase(i) + _T("_Order");
		int ord = theApp.GetProfileInt(_T("DirView"), RegName, -2);
		if (ord<-1 || ord >= m_numcols)
			break;
		m_colorder[i] = ord;
		if (ord>=0)
		{
			++m_dispcols;
			if (m_invcolorder[ord] != -1)
			{
				m_dispcols = -1;
				break;
			}
			m_invcolorder[ord] = i;
		}
	}
	// Check that a contiguous range was set
	for (i=0; i<m_dispcols; ++i)
	{
		if (m_invcolorder[i] < 0)
		{
			m_dispcols = -1;
			break;
		}
	}
	// Must have at least one column
	if (m_dispcols<=1)
	{
		ResetColumnOrdering();
	}

	ValidateColumnOrdering();
}

/**
 * @brief Sanity check column ordering
 */
void CDirView::ValidateColumnOrdering()
{

#if _DEBUG
	ASSERT(m_invcolorder[0]>=0);
	ASSERT(m_numcols == GetColLogCount());
	// Check that any logical->physical mapping is reversible
	for (int i=0; i<m_numcols; ++i)
	{
		int phy = m_colorder[i];
		if (phy >= 0)
		{
			int log = m_invcolorder[phy];
			ASSERT(i == log);
		}
	}
	// Bail out if header doesn't exist yet
	int hdrcnt = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	if (hdrcnt)
	{
		ASSERT(hdrcnt == m_dispcols);
	}
	return;
#endif
}

/**
 * @brief Set column ordering to default initial order
 */
void CDirView::ResetColumnOrdering()
{
	ClearColumnOrders();
	m_dispcols = 0;
	for (int i=0; i<m_numcols; ++i)
	{
		int phy = GetColDefaultOrder(i);
		m_colorder[i] = phy;
		if (phy>=0)
		{
			m_invcolorder[phy] = i;
			++m_dispcols;
		}
	}
	ValidateColumnOrdering();
}

/**
 * @brief Reset all current column ordering information
 */
void CDirView::ClearColumnOrders()
{
	m_colorder.resize(m_numcols);
	m_invcolorder.resize(m_numcols);
	for (int i=0; i<m_numcols; ++i)
	{
		m_colorder[i] = -1;
		m_invcolorder[i] = -1;
	}
}

/**
 * @brief Return display name of column
 */
String CDirView::GetColDisplayName(int col) const
{
	const DirColInfo * colinfo = DirViewColItems_GetDirColInfo(col);
	return theApp.LoadString(colinfo->idName);
}

/**
 * @brief Return description of column
 */
String CDirView::GetColDescription(int col) const
{
	const DirColInfo * colinfo = DirViewColItems_GetDirColInfo(col);
	return theApp.LoadString(colinfo->idDesc);
}

/**
 * @brief Return total number of known columns
 */
int CDirView::GetColLogCount() const
{
	return g_ncols;
}

/**
 * @brief Remove any windows reordering of columns (params are physical columns)
 */
void CDirView::MoveColumn(int psrc, int pdest)
{
	// actually moved column
	m_colorder[m_invcolorder[psrc]] = pdest;
	// shift all other affected columns
	int dir = psrc > pdest ? +1 : -1;
	int i=0;
	for (i=pdest; i!=psrc; i += dir)
	{
		m_colorder[m_invcolorder[i]] = i+dir;
	}
	// fix inverse mapping
	for (i=0; i<m_numcols; ++i)
	{
		if (m_colorder[i] >= 0)
			m_invcolorder[m_colorder[i]] = i;
	}
	ValidateColumnOrdering();
	InitiateSort();
	ValidateColumnOrdering();
}

/**
 * @brief User examines & edits which columns are displayed in dirview, and in which order
 */
void CDirView::OnEditColumns()
{
	CDirColsDlg dlg;
	// List all the currently displayed columns
	for (int col=0; col<GetListCtrl().GetHeaderCtrl()->GetItemCount(); ++col)
	{
		int l = ColPhysToLog(col);
		dlg.AddColumn(GetColDisplayName(l), GetColDescription(l), l, col);
	}
	// Now add all the columns not currently displayed
	int l=0;
	for (l=0; l<GetColLogCount(); ++l)
	{
		if (ColLogToPhys(l)==-1)
		{
			dlg.AddColumn(GetColDisplayName(l), GetColDescription(l), l);
		}
	}

	// Add default order of columns for resetting to defaults
	for (l = 0; l < m_numcols; ++l)
	{
		int phy = GetColDefaultOrder(l);
		dlg.AddDefColumn(GetColDisplayName(l), l, phy);
	}

	if (dlg.DoModal() != IDOK)
		return;

	if (dlg.m_bReset)
		ResetColumnWidths();
	else
		SaveColumnWidths(); // save current widths to registry

	// Reset our data to reflect the new data from the dialog
	const CDirColsDlg::ColumnArray & cols = dlg.GetColumns();
	ClearColumnOrders();
	m_dispcols = 0;
	const int sortColumn = GetOptionsMgr()->GetInt(OPT_DIRVIEW_SORT_COLUMN);
	for (CDirColsDlg::ColumnArray::const_iterator iter = cols.begin();
		iter != cols.end(); ++iter)
	{
		int log = iter->log_col;
		int phy = iter->phy_col;
		m_colorder[log] = phy;
		if (phy>=0)
		{
			++m_dispcols;
			m_invcolorder[phy] = log;
		}

		// If sorted column was hidden, reset sorting
		if (log == sortColumn && phy < 0)
		{
			GetOptionsMgr()->Reset(OPT_DIRVIEW_SORT_COLUMN);
			GetOptionsMgr()->Reset(OPT_DIRVIEW_SORT_ASCENDING);
		}
	}
	if (m_dispcols < 1)
	{
		// Ignore them if they didn't leave a column showing
		ResetColumnOrdering();
	}
	else
	{
		ReloadColumns();
		Redisplay();
	}
	ValidateColumnOrdering();
}

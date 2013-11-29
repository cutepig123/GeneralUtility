#include "stdafx.h"
#include "MainFrm.h"
#include "ClassView.h"
#include "Resource.h"
#include "UIDesigner.h"

//////////////////////////////////////////////////////////////////////////
//CClassViewTree

CClassViewTree::CClassViewTree()
{
}

CClassViewTree::~CClassViewTree()
{
}

BEGIN_MESSAGE_MAP(CClassViewTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CClassViewTree::OnTvnSelChanged)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassViewTree ��Ϣ�������

void CClassViewTree::OnTvnSelChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if(pNMTreeView->action!=0&&pNMTreeView->action!=1)
		return;

	CControlUI* pControl=(CControlUI*)GetItemData(pNMTreeView->itemNew.hItem);
	CUIDesignerView* pUIView=g_pMainFrame->GetActiveUIView();
	if(pUIView)
		pUIView->SelectUI(pControl);
}

void CClassViewTree::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch(nChar)
	{
	case VK_DELETE:
		g_pMainFrame->GetActiveUIView()->OnDeleteUI();
		break;
	}

	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

//////////////////////////////////////////////////////////////////////
//CClassView

CClassView::CClassView()
{
	g_pClassView=this;
}

CClassView::~CClassView()
{
}

BEGIN_MESSAGE_MAP(CClassView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassView ��Ϣ�������

int CClassView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// ������ͼ:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndClassView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("δ�ܴ�������ͼ\n");
		return -1;      // δ�ܴ���
	}

	OnChangeVisualStyle();

	return 0;
}

void CClassView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CClassView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndClassView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// ѡ���ѵ�������:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	CMenu menu;
	menu.LoadMenu(IDR_POPUP_SORT);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}
}

void CClassView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_wndClassView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CClassView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CClassView::OnPaint()
{
	CPaintDC dc(this); // ���ڻ��Ƶ��豸������

	CRect rectTree;
	m_wndClassView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CClassView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndClassView.SetFocus();
}

void CClassView::OnChangeVisualStyle()
{
	m_ClassViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("�޷�����λͼ: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ClassViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ClassViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndClassView.SetImageList(&m_ClassViewImages, TVSIL_NORMAL);
}

void CClassView::InsertUITreeItem(CControlUI* pControl,LPCTSTR pstrName/*=NULL*/)
{
	ASSERT(pControl);
	if(pControl==NULL)
		return;

	CControlUI* pParent=pControl->GetParent();
	HTREEITEM hParent=pParent?(HTREEITEM)(((ExtendedAttributes*)pParent->GetTag())->hItem):TVI_ROOT;

	CString strName(pstrName);
	if(strName.IsEmpty())
		strName=pControl->GetName();

	int nImage = ((ExtendedAttributes*)pControl->GetTag())->nClass - classForm;
	HTREEITEM hItem=m_wndClassView.InsertItem(strName, nImage, nImage,hParent);
	ExtendedAttributes* pExtended=(ExtendedAttributes*)pControl->GetTag();
	pExtended->hItem=hItem;
	m_wndClassView.SetItemData(hItem,(DWORD_PTR)pControl);
	m_wndClassView.Expand(hParent,TVE_EXPAND);
	if(hParent==TVI_ROOT)
		m_wndClassView.SetItemState(hItem,TVIS_BOLD,TVIS_BOLD);
}

BOOL CClassView::RemoveUITreeItem(CControlUI* pControl)
{
	if(pControl==NULL)
		return FALSE;

	HTREEITEM hDelete=(HTREEITEM)(((ExtendedAttributes*)pControl->GetTag())->hItem);

	return RemoveUITreeItem(hDelete);
}

BOOL CClassView::RemoveUITreeItem(HTREEITEM hItem)
{
	if(m_wndClassView.ItemHasChildren(hItem))
	{
		HTREEITEM hChild=m_wndClassView.GetChildItem(hItem);
		while(hChild!=NULL)
		{
			RemoveUITreeItem(hChild);
			hChild=m_wndClassView.GetNextItem(hChild,TVGN_NEXT);
		}
	}

	return m_wndClassView.DeleteItem(hItem);
}

void CClassView::SelectUITreeItem(CControlUI* pControl)
{
	if(pControl==NULL)
		return;

	HTREEITEM hSelect=(HTREEITEM)(((ExtendedAttributes*)pControl->GetTag())->hItem);
	m_wndClassView.SelectItem(hSelect);
}

void CClassView::RenameUITreeItem(CControlUI* pControl, LPCTSTR lpszName)
{
	if(pControl==NULL)
		return;

	HTREEITEM hSelect=(HTREEITEM)(((ExtendedAttributes*)pControl->GetTag())->hItem);
	m_wndClassView.SetItemText(hSelect, lpszName);
}
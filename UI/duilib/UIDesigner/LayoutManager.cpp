#include "StdAfx.h"
#include "LayoutManager.h"
#include "UIUtil.h"
#include <vector>

//////////////////////////////////////////////////////////////////////////
//CFormUI

CFormUI::CFormUI()
{
}

CFormUI::~CFormUI()
{

}

LPCTSTR CFormUI::GetClass() const
{
	return _T("FormUI");
}

LPVOID CFormUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("Form")) == 0 )
		return static_cast<CFormUI*>(this);

	return CContainerUI::GetInterface(pstrName);
}

SIZE CFormUI::GetInitSize()
{
	return m_pManager->GetInitSize();
}

void CFormUI::SetInitSize(int cx, int cy)
{
	m_pManager->SetInitSize(cx,cy);

	SetPos(CRect(0,0,cx,cy));
}

RECT& CFormUI::GetSizeBox()
{
	return m_pManager->GetSizeBox();
}

void CFormUI::SetSizeBox(RECT& rcSizeBox)
{
	m_pManager->SetSizeBox(rcSizeBox);
}

RECT& CFormUI::GetCaptionRect()
{
	return m_pManager->GetCaptionRect();
}

void CFormUI::SetCaptionRect(RECT& rcCaption)
{
	m_pManager->SetCaptionRect(rcCaption);
}

SIZE CFormUI::GetRoundCorner() const
{
	return m_pManager->GetRoundCorner();
}

void CFormUI::SetRoundCorner(int cx, int cy)
{
	m_pManager->SetRoundCorner(cx,cy);
}

SIZE CFormUI::GetMinInfo() const
{
	return m_pManager->GetMinInfo();
}

SIZE CFormUI::GetMaxInfo() const
{
	return m_pManager->GetMaxInfo();
}

void CFormUI::SetMinInfo(int cx, int cy)
{
	ASSERT(cx>=0 && cy>=0);
	m_pManager->SetMinInfo(cx,cy);
}

void CFormUI::SetMaxInfo(int cx, int cy)
{
	ASSERT(cx>=0 && cy>=0);
	m_pManager->SetMaxInfo(cx,cy);
}

bool CFormUI::IsShowUpdateRect() const
{
	return m_pManager->IsShowUpdateRect();
}

void CFormUI::SetShowUpdateRect(bool show)
{
	m_pManager->SetShowUpdateRect(show);
}

void CFormUI::SetPos(RECT rc)
{
	CControlUI::SetPos(rc);
	m_cxyFixed.cx = rc.right - rc.left;
	m_cxyFixed.cy = rc.bottom - rc.top;

	if(m_items.GetSize()==0)
		return;
	CControlUI* pControl=static_cast<CControlUI*>(m_items[0]);
	if(pControl==NULL)
		return;
	pControl->SetPos(rc);//�Ŵ������ͻ���
}

void CFormUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("size")) == 0 ) {
		LPTSTR pstr = NULL;
		int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
		SetInitSize(cx, cy);
	} 
	else if( _tcscmp(pstrName, _T("sizebox")) == 0 ) {
		RECT rcSizeBox = { 0 };
		LPTSTR pstr = NULL;
		rcSizeBox.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		rcSizeBox.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		rcSizeBox.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
		rcSizeBox.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		SetSizeBox(rcSizeBox);
	}
	else if( _tcscmp(pstrName, _T("caption")) == 0 ) {
		RECT rcCaption = { 0 };
		LPTSTR pstr = NULL;
		rcCaption.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		rcCaption.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		rcCaption.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
		rcCaption.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		SetCaptionRect(rcCaption);
	}
	else if( _tcscmp(pstrName, _T("roundcorner")) == 0 ) {
		LPTSTR pstr = NULL;
		int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
		SetRoundCorner(cx, cy);
	} 
	else if( _tcscmp(pstrName, _T("mininfo")) == 0 ) {
		LPTSTR pstr = NULL;
		int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
		SetMinInfo(cx, cy);
	}
	else if( _tcscmp(pstrName, _T("maxinfo")) == 0 ) {
		LPTSTR pstr = NULL;
		int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
		SetMaxInfo(cx, cy);
	}
	else if( _tcscmp(pstrName, _T("showdirty")) == 0 ) {
		SetShowUpdateRect(_tcscmp(pstrValue, _T("true")) == 0);
	}
	else if( _tcscmp(pstrName, _T("pos")) == 0 ) {
		RECT rcPos = { 0 };
		LPTSTR pstr = NULL;
		rcPos.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		rcPos.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);    
		rcPos.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);    
		rcPos.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);    
		SetInitSize(rcPos.right - rcPos.left, rcPos.bottom - rcPos.top);
	}
}

//////////////////////////////////////////////////////////////////////////
//CFormTestWnd

CFormTestWnd::CFormTestWnd():m_pManager(NULL),m_pRoot(NULL)
{

}

CFormTestWnd::~CFormTestWnd()
{
	if(m_pManager)
		delete m_pManager;
}

LPCTSTR CFormTestWnd::GetWindowClassName() const
{
	 return _T("UIFormTest");
}

UINT CFormTestWnd::GetClassStyle() const
{
	return CS_DBLCLKS;
}

void CFormTestWnd::OnFinalMessage(HWND /*hWnd*/)
{
	delete this;
}

void CFormTestWnd::SetManager(CPaintManagerUI* pPaintManager)
{
	ASSERT(pPaintManager);

	m_pManager=pPaintManager;
}

CPaintManagerUI* CFormTestWnd::GetManager() const
{
	return m_pManager;
}

void CFormTestWnd::SetRoot(CControlUI* pControl)
{
	m_pRoot=pControl;
}

void CFormTestWnd::Notify(TNotifyUI& msg)
{
}

LRESULT CFormTestWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
		rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

	m_pManager->Init(m_hWnd);
	m_pManager->AttachDialog(m_pRoot);
	m_pManager->AddNotifier(this);

	SIZE szInitWindowSize = m_pManager->GetInitSize();
	if( szInitWindowSize.cx != 0 ) {
		::SetWindowPos(m_hWnd, NULL, 0, 0, szInitWindowSize.cx, szInitWindowSize.cy, SWP_NOMOVE | SWP_NOZORDER);
	}

	return 0;
}

LRESULT CFormTestWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CFormTestWnd::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CFormTestWnd::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CFormTestWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	RECT rcCaption = m_pManager->GetCaptionRect();
	if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
		&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pManager->FindControl(pt));
			if( pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
				_tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
				_tcscmp(pControl->GetClass(), _T("TextUI")) != 0 )
				return HTCAPTION;
	}

	return HTCLIENT;
}

LRESULT CFormTestWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = m_pManager->GetRoundCorner();
	if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
		RECT rcClient;
		::GetClientRect(*this, &rcClient);
		RECT rc = { rcClient.left, rcClient.top + szRoundCorner.cx, rcClient.right, rcClient.bottom };
		HRGN hRgn1 = ::CreateRectRgnIndirect( &rc );
		HRGN hRgn2 = ::CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right + 1, rcClient.bottom - szRoundCorner.cx, szRoundCorner.cx, szRoundCorner.cy);
		::CombineRgn( hRgn1, hRgn1, hRgn2, RGN_OR );
		::SetWindowRgn(*this, hRgn1, TRUE);
		::DeleteObject(hRgn1);
		::DeleteObject(hRgn2);
	}

	bHandled = FALSE;
	return 0;
}

LRESULT CFormTestWnd::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CRect rcWork = oMonitor.rcWork;
	rcWork.OffsetRect(-rcWork.left, -rcWork.top);

	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;
	lpMMI->ptMaxSize.x		= rcWork.right;
	lpMMI->ptMaxSize.y		= rcWork.bottom;

	bHandled = FALSE;
	return 0;
}

LRESULT CFormTestWnd::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam==VK_ESCAPE)
		Close();

	bHandled=FALSE;
	return 0;
}

LRESULT CFormTestWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch( uMsg ) {
		case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
		case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
		case WM_GETMINMAXINFO: lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;
		case WM_KEYDOWN:	   lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
		default:
			bHandled = FALSE;
	}
	if( bHandled )
		return lRes;
	if( m_pManager->MessageHandler(uMsg, wParam, lParam, lRes) )
		return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
//CLayoutManager

CLayoutManager::CLayoutManager(void)
	: m_pFormUI(NULL), m_bShowGrid(false), m_bShowAuxBorder(true)
{
}

CLayoutManager::~CLayoutManager(void)
{
}

void CLayoutManager::Init(HWND hWnd,LPCTSTR pstrLoad)
{
	m_pFormUI=static_cast<CFormUI*>(NewUI(classForm,
		CRect(0,0,FORM_INIT_WIDTH,FORM_INIT_HEIGHT),NULL, NULL)->GetInterface(_T("Form")));
	ASSERT(m_pFormUI);
	m_pFormUI->SetManager(&m_Manager, NULL);
	m_pFormUI->SetInitSize(FORM_INIT_WIDTH,FORM_INIT_HEIGHT);

	m_Manager.Init(hWnd);
	if(*pstrLoad!='\0')
	{
		m_strSkinDir = pstrLoad;
		int nPos = m_strSkinDir.ReverseFind(_T('\\'));
		if(nPos != -1)
			m_strSkinDir = m_strSkinDir.Left(nPos + 1);
		
		g_HookAPI.SetSkinDir(m_strSkinDir);
		g_HookAPI.EnableCreateFile(true);

		CDialogBuilder builder;
		CControlUI* pRoot=builder.Create(pstrLoad,(UINT)0,NULL,&m_Manager);
		if(pRoot)
			m_pFormUI->Add(pRoot);

		SIZE size=m_Manager.GetInitSize();
		m_pFormUI->SetInitSize(size.cx,size.cy);
	}
	m_Manager.AttachDialog(m_pFormUI);
}

void CLayoutManager::Draw(CDC* pDC)
{
	CSize szForm = GetForm()->GetInitSize();
	CRect rcPaint(0, 0, szForm.cx, szForm.cy);
	CControlUI* pForm = m_Manager.GetRoot();

	pForm->DoPaint(pDC->GetSafeHdc(), rcPaint);

	CContainerUI* pContainer = static_cast<CContainerUI*>(pForm->GetInterface(_T("Container")));
	ASSERT(pContainer);
	DrawAuxBorder(pDC, pContainer->GetItemAt(0));
	DrawGrid(pDC, rcPaint);
}

void CLayoutManager::DrawAuxBorder(CDC* pDC,CControlUI* pControl)
{
	if(!m_bShowAuxBorder||pControl==NULL||!pControl->IsVisible())
		return;

	CContainerUI* pContainer=static_cast<CContainerUI*>(pControl->GetInterface(_T("Container")));

	//draw auxBorder
	if(pControl->GetBorderColor()==0||pControl->GetBorderSize()<=0)
	{
		pDC->SetBkMode(TRANSPARENT);
		CPen DotedPen(PS_SOLID,1,pContainer?RGB(255,0,0):RGB(0,255,0));
		CPen* pOldPen;
		CBrush* pOldBrush;
		pOldPen=pDC->SelectObject(&DotedPen);
		pOldBrush=(CBrush*)pDC->SelectStockObject(NULL_BRUSH);
		CRect rect=pControl->GetPos();
		pDC->Rectangle(&rect);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}

	if(pContainer==NULL)
		return;
	for(int i=0;i<pContainer->GetCount();i++)
	{
		DrawAuxBorder(pDC,pContainer->GetItemAt(i));
	}
}

void CLayoutManager::DrawGrid(CDC* pDC, CRect& rect)
{
	if(!m_bShowGrid)
		return;

	for(int i=rect.left; i<rect.right; i+=10)
	{
		for(int j=rect.top; j<rect.bottom; j+=10)
			pDC->SetPixel(i, j, RGB(0,0,0));
	}
}

CControlUI* CLayoutManager::NewUI(int nClass,CRect& rect,CControlUI* pParent, CLayoutManager* pLayout)
{
	CControlUI* pControl=NULL;
	
	ExtendedAttributes* pExtended=new ExtendedAttributes;
	ZeroMemory(pExtended,sizeof(ExtendedAttributes));
	switch(nClass)
	{
	case classForm:
		pControl=new CFormUI;
		pExtended->nClass=classForm;
		pExtended->nDepth = 0;
		pControl->SetName(pControl->GetClass());
		break;
	case classControl:
		pControl=new CControlUI;
		pExtended->nClass=classControl;
		pControl->SetFloat(true);
		break;
	case classLabel:
		pControl=new CLabelUI;
		pExtended->nClass=classLabel;
		pControl->SetFloat(true);
		break;
	case classText:
		pControl=new CTextUI;
		pExtended->nClass=classText;
		pControl->SetFloat(true);
		break;
	case classButton:
		pControl=new CButtonUI;
		pExtended->nClass=classButton;
		pControl->SetFloat(true);
		break;
	case classEdit:
		pControl=new CEditUI;
		pExtended->nClass=classEdit;
		pControl->SetFloat(true);
		break;
	case classOption:
		pControl=new COptionUI;
		pExtended->nClass=classOption;
		pControl->SetFloat(true);
		break;
	case classProgress:
		pControl=new CProgressUI;
		pExtended->nClass=classProgress;
		pControl->SetFloat(true);
		break;
	case classSlider:
		pControl=new CSliderUI;
		pExtended->nClass=classSlider;
		pControl->SetFloat(true);
		break;
	case classCombo:
		pControl=new CComboUI;
		pExtended->nClass=classCombo;
		pControl->SetFloat(true);
		break;
	case classActiveX:
		pControl=new CActiveXUI;
		pExtended->nClass=classActiveX;
		break;
	case classContainer:
		pControl=new CContainerUI;
		pExtended->nClass=classContainer;
		break;
	case classVerticalLayout:
		pControl=new CVerticalLayoutUI;
		pExtended->nClass=classVerticalLayout;
		break;
	case classHorizontalLayout:
		pControl=new CHorizontalLayoutUI;
		pExtended->nClass=classHorizontalLayout;
		break;
	case classDialogLayout:
		pControl=new CDialogLayoutUI;
		pExtended->nClass=classDialogLayout;
		break;
	case classTileLayout:
		pControl=new CTileLayoutUI;
		pExtended->nClass=classTileLayout;
		break;
	case classTabLayout:
		pControl=new CTabLayoutUI;
		pExtended->nClass=classTabLayout;
		break;
	default:
		delete pExtended;
		return NULL;
	}
	if(pControl == NULL)
	{
		delete pExtended;
		return NULL;
	}
	pControl->SetTag((UINT_PTR)pExtended);

	//pos
	CRect rcParent = pParent ? pParent->GetPos() : CRect(0,0,0,0);
	pControl->SetFixedXY(CSize(rect.left-rcParent.left, rect.top-rcParent.top));
	pControl->SetFixedWidth(rect.right - rect.left);
	pControl->SetFixedHeight(rect.bottom - rect.top);

	//close the delayed destory function
	CContainerUI* pContainer = static_cast<CContainerUI*>(pControl->GetInterface(_T("Container")));
	if(pContainer)
		pContainer->SetDelayedDestroy(false);

	if(pParent)
	{
		CContainerUI* pParentContainer = static_cast<CContainerUI*>(pParent->GetInterface(_T("Container")));
		ASSERT(pParentContainer);
		if(!pParentContainer->Add(pControl))
		{
			delete pExtended;
			delete pControl;
			return NULL;
		}
		ExtendedAttributes* pParentExtended = (ExtendedAttributes*)pParentContainer->GetTag();
		pExtended->nDepth = pParentExtended->nDepth + 1;
		pParent->SetPos(pParent->GetPos());
	}

	if(pLayout)
	{
		CPaintManagerUI* pManager = pLayout->GetManager();
		pControl->SetManager(pManager,pParent);

		//default attributes
		CString strClass = pControl->GetClass();
		strClass = strClass.Left(strClass.GetLength() - 2);
		LPCTSTR pDefaultAttributes = pManager->GetDefaultAttributeList(strClass);
		if(pDefaultAttributes)
			pControl->ApplyAttributeList(pDefaultAttributes);

		//name
		pLayout->SetDefaultUIName(pControl);
	}

	return pControl;
}

BOOL CLayoutManager::DeleteUI(CControlUI* pControl)
{
	if(pControl == NULL)
		return FALSE;

	ReleaseExtendedAttrib(pControl, pControl->GetManager());
	CControlUI* pParent = pControl->GetParent();
	if(pParent)
	{
		CContainerUI* pContainer = static_cast<CContainerUI*>(pParent->GetInterface(_T("Container")));
		pContainer->Remove(pControl);
	}
	else
		delete pControl;

	return TRUE;
}

void CLayoutManager::ReleaseExtendedAttrib(CControlUI* pControl, CPaintManagerUI* pManager)
{
	if(pControl == NULL)
		return;

	ExtendedAttributes* pExtended=(ExtendedAttributes*)pControl->GetTag();
	delete pExtended;
	pControl->SetTag(NULL);
	if(pManager)
		pManager->ReapObjects(pControl);

	CContainerUI* pContainer=static_cast<CContainerUI*>(pControl->GetInterface(_T("Container")));
	if(pContainer != NULL)
	{
		int nCount = pContainer->GetCount();
		for(int i=0; i<nCount; i++)
			ReleaseExtendedAttrib(pContainer->GetItemAt(i), pManager);
	}
}

CPaintManagerUI* CLayoutManager::GetManager()
{
	return &m_Manager;
}

CFormUI* CLayoutManager::GetForm() const
{ 
	return m_pFormUI;
}

CControlUI* CLayoutManager::FindControl(CPoint point) const
{
	return m_Manager.FindControl(point);
}

CLayoutManager::CDelayRepos::CDelayRepos()
{
}

CLayoutManager::CDelayRepos::~CDelayRepos()
{
	m_arrDelay.RemoveAll();
}

BOOL CLayoutManager::CDelayRepos::AddParent(CControlUI* pControl)
{
	if(pControl==NULL)
		return FALSE;

	for(int i=0;i<m_arrDelay.GetSize();i++)
	{
		CControlUI* pParent=m_arrDelay.GetAt(i);

		if(pControl==pParent)
			return FALSE;
	}
	m_arrDelay.Add(pControl);

	return TRUE;
}

void CLayoutManager::CDelayRepos::Repos()
{
	for(int i=0;i<m_arrDelay.GetSize();i++)
	{
		CControlUI* pParent=m_arrDelay.GetAt(i);

		pParent->SetPos(pParent->GetPos());
	}
}

void CLayoutManager::TestForm()
{
	CFormTestWnd* pFrame=new CFormTestWnd();
	CPaintManagerUI* pManager=new CPaintManagerUI();
	SIZE size=m_Manager.GetInitSize();
	pManager->SetInitSize(size.cx,size.cy);
	pManager->SetSizeBox(m_Manager.GetSizeBox());
	pManager->SetCaptionRect(m_Manager.GetCaptionRect());
	size=m_Manager.GetRoundCorner();
	pManager->SetRoundCorner(size.cx,size.cy);
	size=m_Manager.GetMinInfo();
	pManager->SetMinInfo(size.cx,size.cy);
	size=m_Manager.GetMaxInfo();
	pManager->SetMaxInfo(size.cx,size.cy);
	pManager->SetShowUpdateRect(m_Manager.IsShowUpdateRect());

	if( pFrame == NULL )
		return;

	CControlUI* pRoot=CloneControls(GetForm()->GetItemAt(0));
	if(pRoot==NULL)
		return;

	g_HookAPI.EnableInvalidate(false);

	pFrame->SetManager(pManager);
	pRoot->SetManager(NULL,NULL);
	pFrame->SetRoot(pRoot);
	pFrame->Create(m_Manager.GetPaintWindow(),_T("FormTest"),UI_WNDSTYLE_FRAME,0,0,0,size.cx,size.cy);
	pFrame->CenterWindow();

	HWND m_hWnd=pFrame->GetHWND();
	MSG msg = { 0 };
	while( ::IsWindow(m_hWnd) && ::GetMessage(&msg, NULL, 0, 0) ) {
		if( msg.hwnd != m_hWnd ) {
			if( (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST) )
				continue;
			if( msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST ) 
				continue;
			if( msg.message == WM_SETCURSOR )
				continue;
		}
		if( !CPaintManagerUI::TranslateMessage(&msg) ) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		if( msg.message == WM_QUIT ) break;
	}
	if( msg.message == WM_QUIT ) ::PostQuitMessage(msg.wParam);

	g_HookAPI.EnableInvalidate(true);
}

BOOL CLayoutManager::IsEmptyForm() const
{
	return GetForm()->GetItemAt(0)==NULL;
}

CControlUI* CLayoutManager::CloneControls(CControlUI* pControl)
{
	if(pControl==NULL)
		return NULL;

	CContainerUI* pContainer=static_cast<CContainerUI*>(pControl->GetInterface(_T("Container")));
	if(pContainer==NULL)
		return CloneControl(pControl);

	CContainerUI* pCopyContainer=static_cast<CContainerUI*>(CloneControl(pContainer)->GetInterface(_T("Container")));
	pCopyContainer->SetAutoDestroy(false);
	pCopyContainer->RemoveAll();
	pCopyContainer->SetAutoDestroy(true);
	for(int i=0;i<pContainer->GetCount();i++)
	{
		CControlUI* pCopyControl=CloneControls(pContainer->GetItemAt(i));
		pCopyControl->SetManager(NULL,pCopyContainer);
		pCopyContainer->Add(pCopyControl);
	}

	return pCopyContainer;
}

CControlUI* CLayoutManager::CloneControl(CControlUI* pControl)
{
	CControlUI* pCopyControl = NULL;
	int nClass = ((ExtendedAttributes*)pControl->GetTag())->nClass;
	switch(nClass)
	{
	case classControl:
		pCopyControl = new CControlUI(*static_cast<CControlUI*>(pControl->GetInterface(_T("Control"))));
		break;
	case classLabel:
		pCopyControl = new CLabelUI(*static_cast<CLabelUI*>(pControl->GetInterface(_T("Label"))));
		break;
	case classText:
		pCopyControl = new CTextUI(*static_cast<CTextUI*>(pControl->GetInterface(_T("Text"))));
		break;
	case classButton:
		pCopyControl = new CButtonUI(*static_cast<CButtonUI*>(pControl->GetInterface(_T("Button"))));
		break;
	case classEdit:
		pCopyControl = new CEditUI(*static_cast<CEditUI*>(pControl->GetInterface(_T("Edit"))));
		break;
	case classOption:
		pCopyControl = new COptionUI(*static_cast<COptionUI*>(pControl->GetInterface(_T("Option"))));
		break;
	case classProgress:
		pCopyControl = new CProgressUI(*static_cast<CProgressUI*>(pControl->GetInterface(_T("Progress"))));
		break;
	case classSlider:
		pCopyControl = new CSliderUI(*static_cast<CSliderUI*>(pControl->GetInterface(_T("Slider"))));
		break;
	case classCombo:
		pCopyControl = new CComboUI(*static_cast<CComboUI*>(pControl->GetInterface(_T("Combo"))));
		break;
	case classActiveX:
		pCopyControl = new CActiveXUI(*static_cast<CActiveXUI*>(pControl->GetInterface(_T("ActiveX"))));
		break;
	case classContainer:
		pCopyControl = new CContainerUI(*static_cast<CContainerUI*>(pControl->GetInterface(_T("Container"))));
		break;
	case classVerticalLayout:
		pCopyControl = new CVerticalLayoutUI(*static_cast<CVerticalLayoutUI*>(pControl->GetInterface(_T("VerticalLayout"))));
		break;
	case classHorizontalLayout:
		pCopyControl = new CHorizontalLayoutUI(*static_cast<CHorizontalLayoutUI*>(pControl->GetInterface(_T("HorizontalLayout"))));
		break;
	case classDialogLayout:
		pCopyControl = new CDialogLayoutUI(*static_cast<CDialogLayoutUI*>(pControl->GetInterface(_T("DialogLayout"))));
		break;
	case classTileLayout:
		pCopyControl = new CTileLayoutUI(*static_cast<CTileLayoutUI*>(pControl->GetInterface(_T("TileLayout"))));
		break;
	case classTabLayout:
		pCopyControl = new CTabLayoutUI(*static_cast<CTabLayoutUI*>(pControl->GetInterface(_T("TabLayout"))));
		break;
	case classListHeaderItem:
		pCopyControl = new CListHeaderItemUI(*static_cast<CListHeaderItemUI*>(pControl->GetInterface(_T("ListHeaderItem"))));
		break;
	case classListTextElement:
		pCopyControl = new CListTextElementUI(*static_cast<CListTextElementUI*>(pControl->GetInterface(_T("ListTextElement"))));
		break;
	case classListLabelElement:
		pCopyControl = new CListLabelElementUI(*static_cast<CListLabelElementUI*>(pControl->GetInterface(_T("ListLabelElement"))));
		break;
	case classListContainerElement:
		pCopyControl = new CListContainerElementUI(*static_cast<CListContainerElementUI*>(pControl->GetInterface(_T("ListContainerElement"))));
		break;
	}

	return pCopyControl;
}

void CLayoutManager::AlignLeft(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	SIZE szXYLeft=pFocused->GetFixedXY();

	for (int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl->GetParent()!=pFocused->GetParent())
			continue;

		SIZE szXY=pControl->GetFixedXY();
		szXY.cx=szXYLeft.cx;
		pControl->SetFixedXY(szXY);
	}

	CControlUI* pParent=pFocused->GetParent();
	if(pParent)
		pParent->SetPos(pParent->GetPos());
}

void CLayoutManager::AlignRight(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	SIZE szXYRight=pFocused->GetFixedXY();
	int nWidth=pFocused->GetFixedWidth();

	for (int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl->GetParent()!=pFocused->GetParent())
			continue;

		SIZE szXY=pControl->GetFixedXY();
		szXY.cx=szXYRight.cx+nWidth-pControl->GetFixedWidth();
		pControl->SetFixedXY(szXY);
	}

	CControlUI* pParent=pFocused->GetParent();
	if(pParent)
		pParent->SetPos(pParent->GetPos());
}

void CLayoutManager::AlignTop(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	SIZE szXYTop=pFocused->GetFixedXY();

	for (int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl->GetParent()!=pFocused->GetParent())
			continue;

		SIZE szXY=pControl->GetFixedXY();
		szXY.cy=szXYTop.cy;
		pControl->SetFixedXY(szXY);
	}

	CControlUI* pParent=pFocused->GetParent();
	if(pParent)
		pParent->SetPos(pParent->GetPos());
}

void CLayoutManager::AlignBottom(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	SIZE szXYBottom=pFocused->GetFixedXY();
	int nHeight=pFocused->GetFixedHeight();

	for (int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl->GetParent()!=pFocused->GetParent())
			continue;

		SIZE szXY=pControl->GetFixedXY();
		szXY.cy=szXYBottom.cy+nHeight-pControl->GetFixedHeight();
		pControl->SetFixedXY(szXY);
	}

	CControlUI* pParent=pFocused->GetParent();
	if(pParent)
		pParent->SetPos(pParent->GetPos());
}

void CLayoutManager::AlignCenterVertically(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	CControlUI* pParent=pFocused->GetParent();
	if(!pParent)
		return;

	RECT rcParent=pParent->GetPos();

	CRect rectUnion;
	rectUnion.SetRectEmpty();
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl->GetParent()!=pParent)
		{
			arrSelected.RemoveAt(i);
			continue;
		}

		rectUnion.UnionRect(&rectUnion,&pControl->GetPos());
	}

	int nOffsetY=(rcParent.top+rcParent.bottom)/2-(rectUnion.top+rectUnion.bottom)/2;
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);

		SIZE szXY=pControl->GetFixedXY();
		szXY.cy+=nOffsetY;
		pControl->SetFixedXY(szXY);
	}

	pParent->SetPos(pParent->GetPos());
}

void CLayoutManager::AlignCenterHorizontally(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	CControlUI* pParent=pFocused->GetParent();
	if(!pParent)
		return;

	RECT rcParent=pParent->GetPos();

	CRect rectUnion;
	rectUnion.SetRectEmpty();
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl->GetParent()!=pParent)
		{
			arrSelected.RemoveAt(i);
			continue;
		}

		rectUnion.UnionRect(&rectUnion,&pControl->GetPos());
	}

	int nOffsetX=(rcParent.left+rcParent.right)/2-(rectUnion.left+rectUnion.right)/2;
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);

		SIZE szXY=pControl->GetFixedXY();
		szXY.cx+=nOffsetX;
		pControl->SetFixedXY(szXY);
	}

	pParent->SetPos(pParent->GetPos());
}

void CLayoutManager::AlignHorizontal(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	CControlUI* pParent=pFocused->GetParent();
	if(!pParent)
		return;

	CRect rectUnion;
	rectUnion.SetRectEmpty();
	int nTotalWidth=0;
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl->GetParent()!=pParent)
		{
			arrSelected.RemoveAt(i);
			continue;
		}
		nTotalWidth+=pControl->GetWidth();
		rectUnion.UnionRect(&rectUnion,&pControl->GetPos());
	}

	int nCount=arrSelected.GetSize();
	if(nCount<3)
		return;
	int nSpaceX=(rectUnion.Width()-nTotalWidth)/(nCount-1);
	int nMin;
	for(int i=0;i<nCount-1;i++)
	{
		nMin=i;
		int j;
		for(j=i+1;j<nCount;j++)
		{
			if(arrSelected[j]->GetX()<arrSelected[nMin]->GetX())
				nMin=j;
		}
		if(i!=nMin)
		{
			CControlUI* pControl=arrSelected[i];
			arrSelected[i]=arrSelected[nMin];
			arrSelected[nMin]=pControl;
		}
	}

	for(int i=1;i<nCount-1;i++)
	{
		int right=arrSelected[i-1]->GetFixedXY().cx+arrSelected[i-1]->GetWidth();
		SIZE szXY=arrSelected[i]->GetFixedXY();
		szXY.cx=right+nSpaceX;
		arrSelected[i]->SetFixedXY(szXY);
	}

	pParent->SetPos(pParent->GetPos());
}

void CLayoutManager::AlignVertical(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	CControlUI* pParent=pFocused->GetParent();
	if(!pParent)
		return;

	CRect rectUnion;
	rectUnion.SetRectEmpty();
	int nTotalHeight=0;
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl->GetParent()!=pParent)
		{
			arrSelected.RemoveAt(i);
			continue;
		}
		nTotalHeight+=pControl->GetHeight();
		rectUnion.UnionRect(&rectUnion,&pControl->GetPos());
	}

	int nCount=arrSelected.GetSize();
	if(nCount<3)
		return;
	int nSpaceY=(rectUnion.Height()-nTotalHeight)/(nCount-1);
	int nMin;
	for(int i=0;i<nCount-1;i++)
	{
		nMin=i;
		int j;
		for(j=i+1;j<nCount;j++)
		{
			if(arrSelected[j]->GetY()<arrSelected[nMin]->GetY())
				nMin=j;
		}
		if(i!=nMin)
		{
			CControlUI* pControl=arrSelected[i];
			arrSelected[i]=arrSelected[nMin];
			arrSelected[nMin]=pControl;
		}
	}

	for(int i=1;i<nCount-1;i++)
	{
		int bottom=arrSelected[i-1]->GetFixedXY().cy+arrSelected[i-1]->GetHeight();
		SIZE szXY=arrSelected[i]->GetFixedXY();
		szXY.cy=bottom+nSpaceY;
		arrSelected[i]->SetFixedXY(szXY);
	}

	pParent->SetPos(pParent->GetPos());
}

void CLayoutManager::AlignSameWidth(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	int nWidth=pFocused->GetFixedWidth();

	CDelayRepos DelayPos;
	for (int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl==pFocused)
			continue;
		
		pControl->SetFixedWidth(nWidth);
		DelayPos.AddParent(pControl->GetParent());
	}
	DelayPos.Repos();
}

void CLayoutManager::AlignSameHeight(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	int nHeight=pFocused->GetFixedHeight();

	CDelayRepos DelayPos;
	for (int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl==pFocused)
			continue;

		pControl->SetFixedHeight(nHeight);
		DelayPos.AddParent(pControl->GetParent());
	}
	DelayPos.Repos();
}

void CLayoutManager::AlignSameSize(CControlUI* pFocused,CArray<CControlUI*,CControlUI*>& arrSelected)
{
	int nWidth=pFocused->GetFixedWidth();
	int nHeight=pFocused->GetFixedHeight();

	CDelayRepos DelayPos;
	for (int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);
		if(pControl==pFocused)
			continue;

		pControl->SetFixedWidth(nWidth);
		pControl->SetFixedHeight(nHeight);
		DelayPos.AddParent(pControl->GetParent());
	}
	DelayPos.Repos();
}

void CLayoutManager::ShowGrid(bool bShow)
{
	m_bShowGrid=bShow;
}

bool CLayoutManager::IsShowGrid() const
{
	return m_bShowGrid;
}

void CLayoutManager::ShowAuxBorder(bool bShow)
{
	m_bShowAuxBorder=bShow;
}

bool CLayoutManager::IsShowAuxBorder() const
{
	return m_bShowAuxBorder;
}

void CLayoutManager::MicoMoveUp(CArray<CControlUI*,CControlUI*>& arrSelected,int nMoved)
{
	CDelayRepos DelayPos;
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);

		SIZE szXY=pControl->GetFixedXY();
		szXY.cy-=nMoved;
		pControl->SetFixedXY(szXY);
		DelayPos.AddParent(pControl->GetParent());
	}
	DelayPos.Repos();
}

void CLayoutManager::MicoMoveDown(CArray<CControlUI*,CControlUI*>& arrSelected,int nMoved)
{
	CDelayRepos DelayPos;
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);

		SIZE szXY=pControl->GetFixedXY();
		szXY.cy+=nMoved;
		pControl->SetFixedXY(szXY);
		DelayPos.AddParent(pControl->GetParent());
	}
	DelayPos.Repos();
}

void CLayoutManager::MicoMoveLeft(CArray<CControlUI*,CControlUI*>& arrSelected,int nMoved)
{
	CDelayRepos DelayPos;
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);

		SIZE szXY=pControl->GetFixedXY();
		szXY.cx-=nMoved;
		pControl->SetFixedXY(szXY);
		DelayPos.AddParent(pControl->GetParent());
	}
	DelayPos.Repos();
}

void CLayoutManager::MicoMoveRight(CArray<CControlUI*,CControlUI*>& arrSelected,int nMoved)
{
	CDelayRepos DelayPos;
	for(int i=0;i<arrSelected.GetSize();i++)
	{
		CControlUI* pControl=arrSelected.GetAt(i);

		SIZE szXY=pControl->GetFixedXY();
		szXY.cx+=nMoved;
		pControl->SetFixedXY(szXY);
		DelayPos.AddParent(pControl->GetParent());
	}
	DelayPos.Repos();
}

void CLayoutManager::SaveControlProperty(CControlUI* pControl, TiXmlElement* pNode)
{
	TCHAR szBuf[MAX_PATH] = {0};

	if(pControl->GetName() && _tcslen(pControl->GetName()) > 0)
		pNode->SetAttribute("name", StringConvertor::WideToUtf8(pControl->GetName()));

	if(pControl->GetText() && _tcslen(pControl->GetText()) > 0)
		pNode->SetAttribute("text", StringConvertor::WideToUtf8(pControl->GetText()));

	if(pControl->GetToolTip() && _tcslen(pControl->GetToolTip()) > 0)
		pNode->SetAttribute("tooltip", StringConvertor::WideToUtf8(pControl->GetToolTip()));

	if(!pControl->IsVisible() && !((static_cast<IContainerUI*>(pControl->GetInterface(_T("IContainer"))) != NULL) && (static_cast<CContainerUI*>(pControl->GetInterface(_T("Container"))) != NULL)))
	{
		CControlUI* pParent = pControl->GetParent();
		if((pParent != NULL) && ((static_cast<IContainerUI*>(pParent->GetInterface(_T("IContainer"))) != NULL) && (static_cast<CContainerUI*>(pParent->GetInterface(_T("Container"))) != NULL)))
		{
			// ���ͬһ��������Ԫ�ض��ǲ��ɼ��ģ�����������
			bool bVisible = false;
			CContainerUI* pContainerUI = static_cast<CContainerUI*>(pParent->GetInterface(_T("Container")));
			for( int it = 0; it < pContainerUI->GetCount(); it++ )
			{
				CControlUI* pControl = static_cast<CControlUI*>(pContainerUI->GetItemAt(it));
				bVisible = pControl->IsVisible();
				if(bVisible)
					break;
			}
			if(bVisible)
				pNode->SetAttribute("visible", "false");
		}
	}

	if(pControl->GetShortcut() != _T('\0'))
	{
		_stprintf_s(szBuf, _T("%c"),pControl->GetShortcut());
		pNode->SetAttribute("shortcut", StringConvertor::WideToUtf8(szBuf));
	}

	if(pControl->GetBorderSize() != 1)
	{
		_stprintf_s(szBuf, _T("%d"),pControl->GetBorderSize());
		pNode->SetAttribute("bordersize", StringConvertor::WideToUtf8(szBuf));
	}

	if(pControl->IsFloat())
	{
		pNode->SetAttribute("float", "true");

		_stprintf_s(szBuf, _T("%d,%d,%d,%d"), pControl->GetFixedXY().cx, pControl->GetFixedXY().cy, pControl->GetFixedXY().cx + pControl->GetFixedWidth(), pControl->GetFixedXY().cy + pControl->GetFixedHeight());
		pNode->SetAttribute("pos", StringConvertor::WideToUtf8(szBuf));
	}
	else
	{
		if((pControl->GetFixedXY().cx != 0)
			|| (pControl->GetFixedXY().cy != 0)
			|| (pControl->GetFixedWidth() != 0)
			|| (pControl->GetFixedHeight() != 0))
		{
			_stprintf_s(szBuf, _T("%d,%d,%d,%d"), pControl->GetFixedXY().cx, pControl->GetFixedXY().cy, pControl->GetFixedXY().cx + pControl->GetFixedWidth(), pControl->GetFixedXY().cy + pControl->GetFixedHeight());
			pNode->SetAttribute("pos", StringConvertor::WideToUtf8(szBuf));
		}

		if(pControl->GetFixedWidth() > 0)
		{
			_stprintf_s(szBuf, _T("%d"), pControl->GetFixedWidth());
			pNode->SetAttribute("width", StringConvertor::WideToUtf8(szBuf));
		}

		if(pControl->GetFixedHeight() > 0)
		{
			_stprintf_s(szBuf, _T("%d"), pControl->GetFixedHeight());
			pNode->SetAttribute("height", StringConvertor::WideToUtf8(szBuf));
		}
	}

	RECT rcPadding = pControl->GetPadding();
	if((rcPadding.left != 0) || (rcPadding.right != 0) || (rcPadding.bottom != 0) || (rcPadding.top != 0))
	{
		_stprintf_s(szBuf, _T("%d,%d,%d,%d"), rcPadding.left, rcPadding.top, rcPadding.right, rcPadding.bottom);
		pNode->SetAttribute("padding", StringConvertor::WideToUtf8(szBuf));
	}

	if(pControl->GetBkImage() && _tcslen(pControl->GetBkImage()) > 0)
	{
		pNode->SetAttribute("bkimage", StringConvertor::WideToUtf8(ConvertImageFileName(pControl->GetBkImage())));
	}

	if(pControl->GetBkColor() != 0)
	{
		DWORD dwColor = pControl->GetBkColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("bkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pControl->GetBkColor2() != 0)
	{
		DWORD dwColor = pControl->GetBkColor2();
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));

		pNode->SetAttribute("bkcolor2", StringConvertor::WideToUtf8(szBuf));
	}

	if(pControl->GetBorderColor() != 0)
	{
		DWORD dwColor = pControl->GetBorderColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("bordercolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pControl->GetFocusBorderColor() != 0)
	{
		DWORD dwColor = pControl->GetFocusBorderColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("focusbordercolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pControl->GetMaxHeight() != 9999)
	{
		_stprintf_s(szBuf, _T("%d"), pControl->GetMaxHeight());
		pNode->SetAttribute("maxheight", StringConvertor::WideToUtf8(szBuf));
	}

	if(pControl->GetMaxWidth() != 9999)
	{
		_stprintf_s(szBuf, _T("%d"), pControl->GetMaxWidth());
		pNode->SetAttribute("maxwidth", StringConvertor::WideToUtf8(szBuf));
	}

	if(pControl->GetMinWidth() != 0)
	{
		_stprintf_s(szBuf, _T("%d"), pControl->GetMinWidth());
		pNode->SetAttribute("minwidth", StringConvertor::WideToUtf8(szBuf));
	}

	if(pControl->GetMinHeight() != 0)
	{
		_stprintf_s(szBuf, _T("%d"), pControl->GetMinHeight());
		pNode->SetAttribute("minheight", StringConvertor::WideToUtf8(szBuf));
	}

	TRelativePosUI relativePos = pControl->GetRelativePos();
	if(relativePos.bRelative)
	{
		_stprintf_s(szBuf, _T("%d,%d,%d,%d"), relativePos.nMoveXPercent, relativePos.nMoveYPercent, relativePos.nZoomXPercent, relativePos.nZoomYPercent);
		pNode->SetAttribute("relativepos", StringConvertor::WideToUtf8(szBuf));
	}
}

void CLayoutManager::SaveLabelProperty(CControlUI* pControl, TiXmlElement* pNode)
{
	SaveControlProperty(pControl, pNode);

	CLabelUI* pLabelUI = static_cast<CLabelUI*>(pControl->GetInterface(_T("Label")));

	TCHAR szBuf[MAX_PATH] = {0};

	RECT rcTextPadding = pLabelUI->GetTextPadding();
	if((rcTextPadding.left != 0) || (rcTextPadding.right != 0) || (rcTextPadding.bottom != 0) || (rcTextPadding.top != 0))
	{
		_stprintf_s(szBuf, _T("%d,%d,%d,%d"), rcTextPadding.left, rcTextPadding.top, rcTextPadding.right, rcTextPadding.bottom);
		pNode->SetAttribute("textpadding", StringConvertor::WideToUtf8(szBuf));
	}

	if(pLabelUI->GetTextColor() != 0)
	{
		DWORD dwColor = pLabelUI->GetTextColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("textcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pLabelUI->GetDisabledTextColor() != 0)
	{
		DWORD dwColor = pLabelUI->GetDisabledTextColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("disabledtextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	int nFont = pLabelUI->GetFont();
	if(nFont != -1)
		pNode->SetAttribute("font", nFont);

	if(pLabelUI->IsShowHtml())
		pNode->SetAttribute("showhtml", "true");

	std::wstring tstrAlgin;
	UINT uTextStyle = pLabelUI->GetTextStyle();

	if(uTextStyle & DT_LEFT)
		tstrAlgin = _T("left");

	if(uTextStyle & DT_CENTER)
		tstrAlgin = _T("center");

	if(uTextStyle & DT_RIGHT)
		tstrAlgin = _T("right");

	if(uTextStyle & DT_TOP)
		tstrAlgin += _T("top");

	if(uTextStyle & DT_BOTTOM)
		tstrAlgin += _T("bottom");

	if(uTextStyle & DT_WORDBREAK)
		tstrAlgin += _T("wrap");

	if(!tstrAlgin.empty())
		pNode->SetAttribute("align", StringConvertor::WideToUtf8(tstrAlgin.c_str()));
}

void CLayoutManager::SaveButtonProperty(CControlUI* pControl, TiXmlElement* pNode)
{
	SaveLabelProperty(pControl, pNode);
	TCHAR szBuf[MAX_PATH] = {0};

	CButtonUI* pButtonUI = static_cast<CButtonUI*>(pControl->GetInterface(_T("Button")));
	if(pButtonUI->GetNormalImage() && _tcslen(pButtonUI->GetNormalImage()) > 0)
		pNode->SetAttribute("normalimage", StringConvertor::WideToUtf8(ConvertImageFileName(pButtonUI->GetNormalImage())));

	if(pButtonUI->GetHotImage() && _tcslen(pButtonUI->GetHotImage()) > 0)
		pNode->SetAttribute("hotimage", StringConvertor::WideToUtf8(ConvertImageFileName(pButtonUI->GetHotImage())));

	if(pButtonUI->GetPushedImage() && _tcslen(pButtonUI->GetPushedImage()) > 0)
		pNode->SetAttribute("pushedimage", StringConvertor::WideToUtf8(ConvertImageFileName(pButtonUI->GetPushedImage())));

	if(pButtonUI->GetFocusedImage() && _tcslen(pButtonUI->GetFocusedImage()) > 0)
		pNode->SetAttribute("focusedimage", StringConvertor::WideToUtf8(ConvertImageFileName(pButtonUI->GetFocusedImage())));

	if(pButtonUI->GetDisabledImage() && _tcslen(pButtonUI->GetDisabledImage()) > 0)
		pNode->SetAttribute("disabledimage", StringConvertor::WideToUtf8(ConvertImageFileName(pButtonUI->GetDisabledImage())));

	if(pButtonUI->GetFocusedTextColor() != 0)
	{
		DWORD dwColor = pButtonUI->GetFocusedTextColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("focusedtextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pButtonUI->GetPushedTextColor() != 0)
	{
		DWORD dwColor = pButtonUI->GetPushedTextColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("pushedtextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pButtonUI->GetHotTextColor() != 0)
	{
		DWORD dwColor = pButtonUI->GetHotTextColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("hottextcolor", StringConvertor::WideToUtf8(szBuf));
	}
}

void CLayoutManager::SaveOptionProperty(CControlUI* pControl, TiXmlElement* pNode)
{
	SaveButtonProperty(pControl, pNode);
	COptionUI* pOptionUI = static_cast<COptionUI*>(pControl->GetInterface(_T("Option")));

	TCHAR szBuf[MAX_PATH] = {0};

	if(pOptionUI->GetGroup() && _tcslen(pOptionUI->GetGroup()))
		pNode->SetAttribute("group",StringConvertor::WideToUtf8(pOptionUI->GetGroup()));

	if(pOptionUI->IsSelected())
		pNode->SetAttribute("selected", pOptionUI->IsSelected()?"true":"false");

	if(pOptionUI->GetForeImage() && _tcslen(pOptionUI->GetForeImage()) > 0)
		pNode->SetAttribute("foreimage", StringConvertor::WideToUtf8(ConvertImageFileName(pOptionUI->GetForeImage())));

	if(pOptionUI->GetSelectedImage() && _tcslen(pOptionUI->GetSelectedImage()) > 0)
		pNode->SetAttribute("selectedimage", StringConvertor::WideToUtf8(ConvertImageFileName(pOptionUI->GetSelectedImage())));
}

void CLayoutManager::SaveProgressProperty(CControlUI* pControl, TiXmlElement* pNode)
{
	SaveLabelProperty(pControl, pNode);
	CProgressUI* pProgressUI = static_cast<CProgressUI*>(pControl->GetInterface(_T("Progress")));

	TCHAR szBuf[MAX_PATH] = {0};

	if(pProgressUI->GetFgImage() && _tcslen(pProgressUI->GetFgImage()) > 0)
		pNode->SetAttribute("fgimage", StringConvertor::WideToUtf8(ConvertImageFileName(pProgressUI->GetFgImage())));

	_stprintf_s(szBuf, _T("%d"), pProgressUI->GetMinValue());
	pNode->SetAttribute("min", StringConvertor::WideToUtf8(szBuf));


	_stprintf_s(szBuf, _T("%d"), pProgressUI->GetMaxValue());
	pNode->SetAttribute("max", StringConvertor::WideToUtf8(szBuf));

	_stprintf_s(szBuf, _T("%d"), pProgressUI->GetValue());
	pNode->SetAttribute("value", StringConvertor::WideToUtf8(szBuf));

	if(pProgressUI->IsHorizontal())
		pNode->SetAttribute("hor", pProgressUI->IsHorizontal()?"true":"false");
}

void CLayoutManager::SaveSliderProperty(CControlUI* pControl, TiXmlElement* pNode)
{
	SaveProgressProperty(pControl, pNode);

	CSliderUI* pSliderUI = static_cast<CSliderUI*>(pControl->GetInterface(_T("Slider")));
	TCHAR szBuf[MAX_PATH] = {0};

	if(pSliderUI->GetThumbImage() && _tcslen(pSliderUI->GetThumbImage()) > 0)
		pNode->SetAttribute("thumbimage", StringConvertor::WideToUtf8(ConvertImageFileName(pSliderUI->GetThumbImage())));

	if(pSliderUI->GetThumbHotImage() && _tcslen(pSliderUI->GetThumbHotImage()) > 0)
		pNode->SetAttribute("thumbhotimage", StringConvertor::WideToUtf8(ConvertImageFileName(pSliderUI->GetThumbHotImage())));

	if(pSliderUI->GetThumbPushedImage() && _tcslen(pSliderUI->GetThumbPushedImage()) > 0)
		pNode->SetAttribute("thumbpushedimage", StringConvertor::WideToUtf8(ConvertImageFileName(pSliderUI->GetThumbPushedImage())));

	_stprintf_s(szBuf, _T("%d,%d"), pSliderUI->GetThumbRect().right - pSliderUI->GetThumbRect().left, pSliderUI->GetThumbRect().bottom - pSliderUI->GetThumbRect().top);
	pNode->SetAttribute("thumbsize", StringConvertor::WideToUtf8(szBuf));
}

void CLayoutManager::SaveEditProperty(CControlUI* pControl, TiXmlElement* pNode)
{
	SaveLabelProperty(pControl, pNode);
	CEditUI* pEditUI = static_cast<CEditUI*>(pControl->GetInterface(_T("Edit")));

	TCHAR szBuf[MAX_PATH] = {0};	

	if(pEditUI->IsPasswordMode())
		pNode->SetAttribute("password", pEditUI->IsPasswordMode()?"true":"false");

	if(pEditUI->IsReadOnly())
		pNode->SetAttribute("readonly", pEditUI->IsReadOnly()?"true":"false");
}

void CLayoutManager::SaveScrollBarProperty(CControlUI* pControl, TiXmlElement* pNode)
{
	SaveControlProperty(pControl, pNode);
	CScrollBarUI* pScrollbarUI = static_cast<CScrollBarUI*>(pControl->GetInterface(_T("Scrollbar")));
	TCHAR szBuf[MAX_PATH] = {0};

	if(pScrollbarUI->GetBkNormalImage() && _tcslen(pScrollbarUI->GetBkNormalImage()) > 0)
		pNode->SetAttribute("bknormalimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetBkNormalImage())));

	if(pScrollbarUI->GetBkHotImage() && _tcslen(pScrollbarUI->GetBkHotImage()) > 0)
		pNode->SetAttribute("bkhotimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetBkHotImage())));

	if(pScrollbarUI->GetBkPushedImage() && _tcslen(pScrollbarUI->GetBkPushedImage()) > 0)
		pNode->SetAttribute("bkpushedimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetBkPushedImage())));

	if(pScrollbarUI->GetBkDisabledImage() && _tcslen(pScrollbarUI->GetBkDisabledImage()) > 0)
		pNode->SetAttribute("bkdisabledimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetBkDisabledImage())));

	if(pScrollbarUI->GetRailNormalImage() && _tcslen(pScrollbarUI->GetRailNormalImage()) > 0)
		pNode->SetAttribute("railnormalimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetRailNormalImage())));

	if(pScrollbarUI->GetRailHotImage() && _tcslen(pScrollbarUI->GetRailHotImage()) > 0)
		pNode->SetAttribute("railhotimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetRailHotImage())));

	if(pScrollbarUI->GetRailPushedImage() && _tcslen(pScrollbarUI->GetRailPushedImage()) > 0)
		pNode->SetAttribute("railpushedimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetRailPushedImage())));

	if(pScrollbarUI->GetRailDisabledImage() && _tcslen(pScrollbarUI->GetRailDisabledImage()) > 0)
		pNode->SetAttribute("raildisabledimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetRailDisabledImage())));

	if(pScrollbarUI->GetThumbNormalImage() && _tcslen(pScrollbarUI->GetThumbNormalImage()) > 0)
		pNode->SetAttribute("thumbnormalimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetThumbNormalImage())));

	if(pScrollbarUI->GetThumbHotImage() && _tcslen(pScrollbarUI->GetThumbHotImage()) > 0)
		pNode->SetAttribute("thumbhotimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetThumbHotImage())));

	if(pScrollbarUI->GetThumbPushedImage() && _tcslen(pScrollbarUI->GetThumbPushedImage()) > 0)
		pNode->SetAttribute("thumbpushedimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetThumbPushedImage())));

	if(pScrollbarUI->GetThumbDisabledImage() && _tcslen(pScrollbarUI->GetThumbDisabledImage()) > 0)
		pNode->SetAttribute("thumbdisabledimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetThumbDisabledImage())));

	if(pScrollbarUI->GetButton2NormalImage() && _tcslen(pScrollbarUI->GetButton2NormalImage()) > 0)
		pNode->SetAttribute("button2normalimage", StringConvertor::WideToUtf8((pScrollbarUI->GetButton2NormalImage())));

	if(pScrollbarUI->GetButton2HotImage() && _tcslen(pScrollbarUI->GetButton2HotImage()) > 0)
		pNode->SetAttribute("button2hotimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetButton2HotImage())));

	if(pScrollbarUI->GetButton2PushedImage() && _tcslen(pScrollbarUI->GetButton2PushedImage()) > 0)
		pNode->SetAttribute("button2pushedimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetButton2PushedImage())));

	if(pScrollbarUI->GetButton2DisabledImage() && _tcslen(pScrollbarUI->GetButton2DisabledImage()) > 0)
		pNode->SetAttribute("button2disabledimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetButton2DisabledImage())));

	if(pScrollbarUI->GetButton1NormalImage() && _tcslen(pScrollbarUI->GetButton1NormalImage()) > 0)
		pNode->SetAttribute("button1normalimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetButton1NormalImage())));

	if(pScrollbarUI->GetButton1HotImage() && _tcslen(pScrollbarUI->GetButton1HotImage()) > 0)
		pNode->SetAttribute("button1hotimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetButton1HotImage())));

	if(pScrollbarUI->GetButton1PushedImage() && _tcslen(pScrollbarUI->GetButton1PushedImage()) > 0)
		pNode->SetAttribute("button1pushedimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetButton1PushedImage())));

	if(pScrollbarUI->GetButton1DisabledImage() && _tcslen(pScrollbarUI->GetButton1DisabledImage()) > 0)
		pNode->SetAttribute("button1disabledimage", StringConvertor::WideToUtf8(ConvertImageFileName(pScrollbarUI->GetButton1DisabledImage())));

	_stprintf_s(szBuf, _T("%d"), pScrollbarUI->GetLineSize());
	pNode->SetAttribute("linesize", StringConvertor::WideToUtf8(szBuf));

	_stprintf_s(szBuf, _T("%d"), pScrollbarUI->GetScrollRange());
	pNode->SetAttribute("range", StringConvertor::WideToUtf8(szBuf));

	_stprintf_s(szBuf, _T("%d"), pScrollbarUI->GetScrollPos());
	pNode->SetAttribute("value", StringConvertor::WideToUtf8(szBuf));

	if(pScrollbarUI->IsHorizontal())
		pNode->SetAttribute("hor",pScrollbarUI->IsHorizontal()?"true":"false");
}

void CLayoutManager::SaveListProperty(CControlUI* pControl, TiXmlElement* pNode)
{
	SaveControlProperty(pControl, pNode);
	CListUI* pListUI = static_cast<CListUI*>(pControl->GetInterface(_T("CList")));

	TCHAR szBuf[MAX_PATH] = {0};

	if(pListUI->GetHeader() && !pListUI->GetHeader()->IsVisible())
		pNode->SetAttribute("header", "hidden");

	if(pListUI->GetHeader() && pListUI->GetHeader()->GetBkImage() && _tcslen(pListUI->GetHeader()->GetBkImage()) > 0)
		pNode->SetAttribute("headerbkimage", StringConvertor::WideToUtf8(ConvertImageFileName(pListUI->GetHeader()->GetBkImage())));	

	RECT rcTextPadding = pListUI->GetItemTextPadding();
	if((rcTextPadding.left != 0) || (rcTextPadding.right != 0) || (rcTextPadding.bottom != 0) || (rcTextPadding.top != 0))
	{
		_stprintf_s(szBuf, _T("%d,%d,%d,%d"), rcTextPadding.left, rcTextPadding.top, rcTextPadding.right, rcTextPadding.bottom);
		pNode->SetAttribute("itemtextpadding", StringConvertor::WideToUtf8(szBuf));
	}

	if(pListUI->GetItemTextColor() != 0)
	{
		DWORD dwColor = pListUI->GetItemTextColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemtextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pListUI->GetItemBkColor() != 0)
	{
		DWORD dwColor = pListUI->GetItemBkColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itembkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pListUI->GetSelectedItemTextColor() != 0)
	{
		DWORD dwColor = pListUI->GetSelectedItemTextColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemselectedtextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pListUI->GetSelectedItemBkColor() != 0)
	{
		DWORD dwColor = pListUI->GetSelectedItemBkColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemselectedbkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pListUI->GetHotItemTextColor() != 0)
	{
		DWORD dwColor = pListUI->GetHotItemTextColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemhottextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pListUI->GetHotItemBkColor() != 0)
	{
		DWORD dwColor = pListUI->GetHotItemBkColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemhotbkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pListUI->GetDisabledItemTextColor() != 0)
	{
		DWORD dwColor = pListUI->GetDisabledItemTextColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemdisabledtextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pListUI->GetDisabledItemBkColor() != 0)
	{
		DWORD dwColor = pListUI->GetDisabledItemBkColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemdisabledbkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pListUI->GetItemLineColor() != 0)
	{
		DWORD dwColor = pListUI->GetItemLineColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));

		pNode->SetAttribute("itemlinecolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pListUI->GetItemBkImage() && _tcslen(pListUI->GetItemBkImage()) > 0)
		pNode->SetAttribute("itembkimage", StringConvertor::WideToUtf8(ConvertImageFileName(pListUI->GetItemBkImage())));

	if(pListUI->GetSelectedItemImage() && _tcslen(pListUI->GetSelectedItemImage()) > 0)
		pNode->SetAttribute("itemselectedimage", StringConvertor::WideToUtf8(ConvertImageFileName(pListUI->GetSelectedItemImage())));

	if(pListUI->GetHotItemImage() && _tcslen(pListUI->GetHotItemImage()) > 0)
		pNode->SetAttribute("itemhotimage", StringConvertor::WideToUtf8(ConvertImageFileName(pListUI->GetHotItemImage())));

	if(pListUI->GetDisabledItemImage() && _tcslen(pListUI->GetDisabledItemImage()) > 0)
		pNode->SetAttribute("itemdisabledimage", StringConvertor::WideToUtf8(ConvertImageFileName(pListUI->GetDisabledItemImage())));

	if(pListUI->IsItemShowHtml())
		pNode->SetAttribute("itemshowhtml",pListUI->IsItemShowHtml()?"true":"false");

	CContainerUI* pContainerUI = static_cast<CContainerUI*>(pControl->GetInterface(_T("Container")));
	RECT rcInset = pContainerUI->GetInset();
	if((rcInset.left != 0) || (rcInset.right != 0) || (rcInset.bottom != 0) || (rcInset.top != 0))
	{
		_stprintf_s(szBuf, _T("%d,%d,%d,%d"), rcInset.left, rcInset.top, rcInset.right, rcInset.bottom);
		pNode->SetAttribute("inset", StringConvertor::WideToUtf8(szBuf));
	}

	if(pListUI->GetVerticalScrollBar())
		pNode->SetAttribute("vscrollbar","true");

	if(pListUI->GetHorizontalScrollBar())
		pNode->SetAttribute("hscrollbar","true");

	if(pListUI->GetHeader())
	{
		CContainerUI* pContainerUI = static_cast<CContainerUI*>(pListUI->GetHeader()->GetInterface(_T("Container")));
		for( int it = 0; it < pContainerUI->GetCount(); it++ )
		{
			CControlUI* pControl = static_cast<CControlUI*>(pContainerUI->GetItemAt(it));
			SaveProperties(pControl, pNode);
		}
	}
}

void CLayoutManager::SaveComboProperty(CControlUI* pControl, TiXmlElement* pNode)
{
	SaveContainerProperty(pControl, pNode);
	CComboUI* pComboUI = static_cast<CComboUI*>(pControl->GetInterface(_T("Combo")));

	TCHAR szBuf[MAX_PATH] = {0};

	if(pComboUI->GetNormalImage() && _tcslen(pComboUI->GetNormalImage()) > 0)
		pNode->SetAttribute("normalimage", StringConvertor::WideToUtf8(ConvertImageFileName(pComboUI->GetNormalImage())));

	if(pComboUI->GetHotImage() && _tcslen(pComboUI->GetHotImage()) > 0)
		pNode->SetAttribute("hotimage", StringConvertor::WideToUtf8(ConvertImageFileName(pComboUI->GetHotImage())));

	if(pComboUI->GetPushedImage() && _tcslen(pComboUI->GetPushedImage()) > 0)
		pNode->SetAttribute("pushedimage", StringConvertor::WideToUtf8(ConvertImageFileName(pComboUI->GetPushedImage())));

	if(pComboUI->GetFocusedImage() && _tcslen(pComboUI->GetFocusedImage()) > 0)
		pNode->SetAttribute("focusedimage", StringConvertor::WideToUtf8(ConvertImageFileName(pComboUI->GetFocusedImage())));

	if(pComboUI->GetDisabledImage() && _tcslen(pComboUI->GetDisabledImage()) > 0)
		pNode->SetAttribute("disabledimage", StringConvertor::WideToUtf8(ConvertImageFileName(pComboUI->GetDisabledImage())));

	if((pComboUI->GetDropBoxSize().cx != 0) || (pComboUI->GetDropBoxSize().cy != 0))
	{
		_stprintf_s(szBuf, _T("%d,%d"), pComboUI->GetDropBoxSize().cx, pComboUI->GetDropBoxSize().cy);
		pNode->SetAttribute("dropboxsize", StringConvertor::WideToUtf8(szBuf));
	}

	RECT rcItemPadding = pComboUI->GetItemTextPadding();
	if((rcItemPadding.left != 0) || (rcItemPadding.right != 0) || (rcItemPadding.bottom != 0) || (rcItemPadding.top != 0))
	{
		_stprintf_s(szBuf, _T("%d,%d,%d,%d"), rcItemPadding.left, rcItemPadding.top, rcItemPadding.right, rcItemPadding.bottom);
		pNode->SetAttribute("itemtextpadding", StringConvertor::WideToUtf8(szBuf));
	}

	if(pComboUI->GetItemTextColor() != 0)
	{
		DWORD dwColor = pComboUI->GetItemTextColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemtextcolor", StringConvertor::WideToUtf8(szBuf));

	}

	if(pComboUI->GetItemBkColor() != 0)
	{
		DWORD dwColor = pComboUI->GetItemBkColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itembkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pComboUI->GetSelectedItemTextColor() != 0)
	{
		DWORD dwColor = pComboUI->GetSelectedItemTextColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemselectedtextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pComboUI->GetSelectedItemBkColor() != 0)
	{
		DWORD dwColor = pComboUI->GetSelectedItemBkColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemselectedbkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pComboUI->GetHotItemTextColor() != 0)
	{
		DWORD dwColor = pComboUI->GetHotItemTextColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemhottextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pComboUI->GetHotItemBkColor() != 0)
	{
		DWORD dwColor = pComboUI->GetHotItemBkColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemhotbkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pComboUI->GetDisabledItemTextColor() != 0)
	{
		DWORD dwColor = pComboUI->GetDisabledItemTextColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemdisabledtextcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pComboUI->GetDisabledItemBkColor() != 0)
	{

		DWORD dwColor = pComboUI->GetDisabledItemBkColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemdisabledbkcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pComboUI->GetItemLineColor() != 0)
	{
		DWORD dwColor = pComboUI->GetItemLineColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("itemlinecolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pComboUI->GetItemBkImage() && _tcslen(pComboUI->GetItemBkImage()) > 0)
		pNode->SetAttribute("itembkimage", StringConvertor::WideToUtf8(ConvertImageFileName(pComboUI->GetItemBkImage())));

	if(pComboUI->GetSelectedItemImage() && _tcslen(pComboUI->GetSelectedItemImage()) > 0)
		pNode->SetAttribute("itemselectedimage", StringConvertor::WideToUtf8(ConvertImageFileName(pComboUI->GetSelectedItemImage())));

	if(pComboUI->GetHotItemImage() && _tcslen(pComboUI->GetHotItemImage()) > 0)
		pNode->SetAttribute("itemhotimage", StringConvertor::WideToUtf8(ConvertImageFileName(pComboUI->GetHotItemImage())));

	if(pComboUI->GetDisabledItemImage() && _tcslen(pComboUI->GetDisabledItemImage()) > 0)
		pNode->SetAttribute("itemdisabledimage", StringConvertor::WideToUtf8(ConvertImageFileName(pComboUI->GetDisabledItemImage())));

	if(pComboUI->IsItemShowHtml())
		pNode->SetAttribute("itemshowhtml",pComboUI->IsItemShowHtml()?"true":"false");

	CContainerUI* pContainerUI = static_cast<CContainerUI*>(pControl->GetInterface(_T("Container")));
	RECT rcInset = pContainerUI->GetInset();
	if((rcInset.left != 0) || (rcInset.right != 0) || (rcInset.bottom != 0) || (rcInset.top != 0))
	{
		_stprintf_s(szBuf, _T("%d,%d,%d,%d"), rcInset.left, rcInset.top, rcInset.right, rcInset.bottom);
		pNode->SetAttribute("inset", StringConvertor::WideToUtf8(szBuf));
	}
}

void CLayoutManager::SaveListHeaderItemProperty(CControlUI* pControl, TiXmlElement* pNode)
{
	SaveControlProperty(pControl, pNode);

	CListHeaderItemUI* pListHeaderItemUI = static_cast<CListHeaderItemUI*>(pControl->GetInterface(_T("ListHeaderItem")));

	TCHAR szBuf[MAX_PATH] = {0};

	if(pListHeaderItemUI->GetTextColor() != 0)
	{
		DWORD dwColor = pListHeaderItemUI->GetTextColor();					
		_stprintf_s(szBuf, _T("#%02X%02X%02X%02X"), HIBYTE(HIWORD(dwColor)), static_cast<BYTE>(GetBValue(dwColor)), static_cast<BYTE>(GetGValue(dwColor)), static_cast<BYTE>(GetRValue(dwColor)));
		pNode->SetAttribute("textcolor", StringConvertor::WideToUtf8(szBuf));
	}

	if(pListHeaderItemUI->GetSepWidth() != 0)
	{
		_stprintf_s(szBuf, _T("%d"), pListHeaderItemUI->GetSepWidth());
		pNode->SetAttribute("sepwidth", StringConvertor::WideToUtf8(szBuf));
	}

	if(!pListHeaderItemUI->IsDragable())
		pNode->SetAttribute("dragable", "false");

	std::wstring tstrAlgin;
	UINT uTextStyle = pListHeaderItemUI->GetTextStyle();

	if(uTextStyle & DT_LEFT)
		tstrAlgin = _T("left");

	if(uTextStyle & DT_CENTER)
		tstrAlgin = _T("center");

	if(uTextStyle & DT_RIGHT)
		tstrAlgin = _T("right");

	if(uTextStyle & DT_TOP)
		tstrAlgin += _T("top");

	if(uTextStyle & DT_BOTTOM)
		tstrAlgin += _T("bottom");

	if(uTextStyle & DT_WORDBREAK)
		tstrAlgin += _T("wrap");

	if(!tstrAlgin.empty())
		pNode->SetAttribute("align", StringConvertor::WideToUtf8(tstrAlgin.c_str()));

	if(pListHeaderItemUI->GetNormalImage() && _tcslen(pListHeaderItemUI->GetNormalImage()) > 0)
		pNode->SetAttribute("normalimage", StringConvertor::WideToUtf8(ConvertImageFileName(pListHeaderItemUI->GetNormalImage())));

	if(pListHeaderItemUI->GetHotImage() && _tcslen(pListHeaderItemUI->GetHotImage()) > 0)
		pNode->SetAttribute("hotimage", StringConvertor::WideToUtf8(ConvertImageFileName(pListHeaderItemUI->GetHotImage())));

	if(pListHeaderItemUI->GetPushedImage() && _tcslen(pListHeaderItemUI->GetPushedImage()) > 0)
		pNode->SetAttribute("pushedimage", StringConvertor::WideToUtf8(ConvertImageFileName(pListHeaderItemUI->GetPushedImage())));

	if(pListHeaderItemUI->GetFocusedImage() && _tcslen(pListHeaderItemUI->GetFocusedImage()) > 0)
		pNode->SetAttribute("focusedimage", StringConvertor::WideToUtf8(ConvertImageFileName(pListHeaderItemUI->GetFocusedImage())));

	if(pListHeaderItemUI->GetSepImage() && _tcslen(pListHeaderItemUI->GetSepImage()) > 0)
		pNode->SetAttribute("sepimage", StringConvertor::WideToUtf8(ConvertImageFileName(pListHeaderItemUI->GetSepImage())));
}

void CLayoutManager::SaveListElementProperty(CControlUI* pControl, TiXmlElement* pNode)
{
	SaveControlProperty(pControl, pNode);
}

void CLayoutManager::SaveContainerProperty(CControlUI* pControl, TiXmlElement* pNode)
{
	SaveControlProperty(pControl, pNode);
	CContainerUI* pContainerUI = static_cast<CContainerUI*>(pControl->GetInterface(_T("Container")));

	TCHAR szBuf[MAX_PATH] = {0};

	RECT rcInset = pContainerUI->GetInset();
	if((rcInset.left != 0) || (rcInset.right != 0) || (rcInset.bottom != 0) || (rcInset.top != 0))
	{
		_stprintf_s(szBuf, _T("%d,%d,%d,%d"), rcInset.left, rcInset.top, rcInset.right, rcInset.bottom);
		pNode->SetAttribute("inset", StringConvertor::WideToUtf8(szBuf));
	}
}

void CLayoutManager::SaveHorizontalLayoutProperty(CControlUI* pControl, TiXmlElement* pNode)
{
	SaveContainerProperty(pControl, pNode);
}

void CLayoutManager::SaveTileLayoutProperty(CControlUI* pControl, TiXmlElement* pNode)
{
	SaveContainerProperty(pControl, pNode);
}

void CLayoutManager::SaveProperties(CControlUI* pControl, TiXmlElement* pParentNode
									, BOOL bSaveChildren/* = TRUE*/)
{
	if((pControl == NULL) || (pParentNode == NULL))
		return;

	CString strClass = pControl->GetClass();
	strClass = strClass.Mid(0, strClass.GetLength() - 2);
	TiXmlElement* pNode = new TiXmlElement(StringConvertor::WideToUtf8(strClass.GetBuffer()));
	ExtendedAttributes* pExtended=(ExtendedAttributes*)pControl->GetTag();
	switch(pExtended->nClass)
	{
	case classControl:
		SaveControlProperty(pControl, pNode);
		break;
	case classLabel:
	case classText:
		SaveLabelProperty(pControl, pNode);
		break;
	case classButton:
		SaveButtonProperty(pControl, pNode);
		break;
	case classEdit:
		SaveEditProperty(pControl, pNode);
		break;
	case classOption:
		SaveOptionProperty(pControl, pNode);
		break;
	case classProgress:
		SaveProgressProperty(pControl, pNode);
		break;
	case classSlider:
		SaveSliderProperty(pControl, pNode);
		break;
	case classCombo: 
		SaveComboProperty(pControl, pNode);
		break;
	case classActiveX:
// 		SaveActiveXProperty(pControl, pNode);
		break;
	case classContainer:
	case classVerticalLayout:
	case classDialogLayout:
	case classTabLayout:
		SaveContainerProperty(pControl, pNode);
		break;
	case classHorizontalLayout:
		SaveHorizontalLayoutProperty(pControl, pNode);
		break;
	case classTileLayout:
		SaveTileLayoutProperty(pControl, pNode);
		break;
	default:
		delete pNode;
		return;
	}
	TiXmlNode* pNodeElement = pParentNode->InsertEndChild(*pNode);
	delete pNode;

	if(bSaveChildren == FALSE)
		return;

	CContainerUI* pContainer = static_cast<CContainerUI*>(pControl->GetInterface(_T("Container")));
	if(pContainer == NULL)
		return;
	for( int it = 0; it < pContainer->GetCount(); it++ )
	{
		CControlUI* pControl = static_cast<CControlUI*>(pContainer->GetItemAt(it));
		SaveProperties(pControl, pNodeElement->ToElement());
	}
}

void CLayoutManager::SaveSkinFile(LPCTSTR pstrPathName)
{
	CString strPathName(pstrPathName);
	int nPos = strPathName.ReverseFind(_T('\\'));
	if(nPos == -1)
		return;
	m_strSkinDir = strPathName.Left(nPos + 1);

	HANDLE hFile = ::CreateFile(pstrPathName, GENERIC_ALL, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}
	if(hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	TCHAR szBuf[MAX_PATH] = {0};
	TiXmlDocument xmlDoc(StringConvertor::WideToAnsi(pstrPathName));
	TiXmlDeclaration Declaration("1.0","utf-8","yes");
	xmlDoc.InsertEndChild(Declaration);

	TiXmlElement* pFormElm = new TiXmlElement("Window");
	CFormUI* pForm = GetForm();
	ASSERT(pForm);
	SIZE szSize = pForm->GetInitSize();
	if((szSize.cx != 0) || (szSize.cy != 0))
	{
		_stprintf_s(szBuf, _T("%d,%d"), szSize.cx, szSize.cy);
		pFormElm->SetAttribute("size", StringConvertor::WideToUtf8(szBuf));
	}
	RECT rcSizeBox = pForm->GetSizeBox();
	if((rcSizeBox.left != 0) || (rcSizeBox.right != 0) || (rcSizeBox.bottom != 0) || (rcSizeBox.top != 0))
	{
		_stprintf_s(szBuf, _T("%d,%d,%d,%d"), rcSizeBox.left, rcSizeBox.top, rcSizeBox.right, rcSizeBox.bottom);
		pFormElm->SetAttribute("sizebox", StringConvertor::WideToUtf8(szBuf));
	}
	else
		pFormElm->SetAttribute("sizebox", "4,4,6,6");
	RECT rcCaption = pForm->GetCaptionRect();
	if((rcCaption.left != 0) || (rcCaption.right != 0) || (rcCaption.bottom != 0) || (rcCaption.top != 0))
	{
		_stprintf_s(szBuf, _T("%d,%d,%d,%d"), rcCaption.left, rcCaption.top, rcCaption.right, rcCaption.bottom);
		pFormElm->SetAttribute("caption", StringConvertor::WideToUtf8(szBuf));
	}
	else
		pFormElm->SetAttribute("caption", "0,0,0,30");
	SIZE szMinWindow = pForm->GetMinInfo();
	if((szMinWindow.cx != 0) || (szMinWindow.cy != 0))
	{
		_stprintf_s(szBuf, _T("%d,%d"), szMinWindow.cx, szMinWindow.cy);
		pFormElm->SetAttribute("mininfo", StringConvertor::WideToUtf8(szBuf));
	}
	SIZE szMaxWindow = pForm->GetMaxInfo();
	if((szMaxWindow.cx != 0) || (szMaxWindow.cy != 0))
	{
		_stprintf_s(szBuf, _T("%d,%d"), szMinWindow.cx, szMinWindow.cy);
		pFormElm->SetAttribute("maxinfo", StringConvertor::WideToUtf8(szBuf));
	}
	SIZE szRoundCorner = pForm->GetRoundCorner();
	if((szRoundCorner.cx != 0) || (szRoundCorner.cy != 0))
	{
		_stprintf_s(szBuf, _T("%d,%d"), szRoundCorner.cx, szRoundCorner.cy);
		pFormElm->SetAttribute("roundcorner", StringConvertor::WideToUtf8(szBuf));
	}
	bool bShow = pForm->IsShowUpdateRect();
	if(bShow)
	{
		_tccpy(szBuf, _T("true"));
		pFormElm->SetAttribute("showdirty", StringConvertor::WideToUtf8(szBuf));
	}

	TiXmlNode* pNode = xmlDoc.InsertEndChild(*pFormElm);
	if(m_Manager.GetCustomFontCount() > 0)
	{
		TFontInfo* default_info = m_Manager.GetDefaultFontInfo();
		HFONT hDefaultFont = default_info->hFont;
		LOGFONT lfDefault = {0};
		GetObject(hDefaultFont, sizeof(LOGFONT), &lfDefault);

		std::vector<LOGFONT> cachedFonts;

		for (DWORD index = 0; index < m_Manager.GetCustomFontCount(); ++index)
		{
			HFONT hFont = m_Manager.GetFont(index);
			LOGFONT lf = {0};
			GetObject(hFont, sizeof(LOGFONT), &lf);

			bool bSaved = false;
			for (std::vector<LOGFONT>::const_iterator citer = cachedFonts.begin(); citer != cachedFonts.end(); ++citer)
			{
				if((citer->lfWeight == lf.lfWeight) && (citer->lfItalic == lf.lfItalic) && (citer->lfHeight == lf.lfHeight)
					&& (_tcsicmp(lf.lfFaceName, citer->lfFaceName) == 0))
				{
					bSaved = true;
					break;
				}
			}
			if(bSaved)
				continue;

			cachedFonts.push_back(lf);

			TiXmlElement* pFontElem = new TiXmlElement("Font");
			pFontElem->SetAttribute("name", StringConvertor::WideToUtf8(lf.lfFaceName));

			_stprintf_s(szBuf, _T("%d"), -lf.lfHeight);
			pFontElem->SetAttribute("size", StringConvertor::WideToUtf8(szBuf));

			pFontElem->SetAttribute("bold", (lf.lfWeight >= FW_BOLD)?"true":"false");
			pFontElem->SetAttribute("italic", lf.lfItalic?"true":"false");

			if((lfDefault.lfWeight == lf.lfWeight) && (lfDefault.lfItalic == lf.lfItalic) && (lfDefault.lfHeight == lf.lfHeight)
				&& (_tcsicmp(lf.lfFaceName, lfDefault.lfFaceName) == 0))
				pFontElem->SetAttribute("default", "true");

			pNode->ToElement()->InsertEndChild(*pFontElem);

			delete pFontElem;
			pFontElem = NULL;
		}
	}

	const CStdStringPtrMap& defaultAttrHash = m_Manager.GetDefaultAttribultes();
	if(defaultAttrHash.GetSize() > 0)
	{
		for (int index = 0; index < defaultAttrHash.GetSize(); ++index)
		{
			LPCTSTR lpstrKey = defaultAttrHash.GetAt(index);
			LPCTSTR lpstrAttribute = m_Manager.GetDefaultAttributeList(lpstrKey);

			TiXmlElement* pAttributeElem = new TiXmlElement("Default");
			pAttributeElem->SetAttribute("name", StringConvertor::WideToUtf8(lpstrKey));

			CString strAttrib(lpstrAttribute);
			pAttributeElem->SetAttribute("value", StringConvertor::WideToUtf8(strAttrib));

			pNode->ToElement()->InsertEndChild(*pAttributeElem);

			delete pAttributeElem;
			pAttributeElem = NULL;
		}
	}

	SaveProperties(pForm->GetItemAt(0), pNode->ToElement());

	delete pFormElm;
	xmlDoc.SaveFile();
}

void CLayoutManager::SetDefaultUIName(CControlUI* pControl)
{
	m_Manager.ReapObjects(pControl);

	BOOL bNeedName = FALSE;
	CString strName = pControl->GetName();
	if(strName.IsEmpty())
		bNeedName = TRUE;
	else
	{
		if(m_Manager.FindControl(strName))
			bNeedName = TRUE;
		else
			m_Manager.InitControls(pControl);
	}
	if(!bNeedName)
		return;

	int nCount = 0;
	CString strUIName;
	do 
	{
		nCount++;
		strUIName.Format(_T("%s%d"), pControl->GetClass(), nCount);
	}while(m_Manager.FindControl(strUIName));
	pControl->SetName(strUIName);
	m_Manager.InitControls(pControl);
}

CString CLayoutManager::ConvertImageFileName(LPCTSTR pstrImageAttrib)
{
	CString strImageAttrib(pstrImageAttrib);
	CStdString sItem;
	CStdString sValue;
	LPTSTR pstr = (LPTSTR)pstrImageAttrib;
	while( *pstr != _T('\0') ) {
		sItem.Empty();
		sValue.Empty();
		while( *pstr != _T('\0') && *pstr != _T('=') ) {
			LPTSTR pstrTemp = ::CharNext(pstr);
			while( pstr < pstrTemp) {
				sItem += *pstr++;
			}
		}
		if( *pstr++ != _T('=') ) break;
		if( *pstr++ != _T('\'') ) break;
		while( *pstr != _T('\0') && *pstr != _T('\'') ) {
			LPTSTR pstrTemp = ::CharNext(pstr);
			while( pstr < pstrTemp) {
				sValue += *pstr++;
			}
		}
		if( *pstr++ != _T('\'') ) break;
		if( !sValue.IsEmpty() ) {
			if( sItem == _T("file"))
				break;
		}
		if( *pstr++ != _T(' ') ) break;
	}

	if(sValue.IsEmpty())
		sValue = sItem;
	CString strFileName = sValue;
	int nPos = strFileName.ReverseFind(_T('\\'));
	if(nPos != -1)
	{
		strFileName = strFileName.Right(strFileName.GetLength() - nPos - 1);
		strImageAttrib.Replace(sValue, strFileName);
	}

	return strImageAttrib;
}
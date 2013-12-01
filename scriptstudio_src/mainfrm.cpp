// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "AboutDlg.h"
#include "ChildFrm.h"

#include "MainFrm.h"


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
   if( CMDIFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg) ) return TRUE;
   HWND hWnd = MDIGetActive();
   if( hWnd != NULL ) return (BOOL)::SendMessage(hWnd, WM_FORWARDMSG, 0, (LPARAM) pMsg);
   return FALSE;
}

BOOL CMainFrame::OnIdle()
{
   HWND hWnd = MDIGetActive();
   m_ctrlTbBtns.EnableWindow(hWnd != NULL);

   UIUpdateToolBar();
   return FALSE;
}

HWND CMainFrame::_CreateBlockToolBar()
{
   // Create a toolbar based on the MDI menu.
   // A toolbar button is created for each sub-menu.

   CMenu menu;
   menu.LoadMenu(IDR_MDICHILD);
   CMenuHandle submenu = menu.GetSubMenu(4);

   CToolBarCtrl tb;
   tb.Create(m_hWnd, NULL, NULL, ATL_SIMPLE_TOOLBAR_PANE_STYLE | TBSTYLE_LIST, 0, ATL_IDW_TOOLBAR + 1);
   tb.SetExtendedStyle(TBSTYLE_EX_MIXEDBUTTONS);
   tb.SetButtonStructSize();

   int iIndex = 0;
   while( true ) {
      TCHAR szName[100] = { 0 };
      MENUITEMINFO mii = { 0 };
      mii.cbSize = sizeof(MENUITEMINFO);
      mii.fMask = MIIM_STRING;
      mii.dwTypeData = szName;
      mii.cch = (sizeof(szName) / sizeof(TCHAR)) - 2;
      if( !submenu.GetMenuItemInfo(iIndex, TRUE, &mii) ) break;
      int iString = tb.AddStrings(mii.dwTypeData);
      TBBUTTON tbb = { 0 };      
      tbb.iString = iString;
      tbb.iBitmap = -1;
      tbb.idCommand = iIndex;
      tbb.fsStyle = BTNS_DROPDOWN | BTNS_SHOWTEXT | BTNS_WHOLEDROPDOWN | TBSTYLE_BUTTON;
      tbb.fsState = TBSTATE_ENABLED;
      tb.AddButtons(1, &tbb);
      iIndex++;
   }

   tb.SetImageList(NULL);
   tb.SetBitmapSize(0, 0);
   tb.SetButtonSize(20, 20);
   return tb;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // Create command bar window
   HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
   // Attach menu
   m_CmdBar.AttachMenu(GetMenu());
   // Load command bar images
   m_CmdBar.LoadImages(IDR_MAINFRAME);
   m_CmdBar.LoadImages(IDR_BLOCKS);
   // Remove old menu
   SetMenu(NULL);

   HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

   HWND hWndBlockBar = _CreateBlockToolBar();

   m_ctrlTbBtns = hWndBlockBar;
   m_ToolCmdBar.Install(hWndBlockBar);

   CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
   AddSimpleReBarBand(hWndCmdBar);
   AddSimpleReBarBand(hWndToolBar, NULL, TRUE);
   AddSimpleReBarBand(hWndBlockBar, NULL, TRUE);

   CreateSimpleStatusBar();

   CreateMDIClient();
   m_CmdBar.SetMDIClient(m_hWndMDIClient);

   UIAddToolBar(hWndToolBar);
   UISetCheck(ID_VIEW_TOOLBAR, 1);
   UISetCheck(ID_VIEW_STATUS_BAR, 1);

   // Register object for message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != NULL);
   pLoop->AddMessageFilter(this);
   pLoop->AddIdleHandler(this);

   SendMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_NEW, 0));

   return 0;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   PostMessage(WM_CLOSE);
   return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   m_ToolCmdBar.Uninstall();
   bHandled = FALSE;
   return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
   CChildFrame* pChild = new CChildFrame;
   pChild->CreateEx(m_hWndClient);
   bHandled = FALSE;  // Send it to child frame...
   return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   static BOOL bVisible = TRUE;   // Initially visible
   bVisible = !bVisible;
   CReBarCtrl rebar = m_hWndToolBar;
   int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);   // toolbar is 2nd added band
   rebar.ShowBand(nBandIndex, bVisible);
   UISetCheck(ID_VIEW_TOOLBAR, bVisible);
   UpdateLayout();
   return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
   ::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
   UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
   UpdateLayout();
   return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CAboutDlg dlg;
   dlg.DoModal();
   return 0;
}

LRESULT CMainFrame::OnWindowCascade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   MDICascade();
   return 0;
}

LRESULT CMainFrame::OnWindowTile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   MDITile();
   return 0;
}

LRESULT CMainFrame::OnWindowArrangeIcons(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   MDIIconArrange();
   return 0;
}

LRESULT CMainFrame::OnToolBarDropDown(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
   LPNMTOOLBAR lptb = (LPNMTOOLBAR) pnmh;
   if( idCtrl != ATL_IDW_TOOLBAR + 1 ) {
      bHandled = FALSE;
      return 0;
   }

   CToolBarCtrl tb = lptb->hdr.hwndFrom;
   RECT rcItem;
   tb.GetItemRect(tb.CommandToIndex(lptb->iItem), &rcItem);
   POINT pt = { rcItem.left, rcItem.bottom };
   tb.ClientToScreen(&pt);
   CMenu menu;
   menu.LoadMenu(IDR_MDICHILD);
   CMenuHandle submenu = menu.GetSubMenu(4);
   CMenuHandle popup = submenu.GetSubMenu(lptb->iItem);
   m_CmdBar.TrackPopupMenu(popup, TPM_LEFTALIGN | TPM_TOPALIGN, pt.x, pt.y);

   return TBDDRET_DEFAULT;
}


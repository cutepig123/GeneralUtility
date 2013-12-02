// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__9734D076_72F3_4D6B_9E67_4AF87A9311B0__INCLUDED_)
#define AFX_MAINFRM_H__9734D076_72F3_4D6B_9E67_4AF87A9311B0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ToolCmdBar.h"


class CMainFrame : 
   public CMDIFrameWindowImpl<CMainFrame>, 
   public CUpdateUI<CMainFrame>,
   public CMessageFilter, 
   public CIdleHandler
{
public:
   DECLARE_FRAME_WND_CLASS(_T("ScriptStudio_Main"), IDR_MAINFRAME)

   CMDICommandBarCtrl m_CmdBar;
   CToolCmdBarHook m_ToolCmdBar;
   CToolBarCtrl m_ctrlTbBtns;

   virtual BOOL OnIdle();
   virtual BOOL PreTranslateMessage(MSG* pMsg);

   HWND _CreateBlockToolBar();

   BEGIN_UPDATE_UI_MAP(CMainFrame)
      UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
      UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
   END_UPDATE_UI_MAP()

   BEGIN_MSG_MAP(CMainFrame)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
      COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
      COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileNew)
      COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
      COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
      COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
      COMMAND_ID_HANDLER(ID_WINDOW_CASCADE, OnWindowCascade)
      COMMAND_ID_HANDLER(ID_WINDOW_TILE_HORZ, OnWindowTile)
      COMMAND_ID_HANDLER(ID_WINDOW_ARRANGE, OnWindowArrangeIcons)
      NOTIFY_CODE_HANDLER(TBN_DROPDOWN, OnToolBarDropDown)
      CHAIN_MDI_CHILD_COMMANDS()
      CHAIN_MSG_MAP( CUpdateUI<CMainFrame> )
      CHAIN_MSG_MAP( CMDIFrameWindowImpl<CMainFrame> )
   END_MSG_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnToolBarDropDown(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
   LRESULT OnWindowCascade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnWindowTile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnWindowArrangeIcons(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__9734D076_72F3_4D6B_9E67_4AF87A9311B0__INCLUDED_)

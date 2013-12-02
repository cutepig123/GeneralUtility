// ChildFrm.h : interface of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__61A17664_4301_46F3_8F1F_871B8403BA91__INCLUDED_)
#define AFX_CHILDFRM_H__61A17664_4301_46F3_8F1F_871B8403BA91__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "atlsplit.h"

#include "ScriptDoc.h"

#include "ExplorerView.h"
#include "ScriptView.h"


class CChildFrame : 
   public CMDIChildWindowImpl<CChildFrame>,
   public CIdleHandler
{
public:
   DECLARE_FRAME_WND_CLASS(_T("ScriptStudio_ChildFrame"), IDR_MDICHILD)

   typedef CMDIChildWindowImpl<CChildFrame> parentClass;

   CChildFrame();

   CSplitterWindow m_ctrlSplitter;     // Splitter control
   CExplorerView m_viewExplorer;       // The Explorer view
   CScriptView m_viewScript;           // The script view
   CImageListCtrl m_ImagesBlocks;      // The images
   CScriptDoc m_doc;                   // The document
   //
   CScriptBlock* m_pRoot;              // Root block for this level
   WORD m_wNewBlock;                   // Command ID of new item
   bool m_bCapturePlace;               // Capturing mouse for new item creation?

   BOOL OnIdle();
   void OnFinalMessage(HWND hWnd);

   BOOL _PopulateViews(CScriptBlock* pRoot);
   BOOL _PopulateTree();
   BOOL _PopulateScript();
   void _LayoutItem(int iIndex, CScriptBlock* pBlock);
   void _ConnectItems();
   BOOL _ShowRenameDialog(int iItem, CScriptBlock* pBlock);
   BOOL _ShowPropertyPages(int iItem, CScriptBlock* pBlock);

   BEGIN_MSG_MAP(CChildFrame)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
      MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
      MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
      COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
      COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
      COMMAND_ID_HANDLER(ID_FILE_SAVE, OnFileSave)
      COMMAND_ID_HANDLER(ID_FILE_SAVE_AS, OnFileSaveAs)
      COMMAND_ID_HANDLER(ID_EDIT_DELETE, OnBlockDelete)
      COMMAND_ID_HANDLER(ID_NAVIGATE_UP, OnNavigateUp)
      COMMAND_RANGE_HANDLER(ID_BLOCK_FIRST, ID_BLOCK_LAST, OnBlockInsert)
      NOTIFY_HANDLER(IDC_TREE, TVN_SELCHANGED, OnTreeSelection)
      NOTIFY_HANDLER(IDC_SCRIPT, NM_DBLCLK, OnScriptDblClick)
      NOTIFY_HANDLER(IDC_SCRIPT, SCN_KEYDOWN, OnScriptKeyDown)      
      NOTIFY_HANDLER(IDC_SCRIPT, SCN_ITEMMOVED, OnScriptItemMoved)
      NOTIFY_HANDLER(IDC_SCRIPT, SCN_BEGINLABELEDIT, OnScriptLabelEdit)
      NOTIFY_HANDLER(IDC_SCRIPT, SCN_ITEMACTIVATE, OnScriptItemActivate)
      NOTIFY_HANDLER(IDC_SCRIPT, SCN_VALIDATECONNECTION, OnScriptValidateConnection)
      CHAIN_MSG_MAP( parentClass )
   END_MSG_MAP()

   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnForwardMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnCaptureChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
   LRESULT OnFileNew(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnFileOpen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnFileSave(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnFileSaveAs(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnNavigateUp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnBlockInsert(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnBlockDelete(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
   LRESULT OnTreeSelection(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
   LRESULT OnScriptKeyDown(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
   LRESULT OnScriptDblClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
   LRESULT OnScriptItemMoved(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
   LRESULT OnScriptLabelEdit(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
   LRESULT OnScriptItemActivate(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
   LRESULT OnScriptValidateConnection(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__61A17664_4301_46F3_8F1F_871B8403BA91__INCLUDED_)

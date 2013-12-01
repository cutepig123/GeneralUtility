// ChildFrm.cpp : implementation of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "ChildFrm.h"

#include "RenameDlg.h"
#include "WindowAnim.h"


/////////////////////////////////////////////////////////////////////////////
// Constructor 

CChildFrame::CChildFrame() :
   m_bCapturePlace(false),
   m_wNewBlock(0U)
{
}

void CChildFrame::OnFinalMessage(HWND /*hWnd*/)
{
   delete this;
}

BOOL CChildFrame::OnIdle()
{
   return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Implementation

BOOL CChildFrame::_PopulateViews(CScriptBlock* pRoot)
{
   ATLASSERT(pRoot);
   m_pRoot = pRoot;
   if( m_pRoot == NULL ) return FALSE;
   if( !_PopulateTree() ) return FALSE;
   if( !_PopulateScript() ) return FALSE;
   return TRUE;
}

BOOL CChildFrame::_PopulateTree()
{
   m_viewExplorer.Populate(m_doc.GetRoot(), m_pRoot);
   return TRUE;
}

BOOL CChildFrame::_PopulateScript()
{
   ATLASSERT(m_pRoot);
   m_viewScript.DeleteAllItems();
   CScriptBlock* pBlock = m_pRoot->GetChild();
   while( pBlock != NULL ) {
      TCHAR szName[100] = { 0 };
      pBlock->GetName(szName, 99);
      int iItem = m_viewScript.InsertItem(szName, pBlock->GetPos(), pBlock->GetImage(), (LPARAM) pBlock);
      _LayoutItem(iItem, pBlock);
      pBlock = pBlock->GetNext();
   }
   _ConnectItems();
   return TRUE;
}

BOOL CChildFrame::_ShowPropertyPages(int /*iItem*/, CScriptBlock* /*pBlock*/)
{
   // TODO: Add code here...
   return TRUE;
}

BOOL CChildFrame::_ShowRenameDialog(int iItem, CScriptBlock* pBlock)
{
   TCHAR szName[100] = { 0 };
   m_viewScript.GetItemText(iItem, szName, 99);
   CRenameDlg dlg;
   dlg.Init(szName);
   if( dlg.DoModal() != IDOK ) return 0;
   pBlock->SetName(dlg.GetResult());
   m_viewScript.SetItemText(iItem, dlg.GetResult());
   m_doc.SetDirty();
   return TRUE;
}

void CChildFrame::_LayoutItem(int iItem, CScriptBlock* pBlock)
{
   // Respect style!
   DWORD dwStyle = pBlock->GetStyle();
   if( (dwStyle & BLOCK_EX_WIDEBLOCK) != 0 ) m_viewScript.SetItemWidth(iItem, CScriptView::ITEM_BIG_MINSIZE);
   if( (dwStyle & BLOCK_EX_SMALLBLOCK) != 0 ) m_viewScript.SetItemWidth(iItem, CScriptView::ITEM_SMALL_MINSIZE);
   if( (dwStyle & BLOCK_EX_RIGHTALIGNED) != 0 ) m_viewScript.SetItemState(iItem, SCIS_RIGHTALIGN, SCIS_RIGHTALIGN);
   // Remove existing links
   m_viewScript.DeleteAllLinks(iItem);
   // Create the links
   int i;
   for( i = 0; i < pBlock->m_aInput.GetSize(); i++ ) {
      m_viewScript.InsertItemLink(iItem, pBlock->m_aInput[i].szName, 0, pBlock->m_aInput[i].lID);
   }
   for( i = 0; i < pBlock->m_aOutput.GetSize(); i++ ) {
      m_viewScript.InsertItemLink(iItem, pBlock->m_aOutput[i].szName, 1);
   }
}

void CChildFrame::_ConnectItems()
{
   // Rewire all items on this level in the script view
   CScriptBlock* pBlock = m_pRoot->m_pChild;
   while( pBlock != NULL ) {
      int iSrcItem = m_viewScript.FindItem( (LPARAM) pBlock );
      ATLASSERT(iSrcItem>=0);
      for( int i = 0; i < pBlock->m_aOutput.GetSize(); i++ ) {
         if( pBlock->m_aOutput[i].lID == 0 ) continue;
         int iItem = 0;
         int iSubItem = 0;
         if( m_viewScript.SearchLink(pBlock->m_aOutput[i].lID, iItem, iSubItem) ) {
            int iSrcSubItem = m_viewScript.FindLink(iSrcItem, pBlock->m_aOutput[i].szName);
            m_viewScript.ConnectItem(iSrcItem, iSrcSubItem, iItem, iSubItem, 0);
         }
      }
      pBlock = pBlock->GetNext();
   }
}


/////////////////////////////////////////////////////////////////////////////
// Message handlers

LRESULT CChildFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   pLoop->AddIdleHandler(this);

   RECT rcClient;
   GetClientRect(&rcClient);
   m_hWndClient = m_ctrlSplitter.Create(m_hWnd, rcClient);
   m_ctrlSplitter.SetSplitterExtendedStyle(0);
   m_ctrlSplitter.SetSplitterPos(140);

   m_ImagesBlocks.Create(IDR_BLOCKS, 16, 0, RGB(192,192,192));

   m_viewExplorer.Create(m_ctrlSplitter, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE, IDC_TREE);
   m_viewScript.Create(m_ctrlSplitter, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE, IDC_SCRIPT);

   m_viewExplorer.SetImageList(m_ImagesBlocks, TVSIL_NORMAL);
   m_viewScript.SetExtendedScriptStyle(SC_EX_SHAREIMAGELIST);
   m_viewScript.SetImageList(m_ImagesBlocks);

   m_ctrlSplitter.SetSplitterPanes(m_viewExplorer, m_viewScript);

   m_doc.Init(m_hWnd);

   bHandled = FALSE;
   return 0;
}

LRESULT CChildFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   pLoop->RemoveIdleHandler(this);

   bHandled = FALSE;
   return 0;
}

LRESULT CChildFrame::OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
   LPMSG pMsg = (LPMSG) lParam;
   if( parentClass::PreTranslateMessage(pMsg) ) return TRUE;
   if( m_viewExplorer.PreTranslateMessage(pMsg) ) return TRUE;
   return FALSE;
}

LRESULT CChildFrame::OnCaptureChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   m_bCapturePlace = false;
   return 0;
}

LRESULT CChildFrame::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
   ATLASSERT(m_pRoot);
   // In the process of adding a new block?
   if( m_bCapturePlace ) 
   {
      POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
      ::MapWindowPoints(m_hWnd, m_viewScript, &pt, 1);
      // Stop tracking mouse
      ReleaseCapture();
      // Don't add item if it's not within the client rect
      RECT rcClient;
      m_viewScript.GetClientRect(&rcClient);
      if( !::PtInRect(&rcClient, pt) ) return 0;
      // Create block
      CScriptBlock* pBlock = CScriptBlock::CreateInstance(m_wNewBlock);
      if( pBlock == NULL ) {
         AtlMessageBox(m_hWnd, IDS_ERR_BLOCKTYPE, IDS_CAPTION_ERROR, MB_ICONEXCLAMATION);
         return 0;
      }
      pBlock->Init();
      pBlock->SetPos(pt);
      m_pRoot->AppendChild(pBlock);
      // Create GUI item
      TCHAR szName[100] = { 0 };
      pBlock->GetName(szName, 99);
      int iItem = m_viewScript.InsertItem(szName, pBlock->GetPos(), pBlock->GetImage(), (LPARAM) pBlock);
      _LayoutItem(iItem, pBlock);
      // Select item as current
      m_viewScript.SelectItem(iItem);
      m_viewScript.SetFocus();
      // Update tree
      m_viewExplorer.Populate(m_doc.GetRoot(), m_pRoot);
      m_viewExplorer.Select(pBlock);
      // Mark document as dirty
      m_doc.SetDirty();
      return 0;
   }
   bHandled = FALSE;
   return 0;
}

LRESULT CChildFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{   
   m_doc.New();
   _PopulateViews(m_doc.GetRoot());
   return 0;
}

LRESULT CChildFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   UpdateWindow();
   // Show Open File dialog
   CString sFilter(MAKEINTRESOURCE(IDS_FILEFILTER));
   for( int i = 0; i < sFilter.GetLength(); i++ ) if( sFilter[i] == '|' ) sFilter.SetAt(i, '\0');
   CModulePath sCurDir;
   CFileDialog dlg(TRUE, _T("xml"), _T(""), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, sFilter);
   dlg.m_ofn.lpstrInitialDir = sCurDir;
   if( dlg.DoModal() != IDOK ) {
      PostMessage(WM_CLOSE);
      return 0;
   }
   // Load file...
   CWaitCursor cursor;
   m_doc.Load(dlg.m_ofn.lpstrFile);
   _PopulateViews(m_doc.GetRoot());
   return 0;
}

LRESULT CChildFrame::OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if( m_doc.GetFilename().IsEmpty() ) return SendMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_SAVE_AS, 0));
   CWaitCursor cursor;
   if( !m_doc.Save(m_doc.GetFilename()) ) return 0;
   AtlMessageBox(m_hWnd, IDS_FILESAVED, IDS_CAPTION_MESSAGE, MB_ICONINFORMATION);
   return 0;
}

LRESULT CChildFrame::OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   // Save file
   CString sFilter(MAKEINTRESOURCE(IDS_FILEFILTER));
   for( int i = 0; i < sFilter.GetLength(); i++ ) if( sFilter[i] == '|' ) sFilter.SetAt(i, '\0');
   CModulePath sCurDir;
   CFileDialog dlg(FALSE, _T("xml"), _T(""), OFN_EXPLORER | OFN_HIDEREADONLY, sFilter);
   dlg.m_ofn.lpstrInitialDir = sCurDir;
   if( dlg.DoModal() != IDOK ) return 0;
   CWaitCursor cursor;
   if( !m_doc.Save(dlg.m_ofn.lpstrFile) ) return 0;
   AtlMessageBox(m_hWnd, IDS_FILESAVED, IDS_CAPTION_MESSAGE, MB_ICONINFORMATION);
   return 0;
}

LRESULT CChildFrame::OnNavigateUp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   CScriptBlock* pBlock = m_pRoot;
   if( m_pRoot->GetParent() == NULL ) return 0;
   // Build script on parent level
   m_pRoot = m_pRoot->GetParent();
   _PopulateScript();
   m_viewExplorer.Select(m_pRoot);
   // Create a little zoom animation   
   int iItem = m_viewScript.FindItem( (LPARAM) pBlock );
   m_viewScript.EnsureVisible(iItem);
   m_viewScript.SelectItem(iItem);
   UpdateWindow();
   CWindowAnim anim = m_viewScript;
   anim.ZoomOut(m_viewScript.GetItemRect(iItem));
   return 0;
}

LRESULT CChildFrame::OnBlockInsert(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   // User choose a block to insert in the menu/toolbar. Let's capture
   // mouse and see where user placese the block...
   SetCapture();
   ::SetCursor( ::LoadCursor(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_PLACEITEM)) );
   m_wNewBlock = wID;
   m_bCapturePlace = true;
   return 0;
}

LRESULT CChildFrame::OnBlockDelete(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   int iItem;
   while( (iItem = m_viewScript.GetNextItem(-1, SCIS_SELECTED)) >= 0 ) {
      CScriptBlock* pBlock = (CScriptBlock*) m_viewScript.GetItemData(iItem);
      m_viewExplorer.RemoveItem(pBlock);
      m_viewScript.DeleteItem(iItem);
      pBlock->Delete();
   }
   m_doc.SetDirty();
   return 0;
}

LRESULT CChildFrame::OnScriptLabelEdit(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   NMSCRIPTVIEW* pNMSV = (NMSCRIPTVIEW*) pnmh;
   CScriptBlock* pBlock = (CScriptBlock*) pNMSV->lParam;
   _ShowRenameDialog(pNMSV->iItem, pBlock);
   return 0;
}

LRESULT CChildFrame::OnScriptItemActivate(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   NMSCRIPTVIEW* pNMSV = (NMSCRIPTVIEW*) pnmh;
   CScriptBlock* pBlock = (CScriptBlock*) pNMSV->lParam;
   DWORD dwStyle = pBlock->GetStyle();
   // Is it a user-block? Let's zoom in on this one...
   if( (dwStyle & BLOCK_EX_CONTAINER) != 0 ) {
      CWindowAnim anim = m_viewScript;
      anim.ZoomIn(m_viewScript.GetItemRect(pNMSV->iItem));
      // Build script on new level
      m_pRoot = pBlock;
      _PopulateScript();
      m_viewExplorer.Select(m_pRoot);
      return 0;
   }
   // Standard block. Just do the ordinary thing...
   if( _ShowPropertyPages(pNMSV->iItem, pBlock) ) return 0;
   if( _ShowRenameDialog(pNMSV->iItem, pBlock) ) return 0;
   return 0;
}

LRESULT CChildFrame::OnScriptValidateConnection(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   NMSCRIPTVIEW* pNMSV = (NMSCRIPTVIEW*) pnmh;
   CScriptBlock* pBlock = (CScriptBlock*) pNMSV->lParam;
   TCHAR szName[100] = { 0 };
   m_viewScript.GetItemLinkName(pNMSV->iItem, pNMSV->iSubItem, szName, 99);
   SCHITTESTINFO hti = { 0 };
   hti.pt = pNMSV->ptAction;
   hti.iMaxDistance2 = 7000;
   if( m_viewScript.FindClosestItem(pNMSV->iItem, &hti) ) {
      if( m_viewScript.ConnectItem(pNMSV->iItem, pNMSV->iSubItem, hti.iItem, hti.iSubItem, pNMSV->uNewState) ) {
         pBlock->SetLinkTarget(szName, m_viewScript.GetItemLinkData(hti.iItem, hti.iSubItem));
      }
   }
   else {
      if( m_viewScript.DisconnectItem(pNMSV->iItem, pNMSV->iSubItem) ) {
         pBlock->SetLinkTarget(szName, 0);
      }
   }
   return 0;
}

LRESULT CChildFrame::OnTreeSelection(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   NMTREEVIEW* pNMTV = (NMTREEVIEW*) pnmh;
   if( pNMTV->action == TVC_UNKNOWN ) return 0;
   // Populate script with new level
   CScriptBlock* pBlock = (CScriptBlock*) pNMTV->itemNew.lParam;
   if( pBlock == NULL ) return 0;
   if( pBlock == m_doc.GetRoot() ) m_pRoot = pBlock; else m_pRoot = pBlock->GetParent();
   _PopulateScript();
   // Select the item in the script
   int iItem = m_viewScript.FindItem( (LPARAM) pBlock );
   m_viewScript.EnsureVisible(iItem);
   m_viewScript.SelectItem(iItem);
   return 0;
}

LRESULT CChildFrame::OnScriptKeyDown(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   NMSCVKEYDOWN* pNMSC = (NMSCVKEYDOWN*) pnmh;
   if( pNMSC->wVKey == VK_BACK ) PostMessage(WM_COMMAND, MAKEWPARAM(ID_NAVIGATE_UP, 0));
   if( pNMSC->wVKey == VK_DELETE ) PostMessage(WM_COMMAND, MAKEWPARAM(ID_EDIT_DELETE, 0));
   return 0;
}

LRESULT CChildFrame::OnScriptItemMoved(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   NMSCRIPTVIEW* pNMSV = (NMSCRIPTVIEW*) pnmh;
   CScriptBlock* pBlock = (CScriptBlock*) pNMSV->lParam;
   pBlock->SetPos(m_viewScript.GetItemPos(pNMSV->iItem));
   return 0;
}

LRESULT CChildFrame::OnScriptDblClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
   SCHITTESTINFO schti = { 0 };
   DWORD dwPos = ::GetMessagePos();
   schti.pt.x = GET_X_LPARAM(dwPos);
   schti.pt.y = GET_Y_LPARAM(dwPos);
   m_viewScript.ScreenToClient(&schti.pt);
   int iItem = m_viewScript.HitTest(&schti);
   if( iItem < 0 ) return 0;
   switch( schti.flags ) {
   case SCHT_ONITEMICON:
   case SCHT_ONITEMLINK:
      {
         if( m_viewScript.GetItemLinkType(schti.iItem, schti.iSubItem) != 1 ) return 0;
         int iItem = 0;
         int iSubItem = 0;
         if( m_viewScript.GetItemLinkTarget(schti.iItem, schti.iSubItem, iItem, iSubItem) ) {
            // Mouse jumps to destination point!
            m_viewScript.EnsureVisible(iItem);
            RECT rcItem = m_viewScript.GetItemRect(iItem);
            POINT pt = { rcItem.left, rcItem.top };
            m_viewScript.ClientToScreen(&pt);
            ::SetCursorPos(pt.x + 5, pt.y + 5);
            m_doc.SetDirty();
         }
         else {
            // Automatically create a link to closest neighbour
            SCHITTESTINFO target = { 0 };
            if( !m_viewScript.FindClosestItem(schti.iItem, &target) ) return 0;
            if( m_viewScript.ConnectItem(schti.iItem, schti.iSubItem, target.iItem, target.iSubItem, 0) ) {
               CScriptBlock* pBlock = (CScriptBlock*) m_viewScript.GetItemData(schti.iItem);
               TCHAR szName[100] = { 0 };
               m_viewScript.GetItemLinkName(schti.iItem, schti.iSubItem, szName, 99);
               pBlock->SetLinkTarget(szName, m_viewScript.GetItemLinkData(target.iItem, target.iSubItem));
               m_doc.SetDirty();
            }
         }
      }
      break;
   }
   return 0;
}


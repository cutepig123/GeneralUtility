
#include "stdafx.h"
#include "resource.h"

#include "ExplorerView.h"


BOOL CExplorerView::PreTranslateMessage(MSG* pMsg)
{
   pMsg;
   return FALSE;
}

LRESULT CExplorerView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
   LRESULT lResult = DefWindowProc(uMsg, wParam, lParam);
   ModifyStyle(0, TVS_SHOWSELALWAYS | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_DISABLEDRAGDROP);
   return lResult;
}

BOOL CExplorerView::Populate(CScriptBlock* pRoot, CScriptBlock* pLevel)
{
   SetRedraw(FALSE);
   DeleteAllItems();
   _PopulateTree(pRoot, TVI_ROOT);
   Expand(GetRootItem());
   Select(pLevel);
   SetRedraw(TRUE);
   return TRUE;
}

BOOL CExplorerView::Select(CScriptBlock* pBlock)
{
   HTREEITEM hItem = NULL;
   if( !_FindItem(GetRootItem(), pBlock, hItem) ) return FALSE;
   EnsureVisible(hItem);
   SelectItem(hItem);
   Expand(hItem);
   return TRUE;
}

BOOL CExplorerView::RemoveItem(CScriptBlock* pBlock)
{
   HTREEITEM hItem = NULL;
   if( !_FindItem(GetRootItem(), pBlock, hItem) ) return FALSE;
   DeleteItem(hItem);
   return TRUE;
}

BOOL CExplorerView::_FindItem(HTREEITEM hItem, CScriptBlock* pBlock, HTREEITEM& hMatch)
{
   while( hItem != NULL ) {
      LPARAM lParam = GetItemData(hItem);
      if( lParam == (LPARAM) pBlock ) {
         hMatch = hItem;
         return TRUE;
      }
      if( _FindItem(GetChildItem(hItem), pBlock, hMatch) ) return TRUE;
      hItem = GetNextSiblingItem(hItem);
   }
   return FALSE;
}

BOOL CExplorerView::_PopulateTree(CScriptBlock* pBlock, HTREEITEM hParent)
{
   while( pBlock != NULL ) {
      TCHAR szName[100] = { 0 };
      pBlock->GetName(szName, 99);
      int iImage = pBlock->GetImage();
      HTREEITEM hItem = InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM, 
         szName, 
         iImage, iImage, 
         0, 0, 
         (LPARAM) pBlock, 
         hParent, 
         TVI_LAST);
      _PopulateTree(pBlock->GetChild(), hItem);
      pBlock = pBlock->GetNext();
   }
   TVSORTCB tvscb = { 0 };
   tvscb.hParent = hParent;
   tvscb.lpfnCompare = _SortTreeCB;
   SortChildrenCB(&tvscb);
   return TRUE;
}

int CALLBACK CExplorerView::_SortTreeCB(LPARAM lParam1, LPARAM lParam2, LPARAM /*lParamSort*/)
{
   CScriptBlock* pBlock1 = (CScriptBlock*) lParam1;
   CScriptBlock* pBlock2 = (CScriptBlock*) lParam2;

   // START and INPUT block are sorted first; STOP and OUTPUT blocks last.
   // All other blocks are sorted alphabethically.
   WORD wType1 = pBlock1->GetType();
   WORD wType2 = pBlock2->GetType();
   int iType1 = 1;
   int iType2 = 1;
   if( wType1 == ID_BLOCK_START || wType1 == ID_BLOCK_INPUT ) iType1 = 0;
   if( wType2 == ID_BLOCK_START || wType2 == ID_BLOCK_INPUT ) iType2 = 0;
   if( wType1 == ID_BLOCK_STOP || wType1 == ID_BLOCK_OUTPUT ) iType1 = 2;
   if( wType2 == ID_BLOCK_STOP || wType2 == ID_BLOCK_OUTPUT ) iType2 = 2;
   
   if( iType1 - iType2 < 0 ) return -1;
   if( iType1 - iType2 > 0 ) return 1;
   
   TCHAR szName1[100] = { 0 };
   TCHAR szName2[100] = { 0 };
   pBlock1->GetName(szName1, 99);
   pBlock2->GetName(szName2, 99);
   return _tcsicmp(szName1, szName2);
}

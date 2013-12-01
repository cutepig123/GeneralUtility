
#include "stdafx.h"
#include "resource.h"

#include "ScriptView.h"
#include "RoutePlanner.h"

#define GRID_ALIGN(x) (x) / ITEM_GRID_SIZE * ITEM_GRID_SIZE


/////////////////////////////////////////////////////////////////
// Operations

void CScriptView::SetNotifyParent(HWND hWnd)
{
   ATLASSERT(::IsWindow(hWnd));
   m_wndNotify = hWnd;
}

DWORD CScriptView::SetExtendedScriptStyle(DWORD dwStyle)
{
   ATLASSERT(::IsWindow(m_hWnd));
   DWORD dwOldStyle = m_dwExtStyle;
   m_dwExtStyle = dwStyle;
   Invalidate();
   return dwOldStyle;
}

CImageList CScriptView::SetImageList(HIMAGELIST hImageList)
{
   CImageList hOldImages = m_Images;
   m_Images = hImageList;
   return hOldImages;
}

BOOL CScriptView::SetTitleText(LPCTSTR /*pstrTitle*/)
{
   ATLASSERT(::IsWindow(m_hWnd));
   return Invalidate();
}

BOOL CScriptView::SetTitleColors(COLORREF /*clrText*/, COLORREF /*clrBack*/)
{
   ATLASSERT(::IsWindow(m_hWnd));
   return Invalidate();
}

BOOL CScriptView::EnsureVisible(int iIndex)
{
   ATLASSERT(::IsWindow(m_hWnd));
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return FALSE;
   // Test if it's already visible
   RECT rcView = { 0 };
   GetClientRect(&rcView);
   RECT rcItem = GetItemRect(iIndex);
   RECT rcTemp = { 0 };
   if( ::IntersectRect(&rcTemp, &rcItem, &rcView) ) return TRUE;
   // Center view to item...
   const RECT& rcAbs = m_aItems[iIndex].rcItem;
   POINT ptItem = { rcAbs.left + 20, rcAbs.top + 20 };
   int cx = ptItem.x - (rcView.right - rcView.left) / 2;
   int cy = ptItem.y - (rcView.right - rcView.left) / 2;
   SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, cx), 0L);
   SendMessage(WM_VSCROLL, MAKEWPARAM(SB_THUMBPOSITION, cy), 0L);
   return Invalidate();
}

void CScriptView::DeleteAllItems()
{
   ATLASSERT(::IsWindow(m_hWnd));
   
   ReleaseCapture();

   if( m_aItems.GetSize() > 0 ) {
      bool bNotify = _SendNotifyMessage(SCN_DELETEALLITEMS) == 0;
      while( m_aItems.GetSize() > 0 ) _DeleteItem(0, false, bNotify);
   }

   m_wItemID = 0;
   m_wLinkID = 0;
   m_uMaxOrder = 0;
   m_wDragLinkID = 0;
   m_iDragSubItem = -1;
   m_iHightlightItem = -1;
   m_iHightlightSubItem = -1;
   m_CaptureState = CAPTURE_NONE;
   m_ptOffset.x = m_ptOffset.y = 0;
   m_bRecalcLayout = false;

   SetScrollPos(SB_HORZ, 0, TRUE);
   SetScrollPos(SB_VERT, 0, TRUE);

   UpdateLayout();
}

BOOL CScriptView::DeleteItem(int iIndex)
{
   ATLASSERT(::IsWindow(m_hWnd));
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return FALSE;
   BOOL bRes = _DeleteItem(iIndex, true, true);
   UpdateLayout();
   return bRes;
}

int CScriptView::GetItemCount() const
{
   ATLASSERT(::IsWindow(m_hWnd));
   return m_aItems.GetSize();
}

int CScriptView::InsertItem(LPCTSTR pstrName, POINT ptPos, int iImage, LPARAM lParam /*= 0*/)
{
   ATLASSERT(::IsWindow(m_hWnd));
   ptPos.x -= m_ptOffset.x;
   ptPos.y -= m_ptOffset.y;
   // Insert copy
   SCRIPTITEM Item = { 0 };
   ::lstrcpyn(Item.szName, pstrName, (sizeof(Item.szName) / sizeof(TCHAR)) - 1);
   Item.wId = ++m_wItemID;
   Item.clrText = CLR_DEFAULT;
   Item.clrBack = CLR_DEFAULT;
   Item.rcItem.left = GRID_ALIGN(ptPos.x);
   Item.rcItem.top = GRID_ALIGN(ptPos.y);
   Item.cxWidth = ITEM_NORMAL_MINSIZE;
   Item.iImage = iImage;
   Item.lParam = lParam;
   Item.uOrder = ++m_uMaxOrder;
   if( !m_aItems.Add(Item) ) return -1;
   int iIndex = m_aItems.GetSize() - 1;
   // Send notification
   NMSCRIPTVIEW nmsv = { 0 };
   nmsv.iItem = iIndex;
   nmsv.lParam = lParam;
   _SendNotifyMessage(SCN_INSERTITEM, &nmsv);
   // Done
   UpdateLayout();
   return iIndex;
}

LPARAM CScriptView::GetItemData(int iIndex) const
{
   ATLASSERT(::IsWindow(m_hWnd));
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return 0;
   return m_aItems[iIndex].lParam;
}

POINT CScriptView::GetItemPos(int iIndex) const
{
   ATLASSERT(::IsWindow(m_hWnd));
   POINT pt = { 0, 0 };
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return pt;
   // NOTE: Returns the "raw" position - NOT adjusted
   //       for view's scroll-position!
   pt.x = m_aItems[iIndex].rcItem.left;
   pt.y = m_aItems[iIndex].rcItem.top;
   return pt;
}

RECT CScriptView::GetItemRect(int iIndex) const
{
   ATLASSERT(::IsWindow(m_hWnd));
   RECT rcItem = { 0 };
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return rcItem;   
   rcItem = m_aItems[iIndex].rcItem;
   // FIX: May not have been calculated yet!
   if( rcItem.right == 0 && rcItem.bottom == 0 ) {
      rcItem.right = rcItem.left + ITEM_NORMAL_MINSIZE;
      rcItem.bottom = rcItem.top + 40;
   }
   // Translate to client rectangle
   ::OffsetRect(&rcItem, m_ptOffset.x, m_ptOffset.y);
   return rcItem;
}

UINT CScriptView::GetItemState(int iIndex) const
{
   ATLASSERT(::IsWindow(m_hWnd));
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return 0;
   return m_aItems[iIndex].uState;
}

BOOL CScriptView::GetItemText(int iIndex, LPTSTR pstrName, int cchMax) const
{
   ATLASSERT(::IsWindow(m_hWnd));
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return FALSE;
   return ::lstrcpyn(pstrName, m_aItems[iIndex].szName, cchMax) > 0;
}

BOOL CScriptView::SetItemPos(int iIndex, POINT ptPos)
{
   ATLASSERT(::IsWindow(m_hWnd));
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return FALSE;
   m_aItems[iIndex].rcItem.left = GRID_ALIGN(ptPos.x);
   m_aItems[iIndex].rcItem.bottom = GRID_ALIGN(ptPos.y);
   return UpdateLayout();
}

BOOL CScriptView::SetItemText(int iIndex, LPCTSTR pstrName)
{
   ATLASSERT(::IsWindow(m_hWnd));
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return FALSE;
   ::lstrcpyn(m_aItems[iIndex].szName, pstrName, (sizeof(m_aItems[iIndex].szName) / sizeof(TCHAR)) - 1);
   return Invalidate();
}

BOOL CScriptView::SetItemWidth(int iIndex, int cxWidth)
{
   ATLASSERT(::IsWindow(m_hWnd));
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return FALSE;
   m_aItems[iIndex].cxWidth = cxWidth;
   return UpdateLayout();
}

BOOL CScriptView::SetItemColor(int iIndex, COLORREF clrText)
{
   ATLASSERT(::IsWindow(m_hWnd));
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return FALSE;
   m_aItems[iIndex].clrText = clrText;
   return Invalidate();
}

BOOL CScriptView::SetItemData(int iIndex, LPARAM lParam)
{
   ATLASSERT(::IsWindow(m_hWnd));
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return FALSE;
   m_aItems[iIndex].lParam = lParam;
   return TRUE;
}

BOOL CScriptView::SetItemState(int iIndex, UINT state, UINT mask)
{
   ATLASSERT(::IsWindow(m_hWnd));
   bool bModified = false;
   int iStart = iIndex == -1 ? 0 : iIndex;
   int iEnd = iIndex == -1 ? m_aItems.GetSize() - 1 : iIndex;
   for( int i = iStart; i <= iEnd; i++ ) {
      SCRIPTITEM& Item = m_aItems[i];
      if( (Item.uState & mask) != state ) {
         UINT uNewState = (Item.uState & ~mask) | state;
         NMSCRIPTVIEW nmsv = { 0 };
         nmsv.iItem = iIndex;
         nmsv.lParam = Item.lParam;
         nmsv.uOldState = Item.uState;
         nmsv.uNewState = uNewState;
         if( _SendNotifyMessage(SCN_ITEMCHANGING, &nmsv) != 0 ) continue;
         Item.uState = uNewState;
         Item.uOrder = ++m_uMaxOrder;
         _SendNotifyMessage(SCN_ITEMCHANGED, &nmsv);
         bModified = true;
      }
   }
   if( bModified ) Invalidate();
   return TRUE;
}

BOOL CScriptView::SelectItem(int iIndex)
{
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return FALSE;
   SetItemState(-1, 0, SCIS_SELECTED);
   return SetItemState(iIndex, SCIS_SELECTED, SCIS_SELECTED);
}

int CScriptView::FindItem(LPARAM lParam, int iStart /*= 0*/) const
{
   for( int i = iStart; i < m_aItems.GetSize(); i++ ) {
      if( m_aItems[i].lParam == lParam ) return i;
   }
   return -1;
}

BOOL CScriptView::FindClosestItem(int iIndex, SCHITTESTINFO* pHitInfo) const
{
   ATLASSERT(::IsWindow(m_hWnd));
   ATLASSERT(pHitInfo);
   pHitInfo->iItem = -1;
   pHitInfo->iSubItem = -1;
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return FALSE;
   // Start the search...
   bool bExcludeSelf = false;
   const SCRIPTITEM& From = m_aItems[iIndex];
   // We'll center the point if none was specified
   // In this case we always exclude the 'iIndex' node
   // from the successful result!!!
   POINT pt = pHitInfo->pt;
   if( pt.x == 0 && pt.y == 0 ) {
      pt.x = From.rcItem.right;
      pt.y = From.rcItem.top;
      bExcludeSelf = true;
   }
   // First we'll determine what item the mouse points to.
   // We'll start out with determining if there's an item directly
   // below the mouse.
   SCHITTESTINFO schti = { 0 };
   schti.pt = pt;
   int iBestIndex = HitTest(&schti);
   // We'll find the closest item then...
   if( iBestIndex < 0 ) {
      schti.iItem = -1;
      schti.iSubItem = -1;
      int iBestDistance = INT_MAX;
      for( int i = 0; i < m_aItems.GetSize(); i++ ) {
         const SCRIPTITEM& To = m_aItems[i];
         if( i == iIndex ) continue;
         if( bExcludeSelf && To.rcItem.left < From.rcItem.left ) continue;
         POINT ptItem = 
         {
            To.rcItem.left + ((To.rcItem.right - To.rcItem.left) / 2),
            To.rcItem.top + ((To.rcItem.bottom - To.rcItem.top) / 2)
         };
         int iDiffX = (pt.x - ptItem.x);
         int iDiffY = (pt.y - ptItem.y);
         int iDistance = (iDiffX * iDiffX) + (iDiffY * iDiffY);
         if( iDistance < iBestDistance ) {
            iBestDistance = iDistance;
            iBestIndex = i;
         }
      }
      // If we have a max distance and the best bet didn't fall
      // within this range, we'll report failure
      if( pHitInfo->iMaxDistance2 > 0 && iBestDistance > pHitInfo->iMaxDistance2 ) return FALSE;
   }
   if( iBestIndex < 0 ) return FALSE;
   // Did the mouse hit an output link?
   // Yes? We'll just set it to the "main" input link.
   int iBestSubIndex = schti.iSubItem;
   if( iBestSubIndex < 0 ) iBestSubIndex = 0;
   if( GetItemLinkType(iBestIndex, iBestSubIndex) != 0 ) iBestSubIndex = GetItemLinkCount(iBestIndex, 0) == 1 ? 0 : 1;
   // It shouldn't be very easy to loop back to self
   if( iBestIndex == iIndex ) {
      if( bExcludeSelf ) return FALSE;
      if( (long) schti.flags != (GetItemLinkCount(iBestIndex, 0) == 1 ? SCHT_ONITEMICON : SCHT_ONITEMLINK) ) return FALSE;
   }
   // Validate the link...
   if( GetItemLinkType(iBestIndex, iBestSubIndex) != 0 ) return FALSE;
   // Top-input is not allowed if multiple "input" entries!
   if( iBestSubIndex == 0 && GetItemLinkCount(iBestIndex, 0) > 1 ) return FALSE;
   // Finally we can populate the result
   pHitInfo->iItem = iBestIndex;
   pHitInfo->iSubItem = iBestSubIndex;
   return TRUE;
}

int CScriptView::InsertItemLink(int iIndex, LPCTSTR pstrText, int iType /*= 0*/, LPARAM lParam /*= 0*/)
{
   ATLASSERT(::IsWindow(m_hWnd));
   // Must point to a valid item
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return -1;
   SCRIPTITEM& Item = m_aItems[iIndex];
   // Prepare link structure
   SCRIPTLINK* pLink = new SCRIPTLINK;
   if( pLink == NULL ) return -1;
   ::ZeroMemory(pLink, sizeof(SCRIPTLINK));
   ::lstrcpyn(pLink->szName, pstrText, (sizeof(pLink->szName) / sizeof(TCHAR)) - 1);   
   pLink->wID = ++m_wLinkID;
   pLink->iType = iType;
   pLink->lParam = lParam;
   pLink->clrText = CLR_DEFAULT;
   // Append link to end of list
   SCRIPTLINK* pLast = Item.pLinks;
   int nCount = 0;
   if( pLast == NULL ) {
      Item.pLinks = pLink;
   }
   else {
      while( pLast->pNext != NULL ) {
         pLast = pLast->pNext;
         nCount++;
      }
      pLast->pNext = pLink;
   }
   // We're done...
   UpdateLayout();
   return nCount;
}

int CScriptView::FindLink(int iIndex, LPCTSTR pstrName) const
{
   ATLASSERT(!::IsBadStringPtr(pstrName,-1));
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return -1;
   int iSubItem = 0;
   const SCRIPTLINK* pLink = m_aItems[iIndex].pLinks;
   while( pLink != NULL ) {
      if( ::lstrcmp(pstrName, pLink->szName) == 0 ) return iSubItem;
      pLink = pLink->pNext;
      iSubItem++;
   }
   return -1;
}

BOOL CScriptView::SearchLink(LPARAM lParam, int& iIndex, int& iSubItem) const
{
   for( iIndex = 0; iIndex < m_aItems.GetSize(); iIndex++ ) {
      iSubItem = 0;
      const SCRIPTLINK* pLink = m_aItems[iIndex].pLinks;
      while( pLink != NULL ) {
         if( lParam == pLink->lParam ) return TRUE;
         pLink = pLink->pNext;
         iSubItem++;
      }
   }
   return FALSE;
}

BOOL CScriptView::DeleteAllLinks(int iIndex)
{
   ATLASSERT(::IsWindow(m_hWnd));
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return FALSE;
   SCRIPTLINK* pLink = m_aItems[iIndex].pLinks;
   if( pLink == NULL ) return FALSE;
   while( pLink != NULL ) {
      SCRIPTLINK* pNext = pLink->pNext;
      delete pLink;
      pLink = pNext;
   }
   return UpdateLayout();
}

int CScriptView::GetNextItem(int iItem, UINT uState /*= SCIS_ALL*/) const
{
   for( int i = iItem + 1; i < m_aItems.GetSize(); i++ ) {
      if( uState == SCIS_ALL || (m_aItems[i].uState & uState) != 0 ) return i;
   }
   return -1;
}

POINT CScriptView::GetScrollPos() const
{
   return m_ptOffset;
}

int CScriptView::GetSelectedCount() const
{
   int nCount = 0;
   for( int i = 0; i < m_aItems.GetSize(); i++ ) {
      if( (m_aItems[i].uState & SCIS_SELECTED) != 0 ) nCount++;
   }
   return nCount;
}

LPARAM CScriptView::GetItemLinkData(int iIndex, int iSubItem) const
{
   ATLASSERT(::IsWindow(m_hWnd));
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return 0;
   if( iSubItem < 0 ) return 0;
   const SCRIPTLINK* pLink = _GetLink(iIndex, iSubItem);
   if( pLink == NULL ) return 0;
   return pLink->lParam;
}

int CScriptView::GetItemLinkType(int iIndex, int iSubItem) const
{
   ATLASSERT(::IsWindow(m_hWnd));
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return -1;
   if( iSubItem < 0 ) return -1;
   const SCRIPTLINK* pLink = _GetLink(iIndex, iSubItem);
   if( pLink == NULL ) return -1;
   return pLink->iType;
}

BOOL CScriptView::GetItemLinkName(int iIndex, int iSubItem, LPSTR pstrName, int cchMax) const
{
   ATLASSERT(::IsWindow(m_hWnd));
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return -1;
   if( iSubItem < 0 ) return FALSE;
   const SCRIPTLINK* pLink = _GetLink(iIndex, iSubItem);
   if( pLink == NULL ) return FALSE;
   return _tcsncpy(pstrName, pLink->szName, cchMax) > 0;
}

int CScriptView::GetItemLinkCount(int iIndex, int iType /*= -1*/) const
{
   ATLASSERT(::IsWindow(m_hWnd));
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return 0;
   const SCRIPTLINK* pLink = m_aItems[iIndex].pLinks;
   int nCount = 0;
   while( pLink != NULL ) {
      if( iType == -1 || iType == pLink->iType ) nCount++;
      pLink = pLink->pNext;
   }
   return nCount;
}

BOOL CScriptView::GetItemLinkTarget(int iIndex, int iSubItem, int& iTarget, int& iTargetSubItem) const
{
   ATLASSERT(::IsWindow(m_hWnd));
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return FALSE;
   if( iSubItem < 0 ) return FALSE;
   const SCRIPTLINK* pLink = _GetLink(iIndex, iSubItem);
   if( pLink == NULL ) return FALSE;
   if( pLink->wDestItemID == 0 ) return FALSE;
   ATLASSERT(pLink->pTargetLink);
   for( iTarget = 0; iTarget < m_aItems.GetSize(); iTarget++ ) {
      if( m_aItems[iTarget].wId == pLink->wDestItemID ) {         
         iTargetSubItem = FindLink(iTarget, pLink->pTargetLink->szName);
         return iTargetSubItem >= 0;
      }
   }
   return FALSE;
}

BOOL CScriptView::ConnectItem(int iFromIndex, int iFromSubItem, int iToIndex, int iToSubItem, UINT uStyle /* =0*/)
{
   if( iFromIndex < 0 || iFromIndex >= m_aItems.GetSize() ) return FALSE;
   if( iToIndex < 0 || iToIndex >= m_aItems.GetSize() ) return FALSE;
   if( iFromSubItem < 0 ) return 0;
   if( iToSubItem < 0 ) return 0;
   SCRIPTLINK* pFromLink = _GetLink(iFromIndex, iFromSubItem);
   SCRIPTLINK* pToLink = _GetLink(iToIndex, iToSubItem);
   if( pFromLink == NULL ) return FALSE;
   if( pToLink == NULL ) return FALSE;
   if( pFromLink->iType != 1 ) return FALSE;
   if( pToLink->iType != 0 ) return FALSE;
   pFromLink->wDestItemID = m_aItems[iToIndex].wId;
   pFromLink->pTargetLink = pToLink;
   pFromLink->uStyle = uStyle;
   return UpdateLayout();
}

BOOL CScriptView::DisconnectItem(int iIndex, int iSubItem)
{
   if( iIndex < 0 || iIndex >= m_aItems.GetSize() ) return FALSE;
   SCRIPTLINK* pFromLink = _GetLink(iIndex, iSubItem);
   if( pFromLink == NULL ) return FALSE;
   pFromLink->wDestItemID = 0;
   pFromLink->pTargetLink = NULL;
   pFromLink->nPoints = 0;
   return UpdateLayout();
}

int CScriptView::HitTest(SCHITTESTINFO* pHitInfo) const
{ 
   ATLASSERT(pHitInfo);
   POINT pt = { pHitInfo->pt.x - m_ptOffset.x, pHitInfo->pt.y - m_ptOffset.y };
   // Get display order. It also determines which items that are
   // closest to the user on the screen! This solves problems with
   // overlapping items.
   CSimpleArray<int> aOrder;
   _GetDisplayOrder(aOrder);
   // Test if we hit a button...
   for( int i = aOrder.GetSize() - 1; i >= 0; i-- ) {
      // If we hit inside the button, let's investigate...
      int iIndex = aOrder[i];
      const SCRIPTITEM& Item = m_aItems[iIndex];
      if( ::PtInRect(&Item.rcItem, pt) ) {
         pHitInfo->iItem = iIndex;
         pHitInfo->iSubItem = -1;
         pHitInfo->flags = SCHT_NOWHERE;
         // Check where we hit inside the button
         RECT rcIcon = { 0 };
         RECT rcTitle = { 0 };
         _GetItemPartRect(iIndex, rcIcon, rcTitle);
         if( ::PtInRect(&rcIcon, pt) ) {
            pHitInfo->iSubItem = 0;
            pHitInfo->flags = SCHT_ONITEMICON;
            return iIndex;
         }
         if( ::PtInRect(&rcTitle, pt) ) {
            pHitInfo->flags = SCHT_ONTITLE;
            return iIndex;
         }
         // We might have hit one of the link boxes
         int iSubItem = 0;
         const SCRIPTLINK* pLink = Item.pLinks;
         while( pLink != NULL ) {
            if( ::PtInRect(&pLink->rcItem, pt) ) {
               pHitInfo->iSubItem = iSubItem;
               pHitInfo->flags = SCHT_ONITEMLINK;
               return iIndex;
            }
            iSubItem++;
            pLink = pLink->pNext;
         }
         return iIndex;
      }
   }
   // TODO: Test if we hit a line in a route
   return -1;
}

BOOL CScriptView::UpdateLayout(BOOL bRedraw /*= TRUE*/)
{
   ATLASSERT(::IsWindow(m_hWnd));
   // We merely flag that we want to update the layout at a later
   // point. Surely this is not the expected behaviour when a user calls
   // this method directly (positions/sizes are not updated immediately)!
   m_bRecalcLayout = true;
   if( bRedraw ) Invalidate();
   return TRUE;
}


/////////////////////////////////////////////////////////////////
// Message handlers

LRESULT CScriptView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   _Init();
   return 0;
}

LRESULT CScriptView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   bHandled = FALSE;
   // Delete items
   while( m_aItems.GetSize() > 0 ) _DeleteItem(0, false, true);
   // Clean up imagelist
   if( !m_Images.IsNull() && (m_dwExtStyle & SC_EX_SHAREIMAGELIST) == 0 ) m_Images.Destroy();
   return 0;
}

LRESULT CScriptView::OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
   // Send notify
   if( _SendNotifyMessage(NM_SETCURSOR) != 0 ) return 0;
   // Don't mess with it if we're tracking mouse
   if( GetCapture() == m_hWnd ) return 0;
   // Let system handle it
   bHandled = FALSE;
   return 0;
}

LRESULT CScriptView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   RECT rcClient;
   GetClientRect(&rcClient);
   SetScrollRange(SB_HORZ, 0, CX_VIEW - (rcClient.right - rcClient.left), FALSE);
   SetScrollRange(SB_VERT, 0, CY_VIEW - (rcClient.bottom- rcClient.top), FALSE);
   UpdateLayout(FALSE);
   return 0;
}

LRESULT CScriptView::OnScroll(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   int sb = uMsg == WM_HSCROLL ? SB_HORZ : SB_VERT;
   SCROLLINFO si = { sizeof(SCROLLINFO), SIF_POS | SIF_RANGE };
   GetScrollInfo(sb, &si);
   int nPos = si.nPos;
   switch( LOWORD(wParam) ) {
   case SB_LINEUP:
      nPos -= 16;
      break;
   case SB_LINEDOWN:
      nPos += 16;
      break;
   case SB_LEFT:
      nPos -= 6;
      break;
   case SB_RIGHT:
      nPos += 6;
      break;
   case SB_PAGEUP:
      nPos -= 80;
      break;
   case SB_PAGEDOWN:
      nPos += 80;
      break;
   case SB_THUMBTRACK:      
   case SB_THUMBPOSITION:
      nPos = (int) (short) HIWORD(wParam);
      break;
   }
   if( nPos < si.nMin ) nPos = si.nMin;
   if( nPos > si.nMax ) nPos = si.nMax;
   if( nPos == si.nPos ) return 0;
   SetScrollPos(sb, nPos, TRUE);
   // Remember this position
   m_ptOffset.x = -CWindow::GetScrollPos(SB_HORZ);
   m_ptOffset.y = -CWindow::GetScrollPos(SB_VERT);
   // Repaint
   Invalidate();
   return 0;
}

LRESULT CScriptView::OnRButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
   SetFocus();

   ReleaseCapture();

   _SendNotifyMessage(NM_RCLICK);

   // Check where we pressed the button
   SCHITTESTINFO schti = { 0 };
   schti.pt.x = GET_X_LPARAM(lParam);
   schti.pt.y = GET_Y_LPARAM(lParam);
   int iIndex = HitTest(&schti);
   if( iIndex < 0 ) {
      // Clear selection
      SetItemState(-1, 0, SCIS_SELECTED);
      return 0;
   }

   if( schti.flags == SCHT_ONCONNECTOR ) return 0;

   // Toggle selection for item
   if( (wParam & MK_SHIFT) == 0 
       && !(GetSelectedCount() == 1 && GetNextItem(-1, SCIS_SELECTED) == iIndex) )
   {
      SetItemState(-1, 0, SCIS_SELECTED);
   }
   SetItemState(iIndex, SCIS_SELECTED, SCIS_SELECTED);

   return 0;
}

LRESULT CScriptView::OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
   SetFocus();

   POINT pt;
   pt.x = GET_X_LPARAM(lParam);
   pt.y = GET_Y_LPARAM(lParam);

   POINT ptView;
   ptView.x = pt.x - m_ptOffset.x;
   ptView.y = pt.y - m_ptOffset.y;

   _SendNotifyMessage(NM_CLICK);

   // Check where we pressed the button
   SCHITTESTINFO schti = { 0 };
   schti.pt = pt;
   int iIndex = HitTest(&schti);
   if( iIndex < 0 ) {
      // Clear selection
      if( (wParam & MK_SHIFT) == 0 ) SetItemState(-1, 0, SCIS_SELECTED);
      UpdateWindow();
      // Start rubber-band selection?
      if( ::DragDetect(m_hWnd, schti.pt) ) 
      {
         UpdateWindow();   // FIX: Excessive use of UpdateWindow() needed to avoid
                           //      repaint problems.
         SetCapture();
         ::SetRect(&m_rcRubber, ptView.x, ptView.y, ptView.x, ptView.y);
         _DrawRubberBand(m_rcRubber);
         m_CaptureState = CAPTURE_RUBBERBAND;
      }
      return 0;
   }

   if( schti.flags == SCHT_ONCONNECTOR ) return 0;

   // Toggle selection for the item that was hit. Only
   // need to do this if the item wasn't selected already!
   if( (GetItemState(iIndex) & SCIS_SELECTED) == 0 ) {
      if( (wParam & MK_SHIFT) == 0 ) SetItemState(-1, 0, SCIS_SELECTED);
      SetItemState(iIndex, SCIS_SELECTED, SCIS_SELECTED);
      UpdateWindow();
   }

   // Some parts of the item is not selectable/draggable!   
   if( schti.flags == SCHT_ONITEMICON ) {
      if( (GetItemState(iIndex) & SCIS_RIGHTALIGN) != 0 ) schti.flags = SCHT_ONITEMLINK; else return 0;
   }

   // Check if we're trying to drag'n'drop this item
   if( !::DragDetect(m_hWnd, schti.pt) ) return 0;

   // Notify parent that we're beginning to drag item
   NMSCRIPTVIEW nmsv = { 0 };
   nmsv.iItem = schti.iItem;
   nmsv.iSubItem = schti.iSubItem;
   nmsv.ptAction = schti.pt;
   nmsv.lParam = GetItemData(nmsv.iItem);
   _SendNotifyMessage(SCN_BEGINDRAG, &nmsv);  

   if( schti.flags != SCHT_ONITEMLINK || GetSelectedCount() > 1 ) 
   {
      UpdateWindow();
      SetCapture();
      ::SetCursor( ::LoadCursor(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_DRAGITEM)) );
      m_ptDragStart = ptView;
      m_ptDragOffset.x = m_ptDragOffset.y = 0;
      _DrawDragRect(m_ptDragOffset);
      m_CaptureState = CAPTURE_MOVEITEM;
   }
   else 
   {
      // Cannot drag from an "input" box?
      if( GetItemLinkType(schti.iItem, schti.iSubItem) == 0 ) return 0;
      // Remember which item/link this was
      SCRIPTLINK* pLink = _GetLink(schti.iItem, schti.iSubItem);
      ATLASSERT(pLink);
      if( pLink == NULL ) return 0;
      m_iDragSubItem = schti.iSubItem;
      m_wDragLinkID = pLink->wID;
      // Let's drag'n'roll
      UpdateWindow();
      SetCapture();
      m_CaptureState = CAPTURE_CREATELINK;
      // Simulate mousemove to generate correct events at once
      SendMessage(WM_MOUSEMOVE, wParam, lParam);
   }

   return 0;
}

LRESULT CScriptView::OnMouseMove(UINT uMsg, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
   POINT ptView;
   ptView.x = GET_X_LPARAM(lParam) - m_ptOffset.x;
   ptView.y = GET_Y_LPARAM(lParam) - m_ptOffset.y;

   if( m_CaptureState == CAPTURE_MOVEITEM )
   {
      // Moving one or more items
      _DrawDragRect(m_ptDragOffset);
      m_ptDragOffset.x = GRID_ALIGN(ptView.x - m_ptDragStart.x);
      m_ptDragOffset.y = GRID_ALIGN(ptView.y - m_ptDragStart.y);
      _DragScrollView(lParam);
      _DrawDragRect(m_ptDragOffset);
   }
   else if( m_CaptureState == CAPTURE_CREATELINK ) 
   {
      // Currently dragging a link to a new position
      _DragScrollView(lParam);
      NMSCRIPTVIEW nmsv = { 0 };
      nmsv.iItem = GetNextItem(-1, SCIS_SELECTED);
      nmsv.iSubItem = m_iDragSubItem;
      nmsv.lParam = GetItemData(nmsv.iItem);
      nmsv.uNewState = uMsg == WM_MOUSEMOVE ? SCIS_TEMPORARY : 0;
      nmsv.ptAction.x = GET_X_LPARAM(lParam);
      nmsv.ptAction.y = GET_Y_LPARAM(lParam);
      _SendNotifyMessage(SCN_VALIDATECONNECTION, &nmsv);      
   }
   else if( m_CaptureState == CAPTURE_RUBBERBAND )
   {
      // Rubberband selection
      _DrawRubberBand(m_rcRubber);
      m_rcRubber.right = m_rcRubber.left + (ptView.x - m_rcRubber.left);
      m_rcRubber.bottom = m_rcRubber.top + (ptView.y - m_rcRubber.top);
      _DragScrollView(lParam);
      _DrawRubberBand(m_rcRubber);
   }
   else
   {
      // Hovering over link (highlight route)
      SCHITTESTINFO schti = { 0 };
      schti.pt.x = GET_X_LPARAM(lParam);
      schti.pt.y = GET_Y_LPARAM(lParam);
      int iIndex = HitTest(&schti);
      if( m_iHightlightItem != -1 
          && (schti.iItem != m_iHightlightItem || schti.iSubItem != m_iHightlightSubItem) ) 
      {
         SCRIPTLINK* pLink = _GetLink(m_iHightlightItem, m_iHightlightSubItem);
         pLink->uStyle &= ~SCIS_HIGHTLIGHTED;
         Invalidate();
         m_iHightlightItem = -1;
         m_iHightlightSubItem = -1;
      }
      if( iIndex >= 0 && schti.flags == SCHT_ONITEMLINK ) 
      {
         m_iHightlightItem = schti.iItem;
         m_iHightlightSubItem = schti.iSubItem;
         m_aItems[schti.iItem].uOrder = ++m_uMaxOrder;
         SCRIPTLINK* pLink = _GetLink(schti.iItem, schti.iSubItem);
         pLink->uStyle |= SCIS_HIGHTLIGHTED;
         Invalidate();
      }
   }
   return 0;
}

LRESULT CScriptView::OnLButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{  
   if( m_CaptureState == CAPTURE_MOVEITEM ) 
   {
      ReleaseCapture();
      // Commit item drag-operation changes
      int iIndex = GetNextItem(-1, SCIS_SELECTED);
      while( iIndex >= 0 ) {
         // Really move the item!
         m_aItems[iIndex].rcItem.left += m_ptDragOffset.x;
         m_aItems[iIndex].rcItem.top += m_ptDragOffset.y;
         // Notify parent
         NMSCRIPTVIEW nmsv = { 0 };
         nmsv.iItem = iIndex;
         nmsv.lParam = GetItemData(iIndex);
         _SendNotifyMessage(SCN_ITEMMOVED, &nmsv);
         // Next selected item
         iIndex = GetNextItem(iIndex, SCIS_SELECTED);
      }
      UpdateLayout();
   }
   else if( m_CaptureState == CAPTURE_CREATELINK )
   {
      ReleaseCapture();
      // Create the link
      BOOL bDummy;
      OnMouseMove(WM_LBUTTONUP, wParam, lParam, bDummy);
   }
   else if( m_CaptureState == CAPTURE_RUBBERBAND )
   {
      ReleaseCapture();
      // Find items within rubberband bounds and select them
      RECT rcRubber = _NormalizeRect(m_rcRubber);
      for( int iIndex = 0; iIndex < m_aItems.GetSize(); iIndex++ ) {
         RECT rcItem = m_aItems[iIndex].rcItem;
         POINT ptTopLeft = { rcItem.left, rcItem.top };
         POINT ptBottomRight = { rcItem.right, rcItem.bottom };
         if( !::PtInRect(&rcRubber, ptTopLeft) || !::PtInRect(&rcRubber, ptBottomRight) ) continue;
         SetItemState(iIndex, SCIS_SELECTED, SCIS_SELECTED);
      }
   }

   return 0;
}

LRESULT CScriptView::OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
   // Double-clicking on titlebar causes activate event
   SCHITTESTINFO schti = { 0 };
   schti.pt.x = GET_X_LPARAM(lParam);
   schti.pt.y = GET_Y_LPARAM(lParam);
   int iIndex = HitTest(&schti);
   if( iIndex >= 0 && schti.flags == SCHT_ONTITLE ) {
      NMSCRIPTVIEW nmsv = { 0 };
      nmsv.iItem = iIndex;
      nmsv.lParam = GetItemData(iIndex);
      if( _SendNotifyMessage(SCN_ITEMACTIVATE, &nmsv) != 0 ) return 0;
   }
   // Else send regular notifications
   _SendNotifyMessage(NM_DBLCLK);
   return 0;
}

LRESULT CScriptView::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
   // Send notification
   NMSCVKEYDOWN nmsv = { 0 };
   nmsv.wVKey = (WORD) wParam;
   nmsv.flags = 0;
   _SendNotifyMessage(SCN_KEYDOWN, &nmsv);
   // Return key?
   if( wParam == VK_RETURN && GetSelectedCount() == 1 ) {
      if( _SendNotifyMessage(NM_RETURN) == 0 ) {
         NMSCRIPTVIEW nmsv = { 0 };
         nmsv.iItem = GetNextItem(-1, SCIS_SELECTED);
         nmsv.lParam = GetItemData(nmsv.iItem);
         _SendNotifyMessage(SCN_ITEMACTIVATE, &nmsv);
      }
   }
   // F2 key = edit label
   if( wParam == VK_F2 && GetSelectedCount() == 1 ) {
      NMSCRIPTVIEW nmsv = { 0 };
      nmsv.iItem = GetNextItem(-1, SCIS_SELECTED);
      nmsv.lParam = GetItemData(nmsv.iItem);
      _SendNotifyMessage(SCN_BEGINLABELEDIT, &nmsv);
   }
   bHandled = FALSE;
   return 0;
}

LRESULT CScriptView::OnCaptureChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   if( m_CaptureState == CAPTURE_MOVEITEM ) 
   {
      _DrawDragRect(m_ptDragOffset);
      _SendNotifyMessage(NM_RELEASEDCAPTURE);
   }
   else if( m_CaptureState == CAPTURE_CREATELINK ) 
   {
      m_wDragLinkID = 0;
      m_iDragSubItem = -1;
      _SendNotifyMessage(NM_RELEASEDCAPTURE);
      UpdateLayout();
   }
   else if( m_CaptureState == CAPTURE_RUBBERBAND )
   {
      _DrawRubberBand(m_rcRubber);
   }
   m_CaptureState = CAPTURE_NONE;
   Invalidate();
   return 0;
}


/////////////////////////////////////////////////////////////////
// Helper methods


void CScriptView::_Init()
{
   m_wndNotify = GetParent();
   m_dwExtStyle = 0;
   m_szTitle[0] = '\0';

   CClientDC dc = m_hWnd;
   HFONT hOldFont = dc.SelectFont(AtlGetDefaultGuiFont());
   CLogFont lf;
   lf.SetMessageBoxFont();
   m_Settings.fontText.CreateFontIndirect(&lf);
   dc.SelectFont(m_Settings.fontText);
   dc.GetTextMetrics(&m_Settings.tmText);
   lf.SetCaptionFont();
   m_Settings.fontCaption.CreateFontIndirect(&lf);
   dc.SelectFont(m_Settings.fontCaption);
   dc.GetTextMetrics(&m_Settings.tmCaption);
   dc.SelectFont(hOldFont);

   m_Settings.cyLink = m_Settings.tmText.tmHeight + 4 + ITEM_LINK_GAP;
   m_Settings.cyCaption = max(ICON_SIZE + 2, m_Settings.tmCaption.tmHeight + 2) + ITEM_CAPTION_GAP;
   m_Settings.cxyBorders = 2;

   m_Settings.clrBack = ::GetSysColor(COLOR_BTNFACE);
   m_Settings.clrText = ::GetSysColor(COLOR_BTNTEXT);
   m_Settings.clrLink = ::GetSysColor(COLOR_BTNSHADOW);
   m_Settings.clrBtnText = ::GetSysColor(COLOR_BTNTEXT);
   m_Settings.clrTitleBack = ::GetSysColor(COLOR_HIGHLIGHT);
   m_Settings.clrTitleText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
   m_Settings.clrDarkBorder = ::GetSysColor(COLOR_BTNSHADOW);
   m_Settings.clrLightBorder = ::GetSysColor(COLOR_BTNHIGHLIGHT);
   m_Settings.clrActiveCaption = ::GetSysColor(COLOR_ACTIVECAPTION);
   m_Settings.clrInactiveCaption = ::GetSysColor(COLOR_INACTIVECAPTION);
   m_Settings.clrActiveCaptionText = ::GetSysColor(COLOR_WINDOW);
   m_Settings.clrInactiveCaptionText = ::GetSysColor(COLOR_INACTIVECAPTIONTEXT);

   SetScrollRange(SB_HORZ, 0, CX_VIEW, FALSE);
   SetScrollRange(SB_VERT, 0, CY_VIEW, FALSE);

   DeleteAllItems();
}

BOOL CScriptView::_DeleteItem(int iIndex, bool bClearLinks /*= true*/, bool bSendNoify /*= true*/)
{
   SCRIPTITEM& Item = m_aItems[iIndex];
   // Clear all links referrering to this item
   if( bClearLinks ) {
      for( int i = 0; i < m_aItems.GetSize(); i++ ) {
         SCRIPTLINK* pLink = m_aItems[i].pLinks;
         while( pLink != NULL ) {
            if( pLink->wDestItemID == Item.wId ) {
               pLink->wDestItemID = 0;
               pLink->pTargetLink = NULL;
               pLink->nPoints = 0;
            }
            pLink = pLink->pNext;
         }
      }
   }
   // Remove the links and item structures
   SCRIPTLINK* pLink = Item.pLinks;
   while( pLink != NULL ) {
      SCRIPTLINK* pNext = pLink->pNext;
      delete pLink;
      pLink = pNext;
   }
   // Clean up state
   m_iHightlightItem = -1;
   m_iHightlightSubItem = -1;
   // Send notification to parent
   if( bSendNoify ) {
      NMSCRIPTVIEW nmsv = { 0 };
      nmsv.iItem = iIndex;
      nmsv.lParam = Item.lParam;
      _SendNotifyMessage(SCN_DELETEITEM, &nmsv);
   }
   // Finally remove from collection
   return m_aItems.RemoveAt(iIndex);
}

CScriptView::SCRIPTLINK* CScriptView::_GetLink(int iIndex, int iSubItem) const
{
   ATLASSERT(iIndex>=0 && iIndex<m_aItems.GetSize());
   ATLASSERT(iSubItem>=0);
   SCRIPTLINK* pLink = m_aItems[iIndex].pLinks;
   while( pLink != NULL && --iSubItem >= 0 ) pLink = pLink->pNext;
   return pLink;
}

void CScriptView::_GetItemPartRect(int iIndex, RECT& rcIcon, RECT& rcTitle) const
{
   const SCRIPTITEM& Item = m_aItems[iIndex];
   if( (Item.uState & SCIS_RIGHTALIGN) == 0 ) {
      ::SetRect(&rcIcon, Item.rcItem.left + 2, Item.rcItem.top + 2, Item.rcItem.left + ICON_SIZE + 4, Item.rcItem.top + m_Settings.cyCaption);
      ::SetRect(&rcTitle, Item.rcItem.left + ICON_SIZE + 4, Item.rcItem.top + 2, Item.rcItem.right - 3, Item.rcItem.top + m_Settings.cyCaption);
   }
   else {
      ::SetRect(&rcIcon, Item.rcItem.right - ICON_SIZE - 4, Item.rcItem.top + 2, Item.rcItem.right - 2, Item.rcItem.top + m_Settings.cyCaption);
      ::SetRect(&rcTitle, Item.rcItem.left + 2, Item.rcItem.top + 2, Item.rcItem.right - ICON_SIZE - 4, Item.rcItem.top + m_Settings.cyCaption);
   }
}

RECT CScriptView::_NormalizeRect(RECT rc) const
{
   if( rc.left > rc.right ) {
      int iTemp = rc.left;
      rc.left = rc.right;
      rc.right = iTemp;
   }
   if( rc.top > rc.bottom ) {
      int iTemp = rc.top;
      rc.top = rc.bottom;
      rc.bottom = iTemp;
   }
   return rc;
}

LRESULT CScriptView::_SendNotifyMessage(UINT code, LPVOID p)
{
   NMHDR temp = { 0 };
   if( p == NULL ) p = &temp;
   NMHDR* pHeader = (NMHDR*) p;
   pHeader->code = code;
   pHeader->hwndFrom = m_hWnd;
   pHeader->idFrom = GetDlgCtrlID();
   return m_wndNotify.SendMessage(WM_NOTIFY, (WPARAM) pHeader->idFrom, (LPARAM) p);
}

void CScriptView::_GetDisplayOrder(CSimpleArray<int>& aOrder) const
{
   int nCount = m_aItems.GetSize();
   for( int i = 0; i < nCount; i++ ) {
      aOrder.Add(i);
   }
   // TODO: Don't use bubble-sort for this!
   for( int x = 0; x < nCount; x++ ) {
      for( int y = x + 1; y < nCount; y++ ) {
         if( m_aItems[ aOrder[x] ].uOrder > m_aItems[ aOrder[y] ].uOrder ) {
            int iTemp = aOrder[x];
            aOrder[x] = aOrder[y];
            aOrder[y] = iTemp;
         }
      }
   }
}

void CScriptView::_UIUpdate()
{
}


/////////////////////////////////////////////////////////////////
// Painting

void CScriptView::DoPaint(CDCHandle dc)
{
   ATLASSERT(!m_Images.IsNull());

   // Make sure model is up-to-date and everything has
   // been laid out...
   if( m_bRecalcLayout ) {
      _LayoutButtons();
      _LayoutLinks();
      _UIUpdate();
      m_bRecalcLayout = false;
   }

   CSaveDC savedc = dc;

   RECT rcClient;
   GetClientRect(&rcClient);

   CSimpleArray<int> aOrder;
   _GetDisplayOrder(aOrder);     

   dc.FillSolidRect(&rcClient, ::GetSysColor(COLOR_WINDOW));

   // From now on, the DC center is skewed to the scrollbar pos
   dc.SetWindowOrg(-m_ptOffset.x, -m_ptOffset.y);

   // Paint all the lines first
   _DoPaintLinks(dc, rcClient, aOrder);

   // Paint special buttons (such as comments and notes)
   _DoPaintSpecialButtons(dc, rcClient, aOrder);

   // Paint all boxes
   _DoPaintNormalButtons(dc, rcClient, aOrder);

   // Paint additional markers (such as breakpoints)
   _DoPaintMarkers(dc, rcClient, aOrder);
}

void CScriptView::_DoPaintSpecialButtons(CDCHandle /*dc*/, RECT /*rcClip*/, CSimpleArray<int>& /*aOrder*/)
{
   // TODO: Paint comment- and notification-boxes
}

void CScriptView::_DoPaintNormalButtons(CDCHandle dc, RECT /*rcClip*/, CSimpleArray<int>& aOrder)
{
   for( int i = 0; i < m_aItems.GetSize(); i++ ) {
      int iIndex = aOrder[i];
      const SCRIPTITEM& Item = m_aItems[iIndex];
      RECT rcItem = Item.rcItem;

      dc.DrawEdge(&rcItem, EDGE_RAISED, BF_ADJUST | BF_RECT);
      dc.FillSolidRect(&rcItem, m_Settings.clrBack);

      RECT rcIcon = { 0 };
      RECT rcTitle = { 0 };
      _GetItemPartRect(iIndex, rcIcon, rcTitle);

      dc.Draw3dRect(&rcIcon, m_Settings.clrDarkBorder, m_Settings.clrLightBorder);

      POINT ptImage = { rcIcon.left + 1, rcIcon.top + 2 };
      m_Images.Draw(dc, Item.iImage, ptImage, ILD_TRANSPARENT);

      dc.FillSolidRect(&rcTitle, (Item.uState & SCIS_SELECTED) != 0 ? m_Settings.clrActiveCaption : m_Settings.clrInactiveCaption);

      rcTitle.left += 3;
      COLORREF clrText = (Item.uState & SCIS_SELECTED) != 0 ? m_Settings.clrActiveCaptionText : m_Settings.clrInactiveCaptionText;
      if( Item.clrText != CLR_DEFAULT ) clrText = Item.clrText;
      dc.SetTextColor(clrText);
      dc.SetBkMode(TRANSPARENT);
      dc.SelectFont(m_Settings.fontCaption);
      dc.DrawText(Item.szName, -1, &rcTitle, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS);

      dc.SelectFont(m_Settings.fontText);
      const SCRIPTLINK* pLink = Item.pLinks;
      while( pLink != NULL ) {
         RECT rcLink = pLink->rcItem;
         dc.Draw3dRect(&rcLink, m_Settings.clrDarkBorder, m_Settings.clrLightBorder);
         if( ::IsCharAlphaNumeric(pLink->szName[0]) ) {
            ::InflateRect(&rcLink, -1, -1);
            dc.SetTextColor(pLink->clrText == CLR_DEFAULT ? m_Settings.clrBtnText : pLink->clrText);
            dc.DrawText(pLink->szName, -1, &rcLink, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS);   
         }
         pLink = pLink->pNext;
      }
   }
}

void CScriptView::_DoPaintLinks(CDCHandle dc, RECT /*rcClip*/, CSimpleArray<int>& aOrder)
{
   CPen penLink;
   CPen penError;
   CPen penHighlight;
   CPen penSelected;
   penLink.CreatePen(PS_SOLID, 1, m_Settings.clrLink);
   penError.CreatePen(PS_SOLID, 1, RGB(200,0,200));
   penHighlight.CreatePen(PS_SOLID, 1, RGB(240,0,0));
   penSelected.CreatePen(PS_SOLID, 1, RGB(0,0,240));

   CBrush brush;
   brush.CreateSolidBrush(m_Settings.clrLink);
   
   HPEN hOldPen = dc.SelectPen(penLink);
   HBRUSH hOldBrush = dc.SelectBrush(brush);

   for( int i = 0; i < m_aItems.GetSize(); i++ ) {
      int iIndex = aOrder[i];
      const SCRIPTITEM& Item = m_aItems[iIndex];
      const SCRIPTLINK* pLink = Item.pLinks;
      while( pLink != NULL ) {
         // Draw lines between links
         if( pLink->nPoints >= 2 ) {
            
            if( (pLink->uStyle & SCIS_HIGHTLIGHTED) != 0 ) dc.SelectPen(penHighlight);
            else if( pLink->nPoints <= 2 ) dc.SelectPen(penError);
            else if( (Item.uState & SCIS_SELECTED) != 0 ) dc.SelectPen(penSelected);
            else dc.SelectPen(penLink);

            int nPoint = 0;
            const POINT* pts = pLink->ptWaypoints;

            POINT pt = pts[nPoint++];
            dc.MoveTo(pt);

            enum
            {
               CX_BEND = 2,
               CY_BEND = 2,
            };

            POINT ptOld;
            POINT ptNext;
            for( int j = nPoint; j < pLink->nPoints - 1; j++ ) {
               if( j < pLink->nPoints - 1 ) 
               { 
                  // If there's room for this, we're creating
                  // artificial bends along the lines...
                  ptOld = pts[nPoint - 1];
                  pt = pts[nPoint++];
                  ptNext = pts[nPoint];
                  if( ptOld.x == pt.x 
                      && pt.y == ptNext.y
                      && abs(ptOld.y - pt.y) > CY_BEND * 2 ) 
                  {
                     POINT ptBend = pt;
                     pt.y += ptOld.y < pt.y ? -CY_BEND : CY_BEND;
                     dc.LineTo(pt);
                     ptBend.x += pt.x > ptNext.x ? -CX_BEND : CX_BEND;
                     dc.LineTo(ptBend);
                  }
                  else if( ptOld.y == pt.y 
                           && pt.x == ptNext.x 
                           && abs(ptOld.x - pt.x) > CX_BEND * 2 ) 
                  {
                     POINT ptBend = pt;
                     pt.x += ptOld.x < pt.x ? -CX_BEND : CX_BEND;
                     dc.LineTo(pt);
                     ptBend.y += pt.y > ptNext.y ? -CY_BEND : CY_BEND;
                     dc.LineTo(ptBend);
                  }
                  else 
                  {
                     dc.LineTo(pt);
                  }
               }
               else 
               {
                  pt = pts[nPoint++];
                  dc.LineTo(pt);
               }
            }

            // Draw root of arrow...
            pt = pts[nPoint];
            pt.x -= 5;
            dc.LineTo(pt);
            pt.x += 1;
            dc.LineTo(pt);
            // Draw arrow-head...
            POINT arrow[3] = 
            {
               { pt.x + 3, pt.y },
               { pt.x, pt.y - 2 },
               { pt.x, pt.y + 2 }
            };
            dc.Polygon(arrow, 3);
         }
         pLink = pLink->pNext;
      }
   }

   dc.SelectPen(hOldPen);
   dc.SelectBrush(hOldBrush);
}

void CScriptView::_DoPaintMarkers(CDCHandle /*dc*/, RECT /*rcClip*/, CSimpleArray<int>& /*aOrder*/)
{
   // TODO: Paint breakpoint icons, etc.
}

void CScriptView::_DrawDragRect(POINT ptDisplacement) const
{
   CClientDC dc = m_hWnd;
   RECT rcTotal = { 0 };
   SIZE sizeDragFrame = { 1, 1 };
   SIZE sizeDragSmallFrame = { 1, 1 };
   int nSelCount = GetSelectedCount();
   CBrush brush = _GetHalfHalftoneBrush();
   int iIndex = GetNextItem(-1, SCIS_SELECTED);
   while( iIndex >= 0 ) {
      const SCRIPTITEM& Item = m_aItems[iIndex];
      // Expand the area for the big frame
      RECT rcItem = Item.rcItem;
      ::OffsetRect(&rcItem, m_ptOffset.x, m_ptOffset.y);
      ::OffsetRect(&rcItem, ptDisplacement.x, ptDisplacement.y);
      ::UnionRect(&rcTotal, &rcTotal, &rcItem);
      // Don't create a frame for the item if only one is selected
      if( nSelCount == 1 ) break;
      // Draw drag-frame around items too
      dc.DrawDragRect(&rcItem, sizeDragSmallFrame, NULL, sizeDragSmallFrame, brush);
      iIndex = GetNextItem(iIndex, SCIS_SELECTED);
   }
   ::InflateRect(&rcTotal, 2, 2);
   rcTotal = _NormalizeRect(rcTotal);
   dc.DrawDragRect(&rcTotal, sizeDragFrame, NULL, sizeDragFrame);
}

void CScriptView::_DragScrollView(LPARAM lParam)
{
   POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
   RECT rcClient;
   GetClientRect(&rcClient);
   SIZE size = { 0 };
   const int EDGE_GAP = 30;
   if( pt.x < rcClient.left + EDGE_GAP ) size.cx = SB_LEFT;
   if( pt.y < rcClient.top + EDGE_GAP ) size.cy = SB_LEFT;
   if( pt.x > rcClient.right - EDGE_GAP ) size.cx = SB_RIGHT;
   if( pt.y > rcClient.bottom - EDGE_GAP ) size.cy = SB_RIGHT;
   if( size.cx != 0 ) SendMessage(WM_HSCROLL, MAKEWPARAM(size.cx, 0), 0L);
   if( size.cy != 0 ) SendMessage(WM_VSCROLL, MAKEWPARAM(size.cy, 0), 0L);
   if( size.cx != 0 || size.cy != 0 ) UpdateWindow();
}

void CScriptView::_DrawRubberBand(RECT rcFrame) const
{
   CClientDC dc = m_hWnd;
   SIZE sizeDragFrame = { 1, 1 };
   rcFrame = _NormalizeRect(rcFrame);
   ::OffsetRect(&rcFrame, m_ptOffset.x, m_ptOffset.y);
   dc.DrawDragRect(&rcFrame, sizeDragFrame, NULL, sizeDragFrame);
}

CBrushHandle CScriptView::_GetHalfHalftoneBrush() const
{
   HBRUSH halftoneBrush = NULL;
   WORD grayPattern[8] = { 0x1111, 0x2222, 0x4444, 0x8888, 0x1111, 0x2222, 0x4444, 0x8888 };
   HBITMAP grayBitmap = ::CreateBitmap(8, 8, 1, 1, &grayPattern);
   if( grayBitmap != NULL ) {
      halftoneBrush = ::CreatePatternBrush(grayBitmap);
      ::DeleteObject(grayBitmap);
   }
   return CBrushHandle(halftoneBrush);
}


/////////////////////////////////////////////////////////////////
// Layout methods

void CScriptView::_LayoutButtons()
{
   for( int i = 0; i < m_aItems.GetSize(); i++ ) {
      SCRIPTITEM& Item = m_aItems[i];

      // Set rectangle for item
      Item.rcItem.right = Item.rcItem.left + Item.cxWidth;
      Item.rcItem.bottom = Item.rcItem.top + m_Settings.cyCaption + (m_Settings.cxyBorders * 2);

      RECT rcLinks = { 0 };
      ::SetRect(&rcLinks, Item.rcItem.left + 3, Item.rcItem.bottom, Item.rcItem.right - 3, Item.rcItem.bottom);

      // Recalc rectangles for links
      SCRIPTLINK* pLink = Item.pLinks;
      int nLeft = 0;
      int nRight = 0;
      int nCount = 0;
      int iMiddle = rcLinks.left + ((rcLinks.right - rcLinks.left) / 2);
      while( pLink != NULL ) {
         if( nCount == 0 ) {
            RECT rcIcon = { 0 };
            RECT rcTitle = { 0 };
            _GetItemPartRect(i, rcIcon, rcTitle);
            pLink->rcItem = rcIcon;
         }
         else {
            int& nSide = pLink->iType == 0 ? nLeft : nRight;
            pLink->rcItem.top = rcLinks.bottom + (nSide * m_Settings.cyLink);
            pLink->rcItem.bottom = rcLinks.bottom + ((nSide + 1) * m_Settings.cyLink - ITEM_LINK_GAP);
            pLink->rcItem.left = pLink->iType == 0 ? rcLinks.left : iMiddle + 1;
            pLink->rcItem.right = pLink->iType == 0 ? iMiddle - 1 : rcLinks.right;
            nSide++;
         }
         nCount++;
         pLink = pLink->pNext;
      }
      // If there are only right-side links, we align them to
      // the left border instead...
      if( nLeft == 0 && nRight > 0 && nCount > 1 ) {
         SCRIPTLINK* pLink = Item.pLinks;
         pLink = pLink->pNext;
         while( pLink ) {
            pLink->rcItem.left = Item.rcItem.left + ICON_SIZE + 3;
            pLink = pLink->pNext;
         }
      }

      Item.rcItem.bottom += max(nLeft, nRight) * m_Settings.cyLink;
   }
}

void CScriptView::_LayoutLinks()
{
   if( (m_dwExtStyle & SC_EX_SIMPLELAYOUT) != 0 )
   {
      // Simple layout algorithm: Just connect the dots...
      for( int i = 0; i < m_aItems.GetSize(); i++ ) {
         SCRIPTITEM& Item = m_aItems[i];
         SCRIPTLINK* pLink = Item.pLinks;
         while( pLink != NULL ) {
            if( pLink->wDestItemID > 0 
                && (m_wDragLinkID == 0 || m_wDragLinkID == pLink->wID) ) 
            {
               SCRIPTITEM* pTargetItem = NULL;
               for( int j = 0; j < m_aItems.GetSize(); j++ ) {
                  if( m_aItems[j].wId == pLink->wDestItemID ) {
                     pTargetItem = &m_aItems[j];
                     break;
                  }
               }
               ATLASSERT(pTargetItem);
               if( pTargetItem == NULL ) return;
               SCRIPTLINK* pTargetLink = pLink->pTargetLink;
               ATLASSERT(pTargetLink->iType==0);
               pLink->nPoints = 2;
               pLink->ptWaypoints[0].x = Item.rcItem.right;
               pLink->ptWaypoints[0].y = pLink->rcItem.top + ((pLink->rcItem.bottom - pLink->rcItem.top) / 2);
               pLink->ptWaypoints[1].x = pTargetItem->rcItem.left;
               pLink->ptWaypoints[1].y = pTargetLink->rcItem.top + ((pTargetLink->rcItem.bottom - pTargetLink->rcItem.top) / 2);
            }
            pLink = pLink->pNext;
         }
      }
   }
   else
   {
      // Get ready to plan new routes
      CRoutePlanner planner;
      planner.Init(CX_VIEW, CY_VIEW, 8);
      // Block all boxes
      int i;
      for( i = 0; i < m_aItems.GetSize(); i++ ) {
         planner.BlockNode(m_aItems[i].rcItem);
      }
      // Plan routes for each of the links...
      for( i = 0; i < m_aItems.GetSize(); i++ ) {
         SCRIPTITEM& Item = m_aItems[i];
         SCRIPTLINK* pLink = Item.pLinks;
         while( pLink != NULL ) {
           
            // Determine if we need to rewire the route:
            //   If it is a dragged link
            //   If it is connected to another node
            if( pLink->wDestItemID > 0 
                && (m_wDragLinkID == 0 || m_wDragLinkID == pLink->wID) ) 
            {
               SCRIPTITEM* pTargetItem = NULL;
               for( int j = 0; j < m_aItems.GetSize(); j++ ) {
                  if( m_aItems[j].wId == pLink->wDestItemID ) {
                     pTargetItem = &m_aItems[j];
                     break;
                  }
               }
               ATLASSERT(pTargetItem);
               if( pTargetItem == NULL ) return;

               SCRIPTLINK* pTargetLink = pLink->pTargetLink;
               ATLASSERT(pTargetLink->iType==0);

               POINT ptSrc;
               ptSrc.x = Item.rcItem.right;
               ptSrc.y = pLink->rcItem.top + ((pLink->rcItem.bottom - pLink->rcItem.top) / 2);
               POINT ptDest;
               ptDest.x = pTargetItem->rcItem.left;
               ptDest.y = pTargetLink->rcItem.top + ((pTargetLink->rcItem.bottom - pTargetLink->rcItem.top) / 2);

               pLink->nPoints = planner.PlanRoute(ptSrc, ptDest, pTargetLink->wID, pLink->ptWaypoints, MAX_WAYPOINTS);

               // No route found? Let's flag it as an
               // error -> node gets only two edges!
               if( pLink->nPoints == 0 ) {
                  pLink->nPoints = 2;
                  pLink->ptWaypoints[0] = ptSrc;
                  pLink->ptWaypoints[1] = ptDest;
               }
            }

            // In any case, future routes are blocked by this route
            if( pLink->wDestItemID > 0 ) {
               planner.BlockRoute(pLink->ptWaypoints, pLink->nPoints, pLink->pTargetLink->wID);
               planner.Dump();
            }

            pLink = pLink->pNext;
         }
      }
   }
}


#if !defined(AFX_TOOLCMDBAR_H__20040820_C21D_1955_C63A_0080AD509054__INCLUDED_)
#define AFX_TOOLCMDBAR_H__20040820_C21D_1955_C63A_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CToolCmdBarHook : public CMessageMap
{
public:
   typedef ATL::CSimpleMap<DWORD, CToolCmdBarHook*> CMsgHookMap;
   static CMsgHookMap* s_mapThreadMap;
   HWND m_hWnd;
   HHOOK m_hMsgHook;
   DWORD m_dwTimestamp;

   CToolCmdBarHook() : m_hMsgHook(NULL), m_hWnd(NULL), m_dwTimestamp(0)
   {
   }
   ~CToolCmdBarHook()
   {
      ATLASSERT(m_hMsgHook==NULL);  // This is bad!! Use Uninstall() before shutdown procedure begins!!
   }

   BOOL Install(HWND hWnd)
   {
      ATLASSERT(::IsWindow(hWnd));
      CWindowCreateCriticalSectionLock lock;
      lock.Lock();
      DWORD dwThreadID = ::GetCurrentThreadId();
      m_hMsgHook = ::SetWindowsHookEx(WH_GETMESSAGE, MessageHookProc, _Module.GetModuleInstance(), dwThreadID);
      ATLASSERT(m_hMsgHook!=NULL);
      if( m_hMsgHook == NULL) {
         lock.Unlock();
         return FALSE;
      }
      if( s_mapThreadMap == NULL ) ATLTRY(s_mapThreadMap = new CMsgHookMap);
      ATLASSERT(s_mapThreadMap->Lookup(dwThreadID)==NULL);
      s_mapThreadMap->Add(::GetCurrentThreadId(), this);
      m_hWnd = hWnd;
      lock.Unlock();
      return TRUE;
   }
   BOOL Uninstall()
   {
      if( m_hMsgHook == NULL ) return TRUE;
      CWindowCreateCriticalSectionLock lock;
      lock.Lock();
      s_mapThreadMap->Remove(::GetCurrentThreadId());
      if( s_mapThreadMap->GetSize() == 0 ) delete s_mapThreadMap;
      BOOL bRet = ::UnhookWindowsHookEx(m_hMsgHook);
      m_hMsgHook = NULL;
      lock.Unlock();
      ATLASSERT(bRet);
      return bRet;
   }

   static LRESULT CALLBACK MessageHookProc(int nCode, WPARAM wParam, LPARAM lParam)
   {
      LPMSG pMsg = (LPMSG) lParam;
      LRESULT lRet = 0;
      DWORD dwThreadID = ::GetCurrentThreadId();
      CToolCmdBarHook* pThis = s_mapThreadMap->Lookup(dwThreadID);
      if( pThis != NULL ) {
         lRet = ::CallNextHookEx(pThis->m_hMsgHook, nCode, wParam, lParam);
         LRESULT lRes = 0;
         pThis->ProcessWindowMessage(pThis->m_hWnd, pMsg->message, pMsg->wParam, pMsg->lParam, lRes, 0);
      }
      return lRet;
   }

   BEGIN_MSG_MAP(CToolCmdBarHook)
      MESSAGE_HANDLER(WM_MOUSEMOVE, OnHookMouseMove)
   END_MSG_MAP()

   LRESULT OnHookMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
   {
      DWORD dwpt = ::GetMessagePos();
      POINT pt = { GET_X_LPARAM(dwpt), GET_Y_LPARAM(dwpt) };
      bHandled = FALSE;
      if( ::WindowFromPoint(pt) == m_hWnd ) {
         CToolBarCtrl tb = m_hWnd;
         tb.ScreenToClient(&pt);
         int nHit = tb.HitTest(&pt);
         if( nHit >= 0 ) {
            int nPressed = 0;
            for( int i = 0; i < tb.GetButtonCount(); i++ ) nPressed += tb.IsButtonPressed(i) ? 1 : 0;
            if( nPressed > 0 ) m_dwTimestamp = ::GetTickCount();
            TBBUTTON tbb = { 0 };
            tb.GetButton(nHit, &tbb);
            if( ::GetTickCount() - m_dwTimestamp < 1000 
                && (tbb.fsState & TBSTATE_ENABLED) != 0 
                && (tbb.fsState & TBSTATE_PRESSED) == 0 )
            {
               // This one is needed to close a menu if mouse button was down
               tb.PostMessage(WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
               // This one closes a popup menu
               tb.PostMessage(WM_KEYDOWN, VK_ESCAPE);
               // This one opens the new popup menu
               tb.PostMessage(WM_KEYDOWN, VK_SPACE, 0L);
               bHandled = TRUE;
            }
         }
      }
      return 0;
   }
};

__declspec(selectany) CToolCmdBarHook::CMsgHookMap* CToolCmdBarHook::s_mapThreadMap = NULL;


#endif // !defined(AFX_TOOLCMDBAR_H__20040820_C21D_1955_C63A_0080AD509054__INCLUDED_)


#if !defined(AFX_WINDOWANIM_H__20040821_41F2_EC95_4446_0080AD509054__INCLUDED_)
#define AFX_WINDOWANIM_H__20040821_41F2_EC95_4446_0080AD509054__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CWindowAnim - Animating frames
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2004 Bjarke Viksoe.
//
// This code may be used in compiled form in any way you desire. This
// source file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//


class CWindowAnim
{
public:
   CWindow m_wnd;

   CWindowAnim(HWND hWnd)
   {
      ATLASSERT(::IsWindow(hWnd));
      m_wnd = hWnd;
   }

   BOOL ZoomIn(RECT rcItem)
   {
      RECT rcWindow;
      m_wnd.GetClientRect(&rcWindow);
      return _DoZoomEffect(rcItem, rcWindow);
   }
   BOOL ZoomOut(RECT rcItem)
   {
      RECT rcWindow;
      m_wnd.GetClientRect(&rcWindow);
      return _DoZoomEffect(rcWindow, rcItem);
   }

   BOOL _DoZoomEffect(RECT rcFrom, RECT rcTo)
   {
      // Get rid of pending GDI updates
      m_wnd.UpdateWindow();

      // No destination point? Let's create a point in the
      // middle of the view...
      if( ::IsRectEmpty(&rcTo) ) {
         rcTo.left = ((rcFrom.right - rcFrom.left) / 2) - 20;
         rcTo.top = ((rcFrom.bottom - rcFrom.top) / 2) - 30;
         rcTo.right = rcFrom.left + 40;
         rcTo.bottom = rcFrom.top + 80;
      }

      // FIX: Border repaint problem!
      rcTo.right--;
      rcTo.bottom--;
      rcFrom.right--;
      rcFrom.bottom--;

      int iStep = 1;
      float fSpeed = 1.0f;
      float fSign = rcFrom.left < rcTo.left ? -1.0f : 1.0f;
      DWORD dwDelay = 20;

      RECT rcBox[5] = { 0 };

      // Lock the window DC som no one else can paint on it...
      CDCHandle dc = ::GetDCEx(m_wnd, NULL, ::LockWindowUpdate(m_wnd) ? DCX_WINDOW | DCX_CACHE | DCX_LOCKWINDOWUPDATE : DCX_WINDOW | DCX_CACHE);
      if( dc.IsNull() ) return FALSE;

      CPen pen;
      pen.CreatePen(PS_SOLID, 1, RGB(0,0,0));

      int iOldROP = dc.SetROP2(R2_NOT);
      HPEN hOldPen = dc.SelectPen(pen);
      HBRUSH hOldBrush = dc.SelectBrush( (HBRUSH) ::GetStockObject(NULL_BRUSH) );

      rcBox[0] = rcFrom;
      _DrawRects(dc.m_hDC, rcBox, sizeof(rcBox) / sizeof(RECT), iStep);

      while( !::EqualRect(&rcBox[0], &rcTo) ) 
      {
         DWORD dwTick = ::GetTickCount();

         _DrawRects(dc.m_hDC, rcBox, sizeof(rcBox) / sizeof(RECT), iStep);

         // Animate boxes
         ::MoveMemory(&rcBox[1], &rcBox[0], sizeof(rcBox) - sizeof(RECT));

         // Move leading box
         rcBox[0].left   -= (long)( fSpeed * fSign );
         rcBox[0].top    -= (long)( fSpeed * fSign );
         rcBox[0].right  += (long)( fSpeed * fSign );
         rcBox[0].bottom += (long)( fSpeed * fSign );

         // Don't go outside boundries...
         if( fSign > 0.0 ) {
            if( rcBox[0].left   < rcTo.left )   rcBox[0].left = rcTo.left;
            if( rcBox[0].top    < rcTo.top )    rcBox[0].top = rcTo.top;
            if( rcBox[0].right  > rcTo.right )  rcBox[0].right = rcTo.right;
            if( rcBox[0].bottom > rcTo.bottom ) rcBox[0].bottom = rcTo.bottom;
         }
         else {
            if( rcBox[0].left   > rcTo.left )   rcBox[0].left = rcTo.left;
            if( rcBox[0].top    > rcTo.top )    rcBox[0].top = rcTo.top;
            if( rcBox[0].right  < rcTo.right )  rcBox[0].right = rcTo.right;
            if( rcBox[0].bottom < rcTo.bottom ) rcBox[0].bottom = rcTo.bottom;
         }
         
         // Speed up animation
         fSpeed = fSpeed * 1.2f;
        
         _DrawRects(dc.m_hDC, rcBox, sizeof(rcBox) / sizeof(RECT), iStep);

         ::GdiFlush();
         
         while( ::GetTickCount() - dwTick < dwDelay ) ::Sleep(0);
      }

      _DrawRects(dc.m_hDC, rcBox, sizeof(rcBox) / sizeof(RECT), iStep);

      dc.SetROP2(iOldROP);
      dc.SelectPen(hOldPen);
      dc.SelectBrush(hOldBrush);

      ::ReleaseDC(m_wnd, dc.Detach());

      ::LockWindowUpdate(NULL);

      return m_wnd.Invalidate();
   }
   void _DrawRects(CDCHandle dc, LPRECT prc, int nRects, int iStep)
   {
      for( int i = 0; i < nRects; i += iStep) {
         if( ::IsRectEmpty(&prc[i]) ) continue;
         POINT pts[4] =
         {
            { prc[i].left, prc[i].top },
            { prc[i].right, prc[i].top },
            { prc[i].right, prc[i].bottom },
            { prc[i].left, prc[i].bottom },
         };
         dc.Polygon(pts, 4);
      }
   }
};


#endif // !defined(AFX_WINDOWANIM_H__20040821_41F2_EC95_4446_0080AD509054__INCLUDED_)


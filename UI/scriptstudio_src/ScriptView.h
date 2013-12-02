#if !defined(AFX_SCRIPTVIEW_H__20040527_8FCD_7045_816A_0080AD509054__INCLUDED_)
#define AFX_SCRIPTVIEW_H__20040527_8FCD_7045_816A_0080AD509054__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// Script Control - Simulating the Envox Script control
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


// Control notifications
enum
{
   SCN_FIRST = (0U-100U),
   SCN_DELETEITEM = SCN_FIRST,
   SCN_INSERTITEM,
   SCN_BEGINDRAG,
   SCN_BEGINLABELEDIT,
   SCN_DELETEALLITEMS,
   SCN_ITEMCHANGING,
   SCN_ITEMCHANGED,
   SCN_ITEMACTIVATE,
   SCN_ITEMMOVED,
   SCN_GETINFOTIP,
   SCN_KEYDOWN,
   SCN_CONNECTITEMS,
   SCN_VALIDATECONNECTION,
   SCN_INSERTLINK,
   SCN_DELETELINK,
};

// Extended control styles
enum
{
   SC_EX_TITLEBAR         = 0x00000001,
   SC_EX_SHAREIMAGELIST   = 0x00000002,
   SC_EX_SIMPLELAYOUT     = 0x00000004,
};

// Hit-test constants
enum
{
   SCHT_NOWHERE = 0,
   SCHT_ONTITLE,
   SCHT_ONITEMICON,
   SCHT_ONITEMLINK,
   SCHT_ONCONNECTOR,
};

// State constants
enum
{
   SCIS_ALL          = 0x00000000,
   SCIS_SELECTED     = 0x00000001,
   SCIS_RIGHTALIGN   = 0x00000002,
   SCIS_TEMPORARY    = 0x00000004,
   SCIS_HIGHTLIGHTED = 0x00000008,
};

// General notify structure
typedef struct
{
   NMHDR hdr;
   int iItem;
   int iSubItem;
   POINT ptAction;
   LPARAM lParam;
   UINT uOldState;
   UINT uNewState;
} NMSCRIPTVIEW;

// Hit-test notify structure
typedef struct
{
   POINT pt;
   UINT flags;
   int iItem;
   int iSubItem;
   int iMaxDistance2;
} SCHITTESTINFO;

// Keydown notify structure
typedef struct
{
   NMHDR hdr;
   WORD wVKey;
   UINT flags;
} NMSCVKEYDOWN;


class CScriptView : 
   public CWindowImpl<CScriptView>,
   public COffscreenDraw<CScriptView>
{
public:
   DECLARE_WND_CLASS(_T("ScriptStudio_BlockScript"));

   enum
   { 
      ICON_SIZE = 16,                   // Size of item icons
      ITEM_GRID_SIZE = 16,              // Grid resolution in pixels
      ITEM_CAPTION_GAP = 5,             // Gap between caption and links
      ITEM_LINK_GAP = 3,                // Gap between links
      ITEM_SMALL_MINSIZE = 64,          // Width of smallest buttons (Start, End)
      ITEM_NORMAL_MINSIZE = 96,         // Width of standard buttons
      ITEM_BIG_MINSIZE = 96 + 32,       // Width of largest buttons (UserBlock)
      CX_VIEW = 1024,                   // Width of view
      CY_VIEW = 1024,                   // Height of view
   };

   // Structures

   enum { MAX_WAYPOINTS = 9 };

   typedef struct tagSCRIPTLINK
   {
      WORD wID;                         // Local ID for link
      TCHAR szName[64];                 // The name of the Output link
      int iType;                        // Link type (0=input, 1=output)
      WORD wDestItemID;                 // Target item ID
      tagSCRIPTLINK* pTargetLink;       // Target (input) link reference
      LPARAM lParam;                    // Optional parameter
      COLORREF clrText;                 // Color of link text
      COLORREF clrLine;                 // Color of link line
      UINT uStyle;                      // Link style
      RECT rcItem;                      // Link text Rectangle
      int nPoints;                      // Number of waypoints used
      POINT ptWaypoints[MAX_WAYPOINTS]; // Waypoints
      tagSCRIPTLINK* pNext;             // Next link in list
   } SCRIPTLINK;

   typedef struct tagSCRIPTITEM
   {
      WORD wId;                         // Local ID for item
      TCHAR szName[64];                 // The name of the item
      int iImage;                       // Item icon
      LPARAM lParam;                    // Optional parameter
      COLORREF clrText;                 // Color of title text
      COLORREF clrBack;                 // Color of title background
      SCRIPTLINK* pLinks;               // Links
      UINT uState;                      // Item state
      UINT uOrder;                      // Display order
      RECT rcItem;                      // Item rectangle
      int cxWidth;                      // Item width
   } SCRIPTITEM;

   // Operations

   void SetNotifyParent(HWND hWnd);
   DWORD SetExtendedScriptStyle(DWORD dwStyle);
   CImageList SetImageList(HIMAGELIST hImageList);

   BOOL SetTitleText(LPCTSTR pstrTitle);
   BOOL SetTitleColors(COLORREF clrText, COLORREF clrBack);

   void DeleteAllItems();
   BOOL DeleteItem(int iIndex);

   int InsertItem(LPCTSTR pstrName, POINT ptPos, int iImage, LPARAM lParam = 0);
   BOOL GetItemText(int iIndex, LPTSTR pstrName, int cchMax) const;
   LPARAM GetItemData(int iIndex) const;
   POINT GetItemPos(int iIndex) const;
   RECT GetItemRect(int iIndex) const;
   UINT GetItemState(int iIndex) const;
   BOOL SetItemPos(int iIndex, POINT pt);
   BOOL SetItemState(int iIndex, UINT state, UINT mask);
   BOOL SetItemText(int iIndex, LPCTSTR pstrName);
   BOOL SetItemWidth(int iIndex, int cxWidth);
   BOOL SetItemColor(int iIndex, COLORREF clrText);
   BOOL SetItemData(int iIndex, LPARAM lParam);
   
   BOOL DeleteAllLinks(int iIndex);

   int InsertItemLink(int iIndex, LPCTSTR pstrText, int iType = 0, LPARAM lParam = 0);
   int GetItemLinkType(int iIndex, int iSubItem) const;
   BOOL GetItemLinkName(int iIndex, int iSubItem, LPSTR pstrName, int cchMax) const;
   int GetItemLinkCount(int iIndex, int iType = -1) const;
   LPARAM GetItemLinkData(int iIndex, int iSubItem) const;
   BOOL GetItemLinkTarget(int iIndex, int iSubItem, int& iTarget, int& iTargetSubItem) const;

   int FindLink(int iIndex, LPCTSTR pstrName) const;
   int FindItem(LPARAM lParam, int iStart = 0) const;
   BOOL SearchLink(LPARAM lParam, int& iIndex, int& iSubItem) const;
   BOOL FindClosestItem(int iIndex, SCHITTESTINFO* pHitInfo) const;
   BOOL ConnectItem(int iFromIndex, int iFromSubItem, int iToIndex, int iToSubItem, UINT uStyle = 0);
   BOOL DisconnectItem(int iIndex, int iSubItem);

   int GetItemCount() const;
   POINT GetScrollPos() const;
   BOOL SelectItem(int iIndex);
   int GetSelectedCount() const;
   int GetNextItem(int iItem, UINT uState = SCIS_ALL) const;

   BOOL EnsureVisible(int iIndex);
   BOOL UpdateLayout(BOOL bRedraw = TRUE);
   int HitTest(SCHITTESTINFO* pHitInfo) const;

   // Overrides

   void DoPaint(CDCHandle dc);

   // Implementation

   void _Init();
   SCRIPTLINK* _GetLink(int iIndex, int iSubItem) const;
   LRESULT _SendNotifyMessage(UINT code, LPVOID p = NULL);
   BOOL _DeleteItem(int iIndex, bool bClearLinks = true, bool bNotify = true);
   
   void _LayoutButtons();
   void _LayoutLinks();
   void _UIUpdate();
   RECT _NormalizeRect(RECT rc) const;
   void _DragScrollView(LPARAM lParam);
   void _DrawDragRect(POINT ptOffset) const;
   void _DrawRubberBand(RECT rcFrame) const;
   CBrushHandle _GetHalfHalftoneBrush() const;
   void _GetDisplayOrder(CSimpleArray<int>& aOrder) const;
   void _GetItemPartRect(int iIndex, RECT& rcIcon, RECT& rcTitle) const;

   void _DoPaintLinks(CDCHandle dc, RECT rcClip, CSimpleArray<int>& aOrder);
   void _DoPaintSpecialButtons(CDCHandle dc, RECT rcClip, CSimpleArray<int>& aOrder);
   void _DoPaintNormalButtons(CDCHandle dc, RECT rcClip, CSimpleArray<int>& aOrder);
   void _DoPaintMarkers(CDCHandle dc, RECT rcClip, CSimpleArray<int>& aOrder);

   // Data Members

   CWindow m_wndNotify;                // Window to send notifications to
   DWORD m_dwExtStyle;                 // Extended control styles
   CImageList m_Images;                // Image List
   TCHAR m_szTitle[256];               // Window Title (currently not used)
   CSimpleArray<SCRIPTITEM> m_aItems;  // Collection of items
   POINT m_ptOffset;                   // Scroll position
   WORD m_wDragLinkID;                 // ID of link when dragging to new position
   int m_iDragSubItem;                 // Input/output panel in drag'n'drop
   bool m_bRecalcLayout;               // Control need layout calculation?
   int m_iHightlightItem;              // Mouse hover tracking active?
   int m_iHightlightSubItem;           // -- " --
   POINT m_ptDragStart;                // Point where drag'n'drop started
   POINT m_ptDragOffset;               // Distance where drag'n'drop finsihed
   RECT m_rcRubber;                    // Rectangle of selection rubberband
   WORD m_wItemID;                     // Unqiue item ID
   WORD m_wLinkID;                     // Unqiue link ID
   UINT m_uMaxOrder;                   // Sort order counter
   enum
   {
      CAPTURE_NONE,                    // We're not capturing mouse
      CAPTURE_MOVEITEM,                // We're moving items around
      CAPTURE_CREATELINK,              // We're connecting a link
      CAPTURE_RUBBERBAND,              // We're doing rubberband selection
   } m_CaptureState;

   struct SETTINGS
   {
      CFont fontCaption;               // Font of box caption text
      CFont fontText;                  // Font of normal box text
      TEXTMETRIC tmText;               // Text metrics for normal text
      TEXTMETRIC tmCaption;            // Text metrics for caption text
      int cxyBorders;                  // Size of box border
      int cyCaption;                   // Height of caption
      int cyLink;                      // Height of link panel
      COLORREF clrBack;                // Box background color
      COLORREF clrText;                // ?
      COLORREF clrBtnText;             // Box text color
      COLORREF clrDarkBorder;          // Input panel 3d color
      COLORREF clrLightBorder;         // Input panel 3d color
      COLORREF clrInactiveCaption;     // Inactive caption background color
      COLORREF clrActiveCaption;       // Active caption background color
      COLORREF clrActiveCaptionText;   // Active caption text color
      COLORREF clrInactiveCaptionText; // Inactive caption text color
      COLORREF clrTitleText;           // Color of item text
      COLORREF clrTitleBack;           // Color of item background
      COLORREF clrLink;                // Color of links
   } m_Settings;

   // Message map and handler

   BEGIN_MSG_MAP(CScriptView)
      CHAIN_MSG_MAP( COffscreenDraw<CScriptView> )
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      MESSAGE_HANDLER(WM_SIZE, OnSize)
      MESSAGE_HANDLER(WM_HSCROLL, OnScroll)
      MESSAGE_HANDLER(WM_VSCROLL, OnScroll)
      MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
      MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
      MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
      MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
      MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
      MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
      MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
      MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
   END_MSG_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnScroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnCaptureChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};


#endif // !defined(AFX_SCRIPTVIEW_H__20040527_8FCD_7045_816A_0080AD509054__INCLUDED_)

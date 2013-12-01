#if !defined(AFX_EXPLORERVIEW_H__20040527_9D86_6DD6_0483_0080AD509054__INCLUDED_)
#define AFX_EXPLORERVIEW_H__20040527_9D86_6DD6_0483_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CExplorerView : public CWindowImpl<CExplorerView, CTreeViewCtrl>
{
public:
   DECLARE_WND_SUPERCLASS(_T("ScriptStudio_Explorer"), CTreeViewCtrl::GetWndClassName())

   BOOL Select(CScriptBlock* pBlock);
   BOOL Populate(CScriptBlock* pRoot, CScriptBlock* pBlock);
   BOOL RemoveItem(CScriptBlock* pBlock);

   BOOL PreTranslateMessage(MSG* pMsg);

   BOOL _PopulateTree(CScriptBlock* pBlock, HTREEITEM hParent);   
   BOOL _FindItem(HTREEITEM hItem, CScriptBlock* pBlock, HTREEITEM& hMatch);
   static int CALLBACK _SortTreeCB(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

   BEGIN_MSG_MAP(CExplorerView)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
   END_MSG_MAP()

   LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};


#endif // !defined(AFX_EXPLORERVIEW_H__20040527_9D86_6DD6_0483_0080AD509054__INCLUDED_)


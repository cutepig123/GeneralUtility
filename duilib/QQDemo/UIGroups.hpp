#ifndef UIGROUPS_HPP
#define UIGROUPS_HPP

#include "UIListCommonDefine.hpp"

namespace DuiLib
{
struct GroupsListItemInfo
{
	bool folder;
	bool empty;
	tString id;
	tString logo;
	tString nick_name;
	tString description;
};

class CGroupsUI : public CListUI
{
public:
	enum {SCROLL_TIMERID = 10};

	CGroupsUI(CPaintManagerUI& paint_manager);

	~CGroupsUI();

	bool Add(CControlUI* pControl);

	bool AddAt(CControlUI* pControl, int iIndex);

	bool Remove(CControlUI* pControl);

	bool RemoveAt(int iIndex);

	void RemoveAll();

	void DoEvent(TEventUI& event);

	Node* GetRoot();

	Node* AddNode(const GroupsListItemInfo& item, Node* parent = NULL);

	void RemoveNode(Node* node);

	void SetChildVisible(Node* node, bool visible);

	bool CanExpand(Node* node) const;

private:
	Node*	root_node_;
	LONG	delay_deltaY_;
	DWORD	delay_number_;
	DWORD	delay_left_;
	CRect	text_padding_;
	int level_text_start_pos_;
	tString level_expand_image_;
	tString level_collapse_image_;
	CPaintManagerUI& paint_manager_;

    CDialogBuilder m_dlgBuilder;
};

} // DuiLib

#endif // UIGROUPS_HPP
#pragma once
#include "UIProperties.h"
#include "afxwin.h"

// CDialogUIAttribEdit �Ի���

class CDialogUIAttribEdit : public CDialog
{
	DECLARE_DYNAMIC(CDialogUIAttribEdit)

public:
	CDialogUIAttribEdit(CWnd* pParent = NULL);   // ��׼���캯��
	CDialogUIAttribEdit(CControlUI* pControl);
	virtual ~CDialogUIAttribEdit();

// �Ի�������
	enum { IDD = IDD_DIALOG_UI_ATTRIB_EDIT };

private:
	CUIProperties m_wndUIProperties;
	CControlUI* m_pControl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

protected:
	void SetUIValue(CMFCPropertyGridProperty* pProp,int nTag);

protected:
	virtual BOOL OnInitDialog();

private:
	CStatic m_wndUIPropLocation;

protected:
	afx_msg LRESULT OnUIPropChanged(WPARAM wp, LPARAM lp);
};

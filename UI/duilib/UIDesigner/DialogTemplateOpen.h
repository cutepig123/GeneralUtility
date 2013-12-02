#pragma once
#include "afxwin.h"
#include "UIImagePreview.h"

// CDialogTemplateOpen �Ի���

class CDialogTemplateOpen : public CDialog
{
	DECLARE_DYNAMIC(CDialogTemplateOpen)

public:
	CDialogTemplateOpen(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogTemplateOpen();

// �Ի�������
	enum { IDD = IDD_DIALOG_TEMPLATE_OPEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CListBox m_lstTemplates;
	CUIImagePreview m_TemplatePreview;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangeListTemplates();
	afx_msg void OnBnClickedButtonTemplateDelete();
	afx_msg void OnBnClickedOk();
};

#pragma once
#include "resource.h"

// CDialogProjectNew �Ի���

class CDialogProjectNew : public CDialog
{
	DECLARE_DYNAMIC(CDialogProjectNew)

public:
	CDialogProjectNew(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogProjectNew();

// �Ի�������
	enum { IDD = IDD_PROJECT_NEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	CString m_strPath;
	CString m_strName;
	afx_msg void OnBnClickedButtonBrowse();
	static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData);
	afx_msg void OnBnClickedOk();
};

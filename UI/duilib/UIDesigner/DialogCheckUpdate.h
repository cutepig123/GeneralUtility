#pragma once


// CDialogCheckUpdate �Ի���

class CDialogCheckUpdate : public CDialog
{
	DECLARE_DYNAMIC(CDialogCheckUpdate)

public:
	CDialogCheckUpdate(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogCheckUpdate();

// �Ի�������
	enum { IDD = IDD_DIALOG_CHECK_UPDATE };

protected:
	CString GetLatestVersionInfo(LPCTSTR pstrURL);

private:
	CMFCLinkCtrl m_btnUpdateURL;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

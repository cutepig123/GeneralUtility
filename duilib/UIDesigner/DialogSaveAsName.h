#pragma once

// CDialogSaveAsName �Ի���

class CDialogSaveAsName : public CDialog
{
	DECLARE_DYNAMIC(CDialogSaveAsName)

public:
	CDialogSaveAsName(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogSaveAsName();

// �Ի�������
	enum { IDD = IDD_DIALOG__SAVE_AS_NAME };

public:
	CString GetSaveAsName() const { return m_strName; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CString m_strName;
};

// DialogSaveAsName.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UIDesigner.h"
#include "DialogSaveAsName.h"


// CDialogSaveAsName �Ի���

IMPLEMENT_DYNAMIC(CDialogSaveAsName, CDialog)

CDialogSaveAsName::CDialogSaveAsName(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogSaveAsName::IDD, pParent)
	, m_strName(_T("δ����1"))
{

}

CDialogSaveAsName::~CDialogSaveAsName()
{
}

void CDialogSaveAsName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TEMPLATE_NAME, m_strName);
}


BEGIN_MESSAGE_MAP(CDialogSaveAsName, CDialog)
END_MESSAGE_MAP()


// CDialogSaveAsName ��Ϣ�������

// DialogProjectNew.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UIDesigner.h"
#include "DialogProjectNew.h"


// CDialogProjectNew �Ի���

IMPLEMENT_DYNAMIC(CDialogProjectNew, CDialog)

CDialogProjectNew::CDialogProjectNew(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogProjectNew::IDD, pParent)
	, m_strPath(_T(""))
	, m_strName(_T(""))
{

}

CDialogProjectNew::~CDialogProjectNew()
{
}

void CDialogProjectNew::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PROJECT_PATH, m_strPath);
	DDX_Text(pDX, IDC_EDIT_PROJECT_NAME, m_strName);
}


BEGIN_MESSAGE_MAP(CDialogProjectNew, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CDialogProjectNew::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDOK, &CDialogProjectNew::OnBnClickedOk)
END_MESSAGE_MAP()


// CDialogProjectNew ��Ϣ�������

void CDialogProjectNew::OnBnClickedButtonBrowse()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	BROWSEINFO info;
	TCHAR szDefaultDir[MAX_PATH]={0};
	_tcscpy(szDefaultDir,m_strPath);
	ZeroMemory(&info, sizeof(BROWSEINFO));
	info.hwndOwner = this->GetSafeHwnd();
	info.lpszTitle = _T("��ѡ��һ���ļ���:");
	info.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	info.lpfn = BrowseCallbackProc;
	info.lParam = long(&szDefaultDir);
	ITEMIDLIST *pItem;
	pItem = SHBrowseForFolder(&info);
	if(pItem)
	{  
		TCHAR szPath[MAX_PATH]={0};
		SHGetPathFromIDList(pItem, szPath);
		GlobalFree(pItem);
		m_strPath = szPath;

		UpdateData(FALSE);
	}
}

int CALLBACK CDialogProjectNew::BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
{
	switch(uMsg)
	{
	case BFFM_INITIALIZED:
		{
			::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
			break;
		}
	case BFFM_SELCHANGED:
		{
			TCHAR curr[MAX_PATH];   
			SHGetPathFromIDList((LPCITEMIDLIST)lParam, curr);   
			::SendMessage(hwnd, BFFM_SETSTATUSTEXT,0, (LPARAM)curr);   
		}
		break;
	default:
		break;
	}

	return 0;   
}

void CDialogProjectNew::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);

	if(m_strName.IsEmpty())
	{
		MessageBox(_T("��Ŀ���Ʋ�������Ϊ�ա�"),_T("��ʾ"));
		return;
	}
	if(m_strPath.IsEmpty())
	{
		MessageBox(_T("·�����Ʋ�������Ϊ�ա�"),_T("��ʾ"));
		return;
	}
	if(!PathIsDirectory(m_strPath))
	{
		MessageBox(_T("����·���Ѳ����ڣ����ܱ��Ƴ���"),_T("��ʾ"));
		return;
	}
	if(m_strPath.Right(1) != _T("\\"))
		m_strPath += _T("\\");

	UpdateData(FALSE);
	OnOK();
}

// DialogTemplateOpen.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UIDesigner.h"
#include "DialogTemplateOpen.h"


// CDialogTemplateOpen �Ի���

IMPLEMENT_DYNAMIC(CDialogTemplateOpen, CDialog)

CDialogTemplateOpen::CDialogTemplateOpen(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogTemplateOpen::IDD, pParent)
{

}

CDialogTemplateOpen::~CDialogTemplateOpen()
{
}

void CDialogTemplateOpen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TEMPLATES, m_lstTemplates);
	DDX_Control(pDX, IDC_STATIC_TEMPLATE_PREVIEW, m_TemplatePreview);
}


BEGIN_MESSAGE_MAP(CDialogTemplateOpen, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_TEMPLATES, &CDialogTemplateOpen::OnLbnSelchangeListTemplates)
	ON_BN_CLICKED(IDC_BUTTON_TEMPLATE_DELETE, &CDialogTemplateOpen::OnBnClickedButtonTemplateDelete)
	ON_BN_CLICKED(IDOK, &CDialogTemplateOpen::OnBnClickedOk)
END_MESSAGE_MAP()


// CDialogTemplateOpen ��Ϣ�������

BOOL CDialogTemplateOpen::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	WIN32_FIND_DATA FindFileData = {0};
	CString strFind = CGlobalVariable::GetTemplatesDir() + _T("*.*");
	HANDLE hFind = ::FindFirstFile(strFind, &FindFileData);
	if(hFind == INVALID_HANDLE_VALUE)
		return TRUE;
	do
	{
		if((FindFileData.dwFileAttributes | FILE_ATTRIBUTE_DIRECTORY) != 0)
		{
			if(FindFileData.cFileName[0] == '.')
				if (FindFileData.cFileName[1] == '\0' ||
					(FindFileData.cFileName[1] == '.' &&
					FindFileData.cFileName[2] == '\0'))
					continue;

			m_lstTemplates.AddString(FindFileData.cFileName);
		}
	}while(::FindNextFile(hFind, &FindFileData));
	::FindClose(hFind);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDialogTemplateOpen::OnLbnSelchangeListTemplates()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strText;
	int nIndex = m_lstTemplates.GetCurSel();
	if(nIndex == -1)
		return;

	m_lstTemplates.GetText(nIndex, strText);

	m_TemplatePreview.SetPreviewImage(CGlobalVariable::GetTemplatesDir() + strText
		+ _T("\\template.ui"));
}

void CDialogTemplateOpen::OnBnClickedButtonTemplateDelete()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strText;
	int nIndex = m_lstTemplates.GetCurSel();
	if(nIndex == -1)
		return;
	m_lstTemplates.GetText(nIndex, strText);

	if(strText.IsEmpty())
		return;

	CString strDelete = CGlobalVariable::GetTemplatesDir() + strText;
	TCHAR* pstrDelete;
	pstrDelete = strDelete.GetBuffer(strDelete.GetLength() + 1);
	pstrDelete[_tcslen(pstrDelete) + 1] = 0;
	SHFILEOPSTRUCT FileOp;
	FileOp.hwnd = NULL;
	FileOp.pFrom = pstrDelete;
	FileOp.wFunc = FO_DELETE;
	FileOp.pTo = NULL;
	FileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION;
	if(SHFileOperation(&FileOp) == 0)
	{
		m_lstTemplates.DeleteString(nIndex);
		m_TemplatePreview.SetPreviewImage(_T(""));
	}
}

void CDialogTemplateOpen::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strText;
	int nIndex = m_lstTemplates.GetCurSel();
	if(nIndex != -1)
	{
		m_lstTemplates.GetText(nIndex, strText);

		if(!strText.IsEmpty())
		{
			CDocument* pDoc = AfxGetApp()->OpenDocumentFile(CGlobalVariable::GetTemplatesDir() + strText
				+ _T("\\template.xml"));
			theApp.RemoveLastFromMRU();
			pDoc->SetTitle(strText);
			pDoc->SetPathName(_T(""), FALSE);
		}
	}

	OnOK();
}

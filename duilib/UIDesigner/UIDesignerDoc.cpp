
// UIDesignerDoc.cpp : CUIDesignerDoc ���ʵ��
//

#include "stdafx.h"
#include "UIDesigner.h"

#include "UIDesignerDoc.h"
#include "DialogSkinFileNew.h"
#include "UIUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUIDesignerDoc

IMPLEMENT_DYNCREATE(CUIDesignerDoc, CDocument)

BEGIN_MESSAGE_MAP(CUIDesignerDoc, CDocument)
	ON_COMMAND(ID_MDITABS_COPY_FULLPATH, &CUIDesignerDoc::OnMdiCopyFullPath)
	ON_COMMAND(ID_MDITABS_OPEN_FULLPATH, &CUIDesignerDoc::OnMdiOpenFullPath)
END_MESSAGE_MAP()


// CUIDesignerDoc ����/����

CUIDesignerDoc::CUIDesignerDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CUIDesignerDoc::~CUIDesignerDoc()
{
}

BOOL CUIDesignerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)
	CDialogSkinFileNew dlg;
	dlg.m_strUITitle = this->GetTitle();
	if(dlg.DoModal() == IDOK)
	{
		CString strFilePath = dlg.GetStyleFilePath();
		if(!strFilePath.IsEmpty())
			this->SetPathName(strFilePath);
		if(!dlg.m_strUITitle.IsEmpty())
			this->SetTitle(dlg.m_strUITitle);
	}
	else
		return FALSE;

	return TRUE;
}




// CUIDesignerDoc ���л�

void CUIDesignerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CUIDesignerDoc ���

#ifdef _DEBUG
void CUIDesignerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CUIDesignerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CUIDesignerDoc ����

BOOL CUIDesignerDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: �ڴ����ר�ô����/����û���
	POSITION pos = this->GetFirstViewPosition();
	while(pos)
	{
		CView* pView = this->GetNextView(pos);
		CUIDesignerView* pUIView = DYNAMIC_DOWNCAST(CUIDesignerView, pView);
		ASSERT(pUIView);
		pUIView->SaveSkinFile(lpszPathName);
		this->SetModifiedFlag(FALSE);
	}

	return TRUE;/*CDocument::OnSaveDocument(lpszPathName);*/
}

void CUIDesignerDoc::SetTitle(LPCTSTR lpszTitle)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(m_strTitle != lpszTitle)
	{
		POSITION pos = this->GetFirstViewPosition();
		while(pos)
		{
			CView* pView = this->GetNextView(pos);
			CUIDesignerView* pUIView = DYNAMIC_DOWNCAST(CUIDesignerView, pView);
			ASSERT(pUIView);
			pUIView->OnTitleChanged(m_strTitle, lpszTitle);
		}
	}

	CDocument::SetTitle(lpszTitle);
}

void CUIDesignerDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(*lpszPathName == _T('\0'))
		m_strPathName.Empty();
	else
		CDocument::SetPathName(lpszPathName, bAddToMRU);
}

void CUIDesignerDoc::OnMdiCopyFullPath()
{
	if(m_strPathName.IsEmpty())
	{
		MessageBox(NULL, _T("���ȱ��浱ǰ�ļ���"), _T("��ʾ"), MB_ICONINFORMATION);
		return;
	}

	HGLOBAL hClip;
	if(OpenClipboard(NULL))
	{
		EmptyClipboard();
		CStringA strFullPath = StringConvertor::WideToAnsi(m_strPathName);
		hClip = GlobalAlloc(GMEM_MOVEABLE, strFullPath.GetLength() + 1);
		char* pbufCopy;
		pbufCopy = (char*)GlobalLock(hClip);
		strcpy(pbufCopy, strFullPath);
		GlobalUnlock(hClip);
		SetClipboardData(CF_TEXT, hClip);
		CloseClipboard();
	}
}

void CUIDesignerDoc::OnMdiOpenFullPath()
{
	if(m_strPathName.IsEmpty())
	{
		MessageBox(NULL, _T("���ȱ��浱ǰ�ļ���"), _T("��ʾ"), MB_ICONINFORMATION);
		return;
	}

	int nPos = m_strPathName.ReverseFind(_T('\\'));
	if(nPos == -1)
		return;
	CString strDir = m_strPathName.Left(nPos + 1);
	ShellExecute(NULL, _T("open"), strDir, NULL, NULL, SW_SHOW);
}

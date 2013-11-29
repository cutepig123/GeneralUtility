// UIImagePreview.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UIDesigner.h"
#include "UIImagePreview.h"


// CUIImagePreview

IMPLEMENT_DYNAMIC(CUIImagePreview, CStatic)

CUIImagePreview::CUIImagePreview()
{

}

CUIImagePreview::~CUIImagePreview()
{
}


BEGIN_MESSAGE_MAP(CUIImagePreview, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CUIImagePreview ��Ϣ�������

void CUIImagePreview::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CStatic::OnPaint()
	CMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();

	CRect rectClient;
	CRgn rgnClip;
	this->GetClientRect(&rectClient);
	rgnClip.CreateRectRgnIndirect(&rectClient);
	pDC->SelectClipRgn(&rgnClip);
	pDC->FillRect(&rectClient, &afxGlobalData.brBtnFace);
	if(!m_imgPreview.IsNull())
		m_imgPreview.BitBlt(pDC->GetSafeHdc(), 0, 0);
}

void CUIImagePreview::SetPreviewImage(LPCTSTR pstrImage)
{
	if(!m_imgPreview.IsNull())
		m_imgPreview.Destroy();

	if(*pstrImage != _T('\0'))
		m_imgPreview.Load(pstrImage);
	this->Invalidate(TRUE);
}
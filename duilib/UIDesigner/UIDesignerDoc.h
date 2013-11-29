
// UIDesignerDoc.h : CUIDesignerDoc ��Ľӿ�
//


#pragma once


class CUIDesignerDoc : public CDocument
{
protected: // �������л�����
	CUIDesignerDoc();
	DECLARE_DYNCREATE(CUIDesignerDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CUIDesignerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	afx_msg void OnMdiCopyFullPath();
	afx_msg void OnMdiOpenFullPath();

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
};



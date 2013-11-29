
// UIDesigner.h : UIDesigner Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CUIDesignerApp:
// �йش����ʵ�֣������ UIDesigner.cpp
//

class CUIDesignerApp : public CWinAppEx
{
public:
	CUIDesignerApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	CMultiDocTemplate* m_pUIDocTemplate;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	void RemoveLastFromMRU();
	CMultiDocTemplate* GetUIDocTemplate() const { return m_pUIDocTemplate; }

	afx_msg void OnFileNew();
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CUIDesignerApp theApp;

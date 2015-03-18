#pragma once


// CMyBtn

class CMyBtn : public CButton
{
	DECLARE_DYNAMIC(CMyBtn)

public:
	CMyBtn();
	virtual ~CMyBtn();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};



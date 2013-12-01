#if !defined(AFX_BLOCKVARIABLESET_H__20040807_56DE_E8E6_FE09_0080AD509054__INCLUDED_)
#define AFX_BLOCKVARIABLESET_H__20040807_56DE_E8E6_FE09_0080AD509054__INCLUDED_

#pragma once


class CBlockVariableSet : public CScriptBlock
{
public:
   CBlockVariableSet();

   void Init();
   WORD GetType() const;
   DWORD GetStyle() const;
   int GetImage() const;
   BOOL AddWizardPages(IWizardSheet* pSheet);
   BOOL Validate(IValidator* pValidator);
   BOOL Compile(IBuilder* pBuilder);
   BOOL Read(ISerializer* pArc);
   BOOL Write(ISerializer* pArc);

   TCHAR m_szVariable[64];
   TCHAR m_szExpression[100];
};


#endif // !defined(AFX_BLOCKVARIABLESET_H__20040807_56DE_E8E6_FE09_0080AD509054__INCLUDED_)


#if !defined(AFX_BLOCKVARIABLECHOOSE_H__20040807_642F_FDDA_A874_0080AD509054__INCLUDED_)
#define AFX_BLOCKVARIABLECHOOSE_H__20040807_642F_FDDA_A874_0080AD509054__INCLUDED_

#pragma once


class CBlockVariableChoose : public CScriptBlock
{
public:
   CBlockVariableChoose();

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
};


#endif // !defined(AFX_BLOCKVARIABLECHOOSE_H__20040807_642F_FDDA_A874_0080AD509054__INCLUDED_)


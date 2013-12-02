#if !defined(AFX_DELAYBLOCK_H__20040807_BECF_6DFB_D813_0080AD509054__INCLUDED_)
#define AFX_DELAYBLOCK_H__20040807_BECF_6DFB_D813_0080AD509054__INCLUDED_

#pragma once


class CBlockDelay : public CScriptBlock
{
public:
   CBlockDelay();
   
   void Init();
   WORD GetType() const;
   DWORD GetStyle() const;
   int GetImage() const;
   BOOL AddWizardPages(IWizardSheet* pSheet);
   BOOL Validate(IValidator* pValidator);
   BOOL Compile(IBuilder* pBuilder);
   BOOL Read(ISerializer* pArc);
   BOOL Write(ISerializer* pArc);

   long m_lDelay;
};


#endif // !defined(AFX_DELAYBLOCK_H__20040807_BECF_6DFB_D813_0080AD509054__INCLUDED_)


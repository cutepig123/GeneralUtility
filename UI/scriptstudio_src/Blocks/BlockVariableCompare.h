#if !defined(AFX_BLOCKVARIABLECOMPARE_H__20040807_138D_A108_BB21_0080AD509054__INCLUDED_)
#define AFX_BLOCKVARIABLECOMPARE_H__20040807_138D_A108_BB21_0080AD509054__INCLUDED_

#pragma once

#include "../ScriptBlock.h"


class CBlockVariableCompare : public CScriptBlock
{
public:
   CBlockVariableCompare();

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


#endif // !defined(AFX_BLOCKVARIABLECOMPARE_H__20040807_138D_A108_BB21_0080AD509054__INCLUDED_)


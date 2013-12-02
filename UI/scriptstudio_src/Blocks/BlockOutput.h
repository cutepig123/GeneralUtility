#if !defined(AFX_BLOCKOUTPUT_H__20040820_2A71_EF18_08BD_0080AD509054__INCLUDED_)
#define AFX_BLOCKOUTPUT_H__20040820_2A71_EF18_08BD_0080AD509054__INCLUDED_

#pragma once


class CBlockOutput : public CScriptBlock
{
public:
   CBlockOutput();

   void Init();
   WORD GetType() const;
   DWORD GetStyle() const;
   int GetImage() const;
   BOOL AddWizardPages(IWizardSheet* pSheet);
   BOOL Validate(IValidator* pValidator);
   BOOL Compile(IBuilder* pBuilder);
   BOOL Read(ISerializer* pArc);
   BOOL Write(ISerializer* pArc);
};


#endif // !defined(AFX_BLOCKOUTPUT_H__20040820_2A71_EF18_08BD_0080AD509054__INCLUDED_)

#if !defined(AFX_BLOCKROOT_H__20040821_6B4A_148A_E066_0080AD509054__INCLUDED_)
#define AFX_BLOCKROOT_H__20040821_6B4A_148A_E066_0080AD509054__INCLUDED_

#pragma once


class CBlockRoot : public CScriptBlock
{
public:
   CBlockRoot();

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


#endif // !defined(AFX_BLOCKROOT_H__20040821_6B4A_148A_E066_0080AD509054__INCLUDED_)


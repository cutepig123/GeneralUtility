#if !defined(AFX_BLOCKUSERBLOCK_H__20040821_57B6_FC89_309B_0080AD509054__INCLUDED_)
#define AFX_BLOCKUSERBLOCK_H__20040821_57B6_FC89_309B_0080AD509054__INCLUDED_

#pragma once


class CBlockUserBlock : public CScriptBlock
{
public:
   CBlockUserBlock();

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


#endif // !defined(AFX_BLOCKUSERBLOCK_H__20040821_57B6_FC89_309B_0080AD509054__INCLUDED_)



#include "stdafx.h"
#include "../resource.h"

#include "BlockFileOpen.h"


CBlockFileOpen::CBlockFileOpen()
{
   m_szFile[0] = '\0';
   m_szVariable[0] = '\0';
   SetName(_T("FileOpen"));
}

void CBlockFileOpen::Init()
{
   AddInput(0, _T("~"));
   AddOutput(_T("Next"), 0);
}

WORD CBlockFileOpen::GetType() const
{
   return ID_BLOCK_FILEOPEN;
}

DWORD CBlockFileOpen::GetStyle() const
{
   return 0;
}

int CBlockFileOpen::GetImage() const
{
   return 18;
}

BOOL CBlockFileOpen::AddWizardPages(IWizardSheet* pSheet)
{
   return TRUE;
}

BOOL CBlockFileOpen::Validate(IValidator* pValidator)
{
   return TRUE;
}

BOOL CBlockFileOpen::Compile(IBuilder* pBuilder)
{
   return TRUE;
}

BOOL CBlockFileOpen::Read(ISerializer* pArc)
{
   pArc->Read(_T("file"), m_szFile, 63);
   pArc->Read(_T("variable"), m_szVariable, 63);
   return CScriptBlock::Read(pArc);
}

BOOL CBlockFileOpen::Write(ISerializer* pArc)
{
   pArc->Write(_T("file"), m_szFile);
   pArc->Write(_T("variable"), m_szVariable);
   if( !CScriptBlock::Write(pArc) ) return FALSE;
   return TRUE;
}


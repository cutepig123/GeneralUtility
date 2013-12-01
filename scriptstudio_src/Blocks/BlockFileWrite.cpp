
#include "stdafx.h"
#include "../resource.h"

#include "BlockFileWrite.h"


CBlockFileWrite::CBlockFileWrite()
{
   m_szFile[0] = '\0';
   m_szVariable[0] = '\0';
   SetName(_T("FileWrite"));
}

void CBlockFileWrite::Init()
{
   AddInput(0, _T("~"));
   AddOutput(_T("Next"), 0);
}

WORD CBlockFileWrite::GetType() const
{
   return ID_BLOCK_FILEWRITE;
}

DWORD CBlockFileWrite::GetStyle() const
{
   return 0;
}

int CBlockFileWrite::GetImage() const
{
   return 20;
}

BOOL CBlockFileWrite::AddWizardPages(IWizardSheet* pSheet)
{
   return TRUE;
}

BOOL CBlockFileWrite::Validate(IValidator* pValidator)
{
   return TRUE;
}

BOOL CBlockFileWrite::Compile(IBuilder* pBuilder)
{
   return TRUE;
}

BOOL CBlockFileWrite::Read(ISerializer* pArc)
{
   pArc->Read(_T("file"), m_szFile, 63);
   pArc->Read(_T("variable"), m_szVariable, 63);
   return CScriptBlock::Read(pArc);
}

BOOL CBlockFileWrite::Write(ISerializer* pArc)
{
   pArc->Write(_T("file"), m_szFile);
   pArc->Write(_T("variable"), m_szVariable);
   if( !CScriptBlock::Write(pArc) ) return FALSE;
   return TRUE;
}


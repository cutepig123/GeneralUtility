
#include "stdafx.h"
#include "../resource.h"

#include "BlockFileClose.h"


CBlockFileClose::CBlockFileClose()
{
   m_szFile[0] = '\0';
   SetName(_T("FileClose"));
}

void CBlockFileClose::Init()
{
   AddInput(0, _T("~"));
   AddOutput(_T("Next"), 0);
}

WORD CBlockFileClose::GetType() const
{
   return ID_BLOCK_FILECLOSE;
}

DWORD CBlockFileClose::GetStyle() const
{
   return 0;
}

int CBlockFileClose::GetImage() const
{
   return 21;
}

BOOL CBlockFileClose::AddWizardPages(IWizardSheet* pSheet)
{
   return TRUE;
}

BOOL CBlockFileClose::Validate(IValidator* pValidator)
{
   return TRUE;
}

BOOL CBlockFileClose::Compile(IBuilder* pBuilder)
{
   return TRUE;
}

BOOL CBlockFileClose::Read(ISerializer* pArc)
{
   pArc->Read(_T("file"), m_szFile, 63);
   return CScriptBlock::Read(pArc);
}

BOOL CBlockFileClose::Write(ISerializer* pArc)
{
   pArc->Write(_T("file"), m_szFile);
   if( !CScriptBlock::Write(pArc) ) return FALSE;
   return TRUE;
}



#include "stdafx.h"
#include "../resource.h"

#include "BlockDelay.h"


CBlockDelay::CBlockDelay()
{
   m_lDelay = 0;
   SetName(_T("Delay"));
}

void CBlockDelay::Init()
{
   AddInput(0, _T("~"));
   AddOutput(_T("Next"), 0);
}

WORD CBlockDelay::GetType() const
{
   return ID_BLOCK_DELAY;
}

int CBlockDelay::GetImage() const
{
   return 22;
}

DWORD CBlockDelay::GetStyle() const
{
   return 0;
}

BOOL CBlockDelay::AddWizardPages(IWizardSheet* pSheet)
{
   return TRUE;
}

BOOL CBlockDelay::Validate(IValidator* pValidator)
{
   return TRUE;
}

BOOL CBlockDelay::Compile(IBuilder* pBuilder)
{
   return TRUE;
}

BOOL CBlockDelay::Read(ISerializer* pArc)
{
   pArc->Read(_T("period"), m_lDelay);
   return CScriptBlock::Read(pArc);
}

BOOL CBlockDelay::Write(ISerializer* pArc)
{
   pArc->Write(_T("period"), m_lDelay);
   if( !CScriptBlock::Write(pArc) ) return FALSE;
   return TRUE;
}


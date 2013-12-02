
#include "stdafx.h"
#include "../resource.h"

#include "BlockStart.h"


CBlockStart::CBlockStart()
{
   SetName(_T("Start"));
}

void CBlockStart::Init()
{
   AddOutput(_T("~"), 0);
}

WORD CBlockStart::GetType() const
{
   return ID_BLOCK_START;
}

DWORD CBlockStart::GetStyle() const
{
   return BLOCK_EX_RIGHTALIGNED | BLOCK_EX_SMALLBLOCK;
}

int CBlockStart::GetImage() const
{
   return 2;
}

BOOL CBlockStart::AddWizardPages(IWizardSheet* pSheet)
{
   return TRUE;
}

BOOL CBlockStart::Validate(IValidator* pValidator)
{
   return TRUE;
}

BOOL CBlockStart::Compile(IBuilder* pBuilder)
{
   return TRUE;
}

BOOL CBlockStart::Read(ISerializer* pArc)
{
   return CScriptBlock::Read(pArc);
}

BOOL CBlockStart::Write(ISerializer* pArc)
{
   if( !CScriptBlock::Write(pArc) ) return FALSE;
   return TRUE;
}


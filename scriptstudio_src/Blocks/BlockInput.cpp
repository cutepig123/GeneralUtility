
#include "stdafx.h"
#include "../resource.h"

#include "BlockInput.h"


CBlockInput::CBlockInput()
{
   SetName(_T("Input"));
}

void CBlockInput::Init()
{
   AddOutput(_T("~"), 0);
}

WORD CBlockInput::GetType() const
{
   return ID_BLOCK_INPUT;
}

DWORD CBlockInput::GetStyle() const
{
   return BLOCK_EX_RIGHTALIGNED | BLOCK_EX_SMALLBLOCK;
}

int CBlockInput::GetImage() const
{
   return 12;
}

BOOL CBlockInput::AddWizardPages(IWizardSheet* pSheet)
{
   return TRUE;
}

BOOL CBlockInput::Validate(IValidator* pValidator)
{
   return TRUE;
}

BOOL CBlockInput::Compile(IBuilder* pBuilder)
{
   return TRUE;
}

BOOL CBlockInput::Read(ISerializer* pArc)
{
   return CScriptBlock::Read(pArc);
}

BOOL CBlockInput::Write(ISerializer* pArc)
{
   if( !CScriptBlock::Write(pArc) ) return FALSE;
   return TRUE;
}


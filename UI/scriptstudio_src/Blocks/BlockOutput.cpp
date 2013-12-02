
#include "stdafx.h"
#include "../resource.h"

#include "BlockOutput.h"


CBlockOutput::CBlockOutput()
{
   SetName(_T("Output"));
}

void CBlockOutput::Init()
{
   AddInput(0, _T("~"));
}

WORD CBlockOutput::GetType() const
{
   return ID_BLOCK_OUTPUT;
}

DWORD CBlockOutput::GetStyle() const
{
   return BLOCK_EX_SMALLBLOCK;
}

int CBlockOutput::GetImage() const
{
   return 13;
}

BOOL CBlockOutput::AddWizardPages(IWizardSheet* pSheet)
{
   return TRUE;
}

BOOL CBlockOutput::Validate(IValidator* pValidator)
{
   return TRUE;
}

BOOL CBlockOutput::Compile(IBuilder* pBuilder)
{
   return TRUE;
}

BOOL CBlockOutput::Read(ISerializer* pArc)
{
   return CScriptBlock::Read(pArc);
}

BOOL CBlockOutput::Write(ISerializer* pArc)
{
   if( !CScriptBlock::Write(pArc) ) return FALSE;
   return TRUE;
}


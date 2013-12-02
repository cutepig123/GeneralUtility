
#include "stdafx.h"
#include "../resource.h"

#include "BlockStop.h"


CBlockStop::CBlockStop()
{
   SetName(_T("Stop"));
}

void CBlockStop::Init()
{
   AddInput(0, _T("~"));
}

WORD CBlockStop::GetType() const
{
   return ID_BLOCK_STOP;
}

DWORD CBlockStop::GetStyle() const
{
   return BLOCK_EX_SMALLBLOCK;
}

int CBlockStop::GetImage() const
{
   return 3;
}

BOOL CBlockStop::AddWizardPages(IWizardSheet* pSheet)
{
   return TRUE;
}

BOOL CBlockStop::Validate(IValidator* pValidator)
{
   return TRUE;
}

BOOL CBlockStop::Compile(IBuilder* pBuilder)
{
   return TRUE;
}

BOOL CBlockStop::Read(ISerializer* pArc)
{
   return CScriptBlock::Read(pArc);
}

BOOL CBlockStop::Write(ISerializer* pArc)
{
   if( !CScriptBlock::Write(pArc) ) return FALSE;
   return TRUE;
}


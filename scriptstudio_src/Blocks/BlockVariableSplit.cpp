
#include "stdafx.h"
#include "../resource.h"

#include "BlockVariableSplit.h"


CBlockVariableSplit::CBlockVariableSplit()
{
   m_szVariable[0] = '\0';
   SetName(_T("Split"));
}

void CBlockVariableSplit::Init()
{
   AddInput(0, _T("~"));
   AddOutput(_T("Next"), 0);
}

WORD CBlockVariableSplit::GetType() const
{
   return ID_BLOCK_SPLITVARIABLE;
}

DWORD CBlockVariableSplit::GetStyle() const
{
   return 0;
}

int CBlockVariableSplit::GetImage() const
{
   return 7;
}

BOOL CBlockVariableSplit::AddWizardPages(IWizardSheet* pSheet)
{
   return TRUE;
}

BOOL CBlockVariableSplit::Validate(IValidator* pValidator)
{
   return TRUE;
}

BOOL CBlockVariableSplit::Compile(IBuilder* pBuilder)
{
   return TRUE;
}

BOOL CBlockVariableSplit::Read(ISerializer* pArc)
{
   pArc->Read(_T("variable"), m_szVariable, 63);
   return CScriptBlock::Read(pArc);
}

BOOL CBlockVariableSplit::Write(ISerializer* pArc)
{
   pArc->Write(_T("variable"), m_szVariable);
   if( !CScriptBlock::Write(pArc) ) return FALSE;
   return TRUE;
}


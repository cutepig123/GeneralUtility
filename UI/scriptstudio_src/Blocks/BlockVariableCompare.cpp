
#include "stdafx.h"
#include "../resource.h"

#include "BlockVariableCompare.h"


CBlockVariableCompare::CBlockVariableCompare()
{
   m_szVariable[0] = '\0';
   SetName(_T("Compare"));
}

void CBlockVariableCompare::Init()
{
   AddInput(0, _T("~"));
   AddOutput(_T("0"), 0);
   AddOutput(_T("NoMatch"), 0);
}

WORD CBlockVariableCompare::GetType() const
{
   return ID_BLOCK_COMPARE;
}

DWORD CBlockVariableCompare::GetStyle() const
{
   return 0;
}

int CBlockVariableCompare::GetImage() const
{
   return 6;
}

BOOL CBlockVariableCompare::AddWizardPages(IWizardSheet* pSheet)
{
   return TRUE;
}

BOOL CBlockVariableCompare::Validate(IValidator* pValidator)
{
   return TRUE;
}

BOOL CBlockVariableCompare::Compile(IBuilder* pBuilder)
{
   return TRUE;
}

BOOL CBlockVariableCompare::Read(ISerializer* pArc)
{
   pArc->Read(_T("variable"), m_szVariable, 63);
   return CScriptBlock::Read(pArc);
}

BOOL CBlockVariableCompare::Write(ISerializer* pArc)
{
   pArc->Write(_T("variable"), m_szVariable);
   if( !CScriptBlock::Write(pArc) ) return FALSE;
   return TRUE;
}


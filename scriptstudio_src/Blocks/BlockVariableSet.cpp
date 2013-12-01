
#include "stdafx.h"
#include "../resource.h"

#include "BlockVariableSet.h"


CBlockVariableSet::CBlockVariableSet()
{
   m_szVariable[0] = '\0';
   m_szExpression[0] = '\0';
   SetName(_T("SetValue"));
}

void CBlockVariableSet::Init()
{
   AddInput(0, _T("~"));
   AddOutput(_T("Next"), 0);
}

WORD CBlockVariableSet::GetType() const
{
   return ID_BLOCK_SETVARIABLE;
}

DWORD CBlockVariableSet::GetStyle() const
{
   return 0;
}

int CBlockVariableSet::GetImage() const
{
   return 5;
}

BOOL CBlockVariableSet::AddWizardPages(IWizardSheet* pSheet)
{
   return TRUE;
}

BOOL CBlockVariableSet::Validate(IValidator* pValidator)
{
   return TRUE;
}

BOOL CBlockVariableSet::Compile(IBuilder* pBuilder)
{
   return TRUE;
}

BOOL CBlockVariableSet::Read(ISerializer* pArc)
{
   pArc->Read(_T("variable"), m_szVariable, 63);
   pArc->Read(_T("expr"), m_szExpression, 99);
   return CScriptBlock::Read(pArc);
}

BOOL CBlockVariableSet::Write(ISerializer* pArc)
{
   pArc->Write(_T("variable"), m_szVariable);
   pArc->Write(_T("expr"), m_szExpression);
   if( !CScriptBlock::Write(pArc) ) return FALSE;
   return TRUE;
}


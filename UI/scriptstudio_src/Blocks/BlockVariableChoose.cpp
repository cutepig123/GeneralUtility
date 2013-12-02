
#include "stdafx.h"
#include "../resource.h"

#include "BlockVariableChoose.h"


CBlockVariableChoose::CBlockVariableChoose()
{
   m_szVariable[0] = '\0';
   SetName(_T("Choose"));
}

void CBlockVariableChoose::Init()
{
   AddInput(0, _T("~"));
   AddOutput(_T("Next"), 0);
}

WORD CBlockVariableChoose::GetType() const
{
   return ID_BLOCK_CHOOSE;
}

DWORD CBlockVariableChoose::GetStyle() const
{
   return 0;
}

int CBlockVariableChoose::GetImage() const
{
   return 8;
}

BOOL CBlockVariableChoose::AddWizardPages(IWizardSheet* pSheet)
{
   return TRUE;
}

BOOL CBlockVariableChoose::Validate(IValidator* pValidator)
{
   return TRUE;
}

BOOL CBlockVariableChoose::Compile(IBuilder* pBuilder)
{
   return TRUE;
}

BOOL CBlockVariableChoose::Read(ISerializer* pArc)
{
   pArc->Read(_T("variable"), m_szVariable, 63);
   return CScriptBlock::Read(pArc);
}

BOOL CBlockVariableChoose::Write(ISerializer* pArc)
{
   pArc->Write(_T("variable"), m_szVariable);
   if( !CScriptBlock::Write(pArc) ) return FALSE;
   return TRUE;
}


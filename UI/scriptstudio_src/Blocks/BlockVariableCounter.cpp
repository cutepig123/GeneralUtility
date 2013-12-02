
#include "stdafx.h"
#include "../resource.h"

#include "BlockVariableCounter.h"


CBlockVariableCounter::CBlockVariableCounter()
{
   m_szVariable[0] = '\0';
   m_szStart[0] = '\0';
   m_szEnd[0] = '\0';
   m_szStep[0] = '\0';
   SetName(_T("Counter"));
}

void CBlockVariableCounter::Init()
{
   AddInput(0, _T("~"));
   AddInput(0, _T("Start"));
   AddInput(0, _T("Step"));
   AddOutput(_T("Next"), 0);
   AddOutput(_T("Overflow"), 0);
}

WORD CBlockVariableCounter::GetType() const
{
   return ID_BLOCK_COUNTER;
}

DWORD CBlockVariableCounter::GetStyle() const
{
   return BLOCK_EX_WIDEBLOCK;
}

int CBlockVariableCounter::GetImage() const
{
   return 11;
}

BOOL CBlockVariableCounter::AddWizardPages(IWizardSheet* pSheet)
{
   return TRUE;
}

BOOL CBlockVariableCounter::Validate(IValidator* pValidator)
{
   return TRUE;
}

BOOL CBlockVariableCounter::Compile(IBuilder* pBuilder)
{
   return TRUE;
}

BOOL CBlockVariableCounter::Read(ISerializer* pArc)
{
   pArc->Read(_T("variable"), m_szVariable, 63);
   pArc->Read(_T("start"), m_szStart, 99);
   pArc->Read(_T("end"), m_szEnd, 99);
   pArc->Read(_T("step"), m_szStep, 99);
   return CScriptBlock::Read(pArc);
}

BOOL CBlockVariableCounter::Write(ISerializer* pArc)
{
   pArc->Write(_T("variable"), m_szVariable);
   pArc->Write(_T("start"), m_szStart);
   pArc->Write(_T("end"), m_szEnd);
   pArc->Write(_T("step"), m_szStep);
   if( !CScriptBlock::Write(pArc) ) return FALSE;
   return TRUE;
}



#include "stdafx.h"
#include "../resource.h"

#include "BlockFileRead.h"


CBlockFileRead::CBlockFileRead()
{
   m_szFile[0] = '\0';
   m_szVariable[0] = '\0';
   SetName(_T("FileRead"));
}

void CBlockFileRead::Init()
{
   AddInput(0, _T("~"));
   AddOutput(_T("Next"), 0);
}

WORD CBlockFileRead::GetType() const
{
   return ID_BLOCK_FILEREAD;
}

DWORD CBlockFileRead::GetStyle() const
{
   return 0;
}

int CBlockFileRead::GetImage() const
{
   return 19;
}

BOOL CBlockFileRead::AddWizardPages(IWizardSheet* pSheet)
{
   return TRUE;
}

BOOL CBlockFileRead::Validate(IValidator* pValidator)
{
   return TRUE;
}

BOOL CBlockFileRead::Compile(IBuilder* pBuilder)
{
   return TRUE;
}

BOOL CBlockFileRead::Read(ISerializer* pArc)
{
   pArc->Read(_T("file"), m_szFile, 63);
   pArc->Read(_T("variable"), m_szVariable, 63);
   return CScriptBlock::Read(pArc);
}

BOOL CBlockFileRead::Write(ISerializer* pArc)
{
   pArc->Write(_T("file"), m_szFile);
   pArc->Write(_T("variable"), m_szVariable);
   if( !CScriptBlock::Write(pArc) ) return FALSE;
   return TRUE;
}


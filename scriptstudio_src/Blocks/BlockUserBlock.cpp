
#include "stdafx.h"
#include "../resource.h"

#include "BlockUserBlock.h"


CBlockUserBlock::CBlockUserBlock()
{
   SetName(_T("UserBlock"));
}

void CBlockUserBlock::Init()
{
   AddInput(0, _T("~"));
   AddInput(0, _T("Input"));
   AddOutput(_T("Output"), 0);
   //
   CScriptBlock* pInput = CScriptBlock::CreateInstance(ID_BLOCK_INPUT);
   ATLASSERT(pInput);
   pInput->Init();
   POINT ptInput = { 40, 40 };
   pInput->SetPos(ptInput);
   AppendChild(pInput);
   //
   CScriptBlock* pOutput = CScriptBlock::CreateInstance(ID_BLOCK_OUTPUT);
   ATLASSERT(pOutput);
   pOutput->Init();
   POINT ptOutput = { 440, 40 };
   pOutput->SetPos(ptOutput);
   AppendChild(pOutput);
}

WORD CBlockUserBlock::GetType() const
{
   return ID_BLOCK_USERBLOCK;
}

DWORD CBlockUserBlock::GetStyle() const
{
   return BLOCK_EX_WIDEBLOCK | BLOCK_EX_CONTAINER;
}

int CBlockUserBlock::GetImage() const
{
   return 0;
}

BOOL CBlockUserBlock::AddWizardPages(IWizardSheet* pSheet)
{
   return TRUE;
}

BOOL CBlockUserBlock::Validate(IValidator* pValidator)
{
   return TRUE;
}

BOOL CBlockUserBlock::Compile(IBuilder* pBuilder)
{
   return TRUE;
}

BOOL CBlockUserBlock::Read(ISerializer* pArc)
{
   if( !CScriptBlock::Read(pArc) ) return FALSE;

   if( !pArc->ReadGroupBegin("Blocks") ) return TRUE;
   while( pArc->ReadGroupBegin(_T("Block")) ) {
      long lType = 0;
      pArc->Read(_T("type"), lType);
      CScriptBlock* pBlock = CScriptBlock::CreateInstance( (WORD) lType );
      if( pBlock == NULL ) return FALSE;
      pBlock->Read(pArc);
      AppendChild(pBlock);
      pArc->ReadGroupEnd();
   }
   pArc->ReadGroupEnd();

   return TRUE;
}

BOOL CBlockUserBlock::Write(ISerializer* pArc)
{
   if( !CScriptBlock::Write(pArc) ) return FALSE;

   if( GetChild() == NULL ) return TRUE;
   
   if( !pArc->WriteGroupBegin("Blocks") ) return FALSE;
   CScriptBlock* pBlock = GetChild();
   while( pBlock != NULL ) {
      if( !pArc->WriteGroupBegin(_T("Block")) ) return FALSE;
      pBlock->Write(pArc);
      pArc->WriteGroupEnd();
      pBlock = pBlock->GetNext();
   }
   pArc->WriteGroupEnd();
   
   return TRUE;
}


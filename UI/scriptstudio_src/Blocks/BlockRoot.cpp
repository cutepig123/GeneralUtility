
#include "stdafx.h"
#include "../resource.h"

#include "BlockRoot.h"


CBlockRoot::CBlockRoot()
{
   SetName(_T("Script"));
}

void CBlockRoot::Init()
{
}

WORD CBlockRoot::GetType() const
{
   return ID_BLOCK_ROOT;
}

DWORD CBlockRoot::GetStyle() const
{
   return 0;
}

int CBlockRoot::GetImage() const
{
   return 0;
}

BOOL CBlockRoot::AddWizardPages(IWizardSheet* pSheet)
{
   return TRUE;
}

BOOL CBlockRoot::Validate(IValidator* pValidator)
{
   return TRUE;
}

BOOL CBlockRoot::Compile(IBuilder* pBuilder)
{
   return TRUE;
}

BOOL CBlockRoot::Read(ISerializer* pArc)
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

BOOL CBlockRoot::Write(ISerializer* pArc)
{
   if( !CScriptBlock::Write(pArc) ) return FALSE;

   if( m_pChild == NULL ) return TRUE;

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


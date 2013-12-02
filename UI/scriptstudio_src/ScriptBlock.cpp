
#include "stdafx.h"
#include "resource.h"

#include "ScriptBlock.h"

#include "Blocks/BlockRoot.h"
#include "Blocks/BlockStart.h"
#include "Blocks/BlockStop.h"
#include "Blocks/BlockInput.h"
#include "Blocks/BlockOutput.h"
#include "Blocks/BlockDelay.h"
#include "Blocks/BlockUserBlock.h"
#include "Blocks/BlockVariableSet.h"
#include "Blocks/BlockVariableCompare.h"
#include "Blocks/BlockVariableChoose.h"
#include "Blocks/BlockVariableSplit.h"
#include "Blocks/BlockVariableCounter.h"
#include "Blocks/BlockFileOpen.h"
#include "Blocks/BlockFileRead.h"
#include "Blocks/BlockFileWrite.h"
#include "Blocks/BlockFileClose.h"



CScriptBlock::CScriptBlock() : 
   m_pChild(NULL), 
   m_pParent(NULL),
   m_pPrev(NULL),
   m_pNext(NULL)
{
   m_szName[0] = '\0';
   m_ptPos.x = m_ptPos.y = 0;
}

CScriptBlock::~CScriptBlock()
{
}

BOOL CScriptBlock::SetName(LPCTSTR pstrName)
{
   return _tcsncpy(m_szName, pstrName, (sizeof(m_szName) / sizeof(TCHAR)) - 1) > 0;
}

BOOL CScriptBlock::GetName(LPTSTR pstrName, UINT cchMax) const
{
   return _tcsncpy(pstrName, m_szName, cchMax) > 0;
}

POINT CScriptBlock::GetPos() const
{
   return m_ptPos;
}

POINT CScriptBlock::SetPos(POINT pt)
{
   POINT ptOld = m_ptPos;
   m_ptPos = pt;
   return ptOld;
}

BOOL CScriptBlock::AddInput(long lID, LPCTSTR pstrName)
{
   // Find a unqiue ID for this input link
   // TODO: Something better than this, please!
   static long s_lUniqueID = 0;
   if( lID > 0 && lID > s_lUniqueID ) s_lUniqueID = lID + 1;
   if( lID == 0 ) lID = ++s_lUniqueID;
   // Add link
   TScriptLink o = { 0 };
   o.lID = lID;
   _tcscpy(o.szName, pstrName);
   return m_aInput.Add(o);
}

BOOL CScriptBlock::AddOutput(LPCTSTR pstrName, long lTargetID)
{
   // Add link
   TScriptLink o = { 0 };
   o.lID = lTargetID;
   _tcscpy(o.szName, pstrName);
   return m_aOutput.Add(o);
}

BOOL CScriptBlock::SetLinkTarget(LPCTSTR pstrName, long lTargetID)
{
   for( int i = 0; i < m_aOutput.GetSize(); i++ ) {
      if( _tcscmp(m_aOutput[i].szName, pstrName) == 0 ) {
         m_aOutput[i].lID = lTargetID;
         return TRUE;
      }
   }
   return FALSE;
}

CScriptBlock* CScriptBlock::CreateInstance(WORD wID)
{
   // TODO: Add remaining blocks...
   switch( wID ) {
   case ID_BLOCK_ROOT:             return new CBlockRoot();
   case ID_BLOCK_START:            return new CBlockStart();
   case ID_BLOCK_STOP:             return new CBlockStop();
   case ID_BLOCK_INPUT:            return new CBlockInput();
   case ID_BLOCK_OUTPUT:           return new CBlockOutput();
   case ID_BLOCK_DELAY:            return new CBlockDelay();
   case ID_BLOCK_SETVARIABLE:      return new CBlockVariableSet();
   case ID_BLOCK_CHOOSE:           return new CBlockVariableChoose();
   case ID_BLOCK_COMPARE:          return new CBlockVariableCompare();
   case ID_BLOCK_SPLITVARIABLE:    return new CBlockVariableSplit();
   case ID_BLOCK_COUNTER:          return new CBlockVariableCounter();
   case ID_BLOCK_USERBLOCK:        return new CBlockUserBlock();
   case ID_BLOCK_FILEOPEN:         return new CBlockFileOpen();
   case ID_BLOCK_FILEREAD:         return new CBlockFileRead();
   case ID_BLOCK_FILEWRITE:        return new CBlockFileWrite();
   case ID_BLOCK_FILECLOSE:        return new CBlockFileClose();
   default: 
      ATLASSERT(!"Unsupported script block??");
      return NULL;
   }
}

BOOL CScriptBlock::Read(ISerializer* pArc)
{
   long x = 0, y = 0;
   pArc->Read(_T("xpos"), x); m_ptPos.x = x;
   pArc->Read(_T("ypos"), y); m_ptPos.y = y;
   pArc->Read(_T("name"), m_szName, 99);

   if( pArc->ReadGroupBegin(_T("Links")) ) {
      while( pArc->ReadGroupBegin(_T("Link")) ) {
         long lType = 0;
         long lID = 0;
         TCHAR szName[100] = { 0 };
         pArc->Read(_T("type"), lType);
         pArc->Read(_T("id"), lID);
         pArc->Read(_T("name"), szName, 99);
         if( lType == 0 ) AddInput(lID, szName); else AddOutput(szName, lID);
         pArc->ReadGroupEnd();
      }
      pArc->ReadGroupEnd();
   }

   return TRUE;
}

BOOL CScriptBlock::Write(ISerializer* pArc)
{
   pArc->Write(_T("type"), (long) GetType());
   pArc->Write(_T("xpos"), (long) GetPos().x);
   pArc->Write(_T("ypos"), (long) GetPos().y);
   pArc->Write(_T("name"), m_szName);

   if( pArc->WriteGroupBegin(_T("Links")) ) {
      for( int i = 0; i < m_aInput.GetSize(); i++ ) {
         if( !pArc->WriteGroupBegin(_T("Link")) ) return FALSE;
         pArc->Write(_T("type"), 0L);
         pArc->Write(_T("id"), m_aInput[i].lID);
         pArc->Write(_T("name"), m_aInput[i].szName);
         pArc->WriteGroupEnd();
      }
      for( int j = 0; j < m_aOutput.GetSize(); j++ ) {
         if( !pArc->WriteGroupBegin(_T("Link")) ) return FALSE;
         pArc->Write(_T("type"), 1L);
         pArc->Write(_T("id"), m_aOutput[j].lID);
         pArc->Write(_T("name"), m_aOutput[j].szName);
         pArc->WriteGroupEnd();
      }
      pArc->WriteGroupEnd();
   }

   return TRUE;
}

CScriptBlock* CScriptBlock::GetPrev() const
{
   return m_pPrev;
}

CScriptBlock* CScriptBlock::GetNext() const
{
   return m_pNext;
}

CScriptBlock* CScriptBlock::GetParent() const
{
   return m_pParent;
}

CScriptBlock* CScriptBlock::GetChild() const
{
   return m_pChild;
}

BOOL CScriptBlock::AppendChild(CScriptBlock* pBlock)
{
   ATLASSERT(pBlock);
   ATLASSERT(pBlock->m_pParent==NULL);
   if( m_pChild == NULL ) {
      m_pChild = pBlock;
   }
   else {      
      m_pChild->m_pPrev = pBlock;
      pBlock->m_pNext = m_pChild;
      m_pChild = pBlock;
   }
   pBlock->m_pParent = this;
   return TRUE;
}

BOOL CScriptBlock::Delete()
{
   if( m_pChild != NULL ) {
      m_pChild->Delete();
   }
   if( m_pParent->m_pChild == this ) {
      ATLASSERT(m_pPrev==NULL);
      m_pParent->m_pChild = m_pNext;
      if( m_pNext ) m_pNext->m_pPrev = NULL;
   }
   else {
      ATLASSERT(m_pPrev!=NULL);
      m_pPrev->m_pNext = m_pNext;
      if( m_pNext ) m_pNext->m_pPrev = m_pPrev;
   }
   delete this;
   return TRUE;
}


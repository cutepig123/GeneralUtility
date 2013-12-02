
#include "stdafx.h"
#include "resource.h"

#include "Serializer.h"

#include "ScriptDoc.h"


CScriptDoc::CScriptDoc() :
   m_pBlocks(NULL)
{
}

CScriptDoc::~CScriptDoc()
{
   if( m_pBlocks ) delete m_pBlocks;  
}

BOOL CScriptDoc::Init(HWND hWnd)
{
   m_wndView = hWnd;
   m_lUnqiueID = 0;
   m_bDirty = FALSE;
   m_aVars.RemoveAll();
   if( m_pBlocks ) delete m_pBlocks;  
   return TRUE;
}

BOOL CScriptDoc::New()
{
   m_pBlocks = CScriptBlock::CreateInstance(ID_BLOCK_ROOT);
   if( m_pBlocks == NULL ) return FALSE;
   CScriptBlock* pStart = CScriptBlock::CreateInstance(ID_BLOCK_START);
   if( pStart == NULL ) return FALSE;
   POINT pt = { 40, 40 };
   pStart->Init();
   pStart->SetPos(pt);
   m_pBlocks->AppendChild(pStart);
   return TRUE;
}

BOOL CScriptDoc::Load(LPCTSTR pstrFilename)
{
   ATLASSERT(pstrFilename);

   CXmlSerializer arc;
   if( !arc.Open("Script", pstrFilename) ) {
      AtlMessageBox(m_wndView, IDS_ERR_OPENFILE, IDS_CAPTION_ERROR, MB_ICONEXCLAMATION);
      return FALSE;
   }
   if( !arc.ReadGroupBegin("Blocks") ) {
      AtlMessageBox(m_wndView, IDS_ERR_XMLFILE, IDS_CAPTION_ERROR, MB_ICONEXCLAMATION);
      return FALSE;
   }
   m_pBlocks = NULL;
   while( arc.ReadGroupBegin(_T("Block")) ) {
      long lType = 0;
      arc.Read(_T("type"), lType);
      CScriptBlock* pBlock = CScriptBlock::CreateInstance( (WORD) lType );
      if( pBlock == NULL ) {
         AtlMessageBox(m_wndView, IDS_ERR_BLOCKTYPE, IDS_CAPTION_ERROR, MB_ICONEXCLAMATION);
         return FALSE;
      }
      if( !pBlock->Read(&arc) ) {
         AtlMessageBox(m_wndView, IDS_ERR_FILEERROR, IDS_CAPTION_ERROR, MB_ICONEXCLAMATION);
         return FALSE;
      }
      if( m_pBlocks == NULL ) {
         m_pBlocks = pBlock;
      }
      else {
         m_pBlocks->AppendChild(pBlock);
      }
      arc.ReadGroupEnd();
   }
   arc.ReadGroupEnd();
   arc.Close();

   m_sFilename = pstrFilename;
   m_bDirty = FALSE;

   return TRUE;
}

BOOL CScriptDoc::Save(LPCTSTR pstrFilename)
{
   ATLASSERT(pstrFilename);

   CXmlSerializer arc;
   if( !arc.Create("Script") ) {
      AtlMessageBox(m_wndView, IDS_ERR_CREATEFILE, IDS_CAPTION_ERROR, MB_ICONEXCLAMATION);
      return FALSE;
   }
   if( !arc.WriteGroupBegin("Blocks") ) {
      AtlMessageBox(m_wndView, IDS_ERR_XMLFILE, IDS_CAPTION_ERROR, MB_ICONEXCLAMATION);
      return FALSE;
   }
   CScriptBlock* pBlock = m_pBlocks;
   while( pBlock != NULL ) {
      if( !arc.WriteGroupBegin(_T("Block")) ) return FALSE;
      pBlock->Write(&arc);
      arc.WriteGroupEnd();
      pBlock = pBlock->GetNext();
   }
   arc.WriteGroupEnd();
   if( !arc.Save(pstrFilename) ) {
      AtlMessageBox(m_wndView, IDS_ERR_SAVEFILE, IDS_CAPTION_ERROR, MB_ICONEXCLAMATION);
      return FALSE;
   }
   arc.Close();

   m_sFilename = pstrFilename;
   m_bDirty = FALSE;

   return TRUE;
}

void CScriptDoc::SetDirty(BOOL bDirty /* = TRUE*/)
{
   m_bDirty = bDirty;
   // TODO: Build undo/redo action
}

BOOL CScriptDoc::IsDirty() const
{
   return m_bDirty;
}

CString CScriptDoc::GetFilename() const
{
   return m_sFilename;
}

CScriptBlock* CScriptDoc::GetRoot() const
{
   return m_pBlocks;
}


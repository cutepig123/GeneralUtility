#if !defined(AFX_SCRIPTDOC_H__20040527_2AED_DDA4_6D19_0080AD509054__INCLUDED_)
#define AFX_SCRIPTDOC_H__20040527_2AED_DDA4_6D19_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Serializer.h"

#include "ScriptBlock.h"


typedef enum DATATYPE
{
   DT_VARIABLE,
   DT_OBJECT,
   DT_FILE,
};

typedef struct 
{
   TCHAR szName[64];
   DATATYPE dt;
} TVariable;


class CScriptDoc
{
public:
   CScriptDoc();
   ~CScriptDoc();

   BOOL Init(HWND hWnd);
   BOOL New();
   BOOL Load(LPCTSTR pstrFilename);
   BOOL Save(LPCTSTR pstrFilename);

   BOOL IsDirty() const;
   void SetDirty(BOOL bDirty = TRUE);

   CString GetFilename() const;
   CScriptBlock* GetRoot() const;

public:
   CWindow m_wndView;
   CString m_sFilename;
   CScriptBlock* m_pBlocks;
   CSimpleArray<TVariable> m_aVars;
   long m_lUnqiueID;
   BOOL m_bDirty;
};


#endif // !defined(AFX_SCRIPTDOC_H__20040527_2AED_DDA4_6D19_0080AD509054__INCLUDED_)


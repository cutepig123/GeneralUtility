#if !defined(AFX_SCRIPTBLOCK_H__20040613_1713_3132_2A6F_0080AD509054__INCLUDED_)
#define AFX_SCRIPTBLOCK_H__20040613_1713_3132_2A6F_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Serializer.h"

enum
{
   BLOCK_EX_RENAME          = 0x00000001,
   BLOCK_EX_PROPERTIES      = 0x00000002,
   BLOCK_EX_CONTAINER       = 0x00000004,
   BLOCK_EX_RIGHTALIGNED    = 0x00000008,
   BLOCK_EX_WIDEBLOCK       = 0x00000010,
   BLOCK_EX_SMALLBLOCK      = 0x00000020,
};

class IWizardSheet
{
   virtual void AddPage(HPROPSHEETPAGE page);
};


class IBuilder
{
public:
   virtual void EmitCode(LPCTSTR pstrCode) = 0;
};


class IValidator
{
public:
   virtual BOOL EmitError(int iErrorCode, LPCTSTR pstrMessage) = 0;
   virtual BOOL EmitMessage(int iErrorCode, LPCTSTR pstrMessage) = 0;
   virtual BOOL EmitWarning(int iErrorCode, LPCTSTR pstrMessage) = 0;
};


typedef struct tagTScriptLink
{
   long lID;
   TCHAR szName[64];
} TScriptLink;


class CScriptBlock
{
public:
   // Constructor / destructor
   CScriptBlock();
   virtual ~CScriptBlock();

   // Data Members

   POINT m_ptPos;
   TCHAR m_szName[100];
   CSimpleArray<TScriptLink> m_aInput;
   CSimpleArray<TScriptLink> m_aOutput;
   
   CScriptBlock* m_pChild;
   CScriptBlock* m_pParent;
   CScriptBlock* m_pPrev;
   CScriptBlock* m_pNext;

   // Operations

   BOOL SetName(LPCTSTR pstrName);
   BOOL GetName(LPTSTR pstrName, UINT cchMax) const;
   POINT GetPos() const;
   POINT SetPos(POINT pt);

   CScriptBlock* GetNext() const;
   CScriptBlock* GetPrev() const;
   CScriptBlock* GetParent() const;
   CScriptBlock* GetChild() const;

   BOOL Delete();
   BOOL AppendChild(CScriptBlock* pBlock);

   // Implementation

   BOOL AddInput(long lID, LPCTSTR pstrName);
   BOOL AddOutput(LPCTSTR pstrName, long lTargetID);
   BOOL SetLinkTarget(LPCTSTR pstrName, long lTargetID);
   static CScriptBlock* CreateInstance(WORD wID);

   // Virtual methods

   virtual void Init() = 0;
   virtual WORD GetType() const = 0;
   virtual DWORD GetStyle() const = 0;
   virtual int GetImage() const = 0;

   virtual BOOL Read(ISerializer* pArc);
   virtual BOOL Write(ISerializer* pArc);

   virtual BOOL AddWizardPages(IWizardSheet* pSheet) = 0;
   virtual BOOL Validate(IValidator* pValidate) = 0;
   virtual BOOL Compile(IBuilder* pBuilder) = 0;
};


#endif // !defined(AFX_SCRIPTBLOCK_H__20040613_1713_3132_2A6F_0080AD509054__INCLUDED_)

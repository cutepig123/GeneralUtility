#if !defined(AFX_SERIALIZER_H__20030221_1B1A_011D_26AE_0080AD509054__INCLUDED_)
#define AFX_SERIALIZER_H__20030221_1B1A_011D_26AE_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <msxml2.h>


class ISerializer
{
public:
   virtual bool ReadGroupBegin(LPCTSTR pstrName) = 0;
   virtual bool ReadGroupEnd() = 0;
   virtual bool Read(LPCTSTR pstrName, LPTSTR szValue, UINT cchMax) = 0;
   virtual bool Read(LPCTSTR pstrName, SYSTEMTIME& stValue) = 0;
   virtual bool Read(LPCTSTR pstrName, CString& sValue) = 0;
   virtual bool Read(LPCTSTR pstrName, long& lValue) = 0;
   virtual bool Read(LPCTSTR pstrName, bool& bValue) = 0;

   virtual bool WriteGroupBegin(LPCTSTR pstrName) = 0;
   virtual bool WriteGroupEnd() = 0;
   virtual bool Write(LPCTSTR pstrName, LPCTSTR pstrValue) = 0;
   virtual bool Write(LPCTSTR pstrName, SYSTEMTIME stValue) = 0;
   virtual bool Write(LPCTSTR pstrName, long lValue) = 0;
   virtual bool Write(LPCTSTR pstrName, bool bValue) = 0;
};



class CXmlSerializer : public ISerializer
{
public:
   typedef struct tagCONTEXT
   {
      CComBSTR bstrName;
      CComPtr<IXMLDOMNodeList> spNodes;
      long iIndex;
   } CONTEXT;
   CSimpleArray<CONTEXT> m_aStack;
   CComPtr<IXMLDOMNode> m_spNode;
   CComPtr<IXMLDOMDocument> m_spDoc;
   CComPtr<IXMLDOMNamedNodeMap> m_spAttribs; 
   int m_iDepth;

   virtual ~CXmlSerializer();

   bool Create(LPCTSTR pstrTitle);
   bool Save(LPCTSTR pstrFilename);
   bool Open(LPCTSTR pstrTitle, LPCTSTR pstrFilename);
   bool Close();

   // ISerialiable
   bool ReadGroupBegin(LPCTSTR pstrName);
   bool ReadGroupEnd();
   bool Read(LPCTSTR pstrName, LPTSTR szValue, UINT cchMax);
   bool Read(LPCTSTR pstrName, SYSTEMTIME& stValue);
   bool Read(LPCTSTR pstrName, CString& sValue);
   bool Read(LPCTSTR pstrName, long& lValue);
   bool Read(LPCTSTR pstrName, bool& bValue);

   bool WriteGroupBegin(LPCTSTR pstrName);
   bool WriteGroupEnd();
   bool Write(LPCTSTR pstrName, LPCTSTR pstrValue);
   bool Write(LPCTSTR pstrName, SYSTEMTIME stValue);
   bool Write(LPCTSTR pstrName, long lValue);
   bool Write(LPCTSTR pstrName, bool bValue);
};


#endif // !defined(AFX_SERIALIZER_H__20030221_1B1A_011D_26AE_0080AD509054__INCLUDED_)



#include "StdAfx.h"
#include "resource.h"

#include "Serializer.h"

#ifndef PROGID_XMLDOMDocument
   #define PROGID_XMLDOMDocument L"MSXML2.DOMDocument.6.0"
#endif


/////////////////////////////////////////////////////////////////
// CXmlSerializer


CXmlSerializer::~CXmlSerializer()
{
   Close();
}

bool CXmlSerializer::Create(LPCTSTR pstrTitle)
{
   ATLASSERT(!::IsBadStringPtr(pstrTitle,-1));

   // Create XML document
   HRESULT Hr = m_spDoc.CoCreateInstance(PROGID_XMLDOMDocument);
   ATLASSERT(SUCCEEDED(Hr));
   if( FAILED(Hr) ) return false;

   // Create preprossing stuff (for European characters)
   CComPtr<IXMLDOMProcessingInstruction> spPI;
   Hr = m_spDoc->createProcessingInstruction(CComBSTR(L"xml"), CComBSTR(L"version=\"1.0\" encoding=\"ISO-8859-1\""), &spPI);
   ATLASSERT(SUCCEEDED(Hr));
   if( FAILED(Hr) ) return false;
   CComPtr<IXMLDOMNode> spOut;
   if( FAILED( m_spDoc->appendChild(spPI, &spOut) ) ) return false;

   m_spNode = m_spDoc;
   return WriteGroupBegin(pstrTitle);
}

bool CXmlSerializer::Open(LPCTSTR pstrTitle, LPCTSTR pstrFilename)
{
   ATLASSERT(!::IsBadStringPtr(pstrTitle,-1));
   ATLASSERT(!::IsBadStringPtr(pstrFilename,-1));

   // Create XML document
   HRESULT Hr = m_spDoc.CoCreateInstance(PROGID_XMLDOMDocument);
   ATLASSERT(SUCCEEDED(Hr));
   if( FAILED(Hr) ) return false;

   // Load from file
   VARIANT_BOOL vbSuccess;
   m_spDoc->put_async(VARIANT_FALSE);
   if( FAILED( m_spDoc->load(CComVariant(pstrFilename), &vbSuccess) ) ) return false;
   if( vbSuccess == VARIANT_FALSE ) return false;

   // Prepare scan...
   CComQIPtr<IXMLDOMNode> spRoot = m_spDoc;
   if( FAILED( spRoot->get_lastChild(&m_spNode) ) ) return false;
   ATLASSERT(m_spNode);
   
   // Make sure it's the right document type (checks title)...
   USES_CONVERSION;
   CComBSTR bstr;
   m_spNode->get_nodeName(&bstr);
   if( _tcscmp(pstrTitle, OLE2CT(bstr)) != 0 ) return false;

   m_iDepth = 0;

   return true;
}

bool CXmlSerializer::Save(LPCTSTR pstrFilename)
{
   ATLASSERT(!::IsBadStringPtr(pstrFilename,-1));
   if( FAILED( m_spDoc->save(CComVariant(pstrFilename)) ) ) return false;
   return true;
}

bool CXmlSerializer::Close()
{
   m_spDoc.Release();
   m_spNode.Release();
   return true;
}

// ISerialiable

bool CXmlSerializer::ReadGroupBegin(LPCTSTR pstrName)
{
   ATLASSERT(!::IsBadStringPtr(pstrName,-1));
   if( m_spNode == NULL ) return false;
   // Get collection of children of this type
   CComBSTR bstrName = pstrName;
   long iIndex = 0;
   CComPtr<IXMLDOMNode> spNext;
   CComPtr<IXMLDOMNodeList> spNodes;
   if( m_aStack.GetSize() > 0 && m_aStack[ m_aStack.GetSize() - 1 ].bstrName == bstrName ) {
      CONTEXT& c = m_aStack[ m_aStack.GetSize() - 1 ];
      spNodes = c.spNodes;
      iIndex = ++c.iIndex;
      // Pick the next node
      spNodes->get_item(iIndex, &spNext);
      if( spNext == NULL ) return false;
   }
   else {
      m_spNode->selectNodes(CComBSTR(pstrName), &spNodes);
      if( spNodes == NULL ) return false;
      // Pick the next node
      spNodes->get_item(0L, &spNext);
      if( spNext == NULL ) return false;
      // Clean up old context at same level
      while( m_iDepth > 0 && m_aStack.GetSize() > m_iDepth ) m_aStack.RemoveAt( m_aStack.GetSize() - 1 );
      // Push new context
      CONTEXT c;
      c.bstrName = bstrName;
      c.spNodes = spNodes;
      c.iIndex = 0;
      m_aStack.Add(c);
   }
   m_iDepth++;
   m_spNode = spNext;
   m_spAttribs.Release();
   m_spNode->get_attributes(&m_spAttribs);
   return true;
}

bool CXmlSerializer::ReadGroupEnd()
{
   if( m_spNode == NULL ) return false;
   CComPtr<IXMLDOMNode> spParent;
   m_spNode->get_parentNode(&spParent);
   ATLASSERT(spParent);
   if( spParent == NULL ) return false;
   m_spNode = spParent;
   m_spAttribs.Release();
   // Pop context off stack
   while( m_iDepth > 0 && m_aStack.GetSize() > m_iDepth ) m_aStack.RemoveAt( m_aStack.GetSize() - 1 );
   m_iDepth--;
   return true;
}

bool CXmlSerializer::Read(LPCTSTR pstrName, LPTSTR pstrValue, UINT cchMax)
{
   ATLASSERT(!::IsBadStringPtr(pstrName,-1));
   ATLASSERT(!::IsBadWritePtr(pstrValue,cchMax*sizeof(TCHAR)));
   ATLASSERT(cchMax>0);
   ATLASSERT(m_spAttribs);
   USES_CONVERSION;
   *pstrValue = _T('\0');
   if( m_spAttribs == NULL ) return false;
   // Get the attribute
   CComPtr<IXMLDOMNode> spNode;
   HRESULT Hr = m_spAttribs->getNamedItem(T2OLE(const_cast<LPTSTR>(pstrName)), &spNode);
   if( Hr != S_OK ) return false;
   // Read the value
   CComBSTR bstr;
   if( FAILED( spNode->get_text(&bstr) ) ) return false;
   _tcsncpy(pstrValue, OLE2CT(bstr), cchMax);
   return true;
}

bool CXmlSerializer::Read(LPCTSTR pstrName, CString& sValue)
{
   TCHAR szValue[1024] = { 0 };
   if( !Read(pstrName, szValue, (sizeof(szValue)/sizeof(TCHAR)) - 1) ) {
      sValue = _T("");
      return false;
   }
   sValue = szValue;
   return true;
}

bool CXmlSerializer::Read(LPCTSTR pstrName, SYSTEMTIME& stValue)
{
   ::ZeroMemory(&stValue, sizeof(stValue));
   TCHAR szValue[64] = { 0 };
   if( !Read(pstrName, szValue, sizeof(szValue)/sizeof(TCHAR)) ) return false;
   // TODO: Write it in actual XML date format!
   CComVariant v = szValue;
   if( FAILED( v.ChangeType(VT_DATE) ) ) return false;
   if( ::VariantTimeToSystemTime(v.date, &stValue) == 0 ) return false;
   return true;
}

bool CXmlSerializer::Read(LPCTSTR pstrName, long& lValue)
{
   TCHAR szValue[20] = { 0 };
   if( !Read(pstrName, szValue, sizeof(szValue)/sizeof(TCHAR)) ) {
      lValue = 0;
      return false;
   }
   lValue = _ttol(szValue);
   return true;
}

bool CXmlSerializer::Read(LPCTSTR pstrName, bool& bValue)
{
   TCHAR szValue[10];
   if( !Read(pstrName, szValue, sizeof(szValue)/sizeof(TCHAR)) ) {
      bValue = false;
      return false;
   }
   bValue = _tcscmp(szValue, _T("true")) == 0;
   return true;
}

bool CXmlSerializer::WriteGroupBegin(LPCTSTR pstrName)
{
   ATLASSERT(!::IsBadStringPtr(pstrName,-1));
   CComPtr<IXMLDOMElement> spElement;
   if( FAILED( m_spDoc->createElement(CComBSTR(pstrName), &spElement) ) ) return false;
   CComPtr<IXMLDOMNode> spNode;
   if( FAILED( m_spNode->appendChild(spElement, &spNode) ) ) return false;
   m_spNode = spNode;
   m_spAttribs.Release();
   m_spNode->get_attributes(&m_spAttribs);
   return true;
}

bool CXmlSerializer::WriteGroupEnd()
{
   CComPtr<IXMLDOMNode> spNode;
   m_spAttribs.Release();
   m_spNode->get_parentNode(&spNode);
   m_spNode = spNode;
   return true;
}

bool CXmlSerializer::Write(LPCTSTR pstrName, LPCTSTR pstrValue)
{
   ATLASSERT(!::IsBadStringPtr(pstrName,-1));
   ATLASSERT(!::IsBadStringPtr(pstrValue,-1));
   ATLASSERT(m_spAttribs);
   if( m_spAttribs == NULL ) return false;
   CComPtr<IXMLDOMAttribute> spAttrib;
   if( FAILED( m_spDoc->createAttribute(CComBSTR(pstrName), &spAttrib) ) ) return false;
   CComBSTR bstr = pstrValue;
   if( FAILED( spAttrib->put_text(bstr) ) ) return false;
   CComPtr<IXMLDOMNode> spNode;
   if( FAILED( m_spAttribs->setNamedItem(spAttrib, &spNode) ) ) return false;
   return true;
}

bool CXmlSerializer::Write(LPCTSTR pstrName, SYSTEMTIME stValue)
{   
   DATE dDate = 0.0;
   if( ::SystemTimeToVariantTime(&stValue, &dDate) == 0 ) return false;
   CComVariant v = dDate;
   v.vt = VT_DATE;
   if( FAILED( v.ChangeType(VT_BSTR) ) ) return false;
   USES_CONVERSION;
   return Write(pstrName, OLE2CT(v.bstrVal));
}

bool CXmlSerializer::Write(LPCTSTR pstrName, long lValue)
{
   TCHAR szValue[20];
   ::wsprintf(szValue, _T("%ld"), lValue);
   return Write(pstrName, szValue);
}

bool CXmlSerializer::Write(LPCTSTR pstrName, bool bValue)
{
   return Write(pstrName, bValue ? _T("true") : _T("false"));
}


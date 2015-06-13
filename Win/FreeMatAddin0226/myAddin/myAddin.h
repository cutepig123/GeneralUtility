// myAddin.h : main header file for the myAddin DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CmyAddinApp
// See myAddin.cpp for the implementation of this class
//

class CmyAddinApp : public CWinApp
{
public:
	CmyAddinApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

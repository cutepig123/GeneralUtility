
// MFCApplication1_MsgThain.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMFCApplication1_MsgThainApp:
// See MFCApplication1_MsgThain.cpp for the implementation of this class
//

class CMFCApplication1_MsgThainApp : public CWinApp
{
public:
	CMFCApplication1_MsgThainApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMFCApplication1_MsgThainApp theApp;
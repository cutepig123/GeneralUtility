// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

#define __AFXWIN_H__
// Change these values to use different versions
#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0500

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define ASSERT
#define	afx_msg
#define TRACE
#define DECLARE_MESSAGE_MAP()		\
	public: \
	BOOL ProcessWindowMessage(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam,\
		_In_ LPARAM lParam, _Inout_ LRESULT& lResult, _In_ DWORD dwMsgMapID = 0);

class CWnd
{
};

#define	BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)	\
	BOOL CAboutDlg::ProcessWindowMessage(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam,\
		_In_ LPARAM lParam, _Inout_ LRESULT& lResult, _In_ DWORD dwMsgMapID) \
	{ \
		BOOL bHandled = TRUE; \
		(hWnd); \
		(uMsg); \
		(wParam); \
		(lParam); \
		(lResult); \
		(bHandled); \
		switch(dwMsgMapID) \
		{ \
		case 0:

#define	END_MESSAGE_MAP()		\
					break; \
		default: \
			ATLTRACE(ATL::atlTraceWindowing, 0, _T("Invalid message map ID (%i)\n"), dwMsgMapID); \
			ATLASSERT(FALSE); \
			break; \
		} \
		return FALSE; \
	}


#define ON_BN_CLICKED(id, func) \
	if(uMsg == WM_COMMAND && id == LOWORD(wParam)) \
			{ \
		bHandled = TRUE; \
		func(); \
		if(bHandled) \
			return TRUE; \
			}


HINSTANCE	AfxFindResourceHandle(LPCTSTR lpszName, LPCTSTR lpszType);

class CWinApp
{

public:
	CWinApp();
	HICON LoadIcon(LPCTSTR lpszResourceName) const;

	void Init();

	virtual BOOL InitInstance() = 0{ return TRUE; };

	static CWinApp *g_pstApp;
};

CWinApp *AfxGetApp();

template <class T>
class CDialogEx : public CDialogImpl<T>
{
public:
	void OnOK()
	{
		EndDialog(0);
	}
};


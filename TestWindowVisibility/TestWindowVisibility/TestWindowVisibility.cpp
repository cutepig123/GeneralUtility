// TestWindowVisibility.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "windows.h"

//http://bobmoore.mvps.org/Win32/w32tip72.htm
BOOL IsWindowCentreVisibleToUser(HWND hWnd)
{
	BOOL bRetVal = FALSE;
	RECT rcWnd;
	POINT pt;

	if (IsWindow(hWnd))
	{
		GetWindowRect(hWnd, &rcWnd);
		pt.x = rcWnd.left + (rcWnd.right - rcWnd.left) / 2;
		pt.y = rcWnd.top + (rcWnd.bottom - rcWnd.top) / 2;

		bRetVal = (WindowFromPoint(pt) == hWnd);
	}
	return bRetVal;
}

#define IWVTU_FULL    1
#define IWVTU_PARTIAL 2
#define IWVTU_HIDDEN  3
#define IWVTU_ERROR   4
#define IWVTU_NOT_WINDOW   5

const char *GetIsWindowVisibleToUserSts(UINT n)
{
	switch (n)
	{
#define	CHK_STS(x)	case x: return #x;
	CHK_STS(IWVTU_FULL);
	CHK_STS(IWVTU_PARTIAL);
	CHK_STS(IWVTU_HIDDEN);
	CHK_STS(IWVTU_ERROR);
	CHK_STS(IWVTU_NOT_WINDOW);
	default:
		return "IWVTU_ERROR";
		break;
	}
#undef CHK_STS
}

UINT IsWindowVisibleToUser(HWND hWnd)
{
	UINT uRetVal = IWVTU_NOT_WINDOW;
	int  iClipRes;
	HDC hdcWnd;
	RECT rcWndClient;
	RECT rcWndClip;

	if (IsWindow(hWnd))
	{
		// This uses the clipping system to determine if the user
		// can see all or part of the window. The only problem
		// comes when the region returned is a rectangle, i.e. the
		// return code from GetClipBox is SIMPLEREGION. In that
		// case we check against the client area rectangle, on the
		// basis that what we _really_ care about is the contents
		// of the window, not the caption bar.

		hdcWnd = GetDC(hWnd);
		GetClientRect(hWnd, &rcWndClient);
		iClipRes = GetClipBox(hdcWnd, &rcWndClip);
		ReleaseDC(hWnd, hdcWnd);

		switch (iClipRes)
		{
		case NULLREGION:
			uRetVal = IWVTU_HIDDEN;
			break;
		case SIMPLEREGION:
			if (EqualRect(&rcWndClip, &rcWndClient))
				uRetVal = IWVTU_FULL;
			else
				uRetVal = IWVTU_PARTIAL;
			break;
		case COMPLEXREGION:
			uRetVal = IWVTU_PARTIAL;
			break;
		default:
			uRetVal = IWVTU_ERROR;
			break;
		}
	}
	
	return uRetVal;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int nWnd = 0;

	while (1)
	{
		printf("Input HWND (Hex):");
		if (scanf_s("%x", &nWnd) != 1 || nWnd == 0)
			break;
		HWND hWnd = (HWND)nWnd;
		BOOL bSts1 = IsWindowCentreVisibleToUser(hWnd);
		UINT bSts2 = IsWindowVisibleToUser(hWnd);
		BOOL bSts3 = IsWindowVisible(hWnd);

		printf("HWND %x\n", hWnd);
		printf("IsWindowCentreVisibleToUser return %d\n", bSts1);
		printf("IsWindowVisibleToUser return %d (%s)\n", bSts2, GetIsWindowVisibleToUserSts(bSts1));
		printf("IsWindowVisible return %d\n", bSts3);

	}
	return 0;
}


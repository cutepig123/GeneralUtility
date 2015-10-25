#include <Windows.h>
#include <assert.h>
#include <map> 
#include <tchar.h>
#include <Windowsx.h>

#define	Verify(x)	{int t=(x); if(!(t)){assert(0);}}
#define ASSERT assert

class CRgn
{
	
public:
	HRGN m_hObject;

	CRgn() :m_hObject(NULL)
	{
	}

	void CreateRectRgnIndirect(LPCRECT lpRect)
	{
		m_hObject = ::CreateRectRgnIndirect(lpRect);
	}
	void CreateRectRgn(int x1,
		int y1,
		int x2,
		int y2)
	{
		m_hObject = ::CreateRectRgn(x1, y1, x2, y2);
	}
	void CombineRgn(CRgn* pRgn1,
		CRgn* pRgn2,
		int nCombineMode)
	{
		::CombineRgn(m_hObject, pRgn1->m_hObject, pRgn2->m_hObject, nCombineMode);
	}
	void SetRectRgn(LPCRECT lpRect)
	{
		::SetRectRgn(m_hObject, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom);
	}
};

HBRUSH GetHalftoneBrush()
{
	static HBRUSH _afxHalftoneBrush = NULL;

	//AfxLockGlobals(CRIT_HALFTONEBRUSH);
	if (_afxHalftoneBrush == NULL)
	{
		WORD grayPattern[8];
		for (int i = 0; i < 8; i++)
			grayPattern[i] = (WORD)(0x5555 << (i & 1));
		HBITMAP grayBitmap = CreateBitmap(8, 8, 1, 1, grayPattern);
		if (grayBitmap != NULL)
		{
			_afxHalftoneBrush = ::CreatePatternBrush(grayBitmap);
			DeleteObject(grayBitmap);
		}
	}
	//if (!_afxWingdixTerm)
	//	_afxWingdixTerm = (char)!atexit(&AfxWingdixTerm);
	//AfxUnlockGlobals(CRIT_HALFTONEBRUSH);

	return (_afxHalftoneBrush);
}

void DrawDragRect(HDC hDC, LPCRECT lpRect, SIZE size,
	LPCRECT lpRectLast, SIZE sizeLast, HBRUSH pBrush, HBRUSH pBrushLast)
{
	//ASSERT(AfxIsValidAddress(lpRect, sizeof(RECT), FALSE));
	//ASSERT(lpRectLast == NULL ||
	//	AfxIsValidAddress(lpRectLast, sizeof(RECT), FALSE));

	// first, determine the update region and select it
	CRgn rgnNew;
	CRgn rgnOutside, rgnInside;
	rgnOutside.CreateRectRgnIndirect(lpRect);
	RECT rect = *lpRect;
	InflateRect(&rect, -size.cx, -size.cy);
	IntersectRect(&rect, &rect, lpRect);
	rgnInside.CreateRectRgnIndirect(&rect);
	rgnNew.CreateRectRgn(0, 0, 0, 0);
	rgnNew.CombineRgn(&rgnOutside, &rgnInside, RGN_XOR);

	HBRUSH pBrushOld = NULL;
	if (pBrush == NULL)
	{
		pBrush = GetHalftoneBrush();
	}

	//ENSURE(pBrush);

	if (pBrushLast == NULL)
	{
		pBrushLast = pBrush;
	}

	CRgn rgnLast, rgnUpdate;
	if (lpRectLast != NULL)
	{
		// find difference between new region and old region
		rgnLast.CreateRectRgn(0, 0, 0, 0);
		rgnOutside.SetRectRgn(lpRectLast);
		rect = *lpRectLast;
		InflateRect(&rect, -sizeLast.cx, -sizeLast.cy);
		IntersectRect(&rect, &rect, lpRectLast);
		rgnInside.SetRectRgn(&rect);
		rgnLast.CombineRgn(&rgnOutside, &rgnInside, RGN_XOR);

		// only diff them if brushes are the same
		if (pBrush == pBrushLast)
		{
			rgnUpdate.CreateRectRgn(0, 0, 0, 0);
			rgnUpdate.CombineRgn(&rgnLast, &rgnNew, RGN_XOR);
		}
	}
	if (pBrush != pBrushLast && lpRectLast != NULL)
	{
		// brushes are different -- erase old region first
		SelectClipRgn(hDC, rgnLast.m_hObject);
		GetClipBox(hDC, &rect);
		pBrushOld = (HBRUSH)SelectObject(hDC, pBrushLast);
		PatBlt(hDC, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, PATINVERT);
		SelectObject(hDC, pBrushOld);
		pBrushOld = NULL;
	}

	// draw into the update/new region
	SelectClipRgn(hDC, rgnUpdate.m_hObject != NULL ? rgnUpdate.m_hObject : rgnNew.m_hObject);
	GetClipBox(hDC, &rect);
	pBrushOld = (HBRUSH)SelectObject(hDC, pBrush);
	PatBlt(hDC, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, PATINVERT);

	// cleanup DC
	if (pBrushOld != NULL)
		SelectObject(hDC, pBrushOld);
	SelectClipRgn(hDC, NULL);
}


class RectTracker
{
	RECT m_rect;
	//POINT m_pos;


	
public:
	RectTracker()
	{
		m_rect.left = m_rect.top = 0;
		m_rect.bottom = m_rect.right = 200;
		//m_pos.x = m_pos.y = 0;
	}

	void Draw(HDC dc)
	{
		Verify(Rectangle(dc, m_rect.left , m_rect.top , m_rect.right , m_rect.bottom ));
	}

	enum HitTestType
	{
		HitMiddle,
		HitNone
	};

	HitTestType HitTest(POINT pt)
	{
		if (pt.x >= m_rect.left  && pt.x <= m_rect.right  && pt.y >= m_rect.top  && pt.y <= m_rect.bottom )
			return HitMiddle;
		return HitNone;
	}

	void Track(HWND hWnd, POINT initialPt)
	{
		HDC dc = GetDC(hWnd);
		
		SetCapture(hWnd);

		BOOL bExit = FALSE;
		RECT m_rectLast = m_rect;

		while (!bExit)
		{
			MSG msg;
			Verify(GetMessage(&msg, hWnd, 0, 0));

			switch (msg.message)
			{
				case WM_MOUSEMOVE:
				{
					POINTS pt = MAKEPOINTS(msg.lParam);
					RECT rect = m_rect;
					rect.left += pt.x - initialPt.x;
					rect.right += pt.x - initialPt.x;
					rect.top += pt.y - initialPt.y;
					rect.bottom += pt.y - initialPt.y;

					SIZE size = { 2, 2 };
					DrawDragRect(dc, &rect, size, &m_rectLast, size, NULL, NULL);
					m_rectLast = rect;
					break;
				}
				case WM_LBUTTONUP:
				{
					POINTS pt = MAKEPOINTS(msg.lParam);
					RECT rect = m_rect;
					rect.left += pt.x - initialPt.x;
					rect.right += pt.x - initialPt.x;
					rect.top += pt.y - initialPt.y;
					rect.bottom += pt.y - initialPt.y;
					m_rect = rect;
					bExit = TRUE;
					InvalidateRect(hWnd, NULL, TRUE);
					break;
				}
			}
			
		}

		ReleaseCapture();
		ReleaseDC(hWnd,dc);
	}
};

class Window
{
public:
    Window();
    ~Window();

public:
    BOOL Create();

protected:
    LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
    HWND			m_hWnd;
	RectTracker		m_tracker;

protected:
    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static std::map<HWND, Window *> m_sWindows;
};

std::map<HWND, Window *> Window::m_sWindows;

Window::Window()
{

}

Window::~Window()
{
   
}

BOOL Window::Create()
{
    LPCTSTR lpszClassName = _T("ClassName");
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEX wcex    = { sizeof(WNDCLASSEX) };
	wcex.lpfnWndProc = StaticWndProc;
    wcex.hInstance     = hInstance;
    wcex.lpszClassName = lpszClassName;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassEx(&wcex);

    m_hWnd = CreateWindow(lpszClassName, NULL, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	m_sWindows.insert(std::make_pair(m_hWnd, this));

    if (m_hWnd == NULL)
    {
        return FALSE;
    }
    
    ShowWindow(m_hWnd, SW_SHOW);
    UpdateWindow(m_hWnd);

    return TRUE;
}

LRESULT Window::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
		case WM_LBUTTONDOWN:
		{
			POINTS pts = MAKEPOINTS(lParam);
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			
			assert(xPos == pts.x);
			POINT pt;
			pt.x = pts.x;
			pt.y = pts.y;
			if (m_tracker.HitTest(pt) == m_tracker.HitMiddle)
			{
				m_tracker.Track(m_hWnd, pt);
			}
		}
			break;
		case WM_LBUTTONUP:
			MessageBox(m_hWnd, _T("LButtonUp"), _T("Message"), MB_OK | MB_ICONINFORMATION);
			break;
		case WM_RBUTTONUP:
			MessageBox(m_hWnd, _T("RButtonUp"), _T("Message"), MB_OK | MB_ICONINFORMATION);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_PAINT:
		{
			HDC dc;
			PAINTSTRUCT paint;
			dc = BeginPaint(m_hWnd, &paint);
			m_tracker.Draw(dc);
			EndPaint(m_hWnd, &paint);
		}
			break;

		default:
			break;
    }

    return DefWindowProc(m_hWnd, message, wParam, lParam);
}


LRESULT CALLBACK Window::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto it = m_sWindows.find(hWnd);
	if (it == m_sWindows.end())
	{
		//assert(0);
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
    return (it->second)->WndProc(message, wParam, lParam);
}

int main()
{
    Window wnd;
    wnd.Create();

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
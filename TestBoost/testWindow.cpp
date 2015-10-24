#include <Windows.h>
#include <assert.h>
#include <map> 
#include <tchar.h>

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
    HWND          m_hWnd;
    
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
    case WM_LBUTTONUP:
        MessageBox(m_hWnd, _T("LButtonUp"), _T("Message"), MB_OK | MB_ICONINFORMATION);
        break;
    case WM_RBUTTONUP:
        MessageBox(m_hWnd, _T("RButtonUp"), _T("Message"), MB_OK | MB_ICONINFORMATION);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
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
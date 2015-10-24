#include <Windows.h>
#include <assert.h>
#include <map> 
#include <tchar.h>
#pragma pack(push,1)
typedef struct _StdCallThunk
{
    DWORD   m_mov;
    DWORD   m_this;
    BYTE    m_jmp;
    DWORD   m_relproc;

} StdCallThunk;
#pragma pack(pop)

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
    StdCallThunk *m_pThunk;

protected:
    static LRESULT CALLBACK TempWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static std::map<DWORD, Window *> m_sWindows;
};

std::map<DWORD, Window *> Window::m_sWindows;

Window::Window()
{

}

Window::~Window()
{
    VirtualFree(m_pThunk, sizeof(StdCallThunk), MEM_RELEASE);
}

BOOL Window::Create()
{
    LPCTSTR lpszClassName = _T("ClassName");
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEX wcex    = { sizeof(WNDCLASSEX) };
    wcex.lpfnWndProc   = TempWndProc;
    wcex.hInstance     = hInstance;
    wcex.lpszClassName = lpszClassName;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassEx(&wcex);

    DWORD dwThreadId = GetCurrentThreadId();
    m_sWindows.insert(std::make_pair(dwThreadId, this));

    m_pThunk = (StdCallThunk *)VirtualAlloc(NULL, sizeof(StdCallThunk), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    m_pThunk->m_mov = 0x042444c7;
    m_pThunk->m_jmp = 0xe9;

    m_hWnd = CreateWindow(lpszClassName, NULL, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

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

LRESULT CALLBACK Window::TempWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    std::map<DWORD, Window *>::iterator it = m_sWindows.find(GetCurrentThreadId());
    assert(it != m_sWindows.end() && it->second != NULL);

    Window *pThis = it->second;
    m_sWindows.erase(it);

    WNDPROC pWndProc = (WNDPROC)pThis->m_pThunk;

    pThis->m_pThunk->m_this = (DWORD)pThis;
    pThis->m_pThunk->m_relproc = (DWORD)&Window::StaticWndProc - ((DWORD)pThis->m_pThunk + sizeof(StdCallThunk));

    pThis->m_hWnd = hWnd;
    SetWindowLong(hWnd, GWL_WNDPROC, (LONG)pWndProc);

    return pWndProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK Window::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return ((Window *)hWnd)->WndProc(message, wParam, lParam);
}

//int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
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
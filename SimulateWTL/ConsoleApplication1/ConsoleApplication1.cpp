// ConsoleApplication1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "windows.h"

typedef int MY_HWND;

#define	MY_WM_CREATE	1
#define	MY_WM_TEST		2

typedef void(CALLBACK* MY_WNDPROC)(MY_HWND, int Msg);

////////////////////////////////////////////////////////
// 模拟windows api
////////////////////////////////////////////////////////
static MY_WNDPROC g_WNDPROC[10] = { 0 };
static int g_nWNDPROC = 0;

int MyRegisterClassEx(MY_WNDPROC Proc)
{
	g_WNDPROC[g_nWNDPROC] = Proc;
	return g_nWNDPROC++;
}


static MY_HWND g_Window[10] = { 0 };
static int g_nWindow = 0;



void SetWindowLong(MY_HWND hWnd, MY_WNDPROC Proc)
{
	int nClassID = MyRegisterClassEx(Proc);
	g_Window[hWnd] = nClassID;
}

void SendMsg(MY_HWND hWnd, int nMsg)
{
	int nClsID = g_Window[hWnd];
	MY_WNDPROC Proc = g_WNDPROC[nClsID];
	Proc(hWnd, nMsg);
}

MY_HWND MyCreateWindow(int nClsID)
{
	g_Window[g_nWindow] = nClsID;
	g_nWindow++;
	SendMsg(g_nWindow - 1, MY_WM_CREATE);
	return g_nWindow - 1;
}

////////////////////////////////////////////////////////
// 模拟wtl的做法
////////////////////////////////////////////////////////

#pragma pack(push,1)
struct _stdcallthunk
{
	DWORD   m_mov;          // mov dword ptr [esp+0x4], pThis (esp+0x4 is hWnd)
	DWORD   m_this;         //
	BYTE    m_jmp;          // jmp MY_WNDPROC
	DWORD   m_relproc;      // relative jmp
	BOOL Init(
		 DWORD_PTR proc,
		 void* pThis)
	{
		m_mov = 0x042444C7;  //C7 44 24 0C
		m_this = PtrToUlong(pThis);
		m_jmp = 0xe9;
		m_relproc = DWORD((INT_PTR)proc - ((INT_PTR)this + sizeof(_stdcallthunk)));
		// write block from data cache and
		//  flush from instruction cache
		FlushInstructionCache(GetCurrentProcess(), this, sizeof(_stdcallthunk));
		return TRUE;
	}
	//some thunks will dynamically allocate the memory for the code
	void* GetCodeAddress()
	{
		return this;
	}
	_Ret_opt_bytecount_x_(sizeof(_stdcallthunk)) void* operator new(_In_ size_t)
	{
		return VirtualAlloc(NULL,
			4096,
			MEM_COMMIT,
			PAGE_EXECUTE_READWRITE);
		//return __AllocStdCallThunk();
	}
	/*void operator delete(_In_opt_ void* pThunk)
	{
		__FreeStdCallThunk(pThunk);
	}*/
};
#pragma pack(pop)

struct MyWindow
{
	_stdcallthunk *pstThunk;
	MY_HWND m_hwnd;

	MyWindow() :pstThunk(new _stdcallthunk), m_hwnd(0)
	{

	}
	static void CALLBACK WindowProc(MY_HWND hwnd, int nMsg)
	{
		MyWindow *pThis = (MyWindow*)hwnd;

		printf("WindowProc pThis %p pThis->m_hwnd %d, nMsg %d\n", pThis, pThis->m_hwnd, nMsg);
	}
	
	virtual MY_WNDPROC GetWindowProc()
	{
		return WindowProc;
	}
};

void CALLBACK StartWindowProc(MY_HWND hwnd, int nMsg)
{
	MyWindow *pThis = new MyWindow;

	printf("StartWindowProc pThis %p hwnd %d nMsg %d\n", pThis, hwnd, nMsg);
	
	pThis->m_hwnd = hwnd;
	pThis->pstThunk->Init((DWORD_PTR)pThis->GetWindowProc(), pThis);
	MY_WNDPROC pProc = (MY_WNDPROC)pThis->pstThunk->GetCodeAddress();
	SetWindowLong(hwnd, pProc);
}

int _tmain(int argc, _TCHAR* argv[])
{
	int nClassID = MyRegisterClassEx(StartWindowProc);
	int nWinID = MyCreateWindow(nClassID);
	
	SendMsg(nWinID, MY_WM_TEST);

	return 0;
}


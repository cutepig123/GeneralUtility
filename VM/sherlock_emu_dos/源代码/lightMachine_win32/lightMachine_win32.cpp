// lightMachine_win32.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "lightMachine_win32.h"
#include "PCMachine.h"
#include<process.h>
#include<windows.h>
#include<string>
#include"PCMachine.h"
#include "LogRecord/LogMaker.h"
using namespace std;
#define MAX_LOADSTRING 100
const int TIMER_REPAINT = 1;
const int TIMER_RTC = 2;
char displayTest[1000];
bool isReady=false;
// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������
VOID startLightMachine(PVOID pvoid);
HWND displayhWnd;
unsigned int startTimeCount = 0;
PCMachine *pPC;
int char2ScanCode(char &str);
// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LIGHTMACHINE_WIN32, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
	
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LIGHTMACHINE_WIN32));
	_beginthread(startLightMachine, 0, NULL);
//	Sleep(1000);
	
	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	delete pPC;
	return (int) msg.wParam;
	
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
//


VOID startLightMachine(PVOID pvoid)
{
	
	pPC->run();
	return ;
}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LIGHTMACHINE_WIN32));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_LIGHTMACHINE_WIN32);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 640, 320, NULL, NULL, hInstance, NULL);
	
	

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void pasteWordToVirtualMachine(HWND hwnd)
{
	if(!IsClipboardFormatAvailable(CF_TEXT)) 
		return;
	if(!OpenClipboard(hwnd))
		return;
	HGLOBAL hGlobal = GetClipboardData(CF_TEXT); 
	PCHAR pGlobal = (char*)GlobalLock(hGlobal);
	int textSize = GlobalSize(hGlobal);
	char* pText = (char*) malloc(textSize);
	strcpy(pText, pGlobal);
	for(int i=0; i<textSize; i++)
	{
		int scanCode = char2ScanCode(*(pText+i));
		PostMessage(hwnd, WM_CHAR, *(pText+i), scanCode<<16);
	}
	GlobalUnlock(hGlobal);
	CloseClipboard();
	free(pText);
	return;
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		displayhWnd = hWnd;
		pPC = new PCMachine();
		SetTimer(hWnd, TIMER_REPAINT, 50, NULL);//����ʱ�ӣ�ÿ��ˢ��ʮ���������Ļ
		SetTimer(hWnd, TIMER_RTC, 55,NULL);//����BIOS RAM ʱ��
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_PASTE:
			pasteWordToVirtualMachine(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_TIMER:
		//SendMessage(hWnd, WM_PAINT,0,0);
		switch(wParam)
		{
		case TIMER_REPAINT:
			startTimeCount++;
			::pPC->repaintScreen();
			break;
		case TIMER_RTC:
			::pPC->updateTime();
			break;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if(isReady==true)
			::pPC->repaintScreen();
		EndPaint(hWnd, &ps);
		
		break;
	case WM_CHAR:
		::pPC->kbFace.handleKeyBoardChar(wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		::pPC->kbFace.handleKeyBoardPress(message, wParam, lParam);
		break;
	case WM_SETFOCUS:
		if(isReady==true)
			::pPC->kbFace.updateKBStatus(WM_KEYUP, wParam, lParam);
		break;
	case WM_DESTROY:
		LogMaker::logEnd();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}






int char2ScanCode(char &str)
{
	switch(str)
	{
	case 'A':
	case 'a':
		return 0x1E;
	case 'B':
	case 'b':
		return 0x30;
	case 'C':
	case 'c':
		return 0x2E;
	case 'D':
	case 'd':
		return 0x20;
	case 'E':
	case 'e':
		return 0x12;
	case 'F':
	case 'f':
		return 0x21;
	case 'G':
	case 'g':
		return 0x22;
	case 'H':
	case 'h':
		return 0x23;
	case 'I':
	case 'i':
		return 0x17;
	case 'J':
	case 'j':
		return 0x24;
	case 'K':
	case 'k':
		return 0x25;
	case 'L':
	case 'l':
		return 0x26;
	case 'M':
	case 'm':
		return 0x32;
	case 'N':
	case 'n':
		return 0x31;
	case 'O':
	case 'o':
		return 0x18;
	case 'P':
	case 'p':
		return 0x19;
	case 'Q':
	case 'q':
		return 0x10;
	case 'R':
	case 'r':
		return 0x13;
	case 'S':
	case 's':
		return 0x1f;
	case 'T':
	case 't':
		return 0x14;
	case 'U':
	case 'u':
		return 0x16;
	case 'V':
	case 'v':
		return 0x2f;
	case 'W':
	case 'w':
		return 0x11;
	case 'X':
	case 'x':
		return 0x2d;
	case 'Y':
	case 'y':
		return 0x15;
	case 'Z':
	case 'z':
		return 0x2c;
	case '0':
	case ')':
		return 0xb;
	case '1':
	case '!':
		return 0x2;
	case '2':
	case '@':
		return 0x3;
	case '3':
	case '#':
		return 0x4;
	case '4':
	case '$':
		return 0x5;
	case '5':
	case '%':
		return 0x6;
	case '6':
	case '^':
		return 0x7;
	case '7':
	case '&':
		return 0x8;
	case '8':
	case '*':
		return 0x9;
	case '9':
	case '(':
		return 0xA;
	case '\'':
	case '"':
		return 0x28;
	case ',':
	case '<':
		return 0x33;
	case '-':
	case '_':
		return 0x0C;
	case '.':
	case '>':
		return 0x34;
	case '/':
	case '?':
		return 0x35;
	case ':':
	case ';':
		return 0x27;
	case '=':
	case '+':
		return 0x0D;
	case '[':
	case '{':
		return 0x1A;
	case '\\':
	case '|':
		return 0x2B;
	case ']':
	case '}':
		return 0x1B;
	case '`':
	case '~':
		return 0x29;
	case ' '://space
		return 0x39;
	case 0x08://BackSpace
		return 0x0E;
	case '/t':
		return 0x0F;
	case '/r'://����������з����Ͱ�ԭASCII���0x0D����ʾ�س�
		str = 0x0D;
		return 0x1C;
	}
}
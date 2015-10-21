// RectTrackDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RectTrack.h"
#include "RectTrackDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CRectTrackDlg dialog



CRectTrackDlg::CRectTrackDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRectTrackDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	 m_rectTracker.m_rect.SetRect(0,0,100,100); 
     m_rectTracker.m_nStyle=CMyRectTracker::resizeInside|CMyRectTracker::dottedLine; 
	 m_rectTracker.SetText(_T("MyRect"));
}

void CRectTrackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRectTrackDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()


// CRectTrackDlg message handlers

BOOL CRectTrackDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRectTrackDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRectTrackDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//CDialog::OnPaint();
		 // TODO: add draw code for native data here 
		CPaintDC dc(this); // device context for painting
		CBrush brush(RGB(0,0,255));//生成蓝色的画刷； 
		CBrush  *oldBrush=dc.SelectObject(&brush);//将画刷选进dc; 
		//CRect rect; 
		//GetTrueRect(&rect)得到CMyRectTracker中的m_rect的大小，将其传递给rect；
		//m_rectTracker.GetTrueRect(&rect);
		//if(bDraw)
		//	dc.Ellipse (rect);//画椭圆； 
		//Draw tracking rectangle. 
		m_rectTracker.Draw(&dc);//这句画才真正的将这个四边形画出来；
		//Select blue brush out of device context.
		dc.SelectObject (oldBrush);//恢复原来的画刷； 
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRectTrackDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRectTrackDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	int nIn; //定义一个鼠标的点击值；
	nIn=m_rectTracker.HitTest(point); //看看点到了哪了
	if(nIn<0)  //不在四边形区域内；
	{
		//CMyRectTracker  temp;
		//temp.TrackRubberBand(this,point,TRUE);
		//temp.m_rect.NormalizeRect();
		//CMyRectTracker interRect; 
		////在建立一个CMyRectTracker;用于记录鼠标与椭圆的交集。
		//if(interRect.m_rect.IntersectRect(temp.m_rect,m_rectTracker.m_rect))
		//	bDraw=TRUE; //如果有交集，则画四边形的边界，说明选择了椭圆
		//else   bDraw=FALSE;
		//Invalidate(); //引起OnDraw函数的发生；
	}
	else 
		//在四边形区域内：
	{
		CClientDC dc(this);
		m_rectTracker.Draw(&dc);
		m_rectTracker.Track(this,point,TRUE);
		// Track()是CMyRectTracker中最富魅力的函数。它时时的改变调用者的m_rect;
		//bDraw=TRUE;
		Invalidate();
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CRectTrackDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnLButtonUp(nFlags, point);
}

BOOL CRectTrackDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	if (pWnd == this && m_rectTracker.SetCursor(this, nHitTest)) 
          return TRUE; 
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

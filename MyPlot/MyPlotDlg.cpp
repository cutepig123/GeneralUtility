// MyPlotDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MyPlot.h"
#include "MyPlotDlg.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define	MODE_THIS		0
#define	MODE_CONTROL	1
#define	MODE_CONTROL2	2

int g_mode =MODE_THIS;

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


// CMyPlotDlg dialog



CMyPlotDlg::CMyPlotDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyPlotDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMyPlotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combo1);
}

BEGIN_MESSAGE_MAP(CMyPlotDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_ERASEBKGND()
	ON_CBN_SELCHANGE(IDC_COMBO1, &CMyPlotDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CMyPlotDlg message handlers

BOOL CMyPlotDlg::OnInitDialog()
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
	m_combo1.AddString("MODE_THIS");
	m_combo1.AddString("MODE_CONTROL");
	m_combo1.AddString("MODE_CONTROL2");
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMyPlotDlg::OnSysCommand(UINT nID, LPARAM lParam)
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



void CMyPlotDlg::OnPaint() 
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
		if( g_mode ==MODE_THIS )
		{
			CPaintDC dc(this);
			dc.Rectangle(10,10,100,50);
			dc.TextOutA(20,20,"Test",4);
		}
		else if( g_mode ==MODE_CONTROL )
		{
			CPaintDC dc(GetDlgItem(IDC_STATIC_X));
			OnBnClickedOk();
		}

		else if( g_mode ==MODE_CONTROL2 )
		{
			CPaintDC pdc();
			CWindowDC dc(GetDlgItem(IDC_STATIC_X)); 
			dc.Rectangle(10,10,100,50);
			dc.TextOutA(20,20,"Test",4);
		}
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMyPlotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Plot(size_t n, float x[], float y[], CDC *pDC, CRect const &rc, CFont *pFont)
{
	float *x_min = std::min_element(x, x+n);
	float *x_max = std::max_element(x, x+n);
	float *y_min = std::min_element(y, y+n);
	float *y_max = std::max_element(y, y+n);

	float sca_x = (rc.Width())/(*x_max - *x_min);
	float sca_y = (rc.Height())/(*y_max - *y_min);

	// boundary rectangle
	pDC->Rectangle(&rc);

	// Dotted line grid
	size_t nx=10;
	size_t ny=10;

	CPen pen(PS_DASHDOT, 1, RGB(0,0,0));
	CPen *pPenBk = pDC->SelectObject(&pen);

	for( size_t j=0; j<nx; j++ )
	{
		float cx = rc.left + (float)j * rc.Width() / nx ;
		pDC->MoveTo( cx, rc.top );
		pDC->LineTo( cx, rc.bottom );
	}

	for( size_t j=0; j<ny; j++ )
	{
		float cy = rc.top +  (float)j * rc.Height() / ny;
		pDC->MoveTo( rc.left, cy );
		pDC->LineTo( rc.right, cy );
	}
	pDC->SelectObject(pPenBk);

	// data
	for( size_t i=0; i<n; i++ )
	{
		float cx = rc.left + sca_x * ( x[i] - x[0] );
		float cy = rc.top + sca_y * ( y[i] - y[0] );

		TRACE("%f %f\n", cx, cy);

		if( i==0 )
			pDC->MoveTo( cx, cy );
		else
			pDC->LineTo( cx, cy );

		CRect rc2;
		rc2.SetRect( cx-2, cy-2,cx+2, cy+2 );
		pDC->Rectangle(&rc2);
	}

	// axis label
	CString s;
	s.Format( "%g", *x_min );
	pDC->TextOut( rc.left, rc.bottom, s);

	s.Format( "%g", *x_max );
	pDC->TextOut( rc.right-30, rc.bottom, s);

	s.Format( "y" );
	pDC->TextOut( (rc.left + rc.right)/2-30, rc.bottom, s);

	LOGFONT logFont;
	pFont->GetLogFont(&logFont);
	logFont.lfOrientation = 900;
	logFont.lfEscapement = 900;
	CFont font;
	font.CreateFontIndirect( &logFont );

	CFont *pFontBk =pDC->SelectObject(&font);

	s.Format( "%g", *y_max );
	pDC->TextOut( rc.left-15, rc.bottom-10, s);

	s.Format( "%g", *y_min );
	pDC->TextOut( rc.left-15, rc.top+30, s);

	s.Format( "z(um)" );
	pDC->TextOut( rc.left-15, (rc.top+rc.bottom)/2+30, s);

	pDC->SelectObject(pFontBk);
}

void Plot(size_t n, float ax[], float ay[], CWnd *pWnd)
{
	CWindowDC dc(pWnd);
	CRect rc;
	pWnd->GetClientRect(rc);
	
	CFont *pFont = pWnd->GetFont();
	LOGFONT logFont;
	pFont->GetLogFont(&logFont);
	
	int PointSize = MulDiv(-logFont.lfHeight, 72, GetDeviceCaps(dc.m_hDC, LOGPIXELSY));

	rc.DeflateRect( PointSize+5, PointSize+5 );
	
	CFont *pFontBk =dc.SelectObject(pFont);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255, 0, 0));
	Plot( n, ax, ay, &dc, rc,pFont);
	dc.SelectObject(pFontBk);
}

void CMyPlotDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	float ay[] = {1,2,3,4,5,9,4,5,2};
	float ax[100];
	int n=sizeof(ay)/sizeof(ay[0]);

	for(int i=0; i<n; i++)
		ax[i] = i;
	//OnOK();
	CWnd *pWnd = GetDlgItem(IDC_STATIC_X);

	Plot(n,ax,ay,pWnd);
}


BOOL CMyPlotDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return CDialog::OnEraseBkgnd(pDC);
}


void CMyPlotDlg::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
	g_mode =m_combo1.GetCurSel();
	Invalidate();
}

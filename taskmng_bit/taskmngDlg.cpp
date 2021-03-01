// taskmngDlg.cpp : implementation file
//

#include "stdafx.h"
#include "taskmng.h"
#include "taskmngDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskmngDlg dialog

CTaskmngDlg::CTaskmngDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTaskmngDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTaskmngDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTaskmngDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskmngDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTaskmngDlg, CDialog)
	//{{AFX_MSG_MAP(CTaskmngDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskmngDlg message handlers

BOOL CTaskmngDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	HandleTask_Calc();
	SetTimer(100, 60*1000, NULL);
//	HandleTask_Test();
//	SetTimer(200, 5*60*1000, NULL);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTaskmngDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTaskmngDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

#include <sys/types.h>
#include <sys/stat.h>
void CTaskmngDlg::HandleTask_Calc()
{
	time_t tt = time(NULL);
	char path[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH, path);

	/*
	long nRefreshTimet_Bit = GetPrivateProfileInt("POOL","REFRESH",0,(CString)path+"\\PoolInfo_Bit.ini");
	if(tt-nRefreshTimet_Bit>=300)
		::ShellExecute(NULL, _T("open"), "restart_bit.bat", NULL, NULL, SW_SHOW);
	*/

	struct _stat buf;
	int result;
	char filename[MAX_PATH]={0};
	sprintf(filename, "%s\\MarketTicker.txt", path);
	result = _stat( filename, &buf );
	if( result == 0 )
	{
		CTime ft = CTime(buf.st_mtime);
		CTime ct = CTime::GetCurrentTime();
		CTimeSpan tsp = ct-ft;
		int ss = 60*60*tsp.GetHours()+60*tsp.GetMinutes()+tsp.GetSeconds();
		if(ss>=300)
			::ShellExecute(NULL, _T("open"), "restart_bit.bat", NULL, NULL, SW_SHOW);
	}
	else 
		::ShellExecute(NULL, _T("open"), "restart_bit.bat", NULL, NULL, SW_SHOW);

}

void CTaskmngDlg::HandleTask_Test()
{
	time_t tt = time(NULL);
	char path[MAX_PATH]={0};
	GetCurrentDirectory(MAX_PATH, path);

	long nLastTestTimet_bit = GetPrivateProfileInt("POOL","LASTTEST",0,(CString)path+"\\PoolInfo_Bit.ini");
	long nLastNotAllowTest = GetPrivateProfileInt("POOL","NotAllowTest",0,(CString)path+"\\PoolInfo_Bit.ini");
	if(tt-nLastTestTimet_bit>4*60*60&&tt-nLastNotAllowTest>15*60)
		::ShellExecute(NULL, _T("open"), "100nian_bit_test.exe", NULL, NULL, SW_SHOW);

	/*
	long nLastTestTimet_AG = GetPrivateProfileInt("POOL","LASTTEST",0,(CString)path+"\\PoolInfo.ini");
	if(nTT>=17*60+30&&tt-nLastTestTimet_AG>=7*60*60)
		::ShellExecute(NULL, _T("open"), "100nian_test.exe", NULL, NULL, SW_SHOW);
	*/
}

void CTaskmngDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==100)
		HandleTask_Calc();
	else if(nIDEvent==200)
		HandleTask_Test();
	
	CDialog::OnTimer(nIDEvent);
}

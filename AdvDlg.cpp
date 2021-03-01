// AdvDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "AdvDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvDlg dialog


CAdvDlg::CAdvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdvDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAdvDlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nIndex = 0;
}


void CAdvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvDlg)
	DDX_Control(pDX, IDC_EXPLORER1, m_IE);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAdvDlg, CDialog)
	//{{AFX_MSG_MAP(CAdvDlg)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvDlg message handlers

BOOL CAdvDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon,TRUE);
	SetIcon(m_hIcon,TRUE);

	UINT cx=g_AdvertInfo.AdvButton[m_nIndex].advdlg_cx;
	UINT cy=g_AdvertInfo.AdvButton[m_nIndex].advdlg_cy;
	SetWindowText(g_AdvertInfo.AdvButton[m_nIndex].btTxt);

	CRect ScrRect;
    ::GetWindowRect(::GetDesktopWindow(), &ScrRect);

	SetWindowPos(NULL,(ScrRect.Width()-cx)/2,(ScrRect.Height()-cy)/2,cx,cy,SWP_SHOWWINDOW);
	CRect rc;
	GetClientRect(&rc);
	m_IE.MoveWindow(rc,FALSE);
	m_IE.SetSilent(TRUE);
	m_IE.Navigate(g_AdvertInfo.AdvButton[m_nIndex].url, NULL, NULL, NULL, NULL);
	return TRUE;
}

void CAdvDlg::SetAdvIndex(int nIndex)
{
	m_nIndex=nIndex;
}

BOOL CAdvDlg::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

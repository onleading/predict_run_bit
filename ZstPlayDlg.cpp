// ZstPlayDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ZstPlayDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZstPlayDlg dialog


CZstPlayDlg::CZstPlayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CZstPlayDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CZstPlayDlg)
	m_bPlaying = FALSE;
	//}}AFX_DATA_INIT
	m_nGzPerMin = g_nGzPerMin;
	if(m_nGzPerMin < 1 || m_nGzPerMin > 600) m_nGzPerMin = 120;
	m_bHasTimer = FALSE;
	m_nMinCount = 0;
}


void CZstPlayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZstPlayDlg)
	DDX_Text(pDX, IDC_GZPERMIN, m_nGzPerMin);
	DDV_MinMaxUInt(pDX, m_nGzPerMin, 1, 600);
	DDX_Check(pDX, IDC_PLAYSTOP, m_bPlaying);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CZstPlayDlg, CDialog)
	//{{AFX_MSG_MAP(CZstPlayDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_HEAD, OnHead)
	ON_BN_CLICKED(IDC_FORWARD, OnForward)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_TAIL, OnTail)
	ON_BN_CLICKED(IDC_PLAYSTOP, OnPlaystop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZstPlayDlg message handlers

BOOL CZstPlayDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	CRect rc,rc1;
	theViewManager.pActiveView->GetClientRect(&rc);
	GetClientRect(&rc1);
	SetWindowPos(NULL,rc.right-rc1.Width(),rc.top+rc1.Height(),0,0,SWP_NOSIZE);
	return TRUE;
}

void CZstPlayDlg::OnPlaystop()
{
	UpdateData(TRUE);
	if(m_bPlaying)
	{
		GetDlgItem(IDC_PLAYSTOP)->SetWindowText(_F("ÔÝÍ£"));
		if(m_bHasTimer) KillTimer(0x2234);
		SetTimer(0x2234,100,NULL);
		m_bHasTimer = TRUE;
		m_nMinCount = 0;
		GetDlgItem(IDC_GZPERMIN)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_PLAYSTOP)->SetWindowText(_F("²¥·Å"));
		if(m_bHasTimer)	KillTimer(0x2234);
		GetDlgItem(IDC_GZPERMIN)->EnableWindow(TRUE);
	}
}

void CZstPlayDlg::OnCancel()
{
	UpdateData(TRUE);
	g_nGzPerMin = m_nGzPerMin;
	theViewManager.pActiveView->SendMessage(WM_COMMAND,ID_ZSTPLAY);
	CDialog::OnCancel();
}

void CZstPlayDlg::OnOK() 
{
	theViewManager.pActiveView->SendMessage(WM_COMMAND,ID_ZSTPLAY);
}

void CZstPlayDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 0x2234)
	{
		m_nMinCount++;
		if(m_nMinCount >= 600/m_nGzPerMin)
		{
			theViewManager.pActiveView->SendMessage(UM_CONTINUEPLAY,4);
			m_nMinCount = 0;
		}
		return;
	}
	CDialog::OnTimer(nIDEvent);
}

void CZstPlayDlg::OnHead() 
{
	theViewManager.pActiveView->SendMessage(UM_CONTINUEPLAY,0);	
}

void CZstPlayDlg::OnForward() 
{
	theViewManager.pActiveView->SendMessage(UM_CONTINUEPLAY,1);		
}

void CZstPlayDlg::OnNext() 
{
	theViewManager.pActiveView->SendMessage(UM_CONTINUEPLAY,2);		
}

void CZstPlayDlg::OnTail() 
{
	theViewManager.pActiveView->SendMessage(UM_CONTINUEPLAY,3);		
}

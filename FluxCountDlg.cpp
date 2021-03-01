// FluxCountDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "FluxCountDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char *fluxfile="FluxInfo.dat";
/////////////////////////////////////////////////////////////////////////////
// CFluxCountDlg dialog
CFluxCountDlg::CFluxCountDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFluxCountDlg::IDD, pParent)
{
	m_bInCalcState = FALSE;
	m_bReadyStart = TRUE;
	//{{AFX_DATA_INIT(CFluxCountDlg)
	m_strNetIn = _T("0");
	m_strNetOut = _T("0");
	m_strStartTime = _T("-:-:-");
	m_strEndTime = _T("-:-:-");
	//}}AFX_DATA_INIT
}


void CFluxCountDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFluxCountDlg)
	DDX_Text(pDX, IDC_FLUXIN, m_strNetIn);
	DDX_Text(pDX, IDC_FLUXOUT, m_strNetOut);
	DDX_Text(pDX, IDC_STARTTIME, m_strStartTime);
	DDX_Text(pDX, IDC_ENDTIME, m_strEndTime);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFluxCountDlg, CDialog)
	//{{AFX_MSG_MAP(CFluxCountDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_RESET, OnReset)
	ON_BN_CLICKED(ID_SETSTATE, OnSetstate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CFluxCountDlg message handlers

BOOL CFluxCountDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_StartTime = CTime::GetCurrentTime();
	m_EndTime = CTime::GetCurrentTime();

	OnReset();

	UpdateData(FALSE);	
	return TRUE;
}

void CFluxCountDlg::UpDateInfo()
{	
	if(!m_bReadyStart) 
	{
		m_strStartTime.Format(" %04d-%02d-%02d %02d:%02d:%02d", 
			m_StartTime.GetYear(),m_StartTime.GetMonth(),m_StartTime.GetDay(),
			m_StartTime.GetHour(), m_StartTime.GetMinute(), m_StartTime.GetSecond());
		m_strEndTime.Format(" %04d-%02d-%02d %02d:%02d:%02d", 
			m_EndTime.GetYear(),m_EndTime.GetMonth(),m_EndTime.GetDay(),
			m_EndTime.GetHour(), m_EndTime.GetMinute(), m_EndTime.GetSecond());
	}
	CTimeSpan tspan=m_EndTime-m_StartTime;
	int lSeconds = tspan.GetTotalSeconds();
	m_strNetIn.Format("%9ld字节 (%.1fkbps)", g_nNetInCount,lSeconds>0?(g_nNetInCount*8/lSeconds/1024.0):0.0);
	m_strNetOut.Format("%9ld字节 (%.1fkbps)", g_nNetOutCount,lSeconds>0?(g_nNetOutCount*8/lSeconds/1024.0):0.0);
	UpdateData(FALSE);
}

//////////////////////////////////////////////////////////////////////////
void CFluxCountDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==1&&m_bInCalcState)
	{
		m_EndTime = CTime::GetCurrentTime();
		UpDateInfo();
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CFluxCountDlg::OnReset() 
{
	m_strStartTime = _T("-:-:-");
	m_strEndTime = _T("-:-:-");
	m_StartTime=CTime::GetCurrentTime();
	m_bReadyStart = !m_bInCalcState;
	if(!m_bInCalcState) 
		GetDlgItem(ID_SETSTATE)->SetWindowText(_F("开始"));
	InterlockedExchange(&g_nNetInCount, 0);
	InterlockedExchange(&g_nNetOutCount, 0);
	m_nNetIn = g_nNetInCount;
	m_nNetOut = g_nNetOutCount;
	UpDateInfo();
}

void CFluxCountDlg::OnSetstate() 
{
	if(!m_bInCalcState)
	{
		if(m_bReadyStart) 
		{
			m_StartTime=CTime::GetCurrentTime();
			m_bReadyStart = FALSE;
		}
		InterlockedExchange(&g_nNetInCount, m_nNetIn);
		InterlockedExchange(&g_nNetOutCount, m_nNetOut);
		m_bInCalcState = TRUE;
		SetTimer(1, 100, NULL);
		GetDlgItem(ID_SETSTATE)->SetWindowText(_F("暂停"));
	}
	else
	{
		m_nNetIn = g_nNetInCount;
		m_nNetOut = g_nNetOutCount;
		KillTimer(1);
		m_bInCalcState = FALSE;
		GetDlgItem(ID_SETSTATE)->SetWindowText(_F("继续"));
	}
}

void CFluxCountDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
}

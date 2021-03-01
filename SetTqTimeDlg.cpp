// SetTqTimeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "SetTqTimeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetTqTimeDlg dialog


CSetTqTimeDlg::CSetTqTimeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetTqTimeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetTqTimeDlg)
	m_FromTime = 0;
	m_PartTqType = -1;
	//}}AFX_DATA_INIT
}


void CSetTqTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetTqTimeDlg)
	DDX_DateTimeCtrl(pDX, IDC_FROMDATE, m_FromTime);
	DDX_Radio(pDX, IDC_RADIO_BACKTQ, m_PartTqType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetTqTimeDlg, CDialog)
	//{{AFX_MSG_MAP(CSetTqTimeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetTqTimeDlg message handlers

void CSetTqTimeDlg::OnOK() 
{
	UpdateData(TRUE);
	if(m_FromTime<CTime(1990,1,1,0,0,0) || m_FromTime>CTime::GetCurrentTime())
	{
		TDX_MessageBox(m_hWnd,"时间设置不合理!必要时请确认您本机的时间是否正确!",MB_OK|MB_ICONEXCLAMATION);		
		return;
	}
	g_nPartTq = m_PartTqType;
	g_nTqStart = m_FromTime.GetYear()*10000+m_FromTime.GetMonth()*100+m_FromTime.GetDay();
	CDialog::OnOK();
}

void CSetTqTimeDlg::OnCancel() 
{
	CDialog::OnCancel();
}

BOOL CSetTqTimeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();	
	m_FromTime = CTime(g_nTqStart/10000,g_nTqStart/100%100,g_nTqStart%100,0,0,0);
	m_PartTqType = g_nPartTq;
	CTime tt=CTime::GetCurrentTime()+CTimeSpan(720,0,0,0);
	((CDateTimeCtrl *)(GetDlgItem(IDC_FROMDATE)))->SetRange(&(CTime(1990,1,1,0,0,0)),&tt);
	UpdateData(FALSE);
	return TRUE;
}

// TQCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "TQCfgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTQCfgDlg dialog


CTQCfgDlg::CTQCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTQCfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTQCfgDlg)
	m_bAutoConnectTQ = FALSE;
	m_bTQAsBroker = 0;
	//}}AFX_DATA_INIT
}


void CTQCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTQCfgDlg)
	DDX_Control(pDX, IDC_HINT_STATIC, m_Hint_Static);
	DDX_Check(pDX, IDC_AUTOCONNECTTQ_CHECK, m_bAutoConnectTQ);
	DDX_Radio(pDX, IDC_RADIO1, m_bTQAsBroker);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTQCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CTQCfgDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTQCfgDlg message handlers

BOOL CTQCfgDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Hint_Static.SetTextColor(RGB(255,0,0));
	
	m_bAutoConnectTQ = g_bAutoConnectTICQ;
	m_bTQAsBroker = g_bTQAsBroker;
	
	UpdateData(FALSE);
	return TRUE;
}

void CTQCfgDlg::OnOK() 
{
	UpdateData(TRUE);

	g_bAutoConnectTICQ = m_bAutoConnectTQ;
	g_bTQAsBroker = m_bTQAsBroker;
	WritePrivateProfileString("Other","AutoConnectTICQ",g_bAutoConnectTICQ?"1":"0",g_WSXHStr+g_strYxhjCfg);	
	WritePrivateProfileString("Other","TQAsBroker",g_bTQAsBroker?"1":"0",g_WSXHStr+g_strYxhjCfg);	
	
	CDialog::OnOK();
}

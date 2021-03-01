// AreaCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "AreaCfgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAreaCfgDlg dialog


CAreaCfgDlg::CAreaCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAreaCfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAreaCfgDlg)
	m_nCXMode = 0;
	//}}AFX_DATA_INIT
}


void CAreaCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAreaCfgDlg)
	DDX_Radio(pDX, IDC_RADIO1, m_nCXMode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAreaCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CAreaCfgDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAreaCfgDlg message handlers

BOOL CAreaCfgDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_RADIO1)->ShowWindow(g_OEM.bZszqEmbedWTVersion?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_RADIO2)->ShowWindow(g_OEM.bZszqEmbedWTVersion?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_STATIC2)->ShowWindow(g_OEM.bZszqEmbedWTVersion?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_STATIC3)->ShowWindow(g_OEM.bZszqEmbedWTVersion?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_STATIC4)->ShowWindow(g_OEM.bZszqEmbedWTVersion?SW_SHOW:SW_HIDE);
	
	m_nCXMode=GetPrivateProfileInt("Version","CXMode",0,g_WSXHStr+g_strYxhjCfg);
	
	UpdateData(FALSE);
	return TRUE;
}

void CAreaCfgDlg::OnOK() 
{
	UpdateData(TRUE);
	
	WritePrivateProfileString("Version","CXMode",(m_nCXMode==1)?"1":"0",g_WSXHStr+g_strYxhjCfg);

	CDialog::OnOK();
}

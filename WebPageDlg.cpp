// WebPageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "WebPageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWebPageDlg dialog


CWebPageDlg::CWebPageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWebPageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWebPageDlg)
	m_name = _T("");
	m_url = _T("");
	//}}AFX_DATA_INIT
}


void CWebPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWebPageDlg)
	DDX_Text(pDX, IDC_EDIT1, m_name);
	DDV_MaxChars(pDX, m_name, 8);
	DDX_Text(pDX, IDC_EDIT2, m_url);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWebPageDlg, CDialog)
	//{{AFX_MSG_MAP(CWebPageDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebPageDlg message handlers

BOOL CWebPageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_title);
	UpdateData(FALSE);
	return TRUE;
}

void CWebPageDlg::OnOK() 
{
	UpdateData(TRUE);
	m_name.TrimLeft();
	m_name.TrimRight();
	m_url.TrimLeft();
	m_url.TrimRight();
	if(m_name.GetLength()==0 || m_url.GetLength()==0)
	{
		TDX_MessageBox(NULL,"«ÎÃÓ–¥ÕÍ’˚!",MB_OK|MB_ICONEXCLAMATION);
		UpdateData(FALSE);
		return;
	}
	CDialog::OnOK();
}

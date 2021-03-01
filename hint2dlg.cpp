// hint2dlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "hint2dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHint2Dlg dialog


CHint2Dlg::CHint2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHint2Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHint2Dlg)
	m_bCheck = FALSE;
	//}}AFX_DATA_INIT
	m_strHint = "";
	m_strCheck = "";
}


void CHint2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHint2Dlg)
	DDX_Check(pDX, IDC_HINT_CHECK, m_bCheck);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHint2Dlg, CDialog)
	//{{AFX_MSG_MAP(CHint2Dlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHint2Dlg message handlers

BOOL CHint2Dlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_HINT_STATIC)->SetWindowText(m_strHint);
	GetDlgItem(IDC_HINT_CHECK)->SetWindowText(m_strCheck);
	if(m_strCheck.GetLength()<2)
		GetDlgItem(IDC_HINT_CHECK)->ShowWindow(SW_HIDE);
	return TRUE;
}

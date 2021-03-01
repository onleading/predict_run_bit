// HintDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "HintDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHintDlg dialog


CHintDlg::CHintDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHintDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHintDlg)
	m_bAlwaysHint = TRUE;
	//}}AFX_DATA_INIT
	m_strCheck = "";
	m_strHint = "";
	m_bHasCheck = TRUE;
}


void CHintDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHintDlg)
	DDX_Check(pDX, IDC_ALWAYS_HINT_CHECK, m_bAlwaysHint);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHintDlg, CDialog)
	//{{AFX_MSG_MAP(CHintDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHintDlg message handlers

BOOL CHintDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_HINT_STATIC)->SetWindowText(m_strHint);
	GetDlgItem(IDC_ALWAYS_HINT_CHECK)->SetWindowText(m_strCheck);
	if(!m_bHasCheck)
		GetDlgItem(IDC_ALWAYS_HINT_CHECK)->ShowWindow(SW_HIDE);

	return TRUE;
}

void CHintDlg::OnOK() 
{
	CDialog::OnOK();
}

void CHintDlg::OnCancel() 
{
	CDialog::OnCancel();
}

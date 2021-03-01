// GrigHzParaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "GrigHzParaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGrigHzParaDlg dialog


CGrigHzParaDlg::CGrigHzParaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGrigHzParaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGrigHzParaDlg)
	m_nQzType = 0;
	//}}AFX_DATA_INIT
}


void CGrigHzParaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGrigHzParaDlg)
	DDX_Radio(pDX, IDC_AVER_1, m_nQzType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGrigHzParaDlg, CDialog)
	//{{AFX_MSG_MAP(CGrigHzParaDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGrigHzParaDlg message handlers

void CGrigHzParaDlg::OnOK() 
{
	UpdateData(TRUE);
	g_nHZQZFlag = m_nQzType;
	CDialog::OnOK();
}

BOOL CGrigHzParaDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_nQzType = g_nHZQZFlag;
	UpdateData(FALSE);
	
	return TRUE;
}

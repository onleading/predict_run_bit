// MsgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "MsgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgDlg dialog


CMsgDlg::CMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMsgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMsgDlg)
	//}}AFX_DATA_INIT
	m_strCaption = "ÌבÊ¾";
}


void CMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMsgDlg)
	DDX_Control(pDX, IDC_HINT_STATIC, m_Hint_Static);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMsgDlg, CDialog)
	//{{AFX_MSG_MAP(CMsgDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsgDlg message handlers

BOOL CMsgDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(_F(m_strCaption));
	if(m_nMsgFlag==MSG_RED)
		m_Hint_Static.SetTextColor(RGB(255,0,0));
	else if(m_nMsgFlag==MSG_GREEN)
		m_Hint_Static.SetTextColor(RGB(0,128,0));
	m_Hint_Static.SetText(_F(m_strMsg));

	return TRUE;
}

void CMsgDlg::OnOK() 
{
	
	CDialog::OnOK();
}

void CMsgDlg::OnCancel() 
{
	
	CDialog::OnCancel();
}

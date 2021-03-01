// KCLogoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "KCLogoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKCLogoDlg dialog


CKCLogoDlg::CKCLogoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKCLogoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKCLogoDlg)
	m_Url="";
	//}}AFX_DATA_INIT
}


void CKCLogoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKCLogoDlg)
	DDX_Control(pDX, IDC_EXPLORER1, m_IE);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKCLogoDlg, CDialog)
	//{{AFX_MSG_MAP(CKCLogoDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKCLogoDlg message handlers

BOOL CKCLogoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetTimer(0x100,6000,NULL);

	try
	{
		m_IE.SetSilent(TRUE);
		TransferToRealURL(m_Url,NULL);
		m_IE.Navigate(m_Url,NULL,NULL,NULL,NULL);
	}
	catch(...)
	{
	}

	return TRUE;
}

void CKCLogoDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==0x100)	
	{
		PostMessage(WM_COMMAND,IDCANCEL);
		return;
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CKCLogoDlg::OnOK() 
{
}

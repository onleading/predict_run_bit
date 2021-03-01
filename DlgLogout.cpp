#include "stdafx.h"
#include "tdxw.h"
#include "DlgLogout.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 
CDlgLogout::CDlgLogout(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLogout::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLogout)
	//}}AFX_DATA_INIT
	m_nExitCode = 0;
} 

void CDlgLogout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLogout)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgLogout, CDialog)
	//{{AFX_MSG_MAP(CDlgLogout)
	ON_BN_CLICKED(IDC_BTNEXIT, OnBtnexit)
	ON_BN_CLICKED(IDC_BTNRELOGIN, OnBtnrelogin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDlgLogout::OnBtnexit()  
{
	m_nExitCode = 1;
	CDialog::OnOK();
}

void CDlgLogout::OnBtnrelogin() 
{
	m_nExitCode = 2;
	CDialog::OnOK();
}

BOOL CDlgLogout::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;
}

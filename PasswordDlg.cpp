// PasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "PasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog


CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordDlg)
	m_strName = _T("");
	m_strPassword = _T("");
	//}}AFX_DATA_INIT
	m_bFomulaCompareBin = FALSE;

	m_strInputPassword = "";
	memset(m_lpszPassword,0,PASSWORD_LEN*sizeof(char));
}


void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDlg)
	DDX_Control(pDX, IDC_PASSWORD_EDIT, m_Edit);
	DDX_Text(pDX, IDC_NAME_STATIC, m_strName);
	DDX_Text(pDX, IDC_PASSWORD_EDIT, m_strPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg message handlers

void	DecryptPassword(BYTE *pPassword)
{
	CBlowfish dbf;
	dbf.Decrypt(pPassword,((PASSWORD_LEN-1-1)/8)*8);
}

void CPasswordDlg::OnOK() 
{
	UpdateData();
	if(m_bFomulaCompareBin)
	{
		char lpszPassword[PASSWORD_LEN];
		memcpy(lpszPassword,m_lpszPassword,PASSWORD_LEN*sizeof(char));
		DecryptPassword((BYTE*)lpszPassword);
		lpszPassword[PASSWORD_LEN-1]=0;
		if (m_strPassword != lpszPassword)
		{
			m_Edit.SetFocus();
			m_Edit.SetSel(0, -1);
		}
		else
			CDialog::OnOK();		
	}
	else
	{
		if (m_strPassword != m_strInputPassword)
		{
			m_Edit.SetFocus();
			m_Edit.SetSel(0, -1);
		}
		else
			CDialog::OnOK();
	}
}

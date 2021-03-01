// u_selreg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "u_selreg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CU_SelReg dialog


CU_SelReg::CU_SelReg(CWnd* pParent /*=NULL*/)
	: CDialog(CU_SelReg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CU_SelReg)
	m_strPassword = _T("");
	m_strUserName = _T("");
	m_nRegisterType = 1;
	m_strPassword2 = _T("");
	//}}AFX_DATA_INIT
}


void CU_SelReg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CU_SelReg)
	DDX_Control(pDX, IDC_HINT_STATIC, m_Hint_Status);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, 19);
	DDX_Text(pDX, IDC_USERNAME, m_strUserName);
	DDV_MaxChars(pDX, m_strUserName, 18);
	DDX_Radio(pDX, IDC_RADIO1, m_nRegisterType);
	DDX_Text(pDX, IDC_PASSWORD2, m_strPassword2);
	DDV_MaxChars(pDX, m_strPassword2, 19);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CU_SelReg, CDialog)
	//{{AFX_MSG_MAP(CU_SelReg)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CU_SelReg message handlers

BOOL CU_SelReg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Hint_Status.SetTextColor(RGB(200,0,0));

	GetDlgItem(IDC_RADIO1)->ShowWindow(g_LoginInfo.bHasAutoRelation?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_RADIO3)->ShowWindow(g_LoginInfo.bHasWebRelation?SW_SHOW:SW_HIDE);

	EnableOther();
	return TRUE;
}

void CU_SelReg::EnableOther()
{
	GetDlgItem(IDC_USERNAME)->EnableWindow(m_nRegisterType!=0);
	GetDlgItem(IDC_PASSWORD)->EnableWindow(m_nRegisterType!=0);
}

void CU_SelReg::OnOK() 
{
	UpdateData(TRUE);
	if(m_nRegisterType!=0)
	{
		TrimString(m_strPassword);
		TrimString(m_strPassword2);
		TrimString(m_strUserName);
		if(m_strPassword.IsEmpty())
		{
			TDX_MessageBox(m_hWnd,"请输入密码!",MB_OK|MB_ICONINFORMATION);
			GetDlgItem(IDC_PASSWORD)->SetFocus();
			return;
		}
		if(strcmp(m_strPassword,m_strPassword2))
		{
			TDX_MessageBox(m_hWnd,"两次密码输入不一致!",MB_OK|MB_ICONINFORMATION);
			GetDlgItem(IDC_PASSWORD)->SetFocus();
			return;
		}
		if(m_strUserName.IsEmpty())
		{
			TDX_MessageBox(m_hWnd,"请输入用户名!",MB_OK|MB_ICONINFORMATION);
			GetDlgItem(IDC_USERNAME)->SetFocus();
			return;
		}	
	}
	CDialog::OnOK();
}

void CU_SelReg::OnRadio1() 
{
	UpdateData(TRUE);
	EnableOther();	
}

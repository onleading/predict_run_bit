// u_changepwd.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "u_changepwd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CU_ChangePwd dialog


CU_ChangePwd::CU_ChangePwd(CWnd* pParent /*=NULL*/)
	: CDialog(CU_ChangePwd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CU_ChangePwd)
	m_strPassword = _T("");
	m_strPassword2 = _T("");
	m_strOldPassword = _T("");
	//}}AFX_DATA_INIT
	m_bDelMySelf = FALSE;
}


void CU_ChangePwd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CU_ChangePwd)
	DDX_Control(pDX, IDC_HINT_STATIC, m_Hint_Static);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, 19);
	DDX_Text(pDX, IDC_PASSWORD2, m_strPassword2);
	DDV_MaxChars(pDX, m_strPassword2, 19);
	DDX_Text(pDX, IDC_OLDPASSWORD, m_strOldPassword);
	DDV_MaxChars(pDX, m_strOldPassword, 19);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CU_ChangePwd, CDialog)
	//{{AFX_MSG_MAP(CU_ChangePwd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CU_ChangePwd message handlers

BOOL CU_ChangePwd::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Hint_Static.SetTextColor(RGB(200,0,0));
	if(m_bDelMySelf)
	{
		m_Hint_Static.SetText(_F("将从后端系统中删除当前用户,请输入密码."));
		GetDlgItem(IDC_STATIC1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PASSWORD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PASSWORD2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDOK)->SetWindowText(_F("删除用户"));
		SetWindowText(_F("删除用户"));
	}
	return TRUE;
}

void CU_ChangePwd::OnOK() 
{
	if(strcmp(g_strLoginName,"GUEST")==0)
	{
		TDX_MessageBox(m_hWnd,"GUEST用户不支持此功能!",MB_OK|MB_ICONINFORMATION);
		CDialog::OnCancel();
		return;
	}
	if(m_bDelMySelf)
	{
		CString tmpStr;
		tmpStr.Format("你确定删除当前用户\'%s\'吗？",g_strLoginName);
		if(TDX_MessageBox(m_hWnd,tmpStr,MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL)
			return;
	}
	UpdateData(TRUE);
	TrimString(m_strPassword);
	TrimString(m_strPassword2);
	TrimString(m_strOldPassword);
	if(!m_bDelMySelf && m_strPassword.IsEmpty())
	{
		TDX_MessageBox(m_hWnd,"请输入密码!",MB_OK|MB_ICONINFORMATION);
		GetDlgItem(IDC_PASSWORD)->SetFocus();
		return;
	}
	if(!m_bDelMySelf && strcmp(m_strPassword,m_strPassword2))
	{
		TDX_MessageBox(m_hWnd,"两次密码输入不一致!",MB_OK|MB_ICONINFORMATION);
		GetDlgItem(IDC_PASSWORD)->SetFocus();
		return;
	}
	CDialog::OnOK();
}

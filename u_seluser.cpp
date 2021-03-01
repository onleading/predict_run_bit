// u_seluser.cpp : implementation file
//

#include "stdafx.h"
#include "TdxW.h"
#include "u_seluser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CU_SelUser dialog


CU_SelUser::CU_SelUser(CWnd* pParent /*=NULL*/)
	: CDialog(CU_SelUser::IDD, pParent)
{
	//{{AFX_DATA_INIT(CU_SelUser)
	//}}AFX_DATA_INIT
	m_nUserNum = 0;
	m_strSelUser = "";
}


void CU_SelUser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CU_SelUser)
	DDX_Control(pDX, IDC_USER_LIST, m_User_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CU_SelUser, CDialog)
	//{{AFX_MSG_MAP(CU_SelUser)
	ON_LBN_DBLCLK(IDC_USER_LIST, OnDblclkUserList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CU_SelUser message handlers

BOOL CU_SelUser::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_User_List.SetItemHeight(0,16);
	m_User_List.SetItemHeight(-1,16);
	
	for(int i=0;i < m_nUserNum;i++)
		m_User_List.AddString(m_User[i]);
	m_User_List.SetCurSel(0);
	return TRUE;
}

void CU_SelUser::OnOK() 
{
	int sel=m_User_List.GetCurSel();
	if(sel < 0) return;
	
	m_strSelUser=m_User[sel];
	CDialog::OnOK();
}

void CU_SelUser::OnDblclkUserList() 
{
	OnOK();
}

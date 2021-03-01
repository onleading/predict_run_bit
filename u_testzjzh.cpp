// u_testzjzh.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "u_testzjzh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

long WTTypeID[] = 
{
	0,
	1,
	2,
	3,
	8//以资金帐号为主
};

char *WTTypeStr[] = 
{
	"深圳Ａ股",
	"上海Ａ股",
	"深圳Ｂ股",
	"上海Ｂ股",
	"资金帐号"
};

int	 g_nWTTypeNum = chDIMOF(WTTypeID);
CU_TestZjzh::CU_TestZjzh(CWnd* pParent /*=NULL*/)
	: CDialog(CU_TestZjzh::IDD, pParent)
{
	//{{AFX_DATA_INIT(CU_TestZjzh)
	m_strPassWord = _T("");
	m_strZjzh = _T("");
	m_strYYBInfo=_T("");
	//}}AFX_DATA_INIT
	m_WTTypeID = 8;
	m_bJyLogin = FALSE;
}


CU_TestZjzh::~CU_TestZjzh()
{
}

void CU_TestZjzh::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CU_TestZjzh)
	DDX_Control(pDX, IDC_WTTYPE_COMBO, m_WTType_Combo);
	DDX_Control(pDX, IDC_HINT_STATIC, m_Hint_Status);
	DDX_Control(pDX, IDC_YYBINFO_COMBO, m_YYBInfo_Combo);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassWord);
	DDV_MaxChars(pDX, m_strPassWord, 18);
	DDX_Text(pDX, IDC_ZJZH, m_strZjzh);
	DDV_MaxChars(pDX, m_strZjzh, 18);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CU_TestZjzh, CDialog)
	//{{AFX_MSG_MAP(CU_TestZjzh)
	ON_CBN_SELCHANGE(IDC_WTTYPE_COMBO, OnSelchangeWttypeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CU_TestZjzh message handlers

BOOL CU_TestZjzh::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Hint_Status.SetTextColor(RGB(200,0,0));
	if(m_bJyLogin)
		m_Hint_Status.SetWindowText(_F("请输入交易帐号信息:"));
	GetDlgItem(IDC_ZJZH)->SetFocus();
	m_YYBInfo_Combo.SetItemHeight(0,18);
	m_YYBInfo_Combo.SetItemHeight(-1,18);
	m_WTType_Combo.SetItemHeight(0,18);
	m_WTType_Combo.SetItemHeight(-1,18);

	for(int i=0;i < l_YYBNum;i++)
		m_YYBInfo_Combo.AddString(_F(l_pYYBInfo[i].YYBInfo_Str));
	if(l_PrimaryYYB >= 0 && l_PrimaryYYB < l_YYBNum)
		m_YYBInfo_Combo.SetCurSel(l_PrimaryYYB);
	for(i=0;i < g_nWTTypeNum;i++)
		m_WTType_Combo.AddString(_F(WTTypeStr[i]));
	if(!m_bJyLogin && !g_LoginInfo.bMiscJyStyle) //如果不支持多种校验方式
	{
		m_WTType_Combo.SetCurSel(4);
		m_WTType_Combo.EnableWindow(FALSE);
	}
	else
		m_WTType_Combo.SetCurSel(0);
	OnSelchangeWttypeCombo();

	UpdateData(FALSE);
	return FALSE;
}

void CU_TestZjzh::OnOK() 
{
	UpdateData(TRUE);
	int nYYBSel = m_YYBInfo_Combo.GetCurSel();
	if(nYYBSel < 0) return;
	//将这次所选的营业部存盘
	CString	Ini = HomePath+"userhost.cfg";
	WritePrivateProfileString("YYBInfo","PrimaryYYB",IntToStr(nYYBSel),Ini);	
	l_PrimaryYYB = nYYBSel;
	//
	m_strYYBInfo = l_pYYBInfo[nYYBSel].YYBInfo_ID;
	TrimString(m_strPassWord);
	TrimString(m_strZjzh);
	
	if(m_strZjzh.IsEmpty())
	{
		TDX_MessageBox(m_hWnd,"请输入资金帐号!",MB_OK|MB_ICONINFORMATION);
		GetDlgItem(IDC_ZJZH)->SetFocus();
		return;
	}
	if(m_strPassWord.IsEmpty())
	{
		TDX_MessageBox(m_hWnd,"请输入交易密码!",MB_OK|MB_ICONINFORMATION);
		GetDlgItem(IDC_PASSWORD)->SetFocus();
		return;
	}
	int nWTTypeSel=m_WTType_Combo.GetCurSel();
	if(nWTTypeSel < 0) return;
	m_WTTypeID = WTTypeID[nWTTypeSel];
	
	CDialog::OnOK();
}

void CU_TestZjzh::OnSelchangeWttypeCombo() 
{
	int nSel=m_WTType_Combo.GetCurSel();
	switch(nSel)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		GetDlgItem(IDC_ZH_STATIC)->SetWindowText(_F(WTTypeStr[nSel]+(CString)"股东代码:"));break;
	case 4:
		GetDlgItem(IDC_ZH_STATIC)->SetWindowText(_F("资金帐号:"));break;
	case 5:
		GetDlgItem(IDC_ZH_STATIC)->SetWindowText(_F("客户号:"));break;
	case 6:
	case 7:
	case 8:
	case 9:
		GetDlgItem(IDC_ZH_STATIC)->SetWindowText(_F(WTTypeStr[nSel]+(CString)"帐号:"));break;
	}
}

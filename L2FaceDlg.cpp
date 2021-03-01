// L2FaceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "L2FaceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CL2FaceDlg dialog

#define L2FACE_BKCOLOR	RGB(246, 251, 254)

CL2FaceDlg::CL2FaceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CL2FaceDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CL2FaceDlg)
	m_L2Password = _T("");
	m_L2Username = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_brush.CreateSolidBrush(L2FACE_BKCOLOR);
}

CL2FaceDlg::~CL2FaceDlg()
{
	m_brush.DeleteObject();
}

void CL2FaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CL2FaceDlg)
	DDX_Control(pDX, IDC_USERINFO_STATIC, m_UserInfoLink);
	DDX_Control(pDX, IDC_BUY_STATIC, m_BuyLink);
	DDX_Control(pDX, IDC_L2TYPE_COMBO, m_L2Type_Combo);
	DDX_Text(pDX, IDC_L2PASSWORD, m_L2Password);
	DDV_MaxChars(pDX, m_L2Password, 19);
	DDX_Text(pDX, IDC_L2USERNAME, m_L2Username);
	DDV_MaxChars(pDX, m_L2Username, 19);
	DDX_Control(pDX, IDC_HINT_STATIC, m_Hint_Static);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CL2FaceDlg, CDialog)
	//{{AFX_MSG_MAP(CL2FaceDlg)
	ON_MESSAGE(UM_REGLINK_MESSAGE,OnRegLinkMessage)
	ON_MESSAGE(UM_REGLINK_MESSAGE2,OnRegLink2Message)
	ON_CBN_SELCHANGE(IDC_L2TYPE_COMBO, OnSelchangeL2typeCombo)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CL2FaceDlg message handlers

void GetL2Cfg(CString & L2Username,CString & L2Password,int & nType)
{
	char tmpStr[255];
	GetPrivateProfileString("L2","L2UserName","",tmpStr,20,g_WSXHStr+g_strUserCfg);
	L2Username=tmpStr;
	long nValidDate=0;
	short nValidValue=1;
	GetPasswordFromRegister(HQ_L2_VALUE_STRING,L2Username,L2Password,nValidDate,nValidValue);
	nType=GetPrivateProfileInt("L2","L2Type",0,g_WSXHStr+g_strUserCfg);
}

void GetL2Info(CString & L2Username,CString & L2Password,int & nType)
{
	if(g_GC.bZHFromExtern)
	{
		nType=1;
		L2Username=g_GC.Extern_LoginName;
		L2Password=g_GC.Extern_Password;
		return;
	}
	GetL2Cfg(L2Username,L2Password,nType);
	if(g_GC.bMustTdxLevel2)
	{
		nType=1;
		return;
	}
#ifdef OEM_NEWJY
	if(nType==0)
	{
		DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
		if((dwStatus&JY_LOGINED) && TC_GetL2Info_)
		{
			char tmpUserName[255]={0},tmpPassword[255]={0};
			TC_GetL2Info_(tmpUserName,tmpPassword);
			if(strlen(tmpUserName)>0)
			{
				nType=1;
				L2Username=tmpUserName;
				L2Password=tmpPassword;
			}
		}
	}
#endif
}

BOOL CL2FaceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon,FALSE);
	SetIcon(m_hIcon,TRUE);

	m_Hint_Static.SetTextColor(RGB(255,0,0));
	m_Hint_Static.SetBkColor(L2FACE_BKCOLOR);

	SetWindowText(_F(g_strPreName+" 高级行情设置"));
	
	m_L2Type_Combo.SetItemHeight(0,18);
	m_L2Type_Combo.SetItemHeight(-1,18);
	m_L2Type_Combo.AddString(_F("根据资金账号自动登录"));
	m_L2Type_Combo.AddString(_F("输入行情帐号"));
	m_L2Type_Combo.AddString(_F("使用传统行情数据"));
	m_L2Type_Combo.SetItemHeight(0,18);
	m_L2Type_Combo.SetItemHeight(-1,18);

	int nType=0;
	GetL2Cfg(m_L2Username,m_L2Password,nType);
	if(nType<0 || nType>2)
		nType=0;
	if(g_GC.bMustTdxLevel2)
		nType=1;
	
	m_L2Type_Combo.SetCurSel(nType);
	GetDlgItem(IDC_L2PASSWORD)->ShowWindow(nType==1?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_L2USERNAME)->ShowWindow(nType==1?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_STATIC1)->ShowWindow(nType==1?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_STATIC2)->ShowWindow(nType==1?SW_SHOW:SW_HIDE);
	
	m_BuyLink.SetMessageMode(m_hWnd,UM_REGLINK_MESSAGE);
	m_BuyLink.SetUnderline(FALSE);
	m_BuyLink.SetLinkCursor(AfxGetApp()->LoadCursor(IDC_MOVE2));
	m_BuyLink.SetColours(RGB(255,0,0),RGB(128,128,64));
	m_UserInfoLink.SetMessageMode(m_hWnd,UM_REGLINK_MESSAGE2);
	m_UserInfoLink.SetUnderline(FALSE);
	m_UserInfoLink.SetLinkCursor(AfxGetApp()->LoadCursor(IDC_MOVE2));
	m_UserInfoLink.SetColours(RGB(255,0,0),RGB(128,128,64));

	if(g_OEM.bNoAdvert)
	{
		m_BuyLink.ShowWindow(SW_HIDE);
		m_UserInfoLink.ShowWindow(SW_HIDE);
	}

	UpdateData(FALSE);

	if(nType==1)
	{
		if(m_L2Username.GetLength()==0)
			GetDlgItem(IDC_L2USERNAME)->SetFocus();
		else if(m_L2Password.GetLength()==0)
			GetDlgItem(IDC_L2PASSWORD)->SetFocus();
		else 
			GetDlgItem(IDOK)->SetFocus();
		return FALSE;
	}
	return TRUE;
}

void CL2FaceDlg::OnOK() 
{
	UpdateData(TRUE);
	TrimString(m_L2Username);
	TrimString(m_L2Password);
	int nType=m_L2Type_Combo.GetCurSel();
	if(nType==1)
	{
		if(m_L2Username.GetLength()==0)
		{
			TDX_MessageBox(m_hWnd,"请输入用户名",MB_OK|MB_ICONEXCLAMATION);
			GetDlgItem(IDC_L2USERNAME)->SetFocus();
			return;
		}
		if(m_L2Password.GetLength()==0)
		{
			TDX_MessageBox(m_hWnd,"请输入密码",MB_OK|MB_ICONEXCLAMATION);
			GetDlgItem(IDC_L2PASSWORD)->SetFocus();
			return;
		}
	}
	WritePrivateProfileString("L2","L2Type",IntToStr(nType),g_WSXHStr+g_strUserCfg);
	WritePrivateProfileString("L2","L2UserName",m_L2Username,g_WSXHStr+g_strUserCfg);
	WritePasswordToRegister(HQ_L2_VALUE_STRING,(char*)(LPCSTR)m_L2Username,(char*)(LPCSTR)m_L2Password,g_nLastValidDate,g_nLastValidValue);

	CDialog::OnOK();
}

void CL2FaceDlg::OnRegLinkMessage(WPARAM wParam,LPARAM lParam)
{
	ShellExecute(NULL, _T("open"), g_LoginInfo.Web_RegisterURL, NULL, NULL, SW_SHOW);	
}

void CL2FaceDlg::OnRegLink2Message(WPARAM wParam,LPARAM lParam)
{
	ShellExecute(NULL, _T("open"), g_LoginInfo.Web_UserInfoURL, NULL, NULL, SW_SHOW);	
}

void CL2FaceDlg::OnSelchangeL2typeCombo() 
{
	int nType=m_L2Type_Combo.GetCurSel();
	GetDlgItem(IDC_L2PASSWORD)->ShowWindow(nType==1?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_L2USERNAME)->ShowWindow(nType==1?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_STATIC1)->ShowWindow(nType==1?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_STATIC2)->ShowWindow(nType==1?SW_SHOW:SW_HIDE);
}

HBRUSH CL2FaceDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if(nCtlColor==CTLCOLOR_STATIC)
	{
		UINT nCtrlID=pWnd->GetDlgCtrlID();
		if(nCtrlID==IDC_BUY_STATIC || nCtrlID==IDC_USERINFO_STATIC || nCtrlID==IDC_HINT_STATIC)
			return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}

	return (HBRUSH) m_brush.GetSafeHandle();
}

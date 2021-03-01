// other3page.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "other3page.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COther3Page property page

IMPLEMENT_DYNCREATE(COther3Page, CPropertyPage)

COther3Page::COther3Page() : CPropertyPage(COther3Page::IDD)
{
	//{{AFX_DATA_INIT(COther3Page)
	m_hideinfo = FALSE;
	m_nMenuMode = g_nMenuMode;
	m_bExtendXPStyle = g_bExtendXPStyle;
	m_bClassXP = g_bClassXP;
	m_bUseAutoConnect = g_bUseAutoConnect;
	m_bActTogether = FALSE;
	m_bFxtShowInfoDlg = FALSE;
	m_bFxtShowTab = FALSE;
	m_my_rate = g_my_rate;
	m_gy_rate = g_gy_rate;
	m_bBossKey = g_bBossKey;
	m_mintotray = g_bMin3Tray;
	m_nPerScale = g_GH.nPerScale;
	m_nPerSect = g_GH.nPerSect;
	//}}AFX_DATA_INIT
	m_hideinfo = g_hideinfo;
	m_bActTogether = g_bActTogether;
	m_bFxtShowInfoDlg = g_bFxtShowInfoDlg;
	m_bFxtShowTab =	g_bFxtShowTab;
	m_bZBudFlag = g_bZBudFlag;
	m_bTranparentTab = g_bTranparentTab;
	m_bAddBlockPrompt = g_GH.bAddBlockPrompt;
} 

COther3Page::~COther3Page()
{
}

void COther3Page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COther3Page)
	DDX_Check(pDX, IDC_ZBUDFLAG, m_bZBudFlag);
	DDX_Check(pDX, IDC_AddBlockPrompt, m_bAddBlockPrompt);
	DDX_Control(pDX, IDC_HOTKEY1, m_hotkey);
	DDX_Check(pDX, IDC_HIDEINFO_CHECK, m_hideinfo);
	DDX_Radio(pDX, IDC_MENUMODE1, m_nMenuMode);
	DDX_Check(pDX, IDC_EXTENDXPSTYLE_CHECK, m_bExtendXPStyle);
	DDX_Check(pDX, IDC_CLASSXP_CHECK, m_bClassXP);	
	DDX_Check(pDX, IDC_USEAUTOCONNECT_CHECK, m_bUseAutoConnect);
	DDX_Check(pDX, IDC_ACT_TOGETHER, m_bActTogether);
	DDX_Check(pDX, IDC_FXTSHOWINFODLG_CHECK, m_bFxtShowInfoDlg);
	DDX_Check(pDX, IDC_FXTSHOWTAB_CHECK, m_bFxtShowTab);
	DDX_Check(pDX, IDC_TRANSPARENTTAB_CHECK, m_bTranparentTab);
	DDX_Text(pDX, IDC_MY_RATE, m_my_rate);
	DDX_Text(pDX, IDC_GY_RATE, m_gy_rate);
	DDX_Check(pDX, IDC_BOSSKEY_CHECK, m_bBossKey);
	DDX_Check(pDX, IDC_MINTOTRAY_CHECK, m_mintotray);
	DDX_Text(pDX, IDC_EDIT_DB, m_nPerScale);
	DDX_Text(pDX, IDC_EDIT_DF, m_nPerSect);
	DDX_Control(pDX, IDC_DEFPERIOD_COMBO, m_DefPeriod_Combo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COther3Page, CPropertyPage)
	//{{AFX_MSG_MAP(COther3Page)
	ON_BN_CLICKED(IDC_MENUMODE1, OnMenumode1)
	ON_BN_CLICKED(IDC_MENUMODE2, OnMenumode1)
	ON_BN_CLICKED(IDC_BOSSKEY_CHECK, OnBosskeyCheck)
	ON_BN_CLICKED(IDC_PERAXIS_CHECK, OnPeraxisCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COther3Page message handlers

BOOL COther3Page::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	if(!g_bEnableClassXP)
		GetDlgItem(IDC_CLASSXP_CHECK)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_HOTKEY1)->EnableWindow(m_bBossKey);
	GetDlgItem(IDC_HIDEINFO_CHECK)->EnableWindow(m_bBossKey);
	
	GetDlgItem(IDC_EXTENDXPSTYLE_CHECK)->EnableWindow(m_nMenuMode==1);
	m_hotkey.SetHotKey(g_virtkey,g_comkey);

	//添加周期选择串
	m_DefPeriod_Combo.SetItemHeight(0,16);
	m_DefPeriod_Combo.SetItemHeight(-1,16);
	int i=0;
	while(g_CalcPeriodStr[i] != NULL)
	{
		m_DefPeriod_Combo.AddString(_F(g_CalcPeriodStr[i]));
		i++;
	}
	int nCalcPeriod = TdxPeriodToCalcPeriod(g_GH.nDefPeriod);
	m_DefPeriod_Combo.SetCurSel(nCalcPeriod);

	return TRUE;
}

void COther3Page::OnOK() 
{
	UpdateData(TRUE);

	int nCalcPeriod = m_DefPeriod_Combo.GetCurSel();
	if(nCalcPeriod>=0)
		g_GH.nDefPeriod = CalcPeriodToTdxPeriod(nCalcPeriod);

	m_hotkey.GetHotKey(g_virtkey,g_comkey);
	g_bBossKey = m_bBossKey;

	g_GH.nPerScale = m_nPerScale;
	g_GH.nPerScale = max(1,g_GH.nPerScale);
	g_GH.nPerScale = min(10000,g_GH.nPerScale);
	g_GH.nPerSect = m_nPerSect;
	g_GH.nPerSect = max(1,g_GH.nPerSect);
	g_GH.nPerSect = min(100,g_GH.nPerSect);

	g_hideinfo=m_hideinfo;
	g_bActTogether = m_bActTogether;
	g_bFxtShowInfoDlg = m_bFxtShowInfoDlg;
	g_bFxtShowTab =	m_bFxtShowTab;
	g_bZBudFlag = m_bZBudFlag;
		
	g_bTranparentTab = m_bTranparentTab;
	g_nMenuMode = m_nMenuMode;
	g_bExtendXPStyle = m_bExtendXPStyle;
	g_bClassXP = m_bClassXP;
	g_bMin3Tray = m_mintotray;

	g_bUseAutoConnect = m_bUseAutoConnect;
	g_GH.bAddBlockPrompt = m_bAddBlockPrompt;

	g_my_rate = m_my_rate;
	g_gy_rate = m_gy_rate;
	CString	Ini = g_WSXHStr+g_strUserCfg;
//	WritePrivateProfileString("RATE","DOLLAR_RATE",FloatToStr(g_my_rate),Ini);
//	WritePrivateProfileString("RATE","HONKONG_RATE",FloatToStr(g_gy_rate),Ini);

	//设置菜单显示风格
//	CNewMenu::SetMenuDrawMode((g_nMenuMode==0)?CNewMenu::STYLE_ORIGINAL_NOBORDER:
//	(g_bExtendXPStyle?CNewMenu::STYLE_XP_NOBORDER:CNewMenu::STYLE_XP));

	CPropertyPage::OnOK();
}

BOOL COther3Page::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{  
		if(GetFocus()==GetDlgItem(IDC_HOTKEY1)\
			&& pMsg->wParam!=VK_TAB && pMsg->wParam!=VK_SPACE && pMsg->wParam!=VK_BACK && pMsg->wParam!=VK_ESCAPE )
		{
			if(!(GetKeyState(VK_MENU)<0 || GetKeyState(VK_CONTROL)<0 || GetKeyState(VK_SHIFT)<0))
			{
				if( pMsg->wParam!=VK_APPS && pMsg->wParam!=VK_LWIN && pMsg->wParam!=VK_RWIN)
				{
					if(pMsg->wParam==VK_RETURN || (pMsg->wParam>=VK_F1 && pMsg->wParam<=VK_F24))
						m_hotkey.SetHotKey(pMsg->wParam,0);
					else
						m_hotkey.SetHotKey(pMsg->wParam,6);
				}
				return TRUE;
			}
		}
	}
	return CPropertyPage::PreTranslateMessage(pMsg);
}

void COther3Page::OnMenumode1() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_EXTENDXPSTYLE_CHECK)->EnableWindow(m_nMenuMode==1);
}

void COther3Page::OnBosskeyCheck() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_HOTKEY1)->EnableWindow(m_bBossKey);
	GetDlgItem(IDC_HIDEINFO_CHECK)->EnableWindow(m_bBossKey);
}

void COther3Page::OnPeraxisCheck() 
{
	UpdateData(TRUE);
}

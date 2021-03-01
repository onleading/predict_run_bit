// ConnectCfgDlg.cpp : implementation file

#include "stdafx.h"
#include "Tdxw.h"
#include "ConnectCfgDlg.h"
#include "EditServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectCfgDlg dialog


CConnectCfgDlg::CConnectCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectCfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConnectCfgDlg)
	m_bProxy = FALSE;
	m_nProxyPort = 0;
	m_strProxyPass = _T("");
	m_strProxyUser = _T("");
	m_nSockType = -1;
	m_bAutoConnectInfo = g_bAutoConnectInfo;
	m_bNLB = g_bNLB;
	m_bAutoLogo = g_bAutoLogo;
	m_bTdxProxy = FALSE;
	m_bAutoDS = g_bAutoDS;
	//}}AFX_DATA_INIT
	m_bHasWTSet=FALSE;
	m_bFirstLogoStatus=FALSE;
}


void CConnectCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectCfgDlg)
	DDX_Control(pDX, IDC_SERVER_COMBO, m_ServerCombo);
	DDX_Control(pDX, IDC_INFOSERVER_COMBO, m_InfoServerCombo);
	DDX_Control(pDX, IDC_WTSERVER_COMBO,m_WTServerCombo);
	DDX_Control(pDX, IDC_DSSERVER_COMBO, m_DSServerCombo);
	DDX_Control(pDX, IDC_L2SERVER_COMBO,m_L2ServerCombo);

	DDX_Control(pDX, IDC_TQCFG, m_btnTQCfg);
	DDX_Control(pDX, IDC_AREACFG, m_btnAreaCfg);
	DDX_Control(pDX, IDC_PROXYIP, m_ctrlProxyIP);
	DDX_Check(pDX, IDC_PROXYCHECK, m_bProxy);
	DDX_Text(pDX, IDC_PROXYPORT, m_nProxyPort);
	DDV_MinMaxUInt(pDX, m_nProxyPort, 0, 65535);
	DDX_Text(pDX, IDC_PROXYPASS, m_strProxyPass);
	DDV_MaxChars(pDX, m_strProxyPass, 99);
	DDX_Text(pDX, IDC_PROXYUSER, m_strProxyUser);
	DDV_MaxChars(pDX, m_strProxyUser, 99);
	DDX_Radio(pDX, IDC_SOCK4_RADIO, m_nSockType);
	DDX_Check(pDX, IDC_AUTOCONNECTIN_CHECK, m_bAutoConnectInfo);
	DDX_Check(pDX, IDC_NLB_CHECK, m_bNLB);
	DDX_Check(pDX, IDC_AUTOLOGO_CHECK, m_bAutoLogo);
	DDX_Check(pDX, IDC_TDXPROXY, m_bTdxProxy);
	DDX_Check(pDX, IDC_USEDS_CHECK, m_bAutoDS);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConnectCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CConnectCfgDlg)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_MODIFY_BUTTON, OnModifyButton)
	ON_BN_CLICKED(IDC_PROXYCHECK, OnProxycheck)
	ON_BN_CLICKED(IDC_INFOADD_BUTTON, OnInfoaddButton)
	ON_BN_CLICKED(IDC_INFOMODIFY_BUTTON, OnInfomodifyButton)
	ON_BN_CLICKED(IDC_SOCK4_RADIO, OnClickProxyType)
	ON_BN_CLICKED(IDC_WTADD_BUTTON,OnWTAddButton)
	ON_BN_CLICKED(IDC_WTMODIFY_BUTTON,OnWTModifyButton)
	ON_BN_CLICKED(IDC_TDXPROXY, OnTdxproxy)
	ON_BN_CLICKED(IDC_TESTPROXY, OnTestproxy)
	ON_BN_CLICKED(IDC_SELECTPROXY, OnSelectproxy)
	ON_BN_CLICKED(IDC_DEL_BUTTON3, OnWTDelButton)
	ON_BN_CLICKED(IDC_DEL_BUTTON1, OnHQDelButton)
	ON_BN_CLICKED(IDC_DEL_BUTTON2, OnInfoDelButton)
	ON_BN_CLICKED(IDC_TQCFG, OnTqcfg)
	ON_BN_CLICKED(IDC_AREACFG, OnAreacfg)
	ON_BN_CLICKED(IDC_DSADD_BUTTON, OnDsaddButton)
	ON_BN_CLICKED(IDC_DSMODIFY_BUTTON, OnDsmodifyButton)
	ON_BN_CLICKED(IDC_DEL_BUTTON4, OnDsDelButton)
	ON_BN_CLICKED(IDC_L2ADD_BUTTON, OnL2addButton)
	ON_BN_CLICKED(IDC_L2MODIFY_BUTTON, OnL2modifyButton)
	ON_BN_CLICKED(IDC_DEL_BUTTON5, OnL2DelButton)
	ON_BN_CLICKED(IDC_SOCK5_RADIO, OnClickProxyType)
	ON_BN_CLICKED(IDC_HTTP_RADIO, OnClickProxyType)
	ON_BN_CLICKED(IDC_NLB_CHECK, OnNlbCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectCfgDlg message handlers

void CConnectCfgDlg::FillCombo(int which,BOOL bLast)
{
	int i,nSel;
	CString tempStr;
	long nMaxWidth = 0;
	if(which == 1)
	{
		nSel = m_ServerCombo.GetCurSel();
		m_ServerCombo.ResetContent();
		for(i=0;i<m_HQHost.HostNum;i++)
		{
			if(!m_bForbidOtherHost)
				tempStr.Format(" %s(%s)",m_HQHost.HostName[i],m_HQHost.IPAddress[i]);
			else
				tempStr.Format(" %s",m_HQHost.HostName[i]);
			m_ServerCombo.AddString(_F(tempStr));
			nMaxWidth = max(nMaxWidth,(long)strlen(tempStr));
		}
		m_ServerCombo.SetDroppedWidth(nMaxWidth*6.5);
		if(bLast)
			m_ServerCombo.SetCurSel(m_HQHost.HostNum-1);
		else
			m_ServerCombo.SetCurSel(nSel);
	}
	else if(which==2)
	{
		nSel = m_WTServerCombo.GetCurSel();
		m_WTServerCombo.ResetContent();
		for(i=0;i<m_WTHost.HostNum;i++)
		{
			if(!m_bForbidOtherHost)
				tempStr.Format(" %s(%s)",m_WTHost.HostName[i],m_WTHost.IPAddress[i]);
			else
				tempStr.Format(" %s",m_WTHost.HostName[i]);
			m_WTServerCombo.AddString(_F(tempStr));
			nMaxWidth = max(nMaxWidth,(long)strlen(tempStr));
		}
		m_WTServerCombo.SetDroppedWidth(nMaxWidth*6.5);
		if(bLast)
			m_WTServerCombo.SetCurSel(m_WTHost.HostNum-1);
		else
			m_WTServerCombo.SetCurSel(nSel);
	}
	else if(which==3)
	{
		nSel = m_InfoServerCombo.GetCurSel();
		m_InfoServerCombo.ResetContent();
		for(i=0;i<m_InfoHost.HostNum;i++)
		{
			if(!m_bForbidOtherHost)
				tempStr.Format(" %s(%s)",m_InfoHost.HostName[i],m_InfoHost.IPAddress[i]);
			else
				tempStr.Format(" %s",m_InfoHost.HostName[i]);
			m_InfoServerCombo.AddString(_F(tempStr));
			nMaxWidth = max(nMaxWidth,(long)strlen(tempStr));
		}
		m_InfoServerCombo.SetDroppedWidth(nMaxWidth*6.5);
		if(bLast)
			m_InfoServerCombo.SetCurSel(m_InfoHost.HostNum-1);
		else
			m_InfoServerCombo.SetCurSel(nSel);
	}
	else if(which==4)
	{
		nSel = m_DSServerCombo.GetCurSel();
		m_DSServerCombo.ResetContent();
		for(i=0;i<m_DSHost.HostNum;i++)
		{
			if(!m_bForbidOtherHost)
				tempStr.Format(" %s(%s)",m_DSHost.HostName[i],m_DSHost.IPAddress[i]);
			else
				tempStr.Format(" %s",m_DSHost.HostName[i]);
			m_DSServerCombo.AddString(_F(tempStr));
			nMaxWidth = max(nMaxWidth,(long)strlen(tempStr));
		}
		m_DSServerCombo.SetDroppedWidth(nMaxWidth*6.5);
		if(bLast)
			m_DSServerCombo.SetCurSel(m_DSHost.HostNum-1);
		else
			m_DSServerCombo.SetCurSel(nSel);
	}
	else if(which==5)
	{
		nSel = m_L2ServerCombo.GetCurSel();
		m_L2ServerCombo.ResetContent();
		for(i=0;i<m_L2Host.HostNum;i++)
		{
			tempStr.Format(" %s",m_L2Host.HostName[i]);
			m_L2ServerCombo.AddString(_F(tempStr));
			nMaxWidth = max(nMaxWidth,(long)strlen(tempStr));
		}
		m_L2ServerCombo.SetDroppedWidth(nMaxWidth*6.5);
		if(bLast)
			m_L2ServerCombo.SetCurSel(m_L2Host.HostNum-1);
		else
			m_L2ServerCombo.SetCurSel(nSel);
	}
}

void CConnectCfgDlg::AddButton(TDXWHOST &Host,int nWhichCombo)
{
	if(m_bForbidOtherHost) return;
	if(Host.HostNum >= 150)
	{
		TDX_MessageBox(m_hWnd,"主站太多，不能再增加!",MB_OK|MB_ICONINFORMATION);
		return;
	}
	CEditServerDlg dlg;
	dlg.m_bAdd = TRUE;
	dlg.m_nType = nWhichCombo;
	if(dlg.DoModal() == IDOK)
	{
		strncpy(Host.HostName[Host.HostNum],dlg.m_strServerName,48);
		Host.HostName[Host.HostNum][48]=0;
		strncpy(Host.IPAddress[Host.HostNum],dlg.m_strIPAddress,48);
		Host.IPAddress[Host.HostNum][48]=0;
		AllTrim(Host.IPAddress[Host.HostNum]);
		Host.Port[Host.HostNum] = dlg.m_nPort;
		Host.weight[Host.HostNum] = 0;
		Host.szYybids[Host.HostNum][0] = 0;
		Host.HostNum ++;
		FillCombo(nWhichCombo,TRUE);
	}
}

void CConnectCfgDlg::DelButton(TDXWHOST &Host,CComboBox *pComboBox,int nWhichCombo)
{
	if(m_bForbidOtherHost) return;
	if(Host.HostNum < 2 )
	{
		TDX_MessageBox(m_hWnd,"最后一个主站，不能删除!",MB_OK|MB_ICONINFORMATION);
		return;
	}
	if(TDX_MessageBox (m_hWnd,"您确定删除该主站吗？",MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL) 
		return;
	int nSel = pComboBox->GetCurSel();
	
	for(int i=0;i<=nSel;i++);
	if(i<=Host.HostNum&&i>0)
	{
		for(;i<Host.HostNum;i++)
		{
			memcpy(Host.HostName[i-1], Host.HostName[i], sizeof(Host.HostName[i]));
			memcpy(Host.IPAddress[i-1], Host.IPAddress[i], sizeof(Host.IPAddress[i]));
			memcpy(&Host.Port[i-1], &Host.Port[i], sizeof(Host.Port[i]));
			memcpy(&Host.weight[i-1], &Host.weight[i], sizeof(Host.weight[i]));
			memcpy(Host.szYybids[i-1], Host.szYybids[i], sizeof(Host.szYybids[i]));
		}
		--Host.HostNum;
	}
	if(i==Host.HostNum) --Host.HostNum;
	
	if(Host.PrimaryHost==nSel) Host.PrimaryHost = nSel%Host.HostNum;
	FillCombo(nWhichCombo);
	pComboBox->SetCurSel(nSel%Host.HostNum);
}

void CConnectCfgDlg::ModifyButton(TDXWHOST &Host,CComboBox *pComboBox,int nWhichCombo)
{
	if(m_bForbidOtherHost) return;
	int nSel = pComboBox->GetCurSel();
	if(nSel != CB_ERR)
	{
		CEditServerDlg dlg;
		dlg.m_bAdd = FALSE;
		dlg.m_nType = nWhichCombo;
		dlg.m_strServerName =	_F(Host.HostName[nSel]);
		dlg.m_strIPAddress	=	Host.IPAddress[nSel];
		dlg.m_nPort = Host.Port[nSel];
		if(dlg.DoModal() == IDOK)
		{
			strncpy(Host.HostName[nSel],dlg.m_strServerName,48);
			Host.HostName[nSel][48]=0;
			strncpy(Host.IPAddress[nSel],dlg.m_strIPAddress,48);
			Host.IPAddress[nSel][48]=0;
			AllTrim(Host.IPAddress[nSel]);
			Host.Port[nSel] = dlg.m_nPort;
			FillCombo(nWhichCombo);
		}
	}
}

void CConnectCfgDlg::OnAddButton() 
{
	AddButton(m_HQHost,1);
}

void CConnectCfgDlg::OnModifyButton()
{
	ModifyButton(m_HQHost,&m_ServerCombo,1);
}

void CConnectCfgDlg::OnHQDelButton()
{	
	DelButton(m_HQHost,&m_ServerCombo,1);
}

void CConnectCfgDlg::OnInfoaddButton() 
{
	AddButton(m_InfoHost,3);
}

void CConnectCfgDlg::OnInfomodifyButton() 
{
	ModifyButton(m_InfoHost,&m_InfoServerCombo,3);
}

void CConnectCfgDlg::OnInfoDelButton()
{	
	DelButton(m_InfoHost,&m_InfoServerCombo,3);
}

void CConnectCfgDlg::OnWTAddButton()
{	
	AddButton(m_WTHost,2);
}

void CConnectCfgDlg::OnWTModifyButton()
{
	ModifyButton(m_WTHost,&m_WTServerCombo,2);
}

void CConnectCfgDlg::OnWTDelButton()
{	
	DelButton(m_WTHost,&m_WTServerCombo,2);
}

void CConnectCfgDlg::OnDsaddButton() 
{
	AddButton(m_DSHost,4);
}

void CConnectCfgDlg::OnDsmodifyButton() 
{
	ModifyButton(m_DSHost,&m_DSServerCombo,4);
}

void CConnectCfgDlg::OnDsDelButton() 
{
	DelButton(m_DSHost,&m_DSServerCombo,4);
}

void CConnectCfgDlg::OnL2addButton() 
{
	AddButton(m_L2Host,5);
}

void CConnectCfgDlg::OnL2modifyButton() 
{
	ModifyButton(m_L2Host,&m_L2ServerCombo,5);
}

void CConnectCfgDlg::OnL2DelButton() 
{
	DelButton(m_L2Host,&m_L2ServerCombo,5);
}

////////////////////////////////////////////////////////////////////////////////////////////////

void CConnectCfgDlg::EnableHostSet(BOOL bNlb)
{
	GetDlgItem(IDC_L2ADD_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_L2MODIFY_BUTTON)->EnableWindow(FALSE);
	GetDlgItem(IDC_DEL_BUTTON5)->EnableWindow(FALSE);
	m_ServerCombo.EnableWindow(!bNlb);
	m_InfoServerCombo.EnableWindow(!bNlb);
	m_WTServerCombo.EnableWindow(!bNlb);
	m_DSServerCombo.EnableWindow(!bNlb);
	m_L2ServerCombo.EnableWindow(!bNlb);	
	GetDlgItem(IDC_HQ_STATIC)->EnableWindow(!bNlb);
	GetDlgItem(IDC_INFO_STATIC)->EnableWindow(!bNlb);
	GetDlgItem(IDC_WT_STATIC)->EnableWindow(!bNlb);
	GetDlgItem(IDC_DS_STATIC)->EnableWindow(!bNlb);
	GetDlgItem(IDC_L2_STATIC)->EnableWindow(!bNlb);
	if(!bNlb)
	{
		GetDlgItem(IDC_ADD_BUTTON)->EnableWindow(!m_bForbidOtherHost);
		GetDlgItem(IDC_MODIFY_BUTTON)->EnableWindow(!m_bForbidOtherHost);
		GetDlgItem(IDC_DEL_BUTTON1)->EnableWindow(!m_bForbidOtherHost);
		GetDlgItem(IDC_INFOADD_BUTTON)->EnableWindow(!m_bForbidOtherHost);
		GetDlgItem(IDC_INFOMODIFY_BUTTON)->EnableWindow(!m_bForbidOtherHost);
		GetDlgItem(IDC_DEL_BUTTON2)->EnableWindow(!m_bForbidOtherHost);
		GetDlgItem(IDC_WTADD_BUTTON)->EnableWindow(!m_bForbidOtherHost);
		GetDlgItem(IDC_WTMODIFY_BUTTON)->EnableWindow(!m_bForbidOtherHost);
		GetDlgItem(IDC_DEL_BUTTON3)->EnableWindow(!m_bForbidOtherHost);
		GetDlgItem(IDC_DSADD_BUTTON)->EnableWindow(!m_bForbidOtherHost);
		GetDlgItem(IDC_DSMODIFY_BUTTON)->EnableWindow(!m_bForbidOtherHost);
		GetDlgItem(IDC_DEL_BUTTON4)->EnableWindow(!m_bForbidOtherHost);
	}
	else
	{
		GetDlgItem(IDC_ADD_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_MODIFY_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_DEL_BUTTON1)->EnableWindow(FALSE);
		GetDlgItem(IDC_INFOADD_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_INFOMODIFY_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_DEL_BUTTON2)->EnableWindow(FALSE);
		GetDlgItem(IDC_WTADD_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_WTMODIFY_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_DEL_BUTTON3)->EnableWindow(FALSE);
		GetDlgItem(IDC_DSADD_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_DSMODIFY_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_DEL_BUTTON4)->EnableWindow(FALSE);
	}
}

BOOL CConnectCfgDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(g_GC.bShowVIPCfg)
	{
		GetDlgItem(IDC_HQ_STATIC)->SetWindowText(_F("VIP行情主站:"));
		GetDlgItem(IDC_INFO_STATIC)->SetWindowText(_F("VIP资讯主站:"));
		GetDlgItem(IDC_WT_STATIC)->SetWindowText(_F("VIP交易中心:"));
	}
	CString tmpHintInfo;
	tmpHintInfo.Format("自动连接%s主站",g_DS.DSOEMName);
	GetDlgItem(IDC_USEDS_CHECK)->SetWindowText(_F(tmpHintInfo));
	tmpHintInfo.Format("%s主站:",g_DS.DSOEMName);
	GetDlgItem(IDC_DS_STATIC)->SetWindowText(_F(tmpHintInfo));	
	m_bForbidOtherHost = g_bForbidOtherHost;
#ifdef LEVEL2
	if(g_nCurLevel2Tab==0)
		m_bForbidOtherHost = TRUE;
#endif

//	GetDlgItem(IDC_NLB_CHECK)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_TDXPROXY)->ShowWindow(g_bHasTdxProxy?SW_SHOW:SW_HIDE);
	if(!g_bHasTICQ)
		GetDlgItem(IDC_TQCFG)->ShowWindow(SW_HIDE);
	else
	{
		m_btnTQCfg.SetFlat(FALSE);
		m_btnTQCfg.SetColor(CButtonST::BTNST_COLOR_FG_IN,RGB(255,0,0));
		m_btnTQCfg.SetColor(CButtonST::BTNST_COLOR_FG_OUT,RGB(255,0,0));
		m_btnTQCfg.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS,RGB(255,0,0));
	}
	if(!m_bFirstLogoStatus || !g_OEM.bZszqEmbedWTVersion || g_GC.WhichAutoUpInfo==7) //期货版不能进行模式选择
		GetDlgItem(IDC_AREACFG)->ShowWindow(SW_HIDE);
	else
	{
		BOOL bHasCXMode=GetPrivateProfileInt("Version","HasCXMode",0,g_WSXHStr+g_strYxhjCfg);
		if(!bHasCXMode && g_OEM.bZszqEmbedWTVersion)
			GetDlgItem(IDC_AREACFG)->ShowWindow(SW_HIDE);
		else
		{
			m_btnAreaCfg.SetFlat(FALSE);
			m_btnAreaCfg.SetColor(CButtonST::BTNST_COLOR_FG_IN,RGB(0,0,255));
			m_btnAreaCfg.SetColor(CButtonST::BTNST_COLOR_FG_OUT,RGB(0,0,255));
			m_btnAreaCfg.SetColor(CButtonST::BTNST_COLOR_FG_FOCUS,RGB(0,0,255));
		}
	}
#ifdef OEM_NEWJY
	GetDlgItem(IDC_AUTOLOGO_CHECK)->ShowWindow(SW_HIDE);
#endif
	
	//如果是扩展行情中间无用自动停的话,则不显示[自动连接扩展行情主站]
	GetDlgItem(IDC_USEDS_CHECK)->ShowWindow((g_bHasDS && g_nDSAutoStopMinute==0)?SW_SHOW:SW_HIDE);

	BOOL bHasL2 = g_GC.bHasLevel2Engine;
	m_L2Host = g_L2HQHost;
#ifdef LEVEL2	
	GetHostInfoFromFile(&m_HQHost,&m_InfoHost,&m_WTHost,NULL,&m_DSHost,NULL,NULL,TRUE,g_WSXHStr+g_strYxhjCfg,(g_nCurLevel2Tab==0));
	bHasL2=FALSE;
#else
	GetHostInfoFromFile(&m_HQHost,&m_InfoHost,&m_WTHost,NULL,&m_DSHost,NULL,NULL,TRUE,g_WSXHStr+g_strYxhjCfg);
#endif
	//
	EnableHostSet(m_bNLB);
	//设置combo的高度
	m_ServerCombo.SetItemHeight(0,16);
	m_ServerCombo.SetItemHeight(-1,16);
	m_InfoServerCombo.SetItemHeight(0,16);
	m_InfoServerCombo.SetItemHeight(-1,16);
	m_WTServerCombo.SetItemHeight(0,16);
	m_WTServerCombo.SetItemHeight(-1,16);
	m_DSServerCombo.SetItemHeight(0,16);
	m_DSServerCombo.SetItemHeight(-1,16);
	m_L2ServerCombo.SetItemHeight(0,16);
	m_L2ServerCombo.SetItemHeight(-1,16);
	//
	FillCombo(1);
	FillCombo(2);
	FillCombo(3);
	FillCombo(4);
	FillCombo(5);

	if(m_HQHost.HostNum > 0 && m_HQHost.PrimaryHost > -1)
		m_ServerCombo.SetCurSel(m_HQHost.PrimaryHost);
	if(m_InfoHost.HostNum > 0 && m_InfoHost.PrimaryHost > -1)
		m_InfoServerCombo.SetCurSel(m_InfoHost.PrimaryHost);
	if(m_WTHost.HostNum > 0 && m_WTHost.PrimaryHost > -1)
		m_WTServerCombo.SetCurSel(m_WTHost.PrimaryHost);
	if(m_DSHost.HostNum > 0 && m_DSHost.PrimaryHost > -1)
		m_DSServerCombo.SetCurSel(m_DSHost.PrimaryHost);
	if(m_L2Host.HostNum > 0 && m_L2Host.PrimaryHost > -1)
		m_L2ServerCombo.SetCurSel(m_L2Host.PrimaryHost);
	
	m_bProxy = g_ProxyInfo.m_bProxy;
	m_nSockType = g_ProxyInfo.m_nSockType;
	m_nProxyPort = g_ProxyInfo.m_nProxyPort;
	m_strProxyUser = g_ProxyInfo.m_strProxyUser;
	m_strProxyPass = g_ProxyInfo.m_strProxyPass;
	m_ctrlProxyIP.SetWindowText(g_ProxyInfo.m_strProxyIP);
	m_bTdxProxy = g_ProxyInfo.bTdxProxy;
	UpdateData(FALSE);

	OnProxycheck();
	if(m_bProxy)
		RadioEnable();

	if(!g_bHasInfo)
	{
		GetDlgItem(IDC_INFOSERVER_COMBO)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_INFOADD_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_INFOMODIFY_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_INFO_STATIC)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_AUTOCONNECTIN_CHECK)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DEL_BUTTON2)->ShowWindow(SW_HIDE);
	}
	if(m_bHasWTSet)
	{
		GetDlgItem(IDC_WTSERVER_COMBO)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_WTADD_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_WTMODIFY_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_WT_STATIC)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DEL_BUTTON3)->ShowWindow(SW_SHOW);
	}
	if(bHasL2)
	{
		GetDlgItem(IDC_L2SERVER_COMBO)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_L2ADD_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_L2MODIFY_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_L2_STATIC)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DEL_BUTTON5)->ShowWindow(SW_SHOW);
	}
	if(g_bHasDS)
	{
		GetDlgItem(IDC_DSSERVER_COMBO)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DSADD_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DSMODIFY_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DS_STATIC)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DEL_BUTTON4)->ShowWindow(SW_SHOW);
	}
	return TRUE;
}

void CConnectCfgDlg::OnOK() 
{
	UpdateData(TRUE);
	int nSel = m_ServerCombo.GetCurSel();
	if(nSel != CB_ERR) m_HQHost.PrimaryHost = nSel;

	nSel = m_InfoServerCombo.GetCurSel();
	if(nSel != CB_ERR) m_InfoHost.PrimaryHost = nSel;

	nSel = m_WTServerCombo.GetCurSel();
	if(nSel != CB_ERR) m_WTHost.PrimaryHost = nSel;	

	nSel = m_DSServerCombo.GetCurSel();
	if(nSel != CB_ERR) m_DSHost.PrimaryHost = nSel;	

	nSel = m_L2ServerCombo.GetCurSel();
	if(nSel != CB_ERR) m_L2Host.PrimaryHost = nSel;	

	g_ProxyInfo.m_bProxy = m_bProxy;
	g_ProxyInfo.m_nSockType = m_nSockType;
	g_ProxyInfo.m_nProxyPort = m_nProxyPort;
	strcpy(g_ProxyInfo.m_strProxyUser,m_strProxyUser);
	strcpy(g_ProxyInfo.m_strProxyPass,m_strProxyPass);
	m_ctrlProxyIP.GetWindowText(g_ProxyInfo.m_strProxyIP,99);

	g_ProxyInfo.bTdxProxy = m_bTdxProxy;

	g_bAutoConnectInfo = m_bAutoConnectInfo;
	g_bAutoLogo = m_bAutoLogo;
	g_bAutoDS = m_bAutoDS;
	g_bNLB = m_bNLB;

	//是否打开查找最快
//	WritePrivateProfileString("Other","NLB",g_bNLB?"1":"0",g_WSXHStr+"syscomm.ini");
	//代理信息
	WritePrivateProfileString("PROXY","HasProxy",g_ProxyInfo.m_bProxy?"1":"0",g_WSXHStr+g_strYxhjCfg);
	WritePrivateProfileString("PROXY","SockType",IntToStr(g_ProxyInfo.m_nSockType),g_WSXHStr+g_strYxhjCfg);
	WritePrivateProfileString("PROXY","ProxyPort",IntToStr(g_ProxyInfo.m_nProxyPort),g_WSXHStr+g_strYxhjCfg);
	WritePrivateProfileString("PROXY","ProxyIP",g_ProxyInfo.m_strProxyIP,g_WSXHStr+g_strYxhjCfg);
	WritePrivateProfileString("PROXY","ProxyUser",g_ProxyInfo.m_strProxyUser,g_WSXHStr+g_strYxhjCfg);
	WritePrivateProfileString("PROXY","ProxyPass",g_ProxyInfo.m_strProxyPass,g_WSXHStr+g_strYxhjCfg);
	WritePrivateProfileString("TDXPROXY","buse",g_ProxyInfo.bTdxProxy?"1":"0",g_WSXHStr+g_strYxhjCfg);
	WritePrivateProfileString("TDXPROXY","Primary",IntToStr(g_ProxyInfo.tdxprimary),g_WSXHStr+g_strYxhjCfg);
	WritePrivateProfileString("AREA","MyProvineID",IntToStr(g_nMyProvineID),g_WSXHStr+g_strYxhjCfg);
	//启动后自动连接进入
	WritePrivateProfileString("Other","AutoConnectIn",g_bAutoConnectInfo?"1":"0",g_WSXHStr+g_strYxhjCfg);
	WritePrivateProfileString("Other","AutoLogo",g_bAutoLogo?"1":"0",g_WSXHStr+g_strYxhjCfg);	
//	WritePrivateProfileString("Other","UseDS",g_bAutoDS?"1":"0",g_WSXHStr+"syscomm.ini");
	//存入Connect信息
#ifndef LEVEL2
	WriteHostInfoToFile(&m_HQHost,&m_InfoHost,&m_WTHost,NULL,&m_DSHost,NULL,NULL,TRUE,g_WSXHStr+g_strYxhjCfg);
#else
	if(g_nCurLevel2Tab==1)
		WriteHostInfoToFile(&m_HQHost,&m_InfoHost,&m_WTHost,NULL,&m_DSHost,NULL,NULL,TRUE,g_WSXHStr+g_strYxhjCfg);
	else
		WriteHostInfoToFile(NULL,&m_InfoHost,&m_WTHost,NULL,&m_DSHost,NULL,NULL,TRUE,g_WSXHStr+g_strYxhjCfg);
#endif
	//对系统的g_HQHost,g_DSHost重新赋值
	g_HQHost=m_HQHost;
	g_DSHost=m_DSHost;
	g_L2HQHost.PrimaryHost=m_L2Host.PrimaryHost;
	WritePrivateProfileString("OTHERHOST","L2PrimaryHost",IntToStr(g_L2HQHost.PrimaryHost),g_WSXHStr+g_strYxhjCfg);	
	//
	CDialog::OnOK();
}

void CConnectCfgDlg::OnProxycheck() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_SOCK4_RADIO)->EnableWindow(m_bProxy);
	GetDlgItem(IDC_SOCK5_RADIO)->EnableWindow(m_bProxy);
	GetDlgItem(IDC_HTTP_RADIO)->EnableWindow(m_bProxy);
	GetDlgItem(IDC_PROXYIP)->EnableWindow(m_bProxy);
	GetDlgItem(IDC_PROXYPORT)->EnableWindow(m_bProxy);
	GetDlgItem(IDC_PROXYUSER)->EnableWindow(m_bProxy&&m_nSockType!=0);
	GetDlgItem(IDC_PROXYPASS)->EnableWindow(m_bProxy&&m_nSockType!=0);
	GetDlgItem(IDC_PROXYUSER_STATIC)->EnableWindow(m_bProxy&&m_nSockType!=0);
	GetDlgItem(IDC_PROXYPASS_STATIC)->EnableWindow(m_bProxy&&m_nSockType!=0);

	GetDlgItem(IDC_TESTPROXY)->ShowWindow((m_bTdxProxy||m_bProxy)?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_SELECTPROXY)->ShowWindow((m_bTdxProxy&&g_ProxyInfo.tdxproxynum>1)?SW_SHOW:SW_HIDE);
}

void CConnectCfgDlg::OnClickProxyType()
{
	RadioEnable();
	if(m_nSockType!=2)
		m_nProxyPort=1080;
	else
		m_nProxyPort=80;
	UpdateData(FALSE);
}

void CConnectCfgDlg::RadioEnable()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_PROXYUSER)->EnableWindow(m_nSockType!=0);
	GetDlgItem(IDC_PROXYPASS)->EnableWindow(m_nSockType!=0);
	GetDlgItem(IDC_PROXYUSER_STATIC)->EnableWindow(m_nSockType!=0);
	GetDlgItem(IDC_PROXYPASS_STATIC)->EnableWindow(m_nSockType!=0);	
}

void CConnectCfgDlg::OnTdxproxy() 
{
	UpdateData(TRUE);
	if(m_bTdxProxy)
	{
		if(TDX_MessageBox(m_hWnd,"绿色代理通道适用于办公网内由于上网端口限制不能使用本软件的情况,\r\n如果通过代理上网的话,在打开绿色通道时,也需要设置代理服务器地址,\r\n绿色代理方式会造成网络延时,如不是万不得己,请不要选用.\r\n如果使用后不能连接主站,请关闭此选项.\r\n确定使用吗?",MB_OKCANCEL|MB_ICONQUESTION)==IDCANCEL)
		{
			m_bTdxProxy=FALSE;
			UpdateData(FALSE);
		}
		OnSelectproxy();
	}
	GetDlgItem(IDC_SELECTPROXY)->ShowWindow((m_bTdxProxy&&g_ProxyInfo.tdxproxynum>1)?SW_SHOW:SW_HIDE);
	GetDlgItem(IDC_TESTPROXY)->ShowWindow((m_bTdxProxy||m_bProxy)?SW_SHOW:SW_HIDE);
}

#include "SelectOneDlg.h"
void CConnectCfgDlg::OnSelectproxy()
{
	if(g_ProxyInfo.tdxproxynum>1)
	{
		CSelectOneDlg dlg;
		dlg.m_nCurSel = g_ProxyInfo.tdxprimary;
		dlg.m_strHintStr=_F("请选择合适的绿色通道服务器:");
		for(int n=0;n<g_ProxyInfo.tdxproxynum;n++)
		{
			if(strlen(g_ProxyInfo.tdxdesc[n])==0)
				dlg.AddString(_F(g_ProxyInfo.tdxip[n]));
			else
				dlg.AddString(_F(g_ProxyInfo.tdxdesc[n]));
		}
		dlg.DoModal();
		if(dlg.m_nCurSel<0) dlg.m_nCurSel=0;
		g_ProxyInfo.tdxprimary=dlg.m_nCurSel;
	}
}

void CConnectCfgDlg::OnTestproxy() 
{
	int nSel = m_ServerCombo.GetCurSel();
	if(nSel != CB_ERR)
		m_HQHost.PrimaryHost = nSel;	
	if(m_HQHost.PrimaryHost<0) 
	{
		TDX_MessageBox(m_hWnd,"没有选中行情主站",MB_OK|MB_ICONERROR);
		return;
	}
	UpdateData(TRUE);
	//
	PROXYINFO tmpProxy = g_ProxyInfo;
	tmpProxy.m_bProxy = m_bProxy;
	tmpProxy.m_nSockType = m_nSockType;
	tmpProxy.m_nProxyPort = m_nProxyPort;
	strcpy(tmpProxy.m_strProxyUser,m_strProxyUser);
	strcpy(tmpProxy.m_strProxyPass,m_strProxyPass);
	m_ctrlProxyIP.GetWindowText(tmpProxy.m_strProxyIP,99);
	tmpProxy.bTdxProxy=m_bTdxProxy;
	//
	SOCKET tmpSock;
	long tick1 = GetTickCount();
	char errMsg[255]={0};
	BOOL bRet=DirectConnect(tmpSock,tmpProxy,m_HQHost.IPAddress[m_HQHost.PrimaryHost],m_HQHost.Port[m_HQHost.PrimaryHost],tick1,errMsg);
	closesocket(tmpSock);
	if(!bRet)
		TDX_MessageBox(m_hWnd,"测试结果:通过代理不能连接到当前行情主站.",MB_OK|MB_ICONEXCLAMATION);
	else
		TDX_MessageBox(m_hWnd,"测试结果:通过代理连接成功.",MB_OK|MB_ICONINFORMATION);	
}

#include "TQCfgDlg.h"
void CConnectCfgDlg::OnTqcfg() 
{
	CTQCfgDlg dlg;
	dlg.DoModal();
}

#include "AreaCfgDlg.h"
void CConnectCfgDlg::OnAreacfg()
{
	CAreaCfgDlg dlg;
	dlg.DoModal();
}

void CConnectCfgDlg::OnNlbCheck() 
{
	UpdateData(TRUE);
	EnableHostSet(m_bNLB);
}

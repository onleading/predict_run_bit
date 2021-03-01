// ProxyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ProxyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProxyDlg dialog


CProxyDlg::CProxyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProxyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProxyDlg)
	m_bProxy = FALSE;
	m_nProxyPort = 0;
	m_strProxyPass = _T("");
	m_strProxyUser = _T("");
	m_nSockType = -1;
	//}}AFX_DATA_INIT
}


void CProxyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProxyDlg)
	DDX_Text(pDX, IDC_PROXYIP, m_strProxyIP);
	DDV_MaxChars(pDX, m_strProxyIP, 99);
	DDX_Check(pDX, IDC_PROXYCHECK, m_bProxy);
	DDX_Text(pDX, IDC_PROXYPORT, m_nProxyPort);
	DDV_MinMaxUInt(pDX, m_nProxyPort, 0, 65535);
	DDX_Text(pDX, IDC_PROXYPASS, m_strProxyPass);
	DDV_MaxChars(pDX, m_strProxyPass, 99);
	DDX_Text(pDX, IDC_PROXYUSER, m_strProxyUser);
	DDV_MaxChars(pDX, m_strProxyUser, 99);
	DDX_Radio(pDX, IDC_SOCK4_RADIO, m_nSockType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProxyDlg, CDialog)
	//{{AFX_MSG_MAP(CProxyDlg)
	ON_BN_CLICKED(IDC_PROXYCHECK, OnProxycheck)
	ON_BN_CLICKED(IDC_SOCK4_RADIO, OnClickProxyType)
	ON_BN_CLICKED(IDC_SOCK5_RADIO, OnClickProxyType)
	ON_BN_CLICKED(IDC_HTTP_RADIO, OnClickProxyType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProxyDlg message handlers
BOOL CProxyDlg::OnInitDialog() 
{
	m_bProxy = g_ProxyInfo.m_bProxy;
	m_nSockType = g_ProxyInfo.m_nSockType;
	m_nProxyPort = g_ProxyInfo.m_nProxyPort;
	m_strProxyUser = g_ProxyInfo.m_strProxyUser;
	m_strProxyPass = g_ProxyInfo.m_strProxyPass;
	m_strProxyIP = g_ProxyInfo.m_strProxyIP;
	UpdateData(FALSE);

	OnProxycheck();
	if(m_bProxy)
		RadioEnable();

	return TRUE;
}

void CProxyDlg::OnOK() 
{
	UpdateData(TRUE);
	
	g_ProxyInfo.m_bProxy = m_bProxy;
	g_ProxyInfo.m_nSockType = m_nSockType;
	g_ProxyInfo.m_nProxyPort = m_nProxyPort;
	strcpy(g_ProxyInfo.m_strProxyUser,m_strProxyUser);
	strcpy(g_ProxyInfo.m_strProxyPass,m_strProxyPass);
	strcpy(g_ProxyInfo.m_strProxyIP,m_strProxyIP);

	//代理信息
	WritePrivateProfileString("PROXY","HasProxy",g_ProxyInfo.m_bProxy?"1":"0",g_WSXHStr+g_strUserCfg);
	WritePrivateProfileString("PROXY","SockType",IntToStr(g_ProxyInfo.m_nSockType),g_WSXHStr+g_strUserCfg);
	WritePrivateProfileString("PROXY","ProxyPort",IntToStr(g_ProxyInfo.m_nProxyPort),g_WSXHStr+g_strUserCfg);
	WritePrivateProfileString("PROXY","ProxyIP",g_ProxyInfo.m_strProxyIP,g_WSXHStr+g_strUserCfg);
	WritePrivateProfileString("PROXY","ProxyUser",g_ProxyInfo.m_strProxyUser,g_WSXHStr+g_strUserCfg);
	WritePrivateProfileString("PROXY","ProxyPass",g_ProxyInfo.m_strProxyPass,g_WSXHStr+g_strUserCfg);
	WritePrivateProfileString("TDXPROXY","buse",g_ProxyInfo.bTdxProxy?"1":"0",g_WSXHStr+g_strUserCfg);
	WritePrivateProfileString("TDXPROXY","Primary",IntToStr(g_ProxyInfo.tdxprimary),g_WSXHStr+g_strUserCfg);

	CDialog::OnOK();
}

void CProxyDlg::OnProxycheck() 
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
}

void CProxyDlg::OnClickProxyType()
{
	RadioEnable();
	if(m_nSockType!=2)
		m_nProxyPort=1080;
	else
		m_nProxyPort=80;
	UpdateData(FALSE);
}

void CProxyDlg::RadioEnable()
{
	UpdateData(TRUE);
	GetDlgItem(IDC_PROXYUSER)->EnableWindow(m_nSockType!=0);
	GetDlgItem(IDC_PROXYPASS)->EnableWindow(m_nSockType!=0);
	GetDlgItem(IDC_PROXYUSER_STATIC)->EnableWindow(m_nSockType!=0);
	GetDlgItem(IDC_PROXYPASS_STATIC)->EnableWindow(m_nSockType!=0);	
}

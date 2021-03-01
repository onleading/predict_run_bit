// LogoDlg.cpp : implementation file
#include "stdafx.h"
#include "tdxw.h"
#include "LogoDlg.h"
#include "HQComm.h"
#include "getdata.h"
#include "GuideDlg.h"
#include "verify.h"
#include "VUsercomm.h"
#include "U_SelUser.h"
#include "U_TestZjzh.h"
#include "MyInterface.h"
#include "AuthPassMng.h"
#include "L2FaceDlg.h"
#include "HQControl.h"
#include "blowfish.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void GetL2Info(CString & L2Username,CString & L2Password,int & nType);

BOOL l_bTryOutHost=FALSE;
BOOL l_bVipHost=FALSE;
BOOL l_bCXMode=FALSE;
BOOL l_bHideJYControl=FALSE;

CLogoDlg::CLogoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogoDlg)
	m_strPassword = _T("");
	m_bSavePass = FALSE;
	m_strUserName = _T("");
	m_nMode = 0;
	m_bSaveZH = FALSE;
	//}}AFX_DATA_INIT
	m_bHasOffMode = TRUE;
	m_bDirectConnect = FALSE;
	m_bDisableNLB = FALSE;
	m_pSZTmp = NULL;
	m_pSHTmp = NULL;
	m_pSZMem = NULL;
	m_pSHMem = NULL;
	
	hOEMBitmap = NULL;

	m_nMainID = 0;

	bFirstUsedHostIndex = -1;
	UsedHostIndexNum = 0;

	m_nReConnectNum = 0;

	m_bFirstShow = TRUE;
	m_bHasPackTimer = m_bHasPackTimer3 = FALSE; //是否有第一包返回的定时器

	Reset();

	memset(&m_BakHostInfo,0,sizeof(struct BakHostInfo));
	m_BakHostInfo.WhichHost=-1;
	m_bEnableNLB=FALSE;
	m_bOnlyLoginWT=FALSE;

#ifdef OEM_NEWJY
	if(g_resModule)
	{
		m_draw.Init();
		m_draw.GetWndRgn(m_wndRgn);
	}
	m_pCloseBt=m_pCfgBt=m_pHelpBt=m_pSysTestBt=NULL;
	m_pSel1Bt=m_pSel2Bt=m_pSel3Bt=NULL;
	m_pOKBt=m_pCancelBt=m_pCABt=m_pSoftKeyBt=NULL;
	m_pOffline=NULL;
	m_bShowYzm = FALSE;
	memset(&m_yzm,0,sizeof(m_yzm));
#endif
	m_pSpeedTestDlg=NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_L2UserName = "";
	m_L2Password = "";
	m_nL2Type = 0;
	m_OEMBmpRect.SetRectEmpty();
	m_pADIE = NULL;
	m_SoftFocus = 0;
	m_hYhzqIcon = m_hZxjtIcon = 0;
}

void CLogoDlg::Reset()
{
	g_bFirstStep = TRUE;

	szstknum = shstknum = szstartxh = shstartxh = 0;
	szinfdate = shinfdate = g_iToday;
	szinfhms = shinfhms = 0;
	basedifnum = gbbqdifnum = 0;
	basestartxh = gbbqstartxh = 0;
	gbbqpos.setcode = basepos.setcode = 0;
	gbbqpos.pos = basepos.pos = 0;

	blockdat_filelen = 0;
	blockdat_pos = 0;
	zhbzip_filelen = 0;
	zhbzip_pos = 0;
	jgdata_filelen = 0;
	jgdata_pos = 0;
	otherfile_filelen = 0;
	otherfile_pos = 0;
	FileBuffer = NULL;
}

CLogoDlg::~CLogoDlg()
{
	SoftKey_Close();

	TDEL(m_pSZTmp);
	TDEL(m_pSHTmp);
	TDEL(m_pSZMem);
	TDEL(m_pSHMem);
	TDEL(FileBuffer);
	if(hOEMBitmap)
		DeleteObject(hOEMBitmap);
#ifdef OEM_NEWJY
	m_wndRgn.DeleteObject();
	TDEL(m_pCancelBt);
	TDEL(m_pCloseBt);
	TDEL(m_pSel1Bt);
	TDEL(m_pSel2Bt);
	TDEL(m_pSel3Bt);
	TDEL(m_pCABt);
	TDEL(m_pSoftKeyBt);
	TDEL(m_pHelpBt);
	TDEL(m_pCfgBt);
	TDEL(m_pSysTestBt);
	TDEL(m_pOKBt);
	TDEL(m_pOffline);
#endif
	TDEL(m_pSpeedTestDlg);
}

void CLogoDlg::GetFromUserHostCfg()
{
	CString	Ini = HomePath+"userhost.cfg";
	CString tempStr;
	l_YYBNum = GetPrivateProfileInt("YYBInfo","YYBNum",0,Ini);
	l_YYBNum=max(0,l_YYBNum);
	l_YYBNum=min(l_YYBNum,200);
	l_PrimaryYYB = GetPrivateProfileInt("YYBInfo","PrimaryYYB",0,Ini);
	TDEL(l_pYYBInfo);
	l_pYYBInfo = new struct tag_YYBInfo[l_YYBNum];
	for(int i=0;i < l_YYBNum;i++)
	{
		tempStr.Format("YYBINFO_ID%02d",i+1);		//YYBInfo标识
		GetPrivateProfileString("YYBInfo",tempStr,"00",l_pYYBInfo[i].YYBInfo_ID,11,Ini);
		tempStr.Format("YYBINFO_STR%02d",i+1);		//YYBInfo名称
		GetPrivateProfileString("YYBInfo",tempStr,"00",l_pYYBInfo[i].YYBInfo_Str,48,Ini);		
	}
}

void CLogoDlg::InitOtherLogoInfo()
{
	//如果在私人目录下没有bd.dbf,必要拷贝一个过来
	CFile OtherFile;
	if(OtherFile.Open(g_WSXHStr+"LData\\cache\\bd.dbf",CFile::modeRead|CFile::shareDenyNone))
		OtherFile.Close();
	else
		::CopyFile(HomePath+"cwinfo.dat",g_WSXHStr+"LData\\cache\\bd.dbf",FALSE);
}

int GetSecureTypeFromSel(int nSel)
{
	if(!wtdef.bShowSSL && wtdef.bAuthPass && nSel==0)
		return SCLV_AUTHPASS;
	if(!wtdef.bShowSSL && !wtdef.bAuthPass && nSel==0)
		return SCLV_CA;
	if(!wtdef.bShowSSL && wtdef.bAuthPass && nSel==1)
		return SCLV_CA;
	if(wtdef.bShowSSL && !wtdef.bAuthPass && nSel==1)
		return SCLV_CA;
	if(nSel==1)	
		return SCLV_AUTHPASS;
	if(nSel==2)	
		return SCLV_CA;
	return SCLV_SSL;
}

void CLogoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogoDlg)
	DDX_Control(pDX, IDC_COMBO_AQFS, m_Aqfs_Combo);
	DDX_Control(pDX, IDC_CONNECTCFG_BUTTON, m_ConnectCFGButton);
	DDX_Control(pDX, IDC_PASSWORD, m_PassEdit);
	DDX_Control(pDX, IDC_TAB, m_Tab);
	DDX_Control(pDX, IDC_SPEEDTEST, m_SpeedTestLink);
	DDX_Control(pDX, IDC_LOGINTYPE_COMBO, m_LoginType_Combo);
	DDX_Control(pDX, IDC_LOGO_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_HQHOST_COMBO, m_HqHost_Combo);
	DDX_Control(pDX, IDC_STATUS_STATIC, m_Status_Static);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, 19);
	DDX_Check(pDX, IDC_SAVEPASS_CHECK, m_bSavePass);
	DDX_Check(pDX, IDC_NLB_CHECK, m_bNLB);
	DDX_Text(pDX, IDC_USERNAME, m_strUserName);
	DDV_MaxChars(pDX, m_strUserName, 49);
	DDX_Radio(pDX, IDC_MODE1, m_nMode);
	DDX_Check(pDX, IDC_CHECK_SAVEZH, m_bSaveZH);
	DDX_Control(pDX, IDC_CHECK_SAVEZH, m_SaveZHButton);
	DDX_Check(pDX, IDC_CHECK_PROTECTZH, m_bProtectZH);
	DDX_Control(pDX, IDC_CHECK_PROTECTZH, m_ProtectZHButton);
	DDX_Control(pDX, IDC_REG_EXIT, m_RegExit);
	DDX_Control(pDX, IDC_REG_LOG, m_RegLogin);
	DDX_Control(pDX, IDC_PROXY_INFO, m_ProxyInfo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLogoDlg, CDialog)
	//{{AFX_MSG_MAP(CLogoDlg)
	ON_MESSAGE(WM_CHECK_HEXINWT,OnCheckHexinWT)
	ON_MESSAGE(WM_CHECK_TDXWT,OnCheckTDXWT)
	ON_BN_CLICKED(IDC_CONNECTCFG_BUTTON, OnConnectcfgButton)
//	ON_BN_CLICKED(IDC_PROXYCFG, OnProxycfgButton)

	ON_BN_CLICKED(IDC_LEVIN2CFG, OnLevel2Cfg)
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataACK)
	ON_MESSAGE(UM_STARTCONNECT,StartConnect)
	ON_MESSAGE(UM_STARTLOGIN,StartLogin)
	ON_MESSAGE(UM_CONNECT_SUCCESS,OnConnectResult)
	ON_CBN_SELCHANGE(IDC_HQHOST_COMBO, OnSelchangeHqhostCombo)
	ON_BN_CLICKED(IDC_OFFLINE, OnOffline)
	ON_BN_CLICKED(ID_CABT, OnCA)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_NCHITTEST()
	ON_MESSAGE(UM_LOGOENBALE,OnLogoEnableMessage)
	ON_MESSAGE(UM_SWITCHNEXTHOST,SwitchNextHostMsg)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_CBN_SELCHANGE(IDC_LOGINTYPE_COMBO, OnSelchangeLogintypeCombo)
	ON_MESSAGE(UM_REGLINK_MESSAGE2,OnSpeedTest)
	ON_BN_CLICKED(IDC_NLB_CHECK, OnNLBCheckClick)
	ON_CBN_SELCHANGE(IDC_COMBO_AQFS, OnSelchangeComboAqfs)
	ON_MESSAGE(UM_SOFTKEY_NOTIFY,OnSoftkeyMsg)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_COPYDATA()
	ON_CBN_KILLFOCUS(ID_LOGO_EDIT0, OnKillfocusGddmCombo)
	ON_CBN_SETFOCUS(ID_LOGO_EDIT0, OnSetfocusZhInfo)
	ON_EN_SETFOCUS(ID_LOGO_EDIT1, OnSetfocusPassword)
	ON_EN_SETFOCUS(ID_LOGO_EDIT2, OnSetfocusAuthpwd)
	ON_BN_CLICKED(IDC_CHECK_PROTECTZH, OnCheckProtectZH)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_COMMAND(ID_JY_REGUSER,On_Jy_RegUser)
	ON_COMMAND(ID_JY_ACTIVEUSER,On_Jy_ActiveUser)
	ON_MESSAGE(UM_REGEXIT_MESSAGE,OnRegExitMessage)
	ON_MESSAGE(UM_REGLOGIN_MESSAGE,OnRegLoginMessage)
	ON_MESSAGE(UM_PROXY_MESSAGE,OnProxycfgButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void SetAdvHQEnvironment(BOOL bHasEverConnect)
{
	if(bHasEverConnect)
	{
		g_bUserHostEverConnect=TRUE;
		g_bHasUserHost=TRUE;
		g_bHasAdvHQVersion = TRUE;
		strcpy(g_GC.productnbbs,"tdxlevel2");
		g_nLastLoginType=REGISTER_LOGIN;
	}
	else
	{
		g_bUserHostEverConnect=FALSE;
		g_bHasUserHost=FALSE;
		g_bHasAdvHQVersion=FALSE;
	}
}

void GetFromConnectCfg()
{
	//是否是第一次读取配置
	static BOOL l_bFirstGetFromConnectCfg = TRUE;

	int i,j;
	CString	Ini = g_WSXHStr+g_strYxhjCfg;
	CString tempStr;
	char	tempStr2[255];
	
#if defined(OEM_NEWJY) || defined(LEVEL2)
	BOOL bRet1=GetFromComteFile(COMTE_L2_USERHOST,FALSE,FALSE,COMTE_PASSWORD);
	BOOL bRet2=GetFromComteFile(COMTE_L2_HOST,FALSE,FALSE,COMTE_PASSWORD);
	if(!bRet1 || !bRet2 || g_L2HQHost.HostNum<1)
	{
		GetFromComteFile(COMTE_L2_USERHOST,FALSE,TRUE,COMTE_PASSWORD);
		GetFromComteFile(COMTE_L2_HOST,FALSE,TRUE,COMTE_PASSWORD);
	}
	GetFromComteFile(COMTE_VIP_HOST,TRUE,FALSE,COMTE_PASSWORD);
	g_L2HQHost.PrimaryHost = GetPrivateProfileInt("OTHERHOST","L2PrimaryHost",0,Ini);
	g_L2HQHost.PrimaryHost = MAX(0,g_L2HQHost.PrimaryHost);
	g_L2HQHost.PrimaryHost = MIN(g_L2HQHost.HostNum-1,g_L2HQHost.PrimaryHost);
#else
	//用户管理服务器的配置:主认证服务器和辅认证服务器
	GetPrivateProfileString("USERHOST","IPAddress","",g_UserHostIP,48,Ini);
	g_nUserHostPort = GetPrivateProfileInt("USERHOST","Port",7715,Ini);
	GetPrivateProfileString("USERHOST","BakIPAddress","",g_BakUserHostIP,48,Ini);
	g_nBakUserHostPort = GetPrivateProfileInt("USERHOST","BakPort",7715,Ini);
	GetPrivateProfileString("USERHOST","BakIPAddress2","",g_BakUserHostIP2,48,Ini);
	g_nBakUserHostPort2 = GetPrivateProfileInt("USERHOST","BakPort2",7715,Ini);
#endif
	//取主站信息
#ifdef LEVEL2
	GetHostInfoFromFile(&g_HQHost,NULL,NULL,NULL,&g_DSHost,NULL,&g_VipVerify_Info,TRUE,g_WSXHStr+g_strYxhjCfg,(g_nCurLevel2Tab==0));
#else
	GetHostInfoFromFile(&g_HQHost,NULL,NULL,NULL,&g_DSHost,NULL,&g_VipVerify_Info,TRUE,g_WSXHStr+g_strYxhjCfg);
	if(g_HQHost.HostNum == 0)
	{
	//	TDX_MessageBox(NULL,"不能找到主站设置文件,可能原因:\r\n1.快捷方式的起始位置不正确.\r\n2:操作系统被某些恶意程序损坏.\r\n3:程序文件被破坏",MB_OK|MB_ICONERROR);
		g_HQHost.HostNum = 3;
		g_HQHost.PrimaryHost = 0;
		strcpy(g_HQHost.IPAddress[0], "222.73.49.30");
		g_HQHost.Port[0] = 7709;
		strcpy(g_HQHost.IPAddress[1], "222.73.228.210");
		g_HQHost.Port[1] = 7709;
		strcpy(g_HQHost.IPAddress[2], "121.14.110.201");
		g_HQHost.Port[2] = 7709;
	}
#endif
	//如果三方行情没有配置,则不打开三方行情主站的开关
	if(g_DSHost.HostNum<1)
		g_bHasDS=FALSE;	
	//取试用主站信息
	GetPrivateProfileString("HQHOST","TryOutIP","",tempStr2,98,Ini);
	g_HQTryOutIP=tempStr2;
	g_HQTryOutPort=GetPrivateProfileInt("HQHOST","TryOutPort",7709,Ini);

	g_bHasInfo = GetPrivateProfileInt("INFOHOST","Info",0,Ini);			//可能没有资讯

	//第一次登录,要读取扩展市场的本地配置
	if(l_bFirstGetFromConnectCfg)
	{
		GetDSOtherCfg();
		GetDSMarketCfg(HomePath+"dsmarket.dat");
	}
	//外部网页调用,先看user.ini有没有，如果没有，再从TopView.ini中读取
	g_nCusWebPageNum=0;
	CString CusWebIni=Ini;
	tempStr.Format("WebPageURL%d",0);
	DWORD nameLen = GetPrivateProfileString("EXTERN",tempStr,"",tempStr2,MAX_PATH,g_WSXHStr+g_strUserCfg);
	if(nameLen>=1) 
		CusWebIni=g_WSXHStr+g_strUserCfg;
	for(i=0;i < MAX_CUSWEB_NUM;i++)
	{
		tempStr.Format("WebPageURL%d",i);
		nameLen = GetPrivateProfileString("EXTERN",tempStr,"",g_CusWebPage[g_nCusWebPageNum].PageUrl,MAX_PATH,CusWebIni);
		if(nameLen<1) break;
		tempStr.Format("WebPageName%d",i);
		nameLen = GetPrivateProfileString("EXTERN",tempStr,"",g_CusWebPage[g_nCusWebPageNum].PageName,64,CusWebIni);
		if(nameLen<1) break;
		g_nCusWebPageNum++;
	}
	GetPrivateProfileString("EXTERN","F11WebPageURL","",g_F11WebPage.PageUrl,MAX_PATH,Ini);
	GetPrivateProfileString("EXTERN","F11WebPageName","",g_F11WebPage.PageName,64,Ini);
	//绿色通道代理	
	g_bHasTdxProxy = GetPrivateProfileInt("TDXPROXY","HasTdxProxy",0,Ini);
	GetPrivateProfileString("TDXPROXY","IP","",g_ProxyInfo.tdxip[0],49,Ini);
	GetPrivateProfileString("TDXPROXY","Desc","",g_ProxyInfo.tdxdesc[0],49,Ini);
	g_ProxyInfo.tdxport[0] = GetPrivateProfileInt("TDXPROXY","Port",443,Ini);
	g_ProxyInfo.tdxproxynum = 1;
	for(i=0;i<9;i++)
	{
		tempStr.Format("IP%d",i+2);
		GetPrivateProfileString("TDXPROXY",tempStr,"",g_ProxyInfo.tdxip[i+1],49,Ini);
		if(strlen(g_ProxyInfo.tdxip[i+1])==0) 
			break;
		tempStr.Format("Desc%d",i+2);
		GetPrivateProfileString("TDXPROXY",tempStr,"",g_ProxyInfo.tdxdesc[i+1],49,Ini);
		tempStr.Format("Port%d",i+2);
		g_ProxyInfo.tdxport[i+1] = GetPrivateProfileInt("TDXPROXY",tempStr,443,Ini);
		g_ProxyInfo.tdxproxynum++;
	}
	g_ProxyInfo.tdxprimary = GetPrivateProfileInt("TDXPROXY","Primary",0,g_WSXHStr+g_strYxhjCfg);
	if(g_ProxyInfo.tdxprimary<0 || g_ProxyInfo.tdxprimary>=g_ProxyInfo.tdxproxynum)
		g_ProxyInfo.tdxprimary = 0;
	//支持电子邮件地址
	GetPrivateProfileString("Other","EmailStr","",g_Url.EmailStr,48,Ini);
	GetPrivateProfileString("Other","EmailURL","",g_Url.EmailURL,250,Ini);
	//支持的帮助URL
	GetPrivateProfileString("Other","HelpWebStr","",g_Url.HelpWebStr,48,Ini);
	GetPrivateProfileString("Other","HelpWebURL","",g_Url.HelpWebURL,250,Ini);
	//支持外挂两个网站
//	TDX_GetPrivateProfileString("Other","Corp","访问网站",g_Url.Corp,48,Ini);
//	GetPrivateProfileString("Other","CorpWeb",TDX_WEBPAGE,g_Url.CorpWeb,250,Ini);
	//用户论坛
	GetPrivateProfileString("Other","ForumURL","",g_Url.ForumURL,250,Ini);
	if(strlen(g_Url.ForumURL)==0)
		strcpy(g_Url.ForumURL,TDXBBS_WEBPAGE);
	//WebInfo的URL串
	TDX_GetPrivateProfileString("Other","WebInfoURL","",g_Url.WebInfoURL,250,Ini);
	//额外的资讯菜单项
	TDX_GetPrivateProfileString("Other","AddZxStr","",g_Url.AddZxStr,48,Ini);
	TDX_GetPrivateProfileString("Other","AddZxURL","",g_Url.AddZxURL,250,Ini);	
	//合一版界面出现的链接区
	TDX_GetPrivateProfileString("Other","LogoHelpStr","",g_Url.LogoHelpStr,48,Ini);
	TDX_GetPrivateProfileString("Other","LogoHelpURL","",g_Url.LogoHelpURL,250,Ini);
	//名称前缀
//	GetPrivateProfileString("Other","PreName","",tempStr2,60,Ini);
//	g_strPreName = tempStr2;
//	g_strTitleName = g_strPreName+"决策平台";
//	GetPrivateProfileString("Other","TitleName",g_strTitleName,tempStr2,78,Ini);
//	g_strTitleName = tempStr2;
	strcpy(g_cwMenuTopic[0],g_strTitleName);
//	GetPrivateProfileString("Other","HelpURL","",g_Url.HelpURL,250,Ini);
	strcpy(g_Url.HelpURL, "bainian.yhglpt.com/qjjk/sms.asp");
	//是否风险揭示,缺省为真,但对于某些券商(比如华泰,为FALSE)
	g_bRiskPost = GetPrivateProfileInt("Other","RiskPost",1,Ini);
	//启动后自动连接进入资讯
	g_bAutoConnectInfo = GetPrivateProfileInt("Other","AutoConnectIn",0,Ini);
	//启动直接登录行情,合一版不支持
#ifndef OEM_NEWJY
	g_bAutoLogo = GetPrivateProfileInt("Other","AutoLogo",0,Ini);
#endif
	g_bAutoDS = GetPrivateProfileInt("Other","UseDS",0,g_WSXHStr+"syscomm.ini");
	//是否在系统中显示版本号
	g_bShowVersoin = GetPrivateProfileInt("Other","ShowVersoin",1,Ini);
	//在登录对话框中显示广告词
	GetPrivateProfileString("Other","LogoAdvert","",g_strLogoAdvert,198,Ini);
	//辅助区采用右边
	g_bRestRightBtn = GetPrivateProfileInt("Other","RestRightBtn",0,Ini);	
	//每次重进系统选择第一个主站
	g_bEveryFirst = GetPrivateProfileInt("Other","EveryFirstHost",0,Ini);	
	//登录相关信息(4种登录类型)
	g_LoginInfo.Has_GuestLogin=GetPrivateProfileInt("UserHost","GuestLogin",1,Ini);
	g_LoginInfo.Has_RegisterLogin=GetPrivateProfileInt("UserHost","RegisterLogin",1,Ini);
	g_LoginInfo.Has_JyLogin=GetPrivateProfileInt("UserHost","JyLogin",0,Ini);
	TDX_GetPrivateProfileString("UserHost","GuestLogin_Str","浏览用户",g_LoginInfo.GuestLogin_Str,18,Ini);
	TDX_GetPrivateProfileString("UserHost","RegisterLogin_Str","注册用户",g_LoginInfo.RegisterLogin_Str,18,Ini);
	TDX_GetPrivateProfileString("UserHost","JyLogin_Str","交易用户",g_LoginInfo.JyLogin_Str,18,Ini);
	g_LoginInfo.bDialogRegister=GetPrivateProfileInt("UserHost","DialogRegister",0,Ini);
	g_LoginInfo.bCanDelMySelf=GetPrivateProfileInt("UserHost","CanDelMySelf",0,Ini);
	g_LoginInfo.bMiscJyStyle=GetPrivateProfileInt("UserHost","MiscJyStyle",0,Ini);
	g_LoginInfo.bHasAutoRelation=GetPrivateProfileInt("UserHost","HasAutoRelation",0,Ini);
	g_LoginInfo.bHasWebRelation=GetPrivateProfileInt("UserHost","HasWebRelation",0,Ini);
	g_LoginInfo.bEnableUserName=GetPrivateProfileInt("UserHost","EnableUserName",1,Ini);
	//是否只支持网站用户
	g_LoginInfo.bOnlyWebUser=GetPrivateProfileInt("UserHost","OnlyWebUser",1,Ini);
	TDX_GetPrivateProfileString("UserHost","Web_RegisterURL","",g_LoginInfo.Web_RegisterURL,198,Ini);
	if(strlen(g_LoginInfo.Web_RegisterURL)<1)
		strcpy(g_LoginInfo.Web_RegisterURL,TDXVIP_WEBPAGE);
	TDX_GetPrivateProfileString("UserHost","Web_UserInfoURL","",g_LoginInfo.Web_UserInfoURL,198,Ini);
	if(strlen(g_LoginInfo.Web_UserInfoURL)<1)
		strcpy(g_LoginInfo.Web_UserInfoURL,TDXVIP_WEBPAGE);
	//通过网站验证的URL,如果是通过网站验证的话,则主菜单的网页在内部打开
	TDX_GetPrivateProfileString("UserHost","CheckUserURL","",tempStr2,198,Ini);	
	g_strCheckUserURL = tempStr2;
	if(strlen(g_strCheckUserURL)>0)
		g_OEM.bMainWebPageInsideOpen=1;
	//
	//系统功能的判断
	g_bHasUserHost = GetPrivateProfileInt("Other","HasUserHost",1,Ini);	
	g_GC.productnbbs[0]=0;
	g_GC.bNoUrgent=FALSE;
	g_GC.bShowHQNameInStatusBar=FALSE;
#ifdef PERSONAL
	g_bUserHostEverConnect=TRUE;
	if(g_nPersonalVersion==PERSONAL_QD)
	{
		strcpy(g_GC.productnbbs,"qd");
		g_GC.bShowHQNameInStatusBar=TRUE;
		g_GC.bNoUrgent=TRUE;
	}
	else if(g_nPersonalVersion==PERSONAL_BL)
	{
		strcpy(g_GC.productnbbs,"bl");
		g_GC.bShowHQNameInStatusBar=TRUE;
		g_GC.bNoUrgent=TRUE;
	}
	else if(g_nPersonalVersion==PERSONAL_YG)
	{
		strcpy(g_GC.productnbbs,"yg");
		g_GC.bShowHQNameInStatusBar=TRUE;
		g_GC.bNoUrgent=TRUE;
	}
	else if(g_nPersonalVersion==PERSONAL_JJTZ)
	{
		strcpy(g_GC.productnbbs,"jjtz");
		g_GC.bShowHQNameInStatusBar=TRUE;
		g_GC.bNoUrgent=TRUE;
	}
	else if(g_nPersonalVersion==PERSONAL_JJTZEX)
	{
		strcpy(g_GC.productnbbs,"jjtzex");
		g_GC.bShowHQNameInStatusBar=TRUE;
		g_GC.bNoUrgent=TRUE;
	}
	else if(g_nPersonalVersion==PERSONAL_JJDX)
	{
		strcpy(g_GC.productnbbs,"jjtzdx");
		g_GC.bShowHQNameInStatusBar=TRUE;
		g_GC.bNoUrgent=TRUE;
	}		
	else if(g_nPersonalVersion==PERSONAL_JJVIP)
	{
		strcpy(g_GC.productnbbs,"jjtzvip");
		g_GC.bShowHQNameInStatusBar=TRUE;
		g_GC.bNoUrgent=TRUE;
	}
	g_nLastLoginType=REGISTER_LOGIN;
#elif LEVEL2	
	SetAdvHQEnvironment(TRUE);
	g_GC.bShowHQNameInStatusBar=TRUE;
#elif OEM_STAR
	g_bUserHostEverConnect=TRUE;
	strcpy(g_GC.productnbbs,"zqzx");
	g_nLastLoginType=REGISTER_LOGIN;
#elif OEM_STAR_HM
	g_bUserHostEverConnect=TRUE;
	strcpy(g_GC.productnbbs,"zqzx_hm");
	g_nLastLoginType=REGISTER_LOGIN;
#elif OEM_NEWJY
	g_bSavePass=FALSE;
	g_nLastLoginType=GUEST_LOGIN;
	g_bTopTabWorkSpace=TRUE;
#endif
	//
	if(g_bUserHostEverConnect) //对于常连接方式,一般控制变量要处理一下
	{
		g_bHasUserHost=TRUE;
		g_LoginInfo.Has_GuestLogin=0;
		g_LoginInfo.Has_RegisterLogin=1;
		g_LoginInfo.Has_JyLogin=0;
		g_LoginInfo.bDialogRegister=0;
		g_LoginInfo.bCanDelMySelf=0;
		g_LoginInfo.bMiscJyStyle=0;
		g_LoginInfo.bHasAutoRelation=0;
		g_LoginInfo.bHasWebRelation=0;
		g_LoginInfo.bEnableUserName=1;
	}
	//同时看看系统有没有设置不需要通告(比如有些港股版)
	if(!g_GC.bNoUrgent)
		g_GC.bNoUrgent = GetPrivateProfileInt("Other","NoUrgent",0,Ini);	
	g_bHasWebInfo = GetPrivateProfileInt("Other","HasWebInfo",0,Ini);
	g_bHasWebInfo = FALSE;
	//交流区的设置
	int nCnt=0;
	g_ChatArea.ChatAreaNum = GetPrivateProfileInt("Integrity","ChatAreaNum",1,Ini);
	if(g_ChatArea.ChatAreaNum < 0 || g_ChatArea.ChatAreaNum > 20) g_ChatArea.ChatAreaNum = 0;
	for(i = 0;i < g_ChatArea.ChatAreaNum;i++)
	{
		tempStr.Format("ChatAreaStr%02d",i+1);	//标题
		TDX_GetPrivateProfileString("Integrity",tempStr,"",g_ChatArea.ChatAreaStr[nCnt],48,Ini);		
		tempStr.Format("AreaType%02d",i+1);		//交流区类型
		g_ChatArea.AreaType[nCnt] = GetPrivateProfileInt("Integrity",tempStr,0,Ini);		
		if(g_ChatArea.AreaType[nCnt]==AREA_CHATROOM || g_ChatArea.AreaType[nCnt]==AREA_BBS || g_ChatArea.AreaType[nCnt]==AREA_USERINFO)
			continue;
		tempStr.Format("SwitchRefresh%02d",i+1);//如果是网页,切换时是否刷新
		g_ChatArea.SwitchRefresh[nCnt] = GetPrivateProfileInt("Integrity",tempStr,0,Ini);		
		if(g_ChatArea.AreaType[nCnt]==AREA_NORMAL_WEBPAGE || g_ChatArea.AreaType[nCnt]==AREA_UID_PWD_WEBPAGE || g_ChatArea.AreaType[nCnt]==AREA_USERINFO)
		{
			tempStr.Format("ChatURL%02d",i+1);	//URL
			TDX_GetPrivateProfileString("Integrity",tempStr,"",g_ChatArea.ChatURL[nCnt],198,Ini);
			if(g_ChatArea.AreaType[nCnt]==AREA_UID_PWD_WEBPAGE)
			{
				tempStr.Format("URLDataNum%02d",i+1);				//网页参数个数
				g_ChatArea.URLDataNum[nCnt] = GetPrivateProfileInt("Integrity",tempStr,0,Ini);
				for(j=0;j < g_ChatArea.URLDataNum[nCnt];j++)
				{
					tempStr.Format("URLDataType_%02d_%02d",i+1,j+1);//网页参数域的类型
					g_ChatArea.URLData[nCnt][j].InputType = GetPrivateProfileInt("Integrity",tempStr,0,Ini);
					tempStr.Format("URLDataField_%02d_%02d",i+1,j+1);//网页参数域
					TDX_GetPrivateProfileString("Integrity",tempStr,"",g_ChatArea.URLData[nCnt][j].FieldName,60,Ini);
				}
			}
		}
		else if(g_ChatArea.AreaType[nCnt]==AREA_WT)
			g_bHasEmbWT = TRUE;		
		nCnt++;
	}
	g_ChatArea.ChatAreaNum=nCnt;
	//关键按钮的设置
	g_VitalBtn.BtnNum = GetPrivateProfileInt("VitalBtn","BtnNum",0,Ini);
	if(g_VitalBtn.BtnNum < 0 || g_VitalBtn.BtnNum > 20) g_VitalBtn.BtnNum = 0;
	for(i = 0;i < g_VitalBtn.BtnNum;i++)
	{
		tempStr.Format("BtnStr%02d",i+1);		//按钮标题
		TDX_GetPrivateProfileString("VitalBtn",tempStr,"",g_VitalBtn.BtnStr[i],48,Ini);		
		tempStr.Format("BtnType%02d",i+1);		//按钮类型
		g_VitalBtn.BtnType[i] = GetPrivateProfileInt("VitalBtn",tempStr,0,Ini);
		switch(g_VitalBtn.BtnType[i])
		{
		case 1://资讯树
			g_VitalBtn.BtnID[i]=IDC_ZXTREE;break;
		case 2://委托交易
			g_VitalBtn.BtnID[i]=ID_FUNC_WT;break;
		case 3://基本资料
			g_VitalBtn.BtnID[i]=IDC_GGZL;break;
		case 4://连接行情
			g_VitalBtn.BtnID[i]=IDC_CONNECTHQ;break;
		case 5://信息地雷
			g_VitalBtn.BtnID[i]=ID_TDXINFO_DLG;break;
		case 6://理财金算盘
			g_VitalBtn.BtnID[i]=ID_FUNC_PROFIT;break;
		case 7://基本分析
			g_VitalBtn.BtnID[i]=IDC_JBFX;break;
		case 8://财经视频
			g_VitalBtn.BtnID[i]=ID_VIDEO;break;
		case 9://主站测速
			g_VitalBtn.BtnID[i]=ID_SPEEDTEST;break;
		default:
			g_VitalBtn.BtnID[i]=0;break;
		}
		tempStr.Format("BtnExecFile%02d",i+1);	//执行文件
		TDX_GetPrivateProfileString("VitalBtn",tempStr,"",g_VitalBtn.BtnExecFile[i],178,Ini);
	}
	g_bSaveUser = GetPrivateProfileInt("User","SaveUser",1,Ini);
	g_bSavePass = GetPrivateProfileInt("User","SavePass",1,Ini);
	
	g_bNLBOnLogo = GetPrivateProfileInt("Other","NLBOnLogo",0,Ini);
	g_bNLBOnLogo = TRUE;
	//是否提示切换至下一个
	g_bHintSwitch = GetPrivateProfileInt("Other","HintSwitch",1,Ini);
	//是否随即跳转
	g_bRandSwitch = GetPrivateProfileInt("Other","RandSwitch",0,Ini);
	//网络故障后(短时间内连接很多次)是否提示重新登录
	g_bNetErrorReLogin = GetPrivateProfileInt("Other","NetErrorReLogin",1,Ini);
	//行情用户的交易认证方式,如果没有用户管理服务器,可以采用本地的验证方式
	g_bWtUserVerifyStyle = GetPrivateProfileInt("UserHost","WtUserVerifyStyle",1,Ini);
	if(g_bWtUserVerifyStyle < 1 || g_bWtUserVerifyStyle > 6)
		g_bWtUserVerifyStyle = 1;
	g_bHexinWtOnlyTXMM = GetPrivateProfileInt("UserHost","HexinWtOnlyTXMM",0,Ini);
#ifdef OEM_NEWJY	//合一版固定用内部校验方式
	g_bWtUserVerifyStyle = 1;
#endif
	if(g_bWtUserVerifyStyle==4) //如果是联合证券验证方式,需要生成标识串
		WritePrivateProfileString("user","version","tdx_lhzq",HomePath+"buser.ini");
	GetPrivateProfileString("UserHost","VerifyFileName","winwt.exe",tempStr2,99,Ini);
	g_strTdxVerifyFileName=tempStr2;
	//禁止使用增删主站的功能
	g_bForbidOtherHost=GetPrivateProfileInt("Other","ForbidOtherHost",0,Ini);	
	//成交量分色模式,对于湘财等券商为1,其它证券为0
	g_bDiffVolMode = GetPrivateProfileInt("Other","DiffVolMode",0,Ini);
	g_nReConnectNum = GetPrivateProfileInt("Other","ReConnectNum",0,Ini);
	g_nLogoTimeOut = GetPrivateProfileInt("Other","LogoTimeOut",8,Ini);
	if(g_nLogoTimeOut<=0) g_nLogoTimeOut=8;
	//是否是本地数据模式
	g_GC.bLocalData = GetPrivateProfileInt("Other","UseLocalData",0,Ini);
	if(g_GC.bLocalData)
	{
		GetPrivateProfileString("Other","docpath" ,"LData\\", g_TdxCfg.DocPath,256,Ini);
		MendPath(g_TdxCfg.DocPath);
	}
	//
	//关于本系统的链接
	GetPrivateProfileString("Other","AboutURL","",g_Url.AboutURL,250,Ini);
	//是否支持自动升级
	g_GC.bNoAutoup=GetPrivateProfileInt("Other","NoAutoup",0,Ini);
	//自动升级采用集成版的标识
	g_GC.WhichAutoUpInfo=GetPrivateProfileInt("Other","WhichAutoUpInfo",2,Ini);
	//是否是内部调试版
	g_bDebugMode=GetPrivateProfileInt("Other","DebugMode",0,Ini);	
	//版本标题	
	GetPrivateProfileString("Other","Version_Title","",g_GC.version_title,38,Ini);
	if(strlen(g_GC.version_title)==0)
		strcpy(g_GC.version_title,g_strPreName);
	//其它文件的名称
	GetPrivateProfileString("Other","OtherFileName","",g_GC.OtherFileName,38,Ini);
	GetPrivateProfileString("EXTERN","AlongPathFile","",g_GC.AlongPathFile,78,Ini);
	g_GC.bAlongJy=GetPrivateProfileInt("EXTERN","AlongJy",0,Ini);
	g_GC.bAlongOpenWT=GetPrivateProfileInt("EXTERN","AlongOpenWT",0,Ini);
	g_GC.bAlongOpenHXInfo=GetPrivateProfileInt("EXTERN","AlongOpenHXInfo",0,Ini);
	//从注册表中读取文件
	GetPrivateProfileString("EXTERN","HKey_Path","",g_GC.HKey_Path,178,Ini);
	GetPrivateProfileString("EXTERN","HKey_KeyString","",g_GC.HKey_KeyString,178,Ini);	
	GetPrivateProfileString("EXTERN","HKey_RunParam","",g_GC.HKey_RunParam,178,Ini);	
	GetPrivateProfileString("EXTERN","HKey_MenuName","F11资讯程序",g_GC.HKey_MenuName,38,Ini);		
	GetPrivateProfileString("EXTERN","HKey_ErrorInfo","",g_GC.HKey_ErrorInfo,178,Ini);		
	//较短的刷新时间
	g_GC.bShortRefreshHQ=GetPrivateProfileInt("Other","ShortRefreshHQ",0,Ini);
	//视频
	g_GC.Video.nVideoFlag=GetPrivateProfileInt("Video","VideoFlag",0,Ini);
	g_GC.Video.nWndcx=GetPrivateProfileInt("Video","WndCx",400,Ini);
	g_GC.Video.nWndcy=GetPrivateProfileInt("Video","WndCy",258,Ini);
	GetPrivateProfileString("Video","VideoURL","",g_GC.Video.strVideoURL,MAX_PATH-1,Ini);
	//只能支持TQ,TQ是否自动登录
	g_bHasTICQ = GetPrivateProfileInt("Other","HasTICQ",0,Ini);
	g_bAutoConnectTICQ = GetPrivateProfileInt("Other","AutoConnectTICQ",0,Ini);
	g_bTQAsBroker = GetPrivateProfileInt("Other","TQAsBroker",0,Ini);
	//如果是共享模式(无盘),本地数据方式,小流量版等,不允许自动升级,自动下载,写盘等
	g_bCanDown = TRUE;
	if(g_GC.nShareMode || g_GC.bLocalData) 
	{
		g_bCanDown = FALSE;			
		g_bDay_WriteDisk = FALSE;
		g_GC.bNoAutoup = 1;
	}
	//-------------------------------------------------------------------------------------------------------
	//只有用户才能改动的INI
	//代理信息
	g_ProxyInfo.m_bProxy = GetPrivateProfileInt("PROXY","HasProxy",0,g_WSXHStr+g_strUserCfg);
	g_ProxyInfo.m_nSockType = GetPrivateProfileInt("PROXY","SockType",2,g_WSXHStr+g_strUserCfg);
	if(g_ProxyInfo.m_nSockType != 0 && g_ProxyInfo.m_nSockType != 1 && g_ProxyInfo.m_nSockType != 2)
		g_ProxyInfo.m_nSockType = 0;
	g_ProxyInfo.m_nProxyPort = GetPrivateProfileInt("PROXY","ProxyPort",80,g_WSXHStr+g_strUserCfg);
	GetPrivateProfileString("PROXY","ProxyIP","",g_ProxyInfo.m_strProxyIP,99,g_WSXHStr+g_strUserCfg);
	GetPrivateProfileString("PROXY","ProxyUser","",g_ProxyInfo.m_strProxyUser,99,g_WSXHStr+g_strUserCfg);
	GetPrivateProfileString("PROXY","ProxyPass","",g_ProxyInfo.m_strProxyPass,99,g_WSXHStr+g_strUserCfg);
	//我所处的位置
	g_nMyProvineID = GetPrivateProfileInt("AREA","MyProvineID",0,g_WSXHStr+g_strYxhjCfg);	
	//是否是查询模式,本模块有效
	l_bCXMode=GetPrivateProfileInt("Version","CXMode",0,g_WSXHStr+g_strYxhjCfg);

	//只有港股行情，屏蔽AB股行情
	g_GC.bOnlyHK=GetPrivateProfileInt("Version","OnlyHK",0,g_WSXHStr+g_strYxhjCfg);
	//取网络参数
	g_NetParam.bNoPassPort=GetPrivateProfileInt("Net","NoPassPort",0,g_WSXHStr+g_strYxhjCfg);
	//
	l_bFirstGetFromConnectCfg = FALSE;
}

void WriteToConnectCfg()
{
	CString	Ini = g_WSXHStr+g_strYxhjCfg;
	CString tempStr;
	//写当前主站信息
	WritePrivateProfileString("HQHOST","PrimaryHost",IntToStr(g_HQHost.PrimaryHost),Ini);
//	WritePrivateProfileString("OTHERHOST","L2PrimaryHost",IntToStr(g_L2HQHost.PrimaryHost),Ini);
	//写用户信息
//	WritePrivateProfileString("USER","UserName",g_strLoginName,Ini);
	WritePrivateProfileString("User","SaveUser",g_bSaveUser?"1":"0",Ini);
	WritePrivateProfileString("User","SavePass",g_bSavePass?"1":"0",Ini);
	//将风格状态写盘
//	WritePrivateProfileString("Other","SimpleMode",g_bHasCaption?"1":"0",g_WSXHStr+g_strUserCfg);
	//是否打开查找最快
//	WritePrivateProfileString("Other","NLB",g_bNLB?"1":"0",g_WSXHStr+g_strUserCfg);
}
		
void CLogoDlg::EnableOther(BOOL bEn)
{
	if(!bEn) SetWindowPos(NULL,-1000,-1000,0,0,SWP_NOSIZE);
	else CenterWindow();
	if(g_pSplashWnd)
	{
		g_pSplashWnd ->CenterWindow();			//窗口显示在屏幕中心
		g_pSplashWnd ->ShowWindow(TRUE);		//显示窗口
		g_pSplashWnd ->UpdateWindow();
	}
#ifdef OEM_NEWJY
	m_pOKBt->EnableWindow(bEn);
	if(m_bHasOffMode)
	{
		m_pSel1Bt->EnableWindow(bEn);
		m_pSel2Bt->EnableWindow(bEn);
		m_pSel3Bt->EnableWindow(bEn);
		m_pOffline->EnableWindow(bEn);
		m_embzh_Combo.EnableWindow(bEn);
		m_embzh_Combo.RedrawWindow();
		m_edit1.EnableWindow(bEn);
		m_edit1.RedrawWindow();
		m_edit2.EnableWindow(bEn);
		m_edit2.RedrawWindow();
		m_pCABt->EnableWindow(bEn);
		m_pSoftKeyBt->EnableWindow(bEn);
		m_SaveZHButton.EnableWindow(bEn);
		m_ProtectZHButton.EnableWindow(bEn);
		m_LoginType_Combo.EnableWindow(bEn);
		m_LoginType_Combo_yh.EnableWindow(bEn);
		m_Aqfs_Combo.EnableWindow(bEn);
		if(bEn)
			EnableEmbedWTSel();
		else
			_EnableCommbox(FALSE);
	}

	m_pCfgBt->EnableWindow(bEn);
	m_pHelpBt->EnableWindow(bEn);
	m_pSysTestBt->EnableWindow(bEn);
#else
	GetDlgItem(IDOK)->EnableWindow(bEn);
	if(m_bHasOffMode)
		GetDlgItem(IDC_OFFLINE)->EnableWindow(bEn);
	m_SpeedTestLink.EnableWindow(bEn);
	GetDlgItem(IDC_NLB_CHECK)->EnableWindow(bEn);
#endif

	GetDlgItem(IDC_USERNAME)->EnableWindow(bEn);
	GetDlgItem(IDC_PASSWORD)->EnableWindow(bEn);
	m_RegLogin.EnableWindow(bEn);
}

BOOL CLogoDlg::NeedTranInfo(int which)	//判断需不需要传输INF数据
{
	char tmpIP[40];
	unsigned short tmpPort;
	long tmpInfDate,tmpInfHms;
	CString tempFileName;
	tempFileName.Format("%sLData\\cache\\%sex.tnf",g_WSXHStr,GetSetStr(which));
	FILE *fpt = fopen ( tempFileName, "rb");
	if(!fpt) 
		return TRUE;
	fseek(fpt, 0L, SEEK_SET);
	//先将主站的IP,Port,INF修改时间一齐存盘
	fread(tmpIP,1,40,fpt);
	fread(&tmpPort,1,sizeof(unsigned short),fpt);
	fread(&tmpInfDate,1,sizeof(long),fpt);
	fread(&tmpInfHms,1,sizeof(long),fpt);
	fclose(fpt);
 	if(strcmp(tmpIP,g_HQIPStr) || tmpPort!=g_HQPort)
		return TRUE;
	if(which == 0)
	{
		if(tmpInfDate!=szinfdate || (tmpInfHms/100)!=(szinfhms/100))
			return TRUE;
	}
	else
	{
		if(tmpInfDate!=shinfdate || (tmpInfHms/100)!=(shinfhms/100))
			return TRUE;
	}
	return FALSE;
}

BOOL CLogoDlg::SaveIt(int which)
{
	CString tempFileName;
	tempFileName.Format("%sLData\\cache\\%sex.tnf",g_WSXHStr,GetSetStr(which));
	FILE *fpt = fopen(tempFileName,"wb");
	if(!fpt)
	{
		TDX_MessageBox(m_hWnd,"不能将TNF数据存盘",MB_OK|MB_ICONERROR);
		return FALSE;
	}
	fwrite(g_HQIPStr,1,40,fpt);
	fwrite(&g_HQPort,1,sizeof(unsigned short),fpt);
	if(which == 0)
	{
		fwrite(&szinfdate,1,sizeof(long),fpt);
		fwrite(&szinfhms,1,sizeof(long),fpt);
		fwrite(m_pSZMem, 1,(long)szstknum * sizeof ( struct MemStkInfo ),fpt);
	}
	else
	{
		fwrite(&shinfdate,1,sizeof(long),fpt);
		fwrite(&shinfhms,1,sizeof(long),fpt);
		fwrite(m_pSHMem, 1,(long)shstknum * sizeof ( struct MemStkInfo ),fpt);
	}
	fclose(fpt);

	//删除缓存的走势图数据
	DeleteDirInfo(g_WSXHStr+"zst_cache\\");
	memset(g_HisTickCacheHdr, 0, MAX_HISCACHE*sizeof(HisTickCacheHead));
	g_nNextHTCHPos = 0;
	//删除TCU文件
	tempFileName.Format("%sLData\\cache\\%s.tcu",g_WSXHStr,GetSetStr(which));
	unlink(tempFileName);
	//删除TH2文件
	tempFileName.Format("%sLData\\cache\\%s.th2",g_WSXHStr,GetSetStr(which));
	unlink(tempFileName);

	if(which==0) //只删除一次
	{
		//删除主力监控的缓存文件
		tempFileName.Format("%stmp\\clecache.tmp",g_WSXHStr);
		unlink(tempFileName);
		//删除缓存的分钟数据
		unlink(g_WSXHStr+"LData\\cache\\sz.TFZ");
		unlink(g_WSXHStr+"LData\\cache\\sh.TFZ");
	}

	short stocknum = (which==0)?szstknum:shstknum;
	struct  MinCacheHead *tmpHead;
	tmpHead = new struct MinCacheHead[stocknum];
	memset(tmpHead,0,stocknum*sizeof(struct MinCacheHead));
	//创建新的分时缓存文件	
	tempFileName.Format("%sLData\\cache\\%s.tfz",g_WSXHStr,GetSetStr(which));
	fpt = fopen(tempFileName,"wb");
	if(!fpt)
	{
		delete tmpHead;
		return FALSE;
	}
	fwrite(tmpHead,1,stocknum*sizeof(struct MinCacheHead),fpt);
	fclose(fpt);
	//创建新的Tick缓存文件
	tempFileName.Format("%sLData\\cache\\%s.tik",g_WSXHStr,GetSetStr(which));
	fpt = fopen(tempFileName,"wb");
	if(!fpt)
	{
		delete tmpHead;
		return FALSE;
	}
	fwrite(tmpHead,1,stocknum*sizeof(struct MinCacheHead),fpt);
	fclose(fpt);

	delete tmpHead;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CLogoDlg message handlers

void CLogoDlg::EnableEmbedWTSel()
{
#ifdef OEM_NEWJY
	if(g_OEM.bHasTryout)
	{
		if(g_nEmbedTabSel==LOGIN_ONLYHQ && !LocalJyVerify())
			m_pOKBt->SetText(_F("试 用"));
		else
			m_pOKBt->SetText(_F("登 录"));		
		m_pOKBt->Invalidate();
	}
	if(m_bOnlyLoginWT)
	{
		m_embzh_Combo.EnableWindow(TRUE);
		m_embzh_Combo.RedrawWindow();
		m_edit1.EnableWindow(TRUE);
		m_edit1.RedrawWindow();
		m_edit2.EnableWindow(TRUE);
		m_edit2.RedrawWindow();
		m_pSoftKeyBt->EnableWindow(TRUE);
		if(::IsWindow(m_SaveZHButton.m_hWnd))
			m_SaveZHButton.EnableWindow(TRUE);
		if(::IsWindow(m_ProtectZHButton.m_hWnd))
			m_ProtectZHButton.EnableWindow(TRUE);
		m_pSel1Bt->ShowWindow(SW_HIDE);
		m_pSel2Bt->ShowWindow(SW_HIDE);
		m_pSel3Bt->ShowWindow(SW_HIDE);
		_EnableCommbox(TRUE);
		return;
	}
	if(g_nEmbedTabSel==LOGIN_ONLYHQ)
	{
		m_embzh_Combo.EnableWindow(FALSE);
		m_embzh_Combo.RedrawWindow();
		m_edit1.EnableWindow(FALSE);
		m_edit1.RedrawWindow();
		m_edit2.EnableWindow(FALSE);
		m_edit2.RedrawWindow();
		m_pSoftKeyBt->EnableWindow(FALSE);
		if(::IsWindow(m_SaveZHButton.m_hWnd))
			m_SaveZHButton.EnableWindow(FALSE);
		if(::IsWindow(m_ProtectZHButton.m_hWnd))
			m_ProtectZHButton.EnableWindow(FALSE);
		m_pSel1Bt->SetSel(FALSE);
		m_pSel2Bt->SetSel(TRUE);
		m_pSel3Bt->SetSel(FALSE);
		_EnableCommbox(FALSE);
	}
	else
	{
		m_embzh_Combo.EnableWindow(TRUE);
		m_embzh_Combo.RedrawWindow();
		m_edit1.EnableWindow(TRUE);
		m_edit1.RedrawWindow();
		m_edit2.EnableWindow(TRUE);
		m_edit2.RedrawWindow();
		m_pSoftKeyBt->EnableWindow(TRUE);
		if(::IsWindow(m_SaveZHButton.m_hWnd))
			m_SaveZHButton.EnableWindow(TRUE);
		if(::IsWindow(m_ProtectZHButton.m_hWnd))
			m_ProtectZHButton.EnableWindow(TRUE);
		if(g_nEmbedTabSel==LOGIN_HQWT)
		{
			m_pSel1Bt->SetSel(TRUE);
			m_pSel2Bt->SetSel(FALSE);
			m_pSel3Bt->SetSel(FALSE);
		}
		else
		{
			m_pSel1Bt->SetSel(FALSE);
			m_pSel2Bt->SetSel(FALSE);
			m_pSel3Bt->SetSel(TRUE);
		}
		_EnableCommbox(TRUE);
	}
#endif
}

void WINAPI BtOwnerDraw(CWnd *pWnd,CDC *pDC,CRect rcWnd,ButtonState btState,BOOL bFocus)
{
	if(!pWnd || !pDC) return;
	if(bFocus)
	{
		rcWnd.DeflateRect(1,1,1,1);
		g_GUI.DrawLine(pDC,rcWnd.left+1,rcWnd.top,rcWnd.right-1,rcWnd.top,RGB(229,231,104));
		g_GUI.DrawLine(pDC,rcWnd.right-1,rcWnd.top+1,rcWnd.right-1,rcWnd.bottom-1,RGB(229,231,104));
		g_GUI.DrawLine(pDC,rcWnd.left+1,rcWnd.bottom-1,rcWnd.right-1,rcWnd.bottom-1,RGB(229,231,104));
		g_GUI.DrawLine(pDC,rcWnd.left,rcWnd.top+1,rcWnd.left,rcWnd.bottom-1,RGB(229,231,104));
		rcWnd.DeflateRect(1,1,1,1);
		pDC->FrameRect(rcWnd,&CBrush(RGB(243,178,45)));
	}
}

int CLogoDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	hBitmap_i = ::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(g_Scheme_King.LoginDB_ID),IMAGE_BITMAP,0,0,LR_DEFAULTSIZE|LR_CREATEDIBSECTION);
	if(hBitmap_i == NULL)
		return FALSE;
	m_bitmap_i.Attach((HBITMAP)hBitmap_i);
	BITMAP bm_i;
	m_bitmap_i.GetBitmap(&bm_i);

	CRect lrc(0,0,bm_i.bmWidth+2,bm_i.bmHeight+2);
	MoveWindow(&lrc, FALSE);
	CenterWindow();
	
#ifdef OEM_NEWJY
	HINSTANCE tmphi=AfxGetResourceHandle();
	AfxSetResourceHandle(g_resModule);
	CSize sz=g_GUI.GetBitmapSize("IDB_LOGODLG_WND");
	SetWindowPos(NULL,0,0,sz.cx,sz.cy,SWP_NOMOVE);
	SetWindowRgn((HRGN)m_wndRgn,TRUE);
	CRect rc(0,0,0,0);

	LPCTSTR temp4[9]={0,0,"IDB_LOGOBT_KEY_N",
		0,0,"IDB_LOGOBT_KEY_D",
		0,0,"IDB_LOGOBT_KEY_N"};
	m_pCABt=new MyBitmapBt(ID_CABT,"",NormalBt,0,TRUE);
	m_pCABt->SetBK(temp4);
	m_pCABt->SetCustomFunc(BtOwnerDraw);
	m_pCABt->Create(NULL,"",WS_CHILD|WS_TABSTOP,rc,this,ID_CABT);

	LPCTSTR temp5[9]={0,0,"IDB_LOGOBT_KEYBOARD_N",
		0,0,"IDB_LOGOBT_KEYBOARD_D",
		0,0,"IDB_LOGOBT_KEYBOARD_N"};
	m_pSoftKeyBt=new MyBitmapBt(ID_SOFTKEYBT,"",NormalBt,0,TRUE);
	m_pSoftKeyBt->SetBK(temp5);
	m_pSoftKeyBt->SetCustomFunc(BtOwnerDraw);
	m_pSoftKeyBt->Create(NULL,"",WS_VISIBLE|WS_CHILD|WS_TABSTOP,rc,this,ID_SOFTKEYBT);

	m_embzh_Combo.Create(WS_VISIBLE|WS_CHILD|CBS_DROPDOWN|WS_VSCROLL|WS_TABSTOP,rc,this,ID_LOGO_EDIT0);
	m_edit1.Create(WS_CHILD|WS_VISIBLE,rc,this,ID_LOGO_EDIT1);
	m_edit2.Create(WS_CHILD|WS_VISIBLE,rc,this,ID_LOGO_EDIT2);	

	m_embzh_Combo.GetSafeEdit().SetLimitText(30);
	m_edit1.SetLimitText(g_OEM.nLogoMaxPassordLen);
	m_edit2.SetLimitText(g_OEM.nLogoMaxAuthPassLen);
	m_embzh_Combo.ModifyExtendedStyle(ES_EX_GUIFONT|ES_EX_HASBORDER,NULL,FALSE);
	m_edit1.ModifyExtendedStyle(ES_EX_GUIFONT|ES_EX_HASBORDER,NULL,FALSE);
	m_edit2.ModifyExtendedStyle(ES_EX_GUIFONT|ES_EX_HASBORDER,NULL,FALSE);

	LPCTSTR temp2[9]={"IDB_LOGOBT_N_L","IDB_LOGOBT_N_R","IDB_LOGOBT_N_BK",
					"IDB_LOGOBT_D_L","IDB_LOGOBT_D_R","IDB_LOGOBT_D_BK",
					"IDB_LOGOBT_O_L","IDB_LOGOBT_O_R","IDB_LOGOBT_O_BK"};

	LPCTSTR temp2_2[9]={"IDB_LOGOBT2_N_L","IDB_LOGOBT2_N_R","IDB_LOGOBT2_N_BK",
					"IDB_LOGOBT2_D_L","IDB_LOGOBT2_D_R","IDB_LOGOBT2_D_BK",
					"IDB_LOGOBT2_O_L","IDB_LOGOBT2_O_R","IDB_LOGOBT2_O_BK"};
	
	int x_offset=0,y_offset=-1;
	m_pOKBt=new MyBitmapBt(IDOK,_F("登 录"),NormalBt,0,TRUE);
	m_pOKBt->SetBK(temp2);
	if(g_OEM.bWhiteBtnForeColor)
		m_pOKBt->SetTextCor(RGB(220,255,255),RGB(220,255,255),RGB(220,255,255));
	m_pOKBt->SetTextOffset(x_offset,y_offset,1+x_offset,1+y_offset,x_offset,y_offset);
	m_pOKBt->SetCustomFunc(BtOwnerDraw);
	m_pOKBt->Create(NULL,"",WS_VISIBLE|WS_CHILD|WS_TABSTOP,rc,this,IDOK);
	
	m_pOffline=new MyBitmapBt(IDC_OFFLINE,_F("脱 机"),NormalBt,0,TRUE);
	m_pOffline->SetBK(temp2);
	if(g_OEM.bWhiteBtnForeColor)
		m_pOffline->SetTextCor(RGB(220,255,255),RGB(220,255,255),RGB(220,255,255));
	m_pOffline->SetTextOffset(x_offset,y_offset,1+x_offset,1+y_offset,x_offset,y_offset);
	m_pOffline->SetCustomFunc(BtOwnerDraw);
	m_pOffline->Create(NULL,"",WS_VISIBLE|WS_CHILD|WS_TABSTOP,rc,this,IDC_OFFLINE);	

	m_pCancelBt=new MyBitmapBt(IDCANCEL,_F("取 消"),NormalBt,0,TRUE);
	m_pCancelBt->SetBK(temp2);
	if(g_OEM.bWhiteBtnForeColor)
		m_pCancelBt->SetTextCor(RGB(220,255,255),RGB(220,255,255),RGB(220,255,255));
	m_pCancelBt->SetTextOffset(x_offset,y_offset,1+x_offset,1+y_offset,x_offset,y_offset);
	m_pCancelBt->SetCustomFunc(BtOwnerDraw);
	m_pCancelBt->Create(NULL,"",WS_VISIBLE|WS_CHILD|WS_TABSTOP,rc,this,IDCANCEL);
	
	m_pCfgBt=new MyBitmapBt(IDC_CONNECTCFG_BUTTON2,_F("通讯设置"),NormalBt,0,TRUE);
	m_pCfgBt->SetCustomFunc(BtOwnerDraw);
	if(g_OEM.bUseBtn2Back)
		m_pCfgBt->SetBK(temp2_2);
	else
	{
		m_pCfgBt->SetBK(temp2);
		if(g_OEM.bWhiteBtnForeColor)
			m_pCfgBt->SetTextCor(RGB(220,255,255),RGB(220,255,255),RGB(220,255,255));
	}
	m_pCfgBt->SetTextOffset(x_offset,y_offset,1+x_offset,1+y_offset,x_offset,y_offset);
	m_pCfgBt->Create(NULL,"",WS_VISIBLE|WS_CHILD|WS_TABSTOP,rc,this,IDC_CONNECTCFG_BUTTON2);
	
	m_pHelpBt=new MyBitmapBt(ID_HELPBT,_F("使用指南"),NormalBt,0,TRUE);
	if(g_OEM.bUseBtn2Back)
		m_pHelpBt->SetBK(temp2_2);
	else
	{
		m_pHelpBt->SetBK(temp2);
		if(g_OEM.bWhiteBtnForeColor)
			m_pHelpBt->SetTextCor(RGB(220,255,255),RGB(220,255,255),RGB(220,255,255));
	}
	m_pHelpBt->SetTextOffset(x_offset,y_offset,1+x_offset,1+y_offset,x_offset,y_offset);
	m_pHelpBt->SetCustomFunc(BtOwnerDraw);
	m_pHelpBt->Create(NULL,"",WS_VISIBLE|WS_CHILD|WS_TABSTOP,rc,this,ID_HELPBT);
	if(g_OEM.bNoHelpBtnInLogo)
		m_pHelpBt->ShowWindow(SW_HIDE);
	
	m_pSysTestBt=new MyBitmapBt(ID_SYSTESTBT,_F(g_OEM.SysTestCaption),NormalBt,0,TRUE);
	if(g_OEM.bUseBtn2Back)
		m_pSysTestBt->SetBK(temp2_2);
	else
	{
		m_pSysTestBt->SetBK(temp2);
		if(g_OEM.bWhiteBtnForeColor)
			m_pSysTestBt->SetTextCor(RGB(220,255,255),RGB(220,255,255),RGB(220,255,255));
	}
	m_pSysTestBt->SetTextOffset(x_offset,y_offset,1+x_offset,1+y_offset,x_offset,y_offset);
	m_pSysTestBt->SetCustomFunc(BtOwnerDraw);
	m_pSysTestBt->Create(NULL,"",WS_VISIBLE|WS_CHILD|WS_TABSTOP,rc,this,ID_SYSTESTBT);
		
	LPCTSTR temp3[9]={"IDB_LOGOSELBT_N_L","IDB_LOGOSELBT_N_R","IDB_LOGOSELBT_N_BK",
		"IDB_LOGOSELBT_D_L","IDB_LOGOSELBT_D_R","IDB_LOGOSELBT_D_BK",
		"IDB_LOGOSELBT_O_L","IDB_LOGOSELBT_O_R","IDB_LOGOSELBT_O_BK"};

	COLORREF cor_n,cor_d,cor_o;
	cor_n=g_GUI.GetColorCfgFromResource(g_resModule,"IDR_COLOR","LOGODLG_SELBT_N",-1);
	cor_d=g_GUI.GetColorCfgFromResource(g_resModule,"IDR_COLOR","LOGODLG_SELBT_D",-1);
	cor_o=g_GUI.GetColorCfgFromResource(g_resModule,"IDR_COLOR","LOGODLG_SELBT_O",-1);

	DWORD dwStyle=WS_VISIBLE|WS_CHILD|WS_TABSTOP;
	if(g_OEM.bNoSwitchInLogo)
		dwStyle=WS_CHILD|WS_TABSTOP;
	
	m_pSel1Bt=new MyBitmapBt(ID_SELBT1,_F(g_OEM.Sel1Caption),NormalBt,0,TRUE);
	m_pSel1Bt->SetBK(temp3);
	m_pSel1Bt->SetTextCor(cor_n,cor_d,cor_o);
	m_pSel1Bt->SetCustomFunc(BtOwnerDraw);
	m_pSel1Bt->Create(NULL,"",dwStyle,rc,this,ID_SELBT1);
	
	m_pSel2Bt=new MyBitmapBt(ID_SELBT2,_F(g_OEM.Sel2Caption),NormalBt,0,TRUE);
	m_pSel2Bt->SetBK(temp3);
	m_pSel2Bt->SetTextCor(cor_n,cor_d,cor_o);
	m_pSel2Bt->SetCustomFunc(BtOwnerDraw);
	m_pSel2Bt->Create(NULL,"",dwStyle,rc,this,ID_SELBT2);
	
	m_pSel3Bt=new MyBitmapBt(ID_SELBT3,_F(g_OEM.Sel3Caption),NormalBt,0,TRUE);
	m_pSel3Bt->SetBK(temp3);
	m_pSel3Bt->SetTextCor(cor_n,cor_d,cor_o);
	m_pSel3Bt->SetCustomFunc(BtOwnerDraw);
	m_pSel3Bt->Create(NULL,"",dwStyle,rc,this,ID_SELBT3);

	LPCTSTR temp1[9]={0,0,"IDB_BT_CLOSE_N",
		0,0,"IDB_BT_CLOSE_D",
		0,0,"IDB_BT_CLOSE_O"};
	m_pCloseBt=new MyBitmapBt(ID_CLOSEBT,"",NormalBt,0,TRUE);
	m_pCloseBt->SetBK(temp1);
	m_pCloseBt->SetCustomFunc(BtOwnerDraw);
	m_pCloseBt->Create(NULL,"",WS_VISIBLE|WS_CHILD|WS_TABSTOP,rc,this,ID_CLOSEBT);

	AfxSetResourceHandle(tmphi);
	EnableEmbedWTSel();

#endif
	return 0;
}

BOOL CLogoDlg::OnInitDialog() 
{	
	CDialog::OnInitDialog();

	//固定位置 
	GetDlgItem(IDC_USERNAME)->SetWindowPos(NULL,g_Scheme_King.nLup[0],g_Scheme_King.nLup[1],g_Scheme_King.nLup[2],g_Scheme_King.nLup[3],SWP_SHOWWINDOW);
	GetDlgItem(IDC_PASSWORD)->SetWindowPos(NULL,g_Scheme_King.nLup[4],g_Scheme_King.nLup[5],g_Scheme_King.nLup[6],g_Scheme_King.nLup[7],SWP_SHOWWINDOW);
	GetDlgItem(IDC_SAVEPASS_CHECK)->SetWindowPos(NULL,g_Scheme_King.nLup[8],g_Scheme_King.nLup[9],g_Scheme_King.nLup[10],g_Scheme_King.nLup[11],SWP_SHOWWINDOW);
	GetDlgItem(IDC_CHECK_SAVEZH)->SetWindowPos(NULL,g_Scheme_King.nLup[12],g_Scheme_King.nLup[13],g_Scheme_King.nLup[14],g_Scheme_King.nLup[15],SWP_SHOWWINDOW);
	GetDlgItem(IDC_LOGO_PROGRESS)->SetWindowPos(NULL,g_Scheme_King.nLup[16],g_Scheme_King.nLup[17],g_Scheme_King.nLup[18],g_Scheme_King.nLup[19],SWP_SHOWWINDOW);
	m_RegLogin.SetWindowPos(NULL, g_Scheme_King.nLup[20],g_Scheme_King.nLup[21],g_Scheme_King.nLup[22],g_Scheme_King.nLup[23],SWP_SHOWWINDOW);
	m_RegExit.SetWindowPos(NULL, g_Scheme_King.nLup[24],g_Scheme_King.nLup[25],g_Scheme_King.nLup[26],g_Scheme_King.nLup[27],SWP_SHOWWINDOW);
	m_ProxyInfo.SetWindowPos(NULL, g_Scheme_King.nLup[28],g_Scheme_King.nLup[29],g_Scheme_King.nLup[30],g_Scheme_King.nLup[31],SWP_SHOWWINDOW);

	GetDlgItem(IDOK)->SetWindowPos(NULL,0,0,0,0,SWP_SHOWWINDOW);
	GetDlgItem(IDC_STATUS_STATIC)->SetWindowPos(NULL,0,0,0,0,SWP_SHOWWINDOW);
	GetDlgItem(IDC_REG_STATIC)->SetWindowPos(NULL,0,0,0,0,SWP_SHOWWINDOW);

	if(m_bHasOffMode)
		SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
//	SCT_EnableProtect(m_hWnd);

	m_imagelist.Create(16,16,ILC_COLOR8|ILC_MASK,1,0);
	m_imagelist.Add(m_hIcon);
	m_hYhzqIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_YHZQ));
	m_hZxjtIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ZXJT));

	m_Status_Static.SetTextColor(RGB(255,28,28));	

	GetFromConnectCfg();
//	GetFromUserHostCfg();
	InitOtherLogoInfo();
	m_bNLB=g_bNLB;

	hOEMBitmap = LoadImage(NULL,HomePath+(CString)"oem.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);

	SkinWindowSet(m_RegExit.GetSafeHwnd(), 0);
	m_RegExit.SetURL("真的想退出吗?");
	m_RegExit.SetMessageMode(m_hWnd,UM_REGEXIT_MESSAGE);
	m_RegExit.SetUnderline(FALSE);
	m_RegExit.SetLinkCursor(AfxGetApp()->LoadCursor(IDC_MOVE2));

	SkinWindowSet(m_RegLogin.GetSafeHwnd(), 0);
	m_RegLogin.SetURL("登录系统");
	m_RegLogin.SetMessageMode(m_hWnd,UM_REGLOGIN_MESSAGE);
	m_RegLogin.SetUnderline(FALSE);
	m_RegLogin.SetLinkCursor(AfxGetApp()->LoadCursor(IDC_MOVE2));

	SkinWindowSet(m_ProxyInfo.GetSafeHwnd(), 0);
	m_ProxyInfo.SetURL("设置代理");
	m_ProxyInfo.SetMessageMode(m_hWnd,UM_PROXY_MESSAGE);
	m_ProxyInfo.SetUnderline(FALSE);
	m_ProxyInfo.SetLinkCursor(AfxGetApp()->LoadCursor(IDC_MOVE2));

	m_bEnableNLB=TRUE;
	if(m_bHasOffMode && g_bEveryFirst) //复位到第一个主站上
		g_HQHost.PrimaryHost = 0;
	m_nMode = g_bHasCaption?0:1;
	m_HqHost_Combo.EnableWindow(!g_bNLB);
	//有没有脱机运行
	if(!m_bHasOffMode)
		GetDlgItem(IDC_OFFLINE)->EnableWindow(FALSE);
	//用户信息
	CString userName, userPass;
	GetPasswordFromRegister(g_Scheme_King.VersionString,userName,userPass,g_nLastValidDate,g_nLastValidValue);
	if(g_bSaveUser)
		strcpy(g_strLoginName, (char*)(LPCSTR)userName);
	if(g_bSaveUser&&g_bSavePass)
		strcpy(g_strPassword, (char*)(LPCSTR)userPass);
	m_strPassword=g_strPassword;
	m_strUserName	= g_strLoginName;
	m_bSavePass		= g_bSavePass;
	m_bSaveZH		= g_bSaveUser;
//	if(m_bSavePass && g_nLastLoginType!=JY_LOGIN)
//		GetPasswordFromRegister(g_Scheme_King.VersionString,m_strUserName,m_strPassword,g_nLastValidDate,g_nLastValidValue);

	if(g_bShowVersoin)			//如果显示版本号
		SetWindowText(_F(g_strTitleName+" V"+g_szReleaseVersion));
	else
		SetWindowText(_F(g_strTitleName));
	//初始广告提示信息
	if(g_strLogoAdvert[0] == '@')		//如果第一个字符为@,则不显示初始广告信息
		_SetStatusTxt("");
	else if(g_strLogoAdvert[0] == 0)	//需要在Logo对话框中显示广告
	{
		if(!g_OEM.bWritePreNameInLogo)	//如果不在标题上写版本号，就要在信息栏上写版本号
		{
			CString tmpInfo;
			tmpInfo.Format("欢迎使用%s(V%s)",g_strTitleName,g_szReleaseVersion);
			_SetStatusTxt(tmpInfo);
		}
		else
			_SetStatusTxt("欢迎使用"+g_strTitleName);
	}
	else
		_SetStatusTxt(g_strLogoAdvert);

	AfterLoginTypeSel();

	UpdateData(FALSE);	

//	OnOK();
	PostMessage(WM_COMMAND, IDC_OFFLINE);

	return TRUE;
}

BOOL CLogoDlg::DestroyWindow() 
{
#if defined(LEVEL2)
	SCT_DisableProtect(m_hWnd);
#endif	
	TDEL(m_pADIE);

	return CDialog::DestroyWindow();
}

BOOL CLogoDlg::SwitchNextHost() //为TRUE表示用户选择了切换,为FALSE表示取消或不再切换
{
	static BOOL l_bSwitchNextHosting = FALSE;
	if(l_bSwitchNextHosting || l_bTryOutHost || g_bNewJyUseAdvHQHost || l_bVipHost)
		return FALSE;	//试用站不支持切换
	l_bSwitchNextHosting = TRUE;
	if(m_nReConnectNum < g_nReConnectNum)
	{
		m_nReConnectNum++;
		SendMessage(UM_STARTLOGIN);
		l_bSwitchNextHosting = FALSE;
		return TRUE;
	}
	if(m_BakHostInfo.WhichHost==g_HQHost.PrimaryHost) //如果有备份站,则支持备份站的自动跳转
	{
		if(m_BakHostInfo.BakHost.PrimaryHost<m_BakHostInfo.BakHost.HostNum-1)
		{
			m_BakHostInfo.BakHost.PrimaryHost++;
			SendMessage(UM_STARTLOGIN);
			l_bSwitchNextHosting = FALSE;
			return TRUE;
		}
		else
			m_BakHostInfo.WhichHost=-1;
	}
	//如果只有一个主站,则不切换
	if(g_HQHost.HostNum<2) 
	{
		l_bSwitchNextHosting = FALSE;
		return FALSE;
	}
	static vector<int> nSwitchLog;
	int tmpIndex = g_HQHost.PrimaryHost;
	if(g_bRandSwitch == TRUE)
	{
		if(nSwitchLog.size()>=g_HQHost.HostNum)
		{
			l_bSwitchNextHosting = FALSE;
			return FALSE;
		}
		if(find(nSwitchLog.begin(), nSwitchLog.end(), tmpIndex)==nSwitchLog.end()) 
			nSwitchLog.push_back(tmpIndex);
		srand(GetTickCount());
		while(nSwitchLog.size()<g_HQHost.HostNum&&find(nSwitchLog.begin(), nSwitchLog.end(), tmpIndex)!=nSwitchLog.end())
			tmpIndex = rand()%g_HQHost.HostNum;
		nSwitchLog.push_back(tmpIndex);
	}
	else
	{
		tmpIndex=(tmpIndex+1)%g_HQHost.HostNum;
		//如果切换一圈结束,则不切换
		if(tmpIndex == bFirstUsedHostIndex) 
		{
			l_bSwitchNextHosting = FALSE;
			return FALSE;
		}
		UsedHostIndexNum++;
	}
	if(g_bHintSwitch && !g_bNLB)
	{
		CString tmpStr="是否尝试连接另一行情主站:\n";
		tmpStr+=g_HQHost.HostName[tmpIndex];
		if(TDX_MessageBox(m_hWnd,tmpStr,MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL)
		{
			l_bSwitchNextHosting = FALSE;
			return FALSE;
		}
	}
	if(bFirstUsedHostIndex == -1) 
		bFirstUsedHostIndex = tmpIndex;
	m_HqHost_Combo.SetCurSel(tmpIndex);
	g_HQHost.PrimaryHost = tmpIndex;
	SendMessage(UM_STARTLOGIN);
	l_bSwitchNextHosting = FALSE;
	return TRUE;
}

void CLogoDlg::SwitchNextHostMsg(WPARAM wParam,LPARAM lParam)
{
	SwitchNextHost();
}

void CLogoDlg::_SetStatusTxt(LPCSTR str)
{
#ifdef OEM_NEWJY
	m_draw.m_StaticStatus.SetWindowText(_F(str));
#else
	m_Status_Static.SetText(_F(str));
#endif
}

void CLogoDlg::GetTCJyInfo()
{
	ReadTCCookie(&g_JyInfo);
	g_MngRight=g_JyInfo.khqx;
	if(g_MngRight < 1 || g_MngRight > 9) 
		g_MngRight = g_OEM.bOppositeMngRight?5:0;
	if(g_OEM.bOppositeMngRight)
	{	//没登录交易，就返回最低权限
		DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
		if(!(dwStatus&JY_LOGINED))
			g_MngRight = 5;
	}
}

#include "ShowRiskDlg.h"
BOOL l_AlwaysShowRisk = FALSE;
void CLogoDlg::StartConnect(WPARAM wParam,LPARAM lParam)
{
	//如果存在declare.txt文件，则弹出相应提示
	if(m_bHasOffMode && !l_AlwaysShowRisk && TestFileExist(HomePath+"declare.txt"))
	{
		CShowRisktDlg dlg;
		if(dlg.DoModal()==IDCANCEL)
		{
			EnableOther(TRUE);
			return;
		}
		l_AlwaysShowRisk = TRUE;
	}
	_SetStatusTxt("正在连接数据服务...");
#ifdef OEM_NEWJY
	OnPaint();	//界面重新刷一下
	g_bSaveUser=m_bSaveZH;
	g_bSaveZH=m_bSaveZH;
	g_bProtectZH=m_bProtectZH;
	WritePrivateProfileString("OTHER","SAVEZH",g_bSaveZH?"1":"0",g_WSXHStr+"newmodem.ini");
	WritePrivateProfileString("OTHER","ENCRYPTACCOUNT",g_bProtectZH?"1":"0",g_WSXHStr+"newmodem.ini");	
	if( (g_nEmbedTabSel!=LOGIN_ONLYHQ&&m_bHasOffMode) || m_bOnlyLoginWT)
	{
		int sel = m_LoginType_Combo.GetCurSel();
		if(sel<0) 
		{
			TDX_MessageBox(m_hWnd,"请选择登录方式!",MB_OK|MB_ICONINFORMATION);
			EnableOther(TRUE);
			return;
		}
		CString  strZhInfo,strPassword;
		m_embzh_Combo.GetSafeEdit().GetTextSafe(strZhInfo);
		m_edit1.GetTextSafe(strPassword);
		if(strZhInfo.GetLength()<1)
		{
			_SetStatusTxt("请输入用户帐号!");
			TDX_MessageBox(m_hWnd,"请输入用户帐号!",MB_OK|MB_ICONINFORMATION);
			EnableOther(TRUE);
			m_embzh_Combo.SetFocus();
			return;
		}
		else if(strPassword.GetLength()<1)
		{
			_SetStatusTxt("请输入用户密码!");
			TDX_MessageBox(m_hWnd,"请输入用户密码!",MB_OK|MB_ICONINFORMATION);
			EnableOther(TRUE);
			m_edit1.SetFocus();
			return;
		}
		int WTType=wtdef.sc[sel];
		if((WTType==SHHY || WTType==KHWT) && wtdef.nMustAccountLen>0 && strZhInfo.GetLength()!=wtdef.nMustAccountLen)
		{
			CString tmpStr;
			tmpStr.Format("提示:必须输入%d位帐号!",wtdef.nMustAccountLen);
			TDX_MessageBox(m_hWnd,tmpStr,MB_OK|MB_ICONINFORMATION);
			EnableOther(TRUE);
			m_edit1.SetFocus();
			return;
		}
		if(WTType==ZCYH && (g_nEmbedTabSel==LOGIN_ONLYWT || m_bOnlyLoginWT))
		{
			TDX_MessageBox(m_hWnd,"提示:试用帐号不能使用独立交易方式,请选择其它的登录方式",MB_OK|MB_ICONINFORMATION);
			EnableOther(TRUE);
			m_edit1.SetFocus();
			return;
		}
		DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
		if(!(dwStatus&JY_LOGINED))
		{
			if(strPassword.GetLength()<1)
			{
				PostQuitMessage(0);
				CDialog::OnCancel();
				return;
			}
			char LoginMsg[256];
			InputZhInfo tmpInfo;			
			memset(&tmpInfo,0,sizeof(InputZhInfo));
			tmpInfo.nZhType=wtdef.sc[sel];
			strncpy(tmpInfo.ZhInfo,strZhInfo,30);
			tmpInfo.ZhInfo[30]=0;
			strncpy(tmpInfo.Password,strPassword,30);
			tmpInfo.Password[30]=0;
			tmpInfo.SecureType=SCLV_SSL;
			int sel_aqfs=m_Aqfs_Combo.GetCurSel();
			if(sel_aqfs>=0)
				g_nLastAqfs=sel_aqfs;
			g_nLastZhType=sel;
			if(wtdef.bAuthPass || wtdef.bTDXCA)
			{
				tmpInfo.SecureType = GetSecureTypeFromSel(sel_aqfs);
				if(tmpInfo.SecureType!=SCLV_SSL)
				{
					m_edit2.GetTextSafe(strZhInfo);
					strncpy(tmpInfo.SecurePassword,strZhInfo,30);
				}
				else
					tmpInfo.SecurePassword[0]=0;					
				tmpInfo.SecurePassword[30]=0;
			}
			tmpInfo.yyb_tdxid[0]=0;
			//如果是试用帐号,则固定送10000营业部ID
			if(WTType==ZCYH)
			{
				strcpy(tmpInfo.yyb_tdxid,"10000");
				strcpy(g_strLastYYBID,"10000");
			}
			else if(wtdef.yybnum>0)
			{
				g_nLastYYBSel=m_LoginType_Combo_yh.GetCurSel();
				if(g_nLastYYBSel>=0)
				{
					strcpy(tmpInfo.yyb_tdxid,wtdef.yyb_tdxid[g_nLastYYBSel]);
					strcpy(g_strLastYYBID,wtdef.yyb_tdxid[g_nLastYYBSel]);
				}
			}
			//如果是随交易退出的外挂模块的话,向系统广播退出消息 退出,WPARAM参数定为1
			if(g_GC.bAlongJy)
			{
				extern UINT ExtMsg;
				::PostMessage(HWND_BROADCAST,ExtMsg,1,0);
			}
			if(g_OEM.bHasQSRun)
				QS_BeforeLoginJY_(0);
			//加入对TC模块的验证,防非法TCDLL替换
			float fVersion=0;
			char strBuild[255]={0};
			TC_GetVersion_(fVersion,strBuild);
			if(fVersion<1.00 || atol(strBuild)<6010101)
			{
				TDX_MessageBox(m_hWnd,"交易模块已损坏或出错!",MB_OK|MB_ICONINFORMATION);
				PostQuitMessage(0);
				CDialog::OnCancel();
				return;
			}
			if(!TC_Login_(0,1,&tmpInfo,0))
			{
				TC_GetLoginRet_(LoginMsg);
				_SetStatusTxt("登录交易失败!");
				if(m_bHasOffMode && !OnLine && !g_OEM.bNoSwitchInLogo && !g_OEM.bCanTryoutUser)
					strcat(LoginMsg,"\r\n\r\n(如果仅想浏览行情和资讯,可以选择独立行情方式)");
				TDX_MessageBox(m_hWnd,LoginMsg,MB_OK|MB_ICONINFORMATION);
				EnableOther(TRUE);
				m_edit1.SetFocus();
				SetNewYzm(); //失败后再重新生成验证码
				return;
			}
			else
			{
				//保存帐号
				m_embzh_Combo.GetSafeEdit().GetTextSafe(g_strLastZhInfo,30);
				for(int i=0;i<g_nSaveZhNum;i++)
				{
					if(strcmp(g_strSaveZhInfo[i],g_strLastZhInfo)==0)
						break;
				}
				if(i>=g_nSaveZhNum)
				{
					memmove(g_strSaveZhInfo[1],g_strSaveZhInfo[0],(MAX_SAVE_ZHNUM-1)*sizeof(char)*31);
					strcpy(g_strSaveZhInfo[0],g_strLastZhInfo);
					g_nSaveZhNum=min(g_nSaveZhNum+1,MAX_SAVE_ZHNUM);
				}
				//
				g_bWTEverLogin=TRUE;
				//
				TC_GetRightInfo_(g_CategoryInfo,g_MngRight,g_RightStr);
				if(WTType==ZCYH) strcpy(g_RightStr,"试用");
				//生成验证Cookie文件
				MakeJyVerifyFile("");
				//生成其它Cookie文件(包括上次的权限串)
				struct OtherCache_Info OtherInfo;
				memset(&OtherInfo,0,sizeof(struct OtherCache_Info));
				strncpy(OtherInfo.LastRightStr,g_RightStr,254);
				MakeOtherCookie(&OtherInfo);
				//
				if(g_OEM.bHasQSRun)
					QS_AfterLoginJY_(0);
			}
		}
		if(m_bOnlyLoginWT)
		{
			g_bHasAdvJy=FALSE;
			GetTCJyInfo();
			//是否支持快速交易
			g_bHasAdvJy=g_JyInfo.canadvjy;
			CDialog::OnOK();
			return;
		}
		else if(g_nEmbedTabSel==LOGIN_ONLYWT)
		{
			GetTCJyInfo();
			Offline();
			return;
		}
	}
	l_bTryOutHost=FALSE;
	if(g_OEM.bHasOutStandTryOut && g_nEmbedTabSel==LOGIN_ONLYHQ)
		l_bTryOutHost=TRUE;
	if(g_OEM.bHasTryout && g_nEmbedTabSel==LOGIN_ONLYHQ && !LocalJyVerify())
		l_bTryOutHost=TRUE;
	g_bHasAdvJy=FALSE;
	g_bNewJyUseAdvHQHost=FALSE;
	if(!l_bTryOutHost)
	{
		GetTCJyInfo();
		//独立行情进入方式无Level2功能(必须要经过验证后才行)
		if(g_nEmbedTabSel==LOGIN_ONLYHQ && m_bHasOffMode)
			g_JyInfo.canqslevel2 = 0;
		//是否支持快速交易
		g_bHasAdvJy=g_JyInfo.canadvjy;
		//是否支持VIP站点,如果不是查找最快的,则用选择的普通站点
		if(CanUseVipHost() && g_bNLB)
			l_bVipHost=TRUE;
		//如果是机构版,将辅助区的空间扩大
		if(g_JyInfo.canjgb && m_bHasOffMode)
			g_nDefaultRestHeight=432;
		//如果有L2插件,需得到Level2的帐号信息
		if(g_GC.bHasLevel2Engine)
		{
			GetL2Info(m_L2UserName,m_L2Password,m_nL2Type);
			//是否支持Level2行情主站
			if(m_nL2Type==0)
			{
				if(CanUseLevel2Host())
					g_bNewJyUseAdvHQHost=TRUE;
			}
			else if(m_nL2Type==1)
				g_bNewJyUseAdvHQHost=TRUE;
		}
	}
	if(g_bNewJyUseAdvHQHost)
		SetAdvHQEnvironment(TRUE);
	else
		SetAdvHQEnvironment(FALSE);
#elif LEVEL2
	if(g_nCurLevel2Tab==0)
		SetAdvHQEnvironment(TRUE);		
	else
		SetAdvHQEnvironment(FALSE);
#endif
	if(!l_bTryOutHost && !g_bNewJyUseAdvHQHost && !l_bVipHost && m_nReConnectNum==0 && !m_bDisableNLB && m_bEnableNLB && g_bNLB && g_HQHost.HostNum>1) //如果是负载均衡,则要先探索各主站的信息,重连时第一次才查找
	{
		m_bEnableNLB=FALSE;	//如果出现连接问题，暂时取消自动查找功能
		_SetStatusTxt("正在优化数据服务连接...");
		long Which_Minweight=CHQComm_NLB::NLB_GetMinweight(HQ_NLB);
		if(Which_Minweight >= 0)
		{
			g_HQHost.PrimaryHost = Which_Minweight;
			m_HqHost_Combo.SetCurSel(Which_Minweight);
		}
		else
		{
			_SetStatusTxt("数据服务连接失败,请联系我们!");
			EnableOther(TRUE);
			return;
		}
	}
	Reset();
	//
	strcpy(g_strLoginName,m_strUserName);
	strcpy(g_strPassword,m_strPassword);
	//
	if(l_bTryOutHost)	//使用试用站点
	{
		g_HQIPStr = g_HQTryOutIP;
		g_HQNameStr = g_HQTryOutIP;
		g_HQPort = g_HQTryOutPort;
	}
	else if(g_bNewJyUseAdvHQHost)
	{
		if(m_nReConnectNum==0 && !m_bDisableNLB && g_bNLB && g_L2HQHost.HostNum>1) //Level2行情主站，自动查找最快
		{
			_SetStatusTxt("正在查找最快的Level2主站...");
			long Which_Minweight=CHQComm_NLB::NLB_GetMinweight(L2HQ_NLB);
			if(Which_Minweight >= 0)
				g_L2HQHost.PrimaryHost = Which_Minweight;
			else
			{
				_SetStatusTxt("查找最快的主站失败,请检查通讯设置!");
				EnableOther(TRUE);
				return;
			}
		}
		g_HQIPStr = g_L2HQHost.IPAddress[g_L2HQHost.PrimaryHost];
		g_HQNameStr = g_L2HQHost.HostName[g_L2HQHost.PrimaryHost];
		g_HQPort = g_L2HQHost.Port[g_L2HQHost.PrimaryHost];
	}
	else if(l_bVipHost)
	{
		BOOL bFromVipHost=TRUE;
		if(m_nReConnectNum==0 && !m_bDisableNLB) //VIP行情主站，自动查找最快,有一个也要查找
		{
			_SetStatusTxt("正在查找最快的VIP主站...");
			long Which_Minweight=CHQComm_NLB::NLB_GetMinweight(VIPHQ_NLB);
			if(Which_Minweight >= 0)
				g_VipHQHost.PrimaryHost = Which_Minweight;
			else
			{
				long Which_Minweight=CHQComm_NLB::NLB_GetMinweight(HQ_NLB);
				if(Which_Minweight >= 0)
				{
					g_HQHost.PrimaryHost = Which_Minweight;
					m_HqHost_Combo.SetCurSel(Which_Minweight);
					bFromVipHost=FALSE;
				}
				else
				{
					_SetStatusTxt("查找最快的主站失败,请检查通讯设置!");
					EnableOther(TRUE);
					return;
				}
			}
		}
		if(bFromVipHost)
		{
			g_HQIPStr = g_VipHQHost.IPAddress[g_VipHQHost.PrimaryHost];
			g_HQNameStr = g_VipHQHost.HostName[g_VipHQHost.PrimaryHost];
			g_HQPort = g_VipHQHost.Port[g_VipHQHost.PrimaryHost];
		}
		else
		{
			g_HQIPStr = g_HQHost.IPAddress[g_HQHost.PrimaryHost];
			g_HQNameStr = g_HQHost.HostName[g_HQHost.PrimaryHost];
			g_HQPort = g_HQHost.Port[g_HQHost.PrimaryHost];
		}
	}
	else
	{
		g_HQIPStr = g_HQHost.IPAddress[g_HQHost.PrimaryHost];
		g_HQNameStr = g_HQHost.HostName[g_HQHost.PrimaryHost];
		g_HQPort = g_HQHost.Port[g_HQHost.PrimaryHost];
		if(m_BakHostInfo.WhichHost!=g_HQHost.PrimaryHost) 
		{
			CString tempStr,bakIni;
			bakIni.Format("%stmp\\%s.ini",g_WSXHStr,g_HQIPStr);
			GetHostInfoFromFile(&m_BakHostInfo.BakHost,NULL,NULL,NULL,NULL,NULL,NULL,FALSE,bakIni);
			m_BakHostInfo.BakHost.PrimaryHost=-1;
			m_BakHostInfo.WhichHost=g_HQHost.PrimaryHost;
			_SetStatusTxt("正在连接数据服务,稍等...");
		}
		else if(m_BakHostInfo.BakHost.HostNum>0 && m_BakHostInfo.BakHost.PrimaryHost>=0) //如果有备份站
		{
			g_HQIPStr=m_BakHostInfo.BakHost.IPAddress[m_BakHostInfo.BakHost.PrimaryHost];
			g_HQNameStr=m_BakHostInfo.BakHost.HostName[m_BakHostInfo.BakHost.PrimaryHost];
			g_HQPort=m_BakHostInfo.BakHost.Port[m_BakHostInfo.BakHost.PrimaryHost];
			_SetStatusTxt("正在连接备份数据,稍等...");
		}
	}
	//
	g_pComm->SetServerProperties(g_HQIPStr,g_HQPort);
	SetTimer(0x100,g_nLogoTimeOut*1000,NULL); //连接过程的超时
	if(!g_pComm->Connect())
	{
		KillTimer(0x100);
		if(Is_HQInit_Time())
			_SetStatusTxt("数据服务正在初始化,请稍候连接!");
		else
		{
			_SetStatusTxt("无法连接数据服务!");
			if(SwitchNextHost()) return;
		}
		EnableOther(TRUE);
		GetDlgItem(IDOK)->SetFocus();
		return;
	}
	m_bSecondConnect = FALSE;	//开始第一次连接
}

void CLogoDlg::OnConnectResult(WPARAM wParam,LPARAM lParam)
{
	KillTimer(0x100);	//杀掉连接超时定时器
	KillTimer(0x200);
	BOOL bIsComm3=(lParam==1);
	//期货
	if(bIsComm3) 
	{
		if(!AsynConnectResult(TRUE,wParam))
		{
			g_pComm3->Disconnect();	//断开连接
			_SetStatusTxt(g_pComm3->GetErrorStr());
			CLogoDlg::OnOK();
		}
		else
		{
			g_pComm3->StartThread();
			g_SysInfo.ConnectDSHQ_LastYmd=GetYMD();
			g_SysInfo.ConnectDSHQ_LastHms=GetHMS();
			
			SetTimer(0x201,g_nLogoTimeOut*1000,NULL); //第一数据包的超时,防止有些假连接
			m_bHasPackTimer3=TRUE;
			//开始请求数据
			g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
			g_nAssisID= DS_LOGIN_ASSISID;
			_SetStatusTxt("请求扩展行情服务...");
			g_pGetData->DSLogin();
		}
		return;
	}
	//A股
	if(!AsynConnectResult(FALSE,wParam))
	{
		g_pComm->Disconnect();	//断开连接
		if(Is_HQInit_Time())
			_SetStatusTxt("数据服务正在初始化,请稍候连接!");
		else
		{
			_SetStatusTxt(g_pComm->GetErrorStr());
			if(SwitchNextHost()) return;
		}
		EnableOther(TRUE);
		GetDlgItem(IDOK)->SetFocus();
		return;
	}
	else
	{
		g_pComm->StartThread();
		g_SysInfo.ConnectHQ_LastYmd=GetYMD();
		g_SysInfo.ConnectHQ_LastHms=GetHMS();
		
		SetTimer(0x101,g_nLogoTimeOut*1000,NULL); //第一数据包的超时,防止有些假连接
		m_bHasPackTimer=TRUE;
		//开始请求数据
		/*
		if(!g_bAuthSuccess && !g_bWTEverLogin && g_UserHostQSID[0]==0 && g_nLastLoginType!=JY_LOGIN) //需要对行情用户进行认证
		{
			g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
			g_nAssisID = NEEDAUTH_ASSISID;
		#ifdef OEM_NEWJY
			if(g_GC.bHasLevel2Engine && g_bUserHostEverConnect)
				g_pGetData->NeedAuth(g_L2UserName,g_L2Password);
			else
				g_pGetData->NeedAuth("newjy","");
		#else
			g_pGetData->NeedAuth(m_strUserName,m_strPassword);
		#endif
		}
		else
		{
			g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
			g_nAssisID = INMODE2_ASSISID;
		#ifdef OEM_NEWJY
			if(g_GC.bHasLevel2Engine && g_bUserHostEverConnect)
				g_pGetData->Inmode2(g_L2UserName,"");
			else
				g_pGetData->Inmode2("newjy","");
		#else
			g_pGetData->Inmode2(m_strUserName,"");
		#endif
		}
		*/

		g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
		g_nAssisID = NEEDAUTH_ASSISID;
		g_pGetData->NeedAuth("","");
	}
}

void CLogoDlg::OnLevel2Cfg()
{
	CL2FaceDlg dlg;
	dlg.DoModal();
	UpdateData(FALSE);
}

void CLogoDlg::OnOK()
{
	SendMessage(UM_STARTLOGIN);
}

void CLogoDlg::StartLogin(WPARAM wParam,LPARAM lParam)
{
	GetDlgItem(IDOK)->EnableWindow(FALSE);

//	EnableOther(FALSE);
	GetDlgItem(IDC_USERNAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_PASSWORD)->EnableWindow(FALSE);
	m_RegLogin.EnableWindow(FALSE);
	UpdateData(TRUE);
	TrimString(m_strPassword);
	TrimString(m_strUserName);
	//处理连接
	g_bSaveUser = m_bSaveZH;
	g_bSavePass = m_bSavePass;
	g_bHasCaption = (m_nMode==0);

	SendMessage(UM_STARTCONNECT);

	/*
	_SetStatusTxt("正在同步基本配置信息...");
	//!!!
	g_nUseDNSUrl = 0;
	char strSyncWebFile[MAX_PATH]={0};
	sprintf(strSyncWebFile, "http://%s/down/html/download/DX_4/DXRJ.zip", g_strDNSUrl[g_nUseDNSUrl]);
	if(!WebContentDownload(strSyncWebFile, "DXRJ.zip"))
	{
		sprintf(strSyncWebFile, "http://%s/down/html/download/DX_4/DXRJ.zip", g_strDNSUrl[1]);
		if(WebContentDownload(strSyncWebFile, "DXRJ.zip"))
			g_nUseDNSUrl = 1;
	}
	UnZipSomeFile("DXRJ.zip", g_WSXHStr, FALSE);
	DeleteFile(HomePath+"DXRJ.zip");
	*/
}

void CLogoDlg::OnGetDataACK(WPARAM wParam,LPARAM lParam)
{
	BOOL bIsComm3=(wParam==1);
	if(bIsComm3) 
	{
		char pInfo[255]={0};
		BOOL bOK=OnDSLoginGetDataACK(this,LOGO_WIN,m_bHasPackTimer3,m_nMainID,&m_Progress,pInfo);
		if(strlen(pInfo))
			_SetStatusTxt(pInfo);
		if(bOK)
		{
			//登录成功过
			g_AntiHack.bDSLogo = TRUE;

			DSOnLine = TRUE;
			//		
			CDialog::OnOK();
		}
		return;
	}

	//传统行情的处理
	if(m_bHasPackTimer)
	{
		//第一数据包的超时时钟的释放
		KillTimer(0x101);
		m_bHasPackTimer=FALSE;
	}
	switch(g_AnsHeader.AssisID)
	{
	case NEEDAUTH_ASSISID:
		{
			char sucflag = 0,need_auth = 0;
			char errmsg[100];
			g_pGetData->NeedAuthACK(sucflag,errmsg,need_auth);
			need_auth = 0;
			if(sucflag==0)
			{
				g_pComm->Disconnect();	//断开连接
				EnableOther(TRUE);
				_SetStatusTxt(errmsg);
				return;
			}
			/*
			{
				int weekinfo=GetWeek(g_iToday); //开市期间进入再验证
				if((weekinfo!=0 && weekinfo!=6 && Is_Jy_Time(30)) && !Lhzq_JyVerify())
				{
					g_pComm->Disconnect();	//断开连接
					EnableOther(TRUE);
					_SetStatusTxt("行情只给交易客户使用,请打开交易软件验证后再重试!");
					if(TDX_MessageBox(m_hWnd,"网上行情只提供给交易客户使用，请您进行用户验证。\n请点击确定按钮打开交易软件。",MB_ICONQUESTION|MB_OKCANCEL)==IDOK)
					{
						char ExecFileName[256];
						GetPrivateProfileString("EXTERN","WTPATH","",ExecFileName,255,g_WSXHStr+g_strYxhjCfg);
						ExecExtern(ExecFileName,0);
					}
					break;
				}
			}
			else if(g_bWtUserVerifyStyle!=4 && need_auth==1)
			{
				if(l_bTryOutHost)
				{
					g_pComm->Disconnect();	//断开连接
					EnableOther(TRUE);
					TDX_MessageBox(m_hWnd,"试用行情已满!\n\n请使用[行情+交易]方式登录.",MB_OK|MB_ICONEXCLAMATION);
					_SetStatusTxt("试用行情已满!请使用[行情+交易]方式登录.");
					break;
				}
				if(!LocalJyVerify())
				{
					g_pComm->Disconnect();	//断开连接
					EnableOther(TRUE);
					if(g_bWtUserVerifyStyle<2)
					{
						#ifdef OEM_NEWJY	
							TDX_MessageBox(m_hWnd,"连接被拒绝,行情主站需要进行身份验证!\n\n请使用[行情+交易]方式登录客户端.",MB_OK|MB_ICONEXCLAMATION);
						#endif
						_SetStatusTxt("连接被拒绝,行情主站需要进行身份验证!");
					}
					else
					{
						_SetStatusTxt("");
						//打开核新资讯方式比较特别，不需要提示，并且用资讯模块来进行验证，每次都要验证
						if(!g_GC.bAlongOpenHXInfo)
						{
							if(TDX_MessageBox(m_hWnd,"您必须通过委托认证,方可使用本系统.\n是否愿意进行委托认证?",MB_YESNO|MB_DEFBUTTON1|MB_ICONQUESTION)!=IDYES)
								return;
						}
						GetDlgItem(IDC_USERNAME)->EnableWindow(FALSE);
						GetDlgItem(IDC_PASSWORD)->EnableWindow(FALSE);
						GetDlgItem(IDC_SAVEPASS_CHECK)->ShowWindow(SW_HIDE);
						switch(g_bWtUserVerifyStyle)
						{
						case 2:		//核新交易方式(DLL)
							DoHexinDLLWt(m_hWnd,TRUE);
							break;	
						case 3:		//核新交易方式(EXE)
							if(g_GC.bAlongOpenHXInfo)
							{
								//打开核新的独立资讯程序
								char path[256],param[256];
								sprintf(path,"%sinfocenter\\InfoLogin.exe",HomePath);
								sprintf(param,"%X -V",m_hWnd);
								ExecExtern(path,m_hWnd,param,FALSE);
							}
							else
								DoHexinEXEVerify(m_hWnd);
							break;	
						case 5:		//通达信交易方式(状态无关方式,数据字典方式)
							DoTDXDLLWt(m_hWnd,1,1);
							break;
						case 6:
							DoTdxExeWt(m_hWnd);
							break;
						default:
							break;
						}
					}
					break;		
				}
			}
			*/
			g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
			g_nAssisID = HOSTMORE_ASSISID;
			_SetStatusTxt("初始化数据服务...");
			g_pGetData->GetHostMoreInfo();
			//已成功地发出第一个包,g_bFirstStep置为FALSE
			g_bFirstStep = FALSE;
		}
		break;
	case INMODE2_ASSISID:
		{
			if(!g_pGetData->Inmode2ACK())
			{
				_SetStatusTxt("登录被拒绝!");
				g_pComm->Disconnect();	//断开连接
				EnableOther(TRUE);
				return;
			}
			g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
			g_nAssisID = HOSTMORE_ASSISID;
			_SetStatusTxt("初始化数据服务...");
			g_pGetData->GetHostMoreInfo();
			//已成功地发出第一个包,g_bFirstStep置为FALSE
			g_bFirstStep = FALSE;
		}
		break;
	case HOSTMORE_ASSISID:
		{
			char errflag;
			BOOL bRet=g_pGetData->GetHostMoreInfoACK(szinfdate,shinfdate,szinfhms,shinfhms,errflag);
			if(errflag != 0)
			{
				_SetStatusTxt("数据服务连接失败!");
			//	TDX_MessageBox(m_hWnd,"主站拒绝提供服务!可能原因:\n1.您用的版本与服务器不匹配 \n2.服务器端太忙 \n3.网络其它原因,请检查通讯设置.",MB_OK|MB_ICONINFORMATION);
				g_pComm->Disconnect();	//断开连接
				EnableOther(TRUE);
				return;
			}
			if(!bRet)
			{
				_SetStatusTxt("数据服务连接失败!");
				g_pComm->Disconnect();	//断开连接
				EnableOther(TRUE);
				return;
			}

			_SetStatusTxt("提取通行证信息...");
			g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
			g_nAssisID = GETPASSPORT_ASSISID;
			g_pGetData->GetPassPort("tdxlevel2",atof(g_szReleaseVersion));
			//已成功地发出第一个包,g_bFirstStep置为FALSE
			g_bFirstStep = FALSE;
		}
		break;
	case DOWNBAKHOST_ASSISID:
		{
			char   lpBuffer[30000];
			short  DNSize = g_pGetData->GetPathFileACK(lpBuffer);
			if(DNSize>0)
			{
				CFile theFile;
				CString tmpFileName;
				tmpFileName.Format("%stmp\\%s.ini",g_WSXHStr,g_HQIPStr);
				if(theFile.Open(tmpFileName,CFile::modeCreate|CFile::modeWrite))
				{
					theFile.Write(lpBuffer,DNSize);
					theFile.Close();
				}
			}
			_SetStatusTxt("验证登录信息...");
			g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
			g_nAssisID = GETPASSPORT_ASSISID;
			g_pGetData->GetPassPort(g_Scheme_King.QSID,atof(g_szMainExeVersion));
		}
		break;
	case GETPASSPORT_ASSISID:
		{
			if(!g_GC.bNoAutoup)
			{
				char errmsg[100];
				BOOL bRet=g_pGetData->GetPassPortACK(errmsg, TRUE);
				if(!bRet)
				{
					_SetStatusTxt(errmsg);
				//	TDX_MessageBox(m_hWnd,errmsg,MB_OK|MB_ICONERROR);
					g_pComm->Disconnect();	//断开连接
					EnableOther(TRUE);
					return;
				}
			}
			//如果通过认证了,才将bPassorted赋值
		//	g_AntiHack.bPassorted = TRUE;
			
			//略过
			_SetStatusTxt("下载深交所数据服务信息...");
			g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
			g_nAssisID = SZSTOCKNUM_ASSISID;
			g_pGetData->GetStockNum(0,g_iToday);
			break;
			
			//
			if(strlen(g_GC.OtherFileName)==0 || g_bDebugMode)
			{
				_SetStatusTxt("下载股票资讯信息...");
				g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
				g_nAssisID = RXXMCFG_ASSISID;
				g_pGetData->GetRXxmCfg();		
			}
			else
				TryOtherFileReq();			
		}
		break;
	case RXXMCFG_ASSISID:
		{
			g_pGetData->GetRXxmCfgACK();
			/*
			g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
			g_nAssisID = URGENT_ASSISID;
			g_pGetData->GetUrgentCfg(m_strUserName);
			*/
			_SetStatusTxt("下载深交所数据服务信息...");
			g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
			g_nAssisID = SZSTOCKNUM_ASSISID;
			g_pGetData->GetStockNum(0,g_iToday);
		}
		break;
	case URGENT_ASSISID:
		{
			g_pGetData->GetUrgentCfgACK();
		//	_SetStatusTxt("提取通告...");
			g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
			g_nAssisID = URLURGENT_ASSISID;
			g_pGetData->GetURLUrgentCfg();
		}
		break;
	case URLURGENT_ASSISID:
		{
			g_pGetData->GetURLUrgentCfgACK();
			//如果通过认证了,才将XOR字符进行赋值
			g_AntiHack.cLevel2XORChar_Queue		= 0x39;
			g_AntiHack.cLevel2XORChar_TickL2	= 0x49;
			g_AntiHack.cLevel2XORChar_ComboHQ	= 0x39;
			g_AntiHack.cLevel2XORChar_ComboHQ2	= 0x59;
			//
			if(!NeedTranInfo(0) && !NeedTranInfo(1))
			{
				NormalEnd();
				break;
			}
			//
			_SetStatusTxt("下载深交所数据服务信息...");
			g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
			g_nAssisID = SZSTOCKNUM_ASSISID;
			g_pGetData->GetStockNum(0,g_iToday);
		}
		break;
	case SZSTOCKNUM_ASSISID:
		{
			szstknum = g_pGetData->GetStockNumACK();
			if(szstknum < 100)
			{
			//	TDX_MessageBox(m_hWnd,"深交所数据服务有问题!",MB_OK|MB_ICONERROR);
				g_pComm->Disconnect();	//断开连接
				CDialog::OnCancel();
				return;
			}
			m_Progress.SetRange(0,szstknum);
			m_pSZTmp = new remote_TmpStkInfo3[szstknum];
			m_pSZMem = new MemStkInfo[szstknum];
			memset(m_pSZTmp,0,sizeof(remote_TmpStkInfo3)*szstknum);
			memset(m_pSZMem,0,sizeof(MemStkInfo)*szstknum);

			g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
			g_nAssisID = SZSTOCKINF_ASSISID;
			szstartxh = 0;
			g_pGetData->GetNewGPInf(0,szstartxh);
		#if !defined(OEM_STAR) && !defined(OEM_STAR_HM) && !defined(OEM_NEWJY) && !defined(LEVEL2) && !defined(PERSONAL)
			m_Progress.ShowWindow(SW_SHOW);
		#endif
			_SetStatusTxt("下载深交所数据服务信息...");
		}
		break;
	case SZSTOCKINF_ASSISID:
		{
			int num = g_pGetData->GetNewGPInfACK(m_pSZTmp+szstartxh);
			szstartxh+=num;
			m_Progress.SetPos(szstartxh);
			CString tempStr;
			tempStr.Format("下载深交所数据服务数据...");
			_SetStatusTxt(tempStr);
			if(szstartxh < szstknum)
			{
				g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
				g_nAssisID = SZSTOCKINF_ASSISID;
				g_pGetData->GetNewGPInf(0,szstartxh);
			}
			else
			{
				g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
				g_nAssisID = SHSTOCKNUM_ASSISID;
				g_pGetData->GetStockNum(1,g_iToday);
				_SetStatusTxt("下载上交所数据服务信息...");
			}
		}
		break;
	case SHSTOCKNUM_ASSISID:
		{
			shstknum = g_pGetData->GetStockNumACK();
			if(shstknum < 100)
			{
			//	TDX_MessageBox(m_hWnd,"上交所数据服务有问题!",MB_OK|MB_ICONERROR);
				g_pComm->Disconnect();	//断开连接					
				CDialog::OnCancel();
				return;
			}
			m_Progress.SetRange(0,shstknum);
			m_pSHTmp = new remote_TmpStkInfo3[shstknum];
			m_pSHMem = new MemStkInfo[shstknum];
			memset(m_pSHTmp,0,sizeof(remote_TmpStkInfo3)*shstknum);
			memset(m_pSHMem,0,sizeof(MemStkInfo)*shstknum);

			g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
			g_nAssisID = SHSTOCKINF_ASSISID;
			shstartxh = 0;
			g_pGetData->GetNewGPInf(1,shstartxh);
		}
		break;
	case SHSTOCKINF_ASSISID:
		{
			int num = g_pGetData->GetNewGPInfACK(m_pSHTmp+shstartxh);
			shstartxh+=num;
			m_Progress.SetPos(shstartxh);
			CString tempStr;
			tempStr.Format("下载上交所数据服务数据...");
			_SetStatusTxt(tempStr);
			if(shstartxh < shstknum)
			{
				g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
				g_nAssisID = SHSTOCKINF_ASSISID;
				g_pGetData->GetNewGPInf(1,shstartxh);
			}
			else
			{
				int i;
				for (i = 0; i < szstknum; i++ )
				{
					MemStkInfo_rtn3 ( m_pSZTmp + i, m_pSZMem + i );
					m_pSZMem[i].setcode = SZ;	//填充setcode字段,Added 2001.8.18
					m_pSZMem[i].bigmarket = ABGP_TYPE;
				}
				for (i = 0; i < shstknum; i++ )
				{
					MemStkInfo_rtn3 ( m_pSHTmp + i, m_pSHMem + i );
					m_pSHMem[i].setcode = SH;	//填充setcode字段,Added 2001.8.18
					m_pSHMem[i].bigmarket = ABGP_TYPE;
				}
				if(!SaveIt(0) || !SaveIt(1))
				{
				//	TDX_MessageBox(m_hWnd,"数据存盘失败!\n请确认运行目录有没有读写权限或磁盘空间是否足够,\n可能病毒将磁盘文件损坏,\n必要时请用数据维护工具清除缓存。",MB_OK|MB_ICONINFORMATION);
					g_pComm->Disconnect();	//断开连接						
					CDialog::OnCancel();
					break;
				}
				//判断那些股票需要更新
				GetNodeMapCount(szstknum,shstknum,m_pSZMem,m_pSHMem);
				for(i = 0;i < szstknum;i++)
				{
					if(m_pSZMem[i].BaseFreshCount > 0) basedifnum++;
					if(m_pSZMem[i].GbbqFreshCount > 0) gbbqdifnum++;
				}
				for(i = 0;i < shstknum;i++)
				{
					if(m_pSHMem[i].BaseFreshCount > 0) basedifnum++;
					if(m_pSHMem[i].GbbqFreshCount > 0) gbbqdifnum++;
				}
				//
				if(g_bDebugMode)
					NormalEnd();
				else
				{
				//	TryBlockDatReq();
					//
					if(TryBaseReq())
						break;
					if(!TryGbbqReq())
						NormalEnd();
				}
				break;
			}
		}
		break;
	case BLOCKDATLEN_ASSISID:
		{
			char usemdstr,mdstr[33],local_mdstr[33];
			blockdat_filelen = g_pGetData->GetFileLenACK(&usemdstr,mdstr);
			blockdat_pos = 0;
			if(blockdat_filelen > 0)
			{
				BOOL bNeedGet=TRUE;
				FILE *fpt=fopen(g_WSXHStr+"LData\\cache\\block.dat","rb");
				if(fpt)
				{
					long local_size = on_filelength(fpt);
					if(local_size==blockdat_filelen)
					{
						if(usemdstr==0 || mdstr[0]==0)
							bNeedGet=FALSE;
						else
						{
							BYTE *buf = new BYTE[blockdat_filelen];
							fread(buf,blockdat_filelen,1,fpt);
							MD5_Buffer(buf,blockdat_filelen,local_mdstr);
							delete buf;
							if(strcmp(mdstr,local_mdstr)==0)
								bNeedGet=FALSE;
						}
					}
					fclose(fpt);
				}
				if(bNeedGet)
				{
					FileBuffer = new char[blockdat_filelen+30000];
					g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
					g_nAssisID = BLOCKDAT_ASSISID;
					g_pGetData->GetPathFile("block.dat",blockdat_pos);
					m_Progress.SetRange32(0,blockdat_filelen);
					m_Progress.SetPos(0);
					break;
				}
			}
			//
			if(TryBaseReq())
				break;
			if(!TryGbbqReq())
				NormalEnd();
			//掠过
			//	TryZHBZipReq();
			break;
		}
		break;
	case BLOCKDAT_ASSISID:
		{
			long retlen = g_pGetData->GetPathFileACK(FileBuffer+blockdat_pos);
			blockdat_pos+=retlen;
			if(retlen == 30000)
			{
				CString tempStr;
				tempStr.Format("下载概念板块股信息...");
				_SetStatusTxt(tempStr);
				g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
				g_nAssisID = BLOCKDAT_ASSISID;
				g_pGetData->GetPathFile("block.dat",blockdat_pos);
				m_Progress.SetPos(blockdat_pos);
			}
			else
			{
				CFile blockfile;
				if(blockfile.Open(g_WSXHStr+"LData\\cache\\block.dat",CFile::modeCreate|CFile::modeWrite))
				{
					blockfile.Write(FileBuffer,blockdat_filelen);
					blockfile.Close();
				}
				else
					TDX_MessageBox(m_hWnd,"写盘错误!",MB_ICONEXCLAMATION|MB_OK);
				TDEL(FileBuffer);
				
				//
				if(TryBaseReq())
					break;
				if(!TryGbbqReq())
					NormalEnd();
				//掠过
			//	TryZHBZipReq();
			}			
		}
		break;
	case ZHBZIPLEN_ASSISID:
		{
			char usemdstr,mdstr[33],local_mdstr[33];
			zhbzip_filelen = g_pGetData->GetFileLenACK(&usemdstr,mdstr);
			zhbzip_pos = 0;
			if(zhbzip_filelen > 0)
			{
				BOOL bNeedGet=TRUE;
				FILE *fpt=fopen(g_WSXHStr+"LData\\cache\\zhb.zip","rb");
				if(fpt)
				{
					long local_size = on_filelength(fpt);
					if(local_size==zhbzip_filelen)
					{
						if(usemdstr==0 || mdstr[0]==0)
							bNeedGet=FALSE;
						else
						{
							BYTE *buf = new BYTE[zhbzip_filelen];
							fread(buf,zhbzip_filelen,1,fpt);
							MD5_Buffer(buf,zhbzip_filelen,local_mdstr);
							delete buf;
							if(strcmp(mdstr,local_mdstr)==0)
								bNeedGet=FALSE;
						}
					}
					fclose(fpt);
				}
				if(bNeedGet)
				{
					FileBuffer = new char[zhbzip_filelen+30000];
					g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
					g_nAssisID = ZHBZIP_ASSISID;
					g_pGetData->GetPathFile("zhb.zip",zhbzip_pos);
					m_Progress.SetRange32(0,zhbzip_filelen);
					m_Progress.SetPos(0);
					break;
				}
			}
			if(g_GC.bHasGPGrade)
				TryJGDataReq();
			else
			{
				if(TryBaseReq())
					break;
				if(!TryGbbqReq())
					NormalEnd();
			}
		}
		break;
	case ZHBZIP_ASSISID:
		{
			long retlen = g_pGetData->GetPathFileACK(FileBuffer+zhbzip_pos);
			zhbzip_pos+=retlen;
			if(retlen == 30000)
			{
				CString tempStr;
				tempStr.Format("同步信息服务...");
				_SetStatusTxt(tempStr);
				g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
				g_nAssisID = ZHBZIP_ASSISID;
				g_pGetData->GetPathFile("zhb.zip",zhbzip_pos);
				m_Progress.SetPos(zhbzip_pos);
			}
			else
			{
				TDEL(FileBuffer);

				if(TryBaseReq())
					break;
				if(!TryGbbqReq())
					NormalEnd();
			}
		}
		break;
	case JGDATALEN_ASSISID:
		{
			char usemdstr,mdstr[33],local_mdstr[33];
			jgdata_filelen = g_pGetData->GetFileLenACK(&usemdstr,mdstr);
			jgdata_pos = 0;
			if(jgdata_filelen > 0)
			{
				BOOL bNeedGet=TRUE;
				FILE *fpt=fopen(g_WSXHStr+"LData\\cache\\jgdata.zip","rb");
				if(fpt)
				{
					long local_size = on_filelength(fpt);
					if(local_size==jgdata_filelen)
					{
						if(usemdstr==0 || mdstr[0]==0)
							bNeedGet=FALSE;
						else
						{
							BYTE *buf = new BYTE[jgdata_filelen];
							fread(buf,jgdata_filelen,1,fpt);
							MD5_Buffer(buf,jgdata_filelen,local_mdstr);
							delete buf;
							if(strcmp(mdstr,local_mdstr)==0)
								bNeedGet=FALSE;
						}
					}
					fclose(fpt);
				}
				if(bNeedGet)
				{
					FileBuffer = new char[jgdata_filelen+30000];
					g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
					g_nAssisID = JGDATA_ASSISID;
					g_pGetData->GetPathFile("jgdata.zip",jgdata_pos);
					m_Progress.SetRange32(0,jgdata_filelen);
					m_Progress.SetPos(0);
					break;
				}
			}
			if(TryBaseReq())
				break;
			if(!TryGbbqReq())
				NormalEnd();
		}
		break;
	case JGDATA_ASSISID:
		{
			long retlen = g_pGetData->GetPathFileACK(FileBuffer+jgdata_pos);
			jgdata_pos+=retlen;
			if(retlen == 30000)
			{
				CString tempStr;
				tempStr.Format("更新机构数据文件...");
				_SetStatusTxt(tempStr);
				g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
				g_nAssisID = JGDATA_ASSISID;
				g_pGetData->GetPathFile("jgdata.zip",jgdata_pos);
				m_Progress.SetPos(jgdata_pos);
			}
			else
			{
				CFile cfile;
				if(cfile.Open(g_WSXHStr+"LData\\cache\\jgdata.zip",CFile::modeCreate|CFile::modeWrite))
				{
					cfile.Write(FileBuffer,jgdata_filelen);
					cfile.Close();
					_SetStatusTxt("正在处理机构数据文件...");
					BOOL bHasEmbwtFile=FALSE,bHasNewHostFile=FALSE;
					 //解压到本地
					UnZipSomeFile(g_WSXHStr+"LData\\cache\\jgdata.zip",g_WSXHStr+"LData\\cache\\",bHasEmbwtFile,bHasNewHostFile);
				}
				TDEL(FileBuffer);
				if(TryBaseReq())
					break;
				if(!TryGbbqReq())
					NormalEnd();
			}
		}
		break;
	case OTHERFILELEN_ASSISID:
		{
			char usemdstr,mdstr[33],local_mdstr[33];
			otherfile_filelen = g_pGetData->GetFileLenACK(&usemdstr,mdstr);
			otherfile_pos = 0;
			if(otherfile_filelen > 0)
			{
				BOOL bNeedGet=TRUE;
				FILE *fpt=fopen(g_WSXHStr+g_GC.OtherFileName,"rb");
				if(fpt)
				{
					long local_size = on_filelength(fpt);
					if(local_size==otherfile_filelen)
					{
						if(usemdstr==0 || mdstr[0]==0)
							bNeedGet=FALSE;
						else
						{
							BYTE *buf = new BYTE[otherfile_filelen];
							fread(buf,otherfile_filelen,1,fpt);
							MD5_Buffer(buf,otherfile_filelen,local_mdstr);
							delete buf;
							if(strcmp(mdstr,local_mdstr)==0)
								bNeedGet=FALSE;
						}
					}
					fclose(fpt);
				}
				if(bNeedGet)
				{
					FileBuffer = new char[otherfile_filelen+30000];
					g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
					g_nAssisID = OTHERFILE_ASSISID;
					g_pGetData->GetPathFile(g_GC.OtherFileName,otherfile_pos);
					m_Progress.SetRange32(0,otherfile_filelen);
					m_Progress.SetPos(0);
					break;
				}
			}
			_SetStatusTxt("提取远程消息配置...");
			g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
			g_nAssisID = RXXMCFG_ASSISID;
			g_pGetData->GetRXxmCfg();				
		}
		break;
	case OTHERFILE_ASSISID:
		{
			long retlen = g_pGetData->GetPathFileACK(FileBuffer+otherfile_pos);
			otherfile_pos+=retlen;
			if(retlen == 30000)
			{
				CString tempStr;
				tempStr.Format("更新信息文件...");
				_SetStatusTxt(tempStr);
				g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
				g_nAssisID = OTHERFILE_ASSISID;
				g_pGetData->GetPathFile(g_GC.OtherFileName,otherfile_pos);
				m_Progress.SetPos(otherfile_pos);
			}
			else
			{
				CFile otherfile;
				if(otherfile.Open(g_WSXHStr+g_GC.OtherFileName,CFile::modeCreate|CFile::modeWrite))
				{
					otherfile.Write(FileBuffer,otherfile_filelen);
					otherfile.Close();
					if(IsZipFile(g_GC.OtherFileName))
					{
						BOOL bHasEmbwtFile=FALSE,bHasNewHostFile=FALSE;
						//解压到本地的T0002目录下
						UnZipSomeFile(g_WSXHStr+g_GC.OtherFileName,g_WSXHStr,bHasEmbwtFile,bHasNewHostFile);
						if(bHasEmbwtFile)
						{
							//如果是包里面有embwt.cfg,则移动到Homepath下
							CFile somefile;
							if(somefile.Open(g_WSXHStr+"embwt.cfg",CFile::modeRead|CFile::shareDenyNone))
							{
								somefile.Close();
								if(::CopyFile(g_WSXHStr+"embwt.cfg",HomePath+"embwt.cfg",FALSE))
									unlink(g_WSXHStr+"embwt.cfg");
							}
						}
						if(bHasNewHostFile)
						{
							TDXWHOST HQHost,InfoHost,WTHost,VipWTHost,DSHost;
							PROXYINFO TdxProxyHost;
							struct VipVerify_Info VerifyInfo;
							GetHostInfoFromFile(&HQHost,&InfoHost,&WTHost,&VipWTHost,&DSHost,&TdxProxyHost,&VerifyInfo,FALSE,g_WSXHStr+"newhost.lst");
							WriteHostInfoToFile(&HQHost,&InfoHost,&WTHost,&VipWTHost,&DSHost,&TdxProxyHost,&VerifyInfo,FALSE,g_WSXHStr+g_strYxhjCfg);
						}	
						//将必要的文件拷贝过去
						static char *s_NeedCopyCfgFile[]={"jyfx.txt","lcxy.txt","jjfx.txt","lcfx.txt","zzfx.txt","ksfx.txt","welcome.bmp","logo.jpg","oem.bmp","oem.cfg","qscomte.dat","dsmarket.dat","dsys.dat","declare.txt","load.bmp","embui.cfg","autoupex.exe","TcOem.xml","eTrade.xml",""};
						int k=0;
						CString tmpPath1,tmpPath2;
						CFile OtherFile;
						while(strlen(s_NeedCopyCfgFile[k]))
						{
							tmpPath1.Format("%s%s",g_WSXHStr,s_NeedCopyCfgFile[k]);
							if(OtherFile.Open(tmpPath1,CFile::modeRead|CFile::shareDenyNone))
							{
								OtherFile.Close();
								tmpPath2.Format("%s%s",HomePath,s_NeedCopyCfgFile[k]);
								if(::CopyFile(tmpPath1,tmpPath2,FALSE))
									unlink(tmpPath1);
							}
							k++;
						}
					}
				}
				else
					TDX_MessageBox(m_hWnd,"写盘错误!",MB_ICONEXCLAMATION|MB_OK);
				TDEL(FileBuffer);
				_SetStatusTxt("提取远程消息配置...");
				g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
				g_nAssisID = RXXMCFG_ASSISID;
				g_pGetData->GetRXxmCfg();		
			}			
		}
		break;
	case AUTOBASE_ASSISID:
		{
			short retnum = g_pGetData->GetBaseInfACK(szstknum,shstknum,m_pSZMem,m_pSHMem);
			basestartxh+=retnum;
			m_Progress.SetPos(basestartxh);
			if(!TryBaseReq())
			{
				//将Base及Base.Map存档
			//	_SetStatusTxt("正在将更新结果存盘...");				
				if(SaveToBaseDbf())
					SaveMapFile(TRUE);
				if(!TryGbbqReq())
					NormalEnd();
			}
		}
		break;
	case AUTOGBBQ_ASSISID:
		{
			short retnum;
			if(g_pGetData->GetGbbqInfACK(retnum,szstknum,shstknum,m_pSZMem,m_pSHMem) == FALSE)
			{
			//	_SetStatusTxt("权息资料库不能存盘!");
				g_pComm->Disconnect();	//断开连接
				EnableOther(TRUE);
				return;
			}
			gbbqstartxh+=retnum;
			m_Progress.SetPos(gbbqstartxh);
			if(!TryGbbqReq())
			{
				//将Gbbq.Map存档
				SaveMapFile(FALSE);
				NormalEnd();
			}
		}
		break;
	}
}

BOOL CLogoDlg::TryBlockDatReq()
{
	_SetStatusTxt("下载概念板块股信息...");
	g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
	g_nAssisID = BLOCKDATLEN_ASSISID;
	g_pGetData->GetFileLen("block.dat");
	return TRUE;
}

BOOL CLogoDlg::TryZHBZipReq()
{
	_SetStatusTxt("更新信息文件...");
	g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
	g_nAssisID = ZHBZIPLEN_ASSISID;
	g_pGetData->GetFileLen("zhb.zip");
	return TRUE;
}

BOOL CLogoDlg::TryJGDataReq()
{
	_SetStatusTxt("更新机构数据文件...");
	g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
	g_nAssisID = JGDATALEN_ASSISID;
	g_pGetData->GetFileLen("jgdata.zip");
	return TRUE;
}

BOOL CLogoDlg::TryOtherFileReq()
{
	_SetStatusTxt("更新券商信息文件...");
	g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
	g_nAssisID = OTHERFILELEN_ASSISID;
	g_pGetData->GetFileLen(g_GC.OtherFileName);
	return TRUE;
}

BOOL CLogoDlg::TryBaseReq()
{
	char tmpBuf[AUTOBASE_COUNT*(1+SH_CODE_LEN)];
	memset(tmpBuf,0,AUTOBASE_COUNT*(1+SH_CODE_LEN));
	int tmpnum = 0,i;
	if(basepos.setcode == 0)
	{
		for(i = basepos.pos;i < szstknum;i++)
		{
			if(m_pSZMem[i].BaseFreshCount > 0)
			{
				memcpy(tmpBuf+tmpnum*(SH_CODE_LEN+1),&(m_pSZMem[i].setcode),1); //同时将setcode带过去
				memcpy(tmpBuf+tmpnum*(SH_CODE_LEN+1)+1,m_pSZMem[i].Code,SH_CODE_LEN);
				tmpnum+=1;
				basepos.pos = i+1;
				if(tmpnum >= AUTOBASE_COUNT) break;
			}
		}
	}
	if(tmpnum < AUTOBASE_COUNT)
	{
		if(basepos.setcode == 0)
		{
			basepos.setcode = 1;
			basepos.pos = 0;
		}
		for(i = basepos.pos;i < shstknum;i++)
		{
			if(m_pSHMem[i].BaseFreshCount > 0) 
			{
				memcpy(tmpBuf+tmpnum*(1+SH_CODE_LEN),&(m_pSHMem[i].setcode),1);  
				memcpy(tmpBuf+tmpnum*(1+SH_CODE_LEN)+1,m_pSHMem[i].Code,SH_CODE_LEN);
				tmpnum+=1;
				basepos.pos = i+1;
				if(tmpnum >= AUTOBASE_COUNT) break;
			}
		}
	}
	if(tmpnum > 0)
	{
		if(basestartxh==0)
		{
			m_Progress.SetRange(0,basedifnum);
			m_Progress.SetPos(0);
		}

		CString tempStr;
		tempStr.Format("更新基本资料信息...");
		_SetStatusTxt(tempStr);
		g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
		g_nAssisID = AUTOBASE_ASSISID;
		g_pGetData->GetBaseInf(tmpnum,tmpBuf);
		return TRUE;
	}
	return FALSE;
}

BOOL CLogoDlg::TryGbbqReq()
{
	char tmpBuf[AUTOGBBQ_COUNT*(1+SH_CODE_LEN)];
	memset(tmpBuf,0,AUTOGBBQ_COUNT*(1+SH_CODE_LEN));
	int tmpnum = 0,i;
	for(i=0;i<szstknum;i++)
	{
		if(m_pSZMem[i].GbbqFreshCount > 0) 
		{
			memcpy(tmpBuf+tmpnum*(SH_CODE_LEN+1),&(m_pSZMem[i].setcode),1);
			memcpy(tmpBuf+tmpnum*(SH_CODE_LEN+1)+1,m_pSZMem[i].Code,SH_CODE_LEN);
			tmpnum+=1;
			if(tmpnum >= AUTOGBBQ_COUNT) break;
		}
	}
	if(tmpnum<AUTOGBBQ_COUNT)
	{
		for(i = 0;i < shstknum;i++)
		{
			if(m_pSHMem[i].GbbqFreshCount > 0) 
			{
				memcpy(tmpBuf+tmpnum*(1+SH_CODE_LEN),&(m_pSHMem[i].setcode),1);
				memcpy(tmpBuf+tmpnum*(1+SH_CODE_LEN)+1,m_pSHMem[i].Code,SH_CODE_LEN);
				tmpnum+=1;
				if(tmpnum >= AUTOGBBQ_COUNT) break;
			}
		}
	}
	if(tmpnum > 0)
	{
		if(gbbqstartxh==0)
		{
			m_Progress.SetRange(0,gbbqdifnum);
			m_Progress.SetPos(0);
		}
		
		CString tempStr;
		tempStr.Format("更新权息资料库%3d%%",gbbqstartxh*100/gbbqdifnum);
		_SetStatusTxt(tempStr);
		g_nMainID = MainIDPlus(m_nMainID,LOGO_WIN);
		g_nAssisID = AUTOGBBQ_ASSISID;
		g_pGetData->GetGbbqInf(tmpnum,tmpBuf);
		return TRUE;
	}
	return FALSE;
}

void CLogoDlg::WriteOrAppend(DBH_Head *pDbf,MemStkInfo *pStkInfo,BOOL bWrite,int nWhich)
{
	static char BaseRectitle[][7] = 
	{
		"ZGB","GJG","FQRFRG","FRG","BG",
		"HG", "ZGG", "ZZC", "LDZC","GDZC",
		"WXZC","CQTZ","LDFZ","CQFZ","ZBGJJ", "JZC",
		"ZYSY","ZYLY","QTLY","YYLY", "TZSY","BTSY",
		"YYWSZ","SNSYTZ","LYZE","SHLY","JLY","WFPLY","TZMGJZ"
	};
    float *basef;
	char  str1[40];

	memset (pDbf->RecBuf, ' ', pDbf->RecLen);
	
	if(bWrite)	ReadDbf(pDbf,nWhich);

	PrevFieldStr(pDbf,"GPDM",pStkInfo->Code);		
	sprintf(str1,"%d",pStkInfo->setcode);
	PrevFieldStr(pDbf,"SC",str1);		
	sprintf(str1,"%f",pStkInfo->ActiveCapital);
	PrevFieldStr(pDbf,"LTAG",str1);		
	sprintf(str1,"%ld",pStkInfo->J_gxrq);
	PrevFieldStr(pDbf,"GXRQ",str1);
	sprintf(str1,"%ld",pStkInfo->J_start);
	PrevFieldStr(pDbf,"SSDATE",str1);
	sprintf(str1,"%d",pStkInfo->J_addr);
	PrevFieldStr(pDbf,"DY",str1);			
	sprintf(str1,"%d",pStkInfo->J_hy);
	PrevFieldStr(pDbf,"HY",str1);
	sprintf(str1,"%2.0f",pStkInfo->J_HalfYearFlag); //必须用2.0f,不然ZBNB位数少，将之截为0
	PrevFieldStr(pDbf,"ZBNB",str1);

	basef=(float *) &(pStkInfo->J_zgb);
	for (int i = 0;i < 29;i++)
	{ 
		sprintf(str1,"%f",*(basef+i));
		PrevFieldStr(pDbf,BaseRectitle[i],str1);
	}
	if(bWrite)	WriteDbf(pDbf,nWhich);
	else		WriteDbf(pDbf,APPEND_DBF);
	pStkInfo->BaseFreshCount = -(pStkInfo->BaseFreshCount);
}

BOOL CLogoDlg::SaveToBaseDbf()
{
	DBH_Head    CONFIG;
	char basefile[MAX_PATH];
	sprintf(basefile,"%sLData\\cache\\bd.dbf",g_WSXHStr);
	if(OpenBase(&CONFIG,basefile,short(O_BINARY|O_RDWR)) != 0)	return FALSE; //客户端必须有一个bd.dbf
	int i,nWhich = 0;
	char  str1[40];
	MemStkInfo *pStkInfo;
	for (BaseDataTop(&CONFIG);;)
	{
		nWhich++;
		GetFieldStr(&CONFIG, "GPDM", str1);
		AllTrim(str1);
		pStkInfo = NULL;
		for(i=0;i < szstknum;i++)
		{
			if(strcmp(str1,m_pSZMem[i].Code)==0 && m_pSZMem[i].BaseFreshCount > 0)
			{
				pStkInfo = m_pSZMem+i;
				break;
			}
		}
		if(!pStkInfo)
		{
			for(i=0;i < shstknum;i++)
			{
				if(strcmp(str1,m_pSHMem[i].Code)==0 && m_pSHMem[i].BaseFreshCount > 0)
				{
					pStkInfo = m_pSHMem+i;
					break;
				}
			}
		}
		if(pStkInfo)
			WriteOrAppend(&CONFIG,pStkInfo,TRUE,nWhich);
		if(Skip(1,&CONFIG) == -1) //如果到尾，则将没有记录条的股票加到后面
		{
			for(i=0;i < szstknum;i++)
			{
				if(m_pSZMem[i].BaseFreshCount > 0)
					WriteOrAppend(&CONFIG,m_pSZMem+i,FALSE);
			}
			for(i=0;i < shstknum;i++)
			{
				if(m_pSHMem[i].BaseFreshCount > 0)
					WriteOrAppend(&CONFIG,m_pSHMem+i,FALSE);
			}
			break;
		}
	}
	CloseBase(&CONFIG);
	return TRUE;
}

void CLogoDlg::SaveMapFile(BOOL bBase)
{
	CStdioFile theFile;
	CString	tmpLine;
	int i;
	if(bBase)	//存Base.Map
	{
		if(!theFile.Open(g_WSXHStr+"LData\\cache\\base.map",CFile::modeWrite|CFile::modeCreate)) return;
		for(i=0;i < szstknum;i++)
		{
			if(m_pSZMem[i].BaseFreshCount == 0) continue;
			tmpLine.Format("%-6s%-6d\r\n",m_pSZMem[i].Code,abs(m_pSZMem[i].BaseFreshCount));
			theFile.WriteString(tmpLine);
		}
		for(i=0;i < shstknum;i++)
		{
			if(m_pSHMem[i].BaseFreshCount == 0) continue;
			tmpLine.Format("%-6s%-6d\r\n",m_pSHMem[i].Code,abs(m_pSHMem[i].BaseFreshCount));
			theFile.WriteString(tmpLine);
		}
		theFile.Close();
	}
	else		//存Gbbq.Map
	{
		if(!theFile.Open(g_WSXHStr+"LData\\cache\\gbbq.map",CFile::modeWrite|CFile::modeCreate)) return;
		for(i=0;i < szstknum;i++)
		{
			if(m_pSZMem[i].GbbqFreshCount == 0) continue;
			tmpLine.Format("%-6s%-6d\r\n",m_pSZMem[i].Code,abs(m_pSZMem[i].GbbqFreshCount));
			theFile.WriteString(tmpLine);
		}
		for(i=0;i < shstknum;i++)
		{
			if(m_pSHMem[i].GbbqFreshCount == 0) continue;
			tmpLine.Format("%-6s%-6d\r\n",m_pSHMem[i].Code,abs(m_pSHMem[i].GbbqFreshCount));
			theFile.WriteString(tmpLine);
		}
		theFile.Close();
	}
}

void CLogoDlg::NormalEnd()
{
	if(m_bSavePass && g_nLastLoginType!=JY_LOGIN)
		WritePasswordToRegister(g_Scheme_King.VersionString,(char*)(LPCSTR)m_strUserName,(char*)(LPCSTR)m_strPassword,g_nLastValidDate,g_nLastValidValue);
	WriteToConnectCfg();
	//风格信息也写入
	if(m_bHasOffMode)
	{
		if(g_nLastLoginType!=GUEST_LOGIN && stricmp(g_strLoginName,"GUEST"))
			g_bIsGuest = FALSE; //只有当第一次按用户名进入时才不是GUEST
		else
			strcpy(g_strLoginName,"GUEST");
	}		
	//登录成功过
	g_AntiHack.bLogo = TRUE;
	//
	OffLineMode	= FALSE;

	//登录期货
	if(g_bHasDS && !DSOnLine) //如果是负载均衡,则要先探索各主站的信息,重连时第一次才查找
	{
		if(g_DSHost.HostNum>1 && m_bNLB)
		{
			_SetStatusTxt("正在优化数据服务连接...");
			long Which_Minweight=CHQComm_NLB::NLB_GetMinweight(DSHQ_NLB);
			if(Which_Minweight >= 0)
			{
				g_DSHost.PrimaryHost = Which_Minweight;
				WritePrivateProfileString("DSHOST","PrimaryHost",IntToStr(g_DSHost.PrimaryHost),HomePath+"100nian.dat");
			}
		}
		//开始处理扩展行情
		g_DSHQIPStr = g_DSHost.IPAddress[g_DSHost.PrimaryHost];
		g_DSHQNameStr = g_DSHost.HostName[g_DSHost.PrimaryHost];
		g_DSHQPort = g_DSHost.Port[g_DSHost.PrimaryHost];		
		g_pComm3->SetServerProperties(g_DSHQIPStr,g_DSHQPort);
		_SetStatusTxt("正在连接服务器,请稍等...");
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		SetTimer(0x200,g_nLogoTimeOut*1000,NULL);	//连接过程的超时
		if(!g_pComm3->Connect()) 
		{
			KillTimer(0x200);
			_SetStatusTxt("不能连接到主站!");	//检测后还连接不上
		}
		return;
	}

	CDialog::OnOK();
}

void CLogoDlg::OnCancel() 
{
	if(m_bOnlyLoginWT)
	{
		CDialog::OnCancel();
		return;
	}
	g_pComm->Disconnect();	//断开连接	
	WriteToConnectCfg();

	CDialog::OnCancel();
}

void CLogoDlg::OnSelchangeHqhostCombo()
{
	int index = m_HqHost_Combo.GetCurSel();
	if(index != CB_ERR)
		g_HQHost.PrimaryHost = index;
}

void CLogoDlg::AfterLoginTypeSel()
{
	int sel = m_LoginType_Combo.GetCurSel();
	int	sel_aqfs=m_Aqfs_Combo.GetCurSel();
	if(sel<0) return;
#ifdef OEM_NEWJY
	if(wtdef.sc[sel]==ZCYH)
		m_draw.m_Static2.SetWindowText(_F("试用密码"));
	else if(!l_bCXMode)
		m_draw.m_Static2.SetWindowText(g_GC.bIsHKVersion?_F("登入密码"):_F("交易密码"));
	else
		m_draw.m_Static2.SetWindowText(_F("查询密码"));
	int nSecureType = GetSecureTypeFromSel(sel_aqfs);
	if(nSecureType==SCLV_CA || (nSecureType==SCLV_AUTHPASS && g_OEM.bGxzqEmbedWTVersion))
		m_pCABt->ShowWindow(SW_SHOW);
	else
		m_pCABt->ShowWindow(SW_HIDE);
	if(nSecureType==SCLV_CA || nSecureType==SCLV_AUTHPASS)
	{
		m_Aqfs_Combo.MoveWindow(m_draw.m_comboboxRect3);
		m_edit2.MoveWindow(m_draw.m_edit2Rect);
		m_edit2.ShowWindow(SW_SHOW);
		if(g_nEmbedTabSel!=LOGIN_ONLYHQ)
		{
			m_edit2.EnableWindow(TRUE);
			m_edit2.RedrawWindow();
		}
		m_edit2.ModifyExtendedStyle(NULL,ES_EX_PASSWORD,TRUE);
		m_draw.m_Yzm_Static.SetWindowText(" ");
		m_bShowYzm=FALSE;
	}
	else
	{
		m_edit2.ModifyExtendedStyle(ES_EX_PASSWORD,NULL,TRUE);
		m_bShowYzm=TRUE;	
	}
	BOOL bShowYhCombo=FALSE;
	if(wtdef.yybnum>0)
		bShowYhCombo=TRUE;
	if(g_OEM.bZxjtEmbedWTVersion && wtdef.sc[sel]==SHHY)
		bShowYhCombo=FALSE;
	if(bShowYhCombo) 
	{
		m_LoginType_Combo.MoveWindow(m_draw.m_comboboxRect2);
		m_LoginType_Combo_yh.MoveWindow(m_draw.m_comboboxRect_yyb);
		m_LoginType_Combo_yh.ShowWindow(SW_SHOW);
	}
	else
	{
		m_LoginType_Combo_yh.ShowWindow(SW_HIDE);
		m_LoginType_Combo.MoveWindow(m_draw.m_comboboxRect1);		
	}	
	if(strlen(g_OEM.DescSHHYName)>0 && wtdef.sc[sel]==SHHY)
		m_draw.m_Static1.SetWindowText(_F(g_OEM.DescSHHYName));
	else if(strlen(g_OEM.DescKHWTName)>0 && wtdef.sc[sel]==KHWT)
		m_draw.m_Static1.SetWindowText(_F(g_OEM.DescKHWTName));
	else
		m_draw.m_Static1.SetWindowText(_F(wtdef.scname[sel]));
	//控制输入的最大长度
	if(wtdef.nMaxAccountLen>0 && (wtdef.sc[sel]==SHHY || wtdef.sc[sel]==KHWT))
		m_embzh_Combo.GetSafeEdit().SetLimitText(wtdef.nMaxAccountLen);
	else
		m_embzh_Combo.GetSafeEdit().SetLimitText(30);

	if(l_bHideJYControl)
	{
		m_LoginType_Combo.ShowWindow(SW_HIDE);
		m_LoginType_Combo_yh.ShowWindow(SW_HIDE);
		m_embzh_Combo.ShowWindow(SW_HIDE);
		m_edit1.ShowWindow(SW_HIDE);
		m_edit2.ShowWindow(SW_HIDE);
		m_Aqfs_Combo.ShowWindow(SW_HIDE);

		GetDlgItem(IDC_CHECK_SAVEZH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_PROTECTZH)->ShowWindow(SW_HIDE);
		m_pSoftKeyBt->ShowWindow(SW_HIDE);
	}
#else
	g_nLastLoginType = m_LoginType_Combo.GetItemData(sel);
	switch(g_nLastLoginType)
	{
	case GUEST_LOGIN:
	case JY_LOGIN:
		GetDlgItem(IDC_USERNAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_PASSWORD)->EnableWindow(FALSE);
		GetDlgItem(IDC_SAVEPASS_CHECK)->ShowWindow(SW_HIDE);
		m_PassEdit.ModifyStyle(ES_NUMBER,NULL);
		break;
	case REGISTER_LOGIN:
		GetDlgItem(IDC_USERNAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_PASSWORD)->EnableWindow(TRUE);
	//	GetDlgItem(IDC_SAVEPASS_CHECK)->ShowWindow(SW_SHOW);
		m_PassEdit.ModifyStyle(ES_NUMBER,NULL);
		break;
	}
	if(!g_LoginInfo.bEnableUserName)
	{
		GetDlgItem(IDC_USERNAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_PASSWORD)->EnableWindow(FALSE);
		GetDlgItem(IDC_SAVEPASS_CHECK)->ShowWindow(SW_HIDE);
	}
#endif
}

BOOL CLogoDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == UM_STAR_URL1)
		ShellExecute(NULL, _T("open"), "http://info.stockstar.com/hb/blackhorse/blackhorse.htm", NULL, NULL, SW_SHOW);
	else if(pMsg->message == UM_STAR_URL2)
		ShellExecute(NULL, _T("open"), "http://member.stockstar.com/service/kefu/service_faq.asp", NULL, NULL, SW_SHOW);
	else if(pMsg->message == UM_STAR_URL3)
	{
	#ifdef OEM_STAR
		ShellExecute(NULL, _T("open"), "http://sales.stockstar.com/zone/sale2005/productinfo.asp?productid=55&Feetype=118", NULL, NULL, SW_SHOW);
	#else	
		ShellExecute(NULL, _T("open"), "http://sales.stockstar.com/zone/sale2005/productinfo.asp?productid=61&Feetype=117", NULL, NULL, SW_SHOW);
	#endif
	}
	else if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_F1)
		{
			CString tmpStr;
			tmpStr = HomePath+"TopView.chm";
			::ShellExecute(NULL,NULL,tmpStr,NULL,NULL,SW_SHOW);
			return TRUE;
		}
	#ifdef OEM_NEWJY
		if((LOWORD(pMsg->wParam)==VK_DOWN)||(LOWORD(pMsg->wParam)==VK_RETURN))
		{
			CWnd * SubWnd = GetFocus();
			if(SubWnd==&m_LoginType_Combo)
			{ 
				m_embzh_Combo.SetFocus();
				m_embzh_Combo.SetEditSel(0,-1);
				return TRUE;
			} 
			CEdit  *pEdit = (CEdit*)m_embzh_Combo.GetWindow(GW_CHILD);
			if(SubWnd==pEdit)
			{ 
				m_edit1.SetFocus();
				m_edit1.SetSel(0,-1);
				return TRUE;
			} 
			if(SubWnd==&m_edit1 && (wtdef.bAuthPass||wtdef.bTDXCA))
			{ 
				if(m_edit2.IsWindowVisible())
				{
					m_edit2.SetFocus();
					m_edit2.SetSel(0,-1);
					return TRUE;
				}
			}		
		}
		if (pMsg->wParam==VK_UP)
		{
			CWnd * SubWnd = GetFocus();
			if(SubWnd==&m_edit2 && (wtdef.bAuthPass||wtdef.bTDXCA))
			{ 
				m_edit1.SetFocus();
				m_edit1.SetSel(0,-1);
				return TRUE;
			} 
			if(SubWnd==&m_edit1)
			{ 
				m_embzh_Combo.SetFocus();
				m_embzh_Combo.SetEditSel(0,-1);
				return TRUE;
			} 
			CEdit  *pEdit = (CEdit*)m_embzh_Combo.GetWindow(GW_CHILD);
			if(SubWnd==pEdit)
			{ 
				m_LoginType_Combo.SetFocus();
				return TRUE;
			} 
		}
	#endif
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CLogoDlg::OnOffline()
{
	//如果没有脱机分析功能,不可能响应此消息
	if(!m_bHasOffMode)
		return;
	if(m_bOnlyLoginWT) 
	{
		OnCancel();
		return;
	}
	UpdateData(TRUE);

	g_nEmbedTabSel=LOGIN_ONLYHQ;
	Offline();
}

void CLogoDlg::Offline()
{
	g_bHasCaption = (m_nMode==0);
	OffLineMode	= TRUE;
	WriteToConnectCfg();
	_SetStatusTxt("初始化系统平台...");	
	CDialog::OnOK();
}

void CLogoDlg::OnCA()
{
#ifdef OEM_NEWJY
	if(g_OEM.bGxzqEmbedWTVersion)
	{
		int sel_aqfs=m_Aqfs_Combo.GetCurSel();
		if(sel_aqfs==1)
		{
			CAuthPassMng dlg;
			dlg.DoModal();
			return;
		}
	}
	if(TC_GetDlg_)
		TC_GetDlg_(DLG_CA);
#endif
}

#include "ConnectCfgDlg.h"
void CLogoDlg::OnConnectcfgButton()
{	
	if(!m_bOnlyLoginWT)
	{
		//在连接主站中途可以进行通讯设置
		g_pComm->Disconnect();	//断开连接	
		EnableOther(TRUE);
		_SetStatusTxt("通讯设置,就绪.");
		m_Progress.SetPos(0);
	}
	WritePrivateProfileString("HQHOST","PrimaryHost",IntToStr(g_HQHost.PrimaryHost),g_WSXHStr+g_strYxhjCfg);
	CConnectCfgDlg dlg;
#ifdef OEM_NEWJY
	dlg.m_bHasWTSet=TRUE;
#endif
	if(m_bHasOffMode)
		dlg.m_bFirstLogoStatus=TRUE;
	if(dlg.DoModal() == IDOK)
	{
		m_HqHost_Combo.ResetContent();
		for(int i=0;i < g_HQHost.HostNum;i++)
			m_HqHost_Combo.AddString(_F(g_HQHost.HostName[i]));
		m_HqHost_Combo.SetCurSel(g_HQHost.PrimaryHost);
		m_HqHost_Combo.EnableWindow(!g_bNLB);
		m_bNLB=g_bNLB;

		UpdateData(FALSE);
	}
	//使用设置后,如果是查询模式,需要在界面上提示
#ifdef OEM_NEWJY
	int sel = m_LoginType_Combo.GetCurSel();
	if(sel<0) return;
	l_bCXMode=GetPrivateProfileInt("Version","CXMode",0,g_WSXHStr+g_strYxhjCfg);
	if(wtdef.sc[sel]==ZCYH)
		m_draw.m_Static2.SetWindowText(_F("试用密码"));
	else if(!l_bCXMode)
		m_draw.m_Static2.SetWindowText(g_GC.bIsHKVersion?_F("登入密码"):_F("交易密码"));
	else
		m_draw.m_Static2.SetWindowText(_F("查询密码"));
#endif
}

#include "ProxyDlg.h"
void CLogoDlg::OnProxycfgButton()
{
	CProxyDlg dlg;
	dlg.DoModal();
}

void CLogoDlg::DrawLogoBmp(CDC *pDC,CRect rect,HANDLE hBitmap)
{
	CDC memDC;			//创建兼容性的DC
	CBitmap tempBitmap;
	memDC.CreateCompatibleDC (pDC);
	tempBitmap.CreateCompatibleBitmap (pDC,rect.Width(),rect.Height());
	CBitmap *pOldBitmap=memDC.SelectObject (&tempBitmap);
	memDC.FillSolidRect(0,0,rect.Width(),rect.Height(),GetSysColor(COLOR_BTNFACE));	
	memDC.SetBkMode (TRANSPARENT);
	/*
	//画
	CBitmap  tmpBitmap2;
	if(!hBitmap)
		tmpBitmap2.LoadBitmap(IDB_LOGO1);
	else //如果有OEM图片,则用之
		tmpBitmap2.Attach((HBITMAP)hBitmap);
	//得到BMP的位置
	BITMAP szbitmap;
	tmpBitmap2.GetBitmap(&szbitmap);
	m_OEMBmpRect.left=m_OEMBmpRect.top=0;
	m_OEMBmpRect.right=szbitmap.bmWidth;
	m_OEMBmpRect.bottom=szbitmap.bmHeight;
	//
	CDC tmpdc;
	tmpdc.CreateCompatibleDC (&memDC);
	CBitmap *pOldBitmap2 = tmpdc.SelectObject (&tmpBitmap2);
	memDC.BitBlt(0,0,rect.Width(),rect.Height(),&tmpdc,0,0,SRCCOPY);
	tmpdc.SelectObject(pOldBitmap2);
	*/
	pDC->BitBlt (rect.left,rect.top,rect.Width(),rect.Height(),&memDC,0,0,SRCCOPY);

//	memDC.SelectObject(pOldBitmap);
//	tempBitmap.DeleteObject();
	memDC.DeleteDC();
//	if(hBitmap)
//		tmpBitmap2.Detach();
}

void CLogoDlg::OnPaint() 
{
	CDialog::OnPaint();
	return;
	CPaintDC dc(this);
#if defined(OEM_STAR) || defined(OEM_STAR_HM)
	return;
#elif defined(OEM_NEWJY)
	if(m_bShowYzm && !l_bHideJYControl)
	{
		CRect rc;
		rc.left=m_draw.m_yzmrect.left+1;
		rc.top=m_draw.m_yzmrect.top;
		rc.right=rc.left+13;
		rc.bottom=rc.top+21;
		dc.FillSolidRect(rc.left,rc.top,13*YZMLENGTH,21,RGB(218,218,218));
		for(int i=0;i<YZMLENGTH;i++)
		{
			g_d.DrawYZMNumber(&dc,rc,m_yzm[i],wtdef.nColorYzm);
			rc.left+=13; 
			rc.right=rc.left+10;
		}
	}
	if(g_OEM.bWritePreNameInLogo)
	{
		CFont font; 		
		font.CreateFont(14,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,SONG_FONT);
		CFont *pOld=dc.SelectObject(&font);
		dc.SetTextColor(RGB(20,20,20)); 
		dc.SetBkMode(TRANSPARENT);
		if(g_OEM.bYhzqEmbedWTVersion)
			::DrawIconEx(dc.m_hDC,14,12,m_hYhzqIcon,16,16,0,NULL,DI_NORMAL);
		else if(g_OEM.bZxjtEmbedWTVersion)
			::DrawIconEx(dc.m_hDC,14,12,m_hZxjtIcon,16,16,0,NULL,DI_NORMAL);
		else
			m_imagelist.Draw(&dc,0,CPoint(14,12),ILD_NORMAL);
		if(g_OEM.bWriteVersionInLogo)
		{
			CString tmpTitle;
			tmpTitle.Format("%sV%s",g_strPreName,g_szReleaseVersion);
			dc.TextOut(36,14,_F(tmpTitle));
		}
		else
			dc.TextOut(36,14,_F(g_strPreName));
  		dc.SelectObject(pOld);
		font.DeleteObject();		
	}
	if(g_OEM.bHasSoftKeyCaption)
	{
		CFont font; 		
		font.CreateFont(12,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,SONG_FONT);
		CFont *pOld=dc.SelectObject(&font);
		dc.SetTextColor(RGB(40,40,40)); 
		dc.SetBkMode(TRANSPARENT);
		dc.TextOut(m_draw.m_Bt8Rect.right+4,m_draw.m_Bt8Rect.top+4,_F("密码键盘"));
  		dc.SelectObject(pOld);
		font.DeleteObject();
	}
#endif
	//开始写Logo图	
	CRect rect;
	GetClientRect(&rect);
#if defined(OEM_NEWJY)
	rect.DeflateRect(80,60,80,110);
	if(!hOEMBitmap)
		return;
#elif defined(PERSONAL) || defined(LEVEL2)
	GetDlgItem(IDC_BITMAP)->GetClientRect(&rect);
	if(!hOEMBitmap)
		return;
#endif
//	DrawLogoBmp(&dc,rect,hOEMBitmap);
}

BOOL CLogoDlg::OnEraseBkgnd(CDC* pDC) 
{
	CDC dcImage;
	if (!dcImage.CreateCompatibleDC(pDC))
		return FALSE;
	BITMAP bm;
	m_bitmap_i.GetBitmap(&bm);

	// Paint the image.
	CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap_i);
	pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
	dcImage.SelectObject(pOldBitmap);
	
	return TRUE;
}

void CLogoDlg::OnLogoEnableMessage(WPARAM wParam,LPARAM lParam)
{
	_SetStatusTxt("连接失败!");
	g_pComm->Disconnect();
	EnableOther(TRUE);
}

LRESULT CLogoDlg::OnCheckHexinWT(WPARAM wParam,LPARAM lParam)
{
	if(!g_bAuthSuccess && wParam == 1)
	{
		char *p = (char *)lParam;
		p = p+strlen(p)+1;
		char ipaddress[100];
		strncpy(ipaddress,p,99);
		ipaddress[99]=0;
		for(int i=0;i < g_WtVerifyIPNum;i++)
		{
			if(strcmp(g_WTVerifyIP[i].ipaddress,ipaddress)==0)
				break;
		}
		if(i >= g_WtVerifyIPNum && g_WtVerifyIPNum>0)
		{
			MessageBox("该主站不提供对此营业部的行情服务!","TopView",MB_OK|MB_ICONEXCLAMATION);
			return 0;
		}
		MakeJyVerifyFile(ipaddress); //生成验证Cookie文件
		g_bAuthSuccess = TRUE;
		g_pComm->Disconnect();	//断开连接
		PostMessage(WM_COMMAND,IDOK);
	}
	return 0;
}

LRESULT CLogoDlg::OnCheckTDXWT(WPARAM wParam,LPARAM lParam)
{
	if(!g_bAuthSuccess)
	{
		char *p = (char *)wParam;
		char ipaddress[26];
		strncpy(ipaddress,p,25);
		ipaddress[25]=0;
		MakeJyVerifyFile(ipaddress); //生成验证Cookie文件

		g_bAuthSuccess = TRUE;
		g_pComm->Disconnect();	//断开连接
		PostMessage(WM_COMMAND,IDOK);
	}
	return 0;
}

void CLogoDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==0x100 || nIDEvent==0x101)
	{
		if(nIDEvent==0x100)
			KillTimer(0x100);
		else 
		{
			KillTimer(0x101);
			m_bHasPackTimer=FALSE;
		}
		g_pComm->Disconnect();	//断开连接

		_SetStatusTxt("连接超时!");
		if(SwitchNextHost()) return;
		EnableOther(TRUE);
		GetDlgItem(IDOK)->SetFocus();
		return;
	}	
	else if(nIDEvent==0x200 || nIDEvent==0x201)
	{
		if(nIDEvent==0x200)
			KillTimer(0x200);
		else
		{
			KillTimer(0x201);
			m_bHasPackTimer3=FALSE;
		}
	//	SendMessage(UM_CONNECT_SUCCESS, 0, 1);
		g_pComm3->Disconnect();	//断开连接
		_SetStatusTxt("连接超时!");
	}
	CDialog::OnTimer(nIDEvent);
}

void CLogoDlg::OnSelchangeLogintypeCombo() 
{
	AfterLoginTypeSel();
}

void CLogoDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	if(m_bFirstShow && bShow)
	{
		if(g_bAutoLogo || m_bDirectConnect || g_nTmpDirectConnectHq)	
		{
			PeekMsg();
			PostMessage(WM_COMMAND,IDOK);
		}
		g_nTmpDirectConnectHq = FALSE;	
		m_bFirstShow=FALSE;
	}
}

UINT CLogoDlg::OnNcHitTest(CPoint point) 
{
	int tmp =  CDialog::OnNcHitTest(point);
	if(tmp==HTCLIENT)
		return HTCAPTION;
	return tmp;
}

void CLogoDlg::_EnableCommbox(BOOL bEn)
{
	if(!::IsWindow(m_LoginType_Combo.m_hWnd) || !::IsWindow(m_LoginType_Combo_yh.m_hWnd)) 
		return;
	int n=m_LoginType_Combo.GetCurSel();
	if(bEn)
	{
		m_LoginType_Combo.EnableWindow(TRUE);
		if(wtdef.yybnum>0) 
			m_LoginType_Combo_yh.EnableWindow(TRUE);
		else 
			m_LoginType_Combo_yh.EnableWindow(FALSE);
		m_Aqfs_Combo.EnableWindow(TRUE);
	}
	else
	{
		m_LoginType_Combo.EnableWindow(FALSE);
		m_LoginType_Combo_yh.EnableWindow(FALSE);
		m_Aqfs_Combo.EnableWindow(FALSE);
	}
}

LRESULT CLogoDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
#ifdef OEM_NEWJY
	if(message==UM_MYBITMAPBUTTON)
	{
		switch((UINT)lParam)
		{
		case ID_CLOSEBT:
			PostMessage(WM_COMMAND, IDCANCEL);
			break;
		case IDC_CONNECTCFG_BUTTON2:
			PostMessage(WM_COMMAND, IDC_CONNECTCFG_BUTTON);
			break;
		case ID_HELPBT:
			GoToHelp();
			break;
		case ID_SYSTESTBT:
			OnSpeedTest(0,0);
			break;
		case IDOK:
			PostMessage(WM_COMMAND, IDOK);
			break;
		case IDC_OFFLINE:
			PostMessage(WM_COMMAND, IDC_OFFLINE);
			break;
		case IDCANCEL:
			PostMessage(WM_COMMAND, IDCANCEL);
			break;
		case ID_SELBT1:
			{
				g_nEmbedTabSel=LOGIN_HQWT;
				EnableEmbedWTSel();
				_EnableCommbox(TRUE);
				//
				CString tmpStr;
				m_embzh_Combo.GetSafeEdit().GetTextSafe(tmpStr);
				if(strlen(tmpStr)==0)
					m_embzh_Combo.SetFocus();
				else
					m_edit1.SetFocus();
			}
			break;
		case ID_SELBT2:
			{
				if(g_OEM.bCanTryoutUser)
				{
					if(g_OEM.bCanActiveUser)
					{
						CNewMenu showMore;
						showMore.CreatePopupMenu();
						showMore.AppendMenu(0, ID_JY_REGUSER, _F("试用帐号申请"));
						showMore.AppendMenu(0, ID_JY_ACTIVEUSER, _F("试用帐号激活"));						
						CRect rc;
						m_pSel2Bt->GetWindowRect(&rc);
						showMore.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, rc.left, rc.bottom, this);
					}
					else if(TC_GetDlg_)
						TC_GetDlg_(DLG_REGUSER);
					break;
				}
				g_nEmbedTabSel=LOGIN_ONLYHQ;
				EnableEmbedWTSel();
				_EnableCommbox(FALSE);
				m_pOKBt->SetFocus();
				if(g_OEM.bHasTryout && !LocalJyVerify())
					TDX_MessageBox(m_hWnd,"如果您是我公司客户请使用\'行情+交易\'进行身份识别；\r\n如果您不是我公司用户，请点击\'试用\',并尽快到我公司开户。",MB_OK|MB_ICONINFORMATION);
				else
					PostMessage(WM_COMMAND, IDOK);
			}
			break;
		case ID_SELBT3:
			{
				g_nEmbedTabSel=LOGIN_ONLYWT;
				EnableEmbedWTSel();
				_EnableCommbox(TRUE);
				//
				CString tmpStr;
				m_embzh_Combo.GetSafeEdit().GetTextSafe(tmpStr);
				if(strlen(tmpStr)==0)
					m_embzh_Combo.SetFocus();
				else
					m_edit1.SetFocus();
			}
			break;
		case ID_SOFTKEYBT:
			g_hSoftKeyWnd=SoftKey_Switch(m_hWnd,FALSE,NULL,CRect(0,0,0,0));
			switch(m_SoftFocus)
			{
			case 0:
				m_edit1.SetFocus();
				break;
			case 1:
				m_edit2.SetFocus();
				break;
			}
			break;
		case ID_CABT:
			PostMessage(WM_COMMAND,ID_CABT);
			break;
		}
	}
#endif
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CLogoDlg::OnSpeedTest(WPARAM wParam,LPARAM lParam)
{
	if(m_pSpeedTestDlg)
	{
		TDEL(m_pSpeedTestDlg);
	}
	m_pSpeedTestDlg=(CSpeedTestDlg*)new CSpeedTestDlg(this);
	if(m_pSpeedTestDlg->DoModal()==IDOK)
	{
		int nSel=m_pSpeedTestDlg->m_Return;
		g_HQHost.PrimaryHost=nSel;
		m_HqHost_Combo.SetCurSel(nSel);
		m_bEnableNLB=FALSE;
#ifndef OEM_NEWJY
		SendMessage(UM_STARTLOGIN);
#endif
	}
}

void CLogoDlg::OnNLBCheckClick() 
{
	UpdateData(TRUE);
	g_bNLB=m_bNLB;
	m_HqHost_Combo.EnableWindow(!g_bNLB);
}

void CLogoDlg::OnSelchangeComboAqfs() 
{
#ifdef OEM_NEWJY
	if(m_Aqfs_Combo.GetCurSel()==0)
	{
		m_edit2.ShowWindow(SW_SHOW);
		m_edit2.SetTextSafe("");
		SetNewYzm();
	}		
	else
	{
		m_edit2.ShowWindow(SW_SHOW);
		m_edit2.SetTextSafe("");
	}
	m_edit2.SetFocus();
	AfterLoginTypeSel();
#endif
}

void CLogoDlg::OnSoftkeyMsg(WPARAM wParam,LPARAM lParam)
{
#ifdef OEM_NEWJY
	if(lParam==SOFTKEY_VK_CLOSE)
	{
		SoftKey_Close();
		return;
	}
	else if(lParam==SOFTKEY_VK_RETURN)
	{
		switch(wParam)
		{
		case 0:
			m_edit1.SetFocus();
			m_edit1.SetSel(-1,-1);
			break;
		case 1:
			m_edit2.SetFocus();
			m_edit2.SetSel(-1,-1);
			break;
		}
		PostMessage(WM_KEYDOWN,VK_RETURN,0);
		return;
	}
	switch(lParam)
	{
	case 0:
		{
			CString tmpStr,tmpThisStr;
			m_edit1.GetTextSafe(tmpStr);
			if(wParam == VK_BACK)
			{
				int nLen=tmpStr.GetLength();
				if(nLen>0)
					tmpStr=tmpStr.Left(nLen-1);
			}
			else
			{
				if(strlen(tmpStr)<g_OEM.nLogoMaxPassordLen)
				{
					tmpThisStr.Format("%c",wParam);
					tmpStr+=tmpThisStr;
				}
			}
			m_edit1.SetTextSafe(tmpStr);
		}
		break;
	case 1:
		{
			CString tmpStr,tmpThisStr;
			m_edit2.GetTextSafe(tmpStr);
			if(wParam == VK_BACK)
			{
				int nLen=tmpStr.GetLength();
				if(nLen>0)
					tmpStr=tmpStr.Left(nLen-1);
			}
			else
			{
				if(strlen(tmpStr)<g_OEM.nLogoMaxAuthPassLen)
				{
					tmpThisStr.Format("%c",wParam);
					tmpStr+=tmpThisStr;
				}
			}
			m_edit2.SetTextSafe(tmpStr);
		}
		break;
	}
#endif
}

void CLogoDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
#ifdef OEM_NEWJY
	CRect rc1,rc2;
	rc1.SetRectEmpty();
	rc2.SetRectEmpty();
	if(g_GC.bHasLevel2Engine && !g_GC.bZHFromExtern)
		m_draw.m_L2Cfg_Static.GetRect(rc1);
	m_draw.m_HelpURL_Static.GetRect(rc2);
	if(rc2.PtInRect(point))
	{
		if(strlen(g_Url.LogoHelpStr)>0 && strlen(g_Url.LogoHelpURL)>0)
		{
			SetCursor(m_hCanPress);
			m_draw.m_HelpURL_Static.SetTextColor(RGB(0,0,255));
			InvalidateRect(rc2);
		}
	}
	else
	{
		if(rc1.PtInRect(point))
			SetCursor(m_hCanPress);
		else
			SetCursor(LoadCursor(NULL,IDC_ARROW));
		if(strlen(g_Url.LogoHelpStr)>0 && strlen(g_Url.LogoHelpURL)>0)
		{
			m_draw.m_HelpURL_Static.SetTextColor(RGB(0,0,0));
			InvalidateRect(rc2);
		}
	}
#endif
	CDialog::OnMouseMove(nFlags, point);
}

void CLogoDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
#ifdef OEM_NEWJY
	CRect rc;
	m_draw.m_StaticSaveUsername.GetRect(rc);
	if(rc.PtInRect(point))
	{
		int check=m_SaveZHButton.GetCheck();
		check=check==0?1:0;
		m_SaveZHButton.SetCheck(check);
	}

	m_draw.m_StaticProtectUsername.GetRect(rc);
	if(rc.PtInRect(point))
	{
		int check=m_ProtectZHButton.GetCheck();
		check=check==0?1:0;
		m_ProtectZHButton.SetCheck(check);
		SetEditPSAttribute(&m_embzh_Combo,check);
	}

	if(g_GC.bHasLevel2Engine && !g_GC.bZHFromExtern)
	{
		m_draw.m_L2Cfg_Static.GetRect(rc);
		if(rc.PtInRect(point))
			PostMessage(WM_COMMAND,IDC_LEVIN2CFG);
	}
	if(strlen(g_Url.LogoHelpStr)>0 && strlen(g_Url.LogoHelpURL)>0)
	{
		m_draw.m_HelpURL_Static.GetRect(rc);
		if(rc.PtInRect(point))
			ShellExecute(NULL, _T("open"), g_Url.LogoHelpURL, NULL, NULL, SW_SHOW);
	}
#endif
	CDialog::OnLButtonUp(nFlags, point);
}

BOOL CLogoDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	if(!g_bAuthSuccess && pCopyDataStruct && pCopyDataStruct->dwData==1)
	{
		if( (g_bWtUserVerifyStyle==3 && pCopyDataStruct->cbData>100) ||
			(g_bWtUserVerifyStyle==6) )
		{
			char tmpStr[100]={0},ipaddress[100]={0};
			if(g_bWtUserVerifyStyle==3)
			{
				if(g_GC.bAlongOpenHXInfo && pWnd)
					g_hHXInfoWnd=pWnd->m_hWnd;
				strncpy(tmpStr,(char *)pCopyDataStruct->lpData+5,90);
				GetStrDomainValue(tmpStr,"SignHost",ipaddress,100,"\r\n");
				for(int i=0;i < g_WtVerifyIPNum;i++)
				{
					if(strcmp(g_WTVerifyIP[i].ipaddress,ipaddress)==0)
						break;
				}
				if(i >= g_WtVerifyIPNum && g_WtVerifyIPNum>0)
				{
					MessageBox("该主站不提供对此营业部的行情服务!","TopView",MB_OK|MB_ICONEXCLAMATION);
					return FALSE;
				}				
			}
			MakeJyVerifyFile(ipaddress); //生成验证Cookie文件			
			g_bAuthSuccess = TRUE;
			g_pComm->Disconnect();	//断开连接
			PostMessage(WM_COMMAND,IDOK);
			return TRUE;
		}
	}
	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}

//随机生成验证码
void CLogoDlg::GenYzm()
{
	srand((unsigned)time(NULL));
#ifdef OEM_NEWJY
	memset(&m_yzm,0,sizeof(m_yzm));
	for(int i=0;i<YZMLENGTH;i++)
	{
		m_yzm[i]=rand()%10;
	}
#endif
}

BOOL  CLogoDlg::CheckYzm(int nLength,char* yzm,char *str)
{
	if(strlen(str)!=nLength)
		return FALSE;
	BOOL flag=FALSE;
	for(int i=0;i<nLength;i++)
	{
		if(str[i]-'0'!=yzm[i])
			break;
		if(i==nLength-1)
			flag=TRUE;
	}
	return flag;
}

//重新设置新的验证码
void CLogoDlg::SetNewYzm()
{
#ifdef OEM_NEWJY
	GenYzm();
	InvalidateRect(&m_draw.m_yzmrect);
	m_edit2.SetTextSafe("");
	m_edit2.SetSel(0,0); //将Caret复位
	UpdateData(FALSE);
#endif
}

void CLogoDlg::OnSetfocusZhInfo() 
{
#ifdef OEM_NEWJY
	_SetStatusTxt("请输入帐号信息!");
#endif
}

void CLogoDlg::OnSetfocusPassword() 
{
#ifdef OEM_NEWJY
	_SetStatusTxt("请输入密码信息!");
	if(g_hSoftKeyWnd)
		::PostMessage(g_hSoftKeyWnd,UM_SOFTKEY_SETINDEX,0,wtdef.bMustDigitPwd?1:0);
	m_SoftFocus=0;
#endif
}

void CLogoDlg::OnSetfocusAuthpwd() 
{
#ifdef OEM_NEWJY
	int sel_aqfs=m_Aqfs_Combo.GetCurSel();
	int nSecureType = GetSecureTypeFromSel(sel_aqfs);
	switch(nSecureType) 
	{
	case SCLV_SSL:
		if(!g_OEM.bAlwaysSoftKey)
			SoftKey_Close();
		_SetStatusTxt("请输入右边灰色图片中的4个数字!");
		break;
	case SCLV_AUTHPASS:			
		_SetStatusTxt("请输入认证口令!");
		if(g_hSoftKeyWnd)
			::PostMessage(g_hSoftKeyWnd,UM_SOFTKEY_SETINDEX,1,0);
		m_SoftFocus=1;
		break;
	case SCLV_CA:
		if(!g_OEM.bAlwaysSoftKey)
			SoftKey_Close();
		_SetStatusTxt("请输入CA证书密码!点击右边按钮可进行证书管理.");
		break;
	}
#endif
}

void CLogoDlg::OnKillfocusGddmCombo()
{
#ifdef OEM_NEWJY
	if(wtdef.ZJZHMaxLen>0)
	{
		int sel = m_LoginType_Combo.GetCurSel();
		int sel2 = m_LoginType_Combo_yh.GetCurSel();
		if(sel>=0 && (wtdef.sc[sel]==SHHY||wtdef.sc[sel]==KHWT))
		{
			char tmpStr[80];
			char tmpZjzh[31];
			m_embzh_Combo.GetSafeEdit().GetTextSafe(tmpZjzh,30);
			int tmpLen=strlen(tmpZjzh);
			if(tmpLen>0)
			{
				if(sel2>=0)	//如果有营业部,则补营业部编号
				{
					int tmpLen2=strlen(wtdef.yyb_tdxid[sel2]);
					if(tmpLen2>0 && tmpLen2+tmpLen<=wtdef.ZJZHMaxLen && wtdef.yyb_noprefix[sel2]==0)
					{
						strcpy(tmpStr,wtdef.yyb_tdxid[sel2]);
						for(int kk=0;kk<wtdef.ZJZHMaxLen-tmpLen-tmpLen2;kk++)
							strcat(tmpStr,"0");
						strcat(tmpStr,tmpZjzh);
						m_embzh_Combo.GetSafeEdit().SetTextSafe(tmpStr);
					}
				}
				else		//没有营业部，则全部补0
				{
					tmpStr[0]=0;
					for(int kk=0;kk<wtdef.ZJZHMaxLen-tmpLen;kk++)
						strcat(tmpStr,"0");
					strcat(tmpStr,tmpZjzh);
					m_embzh_Combo.GetSafeEdit().SetTextSafe(tmpStr);
				}
			}
		}
	}
#endif
}

void CLogoDlg::SetEditPSAttribute(CSafeComboBox *pCombo,BOOL PSStyle)
{
	if(!pCombo || !::IsWindow(pCombo->m_hWnd)) return;
	//先将原来的内容保存起来
	CString tmpStr="";
	(pCombo->GetSafeEdit()).GetTextSafe(tmpStr);
	//先清空Combo的数据
	pCombo->ResetContent();
	if(PSStyle)
		pCombo->ModifyExtendedStyle(NULL,ES_EX_PASSWORD,TRUE);
	else
	{
		pCombo->ModifyExtendedStyle(ES_EX_PASSWORD,NULL,TRUE);
		if(!g_OEM.bYhzqEmbedWTVersion)
		{
			for(int i=0;i<g_nSaveZhNum;i++)
				pCombo->AddString(g_strSaveZhInfo[i]);
		}
	}
	//再设置回去
	(pCombo->GetSafeEdit()).SetTextSafe(tmpStr);
	pCombo->Invalidate();
}

void CLogoDlg::OnCheckProtectZH() 
{
#ifdef OEM_NEWJY
	UpdateData(TRUE);
	SetEditPSAttribute(&m_embzh_Combo,m_bProtectZH);
#endif
}

void CLogoDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
#ifdef LEVEL2
	g_nCurLevel2Tab=m_Tab.GetCurSel();
	GetDlgItem(IDC_USERNAME)->EnableWindow(g_nCurLevel2Tab==0);
	GetDlgItem(IDC_PASSWORD)->EnableWindow(g_nCurLevel2Tab==0);
	GetDlgItem(IDC_SAVEPASS_CHECK)->EnableWindow(g_nCurLevel2Tab==0);
	//重新填充列表
	GetHostInfoFromFile(&g_HQHost,NULL,NULL,NULL,&g_DSHost,NULL,&g_VipVerify_Info,TRUE,g_WSXHStr+g_strYxhjCfg,(g_nCurLevel2Tab==0));
	m_HqHost_Combo.ResetContent();
	for(int i=0;i < g_HQHost.HostNum;i++)
		m_HqHost_Combo.AddString(_F(g_HQHost.HostName[i]));
	m_HqHost_Combo.SetCurSel(g_HQHost.PrimaryHost);	
#endif
	*pResult = 0;
}

//弹出注册用户
void CLogoDlg::On_Jy_RegUser()
{
	if(TC_GetDlg_)
		TC_GetDlg_(DLG_REGUSER);
}

//弹出激活注册用户
void CLogoDlg::On_Jy_ActiveUser()
{
	if(TC_GetDlg_)
		TC_GetDlg_(DLG_ACTIVEUSER);
}


//////////////////////////////////////////////////////////////////////////
//class DrawInterface
//////////////////////////////////////////////////////////////////////////

#define CHECKRESOURCE(a) {if(!a){\
	TDX_MessageBox(NULL,"资源文件不匹配!",MB_ICONEXCLAMATION|MB_OK);\
	return;}\
}

DrawInterface::DrawInterface()
{
	m_pParentWnd=NULL;
}

void DrawInterface::Init()
{
	if(g_resModule)
	{
		HINSTANCE tmphi= AfxGetResourceHandle();
		AfxSetResourceHandle(g_resModule);
		m_bmWnd.LoadBitmap("IDB_LOGODLG_WND");
		AfxSetResourceHandle(tmphi);
	}
}

void DrawInterface::GetWndRgn(CRgn &wndRgn)
{
	g_GUI.GetBitmapRgn(wndRgn,&m_bmWnd,RGB(0,255,0));
}

void DrawInterface::OnInitDialog(CWnd *pParentWnd)
{
	m_Static0.SetParent(pParentWnd);
	m_Static1.SetParent(pParentWnd);
	m_Static2.SetParent(pParentWnd);
	m_Static3.SetParent(pParentWnd);
	m_StaticStatus.SetParent(pParentWnd);
	m_StaticSaveUsername.SetParent(pParentWnd);
	m_StaticProtectUsername.SetParent(pParentWnd);
	m_Yzm_Static.SetParent(pParentWnd);
	m_L2Cfg_Static.SetParent(pParentWnd);
	m_HelpURL_Static.SetParent(pParentWnd);
	m_Static0.SetWindowText(_F("登录方式"));
	m_Static1.SetWindowText(_F("帐号信息"));
	if(!l_bCXMode)
		m_Static2.SetWindowText(g_GC.bIsHKVersion?_F("登入密码"):_F("交易密码"));
	else
		m_Static2.SetWindowText(_F("查询密码"));
	m_Static3.SetWindowText(_F("安全方式"));
	m_StaticStatus.SetWindowText("");
	m_StaticSaveUsername.SetWindowText(_F("记住帐号"));
	m_StaticProtectUsername.SetWindowText(_F("保护帐号"));
	m_Yzm_Static.SetWindowText("");
	if(g_GC.bHasLevel2Engine && !g_GC.bZHFromExtern)
		m_L2Cfg_Static.SetWindowText(_F("高级行情设置"));
	else
		m_L2Cfg_Static.SetWindowText("");
	m_HelpURL_Static.SetWindowText(g_Url.LogoHelpStr);
	
	if(g_OEM.bWhiteForeColor)
	{
		m_Static0.SetTextColor(RGB(240,230,166));
		m_Static1.SetTextColor(RGB(240,230,166));
		m_Static2.SetTextColor(RGB(240,230,166));
		m_Static3.SetTextColor(RGB(240,230,166));
		m_StaticSaveUsername.SetTextColor(RGB(240,230,166));
		m_StaticProtectUsername.SetTextColor(RGB(240,230,166));
		m_L2Cfg_Static.SetTextColor(RGB(240,230,166));
	}
	else
	{
		m_Static0.SetTextColor(RGB(0,0,0));
		m_Static1.SetTextColor(RGB(0,0,0));
		m_Static2.SetTextColor(RGB(0,0,0));
		m_Static3.SetTextColor(RGB(0,0,0));
		m_StaticSaveUsername.SetTextColor(RGB(0,0,0));
		m_StaticProtectUsername.SetTextColor(RGB(0,0,0));
		m_L2Cfg_Static.SetTextColor(RGB(0,0,255));
	}
	m_Yzm_Static.SetTextColor(0);	
	m_HelpURL_Static.SetTextColor(RGB(0,0,0));
	COLORREF cor=g_GUI.GetColorCfgFromResource(g_resModule,"IDR_COLOR","LOGODLG_STATUS_STATIC",-1);
	m_StaticStatus.SetTextColor(cor);

	HRSRC hres=FindResource(g_resModule,"IDR_POSITION2_LOGODLG",RT_HTML);
	CHECKRESOURCE(hres);
	HGLOBAL hglobal=LoadResource(g_resModule,hres);
 	CString buff=(char *)LockResource(hglobal);
	buff.Replace("\r","");
	buff.Replace("\n","");
	buff.Replace(" ","");
	buff.Replace("\t","");
	char *pStr=new char[buff.GetLength()+1];
 	strcpy(pStr,buff);
	int nn=strlen(pStr);
	char *argv[100];
	int argc=GetCmdLine(pStr,argv,100,",");
	long pl[100]={0};
	for(int i=0;i<argc;i++)
		pl[i]=atol(argv[i]);
	TDEL(pStr);
	i=0;
	m_CloseBtRect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	i+=4;

	m_HelpURLRect.SetRect(pl[0]-250,pl[1],pl[0]-10,pl[1]+20);
	
	m_Bt3Rect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	i+=4;
	m_Bt4Rect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	i+=4;
	m_Bt5Rect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	i+=4;
	
	m_Static0Rect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	i+=4;
	m_comboboxRect2.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]+300);
	m_comboboxRect1.left=pl[i+0];
	m_comboboxRect1.top=pl[i+1];
	i+=4;
	m_comboboxRect_yyb.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]+300);
	m_comboboxRect1.right=pl[i+0]+pl[i+2];
	m_comboboxRect1.bottom=pl[i+1]+pl[i+3]+300;
	i+=4;
	
	m_Static1Rect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	i+=4;
	m_edit0Rect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	m_edit0Rect.bottom+=150;
	i+=4;
	m_CheckSaveUsernameRect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	m_CheckProtectUsernameRect.SetRect(pl[i+0],pl[i+1]-28,pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]-28);
	i+=4;
	m_StaticSaveUsername.SetRect(CRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]));
	m_StaticProtectUsername.SetRect(CRect(pl[i+0],pl[i+1]-28,pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]-28));
	i+=4;
	
	m_Static2Rect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	i+=4;
	m_edit1Rect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	if(!g_bClassXP)
		m_edit1Rect.DeflateRect(1,1,1,1);
	i+=4;
	m_Bt8Rect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	i+=4;

	m_Static3.SetRect(CRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]));
	i+=4;
	m_comboboxRect3.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]+300);

	i+=4;
	m_edit2Rect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	if(!g_bClassXP)
		m_edit2Rect.DeflateRect(1,1,1,1);

	m_yzmrect.left=pl[i+0]+pl[i+2]+4;
	m_yzmrect.top=pl[i+1];
	m_yzmrect.right=pl[i+0]+pl[i+2]+60;
	m_yzmrect.bottom=pl[i+1]+pl[i+3];
	m_L2CfgRect=m_yzmrect;
	m_L2CfgRect.top+=m_yzmrect.Height()+14;
	m_L2CfgRect.bottom+=m_yzmrect.Height()+14;
	m_L2CfgRect.right+=30;
	m_L2CfgRect.left+=g_OEM.nL2Cfg_Shift_left;
	m_L2CfgRect.right+=g_OEM.nL2Cfg_Shift_right;
	m_L2CfgRect.top+=g_OEM.nL2Cfg_Shift_top;
	m_L2CfgRect.bottom+=g_OEM.nL2Cfg_Shift_bottom;

	i+=4;
	m_Bt9Rect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	i+=4;
	
	m_Bt6Rect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	i+=4;
	m_BtOffLineRect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	i+=4;
	m_Bt7Rect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	i+=4;
	
	m_Bt0Rect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	i+=4;
	m_Bt1Rect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	i+=4;
	m_Bt2Rect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
	i+=4;
	
	m_StatusStaticRect.SetRect(pl[i+0],pl[i+1],pl[i+0]+pl[i+2],pl[i+1]+pl[i+3]);
			
	m_Static0.SetRect(m_Static0Rect);
	m_Static1.SetRect(m_Static1Rect);
	m_Static2.SetRect(m_Static2Rect);
	m_StaticStatus.SetRect(m_StatusStaticRect);
	m_Yzm_Static.SetRect(m_yzmrect);
	m_L2Cfg_Static.SetRect(m_L2CfgRect);

	if(strlen(g_Url.LogoHelpStr)==0)
		m_HelpURLRect.SetRect(0,0,0,0);
	m_HelpURL_Static.SetRect(m_HelpURLRect);
}

void DrawInterface::OnEraseBkgnd(CDC *pDC,CRect &rc)
{
	CDC memdc;
	CRect temprc;
	CBitmap membm;
	memdc.CreateCompatibleDC(pDC);
	membm.CreateCompatibleBitmap(pDC,rc.Width(),rc.Height());
	CBitmap *poldbm=memdc.SelectObject(&membm);

	g_GUI.DrawXPBmp(&memdc,&m_bmWnd,CPoint(0,0),CPoint(0,0));
	
	memdc.SetBkMode(TRANSPARENT);
	CFont static_font,small_font,*poldfont;
	if(g_OEM.bMediumFontInLogo)
		static_font.CreateFont(13,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,SONG_FONT);
	else
		static_font.CreateFont(14,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,SONG_FONT);
	small_font.CreateFont(12,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,SONG_FONT);
	poldfont=memdc.SelectObject(&static_font);
	if(g_OEM.bStatusInfoLeftAlign)
		m_StaticStatus.DrawText(&memdc,DT_VCENTER|DT_SINGLELINE,m_StaticStatus.m_cor_text,12,SONG_FONT);
	else
		m_StaticStatus.DrawText(&memdc,DT_CENTER|DT_VCENTER|DT_SINGLELINE,m_StaticStatus.m_cor_text,12,SONG_FONT);
	if(!l_bHideJYControl)
	{
		m_Static0.DrawText(&memdc,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
		m_Static1.DrawText(&memdc,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
		m_Static2.DrawText(&memdc,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
		m_Static3.DrawText(&memdc,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
		m_StaticSaveUsername.DrawText(&memdc,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		m_StaticProtectUsername.DrawText(&memdc,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		m_Yzm_Static.DrawText(&memdc,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
		temprc=m_edit1Rect;
		temprc.InflateRect(1,1,1,1);
		memdc.FrameRect(temprc,&CBrush(RGB(152,152,152)));
		temprc=m_edit2Rect;
		temprc.InflateRect(1,1,1,1);
		memdc.FrameRect(temprc,&CBrush(RGB(152,152,152)));
	}
	poldfont=memdc.SelectObject(&small_font);
	m_L2Cfg_Static.DrawText(&memdc,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
	m_HelpURL_Static.DrawText(&memdc,DT_RIGHT|DT_VCENTER|DT_SINGLELINE);
	pDC->BitBlt(0,0,rc.Width(),rc.Height(),&memdc,0,0,SRCCOPY);
	memdc.SelectObject(poldbm);
	memdc.SelectObject(poldfont);
	membm.DeleteObject();
	static_font.DeleteObject();
	small_font.DeleteObject();
	memdc.DeleteDC();
}

void CLogoDlg::OnRegExitMessage(WPARAM wParam,LPARAM lParam)
{
	PostMessage(WM_CLOSE);
}

void CLogoDlg::OnRegLoginMessage(WPARAM wParam,LPARAM lParam)
{
	PostMessage(WM_COMMAND, IDOK, 0);
}



















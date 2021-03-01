// DSConnectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "DSConnectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDSConnectDlg dialog


CDSConnectDlg::CDSConnectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDSConnectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDSConnectDlg)
	//}}AFX_DATA_INIT
	m_bFirstShow = TRUE;
	m_bAtOnce = FALSE;

	m_hIcon = AfxGetApp()->LoadIcon(IDI_DS);
	m_nMainID	= 0;
	m_bHasPackTimer3 = FALSE;
	m_bTestNLB = FALSE;
	m_bNLB = TRUE;
	m_pHost = NULL;
}


void CDSConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDSConnectDlg)
	DDX_Control(pDX, IDC_STATUS_STATIC, m_Status_Static);
	DDX_Control(pDX, IDC_LOGO_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_CONTENT_EDIT, m_RichEditEx);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDSConnectDlg, CDialog)
	//{{AFX_MSG_MAP(CDSConnectDlg)
	ON_MESSAGE(UM_NETWORK_ERR,OnNetworkErr)
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataACK)
	ON_MESSAGE(UM_CONNECT_SUCCESS,OnConnectResult)
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_CONNECTCFG_BUTTON, OnConnectcfgButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDSConnectDlg message handlers

BOOL CDSConnectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon,TRUE);
	SetIcon(m_hIcon,FALSE);

	CString strLicense=HomePath+"dsys.dat";

	//取得实时扩展行情的地址集
	if(g_GC.bIsSSDS)
	{
		strLicense = HomePath+"dsss.dat";
		BOOL bRet1=GetFromComteFile(COMTE_DS_HOST,FALSE,FALSE,COMTE_PASSWORD);
		if(!bRet1 || g_SSDSHost.HostNum<1)
			GetFromComteFile(COMTE_DS_HOST,FALSE,TRUE,COMTE_PASSWORD);
		m_pHost = &g_SSDSHost;
		m_bNLB = TRUE;
	}
	else
	{
		m_pHost = &g_DSHost;
		m_bNLB = g_bNLB;
	}

	CString tmpHintInfo;
	tmpHintInfo.Format("连接%s主站",g_DS.DSOEMName);
	SetWindowText(_F(tmpHintInfo));

	unsigned mask = ::SendMessage(m_RichEditEx.m_hWnd, EM_GETEVENTMASK, 0, 0);
	::SendMessage(m_RichEditEx.m_hWnd, EM_SETEVENTMASK, 0, mask | ENM_LINK);
	::SendMessage(m_RichEditEx.m_hWnd, EM_AUTOURLDETECT, true, 0);  //自动检测URL
	m_RichEditEx.SetBackgroundColor(FALSE,RGB(240,250,250));
	
	//显示Licence文件内容，如果没内容，则有标准语句
	CString tmpLine;
	CFile WarnFile;
	char *pBuf = NULL;
	if(WarnFile.Open(strLicense,CFile::modeRead|CFile::shareDenyNone))
	{
		long filelen = WarnFile.GetLength();
		pBuf = new char[filelen+1];
		WarnFile.Read(pBuf,filelen);
		pBuf[filelen] = 0;
		_FL(pBuf);
		GetDlgItem(IDC_CONTENT_EDIT)->SetWindowText(pBuf);
		delete pBuf;
		WarnFile.Close();
	}
	else
	{
		tmpLine.Format("\r\n  欢迎使用%s系统！请点击[连接主站]连接服务器，在通讯设置中可以设置网络或选择主站。",g_DS.DSOEMName);
		GetDlgItem(IDC_CONTENT_EDIT)->SetWindowText(_F(tmpLine));
	}
	tmpLine.Format("欢迎使用%s系统.",g_DS.DSOEMName);
	_SetStatusTxt(tmpLine);

	return TRUE;
}

void CDSConnectDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	if(m_bFirstShow && bShow)
	{
		if(m_bAtOnce)
			PostMessage(WM_COMMAND,IDOK);	
		m_bFirstShow=FALSE;
	}
}

void CDSConnectDlg::OnCancel() 
{
	g_pComm3->Disconnect();	//断开连接

	CDialog::OnCancel();
}

void CDSConnectDlg::OnOK() 
{
	if(g_bHasDS && !DSOnLine) //如果是负载均衡,则要先探索各主站的信息,重连时第一次才查找
	{
		if(m_pHost->HostNum>1 && (m_bNLB || m_bTestNLB) )
		{
			_SetStatusTxt("正在检测服务器...");
			long Which_Minweight=-1;
			if(g_GC.bIsSSDS)
				Which_Minweight = CHQComm_NLB::NLB_GetMinweight(SSDSHQ_NLB);
			else
				Which_Minweight = CHQComm_NLB::NLB_GetMinweight(DSHQ_NLB);
			if(Which_Minweight >= 0)
			{
				m_pHost->PrimaryHost = Which_Minweight;
				if(!g_GC.bIsSSDS)
					WritePrivateProfileString("DSHOST","PrimaryHost",IntToStr(m_pHost->PrimaryHost),g_WSXHStr+g_strYxhjCfg);
			}
		}
		//开始处理扩展行情
		g_DSHQIPStr = m_pHost->IPAddress[m_pHost->PrimaryHost];
		g_DSHQNameStr = m_pHost->HostName[m_pHost->PrimaryHost];
		g_DSHQPort = m_pHost->Port[m_pHost->PrimaryHost];		
		g_pComm3->SetServerProperties(g_DSHQIPStr,g_DSHQPort);
		_SetStatusTxt("正在连接服务器,请稍等...");
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		SetTimer(0x200,g_nLogoTimeOut*1000,NULL);	//连接过程的超时
		if(!g_pComm3->Connect()) 
		{
			KillTimer(0x200);
			if(m_bTestNLB) 
				_SetStatusTxt("不能连接到服务器!");	//检测后还连接不上
			else
			{
				m_bTestNLB = TRUE;
				OnOK();
			}
		}
	}
}

void CDSConnectDlg::OnNetworkErr(WPARAM wParam,LPARAM lParam)
{
	g_pComm3->Disconnect();	//断开连接
	_SetStatusTxt("连接被断开!");	
	GetDlgItem(IDOK)->EnableWindow(TRUE);
}

void CDSConnectDlg::OnGetDataACK(WPARAM wParam,LPARAM lParam)
{
	char pInfo[255]={0};
	BOOL bOK=OnDSLoginGetDataACK(this,DSCONNECT_WIN,m_bHasPackTimer3,m_nMainID,&m_Progress,pInfo);
	if(strlen(pInfo))
		_SetStatusTxt(pInfo);
	if(bOK)
	{
		//登录成功过
		g_AntiHack.bDSLogo = TRUE;
		//		
		CDialog::OnOK();
	}
}

void CDSConnectDlg::OnConnectResult(WPARAM wParam,LPARAM lParam)
{
	KillTimer(0x200);
	BOOL bIsComm3=(lParam==1);
	if(!bIsComm3) return;
	if(!AsynConnectResult(TRUE,wParam))
	{
		g_pComm3->Disconnect();	//断开连接
		_SetStatusTxt(g_pComm3->GetErrorStr());
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		return;
	}
	else
	{
		g_pComm3->StartThread();
		g_SysInfo.ConnectDSHQ_LastYmd=GetYMD();
		g_SysInfo.ConnectDSHQ_LastHms=GetHMS();
		
		SetTimer(0x201,g_nLogoTimeOut*1000,NULL); //第一数据包的超时,防止有些假连接
		m_bHasPackTimer3=TRUE;
		//开始请求数据
		g_nMainID = MainIDPlus(m_nMainID,DSCONNECT_WIN);
		g_nAssisID= DS_LOGIN_ASSISID;
		_SetStatusTxt("请求扩展行情服务...");
		g_pGetData->DSLogin();
	}
}

void CDSConnectDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==0x200 || nIDEvent==0x201)
	{
		if(nIDEvent==0x200)
			KillTimer(0x200);
		else
		{
			KillTimer(0x201);
			m_bHasPackTimer3=FALSE;
		}
		g_pComm3->Disconnect();	//断开连接
		_SetStatusTxt("连接超时!");
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		if(m_pHost->HostNum>1)
		{
			if(TDX_MessageBox (m_hWnd,"连接超时,重新检测新的可用主站吗?",MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL)
			{
				GetDlgItem(IDOK)->EnableWindow(TRUE);
				return;
			}
			m_bTestNLB = TRUE;
			OnOK();
		}
	}
}

#include "ConnectCfgDlg.h"
void CDSConnectDlg::OnConnectcfgButton() 
{
	g_pComm3->Disconnect();	//断开连接
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	_SetStatusTxt("通讯设置,就绪.");

	CConnectCfgDlg dlg;
#ifdef OEM_NEWJY
	dlg.m_bHasWTSet=TRUE;
#endif
	dlg.DoModal();
}

void CDSConnectDlg::_SetStatusTxt(LPCSTR str)
{
	m_Status_Static.SetText(_F(str));
}

extern void ProcessRichEditExLink(CRichEditCtrl *pCtrl,NMHDR*  in_pNotifyHeader,LRESULT* out_pResult);
void CDSConnectDlg::OnRichEditExLink(NMHDR*  in_pNotifyHeader,LRESULT* out_pResult)
{
	ProcessRichEditExLink(&m_RichEditEx,in_pNotifyHeader,out_pResult);
}

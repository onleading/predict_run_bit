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

	//ȡ��ʵʱ��չ����ĵ�ַ��
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
	tmpHintInfo.Format("����%s��վ",g_DS.DSOEMName);
	SetWindowText(_F(tmpHintInfo));

	unsigned mask = ::SendMessage(m_RichEditEx.m_hWnd, EM_GETEVENTMASK, 0, 0);
	::SendMessage(m_RichEditEx.m_hWnd, EM_SETEVENTMASK, 0, mask | ENM_LINK);
	::SendMessage(m_RichEditEx.m_hWnd, EM_AUTOURLDETECT, true, 0);  //�Զ����URL
	m_RichEditEx.SetBackgroundColor(FALSE,RGB(240,250,250));
	
	//��ʾLicence�ļ����ݣ����û���ݣ����б�׼���
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
		tmpLine.Format("\r\n  ��ӭʹ��%sϵͳ������[������վ]���ӷ���������ͨѶ�����п������������ѡ����վ��",g_DS.DSOEMName);
		GetDlgItem(IDC_CONTENT_EDIT)->SetWindowText(_F(tmpLine));
	}
	tmpLine.Format("��ӭʹ��%sϵͳ.",g_DS.DSOEMName);
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
	g_pComm3->Disconnect();	//�Ͽ�����

	CDialog::OnCancel();
}

void CDSConnectDlg::OnOK() 
{
	if(g_bHasDS && !DSOnLine) //����Ǹ��ؾ���,��Ҫ��̽������վ����Ϣ,����ʱ��һ�βŲ���
	{
		if(m_pHost->HostNum>1 && (m_bNLB || m_bTestNLB) )
		{
			_SetStatusTxt("���ڼ�������...");
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
		//��ʼ������չ����
		g_DSHQIPStr = m_pHost->IPAddress[m_pHost->PrimaryHost];
		g_DSHQNameStr = m_pHost->HostName[m_pHost->PrimaryHost];
		g_DSHQPort = m_pHost->Port[m_pHost->PrimaryHost];		
		g_pComm3->SetServerProperties(g_DSHQIPStr,g_DSHQPort);
		_SetStatusTxt("�������ӷ�����,���Ե�...");
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		SetTimer(0x200,g_nLogoTimeOut*1000,NULL);	//���ӹ��̵ĳ�ʱ
		if(!g_pComm3->Connect()) 
		{
			KillTimer(0x200);
			if(m_bTestNLB) 
				_SetStatusTxt("�������ӵ�������!");	//�������Ӳ���
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
	g_pComm3->Disconnect();	//�Ͽ�����
	_SetStatusTxt("���ӱ��Ͽ�!");	
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
		//��¼�ɹ���
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
		g_pComm3->Disconnect();	//�Ͽ�����
		_SetStatusTxt(g_pComm3->GetErrorStr());
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		return;
	}
	else
	{
		g_pComm3->StartThread();
		g_SysInfo.ConnectDSHQ_LastYmd=GetYMD();
		g_SysInfo.ConnectDSHQ_LastHms=GetHMS();
		
		SetTimer(0x201,g_nLogoTimeOut*1000,NULL); //��һ���ݰ��ĳ�ʱ,��ֹ��Щ������
		m_bHasPackTimer3=TRUE;
		//��ʼ��������
		g_nMainID = MainIDPlus(m_nMainID,DSCONNECT_WIN);
		g_nAssisID= DS_LOGIN_ASSISID;
		_SetStatusTxt("������չ�������...");
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
		g_pComm3->Disconnect();	//�Ͽ�����
		_SetStatusTxt("���ӳ�ʱ!");
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		if(m_pHost->HostNum>1)
		{
			if(TDX_MessageBox (m_hWnd,"���ӳ�ʱ,���¼���µĿ�����վ��?",MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL)
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
	g_pComm3->Disconnect();	//�Ͽ�����
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	_SetStatusTxt("ͨѶ����,����.");

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

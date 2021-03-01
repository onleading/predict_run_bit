// ustart.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ustart.h"

#include "U_SelReg.h"
#include "U_TestZjzh.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUStart dialog


CUStart::CUStart(CWnd* pParent /*=NULL*/)
	: CDialog(CUStart::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUStart)
	//}}AFX_DATA_INIT
}


void CUStart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUStart)
	DDX_Control(pDX, IDC_HINT_STATIC, m_Hint_Status);
	DDX_Control(pDX, IDC_STATUS_STATIC, m_Status_Static);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUStart, CDialog)
	//{{AFX_MSG_MAP(CUStart)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUStart message handlers

BOOL CUStart::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Hint_Status.SetTextColor(RGB(200,0,0));
	m_Status_Static.SetTextColor(RGB(0,0,255));		
	

	return TRUE;
}

void CUStart::OnOK() 
{
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	g_pUserComm->SetServerProperties(&g_ProxyInfo,g_UserHostIP,g_nUserHostPort,g_BakUserHostIP,g_nBakUserHostPort,g_BakUserHostIP2,g_nBakUserHostPort2);
	BOOL bRet = RegUser();
	g_pUserComm->Disconnect();	
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	if(bRet)
		CDialog::OnOK();
}

BOOL CUStart::RegUser()
{
	int TestRet;
	char ret_zjzh[20];
	CU_TestZjzh dlg;
	if(dlg.DoModal()==IDOK)
	{
		m_Status_Static.SetText(_F("���ڳ�ʼ����ȫ����..."));
		if(g_pUserComm->TestZjzh2(dlg.m_strYYBInfo,dlg.m_WTTypeID,dlg.m_strZjzh,dlg.m_strPassWord,TestRet,ret_zjzh)==0 || TestRet!=1)
		{
			m_Status_Static.SetText(g_pUserComm->GetErrorStr());
			return FALSE;
		}
		else
		{
			m_Status_Static.SetText(_F("У��ɹ�."));
			CString	l_strZjzh=ret_zjzh;
			if(strlen(ret_zjzh)==0)
				l_strZjzh = dlg.m_strZjzh;
			CString	l_strYYBInfo=dlg.m_strYYBInfo;

			while(1) 
			{
				CU_SelReg dlg2;
				if(dlg2.DoModal()==IDOK)
				{
					int RegisterRet;
					m_Status_Static.SetText(_F("����ע���û�..."));
					if(g_pUserComm->RegisterUser(dlg2.m_nRegisterType,l_strYYBInfo,l_strZjzh,dlg2.m_strUserName,dlg2.m_strPassword,0,RegisterRet)==0 || RegisterRet!=1)
					{
						BOOL bAnsError=TRUE; //-4:��ע����һ���û�,��ע�����û���,�������û�
						if(RegisterRet==-4 && TDX_MessageBox(m_hWnd,"�ý����ʺ��Ѿ�ע����һ���û�,\n��ע�����û���,����ע����û������ܵ�¼��ϵͳ.\n�Ƿ�����ע��?",MB_YESNO|MB_ICONINFORMATION|MB_DEFBUTTON2)==IDYES)
						{
							if(g_pUserComm->RegisterUser(dlg2.m_nRegisterType,l_strYYBInfo,l_strZjzh,dlg2.m_strUserName,dlg2.m_strPassword,1,RegisterRet)!=0 && RegisterRet==1)
								bAnsError=FALSE;
						}
						if(bAnsError)
						{
							m_Status_Static.SetText(g_pUserComm->GetErrorStr());
							TDX_MessageBox(m_hWnd,g_pUserComm->GetErrorStr(),MB_OK|MB_ICONERROR);
							continue;
						}
					}
					m_Status_Static.SetText(_F("ע���û��ɹ�.�����ڿ���������վ��."));
					//���ע��ɹ�
					m_strUserName=dlg2.m_strUserName;
					m_strPassword=dlg2.m_strPassword;
					strcpy(g_strLoginName,dlg2.m_strUserName);
					strcpy(g_strPassword,dlg2.m_strPassword);
					UpdateData(FALSE);
					return TRUE;
				}
				else
					break;
			};
			m_Status_Static.SetText(_F("ע�ᱻȡ��."));
			return FALSE;
		}
	}
	else
		m_Status_Static.SetText(_F("ע�ᱻȡ��."));
	return FALSE;
}

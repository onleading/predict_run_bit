// EditServerDlg.cpp : implementation file

#include "stdafx.h"
#include "tdxw.h"
#include "EditServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditServerDlg dialog


CEditServerDlg::CEditServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditServerDlg)
	m_strServerName = _T("");
	m_nPort = 0;
	//}}AFX_DATA_INIT
	m_bAdd = FALSE;
	m_nType = 0;
}


void CEditServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditServerDlg)
	DDX_Control(pDX, IDC_IPADDRESS1, m_ctrIPAddress);
	DDX_Text(pDX, IDC_SERVER_NAME, m_strServerName);
	DDV_MaxChars(pDX, m_strServerName, 100);
	DDX_Text(pDX, IDC_PORT, m_nPort);
	DDV_MinMaxUInt(pDX, m_nPort, 1, 65535);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditServerDlg, CDialog)
	//{{AFX_MSG_MAP(CEditServerDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditServerDlg message handlers

BOOL CEditServerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(m_bAdd)
	{
		if(m_nType==3) 		m_nPort=7711;
		else if(m_nType==2)	m_nPort=7708;
		else if(m_nType==4)	m_nPort=7721;
		else if(m_nType==5)	m_nPort=7709;
		else				m_nPort=7709;
		SetWindowText(_F("�����վ"));
	}
	else
	{
		SetWindowText(_F("�޸���վ"));
		m_ctrIPAddress.SetWindowText(_F(m_strIPAddress));
	}
	if(m_nType == 1)
	{
		SetDlgItemText(IDC_NAME_STATIC,_F("������վ����:"));
		SetDlgItemText(IDC_IP_STATIC,_F("������վ��ַ:"));
	}
	else if(m_nType == 2)
	{
		SetDlgItemText(IDC_NAME_STATIC,_F("������������:"));
		SetDlgItemText(IDC_IP_STATIC,_F("�������ĵ�ַ:"));
	}
	else if(m_nType == 3)
	{
		SetDlgItemText(IDC_NAME_STATIC,_F("��Ѷ��վ����:"));
		SetDlgItemText(IDC_IP_STATIC,_F("��Ѷ��վ��ַ:"));
	}
	else if(m_nType == 4)
	{
		CString tmpHintInfo;
		tmpHintInfo.Format("%s��վ����:",g_DS.DSOEMName);
		SetDlgItemText(IDC_NAME_STATIC,_F(tmpHintInfo));
		tmpHintInfo.Format("%s��վ��ַ:",g_DS.DSOEMName);
		SetDlgItemText(IDC_IP_STATIC,_F(tmpHintInfo));
	}
	else if(m_nType == 5)
	{
		SetDlgItemText(IDC_NAME_STATIC,_F("Level2��վ����:"));
		SetDlgItemText(IDC_IP_STATIC,_F("Level2��վ��ַ:"));
	}

	UpdateData(FALSE);
	return TRUE;
}

void CEditServerDlg::OnOK() 
{
	UpdateData(TRUE);
	m_ctrIPAddress.GetWindowText(m_strIPAddress);
	if(m_strIPAddress.IsEmpty())
	{
		TDX_MessageBox(m_hWnd,"��վ��ַ����Ϊ��!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	TrimString(m_strServerName);
	if(m_strServerName.IsEmpty())
	{
		TDX_MessageBox(m_hWnd,"��վ���Ʋ���Ϊ��!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	CDialog::OnOK();
}

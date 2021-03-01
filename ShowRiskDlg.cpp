// ShowRisktDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ShowRiskDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowRisktDlg dialog


CShowRisktDlg::CShowRisktDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShowRisktDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CShowRisktDlg)
	m_strContent = _T("");
	m_bRiskCheck = g_GH.bCheckRisk;
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CShowRisktDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShowRisktDlg)
	DDX_Control(pDX, IDC_CONTENT_EDIT, m_Content_Edit);
	DDX_Check(pDX, IDC_CHECK, m_bRiskCheck);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShowRisktDlg, CDialog)
	//{{AFX_MSG_MAP(CShowRisktDlg)
	ON_BN_CLICKED(IDC_CHECK, OnCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowRisktDlg message handlers

BOOL CShowRisktDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetIcon(m_hIcon,TRUE);
	SetIcon(m_hIcon,FALSE);
	
	unsigned mask = ::SendMessage(m_Content_Edit.m_hWnd, EM_GETEVENTMASK, 0, 0);
	::SendMessage(m_Content_Edit.m_hWnd, EM_SETEVENTMASK, 0, mask | ENM_LINK);
	::SendMessage(m_Content_Edit.m_hWnd, EM_AUTOURLDETECT, true, 0);  //×Ô¶¯¼ì²âURL
	
	CString tmpLine;
	CFile WarnFile;
	char *pBuf = NULL;
	if(WarnFile.Open(HomePath+"declare.txt",CFile::modeRead|CFile::shareDenyNone))
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

	GetDlgItem(IDOK)->EnableWindow(m_bRiskCheck);

	UpdateData(FALSE);
	return TRUE;
}

extern void ProcessRichEditExLink(CRichEditCtrl *pCtrl,NMHDR*  in_pNotifyHeader,LRESULT* out_pResult);
void CShowRisktDlg::OnRichEditExLink(NMHDR*  in_pNotifyHeader,LRESULT* out_pResult)
{
	ProcessRichEditExLink(&m_Content_Edit,in_pNotifyHeader,out_pResult);
}


void CShowRisktDlg::OnOK() 
{
	UpdateData(TRUE);
	g_GH.bCheckRisk =  m_bRiskCheck;
	CDialog::OnOK();
}

void CShowRisktDlg::OnCheck() 
{
	UpdateData(TRUE);
	GetDlgItem(IDOK)->EnableWindow(m_bRiskCheck);
}

// KQSCfgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "KQSCfgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKQSCfgDlg dialog


CKQSCfgDlg::CKQSCfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKQSCfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKQSCfgDlg)
	m_b10 = g_GH.bKQS_Use10;
	m_bRelation = g_GH.bKQS_RelationGP;
	//}}AFX_DATA_INIT
}


void CKQSCfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKQSCfgDlg)
	DDX_Radio(pDX, IDC_RADIO1, m_b10);
	DDX_Check(pDX, IDC_RELATION_CHECK, m_bRelation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKQSCfgDlg, CDialog)
	//{{AFX_MSG_MAP(CKQSCfgDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKQSCfgDlg message handlers

BOOL CKQSCfgDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_RELATION_CHECK)->EnableWindow(!m_b10);

	UpdateData(FALSE);
	return TRUE;
}

void CKQSCfgDlg::OnOK() 
{
	UpdateData(TRUE);
	g_GH.bKQS_Use10 = m_b10;
	g_GH.bKQS_RelationGP = m_bRelation;

	//½«Ï°¹ßÐ´ÅÌ
	CString	Ini = g_WSXHStr+g_strUserCfg;
	WritePrivateProfileString("Other","KQS_Use10",g_GH.bKQS_Use10?"1":"0",Ini);
	WritePrivateProfileString("Other","KQS_RelationGP",g_GH.bKQS_RelationGP?"1":"0",Ini);

	CDialog::OnOK();
}

void CKQSCfgDlg::OnRadio1() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_RELATION_CHECK)->EnableWindow(!m_b10);
}


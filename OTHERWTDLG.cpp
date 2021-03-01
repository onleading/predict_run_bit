// OTHERWTDLG.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "OtherWTDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COtherWTDlg dialog


COtherWTDlg::COtherWTDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COtherWTDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COtherWTDlg)
	m_strWtPath = _T("");
	m_strWtName = _T("");
	//}}AFX_DATA_INIT
}


void COtherWTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COtherWTDlg)
	DDX_Text(pDX, IDC_EDIT2, m_strWtPath);
	DDV_MaxChars(pDX, m_strWtPath, 255);
	DDX_Text(pDX, IDC_EDIT1, m_strWtName);
	DDV_MaxChars(pDX, m_strWtName, 60);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COtherWTDlg, CDialog)
	//{{AFX_MSG_MAP(COtherWTDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COtherWTDlg message handlers

void COtherWTDlg::OnButton1() 
{
	UpdateData(TRUE);
	CFileDialog FileDialog(TRUE,NULL,NULL,
		OFN_NOCHANGEDIR,
		_F("委托下单程序(*.exe)|*.exe||"));
	if (FileDialog.DoModal() == IDOK)
	{
		m_strWtPath = FileDialog.GetPathName();
		UpdateData(FALSE);
	}
}

void COtherWTDlg::OnOK() 
{
	UpdateData(TRUE);
	m_strWtName.TrimLeft();
	m_strWtName.TrimRight();
	m_strWtPath.TrimLeft();
	m_strWtPath.TrimRight();
	if(m_strWtPath=="" || m_strWtName=="")
	{
		TDX_MessageBox(m_hWnd,"描述和路径不能为空!",MB_ICONEXCLAMATION|MB_OK);
		return;
	}
	CDialog::OnOK();
}

BOOL COtherWTDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_title);
	UpdateData(FALSE);
	return TRUE;
}

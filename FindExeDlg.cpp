// FindWtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "FindExeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindExeDlg dialog


CFindExeDlg::CFindExeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindExeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindExeDlg)
	m_strExePath = _T("");
	m_strExeInfo = _T("");
	m_strCaption = _F("程序路径");
	//}}AFX_DATA_INIT
}


void CFindExeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindExeDlg)
	DDX_Text(pDX, IDC_PATH_EDIT, m_strExePath);
	DDV_MaxChars(pDX, m_strExePath, 255);
	DDX_Text(pDX, IDC_EXEINFO_STATIC, m_strExeInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindExeDlg, CDialog)
	//{{AFX_MSG_MAP(CFindExeDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindExeDlg message handlers

void CFindExeDlg::OnBrowse() 
{
	UpdateData(TRUE);
	CFileDialog FileDialog(TRUE,NULL,NULL,
							OFN_NOCHANGEDIR,
							_F("外挂程序(*.exe)|*.exe||"));
	if (FileDialog.DoModal() == IDOK)
	{
		m_strExePath = FileDialog.GetPathName();
		UpdateData(FALSE);
	}
}

BOOL CFindExeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_strCaption);
	
	return TRUE;
}

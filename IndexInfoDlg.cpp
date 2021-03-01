// IndexInfoDlg.cpp : implementation file
//
#include "stdafx.h"
#include "tdxw.h"
#include "IndexInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIndexInfoDlg dialog


CIndexInfoDlg::CIndexInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIndexInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIndexInfoDlg)
	m_IndexInfo = _T("");
	//}}AFX_DATA_INIT
}

void CIndexInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIndexInfoDlg)
	DDX_Text(pDX, IDC_INDEXINFO_CONTENT, m_IndexInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIndexInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CIndexInfoDlg)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIndexInfoDlg message handlers

BOOL CIndexInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDOK)->ShowWindow(HasOKButton);
	((CEdit *)GetDlgItem(IDC_INDEXINFO_CONTENT))->SetReadOnly(bReadOnly);

	SetWindowText(_F(TitleStr));

	
	return TRUE;
}

HBRUSH CIndexInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if (pWnd->GetDlgCtrlID() == IDC_INDEXINFO_CONTENT)
		return (HBRUSH)GetStockObject(WHITE_BRUSH);

	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

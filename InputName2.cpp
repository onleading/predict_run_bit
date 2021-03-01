// InputName.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "InputName2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputName2 dialog


CInputName2::CInputName2(CWnd* pParent /*=NULL*/)
	: CDialog(CInputName2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputName2)
	m_NewName = _T("");
	//}}AFX_DATA_INIT
	m_bHasBrowse = FALSE;
}


void CInputName2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputName2)
	DDX_Control(pDX, IDC_NEW_NAME_EDIT, m_NewNameEdit);
	DDX_Text(pDX, IDC_NEW_NAME_EDIT, m_NewName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputName2, CDialog)
	//{{AFX_MSG_MAP(CInputName2)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputName2 message handlers

BOOL CInputName2::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(_F(Caption));
	SetDlgItemText(IDC_HINT_INFO,_F(HintInfo));
	GetDlgItem(IDC_BROWSE)->ShowWindow(m_bHasBrowse?SW_SHOW:SW_HIDE);

	return TRUE;
}


void CInputName2::OnBrowse() 
{
	CFileDialog FileDialog(TRUE,NULL,NULL,
							OFN_NOCHANGEDIR,
							_F(m_strTypeStr));
	if (FileDialog.DoModal() == IDOK)
	{
		m_NewName = FileDialog.GetPathName();
		UpdateData(FALSE);
	}	
}

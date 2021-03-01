// InputName.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "InputName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputName dialog


CInputName::CInputName(CWnd* pParent /*=NULL*/)
	: CDialog(CInputName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInputName)
	m_NewName = _T("");
	//}}AFX_DATA_INIT
}


void CInputName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputName)
	DDX_Control(pDX, IDC_NEW_NAME_EDIT, m_NewNameEdit);
	DDX_Text(pDX, IDC_NEW_NAME_EDIT, m_NewName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInputName, CDialog)
	//{{AFX_MSG_MAP(CInputName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputName message handlers

BOOL CInputName::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(_F(Caption));
	SetDlgItemText(IDC_HINT_INFO,_F(HintInfo));

	return TRUE;
}


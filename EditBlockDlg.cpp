// EditBlockDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "EditBlockDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditBlockDlg dialog


CEditBlockDlg::CEditBlockDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditBlockDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditBlockDlg)
	m_BlockName = _T("");
	m_KeyGuyCode = _T("");
	//}}AFX_DATA_INIT
	IsAdd = FALSE;
	m_editKeyGuy.setPermitStyle(CEditEx::PMTSTY_FILENAME);
}


void CEditBlockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditBlockDlg)
	DDX_Control(pDX, IDC_KEYGUYCODE_EDIT, m_editKeyGuy);
	DDX_Text(pDX, IDC_BLOCKNAME_EDIT, m_BlockName);
	DDV_MaxChars(pDX, m_BlockName, 8);
	DDX_Text(pDX, IDC_KEYGUYCODE_EDIT, m_KeyGuyCode);
	DDV_MaxChars(pDX, m_KeyGuyCode, 4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditBlockDlg, CDialog)
	//{{AFX_MSG_MAP(CEditBlockDlg)
	ON_EN_CHANGE(IDC_BLOCKNAME_EDIT, OnChangeBlocknameEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditBlockDlg message handlers

BOOL CEditBlockDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(IsAdd) SetWindowText(_F("新建板块"));
	else	  SetWindowText(_F("板块改名"));

		
	return TRUE;
}

void CEditBlockDlg::OnOK()
{
	UpdateData(TRUE);
	TrimString(m_BlockName);
	TrimString(m_KeyGuyCode);
	if(m_BlockName.GetLength()==0)
	{
		TDX_MessageBox(m_hWnd,"请输入板块名称!",MB_OK|MB_ICONEXCLAMATION);
		GetDlgItem(IDC_BLOCKNAME_EDIT)->SetFocus();
		return;
	}
	if(m_KeyGuyCode.GetLength()==0)
	{
		TDX_MessageBox(m_hWnd,"请输入板块简称!建议用英文字母",MB_OK|MB_ICONEXCLAMATION);
		GetDlgItem(IDC_KEYGUYCODE_EDIT)->SetFocus();
		return;
	}
	if(m_KeyGuyCode=="self"||m_KeyGuyCode=="calc")
	{
		TDX_MessageBox(m_hWnd,"存在相同的板块简称,请重新输入!",MB_OK|MB_ICONEXCLAMATION);
		GetDlgItem(IDC_KEYGUYCODE_EDIT)->SetFocus();
		return;
	}
	CDialog::OnOK();
}

void CEditBlockDlg::OnChangeBlocknameEdit() 
{
	UpdateData(TRUE);
	m_KeyGuyCode=GetPYStr(m_BlockName,4);
	UpdateData(FALSE);	
}

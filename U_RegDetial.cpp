// U_RegDetial.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "U_RegDetial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CU_RegDetial dialog


CU_RegDetial::CU_RegDetial(CWnd* pParent /*=NULL*/)
	: CDialog(CU_RegDetial::IDD, pParent)
{
	//{{AFX_DATA_INIT(CU_RegDetial)
	m_address = _T("");
	m_birth_time = 0;
	m_handscrpt = _T("");
	m_nickname = _T("");
	m_otherlx = _T("");
	m_phone = _T("");
	m_sex = -1;
	m_truename = _T("");
	//}}AFX_DATA_INIT
}


void CU_RegDetial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CU_RegDetial)
	DDX_Control(pDX, IDC_SEX_COMBO, m_sex_combo);
	DDX_Text(pDX, IDC_ADDRESS, m_address);
	DDV_MaxChars(pDX, m_address, 99);
	DDX_DateTimeCtrl(pDX, IDC_BIRTH_DATETIMEPICKER, m_birth_time);
	DDX_Text(pDX, IDC_HANDSCRPT, m_handscrpt);
	DDV_MaxChars(pDX, m_handscrpt, 99);
	DDX_Text(pDX, IDC_NICKNAME, m_nickname);
	DDV_MaxChars(pDX, m_nickname, 49);
	DDX_Text(pDX, IDC_OTHERLX, m_otherlx);
	DDV_MaxChars(pDX, m_otherlx, 99);
	DDX_Text(pDX, IDC_PHONE, m_phone);
	DDV_MaxChars(pDX, m_phone, 49);
	DDX_CBIndex(pDX, IDC_SEX_COMBO, m_sex);
	DDX_Text(pDX, IDC_TRUENAME, m_truename);
	DDV_MaxChars(pDX, m_truename, 49);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CU_RegDetial, CDialog)
	//{{AFX_MSG_MAP(CU_RegDetial)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CU_RegDetial message handlers

BOOL CU_RegDetial::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetDateCtrlRange(GetDlgItem(IDC_BIRTH_DATETIMEPICKER));	
	m_sex_combo.AddString(_F("男"));
	m_sex_combo.AddString(_F("女"));
	m_sex_combo.AddString(_F("不透露"));
	m_sex_combo.SetCurSel(0);
	
	return TRUE;
}

void CU_RegDetial::OnOK() 
{
	UpdateData(TRUE);
	m_sex=m_sex_combo.GetCurSel();
	m_birth=m_birth_time.GetYear()*10000+m_birth_time.GetMonth()*100+m_birth_time.GetDay();
	TrimString(m_truename);
	if(m_truename.GetLength()<3)
	{
		TDX_MessageBox(m_hWnd,"请填写完整的姓名",MB_OK|MB_ICONEXCLAMATION);
		GetDlgItem(IDC_TRUENAME)->SetFocus();
		return;
	}
	CDialog::OnOK();
}

// EditTxtDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "EditTxtDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditTxtDlg dialog


CEditTxtDlg::CEditTxtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditTxtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditTxtDlg)
	m_strTxt = _T("");
	//}}AFX_DATA_INIT
	m_bAdd = TRUE;
	m_color = g_nTxtColor;
	m_colorbtn.SetColor(m_color);
}


void CEditTxtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditTxtDlg)
	DDX_Control(pDX, IDC_BUTTON_COLOR, m_colorbtn);
	DDX_Text(pDX, IDC_TXT_EDIT, m_strTxt);
	DDV_MaxChars(pDX, m_strTxt, 120);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditTxtDlg, CDialog)
	//{{AFX_MSG_MAP(CEditTxtDlg)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnButtonColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditTxtDlg message handlers

BOOL CEditTxtDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_bAdd?_F("Ìí¼ÓÎÄ×Ö"):_F("±à¼­ÎÄ×Ö"));

	UpdateData(FALSE);

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_FONT))->SetBuddy(GetDlgItem(IDC_EDIT_FONT));
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_FONT))->SetBase(10);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_FONT))->SetRange(1,50);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_FONT))->SetPos(g_nTxtFontSize);

	return TRUE;
}

void CEditTxtDlg::OnButtonColor() 
{
	UpdateData(TRUE);
	CColorDialog CDlg;
	if( CDlg.DoModal() == IDOK )
	{
		m_color = CDlg.GetColor();
		g_nTxtColor = m_color;
		m_colorbtn.SetColor(m_color);
		m_colorbtn.Refresh();
	}
	UpdateData(FALSE);
}

void CEditTxtDlg::OnOK() 
{
	UpdateData(TRUE);
	CString tmpstr;
	GetDlgItem(IDC_EDIT_FONT)->GetWindowText(tmpstr);
	g_nTxtFontSize = atoi(LPCSTR(tmpstr));
	if(g_nTxtFontSize>50) g_nTxtFontSize=50;
	if(g_nTxtFontSize<1) g_nTxtFontSize=1;
	m_strTxt.TrimLeft();
	m_strTxt.TrimRight();
	CDialog::OnOK();
}

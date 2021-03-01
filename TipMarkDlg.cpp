// TipMarkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "TipMarkDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTipMarkDlg dialog


CTipMarkDlg::CTipMarkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTipMarkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTipMarkDlg)
	m_strTip = _T("");
	//}}AFX_DATA_INIT
	m_pStock = NULL;
}


void CTipMarkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTipMarkDlg)
	DDX_Text(pDX, IDC_TIPEDIT, m_strTip);
	DDV_MaxChars(pDX, m_strTip, 999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTipMarkDlg, CDialog)
	//{{AFX_MSG_MAP(CTipMarkDlg)
	ON_BN_CLICKED(ID_DELTIPMARK, OnDeltipmark)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTipMarkDlg message handlers

void CTipMarkDlg::OnOK() 
{
	UpdateData(TRUE);
	m_strTip.TrimLeft();
	m_strTip.TrimRight();
	if(m_strTip.GetLength()<1) return;

	CDialog::OnOK();
}

void CTipMarkDlg::OnDeltipmark() 
{
	if(m_pStock) 
		MarkStockFlag(m_pStock, 0);
	CTipMarkDlg::OnCancel();
}

void CTipMarkDlg::OnCancel() 
{
	CDialog::OnCancel();
}

BOOL CTipMarkDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetToolText(m_strTip, m_pStock, FALSE);
	UpdateData(FALSE);

	CString strTitle;
	strTitle.Format("%s(%s)%s", m_pStock->Name,m_pStock->Code,_F("±ê¼ÇÎÄ×Ö"));
	SetWindowText(strTitle);
		
	CEdit *pEdit=((CEdit *)GetDlgItem(IDC_TIPEDIT));
	pEdit->SetFocus();
	int nTmpLen=m_strTip.GetLength();
	pEdit->SetSel(nTmpLen,nTmpLen);

	return FALSE;
}

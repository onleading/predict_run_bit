// leadup.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "leadup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLeadUp dialog


CLeadUp::CLeadUp(CWnd* pParent /*=NULL*/)
	: CDialog(CLeadUp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLeadUp)
	m_bAsk = FALSE;
	//}}AFX_DATA_INIT
}


void CLeadUp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLeadUp)
	DDX_Check(pDX, IDC_CHECK_MONTH, m_bAsk);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLeadUp, CDialog)
	//{{AFX_MSG_MAP(CLeadUp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeadUp message handlers

void CLeadUp::OnOK() 
{
	UpdateData(TRUE);
	CDialog::OnOK();
}

void CLeadUp::OnCancel() 
{
	UpdateData(TRUE);
	CDialog::OnCancel();
}

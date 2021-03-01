// SetQRParamDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "SetQRParamDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetQRParamDlg dialog


CSetQRParamDlg::CSetQRParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetQRParamDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetQRParamDlg)
	m_QRParam1 = 0;
	m_QRParam2 = 0;
	m_QRParam3 = 0;
	m_QRParam4 = 0;
	m_QRParam5 = 0;
	//}}AFX_DATA_INIT
}


void CSetQRParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetQRParamDlg)
	DDX_Text(pDX, IDC_QRPARAM1, m_QRParam1);
	DDX_Text(pDX, IDC_QRPARAM2, m_QRParam2);
	DDX_Text(pDX, IDC_QRPARAM3, m_QRParam3);
	DDX_Text(pDX, IDC_QRPARAM4, m_QRParam4);
	DDX_Text(pDX, IDC_QRPARAM5, m_QRParam5);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetQRParamDlg, CDialog)
	//{{AFX_MSG_MAP(CSetQRParamDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, OnDeltaposSpin2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN3, OnDeltaposSpin3)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN4, OnDeltaposSpin4)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN5, OnDeltaposSpin5)
	ON_EN_KILLFOCUS(IDC_QRPARAM1, OnKillfocusQrparam1)
	ON_EN_KILLFOCUS(IDC_QRPARAM2, OnKillfocusQrparam2)
	ON_EN_KILLFOCUS(IDC_QRPARAM3, OnKillfocusQrparam3)
	ON_EN_KILLFOCUS(IDC_QRPARAM4, OnKillfocusQrparam4)
	ON_EN_KILLFOCUS(IDC_QRPARAM5, OnKillfocusQrparam5)
	ON_BN_CLICKED(ID_SAVEQRPARAM, OnDefaultqrparam)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetQRParamDlg message handlers

void CSetQRParamDlg::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	UpdateData(TRUE);
	m_QRParam1 -= pNMUpDown->iDelta;
	if(m_QRParam1 > 299)	m_QRParam1 = 299;
	if(m_QRParam1 <= 0)	m_QRParam1 = 1;
	UpdateData(FALSE);
	
	*pResult = 0;
}

void CSetQRParamDlg::OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	UpdateData(TRUE);
	m_QRParam2 -= pNMUpDown->iDelta;
	if(m_QRParam2 > 299)	m_QRParam2 = 299;
	if(m_QRParam2 <= 0)		m_QRParam2 = 1;
	UpdateData(FALSE);
	
	*pResult = 0;
}

void CSetQRParamDlg::OnDeltaposSpin3(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	UpdateData(TRUE);
	m_QRParam3 -= pNMUpDown->iDelta;
	if(m_QRParam3 > 299)	m_QRParam3 = 299;
	if(m_QRParam3 <= 0)		m_QRParam3 = 1;
	UpdateData(FALSE);
	
	*pResult = 0;
}

void CSetQRParamDlg::OnDeltaposSpin4(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	UpdateData(TRUE);
	m_QRParam4 -= pNMUpDown->iDelta;
	if(m_QRParam4 > 299)	m_QRParam4 = 299;
	if(m_QRParam4 <= 0)		m_QRParam4 = 1;
	UpdateData(FALSE);
	
	*pResult = 0;
}

void CSetQRParamDlg::OnDeltaposSpin5(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	UpdateData(TRUE);
	m_QRParam5 -= pNMUpDown->iDelta;
	if(m_QRParam5 > 299)	m_QRParam5 = 299;
	if(m_QRParam5 <= 0)		m_QRParam5 = 1;
	UpdateData(FALSE);

	*pResult = 0;
}

void CSetQRParamDlg::OnKillfocusQrparam1() 
{
	UpdateData(TRUE);
	if(m_QRParam1 > 299)	m_QRParam1 = 299;
	if(m_QRParam1 <= 0)		m_QRParam1 = 1;
	UpdateData(FALSE);
}

void CSetQRParamDlg::OnKillfocusQrparam2() 
{
	UpdateData(TRUE);
	if(m_QRParam2 > 299)	m_QRParam2 = 299;
	if(m_QRParam2 <= 0)		m_QRParam2 = 1;
	UpdateData(FALSE);
}

void CSetQRParamDlg::OnKillfocusQrparam3() 
{
	UpdateData(TRUE);
	if(m_QRParam3 > 299)	m_QRParam3 = 299;
	if(m_QRParam3 <= 0)		m_QRParam3 = 1;
	UpdateData(FALSE);
}

void CSetQRParamDlg::OnKillfocusQrparam4() 
{
	UpdateData(TRUE);
	if(m_QRParam4 > 299)	m_QRParam4 = 299;
	if(m_QRParam4 <= 0)		m_QRParam4 = 1;
	UpdateData(FALSE);
	
}

void CSetQRParamDlg::OnKillfocusQrparam5() 
{
	UpdateData(TRUE);
	if(m_QRParam5 > 299)	m_QRParam5 = 299;
	if(m_QRParam5 <= 0)	m_QRParam5 = 1;
	UpdateData(FALSE);	
}

BOOL CSetQRParamDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_QRParam1 = QRParam[0];
	m_QRParam2 = QRParam[1];
	m_QRParam3 = QRParam[2];
	m_QRParam4 = QRParam[3];
	m_QRParam5 = QRParam[4];

	UpdateData(FALSE);
	
	return TRUE;
}

void CSetQRParamDlg::OnOK() 
{
	QRParam[0] = m_QRParam1;
	QRParam[1] = m_QRParam2;
	QRParam[2] = m_QRParam3;
	QRParam[3] = m_QRParam4;
	QRParam[4] = m_QRParam5;

	CDialog::OnOK();
}

void CSetQRParamDlg::OnDefaultqrparam() 
{
	m_QRParam1 = 3;
	m_QRParam2 = 5;
	m_QRParam3 = 10;
	m_QRParam4 = 20;
	m_QRParam5 = 60;
	UpdateData(FALSE);
}

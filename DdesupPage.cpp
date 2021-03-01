// DdesupPage.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "DdesupPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDdesupPage property page

IMPLEMENT_DYNCREATE(CDdesupPage, CPropertyPage)

CDdesupPage::CDdesupPage() : CPropertyPage(CDdesupPage::IDD)
{
	//{{AFX_DATA_INIT(CDdesupPage)
	m_nLongPeriod = g_GH.DDESUP.nLongPeriod;
	m_nShortPeriod = g_GH.DDESUP.nShortPeriod;
	//}}AFX_DATA_INIT
}

CDdesupPage::~CDdesupPage()
{
}

void CDdesupPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDdesupPage)
	DDX_Text(pDX, IDC_LONGPERIOD, m_nLongPeriod);
	DDV_MinMaxUInt(pDX, m_nLongPeriod, 2, 1000);
	DDX_Text(pDX, IDC_SHORTPERIOD, m_nShortPeriod);
	DDV_MinMaxUInt(pDX, m_nShortPeriod, 2, 1000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDdesupPage, CPropertyPage)
	//{{AFX_MSG_MAP(CDdesupPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDdesupPage message handlers

BOOL CDdesupPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;
}

void CDdesupPage::OnOK() 
{	
	UpdateData(TRUE);
	g_GH.DDESUP.nLongPeriod = m_nLongPeriod;
	g_GH.DDESUP.nShortPeriod = m_nShortPeriod;

	CPropertyPage::OnOK();
}

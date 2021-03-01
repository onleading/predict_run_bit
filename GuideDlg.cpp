// GuideDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "GuideDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGuideDlg dialog


CGuideDlg::CGuideDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGuideDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGuideDlg)
	m_nMode = 0;
	m_nNetCondition = 0;
	//}}AFX_DATA_INIT
}


void CGuideDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGuideDlg)
	DDX_Control(pDX, IDC_OTHER_STATIC, m_Other_Static);
	DDX_Radio(pDX, IDC_MODE1, m_nMode);
	DDX_Radio(pDX, IDC_RADIO1, m_nNetCondition);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGuideDlg, CDialog)
	//{{AFX_MSG_MAP(CGuideDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGuideDlg message handlers

BOOL CGuideDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Other_Static.SetTextColor(RGB(0,0,255));

	if(g_bHasCaption)
		m_nMode = 0;
	else
		m_nMode = 1;
	
	m_nNetCondition = g_nNetCondition;

	UpdateData(FALSE);
	return TRUE;
}

void CGuideDlg::OnOK() 
{
	UpdateData(TRUE);
	g_nNetCondition = m_nNetCondition;
	RECENTNUM = (g_nNetCondition+1)*140; //根据上网条件，来设置RECENTNUM

	if(m_nMode==0)
		g_bHasCaption = TRUE;
	else
		g_bHasCaption = FALSE;


	CDialog::OnOK();
}

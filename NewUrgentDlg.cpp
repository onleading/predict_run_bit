// NewUrgentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TdxW.h"
#include "NewUrgentDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewUrgentDlg dialog


CNewUrgentDlg::CNewUrgentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewUrgentDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewUrgentDlg)
	m_strContent = _T("");
	m_strTitle = _T("");
	m_bNews = 1;
	//}}AFX_DATA_INIT
	m_nMainID = 0;
}

void CNewUrgentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewUrgentDlg)
	DDX_Text(pDX, IDC_CONTENT_EDIT, m_strContent);
	DDX_Text(pDX, IDC_TITLE_EDIT, m_strTitle);
	DDX_Radio(pDX, IDC_NEWS, m_bNews);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNewUrgentDlg, CDialog)
	//{{AFX_MSG_MAP(CNewUrgentDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewUrgentDlg message handlers

void CNewUrgentDlg::OnOK() 
{
	UpdateData(TRUE);
	
	//////////////////////////////////////////////////////////////////////////
	g_nMainID = MainIDPlus(m_nMainID,INFOUPLOAD_WIN);
	g_nAssisID = INFOUPLOAD_ASSISID;

	g_pGetData->GetInfoUpload((m_bNews==0?NEWS_TYPE:URGENT_TYPE), time(NULL), (char*)(LPCSTR)m_strTitle, (char*)(LPCSTR)m_strContent);
	
	m_strTitle = "";
	m_strContent = "";
	UpdateData(FALSE);
}


void CNewUrgentDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

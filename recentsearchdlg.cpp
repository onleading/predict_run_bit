// recentsearchdlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "recentsearchdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecentSearchDlg dialog


CRecentSearchDlg::CRecentSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRecentSearchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRecentSearchDlg)
	//}}AFX_DATA_INIT
	m_ItemData = 989;
}


void CRecentSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecentSearchDlg)
	DDX_Control(pDX, IDC_RECENT_LIST, m_Recent_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRecentSearchDlg, CDialog)
	//{{AFX_MSG_MAP(CRecentSearchDlg)
	ON_LBN_DBLCLK(IDC_RECENT_LIST, OnDblclkRecentList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecentSearchDlg message handlers

BOOL CRecentSearchDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Recent_List.SetItemHeight(0,16);
	m_Recent_List.SetItemHeight(-1,16);
	
	//读入并重新挂在树上
	RECENTSEARCH tmpRecentSearch[10];
	int nCnt = GetAllRecentSearch(tmpRecentSearch);
	for(int i=0;i < nCnt;i++)
	{
		m_Recent_List.AddString(_F(tmpRecentSearch[i].description));
		m_Recent_List.SetItemData(i,989+i);
	}
	if(nCnt > 0)
		m_Recent_List.SetCurSel(0);
	return TRUE;
}

void CRecentSearchDlg::OnOK() 
{
	int nSel = m_Recent_List.GetCurSel();
	if(nSel == LB_ERR) return;

	m_ItemData = m_Recent_List.GetItemData(nSel);
	
	CDialog::OnOK();
}

void CRecentSearchDlg::OnDblclkRecentList() 
{
	OnOK();	
}

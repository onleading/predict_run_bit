#include "StdAfx.h"
#include "LocalIndexMainDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLocalIndexMainDlg

IMPLEMENT_DYNAMIC(CLocalIndexMainDlg, CPropertySheet)
CLocalIndexMainDlg::CLocalIndexMainDlg()
{
	m_nActivePage = 0;
	m_bPageValid = TRUE;
}

CLocalIndexMainDlg::CLocalIndexMainDlg(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_nActivePage = 0;
	m_bPageValid = TRUE;
}

CLocalIndexMainDlg::CLocalIndexMainDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_nActivePage = 0;
	m_bPageValid = TRUE;
}

CLocalIndexMainDlg::~CLocalIndexMainDlg()
{
}


BEGIN_MESSAGE_MAP(CLocalIndexMainDlg, CPropertySheet)
	//{{AFX_MSG_MAP(CLocalIndexMainDlg)
	ON_COMMAND(IDOK,OnSaveAndExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocalIndexMainDlg message handlers
void CLocalIndexMainDlg::OnSaveAndExit()
{
	int i=0;
	for(i=0;i<8;i++)
	{
		if(!CheckPage(i))
		{
			SetActivePage(i);
			m_nActivePage = i;
			return;
		}
	}
	PostMessage(PSM_PRESSBUTTON,PSBTN_CANCEL);
}

BOOL CLocalIndexMainDlg::CheckPage(int nPage)
{
	switch((nPage+1))
	{
	default:
		return TRUE;
		break;
	}
	return TRUE;
}

BOOL CLocalIndexMainDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	CTabCtrl* tab = GetTabControl();
	NMHDR * pHdr = (NMHDR *)lParam;
	//在切换页面时要检测页面设置是否正确
	if(tab->m_hWnd == pHdr->hwndFrom)	//修改当前页面完成
	{
		switch(pHdr->code)
		{
		case TCN_SELCHANGE:
			if(m_bPageValid) m_nActivePage = GetActiveIndex();
			else SetActivePage(m_nActivePage);
			break;
		case TCN_SELCHANGING:
			m_bPageValid = CheckPage(m_nActivePage);
			break;
		}
	}
	return CPropertySheet::OnNotify(wParam, lParam, pResult);
}

BOOL CLocalIndexMainDlg::PreTranslateMessage(MSG* pMsg) 
{
	//TAB+CTRL可以切换页面，屏蔽
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB && WORD(GetKeyState(VK_CONTROL))>0x00ff)
	{
		return 1;
	}
	
	return CPropertySheet::PreTranslateMessage(pMsg);
}


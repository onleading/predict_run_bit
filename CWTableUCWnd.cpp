// CWTableUCWnd.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "CWTableUCWnd.h"
#include "CWTableUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCWTableUCWnd

CCWTableUCWnd::CCWTableUCWnd(CCWTableUnit *pCWTableU)
{
	m_pCWTableU = pCWTableU;
}

CCWTableUCWnd::~CCWTableUCWnd()
{
}


BEGIN_MESSAGE_MAP(CCWTableUCWnd, CWnd)
	//{{AFX_MSG_MAP(CCWTableUCWnd)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(UM_ASYN_SQL_MSG,OnAckSQLData)
	ON_MESSAGE(UM_LIST_SELCHANGE,OnListSelChange)
	ON_MESSAGE(UM_LIST_COLSORT,OnListColSort)
	ON_MESSAGE(UM_LIST_ROWGNPOP,OnListRowGnPop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCWTableUCWnd message handlers
void	CCWTableUCWnd::OnAckSQLData(WPARAM wParam,LPARAM lParam)
{
	if(m_pCWTableU) 
		m_pCWTableU->OnGetDataAck(wParam, lParam);
}

void	CCWTableUCWnd::OnListSelChange(WPARAM wParam,LPARAM lParam)
{
	if(m_pCWTableU) 
		m_pCWTableU->OnRefreshChartFromList(UM_LIST_SELCHANGE);
}

void	CCWTableUCWnd::OnListColSort(WPARAM wParam,LPARAM lParam)
{
	
}

void	CCWTableUCWnd::OnListRowGnPop(WPARAM wParam,LPARAM lParam)
{
	if(m_pCWTableU) 
		m_pCWTableU->OnListRowGnPop();
}
//////////////////////////////////////////////////////////////////////////

BOOL CCWTableUCWnd::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

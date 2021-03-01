// CWChartUCWnd.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "CWChartUCWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCWChartUCWnd

CCWChartUCWnd::CCWChartUCWnd(CCWChartUnit *pCWChartU)
{
	m_pCWChart = pCWChartU;
}

CCWChartUCWnd::~CCWChartUCWnd()
{
}


BEGIN_MESSAGE_MAP(CCWChartUCWnd, CWnd)
	//{{AFX_MSG_MAP(CCWChartUCWnd)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCWChartUCWnd message handlers
BOOL CCWChartUCWnd::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

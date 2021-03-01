// URmReportCtrlWnd.cpp: implementation of the URmReportCtrlWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdxw.h"
#include "URmReportCtrlWnd.h"
#include "URmReport.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

URmReportCtrlWnd::URmReportCtrlWnd(URmReport *pURmReport)
{
	m_pURmReport = pURmReport;
}

URmReportCtrlWnd::~URmReportCtrlWnd()
{

}
//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(URmReportCtrlWnd, CWnd)
	ON_COMMAND_RANGE(ID_SELECTDATE,ID_ADDINDEXRANK,OnClickRight)

	ON_COMMAND_RANGE(ID_ALLBLOCKFX,ID_ZJHHYBLOCKFX,OnSelBlockFL)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ALLBLOCKFX,ID_ZJHHYBLOCKFX,OnUpdateSelBlockFL)

	ON_MESSAGE(UM_HSCROLLBAR,OnHScrollMessage)
	ON_MESSAGE(UM_VSCROLLBAR,OnVScrollMessage)

	ON_MESSAGE(UM_EXOPENBLOCKHQ,OnOpenBlockHQ)
	//{{AFX_MSG_MAP(URmReportCtrlWnd)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////
BOOL URmReportCtrlWnd::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

int URmReportCtrlWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	return m_pURmReport->OnCreate();
}

void URmReportCtrlWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	m_pURmReport->OnSize(nType, cx, cy);
}

void URmReportCtrlWnd::OnPaint() 
{	
	CPaintDC dc(this);
	m_pURmReport->OnDraw(&dc);
}

void URmReportCtrlWnd::OnHScrollMessage(WPARAM wParam,LPARAM lParam)
{
	m_pURmReport->OnHScrollMessage(wParam, lParam);
}

void URmReportCtrlWnd::OnVScrollMessage(WPARAM wParam,LPARAM lParam)
{
	m_pURmReport->OnVScrollMessage(wParam, lParam);
}

void URmReportCtrlWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	m_pURmReport->OnKeyDown(nChar, nRepCnt, nFlags);
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void URmReportCtrlWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_pURmReport->OnLButtonDown(nFlags, point);
	CWnd::OnLButtonDown(nFlags, point);
}

void URmReportCtrlWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_pURmReport->OnRButtonUp(nFlags, point);
	CWnd::OnRButtonUp(nFlags, point);
}

void URmReportCtrlWnd::OnClickRight(UINT nID)
{
	m_pURmReport->OnClickRight(nID);
}

void URmReportCtrlWnd::OnSelBlockFL(UINT nID)
{	
	m_pURmReport->OnSelBlockFL(nID);
}

void URmReportCtrlWnd::OnUpdateSelBlockFL(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pURmReport->GetCurBlockFlag()==pCmdUI->m_nID-ID_ALLBLOCKFX);
}

void URmReportCtrlWnd::OnTimer(UINT nIDEvent) 
{
	m_pURmReport->OnTimer(nIDEvent);
	CWnd::OnTimer(nIDEvent);
}

void URmReportCtrlWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_pURmReport->OnLButtonDblClk(nFlags, point);
	CWnd::OnLButtonDblClk(nFlags, point);
}

BOOL URmReportCtrlWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	m_pURmReport->OnMouseWheel(nFlags, zDelta, pt);
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void URmReportCtrlWnd::OnOpenBlockHQ(WPARAM wParam,LPARAM lParam)
{
	m_pURmReport->OnOpenBlockHQ(wParam, lParam);
}
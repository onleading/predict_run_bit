// vesselwnd.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "vesselwnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVesselWnd

CVesselWnd::CVesselWnd(UBaseInner *pUnit,int nUnitType)
{
	m_pUnit = pUnit;
	m_nUnitType = nUnitType;
}

CVesselWnd::~CVesselWnd()
{
}


BEGIN_MESSAGE_MAP(CVesselWnd, CWnd)
	//{{AFX_MSG_MAP(CVesselWnd)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CVesselWnd message handlers

void CVesselWnd::PaintBK()
{
	CDC *pDC=GetDC(); //自己得到DC,可信一些吗?
	if(!pDC) return;
	ReleaseDC(pDC);
}

void CVesselWnd::PaintVessel()
{
	if(::IsWindow(m_hWnd))
		PaintBK();
}

BOOL CVesselWnd::OnEraseBkgnd(CDC* pDC) 
{
	PaintBK();
	return TRUE;
}

LRESULT CVesselWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	return CWnd::WindowProc(message, wParam, lParam);
}

void CVesselWnd::OnTimer(UINT nIDEvent) 
{
	CWnd::OnTimer(nIDEvent);
}

BOOL CVesselWnd::PreTranslateMessage(MSG* pMsg) 
{
	return CWnd::PreTranslateMessage(pMsg);
}

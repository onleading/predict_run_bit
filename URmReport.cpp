// URmReport.cpp: implementation of the URmReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdxw.h"
#include "URmReport.h"
#include "URmReportCtrlWnd.h"
#include "URmReportCtrl.h"
#include "ComView2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

URmReport::URmReport(LPCOMINFO pComInfo, URmReportCtrl *pURmReportCtrl)
{
	m_pURmReportCtrl = pURmReportCtrl;
	m_pURmReportCtrlWnd = NULL;
	m_pRMReport = NULL;
	memcpy(&m_ComInfo, pComInfo, sizeof(m_ComInfo));
}

URmReport::~URmReport()
{
	InterlockedDecrement(&g_nCalcBlockIndex);
	TDEL(m_pRMReport);
	TDEL(m_pURmReportCtrlWnd);
}

//////////////////////////////////////////////////////////////////////////
BOOL	URmReport::InitShow(HWND hVessel)
{
	m_hVessel = hVessel;
	if(!m_pURmReportCtrlWnd||!m_pURmReportCtrlWnd->m_hWnd)
	{
		delete m_pURmReportCtrlWnd;
		m_pURmReportCtrlWnd = new URmReportCtrlWnd(this);
		m_pURmReportCtrlWnd->Create(NULL,"hidef10wnd",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),CWnd::FromHandle(m_hVessel),IDC_HIDENETWND);
		if(!m_pURmReportCtrlWnd||!m_pURmReportCtrlWnd->m_hWnd) return FALSE;
	}

	CRect rc;
	m_pURmReportCtrlWnd->GetClientRect(&rc);
	m_pRMReport->SetRect(rc);
	m_pRMReport->InitReport(g_GH.nCurBlockFlag); //缺省打开所有板块
	m_pRMReport->SetCurLabel(g_GH.nCurBlockFlag);

	InterlockedIncrement(&g_nCalcBlockIndex);

	return TRUE;
}

void	URmReport::ArrangeWnd(CRect &rc)
{
	if(!m_pURmReportCtrlWnd) return;

	if(m_pURmReportCtrlWnd&&m_pURmReportCtrlWnd->m_hWnd)
		m_pURmReportCtrlWnd->MoveWindow(&rc, FALSE);
}
//////////////////////////////////////////////////////////////////////////
void URmReport::OnDraw(CDC* pDC)
{
	m_pRMReport->DrawReport(pDC);
}

int URmReport::OnCreate() 
{
	if(!m_pRMReport)
		m_pRMReport = new RMReport(&m_ComInfo);
	m_pRMReport->Create(m_pURmReportCtrlWnd,CRect(0,0,0,0));

	//用于自动翻页的定时器
	m_pURmReportCtrlWnd->SetTimer(1,AutoPageTime*1000,NULL);
	m_pURmReportCtrlWnd->SetTimer(2,RMREPORT_SECOND*1000,NULL); //热门板块分析的动态计算定时器

	return 0;
}

void URmReport::OnSize(UINT nType, int cx, int cy) 
{
	m_pRMReport->OnSize(nType,cx,cy);
}

void URmReport::OnHScrollMessage(WPARAM wParam,LPARAM lParam)
{
	m_pRMReport->OnHScrollMessage(wParam,lParam);
}

void URmReport::OnVScrollMessage(WPARAM wParam,LPARAM lParam)
{
	m_pRMReport->OnVScrollMessage(wParam,lParam);
}

void URmReport::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	m_pRMReport->OnKeyDown(nChar,nRepCnt,nFlags);

	if(nChar == VK_RETURN)
		m_pRMReport->OpenBlockHQ();
}

void URmReport::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_pRMReport->OnLButtonDown(nFlags, point);
}

void URmReport::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CClientDC dc(m_pURmReportCtrlWnd);
	//如果在行的范围中，则弹出行的右键菜单.
	m_pRMReport->OnClick (&dc,point,TRUE);
}

#include "SelectDateDlg.h"
void URmReport::OnClickRight(UINT nID)
{
	switch(nID)
	{
	case ID_SELECTRMQJ:
		{
			CSelectDateDlg dlg;
			dlg.bSingle = FALSE;
			
			dlg.m_Date1 = m_pRMReport->GetDate(TRUE);
			dlg.m_Date2 = m_pRMReport->GetDate(FALSE);
			if( dlg.DoModal() == IDOK )
				m_pRMReport->SetDateSection(dlg.m_Date1,dlg.m_Date2);
			break;
		}
	case ID_TODAYRM:
		m_pRMReport->SetDateSection(CTime::GetCurrentTime(),CTime::GetCurrentTime());
		break;
	case ID_OPENBLOCKHQ:
		m_pRMReport->OpenBlockHQ();
		break;
	default:
		break;
	}
}

void URmReport::OnSelBlockFL(UINT nID)
{	
	m_pRMReport->InitReport(nID-ID_ALLBLOCKFX);
	m_pRMReport->SetCurLabel(nID-ID_ALLBLOCKFX);
}

void URmReport::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 2)
	{
		if(OnLine && !g_bDowning && theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))
		{
			if(m_pRMReport->m_bTodayRM) 
			{
				m_pRMReport->Start_Calc();
				if(m_pURmReportCtrl)
					m_pURmReportCtrl->BroadcastUnitMsg(UM_UPDATE, 0, 0);
			}
		}
	}
}

void URmReport::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CRect CornerRect;
	m_pRMReport->GetCornerRect(CornerRect);

	if(point.y < CornerRect.bottom)	return;
	m_pRMReport->OpenBlockHQ();
}

BOOL URmReport::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(zDelta < 0)
		OnKeyDown(VK_DOWN,1,0);
	else
		OnKeyDown(VK_UP,1,0);
	return TRUE;
}

void URmReport::OnOpenBlockHQ(WPARAM wParam,LPARAM lParam)
{
	if(m_pURmReportCtrl)
		m_pURmReportCtrl->BroadcastUnitMsg(UM_EXOPENBLOCKHQ, wParam, lParam);
}
// URmReportCtrl.cpp: implementation of the URmReportCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdxw.h"
#include "URmReportCtrl.h"
#include "URmReport.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
URmReportCtrl::URmReportCtrl(LPCOMINFO pComInfo):UBaseCtrl(pComInfo)
{
	m_pURmReport = new URmReport(pComInfo, this);
}

URmReportCtrl::~URmReportCtrl()
{
	TDEL(m_pURmReport)
}

BOOL	URmReportCtrl::ShowUnit(HWND hVessel)
{
	if(m_pURmReport)
		return m_pURmReport->InitShow(hVessel);
	return FALSE;
}

void	URmReportCtrl::ResizeUnit(int cx, int cy)
{
	if(m_pURmReport)
		m_pURmReport->ArrangeWnd(CRect(0, 0, cx, cy));
}

int		URmReportCtrl::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
		m_pURmReport->GetCtrlWindow()->SendMessage(message,wParam,lParam);
		break;
	case UM_COM2SWITCH1:
		m_pURmReport->OnCom2Switch1();
		break;
	}
	return UBaseCtrl::ProcessMsg(message, wParam, lParam);
}
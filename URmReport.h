// URmReport.h: interface for the URmReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_URMREPORT_H__54D9DC01_17C9_46CC_AF36_200B979F37F2__INCLUDED_)
#define AFX_URMREPORT_H__54D9DC01_17C9_46CC_AF36_200B979F37F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RMReport.h"

class URmReportCtrl;
class URmReportCtrlWnd;
class URmReport  
{
public:
	URmReport(LPCOMINFO pComInfo, URmReportCtrl *pURmReportCtrl);
	virtual ~URmReport();

	CWnd*	GetCtrlWindow() { return (CWnd*)m_pURmReportCtrlWnd; }

	BOOL	InitShow(HWND hVessel);
	void	ArrangeWnd(CRect &rc);

	//
	void	OnDraw(CDC* pDC);
	int		OnCreate();
	void	OnSize(UINT nType, int cx, int cy);
	void	OnHScrollMessage(WPARAM wParam,LPARAM lParam);
	void	OnVScrollMessage(WPARAM wParam,LPARAM lParam);
	void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void	OnLButtonDown(UINT nFlags, CPoint point);
	void	OnRButtonUp(UINT nFlags, CPoint point);
	void	OnClickRight(UINT nID);
	void	OnSelBlockFL(UINT nID);
	void	OnTimer(UINT nIDEvent);
	void	OnLButtonDblClk(UINT nFlags, CPoint point);
	BOOL	OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void	OnOpenBlockHQ(WPARAM wParam,LPARAM lParam);
	void	OnCom2Switch1() { m_pRMReport->Com2Switch1(); }

	int		GetCurBlockFlag() { return m_pRMReport->CurBlockFlag; }

private:
	RMReport		*m_pRMReport;
	
	URmReportCtrl	*m_pURmReportCtrl; 
	URmReportCtrlWnd *m_pURmReportCtrlWnd;
	COMINFO			m_ComInfo;
	HWND			m_hVessel;				//ÈÝÆ÷

};

#endif // !defined(AFX_URMREPORT_H__54D9DC01_17C9_46CC_AF36_200B979F37F2__INCLUDED_)

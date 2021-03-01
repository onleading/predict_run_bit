// URmReportCtrl.h: interface for the URmReportCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_URMREPORTCTRL_H__F871E027_70D0_4044_B853_BF95B9BBC11B__INCLUDED_)
#define AFX_URMREPORTCTRL_H__F871E027_70D0_4044_B853_BF95B9BBC11B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UBaseCtrl.h"

class URmReport;
class URmReportCtrl : public UBaseCtrl  
{
public:
	URmReportCtrl(LPCOMINFO pComInfo);
	virtual ~URmReportCtrl();

	BOOL	ShowUnit(HWND hVessel);
	void	ResizeUnit(int cx, int cy);
	int		ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam);

private:
	URmReport	*m_pURmReport;
};

#endif // !defined(AFX_URMREPORTCTRL_H__F871E027_70D0_4044_B853_BF95B9BBC11B__INCLUDED_)

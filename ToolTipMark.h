// ToolTipMark.h: interface for the CToolTipMark class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLTIPMARK_H__4F15E8EC_12AD_46B8_8363_BF405ADDF574__INCLUDED_)
#define AFX_TOOLTIPMARK_H__4F15E8EC_12AD_46B8_8363_BF405ADDF574__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HToolTipEx.h"
struct RegedTipInfo
{
	CWnd		*pWnd;
	CRect		rc;
	MemStkInfo	*pStock;
};

class CToolTipMark  
{
public:
	CToolTipMark(CWnd *pWnd);
	virtual ~CToolTipMark();

	BOOL	InitTool();
	BOOL	AddTool(CWnd* pWnd, LPCTSTR lpszText = LPSTR_TEXTCALLBACK, LPCRECT lpRectTool = NULL, UINT nIDTool = 0);
	void	RelayEvent(LPMSG lpMsg);
	void	RegTipInfo(CRect rc, MemStkInfo *pStock, CWnd *pWnd=NULL);
	void	ShowTool(CPoint pt, CWnd *pWnd);
	void	GetToolTextEx(CString &strRes, CPoint pt, CWnd *pWnd);
	void	ClearToolTipInfo(CWnd *pWnd);

private:
	HToolTipEx	m_ToolTip;
	CWnd			*m_pOwnerWnd;
	vector<RegedTipInfo>	m_RegedTip;
};

#endif // !defined(AFX_TOOLTIPMARK_H__4F15E8EC_12AD_46B8_8363_BF405ADDF574__INCLUDED_)

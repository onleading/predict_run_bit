// URmReportCtrlWnd.h: interface for the URmReportCtrlWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_URMREPORTCTRLWND_H__2096A2D0_4424_45B9_8BD0_CAD790258ACA__INCLUDED_)
#define AFX_URMREPORTCTRLWND_H__2096A2D0_4424_45B9_8BD0_CAD790258ACA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class URmReport;


class URmReportCtrlWnd : public CWnd  
{
public:
	URmReportCtrlWnd(URmReport *pURmReport);
	virtual ~URmReportCtrlWnd();

	void	Resize();

private:
	URmReport	*m_pURmReport;
	BOOL	m_bLayoutFlag;
	BOOL	m_bQuitting;
		
	// Generated message map functions
protected:
	//{{AFX_MSG(URmReportCtrlWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	afx_msg void OnHScrollMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnVScrollMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSelBlockFL(UINT nID);
	afx_msg void OnUpdateSelBlockFL(CCmdUI *pCmdUI);
	afx_msg void OnClickRight(UINT nID);
	afx_msg void OnGPFL();
	afx_msg void OnClickCorner(UINT nID);
	afx_msg void OnUpdateClickCorner(CCmdUI* pCmdUI);
	afx_msg void OnOpenBlockHQ(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_URMREPORTCTRLWND_H__2096A2D0_4424_45B9_8BD0_CAD790258ACA__INCLUDED_)

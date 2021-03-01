// CUF10CtrlWnd.h: interface for the CUF10CtrlWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUF10CTRLWND_H__2B37D716_B577_4F4E_A84F_17B157541128__INCLUDED_)
#define AFX_CUF10CTRLWND_H__2B37D716_B577_4F4E_A84F_17B157541128__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class UF10;

class CUF10CtrlWnd : public CWnd  
{
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTxtView)
	//}}AFX_VIRTUAL

public:
	CUF10CtrlWnd(UF10 *pUF10);
	virtual ~CUF10CtrlWnd();

	void	Resize();

private:
	UF10	*m_pUf10;
	BOOL	m_bLayoutFlag;
	BOOL	m_bQuitting;

protected:
	afx_msg void OnClickFLButton(UINT nID);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRichEditMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSelectJBM(UINT nID);
	afx_msg void OnUpdateSelectJBM(CCmdUI *pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	afx_msg void OnCancelMark(void);
	afx_msg void OnMarkStockFlag(UINT nID);
	afx_msg void OnUpdateMarkStockFlag(CCmdUI* pCmdUI);
	//{{AFX_MSG(CUF10CtrlWnd)
	afx_msg void OnInBlock();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRightAddblock();
	afx_msg void OnRightAddZxg();
	afx_msg void OnDelFromBlock();
	afx_msg void OnOtherjbm();
	afx_msg void OnUpdateOtherjbm(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_CUF10CTRLWND_H__2B37D716_B577_4F4E_A84F_17B157541128__INCLUDED_)

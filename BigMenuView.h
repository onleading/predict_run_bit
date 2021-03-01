// BigMenuView.h : interface of the CBigMenuView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_BIGMENUVIEW_H__3C717A74_F706_11D2_98A2_0088CC032229__INCLUDED_)
#define AFX_BIGMENUVIEW_H__3C717A74_F706_11D2_98A2_0088CC032229__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MenuWin.h"
#include "BlockWin.h"

class CBigMenuView : public CView
{
	BOOL	KeyGuyFlag;
	BOOL	m_bQuitting;	
	BOOL	m_bLayoutFlag;

	void	ChangeToPad(int which);
	void	OnUserMessage(UINT message, WPARAM wParam, LPARAM lParam);
protected: 
	CBigMenuView();
	DECLARE_DYNCREATE(CBigMenuView)
public:
	CMenuWin *	pMenuWin;
	CBlockWin*  pBlockWin;
	BaseWin	   *pActiveWin,*pDeactiveWin;
	int			nActiveWin,nDeactiveWin;

	void		ChangeMenuColor();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBigMenuView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnInitialUpdate();
	protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBigMenuView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CBigMenuView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BIGMENUVIEW_H__3C717A74_F706_11D2_98A2_0088CC032229__INCLUDED_)

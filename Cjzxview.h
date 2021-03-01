#if !defined(AFX_CJZXVIEW_H__09C32BE2_A0CA_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_CJZXVIEW_H__09C32BE2_A0CA_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HisView.h : header file

class CCjzxView : public CView
{
protected:
	CCjzxView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCjzxView)
// Attributes
public:
	BOOL		KeyGuyFlag;
	BOOL		m_bQuitting;
public:
	short	GetCurrentGPIndex();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCjzxView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnDraw(CDC* pDC);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCjzxView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CCjzxView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKeyGuyMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnCjzxMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnCjzxDayMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CJZXVIEW_H__09C32BE2_A0CA_11D3_B921_0000800104D5__INCLUDED_)

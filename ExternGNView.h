#if !defined(AFX_EXTERNGNVIEW_H__2C2FB3A0_DC61_4F2A_BD1C_ABF2E2C8B8EB__INCLUDED_)
#define AFX_EXTERNGNVIEW_H__2C2FB3A0_DC61_4F2A_BD1C_ABF2E2C8B8EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExternGNView.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CExternGNView view

class CExternGNView : public CView
{
protected:
	CExternGNView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CExternGNView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExternGNView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CExternGNView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CExternGNView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKeyGuyMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTERNGNVIEW_H__2C2FB3A0_DC61_4F2A_BD1C_ABF2E2C8B8EB__INCLUDED_)

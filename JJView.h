#if !defined(AFX_JJVIEW_H__13BD63B5_D391_4097_8E57_E9BA067FE8F0__INCLUDED_)
#define AFX_JJVIEW_H__13BD63B5_D391_4097_8E57_E9BA067FE8F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// JJView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJJView view
#include "JJGrid.h"

class CJJView : public CView
{
protected:
	CJJView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CJJView)

// Attributes
public:
	JJGrid	m_JJGrid;
	BOOL	KeyGuyFlag;
	BOOL	m_bQuitting;	
	//¥Ú”°
	CDC * m_pMemDC;  //A memory device context compatible with our printer DC.
	CRect m_PrintRect;    //To hold the dimensions of our printing area while scaling.
	CBitmap * m_pBm; //Capture the screen image as a Bitmap
	
// Operations
public:
	void SpecialReDraw(int Flag);
	void SelectSomeRow(const char* lpszSymbol);
	
	short GetCurrentGPIndex();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJJView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CJJView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CJJView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyGuyMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnHScrollMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnVScrollMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnFilePrintPreview();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSelFundType(UINT nID);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRightOpen();
	afx_msg void OnTopStock();
	afx_msg void OnTop10stock();
	afx_msg void OnTopFund();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JJVIEW_H__13BD63B5_D391_4097_8E57_E9BA067FE8F0__INCLUDED_)

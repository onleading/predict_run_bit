// GridView.h : interface of the CGridView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDVIEW_H__3C717A74_F706_11D2_98A2_0088CC032229__INCLUDED_)
#define AFX_GRIDVIEW_H__3C717A74_F706_11D2_98A2_0088CC032229__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HQGrid.h"
#include "SwitchEx.h"
#include "MyBitmapBt.h"
#include "MyRichEdit.h"

class CGridView : public CView
{
	void SetFySwitch();
private:
	BOOL	KeyGuyFlag;
	BOOL	m_bQuitting;	
	BOOL	m_bLayoutFlag;
	short   m_nMainID;
	BOOL	m_bHasTendBar;
public:
	HQGrid		*m_pHQGrid;
	CNewZfjsTab	*m_pSwitchBar;
	CRect		m_rcFYTitle;
	CString		m_strFYTitle;
	CMyRichEdit m_RichEdit;
	BOOL		m_bEditShow;
protected: 
	void	SwitchEdit(int nWhichGroup);
	CGridView();
	DECLARE_DYNCREATE(CGridView)
public:
	void	ResizeControls();
	BOOL	ProcessGPCode(const char *code,short setcode);
	void	SpecialReDraw(int Flag);
	short	GetCurrentGPIndex();
	short	GetCurrentGPSet(short *pPageGPIndex);
	void	GetCurrentDomain(int & WhichGPType,int & nBlockFlag);
	void	SetFYTitle(LPCSTR str);
	void	GetSYSCurrentLSetInfo(LinkedSetInfo *&pLSetInfo) {pLSetInfo =  m_pHQGrid->GetLinkedSetInfo();}
	UINT	GetGridType() 
	{ 
		if(m_pHQGrid) 
			return  m_pHQGrid->m_nGridType;
		return -1;
	};
	UINT	GetWhichType() 
	{ 
		if(m_pHQGrid) 
			return  m_pHQGrid->m_WhichGPType;
		return -1;
	};
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGridView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnInitialUpdate();
	protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGridView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//////////
	afx_msg void OnInBlock();
	afx_msg void OnBatch();
	afx_msg void OnGotojbzl();
	afx_msg void OnChangeGPType();
	afx_msg void OnAutoPage();
	afx_msg void OnFyReport(UINT nID);
	
	afx_msg void OnDSMarket(UINT nID);
	
	afx_msg void OnUpdateMDFHkHyClick(CCmdUI* pCmdUI);

	afx_msg void OnHQGridCommand(UINT nID);
	afx_msg void OnGridHZ(UINT nID);
	afx_msg void OnGridHZOpt();
	afx_msg void OnUpdateGridHZ(CCmdUI *pCmdUI);
	/////////
	afx_msg void OnUpdateRightClickHead(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClickCorner(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDYBlock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHYBlock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConceptClick(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZJHHYClick(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZHBClick(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRightClickRow(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDSMarket(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFYGrid(CCmdUI* pCmdUI);
	/////////
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);

	afx_msg void OnGridSetTitle(WPARAM wParam,LPARAM lParam);

	afx_msg void OnUpdateMenuBlockClick(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAutopage(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRank(CCmdUI *pCmdUI);

	afx_msg void OnWritetotxtfile();
	afx_msg void OnPageup();
	afx_msg void OnPagedown();
	afx_msg void GetMainID(WPARAM wParam,LPARAM lParam);
	afx_msg void OnResetFySwitchBar(WPARAM wParam,LPARAM lParam);
	afx_msg void OnCancelMark(void);
	afx_msg void OnMarkStockFlag(UINT nID);
	afx_msg void OnUpdateMarkStockFlag(CCmdUI* pCmdUI);
	//{{AFX_MSG(CGridView)
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDVIEW_H__3C717A74_F706_11D2_98A2_0088CC032229__INCLUDED_)

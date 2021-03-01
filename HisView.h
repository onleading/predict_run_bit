#if !defined(AFX_HISVIEW_H__09C32BE2_A0CA_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_HISVIEW_H__09C32BE2_A0CA_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HisView.h : header file

#define RM_REPORT 0		//热门分析
#define ZB_REPORT 1		//历史报表
#define QR_REPORT 2		//强弱分析
#define QJ_REPORT 3		//区间分析

#include "RMReport.h"
#include "ZBReport.h"
#include "QRReport.h"
#include "QJReport.h"

class CHisView : public CView
{
	//用来实现拖放的成员变量
	BOOL		 m_bStartDrag,m_bDragging;
	CImageList	 m_DragImage;
	int			 DraggingGPNo;

	void WakeUp(int WhichReport,int WhichQJType);	//唤醒一个报表
	void ChangeGPType(BOOL bTurn);					//在分类股、股票池、自选股间切换
protected:
	CHisView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CHisView)
// Attributes
public:
	BOOL		bAutoPage;

	BOOL		KeyGuyFlag;
	int			OldCurType;	//上一次报表类型
	int			CurType;	//当前报表类型	

	RMReport	m_RMReport;
	ZBReport	m_ZBReport;
	QRReport	m_QRReport;
	QJReport	m_QJReport;

	BOOL		bHasRM;
	BOOL		bHasZB;
	BOOL		bHasQR;
	BOOL		bHasQJ;

	int		m_WhichGPType;
	int		m_nBlockFlag;

	BOOL	m_bQuitting;

	//打印
	CDC * m_pMemDC;  //A memory device context compatible with our printer DC.
	CRect m_PrintRect;    //To hold the dimensions of our printing area while scaling.
	CBitmap * m_pBm; //Capture the screen image as a Bitmap
// Operations
public:
	short GetCurrentGPIndex();
	short GetCurrentGPSet(short *pPageGPIndex);
	BOOL  ProcessGPCode(const char *code,short setcode);
	void ToGPVip(BOOL bNewView);
	void SpecialReDraw(int Flag);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHisView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CHisView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CHisView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnChangeGPType();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRightAddblock();
	afx_msg void OnRightAddZxg();
	afx_msg void OnUpdateRightAddblock(CCmdUI *pCmdUI);
	afx_msg void OnDelFromBlock();
	afx_msg void OnBatch();
	afx_msg void OnUpdateBatch(CCmdUI *pCmdUI);
	afx_msg void OnQjreport();
	afx_msg void OnUpdateQjreport(CCmdUI* pCmdUI);
	afx_msg void OnWritetotxtfile();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	afx_msg void OnInBlock();
	afx_msg void OnHScrollMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnVScrollMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnGPFL();
	afx_msg void OnClickCorner(UINT nID);
	afx_msg void OnUpdateClickCorner(CCmdUI* pCmdUI);
	afx_msg void OnDYBlock(UINT nID);
	afx_msg void OnUpdateDYBlock(CCmdUI* pCmdUI);
	afx_msg void OnHYBlock(UINT nID);
	afx_msg void OnUpdateHYBlock(CCmdUI* pCmdUI);

	afx_msg void OnConceptClick(UINT nID);
	afx_msg void OnUpdateConceptClick(CCmdUI* pCmdUI);
	afx_msg void OnZHBClick(UINT nID);
	afx_msg void OnUpdateZHBClick(CCmdUI* pCmdUI);
	afx_msg void OnZJHHYClick(UINT nID);
	afx_msg void OnUpdateZJHHYClick(CCmdUI* pCmdUI);

	afx_msg void OnMenuBlockClick(UINT nID);
	afx_msg void OnUpdateMenuBlockClick(CCmdUI *pCmdUI);

	afx_msg void OnToGPVip(UINT nID);
	afx_msg void OnUpdateNewWindow(CCmdUI *pCmdUI);
	
	afx_msg void OnChangeReport(UINT nID);
	afx_msg void OnUpdateChangeReport(CCmdUI *pCmdUI);

	afx_msg void OnSelBlockFX(UINT nID);
	afx_msg void OnUpdateSelBlockFX(CCmdUI *pCmdUI);

	afx_msg void OnClickRight(UINT nID);
	afx_msg void OnUpdateClickRight(CCmdUI *pCmdUI);
	
	afx_msg void OnDelIndexRank();
	afx_msg void OnUpdateDelIndexRank(CCmdUI *pCmdUI);

	afx_msg void OnAutopage();
	afx_msg void OnUpdateAutopage(CCmdUI* pCmdUI);

	afx_msg void OnKeyGuyMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTreeMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChangeBlock(WPARAM wParam, LPARAM lParam);
	
	afx_msg void OnCancelMark(void);
	afx_msg void OnMarkStockFlag(UINT nID);
	afx_msg void OnUpdateMarkStockFlag(CCmdUI* pCmdUI);

	afx_msg void OnFilePrintPreview();
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HISVIEW_H__09C32BE2_A0CA_11D3_B921_0000800104D5__INCLUDED_)

// CMYControlBar.h : header file
/////////////////////////////////////////////////////////////////////////////

#if !defined(CMYCONTROLBAR_H_INCLUDED)
#define CMYCONTROLBAR_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "CMYButton.h"
#include "MyBitmapBt.h"

#define IDC_BUTTON_HIDE		1000
#define IDC_BUTTON_MINI		1001
#define IDC_BUTTON_13SIZE	1002
#define IDC_BUTTON_23SIZE	1003
#define IDC_BUTTON_FULLSIZE	1004

#define IDC_BUTTON_BACK		1010
#define IDC_BUTTON_FORWARD	1011
#define IDC_BUTTON_MAINPAGE	1012

/////////////////////////////////////////////////////////////////////////////
// CMyControlBar class

class CMyControlBar : public CControlBar
{
	DECLARE_DYNAMIC(CMyControlBar)
//
	BOOL RegisterWindowClass(HINSTANCE hInstance,LPCTSTR lpszClassName);
// Construction
public:
	CMyControlBar();

// Attributes
protected:
	BOOL		 m_bInRecalcNC;
    BOOL         m_bTracking;
	BOOL		 m_bGripper;
	BOOL		 m_bButtons;
    UINT         m_cxEdge;
	UINT		 m_menuID;
    UINT         m_nDockBarID;
    CSize        m_sizeMin;
    CSize        m_sizeHorz;
    CSize        m_sizeVert;
    CSize        m_sizeFloat;
    CRect        m_rectBorder;
    CRect        m_rectTracker;
    CPoint       m_ptOld;
	CMyButton    m_btnClose;
	CMyButton    m_btnMinim;
	CMyButton    m_btn1P3Size;
	CMyButton    m_btn2P3Size;
	CMyButton    m_btnFullSize;
	CMyButton    m_btnBack;
	CMyButton    m_btnForward;
	CMyButton    m_btnMainPage;
	MyBitmapBt	 *m_pCloseBt;

	CImageList*  m_ImageList;
	CToolTipCtrl m_ToolTip;

	BOOL		 m_bFlatBtn,m_bHasSizeBtn;

	BOOL		 m_bFullSize;

	BOOL		 m_bRightBtn;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyControlBar)
	public:
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual BOOL Create(CWnd* pParentWnd, UINT nID, LPCTSTR lpszWindowName = NULL, CSize sizeDefault = CSize(200,200), DWORD dwStyle = WS_CHILD|WS_VISIBLE|CBRS_TOP);
    virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );
    virtual CSize CalcDynamicLayout( int nLength, DWORD dwMode );
	virtual BOOL IsFloating();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL IsHorzDocked() const;
	BOOL IsVertDocked() const;
	void DisableDockPos(int flag);

	int  GetHeight();
	void ResetSize();
	void OnMainFrameSize();
	BOOL IsFullSize();
	void SwitchToFullSize(BOOL bFull);
	void ShowFrameStuff(BOOL bGripper = TRUE, BOOL bButtons = TRUE) { 
		m_bGripper = bGripper;
		m_bButtons = bButtons;
	}
	void SetGripper(BOOL bHasGripper);
	void SetRightBtn(BOOL bRightBtn);
	void DrawGripper(CDC* pDC);
	CRect GetGripperRect();
	CRect GetButtonRect();
	UINT GetMenuID();
	void SetMenuID(UINT nID);
	CImageList* SetBtnImageList(CImageList *pImageList);
	void OnInvertTracker(const CRect& rect);
	void StopTracking(BOOL bAccept);
	void StartTracking();
	CPoint& ClientToWnd(CPoint& point);
	virtual ~CMyControlBar();

	virtual void SetControlBarStyle(BOOL bFlatBtn,BOOL bHasSizeBtn);

// Generated message map functions
protected:
	//{{AFX_MSG(CMyControlBar)
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnNcPaint();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButtonClose();
	afx_msg void OnUpdateButtonClose(CCmdUI* pCmdUI);
	afx_msg void OnButtonMinimize();
	afx_msg void OnUpdateButtonMinimize(CCmdUI* pCmdUI);
	afx_msg void OnButton13Size();
	afx_msg void OnUpdateButton13Size(CCmdUI* pCmdUI);
	afx_msg void OnUpdateButton23Size(CCmdUI* pCmdUI);
	afx_msg void OnButton23Size();
	afx_msg void OnUpdateButtonFullSize(CCmdUI* pCmdUI);
	afx_msg void OnButtonFullSize();

	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg void OnBitmapBt(WPARAM wParam, LPARAM lParam); 
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(CJCONTROLBAR_H_INCLUDED)


#if !defined(AFX_ASSISWND_H__D58E5FE4_4815_4189_8FE3_9225045E9D1B__INCLUDED_)
#define AFX_ASSISWND_H__D58E5FE4_4815_4189_8FE3_9225045E9D1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// assiswnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAssisWnd window

#include "webbrowser2.h"
#include "CMYTabCtrlBar.h"

class CAssisWnd : public CWnd
{
	void	ProcessSpecialLink(char *codename);
	void	DoLevin(short which,MemStkInfo *tmpInfo,short type);
public:
	CAssisWnd();
	DECLARE_DYNCREATE(CAssisWnd)

// Attributes
public:
	BOOL			KeyGuyFlag;

	BOOL			m_bLayoutFlag;	
	CWebBrowser2   *m_pExplorer;
	BOOL			m_bWebMode;
	BOOL			m_bStartFunc;

	int				m_nDataFlag;
	int				m_whichGn;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAssisWnd)
	public:
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAssisWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAssisWnd)
	afx_msg void OnShowItMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnStartFuncMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnIEBack(WPARAM wParam,LPARAM lParam);
	afx_msg void OnIEForward(WPARAM wParam,LPARAM lParam);
	afx_msg void OnIEMainPage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDoLevinBuyMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDoLevinSellMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDoWTCancelMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDoQzSDSMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void DocumentComplete(LPDISPATCH /* pDisp */, VARIANT* URL);
	afx_msg void OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel);
	afx_msg void OnNewWindow2(LPDISPATCH FAR* ppDisp, BOOL FAR* Cancel);
	afx_msg void OnWindowClosingExplorer1(BOOL IsChildWindow, BOOL FAR* Cancel);
	afx_msg void OnNcDestroy();
	afx_msg void OnPaint();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASSISWND_H__D58E5FE4_4815_4189_8FE3_9225045E9D1B__INCLUDED_)

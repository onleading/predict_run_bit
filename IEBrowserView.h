#if !defined(AFX_IEBROWSERVIEW_H__FFB12198_BE4A_451C_8268_94FCAD714BB8__INCLUDED_)
#define AFX_IEBROWSERVIEW_H__FFB12198_BE4A_451C_8268_94FCAD714BB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IEBrowserView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIEBrowserView html view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include <afxhtml.h>
#include "mshtml.h"
#include <afxdisp.h>
#include <atlbase.h>

#include "MyToolbar.h"

class CIEBrowserView : public CHtmlView
{
protected:
	CIEBrowserView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CIEBrowserView)

// html Data
public:
	//{{AFX_DATA(CIEBrowserView)
	//}}AFX_DATA
	short	GetCurrentGPIndex();

// Attributes
public:
	CMyToolBar	m_wndIEToolBar;
	BOOL		m_bQuitting;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIEBrowserView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnInitialUpdate();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnDestroy();
	afx_msg void OnWindowClosingExplorer1(BOOL IsChildWindow, BOOL FAR* Cancel);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CIEBrowserView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CString m_strNeedUnlinkFile;
	// Generated message map functions
	//{{AFX_MSG(CIEBrowserView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWebBack();
	afx_msg void OnWebForward();
	afx_msg void OnWebStop();
	afx_msg void OnWebRefresh();
	afx_msg void OnWebClose();
	afx_msg void OnWebInfo_Message(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IEBROWSERVIEW_H__FFB12198_BE4A_451C_8268_94FCAD714BB8__INCLUDED_)

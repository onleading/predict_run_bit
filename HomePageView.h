#if !defined(AFX_HOMEPAGEVIEW_H__FFB12198_BE4A_451C_8268_94FCAD714BB8__INCLUDED_)
#define AFX_HOMEPAGEVIEW_H__FFB12198_BE4A_451C_8268_94FCAD714BB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HomePageView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHomePageView html view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include <afxhtml.h>
#include "mshtml.h"
#include <afxdisp.h>
#include <atlbase.h>

#define MAX_HOMEPAGE_MID_NUM	20

class CHomePageView : public CHtmlView
{
	BOOL	KeyGuyFlag;
	BOOL	m_bQuitting;

protected:
	CHomePageView();
	DECLARE_DYNCREATE(CHomePageView)

// html Data
public:
	//{{AFX_DATA(CHomePageView)
	//}}AFX_DATA
	short	GetCurrentGPIndex();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHomePageView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnInitialUpdate();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnDestroy();
	virtual void OnTimer(UINT nIDEvent);

	afx_msg void OnReloadMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnWindowClosingExplorer1(BOOL IsChildWindow, BOOL FAR* Cancel);
	afx_msg void OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel);	
	DECLARE_EVENTSINK_MAP()
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CHomePageView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CString m_strNeedUnlinkFile;
	// Generated message map functions
	//{{AFX_MSG(CHomePageView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

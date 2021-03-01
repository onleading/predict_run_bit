// CG: This file was added by the Splash Screen component.

#ifndef _SPLASH_SCRN_
#define _SPLASH_SCRN_

// Splash.h : header file
//

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

class CSplashWnd : public CWnd
{
// Construction
public:
	CSplashWnd();
	~CSplashWnd();

// Attributes:
public:
	CBitmap m_bitmap;
	HANDLE  hBitmap;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashWnd)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

protected:
	BOOL Create(CWnd* pParentWnd = NULL);

// Generated message map functions
protected:
	//{{AFX_MSG(CSplashWnd)
	afx_msg BOOL OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnClose();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif

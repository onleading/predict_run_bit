//{{AFX_INCLUDES()
#include "webbrowser2.h"
//}}AFX_INCLUDES
#if !defined(AFX_SHOWURL_H__2E5983D5_5BE2_4B57_9198_8CC2EA3A3184__INCLUDED_)
#define AFX_SHOWURL_H__2E5983D5_5BE2_4B57_9198_8CC2EA3A3184__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowURL.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShowURL dialog

class CShowURL : public CDialog
{
// Construction
public:
	CShowURL(CWnd* pParent = NULL,BOOL bGPMutal = FALSE,BOOL bServiceWeb = FALSE);   // standard constructor
	void SetTitle(const char *pTitle);
	BOOL SetInfo(int nCx,int nCy,const char *pURL,int nGPNo=-1);
	void ChangeStock(int nGPNo);
	void StartNavigate();
	void StartNavigate(CString Url);
// Dialog Data
	//{{AFX_DATA(CShowURL)
	enum { IDD = IDD_SHOWURL };
	CWebBrowser2	m_IE;
	//}}AFX_DATA
	CString			m_strTitle;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShowURL)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON			m_hIcon;
	BOOL			m_bLayouted;

	CString			m_URL;
	int				m_nCx;
	int				m_nCy;

	int				m_nCurGP;
	BOOL			m_bGPMutal;
	BOOL			m_bServiceWeb;
	// Generated message map functions
	//{{AFX_MSG(CShowURL)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWURL_H__2E5983D5_5BE2_4B57_9198_8CC2EA3A3184__INCLUDED_)

//{{AFX_INCLUDES()
#include "webbrowser2.h"
//}}AFX_INCLUDES
#if !defined(AFX_KCLOGODLG_H__888A2482_9447_45FF_8013_F8959F2D66B7__INCLUDED_)
#define AFX_KCLOGODLG_H__888A2482_9447_45FF_8013_F8959F2D66B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KCLogoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKCLogoDlg dialog

class CKCLogoDlg : public CDialog
{
// Construction
public:
	CKCLogoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CKCLogoDlg)
	enum { IDD = IDD_KCLOGO_DIALOG };
	CWebBrowser2	m_IE;
	//}}AFX_DATA
	CString			m_Url;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKCLogoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKCLogoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KCLOGODLG_H__888A2482_9447_45FF_8013_F8959F2D66B7__INCLUDED_)

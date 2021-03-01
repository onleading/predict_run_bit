#if !defined(AFX_WAITDLG_H__C4B78FB3_1FF9_4E43_A8FD_4B4BE5B13E30__INCLUDED_)
#define AFX_WAITDLG_H__C4B78FB3_1FF9_4E43_A8FD_4B4BE5B13E30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// waitdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWaitDlg dialog

#include "label.h"
#include "resource.h"
class CWaitDlg : public CDialog
{
// Construction
public:
	CWaitDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaitDlg)
	enum { IDD = IDD_WAIT_DIALOG };
	CLabel	m_Time_Static;
	CLabel	m_Hint_Static;
	//}}AFX_DATA
	CString	m_strCaption;
	CString	m_strContent;
	UINT	m_nWaitSeconds;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAITDLG_H__C4B78FB3_1FF9_4E43_A8FD_4B4BE5B13E30__INCLUDED_)

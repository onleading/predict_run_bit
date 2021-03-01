#if !defined(AFX_USTART_H__FBAF2D5B_394F_45D9_A389_E0A11674D06F__INCLUDED_)
#define AFX_USTART_H__FBAF2D5B_394F_45D9_A389_E0A11674D06F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ustart.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUStart dialog

#include "Label.h"

class CUStart : public CDialog
{
	BOOL RegUser();
public:
	CUStart(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUStart)
	enum { IDD = IDD_U_START };
	CLabel	m_Hint_Status;
	CLabel	m_Status_Static;
	//}}AFX_DATA

	CString	m_strUserName;
	CString	m_strPassword;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUStart)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUStart)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_USTART_H__FBAF2D5B_394F_45D9_A389_E0A11674D06F__INCLUDED_)

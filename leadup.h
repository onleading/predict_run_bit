#if !defined(AFX_LEADUP_H__1D39ADF1_28F5_41FA_BFA3_595DE61DC627__INCLUDED_)
#define AFX_LEADUP_H__1D39ADF1_28F5_41FA_BFA3_595DE61DC627__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// leadup.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLeadUp dialog

class CLeadUp : public CDialog
{
// Construction
public:
	CLeadUp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLeadUp)
	enum { IDD = IDD_LEADUP };
	BOOL	m_bAsk;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLeadUp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLeadUp)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEADUP_H__1D39ADF1_28F5_41FA_BFA3_595DE61DC627__INCLUDED_)

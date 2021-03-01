#if !defined(AFX_DLGLOGOUT_H__B8E451CB_ABDD_4E42_BC2F_D5B591BC6470__INCLUDED_)
#define AFX_DLGLOGOUT_H__B8E451CB_ABDD_4E42_BC2F_D5B591BC6470__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDlgLogout : public CDialog
{
// Construction
public:
	CDlgLogout(CWnd* pParent = NULL);   // standard constructor

// Dialog Data 
	//{{AFX_DATA(CDlgLogout)
	enum { IDD = IDD_DLGLOGOUT };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLogout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgLogout)
	afx_msg void OnBtnexit();
	afx_msg void OnBtnrelogin();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int		m_nExitCode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLOGOUT_H__B8E451CB_ABDD_4E42_BC2F_D5B591BC6470__INCLUDED_)

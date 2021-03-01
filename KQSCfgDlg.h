#if !defined(AFX_KQSCFGDLG_H__FD1B7A83_2992_44F6_A09A_BA4BEE4E6DD3__INCLUDED_)
#define AFX_KQSCFGDLG_H__FD1B7A83_2992_44F6_A09A_BA4BEE4E6DD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KQSCfgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKQSCfgDlg dialog

class CKQSCfgDlg : public CDialog
{
// Construction
public:
	CKQSCfgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CKQSCfgDlg)
	enum { IDD = IDD_KQSCFG };
	int		m_b10;
	BOOL	m_bRelation;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKQSCfgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKQSCfgDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRadio1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KQSCFGDLG_H__FD1B7A83_2992_44F6_A09A_BA4BEE4E6DD3__INCLUDED_)

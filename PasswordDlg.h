#if !defined(AFX_PASSWORDDLG_H__7CB05681_1871_11D5_B99B_00E04C3DCAC5__INCLUDED_)
#define AFX_PASSWORDDLG_H__7CB05681_1871_11D5_B99B_00E04C3DCAC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PasswordDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog

class CPasswordDlg : public CDialog
{
// Construction
public:
	CString m_strInputPassword;
	CPasswordDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL	m_bFomulaCompareBin;
	char	m_lpszPassword[PASSWORD_LEN];
// Dialog Data
	//{{AFX_DATA(CPasswordDlg)
	enum { IDD = IDD_PASSWORDDLG };
	CEdit	m_Edit;
	CString	m_strName;
	CString	m_strPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasswordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPasswordDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSWORDDLG_H__7CB05681_1871_11D5_B99B_00E04C3DCAC5__INCLUDED_)

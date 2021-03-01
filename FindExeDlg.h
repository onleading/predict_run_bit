#if !defined(AFX_FINDEXEDLG_H__AC82F781_4CDB_11D4_BB99_00E04C66B1EB__INCLUDED_)
#define AFX_FINDEXEDLG_H__AC82F781_4CDB_11D4_BB99_00E04C66B1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindWtDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindExeDlg dialog

class CFindExeDlg : public CDialog
{
// Construction
public:
	CFindExeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFindExeDlg)
	enum { IDD = IDD_FINDWT_DIALOG };
	CString	m_strExePath;
	CString	m_strCaption;
	CString	m_strExeInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindExeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindExeDlg)
	afx_msg void OnBrowse();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDEXEDLG_H__AC82F781_4CDB_11D4_BB99_00E04C66B1EB__INCLUDED_)

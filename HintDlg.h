#if !defined(AFX_HINTDLG_H__371916A5_398A_4CAB_8647_5A82F38994C9__INCLUDED_)
#define AFX_HINTDLG_H__371916A5_398A_4CAB_8647_5A82F38994C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HintDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHintDlg dialog

class CHintDlg : public CDialog
{
// Construction
public:
	CHintDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHintDlg)
	enum { IDD = IDD_HINT_DIALOG };
	BOOL	m_bAlwaysHint;
	//}}AFX_DATA

	CString	m_strHint;
	CString m_strCheck;
	BOOL	m_bHasCheck;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHintDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHintDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HINTDLG_H__371916A5_398A_4CAB_8647_5A82F38994C9__INCLUDED_)

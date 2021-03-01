#if !defined(AFX_TIPMARKDLG_H__C24B00AC_6FB7_443B_A49C_07EB28582DAB__INCLUDED_)
#define AFX_TIPMARKDLG_H__C24B00AC_6FB7_443B_A49C_07EB28582DAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TipMarkDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTipMarkDlg dialog

class CTipMarkDlg : public CDialog
{
// Construction
public:
	CTipMarkDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTipMarkDlg)
	enum { IDD = IDD_TIPMARKDLG };
	CString	m_strTip;
	//}}AFX_DATA

	MemStkInfo	*m_pStock;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTipMarkDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTipMarkDlg)
	virtual void OnOK();
	afx_msg void OnDeltipmark();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIPMARKDLG_H__C24B00AC_6FB7_443B_A49C_07EB28582DAB__INCLUDED_)

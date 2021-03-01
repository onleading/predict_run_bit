#if !defined(AFX_GRIGHZPARADLG_H__AE9D513F_4C4A_4B64_BB55_FF2B77413DDD__INCLUDED_)
#define AFX_GRIGHZPARADLG_H__AE9D513F_4C4A_4B64_BB55_FF2B77413DDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GrigHzParaDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGrigHzParaDlg dialog

class CGrigHzParaDlg : public CDialog
{
// Construction
public:
	CGrigHzParaDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGrigHzParaDlg)
	enum { IDD = IDD_GRIDHZPARAMDLG };
	int		m_nQzType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGrigHzParaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGrigHzParaDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIGHZPARADLG_H__AE9D513F_4C4A_4B64_BB55_FF2B77413DDD__INCLUDED_)

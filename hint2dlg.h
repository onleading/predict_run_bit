#if !defined(AFX_HINT2DLG_H__016A79C6_0506_4592_B587_05FD691AE480__INCLUDED_)
#define AFX_HINT2DLG_H__016A79C6_0506_4592_B587_05FD691AE480__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// hint2dlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHint2Dlg dialog

class CHint2Dlg : public CDialog
{
// Construction
public:
	CHint2Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHint2Dlg)
	enum { IDD = IDD_HINT2_DIALOG };
	BOOL	m_bCheck;
	//}}AFX_DATA

	CString	m_strHint;
	CString m_strCheck;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHint2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHint2Dlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HINT2DLG_H__016A79C6_0506_4592_B587_05FD691AE480__INCLUDED_)

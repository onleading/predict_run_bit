#if !defined(AFX_OTHERWTDLG_H__B0759355_6785_46FA_B029_9D020E4ED382__INCLUDED_)
#define AFX_OTHERWTDLG_H__B0759355_6785_46FA_B029_9D020E4ED382__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OTHERWTDLG.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COtherWTDlg dialog

class COtherWTDlg : public CDialog
{
// Construction
public:
	COtherWTDlg(CWnd* pParent = NULL);   // standard constructor
	CString m_title;
// Dialog Data
	//{{AFX_DATA(COtherWTDlg)
	enum { IDD = IDD_DIALOG_OTHERWT };
	CString	m_strWtPath;
	CString	m_strWtName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COtherWTDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COtherWTDlg)
	afx_msg void OnButton1();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OTHERWTDLG_H__B0759355_6785_46FA_B029_9D020E4ED382__INCLUDED_)

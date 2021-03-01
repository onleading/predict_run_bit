#if !defined(AFX_INDEXINFODLG_H__71832D01_6A26_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_INDEXINFODLG_H__71832D01_6A26_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IndexInfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIndexInfoDlg dialog

class CIndexInfoDlg : public CDialog
{
// Construction
public:
	CIndexInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIndexInfoDlg)
	enum { IDD = IDD_INDEXINFO_DIALOG };
	CString	m_IndexInfo;
	//}}AFX_DATA

	CString TitleStr;
	BOOL    bReadOnly;
	BOOL    HasOKButton;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIndexInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIndexInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INDEXINFODLG_H__71832D01_6A26_11D3_B921_0000800104D5__INCLUDED_)

#if !defined(AFX_WEBPAGEDLG_H__CFE4EF95_FA3D_4C12_A953_61E9A1B5AF78__INCLUDED_)
#define AFX_WEBPAGEDLG_H__CFE4EF95_FA3D_4C12_A953_61E9A1B5AF78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WebPageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWebPageDlg dialog

class CWebPageDlg : public CDialog
{
// Construction
public:
	CWebPageDlg(CWnd* pParent = NULL);   // standard constructor
//Attributes
public:
	CString m_title;

// Dialog Data
	//{{AFX_DATA(CWebPageDlg)
	enum { IDD = IDD_DIALOG_WEBPAGE };
	CString	m_name;
	CString	m_url;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWebPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWebPageDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEBPAGEDLG_H__CFE4EF95_FA3D_4C12_A953_61E9A1B5AF78__INCLUDED_)

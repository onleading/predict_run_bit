#if !defined(AFX_GETTXTDLG_H__D14630E6_B8C4_4D14_B6C3_9804ADDA103C__INCLUDED_)
#define AFX_GETTXTDLG_H__D14630E6_B8C4_4D14_B6C3_9804ADDA103C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetTxtDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGetTxtDlg dialog

class CGetTxtDlg : public CDialog
{
// Construction
public:
	CGetTxtDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetTxtDlg)
	enum { IDD = IDD_GETTXT_DIALOG };
	CRichEditCtrl	m_Content_Edit;
	//}}AFX_DATA
	CString	m_strContent;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetTxtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetTxtDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnFromfile();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETTXTDLG_H__D14630E6_B8C4_4D14_B6C3_9804ADDA103C__INCLUDED_)

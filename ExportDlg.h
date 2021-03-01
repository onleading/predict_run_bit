#if !defined(AFX_EXPORTDLG_H__12DF7781_EE01_11D4_BB9D_00E04C66B1EB__INCLUDED_)
#define AFX_EXPORTDLG_H__12DF7781_EE01_11D4_BB9D_00E04C66B1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExportDlg.h : header file
//
#include "Label.h"
/////////////////////////////////////////////////////////////////////////////
// CExportDlg dialog

class CExportDlg : public CDialog
{
	void ShowOrHide();
public:
	CExportDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExportDlg)
	enum { IDD = IDD_EXPORT_DIALOG };
	CLabel	m_Info_Static;
	CTabCtrl	m_TabCtrl;
	CString	m_strFileName;
	int		m_bAllHQ;
	//}}AFX_DATA
	
	int		m_nTabSel;
	BOOL	m_bGrid;
	CString m_strInitFileName;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExportDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnBrowse();
	afx_msg void OnAdvance();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORTDLG_H__12DF7781_EE01_11D4_BB9D_00E04C66B1EB__INCLUDED_)

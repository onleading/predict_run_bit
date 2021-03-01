#if !defined(AFX_SETCBDLG_H__0F0C1BD4_4973_4361_97EC_A61C4662CA4D__INCLUDED_)
#define AFX_SETCBDLG_H__0F0C1BD4_4973_4361_97EC_A61C4662CA4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetCBDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetCBDlg dialog
#include "resource.h"
#include "UCb.h"
class CSetCBDlg : public CDialog
{
// Construction
public:
	CSetCBDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetCBDlg)
	enum { IDD = IDD_SETUCB };
	CListBox	m_ctlList;
	CTabCtrl	m_ctlTab;
	float	m_fDown;
	int		m_nParam;
	int		m_nWay;
	//}}AFX_DATA

	ENVIORSETUP	m_Setup;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetCBDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	void	SaveSet();
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetCBDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAdd();
	afx_msg void OnSub();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETCBDLG_H__0F0C1BD4_4973_4361_97EC_A61C4662CA4D__INCLUDED_)

//{{AFX_INCLUDES()
#include "webbrowser2.h"
//}}AFX_INCLUDES
#if !defined(AFX_URGENTDLG_H__3A7639A1_5E78_11D4_BB99_00E04C66B1EB__INCLUDED_)
#define AFX_URGENTDLG_H__3A7639A1_5E78_11D4_BB99_00E04C66B1EB__INCLUDED_
#include "webbrowser2.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UrgentDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUrgentDlg dialog

struct Mul_Urgent
{
	char Title[41];
	char FileName[41];
};
#define MAX_MULURGENT_NUM	20

class CUrgentDlg : public CDialog
{
	void ShowContent(CString strContent,BOOL bWeb);
public:
	CUrgentDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUrgentDlg)
	enum { IDD = IDD_URGENTDIALOG };
	CTabCtrl	m_MulTab;
	CRichEditCtrl	m_RichEditEx;
	CString	m_strContent;
	BOOL	m_bCheckUrgent;
	//}}AFX_DATA
	CString m_strTitle;

	CWebBrowser2	m_IE;

	BOOL	m_bHQHostUrgent;
	BOOL	m_bWeb;
	long	lEndDate;
	BOOL	m_bEnterPop;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUrgentDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON	m_hIcon;

	struct Mul_Urgent m_MulUrgent[MAX_MULURGENT_NUM];
	int		m_nMulNum;

	CRect	m_FullRect;
	CRect	m_NoFullRect;
	// Generated message map functions
	//{{AFX_MSG(CUrgentDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnRichEditExLink( NMHDR* in_pNotifyHeader, LRESULT* out_pResult );
	afx_msg void OnShowInfoMsg(WPARAM wParam,LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChecktoday();
	afx_msg void OnSaveit();
	afx_msg void OnSelchangeMulTab(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_URGENTDLG_H__3A7639A1_5E78_11D4_BB99_00E04C66B1EB__INCLUDED_)

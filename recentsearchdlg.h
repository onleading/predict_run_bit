#if !defined(AFX_RECENTSEARCHDLG_H__ECE034C0_6ED3_415C_803C_6701E80A5122__INCLUDED_)
#define AFX_RECENTSEARCHDLG_H__ECE034C0_6ED3_415C_803C_6701E80A5122__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// recentsearchdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRecentSearchDlg dialog

class CRecentSearchDlg : public CDialog
{
// Construction
public:
	CRecentSearchDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRecentSearchDlg)
	enum { IDD = IDD_RECENTSEARCH };
	CListBox	m_Recent_List;
	//}}AFX_DATA

	int			m_ItemData;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecentSearchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRecentSearchDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkRecentList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECENTSEARCHDLG_H__ECE034C0_6ED3_415C_803C_6701E80A5122__INCLUDED_)

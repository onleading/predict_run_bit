#if !defined(AFX_WTPATHSELDLG_H__774862D5_A59E_479D_96D8_C27DDD6951F0__INCLUDED_)
#define AFX_WTPATHSELDLG_H__774862D5_A59E_479D_96D8_C27DDD6951F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WTPathSelDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWTPathSelDlg dialog

class CWTPathSelDlg : public CDialog
{
// Construction
public:
	CWTPathSelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWTPathSelDlg)
	enum { IDD = IDD_WTPATHSEL };
	CListCtrl	m_ExePathList;
	//}}AFX_DATA
	
	BOOL		m_bFromFileCfg;
	char		m_strWTName[3][20];
	int			m_nWTNum;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWTPathSelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList m_ImageList;
	// Generated message map functions
	//{{AFX_MSG(CWTPathSelDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkWtpathlist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int m_nSelExeIndex;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WTPATHSELDLG_H__774862D5_A59E_479D_96D8_C27DDD6951F0__INCLUDED_)

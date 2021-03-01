#if !defined(AFX_INILISTDLG_H__563C5801_C732_4332_ACF7_7D359E64234C__INCLUDED_)
#define AFX_INILISTDLG_H__563C5801_C732_4332_ACF7_7D359E64234C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IniListDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIniListDlg dialog

class CIniListDlg : public CDialog
{
// Construction
public:
	CIniListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIniListDlg)
	enum { IDD = IDD_DLG_INILIST };
	CListCtrl	m_ListINI;
	//}}AFX_DATA

	//input
	short		m_nType;		//1图或0表
	short		m_nStyle;		//图的类型

	//output
	CString		m_strOutIniPath;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIniListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIniListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnOpenini();
	afx_msg void OnDblclkListGncfg(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	WORD GetIniFileInfo( CString strFilePath );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INILISTDLG_H__563C5801_C732_4332_ACF7_7D359E64234C__INCLUDED_)

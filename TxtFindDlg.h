#if !defined(AFX_TXTFINDDLG_H__4D2C4DA9_EBA4_11D4_BB9D_00E04C66B1EB__INCLUDED_)
#define AFX_TXTFINDDLG_H__4D2C4DA9_EBA4_11D4_BB9D_00E04C66B1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TxtFindDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTxtFindDlg dialog
#define TXTCACHE_NUM	20

class CTxtFindDlg : public CDialog
{
// Construction
public:
	CTxtFindDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTxtFindDlg)
	enum { IDD = IDD_TXTFIND_DIALOG };
	CComboBox	m_FindTxt_Combo;
	int		m_nRadio;
	//}}AFX_DATA

	CString	m_strFindTxt;	
	CString m_Str1,m_Str2;
	BOOL	m_bEnable1,m_bEnable2;

	CString	m_FindStr[MAX_ANDSTR];
	int		m_nFindNum;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTxtFindDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTxtFindDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TXTFINDDLG_H__4D2C4DA9_EBA4_11D4_BB9D_00E04C66B1EB__INCLUDED_)

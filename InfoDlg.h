#if !defined(AFX_INFODLG_H__13A63725_DB6F_11D4_BB9D_00E04C66B1EB__INCLUDED_)
#define AFX_INFODLG_H__13A63725_DB6F_11D4_BB9D_00E04C66B1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InfoDlg.h : header file
/////////////////////////////////////////////////////////////////////////////
#include "Label.h"

class CInfoDlg : public CDialog
{
public:
	CInfoDlg(CWnd* pParent = NULL, int nUF = 0);   // standard constructor
	virtual ~CInfoDlg();
// Dialog Data
	//{{AFX_DATA(CInfoDlg)
	enum { IDD = IDD_INFO_DIALOG };
	CLabel	m_Volume_Static;
	CLabel	m_Zanfu_Static;
	CLabel	m_Min_Static;
	CLabel	m_Close_Static;
	CLabel	m_Max_Static;
	CLabel	m_Open_Static;
	CString	m_strAmount;
	CString	m_strHsl;
	CString	m_strTime1;
	CString	m_strValue;
	CString	m_strZhenFu;
	CString	m_strLtgb;
	//}}AFX_DATA

	int	m_nStyle;
	
	CBrush	m_br;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd	* pView;     //用以得到SendMessage的指针
	int		  m_nF;
	// Generated message map functions
	//{{AFX_MSG(CInfoDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnInfoDlgNotify(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INFODLG_H__13A63725_DB6F_11D4_BB9D_00E04C66B1EB__INCLUDED_)

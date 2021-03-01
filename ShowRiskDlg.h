#if !defined(AFX_ShowRisktDlg_H__D14630E6_B8C4_4D14_B6C3_9804ADDA103C__INCLUDED_)
#define AFX_ShowRisktDlg_H__D14630E6_B8C4_4D14_B6C3_9804ADDA103C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowRisktDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShowRisktDlg dialog

class CShowRisktDlg : public CDialog
{
// Construction
public:
	CShowRisktDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShowRisktDlg)
	enum { IDD = IDD_RISK_DIALOG };
	CRichEditCtrl	m_Content_Edit;
	BOOL	m_bRiskCheck;
	//}}AFX_DATA
	CString	m_strContent;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShowRisktDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON	m_hIcon;
	// Generated message map functions
	//{{AFX_MSG(CShowRisktDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRichEditExLink( NMHDR* in_pNotifyHeader, LRESULT* out_pResult );
	afx_msg void OnCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ShowRisktDlg_H__D14630E6_B8C4_4D14_B6C3_9804ADDA103C__INCLUDED_)

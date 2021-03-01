#if !defined(AFX_TJWARNCONFDLG_H__41DFECA6_E6A8_4A9E_AEDA_60ACDFDFA142__INCLUDED_)
#define AFX_TJWARNCONFDLG_H__41DFECA6_E6A8_4A9E_AEDA_60ACDFDFA142__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TjWarnConfDlg.h : header file
//
#include "DescFormDlg.h"
#include "Label.h"
/////////////////////////////////////////////////////////////////////////////
// CTjWarnConfDlg dialog

class CTjWarnConfDlg : public CDialog
{
// Construction
public:
	CTjWarnConfDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTjWarnConfDlg)
	enum { IDD = IDD_WARNTJCONF };
	CComboBox	m_SelPeriod;
	CComboBox	m_SelTj;
	CLabel	m_CStatus;
	//}}AFX_DATA
	BOOL	m_bModefied;
	TJ_WARN m_warnTjConf;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTjWarnConfDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTjWarnConfDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTjexplain();
	afx_msg void OnSelchangeTjsel();
	afx_msg void OnSelchangePeriodsel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CDescFormDlg m_Form;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TJWARNCONFDLG_H__41DFECA6_E6A8_4A9E_AEDA_60ACDFDFA142__INCLUDED_)

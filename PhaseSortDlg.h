#if !defined(AFX_PHASESORTDLG_H__54405B30_CBD2_4F79_88E6_3C48BC94B33A__INCLUDED_)
#define AFX_PHASESORTDLG_H__54405B30_CBD2_4F79_88E6_3C48BC94B33A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PhaseSortDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "Label.h"

class CPhaseSortDlg : public CDialog
{
	void FillCombo();
public:
	CPhaseSortDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPhaseSortDlg)
	enum { IDD = IDD_PHASESORT_DIALOG };
	CLabel	m_Status_Static;
	CComboBox	m_FLCombo;
	CTime	m_BgnTime;
	CTime	m_EndTime;
	int		m_nQjType;
	int		m_nFLType;
	BOOL	m_bHisViewTQ;
	//}}AFX_DATA
	long	m_lQJEndDate;
	long	m_lQJBgnDate;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhaseSortDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPhaseSortDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnRadio5();
	afx_msg void OnRadio6();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHASESORTDLG_H__54405B30_CBD2_4F79_88E6_3C48BC94B33A__INCLUDED_)

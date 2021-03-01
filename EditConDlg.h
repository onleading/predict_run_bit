#if !defined(AFX_EDITCONDLG_H__307A58E1_7823_11D4_BB99_00E04C66B1EB__INCLUDED_)
#define AFX_EDITCONDLG_H__307A58E1_7823_11D4_BB99_00E04C66B1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditConDlg.h : header file
/////////////////////////////////////////////////////////////////////////////

#include "Label.h"

class CEditConDlg : public CDialog
{
	void EnableOrDisable(int flag);
public:
	CEditConDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditConDlg)
	enum { IDD = IDD_EDITCON_DIALOG };
	CStatic	m_Unit_Static;
	CLabel	m_Topic_Static;
	CComboBox	m_Sel_Combo;
	CComboBox	m_Prefix_Combo;
	float	m_fValue;
	BOOL	m_bConCheck;
	int		m_nValue;
	//}}AFX_DATA
	
	BOOL bAdd;
	CUSTXG_CONTABLE *tmpConTable;
	USER_CONS		*tmpUserCons;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditConDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditConDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITCONDLG_H__307A58E1_7823_11D4_BB99_00E04C66B1EB__INCLUDED_)

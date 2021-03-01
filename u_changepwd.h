#if !defined(AFX_U_CHANGEPWD_H__D2CC44F9_3666_4C4D_A72F_2D9DAEED3AA3__INCLUDED_)
#define AFX_U_CHANGEPWD_H__D2CC44F9_3666_4C4D_A72F_2D9DAEED3AA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// u_changepwd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CU_ChangePwd dialog

#include "Label.h"
class CU_ChangePwd : public CDialog
{
// Construction
public:
	CU_ChangePwd(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CU_ChangePwd)
	enum { IDD = IDD_U_CHANGEPWD };
	CLabel	m_Hint_Static;
	CString	m_strPassword;
	CString	m_strPassword2;
	CString	m_strOldPassword;
	//}}AFX_DATA
	BOOL	m_bDelMySelf;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CU_ChangePwd)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CU_ChangePwd)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_U_CHANGEPWD_H__D2CC44F9_3666_4C4D_A72F_2D9DAEED3AA3__INCLUDED_)

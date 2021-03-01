#if !defined(AFX_U_SELREG_H__83A5ACB1_1DE1_4742_AF66_8721DEDB6C50__INCLUDED_)
#define AFX_U_SELREG_H__83A5ACB1_1DE1_4742_AF66_8721DEDB6C50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// u_selreg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CU_SelReg dialog

#include "Label.h"
class CU_SelReg : public CDialog
{
	void EnableOther();
public:
	CU_SelReg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CU_SelReg)
	enum { IDD = IDD_U_SELREG };
	CLabel	m_Hint_Status;
	CString	m_strPassword;
	CString	m_strUserName;
	int		m_nRegisterType;
	CString	m_strPassword2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CU_SelReg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CU_SelReg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRadio1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_U_SELREG_H__83A5ACB1_1DE1_4742_AF66_8721DEDB6C50__INCLUDED_)

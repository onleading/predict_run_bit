#if !defined(AFX_U_TESTZJZH_H__2DB9D16E_1C81_4516_B1D4_ED0D245F1F37__INCLUDED_)
#define AFX_U_TESTZJZH_H__2DB9D16E_1C81_4516_B1D4_ED0D245F1F37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// u_testzjzh.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CU_TestZjzh dialog
#include "Label.h"
class CU_TestZjzh : public CDialog
{
public:
	CU_TestZjzh(CWnd* pParent = NULL);  // standard constructor
	~CU_TestZjzh();						// standard constructor
// Dialog Data
	//{{AFX_DATA(CU_TestZjzh)
	enum { IDD = IDD_U_TESTZJZH };
	CComboBox	m_WTType_Combo;
	CLabel	m_Hint_Status;
	CComboBox	m_YYBInfo_Combo;
	CString	m_strPassWord;
	CString	m_strZjzh;
	CString m_strYYBInfo;
	//}}AFX_DATA
	long	m_WTTypeID;

	BOOL	m_bJyLogin;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CU_TestZjzh)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CU_TestZjzh)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeWttypeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_U_TESTZJZH_H__2DB9D16E_1C81_4516_B1D4_ED0D245F1F37__INCLUDED_)

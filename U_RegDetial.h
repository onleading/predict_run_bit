#if !defined(AFX_U_REGDETIAL_H__2D7CB691_3C0C_426B_92B9_D087417244B4__INCLUDED_)
#define AFX_U_REGDETIAL_H__2D7CB691_3C0C_426B_92B9_D087417244B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// U_RegDetial.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CU_RegDetial dialog

class CU_RegDetial : public CDialog
{
// Construction
public:
	CU_RegDetial(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CU_RegDetial)
	enum { IDD = IDD_U_REGDETIAL };
	CComboBox	m_sex_combo;
	CString	m_address;
	CTime	m_birth_time;
	CString	m_handscrpt;
	CString	m_nickname;
	CString	m_otherlx;
	CString	m_phone;
	int		m_sex;
	CString	m_truename;
	//}}AFX_DATA
	long	m_birth;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CU_RegDetial)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CU_RegDetial)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_U_REGDETIAL_H__2D7CB691_3C0C_426B_92B9_D087417244B4__INCLUDED_)

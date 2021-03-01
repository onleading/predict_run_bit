#if !defined(AFX_U_SELUSER_H__61434628_44CD_47EB_B5FB_AAFBC9F294F1__INCLUDED_)
#define AFX_U_SELUSER_H__61434628_44CD_47EB_B5FB_AAFBC9F294F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// u_seluser.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CU_SelUser dialog

class CU_SelUser : public CDialog
{
// Construction
public:
	CU_SelUser(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CU_SelUser)
	enum { IDD = IDD_U_SELUSER };
	CListBox	m_User_List;
	//}}AFX_DATA
	
	char	m_User[5][50];
	int		m_nUserNum;

	CString m_strSelUser;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CU_SelUser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CU_SelUser)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkUserList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_U_SELUSER_H__61434628_44CD_47EB_B5FB_AAFBC9F294F1__INCLUDED_)

#if !defined(AFX_EXTERNPAGE_H__1EC5C34F_D297_4185_936A_7A3749A7C94F__INCLUDED_)
#define AFX_EXTERNPAGE_H__1EC5C34F_D297_4185_936A_7A3749A7C94F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// externpage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExternPage dialog
typedef struct otherwt 
{
	CString wtName;
	CString wtPath;
}OTHERWT;
typedef std::vector<OTHERWT> VOTHERWT;

typedef struct webpage
{
	CString name;
	CString	url;
}WEBPAGE;
typedef std::vector<WEBPAGE> VWEBPAGE;

class CExternPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CExternPage)

// Construction
public:
	CExternPage();
	~CExternPage();

	VOTHERWT m_vOtherWt;
	void WriteOtherWT();
	VWEBPAGE m_vWebpage;
	void WriteWebpage();
// Dialog Data
	//{{AFX_DATA(CExternPage)
	enum { IDD = IDD_EXTERN_PAGE };
	CComboBox	m_webpage_comb;
	CComboBox	m_otherwt_comb;
	BOOL	m_bExtern_Info;
	BOOL	m_bExtern_Invest;
	CString	m_strInfoExe;
	CString	m_strInvestExe;
	CString	m_strWtPath;
	BOOL	m_bInsideOpenPage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CExternPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CExternPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse1();
	afx_msg void OnBrowse2();
	afx_msg void OnExternInvest();
	afx_msg void OnExternInfo();
	afx_msg void OnBrowse4();
	afx_msg void OnOtherwtAdd();
	afx_msg void OnOtherwtMod();
	afx_msg void OnOtherwtDel();
	afx_msg void OnWebpageAdd();
	afx_msg void OnWebpageMod();
	afx_msg void OnWebpageDel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXTERNPAGE_H__1EC5C34F_D297_4185_936A_7A3749A7C94F__INCLUDED_)

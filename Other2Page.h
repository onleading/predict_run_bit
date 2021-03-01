#if !defined(AFX_OTHER2PAGE_H__392293A0_D791_420B_B2A7_81214D68F7E6__INCLUDED_)
#define AFX_OTHER2PAGE_H__392293A0_D791_420B_B2A7_81214D68F7E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Other2Page.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COther2Page dialog

class COther2Page : public CPropertyPage
{
	DECLARE_DYNCREATE(COther2Page)

// Construction
public:
	COther2Page();
	~COther2Page();

// Dialog Data
	//{{AFX_DATA(COther2Page)
	enum { IDD = IDD_OTHER2_PAGE };
	CButton	m_SubButtonCtrl;
	CButton	m_AddButtonCtrl;
	CListCtrl	m_AllzbList;
	CListCtrl	m_OftenList;
	CString	m_strMockPath;
	int		m_nMode;
	int		m_nNetCondition;
	//}}AFX_DATA

	BOOL	m_bChangeZB;
	int		m_nOftenUseZb;
	char	m_strOftenUseZb[22][ZBCODE_LEN];
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COther2Page)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList m_ImageList;
	// Generated message map functions
	//{{AFX_MSG(COther2Page)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnBrowse2();
	afx_msg void OnUpButton();
	afx_msg void OnDownButton();
	afx_msg void OnSubButton();
	afx_msg void OnDblclkAllzbList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkOftenuseList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OTHER2PAGE_H__392293A0_D791_420B_B2A7_81214D68F7E6__INCLUDED_)

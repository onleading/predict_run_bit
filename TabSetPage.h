#if !defined(AFX_GRIDTABSETDLG_H__F3F95091_412C_46AA_888B_1B06BB448466__INCLUDED_)
#define AFX_GRIDTABSETDLG_H__F3F95091_412C_46AA_888B_1B06BB448466__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GridTabSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTabSetPage dialog

class CTabSetPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTabSetPage)
// Construction
public:
	CTabSetPage();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTabSetPage)
	enum { IDD = IDD_GIDTABSET };
	CTreeCtrl	m_AllItemTree;
	CListCtrl	m_UseItemList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabSetPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList m_ImageList;
	vector<GRIDCATITEM> m_SysDefItems;
	// Generated message map functions
	//{{AFX_MSG(CTabSetPage)
	afx_msg void OnAddButton();
	afx_msg void OnUpButton();
	afx_msg void OnDownButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnDelButton();
	afx_msg void OnDblclkUsetablist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTodef();
	afx_msg void OnDblclkAllitemtree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void FillAllItemTree(vector<GRIDCATITEM> TheDefItems);
	BOOL GetGridItem(GRIDCATITEM &gci, HTREEITEM htm);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDTABSETDLG_H__F3F95091_412C_46AA_888B_1B06BB448466__INCLUDED_)

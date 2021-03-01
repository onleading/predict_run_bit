#if !defined(AFX_SHORTCUTPAGE_H__64E9D8CA_474E_4018_BE3F_30AE5FCFEE3C__INCLUDED_)
#define AFX_SHORTCUTPAGE_H__64E9D8CA_474E_4018_BE3F_30AE5FCFEE3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShortCutPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShortCutPage dialog

class CShortCutPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CShortCutPage)

	void FillList();
// Construction
public:
	CShortCutPage();
	~CShortCutPage();

// Dialog Data
	//{{AFX_DATA(CShortCutPage)
	enum { IDD = IDD_SHORTCUT_PAGE };
	CListCtrl	m_List;
	//}}AFX_DATA
	
	CImageList	m_ImageList;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CShortCutPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL		m_bChanged;
	// Generated message map functions
	//{{AFX_MSG(CShortCutPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHORTCUTPAGE_H__64E9D8CA_474E_4018_BE3F_30AE5FCFEE3C__INCLUDED_)

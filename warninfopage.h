#if !defined(AFX_WARNINFOPAGE_H__B18E7CA1_01C5_4C67_B926_DD2DBBE24971__INCLUDED_)
#define AFX_WARNINFOPAGE_H__B18E7CA1_01C5_4C67_B926_DD2DBBE24971__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// warninfopage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWarnInfoPage dialog

class CWarnInfoPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CWarnInfoPage)

// Construction
public:
	CWarnInfoPage();
	~CWarnInfoPage();

// Dialog Data
	//{{AFX_DATA(CWarnInfoPage)
	enum { IDD = IDD_WARNINFO_PAGE };
	CListCtrl	m_GP_List;
	int		m_nWarnFlag;
	//}}AFX_DATA

private:
	CImageList	m_ImageList2;
	vector<PWARNINGCND> m_TempWg;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWarnInfoPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWarnInfoPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnDel();
	afx_msg void OnSc1();
	afx_msg void OnSc2();
	afx_msg void OnSc3();
	afx_msg void OnReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void FillGpList(void);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WARNINFOPAGE_H__B18E7CA1_01C5_4C67_B926_DD2DBBE24971__INCLUDED_)

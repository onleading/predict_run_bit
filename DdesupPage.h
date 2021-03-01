#if !defined(AFX_DDESUPPAGE_H__83A0885D_1902_45B5_A1B0_0E4219EAC347__INCLUDED_)
#define AFX_DDESUPPAGE_H__83A0885D_1902_45B5_A1B0_0E4219EAC347__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DdesupPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDdesupPage dialog

class CDdesupPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CDdesupPage)

// Construction
public:
	CDdesupPage();
	~CDdesupPage();

// Dialog Data
	//{{AFX_DATA(CDdesupPage)
	enum { IDD = IDD_DDESUP_PAGE };
	UINT	m_nLongPeriod;
	UINT	m_nShortPeriod;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDdesupPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDdesupPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DDESUPPAGE_H__83A0885D_1902_45B5_A1B0_0E4219EAC347__INCLUDED_)

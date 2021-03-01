#if !defined(AFX_ABSWARNPAGE_H__778126E2_AEDB_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_ABSWARNPAGE_H__778126E2_AEDB_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AbsWarnPage.h : header file

#define GPLIST	0
#define TJLIST	1

#include "DragListCtrl.h"
#include "Label.h"

class CAbsWarnPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CAbsWarnPage)

	void FillList();
// Construction
public:
	CAbsWarnPage();
	~CAbsWarnPage();

// Dialog Data
	//{{AFX_DATA(CAbsWarnPage)
	enum { IDD = IDD_ABSWARN_PAGE };
	CDragListCtrl	m_AbsWarnList;
	//}}AFX_DATA
	CImageList m_ImageList;
	int	m_nListFlag;
	int	CurrentAbs;
	CLabel	m_CStatus;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAbsWarnPage)
	public:
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAbsWarnPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddabs();
	afx_msg void OnModifyabs();
	afx_msg void OnDeleteabs();
	afx_msg void OnZapabs();
	afx_msg void OnDblclkAbswarnList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABSWARNPAGE_H__778126E2_AEDB_11D3_B921_0000800104D5__INCLUDED_)

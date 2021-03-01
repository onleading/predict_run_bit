#if !defined(AFX_GPGRADEDLG_H__4922AF6D_AF88_49BB_8C10_853739DBCD64__INCLUDED_)
#define AFX_GPGRADEDLG_H__4922AF6D_AF88_49BB_8C10_853739DBCD64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//

/////////////////////////////////////////////////////////////////////////////
// GpGradeDlg dialog
#include "Label.h"
#include "SortListCtrl.h"

class CGpGradeDlg : public CDialog
{
	void FillList();
public:
	CGpGradeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGpGradeDlg)
	enum { IDD = IDD_GPGRADE };
	CTabCtrl	m_Tab;
	CLabel	m_Hint_Static;
	CSortListCtrl	m_GPList;
	//}}AFX_DATA

	int			m_nTabSel;
	CImageList 	m_ImageList;
	MemStkInfo* m_pCurStkInfo;

	GRADEINFO * m_pAllGradeInfo;
	int			m_nAllGradeNum;
	
	OrgCodeAndName *m_pOrgCodeName;
	int				m_nOrgNum;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGpGradeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGpGradeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void SearchGpGrade(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GPGRADEDLG_H__4922AF6D_AF88_49BB_8C10_853739DBCD64__INCLUDED_)

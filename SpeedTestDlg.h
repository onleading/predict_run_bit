#if !defined(AFX_SPEEDTESTDLG_H__E38EE9E0_D3E1_491E_B2BA_E5223D68260C__INCLUDED_)
#define AFX_SPEEDTESTDLG_H__E38EE9E0_D3E1_491E_B2BA_E5223D68260C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpeedTestDlg.h : header file
//

#include "Label.h"

typedef struct tagSort
{
	int nIndex;
	long yali;
	long zonghe;
	tagSort()
	{
		nIndex=0;
		yali=MAXLONG;
		zonghe=MAXLONG;
	};
}Sort;

/////////////////////////////////////////////////////////////////////////////
// CSpeedTestDlg dialog

class CSpeedTestDlg : public CDialog
{
	void InitList();
public:
	CSpeedTestDlg(CWnd* pParent = NULL);   // standard constructor
// Operation
public:
	CImageList m_imagelist;
	int m_Return;
	CArray<Sort,Sort> aSort;
// Dialog Data
	//{{AFX_DATA(CSpeedTestDlg)
	enum { IDD = IDD_SPEEDTEST };
	CTabCtrl	m_Tab;
	CButton		m_ExportCtrl;
	CLabel		m_Status;
	CButton		m_LogoinCtrl;
	CButton		m_TryAgainCtrl;
	CListCtrl	m_hostlist;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpeedTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int		m_nCurTab;
	// Generated message map functions
	//{{AFX_MSG(CSpeedTestDlg)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTestagain();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLogoin();
	afx_msg void OnDblclkHostlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnExport();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
	void ProceedBatchTest();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPEEDTESTDLG_H__E38EE9E0_D3E1_491E_B2BA_E5223D68260C__INCLUDED_)

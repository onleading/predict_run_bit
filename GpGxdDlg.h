#if !defined(AFX_GpGxdDlg_H__4922AF6D_AF88_49BB_8C10_853739DBCD64__INCLUDED_)
#define AFX_GpGxdDlg_H__4922AF6D_AF88_49BB_8C10_853739DBCD64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// GpGxdDlg dialog
#include "SortListCtrl.h"
#include "CalcIndexChgRsn.h"

class CGpGxdDlg : public CDialog
{
	void CalcWeight();
	void FillList(int nSel);
public:
	CGpGxdDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGpGxdDlg)
	enum { IDD = IDD_GPGXD };
	CTabCtrl		m_Tab;
	CSortListCtrl	m_GPList;
	//}}AFX_DATA

	int			m_nTabSel;
	CImageList 	m_ImageList;

	TCalcIndexChgReason m_SZ399001;
	TCalcIndexChgReason m_SH000001;
	TCalcIndexChgReason m_HS300;
	TCalcIndexChgReason m_SZ399005;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGpGxdDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	MemStkInfo *m_pStk;
	struct CurrStockData m_hq;

	// Generated message map functions
	//{{AFX_MSG(CGpGxdDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void SearchGpGxd(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnExport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GpGxdDlg_H__4922AF6D_AF88_49BB_8C10_853739DBCD64__INCLUDED_)

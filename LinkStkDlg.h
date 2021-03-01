#if !defined(AFX_LINKSTKDLG_H__4922AF6D_AF88_49BB_8C10_853739DBCD64__INCLUDED_)
#define AFX_LINKSTKDLG_H__4922AF6D_AF88_49BB_8C10_853739DBCD64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinkStkDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLinkStkDlg dialog
#include "Label.h"

class CLinkStkDlg : public CDialog
{
// Construction
public:
	CLinkStkDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLinkStkDlg)
	enum { IDD = IDD_LINKSTK };
	CLabel	m_Hint_Static;
	CListCtrl	m_GPList;
	//}}AFX_DATA

	CImageList 	m_ImageList;
	MemStkInfo* m_pCurStkInfo;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkStkDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLinkStkDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINKSTKDLG_H__4922AF6D_AF88_49BB_8C10_853739DBCD64__INCLUDED_)

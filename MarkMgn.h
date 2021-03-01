#if !defined(AFX_MARKMGN_H__7803AFCA_63BB_4EE8_97A1_27BEA7172F40__INCLUDED_)
#define AFX_MARKMGN_H__7803AFCA_63BB_4EE8_97A1_27BEA7172F40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MarkMgn.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMarkMgn dialog
#include "DragListCtrl.h"

class CMarkMgn : public CDialog
{
	void	FillGpList();
public:
	CMarkMgn(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMarkMgn)
	enum { IDD = IDD_MARKMGN };
	CDragListCtrl	m_GPList;
	BOOL	m_bCheck1;
	BOOL	m_bCheck2;
	BOOL	m_bCheck3;
	BOOL	m_bCheck4;
	BOOL	m_bCheck5;
	BOOL	m_bCheck6;
	BOOL	m_bCheck7;
	//}}AFX_DATA

	BOOL	m_bShowMark[MAX_GROUPMARK];
	
	short   m_nStkIndex[MAXGPNUM];
	short	m_nStkNum;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMarkMgn)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMarkMgn)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBatchadd();
	afx_msg void OnSelectall();
	afx_msg void OnSelectnone();
	afx_msg void OnBatchmark();
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnCheck3();
	afx_msg void OnCheck4();
	afx_msg void OnCheck5();
	afx_msg void OnCheck6();
	afx_msg void OnCheck7();
	afx_msg void OnDblclkGplist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg void OnCancelMark(void);
	afx_msg void OnMarkStockFlag(UINT nID);
	
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MARKMGN_H__7803AFCA_63BB_4EE8_97A1_27BEA7172F40__INCLUDED_)

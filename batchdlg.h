#if !defined(AFX_BATCHDLG_H__BD1BA51F_14E7_4A46_85F1_FD87B3EBB85C__INCLUDED_)
#define AFX_BATCHDLG_H__BD1BA51F_14E7_4A46_85F1_FD87B3EBB85C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// batchdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBatchDlg dialog

#include "DragListCtrl.h"

class CBatchDlg : public CDialog
{
// Construction
public:
	CBatchDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBatchDlg)
	enum { IDD = IDD_BATCH_DIALOG };
	CDragListCtrl	m_GPList;
	//}}AFX_DATA
	
	BYTE	m_nOwnerType;
	short   m_nStkIndex[MAXGPNUM];
	short	m_nStkNum;

	BOOL	m_bDisableDelete;	//不允许删除,一般为FALSE
	int		m_WhichGPType;		
	int		m_nBlockFlag;

	BOOL	m_bDeleteSuccess;	//是否删除成功了
	BOOL	m_bTopMost;
	BOOL	m_bAllData;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBatchDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBatchadd();
	afx_msg void OnBatchdelete();
	afx_msg void OnSelectall();
	afx_msg void OnSelectnone();
	afx_msg void OnBatchmark();
	//}}AFX_MSG
	afx_msg void OnCancelMark(void);
	afx_msg void OnMarkStockFlag(UINT nID);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BATCHDLG_H__BD1BA51F_14E7_4A46_85F1_FD87B3EBB85C__INCLUDED_)

#if !defined(AFX_DATADIGDLG_H__0BCD55EC_24EF_486B_B937_A2B61A7CD919__INCLUDED_)
#define AFX_DATADIGDLG_H__0BCD55EC_24EF_486B_B937_A2B61A7CD919__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataDigDlg.h : header file
/////////////////////////////////////////////////////////////////////////////
// CDataDigDlg dialog

#include "Label.h"
#include "DragListCtrl.h"

class CDataDigDlg : public CDialog
{
	void InitList();
	void FillList();
	void ShowContent();
public:
	CDataDigDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDataDigDlg)
	enum { IDD = IDD_DATADIG_DIALOG };
	CEdit	m_Content_Edit;
	CTabCtrl	m_Tab;
	CLabel		m_Info_Status;
	CDragListCtrl	m_GPList;
	//}}AFX_DATA

	long		clientnum;
	long		usedclientnum;
	long		seconds;
	char		HostName[21];
	short		attention_num;
	short		resonate_num;
	short		no_attention_num;
	short		no_resonate_num;
	short		attention_index[80];
	short		resonate_index[80];
	short		no_attention_index[80];
	short		no_resonate_index[80];
	unsigned long attention_rate[80];
	unsigned long resonate_rate[80];
	unsigned long no_attention_rate[80];
	unsigned long no_resonate_rate[80];
	
	long		contentlen;
	char		*pContent;

	int			m_nTabSel;
	CImageList	m_ImageList;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataDigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDataDigDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTozst();
	afx_msg void OnDblclkGpList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATADIGDLG_H__0BCD55EC_24EF_486B_B937_A2B61A7CD919__INCLUDED_)

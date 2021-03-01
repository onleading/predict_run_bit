#if !defined(AFX_LOCALPTSTKMNGDLG_H__2A8DDC88_52E9_43C6_8501_A2D11272BABE__INCLUDED_)
#define AFX_LOCALPTSTKMNGDLG_H__2A8DDC88_52E9_43C6_8501_A2D11272BABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LocalPtStkMngDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLocalPtStkMngDlg dialog

#include "Label.h"

class CLocalPtStkMngDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CLocalPtStkMngDlg)

// Construction
public:
	CLocalPtStkMngDlg();
	~CLocalPtStkMngDlg();

// Dialog Data
	//{{AFX_DATA(CLocalPtStkMngDlg)
	enum { IDD = IDD_LCPTSTKMNG_DIALOG };
	CListCtrl	m_PtStkList;
	CLabel		m_CStatus;
	//}}AFX_DATA
protected:
	LPLCPTSET	m_pLcPtSet;
	long		m_lLcPtSetNum;

	CImageList	m_ImageList;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLocalPtStkMngDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillList();

	// Generated message map functions
	//{{AFX_MSG(CLocalPtStkMngDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnLcptstkmngAddbutton();
	afx_msg void OnLcptstkmngChgbutton();
	afx_msg void OnLcptstkmngDelbutton();
	afx_msg void OnViewcurhq();
	afx_msg void OnDblclkLcptstkmngList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOCALPTSTKMNGDLG_H__2A8DDC88_52E9_43C6_8501_A2D11272BABE__INCLUDED_)

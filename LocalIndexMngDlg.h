#if !defined(AFX_LOCALINDEXMNGDLG_H__D010031E_D6A4_48F6_B761_AF35D7E03037__INCLUDED_)
#define AFX_LOCALINDEXMNGDLG_H__D010031E_D6A4_48F6_B761_AF35D7E03037__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LocalIndexMngDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLocalIndexMngDlg property page

#include "Label.h"

class CLocalIndexMngDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CLocalIndexMngDlg)
		
// Construction
public:
	CLocalIndexMngDlg();   // standard constructor
	~CLocalIndexMngDlg();

// Dialog Data
	//{{AFX_DATA(CLocalIndexMngDlg)
	enum { IDD = IDD_LCINDEXMNG_DIALOG };
	CListCtrl	m_IndexList;
	CString		m_IndexDesc;
	CLabel		m_CStatus;
	//}}AFX_DATA

protected:
	LPLCIDXSET	m_pLcIdxSet;
	long		m_lLcIdxSetNum;

	CImageList	m_ImageList;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLocalIndexMngDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillList();
	void FillEdit();

	// Generated message map functions
	//{{AFX_MSG(CLocalIndexMngDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnLcindexmngAddbutton();
	afx_msg void OnLcindexmngChgbutton();
	afx_msg void OnLcindexmngDelbutton();
	afx_msg void OnRecalcButton();
	afx_msg void OnViewcurhq();
	afx_msg void OnDblclkLcindexmngList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRecalcallButton();
	afx_msg void OnClickLcindexmngList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOCALINDEXMNGDLG_H__D010031E_D6A4_48F6_B761_AF35D7E03037__INCLUDED_)

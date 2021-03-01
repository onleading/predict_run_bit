#if !defined(AFX_URGENTLISTDLG_H__E108C389_635D_4244_B048_4702E2169AC1__INCLUDED_)
#define AFX_URGENTLISTDLG_H__E108C389_635D_4244_B048_4702E2169AC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UrgentListDlg.h : header file
//
#include "MyRichEdit.h"
/////////////////////////////////////////////////////////////////////////////
// CUrgentListDlg dialog

class CUrgentListDlg : public CDialog
{
// Construction
public:
	CUrgentListDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUrgentListDlg)
	enum { IDD = IDD_URGENTLIST };
	CListCtrl	m_TitleList;
	CMyRichEdit	m_RichEditEx;
	CImageList	m_ImageList;
	//}}AFX_DATA
	
	void	FillList(int nSetSel = -1);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUrgentListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUrgentListDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnItemchangedTitlelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGodown();
	afx_msg void OnGoup();
	afx_msg void OnMoreList();
	afx_msg void OnSaveit();
	afx_msg void OnDblclkTitlelist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int		m_nLastSel;
	BOOL	m_bShowContent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_URGENTLISTDLG_H__E108C389_635D_4244_B048_4702E2169AC1__INCLUDED_)

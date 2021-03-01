#if !defined(AFX_LOCALEXTERNMNGDLG_H__D610FB0F_107D_4A76_A055_50CF9ED77E05__INCLUDED_)
#define AFX_LOCALEXTERNMNGDLG_H__D610FB0F_107D_4A76_A055_50CF9ED77E05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LocalExternMngDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLocalExternMngDlg dialog

class CLocalExternMngDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CLocalExternMngDlg)

// Construction
public:
	CLocalExternMngDlg();
	~CLocalExternMngDlg();

// Dialog Data
	//{{AFX_DATA(CLocalExternMngDlg)
	enum { IDD = IDD_LCEXTERNMNG_DIALOG };
	CListCtrl	m_ExternList;
	//}}AFX_DATA

protected:
	LPLCEXTSET	m_pLcExtSet;
	long		m_lLcExtSetNum;

	CImageList	m_ImageList;
	
	void FillList();
	void FillEdit();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLocalExternMngDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLocalExternMngDlg)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnLcexternmngAddbutton();
	afx_msg void OnLcexternmngChgbutton();
	afx_msg void OnLcexternmngDelbutton();
	afx_msg void OnViewcurhq();
	virtual BOOL OnInitDialog();
	afx_msg void OnRecalcButton();
	afx_msg void OnDblclkLcindexmngList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOCALEXTERNMNGDLG_H__D610FB0F_107D_4A76_A055_50CF9ED77E05__INCLUDED_)

#ifndef __LOCALINDEXMAINDLG__H__
#define __LOCALINDEXMAINDLG__H__

#if _MSC_VER > 1000
#pragma once
#endif

//导入品种主管理对话框
class CLocalIndexMainDlg : public CPropertySheet
{
	DECLARE_DYNAMIC(CLocalIndexMainDlg)

// Construction
public:
	CLocalIndexMainDlg();
	CLocalIndexMainDlg(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CLocalIndexMainDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
protected:
	BOOL m_bPageValid;
	int m_nActivePage;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLocalIndexMainDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLocalIndexMainDlg();
protected:
	BOOL CheckPage(int nPage);

	// Generated message map functions
protected:
	//{{AFX_MSG(CLocalIndexMainDlg)
	afx_msg void OnSaveAndExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
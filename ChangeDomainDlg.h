#if !defined(AFX_CHANGEDOMAINDLG_H__4D2C4DA1_EBA4_11D4_BB9D_00E04C66B1EB__INCLUDED_)
#define AFX_CHANGEDOMAINDLG_H__4D2C4DA1_EBA4_11D4_BB9D_00E04C66B1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangeDomainDlg.h : header file

/////////////////////////////////////////////////////////////////////////////
// CChangeDomainDlg dialog

#define MAX_DOMAIN_SELNUM	300

class CChangeDomainDlg : public CDialog
{
	BOOL TestSelected(int flag,int which);
	void FillGPDomain(int i);
public:
	CChangeDomainDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChangeDomainDlg)
	enum { IDD = IDD_CHANGEDOMAIN_DIALOG };
	CListCtrl	m_Domain_List;
	//}}AFX_DATA

	struct GpDomain_Struct m_GpDomain[MAX_DOMAIN_SELNUM];
	int    m_nSelDomain;
	
	BOOL   m_bSelectOne;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeDomainDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList m_ImageList;

	short  m_bChecked[MAX_DOMAIN_SELNUM];
	int	   m_nTotalNum;
	// Generated message map functions
	//{{AFX_MSG(CChangeDomainDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClickDomainList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownDomainList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void FillDomain(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDblclkDomainList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEDOMAINDLG_H__4D2C4DA1_EBA4_11D4_BB9D_00E04C66B1EB__INCLUDED_)

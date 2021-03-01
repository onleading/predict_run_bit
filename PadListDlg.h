#if !defined(AFX_PADLISTDLG_H__4D5B143B_4A42_4786_A826_4510ED1D2B53__INCLUDED_)
#define AFX_PADLISTDLG_H__4D5B143B_4A42_4786_A826_4510ED1D2B53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PadListDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPadListDlg dialog
class CPadListDlg : public CDialog
{
	void	FillList();
public:
	CPadListDlg(CWnd* pParent = NULL);   // standard constructor
	
	BOOL		m_bModified;
	BOOL		m_bSwitchShow;

// Dialog Data
	//{{AFX_DATA(CPadListDlg)
	enum { IDD = IDD_PADLIST };
	CListCtrl	m_PadList;
	CImageList	m_ImageList;
	//}}AFX_DATA
	int			m_nLastSel;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPadListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPadListDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnImportpad();
	afx_msg void OnExportpad();
	afx_msg void OnDelete();
	afx_msg void OnDblclkPadList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedPadlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnShowshortcut();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PADLISTDLG_H__4D5B143B_4A42_4786_A826_4510ED1D2B53__INCLUDED_)

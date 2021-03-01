#if !defined(AFX_SELFLINESHOWDLG_H__9F1A7AA7_3B5C_4284_94D3_4C20ABFF0CE9__INCLUDED_)
#define AFX_SELFLINESHOWDLG_H__9F1A7AA7_3B5C_4284_94D3_4C20ABFF0CE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelfLineShowDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelfLineShowDlg dialog

class CSelfLineShowDlg : public CDialog
{
// Construction
public:
	CSelfLineShowDlg(CWnd* pParent = NULL);   // standard constructor
	~CSelfLineShowDlg();
// Dialog Data
	//{{AFX_DATA(CSelfLineShowDlg)
	enum { IDD = IDD_DIALOG_SELFLINE };
	CListCtrl	m_List;
	//}}AFX_DATA

	DRAWLINE	*m_aDrawLine;
	int			m_nDrawLine;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelfLineShowDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void FillList();

	// Generated message map functions
	//{{AFX_MSG(CSelfLineShowDlg)
	afx_msg void OnDelline();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELFLINESHOWDLG_H__9F1A7AA7_3B5C_4284_94D3_4C20ABFF0CE9__INCLUDED_)

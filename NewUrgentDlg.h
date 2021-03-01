#if !defined(AFX_NEWURGENTDLG_H__CA315D74_864D_47AB_96A7_91AD44A96756__INCLUDED_)
#define AFX_NEWURGENTDLG_H__CA315D74_864D_47AB_96A7_91AD44A96756__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewUrgentDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewUrgentDlg dialog

class CNewUrgentDlg : public CDialog
{
// Construction
public:
	CNewUrgentDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewUrgentDlg)
	enum { IDD = IDD_NEWURGENT };
	CString	m_strContent;
	CString	m_strTitle;
	int		m_bNews;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewUrgentDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewUrgentDlg)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	short m_nMainID;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWURGENTDLG_H__CA315D74_864D_47AB_96A7_91AD44A96756__INCLUDED_)

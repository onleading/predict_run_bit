#if !defined(AFX_GUIDEDLG_H__77F6606B_CC0B_4BEC_B0BF_4CF5E0B20812__INCLUDED_)
#define AFX_GUIDEDLG_H__77F6606B_CC0B_4BEC_B0BF_4CF5E0B20812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GuideDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGuideDlg dialog

#include "Label.h"

class CGuideDlg : public CDialog
{
// Construction
public:
	CGuideDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGuideDlg)
	enum { IDD = IDD_GUIDE };
	CLabel	m_Other_Static;
	int		m_nMode;
	int		m_nNetCondition;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGuideDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGuideDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GUIDEDLG_H__77F6606B_CC0B_4BEC_B0BF_4CF5E0B20812__INCLUDED_)

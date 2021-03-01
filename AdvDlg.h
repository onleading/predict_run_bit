//{{AFX_INCLUDES()
#include "webbrowser2.h"
//}}AFX_INCLUDES
#if !defined(AFX_ADVDLG_H__34D334D0_5835_4B68_A704_726E2064676A__INCLUDED_)
#define AFX_ADVDLG_H__34D334D0_5835_4B68_A704_726E2064676A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdvDlg dialog

class CAdvDlg : public CDialog
{
// Construction
public:
	CAdvDlg(CWnd* pParent = NULL);   // standard constructor
	void SetAdvIndex(int nIndex);
// Dialog Data
	//{{AFX_DATA(CAdvDlg)
	enum { IDD = IDD_DIALOG_ADV };
	CWebBrowser2	m_IE;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

//Attributes
protected:
	int m_nIndex;
	HICON m_hIcon;
	CBrush m_brush;
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAdvDlg)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVDLG_H__34D334D0_5835_4B68_A704_726E2064676A__INCLUDED_)

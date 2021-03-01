#if !defined(AFX_AUTOUPDLG_H__F9906433_2BFF_464E_BEDC_0817295D09DD__INCLUDED_)
#define AFX_AUTOUPDLG_H__F9906433_2BFF_464E_BEDC_0817295D09DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// autoupdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAutoupDlg dialog

#include "Label.h"
class CAutoupDlg : public CDialog
{
	void SureNoHint();
public:
	CAutoupDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAutoupDlg)
	enum { IDD = IDD_AUTOUPDIALOG };
	CRichEditCtrl	m_RichEditEx;
	CLabel	m_ClientVer_Static;
	BOOL	m_bAWeekNoHint;
	//}}AFX_DATA

	BOOL m_bMustAutoup;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutoupDlg)
	afx_msg void OnAutoupButton();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void	OnRichEditExLink( NMHDR* in_pNotifyHeader, LRESULT* out_pResult );
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOUPDLG_H__F9906433_2BFF_464E_BEDC_0817295D09DD__INCLUDED_)

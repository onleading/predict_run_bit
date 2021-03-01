#if !defined(AFX_EDITTXTDLG_H__53247383_FB45_11D4_BB9D_00E04C66B1EB__INCLUDED_)
#define AFX_EDITTXTDLG_H__53247383_FB45_11D4_BB9D_00E04C66B1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditTxtDlg.h : header file
//

#include "ColorButton.h"
/////////////////////////////////////////////////////////////////////////////
// CEditTxtDlg dialog

class CEditTxtDlg : public CDialog
{
// Construction
public:
	CEditTxtDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditTxtDlg)
	enum { IDD = IDD_TXTTOOLS_DIALOG };
	CColorButton	m_colorbtn;
	CString	m_strTxt;
	//}}AFX_DATA	
	COLORREF m_color;
	BOOL	m_bAdd;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditTxtDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditTxtDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonColor();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITTXTDLG_H__53247383_FB45_11D4_BB9D_00E04C66B1EB__INCLUDED_)

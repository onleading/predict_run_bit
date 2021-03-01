#if !defined(AFX_EDITBLOCKDLG_H__F5D10A08_C1E7_11D3_8B1F_00E04C66B9E1__INCLUDED_)
#define AFX_EDITBLOCKDLG_H__F5D10A08_C1E7_11D3_8B1F_00E04C66B9E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditBlockDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditBlockDlg dialog
#include "EditEx.h"
class CEditBlockDlg : public CDialog
{
// Construction
public:
	CEditBlockDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditBlockDlg)
	enum { IDD = IDD_EDIT_BLOCK };
	CEditEx	m_editKeyGuy;
	CString	m_BlockName;
	CString	m_KeyGuyCode;
	//}}AFX_DATA
	CString m_Caption;
	BOOL	IsAdd;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditBlockDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditBlockDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeBlocknameEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITBLOCKDLG_H__F5D10A08_C1E7_11D3_8B1F_00E04C66B9E1__INCLUDED_)

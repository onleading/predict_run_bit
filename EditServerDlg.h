#if !defined(AFX_EDITSERVERDLG_H__BD7AD044_F406_11D3_A4CD_00E04C66B9E1__INCLUDED_)
#define AFX_EDITSERVERDLG_H__BD7AD044_F406_11D3_A4CD_00E04C66B9E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditServerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditServerDlg dialog

class CEditServerDlg : public CDialog
{
// Construction
public:
	CEditServerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditServerDlg)
	enum { IDD = IDD_EDITSERVER_DIALOG };
	CEdit	m_ctrIPAddress;
	CString	m_strServerName;
	UINT	m_nPort;
	//}}AFX_DATA
	
	CString m_strIPAddress;
	BOOL	m_bAdd;
	int		m_nType;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditServerDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITSERVERDLG_H__BD7AD044_F406_11D3_A4CD_00E04C66B9E1__INCLUDED_)

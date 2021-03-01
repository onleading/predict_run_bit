	#if !defined(AFX_PROXYDLG_H__C0D70B2D_A108_4899_A848_A35F4FEC277C__INCLUDED_)
#define AFX_PROXYDLG_H__C0D70B2D_A108_4899_A848_A35F4FEC277C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProxyDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProxyDlg dialog

class CProxyDlg : public CDialog
{
// Construction
public:
	CProxyDlg(CWnd* pParent = NULL);   // standard constructor

	void RadioEnable();
// Dialog Data
	//{{AFX_DATA(CProxyDlg)
	enum { IDD = IDD_PROXYDLG };
	CString	m_strProxyIP;
	BOOL	m_bProxy;
	UINT	m_nProxyPort;
	CString	m_strProxyPass;
	CString	m_strProxyUser;
	int		m_nSockType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProxyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProxyDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnProxycheck();
	afx_msg void OnClickProxyType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROXYDLG_H__C0D70B2D_A108_4899_A848_A35F4FEC277C__INCLUDED_)

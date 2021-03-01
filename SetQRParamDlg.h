#if !defined(AFX_SETQRPARAMDLG_H__6334A361_A4E0_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_SETQRPARAMDLG_H__6334A361_A4E0_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetQRParamDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetQRParamDlg dialog

class CSetQRParamDlg : public CDialog
{
// Construction
public:
	CSetQRParamDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetQRParamDlg)
	enum { IDD = IDD_SETQRPARAM };
	int		m_QRParam1;
	int		m_QRParam2;
	int		m_QRParam3;
	int		m_QRParam4;
	int		m_QRParam5;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetQRParamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetQRParamDlg)
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpin3(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpin4(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpin5(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusQrparam1();
	afx_msg void OnKillfocusQrparam2();
	afx_msg void OnKillfocusQrparam3();
	afx_msg void OnKillfocusQrparam4();
	afx_msg void OnKillfocusQrparam5();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDefaultqrparam();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETQRPARAMDLG_H__6334A361_A4E0_11D3_B921_0000800104D5__INCLUDED_)

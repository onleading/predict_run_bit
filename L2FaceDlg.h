#if !defined(AFX_L2FACEDLG_H__2443398E_AD25_4D96_8ACE_72C8B653F72B__INCLUDED_)
#define AFX_L2FACEDLG_H__2443398E_AD25_4D96_8ACE_72C8B653F72B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// L2FaceDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CL2FaceDlg dialog

#include "HyperLink.h"
#include "Label.h"

class CL2FaceDlg : public CDialog
{
// Construction
public:
	CL2FaceDlg(CWnd* pParent = NULL);   // standard constructor
	~CL2FaceDlg();
// Dialog Data
	//{{AFX_DATA(CL2FaceDlg)
	enum { IDD = IDD_LEVEL2_FACE };
	CHyperLink	m_UserInfoLink;
	CHyperLink	m_BuyLink;
	CLabel		m_Hint_Static;
	CComboBox	m_L2Type_Combo;
	CString		m_L2Password;
	CString		m_L2Username;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CL2FaceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON	m_hIcon;
	CBrush	m_brush;
	// Generated message map functions
	//{{AFX_MSG(CL2FaceDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRegLinkMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnRegLink2Message(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSelchangeL2typeCombo();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_L2FACEDLG_H__2443398E_AD25_4D96_8ACE_72C8B653F72B__INCLUDED_)

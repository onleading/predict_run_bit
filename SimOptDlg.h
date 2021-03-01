#if !defined(AFX_SIMOPTDLG_H__EC3C6456_F6C9_4D34_9A02_5596E1A5D24C__INCLUDED_)
#define AFX_SIMOPTDLG_H__EC3C6456_F6C9_4D34_9A02_5596E1A5D24C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SimOptDlg.h : header file
//
#include "Label.h"
/////////////////////////////////////////////////////////////////////////////
// CSimOptDlg dialog

class CSimOptDlg : public CDialog
{
// Construction
public:
	CSimOptDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSimOptDlg)
	enum { IDD = IDD_OPTDLG };
	CComboBox	m_Scheme_Combo;
	UINT	m_nMinn;
	UINT	m_nDayn;
	CLabel	m_ZS3_Static;
	CLabel	m_ZS2_Static;
	CLabel	m_ZS1_Static;
	//}}AFX_DATA
	struct SYSTEMCOLOR UI;				//当前配置的方案
	int	   nCurSelScheme;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimOptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSimOptDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnZs1Button();
	afx_msg void OnZs2Button();
	afx_msg void OnZs3Button();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMOPTDLG_H__EC3C6456_F6C9_4D34_9A02_5596E1A5D24C__INCLUDED_)

#if !defined(AFX_TQCFGDLG_H__BBDD44B8_953C_4172_BCF0_3830AF9A6661__INCLUDED_)
#define AFX_TQCFGDLG_H__BBDD44B8_953C_4172_BCF0_3830AF9A6661__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TQCfgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTQCfgDlg dialog

#include "Label.h"
class CTQCfgDlg : public CDialog
{
// Construction
public:
	CTQCfgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTQCfgDlg)
	enum { IDD = IDD_TQCFG };
	CLabel	m_Hint_Static;
	BOOL	m_bAutoConnectTQ;
	int		m_bTQAsBroker;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTQCfgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTQCfgDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TQCFGDLG_H__BBDD44B8_953C_4172_BCF0_3830AF9A6661__INCLUDED_)

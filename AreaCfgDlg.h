#if !defined(AFX_AREACFGDLG_H__BBDD44B8_953C_4172_BCF0_3830AF9A6661__INCLUDED_)
#define AFX_AREACFGDLG_H__BBDD44B8_953C_4172_BCF0_3830AF9A6661__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AreaCfgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAreaCfgDlg dialog

#include "Label.h"
class CAreaCfgDlg : public CDialog
{
// Construction
public:
	CAreaCfgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAreaCfgDlg)
	enum { IDD = IDD_AREACFG };
	int		m_nCXMode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAreaCfgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAreaCfgDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AREACFGDLG_H__BBDD44B8_953C_4172_BCF0_3830AF9A6661__INCLUDED_)

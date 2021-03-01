#if !defined(AFX_BLOCKCALCDLG_H__7F267EBC_A45D_4FA1_B27F_800FBEA18120__INCLUDED_)
#define AFX_BLOCKCALCDLG_H__7F267EBC_A45D_4FA1_B27F_800FBEA18120__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// blockcalcdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBlockCalcDlg dialog

#include "Label.h"
#include "DragListCtrl.h"

class CBlockCalcDlg : public CDialog
{
// Construction
public:
	CBlockCalcDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBlockCalcDlg)
	enum { IDD = IDD_BLOCKCALC };
	CComboBox	m_CalcStyle_Combo;
	CDragListCtrl	m_GPList;
	CLabel	m_Block2_Static;
	CLabel	m_Block1_Static;
	//}}AFX_DATA

	CImageList	m_ImageList;

	short	ResultIndex[MAXGPNUM];
	int		ResultGPNum;

	struct GpDomain_Struct m_GpDomain[2];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBlockCalcDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBlockCalcDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBlock1();
	afx_msg void OnBlock2();
	afx_msg void OnCalc();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDblclkGpList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLOCKCALCDLG_H__7F267EBC_A45D_4FA1_B27F_800FBEA18120__INCLUDED_)

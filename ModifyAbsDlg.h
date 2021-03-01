#if !defined(AFX_MODIFYABSDLG_H__F84BE1E1_AF0B_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_MODIFYABSDLG_H__F84BE1E1_AF0B_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ModifyAbsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CModifyAbsDlg dialog

#include "Label.h"

class CModifyAbsDlg : public CDialog
{
	void OnChangeUpBreak();
	void OnChangeDownBreak();
	void FormatFloat(float & ftmp,int xsflag);
public:
	CModifyAbsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CModifyAbsDlg)
	enum { IDD = IDD_MODIFYABS_DIALOG };
	CLabel		m_GPName;
	CComboBox	m_Combo2;
	CComboBox	m_Combo1;
	float	m_fDownBreak;
	float	m_fUpBreak;
	float	m_fPriceDiff;
	float	m_fChangeHand;
	//}}AFX_DATA
	int		nStockNo;
	int		m_nCurSel1,m_nCurSel2;
	BOOL	m_bValidPrice;
	BOOL	m_bHasDeleteBtn;
	BOOL	m_bDeleted;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModifyAbsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON		m_hIcon;
	MemStkInfo *m_pStkInfo;
	int			m_nXSFlag;
	// Generated message map functions
	//{{AFX_MSG(CModifyAbsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpin2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnSelchangeCombo2();
	afx_msg void OnDelete();
	afx_msg void OnKillfocusUpbreakEdit();
	afx_msg void OnKillfocusDownbreakEdit();
	afx_msg void OnAlltj();
	afx_msg void OnDeltaposSpin3(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpin4(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeUpbreakEdit();
	afx_msg void OnChangeDownbreakEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODIFYABSDLG_H__F84BE1E1_AF0B_11D3_B921_0000800104D5__INCLUDED_)

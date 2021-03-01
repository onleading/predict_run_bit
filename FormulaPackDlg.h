#if !defined(AFX_FORMULAPACK_H__7564B97E_AA3D_4E27_ADAF_D273AF9A9A37__INCLUDED_)
#define AFX_FORMULAPACK_H__7564B97E_AA3D_4E27_ADAF_D273AF9A9A37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormulaPackDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormulaPackDlg dialog

class CFormulaPackDlg : public CDialog
{
// Construction
public:
	CFormulaPackDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFormulaPackDlg)
	enum { IDD = IDD_DIALOG_FORMULAPACK };
	CTabCtrl	m_Tab;
	CListCtrl	m_List;
	CString		m_PackName;
	BOOL	m_bSavePeriod;
	//}}AFX_DATA
	BOOL	m_bBand;
	BOOL	m_bNew;
protected:
	FMLPACK		m_FmlPack[MAX_FMLPACK_NUM];
	int			m_nFmlPack;
	FML2STK		m_Fml2Stk[MAX_BANDSTOCK_NUM];
	int			m_nFml2Stk;
	
	FMLPACK		m_SvFmlPack;
	MemStkInfo	*m_pStk;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormulaPackDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetMode(BOOL bNew,BOOL bBand);
	void SetSvFmlPack(FMLPACK &SvFmlPack);
	void SetStock(MemStkInfo *pStk);
protected:
	void FillListHead();
	void FillList();

	CImageList	m_ImageList;

	// Generated message map functions
	//{{AFX_MSG(CFormulaPackDlg)
	afx_msg void OnButtonAddDel();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeTabFormulapack(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListFormulapack(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMULAPACK_H__7564B97E_AA3D_4E27_ADAF_D273AF9A9A37__INCLUDED_)

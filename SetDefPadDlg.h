#if !defined(AFX_SETDEFPADDLG_H__2C0634DA_4E87_436C_AEBC_B952A13AB4A0__INCLUDED_)
#define AFX_SETDEFPADDLG_H__2C0634DA_4E87_436C_AEBC_B952A13AB4A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetDefPadDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetDefPadDlg dialog

class CSetDefPadDlg : public CDialog
{
// Construction
public:
	CSetDefPadDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetDefPadDlg)
	enum { IDD = IDD_SETDEFCUSPAD };
	CListCtrl	m_PadList;
	BOOL	m_bDefCusPad;
	//}}AFX_DATA
	CImageList m_ImageList;
	COLORREF m_clrBackground,m_clrText,m_clrFocused,m_clrSpecial;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetDefPadDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetDefPadDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckcuspad();
	virtual void OnOK();
	afx_msg void OnDblclkAllcuspadlist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void	FillPadList(void);
	void	ShowPadInfo(DynaCmdTable *pCmdInfo);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETDEFPADDLG_H__2C0634DA_4E87_436C_AEBC_B952A13AB4A0__INCLUDED_)

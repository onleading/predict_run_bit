#if !defined(AFX_GETBLOCKDLG_H__ECF9D026_C349_11D3_8B1F_00E04C66B9E1__INCLUDED_)
#define AFX_GETBLOCKDLG_H__ECF9D026_C349_11D3_8B1F_00E04C66B9E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetBlockDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGetBlockDlg dialog

class CGetBlockDlg : public CDialog
{
	void FillBlock();
public:
	CGetBlockDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetBlockDlg)
	enum { IDD = IDD_GETBLOCKDIALOG };
	CListCtrl	m_BlockList;
	//}}AFX_DATA
	CImageList	m_ImageList;
	
	CString		BlockGPName[USERBLOCK_NUM+2]; //支持USERBLOCK_NUM个板块股(包括自选股与股票池)
	int			BlockNum;
	BOOL		bBlockModify;

	int			nSelBlock;

	CString		m_CurGPCode;
	short		m_CurSetCode;
	BOOL		m_bGetBlock;

	//批量加入板块股
	short   m_nStkIndex[MAXGPNUM];
	short	m_nStkNum;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetBlockDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetBlockDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkBlocklist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedBlocklist(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	afx_msg void OnNewblock();
	afx_msg void OnDeleteblock();
	afx_msg void OnRenameblock();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETBLOCKDLG_H__ECF9D026_C349_11D3_8B1F_00E04C66B9E1__INCLUDED_)

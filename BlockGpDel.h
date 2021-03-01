#if !defined(AFX_BLOCKGPDEL_H__B64A7BEF_26B4_4E79_87A2_4F0B2D044B6C__INCLUDED_)
#define AFX_BLOCKGPDEL_H__B64A7BEF_26B4_4E79_87A2_4F0B2D044B6C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BlockGpDel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBlockGpDel dialog

class CBlockGpDel : public CDialog
{
// Construction
public:
	CBlockGpDel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBlockGpDel)
	enum { IDD = IDD_DELBLOCKGP };
	CListCtrl	m_BlockList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBlockGpDel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBlockGpDel)
	virtual BOOL OnInitDialog();
	afx_msg void OnBatchdelete();
	afx_msg void OnSelectall();
	afx_msg void OnSelectnone();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void FillBlockList(CString m_CurGPCode,short m_CurSetCode);

public:
	CString m_CurGPCode;
	short	m_CurSetCode;

private:
	int		m_nBlock;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLOCKGPDEL_H__B64A7BEF_26B4_4E79_87A2_4F0B2D044B6C__INCLUDED_)

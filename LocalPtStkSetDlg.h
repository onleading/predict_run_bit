#if !defined(AFX_LOCALPTSTKSETDLG_H__8C2CCD02_DBB6_40A1_9CA7_0E9408B94109__INCLUDED_)
#define AFX_LOCALPTSTKSETDLG_H__8C2CCD02_DBB6_40A1_9CA7_0E9408B94109__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLocalPtStkSetDlg : public CDialog
{
// Construction
public:
	CLocalPtStkSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLocalPtStkSetDlg)
	enum { IDD = IDD_LCPTSTKSET_DIALOG };
	CListBox	m_StkList;
	CString	m_BaseCode;
	CString	m_Code;
	CString	m_Name;
	int		m_BaseSetcode;
	CString	m_Error;
	//}}AFX_DATA

public:
	LCPTSET	m_LcPtSet;			//退市品种信息
	long	m_LcPtNo;

	BOOL	m_bInited;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLocalPtStkSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetPtSet(LPLCPTSET pLcPtSet = NULL,long lPtNo = 0);
	void FillList();
protected:

	// Generated message map functions
	//{{AFX_MSG(CLocalPtStkSetDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDblclkPtstkList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif

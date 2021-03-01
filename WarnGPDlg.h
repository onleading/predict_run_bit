#if !defined(AFX_WARNGPDLG_H__D9F64A22_4E60_11D3_8C7B_0000800104D5__INCLUDED_)
#define AFX_WARNGPDLG_H__D9F64A22_4E60_11D3_8C7B_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WarnGPDlg.h : header file

class CWarnGPDlg : public CDialog
{
	void FillWarnList();
// Construction
public:
	CWarnGPDlg(CWnd* pParent = NULL);   // standard constructor
	DECLARE_DYNCREATE(CWarnGPDlg);

// Dialog Data
	//{{AFX_DATA(CWarnGPDlg)
	enum { IDD = IDD_WARNGP_DIALOG };
	CListCtrl	m_WarnGPList;
	//}}AFX_DATA
	CImageList	m_ImageList;
	short		m_nWarnType;
	WARN_STRU	*m_pWarnBuffer;
	int			m_nWarnNum;
	BOOL		m_bNeed;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWarnGPDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON	m_hIcon;
	// Generated message map functions
	//{{AFX_MSG(CWarnGPDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateWarnGPMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnWarngpVip();
	virtual void OnCancel();
	afx_msg void OnDblclkWarngpList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnWarngpCfg();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSwitch();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnToblock();
	afx_msg void OnToTxt();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnExport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void RefreshSwitch(void);
	void CarryoutToBlock(WARN_STRU* pWarnBuffer, int nWarnNum, int WhichBlock);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WARNGPDLG_H__D9F64A22_4E60_11D3_8C7B_0000800104D5__INCLUDED_)

#if !defined(AFX_SEATQUERYDLG_H__FC6C54E9_518B_4C52_816F_081D62BF41B6__INCLUDED_)
#define AFX_SEATQUERYDLG_H__FC6C54E9_518B_4C52_816F_081D62BF41B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SeatQueryDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSeatQueryDlg dialog

#include "Label.h"

class CSeatQueryDlg : public CDialog
{
// Construction
public:
	CSeatQueryDlg(CWnd* pParent = NULL);   // standard constructor

	void	DoSearchForInitSeatID();

// Dialog Data
	//{{AFX_DATA(CSeatQueryDlg)
	enum { IDD = IDD_SEATQUERY };
	CListCtrl	m_ListSeares;
	CListCtrl	m_ListBid;
	CListCtrl	m_ListAsk;
	CComboBox	m_SeatNameComb;
	int		m_bSearchID;
	CString	m_strSeatID;
	BOOL	m_bAutoRefresh;
	//}}AFX_DATA

	HICON	m_hIcon;
	short	m_nMainID;
	int		m_nInitSeatID;

	CLabel	m_Bid_Static;
	CLabel	m_Ask_Static;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSeatQueryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSeatQueryDlg)
	afx_msg void OnRadioSeatid();
	afx_msg void OnRadioSeatname();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTosearch();
	afx_msg void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDblclkListSearchres(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListBid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListAsk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnAutorefreshCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void	FillCompDropList();
	void	GetQueryData(int nSeatID);
	void	FillQueryRes(SEATQUERYINFO *pInfo, int nGetNum);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEATQUERYDLG_H__FC6C54E9_518B_4C52_816F_081D62BF41B6__INCLUDED_)

#if !defined(AFX_DSCONNECTDLG_H__16DDE9F7_9771_4124_BD27_84B7806376AD__INCLUDED_)
#define AFX_DSCONNECTDLG_H__16DDE9F7_9771_4124_BD27_84B7806376AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DSConnectDlg.h : header file
//
#include "Label.h"
#include "GradientProgressCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CDSConnectDlg dialog

class CDSConnectDlg : public CDialog
{
// Construction
public:
	CDSConnectDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDSConnectDlg)
	enum { IDD = IDD_DSCONNECT_DLG };
	CLabel	m_Status_Static;
	CGradientProgressCtrl	m_Progress;
	CRichEditCtrl	m_RichEditEx;
	//}}AFX_DATA
	BOOL		m_bAtOnce;
private:
	HICON		m_hIcon;
	BOOL		m_bFirstShow;

	short		m_nMainID;
	BOOL		m_bHasPackTimer3;
	BOOL		m_bTestNLB;

	TDXWHOST *	m_pHost;
	BOOL		m_bNLB;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSConnectDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void	_SetStatusTxt(LPCSTR str);
	// Generated message map functions
	//{{AFX_MSG(CDSConnectDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnConnectcfgButton();
	//}}AFX_MSG
	afx_msg void OnNetworkErr(WPARAM wParam,LPARAM lParam);
	afx_msg void OnGetDataACK(WPARAM wParam,LPARAM lParam);
	afx_msg void OnConnectResult(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnRichEditExLink( NMHDR* in_pNotifyHeader, LRESULT* out_pResult );
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSCONNECTDLG_H__16DDE9F7_9771_4124_BD27_84B7806376AD__INCLUDED_)

#if !defined(AFX_CONNECTCFGDLG_H__BD7AD043_F406_11D3_A4CD_00E04C66B9E1__INCLUDED_)
#define AFX_CONNECTCFGDLG_H__BD7AD043_F406_11D3_A4CD_00E04C66B9E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConnectCfgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConnectCfgDlg dialog

#include "BtnST.h"

class CConnectCfgDlg : public CDialog
{
	void AddButton(TDXWHOST &Host,int nWhichCombo);
	void DelButton(TDXWHOST &Host,CComboBox *pComboBox,int nWhichCombo);
	void ModifyButton(TDXWHOST &Host,CComboBox *pComboBox,int nWhichCombo);

	void FillCombo(int which,BOOL bLast = FALSE);
	void RadioEnable();
	void EnableHostSet(BOOL bNlb);
public:
	CConnectCfgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConnectCfgDlg)
	enum { IDD = IDD_CONNECTCFG };
	CButtonST	m_btnTQCfg;
	CButtonST	m_btnAreaCfg;
	CEdit		m_ctrlProxyIP;
	CComboBox	m_InfoServerCombo;
	CComboBox	m_ServerCombo;
	CComboBox	m_WTServerCombo;
	CComboBox	m_DSServerCombo;
	CComboBox	m_L2ServerCombo;
	BOOL	m_bProxy;
	UINT	m_nProxyPort;
	CString	m_strProxyPass;
	CString	m_strProxyUser;
	int		m_nSockType;
	BOOL	m_bSameTime;
	BOOL	m_bAutoConnectInfo;
	BOOL	m_bNLB;
	BOOL	m_bAutoLogo;
	BOOL	m_bTdxProxy;
	BOOL	m_bAutoDS;
	//}}AFX_DATA
	BOOL		m_bHasWTSet;
	BOOL		m_bFirstLogoStatus;
	TDXWHOST	m_HQHost,m_InfoHost,m_WTHost,m_DSHost,m_L2Host;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnectCfgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL		m_bForbidOtherHost;	
	// Generated message map functions
	//{{AFX_MSG(CConnectCfgDlg)
	afx_msg void OnAddButton();
	afx_msg void OnModifyButton();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnProxycheck();
	afx_msg void OnInfoaddButton();
	afx_msg void OnInfomodifyButton();
	afx_msg void OnClickProxyType();
	afx_msg void OnWTAddButton();
	afx_msg void OnWTModifyButton();
	afx_msg void OnTdxproxy();
	afx_msg void OnTestproxy();
	afx_msg void OnSelectproxy();
	afx_msg void OnWTDelButton();
	afx_msg void OnHQDelButton();
	afx_msg void OnInfoDelButton();
	afx_msg void OnTqcfg();
	afx_msg void OnAreacfg();
	afx_msg void OnDsaddButton();
	afx_msg void OnDsmodifyButton();
	afx_msg void OnDsDelButton();
	afx_msg void OnL2addButton();
	afx_msg void OnL2modifyButton();
	afx_msg void OnL2DelButton();
	afx_msg void OnNlbCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNECTCFGDLG_H__BD7AD043_F406_11D3_A4CD_00E04C66B9E1__INCLUDED_)

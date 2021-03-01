#if !defined(AFX_WIDEDATADLG_H__6C1D631A_CE1C_430F_B0E2_B9F5F9726C06__INCLUDED_)
#define AFX_WIDEDATADLG_H__6C1D631A_CE1C_430F_B0E2_B9F5F9726C06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WideDataDlg.h : header file
/////////////////////////////////////////////////////////////////////////////

#include "GradientProgressCtrl.h"

class CWideDataDlg : public CDialog
{
public:
	CWideDataDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWideDataDlg)
	enum { IDD = IDD_WIDEDATA_DIALOG };
	CGradientProgressCtrl	m_ctlProgress;
	//}}AFX_DATA

	int			m_nType;
	
	short		lpnUsedStkIndex[MAXGPNUM];
	short		m_nTotalRow;

	short		m_nStockNum;
	short		m_nCurGPNo;
	BOOL		m_bGetDSData;

	CString		m_strDesc;
	
	LinkedSetInfo	*m_pLSetInfo;
	BOOL		m_bMsgAfterDeath;
	MSG			m_LastMsg;
	
	short		m_nMainID;	//‘∂≥Ã π”√MainID

	void		ContinueWant();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWideDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWideDataDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIDEDATADLG_H__6C1D631A_CE1C_430F_B0E2_B9F5F9726C06__INCLUDED_)

#if !defined(AFX_QUICKFLUSHTJDLG_H__DF358B88_9A2B_41E2_8205_8F0D25025326__INCLUDED_)
#define AFX_QUICKFLUSHTJDLG_H__DF358B88_9A2B_41E2_8205_8F0D25025326__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QuickFlushTjDlg.h : header file
//
#include "GradientProgressCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CQuickFlushTjDlg dialog

class CQuickFlushTjDlg : public CDialog
{
// Construction
public:
	CQuickFlushTjDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQuickFlushTjDlg)
	enum { IDD = IDD_FLUSHTJBLOCK_DIALOG };
	CGradientProgressCtrl	m_ctlProgress;
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	int			m_nCalcType;
	//计算板块指数
	int			m_nBlockType;
	//计算指标
	long		m_nTotalRow;
	short		*m_lpnUsedStkIndex;

	BOOL		m_bCalcFinished;
	BOOL		m_bCalcCanceled;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQuickFlushTjDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void	CalcTJBlock();
	void	CalcZSBlcok();
	void	CalcDXTJIndex1();
	void	CalcDXTJIndex2();

	// Generated message map functions
	//{{AFX_MSG(CQuickFlushTjDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QUICKFLUSHTJDLG_H__DF358B88_9A2B_41E2_8205_8F0D25025326__INCLUDED_)

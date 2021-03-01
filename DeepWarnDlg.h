#if !defined(AFX_DEEPWARNDLG_H__DF649C99_5941_49A2_9163_C3D7C955B1EB__INCLUDED_)
#define AFX_DEEPWARNDLG_H__DF649C99_5941_49A2_9163_C3D7C955B1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeepWarnDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeepWarnDlg dialog

#include "resource.h"
class CDeepWarnDlg : public CDialog
{
public:
	CDeepWarnDlg(CWnd* pParent = NULL);   // standard constructor
	~CDeepWarnDlg();
// Dialog Data
	//{{AFX_DATA(CDeepWarnDlg)
	enum { IDD = IDD_DEEPWARN_DIALOG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeepWarnDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON		m_hIcon;
	int			m_nCurSel;
	int			m_nRowNum,m_nDrawPos,m_nDrawNum,m_nLastPos;

	int			m_nFlag;
	int			m_nCurGP;
	long		m_nWarnNum;
	LPWARNATOM	m_pWarnAtom;
	
	// Generated message map functions
	//{{AFX_MSG(CDeepWarnDlg)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateWarnGPMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEEPWARNDLG_H__DF649C99_5941_49A2_9163_C3D7C955B1EB__INCLUDED_)

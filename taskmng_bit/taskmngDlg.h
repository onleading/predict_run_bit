// taskmngDlg.h : header file
//

#if !defined(AFX_TASKMNGDLG_H__8B69DF3C_8907_413B_A15B_B5857F1E14E8__INCLUDED_)
#define AFX_TASKMNGDLG_H__8B69DF3C_8907_413B_A15B_B5857F1E14E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTaskmngDlg dialog

class CTaskmngDlg : public CDialog
{
// Construction
public:
	CTaskmngDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTaskmngDlg)
	enum { IDD = IDD_TASKMNG_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskmngDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTaskmngDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void HandleTask_Calc();
	void HandleTask_Test();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKMNGDLG_H__8B69DF3C_8907_413B_A15B_B5857F1E14E8__INCLUDED_)

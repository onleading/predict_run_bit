#if !defined(AFX_FLUXCOUNTDLG_H__D6055FBB_37EE_45EC_91F0_AA292BD6183B__INCLUDED_)
#define AFX_FLUXCOUNTDLG_H__D6055FBB_37EE_45EC_91F0_AA292BD6183B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FluxCountDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CFluxCountDlg dialog

class CFluxCountDlg : public CDialog
{
// Construction
public:
	CFluxCountDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFluxCountDlg)
	enum { IDD = IDD_FLUXCOUNT };
	CString	m_strNetIn;
	CString	m_strNetOut;
	CString	m_strStartTime;
	CString	m_strEndTime;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFluxCountDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFluxCountDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnReset();
	afx_msg void OnSetstate();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void UpDateInfo(void);

private:
	BOOL		m_bInCalcState;
	BOOL		m_bReadyStart;
	CTime		m_StartTime;
	CTime		m_EndTime;
	long		m_nNetIn;
	long		m_nNetOut;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLUXCOUNTDLG_H__D6055FBB_37EE_45EC_91F0_AA292BD6183B__INCLUDED_)

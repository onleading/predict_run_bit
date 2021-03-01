#if !defined(AFX_ZSTPLAYDLG_H__11394113_5F0E_4296_BFF1_7A53AEF5E426__INCLUDED_)
#define AFX_ZSTPLAYDLG_H__11394113_5F0E_4296_BFF1_7A53AEF5E426__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ZstPlayDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CZstPlayDlg dialog

class CZstPlayDlg : public CDialog
{
// Construction
public:
	CZstPlayDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CZstPlayDlg)
	enum { IDD = IDD_ZSTPLAY };
	UINT	m_nGzPerMin;
	BOOL	m_bPlaying;
	//}}AFX_DATA
	
	BOOL	m_bHasTimer;
	
	int		m_nMinCount;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CZstPlayDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CZstPlayDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnHead();
	afx_msg void OnForward();
	afx_msg void OnNext();
	afx_msg void OnTail();
	afx_msg void OnPlaystop();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZSTPLAYDLG_H__11394113_5F0E_4296_BFF1_7A53AEF5E426__INCLUDED_)

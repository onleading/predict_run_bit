#if !defined(AFX_DESCZBDLG_H__5DD8F0A1_070F_11D5_B99A_00E04C3DCAC5__INCLUDED_)
#define AFX_DESCZBDLG_H__5DD8F0A1_070F_11D5_B99A_00E04C3DCAC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DescZBDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDescZBDlg dialog

class CDescZBDlg : public CDialog
{
// Construction
public:
	CString		m_strTitle;
	CString		m_strDesc;

	CDescZBDlg(CWnd* pParent = NULL);   // standard constructor
	void Init(int nSet, TCHAR *acCode);
	void Init(int nSet, long nIndex, BOOL bIsSpecialLine = FALSE);

// Dialog Data
	//{{AFX_DATA(CDescZBDlg)
	enum { IDD = IDD_DESCDLG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDescZBDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDescZBDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DESCZBDLG_H__5DD8F0A1_070F_11D5_B99A_00E04C3DCAC5__INCLUDED_)

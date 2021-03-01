#if !defined(TIPDLG_H_INCLUDED_)
#define TIPDLG_H_INCLUDED_
/////////////////////////////////////////////////////////////////////////////
// CTipDlg dialog

#include "resource.h"

class CTipDlg : public CDialog
{
	void GetTips();
public:
	CTipDlg(CWnd* pParent = NULL);	 // standard constructor

// Dialog Data
	//{{AFX_DATA(CTipDlg)
	enum { IDD = IDD_TIP };
	BOOL	m_bStartup;
	//}}AFX_DATA
	CString	m_strTip;
	CStdioFile TipFile;
	BOOL  bFileError;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTipDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTipDlg();

protected:
	// Generated message map functions
	//{{AFX_MSG(CTipDlg)
	afx_msg void OnNextTip();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLasttip();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(TIPDLG_H_INCLUDED_)

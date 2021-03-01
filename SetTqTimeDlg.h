#if !defined(AFX_SETTQTIMEDLG_H__CF748055_48DB_4A8F_BFE5_E4878B7F0A35__INCLUDED_)
#define AFX_SETTQTIMEDLG_H__CF748055_48DB_4A8F_BFE5_E4878B7F0A35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetTqTimeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetTqTimeDlg dialog

class CSetTqTimeDlg : public CDialog
{
// Construction
public:
	CSetTqTimeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetTqTimeDlg)
	enum { IDD = IDD_DIALOG_TQTIME };
	CTime	m_FromTime;
	int		m_PartTqType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetTqTimeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetTqTimeDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTQTIMEDLG_H__CF748055_48DB_4A8F_BFE5_E4878B7F0A35__INCLUDED_)

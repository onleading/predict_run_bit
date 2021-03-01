#if !defined(AFX_AUTHPASSMNG_H__F39C5CBB_F987_480E_A9A3_4ED4FB478479__INCLUDED_)
#define AFX_AUTHPASSMNG_H__F39C5CBB_F987_480E_A9A3_4ED4FB478479__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AuthPassMng.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAuthPassMng dialog

class CAuthPassMng : public CDialog
{
// Construction
public:
	CAuthPassMng(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAuthPassMng)
	enum { IDD = IDD_AUTHPASSMNG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAuthPassMng)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAuthPassMng)
	virtual void OnOK();
	afx_msg void OnApply();
	afx_msg void OnModify();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTHPASSMNG_H__F39C5CBB_F987_480E_A9A3_4ED4FB478479__INCLUDED_)

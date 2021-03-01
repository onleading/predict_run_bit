#if !defined(AFX_SELECTDATEDLG_H__76D99584_A254_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_SELECTDATEDLG_H__76D99584_A254_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectDateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectDateDlg dialog

class CSelectDateDlg : public CDialog
{
// Construction
public:
	CSelectDateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelectDateDlg)
	enum { IDD = IDD_SELECTDATE_DIALOG };
	CStatic	m_Separator_Static;
	CTime	m_Date1;
	CTime	m_Date2;
	//}}AFX_DATA
	BOOL	bSingle;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectDateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectDateDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTDATEDLG_H__76D99584_A254_11D3_B921_0000800104D5__INCLUDED_)

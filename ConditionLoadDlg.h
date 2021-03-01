#if !defined(AFX_CONDITIONLOADDLG_H__D47269A5_6B50_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_CONDITIONLOADDLG_H__D47269A5_6B50_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConditionLoadDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConditionLoadDlg dialog

class CConditionLoadDlg : public CDialog
{
	void FillList(int whichsel);

public:
	CConditionLoadDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConditionLoadDlg)
	enum { IDD = IDD_LOADFILE_DLG };
	CListBox	m_ConditionFile_List;
	//}}AFX_DATA
	CString ReturnStr;

	int		ConFlag;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConditionLoadDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConditionLoadDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkConditionfileList();
	afx_msg void OnDeleteit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONDITIONLOADDLG_H__D47269A5_6B50_11D3_B921_0000800104D5__INCLUDED_)

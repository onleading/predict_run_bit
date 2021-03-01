#if !defined(AFX_CUSPADSAVEDLG_H__71F8ABC9_E8A9_4847_8B37_5FD2C8F8B420__INCLUDED_)
#define AFX_CUSPADSAVEDLG_H__71F8ABC9_E8A9_4847_8B37_5FD2C8F8B420__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Label.h"
#include "EditEx.h"
// CusPadSaveDlg.h : header file
//
#define MAX_CMDNUM_STRING	4
#define MIN_CMD_NUM			1
#define MAX_CMD_NUM			99
/////////////////////////////////////////////////////////////////////////////
// CCusPadSaveDlg dialog

class CCusPadSaveDlg : public CDialog
{
// Construction
public:
	CCusPadSaveDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCusPadSaveDlg)
	enum { IDD = IDD_SAVECUSPAD };
	CEditEx	m_editEnName;
	CLabel	m_errInfo;
	CString	m_sChName;
	CString	m_sEnName;
	CString	m_sCmdNum;
	//}}AFX_DATA
	int		m_nCmdNum;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCusPadSaveDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCusPadSaveDlg)
	afx_msg void OnChangeChname();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int	m_iDynaTab;

	BOOL CheckInputCmdNum(CDC *pDC);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSPADSAVEDLG_H__71F8ABC9_E8A9_4847_8B37_5FD2C8F8B420__INCLUDED_)

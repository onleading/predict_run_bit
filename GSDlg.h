#if !defined(AFX_GSDLG_H__23F4B611_70B1_469F_947E_A54D1C03B09D__INCLUDED_)
#define AFX_GSDLG_H__23F4B611_70B1_469F_947E_A54D1C03B09D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GSDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGSDlg dialog

class CGSDlg : public CDialog
{
// Construction
public:
	CGSDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGSDlg)
	enum { IDD = IDD_GS_DLG };
	//}}AFX_DATA
	BOOL		KeyGuyFlag;
	CString		m_strGPCode;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGSDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void StartKeyBuy(BOOL bAuto,char tempchar);
	// Generated message map functions
	//{{AFX_MSG(CGSDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GSDLG_H__23F4B611_70B1_469F_947E_A54D1C03B09D__INCLUDED_)

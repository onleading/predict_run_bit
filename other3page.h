#if !defined(AFX_OTHER3PAGE_H__802A145E_0C64_4642_B708_8CB5F0A41BF4__INCLUDED_)
#define AFX_OTHER3PAGE_H__802A145E_0C64_4642_B708_8CB5F0A41BF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// other3page.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COther3Page dialog

class COther3Page : public CPropertyPage
{
	DECLARE_DYNCREATE(COther3Page)

// Construction
public:
	COther3Page();
	~COther3Page();

// Dialog Data
	//{{AFX_DATA(COther3Page)
	enum { IDD = IDD_OTHER3_PAGE };
	BOOL	m_bZBudFlag;
	BOOL	m_bAddBlockPrompt;
	BOOL	m_hideinfo;
	int		m_nMenuMode;
	BOOL	m_bExtendXPStyle;
	BOOL	m_bClassXP;
	BOOL	m_bUseAutoConnect;
	BOOL	m_bActTogether;
	BOOL	m_bFxtShowInfoDlg;
	BOOL	m_bFxtShowTab;
	BOOL	m_bTranparentTab;
	float	m_my_rate;
	float	m_gy_rate;
	BOOL	m_bBossKey;
	BOOL	m_mintotray;
	float	m_nPerScale;
	float	m_nPerSect;

	CHotKeyCtrl	m_hotkey;
	CComboBox	m_DefPeriod_Combo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COther3Page)
	public:
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COther3Page)
	virtual BOOL OnInitDialog();
	afx_msg void OnMenumode1();
	afx_msg void OnBosskeyCheck();
	afx_msg void OnPeraxisCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OTHER3PAGE_H__802A145E_0C64_4642_B708_8CB5F0A41BF4__INCLUDED_)

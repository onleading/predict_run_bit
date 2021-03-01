#if !defined(AFX_OTHERWARN2PAGE_H__778126E1_AEDB_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_OTHERWARN2PAGE_H__778126E1_AEDB_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OtherWarnPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COtherWarn2Page dialog

class COtherWarn2Page : public CPropertyPage
{
	DECLARE_DYNCREATE(COtherWarn2Page)

public:
	COtherWarn2Page();
	~COtherWarn2Page();
	
	BOOL	m_Type;		//0±¾µØ¡¡1Ô¶³Ì

// Dialog Data
	//{{AFX_DATA(COtherWarn2Page)
	enum { IDD = IDD_OTHERWARN2_PAGE };
	BOOL	m_bPopup;
	int		m_nBeepFlag;
	CString	m_strWavFile;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COtherWarn2Page)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COtherWarn2Page)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse();
	afx_msg void OnBeepRadio();
	afx_msg void OnTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OTHERWARN2PAGE_H__778126E1_AEDB_11D3_B921_0000800104D5__INCLUDED_)

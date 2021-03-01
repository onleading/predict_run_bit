#if !defined(AFX_OTHERWARNPAGE_H__778126E1_AEDB_11D3_B921_0000800104D5__INCLUDED_)
#define AFX_OTHERWARNPAGE_H__778126E1_AEDB_11D3_B921_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OtherWarnPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COtherWarnPage dialog

class COtherWarnPage : public CPropertyPage
{
	DECLARE_DYNCREATE(COtherWarnPage)

	void DisableSome();
public:
	COtherWarnPage();
	~COtherWarnPage();

// Dialog Data
	//{{AFX_DATA(COtherWarnPage)
	enum { IDD = IDD_OTHERWARN_PAGE };
	BOOL	m_bNewMax;
	BOOL	m_bNewMin;
	float	m_fDownRatio;
	UINT	m_nLBRatio;
	float	m_fUpRatio;
	float	m_fTickDiff;
	float	m_fTickVol;
	float	m_fTickAmount;
	BOOL	m_bPopup;
	BOOL	m_bStopClose;
	BOOL	m_bDownRatio;
	BOOL	m_bLB;
	BOOL	m_bTickAmount;
	BOOL	m_bTickDiff;
	BOOL	m_bTickVol;
	BOOL	m_bUpRatio;
	BOOL	m_bTickHugeAmo;
	int		m_nHugeAmoMinute;
	int		m_nHugeAmoTime;
	int		m_nBeepFlag;
	CString	m_strWavFile;
	BOOL	m_bBigBuyV;
	BOOL	m_bBigSellV;
	BOOL	m_bHighLow;
	BOOL	m_bLowHigh;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COtherWarnPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COtherWarnPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusUpratio();
	afx_msg void OnKillfocusDownratio();
	afx_msg void OnKillfocusTickdiff();
	afx_msg void OnKillfocusLbratio();
	afx_msg void OnKillfocusTickamount();
	afx_msg void OnKillfocusTickvol();
	afx_msg void OnDefaultButton();
	afx_msg void OnBupratio();
	afx_msg void OnBdownratio();
	afx_msg void OnBtickdiff();
	afx_msg void OnBlb();
	afx_msg void OnBtickamount();
	afx_msg void OnBtickhugeamo();
	afx_msg void OnBrowse();
	afx_msg void OnBeepRadio();
	afx_msg void OnTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OTHERWARNPAGE_H__778126E1_AEDB_11D3_B921_0000800104D5__INCLUDED_)

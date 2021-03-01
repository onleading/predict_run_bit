#if !defined(AFX_OTHERPAGE_H__722C1227_1F72_11D3_98A5_0088CC032229__INCLUDED_)
#define AFX_OTHERPAGE_H__722C1227_1F72_11D3_98A5_0088CC032229__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OtherPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COtherPage dialog

class COtherPage : public CPropertyPage
{
	DECLARE_DYNCREATE(COtherPage)

// Construction
public:
	COtherPage();
	~COtherPage();

// Dialog Data
	//{{AFX_DATA(COtherPage)
	enum { IDD = IDD_OTHER_PAGE };
	CComboBox	m_ChartCombo;
	int		m_AutoPageTime;
	int		m_ZFPeriod;
	BOOL	m_bXaxis;
	BOOL	m_bYaxis;
	UINT	m_nZFMin;
	UINT	m_nMinn;
	UINT	m_nDayn;
	BOOL	m_bVolTQ;
	BOOL	m_bNotifyDownload;
	BOOL	m_bNotifyBigBsp;
	BOOL	m_bHisViewTQ;
	int		m_nToolBarPos;
	BOOL	m_bShowHqMine;
	BOOL	m_bLongSquare;
	BOOL	m_bDrawBlue;
	long	m_nHligtNowVol;
	BOOL	m_bNoXHCol;
	BOOL	m_bNoVScroll;
	BOOL	m_bAutoHideBar;
	short	m_nFixedXAxis;
	BOOL	m_bFixedXAxis;
	BOOL	m_bOnlyDownAGData;
	BOOL	m_bHlight_HighLow;
	BOOL	m_bCodeCanSort;
	BOOL	m_bLockHQSort;
	//}}AFX_DATA

	int		m_nChartNum;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COtherPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COtherPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnOtherButton1();
	afx_msg void OnOtherButton3();
	afx_msg void OnKillfocusAutopagetime();
	afx_msg void OnDefwidthButton();
	afx_msg void OnAutohidebarCheck();
	afx_msg void OnVolTQCheck();
	afx_msg void OnFixxaxis();
	afx_msg void OnXaxisCheck();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OTHERPAGE_H__722C1227_1F72_11D3_98A5_0088CC032229__INCLUDED_)

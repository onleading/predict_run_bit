#if !defined(AFX_OTHERPAG4_H__BFABF8EF_412F_4370_A4F1_E811504F9A9C__INCLUDED_)
#define AFX_OTHERPAG4_H__BFABF8EF_412F_4370_A4F1_E811504F9A9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OtherPag4.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COther4Page dialog
#include "Label.h"

class COther4Page : public CPropertyPage
{
	DECLARE_DYNCREATE(COther4Page)
// Construction
public:
	COther4Page();
	~COther4Page();

// Dialog Data
	//{{AFX_DATA(COther4Page)
	enum { IDD = IDD_OTHER4_PAGE };
	BOOL	m_bDiffVolColor;
	CLabel	m_ZS4_Static;
	CLabel	m_ZS3_Static;
	CLabel	m_ZS2_Static;
	CLabel	m_ZS1_Static;
	BOOL	m_bPillarBSChart;
	BOOL	m_bBigHq;
	BOOL	m_bBoldZSTLine;
	BOOL	m_bLeftHQInfo;
	BOOL	m_bBigStatusBar;
	int		m_nInitKNum;
	BOOL	m_bHasInitKNum;
	int		m_nInitKNum2;
	BOOL	m_bHasInitKNum2;
	BOOL	m_bShowBS;
	BOOL	m_bShowKGap;
	int		m_nShowJunj;
	BOOL	m_bShowMmpChg;
	BOOL	m_bPinAssistPad;
	BOOL	m_bShowTPPriceInZST;
	BOOL	m_bLocalData;
	BOOL	m_bEmptyZstRight;
	BOOL	m_bZsShowAverage;
	BOOL	m_bZsShowLX;
	BOOL	m_bDenseZSTAxis;
	BOOL	m_bHq10Bottom;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COther4Page)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COther4Page)
	virtual BOOL OnInitDialog();
	afx_msg void OnZs1Button();
	afx_msg void OnZs2Button();
	afx_msg void OnZs3Button();
	afx_msg void OnPillarChartCheck();
	afx_msg void OnBighqCheck();
	afx_msg void OnFxtknum();
	afx_msg void OnFxtknum2();
	afx_msg void OnCheckLocaldata();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OTHERPAG4_H__BFABF8EF_412F_4370_A4F1_E811504F9A9C__INCLUDED_)

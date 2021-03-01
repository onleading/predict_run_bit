#if !defined(AFX_TABLETITLEDLG_H__7BEF45C3_BE0A_4128_823B_B3F5F6DFCF4F__INCLUDED_)
#define AFX_TABLETITLEDLG_H__7BEF45C3_BE0A_4128_823B_B3F5F6DFCF4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TableTitleDlg.h : header file
//

#include "Label.h"

class CCWTableUnit;
/////////////////////////////////////////////////////////////////////////////
// CTableTitleDlg dialog

class CTableTitleDlg : public CDialog
{
// Construction
public:
	CTableTitleDlg(CWnd* pParent = NULL);   // standard constructor

	void	SetTableUnit(CCWTableUnit *pTableUnit)
	{
		m_pTableUnit = pTableUnit;
	}

	void	SetTitle(CString strTitle)
	{
		m_TitleLabel.SetWindowText(strTitle);
	}

	void	GetTimeParams(TableBGQ *pBgq);
	void	ShowMore(short nTimeFlag, short nGpInputFlag);

// Dialog Data
	//{{AFX_DATA(CTableTitleDlg)
	enum { IDD = IDD_TABLETITLE_DLG };
	CLabel	m_TitleLabel;
	CTime	m_CWTime1;
	CTime	m_CWTime2;
	CString	m_strGpEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTableTitleDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTableTitleDlg)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnChangeGpedit();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonRefresh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CCWTableUnit *m_pTableUnit;
	short	m_nTimeFlag;
	short	m_nGpInputFlag;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABLETITLEDLG_H__7BEF45C3_BE0A_4128_823B_B3F5F6DFCF4F__INCLUDED_)

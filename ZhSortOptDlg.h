#if !defined(AFX_ZhSortOptDlg_H__64D7D541_07F9_4D51_BE16_960DB4080216__INCLUDED_)
#define AFX_ZhSortOptDlg_H__64D7D541_07F9_4D51_BE16_960DB4080216__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ZhSortOptDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ZhSortOptDlg dialog

class ZhSortOptDlg : public CDialog
{
// Construction
public:
	ZhSortOptDlg(CWnd* pParent = NULL);   // standard constructor

	int		*m_aShowSort;				//现实项目及顺序

// Dialog Data
	//{{AFX_DATA(ZhSortOptDlg)
	enum { IDD = IDD_ZHSORTOPT };
	CListCtrl	m_showSortList;
	CListCtrl	m_allSortList;
	int		m_nShowNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ZhSortOptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList m_ImageList;
	char	TypeStr[9][25];
	// Generated message map functions
	//{{AFX_MSG(ZhSortOptDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnRadio4();
	afx_msg void OnRadio9();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void	Fill2List(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ZhSortOptDlg_H__64D7D541_07F9_4D51_BE16_960DB4080216__INCLUDED_)

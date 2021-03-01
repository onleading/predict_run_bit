#if !defined(AFX_SHOWINFODLG_H__833E9C61_C9F7_11D3_8B1F_00E04C66B9E1__INCLUDED_)
#define AFX_SHOWINFODLG_H__833E9C61_C9F7_11D3_8B1F_00E04C66B9E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowInfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShowInfoDlg dialog

class CShowInfoDlg : public CDialog
{
// Construction
public:
	CShowInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CShowInfoDlg)
	enum { IDD = IDD_SHOWINFO_DIALOG };
	CListCtrl	m_List;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShowInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList	m_ImageList;
	// Generated message map functions
	//{{AFX_MSG(CShowInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSeeurgent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWINFODLG_H__833E9C61_C9F7_11D3_8B1F_00E04C66B9E1__INCLUDED_)

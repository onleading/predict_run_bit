#if !defined(AFX_FINDRESULTDLG_H__39DDC261_EC8F_11D4_BB9D_00E04C66B1EB__INCLUDED_)
#define AFX_FINDRESULTDLG_H__39DDC261_EC8F_11D4_BB9D_00E04C66B1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindResultDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindResultDlg dialog

class CFindResultDlg : public CDialog
{
// Construction
public:
	CFindResultDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFindResultDlg)
	enum { IDD = IDD_FINDRESULT_DIALOG };
	CListCtrl	m_Result_List;
	//}}AFX_DATA
	
	int			m_nStyle;	//0:个股资料 1:公告消息
	CImageList	m_ImageList;

	struct tag_FindResult	m_FindResult[MAX_FINDRESULT];
	struct tag_FindResult2	m_FindResult2[MAX_FINDRESULT];
	int	m_nFindNum;
	int m_nCurSel;

	CString m_strFindFileName;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindResultDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindResultDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkResultList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDRESULTDLG_H__39DDC261_EC8F_11D4_BB9D_00E04C66B1EB__INCLUDED_)

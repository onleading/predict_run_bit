#if !defined(AFX_EMBEDNEWSDLG_H__2BBE75F5_1AC4_44A4_96F2_6FD5A35CD3D3__INCLUDED_)
#define AFX_EMBEDNEWSDLG_H__2BBE75F5_1AC4_44A4_96F2_6FD5A35CD3D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EmbedNewsDlg.h : header file
//

class UHKNews;
class CMyRichEdit;
/////////////////////////////////////////////////////////////////////////////
// CEmbedNewsDlg dialog

class CHKNewsContentDlg : public CDialog
{
// Construction
public:
	CHKNewsContentDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHKNewsContentDlg();

// Dialog Data
	//{{AFX_DATA(CEmbedNewsDlg)
	enum { IDD = IDD_HKNEWSCONTENT };
	//}}AFX_DATA

public:
	void	UpdateContent(CString &strContent);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEmbedNewsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEmbedNewsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	UHKNews		*m_pNewsUnit;
	CString		m_NewsContent;

private:
	HICON		m_hIcon;
	CMyRichEdit	*m_pRichEdit;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMBEDNEWSDLG_H__2BBE75F5_1AC4_44A4_96F2_6FD5A35CD3D3__INCLUDED_)

#if !defined(AFX_HMFLASHLOGO_H__3495E829_18DE_4E7D_910A_71D153902FA5__INCLUDED_)
#define AFX_HMFLASHLOGO_H__3495E829_18DE_4E7D_910A_71D153902FA5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HMFlashLogo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHMFlashLogo dialog

class CHMFlashLogo : public CDialog
{
// Construction
public:
	CHMFlashLogo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHMFlashLogo)
	enum { IDD = IDD_HM_FLASH_LOGO };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHMFlashLogo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHMFlashLogo)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HMFLASHLOGO_H__3495E829_18DE_4E7D_910A_71D153902FA5__INCLUDED_)

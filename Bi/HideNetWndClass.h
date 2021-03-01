#if !defined(AFX_HIDENETWNDCLASS_H__47182F98_CB13_41AE_972E_CD13588E5336__INCLUDED_)
#define AFX_HIDENETWNDCLASS_H__47182F98_CB13_41AE_972E_CD13588E5336__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HideNetWndClass.h : header file
//

class CHideNetWnd;
/////////////////////////////////////////////////////////////////////////////
// CHideNetWndClass window

class CHideNetWndClass : public CWnd
{
// Construction
public:
	CHideNetWndClass(CHideNetWnd *pOwner);

// Attributes
public:
	CHideNetWnd *m_pOwnerThread;

// Operations
public:
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHideNetWndClass)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHideNetWndClass();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHideNetWndClass)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HIDENETWNDCLASS_H__47182F98_CB13_41AE_972E_CD13588E5336__INCLUDED_)

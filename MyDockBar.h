#if !defined(AFX_MYDOCKBAR_H__49CEEF13_D26C_4E72_AC69_D8466669AD8D__INCLUDED_)
#define AFX_MYDOCKBAR_H__49CEEF13_D26C_4E72_AC69_D8466669AD8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyDockBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyDockBar window

class CMyDockBar : public CDockBar
{
// Construction
public:
	CMyDockBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyDockBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyDockBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyDockBar)
	//}}AFX_MSG
	afx_msg LRESULT OnSizeParent(WPARAM wParam,	LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYDOCKBAR_H__49CEEF13_D26C_4E72_AC69_D8466669AD8D__INCLUDED_)

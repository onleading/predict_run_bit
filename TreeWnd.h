#if !defined(AFX_TREEWND_H__86FBA1E0_8862_4E7D_8B11_B27E60F2EEE3__INCLUDED_)
#define AFX_TREEWND_H__86FBA1E0_8862_4E7D_8B11_B27E60F2EEE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeWnd window


class CTreeWnd : public CWnd
{
// Construction
public:
	CTreeWnd();

// Attributes
public:
	BOOL KeyGuyFlag;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEWND_H__86FBA1E0_8862_4E7D_8B11_B27E60F2EEE3__INCLUDED_)

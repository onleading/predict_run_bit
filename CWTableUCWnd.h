#if !defined(AFX_CWTABLEUCWND_H__30A43F68_3370_4726_9050_138FD2FF537D__INCLUDED_)
#define AFX_CWTABLEUCWND_H__30A43F68_3370_4726_9050_138FD2FF537D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CWTableUCWnd.h : header file
//

class CCWTableUnit;
/////////////////////////////////////////////////////////////////////////////
// CCWTableUCWnd window

class CCWTableUCWnd : public CWnd
{
// Construction
public:
	CCWTableUCWnd(CCWTableUnit *pCWTableU);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCWTableUCWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCWTableUCWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCWTableUCWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg	void OnAckSQLData(WPARAM wParam,LPARAM lParam);
	afx_msg	void OnListSelChange(WPARAM wParam,LPARAM lParam);
	afx_msg	void OnListColSort(WPARAM wParam,LPARAM lParam);
	afx_msg	void OnTabSelChange(WPARAM wParam,LPARAM lParam);
	afx_msg void OnListRowGnPop(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	CCWTableUnit	*m_pCWTableU;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CWTABLEUCWND_H__30A43F68_3370_4726_9050_138FD2FF537D__INCLUDED_)

#if !defined(AFX_CWCHARTUCWND_H__CF36C937_04C4_485F_8A75_0AF6A0249992__INCLUDED_)
#define AFX_CWCHARTUCWND_H__CF36C937_04C4_485F_8A75_0AF6A0249992__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CWChartUCWnd.h : header file
//

class CCWChartUnit;
/////////////////////////////////////////////////////////////////////////////
// CCWChartUCWnd window

class CCWChartUCWnd : public CWnd
{
// Construction
public:
	CCWChartUCWnd(CCWChartUnit *pCWChartU);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCWChartUCWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCWChartUCWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCWChartUCWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CCWChartUnit	*m_pCWChart;
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CWCHARTUCWND_H__CF36C937_04C4_485F_8A75_0AF6A0249992__INCLUDED_)

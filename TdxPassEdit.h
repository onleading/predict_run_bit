#if !defined(AFX_TDXPASSEDIT_H__10D48FC3_EB80_4971_90C5_12F95D161F47__INCLUDED_)
#define AFX_TDXPASSEDIT_H__10D48FC3_EB80_4971_90C5_12F95D161F47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TdxPassEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTdxPassEdit window

class CTdxPassEdit : public CEdit
{
// Construction
public:
	CTdxPassEdit();

// Attributes
public:
	BOOL m_bWantGet;
// Operations
public:
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTdxPassEdit)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTdxPassEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTdxPassEdit)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDXPASSEDIT_H__10D48FC3_EB80_4971_90C5_12F95D161F47__INCLUDED_)

#if !defined(AFX_MYRICHEDIT_H__C65350E2_5A0D_11D3_8C7B_0000800104D5__INCLUDED_)
#define AFX_MYRICHEDIT_H__C65350E2_5A0D_11D3_8C7B_0000800104D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyRichEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyRichEdit window

class CMyRichEdit : public CRichEditCtrl
{
// Construction
public:
	CMyRichEdit();
	void SetMenuIndex(int index);
	void ApplyTxtSize();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyRichEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyRichEdit();
	CFont *	m_pFont;
	int		m_menuIndex;
	BOOL	m_bUseTxtColor;
	// Generated message map functions
protected:
	//{{AFX_MSG(CMyRichEdit)
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYRICHEDIT_H__C65350E2_5A0D_11D3_8C7B_0000800104D5__INCLUDED_)

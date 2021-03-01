#if !defined(AFX_ICONBUTTON_H__43CE12A5_3089_40E7_8ECA_11688A4689C9__INCLUDED_)
#define AFX_ICONBUTTON_H__43CE12A5_3089_40E7_8ECA_11688A4689C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IConButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIconButton window

class CIconButton : public CButton
{
// Construction
public:
	CIconButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIconButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	void EnableBorder(bool bEnable);
	void SetDisableColor(COLORREF clr);
	void SetHoverColor(COLORREF clr);
	void SetBkColor(COLORREF clr);
 	bool m_bHover;
	bool m_bEnableBorder;
	void SetIcon(HICON hIcon);
	HICON m_hIcon;
	virtual ~CIconButton();
	COLORREF m_clrBkColor,m_clrHoverColor,m_clrDisableColor;

	// Generated message map functions
protected:
	//{{AFX_MSG(CIconButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
    afx_msg LONG OnMouseLeave(WPARAM,LPARAM);
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );
	afx_msg void OnKillFocus( CWnd* pNewWnd );


	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICONBUTTON_H__43CE12A5_3089_40E7_8ECA_11688A4689C9__INCLUDED_)

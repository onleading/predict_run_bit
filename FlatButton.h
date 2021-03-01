#if !defined(AFX_FLATBUTTON_H__F81B0697_A27F_4239_BDD8_D4CB0812E392__INCLUDED_)
#define AFX_FLATBUTTON_H__F81B0697_A27F_4239_BDD8_D4CB0812E392__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FlatButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFlatButton window

class CFlatButton : public CButton
{
// Construction
public:
	CFlatButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlatButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	COLORREF GetSelColor();
	COLORREF GetHoverColor();
	COLORREF GetTxtColor();
	COLORREF GetBkColor();
	void SetSelColor(COLORREF clr);
	bool IsBtnChecked();
	void CheckButton(bool bCheck);
	void SetHoverColor(COLORREF clr);
	void SetBkColor(COLORREF clr);
	void SetTextColor(COLORREF clr);
	void SetFont(CFont *pFont);

	virtual ~CFlatButton();
	bool m_bHover;
	bool m_bChecked;
	COLORREF m_clrBkColor,m_clrTextColor,m_clrHoverColor,m_clrSelColor;
	CFont *m_pFont;
	// Generated message map functions
protected:
	//{{AFX_MSG(CFlatButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
    afx_msg LONG OnMouseLeave(WPARAM,LPARAM);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLATBUTTON_H__F81B0697_A27F_4239_BDD8_D4CB0812E392__INCLUDED_)

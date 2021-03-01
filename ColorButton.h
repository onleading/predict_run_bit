#if !defined(AFX_COLORBUTTON_H__A0F7DAC0_2C90_48E6_BF36_D8FCC7EBF323__INCLUDED_)
#define AFX_COLORBUTTON_H__A0F7DAC0_2C90_48E6_BF36_D8FCC7EBF323__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorButton window

class CColorButton : public CButton
{
// Construction
public:
	CColorButton();

// Attributes
public:
private:
	COLORREF m_color;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL
	COLORREF GetColor();
	COLORREF SetColor(COLORREF crf);
	void Refresh();

// Implementation
public:
	virtual ~CColorButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorButton)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORBUTTON_H__A0F7DAC0_2C90_48E6_BF36_D8FCC7EBF323__INCLUDED_)

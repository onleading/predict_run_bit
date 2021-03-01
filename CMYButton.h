// CMYButton.h : header file
/////////////////////////////////////////////////////////////////////////////

#if !defined(CMYBUTTON_H_INCLUDED)
#define CMYBUTTON_H_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CMyButton class

class CMyButton : public CButton
{
	DECLARE_DYNAMIC(CMyButton)

// Construction
public:
	CMyButton();

// Attributes
protected:
	CSize       m_cSize;
	CRect		m_rcItem;
	CRect		m_rcIcon;
	UINT		m_nState;
	bool		m_bLBtnDown;
	bool		m_bFlatLook;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void SetIcon(HICON hIcon,CSize cSize);
	//}}AFX_VIRTUAL

// Implementation
public:
	void DisableFlatLook() { m_bFlatLook = false; }
	void SetIconRect();
	virtual ~CMyButton();

// Generated message map functions
protected:
	//{{AFX_MSG(CMyButton)
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(CJBUTTON_H_INCLUDED)


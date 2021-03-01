#if !defined(AFX_COLORLISTBOX_H__89F520C1_AFE2_11D4_BB9C_00E04C66B1EB__INCLUDED_)
#define AFX_COLORLISTBOX_H__89F520C1_AFE2_11D4_BB9C_00E04C66B1EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorListBox.h : header file
/////////////////////////////////////////////////////////////////////////////

class CColorListBox : public CListBox
{
// Construction
public:
	CColorListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorListBox();
	void	SetCustomHeight(int nHeight);

	// Generated message map functions
protected:
	COLORREF	m_ItemColorValue[6];
	int			m_nHeight;

	//{{AFX_MSG(CColorListBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORLISTBOX_H__89F520C1_AFE2_11D4_BB9C_00E04C66B1EB__INCLUDED_)

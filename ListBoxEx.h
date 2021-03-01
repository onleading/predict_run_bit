#if !defined(AFX_LISTBOXEX_H__3443B749_F084_4BAF_A8C3_1784C317BC0D__INCLUDED_)
#define AFX_LISTBOXEX_H__3443B749_F084_4BAF_A8C3_1784C317BC0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListBoxEx.h : header file
//

class UBase;
/////////////////////////////////////////////////////////////////////////////
//  CListBoxEx window

//每一项的相关连的数值低二位为图标索引，高二位表示是否缩进

class CListBoxEx : public CListBox
{
// Construction
public:
	CListBoxEx();

// Attributes
public:
	UBase		*	m_pUnit;	
	int				m_iMaxWidth;
	int				m_iItemHeight;
	bool			m_bIconListBox;
	CImageList		m_ImageList;
	COLORREF		m_clrBackground,m_clrText,m_clrFocused,m_clrSpecial;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListBoxEx)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	void	SetSpecialItemColor(COLORREF clr);
	int		GetItemHeight();
	void	SetSpecialItem(int index);
	void	SetFocusColor(COLORREF clr);
	void	ResetContent();
	void	SetBKColor(COLORREF clr);
	void	SetTextColor(COLORREF clr);
	void	SetItemHeight(int height);
	void	SetImageList(CImageList *pImage);
	void	SetItemIcon(int itemindex,int iconindex);
	void	IndentItem(int index);
	int		InsertString(int nIndex,LPCTSTR lpszItem);
	int		AddString(LPCTSTR lpszItem);
	virtual ~CListBoxEx();

	void	RegisterMsg(UINT message);

	// Generated message map functions
protected:
	//{{AFX_MSG(CListBoxEx)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
    afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );   
	DECLARE_MESSAGE_MAP()
private:
	vector<UINT>	m_aRegedMsg;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTBOXEX_H__3443B749_F084_4BAF_A8C3_1784C317BC0D__INCLUDED_)

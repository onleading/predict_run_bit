#if !defined(AFX_MYSWITCHBAR2_H__1B64A792_7A61_4E5C_8B89_5EEE6E12999D__INCLUDED_)
#define AFX_MYSWITCHBAR2_H__1B64A792_7A61_4E5C_8B89_5EEE6E12999D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// myswitchbar.h : header file
/////////////////////////////////////////////////////////////////////////////

#define MAX_SWTICH_NUM		20

struct tag_SwitchLabel2
{
	char caption[50];
};

class CMySwitchBar2 : public CWnd
{
	void		OnClick(CPoint point);
	void		DrawOEM(CDC *pDC);
	void		DrawInfo(CDC *pDC);
	void		DrawNormal(CDC *pDC);
	void		DrawIt();
public:
	CMySwitchBar2();

	void		Init(HWND hwnd,UINT SelChangeMsg);
	void		SetMinTabWidth(int minWidth);
	CImageList* SetImageList (CImageList *pImageList);
	void		ClearAllItem();
	BOOL		AddItem(char *caption);

	void		SetCurSel(int whichFl);
	int			GetCurSel();
// Attributes
private:
	HWND	m_MsghWnd;
	UINT    m_SelChangeMsg;
	int		m_nMinWidth;
	int		m_nMaxWidth;

	int		m_nLabelNum;
	int		m_nCurLabel;

	CBrush m_brBackground;

	struct tag_SwitchLabel2	m_SwitchLabel[MAX_SWTICH_NUM];

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMySwitchBar2)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMySwitchBar2();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	// Generated message map functions
protected:
	//{{AFX_MSG(CMySwitchBar2)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSWITCHBAR2_H__1B64A792_7A61_4E5C_8B89_5EEE6E12999D__INCLUDED_)

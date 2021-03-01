#if !defined(AFX_MYTREECTRL_H__A5E161DC_CF7B_494F_AC13_DB9994E6CB08__INCLUDED_)
#define AFX_MYTREECTRL_H__A5E161DC_CF7B_494F_AC13_DB9994E6CB08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyTreeCtrl window

#define TVS_NOHSCROLL 0x8000

class CMyTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CMyTreeCtrl(BOOL bOwnerDraw=FALSE);

// Attributes
public:
	CBitmap m_bitmap;
	BOOL m_bOwnerDraw;
// Operations
public:
	BOOL SetBKImage(LPCTSTR LpszResource);
	void SetBkMode(BOOL bOwnerDraw=FALSE);
	virtual void DrawBKImage(CDC *pDC,CRect rc);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyTreeCtrl)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyTreeCtrl();

	static int GetExpandTreeNodeNum(CMyTreeCtrl *pTreeCtrl);
	static HTREEITEM GetTreeItem(CMyTreeCtrl *pTreeCtrl,int nPos);
	static int GetTreeItemPos(CMyTreeCtrl *pTreeCtrl,HTREEITEM hItem);

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyTreeCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYTREECTRL_H__A5E161DC_CF7B_494F_AC13_DB9994E6CB08__INCLUDED_)

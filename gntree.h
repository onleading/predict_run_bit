#if !defined(AFX_GNTREE_H__6699B088_A755_4ADE_A9C9_238FCDD80514__INCLUDED_)
#define AFX_GNTREE_H__6699B088_A755_4ADE_A9C9_238FCDD80514__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// gntree.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CGnTree window
#include "MyScrollbar.h"
#include "mytreectrl.h"
#include "TreeWnd.h"

class CGnTree : public CTreeWnd
{
// Construction
public:
	CGnTree();
	DECLARE_DYNCREATE(CGnTree)

	CMyTreeCtrl*	m_TreeCtrl;
	CFont	*		m_pNewFont;
	CImageList		m_ImageList;

	CMyScrollBar	*pVScrollBar;
// Attributes
public:

// Operations
public:
	void SetAppearance(COLORREF bColor,COLORREF fColor);
	void FillTree();
	void SetScrollPos();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGnTree)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGnTree();

	// Generated message map functions
protected:
	afx_msg void OnVScrollMessage(WPARAM wParam,LPARAM lParam);
	//{{AFX_MSG(CGnTree)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnNcDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GNTREE_H__6699B088_A755_4ADE_A9C9_238FCDD80514__INCLUDED_)

#if !defined(AFX_TREEVIEWEX_H__0FDDABA3_1E72_11D6_A264_00A0CC549430__INCLUDED_)
#define AFX_TREEVIEWEX_H__0FDDABA3_1E72_11D6_A264_00A0CC549430__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeViewEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeViewEx view

class CTreeViewEx : public CTreeView
{
protected:
	DECLARE_DYNCREATE(CTreeViewEx)

// Attributes
public:
	CTreeViewEx();           // protected constructor used by dynamic creation
// Operations
public:
	virtual ~CTreeViewEx();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeViewEx)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeViewEx)
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEVIEWEX_H__0FDDABA3_1E72_11D6_A264_00A0CC549430__INCLUDED_)

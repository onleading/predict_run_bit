#if !defined(AFX_DRAGLISTCTRL_H__61914C66_B201_11D3_B921_00E04C66B9E1__INCLUDED_)
#define AFX_DRAGLISTCTRL_H__61914C66_B201_11D3_B921_00E04C66B9E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DragListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDragListCtrl window

class CDragListCtrl : public CListCtrl
{
// Construction
public:
	CDragListCtrl();

	void SetDragType(BOOL bGP=TRUE);
// Attributes
public:
	CImageList* m_pDragImage;
	BOOL		m_bDragging;
	int			m_nDragIndex;
	int			DraggingGPNo;
	CWnd*		m_pDropWnd;

	BOOL		m_bGP; //拖的是不是股票,缺省是股票
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDragListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDragListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDragListCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAGLISTCTRL_H__61914C66_B201_11D3_B921_00E04C66B9E1__INCLUDED_)

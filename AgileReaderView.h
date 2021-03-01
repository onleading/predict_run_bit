// AgileReaderView.h : interface of the CAgileReaderView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AgileReaderVIEW_H__04317766_98C5_410E_8783_1290020D2B68__INCLUDED_)
#define AFX_AgileReaderVIEW_H__04317766_98C5_410E_8783_1290020D2B68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgileReaderDoc.h"

class CAgileReaderView : public CListView
{
protected: // create from serialization only
	CAgileReaderView();
	DECLARE_DYNCREATE(CAgileReaderView)

// Attributes
public:
	CAgileReaderDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAgileReaderView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAgileReaderView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAgileReaderView)
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSortDateTime(WPARAM wParam,LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bSortListAsc;
	int  m_nSortListItem;
};

#ifndef _DEBUG  // debug version in AgileReaderView.cpp
inline CAgileReaderDoc* CAgileReaderView::GetDocument()
   { return (CAgileReaderDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AgileReaderVIEW_H__04317766_98C5_410E_8783_1290020D2B68__INCLUDED_)

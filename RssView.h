#if !defined(AFX_RSSVIEW_H__98732B74_18D5_40E1_ABC7_635AA236AF97__INCLUDED_)
#define AFX_RSSVIEW_H__98732B74_18D5_40E1_ABC7_635AA236AF97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RssView.h : header file
//
#include "FeedSource.h"
/////////////////////////////////////////////////////////////////////////////
// CRssView view

class CRssView : public CView
{
	class CNode
	{
	public: 
		CNode( CString strLink )
		{
			m_strLink = strLink;
		}
		virtual ~CNode()
		{
		}
		CString		m_strLink;
	};

protected:
	CRssView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRssView)

// Attributes
protected:
	CSplitterWnd m_wndSplitter;
	CSplitterWnd m_wndSplitterRight;
	BOOL		 m_bCreate;
	UINT		 m_nTimer;
	int			 m_nRate;
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	virtual void OnInitialUpdate();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	CHtmlView* GetHtmlView();
	CListCtrl* GetListCtrl();
	CTreeCtrl* GetTreeCtrl();
	virtual ~CRssView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CRssView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	afx_msg LONG OnTreeDblClick(UINT wParam, LONG lParam);
	afx_msg LONG OnTreeItemChange(UINT wParam, LONG lParam);
	afx_msg LONG OnListDblClick(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()
private:
	CWinThread* m_pThread;
	BOOL		m_bRunning;
	CString NormalizeDate( CString strValue );
	CImageList m_image;
	static UINT ProcRefreshAll( void* pParam );
	void DisplayFeed( CFeed& feed );
	void Release( HTREEITEM hParent );
	void AddTreeNode( CString strTitle, CString strLink );
	void InitList();
	void InitTree();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RSSVIEW_H__98732B74_18D5_40E1_ABC7_635AA236AF97__INCLUDED_)

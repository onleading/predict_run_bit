// TreeViewEx.cpp : implementation file
//

#include "stdafx.h"
#include "TreeViewEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeViewEx

IMPLEMENT_DYNCREATE(CTreeViewEx, CTreeView)

CTreeViewEx::CTreeViewEx()
{
}

CTreeViewEx::~CTreeViewEx()
{
}


BEGIN_MESSAGE_MAP(CTreeViewEx, CTreeView)
	//{{AFX_MSG_MAP(CTreeViewEx)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeViewEx drawing

void CTreeViewEx::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CTreeViewEx diagnostics

#ifdef _DEBUG
void CTreeViewEx::AssertValid() const
{
	CTreeView::AssertValid();
}

void CTreeViewEx::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTreeViewEx message handlers

void CTreeViewEx::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	theViewManager.pActiveView->SendMessage( WM_TREEDCLICK );	
	*pResult = 0;
}

void CTreeViewEx::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	theViewManager.pActiveView->SendMessage( WM_TREEITEMCHANGE );	
	*pResult = 0;
}

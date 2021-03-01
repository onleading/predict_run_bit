// AgileReaderView.cpp : implementation of the CAgileReaderView class
//

#include "stdafx.h"
#include "AgileReaderDoc.h"
#include "AgileReaderView.h"
#include "ListCtrlSortClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAgileReaderView

IMPLEMENT_DYNCREATE(CAgileReaderView, CListView)

BEGIN_MESSAGE_MAP(CAgileReaderView, CListView)
	//{{AFX_MSG_MAP(CAgileReaderView)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_MESSAGE(UM_RSSDATETIMESORT, OnSortDateTime)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAgileReaderView construction/destruction

CAgileReaderView::CAgileReaderView()
{
	// TODO: add construction code here

}

CAgileReaderView::~CAgileReaderView()
{
}

BOOL CAgileReaderView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style |= LVS_REPORT;

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAgileReaderView drawing

void CAgileReaderView::OnDraw(CDC* pDC)
{
	CAgileReaderDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

void CAgileReaderView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
}

/////////////////////////////////////////////////////////////////////////////
// CAgileReaderView printing

BOOL CAgileReaderView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CAgileReaderView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CAgileReaderView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CAgileReaderView diagnostics

#ifdef _DEBUG
void CAgileReaderView::AssertValid() const
{
	CListView::AssertValid();
}

void CAgileReaderView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CAgileReaderDoc* CAgileReaderView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAgileReaderDoc)));
	return (CAgileReaderDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAgileReaderView message handlers

void CAgileReaderView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	theViewManager.pActiveView->SendMessage( WM_LISTDCLICK );	
	*pResult = 0;
}

/////////////////////////////////////////////////////////////////////////////
// Sort Column
//
void CAgileReaderView::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	if (pNMListView->iSubItem == m_nSortListItem)
	{
		m_bSortListAsc = !m_bSortListAsc;
	}
	else
	{
		m_nSortListItem = pNMListView->iSubItem;
		m_bSortListAsc = TRUE;
	}

	// Sort list
	CListCtrlSortClass sc(&GetListCtrl(), m_nSortListItem);
	switch ( m_nSortListItem )
	{
		case 5:
			sc.Sort(m_bSortListAsc, CListCtrlSortClass::SortDataType::dtDATETIME);
			break;
		default:
			sc.Sort(m_bSortListAsc, CListCtrlSortClass::SortDataType::dtSTRING);
			break;
	}

	*pResult = 0;
}

void	CAgileReaderView::OnSortDateTime(WPARAM wParam,LPARAM lParam)
{
	m_bSortListAsc = FALSE;
	CListCtrlSortClass sc(&GetListCtrl(), 5);
	sc.Sort(m_bSortListAsc, CListCtrlSortClass::SortDataType::dtDATETIME);
}

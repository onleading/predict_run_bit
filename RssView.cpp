// RssView.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "TreeViewEx.h"
#include "AgileReaderView.h"
#include "RssView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRssView

IMPLEMENT_DYNCREATE(CRssView, CView)

CRssView::CRssView()
{
	// TODO: add member initialization code here
	m_bCreate = FALSE;
	m_image.Create( IDB_READER, 16, 0, RGB(255, 255, 255) );
	m_pThread = NULL;
	m_bRunning = FALSE;
	m_nRate = 60*1000;
	m_nTimer = 0;
}

CRssView::~CRssView()
{
}


BEGIN_MESSAGE_MAP(CRssView, CView)
	//{{AFX_MSG_MAP(CRssView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_TREEDCLICK, OnTreeDblClick)
	ON_MESSAGE(WM_LISTDCLICK, OnListDblClick)
	ON_MESSAGE(WM_TREEITEMCHANGE, OnTreeItemChange)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CRssView diagnostics

#ifdef _DEBUG
void CRssView::AssertValid() const
{
	CView::AssertValid();
}

void CRssView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRssView message handlers
void CRssView::OnDraw(CDC* pDC)
{
	// TODO: add draw code for native data here
}

void CRssView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	theViewManager.OnActivateView(bActivate, this);
	theViewManager.pOldActiveView2 = theViewManager.pActiveView;
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if(bActivate)
	{
		AfxGetMainWnd()->SendMessage(UM_HIDESHOWBUTTON,HideShowBtn_Homepage);
	}
}

void CRssView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	theViewManager.AddView("RSS资讯", this);
//	GetParent()->SetWindowText(_F("RSS资讯"));
	AfxGetMainWnd()->SetWindowText(g_strTitleName);
}

int CRssView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect			rect;
	GetClientRect( &rect );
	int				nWidth1 = (int)0.25 * rect.Width();
	int				nWidth2 = (int)0.75 * rect.Width();
	int				nHeight1 = (int)0.25 * rect.Height();
	int				nHeight2 = (int)0.75 * rect.Height();
	
	if ( !m_wndSplitter.CreateStatic(this, 1, 2) ) 
	{
		return -1;
	}
	
	if(!m_wndSplitter.CreateView(0, 0,
								RUNTIME_CLASS(CTreeViewEx), 
								CSize(nWidth1, rect.Height() ), 
								NULL))
	{
		TRACE(_T("Failed to create top view\n"));
		return -1;
	}
	
	if (!m_wndSplitterRight.CreateStatic(
		&m_wndSplitter,     // our parent window is the first splitter
		2, 1,               // the new splitter is 2 rows, 1 column
		WS_CHILD | WS_VISIBLE | WS_BORDER,  // style, WS_BORDER is needed
		m_wndSplitter.IdFromRowCol(0, 1)
		// new splitter is in the first row, 2nd column of first splitter
		))
	{
		TRACE0("Failed to create nested splitter\n");
		return -1;
	}
	
	
	// create top view
	if(!m_wndSplitterRight.CreateView(0, 0,
								RUNTIME_CLASS(CAgileReaderView), 
								CSize(nWidth2, nHeight1), 
								NULL))
	{
		TRACE(_T("Failed to create top view\n"));
		return -1;
	}
	
	// create bottom view
	if(!m_wndSplitterRight.CreateView(1, 0,
		RUNTIME_CLASS(CHtmlView), 
		CSize(nWidth2, nHeight2), NULL))
	{
		TRACE(_T("Failed to create bottom view\n"));
		return -1;
	}
	
	m_wndSplitterRight.RecalcLayout();
	m_bCreate = TRUE;
	
	InitTree();
	InitList();
	m_nTimer = SetTimer(1, m_nRate, 0);
	
	return 0;
}

void CRssView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	if ( m_bCreate )
	{
		CRect r;
		GetClientRect(&r);
		
		int w1 = (int)(r.Width()*0.25);
		int w2 = (int)(r.Width()*0.75);

		m_wndSplitter.MoveWindow(&r);
		
		m_wndSplitter.SetColumnInfo( 0, w1, 0 );
		m_wndSplitter.SetColumnInfo( 1, w2, 0 );
		m_wndSplitter.RecalcLayout();		
		
		w1 = (int)(r.Height()*0.25);
		w2 = (int)(r.Height()*0.75);
		m_wndSplitterRight.SetRowInfo( 0, w1, 0 );
		m_wndSplitterRight.SetRowInfo( 1, w2, 0 );
		m_wndSplitterRight.RecalcLayout();		
	}	
}

/////////////////////////////////////////////////////////////////////////////
// Get Left TreeCtrl
//
CTreeCtrl* CRssView::GetTreeCtrl()
{
	CTreeViewEx* pTree = (CTreeViewEx*)m_wndSplitter.GetPane(0,0);	
	return &(pTree->GetTreeCtrl());
}

/////////////////////////////////////////////////////////////////////////////
// Get Right Top ListCtrl
//
CListCtrl* CRssView::GetListCtrl()
{
	CListView* pView = (CListView*)m_wndSplitterRight.GetPane(0,0);
	return &(pView->GetListCtrl());
}

/////////////////////////////////////////////////////////////////////////////
// Get Right Bottom HtmlView
//
CHtmlView* CRssView::GetHtmlView()
{
	CHtmlView* pView = (CHtmlView*)m_wndSplitterRight.GetPane(1,0);
	return pView;
}
/////////////////////////////////////////////////////////////////////////////
// Initialize Tree Ctrl
//
void CRssView::InitTree()
{
	SetWindowLong( GetTreeCtrl()->m_hWnd, GWL_STYLE, GetWindowLong( GetTreeCtrl()->m_hWnd, GWL_STYLE ) | TVS_HASLINES | TVS_HASBUTTONS | TVS_SHOWSELALWAYS );
	GetTreeCtrl()->SetImageList( &m_image, TVSIL_NORMAL );

	CStringArray		strTitleArray;
	CStringArray		strLinkArray;
	CFeed::GetFeedSourceList( strTitleArray, strLinkArray );
	CFeed::DeleteListArray(strLinkArray);

	CString	Ini = g_WSXHStr+g_strYxhjCfg;
	CFileStatus FileStatus;
	if( !CFile::GetStatus(Ini,FileStatus) )
		Ini = HomePath+g_strYxhjCfg;

	int nChannel = 1;
	char ChannelSec[MAX_PATH]={0};
	sprintf(ChannelSec, "Channel%d", nChannel);
	int nItemNum = GetPrivateProfileInt(ChannelSec,"ItemNum",0,Ini);
	while(nItemNum>0)
	{
		char ChannelName[MAX_PATH]={0};
		GetPrivateProfileString(ChannelSec,"ChannelName",ChannelSec,ChannelName,MAX_PATH-1,Ini);
		AddTreeNode( _T(ChannelName), _T("") );
		
		for(int k=0;k<nItemNum;k++)
		{
			char ItemSec[MAX_PATH]={0};
			sprintf(ItemSec, "Item%d", k+1);
			char ItemString[MAX_PATH]={0};
			GetPrivateProfileString(ChannelSec,ItemSec,"",ItemString,MAX_PATH-1,Ini);
			if(ItemString[0])
			{
				CString strRSSFile(ItemString);
				CFeed		feed( strRSSFile );
				feed.m_source.m_strLink = strRSSFile;
				if ( feed.m_item.GetSize() > 0 )
				{
					AddTreeNode( feed.m_source.m_strTitle, feed.m_source.m_strLink );
					feed.Save();
				}
			}
		}

		sprintf(ChannelSec, "Channel%d", ++nChannel);
		nItemNum = GetPrivateProfileInt(ChannelSec,"ItemNum",0,Ini);
	}
	
	/*
	AddTreeNode( _T("股票频道"), _T("") );
	
	CString strRSSFile1("http://rss.cnfol.com/1309.xml");
	CFeed		feed1( strRSSFile1 );
	feed1.m_source.m_strLink = strRSSFile1;
	if ( feed1.m_item.GetSize() > 0 )
	{
		AddTreeNode( feed1.m_source.m_strTitle, feed1.m_source.m_strLink );
		feed1.Save();
	}
	CString strRSSFile2("http://rss.cnfol.com/1455.xml");
	CFeed		feed2( strRSSFile2 );
	feed2.m_source.m_strLink = strRSSFile2;
	if ( feed2.m_item.GetSize() > 0 )
	{
		AddTreeNode( feed2.m_source.m_strTitle, feed2.m_source.m_strLink );
		feed2.Save();
	}
	CString strRSSFile3("http://rss.cnfol.com/1325.xml");
	CFeed		feed3( strRSSFile3 );
	feed3.m_source.m_strLink = strRSSFile3;
	if ( feed3.m_item.GetSize() > 0 )
	{
		AddTreeNode( feed3.m_source.m_strTitle, feed3.m_source.m_strLink );
		feed3.Save();
	}
	*/
}

/////////////////////////////////////////////////////////////////////////////
// Initialize List
//
void CRssView::InitList()
{
//	SkinWindowSet(GetListCtrl()->m_hWnd, 0);
	// Step 1. Set the list style
	SetWindowLong( GetListCtrl()->m_hWnd, GWL_STYLE, WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_SHOWSELALWAYS | LVS_REPORT );
	GetListCtrl()->SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT  );
	GetListCtrl()->SetImageList( &m_image, LVSIL_SMALL );
	
	// Step 2. Set the column header
	GetListCtrl()->InsertColumn( 0, _T("Title"), LVCFMT_LEFT, 400 );
	GetListCtrl()->InsertColumn( 1, _T("Description"), LVCFMT_LEFT, 300 );
	GetListCtrl()->InsertColumn( 2, _T("Link"), LVCFMT_LEFT, 200 );
	GetListCtrl()->InsertColumn( 3, _T("Author"), LVCFMT_LEFT, 200 );
	GetListCtrl()->InsertColumn( 4, _T("Category"), LVCFMT_LEFT, 200 );
	GetListCtrl()->InsertColumn( 5, _T("Publish Date"), LVCFMT_LEFT, 200 );
	GetListCtrl()->InsertColumn( 6, _T("Subject"), LVCFMT_LEFT, 200 );
}

/////////////////////////////////////////////////////////////////////////////
// Get current user selection, then load feed into list
//
LONG CRssView::OnTreeDblClick(UINT wParam, LONG lParam)
{
	CWaitCursor		wait;
	HTREEITEM	hItem = GetTreeCtrl()->GetSelectedItem();
	CNode*		pNode = NULL;
	if ( hItem )
	{
		pNode = (CNode*)GetTreeCtrl()->GetItemData( hItem );
		if ( !pNode->m_strLink.IsEmpty() )
		{
			// Step 4. Open XML Document and Parse it there
			CFeed		feed( pNode->m_strLink );
			feed.m_source.m_strLink = pNode->m_strLink;
			feed.Save(FALSE);
			
			// Step 5. Load all the items into list view
			CFeed		feed1;
			feed1.LoadLocal(pNode->m_strLink);
			
			// Step 5. Load all the items into list view
			DisplayFeed( feed1 );
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// When Tree Item Change
//
LONG CRssView::OnTreeItemChange(UINT wParam, LONG lParam)
{
	HTREEITEM	hItem = GetTreeCtrl()->GetSelectedItem();
	CNode*		pNode = NULL;
	if ( hItem )
	{
		pNode = (CNode*)GetTreeCtrl()->GetItemData( hItem );
		if ( !pNode->m_strLink.IsEmpty() )
		{
			// Step 4. Open XML Document and Parse it there
			CFeed		feed;
			feed.LoadLocal(pNode->m_strLink);
			
			// Step 5. Load all the items into list view
			DisplayFeed( feed );
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Get Current Selection and start this page
//
LONG CRssView::OnListDblClick(UINT wParam, LONG lParam)
{
	POSITION	pos = GetListCtrl()->GetFirstSelectedItemPosition();
	CFeedItem	item;
	
	int			nIndex;
	if ( pos != NULL )
	{
		nIndex = GetListCtrl()->GetNextSelectedItem(pos);
		GetHtmlView()->Navigate( GetListCtrl()->GetItemText(nIndex,2) );
		
		// Get item data
		item.m_strTitle = GetListCtrl()->GetItemText( nIndex, 0 );
		item.m_strDescription = GetListCtrl()->GetItemText( nIndex, 1 );
		item.m_strLink = GetListCtrl()->GetItemText( nIndex, 2 );
		item.m_strAuthor = GetListCtrl()->GetItemText( nIndex, 3 );
		item.m_strCategory = GetListCtrl()->GetItemText( nIndex, 4 );
		item.m_strPubDate = GetListCtrl()->GetItemText( nIndex, 5 );
		item.m_strSubject = GetListCtrl()->GetItemText( nIndex, 6 );
		CFeed::MarkItemRead( item.m_strLink );
		
		GetListCtrl()->LockWindowUpdate();
		GetListCtrl()->InsertItem( nIndex+1, item.m_strTitle, 2 );
		GetListCtrl()->SetItemText( nIndex+1, 1, item.m_strDescription );
		GetListCtrl()->SetItemText( nIndex+1, 2, item.m_strLink );
		GetListCtrl()->SetItemText( nIndex+1, 3, item.m_strAuthor );
		GetListCtrl()->SetItemText( nIndex+1, 4, item.m_strCategory );
		GetListCtrl()->SetItemText( nIndex+1, 5, item.m_strPubDate );
		GetListCtrl()->SetItemText( nIndex+1, 6, item.m_strSubject );	
		GetListCtrl()->DeleteItem( nIndex );
		
		GetListCtrl()->SetItemState( nIndex, LVIS_SELECTED, LVIS_SELECTED );
		GetListCtrl()->UnlockWindowUpdate();
		GetListCtrl()->UpdateWindow();
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
// Add Tree Node
//
void CRssView::AddTreeNode(CString strTitle, CString strLink)
{
	CNode*		pNode = NULL;
	HTREEITEM	hItem;
	if ( strLink.IsEmpty() )
	{
		hItem = GetTreeCtrl()->InsertItem( strTitle, 0, 0 );
	}
	else
	{
		hItem = GetTreeCtrl()->InsertItem( strTitle, 3, 3, GetTreeCtrl()->GetRootItem() );
	}
	pNode = new CNode( strLink );
	GetTreeCtrl()->SetItemData( hItem, (DWORD)pNode );
	
	GetTreeCtrl()->Expand( GetTreeCtrl()->GetRootItem(), TVE_EXPAND );
}
/////////////////////////////////////////////////////////////////////////////
// Release Data Attached to each node
//
void CRssView::Release( HTREEITEM hParent )
{
	CNode*		pNode = NULL;
	HTREEITEM	hItem = NULL;
	
	// Step 1. Release current node
	if ( hParent != NULL )
	{
		pNode = (CNode*)GetTreeCtrl()->GetItemData( hParent );
		if ( pNode )
		{
			delete pNode;
			pNode = NULL;
		}
	}
	
	// Step 2. Release child node
	hItem = GetTreeCtrl()->GetChildItem( hParent );
	while( hItem )
	{
		Release( hItem );
		hItem = GetTreeCtrl()->GetNextSiblingItem( hItem );
	}
}

/////////////////////////////////////////////////////////////////////////////
// Display Feed Information into List
void CRssView::DisplayFeed(CFeed& feed)
{
	int			nIndex;
	
	GetListCtrl()->DeleteAllItems();
	for( nIndex = 0; nIndex < feed.m_item.GetSize(); nIndex++ )
	{
		if ( feed.m_item[nIndex].m_strReadStatus == _T("1") )
		{
			GetListCtrl()->InsertItem( nIndex, feed.m_item[nIndex].m_strTitle, 2 );
		}
		else
		{
			GetListCtrl()->InsertItem( nIndex, feed.m_item[nIndex].m_strTitle, 1 );
		}
		GetListCtrl()->SetItemText( nIndex, 1, feed.m_item[nIndex].m_strDescription );
		GetListCtrl()->SetItemText( nIndex, 2, feed.m_item[nIndex].m_strLink );
		GetListCtrl()->SetItemText( nIndex, 3, feed.m_item[nIndex].m_strAuthor );
		GetListCtrl()->SetItemText( nIndex, 4, feed.m_item[nIndex].m_strCategory );
		GetListCtrl()->SetItemText( nIndex, 5, NormalizeDate( feed.m_item[nIndex].m_strPubDate ) );
		GetListCtrl()->SetItemText( nIndex, 6, feed.m_item[nIndex].m_strSubject );
	}
	GetListCtrl()->SendMessage(UM_RSSDATETIMESORT, 0, 0);
}

void CRssView::OnDestroy() 
{
	Release( GetTreeCtrl()->GetRootItem() );	
	KillTimer(m_nTimer);

	CStringArray		strTitleArray;
	CStringArray		strLinkArray;
	CFeed::GetFeedSourceList( strTitleArray, strLinkArray );
	CFeed::DeleteListArray(strLinkArray);

	CView::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
// Normalize a GMT Date Time to Date
//
CString CRssView::NormalizeDate(CString strValue)
{
	int		nPos;
	strValue.MakeUpper();
	strValue.Replace( _T("GMT"),  _T("") );
	strValue.Replace( _T("PST"),  _T("") );
	strValue.Replace( _T("EST"),  _T("") );
	strValue.Replace( _T("MON,"), _T("") );
	strValue.Replace( _T("TUE,"), _T("") );
	strValue.Replace( _T("WED,"), _T("") );
	strValue.Replace( _T("THU,"), _T("") );
	strValue.Replace( _T("FRI,"), _T("") );
	strValue.Replace( _T("SAT,"), _T("") );
	strValue.Replace( _T("SUN,"), _T("") );
	strValue.TrimLeft();
	strValue.TrimRight();
	nPos = strValue.ReverseFind( '-' );
	if ( strValue.GetLength() - nPos == 5 )
	{
		strValue = strValue.Left( nPos-1 );
		strValue.TrimRight();
	}
	return strValue;
}
/////////////////////////////////////////////////////////////////////////////
// Refresh Subscription Thread Function
//
UINT CRssView::ProcRefreshAll( void* pParam )
{
	CRssView*	pMain = (CRssView*)pParam;
	
	CoInitialize( NULL );
	
	// Real Refresh function is a static function
	CFeed::RefreshAll();
	
	CoUninitialize();
	
	// Set status back to ready
	pMain->m_bRunning = FALSE;
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// When it is timer, then start the thread to refresh
//
void CRssView::OnTimer(UINT nIDEvent) 
{
	// Step 1. Check whether there is any refreshing thread running
	if ( m_bRunning )
	{
		// There is a thread running, skip this
		return;
	}
	
	// Step 2. If there is no thread running, then start a thread to refresh it
	m_bRunning = TRUE;
	m_pThread = AfxBeginThread( CRssView::ProcRefreshAll, this );
	
	CView::OnTimer(nIDEvent);
}
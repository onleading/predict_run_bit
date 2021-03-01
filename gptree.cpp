// gptree.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "gptree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGpTree

IMPLEMENT_DYNCREATE(CGpTree, CWnd)

CGpTree::CGpTree()
{
	m_TreeCtrl = NULL;
	m_pNewFont = NULL;

	pVScrollBar = NULL;
}

CGpTree::~CGpTree()
{
	TDEL(pVScrollBar);
}


BEGIN_MESSAGE_MAP(CGpTree, CWnd)
	ON_MESSAGE(UM_VSCROLLBAR,OnVScrollMessage)
	//{{AFX_MSG_MAP(CGpTree)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_GPTREE, OnSelchangedTree)
	ON_WM_DESTROY()
	ON_WM_NCDESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGpTree message handlers

BOOL CGpTree::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CGpTree::SetAppearance(COLORREF bColor,COLORREF fColor) 
{
	m_TreeCtrl->SetBkColor (bColor);
	if(pVScrollBar)
	{
#ifdef OEM_NEWJY
		COLORREF tmpColor[10];
		for(int i=0;i<10;i++)
			tmpColor[i]=g_GUI.GetColorCfgFromResource(g_resModule,"IDR_COLOR","Tree_MyScrollBar",i);
		pVScrollBar->SetScrollColor(tmpColor);
#else
		pVScrollBar->SetScrollColor(RGB(128,128,128),1,bColor,RGB(128,128,128));
#endif
	}
	m_TreeCtrl->SetTextColor (fColor);
}

void CGpTree::FillTree()
{
	HTREEITEM hRoot = TVI_ROOT,hChild;
	m_TreeCtrl->SetRedraw(FALSE);
	m_TreeCtrl->DeleteAllItems();

	for(int i = 0;i < TOTAL_TYPENUM;i++)
	{
		hChild = m_TreeCtrl->InsertItem(_F(GPType[i]),1,1,hRoot,TVI_LAST);
		m_TreeCtrl->SetItemData(hChild,i);
	}
	hRoot = m_TreeCtrl->InsertItem(_F("地区分类板块"),0,0,TVI_ROOT,TVI_LAST);
	for(i = 0;i < TOTAL_DY_NUM;i++)
	{
		hChild = m_TreeCtrl->InsertItem(_F(g_cwAddrTopic[i]),1,1,hRoot,TVI_LAST);
		m_TreeCtrl->SetItemData(hChild,2*65536+i);
	}
	hRoot = m_TreeCtrl->InsertItem(_F("行业分类板块"),0,0,TVI_ROOT,TVI_LAST);
	for(i = 0;i < TOTAL_HY_NUM;i++)
	{
		hChild = m_TreeCtrl->InsertItem(_F(g_cwHYstr[i]),1,1,hRoot,TVI_LAST);
		m_TreeCtrl->SetItemData(hChild,3*65536+i);
	}
	hRoot = m_TreeCtrl->InsertItem(_F("概念分类板块"),0,0,TVI_ROOT,TVI_LAST);
	for(i = 0;i < g_ConceptBlockNum;i++)
	{
		hChild = m_TreeCtrl->InsertItem(_F(g_ConceptBlock[i].strBlockName),1,1,hRoot,TVI_LAST);
		m_TreeCtrl->SetItemData(hChild,4*65536+i);
	}
	hRoot = m_TreeCtrl->InsertItem(_F("组合分类板块"),0,0,TVI_ROOT,TVI_LAST);
	for(i = 0;i < g_ZHBBlockNum;i++)
	{
		hChild = m_TreeCtrl->InsertItem(_F(g_ZHBBlock[i].zhmc),1,1,hRoot,TVI_LAST);
		m_TreeCtrl->SetItemData(hChild,5*65536+i);
	}
	hRoot = m_TreeCtrl->InsertItem(_F("用户定义板块"),0,0,TVI_ROOT,TVI_LAST);
	for(i = 0;i < g_nBlockNum;i++)	//将块板配置加入到树中
	{
		hChild = m_TreeCtrl->InsertItem(g_BlockInfo[i].Name,1,1,hRoot,TVI_LAST);
		m_TreeCtrl->SetItemData(hChild,65536+i);
	}
	if(g_nBlockNum <= 0)	//如果一个板块股都没有，则加10000作为其附值
		m_TreeCtrl->SetItemData(hRoot,10000);
	m_TreeCtrl->SetRedraw(TRUE);
}

int CGpTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_TreeCtrl = new CMyTreeCtrl(FALSE);
	m_TreeCtrl->Create(WS_VISIBLE|WS_CHILD|TVS_SHOWSELALWAYS,CRect(0,0,0,0),this,IDC_GPTREE);	
	pVScrollBar=new CMyScrollBar;
	if(!pVScrollBar || !pVScrollBar->Create(_T("MyScrollBar"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_GPTREE_VSCROLL))
		return -1;
	pVScrollBar->Init(X_VERTICAL,m_hWnd,UM_VSCROLLBAR);

	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,5,0);
	long IconList3[]={IDI_BLOCK2,IDI_BLOCK};
	int IconLength3=sizeof(IconList3)/sizeof(long);
	HICON hIcon3;
	for(int i=0;i<IconLength3;i++)
	{
		hIcon3=::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IconList3[i]));
		m_ImageList.Add(hIcon3);
	}
	m_TreeCtrl->SetImageList (&m_ImageList, TVSIL_NORMAL);

	m_pNewFont=new CFont;
	m_pNewFont->CreateFont(14,0,0,0,FW_MEDIUM,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,SONG_FONT);
	m_TreeCtrl->SetFont(m_pNewFont);
	m_TreeCtrl->SetItemHeight(m_TreeCtrl->GetItemHeight()+4);

	SetAppearance(TreeColor.TreeBackColor,TreeColor.TreeForeColor);

	FillTree();
	m_TreeCtrl->Expand (m_TreeCtrl->GetRootItem( ),TVE_EXPAND);
	
	return 0;
}

void CGpTree::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	if(m_TreeCtrl) 
	{
		m_TreeCtrl->MoveWindow(0,0,cx-15,cy);
		if(pVScrollBar)
		{
			pVScrollBar->MoveWindow(cx-15,0,15,cy);
		}
	}
}

void CGpTree::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM  SelectItem = (pNMTreeView->itemNew).hItem;
	if( SelectItem != NULL && !m_TreeCtrl->ItemHasChildren(SelectItem) )
	{
		//取得树叶节点中的数据
		DWORD ItemData = m_TreeCtrl->GetItemData(SelectItem);
		if(LayoutFlag)
			AfxGetMainWnd()->SendMessage(UM_TREEVIEW_MESSAGE,GP_TREETYPE,ItemData);	//0为功能 1为股票 2为指标
	}	
	*pResult = 0;
}

void CGpTree::SetScrollPos() 
{
	if(!::IsWindow(m_TreeCtrl->m_hWnd)) return;
	if(pVScrollBar)
	{
		SCROLLINFO tmp;
		tmp.cbSize=sizeof(tmp);
		BOOL tmpb=m_TreeCtrl->GetScrollInfo(SB_VERT,&tmp,SIF_ALL);
		if(tmpb==FALSE || tmp.nMax==0) pVScrollBar->SetScrollRange(0,0,FALSE);
		else pVScrollBar->SetScrollRange(tmp.nMin,tmp.nMax-tmp.nPage+1,FALSE);
		pVScrollBar->SetScrollPos(tmp.nPos);
	}
}

void CGpTree::OnDestroy() 
{
	CWnd::OnDestroy();
	
	TDEL(m_TreeCtrl);
	TDEL(m_pNewFont);	
}

void CGpTree::OnNcDestroy() 
{
	CWnd::OnNcDestroy();
	
	delete this;	
}

void CGpTree::OnVScrollMessage(WPARAM wParam,LPARAM lParam)
{
	int nPos = (int)wParam;

	HTREEITEM hWantItem = CMyTreeCtrl::GetTreeItem(m_TreeCtrl,nPos);
	if(hWantItem)
	{
		m_TreeCtrl->Select(hWantItem,TVGN_FIRSTVISIBLE);
		m_TreeCtrl->SetRedraw(TRUE);
	}
}

LRESULT CGpTree::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==UM_RESETSCROLLBAR)
	{
		SetScrollPos();
		return 0;
	}
	
	return CWnd::WindowProc(message, wParam, lParam);
}

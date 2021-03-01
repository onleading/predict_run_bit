// zbtree.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "zbtree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZbTree

IMPLEMENT_DYNCREATE(CZbTree, CWnd) //动态创建的类，在NCDESTROY中必须delete this

CZbTree::CZbTree()
{
	m_TreeCtrl = NULL;
	m_pNewFont = NULL;

	pVScrollBar = pHScrollBar = NULL;
}

CZbTree::~CZbTree()
{
	TDEL(pVScrollBar);
	TDEL(pHScrollBar);
}


BEGIN_MESSAGE_MAP(CZbTree, CWnd)
	ON_MESSAGE(UM_VSCROLLBAR,OnVScrollMessage)
	ON_MESSAGE(UM_HSCROLLBAR,OnHScrollMessage)
	//{{AFX_MSG_MAP(CZbTree)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_ZBTREE, OnSelchangedTree)
	ON_WM_DESTROY()
	ON_WM_NCDESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CZbTree message handlers

BOOL CZbTree::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CZbTree::SetAppearance(COLORREF bColor,COLORREF fColor) 
{
	m_TreeCtrl->SetBkColor (bColor);
	if(pVScrollBar)
		pVScrollBar->SetScrollColor(RGB(128,128,128),1,bColor,RGB(128,128,128));
	if(pHScrollBar)
		pHScrollBar->SetScrollColor(RGB(128,128,128),1,bColor,RGB(128,128,128));
	m_TreeCtrl->SetTextColor (fColor);
}

void CZbTree::InsertZBType(HTREEITEM hFatherItem, long nTypeNo)
{
	TYPEX te={0};
	if(!g_pCalc->_CallDTopViewCalc_12(&te, ZB_TYPE, nTypeNo)) return;
	if(!te.bEnable) return;
	
	int i=0, j=0;
	HTREEITEM hMyItem = TVI_ROOT, hChild = TVI_ROOT;
	char tempItemStr[125]={0};
	TINDEXINFO *tmpIndexInfo=NULL;
	int  nTotalType = g_pCalc->_CallDTopViewCalc_10(ZB_TYPE);
	int nTotalNum = g_pCalc->_CallDTopViewCalc_7(ZB_TYPE);

	hMyItem = m_TreeCtrl->InsertItem(_F(te.lpszName),0,0,hFatherItem,TVI_LAST);
	m_TreeCtrl->SetItemData(hMyItem,65535);
	for(i = 0;i < nTotalType;i++)
	{
		if(nTypeNo==i) continue;
		TYPEX te1={0};
		if(!g_pCalc->_CallDTopViewCalc_12(&te1, ZB_TYPE, i)) continue;
		if(te1.nFatherType==te.nTypeID)		//找到子类型
			InsertZBType(hMyItem, i);
	}
	for(j = 0;j < nTotalNum;j++)
	{
		tmpIndexInfo = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE,j);
		if(tmpIndexInfo && tmpIndexInfo->nType == te.nTypeID )
		{
			sprintf(tempItemStr,"%s %s",tmpIndexInfo->acCode,tmpIndexInfo->acName);
			hChild = m_TreeCtrl->InsertItem(_F(tempItemStr),1,1,hMyItem,TVI_LAST);
			m_TreeCtrl->SetItemData(hChild,j);
			continue;
		}
	}
}

void CZbTree::FillTree()
{
	m_TreeCtrl->SetRedraw(FALSE);
	m_TreeCtrl->DeleteAllItems(); //首先清空原来的树内容

	int i,j,nTotalNum,nTotalType;
	HTREEITEM hRoot = TVI_ROOT,hChild = TVI_ROOT;
	char tempItemStr[125];
	TINDEXINFO *tmpIndexInfo=NULL;

	nTotalType = g_pCalc->_CallDTopViewCalc_10(ZB_TYPE);
	nTotalNum = g_pCalc->_CallDTopViewCalc_7(ZB_TYPE);
	for(i = 0;i < nTotalType;i++)
	{
		TYPEX te={0};
		if(!g_pCalc->_CallDTopViewCalc_12(&te, ZB_TYPE, i)) continue;
		if(te.nFatherType!=-1) continue;
		InsertZBType(TVI_ROOT, i);
	}
	hRoot = m_TreeCtrl->InsertItem(_F("特殊画线指标"),0,0,TVI_ROOT,TVI_LAST);
	m_TreeCtrl->SetItemData(hRoot,65535);
	for(j=0;j<g_nSpeclIndexNum;j++)
	{
		tmpIndexInfo = g_SpeclIndex+j;
		sprintf(tempItemStr,"%s %s",tmpIndexInfo->acCode,tmpIndexInfo->acName);
		hChild = m_TreeCtrl->InsertItem(_F(tempItemStr),1,1,hRoot,TVI_LAST);
		m_TreeCtrl->SetItemData(hChild,4*65536+j);
	}
	nTotalNum = g_pCalc->_CallDTopViewCalc_7(EXP_TYPE);
	hRoot = m_TreeCtrl->InsertItem(_F("交易系统指示"),0,0,TVI_ROOT,TVI_LAST);
	m_TreeCtrl->SetItemData(hRoot,65535);
	for(j = 0;j < nTotalNum;j++)
	{
		tmpIndexInfo = g_pCalc->_CallDTopViewCalc_8(EXP_TYPE,j);
		sprintf(tempItemStr,"%s %s",tmpIndexInfo->acCode,tmpIndexInfo->acName);
		hChild = m_TreeCtrl->InsertItem(_F(tempItemStr),1,1,hRoot,TVI_LAST);
		m_TreeCtrl->SetItemData(hChild,65536+j);
	}
	nTotalNum = g_pCalc->_CallDTopViewCalc_7(KL_TYPE);
	hRoot = m_TreeCtrl->InsertItem(_F("五彩K线指示"),0,0,TVI_ROOT,TVI_LAST);
	m_TreeCtrl->SetItemData(hRoot,65535);
	for(j = 0;j < nTotalNum;j++)
	{
		tmpIndexInfo = g_pCalc->_CallDTopViewCalc_8(KL_TYPE,j);
		sprintf(tempItemStr,"%s %s",tmpIndexInfo->acCode,tmpIndexInfo->acName);
		hChild = m_TreeCtrl->InsertItem(_F(tempItemStr),1,1,hRoot,TVI_LAST);
		m_TreeCtrl->SetItemData(hChild,2*65536+j);
	}
	hRoot = m_TreeCtrl->InsertItem(_F("删除指示"),1,1,TVI_ROOT,TVI_LAST);
	m_TreeCtrl->SetItemData(hRoot,3*65536+0);
	m_TreeCtrl->SetRedraw(TRUE);
}

void ExpandAllTreeNodes(CTreeCtrl *pTree, HTREEITEM hRoot, BOOL bExpand)
{
	UINT nCode = bExpand?TVE_EXPAND:TVE_COLLAPSE;
	
	if (NULL == hRoot)
	{
		// 可能有多个根节点
		hRoot = pTree->GetRootItem();
		while(NULL != hRoot)
		{
			ExpandAllTreeNodes(pTree, hRoot, bExpand);
			hRoot = pTree->GetNextItem(hRoot, TVGN_NEXT);
		}
		return;
	}
	
	HTREEITEM hSon = NULL;
	pTree->Expand(hRoot,nCode);  
	
	// 遍历子节点
	hSon = pTree->GetNextItem(hRoot,TVGN_CHILD);
	while(NULL != hSon)
	{
		//   pTree->Expand(hSon, nCode);
		TRACE("cur:%s,Expand son:%s\n", pTree->GetItemText(hRoot), pTree->GetItemText(hSon));
		ExpandAllTreeNodes(pTree, hSon, bExpand);   
		hSon = pTree->GetNextItem(hSon,TVGN_NEXT);
	}
}

int CZbTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_TreeCtrl = new CMyTreeCtrl(FALSE);
	m_TreeCtrl->Create(WS_VISIBLE|WS_CHILD|TVS_SHOWSELALWAYS,CRect(0,0,0,0),this,IDC_ZBTREE);
	
	pVScrollBar=new CMyScrollBar;
	if(!pVScrollBar || !pVScrollBar->Create(_T("MyScrollBar"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_ZBTREE_VSCROLL))
		return -1;
	pVScrollBar->Init(X_VERTICAL,m_hWnd,UM_VSCROLLBAR);
	pHScrollBar=new CMyScrollBar;
	if(!pHScrollBar || !pHScrollBar->Create(_T("MyScrollBar"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_ZXTREE_HSCROLL))
		return -1;
	pHScrollBar->Init(X_HORIZON,m_hWnd,UM_HSCROLLBAR);

	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,5,0);
	long IconList3[]={IDI_ZB1,IDI_ZB2};
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
//	ExpandAllTreeNodes(m_TreeCtrl, NULL, TRUE);
	m_TreeCtrl->Expand(m_TreeCtrl->GetRootItem( ),TVE_EXPAND);
	
	return 0;
}

void CZbTree::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	if(m_TreeCtrl) 
	{
		m_TreeCtrl->MoveWindow(0,0,cx-15,cy-15);
		if(pVScrollBar)
			pVScrollBar->MoveWindow(cx-15,0,15,cy);
		if(pHScrollBar)
			pHScrollBar->MoveWindow(0, cy-15, cx-15, 15);
	}
}

void CZbTree::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM  SelectItem = (pNMTreeView->itemNew).hItem;
	if( SelectItem != NULL && !m_TreeCtrl->ItemHasChildren(SelectItem) )
	{
		//取得树叶节点中的数据
		DWORD ItemData = m_TreeCtrl->GetItemData(SelectItem);
		if(LayoutFlag)
			AfxGetMainWnd()->SendMessage(UM_TREEVIEW_MESSAGE,ZB_TREETYPE,ItemData);	//0为功能 1为股票 2为指标
	}	
	*pResult = 0;
}

void CZbTree::SetScrollPos() 
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
	if(pHScrollBar)
	{
		SCROLLINFO tmp;
		tmp.cbSize=sizeof(tmp);
		BOOL tmpb=m_TreeCtrl->GetScrollInfo(SB_HORZ,&tmp,SIF_ALL);
		if(tmpb==FALSE || tmp.nMax==0) pHScrollBar->SetScrollRange(0,0,FALSE);
		else pHScrollBar->SetScrollRange(tmp.nMin,tmp.nMax-tmp.nPage+1,FALSE);
		pHScrollBar->SetScrollPos(tmp.nPos);
	}
}

void CZbTree::OnDestroy() 
{
	CWnd::OnDestroy();
	
	TDEL(m_TreeCtrl);
	TDEL(m_pNewFont);	
}

void CZbTree::OnNcDestroy() 
{
	CWnd::OnNcDestroy();
	
	delete this;	
}

void CZbTree::OnVScrollMessage(WPARAM wParam,LPARAM lParam)
{
	int nPos = (int)wParam;

	HTREEITEM hWantItem = CMyTreeCtrl::GetTreeItem(m_TreeCtrl,nPos);
	if(hWantItem)
	{
		m_TreeCtrl->Select(hWantItem,TVGN_FIRSTVISIBLE);
		m_TreeCtrl->SetRedraw(TRUE);
	}
}

void CZbTree::OnHScrollMessage(WPARAM wParam,LPARAM lParam)
{
	m_TreeCtrl->SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, wParam), NULL);
}

LRESULT CZbTree::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==UM_RESETSCROLLBAR)
	{
		SetScrollPos();
		return 0;
	}
	
	return CWnd::WindowProc(message, wParam, lParam);
}

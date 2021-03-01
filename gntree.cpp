// gntree.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "gntree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGnTree

IMPLEMENT_DYNCREATE(CGnTree, CWnd)

CGnTree::CGnTree()
{
	m_TreeCtrl = NULL;
	m_pNewFont = NULL;

	pVScrollBar = NULL;
}

CGnTree::~CGnTree()
{
	TDEL(pVScrollBar);
}


BEGIN_MESSAGE_MAP(CGnTree, CWnd)
	ON_MESSAGE(UM_VSCROLLBAR,OnVScrollMessage)
	//{{AFX_MSG_MAP(CGnTree)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_GNTREE, OnSelchangedTree)
	ON_NOTIFY(NM_CLICK, IDC_GNTREE, OnClickTree)
	ON_WM_DESTROY()
	ON_WM_NCDESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGnTree message handlers

BOOL CGnTree::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CGnTree::SetAppearance(COLORREF bColor,COLORREF fColor) 
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

void CGnTree::FillTree()
{
	HTREEITEM hRoot=TVI_ROOT,hChild=TVI_ROOT;
	int nCode = 0,i,j,k;
	CString tmpStr;
	TINDEXINFO *pTmpInfo;
	//向第一棵中加数据,从数组中读取
	m_TreeCtrl->SetRedraw(FALSE);
	m_TreeCtrl->SelectItem(NULL);
	m_TreeCtrl->DeleteAllItems();
	if(!g_vDynaCmdTab.empty())
	{
		hRoot = m_TreeCtrl->InsertItem(_F("定制版面"),0,0,TVI_ROOT,TVI_LAST);
		for(i=0;i<g_vDynaCmdTab.size();i++)
		{
			if(strcmp(g_DefCusPadName, g_vDynaCmdTab[i].ChName)==0)
				tmpStr.Format("%s %s(初始)", g_vDynaCmdTab[i].CmdInfo.Code, g_vDynaCmdTab[i].ChName);
			else
				tmpStr.Format("%s %s", g_vDynaCmdTab[i].CmdInfo.Code, g_vDynaCmdTab[i].ChName);
			hChild = m_TreeCtrl->InsertItem(_F(tmpStr),1,1,hRoot,TVI_LAST);
			m_TreeCtrl->SetItemData(hChild,2000+i);
		}
	}
	for(i = 0,j = 0;;i++)
	{
		if(strlen(FuncLevel1[i].Text)==0) break;
		if(FuncLevel1[i].SonNum == 0)
		{
			hRoot = m_TreeCtrl->InsertItem(_F(FuncLevel1[i].Text),1,1,TVI_ROOT,TVI_LAST);
			m_TreeCtrl->SetItemData(hRoot,FuncMessageCode[nCode][0]*65536+FuncMessageCode[nCode][1]);
			nCode++;
		}
		else
			hRoot = m_TreeCtrl->InsertItem(_F(FuncLevel1[i].Text),0,0,TVI_ROOT,TVI_LAST);
		k = 0;
		while(k < FuncLevel1[i].SonNum)
		{
			if(i==2) //更换树中的常见指标
			{
				if(k>=g_nOftenUseZb)
				{
					nCode+=5-g_nOftenUseZb;
					k+=5-g_nOftenUseZb;
					break;
				}
				pTmpInfo = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE,g_strOftenUseZb[k]);
				if(pTmpInfo)
				{
					tmpStr.Format(".3%02d %s%s",k+1,pTmpInfo->acCode,pTmpInfo->acName);
					hChild = m_TreeCtrl->InsertItem(_F(tmpStr),1,1,hRoot,TVI_LAST);
				}
				else
					hChild = m_TreeCtrl->InsertItem(_F("未知指标"),1,1,hRoot,TVI_LAST);
			}
			else
				hChild = m_TreeCtrl->InsertItem(_F(FuncLevel2[j+k]),1,1,hRoot,TVI_LAST);
			m_TreeCtrl->SetItemData(hChild,FuncMessageCode[nCode][0]*65536+FuncMessageCode[nCode][1]);
			nCode++;
			k++;
		}
		j+=k;
	}
	m_TreeCtrl->SetRedraw(TRUE);
}

int CGnTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_TreeCtrl = new CMyTreeCtrl(FALSE);
	m_TreeCtrl->Create(WS_VISIBLE|WS_CHILD|TVS_SHOWSELALWAYS,CRect(0,0,0,0),this,IDC_GNTREE);	
	pVScrollBar=new CMyScrollBar;
	if(!pVScrollBar || !pVScrollBar->Create(_T("MyScrollBar"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_GNTREE_VSCROLL))
		return -1;
	pVScrollBar->Init(X_VERTICAL,m_hWnd,UM_VSCROLLBAR);

	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,5,0);
	long IconList3[]={IDI_ICON1,IDI_ICON2};
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
	
	return 0;
}

void CGnTree::OnSize(UINT nType, int cx, int cy) 
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

void CGnTree::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM  SelectItem = (pNMTreeView->itemNew).hItem;
	if( SelectItem != NULL && !m_TreeCtrl->ItemHasChildren(SelectItem) )
	{
		//取得树叶节点中的数据
		DWORD ItemData = m_TreeCtrl->GetItemData(SelectItem);
		if(LayoutFlag)
			AfxGetMainWnd()->SendMessage(UM_TREEVIEW_MESSAGE,GN_TREETYPE,ItemData);
	}	
	*pResult = 0;
}

void CGnTree::SetScrollPos() 
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

void CGnTree::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult) 
{	
	POINT point;
	GetCursorPos(&point);
	m_TreeCtrl->ScreenToClient(&point);
	UINT temp;
	HTREEITEM SelectItem = NULL;
	HTREEITEM  hItem = m_TreeCtrl->HitTest(point,&temp);
	if(hItem != NULL && (temp & TVHT_ONITEM)) //一定要点在ITEM上
	{
		m_TreeCtrl->SelectItem(SelectItem);
		if( SelectItem != NULL && !m_TreeCtrl->ItemHasChildren(SelectItem) )
		{
			//取得树叶节点中的数据
			DWORD ItemData = m_TreeCtrl->GetItemData(SelectItem);
			if(LayoutFlag)
				AfxGetMainWnd()->SendMessage(UM_TREEVIEW_MESSAGE,GN_TREETYPE,ItemData);
		}	
	}
	*pResult = 0;
}

void CGnTree::OnDestroy() 
{
	CWnd::OnDestroy();

	TDEL(m_TreeCtrl);
	TDEL(m_pNewFont);	
}

void CGnTree::OnNcDestroy() 
{
	CWnd::OnNcDestroy();
	
	delete this;	
}

void CGnTree::OnVScrollMessage(WPARAM wParam,LPARAM lParam)
{
	int nPos = (int)wParam;

	HTREEITEM hWantItem = CMyTreeCtrl::GetTreeItem(m_TreeCtrl,nPos);
	if(hWantItem)
		m_TreeCtrl->Select(hWantItem,TVGN_FIRSTVISIBLE);
}

LRESULT CGnTree::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==UM_RESETSCROLLBAR)
	{
		SetScrollPos();
		return 0;
	}
	return CWnd::WindowProc(message, wParam, lParam);
}

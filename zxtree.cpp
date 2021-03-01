// zxtree.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "zxtree.h"
#include "RecentSearchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_ZX_URL_SEP		100000000L
/////////////////////////////////////////////////////////////////////////////
// CZxTree

IMPLEMENT_DYNCREATE(CZxTree, CWnd)

CZxTree::CZxTree()
{
	m_TreeCtrl = NULL;
	m_pNewFont = NULL;

	pVScrollBar = NULL;
}

CZxTree::~CZxTree()
{
	TDEL(pVScrollBar);
}


BEGIN_MESSAGE_MAP(CZxTree, CWnd)
	ON_MESSAGE(UM_VSCROLLBAR,OnVScrollMessage)
	ON_MESSAGE(UM_FILLZXTREE_MESSAGE,OnFillZxTreeMsg)
	//{{AFX_MSG_MAP(CZxTree)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_ZXTREE_CTRL, OnSelchangedTree)
	ON_NOTIFY(NM_CLICK, IDC_ZXTREE_CTRL, OnClickTree)
	ON_WM_DESTROY()
	ON_WM_NCDESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CZxTree message handlers

BOOL CZxTree::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CZxTree::SetAppearance(COLORREF bColor,COLORREF fColor) 
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

void CZxTree::FillZxTool()
{
	HTREEITEM hRoot=TVI_ROOT,hChild=TVI_ROOT;
	//几个特殊的ID 960-969:公告消息 970:热点资讯排名 971:资讯检索 972:发布资讯 973:资讯收藏夹 974:资讯设置 989-999:历史资讯检索
	hRoot = m_TreeCtrl->InsertItem(_F("资讯工具"),0,0,TVI_ROOT,TVI_LAST);	
	hChild = m_TreeCtrl->InsertItem(_F("资讯收藏夹"),1,1,hRoot,TVI_LAST);	
	m_TreeCtrl->SetItemData(hChild,973);
	
	hChild = m_TreeCtrl->InsertItem(_F("资讯设置"),1,1,hRoot,TVI_LAST);	
	m_TreeCtrl->SetItemData(hChild,974);
	hChild = m_TreeCtrl->InsertItem(_F("资讯检索"),1,1,hRoot,TVI_LAST);	
	m_TreeCtrl->SetItemData(hChild,971);
	hChild = m_TreeCtrl->InsertItem(_F("最近检索设置"),1,1,hRoot,TVI_LAST);	
	m_TreeCtrl->SetItemData(hChild,989);
}

void CZxTree::FillGgxx()
{
	HTREEITEM hRoot=TVI_ROOT,hChild=TVI_ROOT;
	hRoot = m_TreeCtrl->InsertItem(_F("公告消息"),0,0,TVI_ROOT,TVI_LAST);		
	CString tempStr;
	for(int i = 0;i < XXMNum;i++)
	{
		tempStr = XXMTopic[i].Left(18);
		hChild = m_TreeCtrl->InsertItem(_F(tempStr),1,1,hRoot,TVI_LAST);		
		m_TreeCtrl->SetItemData(hChild,960+i);
	}
	hChild = m_TreeCtrl->InsertItem(_F("信息港资讯"),1,1,hRoot,TVI_LAST);		
	m_TreeCtrl->SetItemData(hChild,960+i);
}

void CZxTree::FillOneZX(int i)
{
	//开始构造资讯树,目前仅支持三层
	HTREEITEM hRoot=TVI_ROOT,hChild=TVI_ROOT,hChildChild=TVI_ROOT,hChildChildChild=TVI_ROOT;
	int j,k,kk;//i:资讯公司 j:资讯公司大分类 k:资讯公司的树枝
	BOOL bHasChild,bHasChildChild;
	DWORD tmpItemData;
	INFOTREEID tmpInfoTreeID;

	GetFromInfoTree_X(g_TdxInfoINI.companyid[i],tmpInfoTreeID,g_MngRight);
	if(tmpInfoTreeID.infotree_num > 0)
	{
		if(strlen(g_TdxInfoINI.companystr[i])<7)
			hRoot = m_TreeCtrl->InsertItem(_F(g_TdxInfoINI.companystr[i]+(CString)"资讯"),0,0,TVI_ROOT,TVI_LAST);
		else
			hRoot = m_TreeCtrl->InsertItem(_F(g_TdxInfoINI.companystr[i]),0,0,TVI_ROOT,TVI_LAST);
		for(j=0;j < tmpInfoTreeID.infotree_num;j++)
		{
			if(tmpInfoTreeID.infotreeid[j] < 100)
			{
				hChild = m_TreeCtrl->InsertItem(_F(tmpInfoTreeID.infotreestr[j]),0,0,hRoot,TVI_LAST);
				if(strlen(tmpInfoTreeID.infotreeurl[j])>0)
				{
					tmpItemData=MAX_ZX_URL_SEP+j*100+i;
					m_URLMap[tmpItemData]=tmpInfoTreeID.infotreeurl[j];
					m_URLIEModeMap[tmpItemData]=tmpInfoTreeID.infoopenie[j];
				}
				else
					tmpItemData = tmpInfoTreeID.infotreeid[j]*100+g_TdxInfoINI.companyid[i];
				m_TreeCtrl->SetItemData(hChild,tmpItemData);
				bHasChild = FALSE;
				for(k=0;k < tmpInfoTreeID.infotree_num;k++)
				{
					if(tmpInfoTreeID.infotreeid[k]>100 && tmpInfoTreeID.infotreeid[k]/100==tmpInfoTreeID.infotreeid[j])
					{
						bHasChild = TRUE;
						hChildChild = m_TreeCtrl->InsertItem(_F(tmpInfoTreeID.infotreestr[k]),0,0,hChild,TVI_LAST);
						if(strlen(tmpInfoTreeID.infotreeurl[k])>0)
						{
							tmpItemData=MAX_ZX_URL_SEP+k*100+i;
							m_URLMap[tmpItemData]=tmpInfoTreeID.infotreeurl[k];
							m_URLIEModeMap[tmpItemData]=tmpInfoTreeID.infoopenie[k];
						}
						else
							tmpItemData = tmpInfoTreeID.infotreeid[k]*100+g_TdxInfoINI.companyid[i];
						m_TreeCtrl->SetItemData(hChildChild,tmpItemData);
						bHasChildChild = FALSE;
						for(kk=0;kk < tmpInfoTreeID.infotree_num;kk++)
						{
							if(tmpInfoTreeID.infotreeid[kk]>10000 && tmpInfoTreeID.infotreeid[kk]/100==tmpInfoTreeID.infotreeid[k])
							{
								bHasChildChild = TRUE;
								if(strlen(tmpInfoTreeID.infotreeurl[kk])>0)
								{
									hChildChildChild = m_TreeCtrl->InsertItem(_F(tmpInfoTreeID.infotreestr[kk]),2,2,hChildChild,TVI_LAST);
									tmpItemData=MAX_ZX_URL_SEP+kk*100+i;
									m_URLMap[tmpItemData]=tmpInfoTreeID.infotreeurl[kk];
									m_URLIEModeMap[tmpItemData]=tmpInfoTreeID.infoopenie[kk];
								}
								else
								{
									hChildChildChild = m_TreeCtrl->InsertItem(_F(tmpInfoTreeID.infotreestr[kk]),1,1,hChildChild,TVI_LAST);
									tmpItemData = tmpInfoTreeID.infotreeid[kk]*100+g_TdxInfoINI.companyid[i];
								}
								m_TreeCtrl->SetItemData(hChildChildChild,tmpItemData);
							}
						}
						if(!bHasChildChild)
							m_TreeCtrl->SetItemImage(hChildChild,1,1);
					}
				}
				if(!bHasChild)	//如果没有子女,则将其改变成子枝的图标
					m_TreeCtrl->SetItemImage(hChild,1,1);
			}
		}
	}
}

void CZxTree::FillTree()
{
	GetFromTdxInfoINI();	//从tdxinfo.ini中读取信息
	HTREEITEM hRoot=TVI_ROOT,hChild=TVI_ROOT;
	int i;
	m_TreeCtrl->SetRedraw(FALSE);
	m_TreeCtrl->DeleteAllItems();

	if(g_OEM.bGgxxTopInZxTree)
	{
		FillGgxx();
		for(i=0;i < g_TdxInfoINI.companynum;i++)
		{
			if(i!=0 || !g_GC.bHasZxTree23)
				FillOneZX(i);
		}
		FillZxTool();
	}
	else
	{
		BOOL bSpecialTurn=FALSE;
		for(i=0;i < g_TdxInfoINI.companynum;i++)
			if(strstr(g_TdxInfoINI.companystr[i],"中原证券"))
				break;
		if(i < g_TdxInfoINI.companynum) 
			bSpecialTurn=TRUE;
		if(!bSpecialTurn)
			FillZxTool();
		for(i=0;i < g_TdxInfoINI.companynum;i++)
		{
			if(i!=0 || !g_GC.bHasZxTree23)
				FillOneZX(i);
		}
		if(bSpecialTurn)
			FillZxTool();
		FillGgxx();
	}
	HTREEITEM hCurItem = m_TreeCtrl->GetRootItem();
	HTREEITEM hFirstItem = hCurItem;
	//全部展开
	if(g_TdxInfoINI.bExpandAllTree)
	{
		do
		{
			if(m_TreeCtrl->ItemHasChildren(hCurItem))
			{
				m_TreeCtrl->Expand(hCurItem,TVE_EXPAND);
			}
		}while(hCurItem=m_TreeCtrl->GetNextItem(hCurItem,TVGN_NEXT));
	}
	if(hFirstItem) m_TreeCtrl->EnsureVisible(hFirstItem);
	m_TreeCtrl->SetRedraw(TRUE);
}

int CZxTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_TreeCtrl = new CMyTreeCtrl(FALSE);
	m_TreeCtrl->Create(WS_VISIBLE|WS_CHILD|TVS_SHOWSELALWAYS,CRect(0,0,0,0),this,IDC_ZXTREE_CTRL);	
	pVScrollBar=new CMyScrollBar;
	if(!pVScrollBar || !pVScrollBar->Create(_T("MyScrollBar"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_ZXTREE_VSCROLL))
		return -1;
	pVScrollBar->Init(X_VERTICAL,m_hWnd,UM_VSCROLLBAR);

	m_ImageList.Create(16,16,ILC_COLOR32|ILC_MASK,5,0);
	long IconList[]={IDI_FOLDER,IDI_ZX2,IDI_WEB};
	int IconLength=sizeof(IconList)/sizeof(long);
	HICON hIcon;
	for(int i=0;i<IconLength;i++)
	{
		hIcon=::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IconList[i]));
		m_ImageList.Add(hIcon);
	}
	m_TreeCtrl->SetImageList (&m_ImageList, TVSIL_NORMAL);

	m_pNewFont=new CFont;
	m_pNewFont->CreateFont(14,0,0,0,FW_MEDIUM,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,SONG_FONT);
	m_TreeCtrl->SetFont(m_pNewFont);
	m_TreeCtrl->SetItemHeight(m_TreeCtrl->GetItemHeight()+4);
	
	SetAppearance(TreeColor.TreeBackColor,TreeColor.TreeForeColor);
	if(OffLineMode)
		FillTree();
	return 0;
}

void CZxTree::OnSize(UINT nType, int cx, int cy) 
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

void CZxTree::SetScrollPos() 
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

void CZxTree::CarryOut(DWORD ItemData)
{
	if(ItemData==989)
	{
		CRecentSearchDlg dlg;
		if(dlg.DoModal()==IDCANCEL)
			return;
		ItemData=dlg.m_ItemData;
	}
	if(ItemData<MAX_ZX_URL_SEP)
		AfxGetMainWnd()->SendMessage(UM_TREEVIEW_MESSAGE,ZX_TREETYPE,ItemData);
	else
	{
		map<DWORD, string>::iterator iter;
		iter = m_URLMap.find(ItemData);
		if(iter!=m_URLMap.end())				
		{
			WebURL = (*iter).second.c_str();
			if(WebURL.GetLength()>0)
			{
				BOOL bInsideOpen=TRUE;
				map<DWORD, short>::iterator iter2;
				iter2 = m_URLIEModeMap.find(ItemData);
				//如果是在外部打开IE的话
				if(iter2!=m_URLIEModeMap.end() && (*iter2).second==1)
					bInsideOpen=FALSE;
				//打开IE进行处理
				OpenCusWebPage(AfxGetMainWnd(),WebURL,bInsideOpen);
			}
		}
	}
}

void CZxTree::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM  SelectItem = (pNMTreeView->itemNew).hItem;
	if( SelectItem != NULL && !m_TreeCtrl->ItemHasChildren(SelectItem) )
	{
		//取得树叶节点中的数据
		DWORD ItemData = m_TreeCtrl->GetItemData(SelectItem);
		if(LayoutFlag)
			CarryOut(ItemData);
	}	
	*pResult = 0;
}

void CZxTree::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult) 
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
				CarryOut(ItemData);
		}	
	}
	*pResult = 0;
}


void CZxTree::OnDestroy() 
{
	CWnd::OnDestroy();
	
	TDEL(m_TreeCtrl);
	TDEL(m_pNewFont);	
}

void CZxTree::OnNcDestroy() 
{
	CWnd::OnNcDestroy();
	
	delete this;	
}

void CZxTree::OnFillZxTreeMsg(WPARAM wParam,LPARAM lParam)
{
	FillTree();
}

void CZxTree::OnVScrollMessage(WPARAM wParam,LPARAM lParam)
{
	int nPos = (int)wParam;

	HTREEITEM hWantItem = CMyTreeCtrl::GetTreeItem(m_TreeCtrl,nPos);
	if(hWantItem)
	{
		m_TreeCtrl->Select(hWantItem,TVGN_FIRSTVISIBLE);
		m_TreeCtrl->SetRedraw(TRUE);
	}
}

LRESULT CZxTree::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==UM_RESETSCROLLBAR)
	{
		SetScrollPos();
		return 0;
	}
	
	return CWnd::WindowProc(message, wParam, lParam);
}

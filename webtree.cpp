// Webtree.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "webtree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWebTree

IMPLEMENT_DYNCREATE(CWebTree, CWnd)

CWebTree::CWebTree()
{
	m_TreeCtrl = NULL;
	m_pNewFont = NULL;

	pVScrollBar = NULL;
}

CWebTree::~CWebTree()
{
	TDEL(pVScrollBar);
}


BEGIN_MESSAGE_MAP(CWebTree, CWnd)
	ON_MESSAGE(UM_VSCROLLBAR,OnVScrollMessage)
	//{{AFX_MSG_MAP(CWebTree)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_WEBTREE, OnSelchangedTree)
	ON_MESSAGE(UM_TREEREFLECT_MESSAGE,OnTreeReflectMsg)
	ON_WM_DESTROY()
	ON_WM_NCDESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWebTree message handlers

BOOL CWebTree::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CWebTree::SetAppearance(COLORREF bColor,COLORREF fColor) 
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

/////////////////////////////////////////////////////////////////////////////
//根据ini文件构造一棵树的结点
//ptc 标识了树 ，hParent 标识了要存的结点 ，AppName 指明了要构造的结点在ini文件中的appname,
//pinif标识了ini文件，purl存放了所有结的超结链结信息，每一个结点都与一个数据相关，此数据
//为此结点在数据purl中的索引
//index表示了要构造的结点在所在层中的以0为起始的索引，比如此结点是所在层的5个结点，则index为4
//appname的命名方式为：
//每一个结点的子结点我们从零开始编号，各结点间以*隔开
// [0*1*3]表示此结点位于第3层，是第1层的第1个结点的第2个子结点的第4个子结点
// [1]表示第一层第2个结点
void CWebTree::AddNodeFromIni(CTreeCtrl * pTC,HTREEITEM hParent,LPCSTR AppName,CIniFile *pinif,int index,vector<tag_URL> *purl)
{
   pinif->SetAppName(AppName);
   int i;
   int cnt;
   char tmp[10];
   HTREEITEM h;
   char appname[30];
   sprintf(appname,"%s*%d",AppName,index);
   
   CString strText,strURL;
   sprintf(tmp,"url%d",index);
   pinif->GetString(strURL,tmp);
   tag_URL url;
   strcpy(url.addr,strURL);
   purl->push_back(url);
   sprintf(tmp,"text%d",index);
   pinif->GetString(strText,tmp);
   h=pTC->InsertItem(_F(strText),hParent);
   pTC->SetItemData(h,purl->size()-1);
   sprintf(tmp,"node%d",index);
   cnt=pinif->GetInt(tmp,0);
   for(i=0;i<cnt;i++)
   {
	   AddNodeFromIni(pTC,h,appname,pinif,i,purl);
   }
}

//得到某一个结点的子结点个数,h标识了某一结点，pTC指向结点所在树
int CWebTree::GetChildItemCount(HTREEITEM h,CTreeCtrl *pTC)
{
	int cnt=0;
	HTREEITEM hti;
	hti=pTC->GetChildItem(h);
	while(hti)
	{
		cnt++;
		hti=pTC->GetNextSiblingItem(hti);
	}
	return cnt;
}

//设置树中某一结点的子结点的图标，如果不是叶结点，则其图标为索引为0,否则如果个股相关，则图标为1
//其它为2
//hParent标识某一结点，ptc标识了结点所在树,pimage为图标集合,purl为超级链接
void CWebTree::SetNodeIcon(CTreeCtrl *pTC,HTREEITEM hParent,CImageList *pimage,vector<tag_URL> *purl)
{
	HTREEITEM h;
	int index;	
	if(hParent!=TVI_ROOT)
		if(GetChildItemCount(hParent,pTC)==0)//叶结点
		{
			index=pTC->GetItemData(hParent);
			if(index>=0&&index<purl->size())
			{
				if(strstr((*purl)[index].addr,"xxxxxx"))//股票相关代码
				{
					pTC->SetItemImage(hParent,1,1);
				}
				else
				{
					pTC->SetItemImage(hParent,2,2);
				}
			}
		}
		else //非叶结点
		{
			pTC->SetItemImage(hParent,0,0);
		}
	
	h=pTC->GetChildItem(hParent);
	while(h)
	{
		SetNodeIcon(pTC,h,pimage,purl);
		h=pTC->GetNextSiblingItem(h);
	}
}

void CWebTree::CreateTreeFromIni(LPCSTR inifile)
{
	//以下在导航树中展开ini文件中的内容
	m_url.clear();
	
	CIniFile inif;
	inif.SetFileName(inifile);
    int count; 
	count=inif.GetInt("count",0,"root");
	int i;
	for(i=0;i<count;i++)
	{
		AddNodeFromIni(m_TreeCtrl,TVI_ROOT,"0",&inif,i,&m_url);
	}     
    SetNodeIcon(m_TreeCtrl,TVI_ROOT,&m_ImageList,&m_url);    
}

//在网站树前的几个特别树枝:
//1001:注册新用户/用户资料 1002:聊天室 1003:投资论坛 1004:讯达通 ...
void CWebTree::FillTree()
{
	m_TreeCtrl->SetRedraw(FALSE);
	m_TreeCtrl->DeleteAllItems();
	
	//加入几个固定的功能

	CreateTreeFromIni(HomePath+"webinfo.ini");	

	//全部展开
	HTREEITEM hCurItem = m_TreeCtrl->GetRootItem();
	HTREEITEM hFirstItem = hCurItem;
	do
	{
		if(m_TreeCtrl->ItemHasChildren(hCurItem))
		{
			m_TreeCtrl->Expand(hCurItem,TVE_EXPAND);
			break;
		}
	}while(hCurItem=m_TreeCtrl->GetNextItem(hCurItem,TVGN_NEXT));
	if(hFirstItem) m_TreeCtrl->EnsureVisible(hFirstItem);

	m_TreeCtrl->SetRedraw(TRUE);
}

int CWebTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_TreeCtrl = new CMyTreeCtrl(FALSE);
	m_TreeCtrl->Create(WS_VISIBLE|WS_CHILD|TVS_SHOWSELALWAYS,CRect(0,0,0,0),this,IDC_WEBTREE);	
	pVScrollBar=new CMyScrollBar;
	if(!pVScrollBar || !pVScrollBar->Create(_T("MyScrollBar"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_WEBTREE_VSCROLL))
		return -1;
	pVScrollBar->Init(X_VERTICAL,m_hWnd,UM_VSCROLLBAR);

	m_hRoot=LoadIcon(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_WEB_ROOT));
	m_hLink=LoadIcon(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_WEB_LEAF1));
	m_hLinkToStock=LoadIcon(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_WEB_LEAF2));
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,5,0);
	m_ImageList.Add(m_hRoot);
	m_ImageList.Add(m_hLinkToStock);
	m_ImageList.Add(m_hLink);

	m_TreeCtrl->SetImageList (&m_ImageList, TVSIL_NORMAL);

	m_pNewFont=new CFont;
	m_pNewFont->CreateFont(14,0,0,0,FW_MEDIUM,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,SONG_FONT);
	m_TreeCtrl->SetFont(m_pNewFont);
	m_TreeCtrl->SetItemHeight(m_TreeCtrl->GetItemHeight()+4);

	SetAppearance(TreeColor.TreeBackColor,TreeColor.TreeForeColor);

	FillTree();
	
	return 0;
}

void CWebTree::OnSize(UINT nType, int cx, int cy) 
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

BOOL CWebTree::GetFirstURLAboutStock(const char *GpCode,CString &strUrl)
{
	//首先查找当前选中的节点是不是个股相关
	HTREEITEM h=m_TreeCtrl->GetSelectedItem();
	if(h)
	{
		int index;
		index=m_TreeCtrl->GetItemData(h);
		if(index>=0&&index<m_url.size())
		{
			if(strstr(m_url[index].addr,"xxxxxx"))
			{
				char temp[255];
				strcpy(temp,m_url[index].addr);
				char *p=strstr(temp,"xxxxxx");
				memcpy(p,GpCode,6);
				strUrl=temp;
				return true;
			}
		}
	}	
	//从头查找	
	for(int i=0;i<m_url.size();i++)
	{
		if(strstr(m_url[i].addr,"xxxxxx"))
		{
			char temp[255];
			strcpy(temp,m_url[i].addr);
			char *p=strstr(temp,"xxxxxx");
			memcpy(p,GpCode,6);
			strUrl=temp;
			return true;
		}
	}
    return false;
}

void CWebTree::OnTreeReflectMsg(WPARAM wParam,LPARAM lParam)
{
	strcpy(m_strCurStock,WebInfoCurGPCode);
	if(GetFirstURLAboutStock(m_strCurStock,WebURL))
		AfxGetMainWnd()->SendMessage(UM_TREEVIEW_MESSAGE,WEB_TREETYPE,0);	//0为功能 1为股票 2为指标 3为资讯 4为网站
	else
	{

	}
}

void CWebTree::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM  SelectItem = (pNMTreeView->itemNew).hItem;
	if( SelectItem != NULL && !m_TreeCtrl->ItemHasChildren(SelectItem) )
	{
		//取得树叶节点中的数据
		DWORD ItemData = m_TreeCtrl->GetItemData(SelectItem);
		if(LayoutFlag)
		{
			if(ItemData>=1001)
			{
				AfxGetMainWnd()->SendMessage(UM_TREEVIEW_MESSAGE,WEB_TREETYPE,ItemData);
			}
			else if(ItemData>=0 && ItemData<m_url.size())
			{
				if(strlen(m_url[ItemData].addr))
				{
					int tempCurrentGPNo = GetSYSCurrentGPIndex();
					if(tempCurrentGPNo != -1)
					{
						WebInfoCurGPCode = (*g_pStockIO)[tempCurrentGPNo]->Code;
						WebInfoCurSetCode = (*g_pStockIO)[tempCurrentGPNo]->setcode;
					}
					strcpy(m_strCurStock,WebInfoCurGPCode);
					WebURL = m_url[ItemData].addr;
					TransferToRealURL(WebURL,(*g_pStockIO)[tempCurrentGPNo]);
					AfxGetMainWnd()->SendMessage(UM_TREEVIEW_MESSAGE,WEB_TREETYPE,ItemData);	//0为功能 1为股票 2为指标 3为资讯 4为网站
				}				 
			}
		}
	}	
	*pResult = 0;
}

void CWebTree::SetScrollPos() 
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

void CWebTree::OnDestroy() 
{
	CWnd::OnDestroy();
	
	TDEL(m_TreeCtrl);
	TDEL(m_pNewFont);	
}

void CWebTree::OnNcDestroy() 
{
	CWnd::OnNcDestroy();
	
	delete this;	
}

void CWebTree::OnVScrollMessage(WPARAM wParam,LPARAM lParam)
{
	int nPos = (int)wParam;

	HTREEITEM hWantItem = CMyTreeCtrl::GetTreeItem(m_TreeCtrl,nPos);
	if(hWantItem)
	{
		m_TreeCtrl->Select(hWantItem,TVGN_FIRSTVISIBLE);
		m_TreeCtrl->SetRedraw(TRUE);
	}
}

LRESULT CWebTree::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==UM_RESETSCROLLBAR)
	{
		SetScrollPos();
		return 0;
	}
	
	return CWnd::WindowProc(message, wParam, lParam);
}

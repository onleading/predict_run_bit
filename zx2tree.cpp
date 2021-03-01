// zxtree.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "zx2tree.h"
#include "RecentSearchDlg.h"
#include "verify.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_ZX2_DIRECT_SEP		100000000L

/////////////////////////////////////////////////////////////////////////////
// CZx2Tree

IMPLEMENT_DYNCREATE(CZx2Tree, CWnd)

CZx2Tree::CZx2Tree()
{
	m_TreeCtrl = NULL;
	m_pNewFont = NULL;

	pVScrollBar = NULL;

	treeid_num = 0;
}

CZx2Tree::~CZx2Tree()
{
	TDEL(pVScrollBar);
}


BEGIN_MESSAGE_MAP(CZx2Tree, CWnd)
	ON_MESSAGE(UM_VSCROLLBAR,OnVScrollMessage)
	ON_MESSAGE(UM_FILLZXTREE_MESSAGE,OnFillZxTreeMsg)
	ON_MESSAGE(UM_CJZX2_TITLE_MESSAGE,OnCjzx2TitleMsg)
	//{{AFX_MSG_MAP(CZx2Tree)
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
// CZx2Tree message handlers

BOOL CZx2Tree::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CZx2Tree::SetAppearance(COLORREF bColor,COLORREF fColor) 
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

void CZx2Tree::GetAutoDownFile(char *pFileName,struct AutoDownFileStruct &tmpStruct)
{
	if(g_OEM.bZxzqEmbedWTVersion && TC_GetJyStatus_ && strcmp(pFileName,"zxtree3.cfg")==0) 
	{
		DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
		if(dwStatus&JY_LOGINED || LocalJyVerify())
			pFileName="zxtree3s.cfg";
	}
	CString	Ini = g_WSXHStr+pFileName;
	CString	tempStr;
	GetPrivateProfileString("AUTODOWN","MainURL","",tmpStruct.MainURL,198,Ini);
	tmpStruct.URLNum=GetPrivateProfileInt("AUTODOWN","Num",0,Ini);
	tmpStruct.URLNum=min(tmpStruct.URLNum,50);
	for(int i=0;i<tmpStruct.URLNum;i++)
	{
		//标题
		tempStr.Format("Title%02d",i+1);
		GetPrivateProfileString("AUTODOWN",tempStr,"",tmpStruct.Title[i],48,Ini);
		//URL
		tempStr.Format("URL%02d",i+1);
		GetPrivateProfileString("AUTODOWN",tempStr,"",tmpStruct.URL[i],198,Ini);
		//是否在外部打开IE
		tempStr.Format("OPENIE%02d",i+1);
		tmpStruct.OpenIE[i]=GetPrivateProfileInt("AUTODOWN",tempStr,0,Ini);
		//使用什么图标
		tempStr.Format("ICON%02d",i+1);
		tmpStruct.IconIndex[i]=GetPrivateProfileInt("AUTODOWN",tempStr,0,Ini);
	}
}

void CZx2Tree::FillOneZX(int i)
{
	//开始构造资讯树,目前仅支持三层
	HTREEITEM hRoot=TVI_ROOT,hChild=TVI_ROOT,hChildChild=TVI_ROOT,hChildChildChild=TVI_ROOT;
	int j,k,kk,m;//i:资讯公司 j:资讯公司大分类 k:资讯公司的树枝
	BOOL bHasChild,bHasChildChild;
	CString tmpTitle;
	DWORD tmpItemData,tmpItemData2;
	INFOTREEID tmpInfoTreeID;
	struct AutoDownFileStruct tmpStruct;
	GetFromInfoTree_X(g_TdxInfoINI.companyid[i],tmpInfoTreeID,g_MngRight);
	treeid_num = tmpInfoTreeID.infotree_num;
	if(tmpInfoTreeID.infotree_num > 0)
	{
		for(j=0;j < tmpInfoTreeID.infotree_num;j++)
		{
			infotreeid[j]=tmpInfoTreeID.infotreeid[j];
			if(tmpInfoTreeID.infotreeid[j] < 100)
			{
				hChild = m_TreeCtrl->InsertItem(_F(tmpInfoTreeID.infotreestr[j]),0,0,TVI_ROOT,TVI_LAST);
				if(strlen(tmpInfoTreeID.infoautodown[j])>0)
				{
					GetAutoDownFile(tmpInfoTreeID.infoautodown[j],tmpStruct);
					tmpItemData=100000+j*100+i;
					m_URLMap[tmpItemData]=tmpStruct.MainURL;
					m_URLIEModeMap[tmpItemData]=0;
					for(m=0;m<tmpStruct.URLNum;m++)
					{
						if(tmpStruct.Title[m][0]=='@')
							tmpTitle.Format("  %s",tmpStruct.Title[m]+1);
						else
							tmpTitle=tmpStruct.Title[m];
						if(tmpStruct.IconIndex[m]>0)
							hChildChild = m_TreeCtrl->InsertItem(_F(tmpTitle),5+tmpStruct.IconIndex[m],5+tmpStruct.IconIndex[m],hChild,TVI_LAST);
						else
							hChildChild = m_TreeCtrl->InsertItem(_F(tmpTitle),3,3,hChild,TVI_LAST);
						tmpItemData2=1000000+m*10000+j;
						m_URLMap[tmpItemData2]=tmpStruct.URL[m];
						m_URLIEModeMap[tmpItemData2]=tmpStruct.OpenIE[m];
						m_TreeCtrl->SetItemData(hChildChild,tmpItemData2);
					}
					infotreeitem[j]=0;
				}
				else if(strlen(tmpInfoTreeID.infotreeurl[j])>0)
				{
					tmpItemData=1000000+j*100+i;
					m_URLMap[tmpItemData]=tmpInfoTreeID.infotreeurl[j];
					m_URLIEModeMap[tmpItemData]=tmpInfoTreeID.infoopenie[j];
					infotreeitem[j]=0;
				}
				else
				{
					tmpItemData = tmpInfoTreeID.infotreeid[j]*100+g_TdxInfoINI.companyid[i];
					infotreeitem[j]=hChild;
				}
				m_TreeCtrl->SetItemData(hChild,tmpItemData);
				bHasChild = FALSE;
				for(k=0;k < tmpInfoTreeID.infotree_num;k++)
				{
					if(tmpInfoTreeID.infotreeid[k]>100 && tmpInfoTreeID.infotreeid[k]/100==tmpInfoTreeID.infotreeid[j])
					{
						bHasChild = TRUE;
						hChildChild = m_TreeCtrl->InsertItem(_F(tmpInfoTreeID.infotreestr[k]),1,1,hChild,TVI_LAST);
						if(strlen(tmpInfoTreeID.infotreeurl[k])>0)
						{
							tmpItemData=1000000+k*100+i;
							m_URLMap[tmpItemData]=tmpInfoTreeID.infotreeurl[k];
							m_URLIEModeMap[tmpItemData]=tmpInfoTreeID.infoopenie[k];
							infotreeitem[j]=0;
						}
						else
						{
							tmpItemData = tmpInfoTreeID.infotreeid[k]*100+g_TdxInfoINI.companyid[i];
							infotreeitem[j]=hChildChild;
						}
						m_TreeCtrl->SetItemData(hChildChild,tmpItemData);
						bHasChildChild = FALSE;
						for(kk=0;kk < tmpInfoTreeID.infotree_num;kk++)
						{
							if(tmpInfoTreeID.infotreeid[kk]>10000 && tmpInfoTreeID.infotreeid[kk]/100==tmpInfoTreeID.infotreeid[k])
							{
								bHasChildChild = TRUE;
								hChildChildChild = m_TreeCtrl->InsertItem(_F(tmpInfoTreeID.infotreestr[kk]),1,1,hChildChild,TVI_LAST);
								if(strlen(tmpInfoTreeID.infotreeurl[kk])>0)
								{
									tmpItemData=1000000+kk*100+i;
									m_URLMap[tmpItemData]=tmpInfoTreeID.infotreeurl[kk];
									m_URLIEModeMap[tmpItemData]=tmpInfoTreeID.infoopenie[kk];
									infotreeitem[j]=0;
								}
								else
								{
									tmpItemData = tmpInfoTreeID.infotreeid[kk]*100+g_TdxInfoINI.companyid[i];								
									infotreeitem[j]=hChildChildChild;
								}
								m_TreeCtrl->SetItemData(hChildChildChild,tmpItemData);
							}
						}
					}
				}
			}
		}
	}
	cur_refresh_treeid = 0;
	//直接将标题行放在树中
	if(!g_OEM.bNoExpandTitleInQsZXTree)
	{
		if(INFO_IsConnect())
			INFO_AskCjzxTitle(infotreeid[cur_refresh_treeid]*100+g_TdxInfoINI.companyid[0]);
	}
}

void CZx2Tree::FillTree()
{
	g_nCJZX2TitleNum = 0;
	GetFromTdxInfoINI();	//从tdxinfo.ini中读取信息
	HTREEITEM hRoot=TVI_ROOT,hChild=TVI_ROOT;
	int i;
	m_URLMap.clear();
	m_URLIEModeMap.clear();
	m_TreeCtrl->DeleteAllItems();
#ifdef OEM_NEWJY
	//如果有"如果没有登录交易，就没有券商的资讯树"这样的选项
	DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
	if(!(dwStatus&JY_LOGINED) && g_OEM.bHasQsZXOnlyLoginJy)
	{
		m_TreeCtrl->InsertItem(_F("未登录交易,无内容"),4,4,TVI_ROOT,TVI_LAST);
		m_TreeCtrl->SetItemData(hRoot,0);
		return;
	}
#endif
	//
	for(i=0;i < g_TdxInfoINI.companynum;i++)
	{
		if(i==0)
		{
			FillOneZX(i);
			break;
		}
	}
	HTREEITEM hCurItem = m_TreeCtrl->GetRootItem();
	HTREEITEM hFirstItem = hCurItem;
	do
	{
		if(m_TreeCtrl->ItemHasChildren(hCurItem))
		{
			m_TreeCtrl->Expand(hCurItem,TVE_EXPAND);
		}
	}while(hCurItem=m_TreeCtrl->GetNextItem(hCurItem,TVGN_NEXT));
	if(hFirstItem) m_TreeCtrl->EnsureVisible(hFirstItem);
}

int CZx2Tree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_TreeCtrl = new CMyTreeCtrl(FALSE);
	m_TreeCtrl->Create(WS_VISIBLE|WS_CHILD|TVS_SHOWSELALWAYS,CRect(0,0,0,0),this,IDC_ZXTREE_CTRL);	
	pVScrollBar=new CMyScrollBar;
	if(!pVScrollBar || !pVScrollBar->Create(_T("MyScrollBar"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_ZX2TREE_VSCROLL))
		return -1;
	pVScrollBar->Init(X_VERTICAL,m_hWnd,UM_VSCROLLBAR);

	m_ImageList.Create(16,16,ILC_COLOR32|ILC_MASK,10,0);
	long IconList[]={IDI_FOLDER,IDI_ZX2,IDI_ZX1,IDI_WEB,IDI_ZX_GG,IDI_ZX_DAYNGG,IDI_ZX_USER1,IDI_ZX_USER2,IDI_ZX_USER3,IDI_ZX_USER4};
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

void CZx2Tree::OnSize(UINT nType, int cx, int cy) 
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

void CZx2Tree::SetScrollPos() 
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

void CZx2Tree::CarryOut(DWORD ItemData)
{
	if(ItemData>=MAX_ZX2_DIRECT_SEP)
		AfxGetMainWnd()->SendMessage(UM_TREEVIEW_MESSAGE,ZX2_TREETYPE,ItemData);
	else if(g_OEM.bNoExpandTitleInQsZXTree && ItemData<100000)
		AfxGetMainWnd()->SendMessage(UM_TREEVIEW_MESSAGE,ZX2_TREETYPE,ItemData);
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

void CZx2Tree::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM  SelectItem = (pNMTreeView->itemNew).hItem;
	if( SelectItem != NULL)
	{
		//取得树叶节点中的数据
		DWORD ItemData = m_TreeCtrl->GetItemData(SelectItem);
		if(LayoutFlag)
			CarryOut(ItemData);
	}	
	*pResult = 0;
}

void CZx2Tree::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult) 
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
		if( SelectItem != NULL)
		{
			//取得树叶节点中的数据
			DWORD ItemData = m_TreeCtrl->GetItemData(SelectItem);
			if(LayoutFlag)
				CarryOut(ItemData);
		}	
	}
	*pResult = 0;
}


void CZx2Tree::OnDestroy() 
{
	CWnd::OnDestroy();
	
	TDEL(m_TreeCtrl);
	TDEL(m_pNewFont);	
}

void CZx2Tree::OnNcDestroy() 
{
	CWnd::OnNcDestroy();
	
	delete this;	
}

void CZx2Tree::OnFillZxTreeMsg(WPARAM wParam,LPARAM lParam)
{
	FillTree();	
}

void RemovePrefix(CString &tmpStr)
{
	TrimString(tmpStr);
	int nPos;
	if(strncmp(tmpStr,"【",2)==0)
	{
		nPos=tmpStr.Find("】");
		tmpStr=tmpStr.Mid(nPos+2);
	}
	else if(strncmp(tmpStr,"[",1)==0)
	{
		nPos=tmpStr.Find("]");
		tmpStr=tmpStr.Mid(nPos+1);
	}	
	tmpStr=tmpStr.Left(22);
}

void CZx2Tree::OnCjzx2TitleMsg(WPARAM wParam,LPARAM lParam)
{
	int titlenum=INFO_GetCjzxNum();
	if(titlenum>0 && infotreeitem[cur_refresh_treeid])
	{
		titlenum=INFO_GetCjzxTitle(min(titlenum,MAX_INFO_TITLE-g_nCJZX2TitleNum),g_pCJZX2Title+g_nCJZX2TitleNum);
		//
		CString tmpStr;
		for(int i=0;i<titlenum;i++)
		{
			tmpStr=g_pCJZX2Title[g_nCJZX2TitleNum+i].title;
			RemovePrefix(tmpStr);
			HTREEITEM hTmpItem;
			if(g_pCJZX2Title[g_nCJZX2TitleNum+i].time_ymd>=g_lOpenRq && g_pCJZX2Title[g_nCJZX2TitleNum+i].minetoken>0)
				hTmpItem=m_TreeCtrl->InsertItem(_F(tmpStr),5,5,infotreeitem[cur_refresh_treeid],TVI_LAST);
			else if(g_pCJZX2Title[g_nCJZX2TitleNum+i].time_ymd>=g_lOpenRq)
				hTmpItem=m_TreeCtrl->InsertItem(_F(tmpStr),4,4,infotreeitem[cur_refresh_treeid],TVI_LAST);			
			else
				hTmpItem=m_TreeCtrl->InsertItem(_F(tmpStr),1,1,infotreeitem[cur_refresh_treeid],TVI_LAST);
			m_TreeCtrl->SetItemData(hTmpItem,MAX_ZX2_DIRECT_SEP+g_nCJZX2TitleNum+i);
		}
		g_nCJZX2TitleNum+=titlenum;
		HTREEITEM hCurItem = m_TreeCtrl->GetRootItem();
		HTREEITEM hFirstItem = hCurItem;
		do
		{
			if(m_TreeCtrl->ItemHasChildren(hCurItem))
			{
				m_TreeCtrl->Expand(hCurItem,TVE_EXPAND);
			}
		}while(hCurItem=m_TreeCtrl->GetNextItem(hCurItem,TVGN_NEXT));
		if(hFirstItem) m_TreeCtrl->EnsureVisible(hFirstItem);
	}
	if(cur_refresh_treeid<treeid_num-1 && g_nCJZX2TitleNum<MAX_INFO_TITLE)
	{
		cur_refresh_treeid++;
		if(INFO_IsConnect())
			INFO_AskCjzxTitle(infotreeid[cur_refresh_treeid]*100+g_TdxInfoINI.companyid[0]);
	}
}

void CZx2Tree::OnVScrollMessage(WPARAM wParam,LPARAM lParam)
{
	int nPos = (int)wParam;

	HTREEITEM hWantItem = CMyTreeCtrl::GetTreeItem(m_TreeCtrl,nPos);
	if(hWantItem)
	{
		m_TreeCtrl->Select(hWantItem,TVGN_FIRSTVISIBLE);
		m_TreeCtrl->SetRedraw(TRUE);
	}
}

LRESULT CZx2Tree::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==UM_RESETSCROLLBAR)
	{
		SetScrollPos();
		return 0;
	}
	
	return CWnd::WindowProc(message, wParam, lParam);
}

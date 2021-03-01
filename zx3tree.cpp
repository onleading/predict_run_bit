// zx3tree.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "zx3tree.h"
#include "RecentSearchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_ZX3_URL_SEP		100000000L
/////////////////////////////////////////////////////////////////////////////
// CZx3Tree

IMPLEMENT_DYNCREATE(CZx3Tree, CWnd)

CZx3Tree::CZx3Tree()
{
	m_TreeCtrl = NULL;
	m_pNewFont = NULL;

	pVScrollBar = NULL;
	m_pExplorer = NULL;
	m_strIEBody = "";

	m_pMeta = NULL;

	m_pZxCache_Info = NULL;
	m_nZXCookieNum = 0;

	memset(m_pZXSecurePackage,0,1024);
}

CZx3Tree::~CZx3Tree()
{
	TDEL(pVScrollBar);
	TDEL(m_pMeta);
	TDEL(m_pZxCache_Info);
}

BEGIN_MESSAGE_MAP(CZx3Tree, CWnd)
	ON_MESSAGE(UM_VSCROLLBAR,OnVScrollMessage)
	ON_MESSAGE(UM_FILLZXTREE_MESSAGE,OnFillZxTreeMsg)
	//{{AFX_MSG_MAP(CZx3Tree)
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
// CZx3Tree message handlers

BOOL CZx3Tree::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CZx3Tree::SetAppearance(COLORREF bColor,COLORREF fColor) 
{
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
	if(m_TreeCtrl)
	{
		m_TreeCtrl->SetBkColor (bColor);
		m_TreeCtrl->SetTextColor (fColor);
	}
}

void CZx3Tree::SetContentAndNavigate(const char *pContent)
{
	if(!m_pExplorer || !::IsWindow(m_pExplorer->m_hWnd))
		return;
	try
	{
		//读出模板
		CString tmpContent="";
		CFile file;
		char tmpFile[MAX_PATH];
		sprintf(tmpFile,"%smeta_profile.dat",g_WSXHStr);
		if(file.Open(tmpFile,CFile::modeRead|CFile::shareDenyNone))
		{
			int nLen = file.GetLength();
			if(nLen>0)
			{
				char * tmpBuffer = new char[nLen+1];
				file.Read(tmpBuffer,nLen);
				tmpBuffer[nLen]=0;
				tmpContent = tmpBuffer;
				delete tmpBuffer;
			}
			file.Close();
		}
		//替换内容
		tmpContent.Replace("@@@BODY@@@",pContent);
		//写出具体文件
		sprintf(tmpFile,"%smeta_profile.htm",g_WSXHStr);
		if(file.Open(tmpFile,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
		{
			file.Write(tmpContent,tmpContent.GetLength());
			file.Close();
		}
		//开始浏览
		m_pExplorer->SetSilent(TRUE);
		m_pExplorer->Navigate(tmpFile,NULL,NULL,NULL,NULL);
	}
	catch(...)
	{
	}
}

char *PROFILE_PARENT_HEAD=
"<dt><a href=\"#\" id=\"KeyId%dBG\" hidefocus=\"true\" onclick=\'showHideModel(\"KeyId%d\",\"KeyId%dBG\")\'>%s</a></dt>\r\n\
        <dd id=\"KeyId%d\"><ul>\r\n";

char *PROFILE_PARENT_TAIL=
"        </ul></dd>\r\n";

char *PROFILE_CHILD=
"<li><a href=\"HTTP://TREEID/%d\" hidefocus=\"true\"><font color=%s>%s</font></a></li>\r\n";

void CZx3Tree::FillOneZX(int i)
{
	if(g_TdxInfoINI.ismeta[i]==0) return;
	//取Meta的设置信息	
	int j,k,nCnt=0;
	DWORD tmpItemData;
	char tempLine[255],tempContent[255];
	CString tempStr,tempYYBList,tempYYB;
	tempYYB.Format("%04d",g_JyInfo.yybid);
	CString Ini = g_WSXHStr+g_strTdxInfoIniName;
	int nMetaNum = GetPrivateProfileInt("Meta","Num",0,Ini);
	nMetaNum = min(nMetaNum,MAX_META_ITEM);
	nMetaNum = max(nMetaNum,0);
	TDEL(m_pMeta);
	m_pMeta = new META_ITEM_INFO[nMetaNum+1];
	for(j=0;j<nMetaNum;j++)
	{
		tempStr.Format("Info%d",j+1);
		GetPrivateProfileString("Meta",tempStr,"",tempLine,254,Ini);
		memset(tempContent,0,255);
		ParseMessageStr(tempLine,tempContent,",",1);
		m_pMeta[nCnt].itemno=atol(tempContent);
		ParseMessageStr(tempLine,tempContent,",",2);
		m_pMeta[nCnt].itemtype=atol(tempContent);
		ParseMessageStr(tempLine,tempContent,",",3);
		m_pMeta[nCnt].parentitem=atol(tempContent);
		ParseMessageStr(tempLine,tempContent,",",4);
		m_pMeta[nCnt].itemlevel=atol(tempContent);
		ParseMessageStr(tempLine,tempContent,",",5);
		m_pMeta[nCnt].productid=atol(tempContent);
		if(m_pMeta[nCnt].productid<0)
			m_pMeta[nCnt].productid = 0;
		tempStr.Format("Name%d",j+1);
		GetPrivateProfileString("Meta",tempStr,"",m_pMeta[nCnt].itemname,49,Ini);
		tempStr.Format("Url%d",j+1);
		GetPrivateProfileString("Meta",tempStr,"",m_pMeta[nCnt].url,254,Ini);
		tempStr.Format("YYB%d",j+1);
		GetPrivateProfileString("Meta",tempStr,"",m_pMeta[nCnt].yyblist,254,Ini);
		tempYYBList=m_pMeta[nCnt].yyblist;
		TrimString(tempYYBList);
		//隐藏非本营业部的信息
		if(tempYYBList.GetLength()>0 && tempYYBList.Find(tempYYB)<0 && tempYYBList.Find("0000")<0)
			continue;
		//隐藏不能使用的META树
		if(g_OEM.bHintCannotUseMeta && TestInfoMngRight(m_pMeta+nCnt)==0)
			continue;
		if(g_OEM.nHintCannotUseMinMetaRight>0 && TestInfoMngRight(m_pMeta+nCnt)==0 && m_pMeta[nCnt].itemlevel>=g_OEM.nHintCannotUseMinMetaRight)
			continue;
		nCnt++;
	}
	//开始构造资讯树,目前仅支持二层	
	HTREEITEM hRoot=TVI_ROOT,hChild=TVI_ROOT;
	int nParentCnt=0;
	for(j=0;j<nCnt;j++)
	{
		if(m_pMeta[j].itemtype==0)
		{
			if(!g_OEM.bMetaUseWebProfile)
			{
				hRoot = m_TreeCtrl->InsertItem(_F(m_pMeta[j].itemname),0,0,TVI_ROOT,TVI_LAST);
				m_TreeCtrl->SetItemData(hRoot,0);
			}
			else
			{
				CString ParentStr;
				ParentStr.Format(PROFILE_PARENT_HEAD,nParentCnt,nParentCnt,nParentCnt,_F(m_pMeta[j].itemname),nParentCnt);
				m_strIEBody+=ParentStr;
			}
			for(k=0;k<nCnt;k++)
			{
				if(m_pMeta[k].itemtype==1 && m_pMeta[k].parentitem==m_pMeta[j].itemno)
				{
					tmpItemData = m_pMeta[k].productid*100+g_TdxInfoINI.companyid[i];
					if(strlen(m_pMeta[k].url)>0)
					{
						tmpItemData=MAX_ZX3_URL_SEP+k;
						m_LevelFlagMap[tmpItemData]=TestInfoMngRight(m_pMeta+k);
						m_URLMap[tmpItemData]=m_pMeta[k].url;
					}
					else
						m_LevelFlagMap[tmpItemData]=TestInfoMngRight(m_pMeta+k);
					if(!g_OEM.bMetaUseWebProfile)
					{
						hChild = m_TreeCtrl->InsertItem(_F(m_pMeta[k].itemname),1,1,hRoot,TVI_LAST);
						m_TreeCtrl->SetItemData(hChild,tmpItemData);
					}
					else
					{
						CString ChildStr;
						if(TestInfoMngRight(m_pMeta+k)==0)
							ChildStr.Format(PROFILE_CHILD,tmpItemData,"#808080",_F(m_pMeta[k].itemname));
						else
							ChildStr.Format(PROFILE_CHILD,tmpItemData,"#000000",_F(m_pMeta[k].itemname));
						m_strIEBody+=ChildStr;
					}
				}
			}
			if(g_OEM.bMetaUseWebProfile)
				m_strIEBody+=PROFILE_PARENT_TAIL;
			nParentCnt++;
		}
	}
}

void CZx3Tree::FillTree()
{
	GetFromTdxInfoINI();	//从tdxinfo.ini中读取信息

	//清除内容
	if(!g_OEM.bMetaUseWebProfile)
	{
		m_TreeCtrl->SetRedraw(FALSE);
		m_TreeCtrl->DeleteAllItems();
	#ifdef OEM_NEWJY
		if(g_OEM.bTopShowMetaRightInfo) //显示权限信息
		{
			CString tmpStr;
			DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
			if((dwStatus&JY_LOGINED))
			{
				CString tmpRight=g_RightStr;
				char *p = strstr(tmpRight,"-");
				if(p) tmpRight = p+1;
				tmpStr.Format("当前级别:%s",tmpRight);
			}
			else
			{
				struct OtherCache_Info OtherInfo;
				LoadOtherCookie(&OtherInfo);
				CString tmpRight=OtherInfo.LastRightStr;
				char *p = strstr(tmpRight,"-");
				if(p) tmpRight = p+1;
				if(g_MngRight>0)
					tmpStr.Format("缓存级别:%s",tmpRight);
				else
					tmpStr.Format("未知级别,请登录交易");
			}
			HTREEITEM hRoot = m_TreeCtrl->InsertItem(_F(tmpStr),2,2,TVI_ROOT,TVI_LAST);
			m_TreeCtrl->SetItemData(hRoot,0);
		}
	#endif		
	}
	else
		m_strIEBody="";
	//生成内容
	int i;
	for(i=0;i < g_TdxInfoINI.companynum;i++)
	{
		if(i==0)
		{
			FillOneZX(i);
			break;
		}
	}
	if(!g_OEM.bMetaUseWebProfile)
	{
		HTREEITEM hCurItem = m_TreeCtrl->GetRootItem();
		HTREEITEM hFirstItem = hCurItem;
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
	else
		SetContentAndNavigate(m_strIEBody);
}

int CZx3Tree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if(!g_OEM.bMetaUseWebProfile)
	{
		m_TreeCtrl = new CMyTreeCtrl(FALSE);
		m_TreeCtrl->Create(WS_VISIBLE|WS_CHILD|TVS_SHOWSELALWAYS,CRect(0,0,0,0),this,IDC_ZXTREE_CTRL);	
		pVScrollBar=new CMyScrollBar;
		if(!pVScrollBar || !pVScrollBar->Create(_T("MyScrollBar"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_ZX3TREE_VSCROLL))
			return -1;
		pVScrollBar->Init(X_VERTICAL,m_hWnd,UM_VSCROLLBAR);

		m_ImageList.Create(16,16,ILC_COLOR32|ILC_MASK,5,0);
		long IconList[]={IDI_FOLDER,IDI_ZX3,IDI_ZX_GG};
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
	}
	else
	{
		m_pExplorer = new CWebBrowser2;
		CRect rect;
		GetClientRect(&rect);
		m_pExplorer->Create(NULL,NULL,WS_CHILD|WS_VISIBLE,rect,this,IDC_CUSTOMIE);
		SetContentAndNavigate("");
	}
	if(OffLineMode)
		FillTree();
	return 0;
}

void CZx3Tree::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	if(m_TreeCtrl) 
	{
		m_TreeCtrl->MoveWindow(0,0,cx-15,cy);
		if(pVScrollBar)
			pVScrollBar->MoveWindow(cx-15,0,15,cy);
	}
	if(m_pExplorer && ::IsWindow(m_pExplorer->m_hWnd))
		m_pExplorer->MoveWindow(0,0,cx,cy);
}

void CZx3Tree::SetScrollPos() 
{
	if(!m_TreeCtrl || !::IsWindow(m_TreeCtrl->m_hWnd)) 
		return;
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

void CZx3Tree::CarryOut(DWORD ItemData)
{
	if(ItemData==0) 
		return;
	//先判断级别
	map<DWORD, char>::iterator iter;
	iter = m_LevelFlagMap.find(ItemData);
	if(iter!=m_LevelFlagMap.end())				
	{
		char tmpFlag = (*iter).second;
		if(tmpFlag==0)
		{
			if(g_OEM.bCanUpgradeUser)
			{
				if(TDX_MessageBox(m_hWnd,"您目前的服务等级无法浏览，是否马上升级？",MB_ICONQUESTION|MB_OKCANCEL)==IDOK)
				{
					if(TC_GetDlg_)
						TC_GetDlg_(DLG_UPGRADEUSER);
				}
			}
			else
			{
				CString tmpHintInfo;
				tmpHintInfo.Format("您的级别[%d]对此栏目不够。\r\n请先提升级别！",g_MngRight);
				TDX_MessageBox(m_hWnd,tmpHintInfo,MB_OK|MB_ICONINFORMATION);
			}
			return;
		}
	}
	//再处理
	if(ItemData<MAX_ZX3_URL_SEP)
		AfxGetMainWnd()->SendMessage(UM_TREEVIEW_MESSAGE,ZX3_TREETYPE,ItemData);
	else
	{
		map<DWORD, string>::iterator iter;
		iter = m_URLMap.find(ItemData);
		if(iter!=m_URLMap.end())				
		{
			CString tmpURL=(*iter).second.c_str();
			if(stricmp(tmpURL,"Vsat")==0)			//如果是维赛特资讯的话
			{
				char tmpPath[MAX_PATH];
				sprintf(tmpPath,"%sVsat\\VsatClient.exe",HomePath);

				STARTUPINFO info_startup;
				memset(&info_startup, 0, sizeof(STARTUPINFO));				
				PROCESS_INFORMATION info_ps;
				memset(&info_ps, 0, sizeof(info_ps));				
				CreateProcess(tmpPath,"_cvsat_ebscn", NULL, NULL, FALSE, 0, NULL, NULL,  &info_startup, &info_ps);
				return;
			}
			else if(stricmp(tmpURL,"Vsat_search")==0)//如果维赛特股搜的话
			{
				AfxGetMainWnd()->PostMessage(UM_SPECIALZX_MSG,0);
				return;
			}
			else if(stricmp(tmpURL,"Vsat_ccbg")==0)	//如果维赛特持仓报告的话
			{
				AfxGetMainWnd()->PostMessage(UM_SPECIALZX_MSG,1);
				return;
			}
			else if(stricmp(tmpURL,"Vsat_xxsd")==0)	//如果维赛特信息速递的话
			{
				AfxGetMainWnd()->PostMessage(UM_SPECIALZX_MSG,2);
				return;
			}
			else if(stricmp(tmpURL,"calc")==0)		//如果是理财金算盘的话
			{
				AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_FUNC_PROFIT);
				return;
			}
			WebURL = tmpURL;
			if(WebURL.GetLength()>0)				//普通网页
				OpenCusWebPage(AfxGetMainWnd(),WebURL,TRUE);
		}
	}
}

void CZx3Tree::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
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

void CZx3Tree::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult) 
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


void CZx3Tree::OnDestroy() 
{
	CWnd::OnDestroy();
	
	TDEL(m_TreeCtrl);
	TDEL(m_pNewFont);	
	TDEL(m_pExplorer);
}

void CZx3Tree::OnNcDestroy() 
{
	CWnd::OnNcDestroy();
	
	delete this;	
}

int CZx3Tree::TestInfoMngRight(META_ITEM_INFO *pMeta)
{
	//读取权限控制
	if(g_OEM.bMetaRightFromZXCookie)
	{
		for(int i=0;i<m_nZXCookieNum;i++)
		{
			if(m_pZxCache_Info[i].productid==pMeta->productid)
				return m_pZxCache_Info[i].flag;
		}
	}
	else if(g_MngRight>=pMeta->itemlevel)
		return 1;
	return 0;

}

void CZx3Tree::OnFillZxTreeMsg(WPARAM wParam,LPARAM lParam)
{
	//读取权限控制
	if(g_OEM.bMetaRightFromZXCookie)
	{
		TDEL(m_pZxCache_Info);
		m_pZxCache_Info = new struct ZxCache_Info[MAX_ZXCOOKIE_NUM];
		m_nZXCookieNum = ReadZXCookie(m_pZxCache_Info,MAX_ZXCOOKIE_NUM,m_pZXSecurePackage);
	}
	FillTree();
}

void CZx3Tree::OnVScrollMessage(WPARAM wParam,LPARAM lParam)
{
	int nPos = (int)wParam;

	HTREEITEM hWantItem = CMyTreeCtrl::GetTreeItem(m_TreeCtrl,nPos);
	if(hWantItem)
	{
		m_TreeCtrl->Select(hWantItem,TVGN_FIRSTVISIBLE);
		m_TreeCtrl->SetRedraw(TRUE);
	}
}

LRESULT CZx3Tree::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==UM_RESETSCROLLBAR)
	{
		SetScrollPos();
		return 0;
	}
	
	return CWnd::WindowProc(message, wParam, lParam);
}

BEGIN_EVENTSINK_MAP(CZx3Tree, CTreeWnd)
    //{{AFX_EVENTSINK_MAP(CMy1Dlg)
	ON_EVENT(CZx3Tree, IDC_CUSTOMIE, 250 /* BeforeNavigate2 */, OnBeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CZx3Tree::OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel) 
{	
	USES_CONVERSION;
	char url[500];
	strcpy(url,W2A(URL->bstrVal));
	if(memicmp(url,"HTTP://TREEID/",14)==0)
	{
		if(LayoutFlag)
			CarryOut(atol(url+14));
		*Cancel=TRUE;	
	}
}

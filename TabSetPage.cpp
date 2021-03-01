// TabSetPage.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "TabSetPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabSetPage dialog
IMPLEMENT_DYNCREATE(CTabSetPage, CPropertyPage)

CTabSetPage::CTabSetPage() : CPropertyPage(CTabSetPage::IDD)
{
	//{{AFX_DATA_INIT(CTabSetPage)
	//}}AFX_DATA_INIT
	m_SysDefItems.clear();
}


void CTabSetPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabSetPage)
	DDX_Control(pDX, IDC_ALLITEMTREE, m_AllItemTree);
	DDX_Control(pDX, IDC_USETABLIST, m_UseItemList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabSetPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTabSetPage)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_UP_BUTTON, OnUpButton)
	ON_BN_CLICKED(IDC_DOWN_BUTTON, OnDownButton)
	ON_BN_CLICKED(IDC_DEL_BUTTON, OnDelButton)
	ON_NOTIFY(NM_DBLCLK, IDC_USETABLIST, OnDblclkUsetablist)
	ON_BN_CLICKED(IDC_TODEF, OnTodef)
	ON_NOTIFY(NM_DBLCLK, IDC_ALLITEMTREE, OnDblclkAllitemtree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabSetPage message handlers

BOOL CTabSetPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 120;
	lvColumn.iSubItem = 0;
	lvColumn.pszText  = "www";
	m_UseItemList.InsertColumn (0,&lvColumn);	//插入一列
	m_UseItemList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);
	HICON hIcon1 = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_INDEX));
	m_ImageList.Add(hIcon1);
	HICON hIcon2 = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_INDEX2));
	m_ImageList.Add(hIcon2);
	m_AllItemTree.SetImageList(&m_ImageList,LVSIL_NORMAL);
	m_UseItemList.SetImageList(&m_ImageList,LVSIL_SMALL);

	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_STATE|LVIF_IMAGE;
	lvItem.state = 0;
	lvItem.iImage = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	int nTotalItemNum = GetDefGridCatItems(m_SysDefItems,TRUE);
	FillAllItemTree(m_SysDefItems);
	LoadCusGridTab(FALSE);
	for(int j = 0;j < g_CustomGridCat.size();j++)
		m_UseItemList.InsertItem (j, _F(g_CustomGridCat[j].lpTitle), g_CustomGridCat[j].bMenu);
	m_AllItemTree.SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);

	UpdateData(FALSE);
	
	return TRUE;
}

void CTabSetPage::FillAllItemTree(vector<GRIDCATITEM> TheDefItems)
{
	HTREEITEM htm;
	LockWindowUpdate();
	m_AllItemTree.DeleteAllItems();
	for(int i=0;i<TheDefItems.size();i++)
	{
		if(!TheDefItems[i].bMenu)
		{
			htm = m_AllItemTree.InsertItem(_F(TheDefItems[i].lpTitle), 0, 0);
			m_AllItemTree.SetItemData(htm, i);  
		}
		else						//有子菜单
		{
			htm = m_AllItemTree.InsertItem(_F(TheDefItems[i].lpTitle), 1, 1);
			m_AllItemTree.SetItemData(htm, i);
			//子菜单
			if(TheDefItems[i].lParam==-1) //特殊的概念板块
			{
				UINT StartID = ID_CONCEPT_MENU_1;
				for(int j= 0;j < g_ConceptBlockNum;j++)
				{
					HTREEITEM htmi = m_AllItemTree.InsertItem(_F(g_ConceptBlock[j].strBlockName), 0, 0, htm);
					m_AllItemTree.SetItemData(htmi, StartID+j);				//+1
				}
			}
			else if(TheDefItems[i].lParam==-2) //特殊的组合板块
			{
				UINT StartID = ID_ZHB_MENU_1;
				for(int j = 0;j < g_ZHBBlockNum;j++)
				{
					HTREEITEM htmi = m_AllItemTree.InsertItem(_F(g_ZHBBlock[j].zhmc), 0, 0, htm);
					m_AllItemTree.SetItemData(htmi, StartID+j);
				}
			}
			else if(TheDefItems[i].lParam==-3) //特殊的用户板块
			{
				UINT StartID = ID_BLOCK_MENU_1;
				for(int j = 0;j < g_nBlockNum;j++)
				{
					HTREEITEM htmi = m_AllItemTree.InsertItem(g_BlockInfo[j].Name, 0, 0, htm);
					m_AllItemTree.SetItemData(htmi, StartID+j);
				}
			}
			else if(TheDefItems[i].lParam==-5) //特殊的证监会行业板块
			{
				UINT StartID = ID_ZJHHY_MENU_1;
				for(int j = 0;j < g_pConstMng->GetZJHHYBlockNum();j++)
				{
					char *strHYName = g_pConstMng->GetHYFromCommandIDXH(j);
					HTREEITEM htmi = m_AllItemTree.InsertItem(_F(strHYName), 0, 0, htm);
					m_AllItemTree.SetItemData(htmi, StartID+j);
				}
			}
			else if(TheDefItems[i].lParam<=-6)	//三方行情
			{
				BYTE  GUIMarket = -6-TheDefItems[i].lParam;
				UINT StartID = IDM_DS1;
				for(int j = 0,k=0;j < MAX_MKT_NUM;j++)
				{
					if(g_GuiMarketSet[GUIMarket].MarketSet[j]==0) break;
					int nDSMarketID = g_GuiMarketSet[GUIMarket].MarketSet[j];
					HTREEITEM htmi = m_AllItemTree.InsertItem(_F(g_GuiMarketSet[GUIMarket].MarketName[j]), 0, 0, htm);
					m_AllItemTree.SetItemData(htmi, StartID+nDSMarketID);
				}
			}
			else
			{
				CNewMenu menu;
				menu.LoadMenu (TheDefItems[i].wParam);
				CNewMenu *pPopup=(CNewMenu *)menu.GetSubMenu(TheDefItems[i].lParam);
				int nItem = pPopup->GetMenuItemCount();
				for(int j=0;j<nItem;j++)
				{
					char strTitle[13]={0};
					pPopup->GetMenuString(j, strTitle, 12, MF_BYPOSITION);
					HTREEITEM htmi = m_AllItemTree.InsertItem(strTitle, 0, 0, htm);
					m_AllItemTree.SetItemData(htmi, pPopup->GetMenuItemID(j));
				}
			}
		}
	}
	UnlockWindowUpdate();
}

void CTabSetPage::OnAddButton() 
{
	POSITION pos2 = m_UseItemList.GetFirstSelectedItemPosition();
	int iNo2 = 0;

	HTREEITEM htm = m_AllItemTree.GetSelectedItem();
	if(htm==NULL)  return;
	GRIDCATITEM gci={0};
	if(GetGridItem(gci, htm))
	{
		POSITION pos2 = m_UseItemList.GetFirstSelectedItemPosition();	
		if(pos2!=NULL)
		{
			iNo2 = m_UseItemList.GetNextSelectedItem(pos2);
			if(iNo2>=g_CustomGridCat.size()) return;
			//不能重复
			for(int i=0;i<g_CustomGridCat.size();i++)
			{
				if(memcmp(&g_CustomGridCat[i], &gci, sizeof(GRIDCATITEM))==0)
					return;
			}
			m_UseItemList.InsertItem(iNo2, gci.lpTitle, gci.bMenu);
			vector<GRIDCATITEM>::iterator iter = g_CustomGridCat.begin();
			advance(iter,iNo2);
			g_CustomGridCat.insert(iter, gci);
		}
		else
		{
			//不能重复
			for(int i=0;i<g_CustomGridCat.size();i++)
			{
				if(memcmp(&g_CustomGridCat[i], &gci, sizeof(GRIDCATITEM))==0)
					return;
			}
			iNo2 = g_CustomGridCat.size();
			m_UseItemList.InsertItem(iNo2, gci.lpTitle, gci.bMenu);
			g_CustomGridCat.push_back(gci);
		}
		m_UseItemList.EnsureVisible(iNo2,FALSE);
	}
}

BOOL CTabSetPage::GetGridItem(GRIDCATITEM &gci, HTREEITEM htm)
{
	DWORD dwData = m_AllItemTree.GetItemData(htm);
	HTREEITEM topTitem = m_AllItemTree.GetParentItem(htm);
	if(topTitem==NULL && dwData>=0 && dwData<m_SysDefItems.size())
	{
		memcpy(&gci, &m_SysDefItems[dwData], sizeof(GRIDCATITEM));
		return TRUE;
	}
	else if(topTitem)
	{
		DWORD dwTopData = m_AllItemTree.GetItemData(topTitem);
		if(dwTopData>=0&&dwTopData<m_SysDefItems.size()&&m_SysDefItems[dwTopData].bMenu)
		{
			memcpy(&gci, &m_SysDefItems[dwTopData], sizeof(GRIDCATITEM));
			CString strTitle = m_AllItemTree.GetItemText(htm);
			strncpy(gci.lpTitle, strTitle, 12);
			gci.lpTitle[12]=0;
			gci.bMenu = FALSE;
			gci.wParam = dwData;
			gci.lParam = 0;
			return TRUE;
		}
	}
	return FALSE;
	
}

void CTabSetPage::OnDelButton() 
{
	POSITION pos2 = m_UseItemList.GetFirstSelectedItemPosition();	
	if(pos2!=NULL)
	{
		int iNo2 = m_UseItemList.GetNextSelectedItem(pos2);
		if(iNo2>=g_CustomGridCat.size()) return;
		m_UseItemList.DeleteItem(iNo2);
		vector<GRIDCATITEM>::iterator iter = g_CustomGridCat.begin();
		advance(iter,iNo2);
		g_CustomGridCat.erase(iter);
	}
}

void CTabSetPage::OnUpButton() 
{
	POSITION pos2 = m_UseItemList.GetFirstSelectedItemPosition();	
	if(pos2!=NULL)
	{
		int iNo2 = m_UseItemList.GetNextSelectedItem(pos2);
		if(iNo2>=g_CustomGridCat.size()) return;
		int upNo=iNo2-1;
		if(upNo>=0)
		{
			vector<GRIDCATITEM>::iterator iterNo = g_CustomGridCat.begin();
			advance(iterNo, iNo2);
			vector<GRIDCATITEM>::iterator iterUpNo = g_CustomGridCat.begin();
			advance(iterUpNo, upNo);
			iter_swap(iterNo, iterUpNo);

			m_UseItemList.SetItem(iNo2, 0, LVIF_TEXT|LVIF_IMAGE, g_CustomGridCat[iNo2].lpTitle, g_CustomGridCat[iNo2].bMenu, 0, 0, 0);
			m_UseItemList.SetItem(upNo, 0, LVIF_TEXT|LVIF_IMAGE|LVIF_STATE, g_CustomGridCat[upNo].lpTitle, g_CustomGridCat[upNo].bMenu, LVIS_SELECTED|LVIS_SELECTED, LVIS_SELECTED|LVIS_SELECTED, 0);

			m_UseItemList.EnsureVisible(upNo,FALSE);
		}
	}
}

void CTabSetPage::OnDownButton() 
{
	POSITION pos2 = m_UseItemList.GetFirstSelectedItemPosition();	
	if(pos2!=NULL)
	{
		int iNo2 = m_UseItemList.GetNextSelectedItem(pos2);
		if(iNo2>=g_CustomGridCat.size()-1) return;
		int dnNo=iNo2+1;
		if(dnNo>=0)
		{
			vector<GRIDCATITEM>::iterator iterNo = g_CustomGridCat.begin();
			advance(iterNo, iNo2);
			vector<GRIDCATITEM>::iterator iterDnNo = g_CustomGridCat.begin();
			advance(iterDnNo, dnNo);
			iter_swap(iterNo, iterDnNo);

			m_UseItemList.SetItem(iNo2, 0, LVIF_TEXT|LVIF_IMAGE, g_CustomGridCat[iNo2].lpTitle, g_CustomGridCat[iNo2].bMenu, 0, 0, 0);
			m_UseItemList.SetItem(dnNo, 0, LVIF_TEXT|LVIF_IMAGE|LVIF_STATE, g_CustomGridCat[dnNo].lpTitle, g_CustomGridCat[dnNo].bMenu, LVIS_SELECTED|LVIS_SELECTED, LVIS_SELECTED|LVIS_SELECTED, 0);

			m_UseItemList.EnsureVisible(dnNo,FALSE);
		}
	}
}

void CTabSetPage::OnDblclkUsetablist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnDelButton();
	
	*pResult = 0;
}

void CTabSetPage::OnTodef() 
{
	g_CustomGridCat.clear();
	GetDefGridCatItems(g_CustomGridCat,FALSE);
	m_UseItemList.DeleteAllItems();
	for(int j = 0;j < g_CustomGridCat.size();j++)	//填充条件选股指标列表框
		m_UseItemList.InsertItem (j, _F(g_CustomGridCat[j].lpTitle), g_CustomGridCat[j].bMenu);
}

void CTabSetPage::OnOK()
{
	SaveCusGridTab();
	LoadAndResetGridTab();
}

void CTabSetPage::OnDblclkAllitemtree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnAddButton();
	
	*pResult = 0;
}

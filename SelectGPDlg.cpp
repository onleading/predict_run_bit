// SelectGPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "SelectGPDlg.h"
#include "KeyGuy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectGPDlg dialog


CSelectGPDlg::CSelectGPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectGPDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectGPDlg)
	//}}AFX_DATA_INIT
	bSingleSelect = FALSE;
	bOverlapGpType = 0;
	bCurDomain = SH;
	KeyGuyFlag = FALSE;

	bHasDS = TRUE;
	m_pStkInfo = NULL;
	m_nLinkNum = 0;
}


void CSelectGPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectGPDlg)
	DDX_Control(pDX, IDC_SELECTGP_TAB, m_SelectGP_Tab);
	DDX_Control(pDX, IDC_GPTYPE_LIST, m_GPType_List);
	DDX_Control(pDX, IDC_GP_LIST, m_GP_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectGPDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectGPDlg)
	ON_NOTIFY(TCN_SELCHANGE, IDC_SELECTGP_TAB, OnSelchangeSelectgpTab)
	ON_NOTIFY(NM_DBLCLK, IDC_GP_LIST, OnDblclkGpList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_GPTYPE_LIST, OnItemchangedGptypeList)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectGPDlg message handlers

BOOL CSelectGPDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 105;
	lvColumn.iSubItem = 0;
	lvColumn.pszText  = "F0514M0824";
	m_GPType_List.InsertColumn (0,&lvColumn);

	if(bOverlapGpType==1 || bOverlapGpType==4)
		SetWindowText(_F("选择叠加的品种"));
	else if(bOverlapGpType==2)
		SetWindowText(_F("选择基准股票"));
	else if(bOverlapGpType==3)
		SetWindowText(_F("选择对照品种"));
	if(bSingleSelect)
	{
		m_GP_List.ModifyStyle (NULL,LVS_SINGLESEL);
		SetDlgItemText(IDC_HINTINFO_STATIC,_F("仅单选，可用键盘精灵选择"));
		GetDlgItem(IDC_SELECTALL)->ShowWindow(SW_HIDE);
	}
	m_SelectGP_Tab.SetMinTabWidth(30);
	m_SelectGP_Tab.InsertItem(0,_F("分类"));
	m_SelectGP_Tab.InsertItem(1,_F("地区板块"));
	m_SelectGP_Tab.InsertItem(2,_F("行业板块"));
	m_SelectGP_Tab.InsertItem(3,_F("概念板块"));
//	m_SelectGP_Tab.InsertItem(4,_F("组合板块"));
	m_SelectGP_Tab.InsertItem(4,_F("用户板块"));
	if(bOverlapGpType==1 || bOverlapGpType==4 || bOverlapGpType==3)
	{
		if(m_pStkInfo)
			m_nLinkNum = g_pStockIO->GetRLTData(m_pStkInfo, m_LinkData, 100);
		if(g_bHasDS && bHasDS)	
		{
			m_SelectGP_Tab.InsertItem(5,_F(g_DS.DSOEMName));
			m_SelectGP_Tab.InsertItem(6,_F("其它"));
			m_SelectGP_Tab.SetCurSel(6);
			CurrentTabSel = 6;
		}
		else
		{
			m_SelectGP_Tab.InsertItem(5,_F("其它"));
			m_SelectGP_Tab.SetCurSel(5);
			CurrentTabSel = 5;
		}
	}
	else
	{
		if(g_bHasDS && bHasDS)	
			m_SelectGP_Tab.InsertItem(5,_F(g_DS.DSOEMName));
		m_SelectGP_Tab.SetCurSel(0);
		CurrentTabSel = 0;
	}

	m_ImageList1.Create(16,16,ILC_COLOR8|ILC_MASK,1,0);
	m_ImageList2.Create(16,16,ILC_COLOR8|ILC_MASK,1,0);

	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_BLOCK));
	m_ImageList1.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_ImageList2.Add(hIcon);
	m_GPType_List.SetImageList(&m_ImageList1,LVSIL_SMALL);
	m_GP_List.SetImageList(&m_ImageList2,LVSIL_SMALL);

	FillGPTypeList();
	if(bOverlapGpType==1 || bOverlapGpType==4 || bOverlapGpType==3) //对于叠加和对照方式，另行处理
	{
		CurrentGPTypeSel = 0;
		FillGPList();
		m_GP_List.SetFocus();
		if(bCurDomain==SH)  
		{
			m_GP_List.SetItemState(0,LVIS_FOCUSED,LVIS_FOCUSED);
			m_GP_List.SetItemState(0,LVIS_SELECTED,LVIS_SELECTED);
		}
		else if(bCurDomain==SZ)
		{
			m_GP_List.SetItemState(1,LVIS_FOCUSED,LVIS_FOCUSED);
			m_GP_List.SetItemState(1,LVIS_SELECTED,LVIS_SELECTED);
		}
		return FALSE;
	}
	else
		CurrentGPTypeSel = -1;

	return TRUE;
}

void CSelectGPDlg::FillGPTypeList()
{
	m_GPType_List.DeleteAllItems();

	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	lvItem.iImage = 0;

	int i,k,kk,nCnt;
	switch(CurrentTabSel)
	{
	case 0:
		for(i = 0;i < TOTAL_TYPENUM;i++)
		{
			lvItem.iItem = i;
			lvItem.pszText = _F(GPType[i]);
			m_GPType_List.InsertItem (&lvItem);
		}
		if(!g_bHasDS && bHasDS) break;
		nCnt=0;
		for(k=0;k<g_GuiMarketNum;k++)
		{
			for(kk=0;kk < MAX_MKT_NUM;kk++)
			{
				int nDSMarketID = g_GuiMarketSet[k].MarketSet[kk];
				if(nDSMarketID==0) continue;
				lvItem.iItem = TOTAL_TYPENUM+nCnt;
				lvItem.pszText = _F(g_GuiMarketSet[k].MarketName[kk]);
				m_GPType_List.InsertItem (&lvItem);
				nCnt++;
			}
		}
		break;
	case 1:
		for(i = 0;i < TOTAL_DY_NUM;i++)
		{
			lvItem.iItem = i;
			lvItem.pszText = _F(g_cwAddrTopic[i]);
			m_GPType_List.InsertItem (&lvItem);
		}
		break;
	case 2:
		for(i = 0;i < TOTAL_HY_NUM;i++)
		{
			lvItem.iItem = i;
			lvItem.pszText = _F(g_cwHYstr[i]);
			m_GPType_List.InsertItem (&lvItem);
		}
		break;
	case 3:
		for(i = 0;i < g_ConceptBlockNum;i++)
		{
			lvItem.iItem = i;
			lvItem.pszText = _F(g_ConceptBlock[i].strBlockName);
			m_GPType_List.InsertItem (&lvItem);
		}
		break;
	case 4:
		//填充板块列表
		lvItem.iItem = 0;
		lvItem.pszText = _F("自选股");
		m_GPType_List.InsertItem (&lvItem);
		lvItem.iItem = 1;
		lvItem.pszText = _F("股票池");
		m_GPType_List.InsertItem (&lvItem);

		BlockGPName[0] = _F("自选股");
		BlockGPName[1] = _F("股票池");
		for(i = 0;i < g_nBlockNum;i++)
		{
			lvItem.iItem = i+2;
			lvItem.pszText = g_BlockInfo[i].Name;
			m_GPType_List.InsertItem (&lvItem);

			BlockGPName[i+2] = g_BlockInfo[i].Name;
		}
		break;
	case 5:
		if(g_bHasDS && bHasDS)
		{
			int k,kk,nCnt=0;
			for(k=0;k<g_GuiMarketNum;k++)
			{
				for(kk=0;kk < MAX_MKT_NUM;kk++)
				{
					int nDSMarketID = g_GuiMarketSet[k].MarketSet[kk];
					if(nDSMarketID==0) continue;
					lvItem.iItem = nCnt;
					lvItem.pszText = _F(g_GuiMarketSet[k].MarketName[kk]);
					m_GPType_List.InsertItem (&lvItem);
					nCnt++;
				}
			}
		}
		else if(bOverlapGpType==1 || bOverlapGpType==4)
		{
			//填充板块列表
			lvItem.iItem = 0;
			lvItem.pszText = _F("叠加品种");
			m_GPType_List.InsertItem (&lvItem);
			BlockGPName[0] = _F("叠加品种");
		}
		else if(bOverlapGpType==3)
		{
			//填充板块列表
			lvItem.iItem = 0;
			lvItem.pszText = _F("对照品种");
			m_GPType_List.InsertItem (&lvItem);
			BlockGPName[0] = _F("对照品种");
		}
		break;
	case 6:
		if(bOverlapGpType==1 || bOverlapGpType==4)
		{
			//填充板块列表
			lvItem.iItem = 0;
			lvItem.pszText = _F("叠加品种");
			m_GPType_List.InsertItem (&lvItem);
			BlockGPName[0] = _F("叠加品种");
		}
		else if(bOverlapGpType==3)
		{
			//填充板块列表
			lvItem.iItem = 0;
			lvItem.pszText = _F("对照品种");
			m_GPType_List.InsertItem (&lvItem);
			BlockGPName[0] = _F("对照品种");
		}
		break;
	}
}

void CSelectGPDlg::FillGPList()
{
	m_GP_List.SetRedraw(FALSE);
	m_GP_List.DeleteAllItems();
	int i;
	CString tempName;
	short       lpnUsedStkIndex[MAXGPNUM]={0};	//股票序号数组
	switch(CurrentTabSel)
	{
	case 0:
		{
			int nTypeSel=CurrentGPTypeSel;
			if(nTypeSel>=TOTAL_TYPENUM)
			{
				int nXH = DSGetMarketXHFromGuiOrder(nTypeSel-TOTAL_TYPENUM);
				if(nXH<0)
					break;
				nTypeSel = nXH+DS_START;
			}
			int TotalRow=GetGpIndex(-1,nTypeSel,lpnUsedStkIndex);
			for(i = 0;i < TotalRow;i++)
			{
				tempName = (*g_pStockIO)[lpnUsedStkIndex[i]]->Name;
				tempName.TrimRight();
				m_GP_List.InsertItem(i,tempName,0);
				m_GP_List.SetItemData(i,lpnUsedStkIndex[i]);
			}
			break;
		}
	case 1:
	case 2:
		{
			int TotalRow = FiltDYHYCode(CurrentTabSel-1,CurrentGPTypeSel,lpnUsedStkIndex);
			for(i = 0;i < TotalRow;i++)
			{
				tempName = (*g_pStockIO)[lpnUsedStkIndex[i]]->Name;
				tempName.TrimRight();
				m_GP_List.InsertItem(i,tempName,0);
				m_GP_List.SetItemData(i,lpnUsedStkIndex[i]);
			}
			break;
		}
	case 3:
		{
			int TotalRow = FiltConceptCode(CurrentGPTypeSel,lpnUsedStkIndex);
			for(i = 0;i < TotalRow;i++)
			{
				tempName = (*g_pStockIO)[lpnUsedStkIndex[i]]->Name;
				tempName.TrimRight();
				m_GP_List.InsertItem(i,tempName,0);
				m_GP_List.SetItemData(i,lpnUsedStkIndex[i]);
			}
			break;
		}
	case 4:
		{
			int TotalRow=0;
			if(CurrentGPTypeSel==0)
				TotalRow=GetGpIndex(-1,TYPE_ZXG,lpnUsedStkIndex);
			else if(CurrentGPTypeSel==1)
				TotalRow=GetGpIndex(-1,TYPE_TJG,lpnUsedStkIndex);
			else
				TotalRow=GetGpIndex(CurrentGPTypeSel-2,-1,lpnUsedStkIndex);
			for(i = 0;i < TotalRow;i++)
			{
				tempName = (*g_pStockIO)[lpnUsedStkIndex[i]]->Name;
				tempName.TrimRight();
				m_GP_List.InsertItem(i,tempName,0);
				m_GP_List.SetItemData(i,lpnUsedStkIndex[i]);
			}
			break;
		}
	case 5:
	case 6:
		{
			if(CurrentTabSel==6 && g_bHasDS && bHasDS)
			{
				int nXH=DSGetMarketXHFromGuiOrder(CurrentGPTypeSel);
				if(nXH<0)
					break;
				int DSNum = GetGpIndex(-1,nXH+DS_START,lpnUsedStkIndex);
				for(i = 0;i < DSNum;i++)
				{
					MemStkInfo	*pStkInfo = g_pStockIO->GetDSStkInfo(lpnUsedStkIndex[i]);
					if(!pStkInfo) break;
					m_GP_List.InsertItem(i,pStkInfo->Name,0);
					m_GP_List.SetItemData(i,lpnUsedStkIndex[i]);
				}
				break;
			}
			else if(bOverlapGpType==1 || bOverlapGpType==4 || bOverlapGpType==3)
			{
				int gpindex,i,nAddedItem=0;				
				if(bOverlapGpType!=4)
				{
					gpindex = g_pStockIO->GetIndex(SH_DPCODE);
					if(gpindex==-1) break;
					m_GP_List.InsertItem(nAddedItem,(*g_pStockIO)[gpindex]->Name,0);
					m_GP_List.SetItemData(nAddedItem++,gpindex);
					
					gpindex = g_pStockIO->GetIndex(SZ_DPCODE);
					if(gpindex==-1) break;
					m_GP_List.InsertItem(nAddedItem,(*g_pStockIO)[gpindex]->Name,0);
					m_GP_List.SetItemData(nAddedItem++,gpindex);
				}
				//加入ETF对应的指数
				for(i=0;m_pStkInfo && i<g_nETFInfoNum;i++)
				{
					if(g_ETFInfo[i].setcode==m_pStkInfo->setcode && strcmp(m_pStkInfo->Code,g_ETFInfo[i].Code)==0)
					{
						gpindex = g_pStockIO->GetIndex(g_ETFInfo[i].IndexCode,m_pStkInfo->setcode);
						if(gpindex==-1) break;
						m_GP_List.InsertItem(nAddedItem,(*g_pStockIO)[gpindex]->Name,0);
						m_GP_List.SetItemData(nAddedItem++,gpindex);				
					}
				}
				//加入关联品种
				MemStkInfo *tmpInfo;
				CString tmpStr;
				if(m_pStkInfo && !testzs(m_pStkInfo))
				{					
					for(i=0;i < m_nLinkNum;i++)
					{
						if(strlen(m_LinkData[i].Code)<1) 
							continue;
						if(m_pStkInfo->setcode==m_LinkData[i].SetCode&&strcmp(m_pStkInfo->Code,m_LinkData[i].Code)==0) 
							continue;
						if(strlen(m_LinkData[i].Code)==5)
						{
							tmpStr=m_LinkData[i].Code;
							tmpInfo = g_pStockIO->GetDSStkInfo(31,tmpStr);
							if(!tmpInfo)
							{
								tmpInfo = g_pStockIO->GetDSStkInfo(32,tmpStr);
								if(!tmpInfo)
								{
									tmpStr=(CString)"HK"+m_LinkData[i].Code;
									tmpInfo = g_pStockIO->GetDSStkInfo(31,tmpStr);
								}
							}
						}
						else
							tmpInfo = g_pStockIO->Get_MemStkInfo(m_LinkData[i].Code, m_LinkData[i].SetCode);
						if(tmpInfo)
						{
							gpindex = g_pStockIO->GetIndex(tmpInfo->Code);
							m_GP_List.InsertItem(nAddedItem,tmpInfo->Name,0);
							m_GP_List.SetItemData(nAddedItem,gpindex);
							++nAddedItem;
						}
					}
				}
			}
			break;
		}
	}
	m_GP_List.SetRedraw(TRUE);
}

void CSelectGPDlg::OnOK() 
{
	SelectedGPNum = 0;
	POSITION pos = m_GP_List.GetFirstSelectedItemPosition();
	if( pos != NULL )
	{
		int nItem,nIndex;
		while (pos)
		{
			nItem = m_GP_List.GetNextSelectedItem(pos);
			nIndex = m_GP_List.GetItemData(nItem);
			SelectedGPIndex[SelectedGPNum++] = nIndex;
		}
		CDialog::OnOK();
	}
}

void CSelectGPDlg::OnSelchangeSelectgpTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int nCurSel = m_SelectGP_Tab.GetCurSel();
	if(nCurSel != CurrentTabSel)
	{
		CurrentTabSel = nCurSel;
		FillGPTypeList();
		CurrentGPTypeSel = -1;
		m_GP_List.DeleteAllItems();
	}
	*pResult = 0;
}

void CSelectGPDlg::OnDblclkGpList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	*pResult = 0;
}

BOOL CSelectGPDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_CHAR && KeyGuyFlag == FALSE )
	{
		if((pMsg->wParam>=0x30&&pMsg->wParam<=0x39)||(pMsg->wParam>='a'&&pMsg->wParam<='z')
			||(pMsg->wParam>='A'&&pMsg->wParam<='Z')||pMsg->wParam== '.'||pMsg->wParam== '?')
		{
			CKeyGuy dlg;

			CRect rect;
			GetWindowRect(&rect);
			dlg.RightBottomPoint = CPoint(rect.right,rect.bottom);

			dlg.bHasCmd = FALSE;
			KeyGuyFlag = TRUE;

			char tempchar;
			char tempstr[2];

            tempchar=(char)pMsg->wParam; 
			tempstr[0]=tempchar;
			tempstr[1]=0;
			dlg.m_edit=(CString)tempstr;
			if( dlg.DoModal() == IDOK )
			{
				int m_index=-1;
				if(dlg.ReturnType >= 20) //如果是三方行情
					m_index = g_pStockIO->GetIndex(dlg.ReturnValue.Mid(2),dlg.ReturnType-20);
				else if(dlg.ReturnType == 0)
				{
					CString tmpCode = dlg.ReturnValue.Mid(1);
					short	setcode = dlg.ReturnValue[0]-'0';
					m_index = g_pStockIO->GetIndex(tmpCode,setcode);
				}
				//注意是否有此索引值
				if(m_index != -1)
				{
					SelectedGPIndex[0] = m_index;
					SelectedGPNum = 1;
					CDialog::OnOK();
					return TRUE;
				}
			}
			KeyGuyFlag=FALSE;
			return TRUE;
		}
	}	
	return CDialog::PreTranslateMessage(pMsg);
}

void CSelectGPDlg::OnItemchangedGptypeList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	POSITION pos;
	pos = m_GPType_List.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{
		int index = m_GPType_List.GetNextSelectedItem(pos);
		if(index != CurrentGPTypeSel)
		{
			CurrentGPTypeSel = index;
			FillGPList();
		}
	}
	*pResult = 0;
}

void CSelectGPDlg::OnSelectall() 
{
	int nCnt = m_GP_List.GetItemCount();
	if(nCnt <= 0) return;
	m_GP_List.SetFocus();
	for(int i=0;i < nCnt;i++)
		m_GP_List.SetItemState(i,LVIS_SELECTED,LVIS_SELECTED);
}

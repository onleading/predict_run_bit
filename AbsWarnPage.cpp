// AbsWarnPage.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "AbsWarnPage.h"
#include "TjWarnConfDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAbsWarnPage property page

IMPLEMENT_DYNCREATE(CAbsWarnPage, CPropertyPage)

CAbsWarnPage::CAbsWarnPage() : CPropertyPage(CAbsWarnPage::IDD)
{
	//{{AFX_DATA_INIT(CAbsWarnPage)
	//}}AFX_DATA_INIT
}

CAbsWarnPage::~CAbsWarnPage()
{
}

void CAbsWarnPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAbsWarnPage)
	DDX_Control(pDX, IDC_ABSWARN_LIST, m_AbsWarnList);
	DDX_Control(pDX, IDC_STATUS, m_CStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAbsWarnPage, CPropertyPage)
	//{{AFX_MSG_MAP(CAbsWarnPage)
	ON_BN_CLICKED(IDC_ADDABS, OnAddabs)
	ON_BN_CLICKED(IDC_MODIFYABS, OnModifyabs)
	ON_BN_CLICKED(IDC_DELETEABS, OnDeleteabs)
	ON_BN_CLICKED(IDC_ZAPABS, OnZapabs)
	ON_NOTIFY(NM_DBLCLK, IDC_ABSWARN_LIST, OnDblclkAbswarnList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbsWarnPage message handlers

BOOL CAbsWarnPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	if(m_nListFlag == GPLIST)
		m_CStatus.SetText(_F("可修改上破下破价等"));
	else
		m_CStatus.SetText(_F("在公式前打勾启用"));

	UpdateData(FALSE);
	
	m_ImageList.Create(16,16,ILC_COLOR8|ILC_MASK,1,1);

	HICON hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_GP));
	m_ImageList.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_INDEX));
	m_ImageList.Add(hIcon);
	hIcon = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDI_LEVIN));
	m_ImageList.Add(hIcon);	

	m_AbsWarnList.SetImageList(&m_ImageList,LVSIL_SMALL);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_WIDTH|LVCF_TEXT|LVCF_FMT|LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	if(m_nListFlag == GPLIST)
	{
		char *ColStr[7] = {"股票代码","股票名称","昨收价","上破价","下破价","换手(%)","折溢率(%)"};
		int ColWidth[7] = {70,70,60,60,60,70,70};
		for(int i = 0; i < 7;i++)
		{
			lvColumn.pszText = _F(ColStr[i]);
			lvColumn.cx = ColWidth[i];
			lvColumn.iSubItem = i;
			m_AbsWarnList.InsertColumn (i,&lvColumn);
		}
	}
	else if(m_nListFlag == TJLIST)
	{
		m_AbsWarnList.SetExtendedStyle(LVS_EX_CHECKBOXES);
		char *ColStr[2] = {"预警公式", "公式参数"};
		int ColWidth[2] = {250, 135};
		for(int i=0;i<2;i++)
		{
			lvColumn.pszText = _F(ColStr[i]);
			lvColumn.cx = ColWidth[i];
			lvColumn.iSubItem = i;
			m_AbsWarnList.InsertColumn (i,&lvColumn);
		}
	}
	FillList();
	CurrentAbs = -1;
	if(g_pAbsConditionNum > 0 && m_nListFlag==GPLIST)
	{
		CurrentAbs = 0;
		m_AbsWarnList.SetItemState(CurrentAbs, LVIS_SELECTED,LVIS_SELECTED);
		m_AbsWarnList.SetItemState(CurrentAbs, LVIS_FOCUSED,LVIS_FOCUSED);
	}
	return TRUE; 
}

void CAbsWarnPage::FillList()
{
	int i, j;
	m_AbsWarnList.SetRedraw(FALSE);
	m_AbsWarnList.DeleteAllItems ();
	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_STATE;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iSubItem = 0;
	if(m_nListFlag==GPLIST)
	{
		CString tempStr;
		struct MemStkInfo *scodetmp;
		int	xh;
		for(i = 0;i < g_pAbsConditionNum;i++)
		{
			xh = g_pStockIO->GetIndex(g_pAbsCondition[i].code,g_pAbsCondition[i].setcode);
			scodetmp = (*g_pStockIO)[xh];
			if(!scodetmp) break;

			lvItem.iItem = i;
			lvItem.iImage = 0;
			if( (g_pAbsCondition[i].fHighPrice>COMPPREC && g_pAbsCondition[i].bDoLevin1>0) ||
				(g_pAbsCondition[i].fLowPrice>COMPPREC && g_pAbsCondition[i].bDoLevin2>0)
			   )
				lvItem.iImage = 2;
			lvItem.pszText = scodetmp->Code;
			m_AbsWarnList.InsertItem (&lvItem);
			m_AbsWarnList.SetItemData(i,xh);
			//名称
			m_AbsWarnList.SetItemText (i,1,scodetmp->Name);		
			//昨收
			if(scodetmp->Close>COMPPREC)
				tempStr.Format("%7.3f",scodetmp->Close);
			else tempStr.Format("-");
			m_AbsWarnList.SetItemText (i,2,tempStr);
			//上破价
			if(g_pAbsCondition[i].fHighPrice>COMPPREC)
				tempStr.Format("%7.3f",g_pAbsCondition[i].fHighPrice);
			else tempStr.Format("-");
			m_AbsWarnList.SetItemText (i,3,tempStr);
			//下破价
			if(g_pAbsCondition[i].fLowPrice>COMPPREC)
				tempStr.Format("%7.3f",g_pAbsCondition[i].fLowPrice);
			else tempStr.Format("-");

			m_AbsWarnList.SetItemText (i,4,tempStr);
			//换手
			if(g_pAbsCondition[i].TurnOverRate>COMPPREC && g_pAbsCondition[i].TurnOverRate<30000)
				tempStr.Format("%7.2f",g_pAbsCondition[i].TurnOverRate*0.01f);
			else tempStr.Format("-");

			m_AbsWarnList.SetItemText (i,5,tempStr);
			//ETF折溢率
			if(g_pAbsCondition[i].ETFAPRate>COMPPREC && g_pAbsCondition[i].ETFAPRate<10000)
				tempStr.Format("%7.2f",g_pAbsCondition[i].ETFAPRate*0.01f);
			else tempStr.Format("-");

			m_AbsWarnList.SetItemText (i,6,tempStr);
		}
	}
	else if(m_nListFlag==TJLIST)
	{
		GetDlgItem(IDC_ADDABS)->SetWindowText(_F("添加公式"));
		GetDlgItem(IDC_DELETEABS)->SetWindowText(_F("删除公式"));
		TINDEXINFO	* pTJIndex;
		char tmpStr[250];
		for(i=0, j=0;i<MAX_TJWARN;i++)
		{
			TJ_WARN *pTjWarn = &(g_aWarnTjEx[i]);
			if(pTjWarn->nTjIndexNo<0||strcmp(pTjWarn->acCode, "")==0)	continue;
			pTJIndex = g_pCalc->_CallDTopViewCalc_8(TJ_TYPE,pTjWarn->acCode);
			if(!pTJIndex) 
			{
				pTjWarn->nTjIndexNo = -1;
				pTjWarn->nPeriod = DAY_ST;
				continue;
			}
			pTjWarn->nTjIndexNo=pTJIndex->nIndexNo;
			sprintf(tmpStr,"%s %s",pTJIndex->acCode,pTJIndex->acName);
			lvItem.iItem = j;
			lvItem.iImage = 1;
			lvItem.pszText = tmpStr;
			m_AbsWarnList.InsertItem(&lvItem);
			m_AbsWarnList.SetItemData(j, i);
			if(pTjWarn->bUsed) m_AbsWarnList.SetCheck(j);
			int nParam = (*g_pCalc)[TJ_TYPE][pTjWarn->nTjIndexNo].nParaNum;
			CString sParam;
			char sOneParam[80];
			for(int k=0;k<nParam;k++)
			{
				sprintf(sOneParam, "%d", (int)pTjWarn->aTjParam[k]);
				sParam += (CString)(LPCSTR)sOneParam;
				if(k<nParam-1) sParam += (CString)(LPCSTR)", ";
			}
			m_AbsWarnList.SetItemText(j++, 1, (LPCSTR)sParam);
		}
	}
	m_AbsWarnList.SetRedraw(TRUE);
}

void CAbsWarnPage::OnOK() 
{
	UpdateData(TRUE);
	if(m_nListFlag==TJLIST)
	{
		int nIndex, nItem = m_AbsWarnList.GetItemCount();
		for(int i=0;i<nItem;i++)
		{
			nIndex = m_AbsWarnList.GetItemData(i);
			g_aWarnTjEx[nIndex].bUsed = m_AbsWarnList.GetCheck(i);
		}
	}
	CPropertyPage::OnOK();
}

#include "SelectGPDlg.h"
void CAbsWarnPage::OnAddabs() 
{
	if(m_nListFlag==GPLIST)
	{
		if(g_pAbsConditionNum >= MAX_ABSWARNMAXNUM)
		{
			TDX_MessageBox(m_hWnd,"预警股票数太多，不能再添加!",MB_OK|MB_ICONINFORMATION);
			return;
		}
		
		CSelectGPDlg dlg;
		dlg.bSingleSelect = FALSE;
		dlg.bHasDS = TRUE;
		if(dlg.DoModal() == IDOK)
		{
			BOOL bExist;
			MemStkInfo *tempStkInfo;
			BOOL bLastWarnOn = g_bTjWarnSwitch; 
			g_bTjWarnSwitch = FALSE;
			for(int j = 0;j < dlg.SelectedGPNum;j++)
			{
				tempStkInfo = (*g_pStockIO)[dlg.SelectedGPIndex[j]];
				if(!tempStkInfo) continue;
				bExist = FALSE;
				for(int i = 0;i < g_pAbsConditionNum;i++)//看是否已经存在这只股票
				{
					if(tempStkInfo->setcode==g_pAbsCondition[i].setcode && strcmp(tempStkInfo->Code,g_pAbsCondition[i].code)==0)
					{	bExist = TRUE;break; }
				}

				if(!bExist)
				{	
					memset(g_pAbsCondition+g_pAbsConditionNum,0,sizeof(PWARNINGCND));
					strcpy(g_pAbsCondition[g_pAbsConditionNum].code,tempStkInfo->Code);
					g_pAbsCondition[g_pAbsConditionNum].setcode = tempStkInfo->setcode;
					g_pAbsCondition[g_pAbsConditionNum].fLowPrice = 0.0f;
					g_pAbsCondition[g_pAbsConditionNum].fHighPrice = 0.0f;
					g_pAbsCondition[g_pAbsConditionNum].ETFAPRate = 0;
					g_pAbsCondition[g_pAbsConditionNum].TurnOverRate = 0;
					g_pAbsConditionNum++;
					
					if(g_pAbsConditionNum >= MAX_ABSWARNMAXNUM) 
					{
						CString tmpStr;
						tmpStr.Format("仅加入了前面%d只股票.",MAX_ABSWARNMAXNUM);
						TDX_MessageBox(m_hWnd,tmpStr,MB_OK|MB_ICONINFORMATION);
						break;
					}
				}
			}
			FillList();
			CurrentAbs = g_pAbsConditionNum - 1;
			m_AbsWarnList.SetItemState(CurrentAbs, LVIS_SELECTED,LVIS_SELECTED);
			m_AbsWarnList.SetItemState(CurrentAbs, LVIS_FOCUSED,LVIS_FOCUSED);
			if(g_OEM.bEmbedWTVersion && dlg.SelectedGPNum==1)
				OnModifyabs();
			g_bTjWarnSwitch = bLastWarnOn;
		}
	}
	else if(m_nListFlag==TJLIST)
	{
		int nFirstNon = -1;
		for(int i=MAX_TJWARN-1;nFirstNon==-1&&i>=0;i--)
		{
			if(g_aWarnTjEx[i].nTjIndexNo==-1)
				nFirstNon = i;
		}
		if(nFirstNon==-1)
		{
			TDX_MessageBox(m_hWnd,"预警公式条件太多，不能再添加!",MB_OK|MB_ICONINFORMATION);
			return;
		}

		CTjWarnConfDlg dlg;
		dlg.m_bModefied = FALSE;
		memset(&dlg.m_warnTjConf, 0, sizeof(TJ_WARN));
		dlg.m_warnTjConf.nPeriod = DAY_ST;
		dlg.m_warnTjConf.nTjIndexNo = -1;
		if(dlg.DoModal() == IDOK)
		{
			memcpy(&g_aWarnTjEx[nFirstNon], &dlg.m_warnTjConf, sizeof(TJ_WARN));
			g_aWarnTjEx[nFirstNon].bUsed = TRUE;
			FillList();
			CurrentAbs = nFirstNon;
			m_AbsWarnList.SetItemState(CurrentAbs, LVIS_SELECTED,LVIS_SELECTED);
			m_AbsWarnList.SetItemState(CurrentAbs, LVIS_FOCUSED,LVIS_FOCUSED);
		}
	}
}

#include "ModifyAbsDlg.h"
void CAbsWarnPage::OnModifyabs() 
{
	POSITION pos;
	pos = m_AbsWarnList.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{
		CurrentAbs = m_AbsWarnList.GetNextSelectedItem(pos);
		if(m_nListFlag==GPLIST)
		{
			ToZst(g_pAbsCondition[CurrentAbs].code,g_pAbsCondition[CurrentAbs].setcode,FALSE);
			CModifyAbsDlg dlg;
			dlg.m_fUpBreak = g_pAbsCondition[CurrentAbs].fHighPrice;
			dlg.m_fDownBreak = g_pAbsCondition[CurrentAbs].fLowPrice;
			dlg.m_nCurSel1 = g_pAbsCondition[CurrentAbs].bDoLevin1;
			dlg.m_nCurSel2 = g_pAbsCondition[CurrentAbs].bDoLevin2;
			dlg.m_fPriceDiff	= g_pAbsCondition[CurrentAbs].ETFAPRate*0.01;
			dlg.m_fChangeHand	= g_pAbsCondition[CurrentAbs].TurnOverRate*0.01;
			int xh = g_pStockIO->GetIndex(g_pAbsCondition[CurrentAbs].code,g_pAbsCondition[CurrentAbs].setcode);
			dlg.nStockNo = xh;
			if(dlg.DoModal() == IDOK)
			{
				g_pAbsCondition[CurrentAbs].fHighPrice = dlg.m_fUpBreak;
				g_pAbsCondition[CurrentAbs].fLowPrice = dlg.m_fDownBreak;
				g_pAbsCondition[CurrentAbs].bDoLevin1 = dlg.m_nCurSel1;
				g_pAbsCondition[CurrentAbs].bDoLevin2 = dlg.m_nCurSel2;
				g_pAbsCondition[CurrentAbs].ETFAPRate = (unsigned short)(dlg.m_fPriceDiff*100+0.5);
				g_pAbsCondition[CurrentAbs].TurnOverRate = (unsigned short)(dlg.m_fChangeHand*100+0.5);
			}
			CString tempStr;
			if(g_pAbsCondition[CurrentAbs].fHighPrice>COMPPREC)
				tempStr.Format("%7.3f",g_pAbsCondition[CurrentAbs].fHighPrice);
			else tempStr.Format("-");
			m_AbsWarnList.SetItemText (CurrentAbs,3,tempStr);
			if(g_pAbsCondition[CurrentAbs].fLowPrice>COMPPREC)
				tempStr.Format("%7.3f",g_pAbsCondition[CurrentAbs].fLowPrice);
			else tempStr.Format("-");
			m_AbsWarnList.SetItemText (CurrentAbs,4,tempStr);
			if(g_pAbsCondition[CurrentAbs].TurnOverRate>COMPPREC && g_pAbsCondition[CurrentAbs].TurnOverRate<30000)
				tempStr.Format("%7.2f",g_pAbsCondition[CurrentAbs].TurnOverRate*0.01f);
			else tempStr.Format("-");
			m_AbsWarnList.SetItemText (CurrentAbs,5,tempStr);
			if(g_pAbsCondition[CurrentAbs].ETFAPRate>COMPPREC && g_pAbsCondition[CurrentAbs].ETFAPRate<10000)
				tempStr.Format("%7.2f",g_pAbsCondition[CurrentAbs].ETFAPRate*0.01f);
			else tempStr.Format("-");
			m_AbsWarnList.SetItemText (CurrentAbs,6,tempStr);
		}
		else if(m_nListFlag==TJLIST)
		{
			ASSERT(CurrentAbs<=MAX_TJWARN);
			CTjWarnConfDlg dlg;
			dlg.m_bModefied = TRUE;
			int nTj = m_AbsWarnList.GetItemData(CurrentAbs);
			memcpy(&dlg.m_warnTjConf, &g_aWarnTjEx[nTj], sizeof(TJ_WARN));
			if(dlg.DoModal()==IDOK)
			{
				memcpy(&g_aWarnTjEx[nTj], &dlg.m_warnTjConf, sizeof(TJ_WARN));
				FillList();
			}
		}
	}
}

void CAbsWarnPage::OnDeleteabs() 
{
	POSITION pos;
	int nItem,i,j;
	pos = m_AbsWarnList.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{
		if(m_nListFlag==GPLIST)
		{
			while (pos) //先置删除标记-1
			{
				nItem = m_AbsWarnList.GetNextSelectedItem(pos);
				strcpy(g_pAbsCondition[nItem].code,"-1");
			}
			for(i = g_pAbsConditionNum - 1;i >= 0;i--)
			{
				if(strcmp(g_pAbsCondition[i].code,"-1") == 0)
				{
					for(j = i+1;j < g_pAbsConditionNum;j++)
						g_pAbsCondition[j-1] = g_pAbsCondition[j];
					g_pAbsConditionNum--;
				}
			}
		}
		else if(m_nListFlag == TJLIST)
		{
			while (pos)
			{
				nItem = m_AbsWarnList.GetNextSelectedItem(pos);
				int nTj = m_AbsWarnList.GetItemData(nItem);
				memset(&g_aWarnTjEx[nTj], 0, sizeof(TJ_WARN));
				g_aWarnTjEx[nTj].nTjIndexNo = -1;
				g_aWarnTjEx[nTj].nPeriod = DAY_ST;
			}
		}
		FillList();
	}
}

void CAbsWarnPage::OnZapabs() 
{
	if(m_nListFlag==GPLIST)
	{	
		if(g_pAbsConditionNum == 0) return;
		if(TDX_MessageBox(m_hWnd,"你确定清除所有预警股票吗？",MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
		{
			m_AbsWarnList.DeleteAllItems ();
			g_pAbsConditionNum = 0;
		}
	}
	else if(m_nListFlag==TJLIST)
	{
		if(m_AbsWarnList.GetItemCount()==0) return;
		if(TDX_MessageBox(m_hWnd,"你确定清除所有预警公式条件吗？",MB_OKCANCEL|MB_ICONQUESTION) == IDOK)
		{
			m_AbsWarnList.DeleteAllItems ();
			memset(&g_aWarnTjEx, 0, MAX_TJWARN*sizeof(TJ_WARN));
			for(int i=0;i<MAX_TJWARN;i++)
			{
				g_aWarnTjEx[i].nTjIndexNo = -1;
				g_aWarnTjEx[i].nPeriod = DAY_ST;
			}
		}
	}
}

void CAbsWarnPage::OnDblclkAbswarnList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnModifyabs();
	*pResult = 0;
}

BOOL CAbsWarnPage::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN)
	{	
		if(pMsg->wParam==VK_DELETE)
		{
			PostMessage(WM_COMMAND,IDC_DELETEABS);
			return TRUE;
		}
	}		
	return CPropertyPage::PreTranslateMessage(pMsg);
}

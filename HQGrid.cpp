#include "stdafx.h"
#include "HQGrid.h"
#include "TdxW.h"
#include "MainFrm.h"
#include "GridView.h"
#include "ComView.h"
#include "ComView2.h"
#include "TxtView.h"
#include "BatchDlg.h"
/////////////////////////////////////////////////////////////////////////////

HQGrid::HQGrid(LPCOMINFO pComInfo):Grid(2,pComInfo?pComInfo->nMoreFlag:TRUE,pComInfo)
{
	m_nGPCode=0;
	m_LoadFyOK=TRUE;

	m_bInfoConnect = FALSE;
	m_bAutoPage = FALSE;

	m_nColNum = ABGP_COL_NUM;
	m_lpnCol = g_aABGPIdx;

	m_nCurPic	= 1;
	m_bRefreshDraw = FALSE;

	SetPartMode(g_GH.NoVScroll);

	m_bStartDrag = FALSE;
	m_bDragging = FALSE;
	m_DragPoint.x=m_DragPoint.y=0;
	RightClickWhichCol = 0;

	m_bCanMoveGrid = m_bMovingGrid = FALSE;
	m_nMoveItem = -1;

	m_fpItemWidth = new float[TOTAL_COL];
	memcpy(m_fpItemWidth,g_aAllItemWidth,TOTAL_COL*sizeof(float));
	memset(Sort_lpnUsedStkIndex,0,sizeof(short)*MAXGPNUM);
	memset(Fy_nTag,0,sizeof(short)*MAXGPNUM);
	memset(m_FYCol,-1,sizeof(long)*TOTAL_COL);

	memset(&m_LSetInfo, 0, sizeof(LinkedSetInfo));
	
	for(int i=0;i<2*TOTAL_COL;i++)
	{
		m_fAver[i]	= 0.0f;
		m_fSum[i]	= 0.0f;
		m_fMax[i]	= -99999.99f;
		m_fMin[i]	= 99999.99f;
		m_fStd[i]	= 0.0f;
		m_fQZ[i]	= 1;
		m_fStdQZ[i] = 0;
	}
	memset(m_nStatistFlag, 0, STAT_LASTNULL*sizeof(short));

	//
	m_bDDEStatus = FALSE;
	m_bAJHStatus = FALSE;
	m_bJSZJStatus = FALSE;
	m_bSigStatus = FALSE;
	m_nConStatus = -1;
	m_nConFiltCount = 20;

	//风云榜
	InitDynaMap();
}

HQGrid::~HQGrid()
{	
	FirstGPType = m_WhichGPType;	//保存最近使用的市场类型变量
	FirstBlockFlag = m_nBlockFlag;

	TDEL(m_fpItemWidth);
}

void HQGrid::ReSetRefreshTime()
{
	m_pView->KillTimer(1);
	m_pView->SetTimer(1,GetListRefreshTime(),NULL);
	//用于定时刷新行情列表的定时器,缺省是3秒,期货是0.8秒
	m_pView->KillTimer(3);
	m_pView->SetTimer(3,(m_nGridType==QH_TYPE)?800:3000,NULL);
	//用于A+H股对比的刷新,A+H行情联动,20秒刷新一次
	m_pView->KillTimer(4);
	if(m_nGridType==A2H_TYPE) 
		m_pView->SetTimer(4,DDEHQ_SECOND*1000,NULL);
	//用于DDE的刷新,20秒刷新一次
	m_pView->KillTimer(5);
	if(m_nGridType==DDE_TYPE) 
		m_pView->SetTimer(5,STATHQ_SECOND*1000,NULL);
}

static BOOL s_nLastGridType=ABGP_TYPE;
static int  s_nLastWhichGPType=FirstGPType,s_nLastBlockFlag=FirstBlockFlag;
void HQGrid::OnlySetDomain(int nBlockFlag,int WhichType)
{
	if(nBlockFlag==-1 && WhichType==-1)
	{
		WhichType=s_nLastWhichGPType;
		nBlockFlag=s_nLastBlockFlag;
	}
	Grid::SetDomain(nBlockFlag,WhichType);
	//选股报表的过滤
	FiltConFlag();
	
	if(m_WhichGPType>=FY_START&&m_WhichGPType<FY_START+MAX_FY_GROUP)	//风云排行
	{
		m_nFixedLeadCol = 2;
		m_nGridType = FY_TYPE;
		if(nBlockFlag<0) nBlockFlag=0;
		((CGridView*)m_pView)->SetFYTitle(_F(g_vfyGroup[m_WhichGPType-FY_START].strShowTitle[nBlockFlag]));
		InitFYInfo();
		InsertSwitch(-1);
		if(m_bHasSwitch)
			SetSwitchBarSel(m_WhichGPType,m_nBlockFlag);
		m_pView->SendMessage(UM_GETMAINID,0); //必须改变当前的MainID,以免返回的数据错误
	}
	else
	{
		m_nFixedLeadCol = 2;
		if(m_bDDEStatus)
			m_nGridType = DDE_TYPE;
		else if(m_bAJHStatus)
			m_nGridType = A2H_TYPE;
		else if(m_bJSZJStatus)
			m_nGridType = JSZJ_TYPE;
		else if(m_bSigStatus)
			m_nGridType = SIGNAL_TYPE;
		else if(!g_bHasDS)
			m_nGridType = ABGP_TYPE;
		else
		{
			if(m_nBlockFlag==-1)
			{
				if(m_WhichGPType>=DS_START&&m_WhichGPType<FY_START) 
					m_nGridType = g_pDSMarket[m_WhichGPType-DS_START].BigMarket;
				else if(BeBlock(m_WhichGPType)) 
					m_nGridType = TOTAL_TYPE;
				else 
					m_nGridType = ABGP_TYPE;
			}
			else if(m_nBlockFlag>=HKHY_START)
				m_nGridType = HKGP_TYPE;
			else
				m_nGridType = TOTAL_TYPE;
			if(m_nGridType==TOTAL_TYPE)
			{
				for(int i=0;i<nTotalRow;i++)
					if(g_pStockIO->BeDSStock(lpnUsedStkIndex[i]))
						break;
				if(i>=nTotalRow)
					m_nGridType = ABGP_TYPE;
			}
		}
		s_nLastWhichGPType=m_WhichGPType;
		s_nLastBlockFlag=m_nBlockFlag;
		if(s_nLastGridType==FY_TYPE||s_nLastGridType==A2H_TYPE)
		{
			InsertSwitch(0);
			if(m_bHasSwitch)
				SetSwitchBarSel(WhichType,nBlockFlag);
		}
		int nSwitchSel = 0;
		if(m_bDDEStatus) 
			nSwitchSel = 1;
		else if(m_bAJHStatus && !g_bSupportDDE) 
			nSwitchSel = 1;
		else if(m_bAJHStatus) 
			nSwitchSel = 2;
		if(m_bAJHStatus)
		{
			InsertSwitch(0);
			if(m_bHasSwitch)
				SetSwitchBarSel(WhichType,nBlockFlag);
		}
		m_pView->SendMessage(UM_RESETFYSWITCHBAR,nSwitchSel,0);
	}
	if(s_nLastGridType!=m_nGridType)
	{
		s_nLastGridType=m_nGridType;
		ReSetRefreshTime();
	}	
	switch(m_nGridType)
	{
	case FY_TYPE:
		m_nColNum = m_vBaseHeads.size();
		m_lpnCol = m_FYCol;
		break;
	case DDE_TYPE:
		m_nColNum = DDE_COL_NUM;
		m_lpnCol = g_aDDEIdx;
		m_fpItemWidth[ZQDM] = 7;
		m_fpItemWidth[ZQJC] = g_bLongDSName?16:10;
		break;
	case ABGP_TYPE:
		m_nColNum = ABGP_COL_NUM;
		m_lpnCol = g_aABGPIdx;
		m_fpItemWidth[ZQDM] = 7;
		m_fpItemWidth[ZQJC] = 10;
		if(m_nInCom2) 
			nCurSortIndex = 2;
		break;
	case HKGP_TYPE:
		m_nColNum = HKGP_COL_NUM; 
		m_lpnCol = g_aHKGPIdx;
		m_fpItemWidth[ZQDM] = 7;
		m_fpItemWidth[ZQJC] = g_bLongDSName?16:10;
		break;
	case QH_TYPE:
		m_nColNum = QH_COL_NUM; 
		m_lpnCol = g_aQHIdx;
		m_fpItemWidth[ZQDM] = 7;
		m_fpItemWidth[ZQJC] = 10;
		break;
	case FE_TYPE:
		m_nColNum = FE_COL_NUM; 
		m_lpnCol = g_aFEIdx;
		m_fpItemWidth[ZQDM] = 7;
		m_fpItemWidth[ZQJC] = 10;
		break;
	case FOREGP_TYPE:
		m_nColNum = FOREGP_COL_NUM;
		m_lpnCol = g_aFOREGPIdx;
		m_fpItemWidth[ZQDM] = 7;
		m_fpItemWidth[ZQJC] = 10;
		break;
	case IDX_TYPE:
		m_nColNum = IDX_COL_NUM;
		m_lpnCol = g_aIDXIdx;
		m_fpItemWidth[ZQDM] = 7;
		m_fpItemWidth[ZQJC] = g_bLongDSName?16:10;
		break;
	case A2H_TYPE:
		m_nColNum = A2H_COL_NUM;
		m_lpnCol = g_aA2HIdx;
		m_fpItemWidth[ZQDM] = 7;
		m_fpItemWidth[ZQJC] = 10;
		break;
	case JSZJ_TYPE:
		m_nColNum = JSZJ_COL_NUM;
		m_lpnCol = g_aJSZJIdx;
		m_fpItemWidth[ZQDM] = 7;
		m_fpItemWidth[ZQJC] = 10;
		nCurSortIndex = 4;
		break;
	case SIGNAL_TYPE:
		m_nColNum = SIGNAL_COL_NUM;
		m_lpnCol = g_aSignalIdx;
		m_fpItemWidth[ZQDM] = 7;
		m_fpItemWidth[ZQJC] = 10;
		nCurSortIndex = 2;
		break;
	default:
		m_nColNum = ABGP_COL_NUM;
		m_lpnCol = g_aABGPIdx;
		m_fpItemWidth[ZQDM] = 7;
		m_fpItemWidth[ZQJC] = g_bLongDSName?16:10;
		if(m_nInCom2) 
			nCurSortIndex = 2;
		break;
	}
	memcpy(Sort_lpnUsedStkIndex,lpnUsedStkIndex,nTotalRow*sizeof(short));//填充Sort_lpnUsedStkIndex
	for(int i=0;i<nTotalRow;i++) Fy_nTag[i]=i;
	memset(&LastNow,0,MAXGPNUM*sizeof(float)); //将LastNow清空
	memset(&LastBuyp,0,MAXGPNUM*sizeof(float)); //
	memset(&LastSellp,0,MAXGPNUM*sizeof(float)); //
	memset(&LastVol,0,MAXGPNUM*sizeof(long)); //
	
	//将Title赋值
	m_strTitle="";
	if(m_WhichGPType>=FY_START&&m_WhichGPType<FY_START+MAX_FY_GROUP)
		m_strTitle = g_vfyGroup[m_WhichGPType-FY_START].strGroupName;
	else if(m_WhichGPType>=DS_START&&m_WhichGPType<FY_START)
		m_strTitle = DSGetMarketNameFromXH(m_WhichGPType-DS_START);
	else if(m_nBlockFlag == -1)				
		m_strTitle = (CString)GPType[m_WhichGPType];
	else
		m_strTitle = GetBlockCaption(m_nBlockFlag);
	m_pView->SendMessage(UM_GRIDSETTITLE); //设置父窗口标题
}

void HQGrid::ResetRowCol()
{
	nCurRow = nStartRow = nStartCol = 0;
}

//设置行情表的股票市场类别
void HQGrid::SetDomain(int nBlockFlag,int WhichType,BOOL bSort,int SortType,BOOL AscendSort)
{
	OnlySetDomain(nBlockFlag,WhichType);
	InitGrid(bSort,SortType,AscendSort); //调用报表初始化，开始重画报表	
}

void HQGrid::InsertSwitch(int nSCType)
{
	pSwitchBar->ClearAllLabel();

	if(m_nGridType==FY_TYPE)
	{
		m_bFixedGrid=FALSE;
		if(m_WhichGPType>=FY_START && m_WhichGPType<FY_START+MAX_FY_GROUP)
		{
			int i,iNo=0;
			int nItemNum = g_vfyGroup[m_WhichGPType-FY_START].strItemName.size();
			if(nItemNum>1) 
			{
				if(nItemNum>5)
				{
					pSwitchBar->m_bhasFLBtn=TRUE;
					pSwitchBar->InsertSwitchLabel(iNo++,_F("分类"),WM_COMMAND,IDR_LONELY_MENU,-m_WhichGPType,TRUE);
				}
				for(i=0;i<nItemNum;i++)
				{
					pSwitchBar->InsertSwitchLabel(iNo,_F(g_vfyGroup[m_WhichGPType-FY_START].strItemName[i]),WM_COMMAND,
						ID_RIGHT_FYBASE1+(m_WhichGPType-FY_START)*MAX_FYNUM_IN_GROUP+i,0,FALSE);
					COLORREF cor;
					if(GetSpecialTextColor(g_vfyGroup[m_WhichGPType-FY_START].itemAttribute[i],cor))
						pSwitchBar->SetSpecialTextColor(iNo,cor);
					iNo++;
				}
			}
		}
	}
	else if(m_nGridType==A2H_TYPE) 
	{
		m_bFixedGrid = TRUE;
		pSwitchBar->InsertSwitchLabel(0,_F("A+H对照"),WM_COMMAND,0,0,FALSE);
		m_WhichGPType = TYPE_AJH;
		m_nBlockFlag = -1;	
	}
	else
	{
		m_bFixedGrid = TRUE;
		switch(nSCType+HQGRID_ALL)
		{
		case HQGRID_AG:
			pSwitchBar->InsertSwitchLabel(0,_F("Ａ股"),WM_COMMAND,ID_RIGHT_TYPE6,0,FALSE);
			m_WhichGPType = TYPE_AG;
			m_nBlockFlag = -1;		
			break;
		case HQGRID_BG:
			pSwitchBar->InsertSwitchLabel(0,_F("Ｂ股"),WM_COMMAND,ID_RIGHT_TYPE7,0,FALSE);
			m_WhichGPType = TYPE_BG;
			m_nBlockFlag = -1;
			break;
		case HQGRID_QZ:
			pSwitchBar->InsertSwitchLabel(0,_F("权证"),WM_COMMAND,ID_RIGHT_TYPE13,0,FALSE);
			m_WhichGPType = TYPE_QZ;
			m_nBlockFlag = -1;
			break;
		case HQGRID_ZQ:
			pSwitchBar->InsertSwitchLabel(0,_F("债券"),WM_COMMAND,ID_RIGHT_TYPE8,0,FALSE);
			m_WhichGPType = TYPE_ZQ;
			m_nBlockFlag = -1;
			break;
		case HQGRID_JJ:
			pSwitchBar->InsertSwitchLabel(0,_F("基金"),WM_COMMAND,ID_RIGHT_TYPE9,0,FALSE);
			m_WhichGPType = TYPE_JJ;
			m_nBlockFlag = -1;
			break;
		case HQGRID_SB:
			pSwitchBar->InsertSwitchLabel(0,_F("三板"),WM_COMMAND,ID_RIGHT_TYPE16,0,FALSE);
			m_WhichGPType = TYPE_SB;
			m_nBlockFlag = -1;
			break;
		case HQGRID_SPEC:
			pSwitchBar->InsertSwitchLabel(0,_F("板指"),WM_COMMAND,ID_RIGHT_TYPE17,0,FALSE);
			m_WhichGPType = TYPE_SPECINDEX;
			m_nBlockFlag = -1;
			break;
		case HQGRID_ZX:
			pSwitchBar->InsertSwitchLabel(0,_F("自选"),WM_COMMAND,ID_RIGHT_TYPE15,0,FALSE);
			m_WhichGPType = TYPE_ZXG;
			m_nBlockFlag = -1;
			break;
			
		case HQGRID_DY:
			pSwitchBar->InsertSwitchLabel(0,_F("地区"),WM_COMMAND,IDR_LONELY_MENU,16,TRUE);
			m_WhichGPType = -1;
			m_nBlockFlag = DY_START;
			break;
		case HQGRID_HY:
			pSwitchBar->InsertSwitchLabel(0,_F("行业"),WM_COMMAND,IDR_LONELY_MENU,17,TRUE);
			m_WhichGPType = -1;
			m_nBlockFlag = HY_START;
			break;
		case HQGRID_GN:
			pSwitchBar->InsertSwitchLabel(0,_F("概念"),WM_COMMAND,IDR_LONELY_MENU,-1,TRUE);//-1表示是概念板块
			m_WhichGPType = -1;
			m_nBlockFlag = CON_START;
			break;
		case HQGRID_ZH:
			pSwitchBar->InsertSwitchLabel(0,_F("组合"),WM_COMMAND,IDR_LONELY_MENU,-2,TRUE);//-2表示是组合板块
			m_WhichGPType = -1;
			m_nBlockFlag = ZHB_START;
			break;
		case HQGRID_ZD:
			pSwitchBar->InsertSwitchLabel(0,_F("自定"),WM_COMMAND,IDR_LONELY_MENU,-3,TRUE);//-3表示是的自定板块
			break;
		case HQGRID_ZJHHY:
			pSwitchBar->InsertSwitchLabel(0,_F("证监会行业"),WM_COMMAND,IDR_LONELY_MENU,-5,TRUE);//-5表示是证监会行业板块
			m_WhichGPType = -1;
			m_nBlockFlag = ZJHHY_START;
			break;			
		case HQGRID_SHAG:
			pSwitchBar->InsertSwitchLabel(0,_F("沪Ａ"),WM_COMMAND,ID_RIGHT_TYPE0,0,FALSE);
			m_WhichGPType = TYPE_SHAG;
			m_nBlockFlag = -1;
			break;
		case HQGRID_SZAG:
			pSwitchBar->InsertSwitchLabel(0,_F("深Ａ"),WM_COMMAND,ID_RIGHT_TYPE2,0,FALSE);
			m_WhichGPType = TYPE_SZAG;
			m_nBlockFlag = -1;
			break;
		case HQGRID_ZXB:
			pSwitchBar->InsertSwitchLabel(0,_F("中小"),WM_COMMAND,ID_RIGHT_TYPE12,0,FALSE);
			m_WhichGPType = TYPE_ZXB;
			m_nBlockFlag = -1;
			break;
		case HQGRID_CYB:
			pSwitchBar->InsertSwitchLabel(0,_F("创业"),WM_COMMAND,ID_RIGHT_TYPE14,0,FALSE);
			m_WhichGPType = TYPE_CYB;
			m_nBlockFlag = -1;
			break;
		case HQGRID_ALL:
			{
				int iNo=0;
				CString tmpStr;
				m_bFixedGrid = FALSE;
				if(g_OEM.bOutStandZXGHY && !TestInComView2())
				{
					pSwitchBar->m_bhasDPBtn=TRUE;
					pSwitchBar->InsertSwitchLabel(iNo++,_F("大盘"),WM_COMMAND,IDR_LONELY_MENU,2,TRUE);
				}
				pSwitchBar->m_bhasFLBtn=FALSE;
				if(!m_nInCom2)
				{
					for(int i=0;i<g_CustomGridCat.size();i++)
					{
						if(g_CustomGridCat[i].FlagType==DSOPT_ITEM && !g_bHasDS) 
							continue;
						if(g_CustomGridCat[i].wParam==ID_RIGHT_TYPE20 && (g_pLcIdxMng==NULL || g_pLcIdxMng->GetLcNum()==0))
							continue;
						tmpStr=g_CustomGridCat[i].lpTitle;
						pSwitchBar->InsertSwitchLabel(iNo,_F(tmpStr),g_CustomGridCat[i].msg,g_CustomGridCat[i].wParam,g_CustomGridCat[i].lParam,g_CustomGridCat[i].bMenu);
						if(g_CustomGridCat[i].FlagType==DSOPT_ITEM && DSOnLine)
							pSwitchBar->SetSpecialTextColor(iNo,RGB(192,0,0));
						else if(g_OEM.bOutStandZXGHY && g_CustomGridCat[i].wParam==ID_RIGHT_TYPE15)
							pSwitchBar->SetSpecialTextColor(iNo,RGB(192,0,0));
						
						iNo++;
					}
				}
				else
				{
					for(int i=0;i<sizeof(g_SysDefGridCat_Com2)/sizeof(g_SysDefGridCat_Com2[0]);i++)
					{
						if(g_SysDefGridCat_Com2[i].wParam==ID_RIGHT_TYPE20 && (g_pLcIdxMng==NULL || g_pLcIdxMng->GetLcNum()==0))
							continue;
						tmpStr=g_SysDefGridCat_Com2[i].lpTitle;
						pSwitchBar->InsertSwitchLabel(iNo,_F(tmpStr),g_SysDefGridCat_Com2[i].msg,g_SysDefGridCat_Com2[i].wParam,g_SysDefGridCat_Com2[i].lParam,g_SysDefGridCat_Com2[i].bMenu);
						if(g_SysDefGridCat_Com2[i].FlagType==DSOPT_ITEM && DSOnLine)
							pSwitchBar->SetSpecialTextColor(iNo,RGB(192,0,0));
						else if(g_OEM.bOutStandZXGHY && g_SysDefGridCat_Com2[i].wParam==ID_RIGHT_TYPE15)
							pSwitchBar->SetSpecialTextColor(iNo,RGB(192,0,0));
						
						iNo++;
					}
				}
				
			}
			break;
		default:
			{
				int nDSType = nSCType-32;
				int nDSMarketID = g_GuiMarketSet[nDSType].MarketSet[0];
				if(g_GuiMarketSet[nDSType].MarketSet[1]==0)				//只有一个
					pSwitchBar->InsertSwitchLabel(0,_F(g_GuiMarketSet[nDSType].BigsetName),WM_COMMAND,IDM_DS1+nDSMarketID,0,FALSE);
				else 
					pSwitchBar->InsertSwitchLabel(0,_F(g_GuiMarketSet[nDSType].BigsetName),WM_COMMAND,IDR_LONELY_MENU,-6-nDSType,TRUE);
				if(DSOnLine)
					pSwitchBar->SetSpecialTextColor(0,RGB(192,0,0));
				int nDSMktXh = DSGetMarketXH(nDSMarketID);
				m_WhichGPType = DS_START+nDSMktXh;
				m_nBlockFlag = -1;
			}
			break;
		}
	}
	SetRect(CRect(0,0,0,0), TRUE);
}

void HQGrid::InitGrid(BOOL bSort,int SortType,BOOL AscendSort)
{
	bDrawBlue = FALSE;
	if(nCurRow<0 || nCurRow>=nTotalRow)
		nCurRow = nStartRow = 0;
	nTotalCol = m_nColNum-m_nFixedLeadCol; //不包括第1,2列
	bAscendSort = AscendSort;
	if(bSort)  //如果是按涨幅排名
	{
		for(nCurSortIndex=0;nCurSortIndex<m_nColNum && m_lpnCol[nCurSortIndex]!=SortType;++nCurSortIndex);
		bAscendSort = AscendSort;
	}
	else  if(!g_GH.bLockHQSort || m_WhichGPType>=FY_START)
	{
		if(nCurSortIndex<0)
		{
			nCurSortIndex = 0;
			bAscendSort = FALSE;
		}
	}
	if(nCurSortIndex!=0 && ((!OnLine&&m_WhichGPType<DS_START) || (!DSOnLine&&m_WhichGPType>=DS_START&&m_WhichGPType<FY_START) || (m_WhichGPType>=FY_START&&m_WhichGPType<FY_START+MAX_FY_GROUP)) )
		Sort();
	//如果是A+H画面,定时请求所有行情
	if(m_bAJHStatus && DSOnLine)
		m_pView->SendMessage(WM_TIMER, 4);
	//如果是DDE画面,定时请求所有行情
	if(m_bDDEStatus && OnLine)
		m_pView->SendMessage(WM_TIMER, 5);
	//
	DrawFlag = DRAW_ALL;
	ProcessMsg(WM_PAINT);
	RefreshScrollBar();	//重新组织滚动条
}

int HQGrid::GetItemWidth(int ColID)
{
	if(ColID>=TOTAL_COL)
	{
		if(m_vBaseHeads.size()==0)
			return DEF_ITEM_WIDTH*FontSize.cx;
		if(m_vBaseHeads[ColID%TOTAL_COL].width>=0)
			return m_vBaseHeads[ColID%TOTAL_COL].width*FontSize.cx;
		return DEF_ITEM_WIDTH*FontSize.cx;
	}
	else
	{
		if(!m_fpItemWidth || ColID<0) return 0;
		return(m_fpItemWidth[ColID]*FontSize.cx+4);
	}
}

LPSTR HQGrid::GetItemTitle(int ColID)
{
	if(ColID>=TOTAL_COL)
	{
		if(m_vBaseHeads.size()==0)
			return _F("");
		return _F(m_vBaseHeads[ColID%TOTAL_COL].strTitle);
	}
	if(m_nGridType==QH_TYPE && ColID==SYL)
		return (_F("持仓量"));
	if(ColID<0) return _F("NULL");
	return _F(g_lpszAllItemName[ColID]);
}

HEAD_DATA_TYPE HQGrid::GetItemDataType(int ColID)
{
	if(ColID>=TOTAL_COL)
	{
		if(m_vBaseHeads.size()==0)
			return TYPE_FLOAT;
		return m_vBaseHeads[ColID%TOTAL_COL].dataType;
	}
	if(ColID==ZQDM || ColID==ZQJC)
		return TYPE_STRING;
	return TYPE_FLOAT;
}

//主画图函数
void HQGrid::DrawRemoteGrid(CDC *pDC)
{
	if(DrawFlag==DRAW_ALL)		//读数据
	{
		if( ((OnLine&&m_WhichGPType<DS_START)||(DSOnLine&&m_WhichGPType>=DS_START&&m_WhichGPType<FY_START)||(OnLine&&m_WhichGPType>=FY_START&&m_WhichGPType<FY_START+MAX_FY_GROUP)) 
			&& theViewManager.pActiveView==m_pView)
		{
			DrawGrid(pDC,FALSE);
			GetData();
		}
		else
			DrawGrid(pDC,TRUE);
	}
	else if(DrawFlag==DRAW_FRAME)
		DrawGrid(pDC,FALSE);
	else if(m_nCurPic == 1)		//仅画框架
		DrawGrid(pDC,FALSE);
	else if(m_nCurPic == 2)		//真正画
		DrawGrid(pDC,TRUE);
}

void HQGrid::GetSomeSize(CDC *pDC)
{
	FontSize = pDC->GetTextExtent("8");
	nSigTitleHeight = FontSize.cy+3;
	nSigCellHeight = 1.382f*FontSize.cy+6;
	if(g_GH.bBigStatusBar) nSigTitleHeight+=1;
	nTitleHeight0 = nTitleHeight = nSigTitleHeight; //得到标题栏高度
	nCellHeight =  nSigCellHeight;	//得到方格的高度
	nCellHeadWidth = 2.5*FontSize.cx*2;
	if(g_GH.NoXHCol) 
		nCellHeadWidth=0;
	int nHZRow = GetStaticItemCount();
	if(nHZRow>0) nTitleHeight += nHZRow*nCellHeight;
}

void HQGrid::DrawGrid(CDC *pDC,BOOL bRealDraw)
{
	GetDrawRect(m_DRect, m_bHasSwitch==FALSE?-1:0);
	if(m_DRect.Height() < 40) return;
	if(!m_LoadFyOK && m_nGridType == FY_TYPE)
	{
		pDC->FillSolidRect(m_DRect,VipColor.BACKCOLOR);
		return;
	}
	m_bInfoConnect = FALSE; //是否连上了资讯

	int i,j;

	CREATE_GRIDMEMDC
	GetSomeSize(&memDC);

	//画栏目行,在其中初化始一些变量:列数nColNum
//	CFont *pOldFont = g_d.SelectFont(&memDC, INFO_FONT);
	DrawTitle(&memDC);
//	g_d.RestoreFont(&memDC, pOldFont);
	//调节RowNum和ColNum
	AdjustRowColNum();
	//开始画线
	int tempWidth,tempLeft;
	tempLeft=GetCellLeft(m_nFixedLeadCol+nEndCol,tempWidth);

	for(i = 0;i <= GetStaticItemCount();i++)	//画汇总行线
	{
		g_d.DrawLine(&memDC,0,nTitleHeight0+i*nCellHeight-1,
				(tempColFlag==TRUE)?(tempLeft+tempWidth):(m_DRect.Width()),
				nTitleHeight0+i*nCellHeight-1);
	}
	/*
	for(i = 0;i <= nRowNum;i++)	//画行线
	{
		if(i%2==0) continue;
		g_d.Bar(&memDC,0,nTitleHeight+i*nCellHeight,m_DRect.right,nTitleHeight+(i+1)*nCellHeight,GridColor.Grid2GridColor);
	}
	*/
	/*
	for(i = 0;i <= nRowNum;i++)	//画行线
	{
		g_d.DrawLine(&memDC,0,nTitleHeight+i*nCellHeight-1,
				(tempColFlag==TRUE)?(tempLeft+tempWidth):(m_DRect.Width()),
				nTitleHeight+i*nCellHeight-1);
	}
	*/
	
//	int xtmp=nCellHeadWidth ;
//	int ytmp=0;
//	for(j=0;j<m_nFixedLeadCol+nColNum;j++) //画列线
//	{			
//		g_d.DrawLine(&memDC,xtmp-1,ytmp,xtmp-1,(tempRowFlag==TRUE)?(nRowNum*nCellHeight+nTitleHeight):m_DRect.Height());//紧跟上个循环
//		if(j<m_nFixedLeadCol)
//			xtmp+=GetItemWidth(m_lpnCol[j]);
//		else
//			xtmp+=GetItemWidth(m_lpnCol[j+nStartCol]);
//	}
//	g_d.DrawLine(&memDC,xtmp-1,ytmp,xtmp-1,(tempRowFlag==TRUE)?(nRowNum*nCellHeight+nTitleHeight):m_DRect.Height());//紧跟上个循环,画到顶上

	MemStkInfo *zsStkInfo,*pInfo;
	CurrStockData zsCurrStockData={0},hqtmp={0};
	EXCURRDATA hqtmp_ex={0};
	if(bRealDraw||m_nGridType==FY_TYPE||m_nGridType==DDE_TYPE||m_nGridType==A2H_TYPE)
	{
		//计算出m_SzzsZaf和m_ShzsZaf;
		zsStkInfo = g_pStockIO->Get_MemStkInfo ( SZ_DPCODE,SZ );
		g_pStockIO->ReadCurrStockData ( zsStkInfo, &zsCurrStockData );
		m_SzzsZaf = 0.0;
		if ( zsCurrStockData.Close > COMPPREC && zsCurrStockData.Now > COMPPREC )
			m_SzzsZaf = ( zsCurrStockData.Now - zsCurrStockData.Close ) / zsCurrStockData.Close;
		zsStkInfo = g_pStockIO->Get_MemStkInfo ( SH_DPCODE,SH );
		g_pStockIO->ReadCurrStockData ( zsStkInfo, &zsCurrStockData );
		m_ShzsZaf = 0.0;
		if ( zsCurrStockData.Close > COMPPREC && zsCurrStockData.Now > COMPPREC )
			m_ShzsZaf = ( zsCurrStockData.Now - zsCurrStockData.Close ) / zsCurrStockData.Close;
		//写CELL内容
		if(m_bWriteAllHQ) //导出全部时
		{
			for(j = 0;j < nTotalRow;j++)
			{
				pInfo = NULL;
				if(lpnUsedStkIndex[j]>=0)
				{
					pInfo = (*g_pStockIO)[lpnUsedStkIndex[j]];
					if(!pInfo) continue;
					g_pStockIO->ReadCurrStockData (pInfo,&hqtmp,NULL,&hqtmp_ex);
					
					MemStkInfo *pHKStock = NULL;
					if(m_bAJHStatus)
					{
						pHKStock = GetLinkedHKStock(pInfo);
						
						CurrStockData hqtmp_hk={0};
						EXCURRDATA hqtmp_ex_hk={0}; 
						if(pHKStock)
							g_pStockIO->ReadCurrStockData (pHKStock,&hqtmp_hk,NULL,&hqtmp_ex_hk);
						for(i=0;i<tempColNum+m_nFixedLeadCol;i++)
						{
							if(i<m_nFixedLeadCol)
								DrawA2HItem(&memDC,j,i,2,pHKStock,&hqtmp_hk,&hqtmp_ex_hk,pInfo,&hqtmp,&hqtmp_ex);
							else
								DrawA2HItem(&memDC,j,i+nStartCol,GetXSFlag(pInfo),pHKStock,&hqtmp_hk,&hqtmp_ex_hk,pInfo,&hqtmp,&hqtmp_ex);
						}
					}
					else
					{
						for(i=0;i<tempColNum+m_nFixedLeadCol;i++)
						{
							if(i<m_nFixedLeadCol)
								DrawItem(&memDC,j,i,2,pInfo,&hqtmp,&hqtmp_ex);
							else
								DrawItem(&memDC,j,i+nStartCol,GetXSFlag(pInfo),pInfo,&hqtmp,&hqtmp_ex);
						}
					}
				}
				else
				{
					for(i=0;i<tempColNum+m_nFixedLeadCol;i++)
					{
						if(i<m_nFixedLeadCol)
							DrawItem(&memDC,j,i,2,pInfo,&hqtmp,&hqtmp_ex);
						else
							DrawItem(&memDC,j,i+nStartCol,GetXSFlag(pInfo),pInfo,&hqtmp,&hqtmp_ex);
					}
				}
				if(m_pWriteFile)
				{
					m_pWriteFile->WriteString(m_WriteStr+"\n");
					m_WriteStr = "";
				}
			}
		}
		else
		{
			for(j = 0;j < tempRowNum;j++)
			{
				pInfo = NULL;
				if(lpnUsedStkIndex[j]>=0)
				{
					pInfo = (*g_pStockIO)[lpnUsedStkIndex[nStartRow+j]];
					if(!pInfo) continue;
					g_pStockIO->ReadCurrStockData (pInfo,&hqtmp,NULL,&hqtmp_ex);

					MemStkInfo *pHKStock = NULL;
					if(m_bAJHStatus)
					{
						pHKStock = GetLinkedHKStock(pInfo);
						
						CurrStockData hqtmp_hk={0};
						EXCURRDATA hqtmp_ex_hk={0}; 
						if(pHKStock)
							g_pStockIO->ReadCurrStockData (pHKStock,&hqtmp_hk,NULL,&hqtmp_ex_hk);
						for(i=0;i<tempColNum+m_nFixedLeadCol;i++)
						{
							if(i<m_nFixedLeadCol)
								DrawA2HItem(&memDC,j,i,2,pHKStock,&hqtmp_hk,&hqtmp_ex_hk,pInfo,&hqtmp,&hqtmp_ex);
							else
								DrawA2HItem(&memDC,j,i+nStartCol,GetXSFlag(pInfo),pHKStock,&hqtmp_hk,&hqtmp_ex_hk,pInfo,&hqtmp,&hqtmp_ex);
						}
					}
					else
					{
						for(i=0;i<tempColNum+m_nFixedLeadCol;i++)
						{
							if(i<m_nFixedLeadCol)
								DrawItem(&memDC,j,i,2,pInfo,&hqtmp,&hqtmp_ex);
							else
								DrawItem(&memDC,j,i+nStartCol,GetXSFlag(pInfo),pInfo,&hqtmp,&hqtmp_ex);
						}
					}
				}
				else
				{
					for(i=0;i<tempColNum+m_nFixedLeadCol;i++)
					{
						if(i<m_nFixedLeadCol)
							DrawItem(&memDC,j,i,2,pInfo,&hqtmp,&hqtmp_ex);
						else
							DrawItem(&memDC,j,i+nStartCol,GetXSFlag(pInfo),pInfo,&hqtmp,&hqtmp_ex);
					}
				}

				if(m_pWriteFile)
				{
					m_pWriteFile->WriteString(m_WriteStr+"\n");
					m_WriteStr = "";
				}
			}
		}
		for(i = 0;i < nTotalRow;i++)
		{
			pInfo = (*g_pStockIO)[lpnUsedStkIndex[i]];
			if(!pInfo) continue;
			g_pStockIO->ReadCurrStockData (pInfo,&hqtmp);
			LastNow[i] = hqtmp.Now;
			LastVol[i] = hqtmp.Volume;
			LastBuyp[i] = hqtmp.Buyp[0];
			LastSellp[i] = hqtmp.Sellp[0];
		}
		if(nCurSortIndex != 0 && nCurSortIndex != 1)
			bDrawBlue = FALSE;
		else
			bDrawBlue = TRUE;
	}

	//写左边序号内容
	for(i=0;nCellHeadWidth>3 && i<tempRowNum;i++)
	{
		g_d.DisplayText(&memDC,0,nTitleHeight+i*nCellHeight+(nCellHeight-FontSize.cy)/2,VipColor.TXTCOLOR,nCellHeadWidth-6,"%4d",nStartRow+i+1);
		if(!bRealDraw && m_nGridType!=FY_TYPE && m_nGridType!=DDE_TYPE && m_nGridType!=A2H_TYPE && m_nGridType!=JSZJ_TYPE && nCurSortIndex==0)
		{
			pInfo = (*g_pStockIO)[lpnUsedStkIndex[nStartRow+i]];
			if(!pInfo) continue;
			DrawItem(&memDC,i,0,2,pInfo,&hqtmp,&hqtmp_ex);
			DrawItem(&memDC,i,1,2,pInfo,&hqtmp,&hqtmp_ex);
		}
	}
	//如果只处理三方行情(非实时行情)，则对AB股行情进行提示
	if(g_GC.bOnlyHK && !m_bFixedGrid && m_WhichGPType<TYPE_ZXG && m_WhichGPType>=0)
		DrawNoABHintInfo(&memDC,m_DRect);
	//从内存DC拷贝到屏幕DC
	pDC->BitBlt (m_DRect.left,m_DRect.top,m_DRect.Width(),m_DRect.Height(),&memDC,0,0,SRCCOPY);
	
	//画指示线
	DrawHighLine(pDC);
	DrawSortHighLine(pDC);

	DELETE_GRIDMEMDC
	
	if(bRealDraw)	m_nCurPic=2;
	else			m_nCurPic=1;
	DrawFlag = DRAW_NOREADDATA;
}

void HQGrid::DrawHZ(CDC *pDC)
{
	if(GetStaticItemCount()<=0) 
		return;	
	for(int i=0;i<2*TOTAL_COL;i++)
	{
		m_fAver[i]	= 0.0f;
		m_fSum[i]	= 0.0f;
		m_fMax[i]	= -99999.99f;
		m_fMin[i]	= 99999.99f;
		m_fStd[i]	= 0.0f;
		m_fQZ[i]	= 1;
		m_fStdQZ[i] = 0;
	}
	float	fQZSum  = 0.0f;
	float	fQZSum2 = 0.0f;
	MemStkInfo *	pInfo;
	CurrStockData	hqtmp={0};
	EXCURRDATA		hqtmp_ex={0};
	//第一轮
	for(int j = 0;j < nTotalRow;j++)
	{
		pInfo = NULL;
		if(lpnUsedStkIndex[j]>=0)
		{
			pInfo = (*g_pStockIO)[lpnUsedStkIndex[j]];
			if(!pInfo) continue;
			g_pStockIO->ReadCurrStockData (pInfo,&hqtmp,NULL,&hqtmp_ex);
		}
		//统计汇总,统计功能的权重
		float fQZ = 1.0f;
		switch(g_nHZQZFlag) 
		{
		case 1:					//流通股
			if(pInfo) 
				fQZ = pInfo->ActiveCapital/1000;
			break;
		case 2:					//总股本
			if(pInfo) 
				fQZ = pInfo->J_zgb/1000;
			break;
		default:				//平均
			break;
		}
		fQZSum += fQZ;
		if(hqtmp.Now>COMPPREC)
			fQZSum2 += fQZ;
		for(i=0;i<tempColNum+m_nFixedLeadCol;i++)
		{
			if(i<m_nFixedLeadCol)
				CalcItem(j,i,2,pInfo,&hqtmp,&hqtmp_ex,fQZ);
			else
				CalcItem(j,i+nStartCol,GetXSFlag(pInfo),pInfo,&hqtmp,&hqtmp_ex,fQZ);
		}
	}		
	//每个栏目的权重
	for(int k=0;k<2*TOTAL_COL;k++)
	{
		m_fQZ[k] = fQZSum;
		if(k<ABGPHQ_COL_NUM && k!=ZRSP && k!=SYL && k!=J_LTGB && k!=J_ZSZ && k!=J_LTSZ && k!=QRD)
			m_fQZ[k] = fQZSum2;
		//平均值
		if(m_fQZ[k]<COMPPREC)
			m_fAver[k] = 0;
		else
			m_fAver[k] /= m_fQZ[k];
		//修正最值
		if(fabs(m_fMax[k]+99999.99)<0.1) m_fMax[k] = 0.0f;
		if(fabs(m_fMin[k]-99999.99)<0.1) m_fMin[k] = 0.0f;
	}	
	//第二轮
	if(m_nStatistFlag[STAT_STD])
	{
		for(int j = 0;j < nTotalRow;j++)
		{
			pInfo = NULL;
			if(lpnUsedStkIndex[j]>=0)
			{
				pInfo = (*g_pStockIO)[lpnUsedStkIndex[j]];
				if(!pInfo) continue;
				g_pStockIO->ReadCurrStockData (pInfo,&hqtmp,NULL,&hqtmp_ex);
			}
			//统计汇总,统计功能的权重			
			float fQZ = 1.0f;
			for(i=0;i<tempColNum+m_nFixedLeadCol;i++)
			{
				if(i<m_nFixedLeadCol)
					CalcItem(j,i,2,pInfo,&hqtmp,&hqtmp_ex,fQZ,1);
				else
					CalcItem(j,i+nStartCol,GetXSFlag(pInfo),pInfo,&hqtmp,&hqtmp_ex,fQZ,1);
			}			
		}
	}
	//汇总
	int nTitleHeightTmp = nTitleHeight;
	nTitleHeight = nTitleHeight0;
	int nHZIndex = 0;
	for(j=0;j<STAT_LASTNULL;j++)
	{
		if(m_nStatistFlag[j]==0) 
			continue;
		float fCellData[2*TOTAL_COL]={0};
		switch(j) 
		{
		case STAT_AVER:
			memcpy(fCellData, m_fAver, 2*TOTAL_COL*sizeof(float));
			break;
		case STAT_SUM:
			memcpy(fCellData, m_fSum, 2*TOTAL_COL*sizeof(float));
			break;
		case STAT_MAX:
			memcpy(fCellData, m_fMax, 2*TOTAL_COL*sizeof(float));
			break;
		case STAT_MIN:
			memcpy(fCellData, m_fMin, 2*TOTAL_COL*sizeof(float));
			break;
		case STAT_STD:
			memcpy(fCellData, m_fStd, 2*TOTAL_COL*sizeof(float));
			for(k=0;k<2*TOTAL_COL;k++)
			{
				if(m_fStdQZ[k]<COMPPREC)
					fCellData[k] = 0;
				else
					fCellData[k] /= m_fStdQZ[k];
				fCellData[k] = sqrt(fCellData[k]);
			}
			break;
		default:
			continue;
		}		
		for(i=0;i<tempColNum+m_nFixedLeadCol;i++)
		{
			if(i<m_nFixedLeadCol)
				DrawHZItem(pDC,j,fCellData,nHZIndex,i,2);
			else
				DrawHZItem(pDC,j,fCellData,nHZIndex,i+nStartCol,(j==STAT_SUM)?2:3);
		}
		++nHZIndex;
	}
	nTitleHeight = nTitleHeightTmp;
}

void HQGrid::DrawTitle(CDC *pDC)
{
    int i,j,width;
	int tempTitlePos = 1;	//从顶行开始画标题
	if(g_GH.bBigStatusBar)
		tempTitlePos = 2;
	int xtmp = nCellHeadWidth;
	CString tmpTitle;
	HEAD_DATA_TYPE tmpDataType;
	CSize tmpSize;
	CRect TRect(0,0,m_DRect.Width(),m_DRect.Height());
	TRect.bottom = TRect.top+nTitleHeight;
	g_d.Draw3DBar2(pDC, TRect, /*GridColor.GridLineColor*/RGB(128,0,0), FALSE);
	for(i = j = 0;j < nTotalCol+m_nFixedLeadCol-1 && xtmp < m_DRect.right;i++)
	{
  		if (i>=m_nFixedLeadCol) 
			j = i+nStartCol;
		else	  
			j = i;
		tmpTitle = GetItemTitle(m_lpnCol[j]);
		tmpDataType = GetItemDataType(m_lpnCol[j]);
		width = GetItemWidth(m_lpnCol[j]);
		if(width<1) continue;
		if(xtmp+width < m_DRect.Width()+3) //对于超出来的部分,不画
		{
			COLORREF coll = GridColor.GridTitleTextColor;
			if((nCurSortIndex != ((i>=m_nFixedLeadCol)?(nStartCol+i):i)) || (nCurSortIndex == 0) )
			{
				if(tmpDataType==TYPE_STRING)
					g_d.DisplayText(pDC,xtmp+4,tempTitlePos,coll,0,tmpTitle);
				else
					g_d.DisplayText(pDC,xtmp,tempTitlePos,coll,width-4,tmpTitle);
			}
			else
			{	
				if(tmpDataType==TYPE_STRING)
				{
					tmpSize=g_d.DisplayText(pDC,xtmp+1,tempTitlePos,coll,0,tmpTitle);
					g_d.DisplayText(pDC,xtmp+tmpSize.cx-3,tempTitlePos,GridColor.GridTitleArrowColor,0,(bAscendSort==TRUE)?_F("↓"):_F("↑"));
				}
				else
				{
					g_d.DisplayText(pDC,xtmp,tempTitlePos,coll,width-FontSize.cx-1,tmpTitle);
					g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleArrowColor,width+2,(bAscendSort==TRUE)?_F("↓"):_F("↑"));
				}
			}
		}
		xtmp+=width;
		if(m_pWriteFile)
			m_WriteStr+=tmpTitle+(CString)"\t";
	}
	if(m_pWriteFile)	
	{
		m_pWriteFile->WriteString(m_WriteStr+"\n");
		m_WriteStr = "";
	}
	if(xtmp > m_DRect.right)
		nColNum = j-nStartCol-m_nFixedLeadCol;	//顺便计算列数
	else 
		nColNum = j-nStartCol-m_nFixedLeadCol+1;
	if(nColNum < 0) nColNum = 0;
}

//画雷的标记 flag 0:小雷 1:大雷
void HQGrid::DrawMineIcon(CDC *pDC,int row,int col,int flag)
{
	int width = 0,tempPos;
 	int x = GetCellLeft((col+1),width);
	tempPos = nTitleHeight + row*nCellHeight-18;
	pDC->DrawIcon(x-14,tempPos,(flag==0)?m_hOneThunder:m_hMoreThunder);
}

void HQGrid::DrawA2HItem(CDC *pDC,int row,int col,int xsflag,MemStkInfo * pInfo_hk,CurrStockData * hqp_hk,EXCURRDATA *hqpex_hk, MemStkInfo * pInfo,CurrStockData * hqp,EXCURRDATA *hqpex)
{
	int			nColItem = m_lpnCol[col];
	float		fStatData=MEANLESS_DATA;
	CString		tmpstr;
	FormatInfo	fi;
	fi.xsflag = xsflag;
	fi.color  = VipColor.TXTCOLOR;

	CString e_rs="";
	if(nColItem>=0&&nColItem<H_ZQDM)
		TransID2Str(nColItem,pInfo,hqp,hqpex,NULL,fi,fStatData,tmpstr,e_rs);
	else if(nColItem>=H_ZQDM&&nColItem<H_YJ&&pInfo_hk)
	{
		pInfo_hk->J_hg = pInfo->J_hg;
		TransID2Str(nColItem,pInfo_hk,hqp_hk,hqpex_hk,NULL,fi,fStatData,tmpstr,e_rs);
	}
	if(e_rs.GetLength()<1) 
		e_rs = tmpstr;

	switch (nColItem)
	{
	case ZQDM:		//股票代码
		if(!pInfo) break;
		DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,RGB(255,255,0),FALSE,"%s",tmpstr);
		if(m_pWriteFile)	
		{
			if(m_bWriteExcel)
				m_WriteStr+=(CString)"=\""+pInfo->Code+(CString)"\"";
			else
				m_WriteStr+=pInfo->Code;
		}
		break;
	case ZQJC:		//股票名称
		if(!pInfo) break;
		DrawCellText(pDC,row,col,0,pInfo->Name,RGB(255,255,0),TRUE,TRUE,pInfo);
		if(m_pWriteFile)	m_WriteStr+=pInfo->Name;
		if(m_bInfoConnect && g_bShowHqMine && m_nGridType!=FY_TYPE)
		{
			int iNo = g_pStockIO->GetIndex(pInfo->Code,pInfo->setcode);
			if(iNo>=DS_FROM) break;
			if(iNo >= 0 && g_nMineNum[iNo]>0)
				DrawMineIcon(pDC,row,col,(g_nMineNum[iNo]==1)?0:1);
		}
		break;
	case ZJCJ:  //当前现价  画蓝框
		if(	g_bDrawBlue && bDrawBlue && m_nGridType!=FY_TYPE && LastNow[nStartRow+row]>COMPPREC && fabs( LastNow[nStartRow+row] - hqp->Now ) > COMPPREC )
			DrawBlueRect(pDC,row,col,hqp->Now,xsflag);
		else if(hqp->Now>COMPPREC)
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",tmpstr);
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		if(m_pWriteFile) m_WriteStr+=e_rs;
		break;
	case CJL:   //成交量
		if(!pInfo) break;
		DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",tmpstr);
		if(m_pWriteFile) m_WriteStr+=e_rs;
		break;
	case H_YJ:
		{
			if(!pInfo||!pInfo_hk) break;
			float ftmp_hk=hqp_hk->Now*g_gy_rate, ftmp=hqp->Now;
			if(ftmp_hk>COMPPREC&&ftmp>COMPPREC)
			{
				ftmp = ftmp - ftmp_hk;
				fi.color=GetItemColor(ftmp,.0);
				tmpstr=FormatFloat2Str(ftmp,3);
			}
			else 
				tmpstr=MEANLESS_STR;

			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",tmpstr);
			if(m_pWriteFile) m_WriteStr+=tmpstr;
		}
		break;
	case H_YJL:
		{
			if(!pInfo||!pInfo_hk) break;
			float ftmp_hk=hqp_hk->Now*g_gy_rate, ftmp=hqp->Now;
			//修正涨跌和幅度的计算方法,对于开放式集合竞价有用
			if(ftmp_hk>COMPPREC&&ftmp>COMPPREC)
			{
				ftmp = ftmp - ftmp_hk;
				fi.color=GetItemColor(100.0f*ftmp/ftmp_hk,0.0);
				fStatData=100.0f*ftmp/ftmp_hk;
				tmpstr=FormatFloat2Str(fStatData,XS2);
			}
			else 
				tmpstr=MEANLESS_STR;

			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",tmpstr);
			if(m_pWriteFile) m_WriteStr+=tmpstr;
		}
		break;
	default:  
		DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",tmpstr);
		if(m_pWriteFile) m_WriteStr+=e_rs;
		break;
	}

	if(m_pWriteFile)
		m_WriteStr+="\t";
}

void HQGrid::DrawItem(CDC *pDC,int row,int col,int xsflag,MemStkInfo * pInfo,CurrStockData * hqp,EXCURRDATA *hqpex)
{
	int			nColItem = m_lpnCol[col];
	float		fStatData=MEANLESS_DATA,ftmp;
	CString		tmpstr;
	FormatInfo	fi;
	fi.xsflag = xsflag;
	fi.color  = VipColor.TXTCOLOR;

	if(g_GC.bOnlyHK && m_WhichGPType<TYPE_ZXG && m_WhichGPType>=0)
	{
		if(nColItem!=ZQDM && nColItem!=ZQJC) 
			return;
	}
	
	if(nColItem>=TOTAL_COL)		//静态行情
	{
		int nBaseRow = -1, nBaseCol = -1;
		nBaseCol = nColItem%TOTAL_COL;
		if(!pInfo)			//这一行不是股票
			nBaseRow = -1-lpnUsedStkIndex[row + nStartRow];
		else					//股票
			nBaseRow = Fy_nTag[row + nStartRow];
		if(nBaseRow<0||nBaseCol<0) return;
		char strText[256]={0};
		GetString(strText,255,nBaseRow, nBaseCol);
		DrawCellStringText(pDC, row, col,0,m_ItemTextFormat[col],GridColor.Grid_Level_Color, TRUE,"%s",strText);	
		return;
	}	
	LPDDEREPORT pDDEReport = NULL;
	if(m_bDDEStatus)
		pDDEReport = g_pStockIO->ReadDDEReport(pInfo->setcode,pInfo->Index);
	TICKSTAT its=pInfo->its;
	if(m_bJSZJStatus&&its.Minute==0)
		ReadDataTickStat(pInfo, &its);
	CString e_rs="";
	if(nColItem>=0)
		TransID2Str(nColItem,pInfo,hqp,hqpex,pDDEReport,fi,fStatData,tmpstr,e_rs);
	if(e_rs.GetLength()<1) 
		e_rs = tmpstr;	
	if(nColItem != J_START && nColItem != J_GJG && nColItem != J_ZGG && nColItem >= J_GXRQ && nColItem <= J_TZMGJZ)
	{
		if( !testzs(pInfo) )
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",tmpstr);
			if(nColItem == J_GXRQ)
			{
				GetStatData_And_WriteInt(pInfo->J_gxrq)
			}
			else
			{
				GetStatData_And_WriteFloat(fStatData)
			}
		}
		if(m_pWriteFile)
			m_WriteStr+="\t";
		return;
	}
	switch (nColItem)
	{
	case ZQDM:		//股票代码
		if(!pInfo) break;
		DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,RGB(255,255,0),FALSE,"%s",tmpstr);
		if(m_pWriteFile)	
		{
			if(m_bWriteExcel)
				m_WriteStr+=(CString)"=\""+pInfo->Code+(CString)"\"";
			else
				m_WriteStr+=pInfo->Code;
		}
		break;
	case ZQJC:		//股票名称
		{
			if(!pInfo) break;
			DrawCellText(pDC,row,col,0,pInfo->Name,RGB(255,255,0),TRUE,TRUE,pInfo);			if(m_pWriteFile)	m_WriteStr+=pInfo->Name;
			if(m_bInfoConnect && g_bShowHqMine && m_nGridType!=FY_TYPE)
			{
				int iNo = g_pStockIO->GetIndex(pInfo->Code,pInfo->setcode);
				if(iNo>=DS_FROM) break;
				if(iNo >= 0 && g_nMineNum[iNo]>0)
					DrawMineIcon(pDC,row,col,(g_nMineNum[iNo]==1)?0:1);
			}
		}
		break;
	case ZGCJ:	// 最高价 冲击过涨停反显
		ftmp = 0.0;
		if(g_GH.bHlight_HighLow)
		{
			if(pInfo->setcode==SZ || pInfo->setcode==SH)
				ftmp = GetTPPrice(pInfo,hqp->Close,TRUE);
		}
		if(ftmp>COMPPREC && fabs( ftmp - hqp->Max ) < COMPPREC)
			DrawBlueRect(pDC,row,col,hqp->Max,xsflag,TRUE);
		else
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",tmpstr);
		if(m_pWriteFile) m_WriteStr+=e_rs;
		break;
	case ZDCJ:	// 最低价 冲击过跌停反显
		ftmp = 0.0;
		if(g_GH.bHlight_HighLow)
		{
			if(pInfo->setcode==SZ || pInfo->setcode==SH)
				ftmp = GetTPPrice(pInfo,hqp->Close,FALSE);
		}
		if(ftmp>COMPPREC && fabs( ftmp - hqp->Min ) < COMPPREC)
			DrawBlueRect(pDC,row,col,hqp->Min,xsflag,TRUE);
		else
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",tmpstr);
		if(m_pWriteFile) m_WriteStr+=e_rs;
		break;
	case ZJCJ:  //当前现价  画蓝框
		if(	g_bDrawBlue && bDrawBlue && m_nGridType!=FY_TYPE && LastNow[nStartRow+row]>COMPPREC && fabs( LastNow[nStartRow+row] - hqp->Now ) > COMPPREC )
			DrawBlueRect(pDC,row,col,hqp->Now,xsflag);
		else if(hqp->Now>COMPPREC)
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",tmpstr);
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		if(m_pWriteFile) m_WriteStr+=e_rs;
		break;
	case ZGJM:  //叫买
	case BJ1:	//买价1
		if(!pInfo) break;
		if( !testzs(pInfo) && hqp->Buyp[0] > COMPPREC)
		{
			if(g_bDrawBlue && bDrawBlue && pInfo->bigmarket==QH_TYPE && LastBuyp[nStartRow+row]>COMPPREC && fabs(LastBuyp[nStartRow+row] - hqp->Buyp[0]) > COMPPREC)
				DrawBlueRect(pDC,row,col,hqp->Buyp[0],xsflag);
			else 
				DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",tmpstr);
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case ZDJM:	//叫卖
	case SJ1:	//卖价1
		if(!pInfo) break;
		if( !testzs(pInfo) && hqp->Sellp[0] > COMPPREC)
		{
			if(g_bDrawBlue && bDrawBlue && pInfo->bigmarket==QH_TYPE && LastSellp[nStartRow+row]>COMPPREC && fabs(LastSellp[nStartRow+row] - hqp->Sellp[0]) > COMPPREC)
				DrawBlueRect(pDC,row,col,hqp->Sellp[0],xsflag);
			else
				DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",tmpstr);
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case CJL:   //成交量
		if(!pInfo) break;
		DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",tmpstr);
		if(m_pWriteFile) m_WriteStr+=e_rs;
		break;
	case DDX_1:			//当日DDX
		if(!pInfo) break;
		if(pDDEReport && pDDEReport->fDDX!=MEANLESS_DATA)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%.3f",pDDEReport->fDDX);
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case DDY_1:			//当日DDY
		if(!pInfo) break;
		if(pDDEReport && pDDEReport->fDDY!=MEANLESS_DATA)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%.3f",pDDEReport->fDDY);
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case DDZ_1:			//当日DDZ
		if(!pInfo) break;
		if(pDDEReport && pDDEReport->fDDZ!=MEANLESS_DATA)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%.3f",pDDEReport->fDDZ);
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case DDX_60:		//60日DDX
		if(!pInfo) break;
		if(pDDEReport && pDDEReport->fDDX60!=MEANLESS_DATA)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%.3f",pDDEReport->fDDX60);
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case DDY_60:		//60日DDY
		if(!pInfo) break;
		if(pDDEReport && pDDEReport->fDDY60!=MEANLESS_DATA)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%.3f",pDDEReport->fDDY60);
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case DDX_REDNUM:	//DDX连红天数
		if(!pInfo) break;
		if(pDDEReport)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%7ld",pDDEReport->DDXRed);
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case DDX_REDIN10:	//10内DDX红
		if(!pInfo) break;
		if(pDDEReport)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%7ld",pDDEReport->DDXRed10);
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case BIGIOAMOUNT:	//SUP净流向
		if(!pInfo) break;
		if(pDDEReport)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,MakeFloat(pDDEReport->fBigIOAmount));
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case BIGIOWEIGHT:	//主力活跃度
		if(!pInfo) break;
		if(pDDEReport)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%.2f",pDDEReport->fBigIOWeight);
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case BIGBUY_RAT:	//大单买入%
		if(!pInfo) break;
		if(pDDEReport && pDDEReport->fBigBuyRate!=MEANLESS_DATA)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%.2f",pDDEReport->fBigBuyRate);
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case BIGSELL_RAT:	//大单卖出%
		if(!pInfo) break;
		if(pDDEReport && pDDEReport->fBigSellRate!=MEANLESS_DATA)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%.2f",pDDEReport->fBigSellRate);
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case TOTALWTB:		//总买委比
		if(!pInfo) break;
		if(pDDEReport && pDDEReport->fBOrderRate!=MEANLESS_DATA)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%.2f",pDDEReport->fBOrderRate);
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case TICK2_JL:		//逐笔均量
		if(!pInfo) break;
		if(pDDEReport && pDDEReport->fAveTradeVol!=MEANLESS_DATA)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%.2f",pDDEReport->fAveTradeVol);
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case J_GJG:		//国家股暂空
	case J_ZGG:		//职工股暂空
		DrawEmpty_And_WriteEmpty
		break;
	case HY_STR:	//所属行业
		if(!pInfo) break;
		/*
		if(pInfo->J_hy>0 && pInfo->J_hy<=TOTAL_HY_NUM)
		{
			DrawCellStringText(pDC,row,col,0,0,VipColor.TXTCOLOR,TRUE,g_cwHYstr[pInfo->J_hy-1]);
			if(m_pWriteFile) m_WriteStr+=g_cwHYstr[pInfo->J_hy-1];
		}
		*/
		if(pInfo->J_zjhhy>0)
		{
			ZJHHY_Cfg *pZcfg = g_pConstMng->GetHyFromHYID(pInfo->J_zjhhy);
			if(pZcfg)
			{
				DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,VipColor.TXTCOLOR,TRUE,pZcfg->HYName);
				if(m_pWriteFile) m_WriteStr+=pZcfg->HYName;
			}
		}
		break;
	case ON_DXTX:	//短线提醒
		{
			if(!pInfo) break;
			int iIndex = pInfo->CalcRes1.fOut[4];
			if(iIndex>0)
				DrawImage(pDC,row,col,iIndex-1);
		}
		break;
	case ON_ZXTX:	//中线提醒
		{
			if(!pInfo) break;
			int iIndex = pInfo->CalcRes1.fOut[5];
			if(iIndex>0)
				DrawImage(pDC,row,col,iIndex-1);
		}
		break;
	case JS_INDD:	//大单买入
		if(!pInfo) break;
		if(its.Minute>0)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",MakeJE(its.fAmount[1][0]));
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case JS_OUTDD:	//大单卖出
		if(!pInfo) break;
		if(its.Minute>0)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",MakeJE(its.fAmount[1][1]));
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case JS_INZD:	//中单买入
		if(!pInfo) break;
		if(its.Minute>0)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",MakeJE(its.fAmount[2][0]));
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case JS_OUTZD:	//中单卖出
		if(!pInfo) break;
		if(its.Minute>0)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",MakeJE(its.fAmount[2][1]));
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case JS_INXD:	//小单买入
		if(!pInfo) break;
		if(its.Minute>0)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",MakeJE(its.fAmount[0][0]-its.fAmount[1][0]-its.fAmount[2][0]));
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	case JS_OUTXD:	//小单卖出
		if(!pInfo) break;
		if(its.Minute>0)
		{
			DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",MakeJE(its.fAmount[0][1]-its.fAmount[1][1]-its.fAmount[2][1]));
			if(m_pWriteFile) m_WriteStr+=e_rs;
		}
		else
			DrawEmpty_And_WriteEmpty
		break;
	default:  
		DrawCellStringText(pDC,row,col,fi.v_align,fi.h_align,fi.color,FALSE,"%s",tmpstr);
		if(m_pWriteFile) m_WriteStr+=e_rs;
		break;
	}
	if(m_pWriteFile)
		m_WriteStr+="\t";
}

void HQGrid::CalcItem(int row,int col,int xsflag,MemStkInfo * pInfo,CurrStockData * hqp,EXCURRDATA *hqpex,float fQZ,int nCalcWhich)
{
	int				nColItem = m_lpnCol[col];
	float			fStatData=MEANLESS_DATA;
	CString			tmpstr;
	FormatInfo		fi;
	//对第二轮的权重
	if(nCalcWhich==1 && hqp->Now<COMPPREC && nColItem<ABGPHQ_COL_NUM && nColItem!=ZRSP && nColItem!=SYL && 
		nColItem!=J_LTGB && nColItem!=J_ZSZ && nColItem!=J_LTSZ && nColItem!=QRD)
	{
		return;
	}
	if(nColItem>=TOTAL_COL)		//静态行情
	{
		int nBaseRow = -1, nBaseCol = -1;
		nBaseCol = nColItem%TOTAL_COL;
		if(!pInfo)			//这一行不是股票
			nBaseRow = -1-lpnUsedStkIndex[row];
		else					//股票
			nBaseRow = Fy_nTag[row];
		if(nBaseRow<0||nBaseCol<0) return;
		if(m_vBaseHeads[nBaseCol].dataType==TYPE_STRING) return;		
		fStatData = fGetFyValue(nBaseRow, nBaseCol);
	}
	else if(nColItem>=0)		//系统字段
	{
		switch (nColItem)
		{
		case ZQDM:			//股票代码
		case SPELL_CODE:
		case ZQJC:			//股票名称
		case J_START:		//上市日期
		case CURRENCYNAME:	//货币单位
		case EXPIREDATE:	//期权到期日
			return;
		default:  
			break;
		}
		LPDDEREPORT pDDEReport = NULL;
		if(m_bDDEStatus)
			pDDEReport = g_pStockIO->ReadDDEReport(pInfo->setcode,pInfo->Index);	
		CString e_rs;
		TransID2Str(nColItem,pInfo,hqp,hqpex,pDDEReport,fi,fStatData,tmpstr,e_rs);
		if(tmpstr==MEANLESS_STR) return;
	}
	
	//部分统计工作
	if(fabs(fStatData-MEANLESS_DATA)>COMPPREC)
	{
		if(nCalcWhich==0)
		{
			m_fAver[nColItem] += (fStatData*fQZ);
			m_fSum[nColItem] += fStatData;
			m_fMax[nColItem] = max(fStatData, m_fMax[nColItem]);
			m_fMin[nColItem] = min(fStatData, m_fMin[nColItem]);
		}
		else if(nCalcWhich==1)
		{
			m_fStd[nColItem] += pow(fStatData-m_fAver[nColItem],2);
			++m_fStdQZ[nColItem];
		}
	}
}

void HQGrid::DrawHZItem(CDC *pDC,short nHZType,float *fCellData,int row,int col,int xsflag)
{
	int		nColItem = m_lpnCol[col];
	float	ftmp = fCellData[nColItem];
	if(nColItem>=TOTAL_COL)
	{
		int nBaseCol = nColItem%TOTAL_COL;
		if(m_vBaseHeads[nBaseCol].dataType==TYPE_FLOAT)
			DrawCellText( pDC, row, col,0,GridColor.Grid_Level_Color,"%14.2f",ftmp);
		else if(m_vBaseHeads[nBaseCol].dataType==TYPE_INT)
			DrawCellText( pDC, row, col,0,GridColor.Grid_Level_Color,"%ld",(long)ftmp);
		return;
	}
	if(nColItem != J_START && nColItem >= J_GXRQ && nColItem <= J_TZMGJZ)
	{
		if(nColItem == J_GXRQ)
			DrawCellText( pDC, row, col,0,GridColor.Grid_Level_Color,"%ld",(long)ftmp);
		else
			DrawCellText( pDC, row, col,0,GridColor.Grid_Level_Color,"%14.3f",ftmp);
		return;
	}
	switch (nColItem)
	{
	case ZANGSU://涨速排序
		if(ftmp != MEANLESS_DATA)
			DrawCellText(pDC,row,col,0,ftmp,0.0,xsflag);
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	case ZQDM: //股票代码
	case SPELL_CODE:
		break;
	case ZQJC: //股票名称
		{
			CString str = _T("");
			switch(nHZType) 
			{
			case STAT_AVER:	str = _F("平均值");	break;
			case STAT_SUM:	str = _F("合计");	break;
			case STAT_MAX:	str = _F("最大值");	break;
			case STAT_MIN:	str = _F("最小值");	break;
			case STAT_STD:	str = _F("方差");	break;
			default:		str = MEANLESS_STR;		break;
			}
			DrawCellText(pDC,row,col,0,(char*)(LPCSTR)str,GridColor.Grid_CodeName_Color,TRUE,TRUE);
		}
		break;
	case ZRSP:  //昨日收盘
	case JRKP:  //今日开盘
	case ZGCJ:  //最高
	case ZDCJ:  //最低
	case ZJCJ:  //当前现价
		DrawCellText(pDC,row,col,0,ftmp, xsflag,GridColor.Grid_Level_Color);
		break;
	case ZGJM:  //叫买
	case ZDJM:  //叫卖
		if(ftmp > COMPPREC)
			DrawCellText(pDC,row,col,0,ftmp, xsflag,GridColor.Grid_Level_Color);
		else
			DrawCellText(pDC,row,col,0,MEANLESS_STR,GridColor.Grid_Level_Color);
		break;
	case CJL:   //成交量
		DrawCellText(pDC,row,col,0,GridColor.Grid_Volume_Color,"%s",MakeVol3((long)ftmp));
		break;
	case QRSD:  //日涨跌
		DrawCellText(pDC,row,col,0,ftmp,0.0,xsflag);
		break;
	case XS:    //现手
		DrawCellText( pDC, row, col,0, GridColor.Grid_Level_Color, "%s", MakeVol3((long)ftmp));
		break;
	case CJJE:  //成交金额
		DrawCellText( pDC, row, col,0, VipColor.TXTCOLOR,MakeJE(ftmp));
		break;
	case LP:  //内盘
	case WP:  //外盘	
		DrawCellText( pDC, row, col,0, GridColor.Grid_Down_Color,"%s",MakeVol3((long)ftmp));
		break;
	case LWB: //内外比 = 内盘/外盘*100%
		DrawCellText( pDC, row, col,0, VipColor.TXTCOLOR ,"%6.2f",ftmp);
		break;
	case WLC: //委量差 = 五个买盘之和-五个卖盘之和
		DrawCellText( pDC, row, col,0, VipColor.TXTCOLOR,"%s",MakeVol3((long)ftmp));
		break;
	case WTB: //委托比%=(五个买盘之和-五个卖盘之和)/(五个买盘之和+五个卖盘之和)*100
		DrawCellText(pDC, row, col,0,ftmp,0.0,XS2);
		break;
	case ZAF: //涨幅度=日涨跌/昨日收盘*100
		DrawCellText( pDC, row, col,0,ftmp,0.0,2);
		break;
	case ZEF: //振幅度=(最高-最低)/昨日收盘*100
		DrawCellText(pDC,row,col,0,VipColor.TXTCOLOR,"%5.2f",ftmp);
		break;
	case SYL:
		DrawCellText(pDC,row,col,0,VipColor.TXTCOLOR,"%9.2f",ftmp);
		break;
	case JUNJ: //均价
		DrawCellText(pDC,row, col,0, ftmp,fCellData[ZJCJ],xsflag);
		break;
	case LIANGB:  //量比
		DrawCellText(pDC, row, col,0, VipColor.TXTCOLOR,"%6.2f",ftmp);
		break;
	case BJ1:  //买价1
	case BJ2:  //买价2
	case BJ3:  //买价3
	case SJ1://昨结算:卖价1
	case SJ2://卖价二,持仓增减
	case SJ3: //卖价3
		DrawCellText(pDC, row, col,0, ftmp,fCellData[ZJCJ],xsflag);
		break;
	case BJL1:  //买价量1
	case BJL2:  //买价量2
	case BJL3: //买价量3
	case SJL1:  //卖价量1
	case SJL2:  //卖价量2
	case SJL3: //卖价量3
		DrawCellText( pDC, row, col,0,GridColor.Grid_Volume_Color,"%s",MakeVol3((long)ftmp));
		break;
	case DKPH: // 多空平衡
	case DTHL: // 多头获利
	case KTHB: // 空头回补
	case DTZS: // 多头止损
	case KTZS: // 空头止损
		DrawCellText(pDC, row, col,0, ftmp,fCellData[ZJCJ],xsflag);
		break;
	case QRD: //强弱度%
		DrawCellText(pDC,row,col,0,VipColor.TXTCOLOR,"%6.2f",ftmp); //*100用百分比表示
		break;
	case HYD:
		DrawCellText(pDC,row,col,0,VipColor.TXTCOLOR,"%7ld",long(ftmp));
		break;
	case MBZL:
		DrawCellText(pDC,row,col,0,GridColor.Grid_Volume_Color,"%9.2f",ftmp);
		break;
	case MBHSL:
		DrawCellText( pDC, row, col,0, VipColor.TXTCOLOR,"%14.4f",ftmp); //先除10000再乘100
		break;
	case J_HSL   :          //换手率%
		DrawCellText( pDC, row, col,0, VipColor.TXTCOLOR,"%14.2f",ftmp); //先除10000再乘100
		break;
	case J_JYL  :          //净益率%
		DrawCellText( pDC, row, col,0,GridColor.Grid_Level_Color,"%14.2f",ftmp);
		break;
	case J_LTSZ :          //流通市值
		DrawCellText( pDC, row, col,0,VipColor.TXTCOLOR,_F("%14.2f亿"),ftmp);
		break;
	case J_LTGB  :          //流通股本
		DrawCellText( pDC, row, col,0,VipColor.TXTCOLOR,"%14.2f",ftmp);
		break;
	case J_ZSZ:          //AB股总市值
		DrawCellText( pDC, row, col,0,VipColor.TXTCOLOR,_F("%14.2f亿"),ftmp);
		break;
	case J_MGWFP:          //每股未分配
		DrawCellText( pDC, row, col,0,GridColor.Grid_Level_Color,"%14.2f",ftmp );
		break;
	case J_MGSY:			  //每股收益
		{
			char str[64]={0};
			sprintf(str, "%14.3f", ftmp);
			DrawCellText( pDC, row, col, 0, str, GridColor.Grid_Level_Color);
		}
		break;
	case J_MGGJJ:            //每股公积金
	case J_MGJZC:            //每股净资产
	case J_GDQYB://股东权益比
		DrawCellText( pDC, row, col,0,GridColor.Grid_Level_Color,"%14.2f",ftmp);
		break;
	case	J_START:	//上市日期
		break;
	case	QH_JSJ:		//	结算价
	case	QH_YJSJ:	//	期货前结算价
		DrawCellText(pDC,row,col,0,ftmp,fCellData[ZJCJ],xsflag);
		break;
	case SPREAD: //买卖差价
		DrawCellText(pDC,row,col,0,VipColor.TXTCOLOR,"%.3f",ftmp);
		break;
	case BSUNIT:	//买卖单位
		DrawCellText(pDC,row,col,0,VipColor.TXTCOLOR,"%d",(long)ftmp);
		break;
	case CURRENCYNAME:	//货币单位
		break;
	case AVERPRICE:	//平均价
		DrawCellText(pDC,row,col,0,ftmp,fCellData[ZJCJ],xsflag);
		break;
	case YIELDVAL:	//收益率
		DrawCellText(pDC,row,col,0,ftmp,(float)COMPPREC, xsflag);
		break;
	case HIS_HIGH:	//年最高
	case HIS_LOW:	//年最低
	case IEP:		//参考价
		DrawCellText(pDC,row,col,0,ftmp,fCellData[ZJCJ],xsflag);
		break;
	case IEV:		//参考量
		DrawCellText(pDC,row,col,0,GridColor.Grid_Volume_Color,"%.0f",ftmp);
		break;
	case MRKCAP:	//市值
		DrawCellText(pDC,row,col,0,VipColor.TXTCOLOR,"%s",MakeJE(ftmp));
		break;
	case PE:		//市盈率
		DrawCellText(pDC,row,col,0,VipColor.TXTCOLOR,"%.3f",ftmp);
		break;
	case PREMINUM:	//	溢价％
	case GEARING:	//	杠杆比率
		DrawCellText(pDC,row,col,0,VipColor.TXTCOLOR,"%.3f",ftmp);
		break;
	case EXECPRICE:	//	行使价
		DrawCellText(pDC,row,col,0,ftmp,fCellData[ZJCJ],xsflag);
		break;
	case CONVRATIO:	//	认购比率
		DrawCellText(pDC,row,col,0,VipColor.TXTCOLOR,"%.3f",ftmp);
		break;
	case EXPIREDATE://	期权到期日
		break;
	case TODAYZC:
		DrawCellText(pDC,row,col,0,ftmp,fCellData[ZJCJ],xsflag);
		break;
	case TODAYKC:
	case TODAYPC:
		DrawCellText( pDC, row, col,0, VipColor.TXTCOLOR,"%7ld",long(ftmp));
		break;
	case DDX_1:			//当日DDX
	case DDY_1:			//当日DDY
	case DDZ_1:			//当日DDZ
	case DDX_60:		//60日DDX
	case DDY_60:		//60日DDY
		DrawCellText( pDC, row, col,0, VipColor.TXTCOLOR,"%.3f",ftmp);
		break;
	case DDX_REDNUM:	//DDX连红天数
		DrawCellText( pDC, row, col,0, VipColor.TXTCOLOR,"%7ld",long(ftmp));
		break;
	case DDX_REDIN10:	//10内DDX红
		DrawCellText( pDC, row, col,0, VipColor.TXTCOLOR,"%7ld",long(ftmp));
		break;
	case BIGIOAMOUNT:	//SUP净流向
	case BIGIOWEIGHT:	//主力活跃度
	case BIGBUY_RAT:	//大单买入%
	case BIGSELL_RAT:	//大单卖出%
	case TOTALWTB:		//总买委比
	case TICK2_JL:		//逐笔均量
		DrawCellText( pDC, row, col,0, VipColor.TXTCOLOR,MakeFloat(ftmp));
		break;
	default:  
		break;
	}
}

//当鼠标点中一行时，与QRReport不一样，要支持右键更换栏目顺序
void HQGrid::OnClick(CDC *pDC,CPoint point,BOOL IsRight)
{
	int temp,Width,Left;
	if(point.y<m_DRect.top + nTitleHeight0)
	{
		if(!IsRight) return;
		//只有AB股和期货栏目支持调节顺序
		if(m_nGridType!=ABGP_TYPE && m_nGridType!=QH_TYPE)
			return;
		int nPopMenuItem = 1;
		if(m_nGridType == QH_TYPE) 
			nPopMenuItem = 9;
		
		for(int i = m_nFixedLeadCol;i < m_nFixedLeadCol+(nEndCol-nStartCol+1);i++)   // 包括最后一列
		{
			//财务数据不能参与换栏目
			if(i+nStartCol >= ABGPHQ_COL_NUM) break;
			
			Left = GetCellLeft(i+nStartCol,Width);
			CRgn m_rgn;
			m_rgn.CreateRectRgn (m_DRect.left+Left,m_DRect.top,m_DRect.left+Left+Width,m_DRect.top+nSigTitleHeight);
			if(m_rgn.PtInRegion (point))
			{
				CNewMenu menu;
				menu.LoadMenu (IDR_RIGHTMENU);
//				menu.LoadToolBar(IDR_MAINFRAME);
				CNewMenu *pPopup = (CNewMenu *)menu.GetSubMenu(nPopMenuItem);
				(pDC->GetWindow())->ClientToScreen (&point);
				RightClickWhichCol = i;
				pPopup->TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,AfxGetMainWnd());
			}
		}
		return;
	}
	temp = (point.y-m_DRect.top-nTitleHeight) / nCellHeight ;
	if(temp >= 0 && temp < nRowNum)
	{
		DrawHighLine(pDC);
		nCurRow = temp + nStartRow;
		DrawHighLine(pDC);
	}
	if(IsRight)
	{
		CNewMenu menu;
		menu.LoadMenu (IDR_RIGHTMENU);
//		menu.LoadToolBar(IDR_MAINFRAME);
		CNewMenu *pPopup=(CNewMenu *)menu.GetSubMenu(0);
		pPopup->SetDefaultItem(ID_RIGHT_OPEN);
		CNewMenu *pPopup2 = (CNewMenu *)pPopup->GetSubMenu(4);
		CNewMenu ConceptMenu,ZJHHYMenu,ZHBMenu,BlockMenu;
		if(!m_bFixedGrid)
		{
			//添加证监会行业板块股的菜单项
			ZJHHYMenu.CreatePopupMenu();
			UINT StartID = ID_ZJHHY_MENU_1;
			for(int i = 0;i < g_pConstMng->GetZJHHYBlockNum();i++)
			{
				char *strHYName = g_pConstMng->GetHYFromCommandIDXH(i);
				ZJHHYMenu.AppendMenu( MF_STRING,StartID+i,_F(strHYName));
				if( i != 0 && i % 25 == 0 )
					SetMenuBreak(i,&ZJHHYMenu);
			}
			if(i > 0)
				pPopup2->AppendMenu(MF_POPUP,(UINT)(ZJHHYMenu.m_hMenu),_F("证监会行业板块"));

			//添加概念板块股的菜单项
			ConceptMenu.CreatePopupMenu();
			StartID = ID_CONCEPT_MENU_1;
			for(i = 0;i < g_ConceptBlockNum;i++)
			{
				ConceptMenu.AppendMenu( MF_STRING,StartID+i,_F(g_ConceptBlock[i].strBlockName));
				if( i != 0 && i % 18 == 0 )
					SetMenuBreak(i,&ConceptMenu);
			}
			if(i > 0)
				pPopup2->AppendMenu(MF_POPUP,(UINT)(ConceptMenu.m_hMenu),_F("概念板块"));

			//添加组合板块股的菜单项
			ZHBMenu.CreatePopupMenu();
			StartID = ID_ZHB_MENU_1;
			for(i = 0;i < g_ZHBBlockNum;i++)
			{
				ZHBMenu.AppendMenu( MF_STRING,StartID+i,_F(g_ZHBBlock[i].zhmc));
				if( i != 0 && i % 18 == 0 )
					SetMenuBreak(i,&ZHBMenu);
			}
			if(i > 0)
				pPopup2->AppendMenu(MF_POPUP,(UINT)(ZHBMenu.m_hMenu),_F("组合板块"));
			
			//添加板块股的菜单项
			BlockMenu.CreatePopupMenu();
			StartID = ID_BLOCK_MENU_1;
			for(i = 0;i < g_nBlockNum;i++)
			{
				BlockMenu.AppendMenu( MF_STRING,StartID+i,g_BlockInfo[i].Name);
				if( i != 0 && i % 18 == 0 )
					SetMenuBreak(i,&BlockMenu);
			}
			if(i > 0)
				pPopup2->AppendMenu(MF_POPUP,(UINT)(BlockMenu.m_hMenu),_F("用户板块"));			
		}
		else						//删除板块切换菜单
		{
			int iPos, fl = -1; CString str;
			for (iPos=0; iPos < pPopup->GetMenuItemCount(); iPos++)
			{
				pPopup->GetMenuString(iPos, str, MF_BYPOSITION);
				if(str==_F("分类股票")) 
				{
					fl = iPos;
					break;
				}
			}
			if(fl>0)
			{
				pPopup->DeleteMenu(fl, MF_BYPOSITION|MF_POPUP);
				pPopup->GetMenuString(iPos, str, MF_BYPOSITION);
				if(str==_F("板块股票"))
					pPopup->DeleteMenu(fl, MF_BYPOSITION|MF_POPUP);
				pPopup->DeleteMenu(fl, MF_BYPOSITION|MF_SEPARATOR);
			}
		}
		(pDC->GetWindow())->ClientToScreen (&point);
		pPopup->TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,AfxGetMainWnd());
	}
}

//重载虚函数
//计算方格左边位置,Width为返回方格宽度,nCol表示列顺序中绝对序号
int HQGrid::GetCellLeft(int nCol,int & nWidth) 
{   
	int i=0,xtmp=nCellHeadWidth ;
	if(m_nFixedLeadCol<1) i=nStartCol;					//没有固定的派头列
	while(i<nCol)
	{   
		xtmp+=GetItemWidth(m_lpnCol[i]);
	    if (i==m_nFixedLeadCol-1) 
			i=nStartCol+m_nFixedLeadCol;
		else	  
			i++;
	}
	nWidth=GetItemWidth(m_lpnCol[nCol]);
	return xtmp;
}

float HQGrid::fGetSortValue(short xh)
{
	long				wl, wlc;
	float				ftmp;
	short				ii;
	struct CurrStockData	hqtmp={0}, hqtmp_hk={0};
	EXCURRDATA				hqtmp_ex={0}, hqtmp_ex_hk={0};

	MemStkInfo *pInfo = (*g_pStockIO)[xh];
	if(!pInfo) return MEANLESS_DATA;

	MemStkInfo *pInfo_hk = NULL;
	if(m_bAJHStatus&&DSOnLine)
		pInfo_hk = GetLinkedHKStock(pInfo);
	
	g_pStockIO->ReadCurrStockData (pInfo,&hqtmp,NULL,&hqtmp_ex);
	if(pInfo_hk) 
		g_pStockIO->DSGetCurrData(pInfo_hk, &hqtmp_hk, &hqtmp_ex_hk);

	LPDDEREPORT pDDEReport = NULL;
	if(m_bDDEStatus)
		pDDEReport = g_pStockIO->ReadDDEReport(pInfo->setcode,pInfo->Index);
	TICKSTAT its=pInfo->its;
	if(m_bJSZJStatus&&its.Minute==0)
		ReadDataTickStat(pInfo, &its);

	if(nSort_Type >= J_GXRQ && nSort_Type <= J_TZMGJZ)
	{
		if(nSort_Type == SJL) //市净率
		{
			float jly = 0.0f,tmpjz=pInfo->J_tzmgjz;
			if(pInfo->J_zgb > 1 && fabs(pInfo->J_jzc) > COMPPREC) 
				tmpjz=pInfo->J_jzc/pInfo->J_zgb;
			if(tmpjz>COMPPREC) 
			{
				if(hqtmp.Now>COMPPREC)
					jly = hqtmp.Now/tmpjz;
				else if(hqtmp.Close>COMPPREC)
					jly = hqtmp.Close/tmpjz;
				else if(pInfo->Close>COMPPREC)
					jly = pInfo->Close/tmpjz;
			}
			return jly;
		}
		else 
			return *((float *)&pInfo->J_gxrq+(nSort_Type-J_GXRQ));
	}
	switch( nSort_Type)
	{
	case ZANGSU://涨速排序
		if(OnLine && CanShowZangsu(pInfo))	
			ftmp = pInfo->zangsu;
		else		
			ftmp = MEANLESS_DATA;
		return ftmp;
	case ZQDM: //股票代码
		return g_pStockIO->GetIndex(pInfo->Code,pInfo->setcode);
	case ZQJC: //股票名称 
		return atof(pInfo->Code);
	case ZRSP:  //昨日收盘
		return hqtmp.Close;
	case JRKP:  //今日开盘
		return hqtmp.Open;
	case ZGCJ:  //最高
		return hqtmp.Max;
	case ZDCJ:  //最低
		return hqtmp.Min;
	case ZJCJ:
		return hqtmp.Now;
	case ZGJM:  //叫买
		return hqtmp.Buyp[0];
	case ZDJM:  //叫卖
		return hqtmp.Sellp[0];
	case CJL:   //成交量
		return hqtmp.Volume;
	case QRSD:  //日涨跌
		ftmp=hqtmp.Now;
		//修正涨跌和幅度的计算方法,对于开放式集合竞价有用
		if(ftmp<COMPPREC && hqtmp.Buyp[0]>COMPPREC && fabs(hqtmp.Buyp[0]-hqtmp.Sellp[0])<COMPPREC)
			ftmp=hqtmp.Buyp[0];
		if (ftmp < COMPPREC)	ftmp = 0.00;
		else					ftmp = (ftmp - hqtmp.Close);
		return ftmp;
	case XS:    //现手
		if( !testzs(pInfo) )
			return hqtmp.NowVol;
		else
			return MEANLESS_DATA;
	case CJJE:  //成交金额
		return hqtmp.Amount;
	case LP:  //内盘
		if( !testzs(pInfo) )
			return hqtmp.Volume-hqtmp.Outside;
		else
			return MEANLESS_DATA;
	case WP:  //外盘
		if( !testzs(pInfo) )
			return hqtmp.Outside;
		else
			return MEANLESS_DATA;
	case LWB: //内外比=内盘/外盘*100%
		if( !testzs(pInfo) && hqtmp.Outside != 0 )
			return (float)(hqtmp.Volume-hqtmp.Outside)/hqtmp.Outside;
		else
			return MEANLESS_DATA;
	case WLC: //委量差=五个买盘之和-五个卖盘之和
		if( !testzs(pInfo) )
		{
			wlc = 0;
			for (ii = 0;ii < 5;ii++)
				wlc = wlc+(long)hqtmp.Buyv[ii]-(long)hqtmp.Sellv[ii];
			return wlc;
		}
		return MEANLESS_DATA;
	case WTB: //委托比=(五个买盘之和-五个卖盘之和)/(五个买盘之和+五个卖盘之和)*100
		if( !testzs(pInfo) )
		{
			wlc = wl = 0;
			for (ii = 0;ii < 5;ii++)
			{   
				wlc = wlc+(long)hqtmp.Buyv[ii]-(long)hqtmp.Sellv[ii];
				wl = wl +(long)hqtmp.Buyv[ii]+(long)hqtmp.Sellv[ii];
			}
			if (wl != 0l)
				return (float)100.0*wlc/wl;
		}
		return MEANLESS_DATA;
	case ZAF: //涨幅度=日涨跌/昨日收盘*100
		if (hqtmp.Close>COMPPREC)
		{	
			ftmp=hqtmp.Now;
			//修正涨跌和幅度的计算方法,对于开放式集合竞价有用
			if(ftmp<COMPPREC && hqtmp.Buyp[0]>COMPPREC && fabs(hqtmp.Buyp[0]-hqtmp.Sellp[0])<COMPPREC)
				ftmp=hqtmp.Buyp[0];
			if ( ftmp < COMPPREC )
				ftmp = 0.0;
			else
				ftmp = ftmp-hqtmp.Close;
			return ftmp/hqtmp.Close*100.0;
		}
		return MEANLESS_DATA;
	case ZEF: //振幅度=(最高-最低)/昨日收盘*100
		if (hqtmp.Close>0)
		{
			if ( hqtmp.Max > COMPPREC && hqtmp.Min > COMPPREC )
				ftmp = hqtmp.Max - hqtmp.Min;
			else
				ftmp = 0.0;
			return	ftmp/hqtmp.Close*100.0;
		}
		return MEANLESS_DATA;
	case SYL:  //市盈,持仓
		if( !testzs(pInfo) && pInfo->J_zgb > 1)
		{	
			ftmp = (pInfo->J_jly*12/pInfo->J_HalfYearFlag)/pInfo->J_zgb;
			if(ftmp > COMPPREC)
			{
				if(hqtmp.Now > COMPPREC)
				{
					ftmp = hqtmp.Now/ftmp;
					return ftmp;
				}
				else if(hqtmp.Close > COMPPREC)
				{
					ftmp = hqtmp.Close/ftmp;
					return ftmp;
				}					
			}
		}
		return MEANLESS_DATA;
	case JUNJ: //均价
		return cal_averagetmp(pInfo,&hqtmp);
	case LIANGB:  //量比
		if( !testzs(pInfo) )
		{
			short fz[8];
			InitStkTime(pInfo->setcode,fz);
			ii = GetTdxMinute(fz);
			if (ii > 0 && pInfo->VolBase > 0l)
				return (float)hqtmp.Volume/ii/pInfo->VolBase;
		}
		return MEANLESS_DATA;
	case BJ1:  //买价1
		if( !testzs(pInfo) )
			return hqtmp.Buyp[0];
		return MEANLESS_DATA;
	case BJ2:  //买价2
		if( !testzs(pInfo) )
			return hqtmp.Buyp[1];
		return MEANLESS_DATA;
	case BJ3:  //买价3
		if( !testzs(pInfo) )
			return hqtmp.Buyp[2];
		return MEANLESS_DATA;
	case SJ1://昨结算:卖价1
		if( !testzs(pInfo) )
			return hqtmp.Sellp[0];
		return MEANLESS_DATA;
	case SJ2:	      //卖价二
		if( !testzs(pInfo) )
			return hqtmp.Sellp[1];
		return MEANLESS_DATA;
	case SJ3: //卖价3
		if( !testzs(pInfo) )
			return hqtmp.Sellp[2];
		return MEANLESS_DATA;
	case BJL1:  //买价量1
		if( !testzs(pInfo) )
			return hqtmp.Buyv[0];
		return MEANLESS_DATA;
	case BJL2:  //买价量2
		if( !testzs(pInfo) )
			return hqtmp.Buyv[1];
		return MEANLESS_DATA;
	case BJL3: //买价量3
		if( !testzs(pInfo) )
			return hqtmp.Buyv[2];
		return MEANLESS_DATA;
	case SJL1:  //卖价量1
		if( !testzs(pInfo) )
			return hqtmp.Sellv[0];
		return MEANLESS_DATA;
	case SJL2:  //卖价量2
		if( !testzs(pInfo) )
			return hqtmp.Sellv[1];
		return MEANLESS_DATA;
	case SJL3: //卖价量3
		if( !testzs(pInfo) )
			return hqtmp.Sellv[2];
		return MEANLESS_DATA;
	case DKPH: // 多空平衡
		if( !testzs(pInfo) )
			return pInfo->tbp;
		else 
			return MEANLESS_DATA;
	case DTHL: // 多头获利
		if( pInfo->dkflag == 1 && !testzs(pInfo) )
			return pInfo->leave;
		else
			return MEANLESS_DATA;
	case KTHB: // 空头回补
		if( pInfo->dkflag == 0 && !testzs(pInfo) )
			return pInfo->leave;
		else
			return MEANLESS_DATA;
	case DTZS: // 多头止损
		if( pInfo->dkflag == 1 && !testzs(pInfo) )
			return pInfo->stoplost;
		else
			return MEANLESS_DATA;
	case KTZS: // 空头止损
		if( pInfo->dkflag == 0 && !testzs(pInfo) )
			return pInfo->stoplost;
		else
			return MEANLESS_DATA;
	case QRD: //强弱度%
		if (hqtmp.Close>COMPPREC)
		{
			if ( hqtmp.Now < COMPPREC ) ftmp = 0.0;
			else                   ftmp = hqtmp.Now - hqtmp.Close;
			if ( g_pStockIO->GetDomain ( pInfo ) == SZ )
				return (ftmp/hqtmp.Close-m_SzzsZaf)*100.0;
			else
				return (ftmp/hqtmp.Close-m_ShzsZaf)*100.0;
		}
		else 
			return MEANLESS_DATA;
	case HYD:
		if(pInfo->nWarnCount != 0 && !testzs(pInfo))
			return pInfo->nWarnCount;
		else
			return MEANLESS_DATA;
	case MBZL:
		if(hqtmp.ItemNum > 0 && !testzs(pInfo))
			return hqtmp.Volume*1.0/hqtmp.ItemNum;
		else
			return MEANLESS_DATA;
	case MBHSL:
		if(hqtmp.ItemNum > 0 && pInfo->ActiveCapital > 1 && !testzs(pInfo))
			return (1.0*hqtmp.Volume*pInfo->Unit/(pInfo->ActiveCapital*100.0))/hqtmp.ItemNum;
		else
			return MEANLESS_DATA;
	case J_HSL   :          //换手率%
		if( !testzs(pInfo) && pInfo->ActiveCapital > 1 )
			return 1.0*hqtmp.Volume*pInfo->Unit/(pInfo->ActiveCapital*100);
		else 
			return MEANLESS_DATA;
	case J_JYL  :          //净益率%
		if( !testzs(pInfo) && pInfo->J_jzc > 1)
			return pInfo->J_jly/pInfo->J_jzc*100.0;
		else 
			return MEANLESS_DATA;
	case J_LTSZ :          //流通市值排名(万)
		if( !testzs(pInfo) )
		{
			if(hqtmp.Now>COMPPREC)	ftmp = hqtmp.Now;
			else					ftmp = hqtmp.Close;
			return pInfo->ActiveCapital*ftmp;
		}
		else 
			return MEANLESS_DATA;
	case J_LTGB  :          //流通股本
		if( !testzs(pInfo) )
			return pInfo->ActiveCapital;
		else 
			return MEANLESS_DATA;
	case J_ZSZ:          //AB股总市值
		if( !testzs(pInfo) )
		{
			if(hqtmp.Now>COMPPREC)	ftmp = hqtmp.Now;
			else					ftmp = hqtmp.Close;
			return (pInfo->J_zgb-pInfo->J_hg)*ftmp;
		}
		else 
			return MEANLESS_DATA;
	case J_MGWFP:          //每股未分配
		if( !testzs(pInfo) && pInfo->J_zgb > 1)
			return pInfo->J_wfply/pInfo->J_zgb;
		else 
			return MEANLESS_DATA;
	case J_MGSY:			  //每股收益
		if( !testzs(pInfo) && pInfo->J_zgb > 1)
		{
			if(fabs(pInfo->J_mgsy2)>COMPPREC)
				return pInfo->J_mgsy2;	
			return pInfo->J_jly/pInfo->J_zgb;
		}
		else 
			return MEANLESS_DATA;
	case J_MGGJJ:            //每股公积金
		if( !testzs(pInfo) && pInfo->J_zgb > 1)
			return pInfo->J_zbgjj/pInfo->J_zgb;
		else 
			return MEANLESS_DATA;
	case J_MGJZC:            //每股净资产
		if( !testzs(pInfo) && pInfo->J_zgb > 1)
		{
			if(fabs(pInfo->J_jzc)>COMPPREC)
				return pInfo->J_jzc/pInfo->J_zgb;
			else
				return pInfo->J_tzmgjz;
		}
		else 
			return MEANLESS_DATA;
	case J_GDQYB://权益比
		if( !testzs(pInfo) && pInfo->J_zzc > 1)
			return pInfo->J_jzc/pInfo->J_zzc;
		else 
			return MEANLESS_DATA;
	case QH_JSJ: //结算价
		return hqtmp.ClearPrice;
	case QH_YJSJ://期货前结算价
		return hqtmp_ex.PreClear;
	case SPREAD: //买卖差价
		return hqtmp_ex.HSpread;
	case BSUNIT:	//买卖单位
		return hqtmp_ex.PreVol;
	case AVERPRICE:	//平均价
		return hqtmp_ex.AverPrice;
	case YIELDVAL:	//收益率
		return hqtmp_ex.YieldVal;
	case HIS_HIGH:	//年最高
		return hqtmp_ex.HisHigh;
	case HIS_LOW:	//年最低
		return hqtmp_ex.HisLow;
	case IEP:		//参考价
		return hqtmp_ex.IEP;
	case IEV:		//参考量
		return hqtmp_ex.IEV;
	case MRKCAP:	//市值
		return hqtmp_ex.MrkCap;
	case PE:		//市盈率
		return hqtmp_ex.PE;
	case PREMINUM:	//溢价％
		return hqtmp_ex.PreMinum;
	case GEARING:	//杠杆比率
		return hqtmp_ex.Gearing;
	case EXECPRICE:	//行使价
		return hqtmp_ex.ExecPrice;
	case CONVRATIO:	//认购比率
		return hqtmp_ex.ConvRatio;
	case EXPIREDATE://期权到期日,单独排序
		return atol(hqtmp_ex.ExpireDate); 
	case TODAYZC:	//持仓差
		if(hqtmp.Yield_VolInStock.VolInStock>0)
			return (float)hqtmp.Yield_VolInStock.VolInStock-(long)hqtmp_ex.PreVolIn;
		return 0;
	case TODAYKC:	//今开仓	
		if(hqtmp.Volume>0)
			return (float)hqtmp.OpenVolume;
		return 0;
	case TODAYPC:	//今平仓
		if(hqtmp.Volume>0)
			return (float)hqtmp.Volume-(float)hqtmp.OpenVolume;
		return 0;
	case DDX_1:		//当日DDX
		if(pDDEReport) 
			return pDDEReport->fDDX;
		return 0;
	case DDY_1:		//当日DDY
		if(pDDEReport) 
			return pDDEReport->fDDY;
		return 0;
	case DDZ_1:		//当日DDZ
		if(pDDEReport) 
			return pDDEReport->fDDZ;
		return 0;
	case DDX_60:	//60日DDX
		if(pDDEReport) 
			return pDDEReport->fDDX60;
		return 0;
	case DDY_60:	//60日DDY
		if(pDDEReport) 
			return pDDEReport->fDDY60;
		return 0;
	case DDX_REDNUM://DDX连红天数
		if(pDDEReport) 
			return pDDEReport->DDXRed;
		return 0;
	case DDX_REDIN10://10内DDX红
		if(pDDEReport) 
			return pDDEReport->DDXRed10;
		return 0;
	case BIGIOAMOUNT://SUP净流向
		if(pDDEReport) 
			return pDDEReport->fBigIOAmount;
		return 0;
	case BIGIOWEIGHT://主力活跃度
		if(pDDEReport) 
			return pDDEReport->fBigIOWeight;
		return 0;
	case BIGBUY_RAT://大单买入%
		if(pDDEReport) 
			return pDDEReport->fBigBuyRate;
		return 0;
	case BIGSELL_RAT://大单卖出%
		if(pDDEReport) 
			return pDDEReport->fBigSellRate;
		return 0;
	case TOTALWTB:	//总买委比
		if(pDDEReport) 
			return pDDEReport->fBOrderRate;
		return 0;
	case TICK2_JL:	//逐笔均量
		if(pDDEReport) 
			return pDDEReport->fAveTradeVol;
		return 0;
	case JS_INDD:
		if(its.Minute>0)
			return its.fAmount[1][0];
		return 0;
	case JS_OUTDD:
		if(its.Minute>0)
			return its.fAmount[1][1];
		return 0;
	case JS_INZD:
		if(its.Minute>0)
			return its.fAmount[2][0];
		return 0;
	case JS_OUTZD:
		if(its.Minute>0)
			return its.fAmount[2][1];
		return 0;
	case JS_INXD:	//小单买入
		if(its.Minute>0)
			return its.fAmount[0][0]-its.fAmount[1][0]-its.fAmount[2][0];
		return 0;
	case JS_OUTXD:	//小单卖出
		if(its.Minute>0)
			return its.fAmount[0][1]-its.fAmount[1][1]-its.fAmount[2][1];
		return 0;

	case H_ZQDM:
		if(pInfo_hk)
			return g_pStockIO->GetIndex(pInfo_hk->Code,pInfo_hk->setcode);
		return 0;
	case H_ZQJC:
		if(pInfo_hk)
			return atof(pInfo_hk->Code);
		return 0;
	case H_ZJCJ:
		return hqtmp_hk.Now;
	case H_ZAF:
		ftmp=hqtmp_hk.Now;
		if (ftmp < COMPPREC)	ftmp = 0.00;
		else					ftmp = (ftmp - hqtmp_hk.Close);
		return ftmp;
	case H_CJL:
		return hqtmp_hk.Volume;
	case H_GB:
		return pInfo->J_hg;
	case H_HSL:
		if(pInfo_hk && pInfo->J_hg > 1)
			return hqtmp_hk.Volume*pInfo_hk->Unit/(pInfo->J_hg*100.0);
		return 0.00;
	case H_YJ:
		ftmp=hqtmp_hk.Now*g_gy_rate;
		if (ftmp < COMPPREC)	ftmp = 0.00;
		else					ftmp = (hqtmp.Now-ftmp);
		return ftmp;
	case H_YJL:
		ftmp=hqtmp_hk.Now*g_gy_rate;
		if (ftmp < COMPPREC)	ftmp = 0.00;
		else					ftmp = (hqtmp.Now-ftmp)/ftmp;
		return ftmp;
	case HY_STR: //所属行业
		return pInfo->J_hy;
	case ON_DXTX:
		return pInfo->CalcRes1.fOut[4];
	case ON_ZXTX:
		return pInfo->CalcRes1.fOut[5];
	default:
		return MEANLESS_DATA;
	}
}

void HQGrid::Sort(void)
{
	if(m_nColNum<1) return;
	
	int i;
	BOOL bNeedToTail = FALSE;
	nSort_Type = m_lpnCol[nCurSortIndex];

	if(nSort_Type>=TOTAL_COL)			//非动态行情排序
	{
		int nBaseRow = -1, nBaseCol = -1;
		nBaseCol = nSort_Type%TOTAL_COL;

		if(m_vBaseHeads[nBaseCol].dataType==TYPE_STRING)
		{
			for(i=0;i<nTotalRow;i++) 
			{
				Fy_nTag[i] = i;	//排序时重新开始
				MemStkInfo *pInfo = NULL;
				if(Sort_lpnUsedStkIndex[i]>=0)
				{
					pInfo = (*g_pStockIO)[Sort_lpnUsedStkIndex[i]];
					if(!pInfo) continue;
				}
				if(!pInfo)			//这一行不是股票
					nBaseRow = -1-Sort_lpnUsedStkIndex[i];
				else					//股票
					nBaseRow = Fy_nTag[i];
				if(nBaseRow<0||nBaseCol<0) continue;
				char strText[256]={0};
				GetString(strText,255,nBaseRow, nBaseCol);				
				strncpy(lpSort2[i].StrValue,strText,20);
				lpSort2[i].StrValue[20]=0;
				lpSort2[i].nIndex=Sort_lpnUsedStkIndex[i];
				lpSort2[i].tag=Fy_nTag[i];
			}
			qsort((void *) lpSort2, nTotalRow, sizeof(struct sort_struc2), sort_func2 );
			if (bAscendSort == FALSE)
			{
				for ( i=0;i < nTotalRow;i++) 
				{
					lpnUsedStkIndex[i]=lpSort2[i].nIndex;
					Fy_nTag[i]=lpSort2[i].tag;
				}
			}
			else	
			{		
				for ( i=0;i < nTotalRow;i++)
				{
					lpnUsedStkIndex[i]=lpSort2[nTotalRow-1-i].nIndex;
					Fy_nTag[i]=lpSort2[nTotalRow-1-i].tag;
				}
			}
		}
		else
		{
			for (i = 0;i < nTotalRow;i++) 
			{
				Fy_nTag[i] = i;	//排序时重新开始
				MemStkInfo *pInfo = NULL;
				if(Sort_lpnUsedStkIndex[i]>=0)
				{
					pInfo = (*g_pStockIO)[Sort_lpnUsedStkIndex[i]];
					if(!pInfo) continue;
				}
				if(!pInfo)			//这一行不是股票
					nBaseRow = -1-Sort_lpnUsedStkIndex[i];
				else					//股票
					nBaseRow = Fy_nTag[i];
				if(nBaseRow<0||nBaseCol<0) continue;
				if(nSort_Type>=TOTAL_COL)
				{
					lpSort[i].fValue = fGetFyValue(nBaseRow, nBaseCol); //取排序值
					lpSort[i].nIndex = Sort_lpnUsedStkIndex[i];
					lpSort[i].tag = Fy_nTag[i];
				}
			}
			qsort((void *) lpSort, nTotalRow, sizeof(struct sort_struc), sort_func );
			if (bAscendSort == FALSE)
			{
				for ( i=0;i < nTotalRow;i++)
				{
					lpnUsedStkIndex[i] = lpSort[i].nIndex;
					Fy_nTag[i] = lpSort[i].tag;
				}
			}
			else	
			{
				for ( i=0;i < nTotalRow;i++)
				{
					lpnUsedStkIndex[i] = lpSort[nTotalRow-1-i].nIndex;
					Fy_nTag[i] = lpSort[nTotalRow-1-i].tag;
				}
			}
			bNeedToTail = TRUE;
		}
	}
	else if(nSort_Type!=SPELL_CODE && nSort_Type!=ZQJC) //一般栏目排序
	{
		for (i = 0;i < nTotalRow;i++) 
		{
			Fy_nTag[i] = i;	//排序时重新开始
			lpSort[i].fValue = fGetSortValue(Sort_lpnUsedStkIndex[i]); //取排序值
			lpSort[i].nIndex = Sort_lpnUsedStkIndex[i];
			lpSort[i].tag = Fy_nTag[i];
		}
		qsort((void *) lpSort, nTotalRow, sizeof(struct sort_struc), sort_func );
		if (bAscendSort == FALSE)
		{
			for ( i=0;i < nTotalRow;i++)
			{
				lpnUsedStkIndex[i] = lpSort[i].nIndex;
				Fy_nTag[i] = lpSort[i].tag;
			}
		}
		else	
		{
			for ( i=0;i < nTotalRow;i++)
			{
				lpnUsedStkIndex[i] = lpSort[nTotalRow-1-i].nIndex;
				Fy_nTag[i] = lpSort[nTotalRow-1-i].tag;
			}
		}
		bNeedToTail = TRUE;
	}
	else  //股票名称为依据时，排序
	{
		if(nSort_Type==SPELL_CODE)
		{
			for(i=0;i<nTotalRow;i++) 
			{
				Fy_nTag[i] = i;	//排序时重新开始
				if(Sort_lpnUsedStkIndex[i]<0) 
				{
					lpSort2[i].StrValue[0]=0;
					lpSort2[i].nIndex=0;
				}
				else
				{
					strcpy(lpSort2[i].StrValue,(*g_pStockIO)[Sort_lpnUsedStkIndex[i]]->SpellCode);
					lpSort2[i].nIndex=Sort_lpnUsedStkIndex[i];
				}
				lpSort2[i].tag=Fy_nTag[i];
			}
		}
		else
		{
			for(i=0;i<nTotalRow;i++) 
			{
				Fy_nTag[i] = i;	//排序时重新开始
				if(Sort_lpnUsedStkIndex[i]<0) 
				{
					lpSort2[i].StrValue[0]=0;
					lpSort2[i].nIndex=0;
				}
				else
				{
					strcpy(lpSort2[i].StrValue, (*g_pStockIO)[Sort_lpnUsedStkIndex[i]]->Name); //取排序值
					lpSort2[i].nIndex=Sort_lpnUsedStkIndex[i];
				}
				lpSort2[i].tag=Fy_nTag[i];
			}
		}
		qsort((void *) lpSort2, nTotalRow, sizeof(struct sort_struc2), sort_func2 );
		if (bAscendSort == FALSE)
		{
			for ( i=0;i < nTotalRow;i++) 
			{
				lpnUsedStkIndex[i]=lpSort2[i].nIndex;
				Fy_nTag[i] = lpSort2[i].tag;
			}
		}
		else
		{
			for ( i=0;i < nTotalRow;i++)
			{
				lpnUsedStkIndex[i]=lpSort2[nTotalRow-1-i].nIndex;
				Fy_nTag[i] = lpSort2[nTotalRow-1-i].tag;
			}
		}
	}
	if(bNeedToTail)
	{
		//总是将MEANLESS_DATA丢在最后，不参与排序
		for(i = 0;i < nTotalRow && fGetSortValue(lpnUsedStkIndex[i]) != MEANLESS_DATA;i++);
		if(i < nTotalRow)
		{
			int startpos,endpos,tempIndex;
			startpos = i;
			for(i = startpos+1;i < nTotalRow && fGetSortValue(lpnUsedStkIndex[i]) == MEANLESS_DATA;i++);
			endpos = i - 1;
			for(i = endpos + 1;i < nTotalRow;i++)
			{
				tempIndex = lpnUsedStkIndex[i];
				lpnUsedStkIndex[i] = lpnUsedStkIndex[startpos+i-endpos-1];
				lpnUsedStkIndex[startpos+i-endpos-1] = tempIndex;
			}
		}
	}
	return;
}

//用鼠标换栏目顺序的变灰处理
void HQGrid::OnUpdateRightClickHead(CCmdUI* pCmdUI) 
{
	UINT nRightIDStart = ID_RIGHT_2;
	if(m_nGridType==QH_TYPE) 
		nRightIDStart = ID_QHRIGHT_2;

	int temp1 = -1;
	if(m_nGridType==QH_TYPE) 
	{
		for(int i=0;i<m_nColNum;i++)
		{
			if(g_aQHIdx_Def[i] == m_lpnCol[(RightClickWhichCol+nStartCol)])
			{	
				temp1 = i;
				break;
			}
		}
		pCmdUI->Enable(!((pCmdUI->m_nID)-nRightIDStart+2 == temp1));
		return;
	}
	for(int i=0;i<m_nColNum;i++)
	{
		if(m_lpnCol[i] == (pCmdUI->m_nID)-nRightIDStart+2)
		{	
			temp1 = i;
			break;
		}
	}
	pCmdUI->Enable(!((RightClickWhichCol+nStartCol) == temp1));
}

void HQGrid::OnUpdateRank(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_WhichGPType < FY_START);
	if(nCurSortIndex==0)
	{
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_1 );
		return;
	}
	switch(m_lpnCol[nCurSortIndex])
	{
	case ZEF://最大振幅排序
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_2 );
		break;
	case ZAF: //今日强势股和弱势股排序
		if(bAscendSort == TRUE)
			pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_3 );
		else
			pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_4 );
		break;
	case CJL://最大成交量
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_5 );
		break;
	case CJJE://最大成交金额
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_6 );
		break;
	case LIANGB://量比排名
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_7 );
		break;
	case WTB://委比排名
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_8 );
		break;
	case ZJCJ://现价排名
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_9 );
		break;
	case XS://最新大笔排名
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_10 );
		break;
	case SYL://市盈率排名
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_11 );
		break;
	case J_HSL://换手率排名
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_12 );
		break;
	case J_JZC://净资产排名
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_13 );
		break;
	case J_ZZC://总资产排名
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_14 );
		break;
	case J_MGSY://每股收益排名
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_15 );
		break;
	case J_JYL://净资产收益率排名
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_16 );
		break;
	case J_LTSZ://流通市值排名
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_17 );
		break;
	case J_LTGB://流通股本排名
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_18 );
		break;
	case J_ZSZ://AB股总市值排名
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_19 );
		break;
	case J_ZGB://总股本排名
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_20 );
		break;
	case J_ZBGJJ://公积金
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_21 );
		break;
	case HYD:
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_22 );
		break;
	case MBZL:
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_23 );
		break;
	case MBHSL:
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_24 );
		break;
	case J_MGJZC:
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_25 );
		break;
	case SJL:
		pCmdUI->SetCheck( pCmdUI->m_nID == ID_RANK_26 );
		break;
	default:
		break;
	}
}

void HQGrid::DrawBlueRect(CDC *pDC,int row,int col,float f1,int xsflag,BOOL bNarrow)
{
	int i = 0,x;
	x = GetCellLeft(col,i);
	if(bNarrow) { x+=12;i-=12; }
	if(x+i<m_DRect.right)
		g_d.Bar(pDC,x+2,nTitleHeight+row*nCellHeight+2,x+i-2,nTitleHeight+(row+1)*nCellHeight-2,INVERSE_BRUSH);
	DrawCellText(pDC,row,col,0,f1,xsflag,GridColor.Grid_CodeName_Color);
}

void HQGrid::OnRightClickHead(UINT nID)
{
	UINT nRightIDStart = ID_RIGHT_2;
	if(m_nGridType==QH_TYPE) 
		nRightIDStart = ID_QHRIGHT_2;

	int temp,temp1 = -1, tempint=-1;
	if(m_nGridType==QH_TYPE) 
	{
		for(int i=0;i<m_nColNum;i++)
		{
			if(temp1<0 && m_lpnCol[i] == g_aQHIdx_Def[nID-nRightIDStart+m_nFixedLeadCol])
				temp1 = i;
			if(tempint<0 && m_lpnCol[i] == m_lpnCol[RightClickWhichCol + nStartCol])
				tempint = i;
			if(temp1>=0&&tempint>=0) 
				break;
		}
	}
	else
	{
		for(int i = 0;i < m_nColNum;i++)
		{
			if(m_lpnCol[i] == nID-nRightIDStart+m_nFixedLeadCol )
			{
				temp1 = i;	
				break;
			}
		}
		tempint = RightClickWhichCol + nStartCol;
	}
	if(temp1 != -1)
	{	
		temp = m_lpnCol[tempint];
		if(m_nGridType==QH_TYPE) 
			m_lpnCol[tempint] = m_lpnCol[temp1];
		else 
			m_lpnCol[tempint] = nID-nRightIDStart+m_nFixedLeadCol;
		m_lpnCol[temp1] = temp;

		//重画行情视图
		if( !OnLine && nCurSortIndex != 0 && nCurSortIndex != 1)
			Sort();
		else if(BeBlock(m_WhichGPType) && nCurSortIndex != 0 && nCurSortIndex != 1)
		{
			g_nSimhq2Stk = nTotalRow;
			g_Simhq2StkIndex = lpnUsedStkIndex;
			AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_WIDEDATA_HQ);
			Sort();
		}
		bDrawBlue = FALSE;
		DrawFlag = DRAW_ALL;
		ProcessMsg(WM_PAINT);
	}
}

void HQGrid::ProcessHQRefresh()
{
	BOOL bIdle = IsIdleStatus();
	if(bIdle) return;
	if( (OnLine&&m_WhichGPType<DS_START)||(DSOnLine&&m_WhichGPType>=DS_START&&m_WhichGPType<FY_START) )
	{
		g_nPriority = 2;
		GetData();
		g_nPriority = 1;
	}
}

void HQGrid::OnCtrlLeft()
{
	if(nStartCol == 0) return;

	int xtmp,width;
	xtmp = nCellHeadWidth+GetItemWidth(m_lpnCol[0])+GetItemWidth(m_lpnCol[1]);
	for(int i = nStartCol;i >= 0&&xtmp < m_DRect.Width();i--)
	{
		width = GetItemWidth(m_lpnCol[i+m_nFixedLeadCol]);
		xtmp += width;
	}
	if(xtmp > m_DRect.Width()) nStartCol = i+1;
	else if(xtmp == m_DRect.Width()) nStartCol = i;
	else nStartCol = 0;
	DrawFlag = DRAW_NOREADDATA;
	ProcessMsg(WM_PAINT);
}

void HQGrid::ShiftGPInBlock(int DragGPNo,CPoint point)
{
	int nWhichRow = (max(0,point.y-m_DRect.top-nTitleHeight))/nCellHeight+nStartRow;
	if(nWhichRow >= nTotalRow) nWhichRow = nTotalRow-1;
	int DestGPNo = lpnUsedStkIndex[nWhichRow];
	if(DestGPNo==-1 || DragGPNo==-1 || DestGPNo==DragGPNo) return;

	if(BeUserBlock(m_WhichGPType,m_nBlockFlag))
	{
		if(m_WhichGPType==TYPE_ZXG)
			ArrangeBlockGP_InsertBefore(0,DragGPNo,DestGPNo);
		else if(m_WhichGPType==TYPE_TJG)
			ArrangeBlockGP_InsertBefore(1,DragGPNo,DestGPNo);
		else
			ArrangeBlockGP_InsertBefore(m_nBlockFlag+2,DragGPNo,DestGPNo);
		nCurRow=nWhichRow;
		ChangeGPInGrid(DragGPNo,DestGPNo,FALSE);
	}
}

void HQGrid::ToGPVip(BOOL bNewView, int flag)
{
	MemStkInfo *pInfo=NULL, *pInfoEx=NULL;
	int tempCurrentNo = GetCurrentGPIndex();
	if(tempCurrentNo==-1)	return;
	pInfo=(*g_pStockIO)[tempCurrentNo];
	if(pInfo&&flag==1)						//to港股
	{
		pInfoEx = pInfo;
		pInfo = GetLinkedHKStock(pInfoEx);
	}
	if(!pInfo) return;
	
	GetLinkedSetInfo();
	ShowGpInCom(pInfo->Code,pInfo->setcode,bNewView,lpnUsedStkIndex,nTotalRow,tempCurrentNo,&m_LSetInfo);
}

LinkedSetInfo* HQGrid::GetLinkedSetInfo()
{
	int coltype = m_lpnCol[nCurSortIndex];
	int tempSortType = (coltype == 0)?(0):(bAscendSort?1:2);

	m_LSetInfo.WhichGpMarket = m_WhichGPType;
	m_LSetInfo.ColType = coltype;
	m_LSetInfo.nStartRow = nStartRow;
	m_LSetInfo.nTolRow = nTotalRow;
	m_LSetInfo.SortType = tempSortType;
	m_LSetInfo.nValidRow = min(GRIDHQ_MAXNUM,tempRowNum);
	memcpy(m_LSetInfo.lpnUsedStkIndex, lpnUsedStkIndex, sizeof(lpnUsedStkIndex));
	m_LSetInfo.nVisualRow = nRowNum;
	m_LSetInfo.bUsed = TRUE;

	return &m_LSetInfo;
}

void HQGrid::ChangeGPType(BOOL bTurn)  //在分类股、股票池、自选股间切换
{
	if(m_bFixedGrid) return;			//固定版面不切换
	int index = m_WhichGPType;
	if(index == -1) index = 0;
	else
	{
		if(bTurn) index = (index + 1)%TOTAL_TYPENUM; //正序
		else
		{
			if(index == 0)	index = TOTAL_TYPENUM-1;
			else	index = (index - 1)%TOTAL_TYPENUM; //逆序
		}
	}
	m_WhichGPType = index;
	m_nBlockFlag = -1;
	ResetRowCol();
	SetDomain(-1,m_WhichGPType,FALSE,0,0);
}

void HQGrid::SpecialReDraw(int Flag)
{
	DrawFlag = Flag;
	bDrawBlue = FALSE;
	ProcessMsg(WM_PAINT);
	RefreshScrollBar();
}

//行情模块的消息处理----------------------------------------------
int  HQGrid::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case UM_SET_STOCK:					//切换股票到这里来
		{
			int nRet = UBase::ProcessMsg(message,wParam,lParam);
			if(m_bLcIdx)
			{

			}
		}
		break;
	case UM_GET_STOCK:
		return GetCurrentGPIndex();
	case WM_SIZE:
		{
			int x = LOWORD(wParam),y = HIWORD(wParam);
			int cx = LOWORD(lParam),cy = HIWORD(lParam);
			int tmpCy = m_Rect.Height();
			SetRect(CRect(x,y,cx+x,cy+y));
			if(cy > tmpCy)
			{
				bDrawBlue = FALSE;
				DrawFlag = DRAW_ALL;
				ProcessMsg(WM_PAINT);
			}
		}
		return 1;
	case WM_PAINT:
		{
			CDC *pDC = m_pView->GetDC();
			DrawRemoteGrid(pDC);
			m_pView->ReleaseDC(pDC);
		}
		return 1;
	case WM_COMMAND:
		if(wParam>=ID_RIGHT_HQBASE && wParam<=ID_RIGHT_FYGROUP_MAX)
		{
			int		WhichType,nBlockFlag;
			if(wParam==ID_RIGHT_HQBASE || wParam==ID_RIGHT_HQDDE) //使用上一次的类型
			{
				WhichType=-1;
				nBlockFlag=-1;
				m_nGPCode=0;
				m_bDDEStatus=(wParam==ID_RIGHT_HQDDE);
				m_bAJHStatus=(wParam==ID_RIGHT_HQAJH);
				m_bJSZJStatus=(wParam==ID_RIGHT_HQJSZJ_GG||wParam==ID_RIGHT_HQJSZJ_BK);
			}
			else if(wParam==ID_RIGHT_HQAJH)
			{
				WhichType=TYPE_AJH;
				nBlockFlag=-1;
				m_nGPCode=0;
				m_bAJHStatus=TRUE;
				m_bDDEStatus=FALSE;
				m_bJSZJStatus=FALSE;
			}
			else if(wParam==ID_RIGHT_HQJSZJ_GG||wParam==ID_RIGHT_HQJSZJ_BK)
			{
				if(wParam==ID_RIGHT_HQJSZJ_GG) 
					WhichType=TYPE_AG;
				if(wParam==ID_RIGHT_HQJSZJ_BK) 
					WhichType=TYPE_LC;
				nBlockFlag=-1;
				m_nGPCode=0;
				m_bAJHStatus=FALSE;
				m_bDDEStatus=FALSE;
				m_bJSZJStatus=TRUE;
			}
			else
			{
				WhichType=wParam-ID_RIGHT_FYGROUP+FY_START;
				nBlockFlag=g_vfyGroup[wParam-ID_RIGHT_FYGROUP].nCurSel;
				m_nGPCode=-1;
				m_bDDEStatus=m_bAJHStatus=m_bJSZJStatus=FALSE;
			}
			ResetRowCol();
			SetDomain(nBlockFlag,WhichType,FALSE,0,0);
			break;
		}
		if(wParam>=ID_RIGHT_FYBASE1 && wParam<=ID_RIGHT_FYBASE_MAX)
		{
			int WhichType,nBlockFlag;
			WhichType=(wParam-ID_RIGHT_FYBASE1)/MAX_FYNUM_IN_GROUP+FY_START;
			nBlockFlag=(wParam-ID_RIGHT_FYBASE1)%MAX_FYNUM_IN_GROUP;
			g_vfyGroup[m_WhichGPType-FY_START].nCurSel=nBlockFlag;
			ResetRowCol();
			SetDomain(nBlockFlag,WhichType,FALSE,0,0);
			break;
		}
		if(wParam>=ID_RIGHT_2 && wParam<=ID_RIGHT_49)
		{
			OnRightClickHead(wParam);
			break;
		}
		if(wParam>=ID_QHRIGHT_2 && wParam<=ID_QHRIGHT_23)
		{
			OnRightClickHead(wParam);
			break;
		}
		if(wParam>=ID_RIGHT_TYPE0 && wParam<=ID_RIGHT_TYPE20)
		{
			ResetRowCol();
			m_bAJHStatus=FALSE;
			SetDomain(-1,wParam-ID_RIGHT_TYPE0,FALSE,0,1);
			break;
		}
		if(wParam>=ID_DY1 && wParam<=ID_DY32)
		{
			ResetRowCol();
			m_bAJHStatus=FALSE;
			SetDomain(wParam-ID_DY1+DY_START,-1,FALSE,0,0);
			break;
		}
		if(wParam>=ID_HY1 && wParam<=ID_HY52)
		{
			ResetRowCol();
			m_bAJHStatus=FALSE;
			SetDomain(wParam-ID_HY1+HY_START,-1,FALSE,0,0);
			break;
		}
		if(wParam>=ID_CONCEPT_MENU_1 && wParam<=ID_CONCEPT_MENU_100)
		{
			ResetRowCol();
			m_bAJHStatus=FALSE;
			SetDomain(wParam-ID_CONCEPT_MENU_1+CON_START,-1,FALSE,0,0);
			break;
		}
		if(wParam>=ID_ZJHHY_MENU_1 && wParam<=ID_ZJHHY_MENU_200)
		{
			ResetRowCol();
			m_bAJHStatus=FALSE;
			SetDomain(wParam-ID_ZJHHY_MENU_1+ZJHHY_START,-1,FALSE,0,0);
			break;
		}
		if(wParam>=ID_ZHB_MENU_1 && wParam<=ID_ZHB_MENU_100)
		{
			ResetRowCol();
			m_bAJHStatus=FALSE;
			SetDomain(wParam-ID_ZHB_MENU_1+ZHB_START,-1,FALSE,0,0);
			break;
		}
		if(wParam>=ID_MDFHK_HY_1 && wParam<=ID_MDFHK_HY_300)
		{
			ResetRowCol();
			m_bAJHStatus=FALSE;
			SetDomain(wParam-ID_MDFHK_HY_1+HKHY_START,-1,FALSE,0,0);
			break;
		}
		if(wParam>=ID_RANK_1 && wParam<=ID_RANK_26)
		{
			ProcessMsg(WM_KEYGUY_MESSAGE,wParam-ID_RANK_1+201);
			break;
		}
		if(wParam>=ID_BLOCK_MENU_1 && wParam<=ID_BLOCK_MENU_400)
		{
			ResetRowCol();
			m_bAJHStatus=FALSE;
			SetDomain(wParam-ID_BLOCK_MENU_1,-1,FALSE,0,0);
			break;
		}
		if(wParam>=IDM_DS1 && wParam<IDM_DS1+MAX_MKT_NUM)
		{
			ResetRowCol();
			//从市场类型到本地DSMarket中的序号
			int nXH = DSGetMarketXH(wParam-IDM_DS1);
			if(nXH<0) 
				break;
			m_bAJHStatus=FALSE;
			SetDomain(-1,nXH+DS_START,FALSE,0,0);
			break;
		}
		if(wParam>=ID_FYFLAG_0 && wParam<ID_FYFLAG_END)
		{
			int nWhichType = FY_START+(wParam-ID_FYFLAG_0)/MAX_FYNUM_IN_GROUP;
			int nBlockFlag = (wParam-ID_FYFLAG_0)%MAX_FYNUM_IN_GROUP;
			ResetRowCol();
			SetDomain(nBlockFlag,nWhichType,FALSE,0,0);
			break;
		}
		if(wParam==ID_FYFLAG_END)
		{
			ResetRowCol();
			SetDomain(0,200,FALSE,0,0);
			break;
		}
		switch(wParam)
		{
		case ID_RIGHT_OPEN:
			ToGPVip(FALSE);
			break;
		case ID_RIGHT_NEWWINDOW:
			ToGPVip(TRUE);
			break;
		case ID_RIGHT_ADDBLOCK:
			Func_RightAddblock(GetCurrentGPIndex(),FALSE);
			break;
		case ID_RIGHT_ADDZXG:
			Func_RightAddZxg(GetCurrentGPIndex(),FALSE);
			ProcessMsg(WM_PAINT);
			break;
		case ID_DELFROMBLOCK:
			Func_DelFromBlock(GetCurrentGPIndex(),FALSE);
			ProcessMsg(WM_PAINT);
			break;
		case ID_RIGHT_DELETEFROMBLOCK:
			{
				int tempCurrentNo = GetCurrentGPIndex();
				if(tempCurrentNo == -1)	break;
				if(DeleteGPFromBlock(m_WhichGPType,m_nBlockFlag,tempCurrentNo))
				{
					//删除成功后，在行情视图中删除
					if(nRowNum == 1)
					{
						if(nStartRow != 0)
							nStartRow -= 1;
					}
					int i,k;
					//修改排序用的Index
					for(i = 0;nCurRow>=0 && nCurRow<nTotalRow && i<nTotalRow;i++)
						if(Sort_lpnUsedStkIndex[i]==lpnUsedStkIndex[nCurRow]) break;
					if( i < nTotalRow)
					{
						for(k = i;k<nTotalRow-1;k++)
							Sort_lpnUsedStkIndex[k] = Sort_lpnUsedStkIndex[k+1];
					}
					//再修改lpnUsedStkIndex
					if(nCurRow < nTotalRow-1)
					{
						for(i = nCurRow;i<nTotalRow-1 ;i++)
							lpnUsedStkIndex[i] = lpnUsedStkIndex[i+1];
						nTotalRow-=1;
					}
					else
					{
						nTotalRow-=1;
						if(nTotalRow <= 0)
						{
							nTotalRow = 0;
							nCurRow = 0;
						}
						else
							nCurRow = nTotalRow-1;
					}
					//
					bDrawBlue = FALSE;
					DrawFlag = DRAW_ALL;
					ProcessMsg(WM_PAINT);
				}
				break;
			}
		case ID_INBLOCK:
			{
				int tempCurrentNo = GetCurrentGPIndex();
				if(tempCurrentNo == -1)	return 1;
				AfxGetMainWnd()->SendMessage(UM_SEARCH_BLOCK,tempCurrentNo);
			}
			break;
		case ID_BATCH:
			{
				CBatchDlg dlg;
				int i,stknum = 0;
				if(nCurSortIndex==0 || BeBlock(m_WhichGPType))
				{
					if(TDX_MessageBox (m_pView->m_hWnd,"选择批量操作方式：\r\n\r\n选择[是]操作本报表中的所有股票，选择[否]仅操作本屏股票",MB_ICONQUESTION|MB_YESNO)==IDYES)
						dlg.m_bAllData=TRUE;
				}
				if(dlg.m_bAllData)
				{
					for(i=0;i <= nTotalRow-1;i++)
					{
						if(lpnUsedStkIndex[i]>=0)
							dlg.m_nStkIndex[stknum++] = lpnUsedStkIndex[i];
					}
				}
				else
				{
					for(i=nStartRow;i <= min(nEndRow,nTotalRow-1);i++)
					{
						if(lpnUsedStkIndex[i]>=0)
							dlg.m_nStkIndex[stknum++] = lpnUsedStkIndex[i];
					}
				}
				dlg.m_nStkNum = stknum;
				dlg.m_bDisableDelete = FALSE;
				dlg.m_nBlockFlag = m_nBlockFlag;
				dlg.m_WhichGPType = m_WhichGPType;
				dlg.DoModal();
				if(dlg.m_bDeleteSuccess)
				{
					ResetRowCol();
					SetDomain(m_nBlockFlag,m_WhichGPType,FALSE,0,0); //全部重来
				}
			}
			break;
		case ID_GOTOJBZL:
			{
				break;
				if(m_WhichGPType >= DS_START || m_nBlockFlag>=HKHY_START)	
					break; //大于DS_START表示是三方市场
				if(m_bDDEStatus||m_bAJHStatus)	break;
				
				if(nStartCol < ABGPHQ_COL_NUM-m_nFixedLeadCol)
					ToSomeCol(ABGPHQ_COL_NUM-m_nFixedLeadCol);	//回到财务数据的起始栏
				else
					ToSomeCol(0);				//回到行情数据的起始栏

				bDrawBlue = FALSE;
				DrawFlag = DRAW_NOREADDATA;
				ProcessMsg(WM_PAINT);
			}
			break;
		case ID_AUTOPAGE:
			if(!TestInComView2() && !m_bAutoPage && !SureAutoPage(m_pView))
				break;			
			m_bAutoPage = !m_bAutoPage;
			break;
		case ID_CHANGEGPTYPE:
			ChangeGPType(TRUE); //正序切换
			break;
		default:
			break;
		}
		return 1;
	case WM_TREE_MESSAGE:
		{
			DWORD ItemData = (DWORD)wParam;
			WORD LoWord = LOWORD(ItemData);
			WORD HiWord = HIWORD(ItemData);
			switch(HiWord)
			{
			case 0://分类股
				m_nBlockFlag = -1;
				m_WhichGPType = LoWord;
				break;
			case 1://自定义板块
				m_nBlockFlag = LoWord;
				m_WhichGPType = -1;
				break;
			case 2://地区板块
				m_nBlockFlag = DY_START+LoWord;
				m_WhichGPType = -1;
				break;
			case 3://行业板块
				m_nBlockFlag = HY_START+LoWord;
				m_WhichGPType = -1;
				break;
			default:
				m_nBlockFlag = CON_START+LoWord;
				m_WhichGPType = -1;
				break;
			}
			ResetRowCol();
			m_bAJHStatus=FALSE;
			SetDomain(m_nBlockFlag,m_WhichGPType,FALSE,0,0);
		}
		return 1;
	case WM_KEYGUY_MESSAGE:
		{
			if(wParam >=1 && wParam <= 18)
			{
				m_nBlockFlag = -1;
				m_WhichGPType = wParam - 1;	//应为wParam-1
				ResetRowCol();
				m_bAJHStatus=FALSE;
				SetDomain(m_nBlockFlag,m_WhichGPType,FALSE,0,0);
			}
			if(wParam==613)
			{
				m_nBlockFlag = -1;
				if(m_WhichGPType==TYPE_SHAG)
					m_WhichGPType=TYPE_SZAG;
				else
					m_WhichGPType=TYPE_SHAG;
				ResetRowCol();
				m_bDDEStatus=m_bAJHStatus=m_bJSZJStatus=FALSE;
				SetDomain(m_nBlockFlag,m_WhichGPType,TRUE,ZAF,TRUE);	
			}
			if((wParam >= 61 && wParam <= 69) || wParam == 611)//跨市场的涨幅排名
			{
				m_nBlockFlag = -1;
				if(wParam == 612)	//对于创业板
					m_WhichGPType = TYPE_CYB;
				else if(wParam == 611)	//对于深沪权证
					m_WhichGPType = TYPE_QZ;
				else if(wParam == 69)	//对中小企业特殊
					m_WhichGPType = TYPE_ZXB;
				else
					m_WhichGPType = wParam - 61;
				ResetRowCol();
				m_bDDEStatus=m_bAJHStatus=m_bJSZJStatus=FALSE;
				SetDomain(m_nBlockFlag,m_WhichGPType,TRUE,ZAF,TRUE);
			}
			if(wParam > 201 && wParam < 227 && m_WhichGPType < FY_START)
			{
				int tempInt;
				BOOL bAscend;
				switch(wParam)
				{
				case 202://最大振幅排序
					tempInt = ZEF;	bAscend = TRUE;break;
				case 203: //今日强势股排序
					tempInt = ZAF;	bAscend = TRUE;break;
				case 204://今日弱势股排序
					tempInt = ZAF;	bAscend = FALSE;break;
				case 205://最大成交量
					tempInt = CJL;	bAscend = TRUE;break;
				case 206://最大成交金额
					tempInt = CJJE;	bAscend = TRUE;break;
				case 207://量比排名
					tempInt = LIANGB;bAscend = TRUE;break;
				case 208://委比排名
					tempInt = WTB;	bAscend = TRUE;break;
				case 209://现价排名
					tempInt = ZJCJ;	bAscend = TRUE;break;
				case 210://最新大笔排名
					tempInt = XS;	bAscend = TRUE;break;
				case 211://市盈率排名
					tempInt = SYL;	bAscend = FALSE;break;
				case 212://换手率排名
					tempInt = J_HSL;bAscend = TRUE;break;
				case 213://净资产排名
					tempInt = J_JZC;bAscend = TRUE;break;
				case 214://总资产排名
					tempInt = J_ZZC;bAscend = TRUE;break;
				case 215://每股收益排名
					tempInt = J_MGSY;bAscend = TRUE;break;
				case 216://净资产收益率排名
					tempInt = J_JYL;bAscend = TRUE;break;
				case 217://流通市值排名
					tempInt = J_LTSZ;bAscend = TRUE;break;
				case 218://流通股本排名
					tempInt = J_LTGB;bAscend = TRUE;break;
				case 219://AB股总市值排名
					tempInt = J_ZSZ;bAscend = TRUE;break;
				case 220://总股本排名
					tempInt = J_ZGB;bAscend = TRUE;break;
				case 221://每股公积金
					tempInt = J_MGGJJ;bAscend = TRUE;break;
				case 222://活跃度
					tempInt = HYD;bAscend = TRUE;break;
				case 223://每笔均量
					tempInt = MBZL;bAscend = TRUE;break;
				case 224://每笔换手率
					tempInt = MBHSL;bAscend = TRUE;break;
				case 225://每股净资产
					tempInt = J_MGJZC;bAscend = TRUE;break;
				case 226://市净率
					tempInt = SJL;bAscend = TRUE;break;
				}
				//判断当前是否有此栏目,若没有,则返回
				int i;
				for(i=0;i<m_nColNum;i++)
				{
					if(m_lpnCol[i]==tempInt) break;
				}
				if(i>=m_nColNum)
					return 1;
				ResetRowCol();
				m_bDDEStatus=m_bAJHStatus=m_bJSZJStatus=FALSE;
				SetDomain(-1,-1,TRUE,tempInt,bAscend);
			}
			if(wParam == 201)
			{
				if(nCurSortIndex != 0)
				{
					nCurSortIndex = 0;
					ResetRowCol();
					SetDomain(m_nBlockFlag,m_WhichGPType,FALSE,0,0);
				}
			}
		}
		return 1;
	case UM_TBBD_MESSAGE:
		{
			OnlySetDomain(-1,lParam);
			if(wParam >= 201 && wParam < 227)
				ProcessMsg(WM_KEYGUY_MESSAGE,wParam,0);
		}
		return 1;
	case WM_CHANGEBLOCK:
		{
			int WhichGPType,nBlockFlag;
			nCurRow = nStartRow = nStartCol = 0;
			if(wParam==-3)	//风云板块
			{
				m_WhichGPType = g_FyLocation.nGroup+FY_START;
				m_nBlockFlag = g_FyLocation.nItem;
				nCurRow =  g_FyLocation.nRow - 1;
				nRowNum = (m_DRect.Height()-nTitleHeight)/nCellHeight;
				nStartRow = nCurRow/nRowNum*nRowNum;
				SetDomain(m_nBlockFlag,m_WhichGPType,FALSE,0,0);
				return 1;
			}
			if(wParam==-1)
			{
				nBlockFlag = -1;
				WhichGPType = TYPE_TJG;
			}
			else if(wParam==-2)
			{
				nBlockFlag = -1;
				WhichGPType = TYPE_ZXG;
			}
			else if(wParam < DY_START)	//自定义板块
			{
				nBlockFlag = wParam;
				WhichGPType = -1;
			}
			else					//地方板块与行业板块
			{
				nBlockFlag = wParam;
				WhichGPType = -1;
			}
			m_WhichGPType=WhichGPType;
			m_nBlockFlag=nBlockFlag;
			SetFYGroupActive(-1);
			SetDomain(m_nBlockFlag,m_WhichGPType,FALSE,0,0);
			int index = lParam;
			BOOL bFind = FALSE;
			for(int i = 0;i < nTotalRow;i++)
			{
				if(index == lpnUsedStkIndex[i])
				{
					bFind = TRUE;
					break;
				}
			}
			if(bFind) 
			{
				nCurRow = i;
				nRowNum = (m_DRect.Height()-nTitleHeight)/nCellHeight;
				nStartRow = nCurRow/nRowNum*nRowNum;
				DrawFlag = DRAW_ALL;
				ProcessMsg(WM_PAINT);
			}
		}
		return 1;
	case UM_BLOCKMODIFIED:
		if( BeUserBlock(m_WhichGPType,m_nBlockFlag) && nCurSortIndex==0)
		{
			ResetRowCol();
			SetDomain(m_nBlockFlag,m_WhichGPType,FALSE,0,0);
		}
		return 1;
	case UM_CHANGEGRIDWIDTH:
		memcpy(m_fpItemWidth,g_aAllItemWidth,TOTAL_COL*sizeof(float));
		ProcessMsg(UM_GRIDREINIT);
		return 1;
	case UM_GRIDREINIT:
		m_bAutoPage = FALSE;
		m_bStartDrag = FALSE;
		m_bDragging = FALSE;
		if(m_WhichGPType>=FY_START || m_WhichGPType>=DS_START) //如果是风云或扩展行情,恢复到A股行情
			m_WhichGPType=TYPE_AG;
		ResetRowCol();
		m_bDDEStatus=m_bAJHStatus=m_bJSZJStatus=FALSE;
		SetDomain(m_nBlockFlag,m_WhichGPType,FALSE,0,0);
		return 1;
	case UM_EXOPENBLOCKHQ:
		SetDomain(wParam,lParam,FALSE,0,1);
		return 1;
	case UM_HSCROLLBAR:
		OnHScrollMessage(wParam,lParam);
		return 1;
	case UM_VSCROLLBAR:
		OnVScrollMessage(wParam,lParam);
		return 1;
	case WM_CHAR:
		break;
		switch(wParam)
		{
		case '-':
			ProcessMsg(WM_COMMAND,ID_AUTOPAGE); 
			break;
		case ']':
			ChangeGPType(TRUE); //正序
			break;
		case '[':
			ChangeGPType(FALSE);//逆序
			break;
		case VK_TAB:
			if(FiltTabKeyCom()) break;
			ProcessMsg(WM_COMMAND,ID_GOTOJBZL);
			break;
		default:
			break;
		}
		return 1;
	case WM_TIMER:
		if(wParam==1)
		{
			if(g_bDowning) return 1;
			m_bRefreshDraw = TRUE;
			ProcessHQRefresh();
			m_bRefreshDraw = FALSE;
			if(GetStaticItemCount()>0)
				RefreshAllHq(120);
		}
		else if(wParam==2 && m_bAutoPage)
		{
			if(g_bDowning) return 1;
			ProcessAutoPage();
		}
		else if(wParam==3)
		{
			//重画
			CDC *pDC = m_pView->GetDC();
			DrawGrid(pDC,TRUE);
			m_pView->ReleaseDC(pDC);
		}
		else if(wParam==4)
		{
			if(g_bDowning) return 1;
			g_nSimhq2Stk = nTotalRow;
			g_Simhq2StkIndex = lpnUsedStkIndex;
			//把港股也放进去
			short nSetCode = DSGetMarketFromJc("KH");
			for(int i=0;i<nTotalRow;i++)
			{
				MemStkInfo* pCurStkInfo = (*g_pStockIO)[g_Simhq2StkIndex[i]];
				if(!pCurStkInfo) continue;
				STKRELATION  LinkData[100] = {0};
				short nLinkNum = g_pStockIO->GetRLTData(pCurStkInfo, LinkData, 100);
				for(int k=0;k<nLinkNum;k++)
				{
					if(strlen(LinkData[k].Code)<1) continue;
					if(strcmp(LinkData[k].Typestr, "H股")==0)
					{
						int nIDIndexL = g_pStockIO->GetIndex(LinkData[k].Code, nSetCode);
						if(nIDIndexL>=0)
							g_Simhq2StkIndex[g_nSimhq2Stk++] = nIDIndexL;
					}
				}
			}
			AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_WIDEDATA_HQ);
		}
		else if(wParam==5)
		{
			if(g_bDowning) return 1;
			RefreshAllHq(120);
			RefreshAllStatHq();
		}
		return 1;
	case WM_MOUSEMOVE:
		{
			UINT nFlags = wParam;
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			OnMouseMove(nFlags,pt);
		}
		return 1;
	case WM_LBUTTONUP:
		{
			UINT nFlags = wParam;
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			OnLButtonUp(nFlags,pt);
		}
		return 1;
	case WM_LBUTTONDOWN:
		{
			UINT nFlags = wParam;
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			OnLButtonDown(nFlags,pt);
		}
		return 1;
	case WM_LBUTTONDBLCLK:
		{
			UINT nFlags = wParam;
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			OnLButtonDblClk(nFlags,pt);
		}
		return 1;
	case WM_RBUTTONUP:
		{
			UINT nFlags = wParam;
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			OnRButtonUp(nFlags,pt);
		}
		return 1;
	case UM_COM2SWITCH1:
		{
			if(m_nInCom2&&theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))
			{
				short bZfIndex = lpnUsedStkIndex[0];
				if(bZfIndex>=0)
				{
					CComView2 *pView2 = (CComView2*)theViewManager.pActiveView;
					pView2->ChangeSyncUnitGP(bZfIndex);
				}
			}
		}
		return 1;
	default:
		break;
	}
	return 0;
}

void HQGrid::ChangeGPInGrid(int SrcGPNo,int DestGPNo,BOOL bShift)
{
	if(SrcGPNo==DestGPNo) return;
	int Pos1=-1,Pos2=-1;
	for(int i=0;i<nTotalRow;i++)
	{
		if(SrcGPNo==lpnUsedStkIndex[i]) Pos1=i;
		if(DestGPNo==lpnUsedStkIndex[i]) Pos2=i;
	}
	if(Pos1>=0 && Pos2>=0)
	{
		if(bShift)
		{
			if(Pos1==0 && Pos2==nTotalRow-1)
			{
				memmove(lpnUsedStkIndex,lpnUsedStkIndex+1,(nTotalRow-1)*sizeof(short));
				lpnUsedStkIndex[Pos2]=SrcGPNo;
			}
			else if(Pos1==nTotalRow-1 && Pos2==0)
			{
				memmove(lpnUsedStkIndex+1,lpnUsedStkIndex,(nTotalRow-1)*sizeof(short));
				lpnUsedStkIndex[Pos2]=SrcGPNo;
			}
			else
			{
				lpnUsedStkIndex[Pos1]=DestGPNo;
				lpnUsedStkIndex[Pos2]=SrcGPNo;
			}
		}
		else
		{
			/*
			memmove(lpnUsedStkIndex+Pos1,lpnUsedStkIndex+Pos1+1,sizeof(short)*(nTotalRow-Pos1-1));
			nTotalRow--;
			for(int i=0;i<nTotalRow;i++)
			{
				if(DestGPNo==lpnUsedStkIndex[i]) Pos2=i;
			}
			memmove(lpnUsedStkIndex+Pos2+1,lpnUsedStkIndex+Pos2,sizeof(short)*(nTotalRow-Pos2));
			lpnUsedStkIndex[Pos2]=SrcGPNo;
			nTotalRow++;
			*/
			short nTmp = lpnUsedStkIndex[Pos2];
			lpnUsedStkIndex[Pos2] = lpnUsedStkIndex[Pos1];
			lpnUsedStkIndex[Pos1] = nTmp;
		}
		//
		DrawFlag = DRAW_NOREADDATA;
		bDrawBlue = FALSE;
		ProcessMsg(WM_PAINT);	
	}
}

void HQGrid::OnShiftUp()
{
	if(!BeUserBlock(m_WhichGPType,m_nBlockFlag) || nCurSortIndex!=0) return;

	int DestGPNo,DragGPNo = lpnUsedStkIndex[nCurRow];
	if(nCurRow<nStartRow) nCurRow=nStartRow;
	if(nCurRow!=0)
	{
		if(nCurRow==nStartRow)
		{
			if(!m_bPartMode)
			{
				nStartRow--;
			}
			else
			{
				if( nStartRow - (m_DRect.Height() / nCellHeight) <= 0 )
					nStartRow = 0;
				else
					nStartRow -= m_DRect.Height() / nCellHeight ;
			}
		}
		nCurRow--;
	}
	else
	{
		nCurRow=nTotalRow-1;
		if(nRowNum != nTotalRow) 
			nStartRow = nTotalRow-nRowNum;
	}

	DestGPNo=lpnUsedStkIndex[nCurRow];
	if(DestGPNo==-1 || DragGPNo==-1 || DestGPNo==DragGPNo) return;

	if(m_WhichGPType==TYPE_ZXG)
		ArrangeBlockGP_Shift(0,DragGPNo,DestGPNo);
	else if(m_WhichGPType==TYPE_TJG)
		ArrangeBlockGP_Shift(1,DragGPNo,DestGPNo);
	else
		ArrangeBlockGP_Shift(m_nBlockFlag+2,DragGPNo,DestGPNo);
	ChangeGPInGrid(DragGPNo,DestGPNo,TRUE);
}

void HQGrid::OnShiftDown()
{
	if(!BeUserBlock(m_WhichGPType,m_nBlockFlag) || nCurSortIndex!=0) return;

	int DestGPNo,DragGPNo = lpnUsedStkIndex[nCurRow];
	if(nCurRow>nEndRow) nCurRow=nEndRow;
	if(nCurRow!=nTotalRow-1)
	{
		if(nCurRow==nEndRow)
		{
			if(!m_bPartMode)
				nStartRow++;
			else
				if(nCurRow==nEndRow)
					nStartRow=nEndRow+1;
		}
		nCurRow++;
	}
	else
	{
		nCurRow=0;
		nStartRow = 0;
	}
	
	DestGPNo=lpnUsedStkIndex[nCurRow];
	if(DestGPNo==-1 || DragGPNo==-1 || DestGPNo==DragGPNo) return;

	if(m_WhichGPType==TYPE_ZXG)
		ArrangeBlockGP_Shift(0,DragGPNo,DestGPNo);
	else if(m_WhichGPType==TYPE_TJG)
		ArrangeBlockGP_Shift(1,DragGPNo,DestGPNo);
	else
		ArrangeBlockGP_Shift(m_nBlockFlag+2,DragGPNo,DestGPNo);
	ChangeGPInGrid(DragGPNo,DestGPNo,TRUE);
}

void HQGrid::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if((nChar==VK_UP||nChar==VK_DOWN) && ::GetKeyState(VK_SHIFT)<0)
	{
		if(nChar==VK_UP)
			OnShiftUp();
		else
			OnShiftDown();
		return;
	}

	Grid::OnKeyDown(nChar, nRepCnt, nFlags);

	if(nChar == VK_RETURN)
		ToGPVip(FALSE);
	else if(nChar == VK_DELETE)
		ProcessMsg(WM_COMMAND,ID_RIGHT_DELETEFROMBLOCK);
	else if(nChar=='z'||nChar=='Z')
	{
		SHORT CtrlDown=::GetKeyState(VK_CONTROL);
		if(CtrlDown<0)
		{
			if(::GetKeyState(VK_SHIFT)<0)
				ProcessMsg(WM_COMMAND,ID_DELFROMBLOCK);
			else
				ProcessMsg(WM_COMMAND,ID_RIGHT_ADDBLOCK);
			return;
		}
	}
}

void HQGrid::PopupCornerMenu(CRect CornerRect)
{
	CNewMenu menu;
	menu.LoadMenu (IDR_LONELY_MENU);
	CNewMenu *pPopup = (CNewMenu *)menu.GetSubMenu(20);
	CPoint tempPoint(CornerRect.left,CornerRect.bottom-1);
	m_pView->ClientToScreen (&tempPoint);
	pPopup->TrackPopupMenu (TPM_LEFTALIGN,tempPoint.x,tempPoint.y,AfxGetMainWnd());
}

void HQGrid::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CClientDC dc(m_pView);
	int Width,Left;

	if(m_bCanMoveGrid)
	{
		m_bMovingGrid = TRUE;
		SetCursor(m_hMove);
		return;
	}

	OnClick (&dc,point,FALSE);

	if(point.y < m_DRect.top+nTitleHeight0)
	{
		//选某列
		for(int i = 0;i < m_nFixedLeadCol + (nEndCol-nStartCol+1);i++)   // 包括最后一列
		{
			CRgn m_rgn;
			Left = GetCellLeft((i<m_nFixedLeadCol)?i:(i+nStartCol),Width);
			m_rgn.CreateRectRgn (m_DRect.left+Left,m_DRect.top+nTitleHeight0-nCellHeight,m_DRect.left+Left+Width,m_DRect.top+nTitleHeight0);
			if(m_rgn.PtInRegion (point))
			{
				//如果点击代码行可以排序的话
				if(g_GH.bCodeCanSort && i==0 && BeBlock(m_WhichGPType))
				{
					nCurSortIndex = 0;
					bAscendSort = !bAscendSort;
					Sort();
					ToSomeRow(0);
					bDrawBlue = FALSE;
					DrawFlag = DRAW_ALL;
					ProcessMsg(WM_PAINT);
					break;
				}
				if( (nCurSortIndex != i && i < m_nFixedLeadCol) || (nCurSortIndex!=nStartCol+i && i > m_nFixedLeadCol-1) )
				{
					if(i != 0)
					{
						bAscendSort = TRUE;
						if(i > m_nFixedLeadCol-1)	nCurSortIndex = nStartCol+i;
						else	nCurSortIndex = i;
					}
					else
					{
						bAscendSort = FALSE;
						nCurSortIndex = 0;
					}
					if(nCurSortIndex==0)
					{
						ResetRowCol();
						SetDomain(m_nBlockFlag,m_WhichGPType,FALSE,0,0);
					}
					else if((!OnLine&&m_WhichGPType<DS_START) || (!DSOnLine&&m_WhichGPType>=DS_START&&m_WhichGPType<FY_START) || (m_WhichGPType>=FY_START&&m_WhichGPType<FY_START+MAX_FY_GROUP) )
						Sort();
					else if(m_bDDEStatus||m_bAJHStatus||m_bJSZJStatus||m_bSigStatus||m_nConStatus>=0)
						Sort();
					else if(BeBlock(m_WhichGPType))
					{
						g_nSimhq2Stk = nTotalRow;
						g_Simhq2StkIndex = lpnUsedStkIndex;
						AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_WIDEDATA_HQ);
						Sort();
					}
					ToSomeRow(0);
					bDrawBlue = FALSE;
					DrawFlag = DRAW_ALL;
					ProcessMsg(WM_PAINT);
				}
				else if(i != 0)
				{	
					bAscendSort =! bAscendSort;
					if((!OnLine&&m_WhichGPType<DS_START) || (!DSOnLine&&m_WhichGPType>=DS_START&&m_WhichGPType<FY_START) || (m_WhichGPType>=FY_START&&m_WhichGPType<FY_START+MAX_FY_GROUP))
						Sort();
					else if(m_bDDEStatus||m_bAJHStatus||m_bJSZJStatus||m_bSigStatus||m_nConStatus>=0)
						Sort();
					else if(BeBlock(m_WhichGPType))
					{
						g_nSimhq2Stk = nTotalRow;
						g_Simhq2StkIndex = lpnUsedStkIndex;
						AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_WIDEDATA_HQ);
						Sort();
					}
					ToSomeRow(0);
					bDrawBlue = FALSE;
					DrawFlag = DRAW_ALL;
					ProcessMsg(WM_PAINT);
				}
			}
		}
	}
	
	m_bStartDrag = FALSE;
	DraggingGPNo = GetDragItem(point);
	if(DraggingGPNo != -1 && BeUserBlock(m_WhichGPType,m_nBlockFlag) && nCurSortIndex==0)
		m_bStartDrag = TRUE;

	//如果点在左上角位图按钮上，则弹出股票类别菜单
	CRect CornerRect;
	GetCornerRect(CornerRect);
}

void HQGrid::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint pt(point);
	g_pToolTipMark->ShowTool(point, m_pView);

	m_pView->ClientToScreen(&pt);

	int nItem = -1;
	if(m_bStartDrag)
	{
		SetCursor(m_hCannotDrop);
		m_DragPoint=point;
		m_bDragging = TRUE;
		m_bStartDrag = FALSE;
		m_pView->SetCapture();
	}
	else if(m_bDragging)
	{
		CWnd* pDropWnd = m_pView->WindowFromPoint (pt);
		BOOL bCanDrop = FALSE;	//设置可Drop布尔量
		if(pDropWnd && (pDropWnd->IsKindOf (RUNTIME_CLASS (CRichEditCtrl)) || pDropWnd->IsKindOf (RUNTIME_CLASS (CComView))))
			bCanDrop = TRUE;
		if(pDropWnd == m_pView && BeUserBlock(m_WhichGPType,m_nBlockFlag))
		{
			int aRow= (m_DragPoint.y-m_DRect.top-nTitleHeight)/nCellHeight;
			int bRow= (point.y-m_DRect.top-nTitleHeight)/nCellHeight;
			if(aRow!=bRow)
				bCanDrop = TRUE;			
		}
		if(bCanDrop)
			SetCursor(m_hCanDrop);
		else
			SetCursor(m_hCannotDrop);
	}
	else if(HitTestGrid(nItem, point))
	{
		m_nMoveItem = nItem;
		m_bCanMoveGrid = TRUE;
	}
	else if(!m_bMovingGrid) 
		m_bCanMoveGrid = m_bMovingGrid = FALSE;
	if(m_bCanMoveGrid)	
	{
		SetCursor(m_hMove);
		if(m_bMovingGrid&&m_nMoveItem>=0)
		{
			int nBeforeWidth = 0;
			int nBeforeLeft = GetCellLeft(m_nMoveItem+nStartCol, nBeforeWidth);
			int nNewWidth = max((point.x-nBeforeLeft+5)/FontSize.cx, 4);
			if(m_nGridType==FY_TYPE)
			{
				int temp= nStartCol+m_nMoveItem;
				int xx=m_vBaseHeads[temp].width;
				m_vBaseHeads[temp].width=nNewWidth;
			}
			else
			{
				int temp = m_lpnCol[nStartCol+m_nMoveItem];
				if(temp<TOTAL_COL)
				{
					m_fpItemWidth[temp] = nNewWidth;
					memcpy(g_aAllItemWidth, m_fpItemWidth,TOTAL_COL*sizeof(float)); //即时改变生效
				}
				else 
				{
					int nCol = temp%TOTAL_COL;
					m_vBaseHeads[nCol].width = nNewWidth;
				}
			}
			ProcessMsg(WM_PAINT);
		}
	}
}

void HQGrid::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bCanMoveGrid||m_bMovingGrid)
		m_bCanMoveGrid = m_bMovingGrid = FALSE;
	if(m_bStartDrag) m_bStartDrag = FALSE;
	if(m_bDragging)
	{
		CPoint pt(point);
		m_pView->ClientToScreen( &pt);
		CWnd* pDropWnd = m_pView->WindowFromPoint (pt);
		if (pDropWnd && pDropWnd->IsKindOf (RUNTIME_CLASS (CRichEditCtrl)) )
		{
			if(DraggingGPNo!=-1)
			{
				TxtCurGPCode = (*g_pStockIO)[DraggingGPNo]->Code;
				TxtCurSetCode = (*g_pStockIO)[DraggingGPNo]->setcode;
				(pDropWnd->GetParent())->SendMessage(WM_RICHEDIT_MESSAGE);
			}
		}
		else if(pDropWnd && pDropWnd->IsKindOf (RUNTIME_CLASS (CComView)) )
		{
			if(DraggingGPNo!=-1)
			{
				ComCurGPCode = (*g_pStockIO)[DraggingGPNo]->Code;
				ComCurSetCode = (*g_pStockIO)[DraggingGPNo]->setcode;
				ReleaseCapture();
				m_bDragging = FALSE;
				pDropWnd->SendMessage(WM_CHANGEGP_MESSAGE);
				((CComView *)pDropWnd)->SetPageGP(lpnUsedStkIndex+nStartRow,(nTotalRow-nStartRow),DraggingGPNo);
				return;
			}
		}
		if(pDropWnd == m_pView && BeUserBlock(m_WhichGPType,m_nBlockFlag) )
		{
			if(DraggingGPNo!=-1 && abs(m_DragPoint.y-point.y)>3) //执行移位操作
				ShiftGPInBlock(DraggingGPNo,point);
		}
		ReleaseCapture();
		SetCursor(LoadCursor(NULL,IDC_ARROW));
		m_bDragging = FALSE;
	}
}

void HQGrid::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_bCanMoveGrid = m_bMovingGrid = FALSE;
	CClientDC dc(m_pView);
	//如果在行的范围中，则弹出行的右键菜单.
	OnClick (&dc,point,TRUE);
}

void HQGrid::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CRect CornerRect;
	GetCornerRect(CornerRect);
	if(point.y < CornerRect.bottom)	return;

	BOOL bToGg = FALSE;
	if(m_bAJHStatus)
	{
		int Width = 0;
		for(int i = m_nFixedLeadCol;i < m_nFixedLeadCol+(nEndCol-nStartCol+1);i++)   // 包括最后一列
		{
			int Left = GetCellLeft(i+nStartCol,Width);
			if(m_lpnCol[i+nStartCol]>=H_ZQDM&&point.x>Left)
			{
				bToGg = TRUE;
				break;
			}
		}
	}

	if(GetCurrentGPIndex() != -1)
		ToGPVip(FALSE, bToGg);
}

BOOL HQGrid::TestInComView2()
{
	BOOL bInComView2 = FALSE;
	if(m_pView->IsKindOf(RUNTIME_CLASS(CComView2)) )
		bInComView2 = TRUE;
	return bInComView2;
}

void HQGrid::GetData()
{
	if(nTotalRow<1) return;
	if(m_WhichGPType==TYPE_LC)
	{
		Sort();
		CDC *pDC = m_pView->GetDC();
		DrawGrid(pDC,TRUE);
		m_pView->ReleaseDC(pDC);
		return;
	}
	BOOL bInComView2 = TestInComView2();
	int coltype = m_lpnCol[nCurSortIndex];
	int tempSortType = (coltype == 0)?(m_WhichGPType>=DS_START&&m_WhichGPType<FY_START?1:0):(bAscendSort?1:2);
	BOOL bFillGet = TRUE;
	if(!m_bDDEStatus && !m_bAJHStatus && !m_bJSZJStatus && m_nConStatus<0 && m_WhichGPType > -1 && m_WhichGPType < TYPE_ZXG||(m_WhichGPType >= DS_START&&m_WhichGPType < FY_START))
	{
		if(coltype<ABGPHQ_COL_NUM || tempSortType==0 || coltype>=QH_JSJ)
		{
			if(m_WhichGPType>=DS_START)
			{		
				m_pView->SendMessage(UM_GETMAINID,1);
				if(bInComView2)	
					g_nAssisID = SetComAssisID(m_nWhichUnit,HQGRID_REQ+1);
				g_pGetData->DSGetMulData(g_pDSMarket[m_WhichGPType-DS_START].MktType,coltype,nStartRow,tempRowNum,tempSortType);
				bFillGet=FALSE;
			}
			else
			{
				m_pView->SendMessage(UM_GETMAINID,0);
				if(bInComView2)	
					g_nAssisID = SetComAssisID(m_nWhichUnit,HQGRID_REQ);
				g_pGetData->GetMultiCurrStockDataEx(m_WhichGPType,coltype,nStartRow,min(GRIDHQ_MAXNUM,tempRowNum),tempSortType);
				bFillGet=FALSE;
			}
		}
		else
			Sort();
	}
	if(bFillGet)
	{
		char	CodeHead[(1+SH_CODE_LEN)*GRIDHQ_MAXNUM]={0};
		MULIDX	MulIdx[GRIDHQ_MAXNUM]={0};
		int		nDSNum = 0, nABNum = 0;
		MemStkInfo *pInfo = NULL;		
		for(int i = 0,j=min(GRIDHQ_MAXNUM,tempRowNum);i < j;i++)
		{
			pInfo = (*g_pStockIO)[lpnUsedStkIndex[nStartRow+i]];
			if(!pInfo) continue;
			if(IsLocalStock(pInfo)) continue;
			if(g_pStockIO->BeDSStock(pInfo))
			{
				MulIdx[nDSNum].Market = pInfo->setcode;
				memcpy(MulIdx[nDSNum].Code,pInfo->Code,CODE_SIZE);
				nDSNum++;
			}
			else
			{
				memcpy(CodeHead+nABNum*(1+SH_CODE_LEN),&pInfo->setcode,1);
				memcpy(CodeHead+nABNum*(1+SH_CODE_LEN)+1,pInfo->Code,SH_CODE_LEN);
				nABNum++;

				if(m_bAJHStatus&&DSOnLine)
				{
					MemStkInfo *pInfo_hk = GetLinkedHKStock(pInfo);
					if(pInfo_hk&&g_pStockIO->BeDSStock(pInfo_hk))
					{
						MulIdx[nDSNum].Market = pInfo_hk->setcode;
						memcpy(MulIdx[nDSNum].Code,pInfo_hk->Code,CODE_SIZE);
						nDSNum++;
					}	
				}
			}
		}
		if(nDSNum>0)
		{
			m_pView->SendMessage(UM_GETMAINID,1);
			if(bInComView2)	g_nAssisID = SetComAssisID(m_nWhichUnit,HQGRID_REQ+1);
			g_pGetData->DSGetMulCurrData(MulIdx,nDSNum);
		}
		if(nABNum>0)
		{
			m_pView->SendMessage(UM_GETMAINID,0);
			if(bInComView2)	
				g_nAssisID = SetComAssisID(m_nWhichUnit,HQGRID_REQ);
			g_pGetData->GetMultiCurrStockDataEx(CodeHead,nABNum);
		}

		if(m_bDDEStatus||m_bAJHStatus||m_bJSZJStatus||m_bSigStatus||m_nConStatus>=0)
			Sort();
	}
	if(!m_bRefreshDraw)
		ReSetRefreshTime();
}

void HQGrid::GetDataACK(BOOL bDSData)
{
	static int s_widedata_refreshnum=0;
	static struct CurrStockDataEx StockDataExBuf[GRIDHQ_MAXNUM]={0};
	if(bDSData)
	{
		MULIDX  MulIdx[GRIDHQ_MAXNUM]={0};
		g_pGetData->DSGetMulCurrDataACK(MulIdx,GRIDHQ_MAXNUM);
		if(!BeBlock(m_WhichGPType) && g_AnsHeader3.AssisID!=JUSTCURRHQ)
		{
			MemStkInfo *pInfo = NULL;
			for(int j = 0;j < min(GRIDHQ_MAXNUM,tempRowNum);j++)
			{
				pInfo = g_pStockIO->GetDSStkInfo(MulIdx[j].Market,MulIdx[j].Code);
				if(pInfo==NULL) continue;
				lpnUsedStkIndex[nStartRow+j] = pInfo->Index;
			}
		}
	}
	else
	{
		g_pGetData->GetMultiCurrStockDataExACK(StockDataExBuf,GRIDHQ_MAXNUM);
		//重新改写lpnUsedStkIndex数组
		if(!BeBlock(m_WhichGPType))
		{
			for(int j = 0,tempIndex=0;j < min(GRIDHQ_MAXNUM,tempRowNum);j++)
			{
				tempIndex = g_pStockIO->GetIndex(StockDataExBuf[j].Code,StockDataExBuf[j].SetCode);
				lpnUsedStkIndex[nStartRow+j] = tempIndex;
			}
		}
	}
	//对于需要排序的板块类
	if((BeBlock(m_WhichGPType)||m_nConStatus>=0) && nCurSortIndex != 0 && nCurSortIndex != 1)
	{
		Sort();
		s_widedata_refreshnum++;
		if(s_widedata_refreshnum%30==0) //每4*30秒全部请求数据
		{
			g_nSimhq2Stk = nTotalRow;
			g_Simhq2StkIndex = lpnUsedStkIndex;
			AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_WIDEDATA_HQ);
		}
	}
	//重画,对于期货扩展行情,考虑到推送机制的连续性,只进行定时刷新,第一次还是要全部刷新一下
	if(bDSData && g_AnsHeader3.AssisID==JUSTCURRHQ)
		return;
	CDC *pDC = m_pView->GetDC();
	DrawGrid(pDC,TRUE);
	m_pView->ReleaseDC(pDC);
}

void HQGrid::OnGetDataACK(short nMainID,BOOL bDSData)
{
	BOOL bInComView2 = TestInComView2();
	if(bDSData)	//	三方市场
	{
		if(bInComView2)
		{
			if(BeBlock(m_WhichGPType) || nMainID==g_AnsHeader3.MainID) 
				GetDataACK(TRUE);
			return;
		}
		if(g_AnsHeader3.AssisID != HQ_ASSISID+1 && g_AnsHeader3.AssisID != JUSTCURRHQ) 
			return;
		if(BeBlock(m_WhichGPType) ||nMainID==g_AnsHeader3.MainID)
			GetDataACK(TRUE);
		if(g_AnsHeader3.Priority!=2)	//如果不是刷新行情请求，则激活下一缓冲请求
		{
			if(g_pComm3->m_HQBufFlag == 2)
			{
				g_pComm3->SendOneBuf((char*)&g_pComm3->m_HQSendBuf,
					sizeof(REQHEADER)+(g_pComm3->m_HQSendBuf).ReqHeader.PacketLen);
				g_pComm3->m_HQBufFlag = 1;
			}
			else	g_pComm3->m_HQBufFlag = 0;
		}
	}
	else
	{
		if(bInComView2)
		{
			if(BeBlock(m_WhichGPType) || nMainID==g_AnsHeader.MainID) GetDataACK();
			return;
		}
		if(g_AnsHeader.AssisID != HQ_ASSISID) return;
		if(BeBlock(m_WhichGPType) || nMainID==g_AnsHeader.MainID) 
			GetDataACK();
		if(g_AnsHeader.Priority!=2)
		{
			if(g_pComm->m_HQBufFlag == 2)
			{
				g_pComm->SendOneBuf((char*)&(g_pComm->m_HQSendBuf),sizeof(REQHEADER)+(g_pComm->m_HQSendBuf).ReqHeader.PacketLen);
				g_pComm->m_HQBufFlag = 1;
			}
			else	g_pComm->m_HQBufFlag = 0;
		}
	}
}

#include "ExportDlg.h"
void HQGrid::OnWritetotxtfile() 
{
	if(m_nGridType==FY_TYPE && !g_bDebugMode)
	{
		TDX_MessageBox (m_pView->m_hWnd,"当前画面不支持导出!",MB_ICONINFORMATION|MB_OK);
		return;
	}
	CExportDlg dlg;
	dlg.m_bGrid = TRUE;
	dlg.m_strInitFileName = _F(m_strTitle);
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.m_nTabSel == 0 || dlg.m_nTabSel == 1)
		{
			if(dlg.m_bAllHQ)	//如果导出全部,需刷新数据
				RefreshAllHq();
			m_bWriteAllHQ = dlg.m_bAllHQ;
			if(m_bWriteAllHQ) Sort();
			WriteToTxtFile(dlg.m_strFileName,dlg.m_nTabSel);
			m_bWriteAllHQ = FALSE;			
		}
		else
		{
			CRect rect;
			m_pView->GetClientRect(&rect);
			DrawFlag = DRAW_NOREADDATA;
			ProcessMsg(WM_PAINT);
			HDC hdcScreen = ::GetDC(m_pView->m_hWnd);
			HDC hdc = CreateCompatibleDC(hdcScreen);
			HBITMAP hbmp = CreateCompatibleBitmap(hdcScreen,rect.Width(),rect.Height());
			SelectObject(hdc,hbmp);
			BitBlt(hdc,0,0,rect.Width(),rect.Height(),hdcScreen,rect.left,rect.top,SRCCOPY);
			CDC *pDC = CDC::FromHandle(hdc);
			CPen *oldpen = g_d.SelectPen(pDC,AXIS_PEN);
			g_d.DrawRect(pDC,&rect);
			g_d.RestorePen(pDC,oldpen);
			SaveToBMPFile(hbmp,dlg.m_strFileName);
			DeleteDC(hdc);
			::ReleaseDC(m_pView->m_hWnd,hdcScreen);
		}
		if(TDX_MessageBox (m_pView->m_hWnd,"导出成功,是否打开此文件?",MB_ICONQUESTION|MB_OKCANCEL)==IDOK)
		{
			CString tmpStr;
			tmpStr.Format("\"%s\"",dlg.m_strFileName);
			::ShellExecute(NULL,NULL,tmpStr,NULL,NULL,SW_SHOW);
		}		
	}
}

//////////////////////////////////////////////////////////////////////////
//打印
void	HQGrid::CalcPrintPage(CDC* pDC, CPrintInfo* pInfo)
{
	GetSomeSize(pDC);

	//计算每页要打印的列数
	int nPrintStartCol=0;
	m_PColPerPage.clear();
	while(nPrintStartCol<tempColNum)
	{
		int i,j,width,nCurColNum=0;
		int xtmp = nCellHeadWidth;
		for(i = j = 0;j <= nEndCol+m_nFixedLeadCol && xtmp < m_PRect.right;i++) //因为j不是在循环内加1，则应少运行一次循环
		{
			if (i>=m_nFixedLeadCol) j = nStartCol+i+nPrintStartCol;
			else	  j = i;
			width = GetItemWidth(m_lpnCol[j]);
			xtmp+=width;
		}
		if(xtmp > m_PRect.right)
			nCurColNum = j-nStartCol-nPrintStartCol-m_nFixedLeadCol;	//顺便计算列数
		else 
			nCurColNum = j-nStartCol-nPrintStartCol-m_nFixedLeadCol+1;
		if(nCurColNum < 0) nCurColNum = 0;
		nPrintStartCol += nCurColNum;
		m_PColPerPage.push_back(nCurColNum);
	}

	//计算每页打印的行数
	BOOL RowFlag = TRUE;
	int nPTmpRowNum = (m_PRect.Height()-nTitleHeight)/nCellHeight;
	int tmpMod = (m_PRect.Height()-nTitleHeight)%nCellHeight;
	if(tmpMod < nCellHeight-2)
		RowFlag = TRUE;
	m_nPRowPerPage = nPTmpRowNum+((RowFlag==TRUE)?0:1);

	int nMaxPage = m_PColPerPage.size()*(max(tempRowNum-1,0)/m_nPRowPerPage+1);
	pInfo->SetMaxPage(nMaxPage);
}

void	HQGrid::PrintGrid(CDC *pDC, CPrintInfo* pPrintInfo)
{
	int nCurPage = pPrintInfo->m_nCurPage;
	if(nCurPage>pPrintInfo->GetMaxPage()) return;

	CFont *oldfont = g_d.SelectFont(pDC,LIST_FONT);
	CPen  *oldpen = g_d.SelectPen(pDC,GRIDLINE_PEN);
	GetSomeSize(pDC);

	short nColPageFlag = (nCurPage-1)%m_PColPerPage.size();
	short nRowPageFlag = (nCurPage-1)/m_PColPerPage.size();
	
	int i,j;

	int nPStartRow = nRowPageFlag*m_nPRowPerPage;
	int nPRowNum = m_nPRowPerPage;
	if(nPStartRow + m_nPRowPerPage >= tempRowNum)
		nPRowNum = tempRowNum-nPStartRow;
	int nPEndRow = nPStartRow + nPRowNum - 1;
	if(nPEndRow < 0) nPEndRow = -1;

	int nPStartCol = 0;
	int nPColNum = m_PColPerPage[nColPageFlag];
	for(i=0;i<nColPageFlag;i++)
		nPStartCol += m_PColPerPage[i];
	int nPEndCol = nPStartCol + nPColNum - 1;
	if(nPEndCol < 0) nPEndCol = -1;

	//画抬头
	int width, tempTitlePos = 1;	//从顶行开始画标题
	int xtmp = nCellHeadWidth;
	CString tmpTitle;
	HEAD_DATA_TYPE tmpDataType;
	CSize tmpSize;
	int nPTotalCol = 0;
	for(i=0;i<m_PColPerPage.size();i++) nPTotalCol+=m_PColPerPage[i];
	for(i = j = 0;j < nStartCol+nPTotalCol+1 && xtmp < m_PRect.right;i++) //因为j不是在循环内加1，则应少运行一次循环
	{
  		if (i>=m_nFixedLeadCol) j = i+nStartCol+nPStartCol;
		else	  j = i;
		tmpTitle = GetItemTitle(m_lpnCol[j]);
		tmpDataType = GetItemDataType(m_lpnCol[j]);
		width  = GetItemWidth(m_lpnCol[j]);
		if(width<1) continue;
		if(xtmp+width < m_PRect.right) //对于超出来的部分,不画
		{
			if((nCurSortIndex != ((i>=m_nFixedLeadCol)?(nStartCol+nPStartCol+i):i)) || (nCurSortIndex == 0) )
			{
				if(tmpDataType==TYPE_STRING)
					g_d.DisplayText(pDC,xtmp+4,tempTitlePos,GridColor.GridTitleTextColor,0,tmpTitle);
				else
					g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleTextColor,width-4,tmpTitle);
			}
			else
			{	
				if(tmpDataType==TYPE_STRING)
				{
					tmpSize=g_d.DisplayText(pDC,xtmp+1,tempTitlePos,GridColor.GridTitleTextColor,0,tmpTitle);
					g_d.DisplayText(pDC,xtmp+tmpSize.cx-3,tempTitlePos,GridColor.GridTitleArrowColor,0,(bAscendSort==TRUE)?_F("↓"):_F("↑"));
				}
				else
				{
					g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleTextColor,width-FontSize.cx-1,tmpTitle);
					g_d.DisplayText(pDC,xtmp,tempTitlePos,GridColor.GridTitleArrowColor,width+2,(bAscendSort==TRUE)?_F("↓"):_F("↑"));
				}
			}
		}
		xtmp+=width;
	}
	
	//开始画线	
	int tempWidth,tempLeft;
	int nTmpStartCol = nStartCol;
	nStartCol += nPStartCol;
	tempLeft=GetCellLeft(m_nFixedLeadCol+nTmpStartCol+nPEndCol,tempWidth);
	nStartCol = nTmpStartCol;
	
	for(i = 0;i <= nPRowNum;i++)	//画行线
	{
		g_d.DrawLine(pDC,0,nTitleHeight+i*nCellHeight-1,
			(tempColFlag==TRUE)?(tempLeft+tempWidth):(m_PRect.Width()),
			nTitleHeight+i*nCellHeight-1);
	}
	
	xtmp=nCellHeadWidth ;
	for(j=0;j<m_nFixedLeadCol+nPColNum;j++) //画列线
	{
		g_d.DrawLine(pDC,xtmp-1,0,xtmp-1,(tempRowFlag==TRUE)?(nPRowNum*nCellHeight+nTitleHeight):m_PRect.Height());//紧跟上个循环
		if(j<m_nFixedLeadCol)
			xtmp+=GetItemWidth(m_lpnCol[j]);
		else
			xtmp+=GetItemWidth(m_lpnCol[j+nStartCol+nPStartCol]);
	}
	g_d.DrawLine(pDC,xtmp-1,0,xtmp-1,(tempRowFlag==TRUE)?(nPRowNum*nCellHeight+nTitleHeight):m_PRect.Height());//紧跟上个循环,画到顶上
	
	MemStkInfo *	pInfo;
	CurrStockData	hqtmp={0};
	EXCURRDATA		hqtmp_ex={0};

	for(j = 0;j < nPRowNum;j++)
	{
		pInfo = (*g_pStockIO)[lpnUsedStkIndex[nStartRow+nPStartRow+j]];
		if(!pInfo) continue;
		g_pStockIO->ReadCurrStockData (pInfo,&hqtmp,NULL,&hqtmp_ex);
		for(i=0;i<nPColNum+m_nFixedLeadCol;i++)
		{
			if(i<m_nFixedLeadCol)
				DrawItem(pDC,j,i,2,pInfo,&hqtmp,&hqtmp_ex);
			else
			{
				int nTmpStartCol = nStartCol;
				nStartCol += nPStartCol;
				DrawItem(pDC,j,i+nStartCol,GetXSFlag(pInfo),pInfo,&hqtmp,&hqtmp_ex);
				nStartCol = nTmpStartCol;
			}
		}
	}

	//写左边序号内容
	for(i=0;nCellHeadWidth>3 && i<nPRowNum;i++)
		g_d.DisplayText(pDC,0,nTitleHeight+i*nCellHeight+(nCellHeight-FontSize.cy)/2,VipColor.TXTCOLOR,nCellHeadWidth-3,"%4d",nStartRow+nPStartRow+i+1);
	g_d.RestorePen(pDC,oldpen);
	g_d.RestoreFont(pDC,oldfont);
}

BOOL	HQGrid::HitTestGrid(int &nHitItem, CPoint point)
{
	return FALSE;
	if(m_nColNum<1) 
		return FALSE;
	if(GridColor.GridLineColor==GridColor.GridBackColor) 
		return FALSE;
	nHitItem = -1;
	if(point.y < m_DRect.top+nTitleHeight && point.y>m_DRect.top)
	{
		int nLeft ,nWidth;
		//选某列
		for(int i = 0;i < m_nFixedLeadCol + (nEndCol-nStartCol+1);i++)   // 包括最后一列
		{
			nLeft = GetCellLeft((i<m_nFixedLeadCol)?i:(i+nStartCol),nWidth);
			if(abs(point.x-nLeft-nWidth+2)<=2)
			{
				nHitItem = i;
				return TRUE;
			}
		}
	}
	return FALSE;
}

//画高亮度指示线
void HQGrid::DrawSortHighLine(CDC *pDC)
{
	if(nCurSortIndex>0)
	{
		int Width = 0;
		int LeftPos = GetCellLeft(nCurSortIndex,Width);
		int RightPos = LeftPos+Width;
		
		CRect rc;
		rc.SetRect(m_DRect.left+1+LeftPos,m_DRect.top+nSigTitleHeight,m_DRect.left+1+RightPos,m_DRect.bottom);
		
		CRect   XRc(0,0,rc.Width(),rc.Height());
		CREATE_MEMDC(XRc)
	//	MemDC.FillSolidRect(&XRc,VipColor.FLOATCURSORCOLOR);
		g_d.DotLine(&MemDC, XRc.left+1, XRc.top, XRc.left+1, XRc.bottom, VipColor.AXISCOLOR);
		g_d.DotLine(&MemDC, XRc.right-1, XRc.top, XRc.right-1, XRc.bottom, VipColor.AXISCOLOR);
		pDC->BitBlt(m_DRect.left+1+LeftPos,m_DRect.top+nSigTitleHeight,
			XRc.Width(),XRc.Height(),&MemDC,0,0,SRCINVERT);
		DELETE_MEMDC
	}
}

void	HQGrid::FiltConFlag()
{
	if(nTotalRow<1||m_nConStatus<0) return;

	int nPoolID = m_nConStatus%10;

	short lpnUsedStkIndex_Tmp[MAXGPNUM]={-1};
	memcpy(lpnUsedStkIndex_Tmp, lpnUsedStkIndex, nTotalRow*sizeof(lpnUsedStkIndex[0]));
	memset(lpnUsedStkIndex, -1, MAXGPNUM*sizeof(lpnUsedStkIndex[0]));
	int nRet = 0;
	map<float, long, greater_equal<float> >m_SortRes;
	for(int i=0;i<nTotalRow;i++)
	{
		int nIDIndex = lpnUsedStkIndex_Tmp[i];
		MemStkInfo *tempInfo = (*g_pStockIO)[nIDIndex];
		if(!tempInfo||testzs(tempInfo)) continue;
		PzCalcRes *pCalcRes = &tempInfo->CalcRes1;
		if(m_nConStatus/10==1)
			pCalcRes = &tempInfo->CalcRes2;
		if(TBE_MEANLESS(pCalcRes->fOut[nPoolID])&&pCalcRes->fOut[nPoolID]>COMPPREC)
		{
			CurrStockData iData={0};
			g_pStockIO->ReadCurrStockData(tempInfo, &iData);
			if(iData.Now>iData.Close)
				m_SortRes[100.0f*(iData.Now-iData.Close)/iData.Close] = nIDIndex;
		}
	}
	map<float, long, greater_equal<float> >::iterator iter = m_SortRes.begin();
	for(nRet=0;iter!=m_SortRes.end()&&nRet<m_nConFiltCount;iter++)
		lpnUsedStkIndex[nRet++] = (*iter).second;

	nTotalRow = nRet;
}
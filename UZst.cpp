#include "stdafx.h"
#include "Uzst.h"
#include "ComView.h"
#include "ComView2.h"
#include "SelectGPDlg.h"
#include "ZstTickDlg.h"
#include "MainFrm.h"
#include "EditTxtDlg.h"

float	ZstRatio[]={6.0f/9,3.0f/9,4.0f/8,2.0f/8,2.0f/8};
float	ZstRatio_dense[]={5.0f/9,4.0f/9,4.0f/8,2.0f/8,2.0f/8};

int		l_nLxZsNum = 15;	//需要画领先的指数代码
char *	l_LxZsCode[15] = {"000010",SH_DPCODE,SH_ADPCODE,SH_BDPCODE,"000001","000002","000003","399106","399107","399108","399101",SZ_DPCODE,SZ_ADPCODE,SZ_BDPCODE,"399004"};

UZst::UZst(LPCOMINFO pComInfo):UBase(pComInfo)
{
	if(m_nInCom2) 
		m_nStyle = ZST_BIG;
	else 
		m_nStyle = ZST_MMLD;
	m_bPlayStatus = FALSE;
	
	ResetSelfLine();
	m_bEvenSelected = FALSE;
	memset(&m_SelectInfo,0,sizeof(ZSTSELINFO));

	m_bZsLx	= FALSE;
	m_bTotalSect = TRUE;
	m_bSelected = FALSE;
	m_bDrawCross = FALSE;
	m_nTodayMin = 0;
	m_nCurMinute = 0;
	fBasePrice = 0.0;

	nSectNum = 1;
	nNowSect = 0;
	nSecLens = 0;

	m_bInSwim = FALSE;
	m_bHasCross = FALSE;
	m_bCrossMode = FALSE;
	m_CrossPoint = CPoint(0,0);
	m_RealMove = CPoint(0,0);

	m_bDataReady = FALSE;

	m_nTitle   = 0;
	m_pTitle   = NULL;
	m_nTitle2  = 0;
	m_pTitle2  = NULL;
	m_thunderposnum = 0;
	m_jumpPos = -1;
	
	//ToolTip初始化
	m_ToolTip.InitToolTip(m_pView,TTS_WITHTITLE);
	m_LevinToolTip.InitToolTip(m_pView,TTS_WITHTITLE);

	m_bManualPop = FALSE;

	m_bManyDayZST = FALSE;
	m_nZSTManyDay = 1;
	m_nRealTotalMin = GetRealTotalMinute(TimeSec);
	m_bYestodayFalg = g_bYesterdayFlag;

	//叠加股票的状态
	m_pOverlapStkInfo[0] = m_pOverlapStkInfo[1] = m_pOverlapStkInfo[2] = NULL;
	m_nOverlapNum = 0;
	if(m_nStyle<ZST_MULTI)	
	{
		if(g_Overlap[1].nAutoType==0)
		{
			m_nOverlapNum = g_Overlap[1].nNum;
			m_pOverlapStkInfo[0] = g_Overlap[1].pOverlapStk[0] = g_pStockIO->Get_MemStkInfo(g_Overlap[1].StkCode[0],g_Overlap[1].StkSetCode[0]);
			m_pOverlapStkInfo[1] = g_Overlap[1].pOverlapStk[1] = g_pStockIO->Get_MemStkInfo(g_Overlap[1].StkCode[1],g_Overlap[1].StkSetCode[1]);
			m_pOverlapStkInfo[2] = g_Overlap[1].pOverlapStk[2] = g_pStockIO->Get_MemStkInfo(g_Overlap[1].StkCode[2],g_Overlap[1].StkSetCode[2]);
		}
	}
	memset(m_OverlapHq,0,3*sizeof(CurrStockData));
	memset(m_OverlapMinuteData,0,3*1500*sizeof(MinuteData));
	memset(m_ManyDayDate,0,MAX_ZST_MANYDAY*sizeof(long));

	m_bBeginRectangle = FALSE;

	m_lLastMinVolume = 0;
	m_nLastRefreshMinute = -1;
	m_nLastAuctionMinute = -1;
	
	m_nQJPos1 = m_nQJPos2 = -1;

	m_fWarnPriceH = m_fWarnPriceL = -COMPPREC;

	memset(m_QSRelativeRect,0,sizeof(CRect));
	memset(m_RelativeRect,0,MAX_CUSWEB_NUM*sizeof(CRect));
	m_ViewZoomRect.SetRect(0, 0, 0, 0);
	m_ReportBtnRect.SetRect(0, 0, 0, 0);
	m_GubaBtnRect.SetRect(0, 0, 0, 0);
	m_FlushRealInfoRect.SetRect(0, 0, 0, 0);
	m_YMDCfgRect.SetRect(0, 0, 0, 0);
	m_TokenOnOffRect.SetRect(0, 0, 0, 0);
	m_CloseOpenRect.SetRect(0, 0, 0, 0);
	m_bFlushRealInfo	= FALSE;
	m_bShowLayout		= FALSE;
	m_bUseOpenAxisForIPO= FALSE;
	m_PeriodRc.SetRect(0,0,0,0);

	m_lCanUseVol= 0;
	m_pWTList	= NULL;
	m_pGPList	= NULL;
	m_pYmdList	= NULL;
	m_nWTNum = m_nGPNum = m_nYmdNum = 0;
	memset(m_pWTRect,0,MAX_WTCJLIST_NUM*sizeof(RECT));
	memset(m_pYmdRect,0,MAX_YMD_NUM*sizeof(RECT));
	m_nCurWT = m_nCurYmd = m_nCurCopyWT = -1;

	m_pSwitch = new CNewZfjsTab(m_pView->m_hWnd);
	m_pSwitch->SetStyle(SwitchStyle_switchbutton);
	m_pSwitch->SetFontColorSchema(VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.AXISCOLOR,VipColor.TXTCOLOR,VipColor.KNCOLOR,VipColor.LEVELCOLOR,RGB(128,0,0),12,SONG_FONT);
	m_pSwitch->Create(_T("NewZfjsTab"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),m_pView,ID_ZSTSWITCHBAR);
	m_nSwitchBtn = 0;
	m_bHasSwitch = FALSE;

	memset(m_NormalStat,0,1500*sizeof(NORMALSTAT));
	memset(m_TickStat,0,1500*sizeof(TICKSTAT));
	memset(m_pVtlData,0,MAX_AUCTIONNUM*sizeof(VTLDATA));
	m_nLeadData	= 0;
	m_bLeadReqed= FALSE;
	m_nNormalStat	= 0;
	m_nTickStat	= 0;
	m_nVtlData	= 0;
	
	m_bKeyCross = FALSE;
	m_nKeyMin	= 0;
}

UZst::~UZst()
{
	if(m_nStyle<ZST_MULTI && m_lDate == 0)
	{
		if(!m_nInCom2 && m_nWhichUnit==0)
		{
			if(g_Overlap[1].nAutoType==0)
			{
				g_Overlap[1].nNum = m_nOverlapNum;
				for(int i=0;i<m_nOverlapNum;i++)
				{
					if(m_pOverlapStkInfo[i])
					{
						g_Overlap[1].StkSetCode[i] = m_pOverlapStkInfo[i]->setcode;
						strncpy(g_Overlap[1].StkCode[i],m_pOverlapStkInfo[i]->Code,SH_CODE_LEN);
					}
				}
			}
		}
	}
	TDEL(m_pSwitch);
	TDEL(m_pWTList);
	TDEL(m_pGPList);
	TDEL(m_pYmdList);
}

BOOL CanShowLevinInfo()
{
	if(!TC_GetJyStatus_) return FALSE;
	DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
	return g_bHasAdvJy && (dwStatus&JY_LOGINED);
}

int  UZst::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	//如果Unit为空，则另行处理
 	if(m_nCurGP==-1)
	{
		if(message==WM_PAINT)
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);

			CPen  *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
			if((m_nInitPad&0xFF0000) == MULZST_PAD)
				g_d.DrawRect(&m_MyDC,&m_rcIn);
			g_d.RestorePen(&m_MyDC,oldpen);
			//从内存DC拷贝到屏幕DC
			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY);
			DESTORY_TMPDC
			return 1;
		}
		return UBase::ProcessMsg(message,wParam,lParam);
	}
	switch(message)
	{
	case WM_SIZE:
		if(m_nStyle<ZST_MULTI)
		{
			SetSwitchLabel();
			int x = LOWORD(wParam),y = HIWORD(wParam);
			int cx = LOWORD(lParam),cy = HIWORD(lParam);
			if(m_bHasSwitch)
			{
				SetRect(x,y,x+cx,y+cy-SWITCH_HEIGHT2);
				m_pSwitch->SetWindowPos(0,left,bottom,right-left,SWITCH_HEIGHT2,SWP_SHOWWINDOW);
			}
			else
				SetRect(x,y,x+cx,y+cy);
			m_rcIn.CopyRect(this);
			m_rcIn.OffsetRect(-left,-top);
			return 1;
		}
		break;
	case UM_READDATA:
		m_bDataReady = FALSE;
		SetSwitchLabel();
		if(m_bHasSwitch)
			m_pSwitch->SetWindowPos(0,left,bottom,right-left,SWITCH_HEIGHT2,SWP_SHOWWINDOW);
		ReadData(FALSE,wParam==0);
		return 1;
	case UM_GETDATA_ACK://对于远程版,请求和返回分开
		if(wParam>=100) return 1;
		switch(wParam)
		{
		case 0:
			m_bDataReady = TRUE;
			ReadData(TRUE);
			break;
		case ZST_LB:
			if(m_bZs)
			{
				m_nLeadData = m_nTodayMin;
				g_pGetData->GetLeadACK(m_pStkInfo,m_nLeadData,m_LBdata);
				m_bLeadReqed= TRUE;
			}
			CalcIt();
			break;
		case ZST_MMLD:
			g_pGetData->GetMmldACK(m_nTodayMin,m_MinuteData);
			CalcIt();
			break;
		case ZST_AUCTION:
			g_pGetData->GetAuctionACK(m_nVtlData,m_pVtlData);
			if(m_nVtlData < 0) m_nVtlData = 0;
			if(m_nVtlData > MAX_AUCTIONNUM) m_nVtlData = MAX_AUCTIONNUM;
			CalcIt();
			break;
		case ZST_TOTALORDER:
			m_nNormalStat = m_nTodayMin;
			g_pGetData->GetZmzmACK(m_nNormalStat,m_NormalStat);
			m_dTotalOrder.SetData(m_NormalStat,m_nNormalStat);
			CalcIt();
			break;
		case ZST_TOTALCANCEL:
			m_nNormalStat = m_nTodayMin;
			g_pGetData->GetJgjcACK(m_nNormalStat,m_NormalStat);
			m_dCancelOrder.SetData(m_NormalStat,m_nNormalStat);
			CalcIt();
			break;
		case ZST_CANCELSUM:
			m_nNormalStat = m_nTodayMin;
			g_pGetData->GetJgjcACK(m_nNormalStat,m_NormalStat);
			m_dCancelOrder.SetData(m_NormalStat,m_nNormalStat);
			CalcIt();
			break;
		case ZST_TICKNUM:
			m_nTickStat = m_nTodayMin;
			g_pGetData->GetCjbsACK(m_nTickStat,m_TickStat);
			m_dCjbsStat.SetData(m_TickStat,m_nTickStat);
			CalcIt();
			break;
		case ZST_LARGETICK:
			m_nTickStat = m_nTodayMin;
			g_pGetData->GetCjbsACK(m_nTickStat,m_TickStat);
			m_dCjbsStat.SetData(m_TickStat,m_nTickStat);
			CalcIt();
			break;
		case ZST_IOVOL:
			m_nTickStat = m_nTodayMin;
			g_pGetData->GetCjtjACK(m_nTickStat,m_TickStat);
			m_dCjtjStat.SetData(m_TickStat,m_nTickStat);
			CalcIt();
			break;
		case ZST_LARGEVOL:
			m_nTickStat = m_nTodayMin;
			g_pGetData->GetCjtjACK(m_nTickStat,m_TickStat);
			m_dCjtjStat.SetData(m_TickStat,m_nTickStat);
			CalcIt();
			break;
		}
		if(wParam >= 30 && wParam < 34)
		{
			g_pGetData->GetMinuteDataACK(m_pOverlapStkInfo[wParam-30],m_OverlapMinuteData[wParam-30],NULL);
			//重新组织Minute,容错
			for(int i=0;i<m_nTodayMin;i++)
				m_OverlapMinuteData[wParam-30][i].Minute = GetMinuteFromFZ(i,1,TimeSec);
			for(i = 1;i < m_nTodayMin;i++)
			{
				if(m_OverlapMinuteData[wParam-30][i].Now < COMPPREC)
					m_OverlapMinuteData[wParam-30][i].Now = m_OverlapMinuteData[wParam-30][i-1].Now;
			}
		}
		else if(wParam >= 35 && wParam < 40)
		{
			float fIndexClose = 1;
			g_pGetData->GetHisMinuteDataACK(m_pOverlapStkInfo[wParam-35],m_OverlapMinuteData[wParam-35],&fIndexClose);
			m_OverlapHq[wParam-35].Close = fIndexClose;
			//重新组织Minute,容错
			for(int i=0;i<m_nRealTotalMin;i++)
				m_OverlapMinuteData[wParam-35][i].Minute = GetMinuteFromFZ(i,1,TimeSec);
			for(i = 1;i < m_nRealTotalMin;i++)
			{
				if(m_OverlapMinuteData[wParam-35][i].Now < COMPPREC)
					m_OverlapMinuteData[wParam-35][i].Now = m_OverlapMinuteData[wParam-35][i-1].Now;
			}
		}
		else if(wParam >= 40 && wParam < 50)
		{
			g_pGetData->DSGetMinDataACK(m_OverlapMinuteData[wParam-40]);
			//重新组织Minute,容错
			for(int i=0;i<m_nTodayMin;i++)
				m_OverlapMinuteData[wParam-40][i].Minute = GetMinuteFromFZ(i,1,TimeSec);
			for(i = 1;i < m_nTodayMin;i++)
			{
				if(m_OverlapMinuteData[wParam-40][i].Now < COMPPREC)
					m_OverlapMinuteData[wParam-40][i].Now = m_OverlapMinuteData[wParam-40][i-1].Now;
			}
		}
		else if(wParam >= 50 && wParam < 80)
		{
			if(m_bDSFlag) 
			{
				g_pGetData->DSGetHisMinDataACK(m_ManyDayMinuteData[wParam-50],&(m_fManyDayClose[wParam-50]));
				MinuteData *pMinuteData = m_ManyDayMinuteData[wParam-50];
				float fClose = m_fManyDayClose[wParam-50];
				//重新计算Minute,因为由于时间不匹配原因,Minute不准确
				for(int i=0;i<m_nRealTotalMin;i++)
					pMinuteData[i].Minute = GetMinuteFromFZ(i,1,TimeSec);
				//平滑
				if(pMinuteData[0].Now < COMPPREC)
				{
					memset(pMinuteData,0,sizeof(MinuteData));
					pMinuteData[0].Minute = GetMinuteFromFZ(0,1,TimeSec);
					pMinuteData[0].Now = fClose;
					pMinuteData[0].Average = fClose;
				}
				if(pMinuteData[0].Average < COMPPREC)
					pMinuteData[0].Average = fClose;
				for(i = 1;i < m_nRealTotalMin;i++)
				{
					if(pMinuteData[i].Now < COMPPREC)
					{
						memset(pMinuteData+i,0,sizeof(MinuteData));
						pMinuteData[i].Minute = GetMinuteFromFZ(i,1,TimeSec);
						pMinuteData[i].Now = pMinuteData[i-1].Now;
						pMinuteData[i].Average = pMinuteData[i-1].Average;
					}
					if(pMinuteData[i].Average < COMPPREC)
						pMinuteData[i].Average = pMinuteData[i-1].Average;
					if(m_pStkInfo->bigmarket==QH_TYPE && pMinuteData[i].VolInStock_Lead.VolInStock<1)
						pMinuteData[i].VolInStock_Lead.VolInStock = pMinuteData[i-1].VolInStock_Lead.VolInStock;
				}
			}
			else 
				g_pGetData->GetHisMinuteDataACK(m_pStkInfo,m_ManyDayMinuteData[wParam-50],&(m_fManyDayClose[wParam-50]),m_ManyDayDate[wParam-50]);			
			for(int i=0;i < m_nRealTotalMin;i++)
			{
				if(m_bZs)
					m_ManyDayMinuteData[wParam-50][i].NowVol /= 100; //将成交金额变成以万为单位的数值
				if(i>0 && m_ManyDayMinuteData[wParam-50][i].Average < COMPPREC)
					m_ManyDayMinuteData[wParam-50][i].Average = m_ManyDayMinuteData[wParam-50][i-1].Average;
			}
		}
		CalcIt();
		ProcessMsg(WM_PAINT);
		return 1;
	case WM_TIMER:
		{
			if(wParam==0x4445 && g_nFlushRealInfo==1)
			{
				m_bFlushRealInfo=!m_bFlushRealInfo;
				ProcessMsg(WM_PAINT);
				return 1;
			}
			int tmpSeverTotalMin = 0;
			int tmpLocalTotalMin = GetHMS();
			if(m_bDSFlag)
				tmpSeverTotalMin = GetDSMinute(m_pTimer,m_bYestodayFalg);
			else 
				tmpSeverTotalMin = GetTdxMinute(TimeSec);
			//过一分钟后，重新计算上一次的量
			if(m_nLastRefreshMinute!=-1 && ( tmpSeverTotalMin-m_nLastRefreshMinute>0
				|| (tmpLocalTotalMin-m_nLastAuctionMinute>15&&m_nStyle==ZST_AUCTION) ))
			{
				if(tmpSeverTotalMin-m_nLastRefreshMinute>0)
				{
					m_nLastRefreshMinute = tmpSeverTotalMin;
					if(m_bZs)
						m_lLastMinVolume=(long)(m_Hq.Amount/10000.0);
					else
						m_lLastMinVolume = m_Hq.Volume;
				}
				else m_nLastAuctionMinute = tmpLocalTotalMin;
				ProcessMsg(UM_UPDATE);
				//定时请求DUnit的数据
				m_pView->SendMessage(UM_GETMAINID);
				if(OnLine && !m_bDSFlag)
				{
					if(m_nStyle==ZST_AUCTION)
					{
						g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_AUCTION);
						g_pGetData->GetAuction(m_pStkInfo,0,MAX_AUCTIONNUM);
					}
					else if(m_nStyle==ZST_TOTALORDER)	//总买总卖
					{
						g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_TOTALORDER);
						g_pGetData->GetZmzm(m_pStkInfo);
					}
					else if(m_nStyle==ZST_TOTALCANCEL)	//净挂净撤
					{
						g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_TOTALCANCEL);
						g_pGetData->GetJgjc(m_pStkInfo);
					}
					else if(m_nStyle==ZST_CANCELSUM)	//撤单累计
					{
						g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_CANCELSUM);
						g_pGetData->GetJgjc(m_pStkInfo);
					}
					else if(m_nStyle==ZST_TICKNUM)		//涨跌动因统计
					{
						g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_TICKNUM);
						g_pGetData->GetCjbs(m_pStkInfo);
					}
					else if(m_nStyle==ZST_LARGETICK)	//大单成交笔数统计
					{
						g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_LARGETICK);
						g_pGetData->GetCjbs(m_pStkInfo);
					}
					else if(m_nStyle==ZST_IOVOL)		//交量统计
					{
						g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_IOVOL);
						g_pGetData->GetCjtj(m_pStkInfo);
					}
					else if(m_nStyle==ZST_LARGEVOL)		//大单交量统计
					{
						g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_LARGEVOL);
						g_pGetData->GetCjtj(m_pStkInfo);
					}
				}
			}
		}
		return 1;
	case ASYN_HQ_MESSAGE:
		{
			if(!m_pStkInfo)	
				return 1;
			short nTmpCurGp = g_pStockIO->GetIndex(m_pStkInfo->Code, m_pStkInfo->setcode);
			if(wParam == nTmpCurGp)
			{
				int tmpTotalMin = 0;
				if(m_bDSFlag) tmpTotalMin = GetDSMinute(m_pTimer,m_bYestodayFalg);
				else tmpTotalMin = GetTdxMinute(m_Domain==SZ?sz:sh);				//再次判断是否过了1分钟
				if(m_nLastRefreshMinute!=-1 && tmpTotalMin-m_nLastRefreshMinute > 0)//过一分钟后，重新计算上一次的量
				{
					m_nLastRefreshMinute = tmpTotalMin;
					if(m_bZs)
						m_lLastMinVolume=(long)(m_Hq.Amount/10000.0);
					else
						m_lLastMinVolume = m_Hq.Volume;				
				}
				ProcessMsg(UM_UPDATE);
			}
		}
		return 1;
	case UM_UPDATE:
		{
			if(m_lDate||!m_bDataReady)	
				return 1;
			g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,NULL,&m_HqEx);
			if(m_bDSFlag) 
			{
				m_bYestodayFalg = (m_HqEx.HqDate<GetDSCorrespondingDate(m_pTimer));
				m_nTodayMin = GetDSMinute(m_pTimer, m_bYestodayFalg);
			}
			else m_nTodayMin=GetTdxMinute(TimeSec);
			if (m_Hq.Close<COMPPREC && m_Hq.Open>COMPPREC)
				m_Hq.Close=m_Hq.Open;
			SetBasePrice();
			for(int kk=0;kk < m_nOverlapNum;kk++)
			{
				g_pStockIO->ReadCurrStockData(m_pOverlapStkInfo[kk],&m_OverlapHq[kk]);
				if (m_OverlapHq[kk].Close<COMPPREC && m_OverlapHq[kk].Open>COMPPREC)
					m_OverlapHq[kk].Close=m_OverlapHq[kk].Open;
			}
			DTWH_currMinute();
			if(!m_bPlayStatus)
			{
				m_nCurMinute = m_nTodayMin;
				CalcIt();
				ProcessMsg(WM_PAINT);
			}
		}
		return 1;
	case WM_PAINT:
		{
			if(wParam == 1)
				ProcessMsg(UM_READDATA,lParam);
			else
			{
				if(m_bDrawLine || (m_SelectInfo.bExist && m_SelectInfo.IsMovable))
					break;
			}

			if(m_bBeginRectangle && abs(m_OrgPoint.x-m_ArchorPoint.x)>2*SELECT_ON) 
				break;

			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			m_MyDC.SetBkMode(TRANSPARENT);

			CFont *oldfont = g_d.SelectFont(&m_MyDC,GRAPH_FONT);
			CPen  *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
			if((m_nInitPad&0xFF0000) == MULZST_PAD)
				g_d.DrawRect(&m_MyDC,&m_rcIn);
			if( g_GH.bLeftHQInfo && !m_nInCom2 && (m_nStyle==ZST_SMALL||m_nStyle==ZST_GPZS) )
				g_d.DrawLine(&m_MyDC,m_rcIn.right-1,0,m_rcIn.right-1,m_rcIn.bottom);			
			DrawIt(&m_MyDC);
			//如果只处理三方行情，则对AB股行情进行提示
			if(g_GC.bOnlyHK && !m_nInCom2 && !m_bDSFlag && m_nStyle<ZST_MULTI)
				DrawNoABHintInfo(&m_MyDC,m_rcIn);
			DrawQJLine(&m_MyDC);
			g_d.RestorePen(&m_MyDC,oldpen);
			g_d.RestoreFont(&m_MyDC,oldfont);

			//如果需要，顺便画信息区
			CPoint TmpPoint=m_CrossPoint;
			m_CrossPoint=CPoint(0,0);
			if(m_bHasCross)
			{
				CRect rect(xStart,yStart,xEnd,yEndBot);
				if(rect.PtInRect(TmpPoint))
					InSwimArea(&m_MyDC,TmpPoint,FALSE);
				else
					LeaveSwimArea(&m_MyDC);
			}

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC

			if(m_bDataReady)
				ShowSelfLine(pDC);

			DESTORY_TMPDC
		}
		return 1;
	case UM_CALCANDPAINT:
		CalcIt();
		ProcessMsg(WM_PAINT);
		return 1;
	case UM_MANYDAYZST:
		ProcessMsg(UM_READDATA);
		ProcessMsg(WM_PAINT);
		return 1;
	case UM_OVERLAP:
		{
			if(g_Overlap[1].nNum<1&&g_Overlap[1].nAutoType!=0)
			{
				if(TDX_MessageBox(m_pView->m_hWnd,"是否删除自动叠加的股票?",MB_YESNO|MB_ICONQUESTION) == IDNO)
					return 1;
				g_Overlap[1].nAutoType = 0;
				g_Overlap[1].nNum = m_nOverlapNum = 0;
				memset(m_OverlapHq,0,3*sizeof(CurrStockData));
				memset(m_OverlapMinuteData,0,3*1500*sizeof(MinuteData));
				ProcessMsg(WM_PAINT);
			}
			if(m_nOverlapNum >= 3) 
			{
				TDX_MessageBox(m_pView->m_hWnd,"最多只能叠加三只品种!",MB_OK|MB_ICONEXCLAMATION);
				return 1;
			}
			int iGPNo = -1,i;
			CSelectGPDlg dlg;//打开选择股票对话框
			dlg.m_pStkInfo=m_pStkInfo;
			dlg.bSingleSelect = TRUE;
			dlg.bOverlapGpType = 1;//叠加类型
			if(m_bDSFlag && m_pStkInfo->bigmarket==QH_TYPE)
				dlg.bOverlapGpType = 4;//叠加类型
			dlg.bCurDomain = m_Domain;
			if(dlg.DoModal() == IDOK)
			{
				if(dlg.SelectedGPNum > 0)
					iGPNo = dlg.SelectedGPIndex[0];
				if(iGPNo!=-1)
				{
					MemStkInfo *tmpInfo = (*g_pStockIO)[iGPNo];
					if(IsSameOpenCloseTimer(m_pStkInfo,tmpInfo))
					{
						for(i = 0;i < m_nOverlapNum;i++) //找找看有没有重复的叠加股票
							if(m_pOverlapStkInfo[i]==tmpInfo) break;
						if(i >= m_nOverlapNum)
						{
							g_Overlap[1].pOverlapStk[m_nOverlapNum] = m_pOverlapStkInfo[m_nOverlapNum] = tmpInfo;
							g_Overlap[1].StkSetCode[m_nOverlapNum] = tmpInfo->setcode;
							strncpy(g_Overlap[1].StkCode[m_nOverlapNum], tmpInfo->Code, SH_CODE_LEN);
							g_Overlap[1].nNum++;
							m_nOverlapNum++;
							ProcessMsg(UM_READDATA);				
							ProcessMsg(WM_PAINT);
						}
					}
					else
						TDX_MessageBox(m_pView->m_hWnd,IDS_STR_52,MB_OK|MB_ICONERROR);
				}
			}
		}
		return 1;
	case UM_AUTOVERLAP:
		AutoOverlapGP();
		ProcessMsg(UM_READDATA);				
		ProcessMsg(WM_PAINT);
		break;
	case UM_UNOVERLAP:
		if(m_nOverlapNum > 0)
		{
			m_nOverlapNum = 0;
			memset(&g_Overlap[1], 0, sizeof(OverlapInfo));
			memset(m_OverlapHq,0,3*sizeof(CurrStockData));
			memset(m_OverlapMinuteData,0,3*1500*sizeof(MinuteData));
			ProcessMsg(UM_READDATA);
			ProcessMsg(WM_PAINT);
		}
		return 1;
	case WM_KEYDOWN:
		{
			if(m_nStyle>=ZST_MULTI) break;
			UINT nChar = (UINT)wParam;
			SHORT CtrlDown =::GetKeyState(VK_CONTROL);
			SHORT ShiftDown=::GetKeyState(VK_SHIFT);
			int nWantManyDay = 0;
			switch(nChar)
			{
			case VK_DOWN:
				{
					nWantManyDay = 1;
					if(CtrlDown<0)
					{
						int nManyParam = nWantManyDay+m_nZSTManyDay+ID_MANYDAYZST0;
						if(m_nZSTManyDay==1&&!m_bManyDayZST) nManyParam = ID_MANYDAYZST0+1;
						nManyParam = min(nManyParam, ID_MANYDAYZST9);
						nManyParam = max(nManyParam, ID_MANYDAYZST0);
						if(nManyParam<=ID_MANYDAYZST9)
							m_pView->SendMessage(WM_COMMAND, nManyParam);	
					}
				}
				break;
			case VK_UP:
				{
					nWantManyDay = -1;
					if(CtrlDown<0)
					{
						int nManyParam = nWantManyDay+m_nZSTManyDay+ID_MANYDAYZST0;
						nManyParam = min(nManyParam, ID_MANYDAYZST9);
						nManyParam = max(nManyParam, ID_MANYDAYZST0);
						if(nManyParam>=ID_MANYDAYZST0)
							m_pView->SendMessage(WM_COMMAND, nManyParam);	
					}
				}
				break;
			case VK_SPACE:
				{
					break;
					int nNowPos = -1;
					if(m_bCrossMode)
						nNowPos = m_CrossPoint.x;
					else nNowPos = m_ArchorPoint.x;

					if(m_nQJPos1<0)
					{
						m_nQJPos2 = -1;
						m_nQJPos1 = nNowPos;
					}
					else if(m_nQJPos1>=0&&m_nQJPos2<0)
						m_nQJPos2 = nNowPos;
					else 
					{
						m_nQJPos2 = -1;
						m_nQJPos1 = nNowPos;
					}

					CDC *pDC = m_pView->GetDC();
					DrawQJLine(pDC);
					
					if(m_nQJPos1>0&&m_nQJPos2>0)
					{
						if(OnLine)
						{
							if(!m_bDSFlag)
							{
								BYTE nStartDate, nEndDate;
								short nStartMinXh, nEndMinXh;
								int  xPos = max(m_nQJPos1, m_nQJPos2);
								GetPosInfo(xPos, nStartDate, nStartMinXh);
								xPos = min(m_nQJPos1, m_nQJPos2);
								GetPosInfo(xPos, nEndDate, nEndMinXh);
								
								g_pTickZstjDlg = new ZstTickDlg;
								((ZstTickDlg *)g_pTickZstjDlg)->m_pUZst = this;
								((ZstTickDlg *)g_pTickZstjDlg)->m_nCurGPNo = m_nCurGP;
								((ZstTickDlg *)g_pTickZstjDlg)->m_nCurDate = (m_lDate==0?g_lOpenRq:m_lDate);
								((ZstTickDlg *)g_pTickZstjDlg)->m_nStartDate = nStartDate;
								((ZstTickDlg *)g_pTickZstjDlg)->m_nStartMinuteXh = nStartMinXh-1;
								((ZstTickDlg *)g_pTickZstjDlg)->m_nEndDate = nEndDate;
								((ZstTickDlg *)g_pTickZstjDlg)->m_nEndMinuteXh = nEndMinXh-1;
								((ZstTickDlg *)g_pTickZstjDlg)->DoModal();
								TDEL(g_pTickZstjDlg);
							}
						}
						else
							TDX_MessageBox(m_pView->m_hWnd,"请先连接行情主站!",MB_OK|MB_ICONERROR);
						
						m_nQJPos1 = m_nQJPos2 = -1;
						ProcessMsg(WM_PAINT);
					}
				}
				break;
			case VK_LEFT:
				if(ShiftDown<0)
				{					

				}
				else
				{
					if(CtrlDown<0)	
						ToNextMin(0,5);
					else			
						ToNextMin(0,1);
				}
				break;
			case VK_RIGHT:
				if(ShiftDown<0)
				{

				}
				else
				{
					if(CtrlDown<0)	
						ToNextMin(1,5);
					else			
						ToNextMin(1,1);
				}
				break;
			case VK_HOME:
				ToNextMin(2);
				break;
			case VK_END:
				ToNextMin(3);
				break;
			case VK_ESCAPE://如果Escape被处理，则返回1,否则返回0
				//删除ToolTip
				m_ToolTip.DelToolTip(m_pView);
				m_LevinToolTip.DelToolTip(m_pView);

				m_nQJPos1 = m_nQJPos2 = -1;
				if(m_bHasCross && m_nStyle<ZST_MULTI)
				{
					m_bCrossMode = FALSE;
					m_bHasCross = FALSE;
					ProcessMsg(WM_PAINT);
					return 1;
				}
				else
					return 0;
			case VK_RETURN:
				if(GetKeyState(VK_SHIFT) & 0xFF00)
				{
					m_bManualPop = TRUE;
					ProcessMsg(UM_POPINFODLG,0);
					m_bManualPop = FALSE;
					return 1;
				}
				if(!m_bHasCross && !m_nInCom2 && !m_nInHisDlg) //进入分析图
				{
					g_pMainWin->PostMessage(WM_COMMAND,ID_300);
					return 1;
				}
				if(m_bManyDayZST && m_CrossPoint.x < xStart+m_nZSTManyDay*m_nRealTotalMin*1.0*(xEnd-xStart)/nSecLens)
					return 1;
				if(m_bHasCross && m_CrossPoint!=CPoint(0,0))
				{
					int NowAbsTime=GetTimeFromX(m_CrossPoint.x);
					m_pView->SendMessage(UM_REFLECT,Reflect_ToZstime,NowAbsTime);
				}
				return 1;
			case VK_DELETE:
				if(m_SelectInfo.bExist)
				{
					SetSelfLine(ID_DEL);
					m_SelectInfo.bExist = 0;
					break;
				}
				break;
			default:				
				break;
			}
		}
		break;
	case WM_CHAR:
		{
			if(m_nStyle>=ZST_MULTI) return 1;
			UINT nChar = (UINT)wParam;
			switch(nChar)
			{
			case '/':
				ProcessMsg(UM_SWITCHSTYLE,0);
				return 1;
			case '*':
				ProcessMsg(UM_SWITCHSTYLE,1);
				return 1;
			case ';':
				m_pView->PostMessage(WM_COMMAND,ID_TPAXIS);
				return 1;
			default:
				break;
			}
		}
		break;
	case UM_MOUSELEAVE:
		{
			if(m_bBeginRectangle)
			{
				m_bBeginRectangle = FALSE;
				ProcessMsg(WM_PAINT);
			}
			LeaveSwimArea(NULL);			
		}
		return 1;
	case WM_MOUSEMOVE:
		{
			if(m_nStyle >= ZST_MULTI) return 1;
			m_bKeyCross = FALSE;
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			if(m_RealMove == pt)	
				return 1;		//如果没有变化，则不算是MouseMove，因为有些软件如Winamp会频繁调MouseMove
			if(m_RealMove == CPoint(0,0))	{ m_RealMove = pt; return 1; }
			if(m_RealMove.x == xStart)		{ m_RealMove = pt; return 1; }
			m_RealMove = pt;
			CPoint point = pt;
			pt.x-=left;
			pt.y-=top;
			CRect rect(xStart,yStart,xEnd,yEndBot);
			
			if(m_bDrawLine)
			{
				m_ToolTip.PopToolTip();
				m_ToolTip.UpdateToolTip(m_pView,"");
				SetCursor(m_hDraw);
				if(rect.PtInRect(pt))
					InSwimArea(NULL,pt);
				CDC *pDC = m_pView->GetDC();
				DrawingMouseMove(pDC,point,NEWLINE_ID);
				m_pView->ReleaseDC(pDC);
			}
			else
			{
				CUnitView *pUnitView = NULL;
				if(m_pView->IsKindOf(RUNTIME_CLASS(CComView)) || m_pView->IsKindOf(RUNTIME_CLASS(CComView2)))
					pUnitView = (CUnitView*)m_pView;
				CRect UnitRect(m_PeriodRc);
				UnitRect.left += left;
				UnitRect.right += left;
				UnitRect.top += top;
				UnitRect.bottom += top;
				if(pUnitView&&m_pStkInfo&&UnitRect.PtInRect(point))
				{	
					pUnitView->ShowPopPreviewTool(FXT_UNIT, FXT_MULTI, UnitRect, m_pStkInfo);
					break;
				}
				else if(pUnitView) 
					pUnitView->HidePopPreviewTool();
				if(m_SelectInfo.bExist && m_SelectInfo.IsMovable)
				{
					CDC *pDC = m_pView->GetDC();
					SelfLineChg(pDC,point);
					m_pView->ReleaseDC(pDC);
					break;
				}
				if(rect.PtInRect(pt))
					InSwimArea(NULL,pt);
				else
				{
					if(m_bBeginRectangle)
					{
						m_bBeginRectangle = FALSE;
						ProcessMsg(WM_PAINT);
					}
					LeaveSwimArea(NULL);
				}
				if ( m_bBeginRectangle && abs(m_OrgPoint.x-pt.x)>2*SELECT_ON )
				{  
					CDC *pDC = m_pView->GetDC();
					int tempMode = pDC->SetROP2 (R2_XORPEN);
					if ( pt != m_ArchorPoint && m_ArchorPoint.y !=0 )
						g_d.DrawRect(pDC,left+m_OrgPoint.x,top+m_OrgPoint.y,left+m_ArchorPoint.x,top+m_ArchorPoint.y,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
					m_ArchorPoint = pt;
					g_d.DrawRect(pDC,left+m_OrgPoint.x,top+m_OrgPoint.y,left+m_ArchorPoint.x,top+m_ArchorPoint.y,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
					pDC->SetROP2 (tempMode);
				}
				//闪烁弹出播报及是否选中了新股的图标
				if( g_nFlushRealInfo && m_FlushRealInfoRect.PtInRect(pt) )
				{
					SetCursor(m_hCanPress);
					return 1;
				}
				else
					SetCursor(LoadCursor(NULL,IDC_ARROW));
				//能够设置预埋单
				if( g_bHasAdvJy && (m_YMDCfgRect.PtInRect(pt) || m_TokenOnOffRect.PtInRect(pt)) && CanShowLevinInfo())
				{
					SetCursor(m_hCanPress);
					return 1;
				}
				else
					SetCursor(LoadCursor(NULL,IDC_ARROW));
				//捕获相关资讯的位置
				for(int i=0;i<g_nCusWebPageNum;i++)
				{
					if(m_RelativeRect[i].PtInRect(pt))
					{
						SetCursor(m_hCanPress);
						return 1;
					}
				}
				if(i>=g_nCusWebPageNum)
				{
					//捕获券商相关资讯的位置
					if(m_QSRelativeRect.PtInRect(pt))
					{
						SetCursor(m_hCanPress);
						return 1;
					}
					SetCursor(LoadCursor(NULL,IDC_ARROW));
				}
				//捕捉股吧和研究报告的位置
				if( m_bIPO && m_CloseOpenRect.PtInRect(pt) )
				{
					CRect tmpRect=m_CloseOpenRect;
					tmpRect.OffsetRect(left,top);
					m_ToolTip.AddToolTip(m_pView,_F("点击切换分时图中轴方式(昨收盘/今开盘)"),&tmpRect,TOOLTIP_ZST_THUNDER);
					return 1;
				}
				if( m_GubaBtnRect.PtInRect(pt) )
				{
					CRect tmpRect=m_GubaBtnRect;
					tmpRect.OffsetRect(left,top);
					m_ToolTip.AddToolTip(m_pView,_F("点击打开股吧"),&tmpRect,TOOLTIP_ZST_THUNDER);
					return 1;
				}
				if( g_GC.bHasGPGrade && m_ReportBtnRect.PtInRect(pt) )
				{
					CRect tmpRect=m_ReportBtnRect;
					tmpRect.OffsetRect(left,top);
					m_ToolTip.AddToolTip(m_pView,_F("点击打开研究报告交流平台"),&tmpRect,TOOLTIP_ZST_THUNDER);
					return 1;
				}
				ZSTSELINFO OldSelf = m_SelectInfo;
				memset(&m_SelectInfo,0,sizeof(ZSTSELINFO));
				if(SelectSelfLine(point)>=0)
				{
					if(m_SelectInfo.nInOnePoint != 0) SetCursor(AfxGetApp()->LoadCursor(IDC_DRAW_TURN));
					else SetCursor(AfxGetApp()->LoadCursor(IDC_MOVE2));
					m_SelectInfo = OldSelf;
					return 1;
				}
				else SetCursor(LoadCursor(NULL,IDC_ARROW));
				m_SelectInfo = OldSelf;
			}
		}
		return 1;
	case WM_LBUTTONDOWN:
		{
			CPoint tempPoint(LOWORD(lParam),HIWORD(lParam));
			CPoint pt=tempPoint;
			memset(&m_SelectInfo,0,sizeof(ZSTSELINFO));
			pt.x-=left;
			pt.y-=top;
			if(m_bEvenSelected) 
			{
				m_bEvenSelected = FALSE;
				ProcessMsg(WM_PAINT);
			}
			if(m_PeriodRc.PtInRect(pt))
			{
				((CMainFrame*)AfxGetMainWnd())->OnCommCommand(CMD_MAIN,5);
				break;
			}
			if(m_ViewZoomRect.PtInRect(pt))
			{
				m_pView->SendMessage(WM_COMMAND, ID_HIDERIGHT);
				break;
			}
			//选中了新股的图标
			if(m_bIPO && m_CloseOpenRect.PtInRect(pt))
			{
				m_bUseOpenAxisForIPO=!m_bUseOpenAxisForIPO;
				SetBasePrice();
				CalcIt();
				ProcessMsg(WM_PAINT);
				break;
			}
			//设置预埋单
			if(g_bHasAdvJy && m_YMDCfgRect.PtInRect(pt) && CanShowLevinInfo())
			{
				ProcessMsg(WM_COMMAND,ID_LEVIN_EDITYMD);
				break;
			}
			if(g_bHasAdvJy && m_TokenOnOffRect.PtInRect(pt) && CanShowLevinInfo())
			{
				g_bShowToken=!g_bShowToken;
				ProcessMsg(WM_PAINT);
				break;
			}
			//处理相关资讯
			for(int i=0;i<g_nCusWebPageNum;i++)
			{
				if(m_RelativeRect[i].PtInRect(pt))
				{
					g_pMainWin->PostMessage(WM_COMMAND,IDC_CUSWEBPAGE_BASE+i);
					break;
				}					
			}
			if(m_QSRelativeRect.PtInRect(pt))
			{
				g_pMainWin->PostMessage(WM_COMMAND,IDC_QSCUSWEBPAGE);
				break;
			}
			//处理研究报告
			if(g_GC.bHasGPGrade && m_ReportBtnRect.PtInRect(pt))
			{
				AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_TDXREPORT);
				break;
			}
			//处理股吧
			if(m_GubaBtnRect.PtInRect(pt))
			{
				AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_GUBA);
				break;
			}
			if(m_bShowLayout && (pt.x>xEnd && pt.y>yEndBot))
			{
				if(m_lDate==0 && !CanLevel2())
				{
					ShellExecute(NULL, _T("open"), g_LoginInfo.Web_RegisterURL, NULL, NULL, SW_SHOW);
					break;
				}
				ProcessMsg(WM_PAINT);			
				tempPoint.x=left+xEnd;
				tempPoint.y=yEndBot;
				m_pView->ClientToScreen (&tempPoint);
				//先装入已有的菜单项
				CNewMenu menu,*pPopup;
				menu.LoadMenu (IDR_LONELY_MENU);
//				menu.LoadToolBar(IDR_MAINFRAME);
				if(m_lDate>0)
				{
					pPopup = (CNewMenu *)menu.GetSubMenu(23);
					pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, tempPoint.x, tempPoint.y, m_pView);
				}
				else
				{
					pPopup = (CNewMenu *)menu.GetSubMenu(22);
					pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, tempPoint.x, tempPoint.y, AfxGetMainWnd());
				}
			}
			else if(!OnLine || (OnLine&&m_bDataReady))
			{
				int i,nCurWT=-1,nCurCJ=-1,nCurYmd=-1;
				//如果点击可撤单的点
				BOOL bClickOne=FALSE;
				for(i=0;i < m_nWTNum;i++)
				{
					if(RectPtIn(m_pWTRect[i],pt))
					{
						bClickOne=TRUE;
						nCurWT=i;
						break;
					}
				}
				for(i=0;i < m_nYmdNum && !bClickOne;i++)
				{
					if(RectPtIn(m_pYmdRect[i],pt))
					{
						bClickOne=TRUE;
						nCurYmd=i;
						break;
					}
				}
				if(bClickOne)
				{
					m_nCurWT=nCurWT;m_nCurYmd=nCurYmd;
					ProcessMsg(WM_PAINT);
				}
				else if(m_nCurWT>=0 || m_nCurYmd>=0)
				{
					m_nCurWT=-1;m_nCurYmd=-1;
					ProcessMsg(WM_PAINT);
				}
			}
			if(m_bDrawLine)
			{
				//弹出并清空ToolTip
				m_ToolTip.PopToolTip();
				m_ToolTip.UpdateToolTip(m_pView,"");
				CDC *pDC = m_pView->GetDC();
				if(m_nPtNum == 0)
				{
					++m_nPtNum;
					if(!BeginDrawSelfLine(pDC,tempPoint))
						ResetSelfLine();			
				}
				else
				{
					TransSelfLine(tempPoint,NEWLINE_ID,m_nPtNum);
					++m_nPtNum;
					if(m_nPtNum>=g_DrawLineTool.GetMaxSetPtNum(m_nWay))
					{
						g_DrawLineTool.AddLine(&m_NewDrawLine);
						//
						ResetSelfLine();
						ProcessMsg(WM_PAINT);
					}
				}
				m_pView->ReleaseDC(pDC);
			}
			else if(SelectSelfLine(tempPoint)!=SELNONE_ID)
			{
				long xSPos = xStart;
				long xEPos = xEnd;
				if(m_bManyDayZST && m_nZSTManyDay>0)
					xSPos = xStart+m_nZSTManyDay*(xEnd-xStart)/(m_nZSTManyDay+1);
				m_PtInArea		= CRect(xSPos,yStart,xEPos,yVol);
				m_AbsPtInArea	= CRect(min(xSPos+1,xEPos),yStart,xEPos,yVol);
				m_AbsPtInArea.OffsetRect(left,top);
				//
				CRect r(&m_AbsPtInArea);
				m_pView->ClientToScreen(&r);
				ClipCursor(&r);
				ProcessMsg(WM_PAINT);
			}
		}
		return 1;
	case WM_LBUTTONUP:
		if(!m_bDrawLine)
			ClipCursor(NULL);
		if(m_bDrawLine)
		{
			CPoint tempPoint(LOWORD(lParam),HIWORD(lParam));
			CDC *pDC = m_pView->GetDC();
			m_Point[1] = tempPoint;
			TransSelfLine(m_Point[1],NEWLINE_ID,1);
			++m_nPtNum;
			if(m_nWay == ID_TXTTOOLS)
			{
				ClipCursor(NULL);
				m_bDrawLine		= FALSE;
				CEditTxtDlg EditTxt;
				if(EditTxt.DoModal() == IDOK && EditTxt.m_strTxt.GetLength()>0)
				{
					strncpy(m_NewDrawLine.szBuffer,EditTxt.m_strTxt,200);
					m_NewDrawLine.szBuffer[200]=0;
					m_NewDrawLine.FontSize=g_nTxtFontSize;
					m_NewDrawLine.tversion = 1;
					m_NewDrawLine.tcrf = EditTxt.m_color;
					m_NewDrawLine.nUsePoint = (pDC->GetTextExtent(EditTxt.m_strTxt)).cx;
				}
			}
			if(m_nPtNum>=g_DrawLineTool.GetMaxSetPtNum(m_nWay))
			{
				g_DrawLineTool.AddLine(&m_NewDrawLine);
				//
				ResetSelfLine();
				ClipCursor(NULL);
				ProcessMsg(WM_PAINT);
			}
			m_pView->ReleaseDC(pDC);
		}
		if(m_SelectInfo.IsMovable)
		{
			m_SelectInfo.IsMovable = FALSE;
			if(m_SelectInfo.bMoved)
			{
				TransSelfLine(m_Point[0],m_SelectInfo.nSelfLineNo,0);
				TransSelfLine(m_Point[1],m_SelectInfo.nSelfLineNo,1);
				TransSelfLine(m_Point[2],m_SelectInfo.nSelfLineNo,2);
			}
			ResetSelfLine();	//基本变量复位
			ProcessMsg(WM_PAINT);
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			if(m_nStyle >= ZST_MULTI) return 1;
			if(m_thunderposnum > 0)
			{
				int i,tdxmin,tmpx;
				for(i=0;i < m_thunderposnum;i++)
				{
					tdxmin = m_thunderpos[i].tdxmin;
					if(tdxmin < nSectTime[0][0]) continue;
					tmpx = GetXFromMinute(tdxmin);
					if(tmpx==xEnd) tmpx=xEnd-8;
					CRect tmprc;
					tmprc.left = tmpx;
					tmprc.right = tmpx+14;
					tmprc.top = yStart+2;
					tmprc.bottom = yStart+20;
					if(tmprc.PtInRect(CPoint(LOWORD(lParam)-left,HIWORD(lParam)-top)))
					{
						m_bManualPop = TRUE;
						ProcessMsg(UM_POPINFODLG,m_thunderpos[i].firstpos);
						m_bManualPop = FALSE;
						return 1;
					}
				}
			}
			CPoint pt;
			pt.x=LOWORD(lParam)-left;
			pt.y=HIWORD(lParam)-top;
			if(!OnLine || (OnLine&&m_bDataReady))
			{
				
			}
			if(pt.x>xEnd && pt.y<yEndBot && !m_nInCom2 && !m_nInHisDlg) //双击右轴进入分析图
			{
				g_pMainWin->PostMessage(WM_COMMAND,ID_300);
				return 1;
			}
			if(pt.x>xStart && pt.x<xEnd && pt.y>yStart && pt.y<yEndBot)
			{
				m_bCrossMode = !m_bCrossMode;
				m_bHasCross = m_bCrossMode;
				m_CrossPoint = pt;
				ProcessMsg(WM_PAINT);
			}
		}
		return 1;
	case WM_RBUTTONDOWN:
		{
			if(m_nStyle >= ZST_MULTI) return 1;
			CPoint tempPoint(LOWORD(lParam),HIWORD(lParam));
			tempPoint.x-=left;tempPoint.y-=top;
			memset(&m_ArchorPoint,0,sizeof(CPoint));
			memset(&m_OrgPoint ,0,sizeof(CPoint));
			m_OrgPoint = tempPoint;
		//	m_bBeginRectangle = TRUE;
		}
		return 1;
	case WM_RBUTTONUP:
		{
			if(m_nStyle>=ZST_MULTI) return 1;
			CPoint tempPoint(LOWORD(lParam),HIWORD(lParam));
			CPoint pt=tempPoint;
			m_pView->ClientToScreen (&tempPoint);
			if ( m_bBeginRectangle  )
			{
				m_bBeginRectangle = FALSE;
				ProcessMsg(WM_PAINT);
				return 1;
				if ( m_ArchorPoint.x+m_ArchorPoint.y>0 && abs(m_ArchorPoint.x - m_OrgPoint.x) > 2*SELECT_ON )
				{
					CNewMenu menu, *pMenu = NULL;
					menu.LoadMenu(IDR_RIGHTMENU_COM);
					pMenu = (CNewMenu *)menu.GetSubMenu(3);
					ProcessMsg(WM_PAINT);
					pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, tempPoint.x, tempPoint.y, m_pView);
					return 1;
				}
			}
			if(m_lDate) return 1;
			if(!OnLine || (OnLine&&m_bDataReady))
			{
				
			}
			//
			memset(&m_SelectInfo,0,sizeof(ZSTSELINFO));
			if(SelectSelfLine(pt)>=0)
			{
				m_SelectInfo.IsMovable = FALSE;
				CNewMenu menu;
				menu.LoadMenu(IDR_RIGHTMENU_COM);
//				menu.LoadToolBar(IDR_MAINFRAME);
				CNewMenu *pPopup = (CNewMenu *)menu.GetSubMenu(ID_DRAWLINE);
				ProcessMsg(WM_PAINT);
				m_pView->ClientToScreen(&pt);
				pPopup->TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,tempPoint.x,tempPoint.y,AfxGetMainWnd());
				return 1;
			}
			//
			CNewMenu menu;
			menu.LoadMenu (IDR_LONELY_MENU);
//			menu.LoadToolBar(IDR_MAINFRAME);
			CNewMenu *pPopup = (CNewMenu *)menu.GetSubMenu(7);	
			ProcessMsg(WM_PAINT);

			if(g_OEM.bEmbedWTVersion && m_pStkInfo && !m_bZs)
			{
				int tmpLen=strlen(m_pStkInfo->Code);
				if(tmpLen==6 || tmpLen==5)
				{
					pPopup->InsertMenu(0,MF_BYPOSITION|MF_SEPARATOR);
					if(g_OEM.bHasRelativeInfo)
						pPopup->InsertMenu(0,MF_BYPOSITION,IDC_RELATIVEINFO,_F("相关资讯"));
					pPopup->InsertMenu(0,MF_BYPOSITION,IDC_WTCANCEL,_F("撤单查询\t22"));
					pPopup->InsertMenu(0,MF_BYPOSITION,IDC_SELL0,_F("普通卖出\t223"));
					pPopup->InsertMenu(0,MF_BYPOSITION,IDC_BUY0,_F("普通买入\t221"));
					if(g_bHasAdvJy && g_bShowToken)
					{
						pPopup->InsertMenu(0,MF_BYPOSITION|MF_SEPARATOR);
						pPopup->InsertMenu(0,MF_BYPOSITION,ID_LEVIN_EDITYMD,_F("设置预埋单"));
						pPopup->InsertMenu(0,MF_BYPOSITION,ID_LEVIN_PASTEWT,_F("粘贴委托单"));
					}
					pPopup->InsertMenu(0,MF_BYPOSITION|MF_SEPARATOR);
					pPopup->InsertMenu(0,MF_BYPOSITION,IDC_LEVINSELL1,_F("闪电卖出\t23"));
					pPopup->InsertMenu(0,MF_BYPOSITION,IDC_LEVINBUY11,_F("闪电买入\t21"));
				}
			}			
			pPopup->EnableMenuItem(ID_LEVIN_PASTEWT,(m_nCurCopyWT>=0)?(MF_BYCOMMAND|MF_ENABLED):(MF_BYCOMMAND|MF_DISABLED|MF_GRAYED));
			pPopup->TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,tempPoint.x,tempPoint.y,AfxGetMainWnd());
		}
		return 1;
	case WM_COMMAND:
		{
			if(wParam==ID_LEVIN_SENDYMD)
			{
				if(m_nCurYmd>=0)
				{
					g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,NULL,&m_HqEx);
					if(m_pYmdList[m_nCurYmd].bsflag=='0')
						TC_DoLevinJy_(0,-1,m_pStkInfo->setcode,m_pStkInfo->Code,m_pStkInfo->Name,FloatToStr2(m_pYmdList[m_nCurYmd].wtjg,m_XsFlag),IntToStr(m_pYmdList[m_nCurYmd].wtsl),LF_SENDYMD);
					else
						TC_DoLevinJy_(1,-1,m_pStkInfo->setcode,m_pStkInfo->Code,m_pStkInfo->Name,FloatToStr2(m_pYmdList[m_nCurYmd].wtjg,m_XsFlag),IntToStr(m_pYmdList[m_nCurYmd].wtsl),LF_SENDYMD);
				}
			}
			else if(wParam==ID_LEVIN_EDITYMD)
			{
				if(CanUseEmbedWTFunc(m_pView->m_hWnd))
				{
					((CMainFrame *)g_pMainWin)->SwitchToRestSpace(AREA_WT);
					if(TC_DoLevinJy_) 
						TC_DoLevinJy_(5,0,m_pStkInfo->setcode,m_pStkInfo->Code,"","","",LF_NORMAIL);
				}
			}
			else if(wParam==ID_LEVIN_COPYWT)
			{
				if(m_nCurWT>=0)
					m_nCurCopyWT = m_nCurWT;
			}
			else if(wParam==ID_LEVIN_PASTEWT)
			{
				if(m_nCurCopyWT>=0)
				{
					if(m_pWTList[m_nCurCopyWT].nBSFlag==0)
						TC_DoLevinJy_(0,-1,m_pStkInfo->setcode,m_pStkInfo->Code,m_pStkInfo->Name,FloatToStr2(m_pWTList[m_nCurCopyWT].fWtPrice,m_XsFlag),IntToStr(m_pWTList[m_nCurCopyWT].lWtVol),LF_NEWORDER);
					else
						TC_DoLevinJy_(1,-1,m_pStkInfo->setcode,m_pStkInfo->Code,m_pStkInfo->Name,FloatToStr2(m_pWTList[m_nCurCopyWT].fWtPrice,m_XsFlag),IntToStr(m_pWTList[m_nCurCopyWT].lWtVol),LF_NEWORDER);
				}
				else
					TDX_MessageBox(m_pView->m_hWnd,"请先选中一个委托单,点右键选择[复制委托单],再进行粘贴",MB_OK|MB_ICONEXCLAMATION);
			}
			else if(wParam==ID_LEVIN_CANCEL)
			{
				if(m_nCurWT>=0)
				{
					SendLevinCancel(m_pView,m_pWTList+m_nCurWT);
					return 1;
				}
			}
			ProcessMsg(WM_PAINT);
		}
		return 1;
	case UM_SETSEL:
		m_bSelected = wParam;
		return 1;
	case UM_GETSEL:
		return m_bSelected?1:0;
	case UM_SWITCHSTYLE:	//wParam为0正序 为1逆序
		if(m_lDate != 0) return 1;
		if(!m_bDataReady && OnLine) return 1;
		if(m_bDrawLine) return 1;
		if(m_nStyle < ZST_MULTI)
		{
			if(!m_bHasSwitch)
			{
				if(wParam == 0)
					m_nStyle = (m_nStyle+1)%3;
				else
				{
					m_nStyle-=1;
					if(m_nStyle < 0) m_nStyle=2;
				}				
			}
			else
			{
				int nTmpStyle = 0;
				switch(m_pSwitch->GetCurLabel())
				{
				case ZST_BIG:			nTmpStyle = ZST_BIG;	break;
				case ZST_LB:			nTmpStyle = ZST_LB;		break;
				case ZST_MMLD:			nTmpStyle = ZST_MMLD;	break;
				case ZST_AUCTION:		if(m_nSwitchBtn>ZST_AUCTION)	{nTmpStyle = ZST_AUCTION;		break;}
				case ZST_TOTALORDER:	if(m_nSwitchBtn>ZST_TOTALORDER)	{nTmpStyle = ZST_TOTALORDER;	break;}
				case ZST_TOTALCANCEL:	if(m_nSwitchBtn>ZST_TOTALCANCEL){nTmpStyle = ZST_TOTALCANCEL;	break;}
				case ZST_CANCELSUM:		if(m_nSwitchBtn>ZST_CANCELSUM)	{nTmpStyle = ZST_CANCELSUM;		break;}
				case ZST_TICKNUM:		if(m_nSwitchBtn>ZST_TICKNUM)	{nTmpStyle = ZST_TICKNUM;		break;}
				case ZST_LARGETICK:		if(m_nSwitchBtn>ZST_LARGETICK)	{nTmpStyle = ZST_LARGETICK;		break;}
				case ZST_IOVOL:			if(m_nSwitchBtn>ZST_IOVOL)		{nTmpStyle = ZST_IOVOL;			break;}
				case ZST_LARGEVOL:		if(m_nSwitchBtn>ZST_LARGEVOL)	{nTmpStyle = ZST_LARGEVOL;		break;}
				default:				nTmpStyle = m_nStyle;	break;
				}
				if(nTmpStyle == m_nStyle)
				{
					if(wParam == 0)
						m_nStyle = (m_nStyle+1)%m_nSwitchBtn;
					else
					{
						m_nStyle-=1;
						if(m_nStyle < 0) m_nStyle=m_nSwitchBtn-1;
					}
				}
				else m_nStyle = nTmpStyle;
				if(m_nStyle < 0) m_nStyle=0;
			}
			//如果是多日分时图,不支持竞价图
			if(m_nStyle==ZST_AUCTION && m_bManyDayZST)
			{
				g_bManyDayZST = FALSE;
				g_nZSTManyDay = 1;
				ProcessMsg(UM_MANYDAYZST);
			}
			if(OnLine && !m_bDSFlag)
			{
				m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				if(m_nStyle==ZST_LB)				//量比
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_LB);
					g_pGetData->GetLead(m_pStkInfo);
				}
				else if(m_nStyle==ZST_MMLD)		//买卖力道
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_MMLD);
					g_pGetData->GetMmld(m_pStkInfo);
				}
				else if(m_nStyle==ZST_AUCTION)
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_AUCTION);
					g_pGetData->GetAuction(m_pStkInfo,0,MAX_AUCTIONNUM);
				}
				else if(m_nStyle==ZST_TOTALORDER)	//总买总卖
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_TOTALORDER);
					g_pGetData->GetZmzm(m_pStkInfo);
				}
				else if(m_nStyle==ZST_TOTALCANCEL)	//净挂净撤
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_TOTALCANCEL);
					g_pGetData->GetJgjc(m_pStkInfo);
				}
				else if(m_nStyle==ZST_CANCELSUM)	//撤单累计
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_CANCELSUM);
					g_pGetData->GetJgjc(m_pStkInfo);
				}
				else if(m_nStyle==ZST_TICKNUM)		//涨跌动因统计
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_TICKNUM);
					g_pGetData->GetCjbs(m_pStkInfo);
				}
				else if(m_nStyle==ZST_LARGETICK)	//大单成交笔数统计
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_LARGETICK);
					g_pGetData->GetCjbs(m_pStkInfo);
				}
				else if(m_nStyle==ZST_IOVOL)		//成交量统计
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_IOVOL);
					g_pGetData->GetCjtj(m_pStkInfo);
				}
				else if(m_nStyle==ZST_LARGEVOL)		//大单成交量统计
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_LARGEVOL);
					g_pGetData->GetCjtj(m_pStkInfo);
				}
			}
			ProcessMsg(WM_PAINT);
		}
		return 1;
	case UM_SET_STOCK: //对于同步回忆状态,如果重新设置股票，则需要重新预读
		UBase::ProcessMsg(message,wParam,lParam);
		m_nCurWT = m_nCurYmd = m_nCurCopyWT = -1;		
		//删除ToolTip
		m_ToolTip.DelToolTip(m_pView);
		m_LevinToolTip.DelToolTip(m_pView);

		m_bLeadReqed	= FALSE;
		m_nLeadData		= 0;
		m_nNormalStat	= 0;
		m_nTickStat		= 0;
		m_nVtlData		= 0;
		m_dTotalOrder.SetData(m_NormalStat,m_nNormalStat);
		m_dCancelOrder.SetData(m_NormalStat,m_nNormalStat);
		m_dCjbsStat.SetData(m_TickStat,m_nTickStat);
		m_dCjtjStat.SetData(m_TickStat,m_nTickStat);
		m_lMaxVirtualPxOff	= 0;
		m_lMaxVirtualVol	= 0;
		return 1;
	case UM_SET_PLAY:	//设置自动播放
		if(wParam == 1)
		{
			if(m_nTodayMin <= 0) 
				return 0;
			m_bCrossMode = m_bHasCross = FALSE; //使十字光标消失
			m_CrossPoint.x = 0;
			m_nCurMinute = 1;
			int nAbsTotalMinute = m_nTodayMin;
			int nStartMinute = 0;
			if(m_bManyDayZST)
			{
				nAbsTotalMinute = m_nRealTotalMin*m_nZSTManyDay+m_nTodayMin;
				nStartMinute = m_nTodayMin - nAbsTotalMinute;
				m_nCurMinute += nStartMinute;
			}
		}
		else
		{
			m_nCurMinute = m_nTodayMin;
			CalcIt();
		}
		m_bPlayStatus = (wParam==1)?TRUE:FALSE;
		ProcessMsg(WM_PAINT);
		return 1;
	case UM_GET_PLAY:
		return m_bPlayStatus?1:0;
	case UM_CONTINUEPLAY:		//wParam 0:最前,1:向前-10 2:向后+10 3:最后 4:前进一格
		{
			if(!m_bPlayStatus) //如果不是同步状态，则启动它
				ProcessMsg(UM_SET_PLAY,1);
			int nAbsTotalMinute = m_nRealTotalMin*(m_bManyDayZST?m_nZSTManyDay:0)+m_nTodayMin;
			int nStartMinute = m_nTodayMin - nAbsTotalMinute;
			int nPreCurDay=-1,nPreCurMinute=m_nCurMinute;
			if(m_bManyDayZST)
			{
				nPreCurDay = (m_nCurMinute - 1 - nStartMinute)/m_nRealTotalMin;
				nPreCurMinute = (m_nCurMinute - 1 - nStartMinute)%m_nRealTotalMin + 1;
				if(nPreCurMinute==1)
					m_pView->SendMessage(UM_SET_CUR_DATE,m_ManyDayDate[nPreCurDay]);
			}
			switch(wParam)
			{
			case 0: m_nCurMinute = nStartMinute + 1;break;
			case 1: m_nCurMinute = max(m_nCurMinute-10,nStartMinute + 1);break;
			case 2: m_nCurMinute = min(m_nCurMinute+10,m_nTodayMin);break;
			case 3: m_nCurMinute = m_nTodayMin;break;
			case 4: m_nCurMinute = min(m_nCurMinute+1,m_nTodayMin);break;
			}
			CalcIt();
			ProcessMsg(WM_PAINT);
			//发送反映消息
			int nCurDay=m_nZSTManyDay,nCurMinute=m_nCurMinute;
			if(m_bManyDayZST)
			{
				nCurDay = (m_nCurMinute - 1 - nStartMinute)/m_nRealTotalMin;
				nCurMinute = (m_nCurMinute - 1 - nStartMinute)%m_nRealTotalMin + 1;
				if(nCurDay!=nPreCurDay&&nCurDay!=m_nZSTManyDay)
					m_pView->SendMessage(UM_SET_CUR_DATE,m_ManyDayDate[nCurDay]);
				else if(nCurDay!=nPreCurDay&&nCurDay==m_nZSTManyDay)
					m_pView->SendMessage(UM_SET_CUR_DATE,g_lOpenRq);
			}
			if(nCurMinute>0)
			{
				int NowAbsTime;
				if(m_bManyDayZST&&nCurDay!=m_nZSTManyDay) NowAbsTime=m_ManyDayMinuteData[nCurDay][nCurMinute-1].Minute;
				else NowAbsTime=m_MinuteData[nCurMinute-1].Minute;
				m_pView->SendMessage(UM_REFLECT,Reflect_ZstSynch,NowAbsTime);
			}
		}
		return 1;
	case UM_EXOPENBLOCKHQ:
		{
			if(wParam>=ZJHHY_START&&lParam==-1)
			{
				//板块指数咯
				int nBlockNo = wParam-ZJHHY_START;
				char *Code = g_pLcIdxMng->GetIdxStkCode(nBlockNo);
				long nIndex = g_pStockIO->GetIndex(Code, SZ);
				if(nIndex>=0) 
				{
					ProcessMsg(UM_SET_STOCK, nIndex, 0);
					ProcessMsg(WM_PAINT, 1);
				}
			}
		}
		break;
	case UM_POPZSTICKDLG:
		{
			if(!OnLine)
			{
				TDX_MessageBox(m_pView->m_hWnd,"请先连接行情主站!",MB_OK|MB_ICONERROR);
				break;
			}
			if(!m_bDSFlag)
			{
				BYTE nStartDate, nEndDate;
				short nStartMinXh, nEndMinXh;
				if(wParam==0)
				{
					int  xPos = max(m_OrgPoint.x, m_ArchorPoint.x);
					GetPosInfo(xPos, nStartDate, nStartMinXh);
					xPos = min(m_OrgPoint.x, m_ArchorPoint.x);
					GetPosInfo(xPos, nEndDate, nEndMinXh);
				}
				else
				{
					nStartDate = nEndDate = 0;
					if(m_bManyDayZST) nEndDate = m_nZSTManyDay;
					nEndMinXh = nSectTime[0][0]-30;
					nStartMinXh = nSectTime[nSectNum-1][1]+30;
				}

				g_pTickZstjDlg = new ZstTickDlg;
				((ZstTickDlg *)g_pTickZstjDlg)->m_pUZst = this;
				((ZstTickDlg *)g_pTickZstjDlg)->m_nCurGPNo = m_nCurGP;
				((ZstTickDlg *)g_pTickZstjDlg)->m_nCurDate = (m_lDate==0?g_lOpenRq:m_lDate);
				((ZstTickDlg *)g_pTickZstjDlg)->m_nStartDate = nStartDate;
				((ZstTickDlg *)g_pTickZstjDlg)->m_nStartMinuteXh = nStartMinXh+1;
				((ZstTickDlg *)g_pTickZstjDlg)->m_nEndDate = nEndDate;
				((ZstTickDlg *)g_pTickZstjDlg)->m_nEndMinuteXh = nEndMinXh-1;
				((ZstTickDlg *)g_pTickZstjDlg)->DoModal();
				TDEL(g_pTickZstjDlg);
			}
			else
				TDX_MessageBox(m_pView->m_hWnd,"扩展行情不支持分时统计!",MB_OK|MB_ICONEXCLAMATION);
		}
		break;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void UZst::GetPosInfo(int nPos, BYTE &whichHis, short &tmpMinute)
{
	whichHis = 0, tmpMinute = 0;
	int NowMinutes = GetTimeFromX(nPos);
	int nTimeNum=System2Tdx(NowMinutes,TimeSec)-1;
	if(m_bManyDayZST && 1.0*nPos < 1.0*xStart+1.0*m_nZSTManyDay*m_nRealTotalMin*(xEnd-xStart)/nSecLens)
	{
		whichHis = min(m_nZSTManyDay-1,1.0*(nPos-xStart-1)*nSecLens/(1.0*m_nRealTotalMin*(xEnd-xStart))); //精度问题总有一点.
		tmpMinute = m_ManyDayMinuteData[whichHis][nTimeNum].Minute;
		whichHis = m_nZSTManyDay-whichHis;
	}
	else tmpMinute = GetMinuteFromFZ(nTimeNum, 1, TimeSec);
	if(nTimeNum==0)	tmpMinute -= 30;
}

long UZst::GetDate(int whichHis)
{
	if(m_bManyDayZST&&whichHis<MAX_ZST_MANYDAY&&m_ManyDayDate[whichHis]>0) 
		return m_ManyDayDate[whichHis];
	return (m_lDate==0?g_lOpenRq:m_lDate);
}

void UZst::SetStyle(int style)
{
	m_nStyle = style;
	ProcessMsg(WM_PAINT);
}

void UZst::ReCalcZSAverage()
{
	float fzssum = 0;
	float fzsvol = 0;
	for(int i=0;i<m_nTodayMin;i++)
	{
		fzssum += m_MinuteData[i].Now*m_MinuteData[i].NowVol;
		fzsvol += m_MinuteData[i].NowVol;
		if(fzsvol>COMPPREC)
			m_MinuteData[i].Average = fzssum/fzsvol;
		else 
			m_MinuteData[i].Average = m_Hq.Close;
	}
}

void UZst::DTWH_currMinute()	//在调用此函数前，应重新读取行情和m_nTodayMin
{
	//动态维护分时数据
	m_MinuteData[m_nTodayMin-1].Minute=GetMinuteFromFZ(m_nTodayMin-1,1,TimeSec);
	if( m_Hq.Now < COMPPREC )	
	{
		if(m_nTodayMin < 2)	m_Hq.Now = m_Hq.Close;
		else					m_Hq.Now = m_MinuteData[m_nTodayMin-2].Now;
	}
	m_MinuteData[m_nTodayMin-1].Now=m_Hq.Now;

	if(m_nOverlapNum>99) m_nOverlapNum = g_nOverlapNum;
	for(int kk=0;kk < m_nOverlapNum;kk++) //叠加股的动态维护
	{
		if(m_OverlapMinuteData[kk][0].Now < COMPPREC) continue; //如果走势图数据还没有到,不动态维护
		if(m_OverlapHq[kk].Close < COMPPREC) continue;			 //如果没有行情数据,不动态维护
		if(!IsSameOpenCloseTimer(m_pOverlapStkInfo[kk],m_pStkInfo)) continue;
		m_OverlapMinuteData[kk][m_nTodayMin-1].Minute=GetMinuteFromFZ(m_nTodayMin-1,1,TimeSec);
		if( m_OverlapHq[kk].Now < COMPPREC )	
		{
			if(m_nTodayMin < 2)	m_OverlapHq[kk].Now = m_OverlapHq[kk].Close;
			else					m_OverlapHq[kk].Now = m_OverlapMinuteData[kk][m_nTodayMin-2].Now;
		}
		m_OverlapMinuteData[kk][m_nTodayMin-1].Now=m_OverlapHq[kk].Now;
	}
	if(m_pStkInfo->bigmarket==QH_TYPE) 
	{
		if(m_nTodayMin>1 && m_Hq.Yield_VolInStock.VolInStock<COMPPREC)
			m_MinuteData[m_nTodayMin-1].VolInStock_Lead.VolInStock=m_MinuteData[m_nTodayMin-2].VolInStock_Lead.VolInStock;
		else
			m_MinuteData[m_nTodayMin-1].VolInStock_Lead.VolInStock=m_Hq.Yield_VolInStock.VolInStock;
	}
	else if(m_bMinuteTick_Special)
	{
		if(m_Hq.Jjjz>COMPPREC)
			m_MinuteData[m_nTodayMin-1].VolInStock_Lead.Lead=m_Hq.Jjjz;
		else if(m_nTodayMin>1)
			m_MinuteData[m_nTodayMin-1].VolInStock_Lead.Lead=m_MinuteData[m_nTodayMin-2].VolInStock_Lead.Lead;
	}
	else
		m_MinuteData[m_nTodayMin-1].VolInStock_Lead.Lead=0;
	if(m_bZs)
	{
		m_MinuteData[m_nTodayMin-1].Buyv=m_Hq.Buyv[0];
		m_MinuteData[m_nTodayMin-1].Sellv=m_Hq.Sellv[0];
	}
	else
	{
		m_MinuteData[m_nTodayMin-1].Buyv=m_Hq.Buyv[0]+m_Hq.Buyv[1]+m_Hq.Buyv[2]+m_Hq.Buyv[3]+m_Hq.Buyv[4];
		m_MinuteData[m_nTodayMin-1].Sellv=m_Hq.Sellv[0]+m_Hq.Sellv[1]+m_Hq.Sellv[2]+m_Hq.Sellv[3]+m_Hq.Sellv[4];
	}
	if(!m_bDSFlag)	
		m_MinuteData[m_nTodayMin-1].Average=cal_averagetmp(m_pStkInfo,&m_Hq);
	if (m_bZs)
	{
		if(m_Hq.Sellp[2]>COMPPREC)
			m_MinuteData[m_nTodayMin-1].Average*=m_Hq.Close/m_Hq.Sellp[2];
		if(m_MinuteData[m_nTodayMin-1].Average<0.1f && m_nTodayMin>1) //如果Average小于0.1(不能太小),则进行平滑
			m_MinuteData[m_nTodayMin-1].Average=m_MinuteData[m_nTodayMin-2].Average;
		//
		if(m_bZs && m_bZsLx && g_GH.bZsShowAverage)
			ReCalcZSAverage();
	}
	if(m_bDSFlag)
	{
		if(m_pStkInfo->bigmarket==IDX_TYPE||m_pStkInfo->bigmarket==QH_TYPE)
		{
			float fAmo = 0.0f, fVol = 0.0f;
			for(int i=0;i<m_nTodayMin;i++)
			{
				fVol += m_MinuteData[i].NowVol;
				fAmo += m_MinuteData[i].NowVol*m_MinuteData[i].Now;
			}
			if(fVol>COMPPREC)
				m_MinuteData[m_nTodayMin-1].Average = fAmo/fVol;
			else
				m_MinuteData[m_nTodayMin-1].Average = 0.0;
		}
		else if(m_Hq.Volume*m_pStkInfo->Unit > COMPPREC)
			m_MinuteData[m_nTodayMin-1].Average = m_Hq.Amount*m_pStkInfo->AmoUnit/(m_Hq.Volume*m_pStkInfo->Unit);
	}
	//计算该分钟的量
	long	ltmp = 0;
	int		i = 0;
	if(m_lLastMinVolume > 0)  //如果上一次量为0，则使用叠加法计算
		ltmp = m_lLastMinVolume;
	else
	{
		for(int i=0;i < m_nTodayMin-1;i++)
			ltmp+=m_MinuteData[i].NowVol;
	}
	if(m_bZs)	
		m_MinuteData[m_nTodayMin-1].NowVol = max(0,((long)(m_Hq.Amount/10000.0)-ltmp));
	else		
		m_MinuteData[m_nTodayMin-1].NowVol = max(0,(long)(m_Hq.Volume-ltmp));

	//动态维护量比数据
	if(!m_bZs && m_pStkInfo->VolBase>0)
		m_LBdata[m_nTodayMin-1]=(float)m_Hq.Volume/m_nTodayMin/m_pStkInfo->VolBase;
	else if(m_bZs && m_bLeadReqed)
	{
		m_LBdata[m_nTodayMin-1]=m_Hq.Lead;
		m_nLeadData = m_nTodayMin;
	}
}

void UZst::ReadData(BOOL bDataReady,BOOL bReqData)
{
	int i;
	short ItemNum;
	memset(&m_Hq,0,sizeof(CurrStockData));
	memset(&m_HqEx,0,sizeof(EXCURRDATA));
	memset(m_MinuteData,0,1500*sizeof(MinuteData));
	memset(m_LBdata,0,1500*sizeof(float));
	memset(m_NormalStat,0,1500*sizeof(NORMALSTAT));
	memset(m_TickStat,0,1500*sizeof(TICKSTAT));

	if(!bDataReady)
	{
		memset(m_fManyDayClose,0,MAX_ZST_MANYDAY*sizeof(float));
		memset(m_ManyDayMinuteData,0,MAX_ZST_MANYDAY*1500*sizeof(MinuteData));
	}
	AutoOverlapGP();
	if(m_lDate != 0) 
	{
		m_bManyDayZST = FALSE;	//多日走势图的状态
		m_nZSTManyDay = g_nHisZSTManyDay;
		if(g_bHisManyDayZST && m_nStyle<=ZST_MULTI)
		{
			m_bTotalSect = TRUE;
			m_bManyDayZST = TRUE;
		}
	}
	else
	{
		m_bManyDayZST = FALSE;	//多日走势图的状态
		m_nZSTManyDay = g_nZSTManyDay;
		if(g_bManyDayZST && m_nStyle<=ZST_MULTI)
			m_bManyDayZST = TRUE;
	}
	//如果是多日分时图,不支持竞价图
	if(m_bManyDayZST && m_nStyle==ZST_AUCTION)
		m_nStyle=ZST_BIG;
	
	m_nRealTotalMin = GetRealTotalMinute(TimeSec);

	m_nTodayMin = 0;
	fBasePrice = 0.0;
	//判断是否有领先
	m_bZsLx = FALSE;
	if(m_bZs)
	{
		for(int i=0;i < l_nLxZsNum;i++)
		{
			if(strcmp(m_pStkInfo->Code,l_LxZsCode[i])==0)
			{	m_bZsLx = TRUE;break;	}
		}
	}
	if(m_lDate == 0)	//当天MinuteData
	{
		if(m_bDSFlag) 
		{
			g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,NULL,&m_HqEx);
			m_bYestodayFalg = (m_HqEx.HqDate<GetDSCorrespondingDate(m_pTimer));
			ItemNum=GetDSMinute(m_pTimer,m_bYestodayFalg);
		}
		else ItemNum=GetTdxMinute(TimeSec);
		if(ItemNum > 1500) return;
		g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,NULL,&m_HqEx);
		if(m_bDSFlag)
			m_bYestodayFalg = (m_HqEx.HqDate<GetDSCorrespondingDate(m_pTimer));
		if(!bDataReady)
		{
			if(!m_bDSFlag && (!OnLine||m_bLcIdx||m_bLcExt||m_bLcPt)) //如果是脱机状态，直接读取缓存的
			{
				if(GetCahceMinuteData(m_pStkInfo,m_MinuteData))
				{
					m_nTodayMin = ItemNum;
					bDataReady = TRUE;
					Overlap_ReadCache();
					ManyDay_ReadCache(g_lOpenRq);
				}
			}
			if(OnLine && bReqData)	//如果要请求数据
			{
				//先请求分时数据
				m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				g_nAssisID = SetComAssisID(m_nWhichUnit,0);
				if(m_bDSFlag)
					g_pGetData->DSGetMinData(m_pStkInfo);
				else	
				{
					short nNT = GetNowTMinute();
					g_pGetData->GetMinuteData(m_pStkInfo,FALSE,nNT,m_MinuteData,TimeSec);
					if(nNT>0) 
					{
						m_nTodayMin = nNT;
						m_bDataReady = bDataReady = TRUE;
					}
				}
				//如果有领先或指数量比
				if(!m_bDSFlag && (m_bZsLx || (m_bZs && m_nStyle==ZST_LB)))
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_LB);	//量比
					g_pGetData->GetLead(m_pStkInfo);
				}
				if(!m_bDSFlag && m_nStyle==ZST_MMLD)
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_MMLD);	//买卖力道
					g_pGetData->GetMmld(m_pStkInfo);
				}
				else if(!m_bDSFlag && m_nStyle==ZST_AUCTION)			//竞价图
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_AUCTION);
					g_pGetData->GetAuction(m_pStkInfo,0,MAX_AUCTIONNUM);
				}
				else if(!m_bDSFlag && m_nStyle==ZST_TOTALORDER)			//总买总卖
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_TOTALORDER);
					g_pGetData->GetZmzm(m_pStkInfo);
				}
				else if(!m_bDSFlag && m_nStyle==ZST_TOTALCANCEL)		//净挂净撤
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_TOTALCANCEL);
					g_pGetData->GetJgjc(m_pStkInfo);
				}
				else if(!m_bDSFlag && m_nStyle==ZST_CANCELSUM)			//撤单累计
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_CANCELSUM);
					g_pGetData->GetJgjc(m_pStkInfo);
				}
				else if(!m_bDSFlag && m_nStyle==ZST_TICKNUM)			//涨跌动因统计
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_TICKNUM);
					g_pGetData->GetCjbs(m_pStkInfo);
				}
				else if(!m_bDSFlag && m_nStyle==ZST_LARGETICK)			//大单成交笔数统计
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_LARGETICK);
					g_pGetData->GetCjbs(m_pStkInfo);
				}
				else if(!m_bDSFlag && m_nStyle==ZST_IOVOL)				//成交量统计
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_IOVOL);
					g_pGetData->GetCjtj(m_pStkInfo);
				}
				else if(!m_bDSFlag && m_nStyle==ZST_LARGEVOL)			//大单成交量统计
				{
					g_nAssisID = SetComAssisID(m_nWhichUnit,ZST_LARGEVOL);
					g_pGetData->GetCjtj(m_pStkInfo);
				}
				//发送叠加股票的分时图请求
				Overlap_ReqData();
				//发送多日分时图的请求
				ManyDay_ReqData(g_lOpenRq);
			}
		}
		else if(m_bDSFlag)
		{
			g_pGetData->DSGetMinDataACK(m_MinuteData);
			m_nTodayMin = ItemNum;
		}
		else 
		{
			g_pGetData->GetMinuteDataACK(m_pStkInfo,m_MinuteData,NULL);
			m_nTodayMin = ItemNum;
		}
		//重新计算Minute,因为由于时间不匹配原因,Minute不准确
		for(i=0;i<m_nTodayMin;i++)
			m_MinuteData[i].Minute = GetMinuteFromFZ(i,1,TimeSec);
		//平滑
		if (m_Hq.Close<COMPPREC && m_Hq.Open>COMPPREC)
			m_Hq.Close=m_Hq.Open;
		SetBasePrice();
		if(m_MinuteData[0].Now < COMPPREC)
		{
			memset(m_MinuteData,0,sizeof(MinuteData));
			m_MinuteData[0].Minute = GetMinuteFromFZ(0,1,TimeSec);
			m_MinuteData[0].Now = m_Hq.Close;
			m_MinuteData[0].Average = m_Hq.Close;
		}
		if(m_MinuteData[0].Average < COMPPREC)
			m_MinuteData[0].Average = m_Hq.Close;
		//
		if(m_bZs && m_bZsLx && g_GH.bZsShowAverage)
			ReCalcZSAverage();
		//
		if(m_pStkInfo->bigmarket==QH_TYPE && m_MinuteData[0].VolInStock_Lead.VolInStock<1)
		{
			m_MinuteData[0].VolInStock_Lead.VolInStock = m_HqEx.PreVolIn;
			if(m_MinuteData[0].VolInStock_Lead.VolInStock<1)
				m_MinuteData[0].VolInStock_Lead.VolInStock = m_Hq.Yield_VolInStock.VolInStock;
		}
		if(m_bMinuteTick_Special&&m_MinuteData[0].VolInStock_Lead.Lead<COMPPREC)
		{
			m_MinuteData[0].VolInStock_Lead.Lead = m_Hq.Yield_VolInStock.Yield/100.0;
			for(i=0;m_pStkInfo->setcode==SH && i<g_nETFInfoNum;i++)
			{
				if(g_ETFInfo[i].setcode==SH && strcmp(m_pStkInfo->Code,g_ETFInfo[i].Code)==0)
				{	//对于ETF,读取IOPVCode的昨收盘
					struct CurrStockData tmpHQ;
					g_pStockIO->ReadCurrStockDataForCode(&tmpHQ,g_ETFInfo[i].IOPVCode);
					m_MinuteData[0].VolInStock_Lead.Lead = tmpHQ.Close;
					break;
				}
			}						
			if(m_MinuteData[0].VolInStock_Lead.Lead<COMPPREC)
				m_MinuteData[0].VolInStock_Lead.Lead=m_Hq.Close;
		}
		for(i = 1;i < ItemNum;i++)
		{
			if(m_MinuteData[i].Now < COMPPREC)
			{
				memset(m_MinuteData+i,0,sizeof(MinuteData));
				m_MinuteData[i].Minute = GetMinuteFromFZ(i,1,TimeSec);
				m_MinuteData[i].Now = m_MinuteData[i-1].Now;
				m_MinuteData[i].Average = m_MinuteData[i-1].Average;
			}
			if(m_MinuteData[i].Average < COMPPREC)
				m_MinuteData[i].Average = m_MinuteData[i-1].Average;
			if(m_pStkInfo->bigmarket==QH_TYPE && m_MinuteData[i].VolInStock_Lead.VolInStock<1)
				m_MinuteData[i].VolInStock_Lead.VolInStock = m_MinuteData[i-1].VolInStock_Lead.VolInStock;
			if(m_bMinuteTick_Special&&m_MinuteData[i].VolInStock_Lead.Lead<COMPPREC)
				m_MinuteData[i].VolInStock_Lead.Lead = m_MinuteData[i-1].VolInStock_Lead.Lead;
		}
	}
	else
	{
		ItemNum = m_nRealTotalMin;
		if(!bDataReady)
		{
			if(bReqData)	//如果要请求数据
			{
				//先请求分时数据
				m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				g_nAssisID = SetComAssisID(m_nWhichUnit,0);
				if(m_bDSFlag)
					g_pGetData->DSGetHisMinData(m_pStkInfo, m_lDate);
				else g_pGetData->GetHisMinuteData(m_pStkInfo,m_lDate);
			}
			Overlap_ReqHis();
			//发送多日分时图的请求
			ManyDay_ReqData(m_lDate);
		}
		else
		{
			m_nTodayMin = ItemNum;
			if(m_bDSFlag)
			{
				g_pGetData->DSGetHisMinDataACK(m_MinuteData, &(m_Hq.Close));
				//重新计算Minute,因为由于时间不匹配原因,Minute不准确
				for(i=0;i<m_nTodayMin;i++)
					m_MinuteData[i].Minute = GetMinuteFromFZ(i,1,TimeSec);
				//平滑
				if(m_MinuteData[0].Now < COMPPREC)
				{
					memset(m_MinuteData,0,sizeof(MinuteData));
					m_MinuteData[0].Minute = GetMinuteFromFZ(0,1,TimeSec);
					m_MinuteData[0].Now = m_Hq.Close;
					m_MinuteData[0].Average = m_Hq.Close;
				}
				if(m_MinuteData[0].Average < COMPPREC)
					m_MinuteData[0].Average = m_Hq.Close;
				if(m_pStkInfo->bigmarket==QH_TYPE && m_MinuteData[0].VolInStock_Lead.VolInStock<1)
				{
					m_MinuteData[0].VolInStock_Lead.VolInStock = m_HqEx.PreVolIn;
					if(m_MinuteData[0].VolInStock_Lead.VolInStock<1)
						m_MinuteData[0].VolInStock_Lead.VolInStock = m_Hq.Yield_VolInStock.VolInStock;
				}
				for(i = 1;i < ItemNum;i++)
				{
					if(m_MinuteData[i].Now < COMPPREC)
					{
						memset(m_MinuteData+i,0,sizeof(MinuteData));
						m_MinuteData[i].Minute = GetMinuteFromFZ(i,1,TimeSec);
						m_MinuteData[i].Now = m_MinuteData[i-1].Now;
						m_MinuteData[i].Average = m_MinuteData[i-1].Average;
					}
					if(m_MinuteData[i].Average < COMPPREC)
						m_MinuteData[i].Average = m_MinuteData[i-1].Average;
					if(m_pStkInfo->bigmarket==QH_TYPE && m_MinuteData[i].VolInStock_Lead.VolInStock<1)
						m_MinuteData[i].VolInStock_Lead.VolInStock = m_MinuteData[i-1].VolInStock_Lead.VolInStock;
				}
			}
			else 
				g_pGetData->GetHisMinuteDataACK(m_pStkInfo,m_MinuteData,&(m_Hq.Close),m_lDate);

			fBasePrice = m_Hq.Close;
		}
	}
	//校正指数的成交量(即成交金额)
	if(m_bZs)
	{
		for(i=0;i < ItemNum;i++)
		{
			if(m_MinuteData[i].NowVol > 21*10000*10000) //容错: 如果大盘的成交额有错(溢出了 超过DWORD表示范围,不显示)
			{
				m_MinuteData[i].NowVol = 0;
				if(i > 0) m_MinuteData[i].NowVol = m_MinuteData[i-1].NowVol*100;
			}
			m_MinuteData[i].NowVol /= 100; //将成交金额变成以万为单位的数值
		}
	}
	//计算量比等值
	long ltmp = 0;
	if(!m_bZs && m_pStkInfo->VolBase>0)
	{
		for(i=0;i<m_nTodayMin;i++)
		{ 
			ltmp+=m_MinuteData[i].NowVol;
			m_LBdata[i]=(float)ltmp/(i+1)/m_pStkInfo->VolBase;
		}
	}
	//读数据之前，将上一次的量请零
	m_lLastMinVolume = 0;
	m_nLastRefreshMinute = -1;
	m_nLastAuctionMinute = -1;
	if(m_lDate==0 && bDataReady && m_nTodayMin>0 && m_Hq.Now > COMPPREC) //当天,则需要动态维护.如果没有读出来，则动态维护之
		DTWH_currMinute();
	if(bDataReady)	//第一次读取数据，也要记录上一次的量
	{
		long ltmp2 = 0;
		for(int i=0;i < m_nTodayMin-1;i++)
			ltmp2+=m_MinuteData[i].NowVol;
		m_lLastMinVolume = ltmp2;
		if(m_bDSFlag) 
			m_nLastRefreshMinute = GetDSMinute(m_pTimer,m_bYestodayFalg);
		else 
			m_nLastRefreshMinute = GetTdxMinute(TimeSec);
	}
	m_nCurMinute = m_nTodayMin;
	CalcIt();
}

void UZst::CalcIt()
{
	//分时数据
	double fMaxPrice = (m_Hq.Max>COMPPREC)?m_Hq.Max:m_Hq.Close;
	double fMinPrice = (m_Hq.Min>COMPPREC)?m_Hq.Min:m_Hq.Close;
	int i=0,j=0,k=0;
	//
	m_lMaxVol = m_lMaxQHPos = 30;
	if(m_nCurMinute > 0 && m_bPlayStatus)
		fMinPrice = fMaxPrice = m_MinuteData[0].Now;
	m_lMinQHPos = m_MinuteData[0].VolInStock_Lead.VolInStock;
	for(i=0;i<m_nCurMinute;i++)
	{ 
		fMaxPrice = max(fMaxPrice,m_MinuteData[i].Now);
		fMinPrice = min(fMinPrice,m_MinuteData[i].Now);
		if(m_bMinuteTick_Special&&m_MinuteData[i].VolInStock_Lead.Lead>COMPPREC)
		{
			fMaxPrice = max(fMaxPrice,m_MinuteData[i].VolInStock_Lead.Lead);
			fMinPrice = min(fMinPrice,m_MinuteData[i].VolInStock_Lead.Lead);
		}
		if(m_MinuteData[i].NowVol > 2000000000l)
			m_MinuteData[i].NowVol=0;		
		m_lMaxVol   = max(m_lMaxVol,m_MinuteData[i].NowVol);
		m_lMaxQHPos = max(m_lMaxQHPos,m_MinuteData[i].VolInStock_Lead.VolInStock);
		m_lMinQHPos = min(m_lMinQHPos,m_MinuteData[i].VolInStock_Lead.VolInStock);
	}
	//如果是多日走势图
	if(m_bManyDayZST)
	{
		for(i=m_nZSTManyDay-1;i >= 0;i--)
		{
			if(m_fManyDayClose[i] < COMPPREC) continue;
			for(j=0;j<m_nRealTotalMin;j++)
			{
				fMaxPrice = max(fMaxPrice,m_ManyDayMinuteData[i][j].Now);
				fMinPrice = min(fMinPrice,m_ManyDayMinuteData[i][j].Now);
				if(m_bMinuteTick_Special&&m_ManyDayMinuteData[i][j].VolInStock_Lead.Lead>COMPPREC)
				{
					fMaxPrice = max(fMaxPrice,m_ManyDayMinuteData[i][j].VolInStock_Lead.Lead);
					fMinPrice = min(fMinPrice,m_ManyDayMinuteData[i][j].VolInStock_Lead.Lead);
				}
				m_lMaxVol   = max(m_lMaxVol,m_ManyDayMinuteData[i][j].NowVol);
				m_lMaxQHPos = max(m_lMaxQHPos,m_ManyDayMinuteData[i][j].VolInStock_Lead.VolInStock);
				m_lMinQHPos = min(m_lMinQHPos,m_ManyDayMinuteData[i][j].VolInStock_Lead.VolInStock);
			}
		}
	}
	m_lMaxVol *= 1.1;
	long nGap = m_lMaxQHPos-m_lMinQHPos;
	m_lMinQHPos -= max(nGap,100)*0.3;
	m_lMinQHPos =  max(0,m_lMinQHPos);
	m_lMaxQHPos += max(nGap,100)*0.3;
	fMaxDelta=max(fabs(fMaxPrice-fBasePrice),fabs(fMinPrice-fBasePrice));
	fMaxDelta=max(fMaxDelta,fBasePrice*((m_bZs&&m_Hq.Close>1000.0)?0.01:0.004)); //将指数的最小绝对范围改为1%
	//
	double fMaxPrice2 = 0.0,fMinPrice2 = 0.0;
	float ftmpclose = 0.0;
	if(m_nOverlapNum>99) m_nOverlapNum = g_nOverlapNum;
	for(int kk=0;kk < m_nOverlapNum;kk++)
	{
		if(!IsSameOpenCloseTimer(m_pOverlapStkInfo[kk],m_pStkInfo)) continue;
		if(m_pOverlapStkInfo[kk] && m_OverlapMinuteData[kk][0].Now > COMPPREC) //数据到了才行
		{
			if(m_lDate==0)
				ftmpclose = g_pStockIO->GetStockClose(m_pOverlapStkInfo[kk]);
			else ftmpclose = m_OverlapHq[kk].Close;
			if(ftmpclose < COMPPREC) continue;
			fMaxPrice2 = fBasePrice;
			fMinPrice2 = fBasePrice;
			for(i=0;i<m_nCurMinute;i++)
			{ 
				fMaxPrice2 = max(fMaxPrice2,m_OverlapMinuteData[kk][i].Now*fBasePrice/ftmpclose);
				fMinPrice2 = min(fMinPrice2,m_OverlapMinuteData[kk][i].Now*fBasePrice/ftmpclose);
			}
			float tmpmax = max(fabs(fMaxPrice2-fBasePrice),fabs(fMinPrice2-fBasePrice));
			if(fMaxDelta < tmpmax) fMaxDelta = tmpmax;
		}
	}
	//如果是TP坐标,需重新计算fMaxDelta和fMaxDelta
	if(g_bTPAxis)
	{
		fMaxDelta = max(fMaxDelta,GetTPPrice(m_pStkInfo,m_Hq.Close,TRUE)-fBasePrice);
		fMaxDelta = max(fMaxDelta,GetTPPrice(m_pStkInfo,m_Hq.Close,FALSE)-fBasePrice);
	}
	//平滑均价,如果有特别异常的Average(转码机不可避免的错误)
	BOOL bErrAverage = FALSE;
	for(i=0; i< m_nCurMinute;i++)
	{
		if(fabs(m_MinuteData[i].Average-fBasePrice) > fMaxDelta*3.0)
		{	bErrAverage = TRUE; break;	}
	}
	if(bErrAverage)
	{
		if(fabs(m_MinuteData[0].Average-fBasePrice) > fMaxDelta*3.0)//如果均价有异常，则平滑之
			m_MinuteData[0].Average = m_Hq.Close;
		for(i = 1;i < m_nCurMinute;i++)
		{
			if(fabs(m_MinuteData[i].Average-fBasePrice) > fMaxDelta*3.0)
				m_MinuteData[i].Average = m_MinuteData[i-1].Average;
		}
	}
	//根据均价重新计算fMaxDelta
	for(i = 0; i < m_nCurMinute; i++)
	{
		if(fabs(m_MinuteData[i].Average - fBasePrice) > fMaxDelta)
			fMaxDelta = fabs(m_MinuteData[i].Average-fBasePrice);
	}
	if(!g_bTPAxis)
		fMaxDelta=max(fMaxDelta,(m_XsFlag==2)?0.07:0.007); //保证每格刻度不重复
	//
	fMaxLx = 0;
	if(m_bZsLx)
	{
		for (i=0;i<m_nCurMinute;i++)
			fMaxLx=max(fMaxLx,fabs(m_LBdata[i]));
	}
	if(fMaxLx == 0) fMaxLx = 1.0; //防止fMaxLx为0
	fMaxLx *= 3.0;
	//量比
	m_fMinLB=-0.01;
	m_fMaxLB=0.01;
	for (i=0;i<m_nCurMinute;i++)
	{
		m_fMaxLB=max(m_fMaxLB,m_LBdata[i]);
		m_fMinLB=min(m_fMinLB,m_LBdata[i]);
	}
	double gg = (m_fMaxLB-m_fMinLB)/16.0;
	m_fMaxLB+=gg;
	m_fMinLB-=gg;
	//ADL
	m_lMaxADL	= 4;
	for(i=0;i<m_nCurMinute-1;i++)
	{
		m_lMaxADL=max(m_lMaxADL,m_MinuteData[i].Buyv);
		m_lMaxADL=max(m_lMaxADL,m_MinuteData[i].Sellv);
	}
	m_lMaxADL = max(m_lMaxADL,4);
	m_lMaxADL*=1.05;
	if(m_lMaxADL%4!=0)
	{
		m_lMaxADL /= 4;
		m_lMaxADL += 1;
		m_lMaxADL *= 4;
	}
	//总买总卖
	m_lMaxBSVol	= 4;
	for(i=0;i<m_nCurMinute;i++)
	{
		m_lMaxBSVol=max(m_lMaxBSVol,m_NormalStat[i].TotalBuyv);
		m_lMaxBSVol=max(m_lMaxBSVol,m_NormalStat[i].TotalSellv);
	}
	m_lMaxBSVol = max(m_lMaxBSVol,4);
	m_lMaxBSVol*=1.05;
	if(m_lMaxBSVol%4!=0)
	{
		m_lMaxBSVol /= 4;
		m_lMaxBSVol += 1;
		m_lMaxBSVol *= 4;
	}
	m_dTotalOrder.SetLimit(m_lMaxBSVol,0,ZST_TOTALORDER);
	//净挂净撤
	double fBCancelSum = 0.0;
	double fSCancelSum = 0.0;
	m_lMaxOaCVol	= 4;
	m_fMaxCancelSum	= 4;
	for (i=0;i<m_nCurMinute;i++)
	{
		if(i>0)
		{
			m_lMaxOaCVol=max(m_lMaxOaCVol,long(m_NormalStat[i].fBOrder));
			m_lMaxOaCVol=max(m_lMaxOaCVol,long(m_NormalStat[i].fSOrder));
		}
		m_lMaxOaCVol=max(m_lMaxOaCVol,long(m_NormalStat[i].fBCancel));
		m_lMaxOaCVol=max(m_lMaxOaCVol,long(m_NormalStat[i].fSCancel));
		fBCancelSum += max(0,m_NormalStat[i].fBCancel);
		fSCancelSum += max(0,m_NormalStat[i].fSCancel);
	}
	m_fMaxCancelSum = max(fBCancelSum,fSCancelSum);
	m_fMaxCancelSum = max(m_fMaxCancelSum,4);
	m_lMaxOaCVol = max(m_lMaxOaCVol,4);
	m_lMaxOaCVol*=1.05;
	if(m_lMaxOaCVol%4!=0)
	{
		m_lMaxOaCVol /= 4;
		m_lMaxOaCVol += 1;
		m_lMaxOaCVol *= 4;
	}
	m_dCancelOrder.SetLimit(m_lMaxOaCVol,-m_lMaxOaCVol,ZST_TOTALCANCEL);
	m_fMaxCancelSum*=1.05;
	m_dCancelOrder.SetLimit(m_fMaxCancelSum,0,ZST_CANCELSUM);
	//涨跌动因
	fBCancelSum = 0.0;
	fSCancelSum = 0.0;
	m_lMaxCjbs = 4;
	for(i=0;i<m_nCurMinute;i++)
	{
		fBCancelSum = max(fBCancelSum,m_TickStat[i].VolNum[0][0]+m_TickStat[i].VolNum[0][2]);
		fSCancelSum	= max(fSCancelSum,m_TickStat[i].VolNum[0][1]+m_TickStat[i].VolNum[0][3]);
	}
	m_lMaxCjbs = max(fBCancelSum,fSCancelSum);
	m_lMaxCjbs = max(m_lMaxCjbs,4);
	m_lMaxCjbs *=1.05;
	if(m_lMaxCjbs%4!=0)
	{
		m_lMaxCjbs /= 4;
		m_lMaxCjbs += 1;
		m_lMaxCjbs *= 4;
	}
	m_dCjbsStat.SetLimit(m_lMaxCjbs,0,ZST_TICKNUM);
	//大单笔数
	fBCancelSum = 0.0;
	fSCancelSum = 0.0;
	m_lMaxLargeCjbs	= 4;
	for(i=0;i<m_nCurMinute;i++)
	{
		fBCancelSum = max(fBCancelSum,m_TickStat[i].VolNum[1][0]+m_TickStat[i].VolNum[1][2]);
		fSCancelSum	= max(fSCancelSum,m_TickStat[i].VolNum[1][1]+m_TickStat[i].VolNum[1][3]);
	}
	m_lMaxLargeCjbs = max(fBCancelSum,fSCancelSum);
	m_lMaxLargeCjbs = max(m_lMaxLargeCjbs,4);
	m_lMaxLargeCjbs *=1.05;
	if(m_lMaxLargeCjbs%4!=0)
	{
		m_lMaxLargeCjbs /= 4;
		m_lMaxLargeCjbs += 1;
		m_lMaxLargeCjbs *= 4;
	}
	m_dCjbsStat.SetLimit(m_lMaxLargeCjbs,0,ZST_LARGETICK);
	//成交统计
	fBCancelSum = 0.0;
	fSCancelSum = 0.0;
	m_fMaxIOVol	= 4;
	for(i=0;i<m_nCurMinute;i++)
	{
		fBCancelSum = max(fBCancelSum,m_TickStat[i].fVol[0][0]);
		fSCancelSum	= max(fSCancelSum,m_TickStat[i].fVol[0][1]);
	}
	m_fMaxIOVol = max(fBCancelSum,fSCancelSum);
	m_fMaxIOVol = max(m_fMaxIOVol,4);
	m_fMaxIOVol *=1.05;
	m_dCjtjStat.SetLimit(m_fMaxIOVol,0,ZST_IOVOL);
	//大单成交
	fBCancelSum = 0.0;
	fSCancelSum = 0.0;
	m_fMaxLargeVol	= 4;
	for(i=0;i<m_nCurMinute;i++)
	{
		fBCancelSum = max(fBCancelSum,m_TickStat[i].fVol[1][0]+m_TickStat[i].fVol[1][2]);
		fSCancelSum	= max(fSCancelSum,m_TickStat[i].fVol[1][1]+m_TickStat[i].fVol[1][3]);
	}
	m_fMaxLargeVol = max(fBCancelSum,fSCancelSum);
	m_fMaxLargeVol = max(m_fMaxLargeVol,4);
	m_fMaxLargeVol *=1.05;
	m_dCjtjStat.SetLimit(m_fMaxLargeVol,0,ZST_LARGEVOL);
	//竞价图
	m_lMaxVirtualPxOff	= 0;
	m_lMaxVirtualVol	= 0;
	for(i=0;i<m_nVtlData;i++)
	{
		m_lMaxVirtualPxOff	= max(m_lMaxVirtualPxOff,fabs(m_pVtlData[i].fPrice-fBasePrice));
		m_lMaxVirtualVol	= max(m_lMaxVirtualVol,m_pVtlData[i].dwVolume);
	}
	m_lMaxVirtualVol	*= 1.1;
	//价位预警
	m_fWarnPriceH = m_fWarnPriceL = -COMPPREC;
	BOOL bExist = FALSE;
	for(i = 0;i < g_pAbsConditionNum;i++)//看是否已经存在这只股票
	{
		if(m_pStkInfo->setcode==g_pAbsCondition[i].setcode && strcmp(m_pStkInfo->Code,g_pAbsCondition[i].code)==0)
		{	bExist = TRUE;break; }
	}
	if(bExist)
	{
		if(g_pAbsCondition[i].fHighPrice>COMPPREC)
			m_fWarnPriceH = g_pAbsCondition[i].fHighPrice;
		if(g_pAbsCondition[i].fLowPrice>COMPPREC)
			m_fWarnPriceL = g_pAbsCondition[i].fLowPrice;
	}
}

void UZst::DrawFrame(CDC *pDC)
{
	m_PeriodRc.SetRect(m_rcIn.right-ZST_RWIDTH,m_rcIn.bottom-ZST_BHEIGHT,m_rcIn.right,m_rcIn.bottom);
//	g_d.Draw3DBar3(pDC, m_PeriodRc, RGB(128,128,128),FALSE);
//	g_d.DisplayText(pDC, m_PeriodRc, VipColor.FLOATCURSORCOLOR, DT_CENTER|DT_VCENTER, "看K线");

	CSize size;
	int i,j;
	float *pZstRatio = ZstRatio;
	if(g_GH.bDenseZSTAxis)
		pZstRatio = ZstRatio_dense;
	//画边框线，并计算yStart,xEnd等尺寸
	yStart = (m_nStyle>=ZST_SMALL)?12:ZST_THEIGHT;
	if(m_nStyle==ZST_GPZS)
		yStart = ZST_THEIGHT;
	if(m_nStyle>=ZST_MULTI)
	{
		g_d.DrawLine(pDC,0,0,0,m_rcIn.bottom);
		xAucStart = xStart	= 0;
		yEndBot = m_rcIn.bottom-10;
		if(m_nStyle==ZST_MULTI)
			yEndBot = m_rcIn.bottom;
		xEnd = max(0,m_rcIn.right-ZST_RWIDTH);
	}
	else
	{
		xAucStart = xStart  = ZST_LWIDTH;
		yEndBot = max(yStart,m_rcIn.bottom-ZST_BHEIGHT);
		xEnd	= max(ZST_LWIDTH,m_rcIn.right-ZST_RWIDTH);

		if(m_nStyle==ZST_AUCTION)
		{
			xStart += ZST_AUCTIONWIDTH;
			g_d.Bar(pDC,xAucStart,yStart,xStart,yEndBot,GRAYBACK_BRUSH);
		}

		if(!m_nInCom2 && !m_nInHisDlg)
		{
		//	m_ViewZoomRect.SetRect(xEnd-13, m_rcIn.top+4, xEnd-3, m_rcIn.top+14);
		//	m_ViewZoomRect.InflateRect(3,4,3,4);
			/*
			CComView *pComView = (CComView*)m_pView;
			if(pComView->IsHQUnitShow())
			{
				g_d.DrawRect(pDC, xEnd-12, m_rcIn.top+3, xEnd-3, m_rcIn.top+12, VipColor.AXISCOLOR, PS_SOLID, 1);
				g_d.DrawLine(pDC, xEnd-6, m_rcIn.top+3, xEnd-6, m_rcIn.top+12, VipColor.AXISCOLOR);
			}
			else
			{
				g_d.DrawRect(pDC, xEnd-10, m_rcIn.top+3, xEnd-3, m_rcIn.top+10, VipColor.AXISCOLOR);
				g_d.DrawRect(pDC, xEnd-12, m_rcIn.top+5, xEnd-5, m_rcIn.top+12, VipColor.AXISCOLOR);
				g_d.Bar(pDC, xEnd-11, m_rcIn.top+6, xEnd-5, m_rcIn.top+11, VipColor.BACKCOLOR);
			}
			*/
		#ifndef CITY10
			if(!m_bDSFlag)
			{
				BOOL bHasGubaBtn=FALSE;
			#ifdef LEVEL2
				bHasGubaBtn = TRUE;
			#else
				if(g_GC.bHasGPGrade) bHasGubaBtn = TRUE;
			#endif
				if(bHasGubaBtn)
				{
					m_GubaBtnRect=m_ViewZoomRect;
					m_GubaBtnRect.top+=2;
					m_GubaBtnRect.left-=35;m_GubaBtnRect.right-=15;
					g_GUI.DrawColorfulBtn(pDC,m_GubaBtnRect,_F("股吧"),VipColor.BACKCOLOR,VipColor.VOLCOLOR);
				}
				if(g_GC.bHasGPGrade)
				{
					m_ReportBtnRect=m_ViewZoomRect;
					m_ReportBtnRect.top+=2;
					m_ReportBtnRect.left-=65;m_ReportBtnRect.right-=45;
					g_GUI.DrawColorfulBtn(pDC,m_ReportBtnRect,_F("研究"),VipColor.BACKCOLOR,VipColor.KPCOLOR);
				}
			}
		#endif
		}
	}
	g_d.DrawLine(pDC,xStart,yStart,xEnd,yStart);
	g_d.DrawLine(pDC,xStart,yStart,xStart,yEndBot);

	g_d.Draw3DBar3(pDC,CRect(xStart, m_rcIn.top, xEnd, m_rcIn.top+ZST_THEIGHT), Scheme_Hold.VipColor.COMVIEWINFOBC, FALSE);

	if(m_nStyle==ZST_AUCTION)
	{
		g_d.DrawLine(pDC,xAucStart,yStart,xEnd,yStart);
		g_d.DrawLine(pDC,xAucStart,yStart,xAucStart,yEndBot);
	}
	if(m_nStyle>=ZST_MULTI)
		g_d.DrawLine(pDC,xEnd,yStart,xEnd,yEndBot);
	else
	{
		g_d.DrawLine(pDC,xEnd,m_rcIn.top,xEnd,m_rcIn.bottom);
		if(FALSE&&m_bShowLayout)
		{
			if(m_lDate>0)
				g_d.DisplayText(pDC,xEnd+10,yEndBot+1,VipColor.TXTCOLOR,_F("操作▲"));
			else if(!CanLevel2())
				g_d.DisplayText(pDC,xEnd+6,yEndBot+1,VipColor.KPCOLOR,_F("购买L2"));
			else
				g_d.DisplayText(pDC,xEnd+10,yEndBot+1,VipColor.TXTCOLOR,_F("布局▲"));
		}
	}
	g_d.DrawLine(pDC,0,yEndBot,m_rcIn.right,yEndBot);
	if((m_nStyle==ZST_LB || m_nStyle==ZST_MMLD) && !m_bDSFlag)
	{
		yVol		= yStart+pZstRatio[2]*(yEndBot-yStart);
		yEndVol		= yVol+pZstRatio[3]*(yEndBot-yStart);
		yStartBot	= yEndVol+ZST_THEIGHT;
		if(yStartBot > yEndBot) yStartBot = yEndVol;
	}
	else if(m_bLevel2Property && g_bSupportDDE && (m_nStyle>=ZST_TOTALORDER && m_nStyle<=ZST_LARGETICK))
	{	
		yVol		= yStart+pZstRatio[2]*(yEndBot-yStart);
		yEndVol		= yVol+pZstRatio[3]*(yEndBot-yStart);
		yStartBot	= yEndVol+ZST_THEIGHT;
		if(yStartBot > yEndBot) yStartBot = yEndVol;
		m_dTotalOrder.SetRectRgn(xStart,xEnd-xStart,yStartBot,yEndBot-yStartBot);
		m_dCancelOrder.SetRectRgn(xStart,xEnd-xStart,yStartBot,yEndBot-yStartBot);
		m_dCjbsStat.SetRectRgn(xStart,xEnd-xStart,yStartBot,yEndBot-yStartBot);
		m_dCjtjStat.SetRectRgn(xStart,xEnd-xStart,yStartBot,yEndBot-yStartBot);
	}
	else if(m_nStyle>=ZST_TOTALORDER && m_nStyle<=ZST_LARGETICK)	//进行股票切换的时候用得上,强制切为ZST_BIG
	{
		m_nStyle	= ZST_BIG;
		yVol		= yStart+pZstRatio[0]*(yEndBot-yStart);
		yEndVol		= yEndBot;
		yStartBot	= yEndVol;
	}
	else
	{
		yVol		= yStart+pZstRatio[0]*(yEndBot-yStart);
		yEndVol		= yEndBot;
		yStartBot	= yEndVol;
	}
	yBasePrice = (yVol-yStart)/2+yStart;

	if(g_GH.bDenseZSTAxis)
	{
		nPriceLnum=max(1,min(11,(yVol-yStart)/32));
		nVolLnum  =max(1,min(16,(yEndVol-yVol)/18));
		nBotLnum  =max(1,min(8,(yEndBot-yStartBot)/18));
	}
	else
	{
		nPriceLnum=max(1,min(7,(yVol-yStart)/32));
		nVolLnum  =max(1,min(8,(yEndVol-yVol)/18));
		nBotLnum  =max(1,min(4,(yEndBot-yStartBot)/18));
	}
	
	//画横轴线，如果是ZST_MULTI,ZST_SMALL等，则用虚线
	for(i=1;i<=nPriceLnum;i++)
	{
		if(m_nStyle >= ZST_MULTI)
			g_d.DotLine(pDC,xAucStart,yStart+i*(yBasePrice-yStart)/nPriceLnum+1,xEnd,yStart+i*(yBasePrice-yStart)/nPriceLnum+1,VipColor.AXISCOLOR,3);
		else
			g_d.DotLine(pDC,xAucStart,yStart+i*(yBasePrice-yStart)/nPriceLnum,xEnd,yStart+i*(yBasePrice-yStart)/nPriceLnum,VipColor.AXISCOLOR,3);
	}
	g_d.DrawLine(pDC,xAucStart,yBasePrice+1,xEnd,yBasePrice+1);
	for(i=1;i<=nPriceLnum;i++)
	{
		if(m_nStyle >= ZST_MULTI)
			g_d.DotLine(pDC,xAucStart,yBasePrice+i*(yVol-yBasePrice)/nPriceLnum+1,xEnd,yBasePrice+i*(yVol-yBasePrice)/nPriceLnum+1,VipColor.AXISCOLOR,3);
		else
			g_d.DotLine(pDC,xAucStart,yBasePrice+i*(yVol-yBasePrice)/nPriceLnum,xEnd,yBasePrice+i*(yVol-yBasePrice)/nPriceLnum,VipColor.AXISCOLOR,3);
	}
	g_d.DrawLine(pDC,xAucStart,yVol+1,xEnd,yVol+1);
	for(i=1;i<=nVolLnum;i++)
	{
		if(m_nStyle >= ZST_MULTI)
			g_d.DotLine(pDC,xAucStart,yVol+i*(yEndVol-yVol)/nVolLnum,xEnd,yVol+i*(yEndVol-yVol)/nVolLnum,VipColor.AXISCOLOR,3);
		else
			g_d.DotLine(pDC,xAucStart,yVol+i*(yEndVol-yVol)/nVolLnum,xEnd,yVol+i*(yEndVol-yVol)/nVolLnum,VipColor.AXISCOLOR,3);
	}
	if (m_nStyle==ZST_MMLD&&!m_bDSFlag)
	{
		g_d.DotLine(pDC,xStart,yStartBot+1,xEnd,yStartBot+1,VipColor.AXISCOLOR,2);
		for(i=1;i<nBotLnum;i++)
			g_d.DotLine(pDC,xStart,yStartBot+i*(yEndBot-yStartBot)/nBotLnum,xEnd,yStartBot+i*(yEndBot-yStartBot)/nBotLnum,VipColor.AXISCOLOR,3);
		g_d.DotLine(pDC,xStart,yStartBot+(yEndBot-yStartBot)/2,xEnd,yStartBot+(yEndBot-yStartBot)/2,VipColor.AXISCOLOR,2);
	}
	if(m_nStyle==ZST_LB&&!m_bDSFlag)
	{
		g_d.DotLine(pDC,xStart,yStartBot+1,xEnd,yStartBot+1,VipColor.AXISCOLOR,2);
		if(!m_bZs && m_fMinLB<1 && m_fMaxLB>1)
		{
			int nY0 = GetYLbFromValue(1.0);
			g_d.DotLine(pDC,xStart,nY0,xEnd,nY0,VipColor.AXISCOLOR,3);
		}
		else if(m_bZs && m_fMinLB<0 && m_fMaxLB>0)
		{
			int nY0 = GetYLbFromValue(0.0);
			g_d.DotLine(pDC,xStart,nY0,xEnd,nY0,VipColor.AXISCOLOR,3);
		}
	}
	if(m_nStyle==ZST_TOTALORDER&&!m_bDSFlag)
	{
		g_d.DotLine(pDC,xStart,yStartBot+1,xEnd,yStartBot+1,VipColor.AXISCOLOR,2);
		for(i=1;i<nBotLnum;i++)
			g_d.DotLine(pDC,xStart,yStartBot+i*(yEndBot-yStartBot)/nBotLnum,xEnd,yStartBot+i*(yEndBot-yStartBot)/nBotLnum,VipColor.AXISCOLOR,3);
		g_d.DotLine(pDC,xStart,yStartBot+(yEndBot-yStartBot)/2,xEnd,yStartBot+(yEndBot-yStartBot)/2,VipColor.AXISCOLOR,2);
	}
	if(m_nStyle==ZST_TOTALCANCEL&&!m_bDSFlag)
	{
		g_d.DotLine(pDC,xStart,yStartBot+1,xEnd,yStartBot+1,VipColor.AXISCOLOR,2);
		for(i=1;i<nBotLnum;i++)
			g_d.DotLine(pDC,xStart,yStartBot+i*(yEndBot-yStartBot)/nBotLnum,xEnd,yStartBot+i*(yEndBot-yStartBot)/nBotLnum,VipColor.AXISCOLOR,3);
		g_d.DotLine(pDC,xStart,yStartBot+(yEndBot-yStartBot)/2,xEnd,yStartBot+(yEndBot-yStartBot)/2,VipColor.AXISCOLOR,2);
	}
	if(m_nStyle==ZST_CANCELSUM&&!m_bDSFlag)
	{
		g_d.DotLine(pDC,xStart,yStartBot+1,xEnd,yStartBot+1,VipColor.AXISCOLOR,2);
		for(i=1;i<nBotLnum;i++)
			g_d.DotLine(pDC,xStart,yStartBot+i*(yEndBot-yStartBot)/nBotLnum,xEnd,yStartBot+i*(yEndBot-yStartBot)/nBotLnum,VipColor.AXISCOLOR,3);
	}
	if(m_nStyle==ZST_TICKNUM&&!m_bDSFlag)
	{
		g_d.DotLine(pDC,xStart,yStartBot+1,xEnd,yStartBot+1,VipColor.AXISCOLOR,2);
		for(i=1;i<nBotLnum;i++)
			g_d.DotLine(pDC,xStart,yStartBot+i*(yEndBot-yStartBot)/nBotLnum,xEnd,yStartBot+i*(yEndBot-yStartBot)/nBotLnum,VipColor.AXISCOLOR,3);
		g_d.DotLine(pDC,xStart,yStartBot+(yEndBot-yStartBot)/2,xEnd,yStartBot+(yEndBot-yStartBot)/2,VipColor.AXISCOLOR,2);
	}
	if(m_nStyle==ZST_LARGETICK&&!m_bDSFlag)
	{
		g_d.DotLine(pDC,xStart,yStartBot+1,xEnd,yStartBot+1,VipColor.AXISCOLOR,2);
		for(i=1;i<nBotLnum;i++)
			g_d.DotLine(pDC,xStart,yStartBot+i*(yEndBot-yStartBot)/nBotLnum,xEnd,yStartBot+i*(yEndBot-yStartBot)/nBotLnum,VipColor.AXISCOLOR,3);
	}
	if(m_nStyle==ZST_IOVOL&&!m_bDSFlag)
	{
		g_d.DotLine(pDC,xStart,yStartBot+1,xEnd,yStartBot+1,VipColor.AXISCOLOR,2);
		for(i=1;i<nBotLnum;i++)
			g_d.DotLine(pDC,xStart,yStartBot+i*(yEndBot-yStartBot)/nBotLnum,xEnd,yStartBot+i*(yEndBot-yStartBot)/nBotLnum,VipColor.AXISCOLOR,3);
		g_d.DotLine(pDC,xStart,yStartBot+(yEndBot-yStartBot)/2,xEnd,yStartBot+(yEndBot-yStartBot)/2,VipColor.AXISCOLOR,2);
	}
	if(m_nStyle==ZST_LARGEVOL&&!m_bDSFlag)
	{
		g_d.DotLine(pDC,xStart,yStartBot+1,xEnd,yStartBot+1,VipColor.AXISCOLOR,2);
		for(i=1;i<nBotLnum;i++)
			g_d.DotLine(pDC,xStart,yStartBot+i*(yEndBot-yStartBot)/nBotLnum,xEnd,yStartBot+i*(yEndBot-yStartBot)/nBotLnum,VipColor.AXISCOLOR,3);
	}
	short *stmp = TimeSec;
	nSecLens = 0;
	int nDotPeriod=(m_nStyle>=ZST_MULTI?60:30);//画虚线实线的周期
	if(m_bDSFlag)
	{
		PTIMEINF pTimer = g_pStockIO->DSGetTimeInf(m_pStkInfo->TimerID);
		nNowSect = 0;
		nSectNum = 1;
		nSectTime[0][0] = pTimer->OpenTimer[0];
		nSectTime[0][1] = pTimer->CloseTimer[0];
		nSecLens = nSectTime[0][1]-nSectTime[0][0];
		for(i=1;i<MAX_TIMESEC;i++)
		{
			if(pTimer->OpenTimer[i]&&pTimer->OpenTimer[i]!=pTimer->CloseTimer[i])
			{
				++nSectNum;
				nSectTime[i][0] = pTimer->OpenTimer[i];
				nSectTime[i][1] = pTimer->CloseTimer[i];
				nSecLens += nSectTime[i][1]-nSectTime[i][0];
			}
		}

		if(m_bManyDayZST)
		{
			for(j=1;j < m_nZSTManyDay+1;j++)
			{
				for(i=0;i<nSectNum;i++)
				{
					nSectTime[j*nSectNum+i][0] = nSectTime[i][0];
					nSectTime[j*nSectNum+i][1] = nSectTime[i][1];
					nSecLens+=nSectTime[i][1]-nSectTime[i][0];
				}
			}
			nSectNum *= (m_nZSTManyDay+1);
		}
		if(nSecLens>500)
		{
			nDotPeriod=m_nStyle>=ZST_MULTI?360:180;
		}
	}
	else
	{
		if(m_bTotalSect)
		{	
			if(m_bManyDayZST)
			{
				nSectNum = GetTdxSectNum(stmp);
				for(j=0;j < m_nZSTManyDay+1;j++)
				{
					for(i=0;i<nSectNum;i++)
					{
						nSectTime[j*nSectNum+i][0] = stmp[i*2];
						nSectTime[j*nSectNum+i][1] = stmp[i*2+1];
						nSecLens+=nSectTime[i][1]-nSectTime[i][0];
					}
				}
				nSectNum *= (m_nZSTManyDay+1);
			}
			else
			{
				nSectNum = GetTdxSectNum(stmp);
				for(i=0;i<nSectNum;i++)
				{
					nSectTime[i][0] = stmp[i*2];
					nSectTime[i][1] = stmp[i*2+1];
					nSecLens+=nSectTime[i][1]-nSectTime[i][0];
				}
			}
		}
		else
		{
			nNowSect = GetNowTdxSect(stmp);
			nSectTime[0][0] = stmp[nNowSect*2];
			nSectTime[0][1] = stmp[nNowSect*2+1];
			nSecLens = nSectTime[0][1]-nSectTime[0][0];
			nSectNum = 1;
		}
	}
	//开始画段间线条
	int locTime = nSectTime[0][0];
	if(!m_bManyDayZST)
	{
		for(i=0,j=0;i<nSecLens;i+=nDotPeriod)
		{
			if(locTime >= nSectTime[j][1])
			{
				j++;
				locTime=nSectTime[j][0];
				i = System2Tdx(locTime, TimeSec)-1;
			}
			else
				g_d.DotLine(pDC,xStart+i*(xEnd-xStart)*1.0/nSecLens,yStart,xStart+i*(xEnd-xStart)*1.0/nSecLens,yEndVol,VipColor.AXISCOLOR,2);
			locTime+=nDotPeriod;
		}
	}
	else
	{
		for(i=0,j=0;i<nSecLens;i+=nDotPeriod)
			g_d.DotLine(pDC,xStart+i*(xEnd-xStart)*1.0/nSecLens,yStart,xStart+i*(xEnd-xStart)*1.0/nSecLens,yEndVol,VipColor.AXISCOLOR,2);
	}
	int ntmpLen=0;
	for(i=0;i<nSectNum-1;i++)
	{
		ntmpLen+=nSectTime[i][1]-nSectTime[i][0];
		g_d.DrawLine(pDC,xStart+ntmpLen*(xEnd-xStart)/nSecLens,yStart,xStart+ntmpLen*(xEnd-xStart)/nSecLens,yEndVol);
	}
	fPointPerMinute = (xEnd-xStart)*1.0/nSecLens; //时间轴的单元
	//画标题区，分几种风格
	int   pos=xAucStart+2,XWidth=40;
	if(m_nStyle==ZST_MULTI && m_bSelected)	//如果在多股同列版面被选中
	{
		pDC->SetBkMode(OPAQUE);
		pDC->SetBkColor(VipColor.FLOATCURSORCOLOR);
	}
	if(m_nStyle<ZST_SMALL)
	{
		if(m_nStyle==ZST_MULTI || m_bDSFlag)
			size = g_d.DisplayText(pDC,pos,3,VipColor.TXTCOLOR,"%8s ",m_pStkInfo->Name);
		else
			size = g_d.DisplaySameWidtchText(pDC,pos,3,VipColor.TXTCOLOR,"%8s ",m_pStkInfo->Name);
		pos+=size.cx+6;
		if(!m_bTotalSect)
		{
			DC_DRAW_TEXT(TXTCOLOR,"区段",0);
			size = g_d.DisplayText(pDC,pos,3,VipColor.TXTCOLOR,"%s--",GetTdxTimeStr(nSectTime[0][0]));
			pos+=size.cx;
			size = g_d.DisplayText(pDC,pos,3,VipColor.TXTCOLOR,"%s  ",GetTdxTimeStr(nSectTime[0][1]));
			pos+=size.cx+2;
		}
	}
	else if(m_nStyle==ZST_GPZS)
	{
		size = g_d.DisplayText(pDC,pos,3,VipColor.TXTCOLOR,"%8s ",m_pStkInfo->Name);
		pos+=size.cx+6;
	}
	pDC->SetBkMode(TRANSPARENT);
	if(m_nStyle<ZST_MULTI)
	{
		if(m_bManyDayZST) //显示多日走势图
		{
			CString tmpManyDayTitle="";
			if(m_lDate==0)
				tmpManyDayTitle.Format("最近%d日分时",m_nZSTManyDay+1);
			else
				tmpManyDayTitle.Format("%s最近%d日分时",GetTdxDateStr(m_lDate),m_nZSTManyDay+1);
			size = g_d.DisplayText(pDC,pos,3,VipColor.ZBCOLOR[0],_F(tmpManyDayTitle));
			pos+=size.cx+4;
		}
		else if(!m_nInCom2)
		{
			if(m_pStkInfo->J_zjhhy>0)
			{
				ZJHHY_Cfg *pZcfg = g_pConstMng->GetHyFromHYID(m_pStkInfo->J_zjhhy);
				if(pZcfg)
				{
					char strhy[200]={0};
					sprintf(strhy, "  行业:%s  ", pZcfg->HYName);
					size = g_d.DisplayText(pDC,pos,3,VipColor.TXTCOLOR,_F(strhy));
					pos+=size.cx+4;
				}
			}

			size = g_d.DisplayText(pDC,pos,3,VipColor.ZBCOLOR[0],_F("分时")); 
			pos+=size.cx+4;
		}
		if(Width() > 300)
		{
			if(!m_nInCom2)
			{
				if(!m_bZs || (m_bZs && m_bZsLx && g_GH.bZsShowAverage))
				{
					size = g_d.DisplayText(pDC,pos,3,VipColor.ZBCOLOR[1],_F("均线")); 
					pos+=size.cx+4;
				}
				if(g_GH.bDiffVolColor)
				{
					if(m_bZs)
					{
						if(g_bDiffVolMode==0)
						{
							DC_DRAW_TEXT(KPCOLOR,"成",0);
							DC_DRAW_TEXT(DOWNCOLOR,"交",0);
						}
						else
						{
							DC_DRAW_TEXT(VOLCOLOR,"成",0);
							DC_DRAW_TEXT(KNCOLOR,"交",0);
						}
						DC_DRAW_TEXT(LEVELCOLOR,"额(万)",4);
					}
					else
					{
						if(g_bDiffVolMode==0)
						{
							DC_DRAW_TEXT(KPCOLOR,"成",0);
							DC_DRAW_TEXT(DOWNCOLOR,"交",0);
						}
						else
						{
							DC_DRAW_TEXT(VOLCOLOR,"成",0);
							DC_DRAW_TEXT(KNCOLOR,"交",0);
						}
						DC_DRAW_TEXT(LEVELCOLOR,"量",4);
					}
				}
				else
				{
					DC_DRAW_TEXT(VOLCOLOR,(m_bZs||m_bHKZs)?"成交额(万)":"成交量",4);
				}
				if(m_bMinuteTick_Special)
				{
					if(m_bETF)
					{
						DC_DRAW_TEXT(ZBCOLOR[2],"IOPV",4);
					}
					else
					{
						DC_DRAW_TEXT(ZBCOLOR[2],"时净值",4);
					}
				}
			}
			if((m_nStyle==ZST_LB || m_nStyle==ZST_MMLD) && !m_bDSFlag)
			{
				CString	lbTitle;
				if(m_nStyle==ZST_LB)	
					lbTitle=m_bZs?"多空指标":"量比";
				else	
					lbTitle=m_bZs?"涨跌率":"买卖力道";
				DC_DRAW_TEXT(LEVELCOLOR,lbTitle,4);
			}
			else if(m_nStyle==ZST_AUCTION && !m_bDSFlag)
			{
				DC_DRAW_TEXT(LEVELCOLOR,"竞价图",4);
			}
			else if(m_bLevel2Property && g_bSupportDDE && !m_bDSFlag && (m_nStyle>=ZST_TOTALORDER && m_nStyle<=ZST_LARGETICK))
			{
				CString	lbTitle="";
				switch(m_nStyle)
				{
				case ZST_TOTALORDER:	lbTitle="总买总卖";break;
				case ZST_TOTALCANCEL:	lbTitle="净挂净撤";break;
				case ZST_CANCELSUM:		lbTitle="撤单累计";break;
				case ZST_IOVOL:			lbTitle="主买主卖";break;
				case ZST_LARGEVOL:		lbTitle="大单动向";break;
				case ZST_TICKNUM:		lbTitle="涨跌动因";break;
				case ZST_LARGETICK:		lbTitle="大单差分";break;
				}
				DC_DRAW_TEXT(LEVELCOLOR,lbTitle,4);
			}
			if(m_pStkInfo->bigmarket==QH_TYPE)
			{
				size = g_d.DisplayText(pDC,pos,3,VipColor.LEVELCOLOR,_F("持仓量"));
				pos+=size.cx+4;
			}
			if(m_lDate == 0)
			{
				if(m_bYestodayFalg && m_HqEx.HqDate>0 && m_bDSFlag)
				{
					size = g_d.DisplayText(pDC,pos,3,VipColor.TXTCOLOR,_F("行情日期:%08d"),m_HqEx.HqDate);
					pos+=size.cx+4;
				}
				if(m_bDSFlag)
				{
					int nXH = DSGetMarketXH(m_pTimer->Market);
					int nDelayMin = g_pDSMarket[nXH].DelayHQ;
					if(nDelayMin>0)
					{
						if(m_pStkInfo->bigmarket==HKGP_TYPE && g_GC.bOnlyHK)
							size = g_d.DisplayText(pDC,pos,3,VipColor.TXTCOLOR,_F("所有股票报价及认股证报价均延迟最少%d分钟"),nDelayMin);
						else
							size = g_d.DisplayText(pDC,pos,3,VipColor.TXTCOLOR,_F("延时至少%d分钟"),nDelayMin);
						pos+=size.cx+4;
					}
				}
				if(Width() > 500)
				{
					//显示权证相关信息
					char strInfo[100]={0};
					if(m_bQz && !m_nInCom2 && !m_nInHisDlg && m_pStkInfo->J_tzmgjz>COMPPREC)
					{
						GetQzLinkInfo(strInfo,m_pStkInfo);
						if(strlen(strInfo)>0)
							size = g_d.DisplayText(pDC,pos,3,VipColor.TXTCOLOR,strInfo); pos+=size.cx+4;							
					}
					if(g_GH.bShowTPPriceInZST && !m_bZs && !m_nInHisDlg && !m_bDSFlag)
					{
						float ftmp1=GetTPPrice(m_pStkInfo,m_Hq.Close,TRUE);
						float ftmp2=GetTPPrice(m_pStkInfo,m_Hq.Close,FALSE);
						if(ftmp1>COMPPREC)
						{
							size = g_d.DisplayText(pDC,pos,3,VipColor.TXTCOLOR,_F("涨停:"));
							pos+=size.cx+1;
							size = g_d.BjPrintf(pDC,pos,3,ftmp1,m_Hq.Close,m_XsFlag,0);
							pos+=size.cx+4;
						}
						if(ftmp2>COMPPREC)
						{
							size = g_d.DisplayText(pDC,pos,3,VipColor.TXTCOLOR,_F("跌停:"));
							pos+=size.cx+1;
							size = g_d.BjPrintf(pDC,pos,3,ftmp2,m_Hq.Close,m_XsFlag,0);
							pos+=size.cx+4;
						}
					}
					//显示叠加股票名称
					int tmpXsFlag=2;
					for(int kk=0;kk < m_nOverlapNum;kk++)
					{
						if(m_pOverlapStkInfo[kk]&&IsSameOpenCloseTimer(m_pOverlapStkInfo[kk],m_pStkInfo) )
						{
							tmpXsFlag = GetXSFlag(m_pOverlapStkInfo[kk]);
							CurrStockData tmpStkData;
							if(g_pStockIO->ReadCurrStockData(m_pOverlapStkInfo[kk],&tmpStkData))
							{
								if(tmpXsFlag==2)
									sprintf(strInfo,"[%s %.2f%s]",m_pOverlapStkInfo[kk]->Name,tmpStkData.Now,
										(tmpStkData.Now>COMPPREC&&tmpStkData.Close>COMPPREC)?((tmpStkData.Now>tmpStkData.Close+COMPPREC)?_F("▲"):((tmpStkData.Now<tmpStkData.Close+COMPPREC)?_F("▼"):_F("◆"))):"");
								else if(tmpXsFlag==3)
									sprintf(strInfo,"[%s %.3f%s]",m_pOverlapStkInfo[kk]->Name,tmpStkData.Now,
										(tmpStkData.Now>COMPPREC&&tmpStkData.Close>COMPPREC)?((tmpStkData.Now>tmpStkData.Close+COMPPREC)?_F("▲"):((tmpStkData.Now<tmpStkData.Close+COMPPREC)?_F("▼"):_F("◆"))):"");
								else
									sprintf(strInfo,"[%s %.1f%s]",m_pOverlapStkInfo[kk]->Name,tmpStkData.Now,
										(tmpStkData.Now>COMPPREC&&tmpStkData.Close>COMPPREC)?((tmpStkData.Now>tmpStkData.Close+COMPPREC)?_F("▲"):((tmpStkData.Now<tmpStkData.Close+COMPPREC)?_F("▼"):_F("◆"))):"");
							}
							else sprintf(strInfo,"[%s]",m_pOverlapStkInfo[kk]->Name);
							size = g_d.DisplayText(pDC,pos,3,l_KLineColor[9+kk],strInfo); pos+=size.cx+4;
						}
					}
				}
			}			
		}
		if(m_bIPO)
		{
		//	g_BtnImage.Draw(pDC,BTN_IPO,CPoint(pos+4,4),ILD_NORMAL);
		//	m_CloseOpenRect.left=pos;
		//	m_CloseOpenRect.right=pos+20;
		//	m_CloseOpenRect.top=4;
		//	m_CloseOpenRect.bottom=18;
		//	pos+=20;		
		}
		if(!m_nInCom2 && !m_nInHisDlg && m_lDate==0 && m_nWhichUnit==0 && (Width()-pos)>200)
		{
			if(CanShowLevinInfo())
			{
				size = g_d.DisplayText(pDC,pos,3,VipColor.TXTCOLOR,"库存:");
				pos+=size.cx+1;
				size = g_d.DisplayText(pDC,pos,3,VipColor.VOLCOLOR,"%ld",m_lCanUseVol);
				pos+=size.cx+2;
				if(m_nWTNum==MAX_WTCJLIST_NUM)
				{
					pos+=2;
					size = g_d.DisplayText(pDC,pos,3,VipColor.KPCOLOR,"委托数太多,自动关闭图标");
					pos+=size.cx;
					g_bShowToken=FALSE;
				}
				else if(m_nWTNum>500 && g_bShowToken)
				{
					pos+=2;
					size = g_d.DisplayText(pDC,pos,3,VipColor.KPCOLOR,"委托数过多,建议关闭图标");
					pos+=size.cx;
				}
			}
			if( (g_nFlushRealInfo==1&&m_bFlushRealInfo) || g_nFlushRealInfo==2)
			{
				pDC->DrawIcon(pos,-12,m_hZstFlushIcon);
				size = g_d.DisplayText(pDC,pos+27,3,RGB(255,0,0),0,"即时播报");
				m_FlushRealInfoRect.left=pos;
				m_FlushRealInfoRect.right=pos+27+size.cx;
				m_FlushRealInfoRect.top=2;
				m_FlushRealInfoRect.bottom=18;
				pos+=27+size.cx+2;		
			}
		}
	}
	int tmpPos=pos;
	if(fBasePrice > COMPPREC && (m_nInCom2 || m_nStyle==ZST_MULTI) && (Width()-tmpPos) > 90) //多股图显示现价等
	{
		size = g_d.BjPrintf(pDC,pos,3,(m_Hq.Now>COMPPREC)?m_Hq.Now:m_Hq.Close,m_Hq.Close,m_XsFlag);pos+=size.cx+5;
		if(m_Hq.Now > COMPPREC && (Width()-tmpPos) > 130)
		{
			float ftmp = (m_Hq.Now-m_Hq.Close)/m_Hq.Close*100.0;
			size = g_d.BjPrintf(pDC,pos,3,ftmp,0.0,2,0,1);pos+=size.cx+3;
			if(m_Hq.Close > COMPPREC && (Width()-tmpPos) > 170)
			{
				if(m_Hq.Now-m_Hq.Close > 0)
					size = g_d.DisplayText(pDC,pos,3,VipColor.KPCOLOR,_F("▲"));
				else if(m_Hq.Now-m_Hq.Close < 0)
					size = g_d.DisplayText(pDC,pos,3,VipColor.DOWNCOLOR,_F("▼"));
				else
					size = g_d.DisplayText(pDC,pos,3,VipColor.LEVELCOLOR,"  ");
				pos+=size.cx;
				size = g_d.BjPrintf(pDC,pos,3,m_Hq.Now-m_Hq.Close,0.0,m_XsFlag);pos+=size.cx+3;
				if((Width()-tmpPos)>220)
				{
					if(!m_bZs)
					{
						DC_DRAW_TEXT(TXTCOLOR,"现量",2);
						size = g_d.DisplayText(pDC,pos,3,VipColor.VOLCOLOR,0,"%9d",m_Hq.NowVol);pos+=size.cx+3;
						if((Width()-tmpPos)>260 && m_pStkInfo->ActiveCapital > 1)
						{
							DC_DRAW_TEXT(TXTCOLOR,"换手",2);
							ftmp = m_Hq.Volume*m_pStkInfo->Unit/(m_pStkInfo->ActiveCapital*100.0);
							size = g_d.DisplayText(pDC,pos,3,VipColor.TXTCOLOR,0,"%9.2f%%",ftmp);pos+=size.cx+3;
						}
					}
					else if((Width()-tmpPos)>320 && (m_nGxdFlag==0 || m_nGxdFlag==1)) //对于深沪大盘,显示涨跌家数
					{
						DC_DRAW_TEXT(TXTCOLOR,"涨家数",2);
						size = g_d.DisplayText(pDC,pos,3,VipColor.KPCOLOR,0,"%9u",m_Hq.Buyv[0]);pos+=size.cx+3;
						DC_DRAW_TEXT(TXTCOLOR,"跌家数",2);
						size = g_d.DisplayText(pDC,pos,3,VipColor.DOWNCOLOR,0,"%9u",m_Hq.Sellv[0]);pos+=size.cx+3;
					}
				}
			}
		}
	}
}

void UZst::DrawIt(CDC *pDC)
{
	//如果空间太小，不画
	if(m_nStyle < ZST_MULTI && Width() < ZST_LWIDTH+ZST_RWIDTH+20 || Height() < ZST_BHEIGHT+20+20)
	{
		g_d.DrawRect(pDC,0,0,0,m_rcIn.bottom);
		return;
	}
	if(m_nStyle >= ZST_MULTI && Width() < ZST_RWIDTH+20 || Height() < ZST_BHEIGHT+10+20)
	{
		g_d.DrawLine(pDC,0,0,0,m_rcIn.bottom);
		return;
	}
	m_bShowLayout=((m_bLevel2Property && !m_nInCom2 && !m_nInHisDlg) || m_lDate>0);
#ifdef LEVEL2
	if(!CanLevel2() && OnLine && !m_nInCom2 && !m_nInHisDlg)
		m_bShowLayout = TRUE;
#endif
	if(!m_pStkInfo)
		return;

	fMaxPxOff	= fMaxDelta;
	m_lMaxVolEx	= m_lMaxVol;
	if(m_nStyle==ZST_AUCTION && !m_bDSFlag && !m_bManyDayZST)
	{
		fMaxPxOff	= max(fMaxDelta,m_lMaxVirtualPxOff);
		m_lMaxVolEx	= max(m_lMaxVol,m_lMaxVirtualVol);
	}

	DrawFrame(pDC);
	if(fBasePrice < COMPPREC) 
		return;
	if(m_nStyle < ZST_MULTI)	//画坐标轴区域
	{
		DrawAxisArea(pDC,CPoint(0,yEndBot),0);
		DrawAxisArea(pDC,CPoint(xEnd,0),2);
		DrawAxisArea(pDC,CPoint(0,0),1);
	}
	else 
		DrawAxisArea(pDC,CPoint(xEnd,0),3);

	if(m_lDate==0)
	{
		if(m_fWarnPriceH>COMPPREC && m_fWarnPriceH<fBasePrice+fMaxPxOff) 
			g_d.DotLine(pDC, m_rcIn.left+(m_nStyle<ZST_MULTI?ZST_LWIDTH:0)+1, GetYFromValue(m_fWarnPriceH), m_rcIn.right-ZST_RWIDTH, GetYFromValue(m_fWarnPriceH), RGB(192,0,192),2);
		if(m_fWarnPriceL>COMPPREC && m_fWarnPriceL>fBasePrice-fMaxPxOff) 
			g_d.DotLine(pDC, m_rcIn.left+(m_nStyle<ZST_MULTI?ZST_LWIDTH:0)+1, GetYFromValue(m_fWarnPriceL), m_rcIn.right-ZST_RWIDTH, GetYFromValue(m_fWarnPriceL), VipColor.KNCOLOR,2);
	}
	//开始画线
	int i=0;
	int ntmp1[1500],ntmp2[1500];
	CPen *oldpen = g_d.SelectPen(pDC,ZB1_PEN);
	int nAbsTotalMinute = m_nTodayMin;
	int nStartMinute = 0;
	if(m_bPlayStatus&&m_bManyDayZST)
	{
		nAbsTotalMinute = m_nRealTotalMin*m_nZSTManyDay+m_nTodayMin;
		nStartMinute = m_nTodayMin - nAbsTotalMinute;
	}
	//需要画竞价图的,画竞价图
	if(m_nStyle==ZST_AUCTION && !m_bDSFlag && !m_bManyDayZST)
	{
		
		g_d.DotLine(pDC,xAucStart+ZST_AUCTIONWIDTH/2,yStart,xAucStart+ZST_AUCTIONWIDTH/2,yEndVol,VipColor.AXISCOLOR,2);
		for(i=0;i<m_nVtlData;i++)
		{
			g_d.DrawO(pDC,GetXFromTickNo(i),GetYFromValue(m_pVtlData[i].fPrice),NULL,2);
			if(i==0)
			{
				if(m_bIPO && m_bUseOpenAxisForIPO && m_Hq.Open>COMPPREC)
				{
				}
				else g_d.DrawLine(pDC,xAucStart,GetYFromValue(m_Hq.Close),GetXFromTickNo(i),GetYFromValue(m_pVtlData[i].fPrice));
			}
			else g_d.DrawLine(pDC,GetXFromTickNo(i-1),GetYFromValue(m_pVtlData[i-1].fPrice),GetXFromTickNo(i),GetYFromValue(m_pVtlData[i].fPrice));
		}
		for(i=0;i<m_nVtlData;i++)
		{
			if(m_pVtlData[i].dwLeaveQty>0) g_d.SelectPen(pDC,ZST_UP_PEN);
			else if(m_pVtlData[i].dwLeaveQty<0) g_d.SelectPen(pDC,ZST_DOWN_PEN);
			else g_d.SelectPen(pDC,ZST_LEVEL_PEN);
			g_d.DrawLine(pDC,GetXFromTickNo(i),GetYVolFromValue(m_pVtlData[i].dwVolume),GetXFromTickNo(i),yEndVol);
		}
	}
	g_d.SelectPen(pDC,ZB1_PEN);
	if(m_nCurMinute>0)
	{
		//试图解决不能表示开盘情况的问题,只有一笔的话,不画了(处理三板)
		if(m_lDate==0 && !m_bManyDayZST 
			&& m_Hq.Open>COMPPREC && m_MinuteData[0].Now>COMPPREC && m_MinuteData[0].NowVol>0
			&& m_Hq.ItemNum>1)
			g_d.DrawLine(pDC,xStart,GetYFromValue(m_Hq.Open),GetXFromMinute(m_MinuteData[0].Minute),GetYFromValue(m_MinuteData[0].Now));
	}
	for(i=0;i<m_nCurMinute-1;i++)
	{
		if(m_MinuteData[i].Minute<nSectTime[0][0]) continue;
		ntmp1[i] = GetXFromMinute(m_MinuteData[i].Minute);
		ntmp2[i] = GetXFromMinute(m_MinuteData[i+1].Minute);
		if(ntmp1[i] > ntmp2[i])  //及时修正，防止反画，不能是>=
		{
			ntmp1[i] = ntmp2[i];
			ntmp1[i] = max(xStart,ntmp1[i]);
			ntmp1[i] = min(xEnd,ntmp1[i]);
			ntmp2[i] = ntmp1[i];
		}
		//画价线
		if(m_MinuteData[i].Now>COMPPREC && m_MinuteData[i+1].Now>COMPPREC)
			g_d.DrawLine(pDC,ntmp1[i],GetYFromValue(m_MinuteData[i].Now),ntmp2[i],GetYFromValue(m_MinuteData[i+1].Now));
		//画量比线
		if(m_nStyle == ZST_LB && !m_bDSFlag && (!m_bZs || i<m_nLeadData-1))
			g_d.DrawLine(pDC,ntmp1[i],GetYLbFromValue(m_LBdata[i]),ntmp2[i],GetYLbFromValue(m_LBdata[i+1]));
		if(OnLine)
		{
			//画买卖力道的第一根线
			if(m_nStyle == ZST_MMLD && m_lMaxADL > 5 && !m_bDSFlag)
				g_d.DrawLine(pDC,ntmp1[i],GetYMmldFromValue(m_MinuteData[i].Buyv),ntmp2[i],GetYMmldFromValue(m_MinuteData[i+1].Buyv));
		}
	}
	if(m_thunderposnum > 0)
	{
		int i,tdxmin,tmpx;
		for(i=0;i < m_thunderposnum;i++)
		{
			tdxmin = m_thunderpos[i].tdxmin;
			if(tdxmin < nSectTime[0][0]) continue;
			tmpx = GetXFromMinute(tdxmin);
			if(tmpx==xEnd) tmpx=xEnd-8;

			if(m_thunderpos[i].type==0)
			{
				if(m_thunderpos[i].thundernum > 1)
					pDC->DrawIcon(tmpx-15,yStart-20,m_hMoreThunder);
				else
					pDC->DrawIcon(tmpx-15,yStart-20,m_hOneThunder);
			}
			else
			{
				if(m_thunderpos[i].minetoken<0 || m_thunderpos[i].minetoken>5)
					m_thunderpos[i].minetoken=0;
				pDC->DrawIcon(tmpx-15,yStart-20,m_hMyThunder[m_thunderpos[i].minetoken]);
			}
		}
	}
	g_d.SelectPen(pDC,ZB2_PEN);
	for(i=0;i<m_nCurMinute-1;i++)
	{
		if(m_MinuteData[i].Minute<nSectTime[0][0])
			continue;
		if(!m_bZs || m_bZsLx)
		{
			//画均价线
			if(m_MinuteData[i].Average>COMPPREC && m_MinuteData[i+1].Average>COMPPREC)
			{
				g_d.DrawLine(pDC,ntmp1[i],GetYFromValue(m_MinuteData[i].Average),
							 ntmp2[i],GetYFromValue(m_MinuteData[i+1].Average));
			}
		}
		if(OnLine)
		{
			//画买卖力道的第二根线
			if(m_nStyle == ZST_MMLD && m_lMaxADL > 5 && !m_bDSFlag)//nBotMaxValue如果不大于1,不必要画
			{
				g_d.DrawLine(pDC,ntmp1[i],GetYMmldFromValue(m_MinuteData[i].Sellv),
								 ntmp2[i],GetYMmldFromValue(m_MinuteData[i+1].Sellv));
			}
		}
	}
	if(m_bMinuteTick_Special && m_nCurMinute>0 && m_MinuteData[m_nCurMinute-1].VolInStock_Lead.Lead>COMPPREC)
	{
		g_d.SelectPen(pDC,ZB3_PEN);
		for(i=0;i<m_nCurMinute-1;i++)
		{
			if(m_MinuteData[i].Minute<nSectTime[0][0])
				continue;
			//画净值线
			if(m_MinuteData[i].VolInStock_Lead.Lead>COMPPREC && m_MinuteData[i].VolInStock_Lead.Lead>COMPPREC)
			{
				g_d.DrawLine(pDC,ntmp1[i],GetYFromValue(m_MinuteData[i].VolInStock_Lead.Lead),
							 ntmp2[i],GetYFromValue(m_MinuteData[i+1].VolInStock_Lead.Lead));
			}
		}
	}
	//开始画叠加股票的价图形
	float ftmpclose = 0.0;
	for(int kk=0;kk < m_nOverlapNum;kk++)
	{
		if(m_OverlapMinuteData[kk][0].Now < COMPPREC) 
			continue;
		if(m_pOverlapStkInfo[kk]==m_pStkInfo)	//如果是同一只股票 忽略
			continue;
		if(!IsSameOpenCloseTimer(m_pOverlapStkInfo[kk],m_pStkInfo)) continue;
		if(m_lDate==0)
			ftmpclose = g_pStockIO->GetStockClose(m_pOverlapStkInfo[kk]);
		else ftmpclose = m_OverlapHq[kk].Close;
		if(ftmpclose < COMPPREC) 
			continue;
		g_d.SelectPen(pDC,MAX_PEN_NUM+9+kk);
		for(i=0;i<m_nCurMinute-1;i++)
		{
			if(m_OverlapMinuteData[kk][i].Minute<nSectTime[0][0])
				continue;
			//画价线
			if(m_OverlapMinuteData[kk][i].Now>COMPPREC && m_OverlapMinuteData[kk][i+1].Now>COMPPREC)
			{
				g_d.DrawLine(pDC,ntmp1[i],GetYFromValue(m_OverlapMinuteData[kk][i].Now*m_Hq.Close/ftmpclose),
						 ntmp2[i],GetYFromValue(m_OverlapMinuteData[kk][i+1].Now*m_Hq.Close/ftmpclose));
			}
		}
	}

	if(!g_GH.bDiffVolColor)	//如果没有颜色区分的成交量显示
		g_d.SelectPen(pDC,VOL_PEN);
	for(i=0;i<m_nCurMinute-1;i++)
	{
		if(m_MinuteData[i].Minute<nSectTime[0][0])
			continue;
		if(g_GH.bDiffVolColor)
		{
			if(g_bDiffVolMode==0)
			{
				//画成交量
				if( (i==0 && m_MinuteData[i].Now > m_Hq.Close) || (i>0 && m_MinuteData[i].Now > m_MinuteData[i-1].Now) )
					g_d.SelectPen(pDC,ZST_UP_PEN);
				else if( (i==0 && m_MinuteData[i].Now < m_Hq.Close) || (i>0 && m_MinuteData[i].Now < m_MinuteData[i-1].Now))
					g_d.SelectPen(pDC,ZST_DOWN_PEN);
				else
					g_d.SelectPen(pDC,ZST_LEVEL_PEN);
			}
			else
			{
				//画成交量
				if( (i==0 && m_MinuteData[i].Now > m_Hq.Close) || (i>0 && m_MinuteData[i].Now > m_MinuteData[i-1].Now) )
					g_d.SelectPen(pDC,VOL_PEN);
				else if( (i==0 && m_MinuteData[i].Now < m_Hq.Close) || (i>0 && m_MinuteData[i].Now < m_MinuteData[i-1].Now))
					g_d.SelectPen(pDC,KN_PEN);
				else
					g_d.SelectPen(pDC,LEVEL_PEN);
			}
		}
		g_d.DrawLine(pDC,ntmp1[i],GetYVolFromValue(m_MinuteData[i].NowVol),ntmp1[i],yEndVol);
		if(i == m_nCurMinute-2)
			g_d.DrawLine(pDC,ntmp2[i],GetYVolFromValue(m_MinuteData[i+1].NowVol),ntmp2[i],yEndVol);
	}
	g_d.RestorePen(pDC,oldpen);

	if(m_bDSFlag)
	{
		if(m_pStkInfo->bigmarket==QH_TYPE)
		{
			g_d.SelectPen(pDC,LEVEL_PEN);
			for(int i=0;i<m_nCurMinute-1;i++)
			{
				if(m_MinuteData[i].Minute<nSectTime[0][0])	
					continue;
				ntmp1[i] = GetXFromMinute(m_MinuteData[i].Minute);
				ntmp2[i] = GetXFromMinute(m_MinuteData[i+1].Minute);
				if(ntmp1[i] > ntmp2[i])
				{
					ntmp1[i] = ntmp2[i];
					ntmp1[i] = max(xStart,ntmp1[i]);
					ntmp1[i] = min(xEnd,ntmp1[i]);
					ntmp2[i] = ntmp1[i];
				}
				g_d.DrawLine(pDC,ntmp1[i],GetYVolFromValue(m_MinuteData[i].VolInStock_Lead.VolInStock,FALSE),
								 ntmp2[i],GetYVolFromValue(m_MinuteData[i+1].VolInStock_Lead.VolInStock,FALSE));
			}
			g_d.RestorePen(pDC,oldpen);
		}
	}

	long ltmp;
	BOOL bDrawZs = FALSE;
	for(i=0;i < max(0,m_nCurMinute-2);i++)
	{
		if(m_LBdata[i] > COMPPREC || m_LBdata[i] < -COMPPREC)
		{
			bDrawZs = TRUE;
			break;
		}
	}
	for(i=0;i<m_nCurMinute-1;i++)
	{
		if(m_MinuteData[i].Minute<nSectTime[0][0])
			continue;
		//画领先线,如果是多日走势力图,则不画领先
		if(m_bZsLx && !m_bManyDayZST && bDrawZs && g_GH.bZsShowLX)
		{
			g_d.DrawLine(pDC,ntmp1[i],GetYLxFromValue(m_LBdata[i]),
				ntmp1[i],yBasePrice,m_LBdata[i]>0?VipColor.KPCOLOR:VipColor.DOWNCOLOR);
			if(i == m_nCurMinute-2)
			{
				g_d.DrawLine(pDC,ntmp2[i],GetYLxFromValue(m_LBdata[i+1]),
					ntmp2[i],yBasePrice,m_LBdata[i+1]>0?VipColor.KPCOLOR:VipColor.DOWNCOLOR);
			}
		}
		if(OnLine)
		{
			//画买卖力道
			if(m_nStyle == ZST_MMLD && m_lMaxADL > 5 && !m_bDSFlag)
			{
				ltmp = (long)(m_MinuteData[i].Buyv-m_MinuteData[i].Sellv);
				g_d.DrawLine(pDC,ntmp1[i],GetYDiffFromValue(ltmp),
							 ntmp1[i],(yEndBot+yStartBot)/2,ltmp>0?VipColor.KPCOLOR:VipColor.DOWNCOLOR);
				if(i == m_nCurMinute-2)
				{
					ltmp = (long)(m_MinuteData[i+1].Buyv-m_MinuteData[i+1].Sellv);
					g_d.DrawLine(pDC,ntmp2[i],GetYDiffFromValue(ltmp),
								 ntmp2[i],(yEndBot+yStartBot)/2,ltmp>0?VipColor.KPCOLOR:VipColor.DOWNCOLOR);
				}
			}
		}
	}
	for(i=0;i<m_nCurMinute-1;i++)
	{
		if(m_MinuteData[i].Minute>=nSectTime[0][0])	
			break;
	}
	DUnit *pDUnit=NULL;
	switch(m_nStyle)
	{
	case ZST_TOTALORDER:	pDUnit = &m_dTotalOrder;break;
	case ZST_TOTALCANCEL:
	case ZST_CANCELSUM:		pDUnit = &m_dCancelOrder;break;
	case ZST_TICKNUM:
	case ZST_LARGETICK:		pDUnit = &m_dCjbsStat;	break;
	case ZST_IOVOL:
	case ZST_LARGEVOL:		pDUnit = &m_dCjtjStat;	break;
	}
	if(pDUnit)
	{
		if(m_bManyDayZST) 
			pDUnit->SetWillNum(240+m_nZSTManyDay*240,m_nZSTManyDay*240,0);
		else
			pDUnit->SetWillNum(240,0,0);
		if(!m_bDSFlag) 
			pDUnit->Draw(pDC,m_nStyle);
	}

	if(m_bManyDayZST) //画多日走势线
	{
		int i,kk,nCurDay=m_nZSTManyDay,nCurMinute=m_nCurMinute,nCurTotalNum=m_nRealTotalMin;
		if(m_bPlayStatus&&m_bManyDayZST)
		{
			nCurDay = (m_nCurMinute - 1 - nStartMinute)/m_nRealTotalMin;
			nCurMinute = (m_nCurMinute - 1 - nStartMinute)%m_nRealTotalMin + 1;
		}
		for(kk=0;kk <= nCurDay;kk++)
		{
			oldpen = g_d.SelectPen(pDC,ZB1_PEN);
			if(kk==nCurDay) nCurTotalNum = nCurMinute>0?nCurMinute:m_nRealTotalMin;
			for(i=0;i<nCurTotalNum-1;i++)
			{
				if(m_ManyDayMinuteData[kk][i].Minute<nSectTime[0][0])
					continue;
				ntmp1[i] = GetXFromMinute(m_ManyDayMinuteData[kk][i].Minute,kk);
				ntmp2[i] = GetXFromMinute(m_ManyDayMinuteData[kk][i+1].Minute,kk);
				if(ntmp1[i] > ntmp2[i])  //及时修正，防止反画，不能是>=
				{
					ntmp1[i] = ntmp2[i];
					ntmp1[i] = max(xStart,ntmp1[i]);
					ntmp1[i] = min(xEnd,ntmp1[i]);
					ntmp2[i] = ntmp1[i];
				}
				//画价线
				if(m_ManyDayMinuteData[kk][i].Now>COMPPREC && m_ManyDayMinuteData[kk][i+1].Now>COMPPREC)
					g_d.DrawLine(pDC,ntmp1[i],GetYFromValue(m_ManyDayMinuteData[kk][i].Now),
							 ntmp2[i],GetYFromValue(m_ManyDayMinuteData[kk][i+1].Now)	);
			}
			g_d.SelectPen(pDC,ZB2_PEN);
			for(i=0;i<nCurTotalNum-1;i++)
			{
				if(m_ManyDayMinuteData[kk][i].Minute<nSectTime[0][0])
					continue;
				if(!m_bZs || m_bZsLx)
				{
					//画均价线
					if(m_ManyDayMinuteData[kk][i].Average>COMPPREC && m_ManyDayMinuteData[kk][i+1].Average>COMPPREC)
						g_d.DrawLine(pDC,ntmp1[i],GetYFromValue(m_ManyDayMinuteData[kk][i].Average),
									 ntmp2[i],GetYFromValue(m_ManyDayMinuteData[kk][i+1].Average));
				}
			}
			if(!g_GH.bDiffVolColor)	//如果没有颜色区分的成交量显示
				g_d.SelectPen(pDC,VOL_PEN);
			for(i=0;i<nCurTotalNum-1;i++)
			{
				if(m_ManyDayMinuteData[kk][i].Minute<nSectTime[0][0])
					continue;
				if(g_GH.bDiffVolColor)
				{
					if(g_bDiffVolMode==0)
					{
						//画成交量
						if( (i==0 && m_ManyDayMinuteData[kk][i].Now > m_fManyDayClose[kk]) || (i>0 && m_ManyDayMinuteData[kk][i].Now > m_ManyDayMinuteData[kk][i-1].Now) )
							g_d.SelectPen(pDC,ZST_UP_PEN);
						else if( (i==0 && m_ManyDayMinuteData[kk][i].Now < m_fManyDayClose[kk]) || (i>0 && m_ManyDayMinuteData[kk][i].Now < m_ManyDayMinuteData[kk][i-1].Now))
							g_d.SelectPen(pDC,ZST_DOWN_PEN);
						else
							g_d.SelectPen(pDC,ZST_LEVEL_PEN);
					}
					else
					{
						//画成交量
						if( (i==0 && m_ManyDayMinuteData[kk][i].Now > m_fManyDayClose[kk]) || (i>0 && m_ManyDayMinuteData[kk][i].Now > m_ManyDayMinuteData[kk][i-1].Now) )
							g_d.SelectPen(pDC,VOL_PEN);
						else if( (i==0 && m_ManyDayMinuteData[kk][i].Now < m_fManyDayClose[kk]) || (i>0 && m_ManyDayMinuteData[kk][i].Now < m_ManyDayMinuteData[kk][i-1].Now))
							g_d.SelectPen(pDC,KN_PEN);
						else
							g_d.SelectPen(pDC,LEVEL_PEN);
					}
				}
				g_d.DrawLine(pDC,ntmp1[i],GetYVolFromValue(m_ManyDayMinuteData[kk][i].NowVol),ntmp1[i],yEndVol);
				if(i == nCurTotalNum-2)
					g_d.DrawLine(pDC,ntmp2[i],GetYVolFromValue(m_ManyDayMinuteData[kk][i+1].NowVol),ntmp2[i],yEndVol);
			}
			if(m_bMinuteTick_Special && nCurTotalNum>0 && m_ManyDayMinuteData[kk][nCurTotalNum-1].VolInStock_Lead.Lead>COMPPREC)
			{
				g_d.SelectPen(pDC,ZB3_PEN);
				for(i=0;i<nCurTotalNum-1;i++)
				{
					if(m_ManyDayMinuteData[kk][i].Minute<nSectTime[0][0])
						continue;
					//画净值线
					g_d.DrawLine(pDC,ntmp1[i],GetYFromValue(m_ManyDayMinuteData[kk][i].VolInStock_Lead.Lead),
									 ntmp2[i],GetYFromValue(m_ManyDayMinuteData[kk][i+1].VolInStock_Lead.Lead));
				}
			}
			if(m_bDSFlag)
			{
				if(m_pStkInfo->bigmarket==QH_TYPE)
				{
					g_d.SelectPen(pDC,LEVEL_PEN);
					for(int i=0;i<nCurTotalNum-1;i++)
					{
						if(m_ManyDayMinuteData[kk][i].Minute<nSectTime[0][0])	continue;
						ntmp1[i] = GetXFromMinute(m_ManyDayMinuteData[kk][i].Minute,kk);
						ntmp2[i] = GetXFromMinute(m_ManyDayMinuteData[kk][i+1].Minute,kk);
						if(ntmp1[i] > ntmp2[i])
						{
							ntmp1[i] = ntmp2[i];
							ntmp1[i] = max(xStart,ntmp1[i]);
							ntmp1[i] = min(xEnd,ntmp1[i]);
							ntmp2[i] = ntmp1[i];
						}
						g_d.DrawLine(pDC,ntmp1[i],GetYVolFromValue(m_ManyDayMinuteData[kk][i].VolInStock_Lead.VolInStock,FALSE),
							ntmp2[i],GetYVolFromValue(m_ManyDayMinuteData[kk][i+1].VolInStock_Lead.VolInStock,FALSE));
					}
				}
			}
			g_d.RestorePen(pDC,oldpen);
		}
	}
	if(m_nStyle<ZST_MULTI && m_bHasSwitch)
	{
		switch(GetStyle())
		{
		case ZST_BIG:			m_pSwitch->SetCurLabel(ZST_BIG);	break;
		case ZST_LB:			m_pSwitch->SetCurLabel(ZST_LB);		break;
		case ZST_MMLD:			m_pSwitch->SetCurLabel(ZST_MMLD);	break;
		case ZST_AUCTION:		if(m_nSwitchBtn<=ZST_AUCTION)		return;	m_pSwitch->SetCurLabel(ZST_AUCTION);	break;
		case ZST_TOTALORDER:	if(m_nSwitchBtn<=ZST_TOTALORDER)	return;	m_pSwitch->SetCurLabel(ZST_TOTALORDER);	break;
		case ZST_TOTALCANCEL:	if(m_nSwitchBtn<=ZST_TOTALCANCEL)	return;	m_pSwitch->SetCurLabel(ZST_TOTALCANCEL);break;
		case ZST_CANCELSUM:		if(m_nSwitchBtn<=ZST_CANCELSUM)		return;	m_pSwitch->SetCurLabel(ZST_CANCELSUM);	break;
		case ZST_IOVOL:			if(m_nSwitchBtn<=ZST_IOVOL)			return;	m_pSwitch->SetCurLabel(ZST_IOVOL);		break;
		case ZST_LARGEVOL:		if(m_nSwitchBtn<=ZST_LARGEVOL)		return;	m_pSwitch->SetCurLabel(ZST_LARGEVOL);	break;
		case ZST_TICKNUM:		if(m_nSwitchBtn<=ZST_TICKNUM)		return;	m_pSwitch->SetCurLabel(ZST_TICKNUM);	break;
		case ZST_LARGETICK:		if(m_nSwitchBtn<=ZST_LARGETICK)		return;	m_pSwitch->SetCurLabel(ZST_LARGETICK);	break;
		}
	}
}

//nFlag 0左,1右,2最左,3最右
void UZst::ToNextMin(int nFlag,int nSpace)
{
	if(!m_bCrossMode)
	{
		m_bCrossMode = TRUE;
		m_bHasCross = TRUE;
		ProcessMsg(WM_PAINT);
	}
	CPoint pt;
	int EndPos;

	int nMinutes = 0;
	if(!m_bDSFlag&&m_bTotalSect)
	{
		if(m_bManyDayZST)
			EndPos=xStart+m_nZSTManyDay*m_nRealTotalMin*fPointPerMinute + fPointPerMinute*m_nCurMinute;
		else
			EndPos=xStart+fPointPerMinute*m_nCurMinute;
	}
	else 
		EndPos=xStart+fPointPerMinute*(m_nCurMinute+1-System2Tdx(nSectTime[0][0],TimeSec));

	//修改偏移坐标计算方法,使之与成交量线对齐
	long nLastMin = 0;
	if(m_bManyDayZST) 
		nLastMin = m_nZSTManyDay*m_nRealTotalMin+m_nCurMinute;
	else 
		nLastMin = m_nCurMinute;

	if(!m_bKeyCross)
	{
		m_bKeyCross = TRUE;
		m_nKeyMin = long((double)(m_CrossPoint.x-xStart)/fPointPerMinute)+1;
	}
	if(m_nKeyMin<=0) m_nKeyMin = 1;
	if(m_nKeyMin > nLastMin) m_nKeyMin = nLastMin;

	if(nFlag==2)		//最左
	{
		m_nKeyMin = 1;
		pt.x = xStart+max(1,(xEnd-xStart)/nSecLens);
	}
	else if(nFlag==3)	//最右
	{
		m_nKeyMin = nLastMin;
		pt.x = EndPos;
	}
	else if(nFlag==0)	//左
	{
		if(m_CrossPoint==CPoint(0,0))
		{
			m_nKeyMin = nLastMin;
			pt.x = int(m_nKeyMin*fPointPerMinute)+xStart;
		}
		else
		{
			m_nKeyMin -= nSpace;
			if(m_nKeyMin<=0) m_nKeyMin = nLastMin;
			pt.x = int(m_nKeyMin*fPointPerMinute)+xStart;
		}
	}
	else if(nFlag==1)	//右
	{
		if(m_CrossPoint==CPoint(0,0))
		{
			m_nKeyMin = 0;
			pt.x = int(m_nKeyMin*fPointPerMinute)+xStart;
		}
		else
		{
			m_nKeyMin += nSpace;
			if(m_nKeyMin>nLastMin) m_nKeyMin = 1;
			pt.x = int(m_nKeyMin*fPointPerMinute)+xStart;
		}
	}
	pt.y = GetNowYFromX(pt.x);
	InSwimArea(NULL,pt);
}

void UZst::InSwimArea(CDC *pInDC,CPoint pt,BOOL bInview)
{
	//防止绘图越界
	CPoint topleft(xStart, yStart), rightbuttom(xEnd,yEndBot);
	if(!m_rcIn.PtInRect(topleft)||!m_rcIn.PtInRect(rightbuttom)) return;	

	if(m_bCrossMode)
	{
		CDC *pDC=pInDC;
		if(!pInDC)
			pDC = m_pView->GetDC();
		int oldrop = pDC->SetROP2(R2_XORPEN);
		int nLeft = left, nTop = top;
		if(!bInview)
			nLeft = nTop = 0;
		if(m_CrossPoint!=CPoint(0,0))
		{
			//为数值说明作准备	CCC 20080325
			g_d.DrawLine(pDC,nLeft+m_CrossPoint.x,nTop+yStart,nLeft+m_CrossPoint.x,nTop+yEndVol,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
			if(yEndBot > yEndVol)
				g_d.DrawLine(pDC,nLeft+m_CrossPoint.x,nTop+yStartBot,nLeft+m_CrossPoint.x,nTop+yEndBot,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
			if(m_CrossPoint.y<yEndVol || m_CrossPoint.y>yStartBot)
				g_d.DrawLine(pDC,nLeft+xStart, nTop+m_CrossPoint.y, nLeft+xEnd, nTop+m_CrossPoint.y,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
		}
		//为数值说明作准备	CCC 20080325
		g_d.DrawLine(pDC,nLeft+pt.x,nTop+yStart,nLeft+pt.x,nTop+yEndVol,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
		if(yEndBot > yEndVol)
			g_d.DrawLine(pDC,nLeft+pt.x,nTop+yStartBot,nLeft+pt.x,nTop+yEndBot,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
		if(pt.y<yEndVol || pt.y>yStartBot)
			g_d.DrawLine(pDC,nLeft+xStart, nTop+pt.y, nLeft+xEnd, nTop+pt.y,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
		m_CrossPoint = pt;
		pDC->SetROP2(oldrop);
		if(!pInDC)
			m_pView->ReleaseDC(pDC);
		DrawInfoArea(pInDC,pt.x,bInview);
		DrawAddInfo(pInDC,pt.x,FALSE);
		m_bHasCross = TRUE;
	}
	else DrawAddInfo(pInDC,0,TRUE);
	LonelyDrawAxis(pInDC,0,TRUE,pt.x);
	LonelyDrawAxis(pInDC,2,TRUE,pt.y);
	m_bInSwim = TRUE;
}

void UZst::LeaveSwimArea(CDC *pInDC)
{
	if(m_bCrossMode && m_bHasCross)
	{
		LonelyDrawAxis(pInDC,1);
		if(m_CrossPoint != CPoint(0,0))
		{
			CDC *pDC=pInDC;
			if(!pInDC)
				pDC = m_pView->GetDC();
			int oldrop = pDC->SetROP2(R2_XORPEN);
			//为数值说明作准备	CCC 20080325
			g_d.DrawLine(pDC,left+m_CrossPoint.x,top+yStart,left+m_CrossPoint.x,top+yEndVol,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
			if(yEndBot > yEndVol)
				g_d.DrawLine(pDC,left+m_CrossPoint.x,top+yStartBot,left+m_CrossPoint.x,top+yEndBot,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
			if(m_CrossPoint.y<yEndVol || m_CrossPoint.y>yStartBot)
				g_d.DrawLine(pDC,left+xStart, top+m_CrossPoint.y, left+xEnd, top+m_CrossPoint.y,VipColor.TXTCOLOR^VipColor.BACKCOLOR);
			
			pDC->SetROP2(oldrop);
			if(!pInDC)
				m_pView->ReleaseDC(pDC);
		}
		m_CrossPoint=CPoint(0,0);
		m_bHasCross = FALSE;

		LonelyDrawAxis(pInDC,0);
		LonelyDrawAxis(pInDC,2);
		m_bInSwim = FALSE;
	}
	else if(!m_bCrossMode && m_bInSwim)
	{
		LonelyDrawAxis(pInDC,0);
		LonelyDrawAxis(pInDC,2);
		m_bInSwim = FALSE;
	}
	DrawAddInfo(pInDC,0,TRUE);
}

void UZst::DrawAddInfo(CDC *pInDC,int xPos,BOOL bShowLast)
{
	if(yStartBot!=yEndVol+ZST_THEIGHT || yStartBot>yEndBot || xEnd-xStart<10) return;
	CDC *pDC=pInDC;
	int lLeft	= 0;
	int lTop	= 0;
	if(!pInDC)
	{
		lLeft	= left;
		lTop	= top;
		pDC		= m_pView->GetDC();
	}
	CDC m_MyDC;
	CBitmap m_MyBitmap,*m_pOldBitmap;
	m_MyDC.CreateCompatibleDC(pDC);
	m_MyBitmap.CreateCompatibleBitmap(pDC,xEnd-xStart,ZST_THEIGHT);
	m_pOldBitmap = m_MyDC.SelectObject(&m_MyBitmap);
	g_d.Bar(&m_MyDC,0,0,xEnd-xStart,ZST_THEIGHT,BACK_BRUSH);
	m_MyDC.SetBkMode(TRANSPARENT);
	CFont	*oldfont	= g_d.SelectFont(&m_MyDC,GRAPH_FONT);
	//
	BOOL	bHisInfo	= FALSE;
	CSize	size;
	int		nWidth=1;
	int		i=0;
	double	dBVol;
	double	dSVol;
	long	lBCjbs;
	long	lSCjbs;
	if(m_bManyDayZST && 1.0*xPos < 1.0*xStart+1.0*m_nZSTManyDay*m_nRealTotalMin*(xEnd-xStart)/nSecLens)
		bHisInfo = TRUE;
	if(!bHisInfo && !m_bDSFlag)
	{
		int	NowMinutes	= GetTimeFromX(xPos);
		int	nTimeNum	= System2Tdx(NowMinutes,TimeSec)-1;
		if(bShowLast)
		{
			switch(m_nStyle)
			{
			case ZST_LB:
			case ZST_MMLD:
				if(m_nTodayMin>0) nTimeNum = m_nTodayMin-1;
				else nTimeNum = 0;
				break;
			case ZST_TOTALORDER:
			case ZST_TOTALCANCEL:
			case ZST_CANCELSUM:
				if(m_nNormalStat>0) nTimeNum = m_nNormalStat-1;
				else nTimeNum = 0;
				break;
			case ZST_TICKNUM:
			case ZST_LARGETICK:
			case ZST_IOVOL:
			case ZST_LARGEVOL:
				if(m_nTickStat>0) nTimeNum = m_nTickStat-1;
				else nTimeNum = 0;
				break;
			}
		}
		switch(m_nStyle)
		{
		case ZST_LB:
			if(m_bZs) size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.TXTCOLOR,0,_F("多空:%.2f"),m_LBdata[nTimeNum]);
			else  size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.TXTCOLOR,0,_F("量比:%.2f"),m_LBdata[nTimeNum]);
			nWidth+=size.cx+12;
			break;
		case ZST_MMLD:
			if(m_bZs)
			{
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[0],0,"涨家:%d",m_MinuteData[nTimeNum].Buyv);
				nWidth+=size.cx+12;
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[1],0,"跌家:%d",m_MinuteData[nTimeNum].Sellv);
				nWidth+=size.cx+12;
			}
			else
			{
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[0],0,"委买:%d",m_MinuteData[nTimeNum].Buyv);
				nWidth+=size.cx+12;
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[1],0,"委卖:%d",m_MinuteData[nTimeNum].Sellv);
				nWidth+=size.cx+12;
			}
			break;
		case ZST_TOTALORDER:
			if(!m_bZs && m_bLevel2Property && g_bSupportDDE)
			{
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[0],0,"委买总量:%d",m_NormalStat[nTimeNum].TotalBuyv);
				nWidth+=size.cx+12;
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[1],0,"委卖总量:%d",m_NormalStat[nTimeNum].TotalSellv);
				nWidth+=size.cx+12;
			}
			break;
		case ZST_TOTALCANCEL:
			if(!m_bZs && m_bLevel2Property && g_bSupportDDE)
			{
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[0],0,"买方委托增量:%.0f",m_NormalStat[nTimeNum].fBOrder);
				nWidth+=size.cx+12;
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.KPCOLOR,0,"买方委托撤单:%.0f",m_NormalStat[nTimeNum].fBCancel);
				nWidth+=size.cx+12;
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[1],0,"卖方委托增量:%.0f",m_NormalStat[nTimeNum].fSOrder);
				nWidth+=size.cx+12;
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.KNCOLOR,0,"卖方委托撤单:%.0f",m_NormalStat[nTimeNum].fSCancel);
				nWidth+=size.cx+12;
			}
			break;
		case ZST_CANCELSUM:
			if(!m_bZs && m_bLevel2Property && g_bSupportDDE)
			{
				for(i=0,dBVol=0.0,dSVol=0.0;i<=nTimeNum;i++)
				{
					dBVol	+= m_NormalStat[i].fBCancel;
					dSVol	+= m_NormalStat[i].fSCancel;
				}
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[0],0,"买方累计撤单:%.0f",dBVol);
				nWidth+=size.cx+12;
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[1],0,"卖方累计撤单:%.0f",dSVol);
				nWidth+=size.cx+12;
			}
			break;
		case ZST_TICKNUM:
			if(!m_bZs && m_bLevel2Property && g_bSupportDDE)
			{
				for(i=0,lBCjbs=0,lSCjbs=0;i<=nTimeNum;i++)
				{
					lBCjbs	= max(lBCjbs,m_TickStat[i].VolNum[0][0]+m_TickStat[i].VolNum[0][2]);
					lSCjbs	= max(lSCjbs,m_TickStat[i].VolNum[0][1]+m_TickStat[i].VolNum[0][3]);
				}
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[0],0,"买入总笔数:%d",lBCjbs);
				nWidth+=size.cx+12;
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[1],0,"卖出总笔数:%d",lSCjbs);
				nWidth+=size.cx+12;
			}
			break;
		case ZST_LARGETICK:
			if(!m_bZs && m_bLevel2Property && g_bSupportDDE)
			{
				for(i=0,lBCjbs=0,lSCjbs=0;i<=nTimeNum;i++)
				{
					lBCjbs	= max(lBCjbs,m_TickStat[i].VolNum[1][0]+m_TickStat[i].VolNum[1][2]);
					lSCjbs	= max(lSCjbs,m_TickStat[i].VolNum[1][1]+m_TickStat[i].VolNum[1][3]);
				}
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[0],0,"买入大单总笔数:%d",lBCjbs);
				nWidth+=size.cx+12;
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[1],0,"卖出大单总笔数:%d",lSCjbs);
				nWidth+=size.cx+12;
			}
			break;
		case ZST_IOVOL:
			if(!m_bZs && m_bLevel2Property && g_bSupportDDE)
			{
				for(i=0,dBVol=0.0,dSVol=0.0;i<=nTimeNum;i++)
				{
					dBVol	= max(dBVol,m_TickStat[i].fVol[0][0]);
					dSVol	= max(dSVol,m_TickStat[i].fVol[0][1]);
				}
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[0],0,"主买累计成交量:%.0f",dBVol);
				nWidth+=size.cx+12;
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[1],0,"主卖累计成交量:%.0f",dSVol);
				nWidth+=size.cx+12;
			}
			break;
		case ZST_LARGEVOL:
			if(!m_bZs && m_bLevel2Property && g_bSupportDDE)
			{
				for(i=0,dBVol=0.0,dSVol=0.0;i<=nTimeNum;i++)
				{
					dBVol	= max(dBVol,m_TickStat[i].fVol[1][0]+m_TickStat[i].fVol[1][2]);
					dSVol	= max(dSVol,m_TickStat[i].fVol[1][1]+m_TickStat[i].fVol[1][3]);
				}
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[0],0,"买入累计大单量:%.0f",dBVol);
				nWidth+=size.cx+12;
				size = g_d.DisplayText(&m_MyDC,nWidth,1,VipColor.ZBCOLOR[1],0,"卖出累计大单量:%.0f",dSVol);
				nWidth+=size.cx+12;
			}
			break;
		}
	}
	g_d.RestoreFont(&m_MyDC,oldfont);
	pDC->BitBlt(lLeft+xStart+1,lTop+yEndVol+1,xEnd-xStart-2,ZST_THEIGHT-2,&m_MyDC,0,0,SRCCOPY);
	m_MyDC.SelectObject(m_pOldBitmap);
	m_MyBitmap.DeleteObject();
	m_MyDC.DeleteDC();
	if(!pInDC) m_pView->ReleaseDC(pDC);
}

//画信息区
void UZst::DrawInfoArea(CDC *pInDC,int xPos,BOOL bInView)
{
	if(fBasePrice < COMPPREC || (!m_bDataReady && OnLine)) //如果fBasePrice为0,则不画,如果在线状态，数据没有到，也不画
		return;
	CDC *pDC=pInDC;
	if(!pInDC)
		pDC = m_pView->GetDC();
	CDC m_MyDC;
	CBitmap m_MyBitmap,*m_pOldBitmap;
	m_MyDC.CreateCompatibleDC(pDC);
	m_MyBitmap.CreateCompatibleBitmap(pDC,ZST_LWIDTH,Height()-ZST_BHEIGHT);
	m_pOldBitmap = m_MyDC.SelectObject(&m_MyBitmap);
	g_d.Bar(&m_MyDC,0,0,ZST_LWIDTH,Height()-ZST_BHEIGHT,Scheme_Hold.VipColor.COMVIEWINFOBC);
	m_MyDC.SetBkMode(TRANSPARENT);
	CFont *oldfont = g_d.SelectFont(&m_MyDC,INFO_FONT);
	CPen  *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
	int nHeight=2;
	CSize size;
	int NowMinutes = GetTimeFromX(xPos);
	int nTimeNum=System2Tdx(NowMinutes,TimeSec)-1;
	int tmpXsFlag=m_XsFlag;
	float maxprice=max(m_MinuteData[nTimeNum].Now,m_MinuteData[nTimeNum].Average);
	if(maxprice>=100000.0)		tmpXsFlag=0;
	else if(maxprice>=10000.0)	tmpXsFlag=1;

	BOOL bHisInfo = FALSE;
	int  whichHis = 0;
	struct MinuteData tmpMinute = m_MinuteData[nTimeNum];
	float tmpClose = m_Hq.Close;
	long  tmpDate = (m_lDate==0?g_lOpenRq:m_lDate);
	if(m_bManyDayZST && 1.0*xPos < 1.0*xStart+1.0*m_nZSTManyDay*m_nRealTotalMin*(xEnd-xStart)/nSecLens)
	{
		bHisInfo = TRUE;
		whichHis = min(m_nZSTManyDay-1,1.0*(xPos-xStart-1)*nSecLens/(1.0*m_nRealTotalMin*(xEnd-xStart))); //精度问题总有一点?
		tmpMinute = m_ManyDayMinuteData[whichHis][nTimeNum];
		tmpClose = m_fManyDayClose[whichHis];
		tmpDate =  m_ManyDayDate[whichHis];
	}
	if(m_bManyDayZST)
	{
		size=g_d.DisplayText(&m_MyDC,2,nHeight,VipColor.TXTCOLOR,0,_F("日期"));nHeight+=size.cy+2;
		size=g_d.DisplayText(&m_MyDC,0,nHeight,VipColor.LEVELCOLOR,ZST_LWIDTH-2,GetTdxDateStr(tmpDate));nHeight+=size.cy+2;
	}
	size=g_d.DisplayText(&m_MyDC,2,nHeight,VipColor.TXTCOLOR,0,_F("时间"));nHeight+=size.cy+2;
	size=g_d.DisplayText(&m_MyDC,0,nHeight,VipColor.LEVELCOLOR,ZST_LWIDTH-2,GetTdxTimeStr(NowMinutes));nHeight+=size.cy+2;

	size=g_d.DisplayText(&m_MyDC,2,nHeight,VipColor.TXTCOLOR,0,_F("价位"));nHeight+=size.cy+2;
	size=g_d.BjPrintf(&m_MyDC,0,nHeight,tmpMinute.Now,tmpClose,tmpXsFlag,ZST_LWIDTH-2);nHeight+=size.cy+2;

	size=g_d.DisplayText(&m_MyDC,2,nHeight,VipColor.TXTCOLOR,0,_F("均价"));nHeight+=size.cy+2;
	size=g_d.BjPrintf(&m_MyDC,0,nHeight,tmpMinute.Average,tmpClose,tmpXsFlag,ZST_LWIDTH-2);nHeight+=size.cy+2;
	//
	size=g_d.DisplayText(&m_MyDC,2,nHeight,VipColor.TXTCOLOR,0,_F("涨跌"));nHeight+=size.cy+2;
	if(tmpMinute.Now>COMPPREC)
		size=g_d.BjPrintf(&m_MyDC,0,nHeight,tmpMinute.Now-tmpClose,0.0,m_XsFlag,ZST_LWIDTH-2);
	else
		size=g_d.BjPrintf(&m_MyDC,0,nHeight,0.0,0.0,m_XsFlag,ZST_LWIDTH-2,0);
	nHeight+=size.cy+2;
	//
	size=g_d.DisplayText(&m_MyDC,2,nHeight,VipColor.TXTCOLOR,0,_F("涨幅"));nHeight+=size.cy+2;
	if(tmpMinute.Now>COMPPREC && tmpClose>COMPPREC)
		size=g_d.BjPrintf(&m_MyDC,0,nHeight,(tmpMinute.Now-tmpClose)*100/tmpClose,0.0,2,ZST_LWIDTH-2,1);
	nHeight+=size.cy+2;
	//
	size=g_d.DisplayText(&m_MyDC,2,nHeight,VipColor.TXTCOLOR,0,(m_bZs||m_bHKZs)?_F("成交额"):_F("成交量"));nHeight+=size.cy+2;
	size=g_d.DisplayText(&m_MyDC,0,nHeight,VipColor.VOLCOLOR,ZST_LWIDTH-2,MakeVol3(tmpMinute.NowVol*(m_bHKZs?0.01f:1)));nHeight+=size.cy+2;
	if(m_pStkInfo->bigmarket==QH_TYPE)
	{
		size=g_d.DisplayText(&m_MyDC,2,nHeight,VipColor.LEVELCOLOR,0,_F("持仓量"));nHeight+=size.cy+2;
		size=g_d.DisplayText(&m_MyDC,0,nHeight,VipColor.VOLCOLOR,ZST_LWIDTH-2,"%s",MakeVol3(tmpMinute.VolInStock_Lead.VolInStock));nHeight+=size.cy+2;
	}
	else if(!m_bZs && !m_bHKZs && need_justcjl(m_pStkInfo->setcode,m_pStkInfo->Code))
	{
		size=g_d.DisplayText(&m_MyDC,2,nHeight,VipColor.TXTCOLOR,0,_F("成交额"));nHeight+=size.cy+2;
		size=g_d.DisplayText(&m_MyDC,0,nHeight,VipColor.VOLCOLOR,ZST_LWIDTH,MakeVol3(tmpMinute.NowVol*tmpMinute.Now*m_pStkInfo->Unit));nHeight+=size.cy+2;
	}
	if(m_bMinuteTick_Special)
	{
		if(m_bETF)
		{
			size=g_d.DisplayText(&m_MyDC,2,nHeight,VipColor.TXTCOLOR,0,_F("IOPV"));nHeight+=size.cy+2;
			if(tmpMinute.VolInStock_Lead.Lead>COMPPREC)
				size=g_d.BjPrintf(&m_MyDC,0,nHeight,tmpMinute.VolInStock_Lead.Lead,tmpClose,tmpXsFlag,ZST_LWIDTH-2);
			nHeight+=size.cy+2;
			size=g_d.DisplayText(&m_MyDC,2,nHeight,VipColor.TXTCOLOR,0,_F("溢折价"));nHeight+=size.cy+2;
			if(tmpMinute.VolInStock_Lead.Lead>COMPPREC && tmpMinute.Now>COMPPREC)
				size=g_d.BjPrintf(&m_MyDC,0,nHeight,(tmpMinute.Now-tmpMinute.VolInStock_Lead.Lead),0.0,tmpXsFlag,ZST_LWIDTH-2);
			nHeight+=size.cy+2;
			size=g_d.DisplayText(&m_MyDC,2,nHeight,VipColor.TXTCOLOR,0,_F("溢折率"));nHeight+=size.cy+2;
			if(tmpMinute.VolInStock_Lead.Lead>COMPPREC && tmpMinute.Now>COMPPREC)
				size=g_d.BjPrintf(&m_MyDC,0,nHeight,(tmpMinute.Now-tmpMinute.VolInStock_Lead.Lead)*100/tmpMinute.VolInStock_Lead.Lead,0.0,2,ZST_LWIDTH-2,1);
			nHeight+=size.cy+2;
		}
		else
		{
			size=g_d.DisplayText(&m_MyDC,2,nHeight,VipColor.TXTCOLOR,0,_F("时净值"));nHeight+=size.cy+2;
			if(tmpMinute.VolInStock_Lead.Lead>COMPPREC)
				size=g_d.BjPrintf(&m_MyDC,0,nHeight,tmpMinute.VolInStock_Lead.Lead,tmpClose,4,ZST_LWIDTH-2);
			nHeight+=size.cy+2;
		}
	}
	g_d.DrawRect(&m_MyDC,0,0,ZST_LWIDTH-1,nHeight-1);
	g_d.RestorePen(&m_MyDC,oldpen);
	g_d.RestoreFont(&m_MyDC,oldfont);
	if(bInView) 
		pDC->BitBlt (left,top,ZST_LWIDTH,nHeight,&m_MyDC,0,0,SRCCOPY);
	else 
		pDC->BitBlt (0,0,ZST_LWIDTH,nHeight,&m_MyDC,0,0,SRCCOPY);
	m_MyDC.SelectObject(m_pOldBitmap);
	m_MyBitmap.DeleteObject();
	m_MyDC.DeleteDC();
	if(!pInDC)
		m_pView->ReleaseDC(pDC);
}

//单独画轴
void UZst::LonelyDrawAxis(CDC *pInDC,int nWhich,BOOL bSwim,int DrawPos)
{
	if(fBasePrice < COMPPREC) //如果fBasePrice为0,则不画
		return;
	CRect tmpRect;
	if(nWhich==0)		tmpRect.SetRect(0,0,Width()-ZST_RWIDTH,ZST_BHEIGHT);
	else if(nWhich==1)	tmpRect.SetRect(0,0,ZST_LWIDTH,Height()-ZST_BHEIGHT);
	else				tmpRect.SetRect(0,0,ZST_RWIDTH,Height()-ZST_BHEIGHT);
	int tmpleft=0,tmptop=0;
	CDC *pDC=pInDC;
	if(!pInDC)
	{
		pDC = m_pView->GetDC();
		tmpleft = left;
		tmptop = top;
	}
	CDC m_MyDC;
	CBitmap m_MyBitmap,*m_pOldBitmap;
	m_MyDC.CreateCompatibleDC(pDC);
	m_MyBitmap.CreateCompatibleBitmap(pDC,tmpRect.Width(),tmpRect.Height());
	m_pOldBitmap = m_MyDC.SelectObject(&m_MyBitmap);
	g_d.Bar(&m_MyDC,&tmpRect,BACK_BRUSH);
	m_MyDC.SetBkMode(TRANSPARENT);

	CFont *oldfont = g_d.SelectFont(&m_MyDC,GRAPH_FONT);
	CPen  *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);

	if(nWhich==0)
		DrawAxisArea(&m_MyDC,CPoint(0,-1),nWhich,bSwim,DrawPos);
	else if(nWhich==1)
		DrawAxisArea(&m_MyDC,CPoint(0,0),nWhich,bSwim,DrawPos);
	else 
		DrawAxisArea(&m_MyDC,CPoint(-1,0),nWhich,bSwim,DrawPos);

	g_d.RestorePen(&m_MyDC,oldpen);
	g_d.RestoreFont(&m_MyDC,oldfont);

	if(nWhich==0)
		pDC->BitBlt (tmpleft,tmptop+yEndBot+1,tmpRect.Width(),ZST_BHEIGHT-1,&m_MyDC,0,0,SRCCOPY);	//从内存DC拷贝到屏幕DC
	else if(nWhich==1)
		pDC->BitBlt (tmpleft,tmptop,ZST_LWIDTH,tmpRect.Height(),&m_MyDC,0,0,SRCCOPY);				//从内存DC拷贝到屏幕DC
	else
		pDC->BitBlt (tmpleft+xEnd+1,tmptop,ZST_RWIDTH-1,tmpRect.Height(),&m_MyDC,0,0,SRCCOPY);		//从内存DC拷贝到屏幕DC
		 
	m_MyDC.SelectObject(m_pOldBitmap);
	m_MyBitmap.DeleteObject();
	m_MyDC.DeleteDC();
	if(!pInDC)
		m_pView->ReleaseDC(pDC);
}

//nWhich=0:横坐标 1:左纵坐标 2:右纵坐标 3:小图的右纵坐标, 
//RelaPoint为相对坐标点,DrawPos为Unit坐标点(nWhich=0表示x,其它为y)
void UZst::DrawAxisArea(CDC *pDC,CPoint RelPt,int nWhich,BOOL bSwim,int DrawPos)
{
	if(nWhich==2 && g_GH.bEmptyZSTRight)
		return;
	if(nWhich == 0 && m_bManyDayZST)	//横坐标
	{
		int i,j,locTime = nSectTime[0][0],nDotPeriod=(m_nStyle>=ZST_MULTI?60:30);	//画虚线实线的周期
		if(nSecLens>12*60) nDotPeriod=nSecLens/24;
		else if(nSecLens>6*60) nDotPeriod=nSecLens/12;
		if(m_nStyle >= ZST_MULTI) nDotPeriod*=3;
		int nDotGap = max(nSecLens/(6*nDotPeriod),1);
		CSize size=pDC->GetTextExtent("00:00");
		short *stmp = TimeSec;
		int nSectNum = GetTdxSectNum(stmp);
		int nBlockNum = m_nRealTotalMin/(nDotGap*nDotPeriod);
		int nDSDot = m_nRealTotalMin/(nBlockNum+0.5);
		for(i=0,j=0;i<nSecLens;i+=m_bDSFlag?nDSDot:60)
		{
			nBlockNum = m_nRealTotalMin/(nDotGap*nDotPeriod);
			nDSDot = m_nRealTotalMin/(nBlockNum+0.5f);
			if((i+(m_bDSFlag?nDSDot:60))/m_nRealTotalMin>i/m_nRealTotalMin)
			{
				int ii = ((i+(m_bDSFlag?nDSDot:60))/m_nRealTotalMin)*m_nRealTotalMin;
				nDSDot = ii-i;
				i = ii;
			}
			int nMinute = GetMinuteFromFZ(i, 1, TimeSec);
			if(m_bManyDayZST)
			{
				if(i%m_nRealTotalMin==0)
				{
					int k = i/m_nRealTotalMin;
					if(k < m_nZSTManyDay)
						g_d.DisplayText(pDC,RelPt.x+xStart+i*(xEnd-xStart)/nSecLens-size.cx,RelPt.y+1,VipColor.TIMECOLOR,GetTdxDateStr(m_ManyDayDate[k])); //显示日期
				}
				else if((i+(m_bDSFlag?nDSDot:60))/m_nRealTotalMin>i/m_nRealTotalMin)
				{
					int k = (i+(m_bDSFlag?nDSDot:60))/m_nRealTotalMin/m_nRealTotalMin;
					if(k < m_nZSTManyDay)
						g_d.DisplayText(pDC,RelPt.x+xStart+i*(xEnd-xStart)/nSecLens-size.cx,RelPt.y+1,VipColor.TIMECOLOR,GetTdxDateStr(m_ManyDayDate[k])); //显示日期
				}
			}
			if(locTime >= nSectTime[j][1])
			{
				j = (++j)%nSectNum;
				locTime=nSectTime[j][0];
			}
			if(!m_bManyDayZST || (m_bManyDayZST && (i==m_nRealTotalMin*m_nZSTManyDay)))
				g_d.DisplayText(pDC,RelPt.x+xStart+i*(xEnd-xStart)/nSecLens-size.cx,RelPt.y+1,VipColor.TIMECOLOR,GetTdxTimeStr(nMinute/*locTime*/));
			locTime+=m_bDSFlag?nDSDot:60;
		}
		if(bSwim && (m_bDataReady || !OnLine))
			g_d.DrawSwim(pDC,CRect(RelPt.x+DrawPos-size.cx-2,RelPt.y,RelPt.x+DrawPos,RelPt.y+ZST_BHEIGHT),GetTdxTimeStr(GetTimeFromX(DrawPos)));
	}
	else if(nWhich == 0)	//横坐标
	{
		int i,j,locTime = nSectTime[0][0],nDotPeriod=(m_nStyle>=ZST_MULTI?60:30);	//画虚线实线的周期
		if(m_bDSFlag&&nSecLens>500)
			nDotPeriod=m_nStyle>=ZST_MULTI?180:90;
		if(m_nStyle >= ZST_MULTI) nDotPeriod*=3;
		int nCnt = 0;
		CSize size=pDC->GetTextExtent("00:00");
		for(i=0,j=0;i<nSecLens;i+=nDotPeriod)
		{
			if(m_bManyDayZST)
			{
				if(i%m_nRealTotalMin==0)
				{
					int k = i/m_nRealTotalMin;
					if(k < m_nZSTManyDay)
						g_d.DisplayText(pDC,RelPt.x+xStart+i*(xEnd-xStart)/nSecLens-size.cx,RelPt.y+1,VipColor.TIMECOLOR,GetTdxDateStr(m_ManyDayDate[k])); //显示日期
				}
			}
			if(locTime >= nSectTime[j][1])
			{
				j++;
				locTime=nSectTime[j][0];
				i = System2Tdx(locTime, TimeSec);
				nCnt = 0;
			}
			if(!m_bManyDayZST || (m_bManyDayZST && i == m_nRealTotalMin*m_nZSTManyDay))
			{
				if(nCnt%2==0)
					g_d.DisplayText(pDC,RelPt.x+xStart+i*(xEnd-xStart)/nSecLens-size.cx,RelPt.y+1,VipColor.TIMECOLOR,GetTdxTimeStr(locTime));
			}
			locTime+=nDotPeriod;
			++nCnt;
		}
		if(bSwim && (m_bDataReady || !OnLine))
			g_d.DrawSwim(pDC,CRect(RelPt.x+DrawPos-size.cx-2,RelPt.y,RelPt.x+DrawPos,RelPt.y+ZST_BHEIGHT),GetTdxTimeStr(GetTimeFromX(DrawPos)));
	}
	else
	{
		if(nWhich == 1 && !m_nInCom2 && !m_nInHisDlg && m_lDate==0 && CanShowLevinInfo())
		{
			m_TokenOnOffRect.left=1;
			m_TokenOnOffRect.right=13;
			m_TokenOnOffRect.top=3;
			m_TokenOnOffRect.bottom=15;
			g_LevinImage.Draw(pDC,TOKEN_ONOFF,CPoint(m_TokenOnOffRect.left,m_TokenOnOffRect.top),ILD_NORMAL);
			if(g_bShowToken)
			{
				m_YMDCfgRect=m_TokenOnOffRect;
				m_YMDCfgRect.top+=15;
				m_YMDCfgRect.bottom+=15;
				g_LevinImage.Draw(pDC,TOKEN_YMDCFG,CPoint(m_YMDCfgRect.left,m_YMDCfgRect.top),ILD_NORMAL);
			}
			else
				m_YMDCfgRect.SetRect(0, 0, 0, 0);
		}
		//
		BOOL bShowRelativeBtn=FALSE;
		if((g_nCusWebPageNum>0 || g_NewJyCfg.bHasQSRelativeInfo) && !m_nInCom2 && !m_nInHisDlg && m_lDate==0)
			bShowRelativeBtn=TRUE;
		int i,tmpxsflag = m_XsFlag;
		if( (m_bZs&&m_Hq.Close>1000.0) || m_pStkInfo->bigmarket==IDX_TYPE) 
			tmpxsflag = 0;
		double fPer = fMaxPxOff/nPriceLnum;
		int	   nPer = m_lMaxVolEx/nVolLnum;
		int	   nPerVi = (m_lMaxQHPos-m_lMinQHPos)/nVolLnum;
		float  fLBPer = (m_fMaxLB-m_fMinLB)/nBotLnum;
		int    nMmldPer = m_lMaxADL/nBotLnum;
		//小图的右纵坐标
		if(nWhich==3)
		{
			g_d.BjPrintf(pDC,RelPt.x+1,RelPt.y+yBasePrice-7,fBasePrice,m_Hq.Close,tmpxsflag,0);
			for(i=0;i<nPriceLnum;i++)
				g_d.BjPrintf(pDC,RelPt.x+1,RelPt.y+yStart+i*(yBasePrice-yStart)/nPriceLnum-7,fBasePrice+fPer*(nPriceLnum-i),m_Hq.Close,tmpxsflag,0);
			for(i=1;i<nPriceLnum;i++)
				g_d.BjPrintf(pDC,RelPt.x+1,RelPt.y+yBasePrice+i*(yVol-yBasePrice)/nPriceLnum-7,fBasePrice-fPer*i,m_Hq.Close,tmpxsflag,0);
			for(i=1;i<=nVolLnum;i++)
				g_d.DisplayText(pDC,RelPt.x+2,RelPt.y+yEndVol-i*(yEndVol-yVol)/nVolLnum-7,VipColor.VOLCOLOR,0,MakeVol3(nPer*i));
			return;
		}
		//左右纵坐标
		if(nWhich == 1)
			g_d.BjPrintf(pDC,RelPt.x,RelPt.y+yBasePrice-7,fBasePrice,m_Hq.Close,tmpxsflag,ZST_LWIDTH-3);
		else
			g_d.DisplayText(pDC,RelPt.x+3,RelPt.y+yBasePrice-7,VipColor.LEVELCOLOR,0,"0.00%%");
		for(i=0;i<nPriceLnum;i++)
		{
			if(nWhich == 1)
				g_d.BjPrintf(pDC,RelPt.x,RelPt.y+yStart+i*(yBasePrice-yStart)/nPriceLnum-7,fBasePrice+fPer*(nPriceLnum-i),m_Hq.Close,tmpxsflag,ZST_LWIDTH-3);
			else
				g_d.DisplayText(pDC,RelPt.x+3,RelPt.y+yStart+i*(yBasePrice-yStart)/nPriceLnum-7,VipColor.KPCOLOR,0,"%6.2f%%",(fPer*(nPriceLnum-i)*100)/fBasePrice);
		}
		for(i=1;i<nPriceLnum;i++)
		{
			if(nWhich == 1)
				g_d.BjPrintf(pDC,RelPt.x,RelPt.y+yBasePrice+i*(yVol-yBasePrice)/nPriceLnum-7,fBasePrice-fPer*i,m_Hq.Close,tmpxsflag,ZST_LWIDTH-3);
			else
				g_d.DisplayText(pDC,RelPt.x+3,RelPt.y+yBasePrice+i*(yVol-yBasePrice)/nPriceLnum-7,VipColor.DOWNCOLOR,0,"%6.2f%%",(fPer*i*100)/fBasePrice);
		}
		for(i=1;i<=nVolLnum;i++)
		{
			if(nWhich == 1)
			{
				if(!bShowRelativeBtn)
					g_d.DisplayText(pDC,RelPt.x,RelPt.y+yEndVol-i*(yEndVol-yVol)/nVolLnum-7,VipColor.VOLCOLOR,ZST_LWIDTH-3,MakeVol3(nPer*i));
			}
			else
			{
				if(m_pStkInfo->bigmarket==QH_TYPE)
					g_d.DisplayText(pDC,RelPt.x+3,RelPt.y+yEndVol-i*(yEndVol-yVol)/nVolLnum-7,VipColor.LEVELCOLOR,0,MakeVol3(m_lMinQHPos+nPerVi*i));
				else 
					g_d.DisplayText(pDC,RelPt.x+3,RelPt.y+yEndVol-i*(yEndVol-yVol)/nVolLnum-7,VipColor.VOLCOLOR,0,MakeVol3(nPer*i));
			}
		}
		if(m_nStyle == ZST_LB && !m_bDSFlag)
		{
			int nY0 = GetYLbFromValue(m_fMinLB);
			int i=1,iGap=(yEndBot-yStartBot)/nBotLnum;
			for(int nUy=nY0-iGap;nUy>=yStartBot;nUy-=iGap,i++)
			{
				if(nWhich == 1)
				{
					if(!bShowRelativeBtn)
						g_d.DisplayText(pDC,RelPt.x,nUy-7,VipColor.LEVELCOLOR,ZST_LWIDTH-3,"%.2f",m_fMinLB+fLBPer*i);
				}
				else
					g_d.DisplayText(pDC,RelPt.x+3,nUy-7,VipColor.LEVELCOLOR,0,"%.2f",m_fMinLB+fLBPer*i);
			}
			i = 1;
			for(int nDy=nY0+iGap;nDy<=yEndBot;nDy+=iGap,i++)
			{
				if(nWhich == 1)
				{
					if(!bShowRelativeBtn)
						g_d.DisplayText(pDC,RelPt.x,nDy-7,VipColor.LEVELCOLOR,ZST_LWIDTH-3,"%.2f",m_fMinLB-fLBPer*i);
				}
				else
					g_d.DisplayText(pDC,RelPt.x+3,nDy-7,VipColor.LEVELCOLOR,0,"%.2f",m_fMinLB-fLBPer*i);
			}
		}
		if(m_nStyle == ZST_MMLD && !m_bDSFlag)
		{
			for(i=1;i<=nBotLnum;i++)
			{
				if(nWhich == 1)
				{
					if(!bShowRelativeBtn)
						g_d.DisplayText(pDC,RelPt.x,RelPt.y+yEndBot-i*(yEndBot-yStartBot)/nBotLnum-7,VipColor.LEVELCOLOR,ZST_LWIDTH-3,MakeVol3(nMmldPer*i));
				}
				else
					g_d.DisplayText(pDC,RelPt.x+3,RelPt.y+yEndBot-i*(yEndBot-yStartBot)/nBotLnum-7,VipColor.LEVELCOLOR,0,MakeVol3(nMmldPer*i));
			}
		}
		if(m_nStyle == ZST_TOTALORDER && !m_bDSFlag)
		{
			for(i=1;i<=nBotLnum;i++)
			{
				if(nWhich == 1)
				{
					if(!bShowRelativeBtn)
						g_d.DisplayText(pDC,RelPt.x,RelPt.y+yEndBot-i*(yEndBot-yStartBot)/nBotLnum-7,VipColor.LEVELCOLOR,ZST_LWIDTH-3,MakeVol3(m_lMaxBSVol/nBotLnum*i));
				}
				else
					g_d.DisplayText(pDC,RelPt.x+3,RelPt.y+yEndBot-i*(yEndBot-yStartBot)/nBotLnum-7,VipColor.LEVELCOLOR,0,MakeVol3(m_lMaxBSVol/nBotLnum*i));
			}
		}
		if(m_nStyle == ZST_TOTALCANCEL && !m_bDSFlag)
		{
			for(i=1;i<=nBotLnum;i++)
			{
				if(nWhich == 1)
				{
					if(!bShowRelativeBtn)
					{
						g_d.DisplayText(pDC,RelPt.x,RelPt.y+yEndBot-i*(yEndBot-yStartBot)/nBotLnum-7,VipColor.LEVELCOLOR,ZST_LWIDTH-3,
							MakeVol3(abs(2.0*m_lMaxOaCVol/nBotLnum*i-m_lMaxOaCVol)));
					}
				}
				else
				{
					g_d.DisplayText(pDC,RelPt.x+3,RelPt.y+yEndBot-i*(yEndBot-yStartBot)/nBotLnum-7,VipColor.LEVELCOLOR,0,
						MakeVol3(abs(2.0*m_lMaxOaCVol/nBotLnum*i-m_lMaxOaCVol)));
				}
			}
		}
		if(m_nStyle == ZST_CANCELSUM && !m_bDSFlag)
		{
			for(i=1;i<=nBotLnum;i++)
			{
				if(nWhich == 1)
				{
					if(!bShowRelativeBtn)
						g_d.DisplayText(pDC,RelPt.x,RelPt.y+yEndBot-i*(yEndBot-yStartBot)/nBotLnum-7,VipColor.LEVELCOLOR,ZST_LWIDTH-3,MakeVol3(m_fMaxCancelSum/nBotLnum*i));
				}
				else
					g_d.DisplayText(pDC,RelPt.x+3,RelPt.y+yEndBot-i*(yEndBot-yStartBot)/nBotLnum-7,VipColor.LEVELCOLOR,0,MakeVol3(m_fMaxCancelSum/nBotLnum*i));
			}
		}
		if(m_nStyle == ZST_TICKNUM && !m_bDSFlag)
		{
			for(i=1;i<=nBotLnum;i++)
			{
				if(nWhich == 1)
				{
					if(!bShowRelativeBtn)
						g_d.DisplayText(pDC,RelPt.x,RelPt.y+yEndBot-i*(yEndBot-yStartBot)/nBotLnum-7,VipColor.LEVELCOLOR,ZST_LWIDTH-3,MakeVol3(m_lMaxCjbs/nBotLnum*i));
				}
				else
					g_d.DisplayText(pDC,RelPt.x+3,RelPt.y+yEndBot-i*(yEndBot-yStartBot)/nBotLnum-7,VipColor.LEVELCOLOR,0,MakeVol3(m_lMaxCjbs/nBotLnum*i));
			}
		}
		if(m_nStyle == ZST_LARGETICK && !m_bDSFlag)
		{
			for(i=1;i<=nBotLnum;i++)
			{
				if(nWhich == 1)
				{
					if(!bShowRelativeBtn)
						g_d.DisplayText(pDC,RelPt.x,RelPt.y+yEndBot-i*(yEndBot-yStartBot)/nBotLnum-7,VipColor.LEVELCOLOR,ZST_LWIDTH-3,MakeVol3(m_lMaxLargeCjbs/nBotLnum*i));
				}
				else
					g_d.DisplayText(pDC,RelPt.x+3,RelPt.y+yEndBot-i*(yEndBot-yStartBot)/nBotLnum-7,VipColor.LEVELCOLOR,0,MakeVol3(m_lMaxLargeCjbs/nBotLnum*i));
			}
		}
		if(m_nStyle == ZST_IOVOL && !m_bDSFlag)
		{
			for(i=1;i<=nBotLnum;i++)
			{
				if(nWhich == 1)
				{
					if(!bShowRelativeBtn)
						g_d.DisplayText(pDC,RelPt.x,RelPt.y+yEndBot-i*(yEndBot-yStartBot)/nBotLnum-7,VipColor.LEVELCOLOR,ZST_LWIDTH-3,MakeVol3(m_fMaxIOVol/nBotLnum*i));
				}
				else
					g_d.DisplayText(pDC,RelPt.x+3,RelPt.y+yEndBot-i*(yEndBot-yStartBot)/nBotLnum-7,VipColor.LEVELCOLOR,0,MakeVol3(m_fMaxIOVol/nBotLnum*i));
			}
		}
		if(m_nStyle == ZST_LARGEVOL && !m_bDSFlag)
		{
			for(i=1;i<=nBotLnum;i++)
			{
				if(nWhich == 1)
				{
					if(!bShowRelativeBtn)
						g_d.DisplayText(pDC,RelPt.x,RelPt.y+yEndBot-i*(yEndBot-yStartBot)/nBotLnum-7,VipColor.LEVELCOLOR,ZST_LWIDTH-3,MakeVol3(m_fMaxLargeVol/nBotLnum*i));
				}
				else
					g_d.DisplayText(pDC,RelPt.x+3,RelPt.y+yEndBot-i*(yEndBot-yStartBot)/nBotLnum-7,VipColor.LEVELCOLOR,0,MakeVol3(m_fMaxLargeVol/nBotLnum*i));
			}
		}
		if(bSwim && (m_bDataReady || !OnLine))
		{
			char Content[50];
			if(m_XsFlag<2)
				sprintf(Content,"%9.0f",fBasePrice+fMaxPxOff/(yBasePrice-yStart)*(yBasePrice-DrawPos));
			else if(DrawPos <= yVol)
				sprintf(Content,m_XsFlag==3?"%9.3f":"%9.2f",fBasePrice+fMaxPxOff/(yBasePrice-yStart)*(yBasePrice-DrawPos));
			else if(DrawPos <= yEndVol)
				sprintf(Content,"%9.0f",m_lMaxVolEx*1.0/(yEndVol-yVol)*(yEndVol-DrawPos));
			else if(DrawPos < yEndVol)
				Content[0]=0;				
			else if(m_nStyle == ZST_LB && !m_bDSFlag)
				sprintf(Content,"%9.2f",m_fMinLB+(m_fMaxLB-m_fMinLB)/(yEndBot-yStartBot)*(yEndBot-DrawPos));
			else if(m_nStyle == ZST_MMLD && !m_bDSFlag)
				sprintf(Content,"%9.0f",(float)m_lMaxADL/(yEndBot-yStartBot)*(yEndBot-DrawPos));
			else if(m_nStyle == ZST_TOTALORDER && !m_bDSFlag)
				sprintf(Content,"%9.0f",(float)m_lMaxBSVol/(yEndBot-yStartBot)*(yEndBot-DrawPos));
			else if(m_nStyle == ZST_TOTALCANCEL && !m_bDSFlag)
				sprintf(Content,"%9.0f",fabs(2.0*m_lMaxOaCVol/(yEndBot-yStartBot)*(DrawPos-yStartBot)-m_lMaxOaCVol));
			else if(m_nStyle == ZST_CANCELSUM && !m_bDSFlag)
				sprintf(Content,"%9.0f",(float)m_fMaxCancelSum/(yEndBot-yStartBot)*(yEndBot-DrawPos));
			else if(m_nStyle == ZST_TICKNUM && !m_bDSFlag)
				sprintf(Content,"%9.0f",(float)m_lMaxCjbs/(yEndBot-yStartBot)*(yEndBot-DrawPos));
			else if(m_nStyle == ZST_LARGETICK && !m_bDSFlag)
				sprintf(Content,"%9.0f",(float)m_lMaxLargeCjbs/(yEndBot-yStartBot)*(yEndBot-DrawPos));
			else if(m_nStyle == ZST_IOVOL && !m_bDSFlag)
				sprintf(Content,"%9.0f",(float)m_fMaxIOVol/(yEndBot-yStartBot)*(yEndBot-DrawPos));
			else if(m_nStyle == ZST_LARGEVOL && !m_bDSFlag)
				sprintf(Content,"%9.0f",(float)m_fMaxLargeVol/(yEndBot-yStartBot)*(yEndBot-DrawPos));
			AllTrim(Content);
			if(Content[0]!=0 && nWhich==1)	//左纵轴游标
				g_d.DrawSwim(pDC,CRect(RelPt.x,RelPt.y+DrawPos-16,RelPt.x+ZST_LWIDTH,RelPt.y+DrawPos),Content);
			if(Content[0]!=0 && nWhich==2)	//右纵轴游标
				g_d.DrawSwim(pDC,CRect(RelPt.x,RelPt.y+DrawPos-16,RelPt.x+ZST_RWIDTH,RelPt.y+DrawPos),Content);
		}
		else DrawAddInfo(pDC,0,TRUE);
		if(nWhich==1 && bShowRelativeBtn)
		{
			int nPageNum = g_nCusWebPageNum;
			if(g_NewJyCfg.bHasQSRelativeInfo)
				nPageNum++;
			int nTmpC = min(nPageNum,(yEndBot-yVol)/COLORBTN_HEIGHT);
			int nTmpY = RelPt.y+yEndBot-16;
			int k = 0,n = 0;
			if(g_NewJyCfg.bHasQSRelativeInfo)
			{
				m_QSRelativeRect.SetRect(RelPt.x+3,nTmpY-COLORBTN_HEIGHT,RelPt.x+3+COLORBTN_WIDTH+2,nTmpY-COLORBTN_HEIGHT+COLORBTN_HEIGHT);
				g_GUI.DrawColorfulBtn(pDC,m_QSRelativeRect,_F(g_NewJyCfg.QSRelativeName),RGB(200,100,0),RGB(255,255,255));
				k++;
			}
			for(;k<nTmpC;k++)
			{				
				if(g_OEM.bOutStandCusWebPageBack)
				{
					m_RelativeRect[n].SetRect(RelPt.x+3,nTmpY-(k+1)*COLORBTN_HEIGHT,RelPt.x+3+COLORBTN_WIDTH+2,nTmpY-(k+1)*COLORBTN_HEIGHT+COLORBTN_HEIGHT);
					g_GUI.DrawColorfulBtn(pDC,m_RelativeRect[n],_F(g_CusWebPage[n].PageName),(k%2==0)?RGB(220,240,250):RGB(180,255,255),RGB(0,24,82));
				}
				else
				{
					m_RelativeRect[n].SetRect(RelPt.x+3,nTmpY-(k+1)*COLORBTN_HEIGHT,RelPt.x+6+COLORBTN_WIDTH,nTmpY-(k+1)*COLORBTN_HEIGHT+COLORBTN_HEIGHT);
				#ifdef CITY10
					g_GUI.DrawColorfulBtn(pDC,m_RelativeRect[n],_F(g_CusWebPage[n].PageName),RGB(200,100,0),RGB(255,255,255));
				#else
					g_GUI.DrawColorfulBtn(pDC,m_RelativeRect[n],_F(g_CusWebPage[n].PageName),g_GUI.LightenScaleColor((k%2==0)?15:10,VipColor.BACKCOLOR),VipColor.TXTCOLOR);
				#endif
				}
				n++;
			}
		}
	}
}

MemStkInfo * UZst::GetOverlapMemStkInfo(int which)
{
	if(which < 0 || which >= m_nOverlapNum) return NULL;
	return m_pOverlapStkInfo[which];
}	

int UZst::GetTimeFromX(double x)
{
	int minnum=int((float)(x-xStart)/fPointPerMinute+0.5)-1;
	if(m_bTotalSect)
	{
		int totalmin=0,i;
		if(minnum <= totalmin) 
			return nSectTime[0][0];
		for(i=0;i<nSectNum;i++)
		{
			if(minnum < totalmin+nSectTime[i][1]-nSectTime[i][0]) 
				return nSectTime[i][0]+minnum-totalmin;
			totalmin+=nSectTime[i][1]-nSectTime[i][0];
		}
		return nSectTime[i-1][1];
	}
	return nSectTime[0][0]+minnum;
}

int UZst::GetNowYFromX(double x)
{
	int NowMinutes = GetTimeFromX(x);
	int iWhich=-1;
	if(m_bManyDayZST)
	{
		for(int i=0;i<m_nZSTManyDay;i++)
		{
			if(x<xStart+(i+1)*m_nRealTotalMin*fPointPerMinute)
			{
				iWhich=i;
				break;
			}
		}
	}
	int nTimeNum=System2Tdx(NowMinutes,TimeSec)-1;
	float fNow = m_MinuteData[nTimeNum].Now;
	if(iWhich>=0)
		fNow = m_ManyDayMinuteData[iWhich][nTimeNum].Now;
	return GetYFromValue(fNow);
}

int UZst::GetYFromValue(double value)	//根据价位得到Y坐标
{
	return (int)((yVol-yStart)*(fMaxPxOff+fBasePrice-value)/(2*fMaxPxOff)+yStart);
}

int UZst::GetYLxFromValue(double value)
{
	return (int)(yBasePrice-(yBasePrice-yStart)*value/(fMaxLx));
}

int UZst::GetYVolFromValue(double value,BOOL bVolume)
{
	if(bVolume)	return((int)(yEndVol-(yEndVol-yVol)*value/m_lMaxVolEx));
	return((int)(yEndVol-(yEndVol-yVol)*
		(value-m_lMinQHPos)/(m_lMaxQHPos-m_lMinQHPos)));
}

int UZst::GetYLbFromValue(double value)
{
	return yEndBot-(yEndBot-yStartBot)*(value-m_fMinLB)/(m_fMaxLB-m_fMinLB);
}

int UZst::GetYMmldFromValue(long value)
{
	return yEndBot-(yEndBot-yStartBot)*value/(m_lMaxADL);
}

int UZst::GetYDiffFromValue(long value)
{
	value/=2;
	return (yEndBot+yStartBot)/2-(yEndBot-yStartBot)*value/(m_lMaxADL);
}

//一般情况，当前为-1,其它日期的数据按0,1,2
int UZst::GetXFromMinute(int minute,int flag)	//根据绝对分钟数求X坐标
{
	int ntmp = 0;
	for(int i=0;i < nSectNum;i++)
	{
		if(minute <= nSectTime[i][1])
			break;
		ntmp += nSectTime[i][1]-nSectTime[i][0];
	}
	if(i < nSectNum) ntmp+=max(0,minute-nSectTime[i][0]);	//从570开始
	if(nSecLens<1) nSecLens=1; //避免被0除	
	ntmp=max(1,ntmp);
	ntmp=min(nSecLens,ntmp);
	//对于历史走势图数据,位置有所偏+1
	if(m_bManyDayZST)
	{
		if(flag==-1)
			return min(xEnd,xStart+m_nZSTManyDay*(xEnd-xStart)/(m_nZSTManyDay+1)+ntmp*(xEnd-xStart)/nSecLens);
		else
			return min(xEnd,1+xStart+flag*(xEnd-xStart)/(m_nZSTManyDay+1)+ntmp*(xEnd-xStart)/nSecLens);
	}
	if(m_lDate==0)
		return xStart+ntmp*(xEnd-xStart)/nSecLens;
	else
		return 1+xStart+ntmp*(xEnd-xStart)/nSecLens;
}

int UZst::GetXFromTickNo(int nTickNo)
{
	if(m_nVtlData<=0 || nTickNo<0 || nTickNo>=m_nVtlData) return xStart;
	long lSecond = m_pVtlData[nTickNo].Minute*60l+m_pVtlData[nTickNo].Second;
	lSecond -= (9*60+15)*60;
	if(lSecond<0) lSecond = 0;
	if(lSecond>599) lSecond = 599;
	return min(int(1+xAucStart+(1.0*ZST_AUCTIONWIDTH)/600*(lSecond+1)),xStart);
}

short UZst::GetNowTMinute() 
{
	if(m_bDSFlag) 
	{
		m_bYestodayFalg = (m_HqEx.HqDate<GetDSCorrespondingDate(m_pTimer));
		return GetDSMinute(m_pTimer, m_bYestodayFalg);
	}
	else 
		return GetTdxMinute(TimeSec);
}

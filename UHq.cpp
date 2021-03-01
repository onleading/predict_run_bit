#include "stdafx.h"
#include "UHq.h"
#include "ComView.h"
#include "LinkStkDlg.h"
#include "IndexInfoDlg.h"
#include "MainFrm.h"

#ifdef	BIG5
	char *buysellstr[2][5] = {{"卖一","卖二","卖三","卖四","卖五"},{"买一","买二","买三","买四","买五"}};
	char *buysellstr_bighq[2][7] = {{"一","二","三","四","五"},{"一","二","三","四","五"}};
#else
	char *buysellstr[2][5] = {{"卖①","卖②","卖③","卖④","卖⑤"},{"买①","买②","买③","买④","买⑤"}};
	char *buysellstr_bighq[2][7] = {{"①","②","③","④","⑤"},{"①","②","③","④","⑤"}};
#endif	

char *kindcjstr[7] = {"Ａ股成交","Ｂ股成交","国债成交","基金成交","权证成交","债券成交","其它成交"};
char ZsStr[6][7]={SZ_DPCODE,SZ_ADPCODE,SZ_BDPCODE,SH_DPCODE,SH_ADPCODE,SH_BDPCODE};

extern void GetzsGb_Ltgb(short domain,float& zsTotalGb,float& zsTotalLtgb,float& zsTotalSz,float& zsTotalLtSz,float& m_zsSyl,float& m_zsSjl,float& m_zsYieldRate);

UHq::UHq(LPCOMINFO pComInfo,BOOL bLowSize):UBase(pComInfo)
{
	m_bLowSize = bLowSize;
	m_bSyncStatus = FALSE;
	m_nSyncMin = 0;

	m_bEmpty = TRUE;
	m_bHasJGData = FALSE;
	m_nLinkGPNum = 0;

	m_bDataReady = FALSE;

	m_zsTotalGb = m_zsTotalLtgb = m_zsTotalSz = m_zsTotalLtSz = m_zsSyl = m_zsSjl = m_zsYieldRate = 0.0;
	m_zsSZBuyCjPre = m_zsSHBuyCjPre = m_zsSZSellCjPre = m_zsSHSellCjPre = m_zsBuyCjPre = m_zsSellCjPre = 0.0f;
	m_zsSHBuyFlag = m_zsSZBuyFlag = m_zsSHSellFlag = m_zsSZSellFlag = m_zsBuyFlag = m_zsSellFlag = 0;
	m_DKRect = CRect(0,0,0,0);
	m_bDKLine = FALSE;

	for(int i=0;i<5;i++)
	{
		m_BuyRect[i].SetRectEmpty();
		m_SellRect[i].SetRectEmpty();
	}
	m_NowRect.SetRectEmpty();
	m_TVIconRect.SetRectEmpty();
	m_LinkIconRect.SetRectEmpty();
	m_JGDataIconRect.SetRectEmpty();
	m_GxdIconRect.SetRectEmpty();

	memset(&m_Hq,0,sizeof(CurrStockData));
	memset(&m_Hq_L2,0,sizeof(CurrStockData_L2));
	memset(&m_HqEx,0,sizeof(EXCURRDATA));
	memset(&m_ZsHq_SZ,0,3*sizeof(CurrStockData));
	memset(&m_ZsHq_SH,0,3*sizeof(CurrStockData));

	m_pStkInfoBak = NULL;
	m_bChangeStk = TRUE;
	
	m_ToolTip.InitToolTip(m_pView,TTS_WITHTITLE);
}

UHq::~UHq()
{
}

int  UHq::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_nCurGP==-1) //如果Unit为空，则另行处理
	{
		if(message==WM_PAINT)
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
			return 1;
		}
		return UBase::ProcessMsg(message,wParam,lParam);
	}
	switch(message)
	{
	case UM_READDATA:
		m_bDataReady = FALSE;
		ReadData(FALSE,wParam==0);
		return 1;
	case UM_GETDATA_ACK://对于远程版,请求和返回分开
		switch(wParam)
		{
		case 0:	
			m_bDataReady = TRUE;
			ReadData(TRUE);
			break;
		case 2:
			m_bDataReady = TRUE;
			g_pGetData->DSGetCurrDataACK(&m_Hq,&m_HqEx);
			break;
		}
		ProcessMsg(WM_PAINT);
		return 1;
	case ASYN_HQ_MESSAGE:
		{
			BOOL bUpdate=FALSE;
			if(m_bZs)
			{
				int i,pos = 0;
				if(m_pStkInfo->setcode == SH)
					pos = 3;
				for(i=0;i < 3;i++)
				{
					if(wParam == g_pStockIO->GetIndex(ZsStr[pos+i]))
					{	bUpdate=TRUE;	break;	}
				}
			}
			if(!m_pStkInfo)		return 1;
			short nTmpCurGp = g_pStockIO->GetIndex(m_pStkInfo->Code, m_pStkInfo->setcode);
			if(wParam == nTmpCurGp) bUpdate = TRUE;
			if(bUpdate)
				ProcessMsg(UM_UPDATE);
		}
		return 1;
	case UM_UPDATE:
		g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,&m_Hq_L2,&m_HqEx);
		CalcIt();
		ProcessMsg(WM_PAINT);
		return 1;
	case WM_PAINT:
		{
			if(g_pToolTipMark&&!m_nInCom2)
				g_pToolTipMark->ClearToolTipInfo(m_pView);

			if(wParam == 1)
				ProcessMsg(UM_READDATA,lParam);

			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			m_MyDC.SetBkMode(TRANSPARENT);

			CFont *oldfont = g_d.SelectFont(&m_MyDC,INFO_FONT);
			CPen  *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
			g_d.DrawLine(&m_MyDC,0,0,0,m_rcIn.bottom);
			if(g_GH.bLeftHQInfo && !m_nInCom2)
				g_d.DrawLine(&m_MyDC,m_rcIn.right-1,0,m_rcIn.right-1,m_rcIn.bottom);
			g_d.DrawLine(&m_MyDC,0,m_rcIn.bottom,m_rcIn.right,m_rcIn.bottom);
			if(m_bDSFlag) 
				DrawDSView(&m_MyDC);
			else	
				DrawIt(&m_MyDC);
			g_d.RestorePen(&m_MyDC,oldpen);
			g_d.RestoreFont(&m_MyDC,oldfont);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
		}
		return 1;
	case UM_SET_PLAY:
		m_bSyncStatus = (wParam==1)?TRUE:FALSE;
		if(!m_bSyncStatus)
			ProcessMsg(WM_PAINT,1);
		return 1;
	case UM_GET_PLAY:
		return m_bSyncStatus?1:0;
	case UM_ZSTSYNCH:
		{
			if(!m_bSyncStatus) //如果不是同步状态，则启动它
				ProcessMsg(UM_SET_PLAY,1);

			m_nSyncMin = (short)wParam;
			CalcIt();
			ProcessMsg(WM_PAINT);
		}
		return 1;
	case WM_RBUTTONUP:
		{
			CPoint ptt(LOWORD(lParam),HIWORD(lParam));
			CPoint pt=ptt;
			pt.x-=left;pt.y-=top;
			//先装入已有的菜单项
			CNewMenu menu;
			menu.LoadMenu (IDR_LONELY_MENU);
			CNewMenu *pPopup = (CNewMenu *)menu.GetSubMenu(15);
			m_pView->ClientToScreen (&ptt);			
			if(g_OEM.bEmbedWTVersion)
			{
				int n,i=-1,ii=-1,j=-1,jj=-1,k=-1;
				for(n=0;n<5;n++)
				{
					if(m_SellRect[n].PtInRect(pt))
					{	i=n;break;	}
					if(m_BuyRect[n].PtInRect(pt))
					{	j=n;break;	}
				}
				if(m_NowRect.PtInRect(pt))
					k=0;
				if(i>=0 || j>=0 || k>=0)
				{
					pPopup->InsertMenu(0,MF_BYPOSITION|MF_SEPARATOR);
					pPopup->InsertMenu(0,MF_BYPOSITION,IDC_PRICEWARN,_F("设置预警委托"));
					pPopup->InsertMenu(0,MF_BYPOSITION,IDC_WTCANCEL,_F("撤单查询\t22"));					
					if(i>=0)
					{
						pPopup->InsertMenu(0,MF_BYPOSITION,IDC_SELL11+i,_F("普通卖出\t223"));
						pPopup->InsertMenu(0,MF_BYPOSITION,IDC_BUY11+i,_F("普通买入\t221"));
						pPopup->InsertMenu(0,MF_BYPOSITION,IDC_LEVINSELL11+i,_F("闪电卖出\t23"));
						pPopup->InsertMenu(0,MF_BYPOSITION,IDC_LEVINBUY11+i,_F("闪电买入\t21"));
					}
					else if(j>=0)
					{
						pPopup->InsertMenu(0,MF_BYPOSITION,IDC_SELL1+j,_F("普通卖出\t223"));
						pPopup->InsertMenu(0,MF_BYPOSITION,IDC_BUY1+j,_F("普通买入\t221"));
						pPopup->InsertMenu(0,MF_BYPOSITION,IDC_LEVINSELL1+j,_F("闪电卖出\t23"));
						pPopup->InsertMenu(0,MF_BYPOSITION,IDC_LEVINBUY1+j,_F("闪电买入\t21"));
					}
					else
					{
						pPopup->InsertMenu(0,MF_BYPOSITION,IDC_SELL0,_F("普通卖出\t223"));
						pPopup->InsertMenu(0,MF_BYPOSITION,IDC_BUY0,_F("普通买入\t221"));
						pPopup->InsertMenu(0,MF_BYPOSITION,IDC_LEVINSELL0,_F("闪电卖出\t23"));
						pPopup->InsertMenu(0,MF_BYPOSITION,IDC_LEVINBUY0,_F("闪电买入\t21"));
					}
				}
			}
			ProcessMsg(WM_PAINT);
			pPopup->TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,ptt.x,ptt.y,AfxGetMainWnd());
		}
		return 1;
	case WM_MOUSEMOVE:
		{
			break;
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			g_pToolTipMark->ShowTool(pt, m_pView);
			pt.x-=left;pt.y-=top;
			
			if(m_TVIconRect.PtInRect(pt))
			{
				SetCursor(m_hFxtMove);
				return 1;
			}
			if(g_OEM.bEmbedWTVersion)
			{
				int n,i=-1,j=-1,k=-1;
				for(n=0;n<5;n++)
				{
					if(m_SellRect[n].PtInRect(pt))
					{	i=n;break;	}
					if(m_BuyRect[n].PtInRect(pt))
					{	j=n;break;	}
				}
				if(m_NowRect.PtInRect(pt))
					k=0;
				if(i>=0 || j>=0 || k>=0)
				{
					SetCursor(m_hFxtMove);
					return 1;
				}
				else
					SetCursor(LoadCursor(NULL,IDC_ARROW));
			}
			if(m_LinkIconRect.PtInRect(pt))
			{
				CRect tmprc=m_LinkIconRect;
				tmprc.OffsetRect(left,top);
				CString tmpChar = "点击查看关联证券品种";
				m_ToolTip.AddToolTip(m_pView,_F(tmpChar),&tmprc,TOOLTIP_HQ);
				return 1;
			}
			if(m_GxdIconRect.PtInRect(pt))
			{
				CRect tmprc=m_GxdIconRect;
				tmprc.OffsetRect(left,top);
				CString tmpChar = "点击查看个股对该指数的贡献度";
				m_ToolTip.AddToolTip(m_pView,_F(tmpChar),&tmprc,TOOLTIP_HQ);
				return 1;
			}
			if(m_JGDataIconRect.PtInRect(pt))
			{
				CRect tmprc=m_JGDataIconRect;
				tmprc.OffsetRect(left,top);
				CString tmpChar = "点击查看个股的机构评级和诊断";
				m_ToolTip.AddToolTip(m_pView,_F(tmpChar),&tmprc,TOOLTIP_HQ);
				return 1;
			}
			if(m_bZs&&m_bDKLine)
			{
				if(m_DKRect.PtInRect(pt))
				{
					CRect tmprc=m_DKRect;
					tmprc.OffsetRect(left,top);
					CString tmpChar = "大盘多空阵线:\r\n绿线表示当前市场的挂五档卖盘总金额,\r\n绿线前面小条表示市场的挂卖一的总金额.\r\n红线表示当时市场的挂五档买盘总金额,\r\n红线前面小条表示市场的挂买一的总金额.";
					m_ToolTip.AddToolTip(m_pView,_F(tmpChar),&tmprc,TOOLTIP_HQ);
					return 1;
				}
			}
		}
		return 1;
	case WM_LBUTTONDBLCLK:
		{
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			pt.x-=left;pt.y-=top;			
			if(g_OEM.bEmbedWTVersion && g_ExternLevinCfg.bCanDbClickPrice)
			{
				int n,i=-1,j=-1;
				for(n=0;n<5;n++)
				{
					if(m_SellRect[n].PtInRect(pt))
					{	i=n;break;	}
					if(m_BuyRect[n].PtInRect(pt))
					{	j=n;break;	}
				}
				if(i>=0 || j>=0)
				{
					if(i>=0)
						AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_LEVINBUY11+i,0);
					else if(j>=0)
						AfxGetMainWnd()->PostMessage(WM_COMMAND,IDC_LEVINSELL1+j,0);						
				}
			}
		}
		return 1;
	case UM_SWITCHLINK:	//切换相关的几只股票
		if(m_nLinkGPNum>1 && m_pStkInfo)
		{
			for(int i=0;i<m_nLinkGPNum;i++)
			{
				if(strcmp(m_LinkData[i].Code,m_pStkInfo->Code)==0) 
					break;
			}
			if(i<m_nLinkGPNum)
			{
				CString tmpStr;
				MemStkInfo *tmpInfo=NULL;
				int tmpInt=i;
				while(!tmpInfo)
				{
					i=(i+1)%m_nLinkGPNum;
					if(i==tmpInt) break;
					if(strlen(m_LinkData[i].Code)==5)
					{
						tmpStr = m_LinkData[i].Code;
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
						ToZst(tmpInfo->Code,tmpInfo->setcode,FALSE);
						return 1;
					}
				}
			}
		}
		else
			TDX_MessageBox(m_pView->m_hWnd,"无关联板块或关联板块没有行情",MB_OK|MB_ICONINFORMATION);
		return 1;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void UHq::ReadData(BOOL bDataReady,BOOL bReqData)
{
	g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,&m_Hq_L2,&m_HqEx);
	if(m_bZs) GetzsGb_Ltgb(m_bSZSHZS?2:m_Domain,m_zsTotalGb,m_zsTotalLtgb,m_zsTotalSz,m_zsTotalLtSz,m_zsSyl,m_zsSjl,m_zsYieldRate);

	if(!bDataReady)
	{
		if(!OnLine || m_bLcIdx || m_bLcExt || m_bLcPt) //如果是脱机状态，直接读取缓存的
		{
			g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,&m_Hq_L2,&m_HqEx);
			bDataReady = TRUE;
		}
		if(OnLine && bReqData)
		{
			m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
			if(m_bDSFlag)
			{
				g_nAssisID = SetComAssisID(m_nWhichUnit,2);
				g_pGetData->DSGetCurrData(m_pStkInfo);
			}
			else
			{
				g_nAssisID = SetComAssisID(m_nWhichUnit,0);
				g_pGetData->GetCurrStockData(m_pStkInfo);
			}
		}
	}
	else
	{
		if(m_bDSFlag) g_pGetData->DSGetCurrDataACK(&m_Hq,&m_HqEx);
		else	g_pGetData->GetCurrStockDataACK(m_pStkInfo,&m_Hq,&m_Hq_L2);
	}
	CalcIt();

	m_bHasJGData = FALSE;

	m_nLinkGPNum = g_pStockIO->GetRLTData(m_pStkInfo, m_LinkData, 100);
}

void UHq::CalcIt()
{
	if(m_bZs)
	{
		int i;
		for(i=0;i < 3;i++)
			g_pStockIO->ReadCurrStockDataForCode(&m_ZsHq_SZ[i],ZsStr[i]);
		for(i=0;i < 3;i++)
			g_pStockIO->ReadCurrStockDataForCode(&m_ZsHq_SH[i],ZsStr[3+i]);
	}
	
	memset(&m_HqContent,0,sizeof(UHqContent));
	memset(&m_ZsHqContent,0,sizeof(UZsHqContent));
	struct TickData *pTick = NULL;
	TICK_MMP *pMmp = NULL;
	short tickflag=0,mmpflag=0;
	int nTickNum=0,nCurTick=0;
	int i,ii;

	if(m_bZs) //如果是指数
	{
		m_ZsHqContent.Close = m_Hq.Close;
		if(!m_bSyncStatus)
		{
			if(m_bSZSHZS)
			{
				m_ZsHqContent.Cj[0] = m_ZsHq_SZ[1].Amount+m_ZsHq_SH[1].Amount;
				m_ZsHqContent.Cj[1] = m_ZsHq_SZ[2].Amount+m_ZsHq_SH[2].Amount;
				m_ZsHqContent.Cj[2] = m_ZsHq_SZ[0].Buyp[2]+m_ZsHq_SH[0].Buyp[2];
				m_ZsHqContent.Cj[3] = m_ZsHq_SZ[0].Buyp[0]+m_ZsHq_SH[0].Buyp[0];
				m_ZsHqContent.Cj[4] = m_ZsHq_SZ[0].Sellp[0]+m_ZsHq_SH[0].Sellp[0];
				m_ZsHqContent.Cj[5] = m_ZsHq_SZ[0].Buyp[1]+m_ZsHq_SH[0].Buyp[1];
				m_ZsHqContent.Cj[6] = m_ZsHq_SZ[0].Sellp[1]+m_ZsHq_SH[0].Sellp[1];

				m_ZsHqContent.Buy1Cj = m_ZsHq_SH[2].Sellp[0]+m_ZsHq_SH[2].Buyp[0];
				m_ZsHqContent.Sell1Cj = m_ZsHq_SH[2].Sellp[1]+m_ZsHq_SH[2].Buyp[1];
				m_ZsHqContent.Buy12345Cj = m_ZsHq_SH[2].Sellp[3]+m_ZsHq_SH[2].Buyp[3];
				m_ZsHqContent.Sell12345Cj = m_ZsHq_SH[2].Sellp[4]+m_ZsHq_SH[2].Buyp[4];

				if(m_zsBuyCjPre>COMPPREC&&fabs(m_ZsHqContent.Buy12345Cj-m_zsBuyCjPre)>COMPPREC) 
				{
					if(m_ZsHqContent.Buy12345Cj-m_zsBuyCjPre>COMPPREC) m_zsBuyFlag = 1;
					else if(m_ZsHqContent.Buy12345Cj-m_zsBuyCjPre<0-COMPPREC) m_zsBuyFlag = -1;
					else m_zsBuyFlag = 0;
				}
				m_zsBuyCjPre = m_ZsHqContent.Buy12345Cj;

				if(m_zsSellCjPre>COMPPREC&&fabs(m_ZsHqContent.Sell12345Cj-m_zsSellCjPre)>COMPPREC) 
				{
					if(m_ZsHqContent.Sell12345Cj-m_zsSellCjPre>COMPPREC) m_zsSellFlag = 1;
					else if(m_ZsHqContent.Sell12345Cj-m_zsSellCjPre<0-COMPPREC) m_zsSellFlag = -1;
					else m_zsSellFlag = 0;
				}
				m_zsSellCjPre = m_ZsHqContent.Sell12345Cj;
			}
			else if(m_pStkInfo->setcode==SZ)
			{
				m_ZsHqContent.Cj[0] = m_ZsHq_SZ[1].Amount;
				m_ZsHqContent.Cj[1] = m_ZsHq_SZ[2].Amount;
				m_ZsHqContent.Cj[2] = m_ZsHq_SZ[0].Buyp[2];
				m_ZsHqContent.Cj[3] = m_ZsHq_SZ[0].Buyp[0];
				m_ZsHqContent.Cj[4] = m_ZsHq_SZ[0].Sellp[0];
				m_ZsHqContent.Cj[5] = m_ZsHq_SZ[0].Buyp[1];
				m_ZsHqContent.Cj[6] = m_ZsHq_SZ[0].Sellp[1];

				m_ZsHqContent.Buy1Cj = m_ZsHq_SH[2].Buyp[0];
				m_ZsHqContent.Sell1Cj = m_ZsHq_SH[2].Buyp[1];
				m_ZsHqContent.Buy12345Cj = m_ZsHq_SH[2].Buyp[3];
				m_ZsHqContent.Sell12345Cj = m_ZsHq_SH[2].Buyp[4];

				if(m_zsSZBuyCjPre>COMPPREC&&fabs(m_ZsHqContent.Buy12345Cj-m_zsSZBuyCjPre)>COMPPREC) 
				{
					if(m_ZsHqContent.Buy12345Cj-m_zsSZBuyCjPre>COMPPREC) m_zsSZBuyFlag = 1;
					else if(m_ZsHqContent.Buy12345Cj-m_zsSZBuyCjPre<0-COMPPREC) m_zsSZBuyFlag = -1;
					else m_zsSZBuyFlag = 0;
				}
				m_zsSZBuyCjPre = m_ZsHqContent.Buy12345Cj;

				if(m_zsSZSellCjPre>COMPPREC&&fabs(m_ZsHqContent.Sell12345Cj-m_zsSZSellCjPre)>COMPPREC) 
				{
					if(m_ZsHqContent.Sell12345Cj-m_zsSZSellCjPre>COMPPREC) m_zsSZSellFlag = 1;
					else if(m_ZsHqContent.Sell12345Cj-m_zsSZSellCjPre<0-COMPPREC) m_zsSZSellFlag = -1;
					else m_zsSZSellFlag = 0;
				}
				m_zsSZSellCjPre = m_ZsHqContent.Sell12345Cj;
			}
			else
			{
				m_ZsHqContent.Cj[0] = m_ZsHq_SH[1].Amount;
				m_ZsHqContent.Cj[1] = m_ZsHq_SH[2].Amount;
				m_ZsHqContent.Cj[2] = m_ZsHq_SH[0].Buyp[2];
				m_ZsHqContent.Cj[3] = m_ZsHq_SH[0].Buyp[0];
				m_ZsHqContent.Cj[4] = m_ZsHq_SH[0].Sellp[0];
				m_ZsHqContent.Cj[5] = m_ZsHq_SH[0].Buyp[1];
				m_ZsHqContent.Cj[6] = m_ZsHq_SH[0].Sellp[1];

				m_ZsHqContent.Buy1Cj = m_ZsHq_SH[2].Sellp[0];
				m_ZsHqContent.Sell1Cj = m_ZsHq_SH[2].Sellp[1];
				m_ZsHqContent.Buy12345Cj = m_ZsHq_SH[2].Sellp[3];
				m_ZsHqContent.Sell12345Cj = m_ZsHq_SH[2].Sellp[4];
				
				if(m_zsSHBuyCjPre>COMPPREC&&fabs(m_ZsHqContent.Buy12345Cj-m_zsSHBuyCjPre)>COMPPREC) 
				{
					if(m_ZsHqContent.Buy12345Cj-m_zsSHBuyCjPre>COMPPREC) m_zsSHBuyFlag = 1;
					else if(m_ZsHqContent.Buy12345Cj-m_zsSHBuyCjPre<0-COMPPREC) m_zsSHBuyFlag = -1;
					else m_zsSHBuyFlag = 0;
				}
				m_zsSHBuyCjPre = m_ZsHqContent.Buy12345Cj;

				if(m_zsSHSellCjPre>COMPPREC&&fabs(m_ZsHqContent.Sell12345Cj-m_zsSHSellCjPre)>COMPPREC) 
				{
					if(m_ZsHqContent.Sell12345Cj-m_zsSHSellCjPre>COMPPREC) m_zsSHSellFlag = 1;
					else if(m_ZsHqContent.Sell12345Cj-m_zsSHSellCjPre<0-COMPPREC) m_zsSHSellFlag = -1;
					else m_zsSHSellFlag = 0;
				}
				m_zsSHSellCjPre = m_ZsHqContent.Sell12345Cj;
			}
			m_ZsHqContent.Open = m_Hq.Open;
			m_ZsHqContent.Max = m_Hq.Max;
			m_ZsHqContent.Min = m_Hq.Min;
			m_ZsHqContent.Volume  = m_Hq.Volume;
			m_ZsHqContent.Amount  = m_Hq.Amount;
			m_ZsHqContent.UpNum = m_Hq.Buyv[0];
			m_ZsHqContent.DownNum = m_Hq.Sellv[0];
			m_ZsHqContent.TickDiff = m_Hq.TickDiff;
			//现价
			m_ZsHqContent.Now = m_Hq.Now;
			if(m_Hq.Now > COMPPREC)
			{
				//涨跌
				m_ZsHqContent.Zd = m_Hq.Now-m_Hq.Close;
				//幅度
				if(m_Hq.Close > COMPPREC)
					m_ZsHqContent.Zaf = m_ZsHqContent.Zd/m_Hq.Close*100.0;
			}
			if (m_Hq.Close>0)
			{
				float ftmp;
				if ( m_Hq.Max > COMPPREC && m_Hq.Min > COMPPREC )
					ftmp = m_Hq.Max - m_Hq.Min;
				else
					ftmp = 0.0;
				m_ZsHqContent.Zef = ftmp/m_Hq.Close*100.0;
			}
			if(m_zsTotalLtgb*10 > 1)
			{
				if(m_bSZSHZS)
					m_ZsHqContent.Zs_hsl = (m_ZsHq_SZ[0].Volume+m_ZsHq_SH[0].Volume)/m_zsTotalLtgb;
				else if(m_pStkInfo->setcode==SZ)
					m_ZsHqContent.Zs_hsl = m_ZsHq_SZ[0].Volume/m_zsTotalLtgb;
				else
					m_ZsHqContent.Zs_hsl = m_ZsHq_SH[0].Volume/m_zsTotalLtgb;
			}
		}
		else
		{
			//现价
			if(nTickNum > 0)
				m_ZsHqContent.Now = pTick[nCurTick].Now/1000.0;
			if(m_ZsHqContent.Now > COMPPREC)
			{
				//涨跌
				m_ZsHqContent.Zd = m_ZsHqContent.Now-m_Hq.Close;
				//幅度
				if(m_Hq.Close > COMPPREC)
					m_ZsHqContent.Zaf = m_ZsHqContent.Zd/m_Hq.Close*100.0;
			}
		}
	}
	else
	{
		m_HqContent.Close = m_Hq.Close;
		m_HqContent.Open = m_Hq.Open;
		//委比和委差
		long wlc = 0,wl = 0;
		float ftmp;
		
		float tmpNow=0.0,tmpMax=0.0,tmpMin=0.0;
		DWORD  tmpVolume=0,tmpNowVol=0,tmpOutside=0,tmpInside=0;
		
		if(!m_bSyncStatus)
		{
			m_HqContent.buywtl = 0;
			m_HqContent.sellwtl = 0;
			for (ii = 0;ii < 5;ii++)
			{   
				m_HqContent.buywtl += m_Hq.Buyv[ii];
				m_HqContent.sellwtl += m_Hq.Sellv[ii];
				wlc = wlc+(long)m_Hq.Buyv[ii]-(long)m_Hq.Sellv[ii];
				wl = wl +(long)m_Hq.Buyv[ii]+(long)m_Hq.Sellv[ii];
			}
			if(m_bLevel2Property)
			{
				for (ii = 0;ii < 5;ii++)
				{   
					m_HqContent.buywtl += m_Hq_L2.Buyv[ii];
					m_HqContent.sellwtl += m_Hq_L2.Sellv[ii];
					wlc = wlc+(long)m_Hq_L2.Buyv[ii]-(long)m_Hq_L2.Sellv[ii];
					wl = wl +(long)m_Hq_L2.Buyv[ii]+(long)m_Hq_L2.Sellv[ii];
				}
			}
			if (wl != 0l) 
				m_HqContent.Wtb = (float)100.0*wlc/wl;
			m_HqContent.Wlc = wlc;
			//买卖盘
			for(i = 0;i < 5;i++)
			{
				m_HqContent.Buyprice[i] = m_Hq.Buyp[i];
				m_HqContent.Buyvol[i] = m_Hq.Buyv[i];
				m_HqContent.Sellprice[i] = m_Hq.Sellp[i];
				m_HqContent.Sellvol[i] = m_Hq.Sellv[i];
			}
			m_HqContent.Average = cal_averagetmp(m_pStkInfo,&m_Hq);

			tmpNow = m_Hq.Now;
			tmpMax = m_Hq.Max;
			tmpMin = m_Hq.Min;
			tmpVolume = m_Hq.Volume;
			tmpNowVol = m_Hq.NowVol;
			tmpOutside = m_Hq.Outside;
			tmpInside = m_Hq.Volume-m_Hq.Outside;
		}
		else
		{
			m_HqContent.Average = 0;
			if(nTickNum > 0)
			{
				tmpNow = pTick[nCurTick].Now/1000.0;
				tmpNowVol = pTick[nCurTick].NowVol;
				tmpMax = tmpMin = m_Hq.Open;
				for(i = 0;i < nCurTick;i++)
				{
					tmpVolume += pTick[i].NowVol;
					if(pTick[i].InOutFlag==0)
						tmpOutside += pTick[i].NowVol;
					else if(pTick[i].InOutFlag==1)
						tmpInside += pTick[i].NowVol;
					if(pTick[i].Now/1000.0 > tmpMax) tmpMax = pTick[i].Now/1000.0;
					if(pTick[i].Now/1000.0 < tmpMin) tmpMin = pTick[i].Now/1000.0;
				}
			}
		}
		m_HqContent.Now = tmpNow;
		//修正涨跌和幅度的计算方法,对于开放式集合竞价有用
		if(tmpNow<COMPPREC && m_HqContent.Buyprice[0]>COMPPREC && fabs(m_HqContent.Buyprice[0]-m_HqContent.Sellprice[0])<COMPPREC)
			tmpNow=m_HqContent.Buyprice[0];
		//涨跌
		if(tmpNow > COMPPREC)
			m_HqContent.Zd = tmpNow-m_Hq.Close;
		//幅度
		if(m_Hq.Close > COMPPREC)
		{
			m_HqContent.Zaf = m_HqContent.Zd/m_Hq.Close*100.0;
			if ( m_Hq.Max > COMPPREC && m_Hq.Min > COMPPREC )
				m_HqContent.Zef = (m_Hq.Max - m_Hq.Min)/m_Hq.Close*100.0;
		}
		m_HqContent.Max = tmpMax;
		m_HqContent.Min = tmpMin;
		m_HqContent.Volume = tmpVolume;
		m_HqContent.Amount = m_Hq.Amount;
		m_HqContent.Outside = tmpOutside;
		m_HqContent.Inside = tmpInside;
		m_HqContent.NowVol = tmpNowVol;
		m_HqContent.CJBS = m_Hq.CJBS;
		//量比
		short fz[8];
		InitStkTime(m_pStkInfo->setcode,fz);
		ii = GetTdxMinute(fz);
		if (ii > 0 && m_pStkInfo->VolBase > 0l)
			m_HqContent.fLB = (float)tmpVolume/ii/m_pStkInfo->VolBase;
		//股本
		m_HqContent.Zgb = m_pStkInfo->J_zgb;
		m_HqContent.ActiveCapital = m_pStkInfo->ActiveCapital;
		//换手率
		if(m_pStkInfo->ActiveCapital > 1)
			m_HqContent.Hsl = 1.0*tmpVolume*m_pStkInfo->Unit/(m_pStkInfo->ActiveCapital*100.0);
		//市盈率
		if(m_pStkInfo->J_zgb > 1)
		{
			ftmp = (m_pStkInfo->J_jly*12/m_pStkInfo->J_HalfYearFlag)/m_pStkInfo->J_zgb;
			if(ftmp > COMPPREC)
			{
				if(m_HqContent.Now > COMPPREC)
					m_HqContent.Syl = m_HqContent.Now/ftmp;
				else if(m_Hq.Close > COMPPREC)
					m_HqContent.Syl = m_Hq.Close/ftmp;
			}
		}
		//对于国债或开放式基金有效
		m_HqContent.Yjlx = m_Hq.Yield_VolInStock.Yield;
		m_HqContent.Dqsy = m_Hq.GZFlag.YQSY/100.0;
		m_HqContent.Jjjz = m_Hq.Jjjz;
		m_HqContent.LastPV = 0;
		if(m_pStkInfo->setcode==SZ && m_bETF)
		{	//对于深交所的ETF
			m_HqContent.LastPV = m_HqContent.Yjlx/100.0;
		}
		for(int i=0;m_pStkInfo->setcode==SH && i<g_nETFInfoNum;i++)
		{
			if(g_ETFInfo[i].setcode==SH && strcmp(m_pStkInfo->Code,g_ETFInfo[i].Code)==0)
			{
				//对于ETF,LastPV为IOPVCode的昨收盘
				struct CurrStockData tmpHQ;
				g_pStockIO->ReadCurrStockDataForCode(&tmpHQ,g_ETFInfo[i].IOPVCode);
				m_HqContent.LastPV = tmpHQ.Close;
				break;
			}
		}			
		m_HqContent.TPPrice1=GetTPPrice(m_pStkInfo,m_Hq.Close,TRUE);
		m_HqContent.TPPrice2=GetTPPrice(m_pStkInfo,m_Hq.Close,FALSE);
		if(m_bQz)
			GetQZData(m_pStkInfo,&m_Hq,m_HqContent.fGgbl,m_HqContent.fInnerVal,m_HqContent.fYjl);
		if(m_pStkInfo->J_zgb > 1)
		{
			//每股净资产
			if(fabs(m_pStkInfo->J_jzc)>COMPPREC)
				m_HqContent.Mg_Jz = m_pStkInfo->J_jzc/m_pStkInfo->J_zgb;
			else
				m_HqContent.Mg_Jz = m_pStkInfo->J_tzmgjz;
			//每股收益
			if(fabs(m_pStkInfo->J_jly)>COMPPREC)
			{
				if(fabs(m_pStkInfo->J_mgsy2)>COMPPREC)
					m_HqContent.Mg_Sy = m_pStkInfo->J_mgsy2;
				else
					m_HqContent.Mg_Sy = m_pStkInfo->J_jly/m_pStkInfo->J_zgb;
			}
		}
		if(m_pStkInfoBak==NULL || m_pStkInfo==NULL || memcmp(m_pStkInfoBak,m_pStkInfo,sizeof(MemStkInfo))!=0)	//换股
		{
			memcpy(&m_HqContentBak,&m_HqContent,sizeof(UHqContent));
			memcpy(&m_HqContentShow,&m_HqContent,sizeof(UHqContent));
			m_bChangeStk = TRUE;
		}
		else m_bChangeStk = FALSE;
	}
	m_pStkInfoBak = m_pStkInfo;
	TDEL(pTick);
	TDEL(pMmp);
}

void UHq::DrawZsHq(CDC *pDC)
{
	m_bEmpty = m_ZsHqContent.Now < COMPPREC;
	m_bDKLine = (m_ZsHqContent.Buy1Cj+m_ZsHqContent.Sell1Cj+m_ZsHqContent.Buy12345Cj+m_ZsHqContent.Sell12345Cj>COMPPREC);
	if(!g_bG2DT || m_bLcIdx || m_bLcExt || m_bLcPt) 
		m_bDKLine=FALSE;
	int nFixedWidth=m_rcIn.right;
	int YFrom=5,XFrom1=HQ_XSPACE,XFrom2=nFixedWidth/6,XFrom3=nFixedWidth/2+HQ_XSPACE,XFrom4=4*nFixedWidth/6,XWidth=nFixedWidth/2-2*HQ_XSPACE,i;
	if(g_GH.bBigHq)
	{
		DrawBigHqHead(pDC);	
		YFrom=30+34;
	}
	else
	{
		g_d.SelectFont(pDC,MEDIUM_FONT);
		COLORREF cn = RGB(255,255,0);
		if(!m_bSyncStatus)
		{
			CSize tmpSize=g_d.DisplayText(pDC,36,YFrom,cn,"%6s ",m_pStkInfo->Code);
			CSize zs = g_d.DisplaySameWidtchText(pDC,36+tmpSize.cx,YFrom,cn,"%s",m_pStkInfo->Name);
			DrawMarkFlag(m_pStkInfo,pDC,36+tmpSize.cx+zs.cx,YFrom-3,left,top,m_pView);
		}
		else
			g_d.DisplayText(pDC,1,YFrom,cn,"%6s %8s [%s]",m_pStkInfo->Code,m_pStkInfo->Name,GetTdxTimeStr(m_nSyncMin));
		g_d.SelectFont(pDC,INFO_FONT);	
		YFrom+=23;
	}
	g_d.DrawLine(pDC,0,YFrom,m_rcIn.right,YFrom);
	YFrom+=3;
	
	if(m_bDKLine)
	{		
		m_DKRect.left = 0;
		m_DKRect.top = YFrom+6;
		short zsBuyFlag = 0, zsSellFlag = 0;
		if(m_bSZSHZS)
		{
			zsBuyFlag = m_zsBuyFlag;
			zsSellFlag = m_zsSellFlag;
		}
		else if(m_pStkInfo->setcode==SZ)
		{
			zsBuyFlag = m_zsSZBuyFlag;
			zsSellFlag = m_zsSZSellFlag;
		}
		else
		{
			zsBuyFlag = m_zsSHBuyFlag;
			zsSellFlag = m_zsSHSellFlag;
		}

		CFont *oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
		
		float dkmax=m_ZsHqContent.Buy12345Cj;
		if(dkmax<m_ZsHqContent.Sell12345Cj) dkmax=m_ZsHqContent.Sell12345Cj;
		int dk_maxwidth=m_rcIn.right-3*HQ_XSPACE-50;

		YFrom+=6;
		g_d.Bar(pDC,HQ_XSPACE,YFrom,dk_maxwidth*m_ZsHqContent.Sell12345Cj*1.00/dkmax,YFrom+5,DOWN_BRUSH);
		g_d.Bar(pDC,dk_maxwidth*m_ZsHqContent.Sell1Cj*1.00/dkmax,YFrom,dk_maxwidth*m_ZsHqContent.Sell1Cj*1.00/dkmax+1,YFrom+5,BACK_BRUSH);
		g_d.DisplayText(pDC,HQ_XSPACE+dk_maxwidth,YFrom-6,VipColor.VOLCOLOR,48,MakeJE3(m_ZsHqContent.Sell12345Cj*10000.0));
		if(zsSellFlag) 
			pDC->DrawIcon(HQ_XSPACE+dk_maxwidth+1+48,YFrom-6,zsSellFlag>0?m_hGreenUP:m_hGreenDown);
		YFrom+=15;
		g_d.Bar(pDC,HQ_XSPACE,YFrom,dk_maxwidth*m_ZsHqContent.Buy12345Cj*1.00/dkmax,YFrom+5,KP_BRUSH);
		g_d.Bar(pDC,dk_maxwidth*m_ZsHqContent.Buy1Cj*1.00/dkmax,YFrom,dk_maxwidth*m_ZsHqContent.Buy1Cj*1.00/dkmax+1,YFrom+5,BACK_BRUSH);
		g_d.DisplayText(pDC,HQ_XSPACE+dk_maxwidth,YFrom-6,VipColor.VOLCOLOR,48,MakeJE3(m_ZsHqContent.Buy12345Cj*10000.0));
		if(zsBuyFlag) 
			pDC->DrawIcon(HQ_XSPACE+dk_maxwidth+1+48,YFrom-6,zsBuyFlag>0?m_hRedUP:m_hRedDown);

		YFrom+=14;
		g_d.DrawLine(pDC,0,YFrom,m_rcIn.right,YFrom);
		YFrom+=3;

		g_d.RestoreFont(pDC,oldfont);

		m_DKRect.right = HQ_XSPACE+dk_maxwidth;
		m_DKRect.bottom = YFrom-6;
	}
	
	if(g_GH.bBigHq)
	{
		if(m_bSZSHZS)
			g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("沪深市值"));
		else
			g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,(m_Domain==SZ)?(_F("深总市值")):(_F("沪总市值")));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,XWidth,MakeJE3(m_zsTotalSz));
		YFrom += HQ_TEXTH;
		g_d.DrawLine(pDC,0,YFrom,m_rcIn.right,YFrom);
		YFrom += 3;
	}
	if(m_bSZSHZS)
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("沪深成交"));
		if(!m_bEmpty)
			g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.VOLCOLOR,XWidth,MakeJE3(m_ZsHq_SZ[0].Amount+m_ZsHq_SH[0].Amount));
		YFrom += HQ_TEXTH;
		YFrom += 1;
	}
	int ShowZsHzNum=6;
	if(m_bDKLine) ShowZsHzNum=5;
	for(i = 0;i < ShowZsHzNum;i++)
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(kindcjstr[i]));
		if(!m_bEmpty)
			g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.VOLCOLOR,XWidth,MakeJE2(m_ZsHqContent.Cj[i]));
		YFrom += HQ_TEXTH;
	}
	g_d.DrawLine(pDC,0,YFrom,m_rcIn.right,YFrom);
	YFrom += 3;

	if(!g_GH.bBigHq)
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("最新指数"));
		if(!m_bEmpty)
			g_d.BjPrintf(pDC,XFrom3,YFrom,m_ZsHqContent.Now,m_ZsHqContent.Close,m_XsFlag,XWidth);
		YFrom += HQ_TEXTH;
	}

	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("今日开盘"));
	if(!m_bEmpty)
		g_d.BjPrintf(pDC,XFrom3,YFrom,m_ZsHqContent.Open,m_ZsHqContent.Close,m_XsFlag,XWidth);

	YFrom += HQ_TEXTH;	
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("昨日收盘"));
	if(!m_bEmpty)
		g_d.BjPrintf(pDC,XFrom3,YFrom,m_ZsHqContent.Close,m_ZsHqContent.Close,m_XsFlag,XWidth);

	if(!g_GH.bBigHq)
	{
		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("指数涨跌"));
		if(!m_bEmpty)
			g_d.BjPrintf(pDC,XFrom3,YFrom,m_ZsHqContent.Zd,0.0,m_XsFlag,XWidth);
		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("指数涨幅"));
		if(!m_bEmpty)
			g_d.BjPrintf(pDC,XFrom3,YFrom,m_ZsHqContent.Zaf,0.0,2,XWidth,1);
	}

	YFrom += HQ_TEXTH;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("指数振幅"));
	if(!m_bEmpty)
		g_d.NoBjPrintf(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,m_ZsHqContent.Zef,2,XWidth,1);

	if(!m_bDKLine)
	{
		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("指数笔涨 "));
		if(!m_bEmpty)
			g_d.BjPrintf(pDC,XFrom3,YFrom,m_ZsHqContent.TickDiff,0.0,m_XsFlag,XWidth);
	}

	YFrom += HQ_TEXTH;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("总成交量"));
	if(!m_bEmpty && m_ZsHqContent.Volume>0)
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_ZsHqContent.Volume);

	YFrom += HQ_TEXTH;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("总成交额"));
	if(!m_bEmpty && m_ZsHqContent.Amount>0)
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.VOLCOLOR,XWidth,MakeJE2(m_ZsHqContent.Amount));

	YFrom += HQ_TEXTH;	
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("最高指数"));
	if(!m_bEmpty && m_ZsHqContent.Max>0)
		g_d.BjPrintf(pDC,XFrom3,YFrom,m_ZsHqContent.Max,m_ZsHqContent.Close,m_XsFlag,XWidth);
	YFrom += HQ_TEXTH;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("最低指数"));
	if(!m_bEmpty && m_ZsHqContent.Min>0)
		g_d.BjPrintf(pDC,XFrom3,YFrom,m_ZsHqContent.Min,m_ZsHqContent.Close,m_XsFlag,XWidth);

	YFrom += HQ_TEXTH;
	if(m_bSZSHZS)
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("沪深换手"));
	else
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,(m_Domain==SZ)?_F("深证换手"):_F("上证换手"));
	if(!m_bEmpty && m_ZsHqContent.Volume>0)
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,XWidth,"%9.2f%%",m_ZsHqContent.Zs_hsl);

	XWidth=2*nFixedWidth/6-2;
	YFrom += HQ_TEXTH;

	if(!m_bSZSHZS)
	{
		g_d.DrawLine(pDC,0,YFrom,m_rcIn.right,YFrom);
		YFrom += 4;

		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("涨家数"));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("跌家数"));
		if(!m_bEmpty)
		{
			if(m_ZsHqContent.UpNum > 0)
				g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.KPCOLOR,XWidth,"%9u",m_ZsHqContent.UpNum);
			if(m_ZsHqContent.DownNum > 0)
				g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.DOWNCOLOR,XWidth,"%9u",m_ZsHqContent.DownNum);
		}
	}
	g_d.DrawLine(pDC,0,m_rcIn.bottom-1,m_rcIn.right,m_rcIn.bottom-1);
}

void UHq::DrawIt(CDC *pDC)
{
	if(Height() < 20)	return;	//如果空间太小，不画
	int i,j;
	for(j=0;j<5;j++)
	{
		m_BuyRect[j].SetRectEmpty();
		m_SellRect[j].SetRectEmpty();
	}
	m_NowRect.SetRectEmpty();
	/*
	m_TVIconRect.SetRectEmpty();
	m_LinkIconRect.SetRectEmpty();
	m_JGDataIconRect.SetRectEmpty();
	m_GxdIconRect.SetRectEmpty();
	if(!m_bSyncStatus&&m_bHasJGData) 
	{
		m_JGDataIconRect.SetRect(2,4,2+16,4+16);
		pDC->DrawIcon(2,4,m_hIcon_Pu[5]);
	}
	else if(!m_bSyncStatus&&m_nGxdFlag>=0)
	{
		m_GxdIconRect.SetRect(2,4,2+16,4+16);
		pDC->DrawIcon(2,4,m_hIcon_Pu[7]);
	}
	if(!m_bSyncStatus&&m_nLinkGPNum>0) 
	{
		m_LinkIconRect.SetRect(18,4,18+16,4+16);
		pDC->DrawIcon(18,4,m_hIcon_Pu[4]);
	}
	*/
	if(m_bZs) //如果是指数
	{
		DrawZsHq(pDC);
		return;
	}
	if(g_GH.bBigHq) //大字模式时
	{
		DrawBigHq(pDC);
		return;
	}
	//对于新股申购,只有最高最低值，也要显示出来,在早市时，现价为0不代表停牌
	m_bEmpty = m_HqContent.Now+m_HqContent.Max+m_HqContent.Min+m_HqContent.Buyprice[0]+m_HqContent.Sellprice[0]+m_HqContent.Buyvol[0]+m_HqContent.Sellvol[0] < COMPPREC;
	CFont *oldfont = NULL;
	int nFixedWidth=m_rcIn.right;
	int YFrom=6;
	int XFrom1=HQ_XSPACE,XFrom2=nFixedWidth/6+2,XFrom3=nFixedWidth/2+HQ_XSPACE,XFrom4=4*nFixedWidth/6+2;
	int XWidth=2*nFixedWidth/6-HQ_XSPACE;
	g_d.SelectFont(pDC,MEDIUM_FONT);
	COLORREF cn = RGB(255,255,0);
	if(!m_bSyncStatus)
	{
		CSize tmpSize=g_d.DisplayText(pDC,36,YFrom,cn,"%6s ",m_pStkInfo->Code);
		CSize zs = g_d.DisplaySameWidtchText(pDC,36+tmpSize.cx,YFrom,cn,"%s",m_pStkInfo->Name);
		DrawMarkFlag(m_pStkInfo,pDC,36+tmpSize.cx+zs.cx,YFrom-3,left,top,m_pView);
		if(g_bHasTopView && m_bLevel2Property && !m_nInCom2)
		{
			g_d.SelectFont(pDC,BOLDGRAPH_FONT);
			tmpSize=g_d.DisplayText(pDC,nFixedWidth-28,YFrom,RGB(192,0,192),_F("赢富"));
			m_TVIconRect.SetRect(nFixedWidth-50,YFrom,nFixedWidth,YFrom+20);
		}
	}
	else
		g_d.DisplayText(pDC,1,YFrom,cn,"%6s %8s [%s]",m_pStkInfo->Code,m_pStkInfo->Name,GetTdxTimeStr(m_nSyncMin));
	g_d.SelectFont(pDC,INFO_FONT);
	YFrom+=21;
	g_d.DrawLine(pDC,0,YFrom,m_rcIn.right,YFrom);
	YFrom+=2;
	
	if(!m_bLowSize)
	{
		int mmpnum=5;
		if(!g_GH.bPillarBSChart)		//显示数值型买卖盘
		{
			if(g_GH.nShowJUNJ==0)
			{
				g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("委比"));
				g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("委差"));
				if(!m_bEmpty)
				{
					g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.Wtb,0.0,2,XWidth,1);
					g_d.DisplayText(pDC,XFrom4,YFrom,(m_HqContent.Wlc<0?VipColor.DOWNCOLOR:VipColor.KPCOLOR),XWidth,MakeVol3(m_HqContent.Wlc));
				}
				YFrom += HQ_TEXTH;
				g_d.DrawLine(pDC,0,YFrom-1,m_rcIn.right,YFrom-1);
			}		
			YFrom += 2;
			BOOL bBspChg = memcmp(&(m_HqContent.Buyprice[0]),&(m_HqContentBak.Buyprice[0]),10*sizeof(float)+10*sizeof(DWORD));
			if(bBspChg)
			{
				memcpy(&m_HqContentShow,&m_HqContentBak,sizeof(UHqContent));
				memcpy(&m_HqContentBak,&m_HqContent,sizeof(UHqContent));
			}
			//讨论几种情况
			//1.集合竞价之后,不需要显示买卖盘变动
			//2.切换股票时,不需要显示买卖盘变动
			//3.涨停或者跌停,需要显示变动
			for(i = 0;i <mmpnum;i++)
			{
				g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(buysellstr[0][mmpnum-1-i]));
				if(!m_bEmpty)
				{
					if(m_HqContent.Sellprice[mmpnum-1-i]>COMPPREC)
					{
						g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.Sellprice[mmpnum-1-i],m_HqContent.Close,m_XsFlag,XWidth);
						m_SellRect[mmpnum-1-i].SetRect(XFrom1,YFrom+2,XFrom2+XWidth,YFrom+16);
					}
					if(m_HqContent.Sellvol[mmpnum-1-i]>COMPPREC)
					{
						if(g_GH.bShowMmpChg)
						{
							if(m_HqContent.Sellprice[mmpnum-1-i]>COMPPREC)		//当前价位非0
							{
								for(j=0;j<mmpnum;j++)
									if(fabs(m_HqContentBak.Sellprice[mmpnum-1-i]-m_HqContentShow.Sellprice[j])<COMPPREC)	
										break;
								if(m_bChangeStk || j==mmpnum
									|| fabs(m_HqContentShow.Sellprice[0]-m_HqContentShow.Buyprice[0])<COMPPREC)		//找不到对应价位,或者上一笔集合竞价
								{
									if(m_bChangeStk)	//集中撮合
										g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HqContent.Sellvol[mmpnum-1-i]);
									else if(fabs(m_HqContentShow.Sellprice[0]-m_HqContentShow.Buyprice[0])<COMPPREC && (fabs(m_HqContent.Sellprice[0]-m_HqContent.Buyprice[0])>COMPPREC)) //集合竞价最后一笔
										g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HqContent.Sellvol[mmpnum-1-i]);
									else
									{
										g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HqContent.Sellvol[mmpnum-1-i]);
										if(fabs(m_HqContent.Sellprice[0]-m_HqContent.Buyprice[0])>COMPPREC)			//集合竞价期间不显示成交变动
										{
											oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
											g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.KPCOLOR,XWidth,"+%u",m_HqContentBak.Sellvol[mmpnum-1-i]);
											g_d.RestoreFont(pDC,oldfont);
										}
									}
								}
								else if(m_HqContentBak.Sellvol[mmpnum-1-i]==m_HqContentShow.Sellvol[j])
									g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HqContent.Sellvol[mmpnum-1-i]);
								else 
								{
									g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HqContent.Sellvol[mmpnum-1-i]);
									oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
									if(m_HqContentBak.Sellvol[mmpnum-1-i]>m_HqContentShow.Sellvol[j])	//量增加
										g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.KPCOLOR,XWidth,"+%u",m_HqContentBak.Sellvol[mmpnum-1-i]-m_HqContentShow.Sellvol[j]);
									else
										g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.DOWNCOLOR,XWidth,"-%u",m_HqContentShow.Sellvol[j]-m_HqContentBak.Sellvol[mmpnum-1-i]);
									g_d.RestoreFont(pDC,oldfont);
								}
								memcpy(&m_HqContentBak,&m_HqContent,sizeof(UHqContent));
							}
							else 
								g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HqContent.Sellvol[mmpnum-1-i]);
						}
						else 
							g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HqContent.Sellvol[mmpnum-1-i]);
					}
				}
				YFrom+=HQ_TEXTH;
			}
			g_d.DrawLine(pDC,0,YFrom-1,m_rcIn.right,YFrom-1);
			YFrom += 2;
			for(i = 0;i < mmpnum;i++)
			{
				g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(buysellstr[1][i]));
				if(!m_bEmpty)
				{
					if(m_HqContent.Buyprice[i]>COMPPREC)
					{
						g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.Buyprice[i],m_HqContent.Close,m_XsFlag,XWidth);
						m_BuyRect[i].SetRect(XFrom1,YFrom+2,XFrom2+XWidth,YFrom+16);
					}
					if(m_HqContent.Buyvol[i]>COMPPREC)
					{
						if(g_GH.bShowMmpChg)
						{
							if(m_HqContent.Buyprice[i]>COMPPREC)		//买卖盘有变化,并且当前价位非0
							{
								for(j=0;j<mmpnum;j++)
								{
									if(fabs(m_HqContentBak.Buyprice[i]-m_HqContentShow.Buyprice[j])<COMPPREC)
										break;
								}
								if(m_bChangeStk || j==mmpnum
									|| fabs(m_HqContentShow.Sellprice[0]-m_HqContentShow.Buyprice[0])<COMPPREC)		//切换股票,没有收到对比行情,找不到对应价位,或者集合竞价
								{
									if(m_bChangeStk)	//集中撮合
										g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HqContent.Buyvol[i]);
									else if(fabs(m_HqContentShow.Sellprice[0]-m_HqContentShow.Buyprice[0])<COMPPREC && (fabs(m_HqContent.Sellprice[0]-m_HqContent.Buyprice[0])>COMPPREC))	//集合竞价最后一笔
										g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HqContent.Buyvol[i]);
									else
									{
										g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HqContent.Buyvol[i]);
										if(fabs(m_HqContent.Sellprice[0]-m_HqContent.Buyprice[0])>COMPPREC)			//集合竞价期间不显示成交变动
										{
											oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
											g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.KPCOLOR,XWidth,"+%u",m_HqContentBak.Buyvol[i]);
											g_d.RestoreFont(pDC,oldfont);
										}
									}
								}
								else if(m_HqContentBak.Buyvol[i]==m_HqContentShow.Buyvol[j])
									g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HqContent.Buyvol[i]);
								else 
								{
									g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HqContent.Buyvol[i]);
									oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
									if(m_HqContentBak.Buyvol[i]>m_HqContentShow.Buyvol[j])	//量增加 
										g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.KPCOLOR,XWidth,"+%u",m_HqContentBak.Buyvol[i]-m_HqContentShow.Buyvol[j]);
									else
										g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.DOWNCOLOR,XWidth,"-%u",m_HqContentShow.Buyvol[j]-m_HqContentBak.Buyvol[i]);
									g_d.RestoreFont(pDC,oldfont);
								}
								memcpy(&m_HqContentBak,&m_HqContent,sizeof(UHqContent));
							}
							else 
								g_d.DisplayText(pDC,XFrom4*2/3,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HqContent.Buyvol[i]);
						}
						else 
							g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HqContent.Buyvol[i]);
					}
				}
				YFrom+=HQ_TEXTH;
			}
		}
		else							//显示柱线型买卖盘
		{
			DWORD	MaxVolume = 1,endBarX = 0,totalbarlen = 0;
			for(i = 0;i <5;i++)
			{
				if(m_HqContent.Sellvol[4-i] > MaxVolume)
					MaxVolume = m_HqContent.Sellvol[4-i];
			}
			for(i = 0;i <5;i++)
			{
				if(m_HqContent.Buyvol[i] > MaxVolume)
					MaxVolume = m_HqContent.Buyvol[i];
			}
			totalbarlen = XFrom4+18-50;
			if(MaxVolume > 99999)	totalbarlen = XFrom4-50; //如果是基金大量,totalbarlen变短
			YFrom += 6;
			for(i = 0;i < mmpnum;i++)
			{
				if(!m_bEmpty)
				{
					if(m_HqContent.Sellprice[mmpnum-1-i]>COMPPREC)
					{
						g_d.BjPrintf(pDC,XFrom1,YFrom,m_HqContent.Sellprice[mmpnum-1-i],m_HqContent.Close,m_XsFlag,42);
						m_SellRect[mmpnum-1-i].SetRect(XFrom1,YFrom+2,XFrom1+42,YFrom+16);
					}
					if(m_HqContent.Sellvol[mmpnum-1-i]>COMPPREC)
					{
						endBarX = m_HqContent.Sellvol[mmpnum-1-i]*totalbarlen/MaxVolume+50;
						if(endBarX < 48) endBarX = 48;
						if(m_HqContent.Sellprice[mmpnum-1-i] > m_HqContent.Close)
							g_d.Bar(pDC,50,YFrom+6,endBarX,YFrom+9,KP_BRUSH);
						else
							g_d.Bar(pDC,50,YFrom+6,endBarX,YFrom+9,DOWN_BRUSH);
						g_d.DisplayText(pDC,endBarX+3,YFrom,VipColor.VOLCOLOR,0,"%9u",m_HqContent.Sellvol[mmpnum-1-i]);
					}
				}
				YFrom+=HQ_TEXTH;
			}
			g_d.DrawLine(pDC,0,YFrom+5,m_rcIn.right,YFrom+5);
			YFrom += 9;
			for(i = 0;i < mmpnum;i++)
			{
				if(!m_bEmpty)
				{
					if(m_HqContent.Buyprice[i]>COMPPREC)
					{
						g_d.BjPrintf(pDC,XFrom1,YFrom,m_HqContent.Buyprice[i],m_HqContent.Close,m_XsFlag,42);
						m_BuyRect[i].SetRect(XFrom1,YFrom+2,XFrom1+42,YFrom+16);
					}
					if(m_HqContent.Buyvol[i]>COMPPREC)
					{
						endBarX = m_HqContent.Buyvol[i]*totalbarlen/MaxVolume+50;
						if(endBarX < 48) endBarX = 48;
						if(m_HqContent.Buyprice[i] > m_HqContent.Close)
							g_d.Bar(pDC,50,YFrom+6,endBarX,YFrom+9,KP_BRUSH);
						else
							g_d.Bar(pDC,50,YFrom+6,endBarX,YFrom+9,DOWN_BRUSH);
						g_d.DisplayText(pDC,endBarX+3,YFrom,VipColor.VOLCOLOR,0,"%9u",m_HqContent.Buyvol[i]);
					}
				}
				YFrom+=HQ_TEXTH;
			}
			YFrom += 8;
		}	
		g_d.DrawLine(pDC,0,YFrom-1,m_rcIn.right,YFrom-1);
		YFrom+=2;
	}
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("现价"));
	g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("今开"));
	if(!m_bEmpty)
	{
		g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.Now,m_HqContent.Close,m_XsFlag,XWidth);
		m_NowRect.SetRect(XFrom2,YFrom+2,XFrom2+XWidth,YFrom+16);
		g_d.BjPrintf(pDC,XFrom4,YFrom,m_HqContent.Open,m_HqContent.Close,m_XsFlag,XWidth);
	}	
	YFrom += HQ_TEXTH;

	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("涨跌"));
	g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("最高"));
	if(!m_bEmpty)
	{
		g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.Zd,0.0,m_XsFlag,XWidth);
		g_d.BjPrintf(pDC,XFrom4,YFrom,m_HqContent.Max,m_HqContent.Close,m_XsFlag,XWidth);
	}	
	YFrom += HQ_TEXTH;	

	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("涨幅"));
	g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("最低"));
	if(!m_bEmpty)
	{
		g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.Zaf,0.0,2,XWidth,1);
		g_d.BjPrintf(pDC,XFrom4,YFrom,m_HqContent.Min,m_HqContent.Close,m_XsFlag,XWidth);
	}
	if(m_bLowSize || (g_GH.nShowJUNJ>0 && !g_GH.bPillarBSChart))
	{
		YFrom += HQ_TEXTH;			
		if(g_GH.nShowJUNJ==2)
		{
			g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("振幅"));
			g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("金额"));
			if(!m_bEmpty)
			{
				g_d.NoBjPrintf(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,m_HqContent.Zef,2,XWidth,1);
				g_d.DisplayText(pDC,XFrom4,YFrom,GridColor.Grid_Other_Color,XWidth,MakeJE(m_HqContent.Amount));
			}
		}
		else
		{
			g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("现量"));
			g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("均价"));
			if(!m_bEmpty)
			{
				if(IsHligtNowVol(m_HqContent.NowVol,m_pStkInfo))
					g_d.DisplayText(pDC,XFrom2,YFrom,RGB(192,0,192),XWidth,MakeVol3(m_HqContent.NowVol));
				else 
					g_d.DisplayText(pDC,XFrom2,YFrom,(m_Hq.Flag.InOutFlag==0)?(GridColor.Grid_Up_Color):((m_Hq.Flag.InOutFlag==1)?GridColor.Grid_Down_Color:GridColor.Grid_Level_Color),XWidth,MakeVol3(m_HqContent.NowVol));
				g_d.BjPrintf(pDC,XFrom4,YFrom,m_HqContent.Average,m_HqContent.Close,m_XsFlag,XWidth);
			}
		}
	}

	YFrom += HQ_TEXTH;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("总量"));
	g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("量比"));
	if(!m_bEmpty)
	{
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.VOLCOLOR,XWidth,MakeVol3(m_HqContent.Volume));
		g_d.NoBjPrintf(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,m_HqContent.fLB,m_XsFlag,XWidth);
	}
 
	if(m_bLowSize)
	{
		YFrom += HQ_TEXTH;
		if(!m_bQz)
		{
			g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("涨停"));
			g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("跌停"));
			if(!m_bEmpty)
			{
				if(m_HqContent.TPPrice1>COMPPREC)
					g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.TPPrice1,m_HqContent.Close,m_XsFlag,XWidth);
				if(m_HqContent.TPPrice2>COMPPREC)
				g_d.BjPrintf(pDC,XFrom4,YFrom,m_HqContent.TPPrice2,m_HqContent.Close,m_XsFlag,XWidth);
			}
		}
		else
		{
			g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("振幅"));
			g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("金额"));
			if(!m_bEmpty)
			{
				g_d.NoBjPrintf(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,m_HqContent.Zef,2,XWidth,1);
				g_d.DisplayText(pDC,XFrom4,YFrom,GridColor.Grid_Other_Color,XWidth,MakeJE(m_HqContent.Amount));
			}
		}
		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("总笔"));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("每笔"));
		if(!m_bEmpty)
		{
			if(m_HqContent.CJBS>0)
			{
				g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,MakeVol3(m_HqContent.CJBS));
				g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%.1f",1.0*m_HqContent.Volume/m_HqContent.CJBS);
			}
			else
			{
				g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,MEANLESS_STR);
				g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,MEANLESS_STR);
			}			
		}
	}
	YFrom += HQ_TEXTH;
	
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("外盘"));
	g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("内盘"));
	if(!m_bEmpty)
	{
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.KPCOLOR,XWidth,MakeVol3(m_HqContent.Outside));
		g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.DOWNCOLOR,XWidth,MakeVol3(m_HqContent.Inside));
	}	
	YFrom += HQ_TEXTH;
	
	g_d.DrawLine(pDC,0,YFrom-1,m_rcIn.right,YFrom-1);
	YFrom += 2;
	if(m_bGz || m_bQyzq)
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("应计利息"));
		if(!m_bEmpty && m_HqContent.Yjlx>COMPPREC)
			g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.3f",m_HqContent.Yjlx);
		if(m_Domain==SZ)
		{
			YFrom += HQ_TEXTH;
			g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("结算价全价"));
			if(!m_bEmpty && m_HqContent.Yjlx>COMPPREC)
				g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.2f",(m_HqContent.Now>COMPPREC?m_HqContent.Now:m_HqContent.Close)+m_HqContent.Yjlx);
		}
		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,(m_Domain==SZ)?(_F("到期收益率")):(_F("结算价全价")));
		if(!m_bEmpty && m_HqContent.Yjlx>COMPPREC)
			g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.2f",(m_Domain==SZ)?(m_HqContent.Dqsy):((m_HqContent.Now>COMPPREC?m_HqContent.Now:m_HqContent.Close)+m_HqContent.Yjlx));
	}
	else if(m_bETF)
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("IOPV"));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("昨PV"));
		if(m_HqContent.Jjjz>COMPPREC)
			g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.Jjjz,m_HqContent.LastPV,m_XsFlag,XWidth);
		if(m_HqContent.LastPV>COMPPREC)
			g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.3f",m_HqContent.LastPV);

		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("PV涨"));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("PV幅"));
		if(m_HqContent.Jjjz>COMPPREC && m_HqContent.LastPV>COMPPREC)
		{
			g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.Jjjz-m_HqContent.LastPV,0.0,m_XsFlag,XWidth);
			g_d.BjPrintf(pDC,XFrom4,YFrom,(m_HqContent.Jjjz-m_HqContent.LastPV)*100/m_HqContent.LastPV,0.0,2,XWidth,1);
		}
		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("溢价"));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("溢率"));
		if(m_HqContent.Jjjz>COMPPREC && m_HqContent.Now>COMPPREC)
		{
			g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.Now-m_HqContent.Jjjz,0.0,3,XWidth);			
			g_d.BjPrintf(pDC,XFrom4,YFrom,(m_HqContent.Now-m_HqContent.Jjjz)*100/m_HqContent.Jjjz,0.0,2,XWidth,1);
		}
	}
	else if(m_bSZKFSJJ)
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("股本"));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("换手"));
		if(m_HqContent.Zgb>COMPPREC)
			g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,MakeGB(m_HqContent.Zgb));
		if(m_HqContent.Hsl>COMPPREC)
			g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth+1,"%9.2f%%",m_HqContent.Hsl);			

		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("时点单位净值"));
		if(m_HqContent.Jjjz>COMPPREC)
			g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.4f",m_HqContent.Jjjz);

		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("昨日单位净值"));
		if(m_HqContent.Yjlx>COMPPREC)
			g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.4f",m_HqContent.Yjlx/100.0);
	}
	else if(m_bQz)
	{	
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("涨停"));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("跌停"));
		if(m_HqContent.Close>COMPPREC && m_HqContent.TPPrice1>COMPPREC)
			g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.TPPrice1,m_HqContent.Close,m_XsFlag,XWidth);
		if(m_HqContent.Close>COMPPREC && m_HqContent.TPPrice2>COMPPREC)
			g_d.BjPrintf(pDC,XFrom4,YFrom,m_HqContent.TPPrice2,m_HqContent.Close,m_XsFlag,XWidth);
		
		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("换手"));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("杠杆"));
		if(!m_bEmpty)
			g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,"%9.2f%%",m_HqContent.Hsl);
		g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth+1,"%9.2f",m_HqContent.fGgbl);

		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("价值"));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("溢价"));
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,"%9.3f",m_HqContent.fInnerVal);
		g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.2f%%",m_HqContent.fYjl);
	}
	else
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("换手"));
		if(!m_bEmpty)
			g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,"%9.2f%%",m_HqContent.Hsl);
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("股本"));
		g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth+1,MakeGB(m_HqContent.Zgb));

		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,m_bCloseJJ?_F("净值"):_F("净资"));
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,"%9.2f",m_HqContent.Mg_Jz);
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("流通"));
		g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth+1,MakeGB(m_HqContent.ActiveCapital));

		YFrom += HQ_TEXTH;
		char strFlag[32];
		switch((int)m_pStkInfo->J_HalfYearFlag)
		{
		case 3:
			strcpy(strFlag, "收益①");
			break;
		case 6:
			strcpy(strFlag, "收益②");
			break;
		case 9:
			strcpy(strFlag, "收益③");
			break;
		case 12:
			strcpy(strFlag, "收益④");
			break;
		default:
			strcpy(strFlag, "收益");
			break;
		}
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(strFlag));
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,"%9.3f",m_HqContent.Mg_Sy);
		if(!m_bCloseJJ)
		{
			g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("市盈"));
			if(m_HqContent.Syl > 0.0)
				g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.1f",m_HqContent.Syl);
			else if(!m_bEmpty)
				g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth-4,MEANLESS_STR);
		}
		else
		{
			g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("溢价"));		
			if(m_HqContent.Mg_Jz>COMPPREC)
			{
				float tmpFloat=m_HqContent.Now;
				if(tmpFloat<COMPPREC) tmpFloat=m_HqContent.Close;
				g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.1f%%",(tmpFloat-m_HqContent.Mg_Jz)*100.0/m_HqContent.Mg_Jz);
			}
		}
	}
	g_d.DrawLine(pDC,0,m_rcIn.bottom-1,m_rcIn.right,m_rcIn.bottom-1);
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void UHq::DrawDSView(CDC*pDC)
{
	if(Height()<20||m_pStkInfo==NULL) return;
	for(int j=0;j<5;j++)
	{
		m_BuyRect[j].SetRectEmpty();
		m_SellRect[j].SetRectEmpty();
	}
	m_NowRect.SetRectEmpty();
	int nBigMarket = m_pStkInfo->bigmarket;
	int nMarket = m_pStkInfo->setcode;
	g_pStockIO->DSGetCurrData(m_pStkInfo,&m_Hq,&m_HqEx);

	BOOL bNeedCalcRect=FALSE;
	if(nBigMarket==QH_TYPE && (strlen(m_pStkInfo->Code)==6 || strlen(m_pStkInfo->Code)==5))
		bNeedCalcRect=TRUE;
	if(m_nLinkGPNum>0) 
	{
		m_LinkIconRect.SetRect(2,4,2+16,4+16);
		pDC->DrawIcon(2,4,m_hIcon_Pu[4]);	
	}
	int YFrom=6,XFrom=HQ_XSPACE;
	COLORREF cn = RGB(255,255,0);
	CSize tmpSize=g_d.DisplayText(pDC,20,YFrom,cn,"%s ",m_pStkInfo->Code);
	CSize zs = g_d.DisplayText(pDC,20+tmpSize.cx,YFrom,cn,"%s",m_pStkInfo->Name);
	DrawMarkFlag(m_pStkInfo,pDC,20+tmpSize.cx+zs.cx,YFrom-3,left,top,m_pView);
	YFrom+=22;
	g_d.DrawLine(pDC,0,YFrom,m_rcIn.right,YFrom);
	YFrom+=3;
	
	long i,wlc=0,wl=0;
	if(nBigMarket==QH_TYPE)
	{
		wlc=(long)m_Hq.Buyv[0]-(long)m_Hq.Sellv[0];
		wl=(long)m_Hq.Buyv[0]+(long)m_Hq.Sellv[0];
	}
	else
	{
		for(i=0;i<5;++i)
		{
			wlc = wlc+(long)m_Hq.Buyv[i]-(long)m_Hq.Sellv[i];
			wl = wl +(long)m_Hq.Buyv[i]+(long)m_Hq.Sellv[i];
		}
	}
	//涨幅和涨跌
	float	TmpFloat=0;
	memset(&TmpFloat,0xF8,sizeof(float));
	float	Zf=TmpFloat,Zef=TmpFloat;
	if(m_Hq.Close>COMPPREC && m_Hq.Now>COMPPREC)
	{
		Zf = 100.0f*(m_Hq.Now-m_Hq.Close)/m_Hq.Close;
		if(m_Hq.Max>COMPPREC&&m_Hq.Min>COMPPREC) 
			Zef = 100.0f*(m_Hq.Max-m_Hq.Min)/m_Hq.Close;
	}
	else if(m_Hq.Open>COMPPREC && m_Hq.Now>COMPPREC)
	{
		Zf = 100.0f*(m_Hq.Now-m_Hq.Open)/m_Hq.Open;
		if(m_Hq.Max>COMPPREC&&m_Hq.Min>COMPPREC) 
			Zef = 100.0f*(m_Hq.Max-m_Hq.Min)/m_Hq.Open;
	}
	//均价
	float Jj = m_HqEx.AverPrice;
	if(Jj<COMPPREC)
	{
		if(m_Hq.Volume>0)	Jj = m_Hq.Amount/m_Hq.Volume/100.0f;
		if(Jj>m_Hq.Max||Jj<m_Hq.Min) Jj = m_Hq.Close;
	}
	//量比
	float fLB = 0.0f;
	int ii = GetTdxMinute(TimeSec);
	if (ii > 0 && m_HqEx.Gearing > COMPPREC)
		fLB = (float)m_Hq.Volume/ii/m_HqEx.Gearing;
	
	if(nBigMarket==QH_TYPE)
	{
		HQITEM	aQhItem[]=
		{
			{_BA_Ratio,HQ_PLUS_FLOAT_PER,0,0,m_Hq.Now>COMPPREC?(wl!=0?100.0f*wlc/wl:0):TmpFloat,NULL},
			{_BA_Diff,HQ_VOL_LONG,1,m_Hq.Now>COMPPREC?wlc:0xF8F8F8F8,0,NULL},			
			{"",HQ_LINE,1,0,0,NULL},

			{_Ask_1,HQ_PRICE_SEAT,0,0,(m_Hq.Sellp[0]>COMPPREC&&m_Hq.Sellv[0]>0)?m_Hq.Sellp[0]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Sellv[0]>0?m_Hq.Sellv[0]:0xF8F8F8F8,0,NULL},
			{_Bid_1,HQ_PRICE_SEAT,0,0,(m_Hq.Buyp[0]>COMPPREC&&m_Hq.Buyv[0]>0)?m_Hq.Buyp[0]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Buyv[0]>0?m_Hq.Buyv[0]:0xF8F8F8F8,0,NULL},
			{"",HQ_LINE,1,0,0,NULL},
			
			{_New,HQ_BJ_FLOAT,0,0,m_Hq.Now>COMPPREC?m_Hq.Now:TmpFloat,NULL},
			{_Open,HQ_BJ_FLOAT,1,0,m_Hq.Open>COMPPREC?m_Hq.Open:TmpFloat,NULL},
			{_Change,HQ_PLUS_FLOAT,0,0,m_Hq.Now>COMPPREC?(m_Hq.Now-m_Hq.Close):TmpFloat,NULL},
			{_High,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC&&m_Hq.Max>COMPPREC?m_Hq.Max:TmpFloat,NULL},
			{_Change_Ratio,HQ_PLUS_FLOAT_PER,0,0,Zf,NULL},
			{_Low,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC&&m_Hq.Min>COMPPREC?m_Hq.Min:TmpFloat,NULL},			
			{_Cur_Volume,HQ_VOL_LONG,0,m_Hq.Now>COMPPREC?m_Hq.NowVol:0xF8F8F8F8,0,NULL},
			{_Prev_Settle,HQ_BJ_FLOAT,1,0,m_HqEx.PreClear>COMPPREC?m_HqEx.PreClear:TmpFloat,NULL},
			{_Total_Volume,HQ_VOL_PLONG,0,m_Hq.Now>COMPPREC?m_Hq.Volume:0xF8F8F8F8,0,NULL},
			{_Average,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC&&Jj>COMPPREC?Jj:TmpFloat,NULL},
			{_Contract,HQ_VOL_PLONG,0,m_Hq.Now>COMPPREC?m_Hq.Yield_VolInStock.VolInStock:0xF8F8F8F8,0,NULL},
			{_Contract_Diff,HQ_VOL_PLONG,1,m_Hq.Now>COMPPREC?((long)m_Hq.Yield_VolInStock.VolInStock-(long)m_HqEx.PreVolIn):0xF8F8F8F8,0,NULL},
			{_Swing,HQ_FLOAT_PER,0,0,Zef,NULL},
			{_NowIncVol,HQ_VOL_PLONG,1,m_Hq.Now>COMPPREC?m_Hq.VolInStockDiff:TmpFloat,0,NULL},
			{"",HQ_LINE,1,0,0,NULL},

			{_OpenContract,HQ_VOL_PLONG,0,m_Hq.Now>COMPPREC?m_Hq.OpenVolume:0xF8F8F8F8,0,NULL},
			{_CloseContract,HQ_VOL_PLONG,1,m_Hq.Now>COMPPREC?(m_Hq.Volume-m_Hq.OpenVolume):0xF8F8F8F8,0,NULL},
			{_LongOpen,HQ_VOL_PLONG,0,m_HqEx.OutOpen,0,NULL},
			{_LongClose,HQ_VOL_PLONG,1,m_HqEx.OutClose,0,NULL},
			{_ShortOpen,HQ_VOL_PLONG,0,m_HqEx.InOpen,0,NULL},
			{_ShortClose,HQ_VOL_PLONG,1,m_HqEx.InClose,0,NULL},
			{"",HQ_LINE,1,0,0,NULL},

			{_Outside,HQ_RED_LONG,0,m_Hq.Outside>COMPPREC?m_Hq.Outside:0xF8F8F8F8,0,NULL},
			{_Inside,HQ_GREEN_LONG,1,m_Hq.Inside>COMPPREC?m_Hq.Inside:0xF8F8F8F8,0,NULL},
			{"",HQ_LINE,1,0,0,NULL},

			{_Market,HQ_NULL,0,0,0,NULL},
			{"",HQ_STRING,1,0,0,g_pStockIO->GetMarketInfo(m_pStkInfo)},
			{"",HQ_LINE,1,0,0,NULL},
		};
		short	nQhItem = sizeof(aQhItem)/sizeof(HQITEM);
		DrawHqItem(pDC,Width()-1,Height(),YFrom,nQhItem,aQhItem,m_XsFlag,m_Hq.Close,22,6);
		//得到位置
		if(bNeedCalcRect)
		{
			int tmpWidth=Width()/2-2*HQ_XSPACE+1;
			int tmpTop=YFrom+22+6;
			m_SellRect[0].SetRect(HQ_XSPACE,tmpTop,HQ_XSPACE+tmpWidth,tmpTop+HQ_TEXTH-1);
			tmpTop += 22+6;
			m_BuyRect[0].SetRect(HQ_XSPACE,tmpTop,HQ_XSPACE+tmpWidth,tmpTop+HQ_TEXTH-1);
			tmpTop += 22+6;
			m_NowRect.SetRect(HQ_XSPACE,tmpTop,HQ_XSPACE+tmpWidth,tmpTop+HQ_TEXTH-1);
		}
	}
	else	if(nBigMarket==QH_TYPE)		//特殊处理股指期货
	{
		HQITEM	aQhItem2[]=
		{
			{_Ask_5,HQ_PRICE_SEAT,0,0,(m_Hq.Sellp[4]>COMPPREC&&m_Hq.Sellv[4]>0)?m_Hq.Sellp[4]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Sellv[4]>0?m_Hq.Sellv[4]:0xF8F8F8F8,0,NULL},
			{_Ask_4,HQ_PRICE_SEAT,0,0,(m_Hq.Sellp[3]>COMPPREC&&m_Hq.Sellv[3]>0)?m_Hq.Sellp[3]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Sellv[3]>0?m_Hq.Sellv[3]:0xF8F8F8F8,0,NULL},
			{_Ask_3,HQ_PRICE_SEAT,0,0,(m_Hq.Sellp[2]>COMPPREC&&m_Hq.Sellv[2]>0)?m_Hq.Sellp[2]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Sellv[2]>0?m_Hq.Sellv[2]:0xF8F8F8F8,0,NULL},
			{_Ask_2,HQ_PRICE_SEAT,0,0,(m_Hq.Sellp[1]>COMPPREC&&m_Hq.Sellv[1]>0)?m_Hq.Sellp[1]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Sellv[1]>0?m_Hq.Sellv[1]:0xF8F8F8F8,0,NULL},
			{_Ask_1,HQ_PRICE_SEAT,0,0,(m_Hq.Sellp[0]>COMPPREC&&m_Hq.Sellv[0]>0)?m_Hq.Sellp[0]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Sellv[0]>0?m_Hq.Sellv[0]:0xF8F8F8F8,0,NULL},
			{"",HQ_LINE,1,0,0,NULL},

			{_Bid_1,HQ_PRICE_SEAT,0,0,(m_Hq.Buyp[0]>COMPPREC&&m_Hq.Buyv[0]>0)?m_Hq.Buyp[0]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Buyv[0]>0?m_Hq.Buyv[0]:0xF8F8F8F8,0,NULL},
			{_Bid_2,HQ_PRICE_SEAT,0,0,(m_Hq.Buyp[1]>COMPPREC&&m_Hq.Buyv[1]>0)?m_Hq.Buyp[1]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Buyv[1]>0?m_Hq.Buyv[1]:0xF8F8F8F8,0,NULL},
			{_Bid_3,HQ_PRICE_SEAT,0,0,(m_Hq.Buyp[2]>COMPPREC&&m_Hq.Buyv[2]>0)?m_Hq.Buyp[2]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Buyv[2]>0?m_Hq.Buyv[2]:0xF8F8F8F8,0,NULL},
			{_Bid_4,HQ_PRICE_SEAT,0,0,(m_Hq.Buyp[3]>COMPPREC&&m_Hq.Buyv[3]>0)?m_Hq.Buyp[3]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Buyv[3]>0?m_Hq.Buyv[3]:0xF8F8F8F8,0,NULL},
			{_Bid_5,HQ_PRICE_SEAT,0,0,(m_Hq.Buyp[4]>COMPPREC&&m_Hq.Buyv[4]>0)?m_Hq.Buyp[4]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Buyv[4]>0?m_Hq.Buyv[4]:0xF8F8F8F8,0,NULL},
			{"",HQ_LINE,1,0,0,NULL},
			
			{_New,HQ_BJ_FLOAT,0,0,m_Hq.Now>COMPPREC?m_Hq.Now:TmpFloat,NULL},
			{_Open,HQ_BJ_FLOAT,1,0,m_Hq.Open>COMPPREC?m_Hq.Open:TmpFloat,NULL},
			{_Change,HQ_PLUS_FLOAT,0,0,m_Hq.Now>COMPPREC?(m_Hq.Now-m_Hq.Close):TmpFloat,NULL},
			{_High,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC&&m_Hq.Max>COMPPREC?m_Hq.Max:TmpFloat,NULL},
			{_Change_Ratio,HQ_PLUS_FLOAT_PER,0,0,Zf,NULL},
			{_Low,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC&&m_Hq.Min>COMPPREC?m_Hq.Min:TmpFloat,NULL},			
			{_Settle,HQ_BJ_FLOAT,0,0,m_Hq.ClearPrice>COMPPREC?m_Hq.ClearPrice:TmpFloat,NULL},
			{_Prev_Settle,HQ_BJ_FLOAT,1,0,m_HqEx.PreClear>COMPPREC?m_HqEx.PreClear:TmpFloat,NULL},			

			{_Total_Volume,HQ_VOL_PLONG,0,m_Hq.Now>COMPPREC?m_Hq.Volume:0xF8F8F8F8,0,NULL},
			{_Average,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC&&Jj>COMPPREC?Jj:TmpFloat,NULL},
			{_Contract,HQ_VOL_PLONG,0,m_Hq.Now>COMPPREC?m_Hq.Yield_VolInStock.VolInStock:0xF8F8F8F8,0,NULL},
			{_Contract_Diff,HQ_VOL_PLONG,1,m_Hq.Now>COMPPREC?((long)m_Hq.Yield_VolInStock.VolInStock-(long)m_HqEx.PreVolIn):0xF8F8F8F8,0,NULL},
			{"",HQ_LINE,1,0,0,NULL},

			{_OpenContract,HQ_VOL_PLONG,0,m_Hq.Now>COMPPREC?m_Hq.OpenVolume:0xF8F8F8F8,0,NULL},
			{_CloseContract,HQ_VOL_PLONG,1,m_Hq.Now>COMPPREC?(m_Hq.Volume-m_Hq.OpenVolume):0xF8F8F8F8,0,NULL},
			{_Outside,HQ_RED_LONG,0,m_Hq.Outside>COMPPREC?m_Hq.Outside:0xF8F8F8F8,0,NULL},
			{_Inside,HQ_GREEN_LONG,1,m_Hq.Inside>COMPPREC?m_Hq.Inside:0xF8F8F8F8,0,NULL},
			{"",HQ_LINE,1,0,0,NULL},
		};
		short	nQhItem2 = sizeof(aQhItem2)/sizeof(HQITEM);
		DrawHqItem(pDC,Width()-1,Height(),YFrom,nQhItem2,aQhItem2,m_XsFlag,m_Hq.Close,HQ_TEXTH,6);
		//得到位置
		if(bNeedCalcRect)
		{
			int tmpWidth=Width()/2-2*HQ_XSPACE+1;
			int tmpTop=YFrom;
			for(i=4;i>=0;i--)
			{
				m_SellRect[i].SetRect(HQ_XSPACE,tmpTop,HQ_XSPACE+tmpWidth,tmpTop+HQ_TEXTH-1);
				tmpTop+=HQ_TEXTH-1;
			}
			tmpTop += 6;
			for(i=0;i<5;i++)
			{
				m_BuyRect[i].SetRect(HQ_XSPACE,tmpTop,HQ_XSPACE+tmpWidth,tmpTop+HQ_TEXTH-1);
				tmpTop+=HQ_TEXTH-1;
			}
			tmpTop += 6;
			m_NowRect.SetRect(HQ_XSPACE,tmpTop,HQ_XSPACE+tmpWidth,tmpTop+HQ_TEXTH-1);
		}
	}
	else	if(nBigMarket==FOREGP_TYPE && nMarket==38)	//特殊处理上海黄金
	{
		HQITEM	IdxItem[]=
		{
			{_BA_Ratio,HQ_PLUS_FLOAT_PER,0,0,m_Hq.Now>COMPPREC?(wl!=0?100.0f*wlc/wl:0):TmpFloat,NULL},
			{_BA_Diff,HQ_VOL_LONG,1,m_Hq.Now>COMPPREC?wlc:0xF8F8F8F8,0,NULL},
			{"",HQ_LINE,1,0,0,NULL},

			{_Ask_3,HQ_PRICE_SEAT,0,0,(m_Hq.Sellp[2]>COMPPREC&&m_Hq.Sellv[2]>0)?m_Hq.Sellp[2]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Sellv[2]>0?m_Hq.Sellv[2]:0xF8F8F8F8,0,NULL},
			{_Ask_2,HQ_PRICE_SEAT,0,0,(m_Hq.Sellp[1]>COMPPREC&&m_Hq.Sellv[1]>0)?m_Hq.Sellp[1]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Sellv[1]>0?m_Hq.Sellv[1]:0xF8F8F8F8,0,NULL},
			{_Ask_1,HQ_PRICE_SEAT,0,0,(m_Hq.Sellp[0]>COMPPREC&&m_Hq.Sellv[0]>0)?m_Hq.Sellp[0]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Sellv[0]>0?m_Hq.Sellv[0]:0xF8F8F8F8,0,NULL},
			{"",HQ_LINE,1,0,0,NULL},

			{_Bid_1,HQ_PRICE_SEAT,0,0,(m_Hq.Buyp[0]>COMPPREC&&m_Hq.Buyv[0]>0)?m_Hq.Buyp[0]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Buyv[0]>0?m_Hq.Buyv[0]:0xF8F8F8F8,0,NULL},
			{_Bid_2,HQ_PRICE_SEAT,0,0,(m_Hq.Buyp[1]>COMPPREC&&m_Hq.Buyv[1]>0)?m_Hq.Buyp[1]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Buyv[1]>0?m_Hq.Buyv[1]:0xF8F8F8F8,0,NULL},
			{_Bid_3,HQ_PRICE_SEAT,0,0,(m_Hq.Buyp[2]>COMPPREC&&m_Hq.Buyv[2]>0)?m_Hq.Buyp[2]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Buyv[2]>0?m_Hq.Buyv[2]:0xF8F8F8F8,0,NULL},
			{"",HQ_LINE,1,0,0,NULL},

			{_Nominal,HQ_BJ_FLOAT,0,0,m_Hq.Now>COMPPREC?m_Hq.Now:TmpFloat,NULL},
			{_Open,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Open:TmpFloat,NULL},
			{_Change,HQ_PLUS_FLOAT,0,0,m_Hq.Now>COMPPREC?(m_Hq.Now-m_Hq.Close):TmpFloat,NULL},
			{_High,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Max:TmpFloat,NULL},			
			{_Change_Ratio,HQ_PLUS_FLOAT_PER,0,0,Zf,NULL},
			{_Low,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Min:TmpFloat,NULL},			
			{_Total_Volume,HQ_VOL_LONG,0,m_Hq.Now>COMPPREC?m_Hq.Volume:0xF8F8F8F8,0,NULL},
			{_Cur_Volume,HQ_VOL_LONG,1,m_Hq.Now>COMPPREC?m_Hq.NowVol:0xF8F8F8F8,0,NULL},
			{_Total_Amount,HQ_AMOUNT_FLOAT,0,0,m_Hq.Now>COMPPREC?m_Hq.Amount:TmpFloat,NULL},
			{_Aver_Price,HQ_BJ_FLOAT,1,0,Jj>COMPPREC?Jj:TmpFloat,NULL},
			{_VOL_COMPRA,HQ_PLUS_FLOAT_PER,0,0,fLB>COMPPREC?fLB:TmpFloat,NULL},
			{_Swing,HQ_PLUS_FLOAT_PER,1,0,Zef,NULL},
			{"",HQ_LINE,1,0,0,NULL},

			{_Outside,HQ_RED_LONG,0,m_Hq.Outside>COMPPREC?m_Hq.Outside:0xF8F8F8F8,0,NULL},
			{_Inside,HQ_GREEN_LONG,1,m_Hq.Inside>COMPPREC?m_Hq.Inside:0xF8F8F8F8,0,NULL},
			{"",HQ_LINE,1,0,0,NULL},
		};
		short	ItemNum = sizeof(IdxItem)/sizeof(HQITEM);
		DrawHqItem(pDC,Width()-1,Height(),YFrom,ItemNum,IdxItem,m_XsFlag,m_Hq.Close,23);
	}
	else	if(nBigMarket==IDX_TYPE || testzs(m_pStkInfo) || nBigMarket==FOREGP_TYPE)
	{
		HQITEM	IdxItem[]=
		{
			{_Nominal,HQ_NULL,0,0,0,NULL},
			{"",HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Now:TmpFloat,NULL},			
			{_Change,HQ_NULL,0,0,0,NULL},
			{"",HQ_PLUS_FLOAT,1,0,m_Hq.Now>COMPPREC?(m_Hq.Now-m_Hq.Close):TmpFloat,NULL},
			{_Change_Ratio,HQ_NULL,0,0,0,NULL},
			{"",HQ_PLUS_FLOAT_PER,1,0,Zf,NULL},
			{_Swing,HQ_NULL,0,0,0,NULL},
			{"",HQ_PLUS_FLOAT_PER,1,0,Zef,NULL},			
			{"",HQ_LINE,1,0,0,NULL},

			{_Prev_Close,HQ_NULL,0,0,0,NULL},
			{"",HQ_BJ_FLOAT,1,0,m_Hq.Close,NULL},
			{_Open,HQ_NULL,0,0,0,NULL},
			{"",HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Open:TmpFloat,NULL},			
			{_High,HQ_NULL,0,0,0,NULL},
			{"",HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Max:TmpFloat,NULL},			
			{_Low,HQ_NULL,0,0,0,NULL},
			{"",HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Min:TmpFloat,NULL},

			{_Turnover,HQ_NULL,0,0,0,NULL},
			{"",HQ_AMOUNT_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Amount:TmpFloat,NULL},
			{"",HQ_LINE,1,0,0,NULL},

			{_Market,HQ_NULL,0,0,0,NULL},
			{"",HQ_STRING,1,0,0,g_pStockIO->GetMarketInfo(m_pStkInfo)},
			{"",HQ_LINE,1,0,0,NULL},
		};
		short	ItemNum = sizeof(IdxItem)/sizeof(HQITEM);
		DrawHqItem(pDC,Width()-1,Height(),YFrom,ItemNum,IdxItem,m_XsFlag,m_Hq.Close,34);
	}
	else	if(nBigMarket==FE_TYPE)
	{
		HQITEM	FeItem[]=
		{
			{_Nominal,HQ_NULL,0,0,0,NULL},
			{"",HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Now:TmpFloat,NULL},			
			{_Change,HQ_NULL,0,0,0,NULL},
			{"",HQ_PLUS_FLOAT,1,0,m_Hq.Now>COMPPREC?(m_Hq.Now-m_Hq.Close):TmpFloat,NULL},
			{_Change_Ratio,HQ_NULL,0,0,0,NULL},
			{"",HQ_PLUS_FLOAT_PER,1,0,Zf,NULL},
			{_Swing,HQ_NULL,0,0,0,NULL},
			{"",HQ_PLUS_FLOAT_PER,1,0,Zef,NULL},			
			{"",HQ_LINE,1,0,0,NULL},

			{_Prev_Close,HQ_NULL,0,0,0,NULL},
			{"",HQ_BJ_FLOAT,1,0,m_Hq.Close,NULL},
			{_Open,HQ_NULL,0,0,0,NULL},
			{"",HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC&&m_Hq.Open>COMPPREC?m_Hq.Open:TmpFloat,NULL},			
			{_High,HQ_NULL,0,0,0,NULL},
			{"",HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC&&m_Hq.Max>COMPPREC?m_Hq.Max:TmpFloat,NULL},			
			{_Low,HQ_NULL,0,0,0,NULL},
			{"",HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC&&m_Hq.Min>COMPPREC?m_Hq.Min:TmpFloat,NULL},
			{"",HQ_LINE,1,0,0,NULL},

			{_Bid,HQ_NULL,0,0,0,NULL},
			{"",HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Buyp[0]:TmpFloat,NULL},
			{_Ask,HQ_NULL,0,0,0,NULL},
			{"",HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Sellp[0]:TmpFloat,NULL},
			{"",HQ_LINE,1,0,0,NULL},

			{_Market,HQ_NULL,0,0,0,NULL},
			{"",HQ_STRING,1,0,0,g_pStockIO->GetMarketInfo(m_pStkInfo)},
			{"",HQ_LINE,1,0,0,NULL},
		};
		short	ItemNum = sizeof(FeItem)/sizeof(HQITEM);
		DrawHqItem(pDC,Width()-1,Height(),YFrom,ItemNum,FeItem,m_XsFlag,m_Hq.Close,30);
	}
	else if(nBigMarket==HKGP_TYPE)
	{
		float px = m_HqEx.HK_Dividend/100.0f;
		float yl = m_HqEx.HK_Profit/100.0f;
		float syl = m_Hq.Now/yl;
		float zxl = px/m_Hq.Now*100;
		float sz = m_Hq.Now*m_HqEx.HK_StockSum;
		long dw = m_HqEx.PreVol;
		if(m_Hq.Volume>0)	Jj = m_Hq.Amount/m_Hq.Volume;
		if(g_GC.bIsSSDS)
		{
			HQITEM	HkItem[]=
			{
				{_BA_Ratio,HQ_PLUS_FLOAT_PER,0,0,m_Hq.Now>COMPPREC?(wl!=0?100.0f*wlc/wl:0):TmpFloat,NULL},
				{_BA_Diff,HQ_VOL_LONG,1,m_Hq.Now>COMPPREC?wlc:0xF8F8F8F8,0,NULL},
				{"",HQ_LINE,1,0,0,NULL},

				{_Ask_5,HQ_PRICE_SEAT,0,m_HqEx.SellSeatNum[4],m_Hq.Sellp[4]>COMPPREC?m_Hq.Sellp[4]:TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,m_Hq.Sellv[4]>0?m_Hq.Sellv[4]:0xF4F4F4F4,0,NULL},
				{_Ask_4,HQ_PRICE_SEAT,0,m_HqEx.SellSeatNum[3],m_Hq.Sellp[3]>COMPPREC?m_Hq.Sellp[3]:TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,m_Hq.Sellv[3]>0?m_Hq.Sellv[3]:0xF4F4F4F4,0,NULL},
				{_Ask_3,HQ_PRICE_SEAT,0,m_HqEx.SellSeatNum[2],m_Hq.Sellp[2]>COMPPREC?m_Hq.Sellp[2]:TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,m_Hq.Sellv[2]>0?m_Hq.Sellv[2]:0xF4F4F4F4,0,NULL},
				{_Ask_2,HQ_PRICE_SEAT,0,m_HqEx.SellSeatNum[1],m_Hq.Sellp[1]>COMPPREC?m_Hq.Sellp[1]:TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,m_Hq.Sellv[1]>0?m_Hq.Sellv[1]:0xF4F4F4F4,0,NULL},
				{_Ask_1,HQ_PRICE_SEAT,0,m_HqEx.SellSeatNum[0],m_Hq.Sellp[0]>COMPPREC?m_Hq.Sellp[0]:TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,m_Hq.Sellv[0]>0?m_Hq.Sellv[0]:0xF4F4F4F4,0,NULL},
				{"",HQ_LINE,1,0,0,NULL},

				{_Bid_1,HQ_PRICE_SEAT,0,m_HqEx.BuySeatNum[0],m_Hq.Buyp[0]>COMPPREC?m_Hq.Buyp[0]:TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,m_Hq.Buyv[0]>0?m_Hq.Buyv[0]:0xF4F4F4F4,0,NULL},
				{_Bid_2,HQ_PRICE_SEAT,0,m_HqEx.BuySeatNum[1],m_Hq.Buyp[1]>COMPPREC?m_Hq.Buyp[1]:TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,m_Hq.Buyv[1]>0?m_Hq.Buyv[1]:0xF4F4F4F4,0,NULL},
				{_Bid_3,HQ_PRICE_SEAT,0,m_HqEx.BuySeatNum[2],m_Hq.Buyp[2]>COMPPREC?m_Hq.Buyp[2]:TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,m_Hq.Buyv[2]>0?m_Hq.Buyv[2]:0xF4F4F4F4,0,NULL},
				{_Bid_4,HQ_PRICE_SEAT,0,m_HqEx.BuySeatNum[3],m_Hq.Buyp[3]>COMPPREC?m_Hq.Buyp[3]:TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,m_Hq.Buyv[3]>0?m_Hq.Buyv[3]:0xF4F4F4F4,0,NULL},
				{_Bid_5,HQ_PRICE_SEAT,0,m_HqEx.BuySeatNum[4],m_Hq.Buyp[4]>COMPPREC?m_Hq.Buyp[4]:TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,m_Hq.Buyv[4]>0?m_Hq.Buyv[4]:0xF4F4F4F4,0,NULL},
				{"",HQ_LINE,1,0,0,NULL},

				{_Nominal,HQ_BJ_FLOAT,0,0,m_Hq.Now>COMPPREC?m_Hq.Now:TmpFloat,NULL},
				{_Open,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Open:TmpFloat,NULL},
				{_Change,HQ_PLUS_FLOAT,0,0,m_Hq.Now>COMPPREC?(m_Hq.Now-m_Hq.Close):TmpFloat,NULL},
				{_High,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Max:TmpFloat,NULL},			
				{_Change_Ratio,HQ_PLUS_FLOAT_PER,0,0,Zf,NULL},
				{_Low,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Min:TmpFloat,NULL},			
				{_Total_Volume,HQ_VOL_LONG,0,m_Hq.Now>COMPPREC?m_Hq.Volume:0xF8F8F8F8,0,NULL},
				{_Total_Amount,HQ_AMOUNT_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Amount:TmpFloat,NULL},
				
				{_MrkCap,HQ_AMOUNT_FLOAT,0,0,sz>COMPPREC?sz:TmpFloat,NULL},
				{_Lot_Size,HQ_LONG,1,dw>0?dw:0xF8F8F8F8,0,NULL},
				{_Year_High,HQ_BJ_FLOAT,0,0,m_HqEx.HisHigh>COMPPREC?m_HqEx.HisHigh:TmpFloat,NULL},
				{_Year_Low,HQ_BJ_FLOAT,1,0,(m_HqEx.HisLow>COMPPREC&&m_HqEx.HisLow<10000.0)?m_HqEx.HisLow:TmpFloat,NULL},
				{_Aver_Price,HQ_BJ_FLOAT,0,0,Jj>COMPPREC?Jj:TmpFloat,NULL},
				{_Swing,HQ_FLOAT_PER,1,0,Zef,NULL},
				{"",HQ_LINE,1,0,0,NULL},

				{_Outside,HQ_RED_LONG,0,m_Hq.Outside>COMPPREC?m_Hq.Outside:0xF8F8F8F8,0,NULL},
				{_Inside,HQ_GREEN_LONG,1,m_Hq.Inside>COMPPREC?m_Hq.Inside:0xF8F8F8F8,0,NULL},
				{"",HQ_LINE,1,0,0,NULL},
			};
			short	ItemNum = sizeof(HkItem)/sizeof(HQITEM);
			DrawHqItem(pDC,Width()-1,Height(),YFrom,ItemNum,HkItem,m_XsFlag,m_Hq.Close,HQ_TEXTH,5);
		}
		else
		{
			HQITEM	HkItem[]=
			{
				{_BA_Ratio,HQ_PLUS_FLOAT_PER,0,0,m_Hq.Now>COMPPREC?(wl!=0?100.0f*wlc/wl:0):TmpFloat,NULL},
				{_BA_Diff,HQ_VOL_LONG,1,m_Hq.Now>COMPPREC?wlc:0xF8F8F8F8,0,NULL},
				{"",HQ_LINE,1,0,0,NULL},

				{_Ask_5,HQ_PRICE_SEAT,0,0,TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,0xF4F4F4F4,0,NULL},
				{_Ask_4,HQ_PRICE_SEAT,0,0,TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,0xF4F4F4F4,0,NULL},
				{_Ask_3,HQ_PRICE_SEAT,0,0,TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,0xF4F4F4F4,0,NULL},
				{_Ask_2,HQ_PRICE_SEAT,0,0,TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,0xF4F4F4F4,0,NULL},
				{_Ask_1,HQ_PRICE_SEAT,0,m_HqEx.SellSeatNum[0],m_Hq.Sellp[0]>COMPPREC?m_Hq.Sellp[0]:TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,m_Hq.Sellv[0]>0?m_Hq.Sellv[0]:0xF4F4F4F4,0,NULL},
				{"",HQ_LINE,1,0,0,NULL},

				{_Bid_1,HQ_PRICE_SEAT,0,m_HqEx.BuySeatNum[0],m_Hq.Buyp[0]>COMPPREC?m_Hq.Buyp[0]:TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,m_Hq.Buyv[0]>0?m_Hq.Buyv[0]:0xF4F4F4F4,0,NULL},
				{_Bid_2,HQ_PRICE_SEAT,0,0,TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,0xF4F4F4F4,0,NULL},
				{_Bid_3,HQ_PRICE_SEAT,0,0,TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,0xF4F4F4F4,0,NULL},
				{_Bid_4,HQ_PRICE_SEAT,0,0,TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,0xF4F4F4F4,0,NULL},
				{_Bid_5,HQ_PRICE_SEAT,0,0,TmpFloat,NULL},
				{"",HQ_VOL_LONG,1,0xF4F4F4F4,0,NULL},
				{"",HQ_LINE,1,0,0,NULL},

				{_Nominal,HQ_BJ_FLOAT,0,0,m_Hq.Now>COMPPREC?m_Hq.Now:TmpFloat,NULL},
				{_Open,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Open:TmpFloat,NULL},
				{_Change,HQ_PLUS_FLOAT,0,0,m_Hq.Now>COMPPREC?(m_Hq.Now-m_Hq.Close):TmpFloat,NULL},
				{_High,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Max:TmpFloat,NULL},			
				{_Change_Ratio,HQ_PLUS_FLOAT_PER,0,0,Zf,NULL},
				{_Low,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Min:TmpFloat,NULL},			
				{_Total_Volume,HQ_VOL_LONG,0,m_Hq.Now>COMPPREC?m_Hq.Volume:0xF8F8F8F8,0,NULL},
				{_Total_Amount,HQ_AMOUNT_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Amount:TmpFloat,NULL},
				
				{_MrkCap,HQ_AMOUNT_FLOAT,0,0,sz>COMPPREC?sz:TmpFloat,NULL},
				{_Lot_Size,HQ_LONG,1,dw>0?dw:0xF8F8F8F8,0,NULL},
				{_Year_High,HQ_BJ_FLOAT,0,0,m_HqEx.HisHigh>COMPPREC?m_HqEx.HisHigh:TmpFloat,NULL},
				{_Year_Low,HQ_BJ_FLOAT,1,0,(m_HqEx.HisLow>COMPPREC&&m_HqEx.HisLow<10000.0)?m_HqEx.HisLow:TmpFloat,NULL},
				{_Aver_Price,HQ_BJ_FLOAT,0,0,Jj>COMPPREC?Jj:TmpFloat,NULL},
				{_Swing,HQ_FLOAT_PER,1,0,Zef,NULL},
				{"",HQ_LINE,1,0,0,NULL},

				{_Outside,HQ_RED_LONG,0,m_Hq.Outside>COMPPREC?m_Hq.Outside:0xF8F8F8F8,0,NULL},
				{_Inside,HQ_GREEN_LONG,1,m_Hq.Inside>COMPPREC?m_Hq.Inside:0xF8F8F8F8,0,NULL},
				{"",HQ_LINE,1,0,0,NULL},
			};
			short	ItemNum = sizeof(HkItem)/sizeof(HQITEM);
			DrawHqItem(pDC,Width()-1,Height(),YFrom,ItemNum,HkItem,m_XsFlag,m_Hq.Close,HQ_TEXTH,5);
		}
	}
	else if(strcmp(g_pDSMarket[DSGetMarketXH(nMarket)].Jc,"GD")==0)
	{
		HQITEM	GdItem[]=
		{
			{_BA_Ratio,HQ_PLUS_FLOAT_PER,0,0,m_Hq.Now>COMPPREC?(wl!=0?100.0f*wlc/wl:0):TmpFloat,NULL},
			{_BA_Diff,HQ_VOL_LONG,1,m_Hq.Now>COMPPREC?wlc:0xF8F8F8F8,0,NULL},
			{"",HQ_LINE,1,0,0,NULL},
			{_Ask_1,HQ_PRICE_SEAT,0,0,m_Hq.Sellp[0]>COMPPREC?m_Hq.Sellp[0]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Sellv[0]>0?m_Hq.Sellv[0]:0xF8F8F8F8,0,NULL},
			{_Bid_1,HQ_PRICE_SEAT,0,0,m_Hq.Buyp[0]>COMPPREC?m_Hq.Buyp[0]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Buyv[0]>0?m_Hq.Buyv[0]:0xF8F8F8F8,0,NULL},
			
			{"",HQ_LINE,1,0,0,NULL},
			{_Nominal,HQ_BJ_FLOAT,0,0,m_Hq.Now>COMPPREC?m_Hq.Now:TmpFloat,NULL},
			{_Prev_Close,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Close:TmpFloat,NULL},
			{_Change,HQ_PLUS_FLOAT,0,0,m_Hq.Now>COMPPREC?(m_Hq.Now-m_Hq.Close):TmpFloat,NULL},
			{_High,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Max:TmpFloat,NULL},			
			{_Change_Ratio,HQ_PLUS_FLOAT_PER,0,0,Zf,NULL},
			{_Low,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Min:TmpFloat,NULL},			
			{_Swing,HQ_PLUS_FLOAT_PER,0,0,Zef,NULL},
			{_Open,HQ_BJ_FLOAT,1,0,m_Hq.Open,NULL},
			{_Total_Volume,HQ_VOL_LONG,0,m_Hq.Now>COMPPREC?m_Hq.Volume:0xF8F8F8F8,0,NULL},			
			{_Cur_Volume,HQ_VOL_LONG,1,m_Hq.NowVol,0,NULL},
			{_Average,HQ_BJ_FLOAT,0,0,m_HqEx.AverPrice>COMPPREC?m_HqEx.AverPrice:TmpFloat,NULL},
			{_Total_Amount,HQ_AMOUNT_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Amount:TmpFloat,NULL},
			{_Yield,HQ_BJ_FLOAT,0,0,m_HqEx.GdsyNowSyl>COMPPREC?m_HqEx.GdsyNowSyl:TmpFloat,NULL},
			{_InterRate,HQ_BJ_FLOAT,1,0,m_HqEx.GdsyYjlx>COMPPREC?m_HqEx.GdsyYjlx:TmpFloat,NULL},

			{"",HQ_LINE,1,0,0,NULL},
			
			{_Market,HQ_NULL,0,0,0,NULL},
			{"",HQ_STRING,1,0,0,g_pStockIO->GetMarketInfo(m_pStkInfo)},			
			{"",HQ_LINE,1,0,0,NULL},
		};
		short	ItemNum = sizeof(GdItem)/sizeof(HQITEM);
		DrawHqItem(pDC,Width()-1,Height(),YFrom,ItemNum,GdItem,m_XsFlag,m_Hq.Close,HQ_TEXTH+11,8);
	}
	else
	{
		HQITEM	ABItem[]=
		{
			{_BA_Ratio,HQ_PLUS_FLOAT_PER,0,0,m_Hq.Now>COMPPREC?(wl!=0?100.0f*wlc/wl:0):TmpFloat,NULL},
			{_BA_Diff,HQ_VOL_LONG,1,m_Hq.Now>COMPPREC?wlc:0xF8F8F8F8,0,NULL},
			{"",HQ_LINE,1,0,0,NULL},

			{_Ask_5,HQ_PRICE_SEAT,0,0,m_Hq.Sellp[4]>COMPPREC?m_Hq.Sellp[4]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Sellv[4]>0?m_Hq.Sellv[4]:0xF8F8F8F8,0,NULL},
			{_Ask_4,HQ_PRICE_SEAT,0,0,m_Hq.Sellp[3]>COMPPREC?m_Hq.Sellp[3]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Sellv[3]>0?m_Hq.Sellv[3]:0xF8F8F8F8,0,NULL},
			{_Ask_3,HQ_PRICE_SEAT,0,0,m_Hq.Sellp[2]>COMPPREC?m_Hq.Sellp[2]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Sellv[2]>0?m_Hq.Sellv[2]:0xF8F8F8F8,0,NULL},
			{_Ask_2,HQ_PRICE_SEAT,0,0,m_Hq.Sellp[1]>COMPPREC?m_Hq.Sellp[1]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Sellv[1]>0?m_Hq.Sellv[1]:0xF8F8F8F8,0,NULL},
			{_Ask_1,HQ_PRICE_SEAT,0,0,m_Hq.Sellp[0]>COMPPREC?m_Hq.Sellp[0]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Sellv[0]>0?m_Hq.Sellv[0]:0xF8F8F8F8,0,NULL},
			{"",HQ_LINE,1,0,0,NULL},

			{_Bid_1,HQ_PRICE_SEAT,0,0,m_Hq.Buyp[0]>COMPPREC?m_Hq.Buyp[0]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Buyv[0]>0?m_Hq.Buyv[0]:0xF8F8F8F8,0,NULL},
			{_Bid_2,HQ_PRICE_SEAT,0,0,m_Hq.Buyp[1]>COMPPREC?m_Hq.Buyp[1]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Buyv[1]>0?m_Hq.Buyv[1]:0xF8F8F8F8,0,NULL},
			{_Bid_3,HQ_PRICE_SEAT,0,0,m_Hq.Buyp[2]>COMPPREC?m_Hq.Buyp[2]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Buyv[2]>0?m_Hq.Buyv[2]:0xF8F8F8F8,0,NULL},
			{_Bid_4,HQ_PRICE_SEAT,0,0,m_Hq.Buyp[3]>COMPPREC?m_Hq.Buyp[3]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Buyv[3]>0?m_Hq.Buyv[3]:0xF8F8F8F8,0,NULL},
			{_Bid_5,HQ_PRICE_SEAT,0,0,m_Hq.Buyp[4]>COMPPREC?m_Hq.Buyp[4]:TmpFloat,NULL},
			{"",HQ_VOL_LONG,1,m_Hq.Buyv[4]>0?m_Hq.Buyv[4]:0xF8F8F8F8,0,NULL},
			{"",HQ_LINE,1,0,0,NULL},

			{_Nominal,HQ_BJ_FLOAT,0,0,m_Hq.Now>COMPPREC?m_Hq.Now:TmpFloat,NULL},
			{_Prev_Close,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Close:TmpFloat,NULL},
			{_Change,HQ_PLUS_FLOAT,0,0,m_Hq.Now>COMPPREC?(m_Hq.Now-m_Hq.Close):TmpFloat,NULL},
			{_High,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Max:TmpFloat,NULL},			
			{_Change_Ratio,HQ_PLUS_FLOAT_PER,0,0,Zf,NULL},
			{_Low,HQ_BJ_FLOAT,1,0,m_Hq.Now>COMPPREC?m_Hq.Min:TmpFloat,NULL},			
			{_Swing,HQ_PLUS_FLOAT_PER,0,0,Zef,NULL},
			{_Open,HQ_BJ_FLOAT,1,0,m_Hq.Open,NULL},
			{_Total_Volume,HQ_VOL_LONG,0,m_Hq.Now>COMPPREC?m_Hq.Volume:0xF8F8F8F8,0,NULL},			
			{_Cur_Volume,HQ_VOL_LONG,1,m_Hq.NowVol,0,NULL},
			{"",HQ_LINE,1,0,0,NULL},

			{_Market,HQ_NULL,0,0,0,NULL},
			{"",HQ_STRING,1,0,0,g_pStockIO->GetMarketInfo(m_pStkInfo)},			
			{"",HQ_LINE,1,0,0,NULL},
		};
		short	ItemNum = sizeof(ABItem)/sizeof(HQITEM);
		DrawHqItem(pDC,Width()-1,Height(),YFrom,ItemNum,ABItem,m_XsFlag,m_Hq.Close,HQ_TEXTH+1,6);
	}
}

void UHq::DrawBigHqHead(CDC *pDC)
{
	CRect src[4];
	src[0].top = m_rcIn.top + 5;
	src[1].top = m_rcIn.top + 30;
	src[1].left = m_rcIn.left + 2;
	src[2].top = m_rcIn.top + 32;
	src[2].left = m_rcIn.left+m_rcIn.Width()/3+17;
	src[3].top = m_rcIn.top + 32;
	src[3].left = m_rcIn.right-m_rcIn.Width()/3;

	float ftmp;
	CSize size;
	CFont *oldfont = g_d.SelectFont(pDC,BIGHQ1_FONT);
	size = pDC->GetTextExtent("标");
	COLORREF cn = RGB(255,255,0);

	CSize tmpSize=g_d.DisplaySameWidtchText(pDC,36,src[0].top-2,cn,"%s",m_pStkInfo->Name);
	CSize zs=DrawMarkFlag(m_pStkInfo,pDC,32+tmpSize.cx, src[0].top-2-3,left,top,m_pView);
	if(m_pStkInfo->fl_flag>0)
	{
		tmpSize+=zs;
		tmpSize.cx-=8;
	}
	g_d.SelectFont(pDC,INFO_FONT);
	src[0].top += 2;
	src[1].top += 2;
	src[0].left = m_rcIn.left+32+tmpSize.cx+8;
	if(!m_bZs)
	{
		src[2].left -= 5;
		src[3].left += 1;
	}
	g_d.DisplayText(pDC,src[0].left,src[0].top,cn,"%6s",m_pStkInfo->Code);
	if(m_Hq.Close>COMPPREC)
	{
		g_d.SelectFont(pDC,BIGHQ2_FONT);
		if(!m_bZs)
		{
			ftmp=m_HqContent.Now;
			if(ftmp<COMPPREC) ftmp=m_Hq.Close;
			size=g_d.BjPrintf(pDC,src[1].left,src[1].top-2,ftmp,m_Hq.Close,m_XsFlag,0);
			m_NowRect.SetRect(src[1].left,src[1].top,src[1].left+size.cx,src[1].top+16);
			int tmpType = GetStockType(m_pStkInfo->setcode,m_pStkInfo->Code);
			if(tmpType==CODE_ZXB || tmpType==CODE_CYB || tmpType==CODE_SZAG || tmpType==CODE_SZBG || tmpType==CODE_SB || tmpType==CODE_SHAG || tmpType==CODE_SHBG)
			{
				g_d.SelectFont(pDC,INFO_FONT);
			//	g_d.DisplayText(pDC,src[1].left+size.cx+1,src[1].top+2,VipColor.TXTCOLOR,_F("元"));
			}
			if(m_XsFlag==3) src[2].left-=10;
			if(m_Hq.Close > COMPPREC)
			{
				g_d.SelectFont(pDC,BIGHQ3_FONT);
				if(m_XsFlag!=3 || m_HqContent.Now<10.00)
				{
				//	if(m_HqContent.Zd > 0)
				//		size=g_d.DisplayText(pDC,src[2].left,src[2].top,VipColor.KPCOLOR,_F("▲"));
				//	else if(m_HqContent.Zd < 0)
				//		size=g_d.DisplayText(pDC,src[2].left,src[2].top,VipColor.DOWNCOLOR,_F("▼"));
				//	else
						size=g_d.DisplayText(pDC,src[2].left,src[2].top,VipColor.LEVELCOLOR,"  ");		
				}
				else
					size.cx=18;
				g_d.BjPrintf(pDC,src[2].left+size.cx-2,src[2].top,m_HqContent.Zd,0.0,m_XsFlag,0);
				g_d.BjPrintf(pDC,src[3].left,src[3].top,m_HqContent.Zaf,0.0,2,m_rcIn.Width()/3,2);
			}
		}
		else
		{
			src[1].left-=1;
			src[3].left+=1;
			src[2].top+=2;
			src[3].top+=2;			
			//			
			ftmp=m_ZsHqContent.Now;
			if(ftmp<COMPPREC) ftmp=m_Hq.Close;
			size=g_d.BjPrintf(pDC,src[1].left,src[1].top,ftmp,m_Hq.Close,m_XsFlag,0);
			if(m_ZsHqContent.Now > COMPPREC)
			{
				g_d.SelectFont(pDC,BIGHQ3_FONT);
			//	if(m_ZsHqContent.Zd > 0)
			//		size=g_d.DisplayText(pDC,src[2].left,src[2].top,VipColor.KPCOLOR,_F("▲"));
			//	else if(m_ZsHqContent.Zd < 0)
			//		size=g_d.DisplayText(pDC,src[2].left,src[2].top,VipColor.DOWNCOLOR,_F("▼"));
			//	else
					size=g_d.DisplayText(pDC,src[2].left,src[2].top,VipColor.LEVELCOLOR,"  ");		
				g_d.BjPrintf(pDC,src[2].left+size.cx-2,src[2].top,m_ZsHqContent.Zd,0.0,m_XsFlag,0);
				g_d.BjPrintf(pDC,src[3].left,src[3].top,m_ZsHqContent.Zaf,0.0,2,m_rcIn.Width()/3,2);
			}
		}
	}
	g_d.RestoreFont(pDC,oldfont);
}

void UHq::DrawBigHq(CDC *pDC)
{
	m_bEmpty = m_HqContent.Now+m_HqContent.Max+m_HqContent.Min+m_HqContent.Buyprice[0]+m_HqContent.Sellprice[0] < COMPPREC;
	int i,mmpnum=5;
	int nFixedWidth=m_rcIn.right;
	int YFrom=30+25,XFrom1=HQ_XSPACE,XFrom2=nFixedWidth/6,XFrom3=nFixedWidth/2+HQ_XSPACE,XFrom4=4*nFixedWidth/6,XWidth=2*nFixedWidth/6-HQ_XSPACE;

	DrawBigHqHead(pDC);
	
	g_d.DrawLine(pDC,0,YFrom-1,m_rcIn.right,YFrom-1);
	YFrom += 2;

	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("现量"));
	g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("总量"));
	if(!m_bEmpty)
	{
		if(IsHligtNowVol(m_HqContent.NowVol,m_pStkInfo))
			g_d.DisplayText(pDC,XFrom2,YFrom,RGB(192,0,192),XWidth,MakeVol3(m_HqContent.NowVol));
		else
			g_d.DisplayText(pDC,XFrom2,YFrom,(m_Hq.Flag.InOutFlag==0)?(GridColor.Grid_Up_Color):((m_Hq.Flag.InOutFlag==1)?GridColor.Grid_Down_Color:GridColor.Grid_Level_Color),XWidth,MakeVol3(m_HqContent.NowVol));
		g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,MakeVol3(m_HqContent.Volume));
	}
	YFrom += HQ_TEXTH;	
	
	if(m_bLowSize)
	{
		YFrom += 3;
		if(m_HqContent.buywtl+m_HqContent.sellwtl>0)
		{
			g_d.Bar(pDC,0,YFrom-2,m_rcIn.right*m_HqContent.buywtl*1.00/(m_HqContent.buywtl+m_HqContent.sellwtl),YFrom+1,UP_BRUSH);
			g_d.Bar(pDC,m_rcIn.right*m_HqContent.buywtl*1.00/(m_HqContent.buywtl+m_HqContent.sellwtl),YFrom-2,m_rcIn.right,YFrom+1,DOWN_BRUSH);
		}
		YFrom += 5;
	}
	else
	{
		g_d.DrawLine(pDC,0,YFrom-1,m_rcIn.right,YFrom-1);
		YFrom += 2;

		for(i = 0;i <mmpnum;i++)
		{
			if(i==4)
				g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("卖盘"));
			g_d.DisplayText(pDC,XFrom2+7,YFrom,VipColor.TXTCOLOR,_F(buysellstr_bighq[0][mmpnum-1-i]));
			if(!m_bEmpty)
			{
				if(m_HqContent.Sellprice[mmpnum-1-i]>COMPPREC)
				{
					if(i==4)
					{
						CFont *oldfont = g_d.SelectFont(pDC,BIGHQ2_FONT);
						g_d.BjPrintf(pDC,XFrom2+25,YFrom-3,m_HqContent.Sellprice[mmpnum-1-i],m_HqContent.Close,m_XsFlag,XWidth+3);
						g_d.RestoreFont(pDC,oldfont);
					}
					else
						g_d.BjPrintf(pDC,XFrom2+25,YFrom,m_HqContent.Sellprice[mmpnum-1-i],m_HqContent.Close,m_XsFlag,XWidth);
					m_SellRect[mmpnum-1-i].SetRect(XFrom2+25,YFrom+2,XFrom2+25+XWidth,YFrom+16);
				}
				if(m_HqContent.Sellvol[mmpnum-1-i]>COMPPREC)
					g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HqContent.Sellvol[mmpnum-1-i]);
			}
			if(i<3) YFrom+=HQ_TEXTH-1;
			else	YFrom+=HQ_TEXTH;
		}
		if(m_HqContent.buywtl+m_HqContent.sellwtl>0)
		{
			g_d.Bar(pDC,0,YFrom-2,m_rcIn.right*m_HqContent.buywtl*1.00/(m_HqContent.buywtl+m_HqContent.sellwtl),YFrom+1,UP_BRUSH);
			g_d.Bar(pDC,m_rcIn.right*m_HqContent.buywtl*1.00/(m_HqContent.buywtl+m_HqContent.sellwtl),YFrom-2,m_rcIn.right,YFrom+1,DOWN_BRUSH);
		}
		YFrom += 2;
		
		for(i = 0;i < mmpnum;i++)
		{
			if(i==0)
				g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("买盘"));
			g_d.DisplayText(pDC,XFrom2+7,YFrom,VipColor.TXTCOLOR,_F(buysellstr_bighq[1][i]));
			if(!m_bEmpty)
			{
				if(m_HqContent.Buyprice[i]>COMPPREC)
				{
					if(i==0)
					{
						CFont *oldfont = g_d.SelectFont(pDC,BIGHQ2_FONT);
						g_d.BjPrintf(pDC,XFrom2+25,YFrom,m_HqContent.Buyprice[i],m_HqContent.Close,m_XsFlag,XWidth+3);
						g_d.RestoreFont(pDC,oldfont);
					}
					else
						g_d.BjPrintf(pDC,XFrom2+25,YFrom,m_HqContent.Buyprice[i],m_HqContent.Close,m_XsFlag,XWidth);
					m_BuyRect[i].SetRect(XFrom2+25,YFrom+2,XFrom2+25+XWidth,YFrom+16);
				}
				if(m_HqContent.Buyvol[i]>COMPPREC)
					g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_HqContent.Buyvol[i]);
			}
			if(i==0) YFrom+=HQ_TEXTH;
			else	YFrom+=HQ_TEXTH-1;
		}

		g_d.DrawLine(pDC,0,YFrom-1,m_rcIn.right,YFrom-1);
		YFrom += 2;
	}

	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("今开"));
	g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("均价"));
	if(!m_bEmpty)
	{
		g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.Open,m_HqContent.Close,m_XsFlag,XWidth);
		g_d.BjPrintf(pDC,XFrom4,YFrom,m_HqContent.Average,m_HqContent.Close,m_XsFlag,XWidth);
	}	
	YFrom += HQ_TEXTH;

	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("最高"));
	g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("量比"));	
	if(!m_bEmpty)
	{
		g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.Max,m_HqContent.Close,m_XsFlag,XWidth);
		g_d.NoBjPrintf(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,m_HqContent.fLB,m_XsFlag,XWidth);		
	}	
	YFrom += HQ_TEXTH;	
	
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("最低"));
	g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("金额"));
	if(!m_bEmpty)
	{
		g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.Min,m_HqContent.Close,m_XsFlag,XWidth);
		g_d.DisplayText(pDC,XFrom4,YFrom,GridColor.Grid_Other_Color,XWidth,MakeJE(m_HqContent.Amount));
	}
	YFrom += HQ_TEXTH;	

	if(m_bLowSize)
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("总笔"));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("每笔"));
		if(!m_bEmpty)
		{
			if(m_HqContent.CJBS>0)
			{
				g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,MakeVol3(m_HqContent.CJBS));
				g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%.1f",1.0*m_HqContent.Volume/m_HqContent.CJBS);
			}
			else
			{
				g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,MEANLESS_STR);
				g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,MEANLESS_STR);
			}			
		}	
		YFrom += HQ_TEXTH;	
	}
	
	g_d.DrawLine(pDC,0,YFrom-1,m_rcIn.right,YFrom-1);
	YFrom += 2;
	if(m_bGz || m_bQyzq)
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("应计利息"));
		if(!m_bEmpty && m_HqContent.Yjlx>COMPPREC)
			g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.3f",m_HqContent.Yjlx);
		if(m_Domain==SZ)
		{
			YFrom += HQ_TEXTH;
			g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("结算价全价"));
			if(!m_bEmpty && m_HqContent.Yjlx>COMPPREC)
				g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.2f",(m_HqContent.Now>COMPPREC?m_HqContent.Now:m_HqContent.Close)+m_HqContent.Yjlx);
		}
		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,(m_Domain==SZ)?(_F("到期收益率")):(_F("结算价全价")));
		if(!m_bEmpty && m_HqContent.Yjlx>COMPPREC)
			g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.2f",(m_Domain==SZ)?(m_HqContent.Dqsy):((m_HqContent.Now>COMPPREC?m_HqContent.Now:m_HqContent.Close)+m_HqContent.Yjlx));
	}
	else if(m_bETF)
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("IOPV"));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("昨PV"));
		if(m_HqContent.Jjjz>COMPPREC)
			g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.Jjjz,m_HqContent.LastPV,m_XsFlag,XWidth);
		if(m_HqContent.LastPV>COMPPREC)
			g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.3f",m_HqContent.LastPV);

		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("PV涨"));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("PV幅"));
		if(m_HqContent.Jjjz>COMPPREC && m_HqContent.LastPV>COMPPREC)
		{
			g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.Jjjz-m_HqContent.LastPV,0.0,m_XsFlag,XWidth);
			g_d.BjPrintf(pDC,XFrom4,YFrom,(m_HqContent.Jjjz-m_HqContent.LastPV)*100/m_HqContent.LastPV,0.0,2,XWidth,1);
		}
		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("溢价"));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("溢率"));
		if(m_HqContent.Jjjz>COMPPREC && m_HqContent.Now>COMPPREC)
		{
			g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.Now-m_HqContent.Jjjz,0.0,3,XWidth);			
			g_d.BjPrintf(pDC,XFrom4,YFrom,(m_HqContent.Now-m_HqContent.Jjjz)*100/m_HqContent.Jjjz,0.0,2,XWidth,1);
		}
	}
	else if(m_bSZKFSJJ)
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("股本"));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("换手"));
		if(m_HqContent.Zgb>COMPPREC)
			g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,MakeGB(m_HqContent.Zgb));
		if(m_HqContent.Hsl>COMPPREC)
			g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth+1,"%9.2f%%",m_HqContent.Hsl);			

		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("时点单位净值"));
		if(m_HqContent.Jjjz>COMPPREC)
			g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.4f",m_HqContent.Jjjz);

		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("昨日单位净值"));
		if(m_HqContent.Yjlx>COMPPREC)
			g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.4f",m_HqContent.Yjlx/100.0);
	}
	else if(m_bQz)
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("涨停"));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("跌停"));
		if(m_HqContent.Close>COMPPREC && m_HqContent.TPPrice1>COMPPREC)
			g_d.BjPrintf(pDC,XFrom2,YFrom,m_HqContent.TPPrice1,m_HqContent.Close,m_XsFlag,XWidth);
		if(m_HqContent.Close>COMPPREC && m_HqContent.TPPrice2>COMPPREC)
			g_d.BjPrintf(pDC,XFrom4,YFrom,m_HqContent.TPPrice2,m_HqContent.Close,m_XsFlag,XWidth);
		
		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("换手"));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("杠杆"));
		if(!m_bEmpty)
			g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,"%9.2f%%",m_HqContent.Hsl);
		if(m_HqContent.fGgbl>0.0)
			g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth+1,"%9.2f",m_HqContent.fGgbl);

		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("价值"));
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("溢价"));
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,"%9.3f",m_HqContent.fInnerVal);
		g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.2f%%",m_HqContent.fYjl);
	}
	else
	{
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("换手"));
		if(!m_bEmpty)
			g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,"%9.2f%%",m_HqContent.Hsl);
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("股本"));
		g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth+1,MakeGB(m_HqContent.Zgb));

		YFrom += HQ_TEXTH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,m_bCloseJJ?_F("净值"):_F("净资"));
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,"%9.2f",m_HqContent.Mg_Jz);
		g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("流通"));
		g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth+1,MakeGB(m_HqContent.ActiveCapital));

		YFrom += HQ_TEXTH;
		char strFlag[32];
		switch((int)m_pStkInfo->J_HalfYearFlag)
		{
		case 3:
			strcpy(strFlag, "收益①");
			break;
		case 6:
			strcpy(strFlag, "收益②");
			break;
		case 9:
			strcpy(strFlag, "收益③");
			break;
		case 12:
			strcpy(strFlag, "收益④");
			break;
		default:
			strcpy(strFlag, "收益");
			break;
		}
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(strFlag));				
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.TXTCOLOR,XWidth,"%9.2f",m_HqContent.Mg_Sy);
		if(!m_bCloseJJ)
		{
			g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("市盈"));
			if(m_HqContent.Syl > 0.0)
				g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.1f",m_HqContent.Syl);
			else if(!m_bEmpty)
				g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth-4,MEANLESS_STR);
		}
		else
		{
			g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("溢价"));
			if(m_HqContent.Mg_Jz>COMPPREC)
			{
				float tmpFloat=m_HqContent.Now;
				if(tmpFloat<COMPPREC) tmpFloat=m_HqContent.Close;
				g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.TXTCOLOR,XWidth,"%9.1f%%",(tmpFloat-m_HqContent.Mg_Jz)*100.0/m_HqContent.Mg_Jz);
			}
		}
	}
	YFrom += HQ_TEXTH;

	g_d.DrawLine(pDC,0,YFrom-1,m_rcIn.right,YFrom-1);
	YFrom += 2;

	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("外盘"));
	g_d.DisplayText(pDC,XFrom3,YFrom,VipColor.TXTCOLOR,_F("内盘"));
	if(!m_bEmpty)
	{
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.KPCOLOR,XWidth,MakeVol3(m_HqContent.Outside));
		g_d.DisplayText(pDC,XFrom4,YFrom,VipColor.DOWNCOLOR,XWidth,MakeVol3(m_HqContent.Inside));
	}	
	YFrom += HQ_TEXTH;
	
	if(m_HqContent.Outside+m_HqContent.Inside>0)
	{
		g_d.Bar(pDC,0,YFrom-2,m_rcIn.right*m_HqContent.Outside*1.00/(m_HqContent.Outside+m_HqContent.Inside),YFrom+1,UP_BRUSH);
		g_d.Bar(pDC,m_rcIn.right*m_HqContent.Outside*1.00/(m_HqContent.Outside+m_HqContent.Inside),YFrom-2,m_rcIn.right,YFrom+1,DOWN_BRUSH);
	}
	else g_d.DrawLine(pDC,0,m_rcIn.bottom-1,m_rcIn.right,m_rcIn.bottom-1);
}

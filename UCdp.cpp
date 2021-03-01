#include "stdafx.h"
#include "UCdp.h"
#include "ComView.h"

UCdp::UCdp(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_bEmpty = TRUE;
	m_bDataReady = FALSE;

	m_zsTotalGb = m_zsTotalLtgb = m_zsTotalSz = m_zsTotalLtSz = m_zsSyl = m_zsSjl = m_zsYieldRate = 0.0;
	m_TPPrice1 = m_TPPrice2 = 0.0;
}

UCdp::~UCdp()
{

}

void GetzsGb_Ltgb(short domain,float& zsTotalGb,float& zsTotalLtgb,float& zsTotalSz,float& zsTotalLtSz,float& zsSyl,float& zsSjl,float& zsYieldRate)
{
	zsTotalGb = zsTotalLtgb = zsTotalSz = zsTotalLtSz = zsSyl = zsSjl = zsYieldRate = 0.0;
	float AgTotalSz=0.0,AgTotalJzc=0.0,AgTotalYearYield=0.0;
	MemStkInfo *tmpStkInfo;
	struct CurrStockData tmpHQ;
	int tmpInt;
	float tmpNow=0.0;
	for(int i=0;i < g_pStockIO->GetStockNum(ALL_ALL);i++)
	{
		tmpStkInfo = (*g_pStockIO)[i];
		if(!tmpStkInfo) continue;
		if( (domain==SZ && tmpStkInfo->setcode!=SZ) ||
			(domain==SH && tmpStkInfo->setcode!=SH)
		  )
			continue;
		tmpInt = GetStockType(tmpStkInfo->setcode,tmpStkInfo->Code);
		if(tmpInt==CODE_ZXB || tmpInt==CODE_CYB || tmpInt==CODE_SZAG || tmpInt==CODE_SZJJ || tmpInt==CODE_SZBG || 
		   tmpInt==CODE_SHAG || tmpInt==CODE_SHJJ || tmpInt==CODE_SHBG
		  )
		{
			g_pStockIO->ReadCurrStockData(tmpStkInfo,&tmpHQ);
			if(tmpInt==CODE_SZBG || tmpInt==CODE_SHBG)
				zsTotalGb += tmpStkInfo->J_bg;
			else
				zsTotalGb += (tmpStkInfo->J_zgb-tmpStkInfo->J_hg);
			zsTotalLtgb += tmpStkInfo->ActiveCapital;

			if(tmpInt!=CODE_SZJJ && tmpInt!=CODE_SHJJ) //市值不包括封闭式基金
			{
				if(tmpHQ.Now > COMPPREC)
					tmpNow=tmpHQ.Now;
				else
					tmpNow=tmpHQ.Close;
				if(tmpInt==CODE_SZBG)		tmpNow*=g_gy_rate;
				else if(tmpInt==CODE_SHBG)	tmpNow*=g_my_rate;

				if(tmpInt==CODE_SZBG || tmpInt==CODE_SHBG)
					zsTotalSz += tmpStkInfo->J_bg*10000.0*tmpNow;
				else
					zsTotalSz += (tmpStkInfo->J_zgb-tmpStkInfo->J_hg-tmpStkInfo->J_bg)*10000.0*tmpNow;
				zsTotalLtSz += tmpStkInfo->ActiveCapital*10000.0*tmpNow;

				if(tmpInt==CODE_ZXB || tmpInt==CODE_CYB || tmpInt==CODE_SZAG || tmpInt==CODE_SHAG)
				{
					if(tmpStkInfo->J_HalfYearFlag>0 && tmpNow>COMPPREC)
					{
						AgTotalSz+=tmpStkInfo->J_zgb*10000.0*tmpNow;
						AgTotalYearYield+=tmpStkInfo->J_jly*10000.0*12/tmpStkInfo->J_HalfYearFlag;
						AgTotalJzc+=tmpStkInfo->J_jzc*10000.0;
					}
				}
			}
		}
	}
	if(AgTotalYearYield>COMPPREC)	//市盈率
		zsSyl=AgTotalSz/AgTotalYearYield;
	if(AgTotalJzc>COMPPREC)			//市净率
	{								//净益率
		zsSjl=AgTotalSz/AgTotalJzc;									
		zsYieldRate=AgTotalYearYield/AgTotalJzc;
	}
}

int  UCdp::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
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
		}
		ProcessMsg(WM_PAINT);
		return 1;
	case ASYN_HQ_MESSAGE:
		{
			if(!m_pStkInfo)		return 1;
			short nTmpCurGp = g_pStockIO->GetIndex(m_pStkInfo->Code, m_pStkInfo->setcode);
			if(wParam == nTmpCurGp)
				ProcessMsg(UM_UPDATE);
		}
		return 1;
	case UM_UPDATE:
		if(!m_bDataReady) return 1;
		g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,NULL,&m_HqEx);
		ProcessMsg(WM_PAINT);
		return 1;
	case WM_PAINT:
		{
			if(wParam == 1)
				ProcessMsg(UM_READDATA,lParam);

			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			m_MyDC.SetBkMode(TRANSPARENT);

			CFont *oldfont = g_d.SelectFont(&m_MyDC,INFO_FONT);
			CPen *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
			g_d.DrawLine(&m_MyDC,0,0,0,m_rcIn.bottom);
			if(g_GH.bLeftHQInfo && !m_nInCom2)
				g_d.DrawLine(&m_MyDC,m_rcIn.right-1,0,m_rcIn.right-1,m_rcIn.bottom);
			g_d.DrawLine(&m_MyDC,0,m_rcIn.bottom,m_rcIn.right,m_rcIn.bottom);
			if(m_pStkInfo->bigmarket==HKGP_TYPE) 
				DrawHKGP(&m_MyDC);
			else 
				DrawIt(&m_MyDC);
			g_d.RestorePen(&m_MyDC,oldpen);
			g_d.RestoreFont(&m_MyDC,oldfont);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
		}
		return 1;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void UCdp::ReadData(BOOL bDataReady,BOOL bReqData)
{
	g_pStockIO->ReadCurrStockData(m_pStkInfo,&m_Hq,NULL,&m_HqEx);
	if(m_bZs) GetzsGb_Ltgb(m_bSZSHZS?2:m_Domain,m_zsTotalGb,m_zsTotalLtgb,m_zsTotalSz,m_zsTotalLtSz,m_zsSyl,m_zsSjl,m_zsYieldRate);
	if(!OnLine)		return;

	if(!bDataReady)
	{
		if(bReqData)
		{
			if(m_pStkInfo->ah <= COMPPREC && !m_bZs) 
			{
				//先请求分时数据
				m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				g_nAssisID = SetComAssisID(m_nWhichUnit,0);
				g_pGetData->GetCDP(m_pStkInfo);
			}
		}
	}
	else
		g_pGetData->GetCDPACK(m_pStkInfo);
	m_TPPrice1=GetTPPrice(m_pStkInfo,m_Hq.Close,TRUE);
	m_TPPrice2=GetTPPrice(m_pStkInfo,m_Hq.Close,FALSE);
}

char* lpzCDPTopic[]={"涨停","跌停","ＡＨ","ＮＨ","ＣＤＰ","ＮＬ","ＡＬ","多空平衡","空头回补","空头止损"};
char* lpzCDPTopic2[]={"涨停","跌停","ＡＨ","ＮＨ","ＣＤＰ","ＮＬ","ＡＬ","多空平衡","多头获利","多头止损"};
void UCdp::DrawIt(CDC *pDC)
{
	if(Height() < 20 || Width() < 80) return;	//如果空间太小，不画
	m_bEmpty = m_Hq.Now < COMPPREC;
	int nFixedWidth = m_rcIn.right;
	int TextH=HQ_TEXTH,YFrom=3,XFrom1=HQ_XSPACE,XFrom2=nFixedWidth/2+HQ_XSPACE,XWidth=nFixedWidth/2-2*HQ_XSPACE;
	if(m_bDSFlag)
		return;
	g_d.Bar(pDC,1,0,nFixedWidth-1,YFrom+TextH,GRAYBACK_BRUSH);
	g_d.DisplayText(pDC,XFrom1,YFrom-1,VipColor.TXTCOLOR,_F("提示：以下数据仅供参考"));
	YFrom+=TextH;
	g_d.DrawLine(pDC,0,YFrom-2,m_rcIn.right,YFrom-2);
	YFrom+=2;
	//涨跌停板价
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(lpzCDPTopic[0]));
	if(m_Hq.Close>COMPPREC && m_TPPrice1>COMPPREC)
		g_d.BjPrintf(pDC,XFrom2,YFrom,m_TPPrice1,m_Hq.Close,m_XsFlag,XWidth);
	YFrom+=TextH;
	if(YFrom+TextH > Height()) return;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(lpzCDPTopic[1]));
	if(m_Hq.Close>COMPPREC && m_TPPrice2>COMPPREC)
		g_d.BjPrintf(pDC,XFrom2,YFrom,m_TPPrice2,m_Hq.Close,m_XsFlag,XWidth);
	YFrom+=TextH+2;
	if(YFrom+TextH > Height()) return;
	g_d.DrawLine(pDC,0,YFrom-2,m_rcIn.right,YFrom-2);
	if(m_bZs)
	{
		CString tmpStrTitle;
		tmpStrTitle.Format("%s市场统计 含B股,除H股:",m_bSZSHZS?"沪深":(m_Domain==SZ?"深圳":"上海"));
		g_d.DisplayText(pDC,XFrom1-2,YFrom,VipColor.TXTCOLOR,_F(tmpStrTitle));
		YFrom+=TextH+2;
		g_d.DrawLine(pDC,0,YFrom-2,m_rcIn.right,YFrom-2);		
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("总股本 "));
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.VOLCOLOR,XWidth,MakeGB(m_zsTotalGb));
		YFrom+=TextH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("总流通盘 "));
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.VOLCOLOR,XWidth,MakeGB(m_zsTotalLtgb));
		YFrom+=TextH;

		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("AB股总市值 "));
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.VOLCOLOR,XWidth,MakeJE(m_zsTotalSz));
		YFrom+=TextH;

		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("流通市值 "));
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.VOLCOLOR,XWidth,MakeJE(m_zsTotalLtSz));
		YFrom+=TextH;

		YFrom+=2;
		
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("A股平均市盈率 "));
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.VOLCOLOR,XWidth,"%.2f",m_zsSyl);
		YFrom+=TextH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("A股平均市净率 "));
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.VOLCOLOR,XWidth,"%.2f",m_zsSjl);
		YFrom+=TextH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("A股平均净益率 "));
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.VOLCOLOR,XWidth,"%.2f%%",m_zsYieldRate*100.0);

		return;
	}
	else
	{
		float ftmp=m_Hq.Now;
		if(ftmp<COMPPREC) ftmp=m_pStkInfo->Close;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("AB股总市值 "));
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.VOLCOLOR,XWidth,MakeJE3((m_pStkInfo->J_zgb-m_pStkInfo->J_hg)*10000.0*ftmp));
		YFrom+=TextH;
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F("流通市值 "));
		g_d.DisplayText(pDC,XFrom2,YFrom,VipColor.VOLCOLOR,XWidth,MakeJE3(m_pStkInfo->ActiveCapital*10000.0*ftmp));		
		YFrom+=TextH+2;
		if(YFrom+TextH > Height()) return;
		g_d.DrawLine(pDC,0,YFrom-2,m_rcIn.right,YFrom-2);
	}
	//CDP与TBP
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(lpzCDPTopic[2]));
	if(m_pStkInfo->ah > COMPPREC)
		g_d.BjPrintf(pDC,XFrom2,YFrom,m_pStkInfo->ah,m_Hq.Close,m_XsFlag,XWidth);
	YFrom+=TextH;
	if(YFrom+TextH > Height()) return;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(lpzCDPTopic[3]));
	if(m_pStkInfo->ah > COMPPREC)
		g_d.BjPrintf(pDC,XFrom2,YFrom,m_pStkInfo->nh,m_Hq.Close,m_XsFlag,XWidth);
	YFrom+=TextH;
	if(YFrom+TextH > Height()) return;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(lpzCDPTopic[4]));
	if(m_pStkInfo->ah > COMPPREC)
		g_d.BjPrintf(pDC,XFrom2,YFrom,m_pStkInfo->cdp,m_Hq.Close,m_XsFlag,XWidth);
	YFrom+=TextH;
	if(YFrom+TextH > Height()) return;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(lpzCDPTopic[5]));
	if(m_pStkInfo->ah > COMPPREC)
		g_d.BjPrintf(pDC,XFrom2,YFrom,m_pStkInfo->nl,m_Hq.Close,m_XsFlag,XWidth);
	YFrom+=TextH;
	if(YFrom+TextH > Height()) return;
	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(lpzCDPTopic[6]));
	if(m_pStkInfo->ah > COMPPREC)
		g_d.BjPrintf(pDC,XFrom2,YFrom,m_pStkInfo->al,m_Hq.Close,m_XsFlag,XWidth);
	YFrom+=TextH+3;
	if(YFrom+TextH > Height()) return;
	g_d.DrawLine(pDC,0,YFrom-3,m_rcIn.right,YFrom-3);

	g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(lpzCDPTopic[7]));
	if(m_pStkInfo->ah > COMPPREC)
		g_d.BjPrintf(pDC,XFrom2,YFrom,m_pStkInfo->tbp,m_Hq.Close,m_XsFlag,XWidth);
	YFrom+=TextH;
	if(YFrom+TextH > Height()) return;
	if(m_pStkInfo->dkflag == 0)
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(lpzCDPTopic[8]));
	else
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(lpzCDPTopic2[8]));
	if(m_pStkInfo->ah > COMPPREC)
		g_d.BjPrintf(pDC,XFrom2,YFrom,m_pStkInfo->leave,m_Hq.Close,m_XsFlag,XWidth);
	YFrom+=TextH;
	if(YFrom+TextH > Height()) return;
	if(m_pStkInfo->dkflag == 0)
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(lpzCDPTopic[9]));
	else
		g_d.DisplayText(pDC,XFrom1,YFrom,VipColor.TXTCOLOR,_F(lpzCDPTopic2[9]));
	if(m_pStkInfo->ah > COMPPREC)
		g_d.BjPrintf(pDC,XFrom2,YFrom,m_pStkInfo->stoplost,m_Hq.Close,m_XsFlag,XWidth);
}

void UCdp::DrawHKGP(CDC *pDC)
{
	if(Height()<20||m_pStkInfo==NULL) return;
	if(m_pStkInfo->setcode!=31 && m_pStkInfo->setcode!=32 && m_pStkInfo->setcode!=33) return;
	char	spread[32]={0};
	float	B_Spread=COMPPREC,S_Spread=COMPPREC;
	if(m_Hq.Buyp[0]>COMPPREC && m_Hq.Buyp[1]>COMPPREC)
		B_Spread = m_Hq.Buyp[0]-m_Hq.Buyp[1];
	if(m_Hq.Sellp[0]>COMPPREC && m_Hq.Sellp[1]>COMPPREC)
		S_Spread = m_Hq.Sellp[1]-m_Hq.Sellp[0];
	sprintf(spread, "%.3f/%.3f",B_Spread,S_Spread);
	HQITEM	HkItem[]=
	{
		{_Spread,HQ_NULL,0,0,0,NULL},
		{"",HQ_STRING,1,0,0,spread},
		
		{_IEP,HQ_NULL,0,0,0,NULL},
		{"",HQ_FLOAT,1,0,m_HqEx.IEP,NULL},
		
		{_IEV,HQ_NULL,0,0,0,NULL},
		{"",HQ_FLOAT,1,0,m_HqEx.IEV,NULL},		
	};
	short	ItemNum = sizeof(HkItem)/sizeof(HQITEM);
	DrawHqItem(pDC,Width(),Height(),1,ItemNum,HkItem,m_XsFlag,m_Hq.Close,18,3);
}

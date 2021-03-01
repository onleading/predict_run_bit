/*
* Copyright (c) 2002,通达信开发部
* All rights reserved.
* 
* 文件名称：Uzssub.cpp
* 文件标识：
* 摘    要：完成分时图的相关局部函数等.
*
*/

#include "stdafx.h"
#include "UZst.h"
#include "MainFrm.h"

//////////////////////////////////////////////////////////////////////////
//					与股票叠加和多日分时图相关的函数
//////////////////////////////////////////////////////////////////////////

long GetManyDayBackDate(struct AnalyData * AnalyDatap,short nCacheNum,long date)
{
	long lastdate=GetBackDate(date);
	if(AnalyDatap && nCacheNum>0)
	{
		for(int i=nCacheNum-1;i>=0;i--)
		{
			if(AnalyDatap[i].Time.Date==date)
			{
				if(i>0)	lastdate=AnalyDatap[i-1].Time.Date;
				break;
			}
		}
	}
	return lastdate;
}

void UZst::Overlap_ReadCache()
{
	if(m_nOverlapNum > 0 && m_nStyle<ZST_MULTI) //叠加股票的读缓存
	{
		if(IsSameOpenCloseTimer(m_pOverlapStkInfo[0],m_pStkInfo) && m_pOverlapStkInfo[0] && m_OverlapMinuteData[1][0].Now < COMPPREC)
		{
			GetCahceMinuteData(m_pOverlapStkInfo[0],m_OverlapMinuteData[0]);
			//重新组织Minute,容错
			for(int i=0;i<m_nTodayMin;i++)
				m_OverlapMinuteData[0][i].Minute = GetMinuteFromFZ(i,1,TimeSec);
			for(i = 1;i < m_nTodayMin;i++)
			{
				if(m_OverlapMinuteData[0][i].Now < COMPPREC)
					m_OverlapMinuteData[0][i].Now = m_OverlapMinuteData[0][i-1].Now;
			}
		}
		if(IsSameOpenCloseTimer(m_pOverlapStkInfo[1],m_pStkInfo) && m_nOverlapNum > 1 && m_pOverlapStkInfo[1] && m_OverlapMinuteData[1][0].Now < COMPPREC)
		{
			GetCahceMinuteData(m_pOverlapStkInfo[1],m_OverlapMinuteData[1]);
			//重新组织Minute,容错
			for(int i=0;i<m_nTodayMin;i++)
				m_OverlapMinuteData[1][i].Minute = GetMinuteFromFZ(i,1,TimeSec);
			for(i = 1;i < m_nTodayMin;i++)
			{
				if(m_OverlapMinuteData[1][i].Now < COMPPREC)
					m_OverlapMinuteData[1][i].Now = m_OverlapMinuteData[1][i-1].Now;
			}
		}
		if(IsSameOpenCloseTimer(m_pOverlapStkInfo[2],m_pStkInfo) && m_nOverlapNum > 2 && m_pOverlapStkInfo[2] && m_OverlapMinuteData[2][0].Now < COMPPREC)
		{
			GetCahceMinuteData(m_pOverlapStkInfo[2],m_OverlapMinuteData[2]);
			//重新组织Minute,容错
			int i;
			for(i=0;i<m_nTodayMin;i++)
				m_OverlapMinuteData[2][i].Minute = GetMinuteFromFZ(i,1,TimeSec);
			for(i = 1;i < m_nTodayMin;i++)
			{
				if(m_OverlapMinuteData[2][i].Now < COMPPREC)
					m_OverlapMinuteData[2][i].Now = m_OverlapMinuteData[2][i-1].Now;
			}
		}
	}
}

void UZst::Overlap_ReqData()
{
	if(m_nOverlapNum > 0 && m_nStyle<ZST_MULTI)
	{
		short nNT = GetNowTMinute();
		if(IsSameOpenCloseTimer(m_pOverlapStkInfo[0],m_pStkInfo) &&
			m_pOverlapStkInfo[0] && m_OverlapMinuteData[0][0].Now < COMPPREC)
		{
			m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
			if(g_pStockIO->BeDSStock(m_pOverlapStkInfo[0]))
			{
				g_nAssisID = SetComAssisID(m_nWhichUnit,40);
				g_pGetData->DSGetMinData(m_pOverlapStkInfo[0]);
			}
			else 
			{
				g_nAssisID = SetComAssisID(m_nWhichUnit,30);
				g_pGetData->GetMinuteData(m_pOverlapStkInfo[0],FALSE,nNT,m_OverlapMinuteData[0],TimeSec);
			}
		}
		if(IsSameOpenCloseTimer(m_pOverlapStkInfo[1],m_pStkInfo) &&
			m_nOverlapNum > 1 && m_pOverlapStkInfo[1] && m_OverlapMinuteData[1][0].Now < COMPPREC)
		{
			m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
			if(g_pStockIO->BeDSStock(m_pOverlapStkInfo[1]))
			{
				g_nAssisID = SetComAssisID(m_nWhichUnit,41);
				g_pGetData->DSGetMinData(m_pOverlapStkInfo[1]);
			}
			else 
			{
				g_nAssisID = SetComAssisID(m_nWhichUnit,31);
				g_pGetData->GetMinuteData(m_pOverlapStkInfo[1],FALSE,nNT,m_OverlapMinuteData[1],TimeSec);
			}
		}
		if(IsSameOpenCloseTimer(m_pOverlapStkInfo[2],m_pStkInfo) &&
			m_nOverlapNum > 2 && m_pOverlapStkInfo[2] && m_OverlapMinuteData[2][0].Now < COMPPREC)
		{
			m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
			if(g_pStockIO->BeDSStock(m_pOverlapStkInfo[2]))
			{
				g_nAssisID = SetComAssisID(m_nWhichUnit,42);
				g_pGetData->DSGetMinData(m_pOverlapStkInfo[2]);
			}
			else 
			{
				g_nAssisID = SetComAssisID(m_nWhichUnit,32);
				g_pGetData->GetMinuteData(m_pOverlapStkInfo[2],FALSE,nNT,m_OverlapMinuteData[2],TimeSec);
			}
		}
	}
}

void UZst::Overlap_ReqHis()
{
	m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
	g_nAssisID = SetComAssisID(m_nWhichUnit,35);
	if(!m_bDSFlag) g_pGetData->GetHisMinuteData(m_pOverlapStkInfo[0],m_lDate);
}

void UZst::ManyDay_ReadCache(long lNowRq)
{
	if(m_bManyDayZST && m_nZSTManyDay > 0)
	{
		long curDate = GetBackDate(lNowRq);
		for(int i=m_nZSTManyDay-1;i >= 0;i--)
		{
			if(GetCahceMinuteData(m_pStkInfo,m_ManyDayMinuteData[i],curDate,&m_fManyDayClose[i]))
			{
				for(int kkk=0;kkk < m_nRealTotalMin;kkk++)
				{
					if(m_bZs)
						m_ManyDayMinuteData[i][kkk].NowVol /= 100; //将成交金额变成以万为单位的数值
					if(kkk>0 && m_ManyDayMinuteData[i][kkk].Average < COMPPREC)
						m_ManyDayMinuteData[i][kkk].Average = m_ManyDayMinuteData[i][kkk-1].Average;
				}
			}
			m_ManyDayDate[i] = curDate;
			curDate = GetBackDate(curDate);
		}
	}
}

void UZst::ManyDay_ReqData(long lNowRq)
{
	if(m_bManyDayZST && m_nZSTManyDay>0 && m_pStkInfo)
	{
		struct AnalyData * AnalyDatap = new struct AnalyData[RECENTNUM+50];
		short nCacheNum = ReadCacheData(m_pStkInfo,m_pStkInfo->setcode,AnalyDatap,RECENTNUM,PER_DAY,1);
		if(nCacheNum<1)
			nCacheNum = g_pStockIO->ReadVipDocData(m_pStkInfo,AnalyDatap,RECENTNUM,PER_DAY,1,0);
		long curDate = GetManyDayBackDate(AnalyDatap,nCacheNum,lNowRq);
		for(int i=m_nZSTManyDay-1;i >= 0;i--)
		{
			//如果读缓存成功，则不请求数据
			if(!GetCahceMinuteData(m_pStkInfo,m_ManyDayMinuteData[i],curDate,&m_fManyDayClose[i]))
			{
				m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
				g_nAssisID = SetComAssisID(m_nWhichUnit,50+i);//顺序从20开始
				if(m_bDSFlag)
					g_pGetData->DSGetHisMinData(m_pStkInfo, curDate);
				else 
					g_pGetData->GetHisMinuteData(m_pStkInfo,curDate);
			}
			else
			{							
				for(int kkk=0;kkk < m_nRealTotalMin;kkk++)
				{
					if(m_bZs)
						m_ManyDayMinuteData[i][kkk].NowVol /= 100; //将成交金额变成以万为单位的数值
					if(kkk>0 && m_ManyDayMinuteData[i][kkk].Average < COMPPREC)
						m_ManyDayMinuteData[i][kkk].Average = m_ManyDayMinuteData[i][kkk-1].Average;
				}
			}
			m_ManyDayDate[i] = curDate;
			curDate = GetManyDayBackDate(AnalyDatap,nCacheNum,curDate);
		}
		delete AnalyDatap;
	}
}

short	UZst::GetOverlapGp(MemStkInfo **pInfo, short nMaxGp)
{
	short nOverNum = 0;
	if(g_Overlap[1].nAutoType==0)
	{
		for(int i=0;i<g_Overlap[1].nNum&&nOverNum<nMaxGp;i++)
		{
			g_Overlap[1].pOverlapStk[i] = g_pStockIO->Get_MemStkInfo(g_Overlap[1].StkCode[i], g_Overlap[1].StkSetCode[i]);
			pInfo[nOverNum++] = g_Overlap[1].pOverlapStk[i];
		}
	}
	//自动的
	else
	{
		g_Overlap[1].nNum = 0;
		if((g_Overlap[1].nAutoType&AUTO_OVERLAP_IDX)&&nMaxGp>nOverNum)			//叠加指数
		{
			if(m_pStkInfo->setcode==0&&nMaxGp>nOverNum) 
			{
				MemStkInfo *pInf = g_pStockIO->Get_MemStkInfo(SZ_DPCODE, SZ);
				if(pInf!=m_pStkInfo)	pInfo[nOverNum++] = pInf;
			}
			if(m_pStkInfo->setcode==1&&nMaxGp>nOverNum) 
			{
				MemStkInfo *pInf = g_pStockIO->Get_MemStkInfo(SH_DPCODE, SH);
				if(pInf!=m_pStkInfo)	pInfo[nOverNum++] = pInf;
			}
		}
		if((g_Overlap[1].nAutoType&AUTO_OVERLAP_MG)&&nMaxGp>nOverNum)			//叠加权证正股
		{
			short nCurType = GetStockType(m_pStkInfo->setcode, m_pStkInfo->Code);
			if(nCurType==CODE_SZQZ||nCurType==CODE_SHQZ)
			{
				STKRELATION  LinkData[100] = {0};
				short nLinkNum = g_pStockIO->GetRLTData(m_pStkInfo, LinkData, 100);
				for(int i=0;i<nLinkNum;i++)
				{
					if(strlen(LinkData[i].Code)<1) continue;
					MemStkInfo *pInfo0 = g_pStockIO->Get_MemStkInfo(LinkData[i].Code, LinkData[i].SetCode);
					if(!pInfo0) continue;
					short Type=GetStockType(pInfo0->setcode,pInfo0->Code);
					if(Type==CODE_SHAG||Type==CODE_SZAG||Type==CODE_ZXB||Type==CODE_CYB)
					{
						if(pInfo0!=m_pStkInfo)
							pInfo[nOverNum++] = pInfo0;
					}
				}
			}
		}
	}
	return nOverNum;
}

void UZst::AutoOverlapGP()
{
	MemStkInfo* pInfo[5]={0};

	if(m_lDate != 0)
	{
		if(g_Overlap[1].bOverlapHisIndex)
		{
			if(m_pStkInfo->setcode==SZ) 
			{
				m_pOverlapStkInfo[0] = g_pStockIO->Get_MemStkInfo(SZ_DPCODE, SZ);
				m_nOverlapNum = 1;
			}
			else if(m_pStkInfo->setcode==SH) 
			{
				m_pOverlapStkInfo[0] = g_pStockIO->Get_MemStkInfo(SH_DPCODE, SH);
				m_nOverlapNum = 1;
			}
			else m_nOverlapNum = 0;
		}
		else m_nOverlapNum = 0;
		return;
	}
	m_nOverlapNum = GetOverlapGp(pInfo, 5);
	
	if(pInfo[0]!=m_pOverlapStkInfo[0])
	{
		m_pOverlapStkInfo[0] = pInfo[0];
		memset(&m_OverlapHq[0],0,sizeof(CurrStockData));
		memset(m_OverlapMinuteData[0],0,1500*sizeof(MinuteData));
	}
	if(pInfo[1]!=m_pOverlapStkInfo[1])
	{
		m_pOverlapStkInfo[1] = pInfo[1];
		memset(&m_OverlapHq[1],0,sizeof(CurrStockData));
		memset(m_OverlapMinuteData[1],0,1500*sizeof(MinuteData));
	}
	if(pInfo[2]!=m_pOverlapStkInfo[2])
	{
		m_pOverlapStkInfo[2] = pInfo[2];
		memset(&m_OverlapHq[2],0,sizeof(CurrStockData));
		memset(m_OverlapMinuteData[2],0,1500*sizeof(MinuteData));
	}
}
//////////////////////////////////////////////////////////////////////////
//					其它函数
//////////////////////////////////////////////////////////////////////////

void UZst::SetSwitchLabel()
{
	m_pSwitch->ClearAllLabel();
	m_bHasSwitch=FALSE;
	BOOL bCanSwitch=FALSE;
#ifdef LEVEL2
	bCanSwitch=TRUE;
#else
	bCanSwitch=g_bG2DT;
#endif
	bCanSwitch=TRUE;
	if(bCanSwitch && !m_nInCom2 && !m_nInHisDlg && m_lDate==0 && m_nStyle<ZST_MULTI && m_nWhichUnit==0)
	{
		int i=0;	
		if(testzs(m_pStkInfo))
		{
			m_nSwitchBtn = 3;
			for(i=0;i<m_nSwitchBtn;i++) m_pSwitch->InsertSwitchLabel(i,_F(SwitchUnit_ZstZS[i]),WM_COMMAND,ID_SWITCHLBMMLD);
		}
		else if(m_bDSFlag)
		{
			m_nSwitchBtn = 1;
			m_pSwitch->InsertSwitchLabel(0,_F(SwitchUnit_ZstGP[0]),WM_COMMAND,ID_SWITCHLBMMLD);
		}
		else if(m_bLevel2Property && g_bSupportDDE)
		{
			m_nSwitchBtn = 11;
			for(i=0;i<m_nSwitchBtn;i++) m_pSwitch->InsertSwitchLabel(i,_F(SwitchUnit_ZstL2[i]),WM_COMMAND,ID_SWITCHLBMMLD);
		}
		else
		{
			if(g_bG2DT)
				m_nSwitchBtn = 4;
			else
				m_nSwitchBtn = 3;
			for(i=0;i<m_nSwitchBtn;i++) m_pSwitch->InsertSwitchLabel(i,_F(SwitchUnit_ZstGP[i]),WM_COMMAND,ID_SWITCHLBMMLD);
		}
		m_pSwitch->SetFontColorSchema(VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.AXISCOLOR,VipColor.TXTCOLOR,VipColor.KNCOLOR,VipColor.LEVELCOLOR,RGB(128,0,0),12,SONG_FONT);
		m_bHasSwitch=TRUE;
	}
	if(m_nSwitchBtn<4 || !m_bHasSwitch)
	{
		if(m_nStyle>=ZST_AUCTION && m_nStyle<=ZST_LARGETICK)
			m_nStyle=ZST_BIG;
	}
}

BOOL UZst::IsSameOpenCloseTimer(MemStkInfo *Info1,MemStkInfo *Info2)
{
	if(!Info1 || !Info2) return FALSE;
	short TimeSec1[8]={0}, TimeSec2[8]={0};
	StkInfo2Fz(Info1, TimeSec1);
	StkInfo2Fz(Info2, TimeSec2);
	for(int i=0;i<8;i++)
	{
		if(TimeSec1[i]!=TimeSec2[i])
			return FALSE;
	}
	return TRUE;
}

void UZst::SetBasePrice()
{
	fBasePrice = m_Hq.Close;
	if(m_bIPO && m_bUseOpenAxisForIPO && m_Hq.Open>COMPPREC)
		fBasePrice = m_Hq.Open;
}

void UZst::DrawQJLine(CDC *pDC)
{
	int nLeft = left, nTop = top;
	if(m_nQJPos1>=0) g_d.DotLine(pDC,nLeft+m_nQJPos1,nTop+yStart,nLeft+m_nQJPos1,nTop+yEndBot,VipColor.DRAWLINETOOLCOLOR);
	if(m_nQJPos2>=0) g_d.DotLine(pDC,nLeft+m_nQJPos2,nTop+yStart,nLeft+m_nQJPos2,nTop+yEndBot,VipColor.DRAWLINETOOLCOLOR);
}

void UZst::GetQzLinkInfo(char *pInfo,MemStkInfo *pStkInfo)
{
	pInfo[0]=0;
	int nType = GetQZType(pStkInfo);
	MemStkInfo *pLinkInfo=GetQZLinkNow(pStkInfo);
	if(nType!=0 && pLinkInfo)
	{
		char strInfo2[100]={0},strToken[100]={0};
		float fLinkNow = g_pStockIO->GetStockNow(pLinkInfo);
		if(fLinkNow<COMPPREC) fLinkNow=pLinkInfo->Close;
		if(fLinkNow>COMPPREC)
		{
			if(fLinkNow>pLinkInfo->Close+COMPPREC)
				strcpy(strToken,"▲");
			else if(fLinkNow<pLinkInfo->Close-COMPPREC)
				strcpy(strToken,"▼");
			else
				strcpy(strToken,"◆");
		}
		sprintf(strInfo2,"%.2f%s %s:%.2f",fLinkNow,strToken,nType==1?("认购价"):("认沽价"),pStkInfo->J_tzmgjz);
		sprintf(pInfo,"[%s:%s]",pLinkInfo->Name,_F(strInfo2));
	}		
}

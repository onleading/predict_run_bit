// OneCalc.cpp: implementation of the COneCalc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IDefine.h"
#include "OneCalc.h"
#include "CoopCache.h"
#include "PoloTrade.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCriticalSection	g_logcs;
CCoopCache	g_iReqQueue;
//////////////////////////////////////////////////////////////////////////
long g_nTolNumIn, g_nInterReq;
void		LoadTestQueue()
{
	char strTargetCurrency[1000]={0};
	GetPrivateProfileString("POOL","TARGET","",strTargetCurrency,1000,g_WSXHStr+"POOLInfo_Bit.ini");
	char *stocks[100]={0};
	int nTarNum = GetCmdLine(strTargetCurrency, stocks, 100, "\r\n,");
	
	//加队列
	for(int i=0;i<nTarNum;i++)
	{
		ReqItem rq={0};
		strncpy(rq.Currency, stocks[i], 19);
		rq.nAction = ACTION_TEST;
		g_iReqQueue.SendCache(&rq);
	}
	/*
	//补充持仓中的
	vector<BalanceUnit> MyBalances;
	double dwAvailMoney=0.0;
	GetBalances(MyBalances, dwAvailMoney);
	GetPrivateProfileString("POOL","TARGET","",strTargetCurrency,1000,g_WSXHStr+"POOLInfo_Bit.ini");
	for(i=0;i<MyBalances.size();i++)
	{
		if(strstr(strTargetCurrency,MyBalances[i].currency)==NULL)
		{
			ReqItem rq={0};
			strncpy(rq.Currency, MyBalances[i].currency, 19);
			rq.nAction = ACTION_TEST;
			g_iReqQueue.SendCache(&rq);
		}
	}
	*/

	/*
	ReqItem rq={0};
	rq.nAction = ACTION_TEST;
	g_iReqQueue.SendCache(&rq);
	*/
	
	g_nInterReq = 3000;
}

void		LoadCalcQueue()
{
	g_nTolNumIn = 0;

	char strTargetCurrency[1000]={0};
	GetPrivateProfileString("POOL","TARGET","",strTargetCurrency,1000,g_WSXHStr+"POOLInfo_Bit.ini");
	char *stocks[100]={0};
	int nTarNum = GetCmdLine(strTargetCurrency, stocks, 100, "\r\n,");
	
	//加队列
	for(int i=0;i<nTarNum;i++)
	{
		ReqItem rq={0};
		strncpy(rq.Currency, stocks[i], 19);
		rq.nAction = ACTION_CALC;
		g_iReqQueue.SendCache(&rq);
		g_nTolNumIn++;
	}
	/*
	//补充持仓中的
	vector<BalanceUnit> MyBalances;
	double dwAvailMoney=0.0;
	GetBalances(MyBalances, dwAvailMoney);
	GetPrivateProfileString("POOL","TARGET","",strTargetCurrency,1000,g_WSXHStr+"POOLInfo_Bit.ini");
	for(i=0;i<MyBalances.size();i++)
	{
		if(strstr(strTargetCurrency,MyBalances[i].currency)==NULL)
		{
			ReqItem rq={0};
			strncpy(rq.Currency, MyBalances[i].currency, 19);
			rq.nAction = ACTION_CALC;
			g_iReqQueue.SendCache(&rq);
			g_nTolNumIn++;
		}
	}
	*/

	ReqItem rq={0};
	rq.nAction = ACTION_CALC;
	g_iReqQueue.SendCache(&rq);

	g_nInterReq = 1000;
	if(g_nTolNumIn>0)
		g_nInterReq = 10*1000/g_nTolNumIn;
}

long	GetTimeKeyFromLongShortTime(long_short DateTime)
{
	return 1300*1500*DateTime.Daye.Year+1500*DateTime.Daye.Day+DateTime.Daye.Minute;
}

long	GetTimeKeyFromNTime(NTime DateTime)
{
	return 1300*1500*(DateTime.year-2004)+1500*(100*DateTime.month+DateTime.day)+60*DateTime.hour+DateTime.minute;
}
//////////////////////////////////////////////////////////////////////////
COneCalc::COneCalc()
{
	m_pAnalyData = NULL;
	m_nDataNum = m_nMaxDataNum = 0;
	m_pCalc = NULL;
	m_bStoppedByMe = FALSE;

	m_dNowAverPrice = 0.0;

	if(!InitCalcInterface(m_pCalc)) 
		AfxMessageBox("计算引擎起不来");

	m_StartEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

COneCalc::~COneCalc()
{
	TDELA(m_pAnalyData);
	m_nDataNum = 0;
}
//////////////////////////////////////////////////////////////////////////
void	COneCalc::InputBaseData(char *Code, TestParam tp, AnalyData * pAnalyDatap, long nDataNum)
{
	strncpy(m_Code, Code, 19);
	int nCalcDays = tp.nCalcDays;
	m_nPeriod = PER_MINN;

	if(m_nMaxDataNum>=nDataNum)
		memcpy(m_pAnalyData, pAnalyDatap, nDataNum*sizeof(AnalyData));
	else
	{
		TDELA(m_pAnalyData);
		m_nMaxDataNum = nDataNum+200;
		m_pAnalyData = new AnalyData[m_nMaxDataNum];
		memset(m_pAnalyData, 0, m_nMaxDataNum*sizeof(AnalyData));
		memcpy(m_pAnalyData, pAnalyDatap, nDataNum*sizeof(AnalyData));
	}
	m_nDataNum = nDataNum;

	if(m_nPeriod!=PER_DAY&&m_nPeriod!=PER_DAYN)
	{
		CTime tt = CTime::GetCurrentTime();
		int nCurYear = tt.GetYear()-2004;
		m_pAnalyData[m_nDataNum-1].Time.Daye.Year = nCurYear;
		for(int i=m_nDataNum-1;i>=1;i--)
		{
			if(m_pAnalyData[i].Time.Daye.Day<m_pAnalyData[i-1].Time.Daye.Day)
				nCurYear--;
			m_pAnalyData[i-1].Time.Daye.Year = nCurYear;
		}
	}

	m_TimeDict.clear();
	m_NeedCalcFlag.clear();
	m_AllTradeInfo.clear();
	m_AllProfitLine.clear();
	m_BestProfitLine.clear();
	for(int i=0;i<m_nDataNum;i++)
	{
		long TimeKey = 0;
		if(m_nPeriod==PER_DAY||m_nPeriod==PER_DAYN)
			TimeKey = m_pAnalyData[i].Time.Date;
		else 
			TimeKey = GetTimeKeyFromLongShortTime(m_pAnalyData[i].Time);
		m_TimeDict[TimeKey] = i;

		HisProfitLineItem hpli;
		hpli.TimeKey = TimeKey;
		m_AllProfitLine.push_back(hpli);
		m_BestProfitLine.push_back(hpli);
	}
	//last for else use
	HisProfitLineItem hpli;
	hpli.TimeKey = -1;
	m_AllProfitLine.push_back(hpli);

	int nMaxUnit= min(MAX_UNIT, (float)tp.nUnitParam_End/12.0);
	int nStartPos = nMaxUnit*3*72;
	m_nStartTimeKey = GetTimeKeyFromLongShortTime(m_pAnalyData[nStartPos].Time);

	int nAverCount = 3;
	double dwAmoSum=0.0, dwVolSum=0.0;
	for(int j=m_nDataNum-1;j>=0;j--)
	{
		//aver price
		if(nAverCount>0)
		{
			dwAmoSum += pow((double)nAverCount,0.5)*m_pAnalyData[j].Amount;
			dwVolSum += pow((double)nAverCount,0.5)*(double)m_pAnalyData[j].Volume;
			nAverCount--;
		}
		else break;
	}
	if(dwVolSum>COMPPREC)
		m_dNowAverPrice = 1000.0*dwAmoSum/dwVolSum;
}

//////////////////////////////////////////////////////////////////////////
void	GetNewOneAnaly(AnalyData &NewData, AnalyData *pEnrollData, int nUnit)
{
	memcpy(&NewData, &pEnrollData[0], sizeof(AnalyData));
	for(int i=1;i<nUnit;i++)
	{
		if(pEnrollData[i].High>NewData.High)
			NewData.High=pEnrollData[i].High;
		if(pEnrollData[i].Low<NewData.Low)
			NewData.Low=pEnrollData[i].Low;
		NewData.Close = pEnrollData[i].Close;
		NewData.Amount += pEnrollData[i].Amount;
		NewData.Volume += pEnrollData[i].Volume;
		NewData.YClose = pEnrollData[i].YClose;
	}
	NewData.Time = pEnrollData[nUnit-1].Time;
}

void	COneCalc::CreateNewAnalyData(vector<AnalyData> &vNewData, int nUnit, int nOffset)
{
	vNewData.clear();
	for(int i=nOffset;i<m_nDataNum;i+=nUnit)
	{
		AnalyData iData={0};
		GetNewOneAnaly(iData, &m_pAnalyData[i], min(nUnit,m_nDataNum-i));
		vNewData.push_back(iData);
	}
}

void	COneCalc::CalcProfit(HISDAT *pAnaData, int nStartPos, int nEndPos, float *fTriger, CalcParam TradeParams, BOOL bLog)
{
	float fLastActPrice=0.0f, fActionPrice=0.0f;
	int nLastPos = -1, nLastBuyPos = -1, nLastSellPos = -1;
	
	BOOL bStart = FALSE;
	int nLastXh = -1;
	long nCurTimeKey = -1;
	float fHhv = pAnaData[nStartPos].Close;
	float fLlv = pAnaData[nStartPos].Close;
	for(int i=nStartPos;i<=nEndPos;i++)
	{
		fActionPrice=0.0f;
		nCurTimeKey = GetTimeKeyFromNTime(pAnaData[i].Time);

		BOOL bLetsBuy = (i>0&&fTriger[i]>COMPPREC&&fTriger[i-1]<0.0f-COMPPREC);
		BOOL bLetsSell = (i>0&&fTriger[i]<0.0f-COMPPREC&&fTriger[i-1]>COMPPREC);

		if(pAnaData[i].Close>fHhv)
			fHhv = pAnaData[i].Close;
		if(pAnaData[i].Close<fLlv)
			fLlv = pAnaData[i].Close;

		if(bLetsBuy)
		{
			if(nLastPos<0&&nCurTimeKey<m_nStartTimeKey) 
			{
				fHhv = fLlv = pAnaData[i].Close;
				continue;
			}

			nLastXh = -1;
			fActionPrice = fTriger[i];
			if(fLastActPrice>COMPPREC&&nLastPos>=0)		//calc last sell
			{
				float fProfit = 100.0f*(fLastActPrice-fActionPrice)/fLastActPrice;
				if(fProfit>COMPPREC)
				{
					TradeProfitInfo tpi={0};
					tpi.nBSFlag = -1;
					tpi.nRawPos_End = GetRawPosFromTimeKey(nCurTimeKey);
					tpi.nRawPos_Start = GetRawPosFromTimeKey(GetTimeKeyFromNTime(pAnaData[nLastPos].Time));
					if(nLastBuyPos>=0)
						tpi.nRawPos_LastAct = GetRawPosFromTimeKey(GetTimeKeyFromNTime(pAnaData[nLastBuyPos].Time));
					if(tpi.nRawPos_End<0||tpi.nRawPos_Start<0||tpi.nRawPos_End<=tpi.nRawPos_Start)
						AfxMessageBox("GetRawPosFromTimeKey Error!");

					//profit
					tpi.fProfit = fProfit;
					tpi.fGap = 100.0*(fTriger[i]-fLlv)/fLlv;
					for(int k=0;k<6;k++)
						tpi.fProfitPerItem[k] = tpi.fProfit/pow((double)(tpi.nRawPos_End-tpi.nRawPos_Start+1),0.2*(double)k);
					
					TradeCalcInfo tci={0};
					tci.cp = TradeParams;
					tci.tpi = tpi;
					m_AllTradeInfo.push_back(tci);

					int nTradeXh = m_AllTradeInfo.size()-1;
					m_AllProfitLine[tpi.nRawPos_Start].ActionIO.push_back(nTradeXh);
					m_AllProfitLine[tpi.nRawPos_End].ActionIO.push_back(nTradeXh);
					nLastXh = nTradeXh;
				}
			}
			fHhv = fLlv = pAnaData[i].Close;
			fLastActPrice = fActionPrice;
			nLastBuyPos = nLastPos = i;
		}
		else if(bLetsSell)
		{
			if(nLastPos<0&&nCurTimeKey<m_nStartTimeKey) 
			{
				fHhv = fLlv = pAnaData[i].Close;
				continue;
			}

			nLastXh = -1;
			fActionPrice = fabs(fTriger[i]);
			if(fLastActPrice>COMPPREC&&nLastPos>=0)		//calc last buy
			{
				float fProfit = 100.0f*(fActionPrice-fLastActPrice)/fLastActPrice;
				if(fProfit>COMPPREC)
				{
					TradeProfitInfo tpi={0};
					tpi.nBSFlag = 1;
					tpi.nRawPos_End = GetRawPosFromTimeKey(nCurTimeKey);
					tpi.nRawPos_Start = GetRawPosFromTimeKey(GetTimeKeyFromNTime(pAnaData[nLastPos].Time));
					if(nLastSellPos>=0)
						tpi.nRawPos_LastAct = GetRawPosFromTimeKey(GetTimeKeyFromNTime(pAnaData[nLastSellPos].Time));
					if(tpi.nRawPos_End<0||tpi.nRawPos_Start<0||tpi.nRawPos_End<=tpi.nRawPos_Start)
						AfxMessageBox("GetRawPosFromTimeKey Error!");

					//profit
					tpi.fProfit = fProfit;
					tpi.fGap = 100.0*(fHhv-fabs(fTriger[i]))/fHhv;
					for(int k=0;k<6;k++)
						tpi.fProfitPerItem[k] = tpi.fProfit/pow((double)(tpi.nRawPos_End-tpi.nRawPos_Start+1),0.2*(double)k);
					
					TradeCalcInfo tci={0};
					tci.cp = TradeParams;
					tci.tpi = tpi;
					m_AllTradeInfo.push_back(tci);
					
					int nTradeXh = m_AllTradeInfo.size()-1;
					m_AllProfitLine[tpi.nRawPos_Start].ActionIO.push_back(nTradeXh);
					m_AllProfitLine[tpi.nRawPos_End].ActionIO.push_back(nTradeXh);
					nLastXh = nTradeXh;
				}
			}
			fHhv = fLlv = pAnaData[i].Close;
			fLastActPrice = fActionPrice;
			nLastSellPos = nLastPos = i;
		}
		else if(!bStart&&nCurTimeKey>=m_nStartTimeKey)
		{
			bStart = TRUE;
			nLastPos = i;
			if(i>0)
				fLastActPrice = pAnaData[i-1].Close;
			else fLastActPrice = pAnaData[i].Open;
		}
	}
	//record last xh to last pos
	if(nLastXh>=0)
	{
		long nRawPos_Last = m_AllProfitLine.size()-1;
		m_AllProfitLine[nRawPos_Last].ActionIO.push_back(nLastXh);
	}
}

void	COneCalc::CalcRes_Show_Result(char *ZBCode, int nUnit, CalcParam CPParam, int nUnitParamFlag_Start, int nUnitParamFlag_End)
{
	m_nGSIndex = m_pCalc->_CallDTopViewCalc_9(ZB_TYPE, ZBCode);
	if(m_nGSIndex<0)
	{
		AfxMessageBox("找不到指标");
		return;
	}
	if(!m_pAnalyData||m_nDataNum<1)
	{
		AfxMessageBox("找不到数据");
		return;
	}

	TINDEXINFO *pIndex_Org = m_pCalc->_CallDTopViewCalc_8(ZB_TYPE, m_nGSIndex);
	if(!pIndex_Org)
	{
		AfxMessageBox("找不到指标");
		return;
	}
	TINDEXINFO ThisIndexInfo={0};
	memcpy(&ThisIndexInfo, pIndex_Org, sizeof(TINDEXINFO));
	if(pIndex_Org->lpszDefTxt)
	{
		int nDefTxtLen = strlen(pIndex_Org->lpszDefTxt)+1;
		ThisIndexInfo.lpszDefTxt = new char[nDefTxtLen];
		memset(ThisIndexInfo.lpszDefTxt, 0, nDefTxtLen);
		strncpy(ThisIndexInfo.lpszDefTxt, pIndex_Org->lpszDefTxt, nDefTxtLen-1);
	}
	ThisIndexInfo.lpszCompliedInfo = NULL;
	TINDEXINFO *pIndex = &ThisIndexInfo;

	vector<AnalyData> NewAnalyArr;
	CreateNewAnalyData(NewAnalyArr, nUnit, 0);
	long nDataNum = NewAnalyArr.size();
	
	//计算
	HISDAT	*tmp = new HISDAT[nDataNum];
	memset(tmp, 0, nDataNum*sizeof(HISDAT));
	RawData2CalcData(m_Code,0,m_nPeriod,&NewAnalyArr[0],tmp,nDataNum,g_NoTime);
	m_pCalc->_CallDTopViewCalc_3(m_Code,0,m_nPeriod,tmp,nDataNum);
	for(int nParam0=pIndex->aPara[0].nMin;nParam0<=pIndex->aPara[0].nMax;nParam0+=pIndex->aPara[0].nStep)
	{
		for(int i=0;i<MAX_PRDNUM;i++)
			pIndex->aPara[0].nValue[i] = nParam0;
		for(int nParam1=pIndex->aPara[1].nMin;nParam1<=pIndex->aPara[1].nMax;nParam1+=pIndex->aPara[1].nStep)
		{
			for(int j=0;j<MAX_PRDNUM;j++)
				pIndex->aPara[1].nValue[j] = nParam1;


			int nNeedCalcFlag = nUnit*nParam0;
			if(nNeedCalcFlag<nUnitParamFlag_Start||nNeedCalcFlag>nUnitParamFlag_End)
				continue;

			CalcParam cp={0};
			memcpy(&cp, &CPParam, sizeof(CalcParam));
			strncpy(cp.ZBCode, ZBCode, 20);

			float *pCalcData[MAX_LINEOUT]={0};
			if ( -1 == m_pCalc->_CallDTopViewCalc_5(pIndex,pCalcData) )
			{
				for(int k=0;k<MAX_LINEOUT;k++)
					m_pCalc->_CallDTopViewCalc_14(pCalcData[k]);
				AfxMessageBox("计算失败",MB_OK);
				continue;
			}

			int nEndPos = nDataNum-1;
			int nStartPos = 0;
			for(;!TBE_MEANLESS(pCalcData[0][nStartPos])&&nStartPos<=nEndPos;nStartPos++);
			float *fTriger = pCalcData[0];
	
			cp.ZBParam[0] = nParam0;
			cp.ZBParam[1] = nParam1;
			cp.nUnit = nUnit;
			cp.fNow = tmp[nEndPos].Close;
			if(m_dNowAverPrice>COMPPREC)
				cp.fAverPrice = m_dNowAverPrice;
			if(fTriger[nEndPos]>COMPPREC)
				cp.nBSFlag = 1;
			else if(fTriger[nEndPos]<0.0f-COMPPREC)
				cp.nBSFlag = -1;
			cp.nRawPos_Start = -1;
			cp.nRawPos_LastAct = -1;
			cp.fHLValue = 0.0f;
			int nFindHighLow = 0;
			for(j=nEndPos;j>=nStartPos;j--)
			{
				long nThisRawPos = GetRawPosFromTimeKey(GetTimeKeyFromNTime(tmp[j].Time));
				//high low
				if(nFindHighLow==1&&tmp[j].High>=cp.fHLValue)
				{
					cp.fHLValue = tmp[j].High;
					cp.lHLRawPos = 	nThisRawPos;
				}
				else if(nFindHighLow==-1&&tmp[j].Low<=cp.fHLValue)
				{
					cp.fHLValue = tmp[j].Low;
					cp.lHLRawPos = 	nThisRawPos;
				}

				BOOL bLetsBuy = (j>0&&fTriger[j]>COMPPREC&&fTriger[j-1]<0.0f-COMPPREC);
				BOOL bLetsSell = (j>0&&fTriger[j]<0.0f-COMPPREC&&fTriger[j-1]>COMPPREC);
				if(bLetsBuy||bLetsSell)
				{
					if(cp.nRawPos_Start>0)
					{
						cp.nRawPos_LastAct = nThisRawPos;
						if(nFindHighLow==1)
							cp.fLastActPrice = fabs(fTriger[j]);
						else if(nFindHighLow==-1)
							cp.fLastActPrice = fabs(fTriger[j]);
						break;
					}
					else 
					{
						cp.nRawPos_Start = nThisRawPos;
						if(bLetsBuy)
						{
							cp.fActPrice = fabs(fTriger[j]);
							cp.fHLValue = tmp[j].Low;
							cp.lHLRawPos = 	nThisRawPos;

							nFindHighLow = -1;
						}
						else if(bLetsSell)
						{
							cp.fActPrice = fabs(fTriger[j]);
							cp.fHLValue = tmp[j].High;
							cp.lHLRawPos = 	nThisRawPos;

							nFindHighLow = 1;
						}
					}
				}
			}
			CalcProfit(tmp, nStartPos, nEndPos, fTriger, cp);
			m_NeedCalcFlag.push_back(nNeedCalcFlag);

			for(int k=0;k<MAX_LINEOUT;k++)
				m_pCalc->_CallDTopViewCalc_14(pCalcData[k]);

		//	Sleep(1);
		}
	}
	delete []tmp;
	TDELA(pIndex->lpszCompliedInfo);
	TDELA(pIndex->lpszDefTxt);
}


//static
void	COneCalc::FiltBestProfLine_Static(vector<MatchParamInfo> &BestMatchs)
{
	set<int> CurInRangeItems;
	set<int> BestRangeItems;
	int nLastResXh = m_AllProfitLine.size()-1;
	set<int> LastTradeItems;
	for(int i=0;i<m_AllProfitLine[nLastResXh].ActionIO.size();i++)
	{
		int nXh = m_AllProfitLine[nLastResXh].ActionIO[i];
		LastTradeItems.insert(nXh);
	}
	double dwSum4Last = 0.0;
	int nCount4Last = 0;
	vector<MatchParamInfo> vGoodMatch;
	for(i=0;i<m_AllProfitLine.size();i++)
	{
		if(m_AllProfitLine[i].ActionIO.size()>0)		//有动作
		{
			for(int k=0;k<m_AllProfitLine[i].ActionIO.size();k++)
			{
				int nXh = m_AllProfitLine[i].ActionIO[k];
				if(LastTradeItems.find(nXh)!=LastTradeItems.end())		//qu chong
					continue;
				set<int>::iterator iter = CurInRangeItems.find(nXh);
				if(iter==CurInRangeItems.end())		//新的加入
					CurInRangeItems.insert(nXh);
				else								//旧的退出
					CurInRangeItems.erase(iter);
			}
			if(CurInRangeItems.size()<1) 
				continue;
			//calc best
			vector<int> CurInRangeItems4Calc;
			set<int>::iterator iter = CurInRangeItems.begin();
			for(;iter!=CurInRangeItems.end();iter++)
				CurInRangeItems4Calc.push_back((*iter));
			
			map<int, int, greater<int> > Map4Select;
			vector<int> SelectedAll, SelectSingle;
			for(k=0;k<6;k++)
			{
				map<double, int, greater_equal<double> > DataVal;
				for(int j=0;j<CurInRangeItems4Calc.size();j++)
				{
					int nXh = CurInRangeItems4Calc[j];
					if(nXh<0||nXh>=m_AllTradeInfo.size())
						AfxMessageBox("Fetch Wrong TradeInfo");
					TradeCalcInfo tci = m_AllTradeInfo[nXh];
					double dProfPerItem = tci.tpi.fProfitPerItem[k];
					DataVal[dProfPerItem] = nXh;
				}
				vector<int> Selectedin;
				FiltBestFrom(0.05, Selectedin, DataVal);

				for(j=0;j<Selectedin.size();j++)
				{
					int nXh = Selectedin[j];
					if(Map4Select.find(nXh)!=Map4Select.end())
						Map4Select[nXh]++;
					else 
						Map4Select[nXh] = 1;
					
					if(k==3)
						SelectSingle.push_back(nXh);
				}
			}
			map<int, int, greater<int> >::iterator iterSel = Map4Select.begin();
			for(;iterSel!=Map4Select.end();iterSel++)
			{
				if((*iterSel).second>=4)
					SelectedAll.push_back((*iterSel).first);
			}
			if(SelectedAll.empty())
			{
				iterSel = Map4Select.begin();
				for(;iterSel!=Map4Select.end();iterSel++)
				{
					if((*iterSel).second>=3)
						SelectedAll.push_back((*iterSel).first);
				}
			}
			if(SelectedAll.empty())
			{
				iterSel = Map4Select.begin();
				for(;iterSel!=Map4Select.end();iterSel++)
				{
					if((*iterSel).second>=2)
						SelectedAll.push_back((*iterSel).first);
				}
			}
			if(SelectedAll.empty())
			{
				for(int j=0;j<SelectSingle.size();j++)
					SelectedAll.push_back(SelectSingle[j]);
			}
			//All
			for(int j=0;j<SelectedAll.size();j++)
			{
				int nXh = SelectedAll[j];
				if(BestRangeItems.find(nXh)==BestRangeItems.end())
				{
					BestRangeItems.insert(nXh);
					
					TradeCalcInfo tci = m_AllTradeInfo[nXh];
					//add
					BOOL bExist = FALSE;
					for(int n=0;n<vGoodMatch.size();n++)
					{
						if(memcmp(&tci.cp, &vGoodMatch[n].cp, sizeof(CalcParam))==0)
						{
							vGoodMatch[n].fScore1+=1.0;
							vGoodMatch[n].fProfitSum += tci.tpi.fProfit;
							vGoodMatch[n].fGapSum += tci.tpi.fGap;
							vGoodMatch[n].nRangeSum += (tci.tpi.nRawPos_End-tci.tpi.nRawPos_Start+1);
							vGoodMatch[n].nCount++;
							bExist = TRUE;
							break;
						}
					}
					if(!bExist)
					{
						MatchParamInfo mpi={0};
						memcpy(&mpi.cp, &tci.cp, sizeof(CalcParam));
						mpi.fScore1 = 1.0;
						mpi.fProfitSum = tci.tpi.fProfit;
						mpi.fGapSum = tci.tpi.fGap;
						mpi.nRangeSum = (tci.tpi.nRawPos_End-tci.tpi.nRawPos_Start+1);
						mpi.nCount = 1;
						vGoodMatch.push_back(mpi);
					}
				}
			}
		}
	}

	map<double, int, greater_equal<double> > DataVal;
	for(i=0;i<vGoodMatch.size();i++)
		DataVal[vGoodMatch[i].fScore1] = i;
	vector<int> Selected;
	FiltBestFrom(0.05, Selected, DataVal);
	for(i=0;i<Selected.size();i++)
	{
		int nXh = Selected[i];
		BestMatchs.push_back(vGoodMatch[nXh]);
	}
}


//Dyna
void	COneCalc::FiltBestProfLine_Dyna(vector<MatchParamInfo> &BestMatchs)
{
	set<int> CurInRangeItems;
	set<int> BestRangeItems;
	vector<int> LastInRangeItems;
	BOOL bLastPos = FALSE;
	int nLastResXh = m_AllProfitLine.size()-1;
	set<int> LastTradeItems;
	for(int i=0;i<m_AllProfitLine[nLastResXh].ActionIO.size();i++)
	{
		int nXh = m_AllProfitLine[nLastResXh].ActionIO[i];
		LastTradeItems.insert(nXh);
	}
	double dwSum4Last = 0.0;
	int nCount4Last = 0;
	for(i=0;i<m_AllProfitLine.size();i++)
	{
		if(i==nLastResXh)
			bLastPos = TRUE;
		if(m_AllProfitLine[i].ActionIO.size()>0)		//有动作
		{
			if(!bLastPos&&m_AllProfitLine[i].TimeKey<m_nStartTimeKey) 
				AfxMessageBox("How Start Before m_nStartTimeKey");
			for(int k=0;k<m_AllProfitLine[i].ActionIO.size();k++)
			{
				int nXh = m_AllProfitLine[i].ActionIO[k];
				if(!bLastPos&&LastTradeItems.find(nXh)!=LastTradeItems.end())		//qu chong
					continue;
				set<int>::iterator iter = CurInRangeItems.find(nXh);
				if(iter==CurInRangeItems.end())		//新的加入
					CurInRangeItems.insert(nXh);
				else								//旧的退出
					CurInRangeItems.erase(iter);
			}
			if(CurInRangeItems.size()<1) 
				continue;
			//calc best
			vector<int> CurInRangeItems4Calc;
			set<int>::iterator iter = CurInRangeItems.begin();
			for(;iter!=CurInRangeItems.end();iter++)
				CurInRangeItems4Calc.push_back((*iter));
			
			map<int, int, greater<int> > Map4Select;
			vector<int> SelectedAll, SelectSingle;
			for(k=0;k<6;k++)
			{
				map<double, int, greater_equal<double> > DataVal;
				for(int j=0;j<CurInRangeItems4Calc.size();j++)
				{
					int nXh = CurInRangeItems4Calc[j];
					if(nXh<0||nXh>=m_AllTradeInfo.size())
						AfxMessageBox("Fetch Wrong TradeInfo");
					TradeCalcInfo tci = m_AllTradeInfo[nXh];
					double dProfPerItem = tci.tpi.fProfitPerItem[k];
					DataVal[dProfPerItem] = nXh;
				}
				vector<int> Selectedin;
				FiltBestFrom(0.1, Selectedin, DataVal);

				for(j=0;j<Selectedin.size();j++)
				{
					int nXh = Selectedin[j];
					if(Map4Select.find(nXh)!=Map4Select.end())
						Map4Select[nXh]++;
					else 
						Map4Select[nXh] = 1;
					
					if(k==3)
						SelectSingle.push_back(nXh);
				}
			}
			map<int, int, greater<int> >::iterator iterSel = Map4Select.begin();
			for(;iterSel!=Map4Select.end();iterSel++)
			{
				if((*iterSel).second>=4)
					SelectedAll.push_back((*iterSel).first);
			}
			if(SelectedAll.empty())
			{
				iterSel = Map4Select.begin();
				for(;iterSel!=Map4Select.end();iterSel++)
				{
					if((*iterSel).second>=3)
						SelectedAll.push_back((*iterSel).first);
				}
			}
			if(SelectedAll.empty())
			{
				iterSel = Map4Select.begin();
				for(;iterSel!=Map4Select.end();iterSel++)
				{
					if((*iterSel).second>=2)
						SelectedAll.push_back((*iterSel).first);
				}
			}
			if(SelectedAll.empty())
			{
				for(int j=0;j<SelectSingle.size();j++)
					SelectedAll.push_back(SelectSingle[j]);
			}
			//All
			for(int j=0;j<SelectedAll.size();j++)
			{
				int nXh = SelectedAll[j];
				if(BestRangeItems.find(nXh)==BestRangeItems.end())
				{
					TradeCalcInfo tci = m_AllTradeInfo[nXh];
					BestRangeItems.insert(nXh);
					m_BestProfitLine[tci.tpi.nRawPos_Start].ActionIO.push_back(nXh);
					m_BestProfitLine[tci.tpi.nRawPos_End].ActionIO.push_back(nXh);

					if(bLastPos)
						LastInRangeItems.push_back(nXh);
				}
			}
		}
	}
	//
	if(LastInRangeItems.empty()) return;

	vector<MatchParamInfo> vGoodMatch;
	for(i=0;i<LastInRangeItems.size();i++)
	{
		int nXh = LastInRangeItems[i];
		TradeCalcInfo tci_cur = m_AllTradeInfo[nXh];
		CalcParam cp_cur = tci_cur.cp;
		TradeProfitInfo tpi_cur = tci_cur.tpi;
		int nCurBSFlag = tpi_cur.nBSFlag;

		//find previous match
		vector<CalcParam> vPreviousMatch_cur;
		GetPreviousMatch(tci_cur, vPreviousMatch_cur);

		for(int k=0;k<m_BestProfitLine.size();k++)
		{
			for(int j=0;j<m_BestProfitLine[k].ActionIO.size();j++)
			{
				int nHisXh = m_BestProfitLine[k].ActionIO[j];
				TradeCalcInfo tci_his = m_AllTradeInfo[nHisXh];
				CalcParam cp_his = tci_his.cp;
				TradeProfitInfo tpi_his = tci_his.tpi;
				int nHisBSFlag = tpi_his.nBSFlag;
				if(memcmp(&cp_cur, &cp_his, sizeof(CalcParam))==0&&
					nCurBSFlag == nHisBSFlag)					//匹配到历史相同参数
				{
					//find previous match
					vector<CalcParam> vPreviousMatch_his;
					GetPreviousMatch(tci_his, vPreviousMatch_his);
					int nPreMatchCout = 0;
					for(int m=0;m<vPreviousMatch_his.size();m++)
					{
						CalcParam cp_his = vPreviousMatch_his[m];
						for(int n=0;n<vPreviousMatch_cur.size();n++)
						{
							CalcParam cp_cur = vPreviousMatch_cur[n];
							if(memcmp(&cp_his, &cp_cur, sizeof(CalcParam))==0)
							{
								nPreMatchCout++;
								break;
							}
						}
					}
					double fPreMatchFlag = 100.0*(double)nPreMatchCout/((double)vPreviousMatch_cur.size()+(double)vPreviousMatch_his.size());

					//calc next pair item's range
					vector<TradeCalcInfo> vNextMatch_his;
					GetNextMatch(tci_his, vNextMatch_his);
					for(m=0;m<vNextMatch_his.size();m++)
					{
						BOOL bExist = FALSE;
						TradeCalcInfo tci_his_next = vNextMatch_his[m];
						for(int n=0;n<vGoodMatch.size();n++)
						{
							if(memcmp(&tci_his_next.cp, &vGoodMatch[n].cp, sizeof(CalcParam))==0/*&&vGoodMatch[n].cp.nBSFlag==tci_his_next.tpi.nBSFlag*/)
							{
								vGoodMatch[n].fScore1+=1.0;
								vGoodMatch[n].fScore2+=fPreMatchFlag;
								vGoodMatch[n].fProfitSum += tci_his_next.tpi.fProfit;
								vGoodMatch[n].fGapSum += tci_his_next.tpi.fGap;
								vGoodMatch[n].nRangeSum += (tci_his_next.tpi.nRawPos_End-tci_his_next.tpi.nRawPos_Start+1);
								vGoodMatch[n].nCount++;
								bExist = TRUE;
								break;
							}
						}
						if(!bExist)
						{
							CalcParam nNowCp = tci_his_next.cp;
							//检验最新数据
							BOOL bGoodMatch = FALSE;
							if(/*tci_his_next.tpi.nBSFlag+nCurBSFlag==0&&*/
								nNowCp.nRawPos_Start<=tpi_cur.nRawPos_End&&
								tpi_cur.nRawPos_End-nNowCp.nRawPos_Start+1<0.2f*(float)(tpi_cur.nRawPos_End-tpi_cur.nRawPos_Start+1)+0.5f)
								bGoodMatch = TRUE;
							else if(/*tci_his_next.tpi.nBSFlag+nCurBSFlag==0&&*/
								nNowCp.nRawPos_Start>=tpi_cur.nRawPos_End&&
								nNowCp.nRawPos_LastAct>0&&nNowCp.nRawPos_LastAct<tpi_cur.nRawPos_End)
								bGoodMatch = TRUE;

							if(bGoodMatch)
							{
								MatchParamInfo mpi={0};
								memcpy(&mpi.cp, &tci_his_next.cp, sizeof(CalcParam));
								mpi.fScore1 = 1.0; 
								mpi.fScore2 = fPreMatchFlag;
								mpi.fGapSum = tci_his_next.tpi.fGap;
								mpi.fProfitSum = tci_his_next.tpi.fProfit;
								mpi.nRangeSum = (tci_his_next.tpi.nRawPos_End-tci_his_next.tpi.nRawPos_Start+1);
								mpi.nCount = 1;
								vGoodMatch.push_back(mpi);
							}
						}
					}
				}
			}
		}
	}

	map<double, int, greater_equal<double> > DataVal;
	for(i=0;i<vGoodMatch.size();i++)
	{
		double fScore = pow(vGoodMatch[i].fScore1, 1.0)*pow(vGoodMatch[i].fScore2, 1.618);
		DataVal[fScore] = i;
	}
	vector<int> Selected;
	FiltBestFrom(0.1, Selected, DataVal);
	for(i=0;i<Selected.size();i++)
	{
		int nXh = Selected[i];
		BestMatchs.push_back(vGoodMatch[nXh]);
	}
}

void	COneCalc::FiltBestProfLine(vector<MatchParamInfo> &BestMatchs_Static, vector<MatchParamInfo> &BestMatchs_Dyna)
{
	long nSleepCount_1 = 0;
	long nSleepCount_2 = 0;

	set<int> CurInRangeItems;
	set<int> BestRangeItems;
	vector<int> LastInRangeItems;
	BOOL bLastPos = FALSE;
	int nLastResXh = m_AllProfitLine.size()-1;
	set<int> LastTradeItems;
	for(int i=0;i<m_AllProfitLine[nLastResXh].ActionIO.size();i++)
	{
		int nXh = m_AllProfitLine[nLastResXh].ActionIO[i];
		LastTradeItems.insert(nXh);
	}
	double dwSum4Last = 0.0;
	int nCount4Last = 0;
	vector<MatchParamInfo> vGoodMatch_s;
	for(i=0;i<m_AllProfitLine.size();i++)
	{
		if(i==nLastResXh)
			bLastPos = TRUE;
		if(m_AllProfitLine[i].ActionIO.size()>0)		//有动作
		{
			if(!bLastPos&&m_AllProfitLine[i].TimeKey<m_nStartTimeKey) 
				AfxMessageBox("How Start Before m_nStartTimeKey");
			for(int k=0;k<m_AllProfitLine[i].ActionIO.size();k++)
			{
				int nXh = m_AllProfitLine[i].ActionIO[k];
				if(!bLastPos&&LastTradeItems.find(nXh)!=LastTradeItems.end())		//qu chong
					continue;
				set<int>::iterator iter = CurInRangeItems.find(nXh);
				if(iter==CurInRangeItems.end())		//新的加入
					CurInRangeItems.insert(nXh);
				else								//旧的退出
					CurInRangeItems.erase(iter);
			}
			if(CurInRangeItems.size()<1) 
				continue;
			//calc best
			vector<int> CurInRangeItems4Calc;
			set<int>::iterator iter = CurInRangeItems.begin();
			for(;iter!=CurInRangeItems.end();iter++)
				CurInRangeItems4Calc.push_back((*iter));
			
			map<int, int, greater<int> > Map4Select;
			vector<int> SelectedAll, SelectSingle;
			for(k=0;k<6;k++)
			{
				map<double, int, greater_equal<double> > DataVal;
				for(int j=0;j<CurInRangeItems4Calc.size();j++)
				{
					int nXh = CurInRangeItems4Calc[j];
					if(nXh<0||nXh>=m_AllTradeInfo.size())
						AfxMessageBox("Fetch Wrong TradeInfo");
					TradeCalcInfo tci = m_AllTradeInfo[nXh];
					double dProfPerItem = tci.tpi.fProfitPerItem[k];
					DataVal[dProfPerItem] = nXh;
				}
				vector<int> Selectedin;
				FiltBestFrom(0.0382, Selectedin, DataVal);

				for(j=0;j<Selectedin.size();j++)
				{
					int nXh = Selectedin[j];
					if(Map4Select.find(nXh)!=Map4Select.end())
						Map4Select[nXh]++;
					else 
						Map4Select[nXh] = 1;
					
					if(k==3)
						SelectSingle.push_back(nXh);
				}
			}
			map<int, int, greater<int> >::iterator iterSel = Map4Select.begin();
			for(;iterSel!=Map4Select.end();iterSel++)
			{
				if((*iterSel).second>=4)
					SelectedAll.push_back((*iterSel).first);
			}
			if(SelectedAll.empty())
			{
				iterSel = Map4Select.begin();
				for(;iterSel!=Map4Select.end();iterSel++)
				{
					if((*iterSel).second>=3)
						SelectedAll.push_back((*iterSel).first);
				}
			}
			if(SelectedAll.empty())
			{
				iterSel = Map4Select.begin();
				for(;iterSel!=Map4Select.end();iterSel++)
				{
					if((*iterSel).second>=2)
						SelectedAll.push_back((*iterSel).first);
				}
			}
			if(SelectedAll.empty())
			{
				for(int j=0;j<SelectSingle.size();j++)
					SelectedAll.push_back(SelectSingle[j]);
			}
			//All
			for(int j=0;j<SelectedAll.size();j++)
			{
				int nXh = SelectedAll[j];
				if(BestRangeItems.find(nXh)==BestRangeItems.end())
				{
					BestRangeItems.insert(nXh);

					TradeCalcInfo tci = m_AllTradeInfo[nXh];
					m_BestProfitLine[tci.tpi.nRawPos_Start].ActionIO.push_back(nXh);
					m_BestProfitLine[tci.tpi.nRawPos_End].ActionIO.push_back(nXh);

					if(bLastPos)
						LastInRangeItems.push_back(nXh);

					//add static
					BOOL bExist = FALSE;
					for(int n=0;n<vGoodMatch_s.size();n++)
					{
						if(memcmp(&tci.cp, &vGoodMatch_s[n].cp, sizeof(CalcParam))==0)
						{
							vGoodMatch_s[n].fScore1+=1.0;
							vGoodMatch_s[n].fProfitSum += tci.tpi.fProfit;
							vGoodMatch_s[n].fGapSum += tci.tpi.fGap;
							vGoodMatch_s[n].nRangeSum += (tci.tpi.nRawPos_End-tci.tpi.nRawPos_Start+1);
							vGoodMatch_s[n].nCount++;
							bExist = TRUE;
							break;
						}
					}
					if(!bExist)
					{
						MatchParamInfo mpi={0};
						memcpy(&mpi.cp, &tci.cp, sizeof(CalcParam));
						mpi.fScore1 = 1.0;
						mpi.fProfitSum = tci.tpi.fProfit;
						mpi.fGapSum = tci.tpi.fGap;
						mpi.nRangeSum = (tci.tpi.nRawPos_End-tci.tpi.nRawPos_Start+1);
						mpi.nCount = 1;
						vGoodMatch_s.push_back(mpi);
					}
				}
			}
		//	if(++nSleepCount_1%50==0)
		//		Sleep(1);
		}
	}
	//filt static
	map<double, int, greater_equal<double> > DataVal_s;
	for(i=0;i<vGoodMatch_s.size();i++)
		DataVal_s[vGoodMatch_s[i].fScore1] = i;
	vector<int> Selected_s;
	FiltBestFrom(0.0262, Selected_s, DataVal_s);

	DataVal_s.clear();
	for(i=0;i<Selected_s.size();i++)
	{
		int nXh = Selected_s[i];
		BestMatchs_Static.push_back(vGoodMatch_s[nXh]);
		/*
		MatchParamInfo mpi = vGoodMatch_s[nXh];
		double fNearTradeFlag = ((double)mpi.nRangeSum/(double)mpi.nCount)/(double)(m_nDataNum-mpi.cp.lHLRawPos);
		DataVal_s[fNearTradeFlag] = nXh;
		*/
	}
	/*
	Selected_s.clear();
	FiltBestFrom(0.10, Selected_s, DataVal_s);	
	for(i=0;i<Selected_s.size();i++)
	{
		int nXh = Selected_s[i];
		BestMatchs_Static.push_back(vGoodMatch_s[nXh]);
	}
	*/

	//
	if(LastInRangeItems.empty()) return;

	vector<MatchParamInfo> vGoodMatch_d;
	for(i=0;i<LastInRangeItems.size();i++)
	{
		int nXh = LastInRangeItems[i];
		TradeCalcInfo tci_cur = m_AllTradeInfo[nXh];
		CalcParam cp_cur = tci_cur.cp;
		TradeProfitInfo tpi_cur = tci_cur.tpi;
		int nCurBSFlag = tpi_cur.nBSFlag;

		//find previous match
		vector<CalcParam> vPreviousMatch_cur;
		GetPreviousMatch(tci_cur, vPreviousMatch_cur);

		for(int k=0;k<m_BestProfitLine.size();k++)
		{
			for(int j=0;j<m_BestProfitLine[k].ActionIO.size();j++)
			{
				int nHisXh = m_BestProfitLine[k].ActionIO[j];
				TradeCalcInfo tci_his = m_AllTradeInfo[nHisXh];
				CalcParam cp_his = tci_his.cp;
				TradeProfitInfo tpi_his = tci_his.tpi;
				int nHisBSFlag = tpi_his.nBSFlag;
				if(memcmp(&cp_cur, &cp_his, sizeof(CalcParam))==0&&
					nCurBSFlag == nHisBSFlag)					//匹配到历史相同参数
				{
					//find previous match
					vector<CalcParam> vPreviousMatch_his;
					GetPreviousMatch(tci_his, vPreviousMatch_his);
					int nPreMatchCout = 0;
					for(int m=0;m<vPreviousMatch_his.size();m++)
					{
						CalcParam cp_his = vPreviousMatch_his[m];
						for(int n=0;n<vPreviousMatch_cur.size();n++)
						{
							CalcParam cp_cur = vPreviousMatch_cur[n];
							if(memcmp(&cp_his, &cp_cur, sizeof(CalcParam))==0)
							{
								nPreMatchCout++;
								break;
							}
						}
					}
					double fPreMatchFlag = 100.0*(double)nPreMatchCout/((double)vPreviousMatch_cur.size()+(double)vPreviousMatch_his.size());

					//calc next pair item's range
					vector<TradeCalcInfo> vNextMatch_his;
					GetNextMatch(tci_his, vNextMatch_his);
					for(m=0;m<vNextMatch_his.size();m++)
					{
						BOOL bExist = FALSE;
						TradeCalcInfo tci_his_next = vNextMatch_his[m];
						for(int n=0;n<vGoodMatch_d.size();n++)
						{
							if(memcmp(&tci_his_next.cp, &vGoodMatch_d[n].cp, sizeof(CalcParam))==0)
							{
								vGoodMatch_d[n].fScore1+=1.0;
								vGoodMatch_d[n].fScore2+=fPreMatchFlag;
								vGoodMatch_d[n].fProfitSum += tci_his_next.tpi.fProfit;
								vGoodMatch_d[n].fGapSum += tci_his_next.tpi.fGap;
								vGoodMatch_d[n].nRangeSum += (tci_his_next.tpi.nRawPos_End-tci_his_next.tpi.nRawPos_Start+1);
								vGoodMatch_d[n].nCount++;
								bExist = TRUE;
								break;
							}
						}
						if(!bExist)
						{
							CalcParam nNowCp = tci_his_next.cp;
							//检验最新数据
							BOOL bGoodMatch = FALSE;
							if(nNowCp.nRawPos_Start<=tpi_cur.nRawPos_End&&
								tpi_cur.nRawPos_End-nNowCp.nRawPos_Start+1<0.2f*(float)(tpi_cur.nRawPos_End-tpi_cur.nRawPos_Start+1)+0.5f)
								bGoodMatch = TRUE;
							else if(nNowCp.nRawPos_Start>=tpi_cur.nRawPos_End&&
								nNowCp.nRawPos_LastAct>0&&nNowCp.nRawPos_LastAct<tpi_cur.nRawPos_End)
								bGoodMatch = TRUE;

							if(bGoodMatch)
							{
								MatchParamInfo mpi={0};
								memcpy(&mpi.cp, &tci_his_next.cp, sizeof(CalcParam));
								mpi.fScore1 = 1.0; 
								mpi.fScore2 = fPreMatchFlag;
								mpi.fGapSum = tci_his_next.tpi.fGap;
								mpi.fProfitSum = tci_his_next.tpi.fProfit;
								mpi.nRangeSum = (tci_his_next.tpi.nRawPos_End-tci_his_next.tpi.nRawPos_Start+1);
								mpi.nCount = 1;
								vGoodMatch_d.push_back(mpi);
							}
						}
					}
				//	if(++nSleepCount_2%33==0)
				//		Sleep(1);
				}
			}
		}
	}

	map<double, int, greater_equal<double> > DataVal_d;
	for(i=0;i<vGoodMatch_d.size();i++)
	{
		double fScore = pow(vGoodMatch_d[i].fScore1, 1.0)*pow(vGoodMatch_d[i].fScore2, 1.618);
		DataVal_d[fScore] = i;
	}
	vector<int> Selected_d;
	FiltBestFrom(0.1, Selected_d, DataVal_d);
	
	DataVal_d.clear();
	for(i=0;i<Selected_d.size();i++)
	{
		int nXh = Selected_d[i];
		BestMatchs_Dyna.push_back(vGoodMatch_d[nXh]);
		/*
		MatchParamInfo mpi = vGoodMatch_d[nXh];
		double fNearTradeFlag = ((double)mpi.nRangeSum/(double)mpi.nCount)/(double)(m_nDataNum-mpi.cp.lHLRawPos);
		DataVal_d[fNearTradeFlag] = nXh;
		*/
	}
	/*
	Selected_d.clear();
	FiltBestFrom(0.10, Selected_d, DataVal_d);
	for(i=0;i<Selected_d.size();i++)
	{
		int nXh = Selected_d[i];
		BestMatchs_Dyna.push_back(vGoodMatch_d[nXh]);
	}
	*/
}

void	COneCalc::GetPreviousMatch(TradeCalcInfo TargetItem, vector<CalcParam> &vPreviousMatch)
{	
	vPreviousMatch.clear();
	TradeProfitInfo tpi_cur = TargetItem.tpi;
	int nCurBSFlag = tpi_cur.nBSFlag;
	
	int nItemPosRange = tpi_cur.nRawPos_End-tpi_cur.nRawPos_Start+1;
	int nSearchStart = max(0, tpi_cur.nRawPos_Start-(0.2f*(float)nItemPosRange+0.5f));
	int nSearchEnd = tpi_cur.nRawPos_Start+(0.2f*(float)nItemPosRange+0.5f);
	for(int s=nSearchStart;s<=nSearchEnd;s++)
	{
		for(int sj=0;sj<m_BestProfitLine[s].ActionIO.size();sj++)
		{
			int nInSearchXh = m_BestProfitLine[s].ActionIO[sj];
			TradeCalcInfo tci_src = m_AllTradeInfo[nInSearchXh];
			TradeProfitInfo tpi_src = tci_src.tpi;
			BOOL bGoodMatch = FALSE;
			if(tpi_src.nBSFlag+nCurBSFlag==0&&
				tpi_src.nRawPos_Start<tpi_cur.nRawPos_Start&&
				tpi_src.nRawPos_End>=tpi_cur.nRawPos_Start&&
				tpi_src.nRawPos_End-tpi_cur.nRawPos_Start+1<0.2f*(float)(tpi_cur.nRawPos_End-tpi_cur.nRawPos_Start+1)+0.5f&&
				tpi_src.nRawPos_End-tpi_cur.nRawPos_Start+1<0.2f*(float)(tpi_src.nRawPos_End-tpi_src.nRawPos_Start+1)+0.5f)
				bGoodMatch = TRUE;
			else if(tpi_src.nBSFlag+nCurBSFlag==0&&
				tpi_src.nRawPos_End<=tpi_cur.nRawPos_Start&&
				tpi_cur.nRawPos_LastAct>0&&tpi_cur.nRawPos_LastAct<tpi_src.nRawPos_End)
				bGoodMatch = TRUE;
			if(bGoodMatch)
				vPreviousMatch.push_back(tci_src.cp);
		}
	}
}

void	COneCalc::GetNextMatch(TradeCalcInfo TargetItem, vector<TradeCalcInfo> &vNextMatch)
{	
	vNextMatch.clear();
	TradeProfitInfo tpi_cur = TargetItem.tpi;
	int nCurBSFlag = tpi_cur.nBSFlag;
	
	int nItemPosRange = tpi_cur.nRawPos_End-tpi_cur.nRawPos_Start+1;
	int nSearchStart = tpi_cur.nRawPos_End-(0.2f*(float)nItemPosRange+0.5f);
	int nSearchEnd = tpi_cur.nRawPos_End+(0.2f*(float)nItemPosRange+0.5f);
	for(int s=nSearchStart;s<=nSearchEnd;s++)
	{
		for(int sj=0;sj<m_BestProfitLine[s].ActionIO.size();sj++)
		{
			int nInSearchXh = m_BestProfitLine[s].ActionIO[sj];
			TradeCalcInfo tci_src = m_AllTradeInfo[nInSearchXh];
			TradeProfitInfo tpi_src = tci_src.tpi;
			BOOL bGoodMatch = FALSE;
			if(tpi_src.nBSFlag+nCurBSFlag==0&&
				tpi_src.nRawPos_Start<=tpi_cur.nRawPos_End&&
				tpi_src.nRawPos_End>tpi_cur.nRawPos_End&&
				tpi_cur.nRawPos_End-tpi_src.nRawPos_Start+1<0.2f*(float)(tpi_cur.nRawPos_End-tpi_cur.nRawPos_Start+1)+0.5f&&
				tpi_cur.nRawPos_End-tpi_src.nRawPos_Start+1<0.2f*(float)(tpi_src.nRawPos_End-tpi_src.nRawPos_Start+1)+0.5f)
				bGoodMatch = TRUE;
			else if(tpi_src.nBSFlag+nCurBSFlag==0&&
				tpi_src.nRawPos_Start>=tpi_cur.nRawPos_End&&
				tpi_src.nRawPos_LastAct>0&&tpi_src.nRawPos_LastAct<tpi_cur.nRawPos_End)
				bGoodMatch = TRUE;
			if(bGoodMatch)
				vNextMatch.push_back(tci_src);
		}
	}
}

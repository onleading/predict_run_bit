// HostAHq.cpp: implementation of the CHostAHq class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IDefine.h"
#include "HostAHq.h"
#include "PoloTrade.h"

#ifdef _DEBUG	
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include	"..\zconf.h"
#include	"..\zlib.h"
//////////////////////////////////////////////////////////////////////////
CCriticalSection Logcs;
char	Share_Folder[MAX_PATH]={0};
///////////////////////////////////////////////////////////////////////////////////
//工具函数
extern char GetPrecise ( char * code,short setcode);
extern BOOL testzs(const char * code,short setcode);
extern void CurrStockData_rtn ( char * buf, struct CurrStockData * np, short &len, BOOL b5MMP, BOOL bAllInfo);
extern void CurrStockData_Code_rtn ( char * buf, struct CurrStockData_Code * np, short &len ,BOOL b5MMP,BOOL bAllInfo);
//
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////
CHostAHq::CHostAHq()
{
	m_nSourceType = DATA_HOSTA;
	m_hOwnerWnd = NULL;

	m_pAnaData = NULL;
	m_pCalcInst = NULL;

	memset(&m_LastLogTime, 0, sizeof(long_short));
	m_CachedHisData.clear();
}

CHostAHq::~CHostAHq()
{
	DELETE_PTRA(m_pCalcInst);
	for(int s=0;s<m_TradeTestInfo.size();s++)
	{
		TradeTestInfo CurTti = m_TradeTestInfo[s];
		DELETE_PTRA(CurTti.m_pCalcInst);
	}
}

void	CHostAHq::NetRegister(HWND hMsgWnd, UINT MsgID, long lEvent, int iNo)
{
	m_hOwnerWnd = hMsgWnd;
	m_iNo = iNo;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
extern long g_nInterReq;
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void	CHostAHq::GetKLineFromPoloniex(char *currency, int nPerSeed, vector<AnalyData> &CurHisData)
{
	char currencyPair[20]={0};
	sprintf(currencyPair, "USDT_%s", currency);
	
	int nMaxUnit= 1;
	int nCalcDays = 14;
	long nMinStartPos = nMaxUnit*3*60;
	long nDownDays = (1+(nMinStartPos+nCalcDays*(1440*60/nPerSeed))/(1440*60/nPerSeed));
	//prehis
	if(m_PreHisData.empty()||m_nPreEndTT<1||m_dMultyply<COMPPREC)
	{
		char strPreFile[100]={0};
		sprintf(strPreFile, "%s_Pre.txt", currencyPair);
		char url[MAX_PATH]={0};
		time_t tt = time(NULL);
		long startt = tt-tt%(60*60*24)-60*60*24*nDownDays;
		m_nPreEndTT = tt-tt%(60*60*24)-60*60*24;
		sprintf(url, "https://poloniex.com/public?command=returnChartData&currencyPair=%s&start=%d&end=%d&period=%d",currencyPair,startt,m_nPreEndTT,nPerSeed);
		WebContentDownload(url, strPreFile);
		m_PreHisData.clear();
		GetHistoryData_bitc(m_PreHisData, m_dMultyply, strPreFile);
	}
	
	char strCurFile[100]={0};
	sprintf(strCurFile, "%s_Cur.txt", currencyPair);
	char url[MAX_PATH]={0};
//	long nTestEndTT = 1590228000;
//	sprintf(url, "https://poloniex.com/public?command=returnChartData&currencyPair=%s&start=%d&end=%d&period=%d",currencyPair,m_nPreEndTT,nTestEndTT,nPerSeed);
	sprintf(url, "https://poloniex.com/public?command=returnChartData&currencyPair=%s&start=%d&end=9999999999&period=%d",currencyPair,m_nPreEndTT,nPerSeed);
	
	WebContentDownload(url, strCurFile);
	CurHisData.clear();
	GetHistoryData_bitc(CurHisData, m_dMultyply, strCurFile);
	
	if(memcmp(&m_PreHisData[m_PreHisData.size()-1], &CurHisData[0], sizeof(AnalyData)))
		AfxMessageBox("Data Time Not Connected!");
}
*/

void	CHostAHq::GetKLineFromBinance(char *currency, TestParam tp, vector<AnalyData> &CurHisData)
{
	char currencyPair[20]={0};
	sprintf(currencyPair, "%sUSDT", currency);

	int nPerSeed = tp.PerSeed;
	int nMaxUnit= min(MAX_UNIT, (float)tp.nUnitParam_End/12.0);
	int nCalcDays = tp.nCalcDays;
	char fileString[MAX_PATH] = {0};
	int nOneReqLimit = 1000;
	
	char *dataString = "5m";
	sprintf(fileString, "%d", nPerSeed);
	long nMinStartPos = nMaxUnit*3*72;
//	long nDownDays = (1+(nMinStartPos+nCalcDays*(1440*60/nPerSeed))/(1440*60/nPerSeed));
	long nDownDays = (float)(nMinStartPos+(1440/5)*nCalcDays)/(1440.0/5.0)+1.0;
	
	//prehis
	map<int, PreCachedHisData>::iterator iter = m_CachedHisData.find(nPerSeed);
	if(iter==m_CachedHisData.end())
	{
		PreCachedHisData NewCachedData;
		NewCachedData.m_PreHisData.clear();
		NewCachedData.m_dMultyply = 0.0;

		time_t tt = time(NULL);
		long nInitStart = tt-tt%(60*60*24)-60*60*24*nDownDays;
		NewCachedData.m_nPreEndTT = tt-tt%(60*60*24)-60*60*24;

		char strPreFile[100]={0};
		sprintf(strPreFile, "%s_Binance_%s_pre.txt", currencyPair, fileString);
		time_t ttStart = nInitStart;
		while(ttStart<NewCachedData.m_nPreEndTT)
		{
			char url[MAX_PATH]={0};	
			sprintf(url, "https://www.binance.com/api/v3/klines?symbol=%s&interval=%s&startTime=%d000&endTime=%d000&limit=%d",currencyPair,dataString,ttStart,NewCachedData.m_nPreEndTT,nOneReqLimit);
			WebContentDownload(url, strPreFile);
			vector<AnalyData> DataRemote;
			GetHistoryData_binance(DataRemote, NewCachedData.m_dMultyply, ttStart, strPreFile);
			if(DataRemote.empty())
				continue;
			int nStart=1;
			if(NewCachedData.m_PreHisData.empty()) 
				nStart = 0;
			for(int i=nStart;i<DataRemote.size();i++)
				NewCachedData.m_PreHisData.push_back(DataRemote[i]);
			
			Sleep(50);
		}
		m_CachedHisData[nPerSeed] = NewCachedData;
	}

	iter = m_CachedHisData.find(nPerSeed);
	if(iter==m_CachedHisData.end())
		AfxMessageBox("Can't Find PreData!");
	PreCachedHisData ExistCachedData = (*iter).second;

	//CurData
	time_t ttStart = ExistCachedData.m_nPreEndTT;
	char strCurFile[100]={0};
	sprintf(strCurFile, "%s_Binance_%s_cur.txt", currencyPair, fileString);
	CurHisData.clear();
	m_CachedHisData[nPerSeed].m_AppendHisData.clear();
	while(TRUE)
	{
		char url[MAX_PATH]={0};
		long nReqNum = nOneReqLimit;
		sprintf(url, "https://www.binance.com/api/v3/klines?symbol=%s&interval=%s&startTime=%d000&limit=%d",currencyPair,dataString,ttStart,nReqNum);
		WebContentDownload(url, strCurFile);
		vector<AnalyData> DataRemote;
		GetHistoryData_binance(DataRemote, ExistCachedData.m_dMultyply, ttStart, strCurFile);
		if(DataRemote.empty())
			continue;
		for(int i=1;i<DataRemote.size();i++)
			m_CachedHisData[nPerSeed].m_AppendHisData.push_back(DataRemote[i]);
		if(DataRemote.size()<nReqNum)
			break;
		Sleep(50);
	}
	CurHisData = m_CachedHisData[nPerSeed].m_AppendHisData;

}

//1*12=12
//30*72=2160
//12	155.2	441.6	1014.4	2160
//60*72=2160
//12	299.2	873.6	2022.4	4320
//
/*
12
150.96774193548387096774193548387
428.90322580645161290322580645161
984.77419354838709677419354838709
2,096.5161290322580645161290322581
4320

12
290.32258064516129032258064516129
846.96774193548387096774193548387
1,960.258064516129032258064516129
4,186.8387096774193548387096774193
8640
*/
void	CHostAHq::SetTestParamSet(char *ZBCode, char *currency, int nGroupNum)
{
	m_TradeTestInfo.clear();

	TradeInfo ti={0};
	ti.m_bStart2Trade = TRUE;

	double dwGapFlag = 1.618;
//	int	nBaseDays = 21;
	int	nBaseDays = 14;
	double dwDayFlag = 1.33;
	int	 nStartParamFlag = 12, nEndParamFlag = MAX_UNIT*72;

	double dwBaseFlagSum = 0;	
	for(int i=0;i<nGroupNum;i++)
		dwBaseFlagSum += pow(dwGapFlag, i);
	double dwBaseGap = (double)(nEndParamFlag-nStartParamFlag)/dwBaseFlagSum;
	double dwTraceGap = nStartParamFlag, dwTraceDays = nBaseDays;
	for(i=0;i<nGroupNum;i++)
	{
		TestParam	tp={0};
		tp.PerSeed = i;
		tp.nUnitParam_Start=dwTraceGap+(1.0-COMPPREC);
		double dwThisGap = dwBaseGap*pow(dwGapFlag, i);
		tp.nUnitParam_End = dwTraceGap = (double)tp.nUnitParam_Start+dwThisGap;
		tp.nCalcDays = dwTraceDays+0.5;
		dwTraceDays *= dwDayFlag;

		tp.TradeFee = REAL_TRADE_FEE;
		TradeTestInfo tti;
		strncpy(tti.Currency, currency, 20);
		strncpy(tti.ZBCode, ZBCode, 20);
		tti.m_TestParam = tp;
		tti.m_TradeInfo = ti;
		m_TradeTestInfo.push_back(tti);

		TradeSingnal ts={0};
		m_LastSingnal.push_back(ts);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
double  GetStaticSwingPos(MatchParamInfo mpi)
{
	double fProfit = mpi.fProfitSum/(double)mpi.nCount;
	BOOL bThisPositive = (mpi.fProfitSum>COMPPREC);
	if(!bThisPositive)
		fProfit = 0.618/(0.618+fabs(fProfit));
	double fNowPrice = mpi.cp.fAverPrice;

	double dwSwingPos = 0.5;
	float fAverGap = mpi.fGapSum/(float)mpi.nCount;
	double dReverseRange = fAverGap*mpi.cp.fHLValue/100.0;
	if(mpi.cp.nBSFlag==1)	//on up
	{	
		double dwProfSpan = max(0.618*fProfit, 0.618*fProfit+0.382*100.0*(mpi.cp.fLastActPrice-mpi.cp.fActPrice)/mpi.cp.fLastActPrice);

		double dwTop = mpi.cp.fActPrice*(1.0+dwProfSpan/100.0)+dReverseRange;
		double dwBottom = mpi.cp.fHLValue;
		double dwTopTop = dwTop+dReverseRange;
		double dwBottomBottom = dwBottom-dReverseRange;

		if(fNowPrice>dwTopTop)
			dwSwingPos = 1.0;
		else if(fNowPrice<dwBottomBottom)
			dwSwingPos = 0.0;
		else if(fNowPrice>dwTop&&fNowPrice<=dwTopTop)		//0.5->1.0
			dwSwingPos = 0.5+0.5*(fNowPrice-dwTop)/(dwTopTop-dwTop);
		else if(fNowPrice<dwBottom&&fNowPrice>=dwBottomBottom)	//0.5->0.0
			dwSwingPos = 0.5*(fNowPrice-dwBottomBottom)/(dwBottom-dwBottomBottom);
	}
	else if(mpi.cp.nBSFlag==-1)	//on down
	{
		double dwProfSpan = max(0.618*fProfit, 0.618*fProfit+0.382*100.0*(mpi.cp.fActPrice-mpi.cp.fLastActPrice)/mpi.cp.fLastActPrice);

		double dwTop = mpi.cp.fHLValue;
		double dwBottom = mpi.cp.fActPrice*(1.0-dwProfSpan/100.0)-dReverseRange;
		double dwTopTop = dwTop+dReverseRange;
		double dwBottomBottom = dwBottom-dReverseRange;
		
		if(fNowPrice>dwTopTop)
			dwSwingPos = 1.0;
		else if(fNowPrice<dwBottomBottom)
			dwSwingPos = 0.0;
		else if(fNowPrice>dwTop&&fNowPrice<=dwTopTop)		//0.5->1.0
			dwSwingPos = 0.5+0.5*(fNowPrice-dwTop)/(dwTopTop-dwTop);
		else if(fNowPrice<dwBottom&&fNowPrice>=dwBottomBottom)	//0.5->0.0
			dwSwingPos = 0.5*(fNowPrice-dwBottomBottom)/(dwBottom-dwBottomBottom);
	}
	return dwSwingPos;
}
*/


double  GetStaticSwingPos(MatchParamInfo mpi)
{
	double fProfit = mpi.fProfitSum/(double)mpi.nCount;
	BOOL bThisPositive = (mpi.fProfitSum>COMPPREC);
	if(!bThisPositive)
		fProfit = 0.618/(0.618+fabs(fProfit));
	
	double dwSwingPos = 0.5;
	double fNowPrice = mpi.cp.fActPrice;
	float fAverGap = mpi.fGapSum/(float)mpi.nCount;
	double dReverseRange = fAverGap*mpi.cp.fHLValue/100.0;
	if(mpi.cp.nBSFlag==1)	//on up
	{
		double dwProfSpan = max(0.618*fProfit, 0.618*fProfit+0.382*100.0*(mpi.cp.fLastActPrice-mpi.cp.fActPrice)/mpi.cp.fLastActPrice);
		
		double dwTop = mpi.cp.fActPrice*(1.0+dwProfSpan/100.0)+dReverseRange;
		double dwBottom = mpi.cp.fHLValue;
		
		double dWholeRatio = pow((dwTop-fNowPrice)/(dwTop-dwBottom),0.5);
		dwSwingPos  = dWholeRatio;
	}
	else if(mpi.cp.nBSFlag==-1)	//on down
	{
		double dwProfSpan = max(0.618*fProfit, 0.618*fProfit+0.382*100.0*(mpi.cp.fActPrice-mpi.cp.fLastActPrice)/mpi.cp.fLastActPrice);
		
		double dwTop = mpi.cp.fHLValue;
		double dwBottom = mpi.cp.fActPrice*(1.0-dwProfSpan/100.0)-dReverseRange;
		
		double dWholeRatio = pow((dwTop-fNowPrice)/(dwTop-dwBottom),1.5);
		dwSwingPos  = dWholeRatio;	
	}
	return dwSwingPos;
}

/*
double  GetStaticSwingPos(MatchParamInfo mpi)
{	
	double dwPosFlag = 0.5;
	if(mpi.cp.nBSFlag==1)			//on up
		dwPosFlag = 1.0;
	else if(mpi.cp.nBSFlag==-1)	//on down
		dwPosFlag = 0.0;
	return dwPosFlag;
}
*/

double  GetStaticRatio(MatchParamInfo mpi, double dwPostSwing)
{
	double fProfit = mpi.fProfitSum/(double)mpi.nCount;
	BOOL bThisPositive = (mpi.fProfitSum>COMPPREC);
	if(!bThisPositive)
		fProfit = 0.618/(0.618+fabs(fProfit));
	double fNowPrice = mpi.cp.fNow;

	double dwPosFlag = 0.5;
	double dwUpPart = 0.5*(1.0-dwPostSwing);
	double dwDownPart = 0.5*dwPostSwing;
	double dwOnTopFlag = dwPostSwing-min(dwDownPart, 1.5*dwUpPart);
	double dwOnBottomFlag = dwPostSwing+min(dwUpPart, 1.5*dwDownPart);
//	if(dwPostSwing<0.5)
//		dwOnBottomFlag = max(dwOnTopFlag, pow(2.0*dwPostSwing, 2.0));
	float fAverGap = mpi.fGapSum/(float)mpi.nCount;
	double dReverseRange = fAverGap*mpi.cp.fHLValue/100.0;
	if(mpi.cp.nBSFlag==1)	//on up
	{
		double dwProfSpan = max(0.618*fProfit, 0.618*fProfit+0.382*100.0*(mpi.cp.fLastActPrice-mpi.cp.fActPrice)/mpi.cp.fLastActPrice);
	//	double dwProfSpan = fProfit;
		
		double dwTop = mpi.cp.fActPrice*(1.0+dwProfSpan/100.0)+dReverseRange;
		double dwBottom = mpi.cp.fHLValue;
		double dwTopTop = dwTop+dReverseRange;
		double dwBottomBottom = dwBottom-dReverseRange;

		if(fNowPrice<dwBottom)
			dwPosFlag = dwOnBottomFlag;
		/*
		else if(fNowPrice>dwTopTop)
			dwPosFlag = dwPostSwing;
		else if(fNowPrice>dwTop&&fNowPrice<=dwTopTop)
			dwPosFlag = dwOnTopFlag+(dwPostSwing-dwOnTopFlag)*(fNowPrice-dwTop)/(dwTopTop-dwTop);
		*/
		else if(fNowPrice>dwTop)
			dwPosFlag = dwOnTopFlag;
		else 
		{
			double dWholeRatio = 0.5;
			if(fNowPrice<mpi.cp.fActPrice)
				dWholeRatio = pow((dwTop-fNowPrice)/(dwTop-dwBottom),1.0);
			else if(fNowPrice<dwTop-dReverseRange)
				dWholeRatio = pow((dwTop-fNowPrice)/(dwTop-dwBottom),1.0);
			else 
				dWholeRatio = pow((dwTop-fNowPrice)/(dwTop-dwBottom),1.0);
			
			dwPosFlag  = dwOnTopFlag+dWholeRatio*(dwOnBottomFlag-dwOnTopFlag);
		}
	}
	else if(mpi.cp.nBSFlag==-1)	//on down
	{
		double dwProfSpan = max(0.618*fProfit, 0.618*fProfit+0.382*100.0*(mpi.cp.fActPrice-mpi.cp.fLastActPrice)/mpi.cp.fLastActPrice);
	//	double dwProfSpan = fProfit;

		double dwTop = mpi.cp.fHLValue;
		double dwBottom = mpi.cp.fActPrice*(1.0-dwProfSpan/100.0)-dReverseRange;
		double dwTopTop = dwTop+dReverseRange;
		double dwBottomBottom = dwBottom-dReverseRange;

		if(fNowPrice<dwBottom)
			dwPosFlag = dwOnBottomFlag;
		/*
		else if(fNowPrice>dwTopTop)
			dwPosFlag = dwPostSwing;
		else if(fNowPrice>dwTop&&fNowPrice<=dwTopTop)
			dwPosFlag = dwOnTopFlag+(dwPostSwing-dwOnTopFlag)*(fNowPrice-dwTop)/(dwTopTop-dwTop);
		*/
		else if(fNowPrice>dwTop)
			dwPosFlag = dwOnTopFlag;
		else 
		{
			double dWholeRatio = 0.5;
			if(fNowPrice>mpi.cp.fActPrice)
				dWholeRatio = pow((dwTop-fNowPrice)/(dwTop-dwBottom),1.618);
			else if(fNowPrice>dwBottom+dReverseRange)
				dWholeRatio = pow((dwTop-fNowPrice)/(dwTop-dwBottom),1.309);
			else 
				dWholeRatio = pow((dwTop-fNowPrice)/(dwTop-dwBottom),1.0);
			
			dwPosFlag  = dwOnTopFlag+dWholeRatio*(dwOnBottomFlag-dwOnTopFlag);
		}
		
	}
	return dwPosFlag;
}

void	ProceedTradeAndLog(char *currency, vector<MatchParamInfo> BestMatchs_s, vector<MatchParamInfo> BestMatchs_d, TradeInfo &TTinfo, AnalyData iLastData, long nDataNum, double dwPostSwing, char *LogDict)
{
	if(TTinfo.m_bStart2Trade)
	{
		char strInitFund[20]={0};
		GetPrivateProfileString("POOL","InitFund","50000.0",strInitFund,1000,g_WSXHStr+"POOLInfo_Bit.ini");
		TTinfo.m_dwAvalFund = atof(strInitFund);
		TTinfo.m_dwInVol = 0.0;
		TTinfo.m_dwCurInRatio = 0.0;
	}
	TTinfo.m_dwLastInRatio = TTinfo.m_dwCurInRatio;
	
	double dwSwingPart=0.0, dwBoDongPart=1.0;
	//static
	double dwPartSum_s = 0.0, dwNewInRatio_Sum_s = 0.0;
	for(int i=0;i<BestMatchs_s.size();i++)
	{
		MatchParamInfo mpi = BestMatchs_s[i];
		/*
		float fProfit = mpi.fProfitSum/(float)mpi.nCount;
		double dwPart = fProfit;
		BOOL bThisPositive = (mpi.fProfitSum>COMPPREC);
		if(!bThisPositive)
			dwPart = 0.618/(0.618+fabs(fProfit));
		*/
		double dwPart = 1.0;

		dwNewInRatio_Sum_s += dwBoDongPart*dwPart*GetStaticRatio(mpi, dwPostSwing);
		dwPartSum_s += dwPart;
	}
	double dwNewInRatio_s = 0.0;
	if(dwPartSum_s>COMPPREC)
		dwNewInRatio_s = dwNewInRatio_Sum_s/dwPartSum_s;
	//dyna
	double dwPartSum_d = 0.0, dwNewInRatio_Sum_d = 0.0;
	for(i=0;i<BestMatchs_d.size();i++)
	{
		MatchParamInfo mpi = BestMatchs_d[i];
		/*
		float fProfit = mpi.fProfitSum/(float)mpi.nCount;
		double dwPart = fProfit;
		BOOL bThisPositive = (mpi.fProfitSum>COMPPREC);
		if(!bThisPositive)
			dwPart = 0.618/(0.618+fabs(fProfit));
		*/
		double dwPart = 1.0;
		
		dwNewInRatio_Sum_d += dwBoDongPart*dwPart*GetStaticRatio(mpi, dwPostSwing);
		dwPartSum_d += dwPart;
	}
	double dwNewInRatio_d = 0.0;
	if(dwPartSum_d>COMPPREC)
		dwNewInRatio_d = dwNewInRatio_Sum_d/dwPartSum_d;

	double dwNewInRatio = dwPostSwing;
	if(!BestMatchs_s.empty()&&!BestMatchs_d.empty())
		dwNewInRatio = min(dwNewInRatio_s, dwNewInRatio_d);
	else if(BestMatchs_s.empty()&&BestMatchs_d.empty())
		dwNewInRatio = dwPostSwing;
	else 
		dwNewInRatio = max(dwNewInRatio_s,dwNewInRatio_d);

//	dwNewInRatio = min(1.0, dwNewInRatio);
//	dwNewInRatio = max(0.0, dwNewInRatio);
//////////////////////////////////////////////////////////

	if(fabs(dwNewInRatio-TTinfo.m_dwCurInRatio)>COMPPREC)		//need to trade
	{
		double dwAllBal = TTinfo.m_dwAvalFund+TTinfo.m_dwInVol*iLastData.Close;
		if(dwNewInRatio-TTinfo.m_dwCurInRatio>COMPPREC||dwNewInRatio-TTinfo.m_dwCurInRatio>0.08||fabs(dwNewInRatio-1.0)<COMPPREC)		//buy
		{
			if(TTinfo.m_bStart2Trade==TRUE||(TTinfo.m_fLastTradePrice>COMPPREC&&fabs(iLastData.Close-TTinfo.m_fLastTradePrice)/TTinfo.m_fLastTradePrice>TRADE_GAP))
			{
				char ActionPrice[100]={0};
				double dwBuyPrice = iLastData.Close;
				double dwUsedFund = min(TTinfo.m_dwAvalFund, dwAllBal*(dwNewInRatio-TTinfo.m_dwCurInRatio));
				double dwBuyVol = dwUsedFund/(dwBuyPrice*(1.0+(TTinfo.m_bStart2Trade?0.0:REAL_TRADE_FEE)));
				TTinfo.m_dwAvalFund -= dwUsedFund;
				TTinfo.m_dwInVol += dwBuyVol;
				
				TTinfo.m_bStart2Trade = FALSE;
				TTinfo.m_fLastTradePrice = dwBuyPrice;
				TTinfo.m_dwCurInRatio = dwNewInRatio;
			}
		}
		else if(TTinfo.m_dwCurInRatio-dwNewInRatio>COMPPREC||TTinfo.m_dwCurInRatio-dwNewInRatio>0.08||dwNewInRatio<COMPPREC)		//sell
		{
			if(TTinfo.m_bStart2Trade==TRUE||(TTinfo.m_fLastTradePrice>COMPPREC&&fabs(iLastData.Close-TTinfo.m_fLastTradePrice)/TTinfo.m_fLastTradePrice>TRADE_GAP))
			{
				char ActionPrice[100]={0};
				double dwSellPrice = iLastData.Close;
				double dwSellVol = min(TTinfo.m_dwInVol, TTinfo.m_dwInVol-TTinfo.m_dwInVol*dwNewInRatio/TTinfo.m_dwCurInRatio);
				double dwBackFund = dwSellVol*dwSellPrice*(1.0-REAL_TRADE_FEE);
				TTinfo.m_dwAvalFund += dwBackFund;
				TTinfo.m_dwInVol -= dwSellVol;
				
				TTinfo.m_fLastTradePrice = dwSellPrice;
				TTinfo.m_dwCurInRatio = dwNewInRatio;
			}
		}
	}

	if(TRUE)
	{
		char FileName[MAX_PATH]={0};
		sprintf(FileName, "%s\\bestmatch_log.csv", LogDict);
		FILE *BestParamsFile = _fsopen(FileName,"a+",SH_DENYNO);
		if(BestParamsFile)
		{
			fprintf(BestParamsFile, "%.3f, %.3f, %.3f\n", dwPostSwing, TTinfo.m_dwCurInRatio, iLastData.Close);
			fclose(BestParamsFile);
		}
	}
	if(TRUE)
	{
		char FileName[MAX_PATH]={0};
		sprintf(FileName, "%s\\bestmatch_log-(%04d-%02d).txt", LogDict, iLastData.Time.Daye.Day, iLastData.Time.Daye.Minute/60);
		FILE *BestParamsFile = _fsopen(FileName,"a+",SH_DENYNO);
		if(BestParamsFile)
		{
			double dwCurBal = TTinfo.m_dwAvalFund+TTinfo.m_dwInVol*iLastData.Close;
			fprintf(BestParamsFile, "-------------------------------------------------\r\n");
			fprintf(BestParamsFile, "%d-%04d-%02d:%02d(%d):\t%.3f\t%.3f(%.3f,%.3f)\t%.3f\r\n", 
				iLastData.Time.Daye.Year+2004, iLastData.Time.Daye.Day, iLastData.Time.Daye.Minute/60, iLastData.Time.Daye.Minute%60, nDataNum, 
				iLastData.Close, dwCurBal, TTinfo.m_dwCurInRatio, dwPostSwing, dwCurBal/iLastData.Close);
			fprintf(BestParamsFile, "---------static---------\r\n");
			for(int k=0;k<BestMatchs_s.size();k++)
			{
				MatchParamInfo mpi = BestMatchs_s[k];
				if(mpi.nCount<1) continue;
				float fProfit = mpi.fProfitSum/(float)mpi.nCount;
				if(mpi.cp.nBSFlag==-1)
					fProfit = 0.0f-fProfit;
				fprintf(BestParamsFile, "%d\t%d|%d\t%.2f\t%d(%d)\t%.3f(%.3f. %.3f)\r\n", 
					mpi.cp.nUnit, mpi.cp.ZBParam[0], mpi.cp.ZBParam[1], fProfit, mpi.nRangeSum/mpi.nCount, nDataNum-mpi.cp.lHLRawPos+1, mpi.cp.fActPrice, mpi.cp.fHLValue, mpi.cp.fAverPrice);
			}
			fprintf(BestParamsFile, "---------dynamic---------\r\n");
			for(k=0;k<BestMatchs_d.size();k++)
			{
				MatchParamInfo mpi = BestMatchs_d[k];
				if(mpi.nCount<1) continue;
				float fProfit = mpi.fProfitSum/(float)mpi.nCount;
				if(mpi.cp.nBSFlag==-1)
					fProfit = 0.0f-fProfit;
				fprintf(BestParamsFile, "%d\t%d|%d\t%.2f\t%d(%d)\t%.3f(%.3f. %.3f)\r\n", 
					mpi.cp.nUnit, mpi.cp.ZBParam[0], mpi.cp.ZBParam[1], fProfit, mpi.nRangeSum/mpi.nCount, nDataNum-mpi.cp.lHLRawPos+1, mpi.cp.fActPrice, mpi.cp.fHLValue, mpi.cp.fAverPrice);
			}
			fclose(BestParamsFile);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
DWORD	WINAPI	AfxBestMatchesCalc(LPVOID lParam)
{
	TradeTestInfo *pCurTti = (TradeTestInfo*)(lParam);
	TestParam tp = pCurTti->m_TestParam;
	COneCalc *pOcr = pCurTti->m_pCalcInst;
	
	int nMinUnit=(float)(tp.nUnitParam_Start+71)/72.0;
	int nMaxUnit= min(MAX_UNIT, (float)tp.nUnitParam_End/12.0);
	CalcParam cpp_param={0};
	for(int nUnit=nMinUnit;nUnit<=nMaxUnit;nUnit++)
		pOcr->CalcRes_Show_Result(pCurTti->ZBCode, nUnit, cpp_param, tp.nUnitParam_Start, tp.nUnitParam_End);

	pCurTti->m_BestMatchs_s.clear();
	pCurTti->m_BestMatchs_d.clear();
	pOcr->FiltBestProfLine(pCurTti->m_BestMatchs_s, pCurTti->m_BestMatchs_d);
	/*
	vector<MatchParamInfo> BestMatchs_Static;
	pOcr->FiltBestProfLine_Static(BestMatchs_Static);
	for(int i=0;i<BestMatchs_Static.size();i++)
		pCurTti->m_BestMatchs.push_back(BestMatchs_Static[i]);
	*/

	return 0;
}

BOOL	CHostAHq::GetSwingPos(int nTestPart, double &dwPostSwing)
{	
	TradeTestInfo *pCurTti = &m_TradeTestInfo[nTestPart];
	if(pCurTti->m_BestMatchs_s.empty()&&pCurTti->m_BestMatchs_d.empty()) return FALSE;
	
	//static
	double dwSwingPosSum_s=0.0;
	for(int i=0;i<pCurTti->m_BestMatchs_s.size();i++)
	{
		MatchParamInfo mpi = pCurTti->m_BestMatchs_s[i];
		dwSwingPosSum_s += GetStaticSwingPos(mpi);
	}
	double dwPostSwing_s = 0.0;
	if(!pCurTti->m_BestMatchs_s.empty())
		dwPostSwing_s = dwSwingPosSum_s/(double)pCurTti->m_BestMatchs_s.size();

	//dyna
	double dwSwingPosSum_d=0.0;
	for(i=0;i<pCurTti->m_BestMatchs_d.size();i++)
	{
		MatchParamInfo mpi = pCurTti->m_BestMatchs_d[i];
		dwSwingPosSum_d += GetStaticSwingPos(mpi);
	}
	double dwPostSwing_d = 0.0;
	if(!pCurTti->m_BestMatchs_d.empty())
		dwPostSwing_d = dwSwingPosSum_d/(double)pCurTti->m_BestMatchs_d.size();

	if(!pCurTti->m_BestMatchs_s.empty()&&!pCurTti->m_BestMatchs_d.empty())
		dwPostSwing = min(dwPostSwing_s, dwPostSwing_d);
	else 
		dwPostSwing = max(dwPostSwing_s, dwPostSwing_d);

	return TRUE;
}

void	CHostAHq::GetSwingPos(double &dwPostSwing)
{
	dwPostSwing = 0.0;
	double dwPostSwing_Sum = 0.0;
	int nCount = 0;
	for(int s=TEST_START;s<=TEST_END;s++)
	{
		double dwPostSwing_Par = 0.0;
		if(GetSwingPos(s, dwPostSwing_Par))
		{
			dwPostSwing_Sum += dwPostSwing_Par;
			nCount++;
		}
	}
//	double dwUpLevel = 2.0/3.0, dwDownLevel = 1.0/3.0;
	if(nCount>0)
	{
		//!!!new calc
		double dwDynaSwing = dwPostSwing_Sum/(double)nCount;
		dwPostSwing = dwDynaSwing;
		/*
		if(dwDynaSwing>dwUpLevel)
			dwPostSwing = 0.5+0.5*(dwDynaSwing-dwUpLevel)/(1.0-dwUpLevel);
		else if(dwDynaSwing<dwDownLevel)
			dwPostSwing = 0.5*dwDynaSwing/dwDownLevel;
		*/
	}
}

void	CHostAHq::TradeFunc_bitc(char *ZBCode, char *currency)
{
	::KillTimer(m_hOwnerWnd, DATA_HOSTA);

	memset(Share_Folder, 0, sizeof(Share_Folder));
	GetShellPath("share_res.lnk", Share_Folder);
	if(strlen(Share_Folder)==0)
		GetCurrentDirectory(MAX_PATH,Share_Folder);

	if(m_TradeTestInfo.empty())
	{
		//
		WritePrivateProfileString("POOL","CALCING...","0",g_WSXHStr+"PoolInfo_Bit.ini");
		InitData(ZBCode, currency);
		WritePrivateProfileString("POOL","CALCING...","1",g_WSXHStr+"PoolInfo_Bit.ini");
	}
	//记录时间
	time_t tt = time(NULL);
	char strRefreshTimet[20]={0};
	sprintf(strRefreshTimet, "%d", tt);
	WritePrivateProfileString("POOL","REFRESH",strRefreshTimet,g_WSXHStr+"PoolInfo_Bit.ini");

	//确定上次交易价格
	char strLastTrade[20]={0};
	GetPrivateProfileString(currency,"LastTrade","0.0",strLastTrade,20,g_WSXHStr+"POOLInfo_Bit.ini");
	double fLastTrade = atof(strLastTrade);
	BOOL bNeedSetLast = FALSE;
	if(fLastTrade<COMPPREC)
	{
		fLastTrade = GetLastTradePrice(currency);
		if(fLastTrade<COMPPREC)
			bNeedSetLast = TRUE;
	}

	char currencyPair[20]={0};
	sprintf(currencyPair, "%sUSDT", currency);
	//peek 5 mins price to see if cross
	char url[MAX_PATH]={0};
	int nDataNeeded = 5;
	sprintf(url, "https://www.binance.com/api/v3/klines?symbol=%s&interval=%s&limit=%d",currencyPair,"1m",nDataNeeded);
	char strPeekFile[100]={0};
	sprintf(strPeekFile, "%s_Binance_Peek.txt", currencyPair);
	WebContentDownload(url, strPeekFile);
	vector<AnalyData> DataPeek;
	double dMultyply = 0.0;
	time_t ttStart = 0;
	GetHistoryData_binance(DataPeek, dMultyply, ttStart, strPeekFile);
	//!!
	if(DataPeek.empty()) 
	{
		::SetTimer(m_hOwnerWnd, DATA_HOSTA, g_nInterReq, NULL);
		return;
	}

	double fHhl=DataPeek[0].Open, fLlv=DataPeek[0].Open, fLast=DataPeek[0].Open;
	for(int i=0;i<DataPeek.size();i++)
	{
		if(DataPeek[i].High>fHhl)
			fHhl = DataPeek[i].High;
		if(DataPeek[i].Low<fLlv)
			fLlv = DataPeek[i].Low;
		fLast = DataPeek[i].Close;
	}
	if(bNeedSetLast)
	{
		char strLastTrade[20]={0};
		sprintf(strLastTrade, "%9.9f", fLast);
		WritePrivateProfileString(currency, "LastTrade", strLastTrade, g_WSXHStr+"POOLInfo_Bit.ini");
		::SetTimer(m_hOwnerWnd, DATA_HOSTA, g_nInterReq, NULL);
		return;
	}
	
	char strTradeGap[20]={0};
	GetPrivateProfileString("POOL","TradeGap","0.0",strTradeGap,20,g_WSXHStr+"POOLInfo_Bit.ini");
	double fTradeGap = atof(strTradeGap);
	if(fTradeGap<COMPPREC)
		fTradeGap = TRADE_GAP;

	BOOL bRealTrade = FALSE;
	float fSellTarget = fLastTrade*(1.0+1.0618*fTradeGap);
	float fBuyTarget = fLastTrade*(1.0-1.0618*fTradeGap);
	
	if(fLast<fBuyTarget)			//buy
	{
		if(fHhl>fBuyTarget&&fLlv<fBuyTarget)		//real buy
			bRealTrade = TRUE;
		else							//set  
			bNeedSetLast = TRUE;
	}
	else if(fLast>fSellTarget)		//sell
	{
		if(fHhl>fSellTarget&&fLlv<fSellTarget)		//real sell
			bRealTrade = TRUE;
		else							//set  
			bNeedSetLast = TRUE;
	}

	if(bRealTrade)
		bNeedSetLast = Calc2Trade_Func(ZBCode, currency, fLastTrade);
	if(bNeedSetLast)
	{
		char strLastTrade[20]={0};
		sprintf(strLastTrade, "%9.9f", fLast);
		WritePrivateProfileString(currency, "LastTrade", strLastTrade, g_WSXHStr+"POOLInfo_Bit.ini");
		::SetTimer(m_hOwnerWnd, DATA_HOSTA, g_nInterReq, NULL);
		return;
	}
	::SetTimer(m_hOwnerWnd, DATA_HOSTA, g_nInterReq, NULL);
}

double	ProceedPartTrade(char *currency, vector<MatchParamInfo> BestMatchs_s, vector<MatchParamInfo> BestMatchs_d, double dwPostSwing, AnalyData iLastData, long nDataNum, char *LogDict)
{	
	//static
	double dwPartSum_s = 0.0, dwNewInRatio_Sum_s = 0.0;
	for(int i=0;i<BestMatchs_s.size();i++)
	{
		MatchParamInfo mpi = BestMatchs_s[i];
		double dwPart = 1.0;
		
		dwNewInRatio_Sum_s += dwPart*GetStaticRatio(mpi, dwPostSwing);
		dwPartSum_s += dwPart;
	}
	double dwNewInRatio_s = 0.0;
	if(dwPartSum_s>COMPPREC)
		dwNewInRatio_s = dwNewInRatio_Sum_s/dwPartSum_s;
	//dyna
	double dwPartSum_d = 0.0, dwNewInRatio_Sum_d = 0.0;
	for(i=0;i<BestMatchs_d.size();i++)
	{
		MatchParamInfo mpi = BestMatchs_d[i];
		double dwPart = 1.0;
		
		dwNewInRatio_Sum_d += dwPart*GetStaticRatio(mpi, dwPostSwing);
		dwPartSum_d += dwPart;
	}
	double dwNewInRatio_d = 0.0;
	if(dwPartSum_d>COMPPREC)
		dwNewInRatio_d = dwNewInRatio_Sum_d/dwPartSum_d;
	
	double dwNewInRatio = dwPostSwing;
	if(!BestMatchs_s.empty()&&!BestMatchs_d.empty())
		dwNewInRatio = min(dwNewInRatio_s, dwNewInRatio_d);
	else if(BestMatchs_s.empty()&&BestMatchs_d.empty())
		dwNewInRatio = dwPostSwing;
	else 
		dwNewInRatio = max(dwNewInRatio_s,dwNewInRatio_d);
	
	//log
	char FileName[MAX_PATH]={0};
	sprintf(FileName, "%s\\bestmatch_log-(%04d).txt", LogDict, iLastData.Time.Daye.Day);
	FILE *BestParamsFile = _fsopen(FileName,"a+",SH_DENYNO);
	if(BestParamsFile)
	{
		fprintf(BestParamsFile, "-------------------------------------------------\r\n");
		fprintf(BestParamsFile, "%d-%04d-%02d:%02d(%d):\t%.3f\r\n", 
			iLastData.Time.Daye.Year+2004, iLastData.Time.Daye.Day, iLastData.Time.Daye.Minute/60, iLastData.Time.Daye.Minute%60, nDataNum, 
			iLastData.Close);
		fprintf(BestParamsFile, "---------static---------\r\n");
		for(int k=0;k<BestMatchs_s.size();k++)
		{
			MatchParamInfo mpi = BestMatchs_s[k];
			if(mpi.nCount<1) continue;
			float fProfit = mpi.fProfitSum/(float)mpi.nCount;
			if(mpi.cp.nBSFlag==-1)
				fProfit = 0.0f-fProfit;
			fprintf(BestParamsFile, "%d\t%d|%d\t%.2f\t%d(%d)\t%.3f(%.3f. %.3f)\r\n", 
				mpi.cp.nUnit, mpi.cp.ZBParam[0], mpi.cp.ZBParam[1], fProfit, mpi.nRangeSum/mpi.nCount, nDataNum-mpi.cp.lHLRawPos+1, mpi.cp.fActPrice, mpi.cp.fHLValue, mpi.cp.fAverPrice);
		}
		fprintf(BestParamsFile, "---------dynamic---------\r\n");
		for(k=0;k<BestMatchs_d.size();k++)
		{
			MatchParamInfo mpi = BestMatchs_d[k];
			if(mpi.nCount<1) continue;
			float fProfit = mpi.fProfitSum/(float)mpi.nCount;
			if(mpi.cp.nBSFlag==-1)
				fProfit = 0.0f-fProfit;
			fprintf(BestParamsFile, "%d\t%d|%d\t%.2f\t%d(%d)\t%.3f(%.3f. %.3f)\r\n", 
				mpi.cp.nUnit, mpi.cp.ZBParam[0], mpi.cp.ZBParam[1], fProfit, mpi.nRangeSum/mpi.nCount, nDataNum-mpi.cp.lHLRawPos+1, mpi.cp.fActPrice, mpi.cp.fHLValue, mpi.cp.fAverPrice);
		}
		fclose(BestParamsFile);
	}
	
	return dwNewInRatio;
}

void	CHostAHq::InitData(char *ZBCode, char *currency)
{
	SetTestParamSet(ZBCode, currency, TEST_GROUP);
	
	TradeTestInfo *pCurTti = NULL;
	for(int s=TEST_END;s>=TEST_START;s--)
	{
		pCurTti = &m_TradeTestInfo[s];
		TestParam tp = pCurTti->m_TestParam;
		int nPerSeed = tp.PerSeed;
		
		//get data
		vector<AnalyData> CurHisData;
		GetKLineFromBinance(pCurTti->Currency, tp, CurHisData);
	}
}

BOOL	CHostAHq::Calc2Trade_Func(char *ZBCode, char *currency, double fLastTrade)
{
	BOOL bExtTrade = FALSE;
	char currencyPair[20]={0};
	sprintf(currencyPair, "USDT_%s", currency);

	if(m_TradeTestInfo.empty())
		SetTestParamSet(ZBCode, currency, TEST_GROUP);

	int nTestGroup = m_TradeTestInfo.size();
	HANDLE *hCalcThreads = new HANDLE[nTestGroup];
	TradeTestInfo *pCurTti = NULL;
	int nThreadCount = 0;
	for(int s=TEST_END;s>=TEST_START;s--)
	{
		pCurTti = &m_TradeTestInfo[s];
		TestParam tp = pCurTti->m_TestParam;
		int nPerSeed = tp.PerSeed;
		
		//get data
		vector<AnalyData> CurHisData;
		GetKLineFromBinance(pCurTti->Currency, tp, CurHisData);
		map<int, PreCachedHisData>::iterator iter = m_CachedHisData.find(nPerSeed);
		if(iter==m_CachedHisData.end())
			AfxMessageBox("Can't Find PreData!");
		PreCachedHisData pcd = (*iter).second;
		vector<AnalyData> AllHisData(pcd.m_PreHisData);
		long nAppendNum = CurHisData.size();
		for(int i=0;i<nAppendNum;i++)
			AllHisData.push_back(CurHisData[i]);

		long nDataNum = AllHisData.size();
		if(nDataNum>0)
		{
			if(!pCurTti->m_pCalcInst)
				pCurTti->m_pCalcInst = new COneCalc;
			COneCalc *pOcr = pCurTti->m_pCalcInst;
			pOcr->InputBaseData(currencyPair, tp, &AllHisData[0], AllHisData.size());

			DWORD	TmpThreadID;
			hCalcThreads[s] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)AfxBestMatchesCalc,(LPVOID)pCurTti,0,&TmpThreadID);
			nThreadCount++;
		}
	}
	DWORD dw = WaitForMultipleObjects(nThreadCount, &hCalcThreads[TEST_START], TRUE, INFINITE);

	AnalyData iLastData = {0};
	long nDataNum = 0;

	double dwPostSwing=1.0;
	GetSwingPos(dwPostSwing);
	dwPostSwing = pow(dwPostSwing, 0.81);
	double fSumSqrtRatio = 0.0;
	int nCount = 0;
	for(s=TEST_START;s<=TEST_END;s++)
	{
		pCurTti = &m_TradeTestInfo[s];
		iLastData = pCurTti->m_pCalcInst->GetLastData();
		nDataNum = pCurTti->m_pCalcInst->GetDataNum();
		TestParam tp = pCurTti->m_TestParam;
		
		char LogDict[MAX_PATH]={0};
		sprintf(LogDict, "%s\\%d_%.2f", Share_Folder, tp.PerSeed, 100.0*tp.TradeFee);
		CreateDirectory(LogDict, NULL);
		double fPartRatio = ProceedPartTrade(pCurTti->Currency, pCurTti->m_BestMatchs_s, pCurTti->m_BestMatchs_d, dwPostSwing, iLastData, nDataNum, LogDict);
		fSumSqrtRatio += pow(fPartRatio, 2.0);
		nCount++;
	}
	double dwNewInRatio = pow(fSumSqrtRatio/(double)nCount, 0.5);

	//get balance
	double dwAlwRemain = 59.0;	//for trx
	//资金和持仓信息，不清除挂单
	double fCurRatio=0.0;
	double dwCurInVol=0.0;
	vector<BalanceUnit> MyBalances;
	double dwAllValue=0.0, dwAvailMoney=0.0;
	BOOL bHasAllOrders = TRUE;
	GetAllMarketValue(MyBalances, dwAllValue, dwAvailMoney, bHasAllOrders);
	char AllValue[10]={0};
	sprintf(AllValue, "%.2f", dwAllValue);
	WritePrivateProfileString("POOL","AllValue",AllValue,g_WSXHStr+"PoolInfo_Bit.ini");
	double dwCanUseValue = dwAllValue-dwAlwRemain;
	double dwCanSellVol = dwCurInVol;

	int nBalanceNo=-1, nTRXNo=-1;
	char *strTRX_GoodFee = "TRX";
	for(int i=0;i<MyBalances.size();i++)
	{
		if(strcmp(currency, MyBalances[i].currency)==0)
			nBalanceNo = i;
		if(strcmp(strTRX_GoodFee, MyBalances[i].currency)==0)
			nTRXNo = i;
	}
	if(nBalanceNo>=0)
	{
		dwCurInVol = MyBalances[nBalanceNo].dwInVol;
		fCurRatio = MyBalances[nBalanceNo].dwValue/dwCanUseValue;
	}
	//
	if(dwCurInVol<COMPPREC&&dwAvailMoney<COMPPREC)
		return FALSE;

	//pploniex TRX > 49(54,59)
	double dwTRXBalance = 0.0;
	if(nTRXNo>=0)
		dwTRXBalance = MyBalances[nTRXNo].dwValue;
	if(dwTRXBalance<54.0||dwTRXBalance>64.0)
	{
		char Price[20]={0};
		GetNowPrice(strTRX_GoodFee, FALSE, Price);
		if(strlen(Price)>0)
		{
			if(dwTRXBalance<54.0)
			{
				double dwBuyVol = (59.0-dwTRXBalance)/atof(Price);
				TradeAction(strTRX_GoodFee, dwBuyVol, TRUE);
			}
			if(dwTRXBalance>64.0)
			{
				double dwSellVol = (dwTRXBalance-59.0)/atof(Price);
				TradeAction(strTRX_GoodFee, dwSellVol, FALSE);
			}
		}
	}

	//trade
	if(fabs(dwNewInRatio-fCurRatio)>COMPPREC)		//need to trade
	{
		if(dwNewInRatio-fCurRatio>COMPPREC||fabs(dwNewInRatio-1.0)<COMPPREC)		//buy
		{
			double dwBuyPrice = iLastData.Close;
			double dwUsedFund = min(dwAvailMoney, dwAllValue*(dwNewInRatio-fCurRatio));
			double dwBuyVol = dwUsedFund/(dwBuyPrice*(1.0+REAL_TRADE_FEE));
			//BUY!
			TradeAction(currency, dwBuyVol, TRUE);
			bExtTrade = TRUE;
		}
		else if(fCurRatio-dwNewInRatio>COMPPREC||dwNewInRatio<COMPPREC)		//sell
		{
			double dwSellVol = min(dwCurInVol, dwCurInVol-dwCurInVol*dwNewInRatio/fCurRatio);
			//SELL!
			TradeAction(currency, dwSellVol, FALSE);
			bExtTrade = TRUE;
		}
	}
	if(bExtTrade)
	{
		WritePrivateProfileString(currency,"LastTrade","0.0",g_WSXHStr+"POOLInfo_Bit.ini");

		char FileName[MAX_PATH]={0};
		sprintf(FileName, "%s\\trade_log-(%04d).txt", Share_Folder, iLastData.Time.Daye.Day);
		FILE *BestParamsFile = _fsopen(FileName,"a+",SH_DENYNO);
		if(BestParamsFile)
		{
			CTime ct = CTime::GetCurrentTime();
			fprintf(BestParamsFile, "-------------------------------------------------\r\n");
			fprintf(BestParamsFile, "%d-%04d-%02d:%02d:%02d(%d):\t%.3f->%.3f\t%.3f->%.3f\t(%.3f, %.3f)\r\n", 
				ct.GetYear(), 100*ct.GetMonth()+ct.GetDay(), ct.GetHour(), ct.GetMinute(), ct.GetSecond(), nDataNum, 
				fLastTrade, iLastData.Close, fCurRatio, dwNewInRatio, dwAllValue, dwAllValue/iLastData.Close);
			fclose(BestParamsFile);
		}
	}

	return !bExtTrade;
}

void	CHostAHq::CalcFunc_au(char *ZBCode, char *GPCode, char *DataPeriod, char *DataInterval, char *DataNum, int nPerItem)
{
	vector<AnalyData> AllHisData;
	GetDataItems_Au(GPCode,DataPeriod,DataInterval,DataNum,nPerItem,AllHisData);
	
	int nPerSeed = 0;
	if(strcmp(DataPeriod,"Minute")==0&&strcmp(DataInterval,"60")==0)
		nPerSeed = 3600;
	else if(strcmp(DataPeriod,"Minute")==0&&strcmp(DataInterval,"30")==0)
		nPerSeed = 1800;
	else if(strcmp(DataPeriod,"Minute")==0&&strcmp(DataInterval,"15")==0)
		nPerSeed = 900;
	else if(strcmp(DataPeriod,"Minute")==0&&strcmp(DataInterval,"10")==0)
		nPerSeed = 600;
	else if(strcmp(DataPeriod,"Minute")==0&&strcmp(DataInterval,"5")==0)
		nPerSeed = 300;
	else if(strcmp(DataPeriod,"Minute")==0&&strcmp(DataInterval,"1")==0)
		nPerSeed = 60;
	
	if(AllHisData.size()>10)
	{
		COneCalc *m_pCalcInst = new COneCalc;
		COneCalc *pOcr = m_pCalcInst;
	//	pOcr->InputBaseData(GPCode, nPerSeed, &AllHisData[0], AllHisData.size());
		//////////////////////////////////////////////////////////////////////////
		//opt	
		
		delete m_pCalcInst;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL	CHostAHq::InitHq(LPSTR strError)
{
	UnInitHq();
		
	//MSB:50943752
	//WBC:283907
	//BPT:65215
	//WTC:339231743
	//PBH:538365188
	//MVP:207679
	//PNI:7650935
	//CUV:27486573
	//CTD:27528971
	//LOV:82257125
	//APE:46648
	/*
	CalcFunc_au("MYEM","50943752","Minute","5","40",1);
	CalcFunc_au("MYEM","283907","Minute","5","40",1);
	CalcFunc_au("MYEM","65215","Minute","5","40",1);
	CalcFunc_au("MYEM","339231743","Minute","5","40",1);
	CalcFunc_au("MYEM","538365188","Minute","5","40",1);
	CalcFunc_au("MYEM","207679","Minute","5","40",1);
	CalcFunc_au("MYEM","7650935","Minute","5","40",1);
	CalcFunc_au("MYEM","27486573","Minute","5","40",1);
	CalcFunc_au("MYEM","27528971","Minute","5","40",1);
	CalcFunc_au("MYEM","82257125","Minute","5","40",1);
	CalcFunc_au("MYEM","46648","Minute","5","40",1);
	*/
//	CalcFunc_au("FAOOVOL_NOPOS","46648","Minute","5","40",1);
//	CalcFunc_au("MYEM","46648","Minute","1","4",1);

//	GetASXListResults();
//	exit(-1);
	
	//test
//	TradeAction("ETH", 6.6589745, FALSE);
//	vector<BalanceUnit> MyBalances;
//	double fAllValue=0.0, dwAvailMoney=0.0;
//	GetAllMarketValue(MyBalances, fAllValue,dwAvailMoney);
//	char ActionPrice[20]={0};
//	GetMatchPrice("ZEC", TRUE, 50.0, ActionPrice);
//	AfxMessageBox(ActionPrice);
//	double ddd = GetPowFlag(4.0,100,16.0,47);
//	exit(-1);
	
	::SetTimer(m_hOwnerWnd, DATA_HOSTA, g_nInterReq, NULL);
	//记录时间
	time_t tt = time(NULL);
	char strRefreshTimet[20]={0};
	sprintf(strRefreshTimet, "%d", tt);
	WritePrivateProfileString("POOL","REFRESH",strRefreshTimet,g_WSXHStr+"PoolInfo_Bit.ini");

	return TRUE;
}

void	CHostAHq::UnInitHq()
{

}

void	CHostAHq::TradeCalc(char *Currency)
{

}
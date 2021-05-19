// OneCalc.h: interface for the COneCalc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ONECALC_H__2FAD993C_D138_461D_ABCC_8362134BDD79__INCLUDED_)
#define AFX_ONECALC_H__2FAD993C_D138_461D_ABCC_8362134BDD79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define		REAL_TRADE_FEE	0.001
#define		TRADE_GAP		0.00618
#define		TEST_GROUP		13
#define		TEST_START		8
#define		TEST_END		12
#define		MAX_UNIT		60

#define		EGG_COUNT		3


struct  KeyCode 
{
	int nUnit;
	int nOffset;
};

struct cmpcalcsort
{
	bool operator()(const KeyCode &k1, const KeyCode &k2) const
	{
		if(k1.nUnit!=k2.nUnit)
			return k1.nUnit<k2.nUnit;
		return k1.nOffset<k2.nOffset;
	}
};

struct	TestParam
{
	int		PerSeed;
	int		nUnitParam_Start;
	int		nUnitParam_End;
	double	fStepFlag;
	int		nCalcDays;
	double	TradeFee;
};

struct	CalcParam
{
	char ZBCode[21];
	int	 ZBParam[2];
	int  nUnit;

	int nBSFlag;
	long nRawPos_Start;
	long nRawPos_LastAct;
	float fHLValue;
	long lHLRawPos;
	float fActPrice;
	float fLastActPrice;
	float fNow;
	float fAverPrice;

	float fNewTrend;
	float fNewSwing;
};

struct HisProfitLineItem
{
	long  TimeKey;
	vector<int> ActionIO;		//start:+ end:-
};

struct TradeProfitInfo
{
	long nRawPos_Start;
	long nRawPos_End;
	long nRawPos_LastAct;
	int	 nBSFlag;
	double fProfit;
	double fGap;
	double fProfitPerItem[6];
};

struct	TradeCalcInfo
{
	CalcParam	cp;
	TradeProfitInfo tpi;
};

struct	MatchParamInfo
{
	CalcParam	cp;
	double		fScore1;
	double		fScore2;
	double		fProfitSum;
	double		fGapSum;
	long		nRangeSum;
	int			nCount;
};

class COneCalc  
{
public:
	COneCalc();
	virtual ~COneCalc();

private:
	short	m_nPeriod;
	long	m_nGSIndex;
	char	m_Code[20];
	
	DTopViewCalc *m_pCalc;
	AnalyData*	m_pAnalyData;
	long	m_nDataNum, m_nMaxDataNum;

	long	m_nStartTimeKey;
	vector<int>		m_NeedCalcFlag;
	vector<TradeCalcInfo> m_AllTradeInfo;
	map<long, int, greater<long> > m_TimeDict;
	vector<HisProfitLineItem> m_AllProfitLine;
	vector<HisProfitLineItem> m_BestProfitLine;

	double	m_dNowAverPrice;

public:
	HANDLE	m_StartEvent;
	BOOL	m_bStoppedByMe;
	BOOL	m_bTest;

private:
	void	CreateNewAnalyData(vector<AnalyData> &vNewData, int nUnit, int nOffset);
	inline	int		GetRawPosFromTimeKey(long TimeKey)
	{
		map<long, int, greater<long> >::iterator iter = m_TimeDict.find(TimeKey);
		if(iter==m_TimeDict.end())
			return -1;
		return (*iter).second;
	}

	void	GetPreviousMatch(TradeCalcInfo TargetItem, vector<CalcParam> &vPreviousMatch);
	void	GetNextMatch(TradeCalcInfo TargetItem, vector<TradeCalcInfo> &vNextMatch);

public:
	inline	long GetPeriod(){ return m_nPeriod; }
	inline  long GetDataNum(){ return m_nDataNum; }

	inline	AnalyData	GetLastData()
	{
		return m_pAnalyData[m_nDataNum-1];
	}

	void	InputBaseData(char *Code, TestParam tp, AnalyData* pAnalyDatap, long nDataNum);
	void	CalcProfit(HISDAT *pAnaData, int nStartPos, int nEndPos, float *fTriger, CalcParam TradeParams, BOOL bLog=FALSE);

	void	CalcRes_Show_Result(char *ZBCode, int nUnit, CalcParam CPParam, TestParam tp);
	void	FiltBestProfLine_Dyna(vector<MatchParamInfo> &BestMatchs);
	void	FiltBestProfLine_Static(vector<MatchParamInfo> &BestMatchs);
	void	FiltBestProfLine(vector<MatchParamInfo> &BestMatchs_Static, vector<MatchParamInfo> &BestMatchs_Dyna, TestParam tp);

};

#endif // !defined(AFX_ONECALC_H__2FAD993C_D138_461D_ABCC_8362134BDD79__INCLUDED_)



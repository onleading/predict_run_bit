// HostAHq.h: interface for the CHostAHq class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOSTAHQ_H__AA715074_C77E_4BEF_9E74_90DADB501221__INCLUDED_)
#define AFX_HOSTAHQ_H__AA715074_C77E_4BEF_9E74_90DADB501221__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HqSource.h"
#include "OneCalc.h"
#include "CoopCache.h"

enum	TRADE_TYPE
{
	CALC_2_ON_4 = 1,		//CALC_60_ON_300
	CALC_2_ON_8,			//CALC_60_ON_900
	CALC_2_ON_16,			//CALC_60_ON_1800
	CALC_4_ON_8,			//CALC_300_ON_900
	CALC_4_ON_16,			//CALC_300_ON_1800
	CALC_8_ON_16,			//CALC_900_ON_1800
};

struct	TradeInfo
{
	BOOL	m_bStart2Trade;
	double	m_dwAvalFund;
	double	m_dwInVol;
	double	m_dwCurInRatio;
	double	m_dwLastInRatio;
	float	m_fLastTradePrice;

	double	m_dwBDFlag;
};

struct TradeTestInfo
{
	char		Currency[21];
	char		ZBCode[21];
	COneCalc	*m_pCalcInst;
	vector<MatchParamInfo> m_BestMatchs_d;
	vector<MatchParamInfo> m_BestMatchs_s;
	TestParam	m_TestParam;
	TradeInfo	m_TradeInfo;
	double		m_dLastSwingPos;
	BOOL		m_bStartFlag;

	TradeTestInfo()
	{
		memset(Currency, 0, sizeof(Currency));
		memset(ZBCode, 0, sizeof(ZBCode));
		m_pCalcInst = NULL;
		m_BestMatchs_s.clear();
		m_BestMatchs_d.clear();
		memset(&m_TestParam, 0, sizeof(m_TestParam));
		memset(&m_TradeInfo, 0, sizeof(m_TradeInfo));
		m_bStartFlag = FALSE;
	}
};

struct	SimuTradeUnit
{
	int		m_nShortXh;
	int		m_nLongXh;
	double	m_dwMaxFund;

	int		m_nBSFlag_short;
	int		m_nBSFlag_long;

	double	m_dwRatioTrace;
};

struct	TradeSingnal
{
	double	dwBDSingnal;
	double	dwSwingSingnal;
};

struct	PreCachedHisData
{
	vector<AnalyData>	m_PreHisData;
	double	m_dMultyply;
	long	m_nPreEndTT;
	vector<AnalyData>	m_AppendHisData;
};

class CHostAHq : public CHqSource  
{
public:
	CHostAHq();
	virtual ~CHostAHq();

	BOOL	InitHq(LPSTR strError);								//此时为同步
	void	UnInitHq();
	void	NetRegister(HWND hMsgWnd, UINT MsgID, long lEvent, int iNo=0);

private:
	HWND	m_hOwnerWnd;
	char	m_CookieData[4*1024];
	long_short	m_LastLogTime;

	COneCalc *m_pCalcInst;
	AnalyData *m_pAnaData;

	map<int, PreCachedHisData> m_CachedHisData;
	vector<TradeTestInfo>		m_TradeTestInfo;
	vector<SimuTradeUnit>		m_SimuTradeInfo;
	vector<TradeSingnal>		m_LastSingnal;

//	TradeInfo m_TTinfo;

private:
	void	InitData(char *ZBCode, char *currency);
	void	GetKLineFromBinance(char *currency, TestParam tp, vector<AnalyData> &CurHisData);
//	void	GetKLineFromPoloniex(char *currency, int nPerSeed, vector<AnalyData> &CurHisData);

	void	SetTestParamSet(char *ZBCode, char *currency, int nGroupNum);
	void	ProceedSimTrade(SimuTradeUnit *pSimTrade, long_short NowTime, double fNowPrice, char *LogDict);
	double  CalcTradeSingnal(double fNowPrice, double dwLastInVolRatio, vector<TradeSingnal> &ready2change);

	BOOL	GetSwingPos(int nTestPart, double &dwPostSwing);
	void	GetSwingPos(double &dwPostSwing);

public:
	void	CalcFunc_au(char *ZBCode, char *GPCode, char *DataPeriod, char *DataInterval, char *DataNum, int nPerItem);
	void	TradeFunc_bitc(char *ZBCode, char *currency);
	BOOL	Calc2Trade_Func(char *ZBCode, char *currency, double fLastTrade);

	void	ScanActivePred_Asx();

	void	TradeCalc(char *Currency);
};

#endif

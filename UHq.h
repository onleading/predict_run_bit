#ifndef UHQ_H_
#define UHQ_H_

//5档买卖盘
#include "Ubase.h"
#include "HToolTipEx.h"

struct UHqContent
{
	float	Close;
	float	Wtb;
	long	buywtl;
	long	sellwtl;
	long	Wlc;
	float	Buyprice[5];
	DWORD	Buyvol[5];
	float	Sellprice[5];
	DWORD	Sellvol[5];
	float	Now;
	float	Average;
	float	Zd;
	float	Open;
	float	Zaf;
	float	Zef;
	float	Max;
	DWORD	Volume;
	float	Amount;
	float	Min;
	DWORD	NowVol;
	float	fLB;
	DWORD	Outside;
	DWORD	Inside;

	DWORD	CJBS;
	float	Syl;
	float	Hsl;
	float	Zgb;
	float	ActiveCapital;

	float	Yjlx;
	float	Dqsy;
	float	Jjjz;
	float	LastPV;

	float	Mg_Jz;
	float	Mg_Sy;
	float	Mg_Gj;
	float	Mg_Jl;
	
	float	TPPrice1;
	float	TPPrice2;

	float	fGgbl;
	float	fInnerVal;
	float	fYjl;
};

struct UZsHqContent
{
	float	Close;
	float	Cj[7];
	float	Now;
	float	Open;
	float	Zd;
	float	Zaf;
	float	Zef;
	float	Max;
	float	Min;
	DWORD	Volume;
	float	Amount;
	DWORD	UpNum;
	DWORD	DownNum;
	float	Zs_hsl;
	float	TickDiff;

	float	Buy1Cj;
	float	Sell1Cj;
	float	Buy12345Cj;
	float	Sell12345Cj;
};

class UHq : public UBase
{
private:
	BOOL	m_bSyncStatus;					//是不是在同步回忆状态
	short	m_nSyncMin;						//同步时的绝对分钟点

	BOOL	m_bHasLink;
	BOOL	m_bEmpty;
	CurrStockData		m_Hq; 
	CurrStockData_L2	m_Hq_L2; 
	EXCURRDATA	m_HqEx;
	CurrStockData		m_ZsHq_SZ[3],m_ZsHq_SH[3];	//此股票对应的股票行情
	struct UHqContent	m_HqContent;
	struct UZsHqContent	m_ZsHqContent;
	BOOL				m_bChangeStk;				//切换股票
	MemStkInfo			*m_pStkInfoBak;
	struct UHqContent	m_HqContentBak;
	struct UHqContent	m_HqContentShow;

	float	m_zsTotalGb;
	float	m_zsTotalLtgb;
	float	m_zsTotalSz;
	float	m_zsTotalLtSz;
	float	m_zsSyl;
	float	m_zsSjl;
	float	m_zsYieldRate;
	float	m_zsSZBuyCjPre, m_zsSHBuyCjPre, m_zsSZSellCjPre, m_zsSHSellCjPre, m_zsBuyCjPre, m_zsSellCjPre;
	short	m_zsSHBuyFlag, m_zsSZBuyFlag, m_zsSHSellFlag, m_zsSZSellFlag, m_zsBuyFlag, m_zsSellFlag;	
	CRect	m_DKRect;
	
	BOOL	m_bDKLine;
	BOOL	m_bHasJGData;
	STKRELATION m_LinkData[100];
	int			m_nLinkGPNum;
	//远程部分用
	BOOL	m_bDataReady;
	BOOL	m_bHasLinkInfo;
	BOOL	m_bLowSize;

	CRect	m_BuyRect[5],m_SellRect[5],m_NowRect,m_LinkIconRect,m_JGDataIconRect,m_GxdIconRect,m_TVIconRect;
public:
	UHq(LPCOMINFO pComInfo,BOOL bLowSize=FALSE);
	~UHq();

	HToolTipEx	m_ToolTip;

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void CalcIt();
	void DrawZsHq(CDC *pDC);
	void DrawIt(CDC *pDC);
	void DrawDSView(CDC*pDC);
	void DrawBigHqHead(CDC *pDC);
	void DrawBigHq(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);	
};


#endif

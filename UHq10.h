#ifndef UHQ10_H_
#define UHQ10_H_

//10档买卖盘

#include "Ubase.h"

struct UHq10Content
{
	float	Close;
	float	Now;
	float	Max;
	float	Min;
	float	Buyprice[10];
	DWORD	Buyvol[10];
	float	Sellprice[10];
	DWORD	Sellvol[10];
	float	BuyAvp;
	float	SellAvp;
	DWORD	TotalBuyv;
	DWORD	TotalSellv;
};

class UHq10 : public UBase
{
	BOOL	m_bEmpty;
	CurrStockData		m_Hq;
	CurrStockData_L2	m_Hq_L2;
	struct CurrStockData_Code m_CurrData;
	//远程部分用
	BOOL				m_bDataReady;
	struct UHq10Content	m_Hq10Content;
	CRect	m_BuyRect[10],m_SellRect[10];

	BOOL				m_bChangeStk;				//切换股票
	MemStkInfo			*m_pStkInfoBak;
	struct UHq10Content	m_Hq10ContentBak;
	struct UHq10Content	m_Hq10ContentShow;
	struct UHq10Content	m_Hq10AveBak;

	int					m_nBuyAvpFlag,m_nSellAvpFlag;
	int					m_nTotalBuyvFlag,m_nTotalSellvFlag;
private:
	void DrawPriceToken(CDC *pDC,int XFrom,int YFrom,BOOL bBuy,BOOL bPrice10,float fPrice);
public:
	UHq10(LPCOMINFO pComInfo);
	~UHq10();

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void CalcIt();
	void DrawIt(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif

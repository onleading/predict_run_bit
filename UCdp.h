#ifndef UCDP_H_
#define UCDP_H_

//值,CDP,TBP
#include "Ubase.h"

class UCdp : public UBase
{
	BOOL	m_bEmpty;
	CurrStockData	 m_Hq;
	EXCURRDATA		 m_HqEx;

	float	m_zsTotalGb;
	float	m_zsTotalLtgb;
	float	m_zsTotalSz;
	float	m_zsTotalLtSz;
	float	m_zsSyl;
	float	m_zsSjl;
	float	m_zsYieldRate;
	
	float	m_TPPrice1;
	float	m_TPPrice2;
	//远程部分用
	BOOL	m_bDataReady;
public:
	UCdp(LPCOMINFO pComInfo);
	~UCdp();

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void DrawIt(CDC *pDC);
	void DrawHKGP(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif

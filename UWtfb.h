#ifndef UWTFB_H_
#define UWTFB_H_

#include "Ubase.h"

class UWtfb : public UBase
{
	BOOL	m_bEmpty;
	CurrStockData	 m_Hq;
	CurrStockData_L2 m_Hq_L2;
	
	float	m_TPPrice1;
	float	m_TPPrice2;
	//远程部分用
	BOOL	m_bDataReady;
public:
	UWtfb(LPCOMINFO pComInfo);
	~UWtfb();

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void DrawIt(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif

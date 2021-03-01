#ifndef USTurn_H_
#define USTurn_H_

#include "Ubase.h"

class USTurn : public UBase
{
	CURRDATA	m_Hq;
	EXCURRDATA	m_HqEx;

	struct Seat_Content	m_Seat[MAX_SEATNUM];

	int		m_nSeatNum;

	int		m_nSel;
	//远程部分用
	BOOL	m_bDataReady;
public:
	USTurn(LPCOMINFO pComInfo);
	~USTurn();

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void DrawIt(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif
#ifndef UBTurn_H_
#define UBTurn_H_

#include "Ubase.h"

class UBTurn : public UBase
{
	CURRDATA	m_Hq;
	EXCURRDATA	m_HqEx;

	struct Seat_Content	m_Seat[2][MAX_SEATNUM];

	int		m_nSel, m_nSelDomain;
	CRect	m_rcCon;
	BOOL	m_bDrawName;
	//远程部分用
	BOOL	m_bDataReady;
public:
	UBTurn(LPCOMINFO pComInfo);
	~UBTurn();

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void DrawIt(CDC *pDC, BOOL bDrawName = TRUE);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif

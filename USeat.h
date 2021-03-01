#ifndef USeat_H_
#define USeat_H_

//
#include "Ubase.h"
#include "HToolTipEx.h"

class USeat : public UBase
{
	CURRDATA	m_Hq;
	EXCURRDATA	m_HqEx;

	struct Seat_Content	m_Seat[2][MAX_SEATNUM];

	RECT		m_SeatRect[2][MAX_SEATNUM];

	int		m_nSel, m_nSelDomain;
	CRect	m_rcCon;
	//远程部分用
	BOOL	m_bDataReady;
public:
	HToolTipEx	m_ToolTip;
public:
	USeat(LPCOMINFO pComInfo);
	~USeat();

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void DrawIt(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif

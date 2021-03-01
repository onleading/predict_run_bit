#ifndef UFJB_H_
#define UFJB_H_

//分价表

#include "Ubase.h"

class UFjb : public UBase
{
	CurrStockData		m_Hq;
	struct UFjContent	m_FjContent[MAX_FJNUM];

	int		m_nFjNum,m_nStart,m_nPos,m_nMaxItem;

	long	m_lLastVolume;
	float	m_fLastZs;

	long	m_lMaxVolume;

	//远程部分用
	BOOL	m_bDataReady;
public:
	UFjb(LPCOMINFO pComInfo);
	~UFjb();

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void DrawIt(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif

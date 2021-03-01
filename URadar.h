#ifndef URADAR_H_
#define URADAR_H_

//ÊÐ³¡À×´ï
#include "Ubase.h"

class URadar : public UBase
{
	long	m_nLastPos;
	int		m_nDrawTick;

	WARN_STRU *m_pTick;

	int		m_nTickNum,m_nStart,m_nPos,m_nMaxItem;

	int		m_nCurSel;
	int		m_nLaceLen;
	int		TextH;
	int		nOptBW;

	char    Radar_Code[SH_CODE_LEN+1];
	char	Radar_SetCode;
public:
	URadar(LPCOMINFO pComInfo);
	~URadar();
	int  GetStyle() { return m_nStyle; }
	short GetCurrentGPIndex();
	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);

	void DrawIt(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};

#endif
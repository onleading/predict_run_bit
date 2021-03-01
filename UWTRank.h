#ifndef UWTRANK_H_
#define UWTRANK_H_

//委托排名
#include "UBase.h"

class UWTRank : public UBase
{
	BOOL	m_bEmpty;
	int					 m_nZoomFlag;//0:无放大,1:上部分放大 2:下部分放大
	//
	short	m_fltype;
	short	m_setcode;
	//远程部分用
	BOOL	m_bDataReady;
	CRect	m_SellRect[10],m_BuyRect[10];
private:
	void DrawPart1(CDC *pDC,int TotalHeight,int TextH,int XWidth,int XFrom[],int YFrom);
	void DrawPart2(CDC *pDC,int TotalHeight,int TextH,int XWidth,int XFrom[],int YFrom);
public:
	UWTRank(LPCOMINFO pComInfo);
	~UWTRank();

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void DrawIt(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif

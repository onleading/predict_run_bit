#ifndef UShortGuy_H_
#define UShortGuy_H_

//短线精灵
#include "Ubase.h"

class UShortGuy : public UBase
{
	BOOL	m_bEmpty;
	//远程部分用
	BOOL	m_bDataReady;
public:
	UShortGuy(LPCOMINFO pComInfo);
	~UShortGuy();

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void DrawIt(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif

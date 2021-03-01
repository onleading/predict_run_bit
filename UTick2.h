#ifndef UTICK2_H_
#define UTICK2_H_

//��ʳɽ�
#include "Ubase.h"

class UTick2 : public UBase
{
	int		m_nDrawTick;
	int		m_nTickNum,m_nStart,m_nPos,m_nMaxItem;
	//Զ�̲�����
	BOOL	m_bDataReady;
	BOOL	m_bReqing;
	BOOL	m_bIsHead;

	DWORD					m_NowTicSeq,m_SrvTicSeq;
	struct CurrStockData	m_Hq;
	struct MarkInfo			m_MarkInfo;
	struct TickData_L2 *	m_pTick;

	DWORD			   *	m_pTickNoInSecond;
	char			   *	m_pTickZdFlag;
private:
	void CalcTickInfo();
public:
	UTick2(LPCOMINFO pComInfo);
	~UTick2();
	int  GetStyle() { return m_nStyle; }

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void DrawIt(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};

#endif
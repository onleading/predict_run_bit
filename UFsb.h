#ifndef UFSB_H_
#define UFSB_H_

#include "Ubase.h"

//��ʱ��

#define FSB_WIDTH		380

class UFsb : public UBase
{
	CurrStockData		m_Hq; 
	EXCURRDATA			m_HqEx;

	struct MinuteData	m_MinuteData[1500];
	int		m_nTotalMinute;
	int		m_nStart,m_nPos;
	int		m_nMaxItem;

	int		m_nDrawMinute;
	long	m_lLastVolume;

	//Զ�̲�����
	BOOL	m_bDataReady;
private:
	BOOL	m_bYestodayFalg;				//�Ƿ���������,�����赥���ж�

	void	DTWH_currMinute();
	short	GetNowTMinute() 
	{
		if(m_bDSFlag) 
		{
			m_bYestodayFalg = (m_HqEx.HqDate!=GetDSCorrespondingDate(m_pTimer));
			return GetDSMinute(m_pTimer, m_bYestodayFalg);
		}
		else 
			return GetTdxMinute(TimeSec);
	}
public:
	UFsb(LPCOMINFO pComInfo);
	~UFsb();
	int  GetStyle() { return m_nStyle; }

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void DrawIt(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif

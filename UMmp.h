#ifndef UMMP_H_
#define UMMP_H_

//��ϸ������
#include "Ubase.h"

class UMmp : public UBase
{
	int		m_nDrawMmp;

	CurrStockData	 m_Hq;
	TICK_MMP *		 m_pMmp;

	int		m_nMmpNum,m_nStart,m_nPos,m_nMaxItem,m_nAddedMmpNum;

	long	m_lLastMmpVolume;

	//Զ�̲�����
	BOOL	m_bDataReady;
	BOOL	m_bReqing;
	BOOL	m_bIsHead;
public:
	UMmp(LPCOMINFO pComInfo);
	~UMmp();
	int  GetStyle() { return m_nStyle; }

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void DrawOneCell(CDC *pDC,int YFrom,int XFrom2,int XFrom3,int i,int j);
	void DrawIt(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif
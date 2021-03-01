#ifndef UTICK_H_
#define UTICK_H_

//分笔成交
#include "Ubase.h"

struct HasDataToDo
{
	int nDoType;		//0-什么也不do 1-写文件
	char filename[MAX_PATH];
	BOOL bWriteExcel;
	int nAbsMin;
};

class UTick : public UBase
{
	BOOL	m_bSyncStatus;					//是不是在同步回忆状态
	short	m_nSyncMin;						//同步时的绝对分钟点
	int		m_nDrawTick;

	int		m_nTickNum,m_nStart,m_nPos,m_nMaxItem,m_nAddedTickNum;

	long	m_lLastVolume;
	long    m_lLastTickNum;
	float	m_fLastPrice;
	
	int		m_nZstAbsMin;

	//远程部分用
	BOOL	m_bDataReady;
	BOOL	m_bReqing;
	BOOL	m_bIsHead;

	CurrStockData	 m_Hq;
	struct TickData *m_pTick;

private:
	HasDataToDo		m_ToDo;

public:
	UTick(LPCOMINFO pComInfo);
	~UTick();
	int  GetStyle() { return m_nStyle; }

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void DrawIt(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

	void WriteToTxtFile(CString filename,BOOL bWriteExcel, int nAbsMin);

private:
	BOOL	SSMRegTickData(void);
	void	DoSomethingUseData(void);
};

#endif
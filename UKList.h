#ifndef UKList_H_
#define UKList_H_

//分析图F1报表
#include "Ubase.h"

#define KLIST_WIDTH		700

class UKList : public UBase
{
	struct AnalyData m_pAnalyData[1024];

	int		m_nDrawAnaly;
	int		m_nAnalyDataNum;
	int		m_nStart,m_nPos;
	int		m_nMaxItem;

	//远程部分用
	BOOL	m_bDataReady;
public:
	UKList(LPCOMINFO pComInfo);
	~UKList();

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void DrawIt(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};


#endif

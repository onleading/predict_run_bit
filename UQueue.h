#ifndef UQUEUE_H_
#define UQUEUE_H_

//买卖队列
#include "Ubase.h"

struct SavedQueue
{
	DWORD			RefreshNum;				//	4B		刷新数
	float			price[2];				//	8B		委价
	unsigned short	volnum[2];				//	4B		详细挂单笔数
	unsigned short	vol[2][50];				//	200B	队列
};

struct MixQueue
{
	float			price[2];				//	8B		委价
	unsigned short	volnum[2];				//	4B		详细挂单笔数
	unsigned short	vol[2][101];			//	404B	队列
	char			state[2][101];			//	404B	状态(0:普通;1:被吞;2:被啃,3:啃掉,4:被撤,5:新增)
};

class UQueue : public UBase
{
	BOOL					m_bEmpty;
	struct CurrQueue		m_Queue;
	int						m_nZoomFlag;	//0:无放大,1:上部分放大 2:下部分放大
	//动态买卖队列缓冲数据
	MemStkInfo				*m_pStkInfoBak;
	struct SavedQueue		m_QueueNew;
	struct SavedQueue		m_QueueBak;
	struct MixQueue			m_QueueShow;
	//计算匹配时用到的变量
	unsigned short			m_nQueueGap;
	short					m_pTmpMap[50];
	short					m_nTmpMap;
	unsigned short			m_nTmpClosePos;
	unsigned short			m_nTmpUnClosePos;
	unsigned short			m_nTmpFirstPos;
	//远程部分用
	BOOL					m_bDataReady;
	char					m_SingleCodeHead[100];
	struct CurrQueue_Code	m_QueueCode;
private:
	void	DrawPart(BOOL bBuy,CDC *pDC,int TotalHeight,int TextH,int XWidth,int XFrom[],int YFrom);
public:
	UQueue(LPCOMINFO pComInfo);
	~UQueue();

	void ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void DrawIt(CDC *pDC);

	int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	void Transform();								//从CurrQueue转为SavedQueue
	void CompareIt(BOOL bBuy);						//匹配
};


#endif

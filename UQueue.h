#ifndef UQUEUE_H_
#define UQUEUE_H_

//��������
#include "Ubase.h"

struct SavedQueue
{
	DWORD			RefreshNum;				//	4B		ˢ����
	float			price[2];				//	8B		ί��
	unsigned short	volnum[2];				//	4B		��ϸ�ҵ�����
	unsigned short	vol[2][50];				//	200B	����
};

struct MixQueue
{
	float			price[2];				//	8B		ί��
	unsigned short	volnum[2];				//	4B		��ϸ�ҵ�����
	unsigned short	vol[2][101];			//	404B	����
	char			state[2][101];			//	404B	״̬(0:��ͨ;1:����;2:����,3:�е�,4:����,5:����)
};

class UQueue : public UBase
{
	BOOL					m_bEmpty;
	struct CurrQueue		m_Queue;
	int						m_nZoomFlag;	//0:�޷Ŵ�,1:�ϲ��ַŴ� 2:�²��ַŴ�
	//��̬�������л�������
	MemStkInfo				*m_pStkInfoBak;
	struct SavedQueue		m_QueueNew;
	struct SavedQueue		m_QueueBak;
	struct MixQueue			m_QueueShow;
	//����ƥ��ʱ�õ��ı���
	unsigned short			m_nQueueGap;
	short					m_pTmpMap[50];
	short					m_nTmpMap;
	unsigned short			m_nTmpClosePos;
	unsigned short			m_nTmpUnClosePos;
	unsigned short			m_nTmpFirstPos;
	//Զ�̲�����
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
	void Transform();								//��CurrQueueתΪSavedQueue
	void CompareIt(BOOL bBuy);						//ƥ��
};


#endif

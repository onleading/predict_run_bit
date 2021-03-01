#ifndef UTICKZST_H_
#define UTICKZST_H_

#include "Ubase.h"
#include "HToolTipEx.h"

#define TICKZST_LWIDTH	52
#define TICKZST_RWIDTH	48
#define TICKZST_BHEIGHT	18

#define STARTREQ		0	//��ʼ����
#define MINUTE_READY	1	//��ʱ���ݾ���
#define LEAD_READY		2	//���Ⱦ���
#define MMLD_READY		3	//������������

#define	CRITICAL_MIN	0x0001
#define	DRAW_TIME		0x0002

typedef	struct tagTickZSTUnit
{
	UINT	Flag;					//��־
	long	nDataDate;				//����
	long	nMinute;				//����
	int		nRealTickDataXh;		//��Ӧ���󵽵��������

	long	Now;
	long	NowVol;
	short   InOutFlag;             // ������־
	long	Average;
	DWORD	VolInStock;
}TICKZSTUNIT, *LPTICKZSTUNIT;

class UTickZst : public UBase
{
	int		m_nCurXTickPos;					//��ǰ��Tick

	int		m_nTickNum;						//��Ч���ݸ���
	BOOL	m_bDrawCross;					//�Ƿ�����
	int		yStart,yBasePrice,yVol,yEndVol,yEndBot;	
											//���ε�λ�õ�
	int		xStart,xEnd;					//�ұߵ����꿪ʼ��
	int		nPriceLnum,nVolLnum,nBotLnum;	//�������ڵ��߸���
	double	fPointPerXTick;					// X �����굥λ
	double	fBasePrice;						//����
	double	fMaxDelta;						//����λ���,������Ĳ�����
	long	lMaxVol;						//�������ֵ
	long	lMaxVolInStock,lMinVolInStock;	//�ֲ���
	double	fBotMaxValue,fBotMinValue;		//LB,DK�������С
	long	nBotMaxValue;					//MMLD,ADL�����
	int		m_nAddedTickNum;
	
	CurrStockData		m_Hq; 
	EXCURRDATA			m_HqEx;
	struct TickData	  *	m_pTick;			//ʵ�����󵽵�Tick
	int					m_nTotalXTick;		//Ҫ���ĵ���
	LPTICKZSTUNIT		m_pXTick, m_pTotolXTick;

	int		nSecLens;						//�ܹ���Tick��		

	BOOL	m_bInSwim;
	BOOL	m_bCrossMode;
	BOOL	m_bHasCross;
	CPoint	m_CrossPoint;					//Cross�����(�������)

	CPoint	m_RealMove;

	long	m_lLastVolume;
	long    m_lLastTickNum;
	float	m_fLastPrice;
	float	m_fLastZs;
	//Զ�̲�����
	BOOL	m_bDataReady;
	short 	m_ReqingFlag;		//-1:δ����	0:���� ����Ϊ���+1
	BOOL	m_bIsHead;
	
	BOOL				m_bManyDayZST;	//��������ͼ
	int					m_ManyDayTickNum[MAX_ZST_MANYDAY];
	struct TickData		*m_pManyDayTickData[MAX_ZST_MANYDAY];
	float				m_fManyDayClose[MAX_ZST_MANYDAY];
	long				m_fManyDayVolin[MAX_ZST_MANYDAY];
	long				m_ManyDayDate[MAX_ZST_MANYDAY];
	BOOL				m_bManyDayIsHead[MAX_ZST_MANYDAY];
	int					m_nDays;

	//����Ҽ�ͳ��
	CPoint				m_ArchorPoint,m_OrgPoint;

	//�ؼ�ʱ��,8��9��
	long	m_CriticalTime[9];
	long	m_nXMaxTickNum, m_nXTickNum;
	//��ʾ������,UP DOWN ��ʱ�����
	long	m_nFirstShow, m_nShowNum;
	BOOL	m_bShowStepSync;	
protected:
	BOOL	m_bSelected;					//�Ƿ�ѡ��,ֻ����ZST_MULTI
	BOOL	m_bYestodayFalg;				//�Ƿ���������,�����赥���ж�
private:
	void CalcIt();
	void DTWH_currMinute();

	void ToNextMin(int nFlag,int nSpace=0);	//���Ҽ�ͷ�ƶ�ʱ����
	void InSwimArea(CPoint pt);				//���Ʊ�
	void LeaveSwimArea();					//�뿪�Ʊ���ʱ
	void DrawInfoArea(int xPos);			//����Ϣ��
	void LonelyDrawAxis(int nWhich,BOOL bSwim=FALSE,int DrawPos=0);	//��������
	void DrawAxisArea(CDC *pDC,CPoint RelaPoint,int nWhich,BOOL bSwim=FALSE,int DrawPos=0);	//���ắ��
	void DrawFrame(CDC *pDC);				//�����

	int GetXTickPosFromX(double x);			//����x����õ�λ��
	int GetXTickTimeFromX(double x);
	int GetYFromValue(double value);		//���ݼ�λ�õ�Y����
	int GetYVolFromValue(double value,BOOL bVolume=TRUE);
	int GetYLbFromValue(double value);
	int GetYMmldFromValue(long value);
	int GetYDiffFromValue(long value) ;
	int GetXFromXTick(int xh,int flag=-1);	//���ݾ�������X����

	void GetCriticalTime(long *pCriticalTime, short *fz, short nCriSecNum);
	BOOL BeCriticalTime(long nCurMin, long &nNextMin);

	void ManyDay_ReqData(long lNowRq);

public:
	UTickZst(LPCOMINFO pComInfo);
	~UTickZst();

	BOOL	m_bBeginRectangle;

	void	SetStyle(int style);
	int		GetStyle() { return m_nStyle; }
	long	GetDate(int whichHis);
	//ͳ����
	long	Xh2TdxMinute(int nXh)	{return System2Tdx(nXh, TimeSec);};
	void	ReadMore(void);
	void	ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void	DrawIt(CDC *pDC);

	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};

#endif

#ifndef UTICKZST_H_
#define UTICKZST_H_

#include "Ubase.h"
#include "HToolTipEx.h"

#define TICKZST_LWIDTH	52
#define TICKZST_RWIDTH	48
#define TICKZST_BHEIGHT	18

#define STARTREQ		0	//开始请求
#define MINUTE_READY	1	//分时数据就绪
#define LEAD_READY		2	//领先就绪
#define MMLD_READY		3	//买卖力道就绪

#define	CRITICAL_MIN	0x0001
#define	DRAW_TIME		0x0002

typedef	struct tagTickZSTUnit
{
	UINT	Flag;					//标志
	long	nDataDate;				//日期
	long	nMinute;				//分钟
	int		nRealTickDataXh;		//对应请求到的数据序号

	long	Now;
	long	NowVol;
	short   InOutFlag;             // 买卖标志
	long	Average;
	DWORD	VolInStock;
}TICKZSTUNIT, *LPTICKZSTUNIT;

class UTickZst : public UBase
{
	int		m_nCurXTickPos;					//当前的Tick

	int		m_nTickNum;						//有效数据个数
	BOOL	m_bDrawCross;					//是否画竖线
	int		yStart,yBasePrice,yVol,yEndVol,yEndBot;	
											//三段的位置等
	int		xStart,xEnd;					//右边的坐标开始处
	int		nPriceLnum,nVolLnum,nBotLnum;	//三个窗口的线格数
	double	fPointPerXTick;					// X 轴坐标单位
	double	fBasePrice;						//中轴
	double	fMaxDelta;						//最大价位离差,与中轴的差的最大
	long	lMaxVol;						//量的最大值
	long	lMaxVolInStock,lMinVolInStock;	//持仓量
	double	fBotMaxValue,fBotMinValue;		//LB,DK用最大最小
	long	nBotMaxValue;					//MMLD,ADL用最大
	int		m_nAddedTickNum;
	
	CurrStockData		m_Hq; 
	EXCURRDATA			m_HqEx;
	struct TickData	  *	m_pTick;			//实际请求到的Tick
	int					m_nTotalXTick;		//要画的点数
	LPTICKZSTUNIT		m_pXTick, m_pTotolXTick;

	int		nSecLens;						//总共的Tick数		

	BOOL	m_bInSwim;
	BOOL	m_bCrossMode;
	BOOL	m_bHasCross;
	CPoint	m_CrossPoint;					//Cross坐标点(相对坐标)

	CPoint	m_RealMove;

	long	m_lLastVolume;
	long    m_lLastTickNum;
	float	m_fLastPrice;
	float	m_fLastZs;
	//远程部分用
	BOOL	m_bDataReady;
	short 	m_ReqingFlag;		//-1:未请求	0:当天 多日为序号+1
	BOOL	m_bIsHead;
	
	BOOL				m_bManyDayZST;	//多日走势图
	int					m_ManyDayTickNum[MAX_ZST_MANYDAY];
	struct TickData		*m_pManyDayTickData[MAX_ZST_MANYDAY];
	float				m_fManyDayClose[MAX_ZST_MANYDAY];
	long				m_fManyDayVolin[MAX_ZST_MANYDAY];
	long				m_ManyDayDate[MAX_ZST_MANYDAY];
	BOOL				m_bManyDayIsHead[MAX_ZST_MANYDAY];
	int					m_nDays;

	//鼠标右键统计
	CPoint				m_ArchorPoint,m_OrgPoint;

	//关键时间,8段9个
	long	m_CriticalTime[9];
	long	m_nXMaxTickNum, m_nXTickNum;
	//显示的数据,UP DOWN 的时候更新
	long	m_nFirstShow, m_nShowNum;
	BOOL	m_bShowStepSync;	
protected:
	BOOL	m_bSelected;					//是否被选中,只适于ZST_MULTI
	BOOL	m_bYestodayFalg;				//是否昨日行情,三方需单独判断
private:
	void CalcIt();
	void DTWH_currMinute();

	void ToNextMin(int nFlag,int nSpace=0);	//左右箭头移动时处理
	void InSwimArea(CPoint pt);				//画移标
	void LeaveSwimArea();					//离开移标区时
	void DrawInfoArea(int xPos);			//画信息区
	void LonelyDrawAxis(int nWhich,BOOL bSwim=FALSE,int DrawPos=0);	//单独画轴
	void DrawAxisArea(CDC *pDC,CPoint RelaPoint,int nWhich,BOOL bSwim=FALSE,int DrawPos=0);	//画轴函数
	void DrawFrame(CDC *pDC);				//画外框

	int GetXTickPosFromX(double x);			//根据x坐标得到位置
	int GetXTickTimeFromX(double x);
	int GetYFromValue(double value);		//根据价位得到Y坐标
	int GetYVolFromValue(double value,BOOL bVolume=TRUE);
	int GetYLbFromValue(double value);
	int GetYMmldFromValue(long value);
	int GetYDiffFromValue(long value) ;
	int GetXFromXTick(int xh,int flag=-1);	//根据绝对数求X坐标

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
	//统计用
	long	Xh2TdxMinute(int nXh)	{return System2Tdx(nXh, TimeSec);};
	void	ReadMore(void);
	void	ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void	DrawIt(CDC *pDC);

	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};

#endif

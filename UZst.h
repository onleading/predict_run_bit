#ifndef UZST_H_
#define UZST_H_

#include "Ubase.h"
#include "HToolTipEx.h"
#include "levin.h"
#include "SwitchEx.h"

#include "DTotalOrder.h"
#include "DCancelOrder.h"
#include "DCjbsStat.h"
#include "DCjtjStat.h"

#define ZST_AUCTIONWIDTH	90
#define ZST_LWIDTH			55
#define ZST_RWIDTH			50
#define ZST_THEIGHT			20
#define ZST_BHEIGHT			18

#define COLORBTN_WIDTH	48
#define COLORBTN_HEIGHT	22

typedef struct 
{
	BYTE	bExist;			//是否有选中的
	BYTE	IsMovable;		//启动时能否处于正在移动状态 ! 他在右键按下时就置位 0/1 .
	BYTE	nInRegion;		//在那个区域
	BYTE	nSet;			//类型(曲线,自画线等,暂时是用不上的)
	short	nSelfLineNo;	//删除者，暂计标记
	short	nInOnePoint;	//落在某个点上 0 - 无 ,1 - 第一个点 ,2 - 第二个点 ....
	POINT	nBeginPt;		//如果是平移，记录其位置
	BYTE	bMoved;			//如果未移动
}ZSTSELINFO;

struct thunderpos
{
	short	tdxmin;
	short	thundernum;
	short	firstpos;
	short   type;	//0:一般 1:券商发表
	short	minetoken;	//地雷标记类型
};

#define	DC_DRAW_TEXT(Color,lpString,Offset)\
	size = g_d.DisplayText(pDC,pos,3,VipColor.##Color,_F(lpString)); pos+=size.cx+Offset;

class UZst : public UBase
{
	BOOL		m_bPlayStatus;					//是不是在分时重播状态
	int			m_nCurMinute;					//当前的分钟数
	int			m_nRealTotalMin;				//一天完整的数据个数
	int			m_nTodayMin;					//今天已有的数据个数

	BOOL		m_bTotalSect;					//是否是整段
	BOOL		m_bDrawCross;					//是否画竖线
	int			yStart;							//开始画分时图的位置(纵向)
	int			yBasePrice;						//分时图价格中间位置(纵向)
	int			yVol;							//开始画成交量的位置(纵向)
	int			yEndVol;						//结束画成交量的位置(纵向)
	int			yStartBot;						//开始画扩展图形位置(有的话)
	int			yEndBot;						//结束画扩展图形位置(有的话)
	int			xAucStart,xStart;				//开始画分时图的位置(横向)
	int			xEnd;							//结束画分时图的位置(横向)
	int			nPriceLnum,nVolLnum,nBotLnum;	//三个窗口的线格数
	double		fPointPerMinute;				//X 轴坐标单位
	////////////////////////////////////////////////////////////////////////////////
	double		fBasePrice;						//分时图中轴价格
	double		fMaxDelta;						//最大成交价位离差,与中轴的差的最大
	double		fMaxPxOff;						//最大价位离差(成交,可能有集合竞价)
	double		fMaxLx;							//领先最大绝对值
	//
	long		m_lMaxVol;						//量的最大值
	long		m_lMaxVolEx;					//量的最大值(成交,可能有集合竞价)
	long		m_lMaxQHPos,m_lMinQHPos;		//持仓量
	//指标区
	//1.只显示成交量;2.量比/多空;3.委差/ADL;4.集合竞价;5.总买总卖;6.成交量统计;7.大单统计;8.成交笔数统计;9.大单笔数统计;10.撤单挂单;11.累计撤单
	double		m_fMaxLB,m_fMinLB;				//量比,多空(领先)用最大最小
	long		m_lMaxADL;						//买卖力道(委差),ADL(涨跌家数)用最大
	double		m_lMaxVirtualPxOff;				//集合竞价虚拟匹配价位离差
	long		m_lMaxVirtualVol;				//集合竞价虚拟匹配量的最大值
	long		m_lMaxBSVol;					//总委(买/卖)最大
	long		m_lMaxOaCVol;					//分钟最大挂撤量
	double		m_fMaxCancelSum;				//累计撤单(买/卖)最大
	long		m_lMaxCjbs;						//成交笔数累计最大
	long		m_lMaxLargeCjbs;				//大单成交笔数累计最大
	double		m_fMaxIOVol;					//内外盘成交量分级
	double		m_fMaxLargeVol;					//大单成交量累加最大
	double		m_fMaxDDZ;
	double		m_fMinDDZ;
	//数据系列
	CurrStockData		m_Hq;
	EXCURRDATA			m_HqEx;
	struct MinuteData	m_MinuteData[1500];
	float				m_LBdata[1500];			//量比
	short				m_nLeadData;			//领先数据个数
	BOOL				m_bLeadReqed;
	NORMALSTAT			m_NormalStat[1500];
	short				m_nNormalStat;
	TICKSTAT			m_TickStat[1500];
	short				m_nTickStat;
	VTLDATA				m_pVtlData[MAX_AUCTIONNUM];
	short				m_nVtlData;
	//交易时间段
	int					nSectTime[4*MAX_ZST_MANYDAY][2];//最多4*MAX_ZST_MANYDAY段
	int					nSectNum;						//场数
	int					nNowSect;						//现在是第几场,仅对半场有效
	int					nSecLens;						//所有分场的总计分钟数(分场时用)
	//是否画领先线
	BOOL				m_bZsLx;
	//控制十字光标线
	BOOL				m_bInSwim;
	BOOL				m_bCrossMode;
	BOOL				m_bHasCross;
	CPoint				m_CrossPoint;
	BOOL				m_bKeyCross;
	long				m_nKeyMin;
	CPoint				m_RealMove;
	//数据维护
	long				m_lLastMinVolume;	 //上一分钟的量	
	int					m_nLastRefreshMinute;//上次记录的分钟数
	int					m_nLastAuctionMinute;//上一次更新集合竞价时间(本地)
	//远程部分用
	BOOL				m_bDataReady;
	//资讯部分用
	struct ext_info_title  *m_pTitle;
	int						m_nTitle;
	struct ext_ggbw_title  *m_pTitle2;
	int						m_nTitle2;
	struct thunderpos		m_thunderpos[1500];
	int						m_thunderposnum;
	int						m_jumpPos;		 //上次雷跳的位置
	BOOL					m_bManualPop;
	//股票叠加
	short				m_nOverlapNum;	
	MemStkInfo *		m_pOverlapStkInfo[3];//最多允许叠加三只股票
	CurrStockData		m_OverlapHq[3];
	struct MinuteData	m_OverlapMinuteData[3][1500];
	//多日分时
	BOOL				m_bManyDayZST;		//多日走势图
	int					m_nZSTManyDay;		//多日走势图的天数
	struct MinuteData	m_ManyDayMinuteData[MAX_ZST_MANYDAY][1500];
	float				m_fManyDayClose[MAX_ZST_MANYDAY];
	long				m_ManyDayDate[MAX_ZST_MANYDAY];
	//价格预警线
	float				m_fWarnPriceH, m_fWarnPriceL;
	//鼠标右键统计
	CPoint				m_ArchorPoint;
	CPoint				m_OrgPoint;
	int					m_nQJPos1;
	int					m_nQJPos2;
	//点击区域
	CRect				m_QSRelativeRect;				//券商定制网页位置
	CRect				m_RelativeRect[MAX_CUSWEB_NUM];	//网页位置
	CRect				m_ViewZoomRect;					//最大还原位置
	CRect				m_ReportBtnRect;				//研究报告平台的按钮
	CRect				m_GubaBtnRect;					//股吧按钮
	CRect				m_FlushRealInfoRect;			//闪烁区位置
	CRect				m_YMDCfgRect;					//预埋单位置
	CRect				m_TokenOnOffRect;				//显示隐去盘面下单图标
	CRect				m_CloseOpenRect;				//昨收开盘作为中轴的位置
	BOOL				m_bFlushRealInfo;				//是否闪烁即时播报
	BOOL				m_bShowLayout;					//是否在右下角显示"操作按钮"
	BOOL				m_bHasSwitch;					//是否有切换区
	
	CRect				m_PeriodRc;
	//特别指标类
	DTotalOrder			m_dTotalOrder;					//总买总卖
	DCancelOrder		m_dCancelOrder;					//净挂净撤,撤单累计
	DCjbsStat			m_dCjbsStat;					//成交笔数累计,大单笔数累计
	DCjtjStat			m_dCjtjStat;					//大单成交量累计
protected:
	///////////////////////////画线工具
	DRAWLINE			m_NewDrawLine;					//添加画线时的临时信息(画完之后才保存)
	ZSTSELINFO			m_SelectInfo;
	BOOL				m_bEvenSelected;
	//
	int					m_nPtNum;
	BOOL				m_bDraw2nd;
	BOOL				m_bDraw3rd;
	BOOL				m_bDrawLine;					//是否画线
	UINT				m_nWay;							//画的类型
	CRect				m_PtInArea;						//用CRect保存，可以充分利用其函数,相对座标
	CRect				m_AbsPtInArea;					//绝对座标
	int					m_nSelfLineIn;					//锁定在哪个区域画线(价格/成交量/扩展)
	//
	CPoint				m_Point[MAX_LINEPOINTNUM*6];	//一般只用不超过MAX_LINEPOINTNUM,其他的为辅助区,辅助区不初始化
	//
	_inline void	ResetSelfLine()
	{		
		m_bDrawLine		= FALSE;
		m_nWay			= 0;
		m_nPtNum		= 0;
		m_bDraw2nd		= FALSE;
		m_bDraw3rd		= FALSE;
		m_nSelfLineIn	=  0;
		m_AbsPtInArea	= 
		m_PtInArea		= CRect(0,0,0,0);
		m_Point[0]		= 
		m_Point[1]		= 
		m_Point[2]		= 
		m_Point[3]		= 
		m_Point[4]		= CPoint(0,0);
		memset(&m_NewDrawLine,0,sizeof(DRAWLINE));
	};
	//
	void	ShowSelfLine(CDC * pDC);
	BOOL	BeginDrawSelfLine(CDC * pDC,CPoint point );
	void	MoveDrawPanRatio( CDC * pDC,int nNo,BOOL bShow=FALSE,int nDrawMode=R2_XORPEN);
	void	DrawingMouseMove(CDC * pDC,CPoint point,int nNo);
	void	SelfLineChg(CDC * pDC,CPoint point);
	void	TransSelfLine(CPoint point,int nNo,int nWhitch);
	CPoint	UTransToPoint(int nNo,int nWhitch);
	int		SelectSelfLine(CPoint point);
	BOOL	CalcLinear(long xPosS,long xPosE);
	//
	long	GetTimeIndexFromX(long x);				//根据x相对坐标得到时间序数
	long	GetXFromTimeIndex(long ti);				//根据时间序数得到x相对坐标
	//
	float	GetPxValFromY(long y);					//根据y相对坐标得到价格值
	long	GetYFromPxVal(float fVal);				//根据价格值得到y相对坐标
public:
	UINT	GetSelfLine();
	void	SetSelfLine(UINT nID);
	///////////////////////////画线工具结束///////////////////////////////
protected:
	BOOL				m_bSelected;					//是否被选中,只适于ZST_MULTI
	BOOL				m_bYestodayFalg;				//是否昨日行情,三方需单独判断
	BOOL				m_bUseOpenAxisForIPO;			//新股是否采用今开为中轴

	//与盘面下单相关的
	struct LevinGN_WTCJListANS	*m_pWTList;
	struct LevinGN_GPListANS	*m_pGPList;
	struct MAIDAN				*m_pYmdList;
	long						m_lCanUseVol;	
	
	RECT		m_pWTRect[MAX_WTCJLIST_NUM];
	RECT		m_pYmdRect[MAX_YMD_NUM];
	int			m_nWTNum,m_nGPNum,m_nYmdNum;
	int			m_nCurWT,m_nCurYmd,m_nCurCopyWT;
private:
	//与股票叠加和多日分时图相关的函数
	void	Overlap_ReadCache();
	void	Overlap_ReqData();
	void	Overlap_ReqHis();
	void	ManyDay_ReadCache(long lNowRq);
	void	ManyDay_ReqData(long lNowRq);
	short	GetOverlapGp(MemStkInfo **pInfo, short nMaxGp);
	void	AutoOverlapGP();
	void	ReCalcZSAverage();
	//其它函数
	void	SetSwitchLabel();
	BOOL	IsSameOpenCloseTimer(MemStkInfo *Info1,MemStkInfo *Info2);
	void	SetBasePrice();
	void	DrawQJLine(CDC *pDC);
	void	GetQzLinkInfo(char *pInfo,MemStkInfo *pStkInfo);
	//
	void	CalcIt();
	void	DTWH_currMinute();

	void	ToNextMin(int nFlag,int nSpace=0);						//左右箭头移动时处理
	void	InSwimArea(CDC *pInDC,CPoint pt,BOOL bInView=TRUE);		//画移标
	void	LeaveSwimArea(CDC *pInDC);								//离开移标区时
	void	DrawAddInfo(CDC *pInDC,int xPos,BOOL bShowLast);
	void	DrawInfoArea(CDC *pInDC,int xPos,BOOL bInView=TRUE);	//画信息区
	void	LonelyDrawAxis(CDC *pInDC,int nWhich,BOOL bSwim=FALSE,int DrawPos=0);				//单独画轴
	void	DrawAxisArea(CDC *pDC,CPoint RelaPoint,int nWhich,BOOL bSwim=FALSE,int DrawPos=0);	//画轴函数
	void	DrawFrame(CDC *pDC);			//画外框

	int		GetTimeFromX(double x);			//根据x坐标得到绝对时间
	int		GetNowYFromX(double x);
	int		GetYFromValue(double value);	//根据价位得到Y坐标
	int		GetYLxFromValue(double value);
	int		GetYVolFromValue(double value,BOOL bVolume=TRUE);
	int		GetYLbFromValue(double value);
	int		GetYMmldFromValue(long value);
	int		GetYDiffFromValue(long value) ;

	int		GetXFromMinute(int minute,int flag=-1);	//根据绝对分钟数求X坐标
	short	GetNowTMinute();
	int		GetXFromTickNo(int nTickNo);

	void	GetPosInfo(int nPos, BYTE &nHisXh, short &nMinXh);	
public:
	HToolTipEx		m_ToolTip,m_LevinToolTip;
	BOOL			m_bBeginRectangle;
	CNewZfjsTab	*	m_pSwitch;
	short			m_nSwitchBtn;

	UZst(LPCOMINFO pComInfo);
	~UZst();

	void	SetStyle(int style);
	int		GetStyle()				{ return m_nStyle; }
	BOOL	GetDataReady()			{ return m_bDataReady; }
	long	GetRealTotalMinuteNum()	{ return m_nRealTotalMin; }
	long	GetFirstMinute(void)	{ return nSectTime[0][0]; }
	long	GetLastMinute(void)		{ return nSectTime[nSectNum-1][1]; }
	long	Xh2TdxMinute(int nXh)	{ return System2Tdx(nXh, TimeSec); }
	int		GetOverlapNum()			{ return m_nOverlapNum; }

	MemStkInfo *GetOverlapMemStkInfo(int which);

	long		GetDate(int whichHis);
	void		ReadData(BOOL bDataReady=TRUE,BOOL bReqData=TRUE);
	void		DrawIt(CDC *pDC);
	int			ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};

#endif

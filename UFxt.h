/*
* Copyright (c) 2002,通达信开发部
* All rights reserved.
* 
* 文件名称：UFxt.h
* 文件标识：
* 摘    要：完成K线图显示、指标画线、平移等功能.
*/
#ifndef UFXT_H_
#define UFXT_H_

#include "Ubase.h"
#include "Interface.h"
#include "InfoDlg.h"
#include "HToolTipEx.h"
#include "SwitchEx.h"

#define MAX_CWDATA		500
#define MAX_FCDATA		100

#define FXT_BHEIGHT		18
#define MAXDOTLINE		50
#define MAXOBJECT		100

#define TABSWITCH_WID	0	

#define MIN_SHOWNUM		20

#define TEXTHIGH		16
#define DMAXVAL			-99999999
#define DMINVAL			999999999999	

//属于哪个集合 0 指标 1 条件 2 专家 3 K线 4 自定义函数(返回最后一行.)
#define OBJECTTOP 30
enum 
{
	//指标线
	ZB_OBJECT = ZB_TYPE,
	//条件选股指示
	TJ_OBJECT = TJ_TYPE,
	//专家系统指示
	EXP_OBJECT= EXP_TYPE,
	//五彩K线指示
	KL_OBJECT = KL_TYPE,
	//用户自定义输出线
	UF_OBJECT = UF_TYPE,
	//特殊画线类型
	SPECLINE_OBJECT = SEPCLINE_TYPE,
	//画线工具中的自画线
	SELFLINE_OBJECT = OBJECTTOP,
	//股本变迁指示
	GBZS_OBJECT	,
	//地雷
	THUN_OBJECT ,
	//个股备忘
	GGBW_OBJECT	,
	//变名数据
	FC_OBJECT	,
	//历史数据
	HIS_DAT		,	
}; 

enum //返回右键按下后应该选择的类型
{
	ID_ANY = 0,
	ID_MAIN = 1,
	ID_SELECTNORMAL = 2,
	ID_MOVE = 3,
	ID_AXIS = 4,
	ID_PERIOD = 5,
	ID_DRAWLINE = 6,
	ID_DELOVERLAPK = 7,
	ID_SELECTEXPZS = 100,
	ID_SELECTKZS = 101
};

////////////////////////////////////////////////////////////////
//		宏函数，便于调用
////////////////////////////////////////////////////////////////

#define ProcessMouse(pfunc)	\
do{		\
	CPoint pt(LOWORD(lParam),HIWORD(lParam));	\
	if ( !PtInRect(pt) || pt.x>=right )	return 1;	\
	CDC *pDC = m_pView->GetDC();			\
	pfunc(pDC,pt);							\
	m_pView->ReleaseDC(pDC);				\
}while(0)

#define PushLastInfo(p)		\
do{		\
	if ( p->nSet == ZB_OBJECT )	\
	{	m_aLastInfo[m_nLastInfo].nGSIndex = p->nGSIndex;	\
		m_nLastInfo = (++m_nLastInfo)%MAX_ZBT_NUM;			\
	}\
}while(0);

#define PopLastInfo(p)		\
do{	static	nNo = 0;			\
	nNo = (++nNo)%11;			\
	long nZB = 0;		\
	if ( m_nLastInfo>0  )			\
	{	if ( m_aLastInfo[m_nLastInfo-1].nSet == ZB_OBJECT )		\
			nZB = m_aLastInfo[m_nLastInfo-1].nGSIndex ,			\
			p = AddObjectByIndex(0,nZB);		\
		memset(m_aLastInfo+m_nLastInfo-1,0,sizeof(ZBTINFO));	\
		--m_nLastInfo;											\
	}		\
	else p = AddObjectByName(ZB_OBJECT,m_strFavUseZb[nNo]);		\
}while(0)

#define ChgAxis(AXIS)	\
{	\
	m_nMainAxisStatus = AXIS;	\
	m_bChgRatio = TRUE;	\
	ProcessMsg(WM_PAINT);	\
}

//锁定功能
#define Lock_K()	\
{	\
	if ( m_bMouseCross )	\
		m_KLock = m_nLeftNo + GetItemNoFromX(m_OldMousePoint.x);	\
	if ( m_bKeyCross )	\
		m_KLock = m_nLeftNo + GetItemNoFromX(m_OldKeyPoint.x);	\
}
#define Lock_KData()	\
{	if ( m_KLock > 0 )	\
		memcpy(&m_KAnalyData,m_pObject->pHisDay+m_KLock,sizeof(AnalyData));	\
}

//调整十字光标
#define RejustCross()	\
if ( m_KLock != -1 && m_KLock < m_nDataNum )			\
{	CDC  * ptmpDC = m_pView->GetDC();					\
	DrawLastState(ptmpDC);								\
	m_OldKeyPoint = GetPointFromItem(m_KLock-m_nLeftNo);\
	DrawXRule(ptmpDC,m_OldKeyPoint.x,m_OldKeyPoint.y);	\
	DrawYRule(ptmpDC,m_OldKeyPoint.x,m_OldKeyPoint.y);	\
	Disp(ptmpDC,m_OldKeyPoint.x,m_OldKeyPoint.y);		\
	DrawLastState(ptmpDC);		\
	m_pView->ReleaseDC(ptmpDC);	\
}

//定位指示的位置
//将两边部分砍掉
#define LocateZS(pDC,Who,Icon,i)	\
do{	\
	(pDC)->DrawIcon(size.cx-8,m_aRegion[i].Area.top,Icon);	\
	Who##.rect.left =  size.cx;		Who##.rect.right =  size.cx+16;	\
	Who##.rect.top  =  m_aRegion[i].Area.top;			\
	Who##.rect.bottom= m_aRegion[i].Area.top+32;		\
	size.cx += 16;		\
}while(0)

#define AppendRg0(pIdx)	\
do{		\
	TOBJECT * pObj = NULL;		\
	for ( int i=0;i<m_aRegion[0].nObject;i++ )	\
		if ( m_aRegion[0].aIndexInfo[i]->nSet == pIdx->nSet && m_aRegion[0].aIndexInfo[i]->nGSIndex == pIdx->nIndexNo )	\
			break;	\
	if ( i == m_aRegion[0].nObject )	\
	{	pObj = AddObjectByIndex(pIdx->nSet,pIdx->nIndexNo);		\
		m_aRegion[0].aIndexInfo[m_aRegion[0].nObject] = pObj;	\
		RejustOneObject(pObj,m_nDataNum);						\
		CalcOne(pObj);							\
		CalcOneGSRatio(pObj,(m_nLastNo+1)-m_nDrawNum);			\
		CalcOneRegionRatio(0);									\
		m_aRegion[0].nObject++;									\
		m_bChgRatio = TRUE;					\
		ProcessMsg(WM_PAINT);				\
	}	\
}while(0)


//必须在第一个BYTE之上0xFF
#define EXTRA_Y		0x00000100			//有额外Y轴
#define DEFINE_Y	0x00000200			//自定义Y轴坐标
#define PUCU_LINE   0x00000400
#define SAR_LINE	0x00000800	
#define BASE_ZERO	0x00001000

////////////////////////////////////////////////////////////////
//		UFXT使用的结构
////////////////////////////////////////////////////////////////

//属于哪个集合 0 指标 1 条件 2 专家 3 K线 4 自定义函数(返回最后一行.)
//将K线数据也算一个部件  30 -- K 线数据
//注:当使用特殊画线时,只有nGSIndex有用,用来表示是哪一种画线类型,其余的统统忽略,在删除此对象时
//也需要做特殊的处理
typedef struct  TOBJECT
{
	short	nCurGP;				//	股票序号
	BYTE	nSet;				//	指标、指示
	long	nGSIndex;			//  公式的ID号
	char	acCode[ZBCODE_LEN]; //  便于快速查找名字
	BYTE	nPeriod;			//  周期
	BYTE	nRefTimes;			//  被引用的次数
	WORD	nArrayLen;			//  数组长度 != nDataNum
	BYTE	nOutNum;			//	几个输出值
	union	
	{	struct 
		{
			struct AnalyData * pHisDay;	//	历史数据也看成一个部件
			short  nMM[2];	
			short  nMaxPos[4];				//  所有输出的最值位置(仅仅限于nDrawNum中找)
			short  nMinPos[4];
		};
		float  *pCalcData[MAX_LINEOUT];		//  计算好的数据,没有的置为空
	};
	double  dMaxVal;		//  所有输出的最值(仅仅限于nDrawNum中找)
	double  dMinVal;
	DWORD	Property;		//  属性(低字节表示nDrawMode,其它位可用)
	DWORD	Reserved;		//  保留(0 )
	TOBJECT* next;
}TOBJECT;

typedef struct RULERINFO
{
	BYTE	bValid;
	long	nZoom;
	double	dFrom;
	double	dStep;
	double	dDelta;
}RULERINFO;

struct  RECTINFO
{
	WORD	thObj;
	BYTE	thOut;
	RECT	aRect;
};

typedef struct  
{
	RECT	Area;			//  显示区域,纵向坐标的刻度由此Rgion负责
	RECT    DrawRc;			//  画线区域
	WORD	nObject;		//  多少个部件
	double  dMaxVal;		//  画图时必须用到，但是每个区域又不同
	double  dMinVal;
	double  dPerUnit;
	double  dMaxValEx;		//  画图时必须用到，但是每个区域又不同
	double  dMinValEx;
	double  dPerUnitEx;
	RULERINFO RulerInfo;
	//  每个部件的内部指针索引,每个区域最多放 MAXOBJECT 个部件			
	TOBJECT* aIndexInfo[MAXOBJECT];
	//  额外为点画线保存的数据
	BYTE	nDotNum;
	BYTE	nBaseDot;
	WORD	nDotInfo[MAXDOTLINE];
	WORD	nRect;
	struct  RECTINFO aRct[58];
}TREGION;

typedef struct 
{
	BYTE	bExist;			//是否有选中的
	BYTE	IsMovable;		//启动时能否处于正在移动状态 ! 他在右键按下时就置位 0/1 .
	BYTE	nInRegion;		//在那个区域
	BYTE	nSet;			//类型(曲线,自画线等)
	union
	{
		//曲线
		struct
		{	short nInObject;	//在那个部件
			long  nGSIndex;
			BYTE  nOutNo;		//那条输出线
			BYTE  bAdd;
		};
		//自画线
		struct 
		{	short nSelfLineNo;	//删除者，暂计标记
			short nInOnePoint;	//落在某个点上 0 - 无 ,1 - 第一个点 ,2 - 第二个点 ....
			POINT nBeginPt;		//如果是平移，记录其位置
			BYTE  bMoved;		//如果未移动
		};
	};
}FXTSELINFO;

typedef struct
{
	BYTE	nSet;
	short   nCurGP;
	long    nGSIndex;
}ZBTINFO;

typedef struct 
{
	RECT		rect;
	TOBJECT*	p;
	char		strInfo[40];
}FXZSINFO;//指示信息

class UFxt : public UBase
{
	BOOL			m_bSelected;					//是否被选中,只适于FXT_MULTI
	CurrStockData   m_Hq; 
	EXCURRDATA		m_HqEx;

	BOOL			m_bDataReady;					//远程部分用
	BOOL			m_bReqing;
	BOOL			m_bIsHead;
public:
	///////////////////////////提示
	HToolTipEx		m_ToolTip;
	CInfoDlg *		m_pInfoDlg;
	BOOL			m_InfoDlgCreating;
	CNewZfjsTab	*	m_pSwitch;
	CNewZfjsTab	*	m_pTabSwitch;
	int				m_TabSel;
	///////////////////////////周月线更新判定数据
	MemStkInfo *	m_pLastStkInfo;
	int				m_LastVolume;
	float			m_LastAmount;
private:
	int				m_nValidFreqZB;					//有效的常用指标
	int				m_nOverlapPenPos,m_nOverlapBrushPos;
	POINT			m_TipArchor;
	CString			m_TipInfo;						//ToolTip提示的信息
	BOOL			m_bOverStk;						//是否有股票叠加
	int				m_nSpecProcDraw;				//特殊处理K线 0-正常 1-涨跌变化K线 2-OX
	BOOL			m_bKeepOver;
	BOOL			m_bXS3;
	CRect			m_last_rect;
private:
	int				m_nTitle;
	int				m_nTitle2;

	int				m_nFilter;						//为TCPIP设置的变量:虑掉的数据个数
	///////////////////////////控制逻辑
	short			m_nChgMouse;					//鼠标发生变化
	CRect			m_MainYRc;
	CRect			m_PeriodRc;						//存放周期显示的位置
	POINT			m_nDownPoint;
	int				m_nAtItem;
	int				m_nAtRegion;
	double			m_dNowVal;
	//
	char			m_strZbMain[ZBNAME_LEN];
	char			m_strFavUseZb[11][ZBNAME_LEN];	//首选指标局部化
	//
	char			m_strCom2ZbMain[ZBNAME_LEN];
	char			m_strCom2FavUseZb[11][ZBNAME_LEN];
	long			m_Com2WinNum;
	BOOL			m_Com2DrawMain;					//定制版面是否画主图指标
	//
	short			m_NormalRegion;					//普通指标组可见区域个数
	BOOL			m_bUseFmlPack;					//是否有绑定指标组
	BOOL			m_bDrawMainZb;					//是否显示主图指标
	long			m_lBandPackNo;					//自动绑定时的模板号
	long			m_lCurPackNo;					//选择指标模板时的模板号
	//
	TINDEXINFO*		m_pColorKIndex;					//五彩K线
	TINDEXINFO*		m_pExpertIndex;					//交易系统
	TINDEXINFO*		m_pDPFXIndex;					//大盘风向，独特的功能
	//
	int				m_PeriodKeyOrder;				//控制周期的变量,用于"]" "[" F8等
	BOOL			m_bNotRedraw;					//设置周期时不刷屏
	BYTE			m_nCommonPeriod;				//普通周期(非模板)
	BYTE			m_nPeriod;						//当前周期
	BYTE			m_nCalcPeriod;					//当前周期对应Vlaue数组哪个
	TOBJECT			*m_pObject;						//指标、指示等一类部件
	short			m_nRegion;						//多少个区域
	short			m_nOldRegion;					
	short			m_nOldSelRegion;
	BOOL			m_bDbClickFT;					//双击过一次副图
	char			m_strDbSaveZb[15];				//放大时用的指标
	short			m_nDataNum;						//有多少个数据
	TREGION			m_aRegion[MAX_ZBT_NUM];
	float			m_aRatio[RATIONNUM];			//存放不同图的比例关系
													//第i个(i从1开始) Start=i*(i-1)/2  Num=i
	BOOL			m_bEvenSelected;				//输出线、指示等曾经选中过。
	FXTSELINFO		m_SelectInfo;					//详细的选中信息。
	short			m_nSelRegion;					//目前激活的区域信息。
	short			m_nSelRegionBak;
	ZBTINFO         m_aLastInfo[MAX_ZBT_NUM];		//最后变化的指标等的记录，增加时，首先考虑。
	short			m_nLastInfo;					//记录到第几个。
	///////////////////////////指示信息
	BOOL			m_bExistZS;
	FXZSINFO		m_ExpZS;
	FXZSINFO		m_KLZS;
	///////////////////////////画图必须参数
	int				m_nNowMine;
	int				m_nWillNum;				//用户计划显示个数，计算宽度的标准，
	int				m_nDrawNum;				//实际画了多少个。在数据不够时，取nDataNum + 3
	int				m_nTodayNum;			//动态维护
	int				m_nLeftNo;				//重第几个开始
	int				m_nLastNo;				//当前画面的最后一个,从0开始计
	int				m_nTmpLeftNo;
	int				m_nTmpLastNo;
	double			m_dWidth;				//K线等的宽度
	double			m_dDistance;
	long		*	m_pPosOfX;				//一次性计算好的X轴的信息
	long		*	m_pAllPosOfX;
	CPoint		*	m_pPoint;
	int				m_nXAxis;
	long		*	m_pXRule;
	int				m_YDy;					//显示Y轴的刻度时，为避免误差而记录
	float			m_fYVal;
	///////////////////////////锁定K线功能
	AnalyData	    m_KAnalyData;
	int				m_KLock;				// -1 没有锁定，>=0 锁定在第几个（从0计起，包括m_nLeftNo），大于m_nDataNum无效。
											//在每次读入数据时，必须更新此数据，以防锁定无效。换股票同样如此。
	long			m_nToLeft;				//计算到两头的次数。
	long			m_nToRight;
	///////////////////////////
	BOOL			m_bKColor;				//是否画五彩K线
	BOOL			m_bNowOverlap;			//是否迭加
	int				m_nOverlapNum;			//迭加的股票个数
	///////////////////////////画十字光标信息
	POINT			m_RealMove;				//确信鼠标真的移动了
	BOOL			m_bMouseCross;
	POINT			m_OldMousePoint;
	BOOL			m_bKeyCross;
	POINT			m_OldKeyPoint;
	int				m_nKeyNo;
	/////////////////////////////画选中框
	BOOL			m_bLArchor;				//左键开始点击	
	BOOL			m_bRArchor;				//右键开始点击
	BOOL			m_bBeginRectangle;
	POINT			m_ArchorPoint,m_OrgPoint;
	int				m_nRightRgn;			//右键框种
	///////////////////////////画线工具状态打开
	DRAWLINE		m_NewDrawLine;			//添加画线时的临时信息(画完之后才保存)
	//
	BOOL			m_bLineValid;			//临时变量,判断画线定位坐标是否能计算得到
	//
	int				m_nPtNum;
	BOOL			m_bDraw2nd;
	BOOL			m_bDraw3rd;
	BOOL			m_bDrawLine;			//是否画线
	UINT			m_nWay;					//画的类型
	CRect			m_PtInArea;				//用CRect保存，可以充分利用其函数。
	CRect			m_AbsPtInArea;
	int				m_nSelfLineIn;			//锁定在哪个区域画线

	CPoint			m_Point[MAX_LINEPOINTNUM*6];	//一般只用不超过MAX_LINEPOINTNUM,其他的为辅助区,辅助区不初始化

	struct AnalyData *  m_pAnalyData;		//得到时间信息,价格等
	struct AnalyData *  m_pOrignData;
	HISDAT			 *	m_pHisData;
	struct AnalyData *  m_pTmpAnalyData;	//转换画法的临时缓存数据
	int					m_nTmpDataNum;		//转换画法的临时缓存数据数量
	int					m_nSHOWNUM;
	//////////////////////
	int				m_nMainAxisStatus;		//主图的坐标状态
	double          m_dR100Max;				//百分比坐标信息
	int				m_bR100Dot;				//是否用小数点
	///////////////////////////用于优化的变量
	BOOL			m_bChgRegion;			//是否从新计算区域大小
	BOOL			m_bChgRatio;			//是否改变间隔、比例、最值等(m_nWillNum变则变)
	///////////////////////////鼠标平移画面
	long			m_nMousePan;					
	CWDATA			CWData[MAX_CWDATA];
	int				m_nCwNum;
	TDXSTKOLDNAME	FCData[MAX_FCDATA];
	short			m_nFCNum;

	BYTE			m_ResetZSFlag;
	int				m_nQJPos1,m_nQJPos2; 
	
	CRect			m_ViewZoomRect;
	
	struct
	{
		BOOL	bAtEage;	//确定是否在边缘
		POINT	EagePoint;
		int		nWhitch;
	}m_EageInfo;
	struct			
	{
		double	dTopPercent;
		double	dTipPercent;
		double	dRatio;
	}m_R100_AXIS;
	_inline void	ResetSelfLine()
	{
		m_bDrawLine		= FALSE;
		m_nWay			= 0;
		m_nPtNum		= 0;
		m_bDraw2nd		= FALSE;
		m_bDraw3rd		= FALSE;
		m_nSelfLineIn	= 0;
		m_AbsPtInArea	= 
		m_PtInArea		= CRect(0,0,0,0);
		m_Point[0]		= 
		m_Point[1]		= 
		m_Point[2]		= 
		m_Point[3]		= 
		m_Point[4]		= CPoint(0,0);
		memset(&m_NewDrawLine,0,sizeof(DRAWLINE));
	};
	_inline void	ResetData()
	{
		m_nDataNum = g_RMSp;
		m_nWillNum  = 0;
		m_nDrawNum  = 0;
		m_bIsHead = FALSE;
		m_bDataReady = FALSE;
		TDEL(m_pOrignData);
	}
private:
	void	GetLastValue();
	void	SaveLastValue();
	BOOL	IsFxtShowTab();
	//切换区相关函数
	void	SetFormulaPack();
	void	SetFrequentZB();
	void	SetSwitchSel();
	void	ResetAllSwitch();
	//
	void	SavePriv();
	BOOL	LoadPriv();
	//信息框相关函数
	void	CreateInfoDlg();
	void	DeleteInfoDlg();
	void	FillDlgInfo(int x);
	void	FillCbInfo(int nY,int nNo);
	void	PopupInfoDlg();
	//
	void	SaveSelectMany(void);
	void	GetSelectMany(void);
	//
	BOOL	GetTipInfo(int x,int y);
	//与股票叠加相关的函数
	void	OverlapGP();
	void	AutoOverlapGP(BOOL bToOverlap);
	short	GetOverlapGp(MemStkInfo **pInfo, short nMaxGp);
	//其它SUB函数
	BOOL	UseLocaldata();
	void	ModifyProperty(TOBJECT *p);
	//
	void	DrawFrame(CDC *pDC);
	////////////////////////////////////维护部件函数
	TOBJECT* AddObjectByName(BYTE nSet,char * acCode,short nGP=-1 );
	TOBJECT* AddObjectByIndex(BYTE nSet,long nGSIndex,short nGP=-1);
	TOBJECT* AddObjectNameCode(BYTE nSet,char * acCode,char *Code=NULL,short setcode=-1);
	void	RegionDelOne(int nRegionNo,TOBJECT	* p,BOOL bKillSelf=FALSE);
	BOOL	SubObject(TOBJECT*);
	void	RejustOneObject(TOBJECT *,int nArrayLen,BOOL bTickCount=FALSE);
	void	RejustAllObject(int nArrayLen);
	void	DelObject();
	void	ChangGS(int nRegoinNo,int nGSIndex,BYTE nSet = ZB_OBJECT,int bReCalc = TRUE);
	//
	BOOL	CalcOne(TOBJECT * p);
	BOOL	CalcAllGS();
	BOOL	XPan(int ndNo,BOOL bNotQuick=FALSE);
	void	CalcR100_AXIS(int nRegion);
	BOOL	CalcOneRegionRatio(int nRegionNo);
	//
	BOOL	CalcOneGSRatio(TOBJECT * p,int Off);
	BOOL	CalcAllRatio(BOOL bRegion=TRUE,BOOL bKeepKPos=FALSE);
	TOBJECT* LocateObject(BYTE nSet,long nGSIndex,short nGP);
	////////////////////////////////定位取值函数
	int		GetItemNoFromX(int x);
	int		GetAbsItemNoFromX(int x);
	int		GetItemNoFromTime(long lTime);
	POINT	GetPointFromItem(int No);
	int		GetYPosR100_AXIS(int nRegionNo,double fY,int nNo=0);
	int		GetYPosOfRegion(int nRegionNo,double fY,int nNo=0);
	double	GetValFromY(int nRegionNo,int y);
	///////////////////////////
	void	InitRegion();
	_inline double GetRegEndRatio(int nNo,int nRegion);
	/////////////////////////////
	void	CalcRuleDrawAxis();
	void	CalcOneAxis(int nRegion);
	void	Disp(CDC * pDC,int nX = -1,int nY =-1,int IsMemDC=0);
	void	InnerDisp(CDC * pMDC,int nRegion,int ItemNo);
	void	DrawYRule(CDC * pDC,int nX=-1,int nY=-1);
	void	InnerDrawY(CDC * pDC,CDC * pMDC,int nRegion,int nX=-1,int nY=-1);
	void	DrawXRule(CDC * pDC,int nX=-1,int nY=-1);
	void	DrawXGrid(CDC * pDC);
	void	DrawKGap(CDC * pDC);
	void	MarkInfo(CDC * pDC);
	void	DrawZBLine(CDC * pDC,int nRegionNo,int nObject,int nOutNo,DWORD nColor, int nWidth);
	void	DrawQJLine(CDC *pDC);
	void	DrawHisLine(CDC * pDC, int nRegionNo,int nObject,int nDrawMode=0,int nFixRegion=-1,BYTE nCurrPen=MAX_PEN_NUM,BYTE nCurrBrush = MAX_BRUSH_NUM);
	void	DrawStick(CDC * pDC,int nRegionNo,int nObject,int nOutNo, DWORD nColor, int nWidth);
	void	DrawColorStick(CDC * pDC,int nRegionNo,int nObject,int nOutNo);
	void	DrawVolStick(CDC * pDC,int nRegionNo,int nObject,int nOutNo);
	void	DrawLineStick(CDC * pDC,int nRegionNo,int nObject,int nOutNo,DWORD nColor, int nWidth);
	void	DrawCircleDot(CDC *pDC, int nRegion, int nObject, int nOutNum, DWORD nColor, int nWidth);
	void	DrawCrossDot(CDC *pDC, int nRegion, int nObject, int nOutNum, DWORD nColor, int nWidth);
	void	DrawPointDot(CDC *pDC, int nRegion, int nObject, int nOutNum, DWORD nColor, int nWidth);
	void	DrawKLine(CDC *pDC, int nRegion, int nObject, int nOutNum, int nWidth);
	void	DrawKLineEx(CDC *pDC, int nRegion, int nObject, int nOutNum, int nWidth, BOOL bSelfColor, DWORD nColor);
	void	DrawPartLine(CDC * pDC,int nRegionNo,int nObject,int nOutNo,DWORD nColor, int nWidth);
	void	DrawSpecialLine(CDC *pDC, int nRegionNo,int nObject);

	void	FillBar(CDC *pDC,int left,int top,int right,int bottom,COLORREF nColor);
	void	DrawStickLine(CDC *pDC, int nRegion, int nObject, int nOutNo, DWORD nColor, int nWidth);
	void	DrawIcons(CDC *pDC, int nRegionNo, int nObject, int nOutNo);
	void	DrawIconsEx(CDC *pDC, int nRegionNo, int nObject, int nOutNo);
	void	DrawTextLine(CDC *pDC, int nRegionNo, int nObject, int nOutNo, DWORD nColor);
	void	DrawTextLineEx(CDC *pDC, int nRegionNo, int nObject, int nOutNo, DWORD nColor);
	void	DrawNumLine(CDC *pDC, int nRegionNo, int nObject, int nOutNo, DWORD nColor);
	void	DrawBand(CDC *pDC, int nRegionNo, int nObject, int nOutNo);

	COLORREF	GetColor(AnalyData * pAnaly, int& nWhichBrush);
	void		DrawOX(CDC *pDC,short col,float val1,float val2,short flag,double yunit,COLORREF color);
	float		GetAllVolume(int num);
	void		DrawPUCU(CDC * pDC,int nRegionNo,int nObject);
	void		DrawSAR(CDC * pDC,int nRegionNo,int nObject);
	//////////////////////////
	void	DrawLastState(CDC * pDC);
	void	ResetCross(CDC * pDC);
	BOOL	QuickSelect(CPoint point);
	int		IsAtEage(CPoint point);
	void	SetMyAxis();
	///////////////////////////鼠标函数
	int		MouseMove(CDC * pDC,CPoint point);
	int		LButtonDown(CDC * pDC,CPoint point);
	int		LButtonUp(CDC * pDC,CPoint point);
	int		RButtonDown(CDC * pDC,CPoint point);
	int		LButtonDblClk(CDC * pDC,CPoint point);
	int		RButtonDblClk(CDC * pDC,CPoint point);
	///////////////////////////画线工具函数
	//在鼠标的一个上下事件中全部完成,中间过程全部锁定。
	void	ShowSelfLine(CDC * pDC);
	BOOL	BeginDrawSelfLine(CDC * pDC,CPoint point );
	void	MoveDrawPanRatio( CDC * pDC , int nNo , BOOL bShow = FALSE,int nDrawMode = R2_XORPEN);
	void	DrawingMouseMove(CDC * pDC,CPoint point,int nNo );
	void	SelfLineChg(CDC * pDC,CPoint point);
	void	TransSelfLine(CPoint point , int nNo,int nWhitch);	//nNo=-1时,表示新画线
	CPoint	UTransToPoint(int nNo,int nWhitch);
	int		SelectSelfLine(int nRegion,CPoint point);
	BOOL	CalcLinear(long xPosS,long xPosE);
	///////////////////////////数据维护函数
	void	ReadSomeStk(TOBJECT * p);
	void	DuiQiStk(TOBJECT * p);
	void	UpdateData(BOOL bForceCalcZB=FALSE);
	////指示
	void	SetZSInfo(BYTE nSet,long nGSIndex);
	BOOL	SelecZS(int nRegion,int nNo,int nItem,CPoint point);
	//
	void	TQdata(int flag);
	void	SelectMany(UINT nID);
	void	AdjustParam();
	void	ChangeMenuZB(int nID,BOOL bFromMenu=TRUE);
	void	ChangeSelfColor();
	void	DelZs();
	///
	void	TransAnalyData(BYTE nTransFlag);
	void	ProcessZBInRegion(UINT	uiMsg);
public:
	UFxt(LPCOMINFO pComInfo);
	~UFxt();

	void	WriteToTxtFile(CString filename,BOOL bWriteExcel=FALSE);
	//
	BOOL	CheckReqing()		{ return (OnLine && m_bReqing); }
	void	DelOverStk();
	BOOL	ExistZS();
	void	SetStyle(int style);
	int		GetStyle()	const	{ return m_nStyle; }
	int		GetSelected() const { return m_bSelected; }
	int		GetDrawNum() const	{ return m_nDrawNum; }
	void	SetSelfLine(UINT nID);
	UINT	GetSelfLine();
	//
	void	SetXPanState(int nType);
	int		GetXPanState() const{ return m_nMousePan; };
	//
	BOOL	ReadData(BOOL bDataReady=TRUE);
	void	DrawIt(CDC *pDC);
	void	SetPeriod(BYTE nPeriod,BOOL bReadData=TRUE,BOOL bReDraw=TRUE,BOOL bCommon=TRUE);
	void	SetCom2Fml(LPFMLPACK pCom2Fml,BOOL bDrawMain);
	void	GetCom2Fml(LPFMLPACK pCom2Fml);
	int		ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
	//直接插入函数体，节省频繁出入栈的时间
	_inline void DrawOneRegion(CDC * pDC,int nRegion);
	_inline BYTE GetCurrPeriod() const
	{
		return m_nPeriod;
	};
	_inline int GetWindowNum()	const
	{
		return m_nRegion;
	};
	_inline int GetAsixType()  const
	{
		return m_nMainAxisStatus;
	}
	_inline void SetSpecProcDraw(int nType) 
	{
		m_nSpecProcDraw = nType;
	}
	_inline int GetSpecProcDraw()  const
	{
		return m_nSpecProcDraw;
	}
	void	JumpToZB(char *ZBCode,int nTarRegion);
private:
	int		RButtonUp(CDC *pDC, CPoint point);
	int		ChoiceType(CPoint point);
	void	ProcessCommand(WPARAM,LPARAM);
};

#endif

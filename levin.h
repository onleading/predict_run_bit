#ifndef LEVIN_H_
#define LEVIN_H_

#define LEVINGN_BUYSELL			1	//下单
#define LEVINGN_CANCEL			2	//撤单

#define LEVINGN_WTLIST			3	//查委托列表
#define LEVINGN_CJLIST			4	//查成交列表
#define LEVINGN_GPLIST			5	//查所有登录帐户的股票
#define LEVINGN_ZHINFO			6	//查已登录的帐户

struct LevinGN_BuySell
{
	char	YybID[6];
	char	Zjzh[31];

	short	Setcode;	//市场类型 0:深圳 1:上海 其它暂未定
	char	Code[9];	//证券代码

	short	nBSFlag;	//0:买	 1:卖
	short	nKPFlag;	//0:开仓 1:平仓
	short	nWTFS;		//市价方式,根据深沪市场不一样
	
	float	fWtPrice;	//
	long	lVol;		//最小委托单位
};

struct LevinGN_Cancel
{
	char	YybID[6];
	char	Zjzh[31];

	char	Wtbh[31];
};

struct LevinGN_BuySellCancelANS
{
	char	bSuccess;		//委托是否成功 1表示成功
	char	Errmsg[100];	//如果success为0,errmsg为错误信息

	char	YybID[6];
	char	Zjzh[31];
	char	Wtbh[20];
};

typedef struct LevinGN_ListREQ
{
	short	Setcode;		//如果setcode为-1,Code为空,请求所有的委托或成交查询
	char	Code[9];
	char	bTodayReq;		//是否是当日查询
	long	lStartDate;
	long	lEndDate;
}LEVINGN_LISTREQ;

typedef struct LevinGN_WTCJListANS
{
	char	YybID[6];
	char	Zjzh[31];
	char	Gddm[31];
	char	Wtbh[31];

	short	Setcode;
	char	Code[9];
	
	short	nBSFlag;	//0:买	 1:卖
	short	nKPFlag;	//0:开仓 1:平仓
	short	nWTFS;		//市价方式,根据深沪市场不一样
	long	lWtDate;	//委托日期,若是当日查询,为0

	long	lSeconds;		//零点以来的秒数
	float	fWtPrice;		//委托价
	float	fCjPrice;		//成交价
	long	lCjVol;			//成交数量 如果是撤,则为负值
	long	lWtVol;			//委托数量 如果是撤,则为负值
}LEVINGN_WTCJLISTANS;

typedef struct LevinGN_GPZhInfoREQ
{
	short unused;
}LEVINGN_GPZHINFOREQ;

typedef struct LevinGN_GPListANS
{
	char	YybID[6];
	char	Zjzh[31];
	char	Gddm[31];
	short	Setcode;
	char	Code[9];
	float	fCbj;		//成本价
	long	lTotalVol;	//股票余额
	long	lCanUseVol;	//可用股票	
}LEVINGN_GPLISTANS;

typedef struct LevinGN_ZhInfoANS
{
	char	YybID[6];
	char	Zjzh[31];
	char	ZhType;		//0:股票,1:期货

	double	dCanUseRmb;	//可用人民币
	double	dCanUseMy;	//可用美元
	double	dCanUseGy;	//可用港元
}LEVINGN_ZHINFOANS;

struct Extern_LevinCfg
{
	BOOL	bCanDbClickPrice;	//是否双击方式
	BOOL	bTradeAllWtl;		//是否扫单
	BOOL	bGenSell;			//买单成交后立即提示用户生成卖单
	float   fGenSellRate;		//卖单价格为现价的多少()%
	BOOL    bGenBuy;			//卖单成交后立即提示用户生成买单
	float   fGenBuyRate;		//买单价格为现价的多少()%
	char	bStop;				//开启止损止赢单,
	float	fStopLossRate;		//止损比例
	float	fStopEarnRate;		//止赢比例
	BOOL	bNoShowWhenCJOver;	//成交后委托图标不再显示
}EXTERN_LEVINCFG; 

typedef struct MAIDAN						//预埋单
{
	char	zqdm[7];						//股票代码
	char	bsflag;							//买卖标志
	char	gddm[20];						//股东代码
	short	wttype;							//委托类型
	long	wtsl;							//委托数量  
	int     bjfs;							//报价方式
	float	wtjg;							//委托价格
	long	wtrq;							//埋单日期
	int		jydw;							//0--股  1--手  2--张
	char	buf[6];							//凑为8的整数倍
}MaiDan;

enum
{
	LF_NORMAIL,			//常规买卖
	LF_SENDYMD,			//发送预埋单
	LF_NEWORDER,		//生成新单
	LF_MMGSORDER,		//买卖跟随单
	LF_STOPLOSSORDER,	//止损单
	LF_STOPEARNORDER	//止盈单
};

enum
{
	FEEDBACK_NEWWT,
	FEEDBACK_NEWCJ,
	FEEDBACK_NEWCANCEL,
	FEEDBACK_YMDCHANGE,
	FEEDBACK_ZJZHCHANGE,
	FEEDBACK_CFGCHANGE
};

enum
{
	TOKEN_BUY,
	TOKEN_SELL,
	TOKEN_BUYOK,
	TOKEN_SELLOK,
	TOKEN_YMDBUY,
	TOKEN_YMDSELL,
	TOKEN_BUYPARTOK,
	TOKEN_SELLPARTOK,
	TOKEN_YMDCFG,
	TOKEN_ONOFF
};	

enum
{
	WTSTATUS_NULL,		//非法单
	WTSTATUS_NOCJ,		//没成交
	WTSTATUS_PARTCJ,	//部分成交
	WTSTATUS_ALLCJ		//全部成交
};

#define MAX_WTCJLIST_NUM	3000	//
#define MAX_GPLIST_NUM		500		//
#define MAX_YMD_NUM			200		//

extern  struct Extern_LevinCfg	g_ExternLevinCfg;
extern  CImageList	g_LevinImage;
extern BOOL g_bHasAdvJy;
extern BOOL g_bShowToken;

extern BOOL IsSameWTBH(struct LevinGN_WTCJListANS *pList1,struct LevinGN_WTCJListANS *pList2); //是否是相同的委托编号
extern BOOL IsSameZjZh(struct LevinGN_ZhInfoANS *pList1,struct LevinGN_GPListANS *pList2); //是否是相同的资金帐户
extern BOOL GetWTList(struct MemStkInfo *pStkInfo,int & nWTNum,struct LevinGN_WTCJListANS *pWTList);
extern BOOL GetGPList(int & nGPNum,struct LevinGN_GPListANS *pGPList);
extern BOOL GetYmdList(struct MemStkInfo *pStkInfo,int & nYmdNum,struct MAIDAN *pYmdList);
extern void SendLevinCancel(CWnd *pView,struct LevinGN_WTCJListANS *pOne);


extern BOOL GetAllCJList(int & nCJNum,struct LevinGN_WTCJListANS *pCJList);
extern void DetectLevinCJ(CWnd *pMainWnd);

extern void ExitLevinCJ();

#endif

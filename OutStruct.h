#ifndef OUTSTRUCT_H_
#define OUTSTRUCT_H_

extern  BYTE TMEANLESS_DATA[8];
#define TMEANLESS_INT				87654321
#define TSET_MEANLESS(a)	        memset(&(a),0xF8,sizeof(a))
#define TSET_NULL(a,from,num)		memset(a+(from),0xF8,sizeof(float)*(num))
#define TBE_MEANLESS(a)				memcmp(&(a),TMEANLESS_DATA,sizeof(a))

#pragma pack(push,1) //请务必一字节对齐

typedef struct tag_NTime
{
	unsigned short year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
}NTime;

//回调函数
typedef long(CALLBACK * PDATAIOFUNC)(char * Code,short nSetCode,short DataType,void * pData/*内部申请*/,short nDataNum,NTime,NTime,BYTE nTQ,unsigned long);
//实现功能切换的回调函数]
typedef long(CALLBACK * PSWITCHINFOFUNC)(char * Code,short nSetCode,char *buf,long buflen,short FuncType,short Option,unsigned long);


typedef struct tag_HISDAT
{
	NTime Time;			//时间
	float Open;			//单位开盘价
	float High;			//单位最高价
	float Low;			//单位最低价
	float Close;		//单位收盘价
	union
	{
		float	Amount;
		DWORD	VolInStock;
	};
	float fVolume;       //成交量
	union 
	{ 
		float YClose;
		struct
		{   
			WORD up;                // 上涨家数
			WORD down;              // 下跌家数
		}zd;						// 单位前收盘价(除权用)，期货的持仓量
	};
}HISDAT,*LPHISDAT;

typedef struct ExDayInfo					//	120B
{
	long			Date;					//	4B		时间
	//
	float			Cjbs;					//	4B		成交笔数(周期大于天,累加)
	float			BuyCjbs;				//	4B		买入成交笔数(周期大于天,累加)
	float			SellCjbs;				//	4B		卖出成交笔数(周期大于天,累加)
	float			BigBCjbs;				//	4B		买入大单成交笔数(周期大于天,累加)
	float			BigSCjbs;				//	4B		卖出大单成交笔数(周期大于天,累加)
	//
	float			ItemNum;				//	4B		采样点数(周期大于天,累加)
	float			BuyItem;				//	4B		主买采样点数(周期大于天,累加)
	float			SellItem;				//	4B		主卖采样点数(周期大于天,累加)
	float			BigItem;				//	4B		大单采样点数(周期大于天,累加)
	float			BigBItem;				//	4B		主买大单采样点数(周期大于天,累加)
	float			BigSItem;				//	4B		主卖大单采样点数(周期大于天,累加)
	//
	float			BuyVolume;				//	4B		主买成交量(周期大于天,累加)
	float			SellVolume;				//	4B		主卖成交量(周期大于天,累加)
	float			BigVolume;				//	4B		大单成交量(周期大于天,累加,快照)
	float			BigBVolume;				//	4B		成交大买量(周期大于天,累加,快照)
	float			BigSVolume;				//	4B		成交大卖量(周期大于天,累加,快照)
	//
	float			BigTickBVol;			//	4B		买入大单成交量(周期大于天,累加,逐笔)
	float			BigTickSVol;			//	4B		卖出大单成交量(周期大于天,累加,逐笔)
	//
	float			fBOrder;				//	4B		买总挂单(周期大于天,累加)
	float			fBCancel;				//	4B		买总撤单(周期大于天,累加)
	float			fSOrder;				//	4B		卖总挂单(周期大于天,累加)
	float			fSCancel;				//	4B		卖总撤单(周期大于天,累加)
	float			fTotalBOrder;			//	4B		收盘时委买总量(周期大于天,取最后一个)
	float			fTotalSOrder;			//	4B		收盘时委卖总量(周期大于天,取最后一个)
	float			fBuyAvp;				//	4B		收盘时委买均价(周期大于天,取最后一个)
	float			fSellAvp;				//	4B		收盘时委卖均价(周期大于天,取最后一个)
	char			Reserved[12];			//	12B
}EXDAYINFO,*LPEXDAYINFO;

typedef struct tag_REPORTDAT
{
	WORD           ItemNum;              // 采样点数
	float          Close;                // 前收盘价
	float          Open;                 // 今开盘价
	float          Max;                  // 最高价
	float          Min;                  // 最低价
	float          Now;                  // 现价
	DWORD          Volume;               // 总手
	DWORD          NowVol;               // 现手(总手差)
	float          Amount;               // 总成交金额
	DWORD          Inside;               // 内盘
	DWORD          Outside;              // 外盘
	float          TickDiff;             // 笔涨跌(价位差)
	BYTE		   InOutFlag;			 // 内外盘标志 0:Buy 1:Sell 2:None
	union
	{
		struct	//个股
		{
			float          Buy;			// 最高叫买价
			float          Sell;        // 最低叫卖价
			float          Buyp[3];		// 三个叫买价
			DWORD          Buyv[3];		// 对应三个叫买价的三个买盘
			float          Sellp[3];	// 三个叫卖价
			DWORD          Sellv[3];	// 对应三个叫卖价的三个卖盘
		}Ggpv;
		struct	//指数
		{
			float			LxValue;	// 领先指标
			float			Yield;		// 不含加权的指数
			long			UpHome;		// 上涨家数
			long			DownHome;	// 下跌家数
			float			JJAmount;	// 基金成交金额
			float			ZQAmount;	// 债券成交金额
			float			GZAmount;	// 国债成交金额
			float			QZAmount;	// 权证成交金额
			float			QTAmount;	// 指数其它成交金额
		}Zspv;
	}Other;
}REPORTDAT,*LPREPORTDAT;	

typedef struct tag_STOCKINFO 
{
	char        Name[9];			// 证券名称

	short       Unit;               // 交易单位	
	long		VolBase;			// 量比的基量
	float       cdp;				// 昨日中价
	float       nh;					// 卖点
	float       nl;					// 买点
	float       ah;					// 高价突破点
	float       al;					// 低价突破点
 	short       dkflag;				// 适合作多或作空
	float       tbp;				// 多空平衡点
	float       stoplost;			// 止损点
	float       leave;				// 了结点

	float       ActiveCapital;		// 流通股本
    long        J_start;			//上市日期
	short       J_addr;				//所属省份
	short       J_hy;				//所属行业
    float       J_zgb;				//总股本
    float       J_zjhhy;			//证监会行业
    float       J_oldjly;			//上年此期净利润
    float       J_oldzysy;			//上年此期主营收入
    float       J_bg;				//B股
    float       J_hg;				//H股
    float       J_mgsy2;			//季报每股收益 (财报中提供的每股收益,有争议的才填)
    float       J_zzc;				//总资产(元)
    float       J_ldzc;				//流动资产
    float       J_gdzc;				//固定资产
    float       J_wxzc;				//无形资产
    float       J_cqtz;				//长期投资
    float       J_ldfz;				//流动负债
    float       J_cqfz;				//长期负债
    float       J_zbgjj;			//资本公积金
    float       J_jzc;				//股东权益(就是净资产)
    float       J_zysy;				//主营收入
    float       J_zyly;				//主营利润
    float       J_yszk;				//应收帐款
    float       J_yyly;				//营业利润
    float       J_tzsy;				//投资收益
    float       J_jyxjl;			//经营现金净流量
    float       J_zxjl;				//总现金净流量
    float       J_ch;				//存货
    float       J_lyze;				//利益总额
    float       J_shly;				//税后利益
    float       J_jly;				//净利益
    float       J_wfply;			//未分配利益
    float       J_tzmgjz;			//调整每股净资产 物理意义:  净资产/调整后的总股本
	//增加部分
	float		J_jyl;				//净益率%
	float		J_mgwfp;			//每股未分配
	float		J_mgsy;				//每股收益
	float		J_mggjj;			//每股公积金
	float		J_mgjzc;			//每股净资产
	float		J_gdqyb;			//股东权益比
}STOCKINFO,*LPSTOCKINFO;

typedef struct tag_TICKDAT
{ 
	NTime	Time;
	float   Now;            //现价(元)
	DWORD   NowVol;         //现手(手)
	float 	Amount;		    //成交金额(元)
	short   InOutFlag;      //买卖标志
}TICKDAT,*LPTICKDAT;

typedef struct tag_BSPDAT
{
	NTime	Time;
	float   Now;
	unsigned long NowAmount;	//单笔成交金额(百元)
	long	NowVol;
	float   BuyP[3];
	long    BuyV[3];
	float   SellP[3];
	long    SellV[3];
	char    InOutFlag;
	char    MaxFlag;//最高价标志 0：最高价在前面已出现或者Now即为最高价1:最高价存放于Max中
	char    MinFlag;
	float   Max;
	float   Min;
}BSPDAT,*LPBSPDAT;

typedef struct tag_GBInfo	//股本总股本信息
{
	float Zgb;
	float Ltgb;
}GBINFO,*LPGBINFO;

typedef struct tag_OtherInfo
{
	short RelativeMin;	//离开市的相对分钟数
}OTHERINFO,*LPOTHERINFO;

typedef struct tag_ALLGPInfo
{
	short StockNum;			//每种分类的股票数
	char  buf[1];			//按市场[1],代码[6],名称[8]顺序存放
}ALLGPINFO,*LPALLGPINFO;

typedef struct tag_FLInfo
{
	char  FLNum;			//最多100个分类
	char  FLName[100][20];	//分类名
	short FLStockNum[100];	//每种分类的股票数
	char  buf[1];			//按市场[1],代码[6],名称[8]顺序存放
}FLINFO,*LPFLINFO;

typedef struct tag_TPPrice
{
	float Close;
	float TPTop;
	float TPBottom;
}TPPRICE,*LPTPPRICE;

typedef struct tag_SysInfo
{
	char	bYesterday;
	long	lOpenRq;
	char	bOnLine;
	char	productnbbs[10];
	//
	char	canlevel2;
	char	cMagic1;
	char	cMagic2;
	char	cIsStdVersion;
	char	strUserName[50];
	//
	char	bDSOnLine;
	char	bIsSSDS;
	char	unused[24];
}SYSINFO,*LPSYSINFO;

typedef struct tag_CurFxtZbData
{
	char	setcode;
	char	strCode[20];
	float	fNow;
	float	fBuyp;
	float	fSellp;
	float	fOutLine[6];
}CURFXTZB,*PCURFXTZB;

typedef struct tag_GPInfoData
{
	char	strCode[20];
	char	strName[40];
	short	setcode;
	short	xsflag;
	char	unused[14];
}GPINFO,*PGPINFO;

typedef struct tag_JYInfoData
{
	char	isLogin;
	char	zjzh[41];
	WORD	yybid;
	char	khqx;
	char	vipuserflag;
	char	MngRight;
	char	unused[33];
}JYINFODATA,*PJYINFODATA;

typedef struct tag_WinRectData
{
	RECT	MainFrameRect;
	RECT	ActiveViewRect;
}WINRECT_DAT,*PWINRECT_DAT;

/////考虑兼容
#define PER_MIN5		0	//5分钟数据,需要下载5分钟数据
#define PER_MIN15		1	//15分钟数据,需要下载5分钟数据
#define PER_MIN30		2	//30分钟数据,需要下载5分钟数据
#define PER_HOUR		3	//1小时数据,需要下载5分钟数据
#define PER_DAY			4	//日线数据,需要下载日线数据
#define PER_WEEK		5	//周线数据,需要下载日线数据
#define PER_MONTH		6	//月线数据,需要下载日线数据
#define PER_MIN1		7	//
#define PER_MINN		8	//
#define PER_DAYN		9	//多天线数据,需要下载日线数据
#define PER_SEASON		10	//季线,需要下载日线数据
#define PER_YEAR		11	//年线,需要下载日线数据

#define	EXDAY_L2		31	//新扩展数据

#define GBINFO_DAT		103	//股本信息
#define	REPORT_DAT		104	//行情数据
#define	STKINFO_DAT		105	//股票相关数据

#define ALLGPINFO_DAT	106	//所有股票码表信息
#define FLGINFO_DAT		107	//分类股信息
#define DYGINFO_DAT		108	//地区板块信息
#define HYGINFO_DAT		109	//行业板块信息
#define GNGINFO_DAT		110	//概念板块信息
#define ZDGINFO_DAT		111	//自定义板块信息
#define ZHGINFO_DAT		112	//组合板块信息

#define TPPRICE_DAT		121	//涨跌停数据
#define SYSINFO_DAT		122	//系统信息
#define CUR_FXTZB_DAT	123	//当前分析图指标数据
#define CURGPINDEX_DAT	124	//返回当前的股票ID
#define GPINDEX_DAT		125	//根据当前代码得到股票ID
#define GPINFO_DAT		126	//根据股票ID 返回当前的信息
#define JYINFO_DAT		127 //得到交易相关信息
#define WINRECT_DAT		128	//窗口的位置和大小

#define OTHER_DAT		130	//其它

/////////////
#define ASK_ALL			-1


//功能切换的类型
#define SWITCH_TOZST			11
#define SWITCH_TOVIP			12
#define SWITCH_ADDBLOCK			13
#define SWITCH_SAVENEWBLOCK		14
#define SWITCH_OPENURL			15		//打开URL连接
#define SWITCH_REFRESHALLHQ		16		//刷新所有行情
#define SWITCH_TOZSGPC			17		//切换到招商股票池
#define SWITCH_TOWARNWT			18		//预警委托
#define SWITCH_JYGETHQ			22		//通知前端更换到对应的走势图
#define SWITCH_SETKEY			23		//将快捷键传给行情
#define SWITCH_SHIFTCJZX		24		//显隐财经资讯功能树(F7)
#define SWITCH_CLOSECJZX		25		//关闭财经资讯
#define SWITCH_CLOSEASSISWND	26		//关闭辅助区
#define SWITCH_REFRESHTITLE		27		//刷新行情标题栏状态
#define SWITCH_LOGINEMBWT		28
#define SWITCH_QUITEMBWT		29		//关闭委托交易区
#define SWITCH_MAXASSISWND		30		//委托辅助区最大化
#define SWITCH_INZXG			31		//是否已在行情自选股中 返回0 表示不在
#define SWITCH_ADDTOZXG			32		//添加到行情自选股中
#define SWITCH_OPENEMBWT		33
#define SWITCH_TOGGLEASSISWND	34		//切换辅助区
#define SWITCH_JY_ISFULLSIZE	35		//交易区是否是全占状态

//////////////////////////////////////////////////////////////////////

typedef struct tag_OP_Scheme_Color
{ 
	COLORREF	BACKCOLOR;			//背景
	COLORREF	BACKCOLOR2;			//背景2
	COLORREF	TXTCOLOR;			//图形文字

	//分析图相关
	COLORREF	AXISCOLOR;			//坐标轴框
	COLORREF	TIMECOLOR;			//坐标文字(包括十字线、游标框)
	
	COLORREF	KPCOLOR;			//阳Ｋ线及上涨字
	COLORREF	KNCOLOR;			//阴Ｋ线
	COLORREF	DOWNCOLOR;			//下跌字
	COLORREF	LEVELCOLOR;			//平线

	COLORREF	VOLCOLOR;			//成交量颜色
	COLORREF	BIGTITLECOLOR;		//大标题
	COLORREF	FLOATCURSORCOLOR;	//高亮颜色

	//列表相关
	COLORREF	Grid_CodeName_Color;//代码名称颜色
	COLORREF	GridLineColor;		//列表的分隔线颜色
	COLORREF	GridHighLineColor;	//高亮选中线的颜色
	COLORREF	GridTitleTextColor;	//栏目串的颜色
	COLORREF	GridTitleArrowColor;//栏目排序的颜色
	COLORREF	Grid_Spec_Color;	//特别颜色，正常情况下为TXTCOLOR

	//资讯颜色
	COLORREF	ZxColor;
	COLORREF	ZxTitleColor;
	COLORREF	MineTitleTxtColor;
	COLORREF	MineTitleSelColor;
	COLORREF	MineTitleSpeColor;
	COLORREF	F10TitleColor;
	COLORREF	F10TitleSelColor;
			
	//网页相关
	COLORREF	LinkColor;
	COLORREF	ActiveLinkColor;
	COLORREF	VisitedLinkColor;
	COLORREF	WebSpecColor;

	COLORREF	OTHERCOLOR[11];		//其它颜色,用于指标或十大股东的饼图
}SCHEME_COLOR;

typedef struct tag_OP_Scheme_Font
{ 
	char		ListFontName[40];
	short		ListFontSize;
	char		GraphFontName[40];
	short		GraphFontSize;
	char		ZxFontName[40];
	short		ZxFontSize;
}SCHEME_FONT;

typedef struct tag_OP_Scheme_Menu
{ 
	short	nMenuMode; 		//0:传统风格 1:XP风格
	short	bExtendXPStyle; //是否是增强型的XP风格 
	long	Reserved;
}SCHEME_MENU;

struct HandleValue//HandleFloat
{
	DWORD	nValidFlag;			//
	DWORD	nHandleType;
	long	nValueLen;			//char
	char	*pValue;			//一般的串
	long	nDataNum;			//两个选择:1.计算串的个数:(1-5,可由nHandleType得到) 2(选中).float组的个数:m_nDataNum
	float	*pFloat;			//用于计算的串
	char	**ppValue;			//动态生成的串
	
	HandleValue()
	{
		nValidFlag = TMEANLESS_INT;
		nHandleType = 0;
		nValueLen = 0;
		pValue = NULL;
		nDataNum = 0;
		pFloat = NULL;
		ppValue = NULL;
	}
};
//////////////////////////////////////////////////////////////////////


#pragma pack(pop)

#endif

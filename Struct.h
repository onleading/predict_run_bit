#ifndef   STRUC_H
#define   STRUC_H

#include "Interface.h"
#include "info.h"

typedef struct
{
	float Price;
	long  Vol;
}PRICE_VOLSTRUCT; 

struct tag_FindResult
{
	char which;
	char path[256];
};

struct tag_FindResult2
{
	char whichtype;		//类型
	char title[256];	//标题或文件名
	char whichindex;	//第几号
};

struct GpDomain_Struct
{
	short flag;		//flag,0:分类股 1:地域板 2:行业板 3.概念板块 4.组合板块 5:自定义板块
	short which;	
	char  Name[10];
};

struct ZHB_BlockInfo_Struct
{
	char	zhmc[11];	//组合名字
	short	nStockNum;
	short   nContent[MAXGPNUM];	//最多USER_SIZE只股票
};

struct BlockInfo_Struct
{
	int tag;
	char Name[100];
	char KeyGuyCode[50];
};

typedef struct BlockColor
{
	char		BlockName[100];	//板块名
	COLORREF	clrBlock;		//板块颜色
	DWORD		unnsed;			//备用
}BLOCKCOLOR,*LPBLOCKCOLOR;

struct IndexSortInfo_Struct	//支持所有输出线
{
	int Index;
	int Cycle;
	int Param[MAX_PARAMNUM];
};

struct CONDITIONINFO
{
	char acCode[ZBCODE_LEN];//指标英文名称
	int  ParamNum;
	int  ParamValue[MAX_PARAMNUM];
	int  CalcCycle;				//计算周期 0:5分钟线 1:15分钟 ...
};

struct FILEHEAD
{
	int		ConditionNum;
	BOOL	m_IsAndRadio;
	long	nDate;
	short	nRefreshTime;
	DWORD	Flag[3];
};

typedef struct
{
	int		nTjIndexNo;
	BOOL	bUsed;			//是否参与预警
	char	acCode[50]; 	//指标英文名字
	float	aTjParam[6];	//条件参数
	int		nPeriod;		//分析周期
}TJ_WARN_OLD;

typedef struct
{
	int		nTjIndexNo;
	BOOL	bUsed;			//是否参与预警
	char	acCode[50]; 	//指标英文名字
	float	aTjParam[MAX_PARAMNUM];	//条件参数
	int		nPeriod;		//分析周期
}TJ_WARN;

//用于预警系统的结构
typedef struct
{	int			nLB;             	//量比突破点
	BOOL		bLB;				//是否量比预警
	float		fUpRatio;			//涨幅上突破
	BOOL		bUpRatio;			//是否涨幅上突破
	float		fDownRatio;			//跌幅上突破
	BOOL		bDownRatio;			//是否跌幅上突破
	float		fTickAmount;		//大单异常(笔金额)
	BOOL		bTickAmount;		//是否大单异常
	float		fTickDiff;			//单笔异常(笔升跌)
	BOOL		bTickDiff;			//是否单笔异常
	float		fTickVol;			//大量异常(笔成交量)
	BOOL		bTickVol;			//是否大量异常
	BOOL		bLowHigh;			//低开高走
	BOOL		bHighLow;			//高开低走
	BOOL		bStopClose;			//是否进行停板预警
	BOOL		bFzZF;				//是否进行分钟涨速
	BOOL        bNewMaxWarn;		//创新高报警开关
	BOOL        bNewMinWarn;		//创新低报警开关
	BOOL        bHasBigBuy;			//有大买单
	BOOL        bHasBigSell;		//有大卖单
	BOOL		bPopup;				//是否弹出预警对话框;
	BOOL		bTickHugeAmo;		//是否连续大笔预警
	int			nTickAmoMinute;		//连续大笔预警时间段
	int			nTickAmoTime;		//连续大笔频率
	int			nBeepFlag;			//0:无声音 1:系统声音 2:自定义声音
	char		WavFile[80];		//Wav文件路径
}WARNGLOBALPARAM;

typedef struct 
{
	BOOL		bTJPopup;			//是否弹出预警对话框;
	int			nTJBeepFlag;		//0:无声音 1:系统声音 2:自定义声音
	char		TJWavFile[80];		//Wav文件路径
	char		nWarnFlag;			//0:缺省全市场，1:自选股
	char		ununsed[99];		//备用
}WARNGLOBALPARAM2;

typedef struct
{
	SYSTEMTIME   time;
	short   nStockNo;
	float   fPrice;
	long	nVol;
	short	InOutFlag;	
	short   nReason;
	short	nStockType;	//nStockType = 0:系统预警股票,1:指定预警股票
} WARN_STRU;

typedef struct						//价位突破跌破条件
{
	char	code[7];               //股票序号
	char	setcode;
	float   fHighPrice;             //突破价
	float   fLowPrice;			 	//跌破价
	char	bDoLevin1;				//闪电下单1 0:无功作 1:买入 2:卖出
	char	bDoLevin2;				//闪电下单2 0:无功作 1:买入 2:卖出
	char	bEverDo1;				//每次提示1
	char	bEverDo2;				//每次提示2
	unsigned short	ETFAPRate;		//ETF折/溢价率
	unsigned short	TurnOverRate;	//换手率
} PWARNINGCND;

struct FirstLevel
{
	char Text[30];
	int  SonNum;
};

struct sort_struc
{
	float fValue;
    short nIndex;
	short tag;
};

struct sort_struc2
{
	char StrValue[21];
	short nIndex;
	short tag;
};

struct sort_struc3
{
	float fValue;
	short nItemNum;
    short nIndex;
	short tag;
};

//Grid颜色结构
struct GRIDCOLOR
{
	COLORREF GridBackColor;
	COLORREF GridLineColor;
	COLORREF GridHighLineColor;
	COLORREF GridTitleTextColor;
	COLORREF GridTitleArrowColor;
	COLORREF Grid2GridColor;

	COLORREF Grid_Up_Color;
	COLORREF Grid_Down_Color;
	COLORREF Grid_Level_Color;
	COLORREF Grid_CodeName_Color;
	COLORREF Grid_Volume_Color;
	COLORREF Grid_Other_Color;
};

struct VIPCOLOR
{
	COLORREF			BACKCOLOR;	//背景
	COLORREF			AXISCOLOR;  //坐标轴框
	COLORREF			TIMECOLOR;	//坐标文字(包括十字线、游标框)
	COLORREF			KPCOLOR;	//阳Ｋ线及上涨字
	COLORREF			KNCOLOR;    //阴Ｋ线
	COLORREF			LEVELCOLOR; //平线
	COLORREF			DOWNCOLOR;  //下跌字
	COLORREF			BIGTITLECOLOR;//大标题
	COLORREF			USCOLOR;//美国线
	COLORREF			TXTCOLOR;//图形文字
	COLORREF			FLOATCURSORCOLOR;//高亮颜色
	COLORREF			DRAWLINETOOLCOLOR; //分析TOOL的色//画线工具的线色
	COLORREF			ZBCOLOR[6];		//一般不会多于5个指标
	COLORREF			VOLCOLOR;		//成交量
	COLORREF			COMVIEWINFOBC;	//走势图分析图上方信息区
	COLORREF			UNDEFINE1;
	COLORREF			UNDEFINE2;
};

struct TXTCOLOR
{
	COLORREF TxtForeColor;
	COLORREF TxtBackColor;
};

struct TREECOLOR
{
	COLORREF TreeForeColor;
	COLORREF TreeBackColor;
};

struct MENUCOLOR //另外的菜单颜色，考虑到兼容性，无须配置
{
	COLORREF MenuBackColor;
	COLORREF MenuTitleColor;
	COLORREF MenuColor1;
	COLORREF MenuColor2;
	COLORREF MenuColor3;
	COLORREF MenuColor4;
};

//系统颜色结构体,包含四个子结构
struct SYSTEMCOLOR
{
	struct VIPCOLOR		VipColor;
	struct GRIDCOLOR	GridColor;
	struct TXTCOLOR		TxtColor;
	struct TREECOLOR	TreeColor;
};

//KeyGuy中的结构
struct KeyGuyContent
{
	int Type;	//0:股票 1:指标 2:命令
	char Content[19];
};

//系统设置的颜色Item结构
struct SchemeItemContent
{
	char str[30];
	int  id;
};

#define   SH_CODE_LEN      6
#define   NAME_LEN         8

struct CmdTable
{
    char    Name[40];
    char    Code[8];
    WPARAM  wParam;
    LPARAM  lParam;	   
};

struct DynaCmdTable
{
	char EnName[16];
	char ChName[40];
	char Author[30];		//设计人
	char Descript[200];		//描述
	char HYString[HY_STRING_LEN];	//行业串
	BOOL ShowShortCut;		//是否显示快捷
	DWORD DefPadSet;
	DWORD DefPadCmdID;
	CmdTable CmdInfo;
	int nCmdNum;			//命令快捷数
	int	nNextDefNum;		//除去已用快捷数下一个缺省可用快捷数
};

//
typedef struct TickStat					//	122B	在TMN中
{
	WORD		Minute;					//	2B		统计时间
	float		fAmount[3][5];			//	60B		总/大/中:主买/主卖/被买/被卖/未知数量
	float		fVol[3][5];				//	60B		总/大/中:主买/主卖/被买/被卖/未知数量
	DWORD		VolNum[3][5];			//	60B		总/大/中:主买/主卖/被买/被卖/未知数量
}TICKSTAT,*LPTICKSTAT;

typedef struct PzCalcRes 
{
	time_t		CalcTmie;					//计算时间
	long		nDataNum;					//数据个数
	int			nCalcOKNum[MAX_LINEOUT];	//满足条件的个数
	float		fOut[MAX_LINEOUT];			//计算结果
}PZCALCRES, *LPPZCALCRES;

struct DiskStkInfo         // 用于在.INF文件中定位的临时数据结构
{
  char           Code[SH_CODE_LEN+1];   // 证券代码
  char           tp_flag;               // 停牌标志
  short          Index;                 // 在.IHQ中的序号
  short          Unit;                  // 交易单位
  float          ActiveCapital;         // 流通股本
  char           Name[NAME_LEN+1];      // 证券名称
  char           fl_flag;               // 分类标志
  long	    	 VolBase;               // 量比的基量(前5天的量比)
  short          IndexOfHQ;            // 在Z_HQ中的序号
  short          IndexOfMMP;           // 在Z_MMP中的序号
};

struct MemStkInfo           // 在.INF文件中读入的在内存中的数据结构
{
	char        Code[CODE_SIZE];   // 证券代码
	char        tp_flag;               // 停牌标志
	short       Index;                 // 在.IHQ中的序号
	float       Unit;                  // 交易单位
	float		AmoUnit;
	float       ActiveCapital;         // 流通股本
	char        Name[NAME_SIZE];       // 证券名称
	char        fl_flag;               // 分类标志,现在用于标记mark
	long		VolBase;               // 量比的基量
	float       cdp;                   // 昨日中价
	float       nh;                    // 卖点
	float       nl;                    // 买点
	float       ah;                    // 高价突破点
	float       al;                    // 低价突破点
 	short       dkflag;                // 适合作多或作空
	float       tbp;                   // 多空平衡点
	float       stoplost;              // 止损点
	float       leave;                 // 了结点

	short       J_addr;         //所属省份
	short       J_hy;           //所属行业
    long        J_gxrq;         //更新日期
	long		J_start;		//上市日期
    float       J_zgb;          //总股本
    float       J_zjhhy;        //证监会行业
    float       J_oldjly;       //上年此期净利润
    float       J_oldzysy;      //上年此期主营收入
    float       J_bg;           //B股
    float       J_hg;           //H股
    float       J_mgsy2;        //季报每股收益 (财报中提供的每股收益,有争议的才填)
    float       J_zzc;          //总资产(万元)
    float       J_ldzc;         //流动资产
    float       J_gdzc;         //固定资产
    float       J_wxzc;         //无形资产
    float       J_cqtz;         //长期投资
    float       J_ldfz;         //流动负债
    float       J_cqfz;         //长期负债
    float       J_zbgjj;        //资本公积金
    float       J_jzc;          //股东权益(就是净资产)
    float       J_zysy;         //主营收入
    float       J_zyly;         //主营利润
    float       J_yszk;         //应收帐款
    float       J_yyly;         //营业利润
    float       J_tzsy;         //投资收益
    float       J_jyxjl;        //经营现金净流量
    float       J_zxjl;			//总现金净流量
    float       J_ch;			//存货
    float       J_lyze;         //利益总额
    float       J_shly;         //税后利益
    float       J_jly;          //净利益
    float       J_wfply;        //未分配利益
    float       J_tzmgjz;       //调整每股净资产 物理意义:  净资产/调整后的总股本
	float		J_HalfYearFlag;		//全部更改为以月为单位

	//用于日线统计数据的更新
	WORD		nImgStatLastSeconds;	//此股上次日线统计数据的更新时间
	WORD		nTckStatLastSeconds;	//此股上次日线统计数据的更新时间
	//各股票的预警系统的基准变量
	short		BuySellFlag;			//大买卖盘的标志 1买 -1卖
	short		CloseStopFlag;			//停板标志 0,1,2
	long		nWarnCount;				//报警次数
	float		zangsu;					//在基本资料中补充涨速
	//其它信息,用于远程
	short		BaseFreshCount;			//基本资料的更新次新
	short		GbbqFreshCount;			//股本变迁的更新次新
	float		Close;
	//增加setcode,标识其市场
	char		setcode;				//注意是setcode,主要用于远程方便
	//增加bigmarket,标识商品大类
	char		bigmarket;
	BYTE		DocNum;					//对于三方行情,有意义
	short		TimerID;
	char		SpellCode[SPELLCODE_SIZE];

	TICKSTAT	its;
	PzCalcRes	CalcRes1;
	PzCalcRes	CalcRes2;
	PzCalcRes	CalcRes3;
	time_t		NearHqTime;
};

typedef	struct CurrStockData     // 在.IHQ文件中读入的数据结构 Modified 2003.9.29
{
  DWORD          ItemNum;               // 采样点数
  float          Close;                 // 前收盘价,指数
  float          Open;                  // 今开盘价,指数
  float          Max;                   // 最高价,指数
  float          Min;                   // 最低价,指数
  float          Now;                   // 现价,最近指数
  union
  {
	  DWORD          RefreshNum;        // 刷新数,对于普通行情有效
	  DWORD          OpenVolume;        // 开仓量
  };
  float			 Lead;					// 领先指标
  DWORD          Volume;                // 总手
  DWORD          NowVol;                // 现手(总手差)
  float          Amount;                // 总成交金额
  DWORD          Inside;                // 内盘
  DWORD          Outside;               // 外盘
  union
  {
		float	TickDiff;				// 笔升跌(价位差)
		long	VolInStockDiff;			// 持仓差,对于期货而言
  };
  union
  {      
	  float Yield;           // 市盈率1,不含加权的指数
	  DWORD VolInStock;      // 持仓量
  }Yield_VolInStock;
  float          Buyp[5];               // 五个叫买价
					// 基金成交金额:Buyp[0]
					// 债券成交金额:Buyp[1]
					// 国债成交金额:Buyp[2]
  DWORD          Buyv[5];               // 对应五个叫买价的五个买盘
					// 上涨家数:Buyv[0]
  float          Sellp[5];              // 五个叫卖价
					// 权证成交金额:Sellp[0]
					// 期货昨持仓量:Sellp[1]
					// 指数其它成交金额:Sellp[1]
					// 期货昨结算价:Sellp[2]
					// 指数昨不含加权指数:Sellp[2]
  DWORD          Sellv[5]; // 对应五个叫卖价的五个卖盘
					// 下跌家数:Sellv[0]
					//	期货特殊的买卖标志:Sellv[1]
					//  期货内盘中开仓Sellv[2] 
					//	期货外盘中开仓Sellv[3]
  union
  {
      struct
      {
	    WORD     InOutFlag:2;           // 内外盘标志
	    short    sdunit:4;              // 笔升跌单位(用于计算领先指标)
	    WORD     RestVol:7;             // 零股
	    WORD     Unused:3;              // 未用标志
      }          Flag;                  // 标志位结构
      struct
      {
        WORD     InOutFlag:2;           // 内外盘标志
        WORD     YQSY:14;               // 国债预期收益
      }          GZFlag;
  };
  union
  {
	  DWORD         CJBS;				// 升位增加的字段，成交笔数
	  float			ClearPrice;			// 期货结算价
	  char			HKTTFlag;			//	港股分笔特殊标记
  };
  union
  {
  	float        Jjjz;
	DWORD		 Reserved;
  };
}CURRDATA,*PCURRDATA;

typedef	struct ExCurrData
{
	float	AverPrice;						//	平均价
	union
	{
			float	PE;						//	市盈率
			float	PreClear;				//  前结算
			float	HK_Dividend;			//	港股每股派息
			float	GdsyNowSyl;				//	固定收益的当日最新收益率
	};
	union
	{
			float	YieldVal;				//	收益率
			long	lSanRatio;				//  期货分笔多开比例
			float	HK_Profit;				//	港股每股盈利
	};
	union
	{
			float	MrkCap;					//	市值
			DWORD	PreVolIn;				//	前持仓
			float	HK_StockSum;			//	港股股本
			DWORD	GdsyYjlx;				//	固定收益应计利息
	};
	float	HisHigh;						//	历史最高价
	float	HisLow;							//	历史最低价
	union
	{
			float	PreDelta;				//	昨虚实度
			float	IEP;					//	开盘参考竞价
	};
	union
	{
			float	NowDelta;				//	今虚实度
			float	IEV;					//	开盘参考成交量
	};
	union
	{
		float	PreMinum;					//	溢价％
		DWORD	OutOpen;					//  外盘开	
	};
	union
	{
		float	Gearing;					//	杠杆比率
		DWORD	OutClose;					//	外盘平
	};
	union
	{
		float	ExecPrice;					//	行使价
		DWORD	InOpen;						//	内盘开
	};
	union
	{
		float	ConvRatio;					//	认购比率
		DWORD	InClose;					//	内盘平
	};
	char	ExpireDate[16];					//	期权到期日
	WORD	BuySeatNum[5];					//  买队列
	WORD	SellSeatNum[5];					//  卖队列			
	BYTE	HasLink;						//	有没有相关股票
	float	HSpread;						//	高买卖差额
	float	LSpread;						//	低买卖差额
	DWORD	PreVol;							//  每手股数
	long    HqDate;							//  行情数据日期
	long	HqTime;							//	行情数据时间
	float	fZhangsu;						//	涨速
	long	VolBase;						//  量比的基量
	char	Info1[13];
	char	Info2[13];
	BYTE	Reserved[1];					//	标示行情源
}EXCURRDATA, *PEXCURRDATA;

struct CurrStockDataEx
{
	char		SetCode;
	char        Code[SH_CODE_LEN+1];
	struct CurrStockData hq;		   // 原始行情
	char        dkflag;				   // 
	float       tbp;                   // 多空平衡点
	float       stoplost;              // 止损点
	float       leave;                 // 了结点
	float		zangsu;			   // 涨速
	unsigned short	nWarnCount;			   // (活跃度)报警次数
};

typedef struct TickData      // 在.TIC文件中读入的数据结构
{ 
	short          Minute;              // 零点以来的分钟数
	long           Now;                 // 现价*1000
	long		   NowVol;              // 现手
	long           VolInStock_dif;      // 持仓量增减
	union
	{
		short      InOutFlag;             // 对于期货,数值部分表示开市部分占NowVol的?/%%-1 +10000*InOutFlag
		struct
		{
					char HKTTFlag;
					char HKInOutFlag;
		} HKFlag;
	};
} TICKDATA,*PTICKDATA;

typedef struct VirtualData					//	16B		集合竞价
{
	short		Minute;						//	2B		时间
	float		fPrice;						//	4B		虚拟匹配价格
	DWORD		dwVolume;					//	4B		虚拟匹配成交
	long		dwLeaveQty;					//	4B		虚拟未匹配量(为正时,表示买单一侧,为负表示卖单一侧)
	char		reserved;					//	1B
	char		Second;						//	1B		秒数
}VTLDATA,*LPVTLDATA;

typedef struct TickDataOfMtc      // 在.TIC文件中读入的数据结构
{ 
	short       Minute;             // 零点以来的分钟数
	long		Now;                // 现价*1000
	long		NowVol;             // 现手
	long        VolInStock_dif;     // 持仓量增减
	short       InOutFlag;          // 买卖标志
	long        Lead;               // 指数领先指标
	DWORD		Buyv;				// 叫买量　上涨家数　　　　叫买量
	DWORD		Sellv;				// 叫卖量　下跌家数　　　　叫卖量　
	float       NowAmount;
} TICKDATAOFMTC;

typedef struct  MinuteData                      // 在.IFZ文件中读入的数据结构1
{
	short Minute;                         // 零点以来的分钟数
	float Now;           // 现价   现指数          现价        White Line
	float Average;       // 均价 不加权指数        动态结算价  Yellow Line 
	DWORD NowVol;        // 现手 现成交金额(100元) 现手
	DWORD Buyv;          //叫买量　上涨家数　　　　叫买量
	DWORD Sellv;         //叫卖量　下跌家数　　　　叫卖量　
	union
	{
	  DWORD VolInStock;   //       持仓量(期货)
	  float Lead;         //       领先指标(指数)
	}VolInStock_Lead;
}MINDATA,*PMINDATA;

union long_short
{         
	long    Date;               // 日期YYYYMMDD(日线)
    struct
    {
		WORD Day:11;			//月日
		WORD Year:5;			//年, 离2004的年数
		WORD Minute:16;         // 零点以来的分钟数
    }Daye;
};

typedef struct AnalyData	// 在.IFZ文件中读入的历史数据记录
{
  union long_short Time;				 // 时间,历史原因, 存到盘上的数据都是 *100 以后的 DWORD 型数据
  union { DWORD l_Open;  float Open;  }; // 单位开盘价
  union { DWORD l_High;  float High;  }; // 单位最高价
  union { DWORD l_Low;   float Low;   }; // 单位最低价
  union { DWORD l_Close; float Close; }; // 单位收盘价
  union {float Amount;  DWORD QHVolInStock;}; 
										 // 单位成交金额(日线/指数)
  DWORD Volume;                          // 单位成交量,单位成交金额(指数,100元)
  union 
  { 
	  float YClose;
	  struct
	  {   WORD up;                  // 上涨家数
	      WORD down;                // 下跌家数
	  }zd;							// 单位前收盘价(除权用)，期货的持仓量
  };								// 指数,涨跌家数
}ANALYDATA,*LPANALYDATA;

/////////////////////////////////////////////////////////////////////////////////
//扩展数据盘后
#define EXDAYBLOCK_LEN				3072

typedef struct ExdayHead					//	3072B	文件头:其后为索引区,再往后为数据区
{
	long lStockNum;							//	4B		股票个数
	long lIndexSpc;							//	4B		股票索引区空间(目前为6000)
	long lBlockNum;							//	4B		当前总的Block个数
	char Reserved[3060];					//	3060B	保留基本块大小
}EXDAYHEAD,*LPEXDAYHEAD;

typedef struct ExDayIndex					//	3072B	盘后扩展数据股票索引
{
	char	Code[16];						//	16B		股票代码
	long	lDataNum;						//	4B		数据个数
	long	lDataBlockNum;					//	4B		数据块使用个数
	long	lDataBlockNo[762];				//	3048B	数据块编号
}EXDAYINDEX,*LPEXDAYINDEX;
//
typedef struct ExDayData					//	96B
{
	long			Time;					//	4B		时间
	//
	unsigned long	Cjbs;					//	4B	L2	成交笔数
	unsigned short	BuyCjbsRatio;			//	2B	L2	买入成交笔数/成交笔数*50000,取整
	unsigned short	SellCjbsRatio;			//	2B	L2	卖出成交笔数/成交笔数*50000,取整
	unsigned short	BigBuyCjbsRatio;		//	2B	L2	买入大单成交笔数/买入成交笔数*50000,取整
	unsigned short	BigSellCjbsRatio;		//	2B	L2	卖出大单成交笔数/卖出成交笔数*50000,取整
	//
	unsigned long	ItemNum;				//	4B		采样点数
	unsigned short	BuyItemRatio;			//	2B		主买采样点数比例*50000,取整
	unsigned short	BigItemRatio;			//	2B		大单采样点数比例*50000,取整
	unsigned short	BigBuyItemRatio;		//	2B		主买大单采样点数占大单比例*50000,取整
	//
	unsigned long	Volume;					//	4B		成交量
	unsigned short	OutSideRatio;			//	2B		主买成交量/成交量*50000,取整(L1:快照;L2:逐笔)
	unsigned short	BigVolRatio;			//	2B		大单成交量/成交量*50000,取整(快照)
	unsigned short	BigBuyVolRatio;			//	2B		主买大单成交量/大单成交量*50000,取整(快照)
	unsigned short	BigBuyTckRatio;			//	2B		买入大单成交量/成交量*50000,取整(逐笔)
	unsigned short	BigSellTckRatio;		//	2B		卖出大单成交量/成交量*50000,取整(逐笔)
	//
	float			fBOrder;				//	4B	L2	Stock:买净挂单(备用:Index:总委买)
	float			fBCancel;				//	4B	L2	Stock:买净撤单
	float			fSOrder;				//	4B	L2	Stock:卖净挂单(备用:Index:总委卖)
	float			fSCancel;				//	4B	L2	Stock:卖净撤单
	float			fBuyAvp;				//	4B	L2	Stock:委买均价
	float			fSellAvp;				//	4B	L2	Stock:委卖均价
	//
	char			Code[13];				//	13B		股票代码
	char			bTransed;				//	1B		是否已经转档
	char			Reserved[18];			//	18B		保留
}EXDAYDATA,*LPEXDAYDATA;
//Level2的新数据结构
typedef struct CurrStockData_L2				//	96B		在.IH2文件中读入的数据结构
{
	float		Buyp[5];					//	20B		叫买价
	DWORD		Buyv[5];					//	20B		对应叫买价的买盘	  
	float		Sellp[5];					//	20B		叫卖价
	DWORD		Sellv[5];					//	20B		对应叫卖价的卖盘
	float		BuyAvp;						//	4B		委买均价
	float		SellAvp;					//	4B		委卖均价
	DWORD		TotalBuyv;					//	4B		总委买量
	DWORD		TotalSellv;					//	4B		总委卖量
}CURRSTOCKDATA_L2;

typedef struct CurrQueue					//  216B
{
	DWORD			RefreshNum;				//	4B		刷新数
	float			Buyp;					//	4B		委买价
	float			Sellp;					//	4B		委卖价
	unsigned short	buy1num;				//	2B		买一的详细挂单笔数
	unsigned short	sell1num;				//	2B		卖一的详细挂单笔数
	unsigned short	buy1vol[50];			//	100B	买一队列
	unsigned short	sell1vol[50];			//	100B	卖一队列
}CURRQUEUE;

typedef struct MarkInfo						//	4B		在.MRK中读入的数据结构
{
	DWORD		L2TicNum;					//	4B		L2逐笔成交数
}MARKINFO,*LPMARKINFO;

typedef struct TickData_L2					//	12B		在.TI2文件中读入的数据结构
{
	WORD		nSecond;					//	2B		早上6点以来的秒数
	DWORD		Now;						//	4B		现价*10000
	DWORD		NowVol;						//	4B		现量
	union
	{
		WORD Rest;
		struct
		{
			char	RestVol;				//	1B		零股
			char	InOutFlag;				//	1B		内外盘标志
		};
	};
}TICKDATA_L2;

typedef struct NormalStat					//	46B		行情统计信息
{
	WORD		Minute;							//	2B		统计时间
	float		Now;							//	4B		最终价格
	float		BuyAvp;							//	4B		买均
	DWORD		TotalBuyv;						//	4B		总买
	float		SellAvp;						//	4B		卖均
	DWORD		TotalSellv;						//	4B		总卖
	union {DWORD NetBOrder;	float fBOrder;};	//	4B		买净挂单.分钟:DWORD;累计:float
	union {DWORD NetBCancel;float fBCancel;};	//	4B		买净撤单.分钟:DWORD;累计:float
	union {DWORD NetSOrder;	float fSOrder;};	//	4B		卖净挂单.分钟:DWORD;累计:float
	union {DWORD NetSCancel;float fSCancel;};	//	4B		卖净撤单.分钟:DWORD;累计:float
}NORMALSTAT,*LPNORMALSTAT;

//
typedef struct SumTckStat				//	126B	在TST中
{
	WORD		Minute;					//	2B		统计时间
	float		fVol[3][5];				//	60B		总/大/中:主买/主卖/被买/被卖/未知数量
	DWORD		VolNum[3][5];			//	60B		总/大/中:主买/主卖/被买/被卖/未知数量
	DWORD		CJBS;					//	4B
}SUMTCKSTAT,*LPSUMTCKSTAT;

typedef struct SumImgStat				//	52B		在IST中
{
	long			RefreshNum;
	float			LargeVol;			//	4B		大单总量(源于快照,跟随tick)
	float			LargeInVol;			//	4B		主买大单总量(源于快照,跟随tick)
	float			LargeOutVol;		//	4B		主卖大单总量(源于快照,跟随tick)
	unsigned long	InItemNum;			//	4B		主买Tick个数(源于快照,跟随tick)
	unsigned long	OutItemNum;			//	4B		主卖Tick个数(源于快照,跟随tick)
	unsigned long	LargeItemNum;		//	4B		大单Tick个数(源于快照,跟随tick)
	unsigned long	LargeInItemNum;		//	4B		大单主买Tick个数(源于快照,跟随tick)
	unsigned long	LargeOutItemNum;	//	4B		大单主卖Tick个数(源于快照,跟随tick)
	float			fBOrder;			//	4B	L2	买净挂单
	float			fBCancel;			//	4B	L2	买净撤单
	float			fSOrder;			//	4B	L2	卖净挂单
	float			fSCancel;			//	4B	L2	卖净撤单
}SUMIMGSTAT,*LPSUMIMGSTAT;
//
typedef struct DDEReport
{
	float		fDDX;					//		当日DDX
	float		fDDY;					//		当日DDY
	float		fDDZ;					//		当日DDZ
	float		fDDX59;					//		前59日DDX
	float		fDDX60;					//		60日DDX
	float		fDDY59;					//		前59日DDY
	float		fDDY60;					//		60日DDY
	short		DDXRed59;				//		60天内,之前最近的连红DDX天数
	short		DDXRed9;				//		之前9天内DDX红的天数
	short		DDXRed;					//		DDX连红天数
	short		DDXRed10;				//		DDX最近10天红的天数
	float		fBigBuyRate;
	float		fBigSellRate;
	float		fBOrderRate;
	float		fAveTradeVol;
	float		fBigIOAmount;
	float		fBigIOWeight;
}DDEREPORT,*LPDDEREPORT;

//拖拉机
typedef struct TractorObj										//	14B
{
	BYTE	WarnFlag;											//	1B		买卖方向
	float	Price;												//	4B		拖拉机售价
	BYTE	OrderNum;											//	1B		拖拉机车厢数
	long	FirstQty;											//	4B		第一节大小
	long	DiffQty;											//	4B		前后差别,后一笔减去前一笔
}TRACTOROBJ,*LPTRACTOROBJ;
//委托异动
typedef struct OrderShockObj									//	14B
{
	BYTE	WarnFlag;											//	1B		方向:1:挂买单;2:撤买单;3:挂卖单;4:撤卖单
	float	Amount;												//	4B		估计委托金额
	float	Volume;												//	4B		估计委托数量
	char	Reserved[5];										//	5B
}ORDERSHOCKOBJ,*LPORDERSHOCKOBJ;
//主力大单
typedef struct LargeVolObj
{
	BYTE	WarnFlag;											//	1B		方向:0:主动买;1:主动卖
	float	Price;												//	4B		
	float	Volume;												//	4B		
	char	Reserved[5];										//	5B		
}LARGEVOLOBJ,*LPLARGEVOLOBJ;
//涨跌加速
typedef	struct QuickChgObj
{
	BYTE	WarnFlag;											//	1B		方向:0:价格上涨;1:价格下跌
	float	QuickChgRate;										//	4B		连续涨跌幅度
	char	Reserved2[9];										//	9B
}QUICKCHGOBJ,*LPQUICKCHGOBJ;
//价格反转
typedef	struct ReversalObj
{
	BYTE	WarnFlag;											//	1B		方向:0:价格反弹;1:价格跳水
	float	ReversalRate;										//	4B		价格反转幅度
	char	Reserved2[9];										//	9B
}REVERSALOBJ,*LPREVERSALOBJ;
//分钟价格异动
typedef struct PxShockObj
{
	BYTE	WarnFlag;											//	1B		方向:0:价格上破;1:价格下破
	float	PxShockRate;										//	4B		价格5分钟突破幅度
	char	Reserved2[9];										//	9B
}PXSHOCKOBJ,*LPPXSHOCKOBJ;
//单笔异动
typedef struct TickShockObj
{
	BYTE	Reserved1;											//	1B
	float	TickPxRate;											//	4B		单笔价格偏离幅度
	char	Reserved2[9];										//	9B
}TICKSHOCKOBJ,*LPTICKSHOCKOBJ;
//集中放量
typedef struct VolShockObj
{
	BYTE	Reserved1;											//	1B
	float	VolumeRate;											//	4B		最近三分钟的放量情况
	char	Reserved2[9];										//	9B
}VOLSHOCKOBJ,*LPVOLSHOCKOBJ;

typedef struct WarnAtom											//	32B
{
	short		SetCode;										//	2B
	char		Code[SH_CODE_LEN+1];							//	7B
	short		WarnType;										//	2B		预警类型
	DWORD		WarnID;											//	4B		预警ID
	union Detail												//	14B
	{
		TRACTOROBJ		uTractor;
		ORDERSHOCKOBJ	uOrderShock;
		LARGEVOLOBJ		uLargeVol;
		QUICKCHGOBJ		uQuickChg;
		REVERSALOBJ		uReversal;
		PXSHOCKOBJ		uPxShock;
		TICKSHOCKOBJ	uTickShock;
		VOLSHOCKOBJ		uVolShock;
	}uDetail;
	char		Hour;											//	1B		hh
	short		MinSec;											//	2B		mmdd
}WARNATOM,*LPWARNATOM;

#define DWARN_SPACE		1000

typedef struct DWarnInf
{
	long		nWarnNum;
	long		nSZIDPos;
	long		nSHIDPos;
	LPWARNATOM	pWarnAtom;
	long		nWarnSpace;
}DWARNINF,*LPDWARNINF;

//以下结构为.mtl文件的构成
struct WTRankings							//
{
	char	Code[SH_CODE_LEN+1];			//			证券代码
	double	TotalOrderQty;					//			委托量
	double	TotalVolumeTraded;				//			成交量
	double	TotalValueTraded;				//			成交额
};

struct HYRankings							//
{
	char	IndustryCode[8];				//			行业代码
	double	IndustryTotalValueTraded;		//			行业总成交额
};

struct HYGPRankings							//
{
	char	Code[SH_CODE_LEN+1];			//			证券代码
	double	TotalValueTraded;				//			成交额
};

struct SingleCancelRankings					//
{
	char	Code[SH_CODE_LEN+1];			//			证券代码
	double	OrderEntryTime;					//			委托时间(DateTime)
	double	Quantity;						//			委托时间
	float	Price;							//			价格
};

struct TotalCancelRankings					//
{
	char	Code[SH_CODE_LEN+1];			//			证券代码
	double	TotalWithdrawnQty;				//			总撤单量
};

struct MarketTotalInfo											//
{
	struct WTRankings			WTBuyRank[12][10];				//			买委托排名
	struct WTRankings			WTSellRank[12][10];				//			卖委托排名
	double						MarketTotalValueTraded;			//			市场总成交额
	struct HYRankings			HYRank[10];						//			行业排名
	struct HYGPRankings			HYGPRank[10*5];					//			行业内股票排名
	struct SingleCancelRankings	SingleCancelBuyRank[12][10];	//			单笔买撤单排名
	struct SingleCancelRankings	SingleCancelSellRank[12][10];	//			单笔买撤单排名
	struct TotalCancelRankings	TotalCanceBuyRank[12][10];		//			撤单卖总量排名
	struct TotalCancelRankings	TotalCanceSellRank[12][10];		//			撤单卖总量排名
};

struct CurrStockData_Code
{
	char   SetCode;
	char   Code[SH_CODE_LEN+1];
	struct CurrStockData	hq;		// 原始行情
	struct CurrStockData_L2 hql2;	// Level2行情
};

struct CurrQueue_Code
{
	char   SetCode;
	char   Code[SH_CODE_LEN+1];
	struct CurrQueue	queue;		// 买卖队列
};

/////////////////////////////////////////////////////////////////////////////////

struct  PriceVolTable
{ 
	DWORD Price;
	DWORD Volume;
	DWORD Kcl;
	DWORD Pcl;
};

struct fltype               // 股票种类过滤条件位结构
{  
	WORD     astk:1;    //A股
	WORD     bstk:1;    //B股
	WORD     jj:1;      //基金
	WORD     gz:1;      //国债
	WORD     qiz:1;     //企业债券
	WORD     qz:1;      //认,配股权证
	WORD     qtstk:1;   //其它
	WORD	 zxbstk:1;	//中小企业
	WORD	 sb:1;		//三板
	WORD	 addcode:1;	//配售比如003
	WORD     cybstk:1;  //创业板
	WORD     unused:5;  //
};

union   flunion 
{
	struct	fltype ftype;
	WORD	fshort;
};

struct bc_date
{
    unsigned short da_year; // Year - 1980
    char  da_day;  // Day of the month
    char  da_mon;  // Month (1 = Jan)
};

struct bc_time
{
    char  ti_min;  // Minutes
    char  ti_hour; // Hours
    char  ti_hund; // Hundredths of seconds
    char  ti_sec;  // Seconds
};

struct date 
{
  int da_year;     /* current year */
  char da_day;     /* day of the month */
  char da_mon;     /* month (1 = Jan) */
};

struct time 
{
  unsigned char  ti_min;   /* minutes */
  unsigned char  ti_hour;  /* hours */
  unsigned char  ti_hund;  /* hundredths of seconds */
  unsigned char  ti_sec;   /* seconds */
};

typedef struct
{
	char   DocPath[256];	//历史数据路径
	short  GPFZNUM;			//股票分钟数
	short  QHFZNUM;			//期货分钟数
	short  JbmNum;			//基本面路径个数
	char   Jbmname[3][10];	//基本面来源名称
	char   SZBASE[3][100];	//深圳基本面路径
	char   SHBASE[3][100];	//上海基本面路径
} CFGINFO ;

typedef struct
{	long 	Date;     			// 日期
	long 	TimeFlag;           // 时间戳
	float 	Close;              // 昨收
	struct 	TickData *pTick;    // Tick 存放缓冲区
	short 	nTickNum;           // Tick 数目
}FBTICKINFO;

typedef struct
{   long 	Date;               // 日期
	short   nTickNum;           // Tick 数目
}DATEANDNUM;

////////////////////////////////////////////////////////
typedef struct 
{
	char	topic[40];
	short	MsgNo;
	short	Status;
	short	type;
}CLE_CONTABLE;

typedef struct
{
	unsigned int Cxg_flag:1;   //次新股标志
	unsigned int P_flag:2;
	//大盘股标志 =0
	//中盘股标志 =1
	//小盘股标志 =2
	unsigned int J_flag:2;
	//绩优股标志 = 0
	//绩平股标志 = 1
	//绩差股标志 = 2
	//亏损股标志 = 3
	unsigned int Jzc_GD_flag:1;
	//净资产高 = 1
	//净资产低 = 0
	unsigned int Gjj_G_flag:1;
	//公积金高 = 1
	unsigned int Gclr_G_flag:1;
	//滚存利润高标志
	unsigned int BIAS_Buy_flag:1;	 //BIAS买入指示标志
	unsigned int SAR_Buy_flag:1;     //SAR买入指示标志
	unsigned int KDJ_Buy_flag:1;     //KDJ买入指示标志
	unsigned int RSI_Buy_flag:1;     //RSI买入指示标志
	unsigned int MACD_Buy_flag:1;    //MACD买入指示标志
	unsigned int MTM_Buy_flag:1;     //MTM买入指示标志
	unsigned int SCNX_flag:1;        //上穿年线标志
	unsigned int Free_Bit:17;		 //备用bit
}CLE_OTHERXG_RES;

typedef struct
{
	long Xz_sign;//小盘,中盘分界  4500万股
	long Dz_sign;//大盘,中盘分界  15000万股

	float Yp_sign;//绩优,绩平分界  每股税后利润（每股收益）>= 0.60元
	float Pc_sign;//绩平,绩差分界  每股税后利润（每股收益）<  0.60元 并且
							   //每股税后利润（每股收益）>= 0.10元
	float Ks_sign;//亏损标志

	short Cxg_Day_L;//次新股上市时间次新标志

	float Jzc_G_sign;//净资产高标志
	float Jzc_D_sign;//净资产低标志

	float Gjx_G_sign;//公积金高标志
	float Gclr_G_sign;//滚存利润高标志
	short DMI_Day;	  //DMI： 14日DMI
	short RSI_Day;    //长周期RSI：12日RSI
	short WVAD_Day;	  //24日WVAD
} CLESIGN;

typedef struct
{
	char	topic[30];	//描述
	short	MsgNo;		//条件号
	short	ConType;	//行情，基本面,技术指标值等几种
	short	ValueType;	//><= 是否 选择等
	char	UnitStr[10];//单位字串
} CUSTXG_CONTABLE;

typedef struct
{
	short	MsgNo;		//条件号
	int		PreFix;
	union
	{
		float	fValue;
		DWORD	nValue;
		DWORD	nSel;
	}Value;
	char	ConStr[50];
} USER_CONS;

typedef struct
{
	short	MsgNo;		//条件号
	int		PreFix;
	union
	{
		float	fValue;
		DWORD	nValue;
		DWORD	nSel;
	}Value;
} REMOTE_USER_CONS;

typedef struct
{   
	char     Flag[8];
	struct
	{	char Version:4;
		char Compressed:1;
		char Reserved:3;
	}Info;
	short FieldNum;
	char reveser[5];
}ATTHEADER,*LPATTHEADER;

typedef struct
{
	char 	Source[40];
	char 	Title[100];
	char 	URL[194];
	short 	Type;
	long 	pos;
	long 	len;
}ATTBODY,*LPATTBODY;

struct NotifyFile
{
	char cardno[30];
	char serno[11];
	char cardname[30];
	char version[13];
	short status;
};

typedef struct 
{
	short Minute;
	float Now;
	DWORD NowVol;
	float BuyP[5];
	DWORD BuyV[5];
	float SellP[5];
	DWORD SellV[5];		

	float NowAverage;		//当前成交均价
	DWORD CJBSDIF;

	short InOutFlag;     //用于读买卖盘时带出Tic的买卖标志
}TICK_MMP;

enum
{
	NORMAL_AXIS	=   0,
	SAMESCALE_AXIS,	
	SAMESECT_AXIS,
	R100_AXIS,
	LOG10_AXIS ,
	GOLDEN_AXIS ,
};

struct UFjContent
{
	long  price;	//价位*1000
	long  volume;
	long  buyvol;
	long  sellvol;
	short rate;		//比例*1000
	float buyrate;	//竞买率
};

struct CacheFile_Head
{
	short dataflag;
	short nDaynNum;//仅对多日线有效,其它为0
};

//////////////////////////////////////////////////////////////////

typedef struct tag_InfoDlg
{
	int	 style;
	char name[NAME_SIZE];
	char time1[20];
	char value[20];
	char open[20];
	char max[20];
	char min[20];
	char close[20];
	char volume[20];
	char amount[20];
	char volinstk[20];
	char junja[20];
	char zhangdie[20];
	char zhangfu[20];
	char zhenfu[20];
	char lastclose[20];
	char hsl[20];
	char ltgb[20];
	int	 datanum;
}INFODLG,*LPINFODLG;

typedef struct tag_CbInfo
{
	struct AnalyData *m_pData;
	int	 m_nDataNum;
	int  m_nWhichPeriod;
	int  m_nCurr;
	int  m_nY;
	int	 m_nYC;	
	int	 m_nBottom;
	int	 m_nTop;
	float	m_fNowVal;
	double  m_dMaxPrice;
	double  m_dMinPrice;
	MemStkInfo	* pStk;
}CBINFO,*LPCBINFO;

//画线工具部件
//版本:
//0:最原始的版本
//1:增加文字注释的字号及颜色初始设置
//2:修改时间格式,增加画线的线宽,线型,颜色属性
//3:分时画线目前以绝对分钟序号来标识,仅画当天的
typedef struct DRAWLINE
{
	char		SetCode;
	char		Code[8];		//证券代码
	char		acCode[10];  	//指标英文简称
	short		nPeriod;		//分析周期
	UINT		nWay;			//分析方法
	long		TimeS;			//开始时间
	float		fPriceS;		//价格的定位
	long		lLineNo;		//画线的编号(从0起,载入时以及增删(增时加到末尾,删时置为-1,退出存盘时清除删掉的)时维护,运行时不改变)
	union
	{
		struct
		{
			long        TimeE;
			float       fPriceE;
			long        TimeR[10];			//保留字段
			float		fPriceR[10];
			DWORD		lversion;			//用来处理老版本属性
			long		lLineWidth;			//线宽
			long		lLineType;			//线型:实线/虚线/点划线
			COLORREF	lcrf;				//颜色
		};
		struct
		{
			char		szBuffer[216];			//文字
			char		tReserve[20];
			DWORD		tversion;				//用来处理老版本颜色
			int			width;
			int			height;
			int			FontSize;				//字体大小//真实大小:-1*(FontSize+6)
			COLORREF	tcrf;					//一个DWORD存放颜色
			int			nUsePoint;
		};
		BYTE		bUpDown;
	};
	BYTE		bDelete;					//如果删除，暂且放着，到程序退出再处理，便于中间检索。
}DRAWLINE,*LPDRAWLINE;

typedef struct FormulaPack				//公式组模板
{
	char	aPackName[32];				//描述
	short	lRectNum;					//可见个数
	short	nPeriod;					//周期,锁定周期为nPeriod+1
	char	aFormular[10][32];			//公式排列(全部)
}FMLPACK,*LPFMLPACK;

typedef struct FormularToStock			//个股绑定到模板
{
	short	setcode;
	char	code[13];
	char	aPackName[32];				//描述
}FML2STK,*LPFML2STK;

////////////////////////////////////////////////////////////////////////
//			远程结构
////////////////////////////////////////////////////////////////////////

struct ManyHostInfo
{
	char strHostName[21];
	char bWebPage;
	char strURL[100];
	char NotifyStr[91];
};

typedef struct
{
	int		Num;
	char	SetCode[20];
	char	Code[20][7];
	float	Now[20];
	float	Other[20];
}ZHRESULT,*LPZHRESULT;

typedef struct
{
	int  HostNum;
	int  PrimaryHost;
	char HostName[150][50];
	char IPAddress[150][50];
	unsigned short Port[150];
	int  weight[150];							//权重
	char szYybids[150][100];
}TDXWHOST;

typedef struct MinCacheHead
{
	char	suc;	//有没有缓存
	long	pos;
}MinCacheHead;

typedef struct CacheHisZst
{
	long	date;
	float	Close;
	struct MinuteData mindata[250];
}CACHEHISZST,*LPCACHEHISZST;

struct HisTickCacheHead
{
	long	nGPIndex;
	long	nDate;
	long	nTickNum;
	long	nStartPos;
};


//////////////////////////传统版///////////////////////////
typedef struct
{   
	char            Name[80] ;         // 子菜单名
    unsigned long	NextMenuID;        // 下一级菜单的ID
	unsigned long	ItemID;			   // 此菜单项的ID,为-1表示无意义,其它在不同的菜单中表示不一致
}MENUITEMTDX;

struct SYS_INFO
{
	char content[256];//系统消息的内容，最长255个字符
	char url[256];//如果指向某一个链接，填接地址，否则url[0]=='\0';
};
struct NLBInfo
{
	BOOL bProcTest;
	long ClientNum;
	long MaxConnectNum;
	long Diff_TickCnt;
};

struct TdxJyHQ
{
	float   Close;
	float   Now;
	float   Buyp[5];
	DWORD   Buyv[5];
	float   Sellp[5];
	DWORD   Sellv[5];
	float	gzlx;
	DWORD	Volume;
	float   unused[4];
};

struct ExternHQ
{
	//现价，今开盘，前收盘，最高，最低
	long last_price;
	long open_price;
	long close_price;
	long high_price;
	long low_price;
	//成交金额，成交量
	long business_balance;
	long business_amount;
	//买入价
	long buy_price1;
	long buy_price2;
	long buy_price3;
	long buy_price4;
	long buy_price5;	
	//卖出价
	long sale_price1;
	long sale_price2;
	long sale_price3;
	long sale_price4;
	long sale_price5;
	//买入量
	long buy_amount1;
	long buy_amount2;
	long buy_amount3;
	long buy_amount4;
	long buy_amount5;
	//卖出量
	long sale_amount1;
	long sale_amount2;
	long sale_amount3;
	long sale_amount4;
	long sale_amount5;
	//股票代码
	char Logo_Stock_Code[11];
};

struct ETInfo
{
	char GPSetCode;
	char GPCode[SH_CODE_LEN+1];
	char Ready;
	char IsTdxJyHQ;
	HWND HWnd;
	struct ExternHQ HQ;
	struct TdxJyHQ JyHQ;
};

#define GUEST_LOGIN		0
#define REGISTER_LOGIN	1
#define JY_LOGIN		2

typedef struct tag_LoginInfo
{
	char Has_GuestLogin;
	char GuestLogin_Str[20];
	char Has_RegisterLogin;
	char RegisterLogin_Str[20];
	char Has_JyLogin;
	char JyLogin_Str[20];

	char bDialogRegister;
	char bCanDelMySelf;
	char bMiscJyStyle;
	char bHasAutoRelation;
	char bHasWebRelation;
	BOOL bOnlyWebUser;
	BOOL bEnableUserName;
	char Web_RegisterURL[200];
	char Web_UserInfoURL[200];
}LOGININFO;

struct RollInfo_Habit
{
	BOOL		bRollOpen[2];		//是不是处开打开状态
	int			nScrollType[2];		//0,连续滚动，1，滚动一屏时暂停
	int			nInfoType[2];		//0,只显示系统信息，1,只显示自选股
	int			nMovingSpaceTime[2];//滚动间隔时间,越大越慢
	int			nMovingStopTime[2];	//滚动停止时间,越大停止时间越长
	COLORREF	crColor[2];			//缺省的滚动颜色

	BOOL		bShow;				//是否显示
	BOOL		bFloat;				//是否是浮动的
	BOOL		bTopMost;			//浮动后是否最在最前
};

struct Global_Habit
{
	BOOL AWeekNoHint;
	long NoHintStartDate;
	BOOL NoXHCol;
	BOOL NoVScroll;
	int  nShowJUNJ;
	int	 nCurBlockFlag;
	BOOL bRealUPK;
	BOOL bDrawMainZb;			//是否画主图指标
	long nLastHintAutoupTime;	//上次提示查看自动升级的日期
	BOOL bPillarBSChart;
	BOOL bBigHq;
	BOOL bBoldZSTLine;
	BOOL bLeftHQInfo;
	BOOL bBigStatusBar;
	BOOL bDynaTJBlock;
	
	int  nPerScale;
	int  nPerSect;
	
	BOOL bLocalData;
	int	 nInitKNum;
	int	 nInitKNum2;
	BOOL bOnlyDownAGData;
	BOOL bShowKGap;
	BOOL bShowMmpChg;
	
	BOOL bFullScreenNotify;
	BOOL bAddBlockPrompt;
	BOOL bLockHQSort;
	BOOL bZSTLevel2Face;
	BOOL bFXTLevel2Face;
	BOOL bTick2Priority;
	
	BOOL bShowBS;
	int	 StatBigVol;
	BOOL bPinAssistPad;
	BOOL bCodeCanSort;
	BOOL bShowTPPriceInZST;
	BOOL bDiffVolColor;

	long lCheckUrgentDate;
	BOOL bCheckUrgent;
	
	long lUseCount;

	BOOL bEmptyZSTRight;
	BOOL bZsShowAverage;
	BOOL bZsShowLX;
	BOOL bHq10Bottom;
	BOOL bDenseZSTAxis;

	UINT nDefPeriod;

	BOOL bHlight_HighLow;

	BOOL bCheckRisk;

	BOOL bKQS_Use10;
	BOOL bKQS_RelationGP;

	BOOL bShowHomePage;

	struct RollInfo_Habit Roll;
};

struct Global_Video
{
	short	nVideoFlag;
	char	strVideoURL[MAX_PATH];
	short	nWndcx;
	short	nWndcy;
};

struct Global_EmbOem
{
	BOOL bEmbedWTVersion;
	BOOL bZszqEmbedWTVersion;	
	BOOL bZxzqEmbedWTVersion;
	BOOL bDgzqEmbedWTVersion;
	BOOL bGxzqEmbedWTVersion;
	BOOL bStdEmbedWTVersion;
	BOOL bYhzqEmbedWTVersion;
	BOOL bGtjaEmbedWTVersion;
	BOOL bShzqEmbedWTVersion;
	BOOL bHyzqEmbedWTVersion;
	BOOL bXyzqEmbedWTVersion;
	BOOL bGdzqEmbedWTVersion;
	BOOL bZxjtEmbedWTVersion;
	BOOL bHlzqEmbedWTVersion;

	BOOL bWebUseInputZjzh;			//在网页中是否采用用户输入的帐号
	BOOL bHasTryout;				//是否有试用行情主站的功能
	BOOL bHasOutStandTryOut;		//是否是突出的试用行情主站的功能
	BOOL bNoHelpBtnInLogo;			//登录对话框上是否有帮助按钮
	BOOL bHasSoftKeyCaption;		//是否有软键盘的提示
	char SysTestCaption[40];		//系统检测按钮的标题
	char Sel1Caption[40];			//行情+交易的按钮的标题
	char Sel2Caption[40];			//独立行情的按钮的标题
	char Sel3Caption[40];			//独立交易的按钮的标题
	BOOL bWritePreNameInLogo;		//是否在标题栏上写标题
	BOOL bWriteVersionInLogo;		//是否在标题栏上写版本号
	BOOL bMediumFontInLogo;			//在对话框上采用较小的字体
	BOOL bCustomQsZXTree;			//是否定制券商自制资讯树
	char CustomQsZxTreeName[40];	//券商自制资讯树的标题
	BOOL bCustomQsZxPosSecond;		//券商自制资讯树的标题在第二个树的位置
	BOOL bStatusInfoLeftAlign;		//登录对话框的状态信息左对齐
	char ResDllName[40];			//资源DLL文件名
	char SpecialCfgFileName[40];	//自定的cfg文件名
	BOOL bNoExpandTitleInQsZXTree;	//在券商自定义树中自动展开所有的标题
	BOOL bOutStandCusWebPageBack;	//是否是突出的个股关联字串的背景
	char SSLAqfsName[40];			//通达信SSL的名称
	char AuthPassAqfsName[40];		//认证口令的名称
	char TdxCAAqfsName[40];			//CA证书的名称
	char SmsAqfsName[40];			//短信口令
	char TokenCardAqfsName[40];		//令牌卡
	char UKeyAqfsName[40];			//EPASS独立登陆
	BOOL bOppositeMngRight;			//是否是相反的权限,最低是5,最高是0
	BOOL bFormatAndUseYYBID;		//是否处理营业部编号
	BOOL bHasRelativeInfo;			//在分时图中是否有相关资讯
	BOOL bRealInfoZstFlush;			//实时资讯在分时图中是否进行闪烁
	char ComboBoxKHWTName[40];		//市场列表中显示的客户号的名称
	char ComboBoxSHHYName[40];		//市场列表中显示的资金帐号的名称
	char DescKHWTName[40];			//描述的客户号的名称
	char DescSHHYName[40];			//描述的资金帐号的名称
	BOOL bGgxxTopInZxTree;			//公告消息是否放在资讯树的上部
	BOOL bMainWebPageInsideOpen;	//主菜单的网页链接内部打开
	BOOL bHasQsZXOnlyLoginJy;		//如果没有登录交易，就没有券商的资讯树
	BOOL bHasMetaCJZX;				//是否支持复合的财经资讯
	BOOL bMetaUseWebProfile;		//Meta信息的显示采用网页格式
	BOOL bHintCannotUseMeta;		//是否隐藏不能使用的META树
	int	 nHintCannotUseMinMetaRight;//隐藏META树中的不能用的最小级别
	BOOL bTopShowMetaRightInfo;		//在顶行显示复合资讯的权限信息
	BOOL bUrgentUseQSIcon;			//播报对话框是否用券商的图标
	
	BOOL bWebButtonInCaption;		//是否在主菜单上还有一个额外的网址连接(需res*.dll支持)
	BOOL bRightWebBtn;				//这个额外按钮是否在右边
	char strWebButtonURL[100];		//这个额外的网址连接
	char strCaptionQSWebURL[100];	//右上角自有券商页面的地址
	char strCaptionHQURL[100];		//右上角行情按钮页面的地址

	char strVsatHttp[100];			//维赛特网页地址(光大等券商使用)
	BOOL bShowPreNameInTopCenter;	//在主菜单的中间显示定制名

	BOOL bNoSwitchInLogo;			//在登录画面不支持切换独立行情，独立交易等
	BOOL bHasTdxTend;				//是否支持通达信的趋势导航信息
	BOOL bWhiteForeColor;			//是否是亮色的前景色
	BOOL bWhiteBtnForeColor;		//是否是亮色的前景色(对于登录界面上的按钮)
	BOOL bHideCloseBt;				//是否隐去登录框上的关闭按钮
	BOOL bUseBtn2Back;				//通讯设置,系统检测和帮助是否采用第二套按钮风格
	char TendName[40];				//趋势导航的名字,如果不为空,则启用趋势导航
	BOOL OnlyJYTend;				//仅仅登录交易才有股票池功能
	
	int	 nL2Cfg_Shift_left;			//L2设置按钮的位置偏移
	int	 nL2Cfg_Shift_right;
	int	 nL2Cfg_Shift_top;
	int	 nL2Cfg_Shift_bottom;

	BOOL bF9WTUseF12Key;			//F9委托项使用F12键
	BOOL bF12SelectWT;				//使用F12时需选择委托
	BOOL bHideJYControl;			//隐藏不能使用的委托控件区 

	int	 nLogoMaxPassordLen;		//登录界面最大的密码长度
	int	 nLogoMaxAuthPassLen;		//登录界面最大的认证口令长度

	BOOL bHasQSRun;					//是否调用QSRun.dll模块
	BOOL bDSMustJy;					//延时扩展主站使用是交易用户才能使用
	BOOL bShowTwoHostName;			//在状态栏上同时显示两个主站名 远程的/本地的
	BOOL bAlwaysSoftKey;			//是否一直有软键盘

	BOOL bOutStandZXGHY;			//对自选股和分类的标签进行突出显示
	BOOL bNoAdvert;					//不显示内部的一些广告
	//以下是TC6特有的
	BOOL bCanTryoutUser;			//是否有注册用户功能
	BOOL bCanActiveUser;			//是否有激活用户功能
	BOOL bCanUpgradeUser;			//是否有用户升级功能
	BOOL bMetaRightFromZXCookie;	//复合资讯的权限来自于zxcookie.dat文件
	BOOL bFirstHideWTFace;			//第一次进入后隐藏交易界面
	BOOL bFirstOpenZXTree;			//第一次进入后强制打开资讯树
};

struct Global_Cfg
{
	BOOL bLocalData;
	BOOL bShowVIPCfg;

	BOOL bHasLevel2Engine;
	char Level2_AutoupId[11];
	char Level2_SepcComte[21];
	BOOL bHasTdxInfo;
	BOOL bHasTdxAdv;
	BOOL bMustTdxLevel2;
	BOOL bAutoupFromLevel1;
	BOOL bHasQzSDS;
	BOOL bZHFromExtern;
	char Extern_LoginName[50];
	char Extern_Password[50];
	
	BOOL bIsInsideVersion;	
	BOOL bIsHKVersion;
	BOOL bIsSHLevel2;
	BOOL bIsSZLevel2;
	BOOL bIsSSDS;
	BOOL bHasKQSFunc;
	BOOL bHasZxTree23;
	BOOL bSupportStatusRoll;
	BOOL bSupportSlowStatusRoll;
	BOOL bNoAutoup;
	BOOL bShowHQNameInStatusBar;
	BOOL bNoUrgent;
	long WhichAutoUpInfo;
	char productnbbs[10];
	
	char version_title[40];
	BOOL nShareMode;
	char strHabitPath[100];
	
	char OtherFileName[40];
	char AlongPathFile[80];
	BOOL bAlongJy;
	BOOL bAlongOpenWT;
	BOOL bAlongOpenHXInfo;
	BOOL bOnlyHK;
	
	char HKey_Path[180];		//注册表项
	char HKey_KeyString[180];	//注册表键值
	char HKey_RunParam[180];	//注册表程序运行时的参数
	char HKey_MenuName[40];		//加入的菜单名称
	char HKey_ErrorInfo[180];	//运行出错时报错信息

	BOOL bShortRefreshHQ;
	BOOL bHasGPGrade;
	BOOL bHasTend;

	struct Global_Video Video;
};

struct Global_Url
{
	char EmailStr[50];;
	char EmailURL[256];;
	char HelpWebStr[50];
	char HelpWebURL[256];
	char ForumURL[256];
	char Corp[50];
	char CorpWeb[256];
	char HelpURL[256];
	char AboutURL[256];
	char WebInfoURL[256];
	char AddZxStr[50];
	char AddZxURL[256];
	char LogoHelpStr[50];
	char LogoHelpURL[256];
};

struct Global_SysInfo
{
	long Program_StartYmd;
	long Program_StartHms;
	long ConnectHQ_LastYmd;
	long ConnectHQ_LastHms;
	long ConnectDSHQ_LastYmd;
	long ConnectDSHQ_LastHms;
	long ConnectZx_LastYmd;
	long ConnectZx_LastHms;
};

struct Global_DSCfg
{
	char	DSOEMName[50];
	BOOL	DSTabFirst;
	BOOL	HasHKMarket;
	BOOL	HasHKYSInfo;
	BOOL	MustInGUI;
};

struct AntiHack_Cfg
{
	BOOL  bJudgeExpireDate;
	BOOL  bUserHostEverBOOL;
	BOOL  bPassorted;
	BOOL  bLogo;
	BOOL  bDSLogo;
	char  cLevel2XORChar_Queue;		//队列
	char  cLevel2XORChar_TickL2;	//逐笔
	char  cLevel2XORChar_ComboHQ;	//旧组合行情
	char  cLevel2XORChar_ComboHQ2;	//新组合行情
};

struct NetParam_Cfg
{
	BOOL  bNoPassPort;
};

//////////////////集成平台////////////////////////////////////

struct tag_VitalBtn
{
	int BtnNum;
	char BtnStr[20][50];
	char BtnType[20];	//0:本地程序 1:资讯系统 2:交易委托 3:基本资料 4:个人理财 ...
	char BtnExecFile[20][180];
	UINT BtnID[20];
};

struct tag_OP_AssisLevel
{
	char Text[80];
	short GnNO;
	short SonNum;
};

struct tag_OP_SubAssisLevel
{
	char Text[80];
	short GnNO;
};

//与WEB有关的结构

typedef	struct	tagIEInputData
{
	char	FieldName[64];
	char	SetValue[64];
	BYTE	InputType;
}IEINPUTDATA;

typedef struct tag_ChatArea
{
	int		ChatAreaNum;		//交流区的数目,不能超过20个
	char	ChatAreaStr[20][50];//交流区标题
	char	AreaType[20];		//交流区类型
	char    SwitchRefresh[20];	//切换时是否刷新,有些网页切换时必刷新
	char	ChatURL[20][200];	//交流区的URL(如果交流区是网页的话,否则为空)
	int		URLDataNum[20];		//URL的登录参数，不能超过10个
	IEINPUTDATA URLData[20][10];//URL的登录参数
}CHATAREA;

typedef struct
{
	short infotree_num;
	short infotreeid[100];
	char  infotreestr[100][50];
	char  infotreeurl[100][200];
	short infoopenie[100];
	char  infoautodown[100][50];
	short infotreeqx[100];
}INFOTREEID;

typedef struct
{
	char	description[80];
	short	m_nSelHY;
	short	m_nSelDY;
	char	m_setcode;
	char	m_code[SH_CODE_LEN+1];
	short	m_zxcompanynum;
	short	m_zxcompany[30];
	char	m_search_type;
	long	m_time_ymd[2];
	short	m_hy_relation;
	short	m_dy_relation;
	char	m_author[30];
	char	m_keywordstr[100];
}RECENTSEARCH;	//必须与ViewThem.DLL中保持一致

//////////////////////////////////////adde by iwld 2003.4.23自定指数
typedef struct
{
	char Code[SH_CODE_LEN+1];
	char Name[NAME_LEN+1];
	NTime StartTime;		//指数起始时间
	float fBaseValue;		//基指
	float fBaseScale;		//和基指的比价
	int nStart;				//个股几天后记入
	UINT CalcProp;			//个股权重计算方式
	BOOL bRealTime;			//是否实时刷新
	int nGp;				//个股数
}SelfZSHd;

typedef struct
{
	int nIndex;			//股票索引
	float fBasePrice;	//起始价
	float fPropValue;	//所占权值,未上市时或停牌时为0
}SelfZSUnit;

typedef	struct 
{
	char BigsetName[32];
	long MarketSet[MAX_MKT_NUM];
	char MarketName[MAX_MKT_NUM][MRK_NAME_SIZE];
}GUIMARKET;

enum
{
	HQ_LINE=0,			//	分割线
	HQ_BJ_FLOAT,		//	涨跌颜色显示float
	HQ_PLUS_FLOAT,		//	同0比较的float
	HQ_PLUS_FLOAT_PER,	//	同0比较的float(百分比)
	HQ_VOL_LONG,		//	显示成交量
	HQ_RED_LONG,		//	显示红色
	HQ_GREEN_LONG,		//	显示绿色
	HQ_VOL_PLONG,		//  显示成交量(精确显示整数)
	HQ_FLOAT,			//	普通float
	HQ_FLOAT_PER,		//	百分比显示float
	HQ_LONG,			//	普通long
	HQ_STRING,			//	字符串
	HQ_PRICE_SEAT,		//  价位+席位数
	HQ_AMOUNT_FLOAT,	//	显示金额
	HQ_NULL
};

typedef	struct	tagHqItem
{
	char	lpScript[13];
	BYTE	Type;
	BYTE	Pos;
	long	lData;
	float	fData;
	LPSTR	lpString;
}HQITEM,*PHQITEM;

typedef	struct tagCusWebPage
{
	char	PageUrl[MAX_PATH];
	char	PageName[64];
}CUSWEBPAGE, *PCUSWEBPAGE;

enum
{
	ABDEF_ITEM,
	DSOPT_ITEM,
	SELF_ITEM
};

typedef	struct tagGridCatItem
{
	BYTE	FlagType;			//
	char	lpTitle[13];
	UINT	msg;
	WPARAM	wParam;
	LPARAM	lParam;
	BOOL	bMenu;
	long	lData;
	DWORD	Reserved;
}GRIDCATITEM, *PGRIDCATITEM;

typedef	struct
{
	UINT		ItemID;
	char		ItemString[50];
	COLORREF	Color;
}SPECMENUCOLOR;

struct ETF_Info
{
	short setcode;
	char  Code[SH_CODE_LEN+1];
	char  Desc[20];
	char  IOPVCode[SH_CODE_LEN+1];
	char  IndexCode[SH_CODE_LEN+1];
};

struct Status_GP
{
	char	lpScript[20];
	BYTE	setcode;
	char    Code[SH_CODE_LEN+1];
};

struct News_Title
{
	short   info_type;	//信息类型
	long    rec_id;		//记录的唯一ID
	long	time_ymd;	//发表日期，年月日
	long	time_hms;	//发表时间，时分秒 
	char	title[64];	//标题
	char	infosrc[20];//信息来源
	short	infoformat;	//信息格式
	char	author[30];	//作者
};

struct MineTitleCache
{
	long  RefreshHms;
	short TitleNum;
	struct ext_info_title *pList;
};

typedef struct			//主图历史数据模式
{
	char KeyString[10];
	char InfoString[20];
}MAINLINE;

struct VipVerify_Info
{
	char RemoteVerify;
	char ReVerifySpan;	//需要重新验证的周期,缺省为80天
	char DenyInfo;		//不验证是不是不能登录资讯
};

struct FY_GROUP_INFO
{
	CString			strGroupName;
	int				groupAttribute;	//group特性种类
	int				belong;			//父group的序号
	vector<UINT>	subGroup;		//子group的序号
	vector<CString> strItemName;	
	vector<CString> strShowTitle;	
	vector<int>		itemAttribute;	//指定子板块特性种类,以确定标签字体颜色等等
	UINT nCurSel;					//该组处于激活状态的item
	UINT nIndex;					//在switchbar中的序号(以1为基数)
	UINT nIndex_file;				//对应的文件编号(以1为基数)
	BOOL bEncrypted;				//是否加密
	BOOL bActive;					//实体group处于打开状态
	int  nType;						//类型:0表示列表 1表示文本
	FY_GROUP_INFO()
	{
		nIndex=-1;
		groupAttribute=0;
		belong=-1;
		nCurSel=0;
		bEncrypted=FALSE;
		nIndex_file=0;
		bActive=FALSE;
		nType=0;
	}
};

struct _FY_GROUP_INFO
{
	CString strGroupName;
	CString	parentName;
	int nIndex;		//对应vector<FY_GROUP_INFO>中的序号
	int nSubIndex;	//若是二级group,则对应FY_GROUP_INFO的subGroup的序号
};

struct FY_LOCATION
{
	int		nGroup;
	int		nItem;
	short	nRow;
};

typedef struct ADVERTBUTTON
{
	char btTxt[21];
	char url[200];
	COLORREF txtCor;
	UINT advdlg_cx;
	UINT advdlg_cy;
	CRect rect;
}ADVERTBUTTON,*PADVERTBUTTON;

struct ADVERT_INFO
{
	char F10AdvertStr1[256];
	char F10AdvertStr2[256];
	short nAdvBt;
	ADVERTBUTTON AdvButton[5];
};

struct NewJy_CFG
{
	char	CaptionWeb1[200];
	char	RelativeInfo[200];
	BOOL	bHasQSRelativeInfo;
	char	QSRelativeName[20];
	char	QSRelativeInfo[200];

	char	AddedWT_Name[20];
	char	AddedWT1_Name[20];
	char	AddedWT1_Path[200];
	char	AddedWT2_Name[20];
	char	AddedWT2_Path[200];

	BOOL	bAlwaysStatusRoll;

	char	ZXHomePage[200];
};

struct LinkedSetInfo
{
	BOOL	bUsed;
	
	int		WhichGpMarket;
	short	ColType;
	int		nStartRow;
	int		nTolRow;
	int		nValidRow;
	int		nVisualRow;
	int		SortType;
	CWnd	*pMsgRtnWnd;
	short	lpnUsedStkIndex[MAXGPNUM];
};

//////////////////////////////////////////////////////////////////////////
//预警相关结构

struct WarnHQCache
{
	short			nLastCalcMin[MAX_TJWARN];
	CurrStockData	iHq;
};

//叠加的信息
struct OverlapInfo
{
	char		nAutoType;			//自动叠加类型,为0表示不是自动叠加
	short		nAxis;				//叠加坐标
	short		nNum;				//手动叠加个数
	short		StkSetCode[3];
	char		StkCode[3][SH_CODE_LEN+1];
	char		bOverlapHisIndex;
	MemStkInfo	*pNowStk;			//当前浏览股的指针
	MemStkInfo	*pOverlapStk[3];	//手动叠加股的指针
	DWORD		unused[100];
};

//汉字输入键盘精灵
struct KeyGuyCNInfo 
{
	CWnd	*pOwner;
	long	nKeyGuyCNing;
	char	strKeyGuyCN[20];
};

//////////////////////////////////////////////////////////////////////////
//定制版面及其之类
struct cmpstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

class UBase;
struct UnitNodeInfo
{
	int NodeID;				//节点ID,左节点＝父ID*2,右节点＝父ID*2+1,根节点＝0
	int splitType;			//分割类型:-1横切 1竖切 0完整单元
	float toSplit;			//分割百分比,和兄弟节点共100
	CRect rect;				//分割后的区域
	int unitType;			//Unit类型,有子节点则为-1
	BOOL bContrallUnit;		//是否主控Unit
	UBase*	pUnit;			//Unit的指针,有子节点则为NULL
	short	style;			//此种Unit的风格,有子节点则为-1
	short	var_GpIndex;	//Unit包含的股票,有子节点则为-1, 作为分组信息
	BOOL	bLocked;		//是否被锁定
	long	nJHMin[2];		//是否几何索定，0--width 1--height
};

struct UnitTreeNode
{
	UnitNodeInfo* pCurUnit;
	UnitTreeNode* pParent;
	UnitTreeNode* pLeftChild;
	UnitTreeNode* pRightChild;
};

struct SplitManInfo
{
	int splitType;
	CRect cursorRt;
	COLORREF borderColor;
	UnitTreeNode *node;
};

struct SplitActInfo
{
	int nDepUnit;
	int nSplitType;
	float fToSplit;
	UnitTreeNode* pNode;
};


//港股数据库行业分类
struct	FL_CodeName 
{
	char	SectorCode[21];
	char	SectorName[21];
};

//基金基本信息
typedef struct FUNDBASE
{
	char szSymbol[7];	//基金代码
	char szName[21];	//基金简称
	char chType;		//基金类型，0：开放式，1：封闭式,2:开放式货币型基金
	
	char szCompanyAName[31]; //基金管理人名称
	char szInvestStyle[21];	 //基金投资风格
	char szInvestStylePro[21];//基金投资风格属性
	
	long  lNetDate;		//净值日期
	float fNet;			//基金最新净值
	float fLastNet;		//基金上期净值
	float fCumNet;		//基金最新累计净值
	float fFundShare;	//基金季报份额
	float fTotalNet;	//基金季报净值总额
	float fLastTotalNet;//上一期季报基金净值总额
	float fStockTotal;	//最新季报股票市值
	float fStockRate;	//股票投资占净值比例
}FUNDBASE,*pFUNDBASE;

//基金持股信息
struct MYSTOCKINFO
{
	char szSymbol[7];	//股票代码
	float fAcount;		//本期数量
	float fMarketValue;	//本期市值
	float fLastAcount;	//上期市值
};

typedef struct FUNDHOLDSTOCK
{
	char szSymbol[7];	//基金代码
	short nNum;			//股票数目
	MYSTOCKINFO stockinfo[10];//股票结构数组
}FUNDHOLDSTOCK,*pFUNDHOLDSTOCK;

struct Seat_Code2Com 
{
	short	setCode;
	long	ComID;
};

struct Seat_Code2Com_Ex
{
	long	SeatID;
	short	setCode;
	long	ComID;
};

struct Seat_Com2Name 
{
	char	ComNameS[21];
	char	ComNameL[61];
};

struct Seat_Com2Name_Ex
{
	long	ComID;
	char	ComNameS[21];
	char	ComNameL[61];
};

struct ShortCut_Cfg
{
	char	setcode;
	char	RawCode[SH_CODE_LEN+1];
};

///////////////////////////////////////////////////////////////////////////
//插件
#include "CMLDPlugInInterface.h"
struct	ClintPluginInfo
{
	ICMLDPlugIn_Client		*g_pClientInterface;
	CNewMenu				*g_pCmldAppMenu;
	CString					g_MeneItemString;
	
	ClintPluginInfo()
	{
		g_pClientInterface = NULL;
		g_pCmldAppMenu = NULL;
		g_MeneItemString = _T("");
	}
};

//////////////////////////////////////////////////////////////////////////
//界面方案
struct Scheme_King
{
	char	QSID[11];			//不同开发客户
	char	VersionString[21];	//版本串
	int		nVersion;			//版本编号

	int		Scheme_Hold_No;		//配色方案的序号

	UINT	LoginDB_ID;
	UINT	MainMenuID;
	UINT	ToolBarID;
	UINT	ToolBarID_Hot;
	UINT	ToolBarID_Disable;
	char	PopFileUrl[MAX_PATH];
	short	nLup[32];

	BOOL	bShowToolBar;
	CString	HomePage;
	int		nVertiOff;
	int		nParaOff;
	CString	StartDownFile;
	CString	YxhjPath;
	BOOL	bStatusSwitch;

	Scheme_King()
	{
		memset(nLup, 0, sizeof(nLup));
	}
};
/////////////////////////////////////////////////////////////////////////////
#endif

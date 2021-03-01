#ifndef DDE_H_
#define DDE_H_

//品种的大类型
#define TOTAL_TYPE			0	//综合类型
#define ABGP_TYPE			1	//大陆AB股
#define HKGP_TYPE			2	//香港股票
#define QH_TYPE				3	//期货品种
#define FE_TYPE				4	//外汇品种
#define IDX_TYPE			5	//国际指数
#define FOREGP_TYPE			6	//国外股票
#define FY_TYPE				8	//风云排行
#define DDE_TYPE			9	//DDE排行
#define A2H_TYPE			10	//A+H排行
#define	JSZJ_TYPE			11	//资金
#define SIGNAL_TYPE			12	//画牛熊信号
#define LAST_TYPE			15	//作为保留

//栏目值
#define ABGP_COL_NUM	67		//所有行情栏目个数
#define ABGPHQ_COL_NUM	30		//纯行情栏目个数

#define HKGP_COL_NUM	24		//港股行情栏目个数
#define QH_COL_NUM		25		//期货行情栏目个数
#define FE_COL_NUM		12		//外汇行情栏目个数
#define FOREGP_COL_NUM	21		//外股行情栏目个数
#define IDX_COL_NUM		12		//指数行情栏目个数
#define DDE_COL_NUM		17		//DDE栏目个数
#define A2H_COL_NUM		12		//A+H栏目个数
#define GLHQ_COL_NUM	18		//关联行情的栏目
#define	JSZJ_COL_NUM	10
#define	SIGNAL_COL_NUM	32		//画个信号

#define TOTAL_COL		140		//所有系统支持的栏目数

//股票栏目代码
const short ZQDM	= 0;		//代码
const short ZQJC	= 1;		//证券名
const short ZRSP	= 2;		//昨收
const short JRKP	= 3;		//今开
const short ZGCJ	= 4;		//最高
const short ZDCJ	= 5;		//最低
const short ZJCJ	= 6;		//现价
const short ZGJM	= 7;		//叫买价
const short ZDJM	= 8;		//叫卖价
const short CJL		= 9;		//总手
const short CJJE	= 10;		//总金额
const short XS		= 11;		//现手
const short QRSD	= 12;		//日涨跌
//const short QBSD	= 13;		//笔涨跌
const short HY_STR	= 13;		//所属行业
const short ZAF		= 14;		//涨幅
const short ZEF		= 15;		//振幅
const short JUNJ	= 16;		//均价
const short SYL		= 17;		//市盈率
const short WTB		= 18;		//委比
const short LP		= 19;		//内盘
const short WP		= 20;		//外盘
const short LWB		= 21;		//内外比
const short WLC		= 22;		//委量差
const short BJL1	= 23;		//买手一
const short SJL1	= 24;		//卖手一
const short BJ1		= 25;		//买价一
const short SJ1		= 26;		//卖价一
const short BJ2		= 27;		//买价二
const short BJL2	= 28;		//买手二
const short SJ2		= 29;		//卖价二
const short SJL2	= 30;		//卖手二
const short BJ3		= 31;		//买价三
const short BJL3	= 32;		//买手三
const short SJ3		= 33;		//卖价三
const short SJL3	= 34;		//卖手三
const short LIANGB	= 35;		//量比
const short J_HSL	= 36;		//换手率
const short J_LTGB	= 37;		//流通股本
const short J_LTSZ	= 38;		//流通市值
const short J_ZSZ	= 39;		//总市值

const short DKPH	= 40;		//多空平衡
const short DTHL	= 41;		//多头获利
const short DTZS	= 42;		//多头止损
const short KTHB	= 43;		//空头回补
const short KTZS	= 44;		//空头止损
const short QRD		= 45;		//强弱度
const short ZANGSU  = 46;		//涨速
const short HYD		= 47;		//活路度
const short MBZL	= 48;		//每笔均量
const short MBHSL	= 49;		//每笔换手

const short J_GXRQ	= 50;		//更新日期
const short J_START = 51;		//上市日期
const short J_ZGB	= 52;		//总股本
const short J_GJG   = 53;		//国家股
const short J_OLDJLY= 54;		//上年此期净利润
const short J_OLDZYSY=55;		//上年此期主营收入
const short J_BG	= 56;		//B股
const short J_HG	= 57;		//H股
const short J_ZGG	= 58;		//季报每股收益 (财报中提供的每股收益,有争议的才填)
const short J_ZZC	= 59;		//总资产(万元)
const short J_LDZC	= 60;		//流动资产
const short J_GDZC	= 61;		//固定资产
const short J_WXZC	= 62;		//无形资产
const short J_CQTZ	= 63;		//长期投资
const short J_LDFZ	= 64;		//流动负债
const short J_CQFZ	= 65;		//长期负债
const short J_ZBGJJ	= 66;		//资本公积金
const short J_JZC	= 67;		//股东权益(就是净资产)
const short J_ZYSY	= 68;		//主营收入
const short J_ZYLY	= 69;		//主营利润
const short SJL		= 70;		//市盈率
const short J_YYLY	= 71;		//营业利润
const short J_TZSY	= 72;		//投资收益
const short J_JYXJL	= 73;		//经营现金净流量
const short J_ZXJL	= 74;		//总现金净流量
const short J_CH	= 75;		//存货
const short J_LYZE	= 76;		//利益总额
const short J_SHLY	= 77;		//税后利益
const short J_JLY	= 78;		//净利益
const short J_WFPLY	= 79;		//未分配利益
const short J_TZMGJZ= 80;		//调整每股净资产
const short J_JYL	= 81;		//净资产收益率
const short J_MGWFP	= 82;		//每股未分配
const short J_MGSY	= 83;		//每股收益
const short J_MGGJJ	= 84;		//每股公积金
const short J_MGJZC	= 85;		//每股净资产
const short J_GDQYB	= 86;		//股东权益比

const short ZBCOL   = 87; 		//指标排序栏目

const short	SPELL_CODE	= 88;	//外汇简称
const short QH_JSJ		= 89;	//期货结算价
const short QH_YJSJ		= 90;	//期货前结算价

const short SPREAD		= 91;	//买卖差价
const short BSUNIT		= 92;	//买卖单位
const short CURRENCYNAME= 93;	//货币单位
const short AVERPRICE	= 94;	//平均价
const short YIELDVAL	= 95;	//收益率
const short HIS_HIGH	= 96;	//年最高
const short HIS_LOW		= 97;	//年最低
const short IEP			= 98;	//参考价
const short IEV			= 99;	//参考量
const short MRKCAP		= 100;	//市值
const short PE			= 101;	//市盈率
const short PREMINUM	= 102;	//溢价%
const short GEARING		= 103;	//贡杆比率%
const short EXECPRICE	= 104;	//行使价
const short CONVRATIO	= 105;	//换购比率
const short EXPIREDATE	= 106;	//到期日
const short TODAYZC		= 107;	//持仓差
const short TODAYKC		= 108;	//今日开仓
const short TODAYPC		= 109;	//今日平仓

const short DDX_1		= 110;	//当日DDX
const short DDY_1		= 111;	//当日DDY
const short DDZ_1		= 112;	//当日DDZ
const short DDX_60		= 113;	//60日DDX
const short DDY_60		= 114;	//60日DDY
const short DDX_REDNUM	= 115;	//DDX连红天数
const short DDX_REDIN10	= 116;	//10内DDX红

const short BIGIOAMOUNT	= 117;	//主力资金流向
const short BIGIOWEIGHT = 118;	//主力活跃度

const short BIGBUY_RAT	= 119;	//大单买入%
const short BIGSELL_RAT	= 120;	//大单卖出%
const short TOTALWTB	= 121;	//总买委比
const short TICK2_JL	= 122;	//逐笔均量

const short H_ZQDM		= 123;	//H股代码
const short H_ZQJC		= 124;	//H股名称
const short H_ZJCJ		= 125;	//H股现价
const short H_ZAF		= 126;	//H股涨幅
const short H_CJL		= 127;	//H股总量
const short H_GB		= 128;	//H股股本
const short H_HSL		= 129;	//H股换手
const short H_YJ		= 130;	//溢价
const short H_YJL		= 131;	//溢价率

const short JS_INDD		= 132;	//大单流入
const short JS_OUTDD	= 133;	//大单流出
const short JS_INZD		= 134;	//中单流入
const short JS_OUTZD	= 135;	//中单流出
const short JS_INXD		= 136;	//小单流入
const short JS_OUTXD	= 137;	//小单流出

const short ON_DXTX		= 138;	//短线提醒
const short ON_ZXTX		= 139;	//中线提醒

/////////////////////////////////////////////////////

extern char		g_strBigMarket[16][11];

extern char		g_lpszAllItemName[TOTAL_COL][18];
extern float *	g_aAllItemWidth;
extern float	g_aAllItemWidth_Def[TOTAL_COL];

extern long		g_aABGPIdx[ABGP_COL_NUM];
extern long		g_aABGPIdx_Def[ABGP_COL_NUM];

extern long		g_aHKGPIdx[HKGP_COL_NUM];
extern long		g_aHKGPIdx_Def[HKGP_COL_NUM];

extern long		g_aFOREGPIdx[FOREGP_COL_NUM];
extern long		g_aFOREGPIdx_Def[FOREGP_COL_NUM];

extern long		g_aFEIdx[FE_COL_NUM];
extern long		g_aFEIdx_Def[FE_COL_NUM];

extern long		g_aQHIdx[QH_COL_NUM];
extern long		g_aQHIdx_Def[QH_COL_NUM];

extern long		g_aIDXIdx[IDX_COL_NUM];
extern long		g_aIDXIdx_Def[IDX_COL_NUM];

extern long		g_aDDEIdx[DDE_COL_NUM];
extern long		g_aDDEIdx_Def[DDE_COL_NUM];

extern long		g_aA2HIdx[A2H_COL_NUM];
extern long		g_aA2HIdx_Def[A2H_COL_NUM];

extern long		g_aJSZJIdx[JSZJ_COL_NUM];
extern long		g_aJSZJIdx_Def[JSZJ_COL_NUM];

extern long		g_aSignalIdx[SIGNAL_COL_NUM];
extern long		g_aSignalIdx_Def[SIGNAL_COL_NUM];

extern long		g_aGlHqIdx_Def[GLHQ_COL_NUM];

#endif

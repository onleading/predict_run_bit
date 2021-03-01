#include "stdafx.h"
#include "dde.h"

//大类型的描述
char  g_strBigMarket[16][11]={"综合","国内","香港","期货","外汇","指数","外股","定制","风云","DDE","","","","","","保留"};

//所有栏目的名称
char  g_lpszAllItemName[TOTAL_COL][18] =
{
	 "代码   ", "名称", "前收", "今开", "最高",
	 "最低","最新", "委买价", "委卖价", "总手", 
	 "总额", "现手","涨跌", "行业名称", "涨幅",
	 "振幅", "均价", "市盈率","委比", "内盘", 
	 "外盘", "内外比", "委差","委买量", "委卖量", 
	 "买价一", "卖价一","买价二", "买量二", "卖价二",
	 "卖量二","买价三", "买量三", "卖价三", "卖量三",
	 "量比",  "换手率","流通股本","流通市值", "AB股总市值",

	 "多空平衡","多头获利","多头止损","空头回补","空头止损","强弱度%%","涨速","活跃度","每笔均量","每笔换手",

	 "更新日期","上市日期","总股本(万)","国家股","利润同比","收入同比",
	 "B股(或A)","H股","职工股","总资产(万)","流动资产",
	 "固定资产","无形资产","长期投资","流动负债","长期负债",
	 "公积金","净资产","主营收入","主营利润","市净率",
	 "营业利润","投资收益","经营现金流","总现金流","存货",
	 "利润总额","税后利润","净利润","未分利润","调整后净资",
 
	 "净益率%%","每股未分配","每股收益","每股公积","每股净资","权益比%%",

	 ""	,
	 "外汇简称",
	 _Settle,_Prev_Settle,_Spread,_Lot_Size,_Currency,
	 _Aver_Price,_Yield,_Year_High,_Year_Low,"参考价",
	 "参考量",_MrkCap,_PE_Ratio,_PreMinum,_Gearing,
	 _ExecPrice,_ConvRatio,_Expire_Date,_Contract_Diff,_OpenContract,_CloseContract,
	 "当日DDX","当日DDY","当日DDZ","60日DDX","60日DDY","DDX连红","10日红DDX","SUP净流向","主力活跃%%","大单买入%%","大单卖出%%","总买委比","逐笔均量",
	 "H股代码","H股名称","H现价","H涨幅%%","H总量","H股本","H换手%%","溢价(汇)","溢价率%%",
	 "大单流入","大单流出","中单流入","中单流出","小单流入","小单流出",
	 "短线提醒","中线提醒",
};

//所有栏目的宽度
float *g_aAllItemWidth=NULL;
float g_aAllItemWidth_Def[TOTAL_COL]={6,8,8,8,8,8,8,7,7,7,
						9,6,8,15,7,7,8,8,7,7,
						9,7,7,7,7,8,8,7,7,7,
						7,7,7,7,7,6,7,11,10,11,

						9,9,9,9,9,9,7,7,9,9,

						9,9,11,10,9,9,9,10,9,12,12,
						12,12,12,12,12,12,12,12,12,9,
						11,11,11,11,11,10,10,10,10,11,

						8,11,9,9,9,8,
						12,
						8,
						8,9,8,8,8,
						8,8,8,8,8,
						9,9,8,9,9,
						8,9,11,8,8,8,
						9,9,9,9,9,9,10,11,10,10,10,9,9,
						8,17,9,9,9,8,9,8,9,
						9,9,9,9,9,9,
						9,9};

//AB股栏目
long g_aABGPIdx[ABGP_COL_NUM];

long g_aABGPIdx_Def[ABGP_COL_NUM]={ZQDM,ZQJC,ZAF,LIANGB,J_HSL,
					ZRSP,JRKP,ZGCJ,ZDCJ,ZJCJ,
					CJL,CJJE,XS,ZANGSU,QRSD,
					ZEF,JUNJ,SYL,J_LTGB,J_LTSZ,HY_STR,
					WP,LP,LWB,ZGJM,BJL1,
					ZDJM,SJL1,WTB,WLC,

					J_GXRQ,J_START,J_ZGB,J_GJG,J_OLDJLY,J_OLDZYSY,
					J_BG,J_HG,J_ZGG,J_ZZC,J_LDZC,
					J_GDZC,J_WXZC,J_CQTZ,J_LDFZ,J_CQFZ,
					J_ZBGJJ,J_MGGJJ,J_JZC,J_ZYSY,J_ZYLY,SJL,
					J_YYLY,J_TZSY,J_JYXJL,J_ZXJL,J_CH,
					J_LYZE,J_SHLY,J_JLY,J_WFPLY,J_MGJZC,J_TZMGJZ,
					J_JYL,
					J_MGWFP,J_MGSY,J_GDQYB};


//港股栏目
long  g_aHKGPIdx[HKGP_COL_NUM];

long  g_aHKGPIdx_Def[HKGP_COL_NUM]={ZQDM,ZQJC,ZAF,ZANGSU,ZJCJ,BJ1,
					SJ1,XS,LIANGB,CJL ,JRKP,ZGCJ,
					ZDCJ,CJJE,ZEF,QRSD,ZRSP,
					BSUNIT,HIS_HIGH,HIS_LOW,
					IEP,IEV,MRKCAP,PE};

//外股栏目
long  g_aFOREGPIdx[FOREGP_COL_NUM];

long  g_aFOREGPIdx_Def[FOREGP_COL_NUM]={ZQDM,ZQJC,ZAF,ZJCJ,BJ1,
					SJ1,XS ,CJL ,JRKP,ZGCJ,
					ZDCJ,CJJE,ZEF,QRSD,ZRSP,
					JUNJ,WTB,LP,WP,
					LWB ,WLC};

//外汇栏目
long  g_aFEIdx[FE_COL_NUM];

long  g_aFEIdx_Def[FE_COL_NUM]={ZQDM,ZQJC,ZJCJ,
					QRSD,ZAF,ZEF,ZGCJ,
					ZDCJ,JRKP,ZRSP,BJ1,SJ1};

//期货栏目
long  g_aQHIdx[QH_COL_NUM];

long  g_aQHIdx_Def[QH_COL_NUM]={ZQDM,ZQJC,ZAF,ZANGSU,ZJCJ,BJ1,SJ1,XS,LIANGB,WTB,BJL1,SJL1,
					CJL,QRSD,SYL,TODAYZC,QH_JSJ,JRKP,ZGCJ,ZDCJ,QH_YJSJ,
					TODAYKC,TODAYPC,LP,WP};

//指数栏目
long  g_aIDXIdx[IDX_COL_NUM];

long  g_aIDXIdx_Def[IDX_COL_NUM]={ZQDM,ZQJC,ZAF,ZJCJ,ZANGSU,
					QRSD,ZEF,ZGCJ,CJJE,
					ZDCJ,JRKP,ZRSP};

//DDE栏目
long  g_aDDEIdx[DDE_COL_NUM];
long  g_aDDEIdx_Def[DDE_COL_NUM]={ZQDM,ZQJC,ZJCJ,ZAF,
								DDX_1,DDY_1,DDZ_1,DDX_60,DDY_60,DDX_REDNUM,DDX_REDIN10,BIGIOAMOUNT,BIGIOWEIGHT,
								BIGBUY_RAT,BIGSELL_RAT,TOTALWTB,TICK2_JL};

//A2H栏目
long  g_aA2HIdx[A2H_COL_NUM];
long  g_aA2HIdx_Def[A2H_COL_NUM]={ZQDM, ZQJC, ZJCJ, ZAF, J_HSL, 
								H_ZQDM, H_ZQJC, H_ZJCJ, H_ZAF, H_HSL, 
								H_YJ, H_YJL};

//关联行情的栏目
long g_aGlHqIdx_Def[GLHQ_COL_NUM]={ZQJC,ZAF,ZJCJ,ZANGSU,LIANGB,
								QRSD,ZGJM,ZDJM,CJL,J_HSL,
								XS,JRKP,ZRSP,ZGCJ,ZDCJ,
								CJJE,ZEF,J_LTSZ};

//ZJ栏目
long  g_aJSZJIdx[JSZJ_COL_NUM];
long  g_aJSZJIdx_Def[JSZJ_COL_NUM]={ZQDM,ZQJC,ZJCJ,ZAF,JS_INDD,JS_OUTDD,JS_INZD,JS_OUTZD,JS_INXD,JS_OUTXD};

//信号栏目
long g_aSignalIdx[SIGNAL_COL_NUM];

long g_aSignalIdx_Def[SIGNAL_COL_NUM]={ZQDM,ZQJC,ZAF,ON_DXTX,ON_ZXTX,LIANGB,J_HSL,
					ZRSP,JRKP,ZGCJ,ZDCJ,ZJCJ,
					CJL,CJJE,XS,ZANGSU,QRSD,
					ZEF,JUNJ,SYL,J_LTGB,J_LTSZ,HY_STR,
					WP,LP,LWB,ZGJM,BJL1,
					ZDJM,SJL1,WTB,WLC};

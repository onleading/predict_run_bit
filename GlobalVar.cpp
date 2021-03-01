#include "stdafx.h"
#include "ViewManager.h"
#include "Znxg.h"
#include "interface.h"
#include "custompad.h"

HINSTANCE g_resModule=NULL;

//!!!
char	g_szReleaseVersion[10]="7.3";
char	g_szMainExeVersion[10]="7.3";
char	g_szDSExeVersion[10]="1.02";

UINT	UM_MINEMSG	= RegisterWindowMessage("SendMineMessage");
UINT	UM_GGBWMSG	= RegisterWindowMessage("ReReadGGBWMessage");
UINT	UM_FLZLMSG  = RegisterWindowMessage("ReqFLZLMessage");
UINT    UM_HQ_MINE_MSG	= RegisterWindowMessage("HqMineInfoMesssage");
UINT	UM_CJZXMSG	= RegisterWindowMessage("SendCjzxMessage");

const TCHAR szMainFrameClass[] = _T("TopView_MainFrame_Class");

class CHQComm	*g_pComm = NULL,*g_pComm3 = NULL;
class CGetData	*g_pGetData = NULL;

int		RECENTNUM	= 140;
int		g_RMSp		= 0;

int		g_nLinkage	= 0;			//0:不联动 1:组内联动 2:组间联动

BOOL	g_DDEDataNeedLoad = TRUE;	//是否需要重新载入Exday(主站连接成功,下载数据后需要重新载入)

BOOL	OnLine = FALSE;				//普通行情Internet在线标志
BOOL	DSOnLine = FALSE;
BOOL	OffLineMode = FALSE;	//是否是脱机状态

BOOL  g_bAllWarnSwitch = FALSE;	//预警开关
BOOL  g_bTjWarnSwitch = FALSE;	//条件预警开关
BOOL  g_bNotifyDownload = TRUE;	//退出系统后是否提示数据下载
BOOL  g_bDay_WriteDisk = FALSE;	//日线自动写盘

BOOL  g_bDrawBlue = TRUE;

//!!!
long	g_nDefUserLevel = 1;
CString g_strPreName = "";//产品名称前缀
CString g_strTitleName = "";//产品名称
CString g_strAdName = "";
CString g_strMoving = "";

//根据版本和应用情况，选用适当的值
BOOL  g_bInsideOpenPage = FALSE;//在程序内部界面打开链接
BOOL  g_bExtern_Info = FALSE;	//是不是外挂资讯
BOOL  g_bExtern_Invest = FALSE;	//是不是外挂理财系统
short g_nSwitchMainID = 0;		//用于Unit得到ComView或HisDlg的主ID

char  g_strMovingZXG[MAXGPNUM][SH_CODE_LEN+10];
short g_nMovingZXG = 0;

BOOL  LayoutFlag = FALSE;
int	  g_nMinnNum = 10,g_nDaynNum = 45;
NTime g_NoTime;

struct AnalyData g_SyncAnalyData;
float	g_SyncClose = 0.0;

int	  g_nGzPerMin = 120;

int	  g_nWhichHisSwitch = 0;
BOOL  g_bVolTQ = FALSE;
BYTE   TMEANLESS_DATA[8]={0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8};

DTopViewCalc	* g_pCalc = NULL, * g_pExtCalc = NULL;

UINT  g_WTMsgID,g_WTMsgID2;
UINT  g_uRefreshAllHQ;
UINT  g_uCloseMsg;
BOOL  g_bSpecClose = FALSE;	//是不是由外部程序引起的
BOOL  g_bDirectQuit = TRUE;
BOOL  g_bReLoginStatus = FALSE;

long   g_lOpenRq = -1;

BOOL   g_bDonotClose = FALSE;
CZnxg *g_pZnxg = NULL;
CLESIGN g_pXGSign;

//用于热门板块分析的ESC逻辑
BOOL	g_bSepEsc = FALSE;

BOOL	g_bOldGridDomain=TRUE;
//涨速周期
int		 g_nZFFzNum = 5;//全景排行涨速周期

//
//本机网卡地址
CString  g_NetCardAddress = "";
//用于板块股
BlockInfo_Struct g_BlockInfo[USERBLOCK_NUM]={0};
int		g_nBlockNum = 0;
//用于概念板块股
MEM_BLOCK g_ConceptBlock[100];
int		  g_ConceptBlockNum = 0;
//用于组合板块股
ZHB_BlockInfo_Struct *g_ZHBBlock = NULL;
int					 g_ZHBBlockNum = 0;

//板块绑定颜色
char		g_ColorBlockGp[8][MAXGPNUM][SH_CODE_LEN+10];
int			g_ColorBlockGpNum[8];
BLOCKCOLOR	g_BlockColor[8];

KeyWordSeach* g_pKeyWordSearch=NULL;

short	GPFZNUM;
short	QHFZNUM;
short	fztb[4]={1,3,6,12};

int		ScreenCx=800,ScreenCy=600;

char *DpGpCode[19]={"000010",SH_DPCODE/*上证领先*/,SH_ADPCODE,SH_BDPCODE,SH_DPCODE/*ADL*/,SH_DPCODE/*DK*/,"399004",
					SZ_DPCODE/*深圳领先*/,"399106","399107","399108","399106","399106","000016","000011",CYB_DPCODE,"399101","000015",HS300_SZZS};

class StockDataIo	*g_pStockIO	= NULL;
class LcIdxMng		*g_pLcIdxMng= NULL;
class ExdayMng		*g_pExdayMng= NULL;
class ConstMng		*g_pConstMng= NULL;

CFGINFO   g_TdxCfg;
short	  g_WSXH=2;
CString	  g_WSXHStr;
CTime	  g_tNowTime;
long	  g_iToday=0;
int       g_bYesterdayFlag = 0;

short anFZRatio[10]={5,15,30,60,1,1,1,1,10,10}; // <- anFzRatio[7]={1,3,6,12,1,1,1}
short sz[8]={570,690,780,900,900,900,900,900};
short sh[8]={570,690,780,900,900,900,900,900};
char  lpszTimeStr[12][6] = {"SZQSK",  //深市前市开盘时间
			          "SZQSS",  //深市前市收盘时间
			          "SZHSK",  //深市后市开盘时间
			          "SZHSS",  //深市后市收盘时间
					  "SZYSK",
					  "SZYSS",
			          "SHQSK",  //沪市前市开盘时间
                      "SHQSS",  //沪市前市收盘时间
                      "SHHSK",  //沪市后市开盘时间
                      "SHHSS",  //沪市后市收盘时间
					  "SHYSK",
					  "SHYSS"};

char	g_DefCusPadName[64] = "";
vector<DynaCmdTable> g_vDynaCmdTab;
BOOL	g_bFirstCusPad = FALSE;

struct CmdTable g_pCmdTable[]=
{
	{"0       功能菜单",		"0",		CMD_MAIN,0},

	{".5      分时走势图",		".5",		CMD_COM,501},
	{".501    分时走势图",		".501",		CMD_COM,501},
	{"FST     分时走势图",		"FST",		CMD_COM,501},
	{"ZST     分时走势图",		"ZST",		CMD_COM,501},

	{".502    分时成交明细",	".502",		CMD_COM,502},
	{"01      分时成交明细",	"01",		CMD_COM,502},
	{"TICK    分时成交明细",	"TICK",		CMD_COM,502},

	{".503    分价表",			".503",		CMD_COM,503},
	{"02      分价表",			 "02",		CMD_COM,503},
	{"FJB     分价表",			"FJB",		CMD_COM,503},

	{".504    逐笔成交明细",   ".504",		CMD_COM,504},

	{".506    量比(多空)指标",  ".506",		CMD_COM,506},
	{"LBZB    量比(多空)指标",  "LBZB",		CMD_COM,506},

	{".507    买卖力道(涨跌率)",".507",		CMD_COM,507},
	{"MMLD    买卖力道(涨跌率)","MMLD",		CMD_COM,507},

	{"08      切换周期",		"08",		CMD_COM,8},
	{"M1      1分钟线",			"M1",		CMD_COM,801},
	{"M5      5分钟线",			"M5",		CMD_COM,802},
	{"M15     15分钟线",		"M15",		CMD_COM,803},
	{"M30     30分钟线",		"M30",		CMD_COM,804},
	{"M60     60分钟线",		"M60",		CMD_COM,805},
	{"MN      N分钟线",			"MN",		CMD_COM,806},
	{"DAY     日线",			"DAY",		CMD_COM,807},
	{"WEEK    周线",			"WEEK",		CMD_COM,808},
	{"MONTH   月线",			"MONTH",	CMD_COM,809},
	{"DAYN    N天线",			"DAYN",		CMD_COM,810},
	{"SEASON  季线",			"SEASON",	CMD_COM,811},
	{"YEAR    年线",			"YEAR",		CMD_COM,812},

	{".101    上证180走势   ",	".101",		CMD_COM,101},
	{".102    上证综指走势 ",	".102",		CMD_COM,102},
	{"03      上证领先指标 ",	"03",		CMD_COM,102},
	{".103    上证A股走势  ",	".103",		CMD_COM,103},
	{".104    上证B股走势  ",	".104",		CMD_COM,104},
	{".105    上证ADL指标  ",	".105",		CMD_COM,105},
	{".106    上证多空指标 ",	".106",		CMD_COM,106},

	{".107    深证100走势 ",	".107",		CMD_COM,107},
	{".108    深证成份走势 ",	".108",		CMD_COM,108},
	{"04      深证领先指标 ",	"04",		CMD_COM,108},
	{".109    深证综指走势 ",	".109",		CMD_COM,109},
	{".110    深证A股走势  ",	".110",		CMD_COM,110},
	{".111    深证B股走势  ",	".111",		CMD_COM,111},
	{".112    深证ADL指标  ",	".112",		CMD_COM,112},
	{".113    深证多空指标 ",	".113",		CMD_COM,113},

	{".114    上证50走势 ",		".114",		CMD_COM,114},
	{".115    上证基金指数  ",	".115",		CMD_COM,115},
	{".116    创业板指数 ",	".116",		CMD_COM,116},
	{".117    中小企业指数  ",	".117",		CMD_COM,117},
	{".118    上证红利指数  ",	".118",		CMD_COM,118},
	{".119    沪深300走势  ",	".119",		CMD_COM,119},

	{".201    分类行情显示 ",	".201",		CMD_HQ,201},
	{"HQT     分类行情显示 ",	"HQT",		CMD_HQ,201},

	{".202    振幅排名",		".202",		CMD_HQ,202},
	{".203    今日强势股",		".203",		CMD_HQ,203},
	{".204    今日弱势股",		".204",		CMD_HQ,204},
	{".205    成交量排名",		".205",		CMD_HQ,205},
	{".206    总金额排名",		".206",		CMD_HQ,206},
	{".207    量比排名",		".207",		CMD_HQ,207},
	{".208    委比排名",		".208",		CMD_HQ,208},
	{".209    现价排名",		".209",		CMD_HQ,209},
	{".210    最新大笔排名",	".210",		CMD_HQ,210},
	{".211    市盈率排名",		".211",		CMD_HQ,211},
  	{".212    换手率排名",		".212",		CMD_HQ,212},
	{".213    净资产排名",		".213",		CMD_HQ,213},
	{".214    总资产排名",		".214",		CMD_HQ,214},
	{".215    每股收益排名",	".215",		CMD_HQ,215},
	{".216    净资产收益率排名",".216",		CMD_HQ,216},
	{".217    流通市值排名",	".217",		CMD_HQ,217},
	{".218    流通股本排名",	".218",		CMD_HQ,218},
	{".219    AB股总市值排名",  ".219",		CMD_HQ,219},
	{".220    总股本排名",		".220",		CMD_HQ,220},
	{".221    每股公积金排名",  ".221",		CMD_HQ,221},
	{".222    活跃度排名",		".222",		CMD_HQ,222},
	{".223    每笔均量排名",	".223",		CMD_HQ,223},
	{".224    每笔换手排名",	".224",		CMD_HQ,224},
	
	{".225    每股净资产排名",	".225",		CMD_HQ,225},
	{".226    市净率排名",		".226",		CMD_HQ,226},

	{".300    技术分析", 		".300",		CMD_COM,300},
	{"FXT     技术分析",		"FXT",		CMD_COM,300},

	{"ＭＡＣＤ趋向指标",		".301",		CMD_COM,301},
	{"ＤＭＩ趋向指标",			".302",		CMD_COM,302},
	{"ＤＭＡ趋向指标",			".303",		CMD_COM,303},
	{"ＥＸＰＭＡ趋向指标",		".304",		CMD_COM,304},
	{"ＴＲＩＸ趋向指标",		".305",		CMD_COM,305},
	{"ＢＲＡＲ能量指标",		".306",		CMD_COM,306},
	{"ＣＲ能量指标",			".307",		CMD_COM,307},
	{"ＶＲ能量指标",			".308",		CMD_COM,308},
	{"ＯＢＶ量价指标",			".309",		CMD_COM,309},
	{"ＡＳＩ量价指标",			".310",		CMD_COM,310},
	{"ＥＭＶ量价指标",			".311",		CMD_COM,311},
	{"ＷＶＡＤ量价指标",		".312",		CMD_COM,312},
	{"ＲＳＩ强弱指标",			".313",		CMD_COM,313},
	{"Ｗ％Ｒ强弱指标",			".314",		CMD_COM,314},
	{"ＳＡＲ停损指标",			".315",		CMD_COM,315},
	{"ＫＤＪ超买超卖",			".316",		CMD_COM,316},
	{"ＣＣＩ超买超卖",			".317",		CMD_COM,317},
	{"ＲＯＣ超买超卖",			".318",		CMD_COM,318},
	{"ＭＩＫＥ压力支撑",		".319",		CMD_COM,319},
	{"ＢＯＬＬ压力支撑",		".320",		CMD_COM,320},
	{"ＰＳＹ心理线",			".321",		CMD_COM,321},
	{"ＣＤＰ逆势操作系统",		".322",		CMD_COM,322},

	{"05      类型切换",		"05",		CMD_MAIN,5},

	{"1       上证Ａ股",		"1",		CMD_HQ,1},
	{"2       上证Ｂ股",		"2",		CMD_HQ,2},
	{"3       深证Ａ股",		"3",		CMD_HQ,3},
	{"4       深证Ｂ股",		"4",		CMD_HQ,4},
	{"5       上证债券",		"5",		CMD_HQ,5},
	{"6       深证债券",		"6",		CMD_HQ,6},
	{"7       深沪Ａ股",		"7",		CMD_HQ,7},
	{"8       深沪Ｂ股",		"8",		CMD_HQ,8},
	{"9       中小企业",		"9",		CMD_HQ,13},
	{"11      深沪权证",		"11",		CMD_HQ,14},
	{"12      创业板",			"12",		CMD_HQ,15},
	{"06      自选股",			"06",		CMD_HQ,16},
	
	{"10      基本资料",		"10",		CMD_JBM,801},


	{"180     上证180走势   ",	"180",		CMD_COM,101},
	{"100     深证100走势   ",	"100",		CMD_COM,107},
	{"300     沪深300走势   ",	"300",		CMD_COM,119},

	{"61      沪Ａ涨幅排名",	"61",		CMD_HQ,61},
	{"62      沪Ｂ涨幅排名",	"62",		CMD_HQ,62},
	{"63      深Ａ涨幅排名",	"63",		CMD_HQ,63},
	{"64      深Ｂ涨幅排名",	"64",		CMD_HQ,64},
	{"65      沪债涨幅排名",	"65",		CMD_HQ,65},
	{"66      深债涨幅排名",	"66",		CMD_HQ,66},
	{"67      深沪Ａ涨幅排名",	"67",		CMD_HQ,67},
	{"68      深沪Ｂ涨幅排名",	"68",		CMD_HQ,68},
	{"69      中小企业涨幅排名","69",		CMD_HQ,69},
	{"611     深沪权证涨幅排名","611",		CMD_HQ,611},
	{"612     创业板涨幅排名",  "612",		CMD_HQ,612},

	{"613     切换涨幅排名",	"613",		CMD_HQ,613},

	{"81      沪Ａ综合排名",	"81",		CMD_MAIN,81},
	{"82      沪Ｂ综合排名",	"82",		CMD_MAIN,82},
	{"83      深Ａ综合排名",	"83",		CMD_MAIN,83},
	{"84      深Ｂ综合排名",	"84",		CMD_MAIN,84},
	{"85      沪债综合排名",	"85",		CMD_MAIN,85},
	{"86      深债综合排名",	"86",		CMD_MAIN,86},
	{"87      深沪Ａ综合排名",	"87",		CMD_MAIN,87},
	{"88      深沪Ｂ综合排名",	"88",		CMD_MAIN,88},
	{"89      中小企业综合排名","89",		CMD_MAIN,89},
	{"811     深沪权证综合排名","811",		CMD_MAIN,811},
	{"812     创业板综合排名"  ,"812",		CMD_MAIN,812},

	{"9.      1分钟线",			"9.",		CMD_COM,801},
	{"96      5分钟线",			"96",		CMD_COM,802},
	{"97      15分钟线",		"97",		CMD_COM,803},
	{"98      30分钟线",		"98",		CMD_COM,804},
	{"99      60分钟线",		"99",		CMD_COM,805},
	{"91      日线",			"91",		CMD_COM,807},
	{"92      周线",			"92",		CMD_COM,808},
	{"93      月线",			"93",		CMD_COM,809},
	{"90      N天线",			"90",		CMD_COM,810},
	{"910     N分钟线",			"910",		CMD_COM,806},
	{"94	  季线",			"94",		CMD_COM,811},
	{"95      年线",			"95",		CMD_COM,812},

	{".902    条件选股测试",	".902",		CMD_MAIN,902},
	{".903    交易系统测试",	".903",		CMD_MAIN,903},
	{".904    探索最佳指标",	".904",		CMD_MAIN,904},

	{".905    条件选股",		".905",		CMD_MAIN,905},
	{".906    定制选股",		".906",		CMD_MAIN,906},
	{".909    综合选股",		".909",		CMD_MAIN,909},

	{".910    市场雷达设置",	".910",		CMD_MAIN,910},
	{".911    市场雷达列表",	".911",		CMD_MAIN,911},
	{".918    条件预警设置",	".918",		CMD_MAIN,918},
	{".919    条件预警列表",	".919",		CMD_MAIN,919},

	{".913    股票组合计算",	".913",		CMD_MAIN,913},

	{".930    系统设置",		".930",		CMD_MAIN,930},
	{".931    设置用户板块",	".931",		CMD_MAIN,931},
	{".933    盘后数据下载",	".933",		CMD_MAIN,933},
	{".936    帮助说明书",		".936",		CMD_MAIN,936},

    {"",0,0,0}  //Code为0表示结束
};

/*
struct CmdTable g_pCmdTable[]=
{
	{"0       功能菜单",		"0",		CMD_MAIN,0},

	{".8      交易委托",		".8",		CMD_MAIN,1000},

	{".5      分时走势图",		".5",		CMD_COM,501},
	{".501    分时走势图",		".501",		CMD_COM,501},
	{"FST     分时走势图",		"FST",		CMD_COM,501},
	{"ZST     分时走势图",		"ZST",		CMD_COM,501},

	{".502    分时成交明细",	".502",		CMD_COM,502},
	{"01      分时成交明细",	"01",		CMD_COM,502},
	{"TICK    分时成交明细",	"TICK",		CMD_COM,502},

	{".503    分价表",			".503",		CMD_COM,503},
	{"02      分价表",			 "02",		CMD_COM,503},
	{"FJB     分价表",			"FJB",		CMD_COM,503},

	{".504    逐笔成交明细",   ".504",		CMD_COM,504},

	{".505    详细买卖盘",		".505",		CMD_COM,505},
	{"MMP     详细买卖盘",		"MMP",		CMD_COM,505},

	{".506    量比(多空)指标",  ".506",		CMD_COM,506},
	{"LBZB    量比(多空)指标",  "LBZB",		CMD_COM,506},

	{".507    买卖力道(涨跌率)",".507",		CMD_COM,507},
	{"MMLD    买卖力道(涨跌率)","MMLD",		CMD_COM,507},
	{".508    闪电走势图",		".508",		CMD_COM,508},

	{"08      切换周期",		"08",		CMD_COM,8},
	{"M1      1分钟线",			"M1",		CMD_COM,801},
	{"M5      5分钟线",			"M5",		CMD_COM,802},
	{"M15     15分钟线",		"M15",		CMD_COM,803},
	{"M30     30分钟线",		"M30",		CMD_COM,804},
	{"M60     60分钟线",		"M60",		CMD_COM,805},
	{"MN      N分钟线",			"MN",		CMD_COM,806},
	{"DAY     日线",			"DAY",		CMD_COM,807},
	{"WEEK    周线",			"WEEK",		CMD_COM,808},
	{"MONTH   月线",			"MONTH",	CMD_COM,809},
	{"DAYN    N天线",			"DAYN",		CMD_COM,810},
	{"SEASON  季线",			"SEASON",	CMD_COM,811},
	{"YEAR    年线",			"YEAR",		CMD_COM,812},

	{".101    上证180走势   ",	".101",		CMD_COM,101},
	{".102    上证综指走势 ",	".102",		CMD_COM,102},
	{"03      上证领先指标 ",	"03",		CMD_COM,102},
	{".103    上证A股走势  ",	".103",		CMD_COM,103},
	{".104    上证B股走势  ",	".104",		CMD_COM,104},
	{".105    上证ADL指标  ",	".105",		CMD_COM,105},
	{".106    上证多空指标 ",	".106",		CMD_COM,106},

	{".107    深证100走势 ",	".107",		CMD_COM,107},
	{".108    深证成份走势 ",	".108",		CMD_COM,108},
	{"04      深证领先指标 ",	"04",		CMD_COM,108},
	{".109    深证综指走势 ",	".109",		CMD_COM,109},
	{".110    深证A股走势  ",	".110",		CMD_COM,110},
	{".111    深证B股走势  ",	".111",		CMD_COM,111},
	{".112    深证ADL指标  ",	".112",		CMD_COM,112},
	{".113    深证多空指标 ",	".113",		CMD_COM,113},

	{".114    上证50走势 ",		".114",		CMD_COM,114},
	{".115    上证基金指数  ",	".115",		CMD_COM,115},
	{".116    创业板指数 ",	".116",		CMD_COM,116},
	{".117    中小企业指数  ",	".117",		CMD_COM,117},
	{".118    上证红利指数  ",	".118",		CMD_COM,118},
	{".119    沪深300走势  ",	".119",		CMD_COM,119},

	{".201    分类行情显示 ",	".201",		CMD_HQ,201},
	{"HQT     分类行情显示 ",	"HQT",		CMD_HQ,201},

	{".202    振幅排名",		".202",		CMD_HQ,202},
	{".203    今日强势股",		".203",		CMD_HQ,203},
	{".204    今日弱势股",		".204",		CMD_HQ,204},
	{".205    成交量排名",		".205",		CMD_HQ,205},
	{".206    总金额排名",		".206",		CMD_HQ,206},
	{".207    量比排名",		".207",		CMD_HQ,207},
	{".208    委比排名",		".208",		CMD_HQ,208},
	{".209    现价排名",		".209",		CMD_HQ,209},
	{".210    最新大笔排名",	".210",		CMD_HQ,210},
	{".211    市盈率排名",		".211",		CMD_HQ,211},
  	{".212    换手率排名",		".212",		CMD_HQ,212},
	{".213    净资产排名",		".213",		CMD_HQ,213},
	{".214    总资产排名",		".214",		CMD_HQ,214},
	{".215    每股收益排名",	".215",		CMD_HQ,215},
	{".216    净资产收益率排名",".216",		CMD_HQ,216},
	{".217    流通市值排名",	".217",		CMD_HQ,217},
	{".218    流通股本排名",	".218",		CMD_HQ,218},
	{".219    AB股总市值排名",  ".219",		CMD_HQ,219},
	{".220    总股本排名",		".220",		CMD_HQ,220},
	{".221    每股公积金排名",  ".221",		CMD_HQ,221},
	{".222    活跃度排名",		".222",		CMD_HQ,222},
	{".223    每笔均量排名",	".223",		CMD_HQ,223},
	{".224    每笔换手排名",	".224",		CMD_HQ,224},
	
	{".225    每股净资产排名",	".225",		CMD_HQ,225},
	{".226    市净率排名",		".226",		CMD_HQ,226},

	{".400    热门板块分析",	".400",		CMD_HIS,400},
	{".401    历史行情报表",	".401",		CMD_HIS,401},
	{".402    强弱分析报表",	".402",		CMD_HIS,402},
	{".403    区间涨跌幅度",	".403",		CMD_HIS,403},
	{".404    区间换手排名",	".404",		CMD_HIS,404},
	{".405    区间量变幅度",	".405",		CMD_HIS,405},
	{".406    区间振荡幅度",	".406",		CMD_HIS,406},

	{".300    技术分析", 		".300",		CMD_COM,300},
	{"FXT     技术分析",		"FXT",		CMD_COM,300},

	{"ＭＡＣＤ趋向指标",		".301",		CMD_COM,301},
	{"ＤＭＩ趋向指标",			".302",		CMD_COM,302},
	{"ＤＭＡ趋向指标",			".303",		CMD_COM,303},
	{"ＥＸＰＭＡ趋向指标",		".304",		CMD_COM,304},
	{"ＴＲＩＸ趋向指标",		".305",		CMD_COM,305},
	{"ＢＲＡＲ能量指标",		".306",		CMD_COM,306},
	{"ＣＲ能量指标",			".307",		CMD_COM,307},
	{"ＶＲ能量指标",			".308",		CMD_COM,308},
	{"ＯＢＶ量价指标",			".309",		CMD_COM,309},
	{"ＡＳＩ量价指标",			".310",		CMD_COM,310},
	{"ＥＭＶ量价指标",			".311",		CMD_COM,311},
	{"ＷＶＡＤ量价指标",		".312",		CMD_COM,312},
	{"ＲＳＩ强弱指标",			".313",		CMD_COM,313},
	{"Ｗ％Ｒ强弱指标",			".314",		CMD_COM,314},
	{"ＳＡＲ停损指标",			".315",		CMD_COM,315},
	{"ＫＤＪ超买超卖",			".316",		CMD_COM,316},
	{"ＣＣＩ超买超卖",			".317",		CMD_COM,317},
	{"ＲＯＣ超买超卖",			".318",		CMD_COM,318},
	{"ＭＩＫＥ压力支撑",		".319",		CMD_COM,319},
	{"ＢＯＬＬ压力支撑",		".320",		CMD_COM,320},
	{"ＰＳＹ心理线",			".321",		CMD_COM,321},
	{"ＣＤＰ逆势操作系统",		".322",		CMD_COM,322},

	{"05      类型切换",		"05",		CMD_MAIN,5},

	{"1       上证Ａ股",		"1",		CMD_HQ,1},
	{"2       上证Ｂ股",		"2",		CMD_HQ,2},
	{"3       深证Ａ股",		"3",		CMD_HQ,3},
	{"4       深证Ｂ股",		"4",		CMD_HQ,4},
	{"5       上证债券",		"5",		CMD_HQ,5},
	{"6       深证债券",		"6",		CMD_HQ,6},
	{"7       深沪Ａ股",		"7",		CMD_HQ,7},
	{"8       深沪Ｂ股",		"8",		CMD_HQ,8},
	{"9       中小企业",		"9",		CMD_HQ,13},
	{"11      深沪权证",		"11",		CMD_HQ,14},
	{"12      创业板",			"12",		CMD_HQ,15},
	{"06      自选股",			"06",		CMD_HQ,15},
	
	{"10      基本资料",		"10",		CMD_JBM,801},
	{"TDXJBZL 权息资料",		"TDXJBZL",	CMD_JBM,802},

	{"16      信息地雷",		"16",		CMD_COM,16},
	{"17      投资日记",		"17",		CMD_COM,17},
	{"18      机构评测和诊断",	"18",		CMD_MAIN,916},
#if defined(OEM_NEWJY) || defined(LEVEL2)
	{"19      研究报告平台",	"19",		CMD_MAIN,1080},
#endif
#ifdef OEM_NEWJY 
	{"20      交易委托",		"20",		CMD_MAIN,1000},
	{"21      闪电买入",		"21",		CMD_MAIN,1001},
	{"22      撤单查询",		"22",		CMD_MAIN,1002},	
	{"23      闪电卖出",		"23",		CMD_MAIN,1003},
	{"25      闪电手",			"25",		CMD_MAIN,1005},
	{"221     普通买入",		"221",		CMD_MAIN,1006},
	{"223     普通卖出",		"223",		CMD_MAIN,1007},

	{".+1     卖一价买入",		".+1",		CMD_MAIN,1011},
	{".+2     卖二价买入",		".+2",		CMD_MAIN,1012},
	{".+3     卖三价买入",		".+3",		CMD_MAIN,1013},
	{".+4     卖四价买入",		".+4",		CMD_MAIN,1014},
	{".+5     卖五价买入",		".+5",		CMD_MAIN,1015},

	{".+01    买一价买入",		".+01",		CMD_MAIN,1016},
	{".+02    买二价买入",		".+02",		CMD_MAIN,1017},
	{".+03    买三价买入",		".+03",		CMD_MAIN,1018},
	{".+04    买四价买入",		".+04",		CMD_MAIN,1019},
	{".+05    买五价买入",		".+05",		CMD_MAIN,1020},

	{".-1     买一价卖出",		".-1",		CMD_MAIN,1021},
	{".-2     买二价卖出",		".-2",		CMD_MAIN,1022},
	{".-3     买三价卖出",		".-3",		CMD_MAIN,1023},
	{".-4     买四价卖出",		".-4",		CMD_MAIN,1024},
	{".-5     买五价卖出",		".-5",		CMD_MAIN,1025},

	{".-01    卖一价卖出",		".-01",		CMD_MAIN,1026},
	{".-02    卖二价卖出",		".-02",		CMD_MAIN,1027},
	{".-03    卖三价卖出",		".-03",		CMD_MAIN,1028},
	{".-04    卖四价卖出",		".-04",		CMD_MAIN,1029},
	{".-05    卖五价卖出",		".-05",		CMD_MAIN,1030},
#endif
	{"30      切换关联品种",	"30",		CMD_COM,30},

	{"31      理财金算盘",		"31",		CMD_MAIN,912},
	{"32      个人理财",		"32",		CMD_MAIN,915},
	{"33      在线人气",		"33",		CMD_MAIN,917},
	{"34      数据导出",		"34",		CMD_MAIN,920},
	{"35      江恩正方",		"35",		CMD_MAIN,921},
	{"36      重仓持股基金",	"36",		CMD_MAIN,922},
	{"37      基金分析平台",	"37",		CMD_MAIN,923},
	{"38      版面管理器",		"38",		CMD_MAIN,924},
	{"39      显隐版面切换区",	"39",		CMD_MAIN,925},

	{"180     上证180走势   ",	"180",		CMD_COM,101},
	{"100     深证100走势   ",	"100",		CMD_COM,107},
	{"300     沪深300走势   ",	"300",		CMD_COM,119},

	{"61      沪Ａ涨幅排名",	"61",		CMD_HQ,61},
	{"62      沪Ｂ涨幅排名",	"62",		CMD_HQ,62},
	{"63      深Ａ涨幅排名",	"63",		CMD_HQ,63},
	{"64      深Ｂ涨幅排名",	"64",		CMD_HQ,64},
	{"65      沪债涨幅排名",	"65",		CMD_HQ,65},
	{"66      深债涨幅排名",	"66",		CMD_HQ,66},
	{"67      深沪Ａ涨幅排名",	"67",		CMD_HQ,67},
	{"68      深沪Ｂ涨幅排名",	"68",		CMD_HQ,68},
	{"69      中小企业涨幅排名","69",		CMD_HQ,69},
	{"611     深沪权证涨幅排名","611",		CMD_HQ,611},
	{"612     创业板涨幅排名",  "612",		CMD_HQ,612},

	{"613     切换涨幅排名",	"613",		CMD_HQ,613},

	{"81      沪Ａ综合排名",	"81",		CMD_MAIN,81},
	{"82      沪Ｂ综合排名",	"82",		CMD_MAIN,82},
	{"83      深Ａ综合排名",	"83",		CMD_MAIN,83},
	{"84      深Ｂ综合排名",	"84",		CMD_MAIN,84},
	{"85      沪债综合排名",	"85",		CMD_MAIN,85},
	{"86      深债综合排名",	"86",		CMD_MAIN,86},
	{"87      深沪Ａ综合排名",	"87",		CMD_MAIN,87},
	{"88      深沪Ｂ综合排名",	"88",		CMD_MAIN,88},
	{"89      中小企业综合排名","89",		CMD_MAIN,89},
	{"811     深沪权证综合排名","811",		CMD_MAIN,811},
	{"812     创业板综合排名"  ,"812",		CMD_MAIN,812},

	{"91      1分钟线",			"91",		CMD_COM,801},
	{"92      5分钟线",			"92",		CMD_COM,802},
	{"93      15分钟线",		"93",		CMD_COM,803},
	{"94      30分钟线",		"94",		CMD_COM,804},
	{"95      60分钟线",		"95",		CMD_COM,805},
	{"96      日线",			"96",		CMD_COM,807},
	{"97      周线",			"97",		CMD_COM,808},
	{"98      月线",			"98",		CMD_COM,809},
	{"99      N天线",			"99",		CMD_COM,810},
	{"910     N分钟线",			"910",		CMD_COM,806},
	{"911     季线",			"911",		CMD_COM,811},
	{"912     年线",			"912",		CMD_COM,812},

	{".7      财经资讯",		".7",		CMD_MAIN,1101},
	{"70      财经资讯",		"70",		CMD_MAIN,1101},
	{"07      财经资讯",		"07",		CMD_MAIN,1101},

	{".701",					".701",		CMD_NEWS,701},
	{"71",						"71",		CMD_NEWS,701},

	{".702 ",					".702",		CMD_NEWS,702},
	{"72",						"72",		CMD_NEWS,702},
	{".703 ",					".703",		CMD_NEWS,703},
	{"73",						"73",		CMD_NEWS,703},
	{".704 ",					".704",		CMD_NEWS,704},
	{"74",						"74",		CMD_NEWS,704},
	{".705 ",					".705",		CMD_NEWS,705},
	{"75",						"75",		CMD_NEWS,705},
	{".706 ",					".706",		CMD_NEWS,706},
	{"76",						"76",		CMD_NEWS,706},
	{".707 ",					".707",		CMD_NEWS,707},
	{"77",						"77",		CMD_NEWS,707},
	{".708 ",					".708",		CMD_NEWS,708},
	{"78",						"78",		CMD_NEWS,708},
	{".709 ",					".709",		CMD_NEWS,709},
	{"79",						"79",		CMD_NEWS,709},
	{".710 ",					".710",		CMD_NEWS,710},
	{".711 ",					".711",		CMD_NEWS,711},
	{".712 ",					".712",		CMD_NEWS,712},
	{".713 ",					".713",		CMD_NEWS,713},
	{".714 ",					".714",		CMD_NEWS,714},
	{".715 ",					".715",		CMD_NEWS,715},
	{".716 ",					".716",		CMD_NEWS,716},
	{".717 ",					".717",		CMD_NEWS,717},
	{".718 ",					".718",		CMD_NEWS,718},
	{".719 ",					".719",		CMD_NEWS,719},
	{".720 ",					".720",		CMD_NEWS,720},
	
	{"WT      交易委托",		"WT",		CMD_MAIN,1000},

	{".901    公式管理器",		".901",		CMD_MAIN,901},
	{".902    条件选股测试",	".902",		CMD_MAIN,902},
	{".903    交易系统测试",	".903",		CMD_MAIN,903},
	{".904    探索最佳指标",	".904",		CMD_MAIN,904},

	{".905    条件选股",		".905",		CMD_MAIN,905},
	{".906    定制选股",		".906",		CMD_MAIN,906},
	{".907    智能选股",		".907",		CMD_MAIN,907},
	{".908    插件选股",		".908",		CMD_MAIN,908},
	{".909    综合选股",		".909",		CMD_MAIN,909},

	{".910    市场雷达设置",	".910",		CMD_MAIN,910},
	{".911    市场雷达列表",	".911",		CMD_MAIN,911},
	{".918    条件预警设置",	".918",		CMD_MAIN,918},
	{".919    条件预警列表",	".919",		CMD_MAIN,919},

	{".912    理财金算盘",		".912",		CMD_MAIN,912},
	{".913    股票组合计算",	".913",		CMD_MAIN,913},
	{".914    数据挖掘参考",	".914",		CMD_MAIN,914},

	{".920    星空图",			".920",		CMD_ADVHQ,0},
	{".921    江恩正方",		".921",		CMD_MAIN,921},

	{".930    系统设置",		".930",		CMD_MAIN,930},
	{".931    设置用户板块",	".931",		CMD_MAIN,931},
	{".932    通讯设置",		".932",		CMD_MAIN,932},	
	{".933    盘后数据下载",	".933",		CMD_MAIN,933},
	{".934    数据维护工具",	".934",		CMD_MAIN,934},
	{".935    自动升级",		".935",		CMD_MAIN,935},
	{".936    帮助说明书",		".936",		CMD_MAIN,936},
	{".937    用户论坛",		".937",		CMD_MAIN,937},

    {"",0,0,0}  //Code为0表示结束
};
*/

int g_nCmdTable = chDIMOF(g_pCmdTable);
WORD FuncMessageCode[][2] =	//功能树Item附带消息代码
{ 
	CMD_COM,101, CMD_COM,102, CMD_COM,103, CMD_COM,104, CMD_COM,105,
	CMD_COM,106, CMD_COM,107, CMD_COM,108, CMD_COM,109, CMD_COM,110,
	CMD_COM,111, CMD_COM,112, CMD_COM,113, CMD_COM,114, CMD_COM,115, CMD_COM,116,CMD_COM,117,CMD_COM,118,CMD_COM,119,
	CMD_HQ, 201, CMD_HQ, 202, CMD_HQ, 203, CMD_HQ, 204, CMD_HQ, 205,
	CMD_HQ, 206, CMD_HQ, 207, CMD_HQ, 208, CMD_HQ, 209, CMD_HQ, 210,
	CMD_HQ, 211, CMD_HQ, 212, CMD_HQ, 213, CMD_HQ, 214, CMD_HQ, 215, CMD_HQ, 216,
	CMD_HQ, 217, CMD_HQ, 218, CMD_HQ, 219, CMD_HQ, 220, CMD_HQ, 221, CMD_HQ, 222,
	CMD_HQ, 223, CMD_HQ, 224, CMD_HQ, 225, CMD_HQ,226,
	CMD_COM,301, CMD_COM,302, CMD_COM,303, CMD_COM,304, CMD_COM,305,
	CMD_COM,306, CMD_COM,307, CMD_COM,308, CMD_COM,309, CMD_COM,310,
	CMD_COM,311, CMD_COM,312, CMD_COM,313, CMD_COM,314, CMD_COM,315, 
	CMD_COM,316, CMD_COM,317, CMD_COM,318, CMD_COM,319, CMD_COM,320, 
	CMD_COM,321, CMD_COM,322,
	CMD_HIS,400, CMD_HIS,401, CMD_HIS,402, CMD_HIS,403,CMD_HIS,404, CMD_HIS,405, CMD_HIS,406,
	CMD_COM,501, CMD_COM,502, CMD_COM,503, CMD_COM,504, CMD_COM,505,CMD_COM,506,CMD_COM,507,CMD_COM,508,
	CMD_MAIN,1100,		//基本资料
	CMD_MAIN,1101,		//资讯系统
	CMD_MAIN,1102,		//个人理财
	CMD_MAIN,1000,		//交易委托
	CMD_MAIN,901,CMD_MAIN,902,CMD_MAIN,903,CMD_MAIN,904,
	CMD_MAIN,905,CMD_MAIN,906,CMD_MAIN,907,CMD_MAIN,908,CMD_MAIN,909,
	CMD_MAIN,910,CMD_MAIN,911,CMD_MAIN,918,CMD_MAIN,919,
	CMD_MAIN,912,CMD_MAIN,913,CMD_MAIN,914,
	CMD_ADVHQ,0, CMD_MAIN,921,
	CMD_MAIN,930,CMD_MAIN,931,CMD_MAIN,932,CMD_MAIN,933,CMD_MAIN,934,CMD_MAIN,935,CMD_MAIN,936,CMD_MAIN,937
};

struct FirstLevel FuncLevel1[] =
{	
	{"大盘分析",19},{"报价分析",26},
	{"技术分析",22},{"报表分析",7},
	{"即时分析",8},
	{"基本资料",0},{"资讯系统",0}, 
	{"个人理财",0},{"交易委托",0},
	{"专家系统",4},
	{"选股器",5},
	{"预警系统",4},
	{"其它功能",3},
	{"高级行情功能",2},
	{"系统工具",8},
	{"",0}
};

char *FuncLevel2[] = 
{
	".101 上证１８０走势",
	".102 上证综指走势",
	".103 上证Ａ股走势",
	".104 上证Ｂ股走势",
	".105 上证ＡＤＬ指标",
	".106 上证多空指标",
	".107 深证１００走势",
	".108 深证成份走势",
	".109 深证综指走势",
	".110 深证Ａ股走势",
	".111 深证Ｂ股走势",
	".112 深证ＡＤＬ指标",
	".113 深证多空指标",
	".114 上证５０走势",
	".115 上证基金指数",
	".116 创业板指数",
	".117 中小企业指数",
	".118 上证红利指数",
	".119 沪深300走势",

	".201 分类行情显示",
	".202 振幅排名",
	".203 今日强势股",
	".204 今日弱势股",
	".205 成交量排名",
	".206 总金额排名",
	".207 量比排名",
	".208 委比排名",
	".209 现价排名",
	".210 最新大笔排名",
	".211 市盈率排名",
	".212 换手率排名",
	".213 净资产排名",
	".214 总资产排名",
	".215 每股收益排名",
	".216 净资产收益率排名",
	".217 流通市值排名",
	".218 流通股本排名",
	".219 AB股总市值排名",
	".220 总股本排名",
	".221 每股公积金排名",
	".222 活跃度排名",
	".223 每笔均量排名",
	".224 每笔换手排名",
	".225 每股净资产排名",
	".226 市净率排名",	
	
	".301 ＭＡＣＤ趋向指标",
	".302 ＤＭＩ趋向指标",
	".303 ＤＭＡ趋向指标",
	".304 ＥＸＰＭＡ趋向指标",
	".305 ＴＲＩＸ趋向指标",
	".306 ＢＲＡＲ能量指标",
	".307 ＣＲ能量指标",
	".308 ＶＲ能量指标",
	".309 ＯＢＶ量价指标",
	".310 ＡＳＩ量价指标",
	".311 ＥＭＶ量价指标",
	".312 ＷＶＡＤ量价指标",
	".313 ＲＳＩ强弱指标",
	".314 Ｗ％Ｒ强弱指标",
	".315 ＳＡＲ停损指标",
	".316 ＫＤＪ超买超卖",
	".317 ＣＣＩ超买超卖",
	".318 ＲＯＣ超买超卖",
	".319 ＭＩＫＥ压力支撑",
	".320 ＢＯＬＬ压力支撑",
	".321 ＰＳＹ心理线",
	".322 ＣＤＰ逆势操作",

	".400 热门板块分析",
	".401 历史行情报表",
	".402 强弱分析报表",
	".403 区间涨跌幅度",
	".404 区间换手排名",
	".405 区间量变幅度",
	".406 区间振荡幅度",

	".501 分时走势图",
	".502 分时成交明细",
	".503 分价表",
	".504 逐笔成交明细",
	".505 详细买卖盘",
	".506 量比/多空指标",
	".507 买卖力道/涨跌率",
	".508 闪电走势图",

	".901 公式管理器",
	".902 条件选股测试",
	".903 交易系统测试",
	".904 探索最佳指标",

	".905 条件选股",
	".906 定制选股",
	".907 智能选股",
	".908 插件选股",
	".909 综合选股",

	".910 市场雷达设置",
	".911 市场雷达列表",
	".918 条件预警设置",
	".919 条件预警列表",

	".912 理财金算盘",
	".913 股票组合计算",
	".914 数据挖掘参考",

	".920 星空图",
	".921 江恩正方",

	".930 系统设置",
	".931 设置用户板块",
	".932 通讯设置",
	".933 盘后数据下载",
	".934 数据维护工具",
	".935 自动升级",
	".936 帮助说明书",
	".937 用户论坛"
};

int   lpnSysDomainType[]={SH,SH,SZ,SZ,SH,SZ,SZ_SH,SZ_SH,SZ_SH,SZ_SH,SZ_SH,SZ_SH,SZ,SZ_SH,SZ};  //上海深圳类别,新增中小企业创业板
short lpsSysClassType_new []={0x01,0x02,0x481,0x02,0x058,0x058,0x481,0x02,0x058,0x04,0x00,0x99,0x80,0x20,0x400};   //股票类别:A/B/C

char  *GPType[]=
{
	"上证Ａ股",
	"上证Ｂ股",   
	"深证Ａ股",   
	"深证Ｂ股",   
	"上证债券",   
	"深证债券",   
	"深沪Ａ股",   
	"深沪Ｂ股",   
	"深沪债券",
	"深沪基金",
	"所有商品",
	"所有指数",
	"中小企业",
	"深沪权证",
	"创业板",
	"自选股",
	"股票池",
	"三板证券",
	"板块指数",
	"A+H股",
	"行业指数",
};
//////////////////////////////////////////////////////////////////////////
char lpszRMItemName_All[][20] = 
{	"板块名称",
	"加权价",	//0
	"权涨幅%%",	//1
	"成交量",		//5
	"总金额",		//6
	"市场比%%",		//7
	"换手率%%",		//8
	"市盈(动)",		//9
	"领涨股票",
	"涨股比",

	"大单流入",
	"大单流出",
	"中单流入",
	"中单流出",
	"小单流入",
	"小单流出",
};
float lpnRMItemWidth_All[]={10,9,9,10,10,9,9,9,10,8,9,9,9,9,9,9};
float lpnRMItemWidth_Long_All[]={30,9,9,10,10,9,9,9,10,8,9,9,9,9,9,9};

int nColToXh_Hq[]={0,1,2,3,4,5,6,7,8,9};
int nColToXh_Zj[]={0,2,3,4,10,11,12,13,14,15};
//////////////////////////////////////////////////////////////////////////
//历史报表的栏目设置
char  lpszZBItemName[][20] =
{	"代码   ",			//0
	"名称",			//1
	" 涨幅%%",			//6
	" 开盘",			//2
	" 最高",			//3
	" 最低",			//4
	" 收盘",			//5
	"成交量",			//7
	"总金额",			//8
	"换手率%%",          //9
	"输出线1",
	"输出线2",
	"输出线3",
	"输出线4",
	"输出线5",
	"输出线6",

	"输出线1",
	"输出线2",
	"输出线3",
	"输出线4",
	"输出线5",
	"输出线6",

	"输出线1",
	"输出线2",
	"输出线3",
	"输出线4",
	"输出线5",
	"输出线6",

	"输出线1",
	"输出线2",
	"输出线3",
	"输出线4",
	"输出线5",
	"输出线6"
};
float lpnZBItemWidth[]={7,9,7,7,7,7,7,8,9,8,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9};

//强弱分析的栏目设置
char  lpszQRItemName[][20] =
{	"代码   ",				//0
	"名称",			//1
	"今日强",			//2
	"        ",			//3
	"        ",			//4
	"        ",			//5
	"        ",			//6
	"        ",			//7
	"年强度",			//8
	"开盘",				//9
	"最高",				//10
	"最低",				//11
	"收盘",				//12	
	"总金额"			//13
};
float lpnQRItemWidth[] = {7,9,7,7,7,7,7,7,7,7,7,7,7,9};
//区间分析的栏目
char lpszQJItemName[][20] =
{	"代码   ",		//0
	"名称",			//1
	"前收盘",		//2
	"最高",			//3
	"最低",			//4
	"收盘",			//5
	"涨跌幅度",		//6
	"振荡幅度",		//7
	"成交量",       //8
	"总金额",       //9
	"市场比%%",		//10				//总金额/9999总金额
	"换手率%%",		//11              	//总金额/总金额
	"5日量变%%"   	//12				//近5日量变幅度
};

float lpnQJItemWidth[]={7,9,7,7,7,7,9,9,9,9,8,8,10};

int naPriceExtent []={0,1,6,2,3,4,5,7,8,9,10,11,12};	//涨跌幅度
int naPopPoint    []={0,1,11,2,3,4,5,6,7,8,9,10,12};	//换手率排名
int naVolumeExtent[]={0,1,12,2,3,4,5,6,7,8,9,10,11};	//量变幅度
int naDiffExtent  []={0,1,7,2,3,4,5,6,8,9,10,11,12};	//振荡幅度

char *QJTypeStr[] = {"涨跌幅度","换手率排名","量变幅度","振荡幅度"};

char *AmiComboStr[8] = {"总收益最大化","平均收益最大化","扣除最大赢利后收益最大化","平均赢利最大化","平均亏损最小化","单次最大赢利最大化","单次最大亏损最小化","盈亏系数最大化"};

BOOL	bStartTips = TRUE;
int		nTipFilePos = 0;

HWND			FirstWindow,BigMenuWindow;
CViewManager	theViewManager;

struct GRIDCOLOR	GridColor;
struct VIPCOLOR		VipColor;
struct TXTCOLOR		TxtColor;
struct TREECOLOR	TreeColor;
struct SYSTEMCOLOR  SystemColor;

struct MENUCOLOR	MenuColor;

int					CurrentScheme;

CString HomePath="";

int AutoPageTime=8;
int HQRefreshTime = 4;
int StatusRefreshTime=5;

BOOL g_bAutoConnectInfo = FALSE;	//启动后自动登录资讯
BOOL g_bAutoLogo = FALSE;			//启动后自动登录行情
BOOL g_bAutoDS = FALSE;				//启动后自动登录扩展行情
BOOL g_bUseAutoConnect = TRUE;	//是否使用自动重连
int	 g_nAutoConnectFlag = AUTOCONNECT_RESET;	//AUTOCONNECT_RESET,AUTOCONNECT_PENDING,AUTOCONNECT_RECONNECT
int	 g_nDSAutoConnectFlag = AUTOCONNECT_RESET;
int	 g_nAutoConnectNum = 0;		//自动重连的次数
BOOL g_bHasTdxProxy = FALSE;	//是否有ISA代理

BOOL bTreeShow=FALSE;
BOOL bRestShow=FALSE;
int	 nLastTreeSel=0;
int	 nLastRestSel=0;
BOOL bToolBarShow=TRUE;
BOOL bStatusBarShow=TRUE;
BOOL g_bRightShow=TRUE;		//是否隐藏右边的数据
BOOL g_nRollInfoHeight=24;	//滚动资讯的高度

CString FirstBlockName;
int FirstGPType;
int FirstBlockFlag;
CString FirstHisBlockName;
int FirstHisGPType;
int FirstHisBlockFlag;

int g_CurrentJBM;	//当前用户最喜欢的基本资料

//用于在基本资料模块中传递股票代码
CString TxtCurGPCode;
short	TxtCurSetCode = -1;	//-1:仅按代码查找,SZ:在深圳中查找,SH:在上海中查找
//用于在分析模块中传递股票代码
CString ComCurGPCode = "600839";
short	ComCurSetCode = SH;	//-1:仅按代码查找,SZ:在深圳中查找,SH:在上海中查找
//用于服务资讯的
CString WebInfoCurGPCode = "600839";
short	WebInfoCurSetCode = SH;	//-1:仅按代码查找,SZ:在深圳中查找,SH:在上海中查找
CString WebURL;
//用于财经资讯
CString CjzxCurGPCode = "600839";
short	CjzxCurSetCode = SH;	//-1:仅按代码查找,SZ:在深圳中查找,SH:在上海中查找
//用于基本资料
CString JbfxCurGPCode = "600839";
short	JbfxCurSetCode = SH;	//-1:仅按代码查找,SZ:在深圳中查找,SH:在上海中查找

//用于在历史分析中传递当前用户首先打开的历史分析类型
int		HisCurType = 0;

int XXMNum = 0;
int XXMType[20];
CString XXMTopic[20];

struct SYSTEMCOLOR Scheme_Hold; //系统保留方案
struct SYSTEMCOLOR Scheme[20];
CString SchemeStr[20];
int SchemeNum;

struct SchemeItemContent theItemContent[28] = 
{
	"背景色",0,
	"阳线及上涨字",1,
	"阴线",2,
	"下跌字",3,
	"平线",4,
	"其它线",5,
	"坐标轴,框",6,
	"大标题",7,
	"坐标文字颜色",8,
	"成交量颜色",9,
	"图形文字",10,
	"高亮背景色",11,
	"画线工具画线颜色",12,
	"指标输出1颜色",13,
	"指标输出2颜色",14,
	"指标输出3颜色",15,
	"指标输出4颜色",16,
	"指标输出5颜色",17,
	"指标输出6颜色",18,
	"行情表框线(与背景相同则隐去)",19,
	"行情表选中线",20,
	"行情表固定行列文本",21,
	"行情表固定行列排序箭头",22,
	"行情表股票代码和名称",23,
	"行情表其它颜色/自选股颜色",24,
	"资料和资讯文本",25,
	"功能树文本",26,
	"资料、资讯和功能树背景",27
};

//用于条件选股
char *g_CalcPeriodStr[] = {"1分钟","5分钟","15分钟","30分钟","60分钟","日线","周线","月线","多分钟","多日线","季线","年线",NULL};

int		IndexSortNum;	//排序指标个数,最多支持4个指标
int		CurIndexSort;	//当前正在处理的排序指标
struct	IndexSortInfo_Struct IndexSortInfo[4];

//用于存放强弱分析的天数参
int QRParam[5];

WARN_STRU		g_WarnBuffer[WARN_BUFFER_SIZE];
int				g_nWarnNum = 0 ;
WARN_STRU		g_TJWarnBuffer[WARN_BUFFER_SIZE];
int				g_nTJWarnNum = 0 ;
int				g_nWarnRangeMinute = 5;
deque<WARN_STRU> g_TimeRangeWarnBuffer;
int				g_nWarnHQPos = 0;

WARNGLOBALPARAM		g_WarnParam;
WARNGLOBALPARAM2	g_WarnParam2;
TJ_WARN				g_aWarnTjEx[MAX_TJWARN];
short				g_aWarnGpSet[MAXGPNUM]={0};
int					g_nWarnGpNum = 0;
PWARNINGCND			*g_pAbsCondition=NULL;
int					g_pAbsConditionNum = 0;
long				g_nUnitRadarFlag = 0;
int					g_nWarnRefresh=5;
long				g_nUnitDeepWarnFlag = 0;

//用来实现拖放的光标
HCURSOR		m_hCanPressB,m_hCanPressS,m_hCanPress,m_hCanDrop,m_hCanDrop_Overlap,m_hCannotDrop,m_hZoomCur,m_hZoomCur2,m_hZoomCur3,m_hVerSplit,m_hHorSplit,m_hMove,m_hCursor2,m_hDraw,m_hDrawMove,m_hDrawTurn,m_hFxtMove,m_hToBig,m_hToSmall;
HICON		m_hExpUpShow,m_hExpDownShow,m_hKLineShow,m_hExpUpShow_Sel,m_hExpDownShow_Sel,m_hKLineShow_Sel,m_hGBBQShow,m_hGGBWShow,m_hOneThunder,m_hMoreThunder,m_hMyThunder[6],m_hIcon_Pu[8],m_hFCIcon,m_hZstFlushIcon;
HICON		m_hRedUP,m_hRedDown,m_hGreenUP,m_hGreenDown,m_hIconUp,m_hIconDown,m_hIconPBuy[3],m_hIconPSell[3];

char g_cwAddrTopic[][10]=
{ 
	"黑龙江","新疆板块","吉林板块","甘肃板块","辽宁板块","青海板块","北京板块","陕西板块",
	"天津板块" ,"广西板块","河北板块","广东板块","河南板块","宁夏板块","山东板块","上海板块",
	"山西板块" ,"深圳板块","湖北板块","福建板块","湖南板块","江西板块","四川板块","安徽板块",
	"重庆板块" ,"江苏板块","云南板块","浙江板块","贵州板块","海南板块","西藏板块","内蒙板块"
};
char g_pDyBlockPy[TOTAL_DY_NUM][5]=
{
  "HLJ" ,"XJBK","JLBK","GSBK","LLBK","QHBK","BJBK","SXBK",
  "TJBK","GXBK","HBBK","GDBK","HNBK","LXBK","SDBK","SHBK",
  "SXBK","SZBK","HBBK","FJBK","HNBK","JXBK","SCBK","AHBK",
  "CQBK","JSBK","YNBK","ZJBK","GZBK","HNBK","XZBK","NMBK"
};
char g_cwHYstr[][9]=
{
"金融行业","其它类一","钢铁行业","家具行业","石油行业","公路桥梁",
"汽车制造","交通运输","医疗器械","酒店旅游","房地产业","商业百货",
"物资外贸","食品行业","纺织行业","电力行业","农林牧渔","传媒娱乐",
"化工行业","煤炭行业","建筑建材","水泥行业","家电行业","电子信息",
"综合行业","机械行业","化纤行业","农药化肥","电器行业","摩托车",
"开发区", "自行车","船舶制造","生物制药","电子器件","有色金属",
"酿酒行业","造纸行业","环保行业","陶瓷行业","服装鞋类","供水供气",
"发电设备","其它类二","纺织机械","印刷包装","塑料制品","玻璃行业",
"飞机制造","其它类三","仪器仪表","其它类四"
};
char   g_pHyBlockPy[TOTAL_HY_NUM][5]=
{ 
	"JRHY","SSJG","GTHY","JJHY","SYHY","GLQL",
	"QCZZ","JTYS","YLQX","JDLY","FDC","SYBH",
	"WZWM","SPGY","FZHY","DLHY","NLMY","CMYL",
	"HGHY","MTHY","JZJC","SNHY","JDHY","DZXX",
	"ZHHY","JXHY","HXHY","NYHF","DQHY","MTC",
	"KFQ" ,"ZXC" ,"CBZZ","SWZY","DZQJ","YSJS",
	"NJHY","ZZHY","HBHY","TCHY","FZXL","GSGQ",
	"FDSB","ZBHY","FZJX","YSBZ","SLZP","BLHY",
	"FJZZ","GGCL","YQYB","QTHY"
};

CLE_CONTABLE g_pCleKLine1[]=
{
	{"低档多条并列阳线",    KLINE_1001   ,KLINETL_LOW         ,CLETYPE_0},
	{"低档多条并列阴线",    KLINE_1002   ,KLINETL_LOW         ,CLETYPE_0},
	{"低档横盘中剑形线",    KLINE_1003   ,KLINETL_LOWDULL     ,CLETYPE_0},
	{"低档横盘中弓形线",    KLINE_1004   ,KLINETL_LOWDULL     ,CLETYPE_0},
	{"低档横盘下跳大阳线",  KLINE_1005   ,KLINETL_LOWDULL     ,CLETYPE_0},
	{"低档横盘上跳大阴线",  KLINE_1006   ,KLINETL_LOWDULL     ,CLETYPE_0},

	{"低档横盘末期弓形线",  KLINE_1007   ,KLINETL_LOWDULL3    ,CLETYPE_1},
	{"低档横盘该期剑形线",  KLINE_1008   ,KLINETL_LOWDULL3    ,CLETYPE_1},
	{"低档横盘末大阳突破",  KLINE_1009   ,KLINETL_LOWDULL3    ,CLETYPE_1},
	{"小幅盘落后大阳向上",  KLINE_1010   ,KLINETL_LOWSLOWDOWN ,CLETYPE_1},
	{"低档横盘末期红三兵",  KLINE_1011   ,KLINETL_LOWDULL     ,CLETYPE_1},

	{"低档急跌三大阴",      KLINE_1012   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"三空阴线",            KLINE_1013   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档急跌下跳十字星",  KLINE_1014   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档急跌下跳剑形线",  KLINE_1015   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档急跌下跳弓形线",  KLINE_1016   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档急跌后尽头阴线",  KLINE_1017   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档急跌后独阳线",    KLINE_1018   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档急跌后独阴线",    KLINE_1019   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档急跌后阳包阴",    KLINE_1020   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档急跌后包容阴线",  KLINE_1021   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档急跌后阴孕阳",    KLINE_1022   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档急跌后阴孕阴",    KLINE_1023   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档急跌阴孕十字星",  KLINE_1024   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档急跌阴孕弓形线",  KLINE_1025   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档急跌阴孕剑形线",  KLINE_1026   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档急跌后切入线",    KLINE_1027   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档急跌后阴夹阳",    KLINE_1028   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档急跌后上拉线",    KLINE_1029   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档急跌连拉剑形线",  KLINE_1030   ,KLINETL_LOWFASTDOWN ,CLETYPE_2},
	{"低档二次狙击性阳线",  KLINE_1031   ,KLINETL_LOW         ,CLETYPE_2},
	{""  ,0,0,0}
};

CLE_CONTABLE g_pCleKLine2[]=
{
	{"上档盘整后大阳突破",  KLINE_1032   ,KLINETL_HIGH        ,CLETYPE_3},
	{"跳高一字线后不补缺",  KLINE_1033   ,KLINETL_HIGH        ,CLETYPE_3},
	{"二次高开阳线",        KLINE_1034   ,KLINETL_HIGH        ,CLETYPE_3},
	{"跳空攀援线",          KLINE_1035   ,KLINETL_HIGH        ,CLETYPE_3},
	{"渐大三连阳",          KLINE_1036   ,KLINETL_UP          ,CLETYPE_3},
	{"上升中跳高十字星",    KLINE_1037   ,KLINETL_UP          ,CLETYPE_3},
	{"上升中跳高剑形线",    KLINE_1038   ,KLINETL_UP          ,CLETYPE_3},
	{"上升中跳高弓形线",    KLINE_1039   ,KLINETL_UP          ,CLETYPE_3},
	{"上升中小压迫线",      KLINE_1040   ,KLINETL_UP          ,CLETYPE_3},
	{"上升中伪阴线",        KLINE_1041   ,KLINETL_UP          ,CLETYPE_3},
	{"上升中阳包阴",        KLINE_1042   ,KLINETL_UP          ,CLETYPE_3},
	{"上升中归顺线",        KLINE_1043   ,KLINETL_UP          ,CLETYPE_3},
	{"上升中上拉线",        KLINE_1044   ,KLINETL_UP          ,CLETYPE_3},
	{"上升中阳孕阴",        KLINE_1045   ,KLINETL_UP          ,CLETYPE_3},

	{"上升中切入线",        KLINE_1046   ,KLINETL_UP          ,CLETYPE_4},
	{"上升中阳孕阳",        KLINE_1047   ,KLINETL_UP          ,CLETYPE_4},
	{"上升中并阳线",        KLINE_1048   ,KLINETL_UP          ,CLETYPE_4},
	{"上升中滑行线",        KLINE_1049   ,KLINETL_UP          ,CLETYPE_4},
	{"上升三连击",          KLINE_1050   ,KLINETL_UP          ,CLETYPE_4},
	{"上升三法",            KLINE_1051   ,KLINETL_UP          ,CLETYPE_4},
	{"上升跳空三法",        KLINE_1052   ,KLINETL_UP          ,CLETYPE_4},
	{"超越覆盖压迫线",      KLINE_1053   ,KLINETL_UP          ,CLETYPE_4},
	{"上升中途受阻红三兵",  KLINE_1054   ,KLINETL_UP          ,CLETYPE_4},
	{"上升中途思考红三兵",  KLINE_1055   ,KLINETL_UP          ,CLETYPE_4},
	{"上升中途阴夹阳",      KLINE_1056   ,KLINETL_UP          ,CLETYPE_4},
	{"上升中途阳包阳",      KLINE_1057   ,KLINETL_UP          ,CLETYPE_4},
	{"上升中途伪阳线",      KLINE_1058   ,KLINETL_UP          ,CLETYPE_4},
	{"",0,0,0}
};

CLE_CONTABLE g_pCleKLine3[]=
{
	{"高档多条并列阳线",    KLINE_1059   ,KLINETL_HIGH        ,CLETYPE_5},
	{"高档多根并列阴线",    KLINE_1060   ,KLINETL_HIGH        ,CLETYPE_5},
	{"反击滑行线",          KLINE_1061   ,KLINETL_HIGH        ,CLETYPE_5},
	{"高档横盘上跳大阴线",  KLINE_1062   ,KLINETL_HIGH        ,CLETYPE_5},
	{"高档横盘中弓形线",    KLINE_1063   ,KLINETL_HIGH        ,CLETYPE_5},
	{"高档横盘中剑形线",    KLINE_1064   ,KLINETL_HIGH        ,CLETYPE_5},
	{"滑行线接滑行线",      KLINE_1065   ,KLINETL_HIGH        ,CLETYPE_5},
	{"见顶三鸦",            KLINE_1066   ,KLINETL_HIGH        ,CLETYPE_5},

	{"高档盘整末剑形线",    KLINE_1067   ,KLINETL_HIGHDULL3   ,CLETYPE_6},
	{"高档盘整末大阴",     	KLINE_1068   ,KLINETL_HIGHDULL3   ,CLETYPE_6},
	{"高档盘整末黑三兵",    KLINE_1069   ,KLINETL_HIGHDULL3   ,CLETYPE_6},
	{"小幅盘升后大阴",      KLINE_1070   ,KLINETL_HIGH        ,CLETYPE_6},
	{"暴跌三鸦",            KLINE_1071   ,KLINETL_HIGH        ,CLETYPE_6},
	{"炮楼顶",              KLINE_1072   ,KLINETL_HIGH        ,CLETYPE_6},

	{"高档急升三大阳",      KLINE_1073   ,KLINETL_HIGHFASTUP  ,CLETYPE_7},
	{"三空阳线",            KLINE_1074   ,KLINETL_HIGHFASTUP  ,CLETYPE_7},
	{"高档弃十字星",        KLINE_1075   ,KLINETL_HIGH        ,CLETYPE_7},
	{"高档弃弓形线",        KLINE_1076   ,KLINETL_HIGH        ,CLETYPE_7},
	{"高档弃吊劲线",        KLINE_1077   ,KLINETL_HIGHFASTUP  ,CLETYPE_7},
	{"高档尽头阳线",        KLINE_1078   ,KLINETL_HIGH        ,CLETYPE_7},
	{"绝顶独阴线",          KLINE_1079   ,KLINETL_HIGH        ,CLETYPE_7},
	{"绝顶独阳线",          KLINE_1080   ,KLINETL_HIGH        ,CLETYPE_7},
	{"高档跳空滑行线",      KLINE_1081   ,KLINETL_HIGH        ,CLETYPE_7},
	{"高档阴包阳",          KLINE_1082   ,KLINETL_HIGH        ,CLETYPE_7},
	{"高档最后包容阳线",    KLINE_1083   ,KLINETL_HIGH        ,CLETYPE_7},
	{"高档阴包阴",          KLINE_1084   ,KLINETL_HIGH        ,CLETYPE_7},
	{"高档阴包剑形线",      KLINE_1085   ,KLINETL_HIGH        ,CLETYPE_7},
	{"高档阳孕阴",          KLINE_1086   ,KLINETL_HIGH        ,CLETYPE_7},
	{"高档阳孕阳",          KLINE_1087   ,KLINETL_HIGH        ,CLETYPE_7},
	{"高档阳孕十字星",      KLINE_1088   ,KLINETL_HIGH        ,CLETYPE_7},
	{"高档阳孕吊颈线",      KLINE_1089   ,KLINETL_HIGH        ,CLETYPE_7},
	{"高档覆盖线",          KLINE_1090   ,KLINETL_HIGH        ,CLETYPE_7},
	{"高档阳夹阴",          KLINE_1091   ,KLINETL_HIGH        ,CLETYPE_7},
	{"高档下拖线",          KLINE_1092   ,KLINETL_HIGH        ,CLETYPE_7},
	{"高档二次狙击阴线",    KLINE_1093   ,KLINETL_HIGH        ,CLETYPE_7},
	{"",0,0,0}
};

CLE_CONTABLE g_pCleKLine4[]=
{
	{"下档盘整大阴突破",    KLINE_1094   ,KLINETL_LOWDULL     ,CLETYPE_8},
	{"跳水一字线后不补缺",  KLINE_1095   ,KLINETL_LOWDULL     ,CLETYPE_8},
	{"二次跳空阴线",        KLINE_1096   ,KLINETL_DOWN        ,CLETYPE_8},
	{"跳空滑行线",          KLINE_1097   ,KLINETL_DOWN        ,CLETYPE_8},
	{"渐大三连阴",          KLINE_1098   ,KLINETL_DOWN        ,CLETYPE_8},
	{"下跌中跳水十字星",    KLINE_1099   ,KLINETL_DOWN        ,CLETYPE_8},
	{"下跌中跳水弓形线",    KLINE_1100   ,KLINETL_DOWN        ,CLETYPE_8},
	{"下跌中跳水剑形线",    KLINE_1101   ,KLINETL_DOWN        ,CLETYPE_8},
	{"下跌中小奉承线",      KLINE_1102   ,KLINETL_DOWN        ,CLETYPE_8},
	{"下跌中伪阳线",        KLINE_1103   ,KLINETL_DOWN        ,CLETYPE_8},
	{"下跌中阴包阳",        KLINE_1104   ,KLINETL_DOWN        ,CLETYPE_8},
	{"下跌中反叛线",        KLINE_1105   ,KLINETL_DOWN        ,CLETYPE_8},
	{"下跌中下拖线",        KLINE_1106   ,KLINETL_DOWN        ,CLETYPE_8},
	{"下跌中阴孕阳",        KLINE_1107   ,KLINETL_DOWN        ,CLETYPE_8},

	{"下跌中覆盖线",        KLINE_1108   ,KLINETL_DOWN        ,CLETYPE_9},
	{"下跌中阴孕阴",        KLINE_1109   ,KLINETL_DOWN        ,CLETYPE_9},
	{"下跌中并阳线",        KLINE_1110   ,KLINETL_DOWN        ,CLETYPE_9},
	{"下跌中并阴线",        KLINE_1111   ,KLINETL_DOWN        ,CLETYPE_9},
	{"下跌中攀援线",        KLINE_1112   ,KLINETL_DOWN        ,CLETYPE_9},
	{"下降三连击",          KLINE_1113   ,KLINETL_DOWN        ,CLETYPE_9},
	{"下降三法",            KLINE_1114   ,KLINETL_DOWN        ,CLETYPE_9},
	{"下降跳空三法",        KLINE_1115   ,KLINETL_DOWN        ,CLETYPE_9},
	{"超越切入奉承线",      KLINE_1116   ,KLINETL_DOWN        ,CLETYPE_9},
	{"下跌中受阻黑三兵",    KLINE_1117   ,KLINETL_DOWN        ,CLETYPE_9},
	{"下跌中思考黑三兵",    KLINE_1118   ,KLINETL_DOWN        ,CLETYPE_9},
	{"下跌中阳夹阴",        KLINE_1119   ,KLINETL_DOWN        ,CLETYPE_9},
	{"下跌中阴包阴",        KLINE_1120   ,KLINETL_DOWN        ,CLETYPE_9},
	{"下跌中伪阴线",        KLINE_1121   ,KLINETL_DOWN        ,CLETYPE_9},
	{"",0,0,0}
};

CLE_CONTABLE g_pCleJBM[] =
{
	{"小盘次新股",		JBMXG_1001,0,CLETYPE_10},
	{"中盘次新股",		JBMXG_1002,0,CLETYPE_10},
	{"大盘次新股",		JBMXG_1003,0,CLETYPE_10},
	{"小盘绩优股",		JBMXG_1004,0,CLETYPE_10},
	{"小盘绩平股",		JBMXG_1005,0,CLETYPE_10},
	{"小盘绩差股",		JBMXG_1006,0,CLETYPE_10},
	{"中盘绩优股",		JBMXG_1007,0,CLETYPE_10},
	{"中盘绩平股",		JBMXG_1008,0,CLETYPE_10},
	{"中盘绩差股",		JBMXG_1009,0,CLETYPE_10},
	{"大盘绩优股",		JBMXG_1010,0,CLETYPE_10},
	{"大盘绩平股",		JBMXG_1011,0,CLETYPE_10},
	{"大盘绩差股",		JBMXG_1012,0,CLETYPE_10},
	{"亏损股",			JBMXG_1013,0,CLETYPE_10},
	{"具股本扩张能力股",JBMXG_1014,0,CLETYPE_10},
	{"具高送配能力股"  ,JBMXG_1015,0,CLETYPE_10},
	{"具丰厚滚存利润股",JBMXG_1016,0,CLETYPE_10},
	{"",0,0,0}
};

CLE_CONTABLE g_pCleJSM[] =
{
	{"BIAS买入指示",	JSMXG_1001,0,CLETYPE_11}, 
	{"SAR买入指示",		JSMXG_1002,0,CLETYPE_11}, 
	{"KDJ买入指示",		JSMXG_1003,0,CLETYPE_11}, 
	{"RSI买入指示",		JSMXG_1004,0,CLETYPE_11}, 
	{"MACD买入指示",	JSMXG_1005,0,CLETYPE_11}, 
	{"MTM买入指示",		JSMXG_1006,0,CLETYPE_11}, 
	{"",0,0,0}
};

//K线趋势形态编码表
short TrendCodeTable[] =
{
	KLINETL_HIGH                    ,
	KLINETL_LOW                     ,
	KLINETL_TOSS                    ,
	KLINETL_DULL                    ,
	KLINETL_UP                      ,
	KLINETL_DOWN                    ,
	KLINETL_HIGHTOSS                ,
	KLINETL_LOWTOSS                 ,
	KLINETL_HIGHDULL                ,
	KLINETL_LOWDULL                 ,
	KLINETL_FASTUP                  ,
	KLINETL_FASTDOWN                ,
	KLINETL_SLOWUP                  ,
	KLINETL_SLOWDOWN                ,
	KLINETL_HIGHTOSS1               ,
	KLINETL_LOWTOSS1                ,
	KLINETL_HIGHTOSS2               ,
	KLINETL_LOWTOSS2                ,
	KLINETL_HIGHTOSS3               ,
	KLINETL_LOWTOSS3                ,
	KLINETL_HIGHDULL1               ,
	KLINETL_LOWDULL1                ,
	KLINETL_HIGHDULL2               ,
	KLINETL_LOWDULL2                ,
	KLINETL_HIGHDULL3               ,
	KLINETL_LOWDULL3                ,
	KLINETL_HIGHFASTUP              ,
	KLINETL_HIGHFASTDOWN            ,
	KLINETL_HIGHSLOWUP              ,
	KLINETL_HIGHSLOWDOWN            ,
	KLINETL_LOWFASTUP               ,
	KLINETL_LOWFASTDOWN             ,
	KLINETL_LOWSLOWUP               ,
	KLINETL_LOWSLOWDOWN             
};

CUSTXG_CONTABLE g_nCustCon[] = 
{
	{"总股本",C_ZGB,C_CUSTJBM,C_VT_FBJ,"万股"},
	{"流通股本",C_LTB,C_CUSTJBM,C_VT_FBJ,"万股"},

	{"B股/A股",C_BG,C_CUSTJBM,C_VT_FBJ,"万股"},
	{"H股",C_HG,C_CUSTJBM,C_VT_FBJ,"万股"},

	{"AB股总市值",C_ZSZ,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"流通值",C_LTSZ,C_CUSTJBM,C_VT_FBJ,"万元"},

	{"总资产",C_ZZC,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"流动资产",C_LDZC,C_CUSTJBM,C_VT_FBJ,"万元"},

	{"无形资产",C_wxzc,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"长期投资",C_cqtz,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"流动负债",C_ldfz,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"长期负债",C_cqfz,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"资本公积金",C_zbgjj,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"应收帐款",C_yszk,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"营业利润",C_yyly,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"投资收益",C_tzsy,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"经营现金流量",C_jyxjl,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"总现金流量",C_zxjl,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"存货",C_ch,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"净利润",C_jly,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"未分配利润",C_wfply,C_CUSTJBM,C_VT_FBJ,"万元"},

	{"净资产(股东权益)",C_JZC,C_CUSTJBM,C_VT_FBJ,"万元"},

	{"每股净资产",C_MGJZC,C_CUSTJBM,C_VT_FBJ,"元"},
	{"每股公积金",C_MGGJJ,C_CUSTJBM,C_VT_FBJ,"元"},
	{"每股未分配",C_MGWFP,C_CUSTJBM,C_VT_FBJ,"元"},
	{"每股收益",C_MGSY,C_CUSTJBM,C_VT_FBJ,"元"},

	{"股东权益比",C_GDQYB,C_CUSTJBM,C_VT_FBJ,"%"},
	{"资本公积金",C_ZBGJJ,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"主营收入",C_ZYSY,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"利润总额",C_LYZE,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"税后利润",C_SHLY,C_CUSTJBM,C_VT_FBJ,"万元"},
	{"净资产收益率",C_JYL,C_CUSTJBM,C_VT_FBJ,"%"},

	{"现价",C_NOW,C_CUSTHQ,C_VT_FBJ,"元"},
	{"最高",C_MAX,C_CUSTHQ,C_VT_FBJ,"元"},
	{"最低",C_MIN,C_CUSTHQ,C_VT_FBJ,"元"},
	{"今开",C_OPEN,C_CUSTHQ,C_VT_FBJ,"元"},
	{"昨收",C_CLOSE,C_CUSTHQ,C_VT_FBJ,"元"},
	{"总量",C_VOLUME,C_CUSTHQ,C_VT_FBJ,"手"},
	{"总金额",C_AMOUNT,C_CUSTHQ,C_VT_FBJ,"元"},

	{"涨幅",C_ZAF,C_CUSTHQ,C_VT_FBJ,"%"},
	{"振幅",C_ZEF,C_CUSTHQ,C_VT_FBJ,"%"},
	{"市盈(动)",C_SYL,C_CUSTHQ,C_VT_FBJ,"倍"},
	{"换手率",C_HSL,C_CUSTHQ,C_VT_FBJ,"%"},
	{"量比",C_LIANGB,C_CUSTHQ,C_VT_FBJ,"倍"},

	{"上市天数",C_SSNUM,C_CUSTOTHER,C_VT_NBJ,"天"},
	{"KDJ中的J值",C_KDJ_J,C_CUSTOTHER,C_VT_NBJ,""},
	{"RSI指标值",C_RSI,C_CUSTOTHER,C_VT_NBJ,""},
	{"DMI中的ADX值",C_DMI_ADX,C_CUSTOTHER,C_VT_NBJ,""},
	{"WR指标",C_WR,C_CUSTOTHER,C_VT_NBJ,""},
	{"VR指标值",C_VR,C_CUSTOTHER,C_VT_NBJ,""},
	{"",0,0,0,""}
};

char *BjCmpStr[] = {">","=","<"};

float g_my_rate=7.800f,g_gy_rate=1.000f;

int		ComCurMessageCode = -1;

char WeekInfo[][10]=
{
	"星期日","星期一","星期二","星期三","星期四","星期五","星期六"
};

char * strWeekInfo[8] = {"日","一","二","三","四","五","六",};

int	g_InfoDlgPosx=600,g_InfoDlgPosy=500;
BOOL g_bInfoMinimize=FALSE;

/////////////////////////////////////////////////////////////////////
DDrawLineTool	g_DrawLineTool;
int			g_nTxtFontSize	= 8;
COLORREF	g_nTxtColor = 0x00ffffff;

CustomFunc	g_CF;
FMLPACK		g_FmlPack[MAX_FMLPACK_NUM];
int			g_nFmlPack = 0;
FML2STK		g_Fml2Stk[MAX_BANDSTOCK_NUM];
int			g_nFml2Stk = 0;

Gdi			g_d;

int			g_nMulNum = 9,g_nNewMulNum=9;
BOOL		g_bDynaZoom = TRUE;
BOOL		g_bZBudFlag = TRUE;
BOOL		g_bTPAxis = FALSE;
BOOL		g_bLockRight = FALSE;
INFODLG		g_DlgInfo;
CBINFO		g_CbInfo;

UINT		g_uBigToolBarBtn[100]={0};
int			g_nBigToolBarBtn = 0;

UINT		g_uBigToolBarBtn_I[]={IDC_DEFTOOLGN1,IDC_DEFTOOLGN2,ID_SEPARATOR,IDC_DEFTOOLGN3,IDC_DEFTOOLGN4,IDC_DEFTOOLGN5,ID_SEPARATOR,IDC_DEFTOOLGN6,IDC_DEFTOOLGN7,IDC_DEFTOOLGN8,IDC_DEFTOOLGN9,IDC_NOTSUPPORT,ID_SEPARATOR,IDC_DEFTOOLGN11,IDC_DEFTOOLGN12,IDC_NOTSUPPORT,IDC_NOTSUPPORT,IDC_NOTSUPPORT,ID_SEPARATOR,IDC_NOTSUPPORT,IDC_NOTSUPPORT,IDC_DEFTOOLGN17,IDC_DEFTOOLGN18,IDC_DEFTOOLGN25,ID_SEPARATOR,IDC_NOTSUPPORT,IDC_NOTSUPPORT,IDC_NOTSUPPORT,IDC_NOTSUPPORT,ID_SEPARATOR,IDC_DEFTOOLGN23,IDC_NOTSUPPORT};
int			g_nBigToolBarBtn_I = chDIMOF(g_uBigToolBarBtn_I);

UINT		g_uBigToolBarBtn_II[]={IDC_DEFTOOLGN1,IDC_DEFTOOLGN2,ID_SEPARATOR,IDC_DEFTOOLGN3,IDC_DEFTOOLGN4,IDC_DEFTOOLGN5,ID_SEPARATOR,IDC_DEFTOOLGN6,IDC_DEFTOOLGN7,IDC_DEFTOOLGN8,IDC_DEFTOOLGN9,IDC_DEFTOOLGN10,ID_SEPARATOR,IDC_DEFTOOLGN11,IDC_DEFTOOLGN12,IDC_DEFTOOLGN13,IDC_DEFTOOLGN27,IDC_DEFTOOLGN28,ID_SEPARATOR,IDC_DEFTOOLGN15,IDC_DEFTOOLGN16,IDC_DEFTOOLGN17,IDC_DEFTOOLGN25,ID_SEPARATOR,IDC_DEFTOOLGN19,ID_SEPARATOR,IDC_DEFTOOLGN23,IDC_DEFTOOLGN24};
int			g_nBigToolBarBtn_II = chDIMOF(g_uBigToolBarBtn_II);

UINT		g_uBigToolBarBtn_III[]={IDC_DEFTOOLGN1,IDC_DEFTOOLGN2,ID_SEPARATOR,IDC_DEFTOOLGN3,IDC_DEFTOOLGN4,IDC_DEFTOOLGN5,ID_SEPARATOR,IDC_DEFTOOLGN6,IDC_DEFTOOLGN7,IDC_DEFTOOLGN8,IDC_DEFTOOLGN9,IDC_DEFTOOLGN10,ID_SEPARATOR,IDC_DEFTOOLGN11,IDC_DEFTOOLGN12,IDC_DEFTOOLGN13,IDC_DEFTOOLGN27,IDC_DEFTOOLGN28,ID_SEPARATOR,IDC_DEFTOOLGN15,IDC_DEFTOOLGN16,IDC_DEFTOOLGN17,IDC_DEFTOOLGN25,ID_SEPARATOR,IDC_DEFTOOLGN19,IDC_DEFTOOLGN20,ID_SEPARATOR,IDC_DEFTOOLGN23,IDC_DEFTOOLGN24};
int			g_nBigToolBarBtn_III = chDIMOF(g_uBigToolBarBtn_III);

UINT		g_uBigToolBarBtn_IIII[]={IDC_DEFTOOLGN1,IDC_DEFTOOLGN2,ID_SEPARATOR,IDC_DEFTOOLGN3,IDC_DEFTOOLGN4,IDC_DEFTOOLGN5,ID_SEPARATOR,IDC_DEFTOOLGN6,IDC_DEFTOOLGN7,IDC_DEFTOOLGN8,IDC_DEFTOOLGN9,IDC_DEFTOOLGN10,ID_SEPARATOR,IDC_DEFTOOLGN11,IDC_DEFTOOLGN12,IDC_DEFTOOLGN13,IDC_DEFTOOLGN14,IDC_DEFTOOLGN27,ID_SEPARATOR,IDC_DEFTOOLGN15,IDC_DEFTOOLGN16,IDC_DEFTOOLGN17,IDC_DEFTOOLGN18,IDC_DEFTOOLGN25,ID_SEPARATOR,IDC_DEFTOOLGN19,IDC_DEFTOOLGN20,IDC_DEFTOOLGN21,IDC_DEFTOOLGN22,ID_SEPARATOR,IDC_DEFTOOLGN23,IDC_DEFTOOLGN24};
int			g_nBigToolBarBtn_IIII = chDIMOF(g_uBigToolBarBtn_IIII);


#if defined(OEM_STAR) || defined(OEM_STAR_HM)
	UINT g_uFixedBtn[]={ID_SHOWHQ,ID_SHOWZST,ID_SHOWVIP,ID_SHOWHISTORY,IDC_GGZL,IDC_ZXTREE,IDC_BROKER,ID_FUNC_WT,ID_FORMULA,ID_TDXCHOICEGP,ID_ETF};
#else
	UINT g_uFixedBtn[]={ID_SYSPAD,ID_SHOWHQ,ID_SHOWZST,ID_SHOWVIP,ID_SHOWHISTORY,IDC_GGZL,IDC_ZXTREE,IDC_BROKER,ID_FUNC_WT,ID_FORMULA,ID_TDXCHOICEGP,ID_ETF,ID_SEPARATOR};
#endif

int			g_nFixedBtn = chDIMOF(g_uFixedBtn);

UINT		g_uTotalDynBtn[]={ID_MULUNIT,ID_TOOLS,ID_SWITCHLBMMLD,ID_HIDERIGHT,ID_TPAXIS,ID_PAGEUP,ID_PAGEDOWN,ID_RMREPORT,ID_ZBREPORT,ID_QRREPORT,ID_QJREPORT,ID_RIGHT_ALLWINDOW,
							  ID_OTHERJBM,ID_TDX_JBZL,ID_SELECTDATE,ID_LAST_DATE,ID_NEXT_DATE,ID_SELECTQJ,ID_INDEXRANK,ID_QR_OPENBASE,
							  ID_GPFL,ID_GPSORT,ID_GPZHSORT,ID_RIGHT_ADDBLOCK,ID_TXTFIND,ID_FINDRESULT,
							  ID_MANYDAYZST,ID_HXZBDLG,ID_FXT_MOVE,ID_SHOWPERIOD,ID_SHOWTQ,ID_HISZS,ID_SHOWWINNUM,
							  ID_WEB_BACK,ID_WEB_FORWARD,ID_WEB_STOP,ID_WEB_REFRESH,ID_PADLINKAGE,ID_BEGINSPLIT,ID_SYSPAD
							 };
int			g_nTotalDynBtn = chDIMOF(g_uTotalDynBtn);

UINT		g_uDynBtn[14][30] = 
{
	{ID_GPFL,ID_GPSORT,ID_GPZHSORT,ID_RIGHT_ADDBLOCK},
	{ID_MULUNIT,ID_SWITCHLBMMLD,ID_MANYDAYZST,ID_TPAXIS,ID_HIDERIGHT,ID_TOOLS,ID_PAGEUP,ID_PAGEDOWN},
	{ID_MULUNIT,ID_HXZBDLG,ID_SHOWPERIOD,ID_SHOWWINNUM,ID_SHOWTQ,ID_HISZS,ID_HIDERIGHT,ID_TOOLS,ID_FXT_MOVE,ID_PAGEUP,ID_PAGEDOWN},
	{ID_MULUNIT,ID_TPAXIS,ID_PAGEUP,ID_PAGEDOWN},
	{ID_PAGEUP,ID_PAGEDOWN},
	{ID_RMREPORT,ID_ZBREPORT,ID_QRREPORT,ID_QJREPORT},
	{ID_RMREPORT,ID_ZBREPORT,ID_QRREPORT,ID_QJREPORT,ID_SEPARATOR,ID_GPFL,ID_SELECTDATE,ID_LAST_DATE,ID_NEXT_DATE,ID_INDEXRANK},
	{ID_RMREPORT,ID_ZBREPORT,ID_QRREPORT,ID_QJREPORT,ID_SEPARATOR,ID_GPFL,ID_QR_OPENBASE},
	{ID_RMREPORT,ID_ZBREPORT,ID_QRREPORT,ID_QJREPORT,ID_SEPARATOR,ID_GPFL,ID_SELECTQJ},
	{ID_OTHERJBM,ID_TDX_JBZL,ID_TXTFIND,ID_FINDRESULT},
	{ID_RIGHT_ALLWINDOW,ID_TXTFIND,ID_FINDRESULT},
	{ID_TDX_JBZL},
	{ID_PADLINKAGE,ID_BEGINSPLIT},
	{ID_SEPARATOR}
};
int			g_nDynBtn[14]={4,8,11,4,2,4,10,7,7,4,3,1,2,1};

CString		g_TxtFindStr[MAX_ANDSTR];
int			g_nTxtFindNum=0;
CString		g_NewsFindStr[MAX_ANDSTR];
int			g_nNewsFindNum=0;
int			g_nTxtFindNo = -1;
int			g_nNewsFindNo = -1;
BOOL		g_nFindRadio = 0;

int			g_nFixedWidth = 198;
int			g_nFixedWidth2 = 214;

float		g_fProfitFee = 6.5;

//22种常用指标
char		g_strOftenUseZb[][ZBCODE_LEN] = {"趋势逆转","操盘高手","红绿信号","超级波段","大师解盘","发现庄迹","主力追踪","资金动向","底部建仓","机构控盘","金鹰踏庄","底部进攻","追踪黑马","顺势而为","金鹰猎庄","涨停追踪","金玉满堂","机构吸筹","捉庄散户","资金博弈"};
//char		g_strOftenUseZb[][ZBCODE_LEN] = {"MACD","DMI","DMA","FSL","TRIX","BRAR","CR","VR","OBV","ASI","EMV",
//								   "VOL","RSI","WR","SAR","KDJ","CCI","ROC","MTM","BOLL","PSY","MCST"};
int			g_nOftenUseZb=sizeof(g_strOftenUseZb)/sizeof(char[ZBCODE_LEN]);
//11个副图指标
char		g_strFavUseZb[][ZBCODE_LEN] = {"VOL","MACD","KDJ","DMI","DMA","FSL","TRIX","BRAR","CR","VR","OBV","ASI"};
char		g_strZbMain[ZBCODE_LEN] = "聚赢波段";
char		g_strLastExp[ZBCODE_LEN] = "";
char		g_strLastKLine[ZBCODE_LEN] = "";
char		g_strLastZBSort[ZBCODE_LEN] = "";

int			g_nMainZbFlag=0;	//0:K线 1:BAR线
BOOL		g_bHasXHidden=FALSE,g_bHasYHidden=TRUE;

long		g_nHligtNowVol = 1000; //手

char		g_strFontName[11][40] = {"Tahoma","Tahoma",KAI_FONT,"Tahoma",SONG_FONT,SONG_FONT,"Tahoma",KAI_FONT,KAI_FONT,"Tahoma","Tahoma"};
int			g_nFontSize[11] = {16,13,16,16,13,13,16,21,19,18,19};
int			g_nFontWeigth[11] = {FW_NORMAL,FW_NORMAL,FW_SEMIBOLD,FW_MEDIUM,FW_NORMAL,FW_NORMAL,FW_NORMAL,FW_BOLD,FW_BOLD,FW_BOLD,FW_BOLD};
BOOL		g_bBigFontGrid = FALSE;
CPoint		g_ZbDragPoint = CPoint(0,0);

TINDEXINFO	g_SpeclIndex[]=
{
	{	0 , SEPCLINE_TYPE  , "EQUVOL" ,  "等量图" , INDEX_STYLE , MainAble ,
	 	0 , {0},	/*aPara */ 	0 , {0},	/*aLine*/
		0 , 0 ,	{0},{0},"" ,//PassWord  
		NULL,NULL,	NULL,"1.等量图与压缩图的画法一样，但是，其肥大和狭窄的K 线不会\
互相重叠。\r\n\
2.等量图没有阴阳的分。\r\n\
3.等量图主要是观察趋势行进中，产生大成交量时股价的变化。",	1
	} , 
	{	1 , SEPCLINE_TYPE  , "EQUK" ,  "等量Ｋ线" , INDEX_STYLE , MainAble ,
	 	0 , {0},	/*aPara */ 	0 , {0},	/*aLine*/
		0 , 0 ,	{0},{0},"" ,//PassWord  
		NULL,NULL,	NULL,"1.等量K 线与压缩图的画法一样，但是，其肥大和狭窄的K 线不\
会互相重叠。\r\n\
2.等量K 线可以区分阴阳线。\r\n\
3.等量K 线主要是观察趋势行进中，产生大成交量时股价的变化",	1
	} , 
	{	2 , SEPCLINE_TYPE  , "CAN" ,  "压缩图" , INDEX_STYLE , MainAble ,
	 	0 , {0},	/*aPara */ 	0 , {0},	/*aLine*/
		0 , 0 ,	{0},{0},"" ,//PassWord  
		NULL,NULL,	NULL,"1.此为结合成交量的K 线画法，成交量越大，则K 线越宽；成交\
量越小，则K 线越窄。\r\n\
2.图形上横向宽度格值的距离不变，所以，当K 线因为大成交量\
而变得肥大时，会覆盖掉旁边狭窄的K 线，也就是说，成交量\
小的K 线会被忽略，只有较宽的K 线才具有意义。",	1
	} , 
	{	3 , SEPCLINE_TYPE  , "OX" ,  "圈叉图" , INDEX_STYLE , MainAble ,
	 	0 , {0},	/*aPara */ 	0 , {0},	/*aLine*/
		0 , 0 ,	{0},{0},"" ,//PassWord  
		NULL,NULL,	NULL,"1.图形由「○」变成「×」时，表示买进。\r\n\
2.图形由「×」变成「○」时，表示卖出。\r\n\
3.OX图上可以画趋势线，当○或×向上突破趋势线时，视为买进信号。\r\n\
4.OX图上可以画趋势线，当○或×向下跌破趋势线时，视为卖出信号。\r\n\
5.OX图可以预测未来涨跌幅。",	1
	} , 
	{	4 , SEPCLINE_TYPE  , "OXHL" ,  "高低圈叉图" , INDEX_STYLE , MainAble ,
	 	0 , {0},	/*aPara */ 	0 , {0},	/*aLine*/
		0 , 0 ,	{0},{0},"" ,//PassWord  
		NULL,NULL,	NULL,"1.图形由「○」变成「×」时，表示买进。\r\n\
2.图形由「×」变成「○」时，表示卖出。\r\n\
3.OX图上可以画趋势线，当○或×向上突破趋势线时，视为买进信号。\r\n\
4.OX图上可以画趋势线，当○或×向下跌破趋势线时，视为卖出信号。\r\n\
5.OX图可以预测未来涨跌幅。",	1
	} , 
	{	5 , SEPCLINE_TYPE  , "TBL" ,  "新三价线" , INDEX_STYLE , MainAble ,
	 	0 , {0},	/*aPara */ 	0 , {0},	/*aLine*/
		0 , 0 ,	{0},{0},"" ,//PassWord  
		NULL,NULL,	NULL,"1.新三价线由黑色变成红色时，视为买进信号。\r\n\
2.新三价线由红色变成黑色时，视为卖出信号。※新三价线如果持续上涨，\
则会由原新三价线向上延伸，因此，新三价线不会与K 线相对称，反之亦然。这一点请特别注意！",	1
	} , 
	{	6 , SEPCLINE_TYPE  , "TOW" ,  "宝塔线" , INDEX_STYLE , MainAble ,
	 	0 , {0},	/*aPara */ 	0 , {0},	/*aLine*/
		0 , 0 ,	{0},{0},"" ,//PassWord  
		NULL,NULL,	NULL,"1.宝塔线由黑色变成红色时，视为买进信号。\r\n\
2.宝塔线由红色变成黑色时，视为卖出信号。※宝塔线如果持续上涨，\
则会由原宝塔线向上延伸，因此，宝塔线不会与K 线相对称，反之亦然。这一点请特别注意！",	1
	} , 
};

int	g_nSpeclIndexNum = chDIMOF(g_SpeclIndex);
int	g_nZBWinNum = 3;

////////////////////////////////////////////////////////////////////////
//					TCPIP远程变量
////////////////////////////////////////////////////////////////////////

struct ManyHostInfo g_ManyHostInfo;
struct	CurrStockData_Code g_ZSHQData[10];

BOOL		g_bRiskPost = TRUE;		//是否有风险揭示
BOOL		g_bRestRightBtn = FALSE;	//辅助区采用右边
BOOL		g_bEveryFirst = FALSE;	//每次重进系统选择第一个主站
BOOL		g_bIsGuest = TRUE;
char		g_strLoginName[50];
char		g_strPassword[50];
char		g_L2UserName[50];
char		g_L2Password[50];
int			g_nL2Type = 0;
BOOL		g_bSaveUser = TRUE, g_bSavePass = TRUE;
TDXWHOST	g_HQHost;
TDXWHOST	g_VipHQHost,g_L2HQHost;
TDXWHOST	g_WTHost;
TDXWHOST	g_DSHost,g_SSDSHost;
PROXYINFO	g_ProxyInfo;

CString		g_HQIPStr="",g_HQNameStr="";
unsigned short g_HQPort=7709;

CString		g_DSHQIPStr="",g_DSHQNameStr="";
unsigned short g_DSHQPort=7721;

CString		g_HQTryOutIP="";
unsigned short g_HQTryOutPort=7709;

short	g_nMainID = 0;
short	g_nAssisID = 0;
char	g_nPriority = 1;
char	g_pReceive[ANSBUFFER_LEN+20],g_pReceive3[ANSBUFFER_LEN+20];
ANSHEADER g_AnsHeader,g_AnsHeader3;

short	g_nWideDataMainID = 0;	//用于宽带数据对齐的全局MainID,每次弹出一个MainID加1

CWnd *  g_pConnectWnd = NULL;
CSplashWnd * g_pSplashWnd = NULL;
CWnd *  g_pMainWin = NULL;
CWnd *  g_pHisDlg = NULL;
CWnd *  g_pWideDataDlg = NULL;
CWnd *	g_pDownLoadDlg = NULL;
CWnd *	g_pTickZstjDlg = NULL;
CWnd *  g_pDataSyncDlg = NULL;
CWnd *	g_pDSConnectWnd = NULL;
long	g_nLastQuerySeatID = 0;

class CBkData * g_pBkData = NULL;

int		g_nDataSyncType = DATASYNC_NULL;
CString	g_strDataSyncFile = "";

BOOL   g_bFirstStep=TRUE;
char   g_nUrgentNum = 1;


struct HostList g_HqHostList;

LOGININFO g_LoginInfo;

long  g_lRefreshHQSeconds = 0;		//上次刷新行情的时间
long  g_lRefreshStatHQSeconds = 0;	//上次刷新统计行情的时间
BOOL  g_bDowning = FALSE;			//是否正在下载
BOOL  g_bSelftPHing = FALSE;		//是否正在盘后处理
BOOL  g_bZnxg_Down = FALSE;			//是否下载过，如果是，要重新智能选股

long	g_lQJBgnDate = 0,g_lQJEndDate = 0;
long	g_nCurQJType = 0;
BOOL	g_bHisViewTQ = TRUE;
int		g_nToolBarPos = 0;
short   *g_Simhq2StkIndex = NULL;	//用于交换(请求完全行情的序号缓冲区)
short	g_nSimhq2Stk = 0;
BOOL	g_bBlockModified = FALSE;	//板块股是否修改过
short	g_nMineNum[MAXGPNUM];		//雷的数目，按股票序号存放

BOOL	g_bShowHqMine = TRUE;		//是否在行情表中显示信息雷

BOOL	g_bLongHighLine = TRUE;	//是否是长的高亮线

int		g_nNetCondition = 1;//上网环境

//////////////////////////传统版///////////////////////////
BOOL	g_bHasCaption = FALSE;
int		g_nMenuPeriod = PER_DAY;
int		g_nMenuSortType = 0;
int		g_nMul_WhichGPType = 6;
int		g_nMul_nBlockFlag = -1;
int		g_nMul_CurGP = -1;


int			g_nMenuStack[20];
int			g_nMenuStackTop = 0;

short g_awMenuStartNum[] =
{	0,11,
	11,19, 
	30,6, 
	36,8,
	44,12,
	56,22,
	78,26,
	104,6,
	110,20,
	130,14,
	144,13,
	157,14,
	171,14,
	185,9,
	194,4,
	198,5,
	203,4
};

char g_cwMenuTopic[][80] = 
{
 "大盘分析",
 "报价分析",
 "即时分析",
 "分析周期",
 "技术分析",
 "特别报道",
 "选择类别",
 "资讯系统",
 "特色功能",
 "系统工具",
 "选择类别",
 "选择类别",
 "选择类别",
 "专家系统",
 "选股器",
 "预警系统"
};

MENUITEMTDX  g_pMenu[] = {
//MENU_TOP
{"1、大盘分析",          MENU_DPFX    ,-1},
{"2、报价分析",          MENU_BJFX    ,-1},
{"3、即时分析",			 MENU_JSFX    ,-1},
{"4、技术分析",          MENU_PERIOD  ,-1},
{"5、多股同列",          MENU_MULGP	 ,-1},
{"6、特别报道",          MENU_TBBD    ,-1},
{"7、资讯系统",          MENU_XXM     ,-1},
{"8、交易委托",          MENU_LEAF	 ,L_WT},
{"9、特色功能",          MENU_SPEC	 ,-1},
{"10.系统工具",          MENU_OTHER   ,-1},
{"11.退出系统",          MENU_LEAF    ,L_QUIT},
//MENU_DPFX
{"1、上证１８０走势"      ,MENU_LEAF    ,L_DPFX_BASE+101},
{"2、上证综指走势(F3)"   ,MENU_LEAF    ,L_DPFX_BASE+102},
{"3、上证Ａ股走势"      ,MENU_LEAF    ,L_DPFX_BASE+103},
{"4、上证Ｂ股走势"      ,MENU_LEAF    ,L_DPFX_BASE+104},
{"5、上证ＡＤＬ"        ,MENU_LEAF    ,L_DPFX_BASE+105},
{"6、上证多空指标"      ,MENU_LEAF    ,L_DPFX_BASE+106},
{"7、上证５０走势"      ,MENU_LEAF    ,L_DPFX_BASE+114},
{"8、上证基金指数"		,MENU_LEAF	  ,L_DPFX_BASE+115},
{"9、上证红利指数"    ,MENU_LEAF	 ,L_DPFX_BASE+118},
{"10.深证１００走势"	   ,MENU_LEAF    ,L_DPFX_BASE+107},
{"11.深证成份走势(F4)"   ,MENU_LEAF    ,L_DPFX_BASE+108},
{"12.深证综指走势"      ,MENU_LEAF    ,L_DPFX_BASE+109},
{"13.深证Ａ股走势"      ,MENU_LEAF    ,L_DPFX_BASE+110},
{"14.深证Ｂ股走势"      ,MENU_LEAF    ,L_DPFX_BASE+111},
{"15.深证ＡＤＬ"        ,MENU_LEAF    ,L_DPFX_BASE+112},
{"16.深证多空指标"      ,MENU_LEAF    ,L_DPFX_BASE+113},
{"17.创业板指数"      ,MENU_LEAF    ,L_DPFX_BASE+116},
{"18.中小企业指数"    ,MENU_LEAF	 ,L_DPFX_BASE+117},
{"19.沪深３００走势"    ,MENU_LEAF	 ,L_DPFX_BASE+119},
//MENU_BJFX
{"1、自选股  "      ,   MENU_LEAF         ,L_BJFX_BASE+14},
{"2、分类股  "      ,   MENU_BJFX_LB      ,-1},
{"3、三板证券"      ,   MENU_LEAF         ,L_BJFX_BASE+16},
{"4、板块指数"      ,   MENU_LEAF         ,L_BJFX_BASE+17},
{"5、股票池  "      ,   MENU_LEAF         ,L_BJFX_BASE+15},
{"6、板块股  "      ,   MENU_BJFX_SELBLOCK,-1},
//MENU_JSFX
{"1、分时走势图          .501" ,MENU_LEAF,L_JSFX_ZST},
{"2、分时成交明细       .502" ,MENU_LEAF,L_JSFX_TICK},
{"3、分价表             .503" ,MENU_LEAF,L_JSFX_FJB},
{"4、逐笔成交明细       .504" ,MENU_LEAF,L_JSFX_FSB},
{"5、详细买卖盘         .505" ,MENU_LEAF,L_JSFX_MMP},
{"6、量比/多空指标      .506" ,MENU_LEAF,L_JSFX_LB},
{"7、买卖力道/涨跌率    .507" ,MENU_LEAF,L_JSFX_MMLD},
{"8、闪电走势图         .508" ,MENU_LEAF,L_JSFX_TICKT},

//MENU_PERIOD
{"1、日    线"            ,MENU_INDEX   ,PER_DAY},
{"2、周    线"            ,MENU_INDEX   ,PER_WEEK},
{"3、月    线"            ,MENU_INDEX   ,PER_MONTH},
{"4、多 日 线"            ,MENU_INDEX   ,PER_DAYN},
{"5、１ 分 钟"            ,MENU_INDEX   ,PER_MIN1},
{"6、５ 分 钟"            ,MENU_INDEX   ,PER_MIN5},
{"7、１５分钟"            ,MENU_INDEX   ,PER_MIN15},
{"8、３０分钟"            ,MENU_INDEX   ,PER_MIN30},
{"9、６０分钟"            ,MENU_INDEX   ,PER_HOUR},
{"10.多 分 钟"            ,MENU_INDEX   ,PER_MINN},
{"11.季    线"            ,MENU_INDEX   ,PER_SEASON},
{"12.年    线"            ,MENU_INDEX   ,PER_YEAR},
//MENU_INDEX
{"1、ＭＡＣＤ趋向指标"  ,MENU_LEAF  ,L_INDEX_BASE+301 },
{"2、ＤＭＩ趋向指标"    ,MENU_LEAF  ,L_INDEX_BASE+302 },
{"3、ＤＭＡ趋向指标"    ,MENU_LEAF  ,L_INDEX_BASE+303 },
{"4、ＥＸＰＭＡ趋向指标",MENU_LEAF  ,L_INDEX_BASE+304 },
{"5、ＴＲＩＸ趋向指标"  ,MENU_LEAF  ,L_INDEX_BASE+305 },
{"6、ＢＲＡＲ能量指标"  ,MENU_LEAF  ,L_INDEX_BASE+306 },
{"7、ＣＲ能量指标"      ,MENU_LEAF  ,L_INDEX_BASE+307 },
{"8、ＶＲ能量指标"      ,MENU_LEAF  ,L_INDEX_BASE+308 },
{"9、ＯＢＶ量价指标"    ,MENU_LEAF  ,L_INDEX_BASE+309 },
{"10.ＡＳＩ量价指标"    ,MENU_LEAF  ,L_INDEX_BASE+310 },
{"11.ＥＭＶ量价指标"    ,MENU_LEAF  ,L_INDEX_BASE+311 },
{"12.ＷＶＡＤ量价指标"  ,MENU_LEAF  ,L_INDEX_BASE+312 },
{"13.ＲＳＩ强弱指标"    ,MENU_LEAF  ,L_INDEX_BASE+313 },
{"14.Ｗ％Ｒ强弱指标"    ,MENU_LEAF  ,L_INDEX_BASE+314 },
{"15.ＳＡＲ停损指标"    ,MENU_LEAF  ,L_INDEX_BASE+315 },
{"16.ＫＤＪ超买超卖"    ,MENU_LEAF  ,L_INDEX_BASE+316 },
{"17.ＣＣＩ超买超卖"    ,MENU_LEAF  ,L_INDEX_BASE+317 },
{"18.ＲＯＣ超买超卖"    ,MENU_LEAF  ,L_INDEX_BASE+318 },
{"19.ＭＩＫＥ压力支撑"  ,MENU_LEAF  ,L_INDEX_BASE+319 },
{"20.ＢＯＬＬ压力支撑"  ,MENU_LEAF  ,L_INDEX_BASE+320 },
{"21.ＰＳＹ心理线"      ,MENU_LEAF  ,L_INDEX_BASE+321 },
{"22.ＣＤＰ逆势操作"	   ,MENU_LEAF  ,L_INDEX_BASE+322 },
//MENU_TBBD
//{"1、分类行情显示"      ,MENU_TBBD_LB     ,0},
{"1、振幅排名"          ,MENU_TBBD_LB     ,1},
{"2、今日强势股"		   ,MENU_TBBD_LB     ,2},
{"3、今日弱势股"        ,MENU_TBBD_LB     ,3},
{"4、成交量排名"        ,MENU_TBBD_LB     ,4},
{"5、总金额排名"      ,MENU_TBBD_LB     ,5},
{"6、量比排名"          ,MENU_TBBD_LB     ,6},
{"7、委比排名"          ,MENU_TBBD_LB     ,7},
{"8、现价排名"          ,MENU_TBBD_LB     ,8},
{"9、最新大笔排名"      ,MENU_TBBD_LB     ,9},
{"10.市盈率排名"        ,MENU_TBBD_LB     ,10},
{"11.换手率排名"        ,MENU_TBBD_LB     ,11},
{"12.净资产排名"        ,MENU_TBBD_LB     ,12},
{"13.总资产排名"        ,MENU_TBBD_LB     ,13},	
{"14.每股收益排名"      ,MENU_TBBD_LB     ,14},
{"15.净资产收益率排名"  ,MENU_TBBD_LB     ,15},
{"16.流通市值排名"      ,MENU_TBBD_LB     ,16},
{"17.流通股本排名"      ,MENU_TBBD_LB     ,17},
{"18.AB股总市值排名"    ,MENU_TBBD_LB     ,18},
{"19.总股本排名"        ,MENU_TBBD_LB     ,19},
{"20.每股公积金排名"    ,MENU_TBBD_LB     ,20},
{"21.活跃度排名"        ,MENU_TBBD_LB     ,21},
{"22.每笔均量排名"      ,MENU_TBBD_LB     ,22},
{"23.每笔换手排名"      ,MENU_TBBD_LB     ,23},
{"24.每股净资产排名"	,MENU_TBBD_LB     ,24},
{"25.市净率排名"		,MENU_TBBD_LB     ,25},
{"26.全景排行"          ,MENU_TBBD_LB     ,27},
//MENU_MULGP
{"1、自选股  "      ,   MENU_LEAF         ,L_MULGP_BASE+14},
{"2、分类股  "      ,   MENU_MULGP_LB     ,-1},
{"3、三板证券"      ,   MENU_LEAF         ,L_MULGP_BASE+16},
{"4、板块指数"      ,   MENU_LEAF         ,L_MULGP_BASE+17},
{"5、股票池  "      ,   MENU_LEAF         ,L_MULGP_BASE+15},
{"6、板块股  "      ,   MENU_MULGP_SELBLOCK     ,-1},
//MENU_XXM
{"1、远程消息       .701", MENU_LEAF  ,L_XXM_BASE+0},
{"2、远程消息       .702", MENU_LEAF  ,L_XXM_BASE+1},
{"3、远程消息       .703", MENU_LEAF  ,L_XXM_BASE+2},
{"4、远程消息       .704", MENU_LEAF  ,L_XXM_BASE+3},
{"5、远程消息	   .705", MENU_LEAF  ,L_XXM_BASE+4},
{"6、远程消息       .706", MENU_LEAF  ,L_XXM_BASE+5},
{"7、远程消息	   .707", MENU_LEAF  ,L_XXM_BASE+6},
{"8、远程消息	   .708", MENU_LEAF  ,L_XXM_BASE+7},
{"9、远程消息	   .709", MENU_LEAF  ,L_XXM_BASE+8},
{"10.远程消息	   .710", MENU_LEAF  ,L_XXM_BASE+9},
{"11.远程消息	   .711", MENU_LEAF  ,L_XXM_BASE+10},
{"12.远程消息	   .712", MENU_LEAF  ,L_XXM_BASE+11},
{"13.远程消息       .713", MENU_LEAF  ,L_XXM_BASE+12},
{"14.远程消息       .714", MENU_LEAF  ,L_XXM_BASE+13},
{"15.远程消息	   .715", MENU_LEAF  ,L_XXM_BASE+14},
{"16.远程消息       .716", MENU_LEAF  ,L_XXM_BASE+15},
{"17.远程消息	   .717", MENU_LEAF  ,L_XXM_BASE+16},
{"18.远程消息	   .718", MENU_LEAF  ,L_XXM_BASE+17},
{"19.远程消息	   .719", MENU_LEAF  ,L_XXM_BASE+18},
{"20.远程消息	   .720", MENU_LEAF  ,L_XXM_BASE+19},
//MENU_SPEC
{"1、热门板块分析",	MENU_LEAF  ,L_RMREPORT},
{"2、历史行情报表",	 MENU_LEAF,L_ZBREPORT},
{"3、强弱分析报表",   MENU_LEAF,L_QRREPORT},
{"4、区间涨跌幅度",   MENU_LEAF,L_QJTYPE1},
{"5、区间换手排名",   MENU_LEAF,L_QJTYPE2},
{"6、区间量变幅度",   MENU_LEAF,L_QJTYPE3},
{"7、区间振荡幅度",   MENU_LEAF,L_QJTYPE4},
{"8、专家系统",		MENU_EXPSYS  ,-1},
{"9、选股器",		MENU_CHOICESYS  ,-1},
{"10.预警系统",		MENU_WARNSYS  ,-1},
{"11.理财金算盘",	MENU_LEAF  ,L_PROFIT},
{"12.股票组合计算",	MENU_LEAF  ,L_BLOCKCALC},
{"13.数据挖掘参考",	MENU_LEAF  ,L_DATADIG},
{"14.个人理财",		MENU_LEAF  ,L_INVEST},
//MENU_OTHER
{"1、系统设置",		MENU_LEAF  ,L_OPTION},
{"2、设置用户板块",	MENU_LEAF  ,L_BLOCKOPTION},
{"3、通讯设置",		MENU_LEAF  ,L_CONNECTCFG},
{"4、盘后数据下载",	MENU_LEAF  ,L_DOWNLOAD},
{"5、数据维护工具",	MENU_LEAF  ,L_NODETOOL},
{"6、连接行情主站",	MENU_LEAF  ,L_CONNECT},
{"7、断开行情主站",	MENU_LEAF  ,L_DISCONNECT},
{"8、连接资讯主站",	MENU_LEAF  ,L_CONNECTINFO},
{"9、断开资讯主站",	MENU_LEAF  ,L_DISCONNECTINFO},
{"10.系统备忘录",	MENU_LEAF  ,L_COOLINFO},
{"11.自动升级",		MENU_LEAF  ,L_AUTOUP},
{"12.帮助说明书",	MENU_LEAF  ,L_HELP_FINDER},
{"13.用户论坛",		MENU_LEAF  ,L_FORUM},
//MENU_BJFX_LB
{"1、上证Ａ股      ",   MENU_LEAF,L_BJFX_BASE+0},
{"2、上证Ｂ股      ",   MENU_LEAF,L_BJFX_BASE+1},
{"3、深证Ａ股      ",   MENU_LEAF,L_BJFX_BASE+2},
{"4、深证Ｂ股      ",   MENU_LEAF,L_BJFX_BASE+3},
{"5、上证债券      ",   MENU_LEAF,L_BJFX_BASE+4},
{"6、深证债券      ",   MENU_LEAF,L_BJFX_BASE+5},
{"7、深沪Ａ股      ",   MENU_LEAF,L_BJFX_BASE+6},
{"8、深沪Ｂ股      ",   MENU_LEAF,L_BJFX_BASE+7},
{"9、深沪债券      ",   MENU_LEAF,L_BJFX_BASE+8},
{"10.深沪基金      ",   MENU_LEAF,L_BJFX_BASE+9},
{"11.所有商品      ",   MENU_LEAF,L_BJFX_BASE+10},
{"12.所有指数      ",   MENU_LEAF,L_BJFX_BASE+11},
{"13.中小企业      ",   MENU_LEAF,L_BJFX_BASE+12},
{"14.深沪权证      ",   MENU_LEAF,L_BJFX_BASE+13},
{"15.创业板        ",   MENU_LEAF,L_BJFX_BASE+14},
//MENU_MULGP_LB
{"1、上证Ａ股      ",   MENU_LEAF,L_MULGP_BASE+0},
{"2、上证Ｂ股      ",   MENU_LEAF,L_MULGP_BASE+1},
{"3、深证Ａ股      ",   MENU_LEAF,L_MULGP_BASE+2},
{"4、深证Ｂ股      ",   MENU_LEAF,L_MULGP_BASE+3},
{"5、上证债券      ",   MENU_LEAF,L_MULGP_BASE+4},
{"6、深证债券      ",   MENU_LEAF,L_MULGP_BASE+5},
{"7、深沪Ａ股      ",   MENU_LEAF,L_MULGP_BASE+6},
{"8、深沪Ｂ股      ",   MENU_LEAF,L_MULGP_BASE+7},
{"9、深沪债券      ",   MENU_LEAF,L_MULGP_BASE+8},
{"10.深沪基金      ",   MENU_LEAF,L_MULGP_BASE+9},
{"11.所有商品      ",   MENU_LEAF,L_MULGP_BASE+10},
{"12.所有指数      ",   MENU_LEAF,L_MULGP_BASE+11},
{"13.中小企业      ",   MENU_LEAF,L_MULGP_BASE+12},
{"14.深沪权证      ",   MENU_LEAF,L_MULGP_BASE+13},
{"15.创业板        ",   MENU_LEAF,L_MULGP_BASE+14},
//MENU_TBBD_LB
{"1、上证Ａ股      ",   MENU_LEAF,L_TBBD_BASE+0},
{"2、上证Ｂ股      ",   MENU_LEAF,L_TBBD_BASE+1},
{"3、深证Ａ股      ",   MENU_LEAF,L_TBBD_BASE+2},
{"4、深证Ｂ股      ",   MENU_LEAF,L_TBBD_BASE+3},
{"5、上证债券      ",   MENU_LEAF,L_TBBD_BASE+4},
{"6、深证债券      ",   MENU_LEAF,L_TBBD_BASE+5},
{"7、深沪Ａ股      ",   MENU_LEAF,L_TBBD_BASE+6},
{"8、深沪Ｂ股      ",   MENU_LEAF,L_TBBD_BASE+7},
{"9、深沪债券      ",   MENU_LEAF,L_TBBD_BASE+8},
//MENU_EXPSYS
{"1、公式管理器",	 MENU_LEAF,L_FORMULA},
{"2、条件选股测试",   MENU_LEAF,L_TJTEST},
{"3、专家系统参数",   MENU_LEAF,L_JYTEST},
{"4、探索最佳指标",   MENU_LEAF,L_TESTINDEX},
//MENU_CHOICESYS
{"1、条件选股",	 MENU_LEAF,L_CONDITIONCHOICE},
{"2、定制选股",   MENU_LEAF,L_SIM_CHOICE},
{"3、智能选股",   MENU_LEAF,L_CLECHOICE},
{"4、插件选股",   MENU_LEAF,L_PLUGIN},
{"5、综合选股",   MENU_LEAF,L_SELGP},
//MENU_WARNSYS
{"1、市场雷达设置",   MENU_LEAF,L_ALLWARNCFG},
{"2、市场雷达列表",	 MENU_LEAF,L_ALLWARNGP},
{"3、条件预警设置",   MENU_LEAF,L_TJWARNCFG},
{"4、条件预警列表",	 MENU_LEAF,L_TJWARNGP},

{"",0,0}
};

int	 g_nOverlapNum = 0;								//走势图叠加的个数
MemStkInfo * g_pOverlapStkInfo[3];					//最多允许叠加三只股票
BOOL g_bManyDayZST = FALSE,g_bHisManyDayZST = FALSE;
int	 g_nZSTManyDay = 1,g_nHisZSTManyDay = 1;		//多日走势图的天数,最多允许MAX_ZST_MANYDAY天,不包括当天的

BOOL g_nTmpDirectConnectHq = FALSE;

BOOL g_bShowVersoin = TRUE;
char g_strLogoAdvert[200];
BOOL g_bDiffVolMode = 0;			//0:分色模式1 1:分色模式2

int g_nTQFlag = 1;	//添权类型	0--无复权 1--前复权 2--后复权 3--定点复权
long g_nPartTq = 0;
long g_nTqStart = 20060101;
int	g_nMainAxis = 0; //主图的坐标
//主图历史数据模式
const MAINLINE g_MainLineName[] = {{"K","K线(空心阳线)"},{"KR","K线(实心阳线)"},{"BAR","美国线"},{"CLOSE","收盘线"},
	{"SCLOSE","收盘站线"},{"MTOW","宝塔线"}};
const int g_MainLineNum = 6;

WORD    g_virtkey;
WORD    g_comkey;
BOOL    g_hideinfo=FALSE;
BOOL	g_bBossKey=FALSE;

int		g_nMenuMode = 0; //0:传统风格 1:XP风格
BOOL	g_bExtendXPStyle = 0; //是否是增强型的XP风格 
BOOL	g_bEnableClassXP = FALSE;
BOOL	g_bClassXP = FALSE;	//

BOOL	g_bMin3Tray = FALSE;

long  g_URLEndTime = 0;		//URL结束时间
char  g_URLTitle[81];			//URL串的标题
char  g_URL[81];				//URL

float g_fJiangEnStep = 0.1f;
int   g_nJiangEnMode = 0;		//0:阻力位 1:支撑位
BOOL  g_nCBSwitch = 1;			//移动筹码分析的类型

unsigned long  g_LoginServerSeconds=0,g_LoginLocalSeconds=0,g_LoginServerDate=0;
long  g_LoginDSServerSubLocal=0;

int	  g_nWebPageNum = 0;	//最多显示40个
char  g_strWebPageStr[40][50];
char  g_strWebPageURL[40][150];
char  g_strMainWebTitle[100];

int	  g_nWebPageNum2 = 0;	//最多显示20个
char  g_strWebPageStr2[20][50];
char  g_strWebPageURL2[20][150];
char  g_strMainWebTitle2[100];

struct tag_VitalBtn g_VitalBtn;

//由服务器端控制的功能
BOOL  g_bNo003InAG = TRUE;
BOOL  g_bSupportETF = TRUE;
BOOL  g_bSupportLevel2 = FALSE;
BOOL  g_bSupportDownMin5 = TRUE;
BOOL  g_bG2DT = FALSE;
BOOL  g_bUseSpecVolumeUnit = FALSE;	//是否使用特别的量处理
//OEM控制的功能
BOOL	g_bHasInfo = FALSE;
BOOL	g_bHasUserHost = TRUE;
CString g_strCheckUserURL = "";

BOOL	g_bHasDS = FALSE;
BOOL	g_bZBEditable = FALSE;

long	g_nDSAutoStopMinute = 0;
long	g_nDSAutoStopTimer = 0;
long	g_nDSCheckHostMinute = 0;

BOOL  g_bLongDSName = FALSE;
BOOL  g_bHasWebInfo = FALSE;
BOOL  g_bHasEmbWT = FALSE;
BOOL  g_bUserHostEverConnect = FALSE;
BOOL  g_bHasAdvHQVersion = FALSE;
int	  g_nPersonalVersion=0;
BOOL  g_bJRSConnected=FALSE;	//是否与用户服务器(前导或VIP)保持连接
BOOL  g_bHasTICQ = FALSE;
BOOL  g_bAutoConnectTICQ = FALSE;
BOOL  g_bTQAsBroker = FALSE;

char *SwitchUnit_ZstZS[3] = {"成交额","多空指标","涨跌率"};
char *SwitchUnit_ZstGP[4] = {"成交量","量比","买卖力道","竞价图"};
char *SwitchUnit_ZstL2[11] = {"成交量","量比","买卖力道","竞价图","总买总卖","净挂净撤","撤单累计","主买主卖","大单动向","涨跌动因","大单差分"};

char *SwitchUnitStr[4] = {"资","笔","异","价"};
char *SwitchUnitStr_Tips[4] = {"资金动向","分时成交明细","主力异动","分价表"};
int	  SwitchUnitNum = 4;
char *SwitchUnitStr_Mid[7] = {"队列","分布","龙虎","即撤","总撤","总委","行业"};
char *SwitchUnitStr_Mid_Tips[7] = {"最有可能成交的前50个委托单队列信息\r\n双击内容区放大,再次双击返回","当前买卖盘委托分布","针对逐笔成交的各类成交统计信息","当前单笔数量最大的10笔买/卖撤单\r\n双击内容区放大,再次双击返回","撤消买入/卖出委托累计数量最多的前10只股票\r\n双击内容区放大,再次双击返回","委托买入/卖出数量最多的前10只证券\r\n双击内容区放大,再次双击返回","行业成额排名,行业内成交金额前5位个股\r\n双击内容区放大,再次双击返回"};
int	  SwitchUnitNum_Mid = 7;

int   RsaBitsInt[5] = {128,256,512,768,1024};
int   KeyLenInt[5] =  {8,16,32,64,128};

//////////////////////商业智能平台的变量/////////////////////////////////

BOOL  g_bHasF10Cfg = TRUE;

BOOL    g_bConnectInfoStatus = FALSE;
struct f10cfg_title g_F10Caption[MAX_F10CAPTION];
struct new_f10cfg_title g_NewF10Caption[MAX_F10CAPTION];
char	g_F10CaptionNum = 0;
BOOL	g_bNLB = TRUE;
BOOL	g_bNLBOnLogo = TRUE;
struct NLBInfo g_NLBInfo[150];
BOOL	g_bWtUserVerifyStyle = 0;
BOOL	g_bHexinWtOnlyTXMM = FALSE;
short	g_WtVerifyIPNum = 0;
struct IpInfo *g_WTVerifyIP = NULL;
struct VipVerify_Info g_VipVerify_Info;
BOOL g_bAuthSuccess = FALSE;
BOOL g_bWTEverLogin = FALSE;	//交易是否曾经登录过

BOOL g_bHintSwitch = TRUE;
BOOL g_bRandSwitch = FALSE;
BOOL g_bNetErrorReLogin = TRUE;

struct ETInfo			g_ET;
struct Global_Habit		g_GH;
struct Global_Cfg		g_GC;
struct Global_EmbOem	g_OEM;
struct Global_Url		g_Url;
struct Global_SysInfo	g_SysInfo;
struct Global_DSCfg		g_DS;
struct JyCookie_Info	g_JyInfo;
struct AntiHack_Cfg		g_AntiHack;
struct NetParam_Cfg		g_NetParam;

BOOL g_bForbidOtherHost=FALSE;
BOOL g_bUseKList=FALSE;	//按F1是否进入日线报表
CString g_PersonalPath;
////////////////////集成平台////////////////////////////////

CHATAREA g_ChatArea;
	
long	g_nTreePos[20];
long	g_nRestPos[20];

char	g_UserHostIP[50]={0},g_BakUserHostIP[50]={0},g_BakUserHostIP2[50]={0};
unsigned short g_nUserHostPort = 7715,g_nBakUserHostPort = 7715,g_nBakUserHostPort2 = 7715;

char g_FHostIP[50]={0};
unsigned short g_nFHostPort = 7719;

char	g_UserHostQSID[11]={0};		//	非常连接用户管理服务器的券商ID
CString	g_strTdxVerifyFileName="winwt.exe";

char	g_MngRight = 0;				//	管理权限,0＝备用 1＝普通用户 2＝VIP用户 3＝核心用户 4＝研究员 5＝超级用户/总编
char	g_UserType = 0;				//	用户类型,0=客户 1＝经纪人 2＝即是客户又是经纪人
char	g_strUserType[9] = {0};		//  用户类型串
char	g_strUserCharacter[9] = {0};//  用户性质串
char	g_strUserLimits[20] = {0};	//  用户权限串

DWORD	g_CategoryInfo = 0;			//	单独的信息类型,为一位段结构
char	g_RightStr[255] = {0};		//	用户权限串
long	g_ExpireDate = 0;
long 	g_UrgentNum = 0;

CString SwitchCurGPCode = "600839";
short	SwitchCurSetCode = SH;

BOOL	g_bActTogether = TRUE;
BOOL	g_bFxtShowInfoDlg = FALSE;
BOOL	g_bFxtShowTab = FALSE;
BOOL	g_bTranparentTab = TRUE;

int l_YYBNum=0;
int l_PrimaryYYB=0;
struct tag_YYBInfo *l_pYYBInfo = NULL;

BOOL g_b2000XPSys=TRUE;
BOOL g_bUseBigJBM=FALSE;
BOOL g_bCanUserUrgent = FALSE;
BOOL g_bHasBakHostFile = FALSE;
BOOL g_bHasZXQY=TRUE;	//服务器端是否中小企业板
BOOL g_bCanDown = TRUE;
BOOL g_bLastRqDown = FALSE;
BOOL g_nReConnectNum=0; //重连接
int  g_nLogoTimeOut=8;	//登录过程中的超时秒数
int  g_nCusWebPageNum=0;
CUSWEBPAGE g_CusWebPage[MAX_CUSWEB_NUM];
CUSWEBPAGE g_F11WebPage;
float	l_aRatio[RATIONNUM]={0.0};//存放不同图的比例关系

/****************************************************
//	第三方市场的全局变量
****************************************************/
char			g_pNbbs_DS[10]={0};
SRVINFOANS		g_iDsSrvInfo={"",0,0,0,0,0};	//	读取的主站信息
BOOL			g_bDSNeedIdx = FALSE;
BOOL			g_bDSNeedTimer = FALSE;
int				g_GuiMarketNum=0;
int				g_LocalGuiMarketNum=0;
GUIMARKET		g_GuiMarketSet[MAX_MKTGUI_NUM];
short			g_nDSMarketNum = 0;
MKTINFO			g_pDSMarket[MAX_MKT_NUM]={0};
  
////////////////////////////////////////////////////////////////////////
CGNPlugin	g_GNPlug;

GRIDCATITEM	g_SysDefGridCat[12]=
{
	{ABDEF_ITEM, "自选股", WM_COMMAND, ID_RIGHT_TYPE15, 0, FALSE, 15, 0},
	{ABDEF_ITEM, "沪深Ａ股", WM_COMMAND, ID_RIGHT_TYPE6, 0, FALSE, 6, 0},
	{ABDEF_ITEM, "中小企业", WM_COMMAND, ID_RIGHT_TYPE12, 0, FALSE, 12, 0},
	{ABDEF_ITEM, "创业板", WM_COMMAND, ID_RIGHT_TYPE14, 0, FALSE, 14, 0},
	{ABDEF_ITEM, "沪深权证", WM_COMMAND, ID_RIGHT_TYPE13, 0, FALSE, 13, 0},
	{ABDEF_ITEM, "沪深基金", WM_COMMAND, ID_RIGHT_TYPE9, 0, FALSE, 9, 0},
	{ABDEF_ITEM, "沪深债券", WM_COMMAND, ID_RIGHT_TYPE8, 0, FALSE, 8, 0},
	{ABDEF_ITEM, "沪深Ｂ股", WM_COMMAND, ID_RIGHT_TYPE7, 0, FALSE, 7, 0},
	{ABDEF_ITEM, "沪深指数", WM_COMMAND, ID_RIGHT_TYPE11, 0, FALSE, 11, 0},
//	{ABDEF_ITEM, "板块指数", WM_COMMAND, ID_RIGHT_TYPE20, 0, FALSE, TYPE_LC, 0},
	{ABDEF_ITEM, "地区", WM_COMMAND, IDR_LONELY_MENU, 16, TRUE, -1, 0},
	{ABDEF_ITEM, "行业", WM_COMMAND, IDR_LONELY_MENU, -5, TRUE, -1, 0},
	{ABDEF_ITEM, "条件选股", WM_COMMAND, IDR_LONELY_MENU, -2, TRUE, -1, 0},
};

GRIDCATITEM	g_SysDefGridCat_Com2[12]=
{
	{ABDEF_ITEM, "自选", WM_COMMAND, ID_RIGHT_TYPE15, 0, FALSE, 15, 0},
	{ABDEF_ITEM, "沪深", WM_COMMAND, ID_RIGHT_TYPE6, 0, FALSE, 6, 0},
	{ABDEF_ITEM, "中小", WM_COMMAND, ID_RIGHT_TYPE12, 0, FALSE, 12, 0},
	{ABDEF_ITEM, "创业", WM_COMMAND, ID_RIGHT_TYPE14, 0, FALSE, 14, 0},
	{ABDEF_ITEM, "权证", WM_COMMAND, ID_RIGHT_TYPE13, 0, FALSE, 13, 0},
	{ABDEF_ITEM, "基金", WM_COMMAND, ID_RIGHT_TYPE9, 0, FALSE, 9, 0},
	{ABDEF_ITEM, "债券", WM_COMMAND, ID_RIGHT_TYPE8, 0, FALSE, 8, 0},
	{ABDEF_ITEM, "Ｂ股", WM_COMMAND, ID_RIGHT_TYPE7, 0, FALSE, 7, 0},
	{ABDEF_ITEM, "指数", WM_COMMAND, ID_RIGHT_TYPE11, 0, FALSE, 11, 0},
	{ABDEF_ITEM, "地区", WM_COMMAND, IDR_LONELY_MENU, 16, TRUE, -1, 0},
	{ABDEF_ITEM, "行业", WM_COMMAND, IDR_LONELY_MENU, -5, TRUE, -1, 0},
	{ABDEF_ITEM, "条件", WM_COMMAND, IDR_LONELY_MENU, -2, TRUE, -1, 0},
};

GRIDCATITEM	g_SysDefGridCat_All[16]=
{
	{ABDEF_ITEM, "Ａ股", WM_COMMAND, ID_RIGHT_TYPE6, 0, FALSE, 6, 0},
	{ABDEF_ITEM, "中小", WM_COMMAND, ID_RIGHT_TYPE12, 0, FALSE, 12, 0},
	{ABDEF_ITEM, "创业", WM_COMMAND, ID_RIGHT_TYPE14, 0, FALSE, 14, 0},
	{ABDEF_ITEM, "Ｂ股", WM_COMMAND, ID_RIGHT_TYPE7, 0, FALSE, 7, 0},
	{ABDEF_ITEM, "权证", WM_COMMAND, ID_RIGHT_TYPE13, 0, FALSE, 13, 0},
	{ABDEF_ITEM, "基金", WM_COMMAND, ID_RIGHT_TYPE9, 0, FALSE, 9, 0},
	{ABDEF_ITEM, "债券", WM_COMMAND, ID_RIGHT_TYPE8, 0, FALSE, 8, 0},
	{ABDEF_ITEM, "板指", WM_COMMAND, ID_RIGHT_TYPE17, 0, FALSE, 17, 0},	
	{ABDEF_ITEM, "定制", WM_COMMAND, ID_RIGHT_TYPE20, 0, FALSE, 20, 0},	
	{ABDEF_ITEM, "自选", WM_COMMAND, ID_RIGHT_TYPE15, 0, FALSE, 15, 0},
	{ABDEF_ITEM, "地区", WM_COMMAND, IDR_LONELY_MENU, 16, TRUE, -1, 0},
	{ABDEF_ITEM, "行业", WM_COMMAND, IDR_LONELY_MENU, 17, TRUE, -1, 0},
	{ABDEF_ITEM, "证监会行业", WM_COMMAND, IDR_LONELY_MENU, -5, TRUE, -1, 0},
	{ABDEF_ITEM, "概念", WM_COMMAND, IDR_LONELY_MENU, -1, TRUE, -1, 0},
	{ABDEF_ITEM, "组合", WM_COMMAND, IDR_LONELY_MENU, -2, TRUE, -1, 0},
	{ABDEF_ITEM, "自定", WM_COMMAND, IDR_LONELY_MENU, -3, TRUE, -1, 0}
};

long g_nSysDefGridCatNum=12,g_nSysDefGridCat_All=15;

vector<GRIDCATITEM>	g_CustomGridCat;

SPECMENUCOLOR	g_SpecMenuColor[] = 
{
	{ID_KQS,"",RGB(255,0,0)},
	{IDC_QZSDS,"",RGB(255,0,0)},
	{ID_GPGRADE_DLG,"",RGB(255,0,0)},
	{IDC_F11WEBPAGE,"",RGB(255,0,0)},
	{IDC_HKEYPROGRAM,"",RGB(255,0,0)},	
	{ID_HKSEATQUERY,"",RGB(255,0,0)},	
	{ID_MARK_ADD7,"",RGB(255,0,0)},	
	{ID_508,"",RGB(255,0,0)},
	{ID_RIGHT_ADDZXG,"",RGB(255,0,0)},

	{IDM_BUYDS,"",RGB(192,0,0)},

	{0,"定制版面",RGB(255,0,0)},
	{0,"",RGB(255,0,0)}
};

int g_nSpecMenuColor = chDIMOF(g_SpecMenuColor);

struct ETF_Info g_ETFInfo[]=
{
	{SH, "510050","上证50  ETF","510051","000016"},
	{SH, "510180","上证180 ETF","510181","000010"},
	{SH, "510880","上证红利ETF","510881","000015"},
	{SZ, "159901","深证100 ETF","159901","399004"},
	{SZ, "159902","中小板  ETF","159902",ZXB_DPCODE}
};
int g_nETFInfoNum = chDIMOF(g_ETFInfo);

Status_GP	g_StatusGP[3];

long		g_nNetInCount = 0;
long		g_nNetOutCount = 0;

vector<PWARNINGCND> g_aSelfWarnGp;

//报表汇总权重类型
short		g_nHZQZFlag = 0;		//0-平均 1-流通盘 2-总股本

int			g_nZHStyle = 1;
int			g_nZBReportSortIndex = 2;

BOOL		g_bAutoHideBar = FALSE;
CMyToolBar	*g_pMyToolBar = NULL;
BOOL	g_bShowBigToolBar = TRUE;

char		*g_strGroup[MAX_GROUPMARK] = {"①","②","③","④","⑤","⑥","T"};


vector<FY_GROUP_INFO> g_vfyGroup;
BOOL g_bFYDataAlwaysSync=FALSE;
struct FY_LOCATION g_FyLocation;

BOOL g_bTopTabWorkSpace=FALSE;
BOOL g_bEmbWTCreated=FALSE;
struct ADVERT_INFO g_AdvertInfo;
struct NewJy_CFG g_NewJyCfg;

struct Server_MarketInfo g_Mtl;

CString	g_strTendName="tend_std";
CString g_strTendCfg="tend_std.cfg";
CString g_strNeedAdName="needad.dat";
CString g_strTdxInfoIniName="kninfo.ini";

BOOL	g_bSaveZH=TRUE,g_bProtectZH=FALSE;
HWND	g_hSoftKeyWnd=NULL;	//软键盘的窗口句柄
HWND	g_hHXInfoWnd=NULL;	//核新资讯认证窗口的句柄
OverlapInfo	g_Overlap[2];

BOOL	g_bDebugMode=FALSE;
BOOL	g_bChangeDebugCode=FALSE;

HisTickCacheHead	g_HisTickCacheHdr[MAX_HISCACHE];
int					g_nNextHTCHPos = 0;

CToolTipMark		*g_pToolTipMark = NULL;
BOOL				g_bFixedXAxis=FALSE;
short				g_nFixedXAxis=20;
int					g_nDefaultZxWidth=214;
int					g_nDefaultRestHeight=250;

KeyGuyCNInfo g_KeyGuyCnInfo={0};
//BI SERVER配置
char	g_BIHostIP[50],g_BakBIHostIP[50];
unsigned short g_nBIHostPort = 7717,g_nBakBIHostPort = 7717;

int					g_nFlushRealInfo=0;
int					g_nCurLevel2Tab=0;
BOOL				g_bNewJyUseAdvHQHost=FALSE;
//港股行业
vector<FL_CodeName>	g_SectorCN;

//////////////////////////////////////////////////////////////////////////
// 基金分析平台
char lpszJJItemName[][20] = 
{
		"代码",
		"基金名称",
		"更新日期",
		"净值",
		"上期净值",
		"累计净值",
		"总份额(万)",
		"净值总额(万)",
		"净值季增(万)",
		"股票比",
		"今日首席股",
		"首席增值(万)",
		"基金公司名称",
		"投资类型",
		"投资属性"
};
float lpnOJJItemWidth[] = {7,17,9,7,9,9,11,12,12,7,12,13,20,15,15};
float lpnCJJItemWidth[] = {7,10,9,7,9,9,11,12,12,7,12,13,20,15,15};

char	lpszhbItemName[][20] = 
{
	"代码",
	"基金名称",
	"更新日期",
	"万份收益",
	"上期万份收益",
	"7日年收益率",
	"总份额(万份)",
	"净值总额(万)",
	"净值季增(万)",
	"基金公司名称",
	"投资类型"
};
float	lpnhbItemWidth[] = {7,15,9,9,13,12,12,12,12,19,12};

char lpszJJStockName[][20] = 
{
		"代码",
		"股票名称",
		"重仓基金家数",
		"合计持股(万)",
		"合计季增(万)",
		"占上市公司",
		"合计市值(万)"
};
float lpnJJStockWidth[] = {8,9,14,12,12,12,12};
pFUNDBASE		g_pFundBase = NULL;
int				g_nFundNum = 0;
pFUNDHOLDSTOCK	g_pFundHoldStock = NULL;
int				g_nHoldNum = 0;
BOOL			g_bFundDataAlwaysSync = FALSE;
//////////////////////////////////////////////////////////////////////////

struct WriteUrgentInfo *g_pWriteUrgent=NULL;
//////////////////////////////////////////////////////////////////////////
DWARNINF g_DWarnInf;
//////////////////////////////////////////////////////////////////////////
//和Comview2及其子类
char **		g_ppHqStyleStr;
char*		nullPadName = "tmp";
time_t		tmp_SetGPtime;

map<char*, int, cmpstr>			g_mUnitType;
map<char*, int, cmpstr>			g_mUnitStyle;
map<UINT, int, greater<UINT> >	g_mUnitMenu;

char		g_LayerPath[MAX_PATH]={0}, g_ContentPath[MAX_PATH]={0};
//是否支持更多的Level2功能
BOOL		g_bSupportDDE = FALSE;
CImageList	g_BtnImage,g_GridImage;

//TopViwe相关功能
BOOL			g_bHasTopView = FALSE;
BOOL			g_bTopViewStatus = FALSE;

char			g_TVHostIP[50] = {0};
unsigned short	g_nTVHostPort = 4010;
short			g_nMyProvineID = 0;
char			*g_pProvinceStr[35]={"其它","北京","上海","天津","重庆","安徽","福建","甘肃","广东","广西",
									 "贵州","海南","河北","河南","黑龙江","湖北","湖南","吉林","江苏","江西",
									 "辽宁","内蒙古","宁夏","青海","山东","山西","陕西","四川","新疆","西藏",
									 "云南","浙江","台湾","香港","澳门"};

//港股大利势
map<long, Seat_Code2Com, greater<long> > g_mSeat2Com;
map<long, Seat_Com2Name, greater<long> > g_mSeatCode2Name;

BOOL			g_bTickTStatus = FALSE;
BOOL			g_bShowCustomPadSwitch = TRUE;
BOOL			g_bHasHomePage = FALSE;	//进入主程序后采用主页模式
CString			g_InitDefHomePage = "";

int				g_nTopDockTop = 24;

map<CString, ShortCut_Cfg, cmpstr > g_mShortCut;

char			g_strYxhjCfg[MAX_PATH] = "100nian.dat";
char			g_strUserCfg[MAX_PATH] = "VipUser.dat";
char			g_strBlockNew[MAX_PATH] = "selfbk\\";

//定时请求的信息
time_t	g_LastTimeTag = 0;
vector<URGENTINFO>	g_aUrgentInfo;

long					g_nAutoCalcLock = 0;
BOOL					g_bShow3DKLine = TRUE;

short					g_nUseDNSUrl = 0;
//!!!
char					*g_strDNSUrl[2] = {"", ""};
char					*g_strTwinserverUrl = "60.169.75.45";
//char					*g_strTwinserverUrl = "127.0.0.1";

CPreviewUnitDlg *g_pPreviewUnitDlg = NULL;
CHqSrcMgn *g_pHqSrc=NULL;
vector<MemStkInfo>			head[2];

map<long, long, greater<long> > g_DBIndexMap;
CDBFile *g_pHQDBFile = NULL;

long	g_nCountCalcBlockTJ = 1;
long	g_nCalcBlockIndex = 0;

long	g_nLastValidDate = 0;
short	g_nLastValidValue = 1;
long	g_nPopInfoLastTime = 0;

Scheme_King		g_Scheme_King;

char	*g_strCalc1 = 
"A1:=MAX(MA(C,60),MA(C,120));\r\n\
B1:=MIN(MA(C,60),MA(C,120));\r\n\
N1:=MA(C,60);\r\n\
M1:=MA(C,120);\r\n\
N2:=MA(C,60);\r\n\
M2:=MA(C,120);\r\n\
黑马栏:CLOSE<=M1 AND CLOSE>=N1;\r\n\
牛股栏:C>A1;\r\n\
白马栏:CLOSE<=N2 AND CLOSE>=M2;\r\n\
A:=MAX(MA(C,60),MA(C,120));\r\n\
B:=MIN(MA(C,60),MA(C,120));\r\n\
熊股栏:C<=B;\r\n\
AA:=EMA(CLOSE,10)>REF(EMA(CLOSE,10),1);\r\n\
BB:=EMA(CLOSE,10)<REF(EMA(CLOSE,10),1);\r\n\
短线:IF(AA,1,IF(BB,2,0));\r\n\
AAA:=EMA(CLOSE,30)>REF(EMA(CLOSE,30),1);\r\n\
BBB:=EMA(CLOSE,30)<REF(EMA(CLOSE,30),1);\r\n\
中线:IF(AAA,1,IF(BBB,2,0));";

char	*g_strCalc2 = 
"A:=MA(C,5);\r\n\
A1:=MA(C,10);\r\n\
A2:=MA(C,20);\r\n\
A3:=MA(C,60);\r\n\
A4:=MA(C,120);\r\n\
A5:=REF(C,1)<REF(A,1) OR REF(C,1)<REF(A1,1) OR REF(C,1)<REF(A2,1) OR REF(C,1)<REF(A3,1) OR REF(C,1)<REF(A4,1);\r\n\
A6:=C>A AND C>A1 AND C>A2 AND C>A3 AND C>A4;\r\n\
B1:=(VOL/CAPITAL)>0.04 AND (VOL/CAPITAL)<0.11;\r\n\
B2:=CLOSE/REF(CLOSE,1)>1.02;\r\n\
升势再起:=A5 AND A6 AND B1 AND B2;\r\n\
ZJ:=(O+H+L+C)/4;\r\n\
YZ:=IF(BARSCOUNT(C)>60,VOL/SUM(VOL,60),VOL/SUM(VOL,BARSCOUNT(C)));\r\n\
CYC12:=DMA(ZJ,YZ/0.125);\r\n\
CYC25:=DMA(ZJ,YZ/0.25);\r\n\
CYC50:=DMA(ZJ,YZ/0.5);\r\n\
CYC0:=DMA(ZJ,YZ);\r\n\
洗盘结束:=FILTER(COUNT(VOL/CAPITAL<0.015,10)>=1 \r\n\
AND (MA(VOL/CAPITAL,10)<0.025 OR COUNT(VOL<MA(VOL,10),10)>=7) \r\n\
AND CLOSE>=HHV(CLOSE,10) \r\n\
AND COUNT(CLOSE<CYC0,20)<=5 \r\n\
AND CLOSE/CYC12<1.1 \r\n\
AND CLOSE/CYC0<1.3 \r\n\
AND LAST(CYC50>CYC0,30,0),10);\r\n\
涨停先锋:=CLOSE/REF(CLOSE,1)>=1.099 \r\n\
AND AMOUNT>50000000 \r\n\
AND  VOL/CAPITAL>=0.04 \r\n\
AND 3*VOL/SUM(V,3)>1.5 \r\n\
AND CLOSE/REF(CLOSE,20)<1.6 \r\n\
AND CAPITAL<5000000;\r\n\
逆势拉抬:=C>=CYC12 \r\n\
AND CYC12>CYC25 AND CYC25>CYC50 \r\n\
AND INDEXC/REF(INDEXC,1)<0.975 \r\n\
AND C/REF(C,1)>1.03 \r\n\
AND VOL/CAPITAL>0.05;\r\n\
VAR3:=(MA(AMOUNT,5)/MA(VOL,5)/100-MA(AMOUNT,125)/MA(VOL,125)/100)/(MA(AMOUNT,5)/MA(VOL,5)/100);\r\n\
VAR4:=MA((LLV(LOW,45)-CLOSE)/(HHV(HIGH,45)-LLV(LOW,45))*100,3);\r\n\
绝地反弹:=CROSS(-5,VAR4) AND VAR3<-0.4;\r\n\
红色:升势再起 AND 洗盘结束 AND 涨停先锋;\r\n\
黄色:升势再起 AND 洗盘结束 AND 涨停先锋;\r\n\
青色:涨停先锋;\r\n\
蓝色:绝地反弹;\r\n\
超跌:逆势拉抬;\r\n\
短线买入:CROSS((C+REF(L,1))/2,EMA((H+O+L+C)/4,10));\r\n\
中线买入:CROSS((C+REF(L,1))/2,EMA((H+O+L+C)/4,30));";

char	*g_strCalc3 = 
"N:=MA(C,60);\r\n\
M:=MA(C,120);\r\n\
红:=CLOSE>N AND CLOSE>M;\r\n\
黄:=CLOSE<M AND CLOSE>N;\r\n\
青:=CLOSE>M AND CLOSE<N;\r\n\
蓝:=CLOSE<N AND CLOSE<M;\r\n\
IF(红,1,IF(黄,2,IF(青,3,IF(蓝,4,0))))";
//
CCriticalSection	g_DXIndexCalc;
CCriticalSection	g_HisDataReader;
BOOL	g_bAllLocalData = FALSE;
BOOL	g_bTickStatReaded = FALSE;

CCriticalSection	g_CalcIniLock, g_PoloReqSec;
map<long, float, greater<long> > g_mTolStat;

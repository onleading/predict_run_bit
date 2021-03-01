#ifndef H_PROTOCOL
#define H_PROTOCOL

#define NOW_VERSION		12		//通达信.net远程版版本为8,支持新数据接口升为10,支持成交金额为11,升级到V4.0使用12
#define SEPARATOR_NUM	7654321
#define REQBUFFER_LEN	16*1024
#define ANSBUFFER_LEN	4*8196	//32K

#define  SH_CODE_LEN   6
#define  NAME_LEN      8

#define  GP_MAXGBBQNUM	200

typedef struct tagReqHeader
{
	struct
	{
		unsigned char Version:4;
		unsigned char Compressed:1;
		unsigned char Encrypted:1;
		unsigned char Reserved:2;
	}     Info;
	short MainID;
	short AssisID;
	char  Priority;
	unsigned short PacketLen;
	unsigned short RawLen;
} REQHEADER;

typedef struct tagAnsHeader
{
	long	Separator;
	struct
	{
		unsigned char Version:4;
		unsigned char Compressed:1;
		unsigned char Encrypted:1;
		unsigned char Reserved:2;
	}     Info;
	short MainID;
	short AssisID;
	char  Priority;
	short req;
	unsigned short PacketLen;
	unsigned short RawLen;
} ANSHEADER;

typedef struct 
{
	REQHEADER ReqHeader;
	char      ReqBuffer[REQBUFFER_LEN];
} REQBUFFER;

const short TIME_REQ	= 1;
const short XXMCFG_REQ  = 2;
const short JBZLDATE_REQ= 3;
const short ALIVE_REQ	= 4;
const short URGENT_REQ	= 5;
const short HOSTINF_REQ = 6;
/****/
const short PRETRANS_REQ	= 7;
const short PUBKEY_REQ		= 8;
const short SESSIONKEY_REQ	= 9;
/****/
const short USER_URGENT_REQ	= 10;		//用户名相关的通告

const short INMODE_EREQ=	11;
const short INMODE2_EREQ=	12;
const short HOSTMORE_EREQ=	13;
const short AUTOBASE_EREQ=	14;
const short AUTOGBBQ_EREQ=	15;
const short AUTOBASE2_EREQ=	16;

const short OTHERINFO_REQ = 21;	//内部窥视信息

const short NEED_AUTHP_REQ = 30;//是否需要认证,传给主站自己的信息,guest用户及密码,券商用户及密码
const short CODE_REQ=     101;
const short GPT_REQ=      102;
const short JYMM_REQ=     105;
const short JYMM_REQ2=    106;			  //而且要校验网卡信息
const short CHMM_REQ=	  107;			  //更改密码

const short CJ_REQ		  =401;       //总成交

const short XXMITEM_REQ = 702;            //文字消息
const short FILE_REQ    = 704;
const short XXMWZ_LONG_REQ   = 706;       //文字消息
const short JBMWZ_LONG_REQ   = 707;       //文字消息
const short MAILITEM_REQ     = 708;
const short FILELEN_REQ = 709;            //文件长度
const short XXM_LEN_REQ = 710;            //消息面文件长度
const short JBM_LEN_REQ = 711;            //基本资料文件长度
const short JBMWZ_ALL_REQ   = 713;       //所有文字消息
const short F10CFG_REQ		= 714;       //请求多功能F10的配置信息
const short JBMWZ_LONG2_REQ = 715;		 //大量的请求基本资料,每次请求10K

const short NEW_F10CFG_REQ	= 719;       //请求多功能F10的配置信息
const short NEW_F10TXT_REQ = 720;		 //请求多功能F10的内容信息

const short CODE_NREQ=     1101;
const short STOCKNUM_NREQ= 1102;
const short CODE2_NREQ=	   1103;
const short CODE3_NREQ=	   1104;
const short IHQ_NREQ=      1107;
const short CODELS_NREQ=   1108;
const short IHQLS_NREQ =   1109;

const short EXTHQ_NREQ=    1201;          // 分类行情显示
const short ZDHQ_NREQ=     1202;          // 分类行情显示
const short MULTIHQ_NREQ=  1203;          // 支持任意栏目排行的行情显示
const short COMBHQ_NREQ=   1204;          // 组合任意股票的行情显示(自选股,版块股,指数)
const short ZHSORT_NREQ=   1205;
const short FST_NREQ=      1301;          // 分时走势图
const short GGHQ_NREQ=     1302;          // 个股行情
const short ZSHQ_NREQ=     1303;          // 指数
const short HQB_NREQ=      1304;          // 行情表
const short FBB_NREQ=      1305;          // 每笔成交明细
const short FJB_NREQ=      1306;          // 分价表
const short MMLD_NREQ=     1307;          // buyv,salev 买卖力道/涨跌家数
const short CCL_NREQ=      1308;          //持仓量
const short COMBFST_NREQ=  1309;          // 组合分时走势图(可包括行情和/或每笔成交明细)
const short COMBHQB_NREQ=  1310;          // 组合行情表(行情和每笔成交明细)

const short GGHQ_5MMP_NREQ=		1311;		  // 支持5档买卖盘的个股行情协议
const short MULTIHQ_5MMP_NREQ=	1312;		  // 支持5档买卖盘
const short COMBHQ_5MMP_NREQ=	1313;		  // 支持5档买卖盘
const short MULTIHQ2_5MMP_NREQ=	1314;		  // 支持5档买卖盘
const short COMBHQ2_5MMP_NREQ=	1315;		  // 支持5档买卖盘
const short MULTIHQ3_5MMP_NREQ=	1316;		  // 支持5档买卖盘,支持屏蔽在A股中003和609
const short G2_COMBHQ_NREQ=		1318;		  // 第二代行情协议
const short L2_COMBQUEUE_NREQ=	1319;		  // 支持Level2协议的行情协议

//为超越增加的新协议
const short MULTIHQ2_NREQ= 1320;
const short COMBHQ2_NREQ = 1321;
const short CDP_NREQ	 = 1322;
const short CODEFX_NREQ	 = 1323;			//仅针对代码来处理请求数据
const short COMBFX3_NREQ = 1324;
//对于ETF等的补充协议
const short COMBFX4_NREQ		= 1325;	// 
const short COMBFST2_NREQ		= 1326;	// 组合分时走势图(可包括行情和/或每笔成交明细)
const short FBB2_NREQ			= 1327; // 每笔成交明细
const short GGHQ2_5MMP_NREQ		= 1328;	// 支持5档买卖盘的个股行情协议
const short COMBHQ_2_5MMP_NREQ	= 1329;	// 支持5档买卖盘

const short MULTIHQ_NEW_NREQ	= 1341;	//支持分钟涨速的分钟数等设置功能
const short COMBHQ_NEW_NREQ		= 1342;	//支持分钟涨速的分钟数等设置功能
const short ZHSORT_NEW_NREQ		= 1343; //支持分钟涨速的分钟数等设置功能

const short G2_COMBHQ2_NREQ		= 1349;	// 第二代行情协议 组合行情 扩充

const short L2_TICNUM_NREQ		= 1360;	// Level2中的逐笔成交的数目
const short L2_REVTIC_NREQ		= 1361;	// Level2中的逐笔成交(反向)
const short L2_TIC_NREQ			= 1362;	// Level2中的逐笔成交(正向)

const short L2_WT_RANK_NREQ				= 1370;	// 总委托排名
const short L2_SINGLECANCEL_RANK_NREQ	= 1371;	// 即时撤单排名
const short L2_TOTALCANCEL_RANK_NREQ	= 1372;	// 总撤单排名
const short L2_HY_RANK_NREQ				= 1373;	// 行业成交排名

const short G2_DEEPWARN_NREQ			= 1379;	// 请求预警
const short L2_ZMZM_NREQ				= 1380;	// 分钟总买总卖记录
const short L2_JGJC_NREQ				= 1381;	// 分钟净挂单净撤单
const short L2_AUCTION_NREQ				= 1385;	// 集合竞价

const short L2_IMGSTAT_NREQ				= 1390;	// 统计数据的批量请求(Img)

const short L2_NEWCJBS_NREQ				= 1392;	// 新成交笔数
const short L2_NEWVOLUME_NREQ			= 1393;	// 新大单统计
const short L2_NEWTICKSTAT_NREQ			= 1394;	// 新龙虎看盘
const short L2_NEWTCKSTAT_NREQ			= 1395;	// 统计数据的批量请求(Tck)

const short FX_NREQ=       1501;          // 技术分析
const short COMBFX_NREQ=   1502;          // 组合技术分析(可包括行情和/或每笔成交明细)
const short AMOUNT_NREQ=   1503;          // 历史数据成交量
const short UPDOWN_NREQ=   1504;          // 历史数据涨跌家数(指数)

const short PUTFILE_REQ   =1712;
const short PATHITEM_REQ  =1713;
const short GETCWD_REQ    =1716;
const short PATHFILE_REQ		=1721;

//////////////////	4000 以上的新协议(专为通达信.net远程版而作) /////
const short COMTRY_NREQ			= 4010;	 //组合版面试探性包
const short REVBSP1_NREQ		= 4013;	 //缩小买卖盘数据,反向取数据
const short REVFBB_NREQ			= 4015;	 
const short COMBFST_HIS_NREQ	= 4020;	//历史走势数据
const short REVFBB_HIS_NREQ		= 4021;	//历史Tick数据
const short REVBSP1_HIS_NREQ	= 4023;	//历史Bsp数据,反向取数据
const short FJB_HIS_NREQ		= 4024;	//历史分价表
//宽带协议
const short SIMHQ_NREQ			= 4029;	//很多精简行情,一般为200只股票
const short SIMFX_NREQ			= 4030;	//很多最近的分析数据,一般为10只股票
const short PREDOWNLOAD_NREQ	= 4031;	//开始下载之前
const short SIMFX2_NREQ			= 4032;	//很多最近的分析数据,包括成交金额

//对于ETF等的补充协议
const short COMBFST2_HIS_NREQ	= 4036;	//历史走势数据
const short REVFBB2_NREQ		= 4037;	//反向Tick数据
const short REVFBB2_HIS_NREQ	= 4038;	//历史Tick数据

const short FXFILE_NREQ			= 4040;	//分析数据文件请求

const short EXDAYFILE_NREQ		= 4043;	//分时数据文件请求

const short DATADIG_NREQ		= 4050;	//数据挖掘请求
const short DATADIG_EXT_NREQ	= 4051;	//扩充数据挖掘请求

const short BIGVOLTICK_NREQ		= 4055;	//主力大单请求

const short GET_PASSPORT_NREQ	= 4059; //得到通行证
const short GET_QSID_NREQ		= 4060; //取券商标识
const short PUT_URGENT_NREQ		= 4061; //提交紧急通告信息
const short GET_URLURGENT_NREQ	= 4062; //取URL紧急通告
const short RESTART_NREQ		= 4063;	//重启动

const short GET_IW_TITLE		= 4080;	//得到资讯工场标题信息

const short ACTIVEINFO_NREQ		= 4104;	//公告
const short INFOUPLOAD_NREQ		= 4105;	//公告发布

struct remote_cod_info
{ 
	char           code[SH_CODE_LEN+1];   //证券代码
	short          unit;                  //交易单位
	long           ltgb;                  //流通股本
	char           name[NAME_LEN+1];      //证券名称
	short          lbbase;
	char           precise;
};

struct remote_hq_struc
{
	char   code[6];
	WORD   itemnum;               //采样点数
	WORD   close;                 //前收盘价,指数
	WORD   open;                  //今开盘价,指数
	WORD   max;                   //最高价,指数
	WORD   min;                   //最低价,指数
	WORD   now;                   //现价,最近指数
	WORD   buy;                   //最高叫买价
	WORD   sale;                  //最低叫卖价
	long   cjl;                   //总成交量
	WORD   now_cjl;               //现手
	float  cjje;                  //总成交金额
	long   inside;                //内盘
	long   outside;               //外盘
	WORD   sd2;                   //笔涨跌
	long   ccl;                   //市盈率1,持仓量
	WORD   buyp[3];               //三个叫买价
	WORD   buyv[3];               //对应三个叫买价的三个买盘
	WORD   salep[3];              //三个叫卖价
	WORD   salev[3];              //对应三个叫卖价的三个卖盘
};

struct hq_struc                 // 在.IHQ文件中读入的数据结构
{
  short          itemnum;               // 采样点数
  float          close;                 // 前收盘价,指数
  float          open;                  // 今开盘价,指数
  float          max;                   // 最高价,指数
  float          min;                   // 最低价,指数
  float          now;                   // 现价,最近指数
  float          buy;                   // 最高叫买价
  float          sale;                  // 最低叫卖价,领先指标(指数)
  long           cjl;                   // 总手
  WORD           now_cjl;               // 现手(总手差)
  float          cjje;                  // 总成交金额
  long           inside;                // 内盘
  long           outside;               // 外盘
  float          sd2;                   // 笔升跌(价位差)
  union
  {
                 float syl;             // 市盈率1,不含加权的指数
                 long ccl;              // 持仓量
  }              syl_ccl;
  float          buyp[3];               // 三个叫买价
                                        // 基金成交金额:buyp[0]
                                        // 债券成交金额:buyp[1]
                                        // 国债成交金额:buyp[2]
  WORD           buyv[3];               // 对应三个叫买价的三个买盘
                                        // 上涨家数:buyv[0]
  float          salep[3];              // 三个叫卖价
					// 权证成交金额:salep[0]
					// 期货昨持仓量:salep[1]
					// 指数其它成交金额:salep[1]
					// 期货昨结算价:salep[2]
                                        // 指数昨不含加权指数:salep[2]
  WORD           salev[3];              // 对应三个叫卖价的三个卖盘
                                        // 下跌家数:salev[0]
  struct
  {
        WORD           inoutflag:2;     // 内外盘标志
        short          sdunit:4;        // 笔升跌单位(用于计算领先指标)
        WORD           unused:10;       // 未用标志
  }              flag;                  // 标志位结构
};

struct remote_ls_struc
{ 
	WORD            minute;
	WORD            now;                   //现价或最近指数
	WORD            now_cjl;               //现手
	short           ccl_dif;               //持仓量增减
	short           bs_flag;               //买卖标志
};

struct remote_fz_data
{ 
	WORD            now;
	WORD            average;
	WORD            now_cjl;
};

struct remote_mmld_struc
{ 
	WORD            buyv;
	WORD            salev;
};

struct remote_ls_data
{ 
	union long_short time;
	WORD open;
	WORD high;
	WORD low;
	WORD close;
	long volume;
};

struct remote_TmpStkInfo
{ 
	char           Code[SH_CODE_LEN];     // 证券代码
	short          Unit;                  // 交易单位
	float          ActiveCapital;         // 流通股本
	char           Name[NAME_LEN];        // 证券名称
	long           VolBase;               // 量比的基量
	char           precise;
};

struct remote_TmpStkInfo2
{
	char           Code[SH_CODE_LEN];     // 证券代码
	short          Unit;                  // 交易单位
	char           Name[NAME_LEN];        // 证券名称
	long           VolBase;               // 量比的基量
	char           precise;
	float		   Close;				  //昨收
};

struct remote_TmpStkInfo3
{
	char           Code[SH_CODE_LEN];     // 证券代码
	short          Unit;                  // 交易单位
	char           Name[NAME_LEN];        // 证券名称
	long           VolBase;               // 量比的基量
	char           precise;
	float		   Close;				  //昨收

	short		   BaseFreshCount;		  //基本资料的更新次新
	short		   GbbqFreshCount;		  //股本变迁的更新次新
};

struct remote_BaseInfo2	//Protocol Change
{
	char		setcode;			//市场类型
	char        Code[SH_CODE_LEN];  // 证券代码
	float       ActiveCapital;      //流通股本
	short       J_addr;				//所属省份
	short       J_hy;				//所属行业
    long        J_gxrq;				//更新日期
	long		J_start;			//上市日期
    float       J_zgb;				//总股本
    float       J_zjhhy;			//证监会行业
    float       J_oldjly;			//上年此期净利润
    float       J_oldzysy;			//上年此期主营收入
    float       J_bg;				//B股
    float       J_hg;				//H股
    float       J_mgsy2;			//季报每股收益 (财报中提供的每股收益,有争议的才填)
    float       J_zzc;				//总资产(千元)
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
	float		J_HalfYearFlag;		//全部更改为以月为单位
};

struct remote_cdpInfo
{
	float       cdp;                   // 昨日中价
	float       nh;                    // 卖点
	float       nl;                    // 买点
	float       ah;                    // 高价突破点
	float       al;                    // 低价突破点
 	char		dkflag;                // 适合作多或作空
	float       tbp;                   // 多空平衡点
	float       stoplost;              // 止损点
	float       leave;                 // 了结点
};

//精简行情结构
struct simhq_Info
{
  float          Open;                  // 今开盘价,指数
  float          Max;                   // 最高价,指数
  float          Min;                   // 最低价,指数
  float          Now;                   // 现价,最近指数
  DWORD          Volume;                // 总手
  float          Amount;                // 总成交金额  
};

struct inmode_req
{
	short req;
	char  type;
	char  inmode;
	char  username[13];
	char  password[13];
	char  bnetcard;
	char  netcard[13];
	char  castring[13];
};

struct Protocol_IpInfo
{
	char ipaddress[26];
	unsigned short port;
	char description[50];
};

struct inmode_ans
{
	char  ch;	//成功标志,0:成功,客户端断开连接,连接新IP 1:亦成功,客户端继续保持连接 2:亦成功,客户端继续保持连接，下一请求必须发送一营业部编号。为其它:错误号
	short minute;
	char  right;
	char  ipnum;
	struct Protocol_IpInfo Info[1];
};

struct inmode2_req
{
	short req;
	char  type;
	char  inmode;
	char  username[13];
	char  nickname[21];
};

struct inmode2_ans
{ 
	char  ch;		//成功标志,0(不是'0'):成功 其它:错误号
};

struct sendyybno_req
{
	short req;
	short whichyyb;
};

struct sendyybno_ans
{
	char ch;		//成功标志,0(不是'0'):成功 其它:错误号
};

struct autobase_req //在请求BASE和GBBQ前，应先读进INF
{
	short req;
	short stknum;
	char  buf[1];	//股票代码缓冲区,如果是autobase2，则代码后紧跟setcode
};

struct autobase2_ans	//Protocol Change
{
	short stknum;
	struct remote_BaseInfo2 basep[1];
};

struct autogbbq_req
{
	short req;
	short stknum;
	char  buf[1];
};

struct autogbbq_ans
{
	short stknum;
	char  buf[1]; //存放股票市场代码,变迁个数和具体变迁的缓冲区
};

struct cod_req
{ 
	short req;
	short setcode;
	short startxh;
};

struct stknum_req
{
	short req;
	short setcode;
	long  rq;
};

struct gpt_req
{ 
	short req;
};

struct mm_req
{ 
	short req;
	char  username[13];
	char  password[13];
};

struct mm_req2
{ 
	short req;
	char  username[13];
	char  password[13];
	char  netcard[13];
};

struct chmm_req
{
	short req;
	char  oldpassword[13];
	char  newpassword[13];
};

struct chmm_ans
{
	short chret;//-1:老密码不正确，0:其它原因不能修改,1:成功
};

struct ihq_req
{ 
	short req;
	short setcode;
	short startxh;
};
struct codls_req
{ 
	short req;
	short setcode;
	short startxh;
	long  rq;
};

struct ihqls_req
{ short req;
  short setcode;
  short startxh;
  long  rq;
};

struct hq_req
{ short req;
  short setcode;
  short htype;
  short ftype;
  short startxh;
  char  choicestr[4];
};
struct exthq_req
{ short req;
  short setcode;
  short htype;
  short ftype;
  short startxh;
  short wantnum;
  char  choicestr[4];
};

struct  zdhq_req
{ short req;
  short setcode;
  char  code[19*SH_CODE_LEN];
};

struct multihq_req
{ short req;
  short setDomain;
  short coltype;
  short startxh;
  short wantnum;
  short sorttype;
};

struct combhq_req
{ short req;
  short num;
  char  codehead[1];
};

struct multihq_new_req
{ short req;
  short setDomain;
  short coltype;
  short startxh;
  short wantnum;
  short sorttype;
  short zfmin;
  short unused[3];
};

struct combhq_new_req
{ short req;
  short zfmin;
  short unused[3];
  short num;
  char  codehead[1];
};

struct zst_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
};

struct gghq_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
};

struct hqb_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
};

struct fbb_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
  short startxh;
  short num;
};

struct fbb_his_req
{ short req;
  long  ldate;
  short setcode;
  char  code[SH_CODE_LEN];
  short startxh;
  short num;
};

struct fjb_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
};

struct fjb_his_req
{ short req;
  long  ldate;
  short setcode;
  char  code[SH_CODE_LEN];
};

struct mmld_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
};

struct ccl_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
};

struct combzst_req
{ short req;
  short setcode;			//增加setcode
  char  code[SH_CODE_LEN];
  short hqflag;
  short ticknum;
};

struct combzst_his_req
{ short req;
  long ldate;
  char setcode;
  char code[SH_CODE_LEN];
};

struct combhqb_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
  short hqflag;
  short ticknum;
};

struct cj_req
{ 
	short req;
	short flag;
	short setcode;
	short startxh;
	short recnum;
};

//传输数据前的请求
struct pretrans_ans
{
	unsigned char  SecureRank;
	unsigned char  HQSecure;
	unsigned char  RsaBits;
	unsigned char  WhichSession;
	unsigned char  WhichHash;
	unsigned char  KeyRank;
	unsigned char  AllInit;
	unsigned char  Reserved;
};

//////////

struct fx_req
{ short req;
  short offset;
  short setcode;
  short linetype;
  char  code[SH_CODE_LEN];
};

struct codefx_req
{ 
	short req;
	short setcode;
	char  code[SH_CODE_LEN];
	short offset;
	short itemnum;
};

struct combfx2_req
{ 
	short req;
	short setcode;
	char  code[SH_CODE_LEN];
	short linetype;
	short mulnum;
	short offset;
	short itemnum;
};

struct amount_req
{ 
	short req;
	short offset;
	short linetype;
	char  code[SH_CODE_LEN];
	short itemnum;
};

struct updown_req
{ 
	short req;
	short offset;
	short linetype;
	char  code[SH_CODE_LEN];
	short itemnum;
};

struct wz_req
{ 
	short req;
	short htype;
	short startxh;
};

struct file_req
{  
	short req;
	DWORD pos;
	char  fname[40];
};


struct filelen_req
{  
	short req;
	char  fname[40];
};

struct filelen_ans
{
   DWORD filelen;
   char  usemdstr;
   char	 mdstr[33];
};

struct mailitem_req
{  short req;
   char  code[7];
   short startxh;
};

struct xxmwz_long_req
{ short req;
  short xxmxh;
  short xh;
  DWORD pos;
};

struct xxm_len_req
{ short req;
  short xxmxh;
  short xh;
};
struct jbmwz_long_req
{ short req;
  DWORD pos;
  short setcode;
  char code[SH_CODE_LEN];
};

struct jbmwz_all_req
{ short req;
  short setcode;
  char code[SH_CODE_LEN];
};

struct jbm_len_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
};

struct pathfile_req
{
	short	req;
	long	pos;
	long	len;
	char	pathfile[100];
};

struct pathfile_ans
{
	long	len;
	char	buf[1];
};

struct secret_req
{ short req;
  DWORD bios;
  DWORD biosppp;
};

struct openfile_req
{ short req;
  char  fname[40];
};

struct putfile_req
{ short req;
  DWORD pos;
  WORD  len;
  char  fname[40];
  char  buf[1];
};

struct pathitem_req
{ short req;
  WORD  startxh;
  char  path[80];
};

struct gpt_ans
{ 
	short sz[4];
	short sh[4];
};

struct mm_ans
{ 
	char  ch;
	short minute;
	char  right;
};

struct hq_ans
{ short  totalnum;
  short  dispnum;
  struct remote_hq_struc hqp[19];
};

struct cj_ans
{ 
	short startxh;
	short recnum;
	char  disp_string[1];
};

struct wz_ans
{ 
	short num;
	char  buf[1];
};

struct time_ans
{ 
	struct bc_date date;
	struct bc_time time;
};

struct otherinfo_ans
{
	long ClientNum;		//当前在线人数
	long MaxConnectNum;
	long PackageNum;
	char HasStatus;
	char HasLog;
	char bHQ;
	char bWT;
	char starttime[25];

	char HostVer[30];
	char ProtocolVer;
	long TotalClientNum;//当前总在线人数
	long UsedClientNum;	//从开启主站到现在有多少人登陆过
	char bAutoBase;
	char HomePath[255];
	char NetCardStr[20];
	long InfDate;
	long InfHms;
	char HostType;		//服务器类型 独立，子，主
	char ProcType;		//0:批作业方式 1:多线程方式
	char ZlibType;		//压缩方式 0:自动优化 1:完全不压缩 2:最大限度压缩
	char bDayToMem;
	char bWeekToMem;
	char bMonToMem;
	char bMinToMem;
	char bKExplain;			//是不是成功地启动了选股等服务
	char bAlarm;
	char bHqStat;
	char bCanOldFy;		//是不是允许下列拨入
	char bCanDos;
	char bCanTdxW;
	char bCanWeb;
	char bCanPda;
	char bCanWide;
	char bCanPlay;
	char bCanJdh;

	char bTapi;
	char bUdp;
	char bChat;
	short RecSocket;
	short UdpSocket;

	short ProcessNum;	//进程数
	short ThreadNum;	//线程数
	short unused1;

	char  TicFlag;		//TIC标志
	char  MtcFlag;		//MTC标志
	char  unused2[30];
};

struct jbzldate_ans
{
	int date;
	int time;
};

struct xxmwz_ans
{ 
	short num;
	char  buf[1];
};

struct cod_new_ans
{ 
	short  stknum;
	struct remote_TmpStkInfo headp[1];
};

struct cod_new_ans2
{ 
	short  stknum;
	struct remote_TmpStkInfo2 headp[1];
};

struct cod_new_ans3
{ 
	short  stknum;
	struct remote_TmpStkInfo3 headp[1];
};

struct ihq_new_ans
{ 
	short  stknum;
	struct CurrStockData hqp[1];
};

struct secret_ans
{ 
	short seconds;
	short trustee;
};

struct urgent_req
{
	short req;
};

struct user_urgent_req
{
	short req;
	char  username[50];
	char  reserved[4];
};

struct urgent_ans
{
	char valid;
	long ldate;
	short advertlen;
	short sourcelen;
	short contentlen;
	char buf[1];
};

struct hostinf_ans
{
	char webpage;
	char hostname[21];
	char url[100];
};

struct hostmore_req
{
	short req;
	char  verflag;	
};

struct hostmore_ans
{
	char   errflag;	//errflag为0表示可用，其它则表示有错
	struct bc_date date;
	struct bc_time time;
	short sz[8];
	short sh[8];
	char  byesterday;
	long  szinfdate;
	long  szinfhms;
	long  shinfdate;
	long  shinfhms;
	char  bserv1;
	char  bserv2;
	char  bserv3;
	char  nUrgentNum;
	char  linuxcheck;
	char  bbigjbm;
	char  jbmnum;
	char  bcanuserurgent;
	char  hasBakHostFile;
	char  webpage;
	char  hostname[21];
	char  url[70];	
	char  Jbmname[3][10];
};

struct zhsort_req
{
	short req;
	char  domain;
	char  num;
};

struct zhsort_new_req
{
	short req;
	char  domain;
	short zfmin;
	short zhtype;	//0:早期的方式,委托比 1:新的方式,显示总量换手率和现量换手率
	short unused[2];
	char  num;
};

struct zhsort_ans
{
	char num;//每个排名的股票数目
	char buf[1];
};

struct cdp_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];  
};

struct bsp1_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
  short startxh;
  short num;
};

struct bsp1_his_req
{ short req;
  long  ldate;
  short setcode;
  char  code[SH_CODE_LEN];
  short startxh;
  short num;
};

//宽带协议
struct simhq_req //很多精简行情
{ short req;
  short num;	
  char  buf[1];	//SetCode和代码缓冲区
};

struct simhq_ans
{
  short num;	//多少只股票
  char buf[1];	//具体的精简行情缓冲区  
};

struct simfx_req //很多最近的分析数据
{ short req;
  short recentnum;	//最近股票数
  char	type;		//周期
  short mulnum;		//多周期的数目,一般填1
  short num;
  char  buf[1];		//代码缓冲区
};

struct simfx_ans
{ short num;
  char  type;
  char buf[1];		//具体的分析数据缓冲区(内有每只股票的分析数据个数)
};

struct fxfile_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
  long  startdate;
  long  enddate;  
  short type;
};

struct fxfile_ans
{ 
  short type;
  long filelen;
  char buf[1];
};

#define	EXDAYSZ_FILE	0
#define	EXDAYSH_FILE	1

struct dayfile_req
{
  short req;
  short type;
  long  ldate;
  long	lpos;
  long	reqlen;
};

struct dayfile_ans
{ 
  short type;
  long filelen;
  long sendlen;
  char buf[1];
};

struct HostList
{
	char  ch;	//成功标志,0:成功,客户端断开连接,连接新IP 1:亦成功,客户端继续保持连接 2:亦成功,客户端继续保持连接，下一请求必须发送一营业部编号。为其它:错误号
	char  ipnum;
	struct Protocol_IpInfo Info[100];	//最多支持100个委托主站或营业部
};

struct pre_download_req
{
	short req;
};

struct pre_download_ans
{
	char sucflag;		//成功标志 0:不能下载 1:可下载
	char errmsg[100];	//返回信息串
};

struct datadig_req
{
	short req;
	short reserved;
};

struct datadig_setcode_code
{
	char setcode;
	char stockcode[SH_CODE_LEN];
	unsigned long rate;	//关注度或共鸣度
};

struct datadig_ans
{
	long clientnum;
	long usedclientnum;
	short attention_num;
	short resonate_num;
	struct datadig_setcode_code attention[50];
	struct datadig_setcode_code resonate[50];
	long contentlen;
	char content[1];
};

struct datadig_ext_ans
{
	long clientnum;
	long usedclientnum;
	long seconds; //
	char HostName[21];
	short attention_num;
	short no_attention_num;
	short resonate_num;
	short no_resonate_num;
	struct datadig_setcode_code attention[80];
	struct datadig_setcode_code no_attention[80];
	struct datadig_setcode_code resonate[80];
	struct datadig_setcode_code no_resonate[80];
	long contentlen;
	char content[1];
};

struct BigVolTick
{
	char			SetCode;
	char			Code[SH_CODE_LEN];
	short			Minute;                // 零点以来的分钟数
	float			Now;                   // 现价
	DWORD			NowVol;                // 现手
	char			InOutFlag;             // 买卖标志
}; //结构为18个字节,按32K的应答,可以传1800个大单

struct bigvoltick_req
{
	short	req;
	long	getpos;  //如果getpos为-1,则请求最近的500个
	long    avail_vol;	//量的门限值,缺省为500,必须大于400
	long	reserved;
};

struct bigvoltick_ans
{
	long	lastpos;
	long	ticknum;
	struct BigVolTick list[1];
};

struct get_urlurgent_req
{
	short req;
};

struct get_urlurgent_ans
{
	long  URLEndTime;
	char  URLTitle[81];
	char  URL[81];
	char  reserved[21];
};

struct f10cfg_req
{ short req;
  short setcode;
  char code[SH_CODE_LEN];
};

struct f10cfg_title
{
	char title[16];	
	long pos;
	long len;
};

struct f10cfg_ans
{
  short titlenum;
  struct f10cfg_title list[1];
};

///////////////////////

struct new_f10cfg_req
{
	short req;
	short setcode;
	char code[SH_CODE_LEN];
	short whichjbm;
	short reserved;
};

struct new_f10cfg_title
{
	char sTitle[64];
	char sFilePath[80];
	long nOffset;
	long nLength;
};

struct new_f10cfg_ans
{
  short titlenum;
  struct new_f10cfg_title list[1];
};

struct new_f10txt_req
{
	short req;
	short setcode;
	char code[SH_CODE_LEN];
	short which;
	char sFilePath[80];
	long nOffset;
	long nLength;
	short whichjbm;
	short reserved;
};

struct new_f10txt_ans
{
	short setcode;
	char code[SH_CODE_LEN];
	short which;
	short num;
	char  buf[1];
};

///////////////////////

struct need_auth_req
{
  short req;
  char  username[13];
  char  password[13]; 
  char  needcheckip;
  char  reserved[3];
};

struct IpInfo
{
	char ipaddress[26];
};

struct need_auth_ans
{
  char  sucflag;	//用户是否成功
  char  errmsg[50];
  char  need_auth;	//是否需要认证
  short ipnum;
  struct IpInfo Info[1];
};

struct get_qsid_req
{
	short req;
};

struct get_qsid_ans
{
	char  qs_id[11];
	char  reserved[21];
};

struct get_passport_req
{
	short req;
	char QsId[11];
	float clientver;
	char  AutoupOrder;	//自动升级序号
	char  ProductIDchar;//产品ID升级Char
	char  magicchar2;	//今日的第二个MagicChar
	char  ProductID[11];//产品ID
	char  valid_level;	//有效等级,早期是0,主要是防止客户端的早期破解版的使用
};

struct get_passport_ans
{
	char Sucflag;			//0:失败 1:成功
	char bAutoupFromHQHost;	//从行情主站上进行升级
	char bVerLimit;			//0:版本没有受限制 1:版本受限制
	char errmsg[50];		//错误信息
	char bNeedAutoup;		//是否需要升级 0:无需升级 1:需升级 2:需强制升级
	float ThisVer;			//客户端版本
	char  AutoupURL[120];	//升级的网址目录
	char  Descripton[1];	//升级说明
};

//公告相关
enum
{
	SYSAD_TYPE,
	NEWS_TYPE,
	URGENT_TYPE,
	WEBURL_TYPE
};

typedef	struct	tagReqInfoNull
{
	short	ReqID;
	time_t	TimeTag;
}REQINFONULL,*PREQINFONULL;

typedef	struct	tagReqInfoTimer
{
	short	ReqID;
	long	LogTimer;
	BYTE	MoreInfo;		//0无其他信息 1紧急公告
	long	TimeTag;		//时间
	long	InfoLen;
	char	Info[1];
}REQINFOTIMER,*PREQINFOTIMER;

typedef	struct tagUrgentInfo
{
	BYTE	Type;
	time_t	TimeTag;
	short	TitleLen;
	short	ContentLen;
	char	*Info;
	DWORD	Reserved[4];
}URGENTINFO, *PURGENTINFO;

typedef	struct	tagReqInfoUpload
{
	short	ReqID;
	short	ReqUpLoadLen;	//整个消息的大小
	BYTE	Type;			//0无其他信息 1紧急公告
	time_t	TimeTag;		//时间
	long	InfoLen;
	char	Info[1];
}REQINFOUPLOAD,*PREQINFOUPLOAD;
//////////////////////////////////////////////////////////////////////////
struct iw_title_req
{
	short	req;
	char	search_type;			//0:根据股票代码 1:根据行业代码 2:根据分类ID 3:取观察员数据
	long	from_order;				//从那个顺序开始向后取数据
	short	wantnum;				//最多80个
	
	short	setcode;				//证券市场
	char	code[11];				//代码
	long	hy_id;					//行业ID
	long	type_id;				//分类ID
	
	short	magicshort2;			//用户管理服务器返回的验证串
	char	LoginName[20];			//用户名
	
	char	ununsed[8];				//备用
};

struct iw_title_ans
{
	short	totalnum;
	short	titlenum;
	struct	iw_client_title list[1];
};
////////////////////////////////////////////////////////////////////////////////////
//Level2相关协议
//逐笔数
struct l2ticnum_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
  DWORD localSeq;
  short reserved;
};

struct l2ticnum_ans
{
  short setcode;
  char  code[SH_CODE_LEN];
  DWORD MaxSeq;
};

//逐笔请求
struct l2tic_req
{ short req;
  short setcode;
  char  code[SH_CODE_LEN];
  DWORD startxh;
  short num;
};

//委托排名
struct l2_wt_rank_req
{
	short req;
	short setcode;
	short fltype;
	DWORD reserved;
};

struct l2_WTRankings
{
	char	Code[SH_CODE_LEN];	//证券代码
	float	TotalOrderQty;		//委托量
	float	TotalVolumeTraded;	//成交量
};

struct l2_wt_rank_ans
{
	struct l2_WTRankings list[20];
};

//即时撤单排名
struct l2_singlecancel_rank_req
{
	short req;
	short setcode;
	short fltype;
	DWORD reserved;
};

struct l2_SingleCancelRankings
{
	char	Code[SH_CODE_LEN];	//证券代码
	WORD	OrderSecond;		//委托时间(6点钟以后的秒)
	float	Quantity;			//委托量
	float	Price;				//价格
};

struct l2_singlecancel_rank_ans
{
	struct l2_SingleCancelRankings list[20];
};

//总撤单排名
struct l2_totalcancel_rank_req
{
	short req;
	short setcode;
	short fltype;
	DWORD reserved;
};

struct l2_TotalCancelRankings
{
	char	Code[SH_CODE_LEN];	//证券代码
	float	TotalWithdrawnQty;	//总撤单量
};

struct l2_totalcancel_rank_ans
{
	struct l2_TotalCancelRankings list[20];
};

//行业成交排名
struct l2_hy_rank_req
{
	short req;
	short setcode;
	DWORD reserved;
};

struct l2_HYRankings
{
	char	IndustryCode[8];			//行业代码
	float	IndustryTotalValueTraded;	//行业总成交额
};

struct l2_HYGPRankings
{
	char	Code[SH_CODE_LEN];			//证券代码
	float	TotalValueTraded;			//成交额
};

struct l2_hy_rank_ans
{
	float  MarketTotalValueTraded;
	struct l2_HYRankings HYlist[10];
	struct l2_HYGPRankings HYGPlist[10*5];
};

struct Server_MarketInfo
{
	struct l2_WTRankings WTRankings[20];
	struct l2_SingleCancelRankings SingleCancelRankings[20];
	struct l2_TotalCancelRankings TotalCancelRankings[20];
	float  MarketTotalValueTraded;
	struct l2_HYRankings	HYList[10];
	struct l2_HYGPRankings	HYGPList[10*5];
};	

//分时图相关
struct l2_zmzm_req
{
	short req;
	short setcode;
	char  code[SH_CODE_LEN];
};

struct l2_jgjc_req
{
	short req;
	short setcode;
	char  code[SH_CODE_LEN];
};

struct l2_newcjbs_req
{
	short req;
	short setcode;
	char  code[SH_CODE_LEN];
};

struct l2_newvolume_req
{
	short req;
	short setcode;
	char  code[SH_CODE_LEN];
};

struct l2_newtickstat_req
{
	short req;
	short setcode;
	char  code[SH_CODE_LEN];
};

#define MAX_AUCTIONNUM	500
struct l2_auction_req
{
	short req;
	short setcode;
	char  code[SH_CODE_LEN];
	long  ItemPos;	//希望得到的信息ID,从0起,合法ItemPos<ItemNum
	long  ItemNum;	//可以接收到的个数
};

#define MAX_DEEPWARN		100
struct deepwarn_req
{
	short req;
	short setcode;
	long  ItemPos;	//希望得到的信息ID,从0起,合法ItemPos<ItemNum
	long  ItemNum;	//可以接收到的个数
};

////////////////////////////////////////////////////////////////////////////////////

//行情版面请求包(10)
#define HQ_ASSISID			10
//直接发送(40-99)
#define HQALIVE_ASSISID		40
#define JUSTCURRHQ			41
#define STATUSZSHQ			42
#define WARNGPHQ_ASSISID	43
#define REFRESHHQ_ASSISID	44
#define SZ_DWARN_ASSISID	45
#define SH_DWARN_ASSISID	46
#define OLD_BIGVOL_ASSISID	47
#define IMGSTATHQ_ASSISID	48
#define TCKSTATHQ_ASSISID	49
#define MOREINFO			50

//前一个请求应答后才发送(100-255)
#define JBM_ASSISID			100
#define JBMLEN_ASSISID		101
#define XXM_ASSISID			102
#define XXMLEN_ASSISID		103
#define XXMTITLE_ASSISID	104
#define JGDATALEN_ASSISID	105
#define JGDATA_ASSISID		106
#define SZSTOCKNUM_ASSISID	107
#define SHSTOCKNUM_ASSISID	108
#define SZSTOCKINF_ASSISID	109
#define SHSTOCKINF_ASSISID	110
#define URLURGENT_ASSISID	111

#define INMODE2_ASSISID		116
#define AUTOBASE_ASSISID	117
#define AUTOGBBQ_ASSISID	118
#define BLOCKDATLEN_ASSISID	119
#define BLOCKDAT_ASSISID	120
#define ZHBZIPLEN_ASSISID	121
#define ZHBZIP_ASSISID		122
#define NEEDAUTH_ASSISID	123

#define OTHERFILELEN_ASSISID	124
#define OTHERFILE_ASSISID		125
#define	SYNCDATALEN_ASSISID		126
#define	SYNCDATA_ASSISID		127
#define	IWTITLE_ASSISID			128

#define SIMHQ_ASSISID			133
#define WIDEHQ_ASSISID			134
#define SIMFX_ASSISID			135
#define BKDATA_ASSISID			136
#define DOWN_MIN_ASSISID		137
#define DOWN_JBMLEN_ASSISID		138
#define DOWN_JBM_ASSISID		139
#define DATADIG_ASSISID			140
#define DOWNBAKHOST_ASSISID		141
#define DOWN_FXMIN_ASSISID		142
#define	DOWN_EXDAYFILE_ASSISID	143

#define URGENT_ASSISID		145
#define AD_ASSISID			146
#define HOSTMORE_ASSISID	147
#define JYMM_ASSISID		148
#define RXXMCFG_ASSISID		149
#define ZHSORT_ASSISID		150  //应小于255
#define JBMCFG_ASSISID		151  //应小于255
#define	HKSEATQ_ASSISD		152

#define GETPASSPORT_ASSISID	153  //得到通行证

#define NEW_JBMCFG_ASSISID	155
#define NEW_JBMTXT_ASSISID	156
#define DOWN_NEW_JBMCFG_ASSISID	157
#define DOWN_NEW_JBMTXT_ASSISID	158
#define TICKRANGEHIS_ASSIST	163
#define TICKRANGECUR_ASSIST	164

#define	XXGTITLE_ASSISID	166
#define	XXGTXT_ASSISID		167

#define BIWARNQ_ASSISID		171

#define	INFOUPLOAD_ASSISID	180
//请求包缓冲(255以上)

#endif

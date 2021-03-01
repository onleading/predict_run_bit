#ifndef	_FOREX_PROTOCOL_DEFINE_
#define	_FOREX_PROTOCOL_DEFINE_

typedef	struct tagCodeMapInfo
{
	BYTE Market;
	char Code[CODE_SIZE];
}CODEMAPINFO, *PCODEMAPINFO;

struct CodeStrCmp
{
	BOOL operator()(const CODEMAPINFO c1, const CODEMAPINFO c2) const
	{
		if(c1.Market!=c2.Market)
			return c1.Market - c2.Market < 0;
		else return lstrcmp(c1.Code, c2.Code) < 0;
	}
};

/////////////////////////////////////////////////////////////////////////////
//时间配置,必须与服务器同步定义

#define MAX_TIMESEC			4
#define MAX_DTFNUM			10

typedef	struct	tagTimeInfo	//	时间属性配置
{
	BYTE	Market;						//	市场类别
	char	strMask[4];					//	特征代码
	char	TimeZero;					//	时区
	short	OpenTimer[MAX_TIMESEC];		//	开市时间
	short	CloseTimer[MAX_TIMESEC];	//	收市时间
	short	InitTimer;					//	初始化时间
	short	EndTimer;					//	收盘时间
}TIMEINF,*PTIMEINF;

typedef	struct	tagMktInfo	// 市场配置
{
	BYTE	BigMarket;
	char	Name[MRK_NAME_SIZE];
	char	Code[MRK_CODE_SIZE];
	BYTE	MktType;
	char	Jc[MRK_JC_SIZE];
	char	unused1[10];
	short	TimeZero;
	BYTE	DocNum;
	short	DelayHQ;
	char	unused2[7];
}MKTINFO,*PMKTINFO;

typedef struct tagDSStkInfo	// 码表信息
{
	BYTE	BigMarket;					//  大类型
	BYTE	setcode;					//	分类市场类别
	BYTE	DocNum;						//	小数位数
	WORD	Type;						//  品种属性
	char	Code[CODE_SIZE];			//	代码
	char	Name[NAME_SIZE];			//	中文简称
	char	SpellCode[SPELLCODE_SIZE];	//	英文简称

	float	Unit;						//  行情与日线单位量倍数,港股为1,港股指数为万分之一,AB股按对应规则
	float	AmoUnit;					//  行情与日线单位金额倍数,港股为1,港股指数为百万分之一,AB股按对应规则
	BYTE	IsNeedJustCJL;				//  是否需要调节成交量
	BYTE	Reserved;					//  保留

	long	nIDIndex;					//	全局索引号
	short	TimerID;					//	对应时间ID
	
	long	QHVol_BaseRate;				//  期货的每手对应最小单位的倍数(每手乘数)
	short	QHPrice_MinRate;			//  期货的最小变动价位(乘100)
	char	QHPrice_UPDownRate;			//	扩展行情的涨停限制比例,0表示不处理,5表示5%
	char	Currency;					//	货币单位(备用)
}STKINFO,*PSTKINFO;


/////////////////////////////////////////////////////////////////////////////
const	int	SERVER_INFO			=	9200;	//	取得服务信息
const	int	TIME_INFO			=	9203;	//	商品时间设置
const	int	MARKET_INFO			=	9204;	//	市场属性请求
const	int	CODE_LIST_INFO_5	=	9205;	//  码表信息,只支持五个买卖盘的客户端版本

const	int	NOW_CURR_DATA		=	9210;	//	最新单个行情请求
const	int	MUL_CURR_DATA		=	9211;	//	最新多个行情请求
const	int	TICK_DATA			=	9212;	//	Tick数据请求
const	int	MIN_DATA			=	9213;	//	分钟数据请求
const	int	HQB_DATA			=	9214;	//	行情排名请求,不推
const	int	HIS_DATA			=	9215;	//	历史数据请求
const	int	PUSH_HQB_DATA		=	9216;	//	行情排名请求,推
const	int	FJB_DATA			=	9217;	//	分价表请求
const	int	SEAT_DATA			=	9218;	//	券商队列
const	int	JYHQINFO_DATA		=	9221;	//	某市场交易相关行情,每次最多能请求200只
const	int	HISTICK_DATA		=	9222;	//	历史tick
const	int	HISMIN_DATA			=	9223;	//	历史分钟线
const	int	HISTICK2_DATA		=	9224;	//	历史tick2
const	int	ZHSORT_DS_NREQ		=	9225;	//	综合排名
const	int	MUL_CURR_DATA_HQB	=	9226;	//	最新多个行情请求(行情表专用)

const	int MIN2_DATA			=	9227;	//	分钟数据请求(小结构)
const	int	HISMIN2_DATA		=	9228;	//	历史分钟线(小结构)
const	int HISFX_DATA			=	9229;	//  历史数据下载

const	int	DS_LOGIN_REQ		=	9300;	//登录信息
const	int	DS_OTHERINFO_REQ	=	9301;	//内部窥视信息
const	int	DS_FILELEN_REQ		=	9304;	//文件长度
const	int	DS_FILE_REQ			=	9305;	//下载文件

const	int	DS_F10CFG_REQ		=	9400;   //请求多功能F10的配置信息
const	int	DS_F10TXT_REQ		=	9401;	//请求多功能F10的内容信息

const	int	DS_SEATQUERY_REQ	=	9420;	//请求经纪席位的查询信息

const	int	DS_ALLHQCODE_REQ	=	9500;	//请求所有股票的简约代码
const	int	DS_ALLHQINFO_REQ	=	9501;	//请求所有股票的简约行情
const	int	DS_HOSTMORE_REQ		=	9502;	//日期信息 

/////////////////////////////////////////////////////////////////////////////

typedef	struct	tagReqNull
{
	short	ReqID;
}REQNULL,*PREQNULL;

struct ds_login_req
{
	short	req;
	char	qsid[11];		//	商用客户标识
	char	user[50];		//	用户名
	char	pwd[20];		//	密码
	char	productfl;		//	客户端类型
	float	clientver;		//	客户端版本
	char	istdxuser;		//  是不是通达信的版本用户
	char	unused[7];
	short	magicshort1;	//  第一个MagicShort
};

struct ds_login_req_t
{
	short	req;
	char	user[50];		//	用户名
	char	pwd[20];		//	密码
	char	productfl;		//	客户端类型
	float	clientver;		//	客户端版本
	char	istdxuser;		//  是不是通达信的版本用户
	char	unused[2];
	short	magicshort1;	//  第一个MagicShort
};

struct ds_login_ans
{
	char	Sucflag;			//0:失败 1:成功
	char	unused1[2];
	char	errmsg[50];			//错误信息
	struct bc_date date;		//服务器日期
	struct bc_time time;		//服务器时间
	char	ds_qsid[21];		//主站的名称
	float	hostver;			//主站版本
	char	bMustSameQSID;		//是否必须是相同的券商ID
	int 	Authority;
	char	unused2[212];
};

struct ds_otherinfo_req
{
	short req;
};

struct ds_otherinfo_ans
{
	long ClientNum;			//当前在线人数
	long MaxConnectNum;		//允许的最大连接数
	long HisMaxClientNum;	//历史上出现的最大在线数
	long UsedClientNum;		//暂无效
	
	char starttime[25];		//服务开始时间串
	char HostVer[30];
	char ProtocolVer;
	long nCanMaxStockNum;	//支持的最大品种数
	long nStockNum;			//当前的品种数

	long  machine_date;		//服务器日期
	long  machine_time;		//服务器时间
	char  unused1[10];
	long  nSyncDirNum;		//同步目录数
	char  bSSDS;			//是否是实时站

	char unused2[224];

	char bCompress;			//压缩方式 0:不压缩 1:压缩
	
	short ProcessNum;		//进程数
	short ThreadNum;		//线程数
	
	char  bExceedNumCfg;	//是否超过股票设置数

	char  nHKType;			//0不支持返回恒指量，1返回延时成交量 2:返回实量成交量 HSI
	long  nHKZSVol;		
	char  nIFType;			//0不支持返回IF连3量，1返回延时成交量 2:返回实量成交量 IFL3
	long  nIFLVol;		
	char  nCUType;			//0不支持返回Cu连3量，1返回延时成交量 2:返回实量成交量 CUL3
	long  nCULVol;		

	char  nDTFConStatus[MAX_DTFNUM];
							//MAX_DTFNUM路DTF的连接状态,0表示不支持此路,1表示断开,2表示连接
	char  unused3[4];
};

//////////////////////////////////////////////////////////////////////////
typedef	struct	tagMktInfAns	//	市场属性请求
{
	short	MktNum;
	MKTINFO	MktInf[1];
}MKTINFANS,*PMKTINFANS;

/////////////////////////////////////////////////////////////////////////////
typedef	struct	tagSrvInfoAns	//	服务器信息请求
{
	char	QSID[11];
	long	IndexVer;	//	码表版本号
	long	TimerVer;	//	时间设置版本号
	long	IndexNum;	//	码表数目
	long	TimerNum;	//	时间设置数目
	long	WorkType;	//	运行方式
}SRVINFOANS,*PSRVINFOANS;

/////////////////////////////////////////////////////////////////////////////
typedef	struct	tagStkInfReq	//	码表信息
{
	short	ReqID;	//	请求号
	long	Offset;	//	偏移位置
	short	StkNum;	//	数目
}STKINFREQ,*PSTKINFREQ;

typedef	struct	tagStkInfAns
{
	long		Offset;
	short		StkNum;
	STKINFO	StkInfo[1];
}STKINFANS,*PSTKINFANS;

/////////////////////////////////////////////////////////////////////////////
typedef	struct	tagTimeInfReq	//	时间属性
{
	short	ReqID;
	long	Offset;
	short	TimeNum;
}TIMEINFREQ,*PTIMEINFREQ;

typedef	struct	tagTimeInfAns
{
	long		Offset;
	short		TimeNum;
	TIMEINF	TimeInf[1];
}TIMEINFANS,*PTIMEINFANS;

//////////////////////////////////////////////////////////////////////////
typedef	struct	tagICurrDataReq	//	最新行情请求
{
	short	ReqID;
	BYTE	Market;
	char	Code[CODE_SIZE];
}ICDREQ,*PICDREQ;

typedef	struct	tagICurrDataExAns
{
	char	buf[1];
}ICDEXANS,*PICDEXANS;

/////////////////////////////////////////////////////////////////////////////
typedef	struct	tagMulIdx
{
	BYTE	Market;
	char	Code[CODE_SIZE];
}MULIDX,*PMULIDX;

typedef	struct	tagMulDataReq	//	多个行情请求
{
	short	ReqID;
	WORD	StkNum;
	MULIDX	MulIdx[1];
}MULDATAREQ,*PMULDATAREQ;

typedef	struct	tagSortReq		//	行情请求排序
{
	short	ReqID;
	BYTE	Market;
	short	SortID;
	WORD	From;
	WORD	SortNum;
	WORD	SortType;
}SORTREQ,*PSORTREQ;

typedef	struct	tagMulDataExAns
{
	WORD	StkNum;
	char	buf[1];
}MULDATAEXANS,*PMULDATAEXANS;

/////////////////////////////////////////////////////////////////////////////
typedef	struct	tagTickReq		//	分笔信息
{
	short	ReqID;
	BYTE	Market;
	char	Code[CODE_SIZE];
	DWORD	Offset;
	WORD	TickNum;
}TICKREQ,*PTICKREQ;

typedef	struct	tagTickAns
{
	BYTE	Market;
	char	Code[CODE_SIZE];
	DWORD	Offset;
	WORD	TickNum;
	TICKDATA	iTick[1];
}TICKANS,*PTICKANS;

typedef	struct	tagTick2Ans
{
	BYTE	Market;
	char	Code[CODE_SIZE];
	long	nDate;			//日期
	float	fClose;			//前收
	long	Volin;			//结算价
	DWORD	Offset;
	WORD	TickNum;
	TICKDATA	iTick[1];
}TICK2ANS,*PTICK2ANS;

/////////////////////////////////////////////////////////////////////////////
typedef	struct	tagHisDataReq	//	历史数据
{
	short	ReqID;
	BYTE	Market;
	char	Code[CODE_SIZE];
	WORD	Period;
	WORD	MulNum;
	DWORD	Offset;
	WORD	DataNum;
}HISDATAREQ,*PHISDATAREQ;

typedef	struct	tagHisDataAns
{
	BYTE	Market;
	char	Code[CODE_SIZE];
	WORD	Period;
	WORD	MulNum;
	DWORD	Offset;
	WORD	DataNum;
	ANALYDATA	iData[1];
}HISDATAANS,*PHISDATAANS;
//////////////////////////////////////////////////////////////////////////
typedef	struct	tagHisFxDataReq	//历史数据下载
{
	short	ReqID;
	BYTE	Market;
	char	Code[CODE_SIZE];
	WORD	Period;
	long	startdate;
	long	enddate;
}HISFXDATAREQ,*PHISFXDATAREQ;

typedef	struct	tagHisFxDataAns
{
	BYTE	Market;
	char	Code[CODE_SIZE];
	WORD	Period;
	WORD	DataNum;
	ANALYDATA	iData[1];
}HISFXDATAANS,*PHISFXDATAANS;
/////////////////////////////////////////////////////////////////////////////
typedef	struct	MinuteData_DS	//	分钟数据
{
	short	Minute;			//	零点以来的分钟数
	float	Now;			//	现价
	float	Average;		//	均价,动态结算价(期货)
	DWORD	NowVol;			//	现手,现成交金额(100元)
	DWORD	VolInStock;		//	持仓量(期货)
}MINDATA_DS,*PMINDATA_DS;

typedef	struct	tagMinDataReq	//	分钟数据
{
	short	ReqID;
	BYTE	Market;
	char	Code[CODE_SIZE];
}MINDATAREQ,*PMINDATAREQ;

typedef	struct	tagMinDataAns
{
	BYTE		Market;
	char		Code[CODE_SIZE];
	WORD		DataNum;
	char		iData[1];
}MINDATAANS,*PMINDATAANS;

typedef	struct	tagHisTickReq		//	分笔信息
{
	short	ReqID;
	long	nDate;				//日期
	BYTE	Market;
	char	Code[CODE_SIZE];
	DWORD	Offset;
	WORD	TickNum;
}HISTICKREQ,*PHISTICKREQ;

typedef	struct	tagHisMinDataReq	//	历史分钟数据
{
	short	ReqID;
	long	nDate;				//日期
	BYTE	Market;
	char	Code[CODE_SIZE];
}HISMINDATAREQ,*PHISMINDATAREQ;

typedef	struct	tagHisMinDataAns
{
	BYTE		Market;
	char		Code[CODE_SIZE];
	long		nDate;				//日期
	float		fClose;				//前收
	WORD		DataNum;
	char		iData[1];
}HISMINDATAANS,*PHISMINDATAANS;

//////////////////////////////////////////////////////////////////////////
typedef struct tagFJBDataReq
{ 
	short	req;
	BYTE	Market;
	char	Code[CODE_SIZE];
}FJBDATAREQ,*PFJBDATAREQ;

typedef	struct tagFJBDataAns	//分价表信息
{ 
	BYTE	Market;
	char	Code[CODE_SIZE];
	short num;
	PriceVolTable idx[1];
}FJBDATAANS,*PFJBDATAANS;

//////////////////////////////////////////////////////////////////////////
//买卖队列
struct Seat_Req
{
	short	ReqID;
	BYTE	BSFlag;	//0:买队列 1:卖队列
	BYTE	WantNum;
	BYTE	Market;
	char	Code[CODE_SIZE];
};

struct	Seat_Content
{
	char SeatNo[7];
	char SeatStr[31];
};

struct Seat_Ans
{
	BYTE			Market;
	char			Code[CODE_SIZE];
	BYTE			BSFlag;
	BYTE			NumB;
	BYTE			NumS;
	Seat_Content	List[1];
};

struct ds_filelen_req
{  
	short req;
	char  fname[40];
};

struct ds_filelen_ans
{
   DWORD filelen;
   char  usemdstr;
   char	 mdstr[33];
};

struct ds_file_req
{  
	short req;
	DWORD flag;				//标识特殊属性
	DWORD pos;
	char  fname[40];
};

struct ds_wz_ans
{ 
	DWORD flag;				//标识特殊属性
	short num;
	char  buf[1];
};

struct ds_f10cfg_req
{
	short req;
	short setcode;
	char code[SH_CODE_LEN];
	short whichjbm;
	short reserved;
};

struct ds_f10cfg_title
{
	char sTitle[64];
	char sFilePath[80];
	long nOffset;
	long nLength;
};

struct ds_f10cfg_ans
{
  short titlenum;
  struct ds_f10cfg_title list[1];
};

struct ds_f10txt_req
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

struct ds_f10txt_ans
{
	short setcode;
	char code[SH_CODE_LEN];
	short which;
	short num;
	char  buf[1];
};

struct ds_hostmore_req
{
	short req;
	char  verflag;	
};

struct ds_hostmore_ans
{
	struct bc_date date;
};
//////////////////////////////////////////////////////////////////////////
//经纪席位查询
typedef	struct _SEATQUERYINFO 
{
	BYTE	Market;
	char	Code[CODE_SIZE];
	short	nSeatItem;								//席位号
	short	nSeatType;								//席位方位
	float	fPrice;									//席位价格
}SEATQUERYINFO;

typedef	struct _GETSEATQUERY_REQ
{
	short		iRetCode;							//返回信息
	int			nReqNum;							//返回多个席位号查询
	short		nSeatItems[1];
}GETSEATQUERY_REQ;

typedef	struct _GETSEATQUERY_ANS 
{
	int				nInfoNum;						//返回个数,暂时用1个
	SEATQUERYINFO	cSeatItem[1];
}GETSEATQUERY_ANS;

/////////////////////////////////////////////////////////////////////////////

//直接发送
#define DS_HQALIVE_ASSISID			40
//前一个请求应答后才发送(100-255)
#define DS_LOGIN_ASSISID			101
#define DS_SERVER_INFO_ASSISID		102
#define DS_CODE_LIST_INFO_ASSISID	103
#define DS_MARKET_TIMER_ASSIDID		104
#define	DS_MKT_INFO_ASSIDID			105
#define DS_REQFILELEN_ASSISID		108
#define	DS_REQFILE_ASSISID			109

#endif
/////////////////////////////////////////////////////////////////////////////

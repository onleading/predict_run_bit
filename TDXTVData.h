#ifndef __TDX__TOPVIEW__DATA__H__
#define __TDX__TOPVIEW__DATA__H__

#pragma pack(push,1)
#pragma warning(disable:4200)

#define COMMON_MARKETCODE_MAXLEN		11
typedef char SCommonMarketCode[COMMON_MARKETCODE_MAXLEN];

#define TDP_DAY			(BYTE(1))
#define TDP_WEEK		(BYTE(2))
#define TDP_MONTH		(BYTE(3))

//#define TDT_SEAT		(WORD(0000))	//席位信息
#define TDT_0301		(WORD(301))		//个股/内外盘
#define TDT_1013		(WORD(1013))	//个股/持股/分类账户
#define TDT_1001		(WORD(1001))	//个股/持股/区间
#define TDT_0401		(WORD(401))		//个股/买卖/营业部排序
#define TDT_0801		(WORD(801))		//营业部/买卖/市场
#define TDT_0101		(WORD(101))		//个股/成交
#define TDT_0501		(WORD(501))		//个股/买卖/分类帐户
#define TDT_0502		(WORD(502))		//市场/买卖/分类帐户

#define TDT_SHHQ000001_0401	(WORD(9999))		//上证指数/买卖/营业部排序
#define TDT_ZXW_EX		(WORD(9900))	// _TopView_ZXW

struct STDCommonHead
{
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	char	data[0];
};

struct STDHoriHead
{
	SCommonMarketCode	marketCode;
	char	data[0];
};

struct STDVertHead
{
	DWORD	dwDate;	
	char	data[0];
};

// 合并的报表日期信息
struct SUnionDateItem
{
	DWORD			dwDate;			// 日期
	DWORD			dwType;			// 类型    TDP_DAY, TDP_WEEK, TDP_MONTH
};

//1.	_TopView_BS_XXX个股内外盘交易统计信息03_01
//分三个周期--日、周、月。对应03_01
struct STD0301BaseItem
{
	float	fTotAmt;			//统计周期内的总成交金额
	float	fBuyAmt;			//统计周期内的内盘成交金额	实为主卖
	float	fSellAmt;			//统计周期内的外盘成交金额  实为主买
};
struct STD0301Item
{
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	STD0301BaseItem	base;
};
//2.	_TopView_C_XXX  A股股东分类帐户持股统计10_13
//分三个周期--日、周、月。对应10_13
//帐户分4类--A、B、D、F。具体含意每一类帐户的含义问问陈宽余，我也不清楚。
struct STD1013BaseItem
{
	float	fMktVol;			//流通A股股本，即流通盘
	int		nTotAcctCount;		//流通A股帐户总数，即股东总数
	float	fAHold;				//A类帐户持股数量
	int		nAAcctCount;		//A类帐户数
	float	fBHold;				//B类帐户持股数量
	int		nBAcctCount;		//B类帐户数
	float	fDHold;				//D类帐户持股数量
	int		nDAcctCount;		//D类帐户数
	float	fFHold;				//F类帐户持股数量
	int		nFAcctCount;		//F类帐户数
};
struct STD1013Item
{
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	STD1013BaseItem	base;
};
//3.	_TopView_I_XXX  A股股东持股区间结构统计10_01
//分三个周期--日、周、月。对应10_01
struct STD1001BaseItem 
{
	float	fMktVol;			//流通A股股本，即流通盘
	int		nTotAcctCount;		//流通A股帐户总数，即股东总数
	int		nK1AcctCount;		//持股数<1k股的帐户数
	float	fK1Hold;			//持股数<1k股的帐户的持股总数
	int		nK10AcctCount;		//持股数在[1k，10k]股的帐户数
	float	fK10Hold;			//持股数在[1k，10k]股的持股总数
	int		nK50AcctCount;		//持股数在[10k，50k]股的帐户数
	float	fK50Hold;			//持股数在[10k，50k]股的持股总数
	int		nK100AcctCount;		//持股数在[50k，100k]股的帐户数
	float	fK100Hold;			//持股数在[50k，100k]股的持股总数
	int		nK500AcctCount;		//持股数在[100k，500k]股的帐户数
	float	fK500Hold;			//持股数在[100k，500k]股的持股总数
	int		nM1AcctCount;		//持股数在[500k，1M]股的帐户数
	float	fM1Hold;			//持股数在[500k，1M]股的持股总数
	int		nM5AcctCount;		//持股数在[1M，5M]股的帐户数
	float	fM5Hold;			//持股数在[1M，5M]股的持股总数
	int		nM10AcctCount; 		//持股数在[5M，10M]股的帐户数
	float	fM10Hold;			//持股数在[5M，10M]股的持股总数
	int		nGt10MAcctCount; 	//持股数在[10M，∞]股的帐户数
	float	fGt10MHold;			//持股数在[10M，∞]股的持股总数
};
struct STD1001Item 
{
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	STD1001BaseItem base;
};
//4.	_TopView_S_XXX个股买卖量Top10营业部统计04_01
//分三个周期--日、周、月。对应04_01
struct STD0401BaseItem
{
	BYTE	rankType;		//排序类型-1:买入金额；2:卖出金额；3:净买入金额；4:净卖出金额
#define TOPVIEW_SEAT_RANKTYPE_Buy			1	// 买入
#define TOPVIEW_SEAT_RANKTYPE_Sell			2	// 卖出
#define TOPVIEW_SEAT_RANKTYPE_NetBuy		3	// 净买入
#define TOPVIEW_SEAT_RANKTYPE_NetSell		4	// 净卖出
	BYTE	Rank;			//序号
	DWORD	dwSeatId;		//营业部ID
	float	fTotAmt;		//股票总成交额
	float	fBuyAmt;		//营业部买入金额
	float	fSellAmt;		//营业部卖出金额
};
struct STD0401Item
{
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	STD0401BaseItem	base;
};

//转置之后，大小减少60％
struct STD0401BaseBSItemEx
{
	DWORD	dwSeatId:24;		//营业部ID
	float	fBuyAmt;		//营业部买入金额
	float	fSellAmt;		//营业部卖出金额
};
struct STD0401BaseItemEx
{
	float	fTotAmt;		//股票总成交额
	STD0401BaseBSItemEx		bs[40];	//分别是10个买入，10个卖出，10个净买入，10个净卖出
};
struct STD0401ItemEx
{
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	STD0401BaseItemEx	base;
};

//5.	_TopView_S_Seat 营业部名称表
//营业部名称表，没有统计周期属性，_TopView_S_XXX、_TopView_Seat_Trade_XXX中用到的SeatId都应该关联这个表。
struct STDSeatItem
{
	DWORD	dwSeatId;			//营业部ID，指南针内部分配
	char	szSeatCode[21];		//营业部编号，上证所信息公司分配
	char	szSeatName[101];	//营业部名称
};


struct STDSeatExItem
{
	DWORD	dwSeatId;			//营业部ID，指南针内部分配
	char	szSeatCode[21];		//营业部编号，上证所信息公司分配
	char	szSeatName[101];	//营业部名称
	char	szMemberCode[21];	//会员Code
};

//6.	_TopView_Seat_Trade_XXX 营业部交易统计08_01
//分三个周期--日、周、月。对应08_01

struct STD0801BaseItem
{ 	
	float	fABuy;			//营业部A股买入金额
	float	fASell;			//营业部A股卖出金额
	float	fBBuy;			//营业部B股买入金额
	float	fBSell;			//营业部B股卖出金额
	float	fFondBuy;		//营业部基金股买入金额
	float	fFondSell;		//营业部基金卖出金额
	float	fTbondBuy;		//营业部国债买入金额
	float	fTbondSell;		//营业部国债卖出金额
	float	fCbondBuy;		//营业部企业债买入金额
	float	fCbondSell;		//营业部企业卖出金额
	float	fCvtBuy;		//营业部可转债买入金额
	float	fCvtSell;		//营业部可转债卖出金额
	float	fBondBuy;		//营业部债券买入金额
	float	fBondSell;		//营业部债券卖出金额
	float	fRepurBuy;		//营业部回购买入金额
	float	fRepurSell;		//营业部回购卖出金额
};

struct STD0801Item
{ 	
	SCommonMarketCode	szSeatId;//marketCode;
						//为保持数据结构一致性，把这个改成代码 dwSeatId=atol(szSeatId),
						//szSeatId = Format("%d",dwSeatId);
//	DWORD	dwSeatId;			//营业部ID
 	DWORD	dwDate;
	STD0801BaseItem	base;
};

//7.	_TopView_Stk_Trade_XXX 个股成交统计01_01
//分三个周期--日、周、月。对应01_01
struct STD0101BaseItem
{ 
	int		nTradeCount;	//股票统计周期内的总成交笔数
	float	fTq;			//A股流通盘
	float	fVol;			//A股成交量
	float	fAmt;			//A股成交额
};
struct STD0101Item
{ 
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	STD0101BaseItem	base;
};
//10.	_TopView_Stk_C_Trade_XXX 个股分类帐户买卖统计
//分三个周期--日、周、月。对应05_01
//帐户分4类--A、B、D、F。具体含意每一类帐户的含义问问陈宽余。
struct STD0501BaseItem
{
	float	fVol;			//统计期间总成交量
	float	fAmt;			//统计期间总成交额
	float	fABuyVol;		//A类帐户总买入量
	float	fABuyAmt;		//A类帐户总买入金额
	float	fASellVol;		//A类帐户总卖出量
	float	fASellAmt;		//A类帐户总卖出金额
	float	fBBuyVol;		//B类帐户总买入量
	float	fBBuyAmt;		//B类帐户总买入金额
	float	fBSellVol;		//B类帐户总卖出量
	float	fBSellAmt;		//B类帐户总卖出金额
	float	fDBuyVol;		//D类帐户总买入量
	float	fDBuyAmt;		//D类帐户总买入金额
	float	fDSellVol;		//D类帐户总卖出量
	float	fDSellAmt;		//D类帐户总卖出金额
	float	fFBuyVol;		//F类帐户总买入量
	float	fFBuyAmt;		//F类帐户总买入金额
	float	fFSellVol;		//F类帐户总卖出量
	float	fFSellAmt;		//F类帐户总卖出金额
};
struct STD0501Item
{
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	STD0501BaseItem	base;
};

//11.	_TopView_IDX_C_Trade_XXX 分类市场分类帐户买卖统计
//分三个周期--日、周、月。对应05_02
//帐户分4类--A、B、D、F。具体含意每一类帐户的含义问问陈宽余。
struct  STD0502BaseItem
{
	float	fATotVol;		//A类帐户分类市场总成交量
	float	fATotAmt;		//A类帐户分类市场总成交额
	float	fABuyVol;		//A类帐户分类市场总买入量
	float	fABuyAmt;		//A类帐户分类市场总买入金额
	float	fASellVol;		//A类帐户分类市场总卖出量
	float	fASellAmt;		//A类帐户分类市场总卖出金额
	float	fBTotVol;		//B类帐户分类市场总成交量
	float	fBTotAmt;		//B类帐户分类市场总成交额
	float	fBBuyVol;		//B类帐户分类市场总买入量
	float	fBBuyAmt;		//B类帐户分类市场总买入金额
	float	fBSellVol;		//B类帐户分类市场总卖出量
	float	fBSellAmt;		//B类帐户分类市场总卖出金额
	float	fDTotVol;		//D类帐户分类市场总成交量
	float	fDTotAmt;		//D类帐户分类市场总成交额
	float	fDBuyVol;		//D类帐户分类市场总买入量
	float	fDBuyAmt;		//D类帐户分类市场总买入金额
	float	fDSellVol;		//D类帐户分类市场总卖出量
	float	fDSellAmt;		//D类帐户分类市场总卖出金额
	float	fFTotVol;		//F类帐户分类市场总成交量
	float	fFTotAmt;		//F类帐户分类市场总成交额
	float	fFBuyVol;		//F类帐户分类市场总买入量
	float	fFBuyAmt;		//F类帐户分类市场总买入金额
	float	fFSellVol;		//F类帐户分类市场总卖出量
	float	fFSellAmt;		//F类帐户分类市场总卖出金额
};
struct  STD0502Item
{
	SCommonMarketCode	marketCode;
	DWORD	dwDate;
	STD0502BaseItem	base;
};

struct STDRightItem
{
	DWORD dwTypeId;			//扩展信息类型 在 DB_TextInfo.dbo._InfoServer_ExpandInfo 中有定义
	DWORD dwClientType;	    //客户端类型
	DWORD dwProductId;      //产品ID
	DWORD dwDelayDay;		//对于高登数据,数据延迟时间
	DWORD dwDataType;		//高登数据类型
};


struct STDZXWExBaseItem
{
	float		fZXWBuy;			// 主席位买入额（万）
	float		fZXWSell;			// 主席位卖出额（万）
	float		fFundBuy;			// 基金买入额
	float		fFundSell;			// 基金卖出额
	float		fCompanyBuy;		// 营业部买入额
	float		fCompanySell;		// 营业部卖出额
	float		fOthersBuy;			// 资管买入额
	float		fOthersSell;		// 资管卖出额
	float		fDaPanJC;			// 大盘主力资金进出
};
struct STDZXWExItem
{
	SCommonMarketCode	marketCode;
	DWORD				dwDate;
	STDZXWExBaseItem	base;
};

#pragma warning(default:4200)	//#pragma warning(disable:4200)
#pragma pack(pop)

interface	IGeneric
{
	virtual	void*	CALLBACK	QueryInterface(DWORD dwID)=0;
};

interface IZxwDataManager : public IGeneric
{
	// 所有返回数据均为正向排序
	// 认为所有函数都是线程安全
	// 功能：取得指定类型、指定周期、指定日期的所有数据（多只股票同一天的数据）
	// 输入：dwDataType：数据类型    dwCycleType：数据周期   dwDate：日期信息  
	// 输出：void*：数据列表，可转换成相应类型，如下所示。以下函数皆同。
	// STDCommonHead
	// STD0301Item
	// STD1013Item
	// STD1001Item
	// STD0401ItemEx
	// STD0801Item
	// STD0101Item
	// STD0501Item
	// STD0502Item
	// pdwCount：可为NULL，非NULL时，为数据列表中数据个数
	// pdwReportDate：可以传NULL，非NULL时，返回实际返回的数据日期，可能与dwDate不等
	// IsGetLast 表示当前日期没有数据时,是否获取上一期数据
	virtual void* GetData(DWORD dwDataType, DWORD dwCycleType, DWORD dwDate, DWORD *pdwCount, BOOL IsGetLast = FALSE) = 0;

	// 功能：取得指定类型、指定周期、指定股票的所有数据（一只股票的多天数据）
	// 输入：dwDataType：数据类型    dwCycleType：数据周期   pszCode：股票的MarketCode  
	// 输出：void*：数据列表，可转换成相应类型    pdwCount：可为NULL，非NULL时，为数据列表中数据个数
	// 注：  如果dwDataType为DATA_SEAT_TRADE，则pszCode需要转换为席位ID，即dwSeatID = atol(pszCode)
	virtual void* GetData(DWORD dwDataType, DWORD dwCycleType, const char *pszMarketCode, DWORD *pdwCount) = 0;

	// 功能：取得指定类型、指定周期、指定股票的指定日期的数据（一只股票的某天数据）
	// 输入：dwDataType：数据类型    dwCycleType：数据周期   pszCode：股票的MarketCode   dwDate：日期信息
	// 输出：void*：数据列表，可转换成相应类型    pdwCount：可为NULL，非NULL时，为数据列表中数据个数
	//       pdwReportDate：可以传NULL，非NULL时，返回实际返回的数据日期，可能与dwDate不等
	// 注：  如果dwDataType为DATA_SEAT_TRADE，则pszCode需要转换为席位ID，即dwSeatID = atol(pszCode)
	// IsGetLast 表示当前日期没有数据时,是否获取上一期数据
	virtual void* GetData(DWORD dwDataType, DWORD dwCycleType, const char *pszMarketCode, DWORD dwDate, BOOL IsIsGetLast = FALSE) = 0;

	// 功能：取得指定类型、指定周期的数据报表之日期列表
	// 输入：dwDataType：数据类型    dwCycleType：数据周期   pdwCount：可为NULL，非NULL时，为数据列表中数据个数
	// 输出：日期列表
	virtual DWORD* GetReportList(DWORD dwDataType, DWORD dwCycleType, DWORD *pdwCount) = 0;

	// 功能：根据指定席位ID，取得席位名称
	// 输入：pszSeatId：席位ID
	// 输出：席位名称
	//		 FALSE表示席位Id错误
	virtual BOOL GetSeatName(const char *pszSeatId,char* pszSeatName,int nMaxSeatNameLen) = 0;

#define SEAT_GENERAL				0		// 普通席位
#define SEAT_FUND					1		// 基金席位
#define SEAT_REST					2		// 其他席位
	// 功能：判断指定ID的席位的席位类型
	// 输入：pszSeatId：席位ID
	// 输出：席位类型
	virtual BYTE GetSeatType(const char *pszSeatId) = 0;

	// 功能：取得指定类型的联合数据报表之日期（有可能日最新，有可能周最新，有可能月最新）
	// 输入：dwDataType：数据类型
	// 输出：联合日期链表
	//		返回的结果可以转换成SUnionDateItem结构
	virtual void* GetUnionReportList(DWORD dwDataType, DWORD *pdwCount) = 0;
};

typedef BOOL (CALLBACK *DllInit)(char* szInstallPath,DWORD dwPersonId,char* szPassword,char* szServerIp,DWORD dwServerPort,DWORD dwProvinceId);
typedef IZxwDataManager* (CALLBACK *DllGetZXWDataInterface)();
typedef void (CALLBACK *DllReceiveData)();
typedef void (CALLBACK	*DllRelease)();

//1、zxwdata.dll和tdxtvdata.dll保存在一个目录下。
//2、LoadLibrary(tdxtvdata.dll)
//3、调用DllInit初始化，
//	szInstallPath需要“\\”结尾，是数据保存目录,
//	dwPersonId 指南针给的卡号,
//	szPassword是密码，
//	szServerIp,dwServerPort是指南针服务器的IP，端口,
//	dwProvinceId是省份信息，接收数据的时候会参考这个来接收，各省份Id如下
// 		pcUndefine		=0,		//其它
// 		pcBeiJing		=1,		//北京
// 		pcShangHai		=2,		//上海
// 		pcTianJin		=3,		//天津
// 		pcChongQing		=4,		//重庆
// 		pcAnHui			=5,		//安徽
// 		pcFuJian		=6,		//福建
// 		pcGanSu			=7,		//甘肃
// 		pcGuangDong		=8,		//广东
// 		pcGuangXi		=9,		//广西
// 		pcGuiZhou		=10,	//贵州
// 		pcHaiNan		=11,	//海南
// 		pcHeBei			=12,	//河北
// 		pcHeNan			=13,	//河南
// 		pcHeiLongJiang	=14,	//黑龙江
// 		pcHuBei			=15,	//湖北
// 		pcHuNan			=16,	//湖南
// 		pcJiLin			=17,	//吉林
// 		pcJiangSu		=18,	//江苏
// 		pcJiangXi		=19,	//江西
// 		pcLiaoNing		=20,	//辽宁
// 		pcNeiMeng		=21,	//内蒙
// 		pcNingXia		=22,	//宁夏
// 		pcQingHai		=23,	//青海
// 		pcShanDong		=24,	//山东
// 		pcShan1Xi		=25,	//山西
// 		pcShan3Xi		=26,	//陕西
// 		pcSiChuan		=27,	//四川
// 		pcXinJiang		=28,	//新疆
// 		pcXiZang		=29,	//西藏
// 		pcYunNan		=30,	//云南
// 		pcZheJiang		=31		//浙江
//4、调用DllReceiveData接收数据
//5、DllGetZXWDataInterface取读数接口
//6、退出时候要调用DllRelease

#endif

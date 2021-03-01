#ifndef __LOCALINDEXMANAGE__H__
#define __LOCALINDEXMANAGE__H__

#define LOCALINDEX_MAXSPACE		200			//最多允许的自定义品种个数
#define	INDEXSETSPACE_STEP		20			//自定品种存储区缓存申请步长
//
#define	LCEXTFORMAT_SKIP		0
#define	LCEXTFORMAT_DATE		1
#define	LCEXTFORMAT_OPEN		2
#define	LCEXTFORMAT_HIGH		3
#define	LCEXTFORMAT_LOW			4
#define	LCEXTFORMAT_CLOSE		5
#define	LCEXTFORMAT_VOLUME		6
#define	LCEXTFORMAT_AMOUNT		7

#define	LCEXTDATE__M__D__Y		0			//MM/DD/YYYY
#define	LCEXTDATE__D__M__Y		1			//DD/MM/YYYY
#define	LCEXTDATE__Y__M__D		2			//YYYY/MM/DD
#define	LCEXTDATE_M_D_Y			3			//MM-DD-YYYY
#define	LCEXTDATE_D_M_Y			4			//DD-MM-YYYY
#define	LCEXTDATE_Y_M_D			5			//YYYY-MM-DD
#define	LCEXTDATEYMD			6			//YYYYMMDD

#define	LCEXTVOL_1P1			0
#define	LCEXTVOL_1P10			1
#define	LCEXTVOL_1P100			2
#define	LCEXTVOL_1P1000			3
#define	LCEXTVOL_1P10000		4

#define	LCEXTSPACE_COMMA		0
#define	LCEXTSPACE_SEM			1
#define	LCEXTSPACE_TAB			2
#define	LCEXTSPACE_SPACE		3
//
#define	INDEXSTKSPACE_STEP		50			//自定品种成分股缓存申请步长
#define	INDEXDAYSPACE_STEP		300			//日线数据缓存申请步长
#define CWDATASPACE_STEP		100			//财务数据缓存申请步长

#define LCIDXWEIGHT_FLOWCAP		0			//流通股本加权
#define LCIDXWEIGHT_ADJUSTCAP	1			//调整流通股本加权
#define LCIDXWEIGHT_TOTALCAP	2			//总股本加权
#define LCIDXWEIGHT_PRICE_NOTQ	3			//不加权价格权重
#define LCIDXWEIGHT_PRICE_EFTQ	4			//等比前复权价格权重
#define LCIDXWEIGHT_PRICE_EBTQ	5			//等比后复权价格权重
#define LCIDXWEIGHT_SAME		6			//等比复权同权重

#define NEWLCIDXSPACE(bRet,pt,spc,req,step,type) {\
	if((req)<=0) (bRet) = FALSE;\
	if((bRet) && ((pt)==NULL || (spc)<(req)))\
	{\
		TDELA(pt);\
		(spc) = 0;\
		(pt) = new type[(req)+(step)];\
		if((pt)==NULL) (bRet) = FALSE;\
		else (spc) = (req)+(step);\
	}\
}

#define CHECKLCIDXSPACE(bRet,pt,tmppt,spc,req,step,type) {\
	if((req)<=0) (bRet) = FALSE;\
	if((bRet) && ((pt)==NULL || (spc)<(req)))\
	{\
		(tmppt) = new type[(req)+(step)];\
		if((tmppt)==NULL) (bRet) = FALSE;\
		else\
		{\
			memset((tmppt),0,((req)+(step))*sizeof(type));\
			if((spc)>0 && (pt)!=NULL) memcpy((tmppt),(pt),(spc)*sizeof(type));\
			TDELA(pt);\
			(pt) = (tmppt);\
			(spc) = (req)+(step);\
		}\
	}\
}

//相关文件:退市信息文件(lcpts.lpi),导入信息文件(lcext.lei),组合信息文件(lcidx.lii),组合股票集(393XXX.lis)
//流程:读取lcpts.lpi,lcext.lei和lcidx.lii,进行审核并生成定制品种索引,初始化顺序号
//操作:增加:在综合索引和分类信息末尾添加,同时加到代码链末尾;删除:寻找在代码链和分类信息中的位置,分别删除,在综合索引中删除并修正顺序号
//
typedef struct LocalSecIndex		//			定制品种索引
{
	long	lType;					//			定制品种类别:1:组合品种;2:外部品种;3:退市品种
	char	Code[CODE_SIZE];	//			定制品种代码
	char	Name[NAME_SIZE];		//			简称
	long	lMemStkIdx;				//			定制品种在代码链中的位置
	long	lSubIndex;				//			定制品种在定制分类中的位置
}LCSECIDX,*LPLCSECIDX;
//
typedef struct LocalPtSet			//	320B	退市品种基本信息
{
	char	Code[CODE_SIZE];	//	7B		代码
	char	Name[NAME_SIZE];		//	9B		简称
	short	BaseSetcode;			//	2B		原始市场
	char	BaseCode[CODE_SIZE];//	7B		原始代码
	char	Reserved[295];			//	295B
}LCPTSET,*LPLCPTSET;
//
typedef struct LocalExtSet			//	320B	外部品种基本信息
{
	char	Code[CODE_SIZE];	//	7B		代码
	char	Name[NAME_SIZE];		//	9B		简称
	short	nDateFormat;			//	2B		日期格式
	short	nSpaceType;				//	2B		间隔符
	long	lSkipLines;				//	4B		忽略行数
	short	nFieldFormat[10];		//	20B		字段格式
	char	Reserved[18];			//	18B
	short	nVolRatio;				//	2B		成交量比例
	char	SrcFilePath[256];		//	256B	源文件路径
}LCEXTSET,*LPLCEXTSET;
//
typedef struct LocalIndexSet		//	320B	组合品种基本信息
{
	long	nRefreshedTime;			//	4B		是否需要刷新
	char	Code[CODE_SIZE];	//	7B		代码
	char	Name[NAME_SIZE];		//	9B		简称
	char	Author[32];				//	32B		设计人
	long	nHYID;					//			行业ID
	short	nUnitNum;				//	2B		商品个数
	short	QZType;					//	2B		加权方式:0-流通股权重,1-总股本权重,2-自定义权重,3-平等权重方式
	long	BaseDate;				//	4B		基准时间(实际数据时间从次日开始)
	float	BaseValue;				//	4B		基准值
	long	lGiveupDays;			//	4B		除权:同等权重和自定义权重有效
	char	Descript[252];			//	252B	说明
}LCIDXSET,*LPLCIDXSET;

typedef struct LocalIndexStock		//	16B		定制品种成分信息
{
	short	nSetCode;				//	2B		市场代码
	char	Code[CODE_SIZE];	//	7B		代码
	float	fWeight;				//	4B		自定义权重
	char	Reserved[3];			//	3B		保留
}LCIDXSTK,*LPLCIDXSTK;

typedef struct LocalIndexDayData	//	48B
{
	long	lDate;					//	4B		日期
	double	fOpen;					//	4B		加权开盘值
	double	fMax;					//	4B		加权最高值
	double	fMin;					//	4B		加权最低值
	double	fClose;					//	4B		加权收盘值
	double	fYClose;				//	4B		昨日收盘在今日的除权调整值(等比除权)
	double	fVolume;				//	4B		成交量
	double	fAmount;				//	4B		成交金额
	unsigned short	wAdvance;		//	2B		上涨家数
	unsigned short	wDecline;		//	2B		下跌家数
	double	fFlowCap;				//	4B		流通股本
	double	fTotalCap;				//	4B		总股本
	double	fFinalWeight;			//	4B		最终权重:流通股本/调整流通股本/总股本/1/1/1/1
}LCIDXDAY,*LPLCIDXDAY;

#include "cwcl.h"
//自定组合处理模块
//1.负责新增,删除,更新自定组合的具体操作
//2.负责自定组合的数值计算
//3.对自定组合的编辑,在编辑完成之后立即存盘
//4.对自定组合的数值计算,计算结束后立即存盘
//5.删除自定组合时,同时删除存盘数值
//6.修改自定组合时,计算并更新存盘数值
//7.下载数据后,需要重新计算
//8.自定组合数据个数按照上证综指的数据个数安排,个股数据计算完之后与之进行时间匹配,累加.
//9.若累加结果在某日为0,直接剔除

class LcIdxMng
{
protected:
	LPLCSECIDX	m_pLcSecIdx;		//综合索引
	long		m_LcSecIdxNum;
	///////////////////////////////////////////////////////////////////////////
	LPLCPTSET	m_pLcPtSet;			//退市品种信息
	long		m_LcPtSetNum;		//退市品种个数
	long		m_LcPtSetSpc;		//退市品种信息区大小(sizeof(LCPTSET)为单位)
	///////////////////////////////////////////////////////////////////////////
	LPLCEXTSET	m_pLcExtSet;		//外部品种信息
	long		m_LcExtSetNum;		//外部品种个数
	long		m_LcExtSetSpc;		//外部品种信息区大小(sizeof(LCEXTSET)为单位)
	//
	char		*m_pSrcText;		//导入文件缓存
	long		m_lSrcTextLen;		//导入文件大小
	long		m_lSrcTextSpc;		//导入文件缓存大小
	///////////////////////////////////////////////////////////////////////////
	//自定组合信息
	LPLCIDXSET	m_pLcIdxSet;		//自定组合信息
	long		m_LcIdxSetNum;		//自定组合个数
	long		m_LcIdxSetSpc;		//自定组合信息区大小(sizeof(LCIDXSET)为单位)
	//自定组合成分(属于全局临时变量),所有自定组合公用
	LPLCIDXSTK	m_pLcIdxStk;		//自定组合成分股信息
	long		m_LcIdxStkNum;		//自定组合成分股个数
	long		m_LcIdxStkSpc;		//自定组合成分股信息大小(sizeof(LCIDXSTKINFO)为单位)
	//自定组合计算结果
	LPLCIDXDAY	m_pLcIdxDay;		//自定组合累加缓冲区
	long		m_LcIdxDayNum;		//单个自定组合日线数据个数
	long		m_LcIdxDaySpc;		//自定组合累加缓冲区大小(sizeof(LCIDXDAY)为单位)
	//
	LPANALYDATA	m_pIndexAnalyData;	//存盘组合数据
	long		m_IndexAnalyNum;	//单个自定组合日线数据个数
	long		m_IndexAnalySpc;	//自定组合累加缓冲区大小(sizeof(LCIDXDAY)为单位)
	//自定组合中间数据(个股计算市值,累加入指数)
	MemStkInfo	*m_pCurStock;		//当前处理的股票
	LPLCIDXDAY	m_pLcStkDay;		//个股市值调整缓冲区
	long		m_LcStkDayNum;		//个股日线数据个数
	long		m_LcStkDaySpc;		//个股市值调整缓冲区大小(sizeof(LCIDXDAY)为单位)
	//
	LPANALYDATA	m_pStockAnalyData;	//存盘个股数据
	long		m_StockAnalyNum;	//单个自定组合日线数据个数
	long		m_StockAnalySpc;	//自定组合累加缓冲区大小(sizeof(LCIDXDAY)为单位)
	//除权信息
	Cwcl		*m_pCwcl;
	LPCWDATA	m_pCWData;
	long		m_lCWDataNum;
	long		m_lCWDataSpc;
	//
	long		m_MaxDays;

	CCriticalSection	m_CalcSec;

public:
	LcIdxMng();
	~LcIdxMng();
	//
	BOOL	LoadAll(BOOL bInit=FALSE);
	BOOL	SaveAll();
	long	GetSecNum();
	long	GetSecIdx(LPLCSECIDX &pLcSecIdx);
	///////////////////////////////////////////////////////////
	//
	BOOL	LoadPtSet(BOOL bInit=FALSE);
	BOOL	SavePtSet();
	//
	BOOL	AddLcPt(LPLCPTSET pLcPtSet);
	BOOL	ChgLcPt(LPLCPTSET pLcPtSet,long lPtNo);
	BOOL	DelLcPt(long lPtNo);
	//
	long	GetPtNum();
	long	GetPtSet(LPLCPTSET &pLcPtSet);
	//
	BOOL	TestPtExist(char *pPtCode,long lPtNo = -1);
	
	LPLCPTSET GetLcPTSet(char *pPtCode);
	///////////////////////////////////////////////////////////
	//
	BOOL	LoadExtSet(BOOL bInit=FALSE);
	BOOL	SaveExtSet();
	BOOL	ReLoad(long lExtNo = -1);
	//
	BOOL	AddLcExt(LPLCEXTSET pLcExtSet);
	BOOL	ChgLcExt(LPLCEXTSET pLcExtSet,long lExtNo);
	BOOL	DelLcExt(long lExtNo);
	//
	long	GetExtNum();
	long	GetExtSet(LPLCEXTSET &pLcExtSet);
	//
	BOOL	TestExtExist(char *pExtCode,long lExtNo = -1);
	//////////////////////////////////////////////////////////
	BOOL	LoadIdxSet(BOOL bInit=FALSE);
	BOOL	SaveIdxSet();
	BOOL	ReCalc(long lIndexNo = -1);
	//
	BOOL	AddLcIndex(LPLCIDXSET pLcIdxSet,LPLCIDXSTK pLcIdxStk,long LcIdxStkNum);
	BOOL	ChgLcIndex(LPLCIDXSET pLcIdxSet,long lIndexNo,LPLCIDXSTK pLcIdxStk,long LcIdxStkNum);
	BOOL	DelLcIndex(long lIndexNo);
	//
	long	GetIdxNum();
	long	GetIdxSet(LPLCIDXSET &pLcIdxSet);
	BOOL	GetIdxStk(LPLCIDXSTK &pLcIdxStk,long &LcIdxStkNum,long &LcIdxStkSpc,long lIndexNo);
	BOOL	SaveIdxStk(LPLCIDXSTK pLcIdxStk,long LcIdxStkNum,LPLCIDXSET pLcIdxSet,LPLCIDXSET pOldLcIdxSet = NULL);
	void	DelIdxStk(long lIndexNo);
	LPLCIDXSET	GetIdxInfo(long lIndexNo)
	{
		if(lIndexNo>=0&&lIndexNo<GetIdxNum())
			return &m_pLcIdxSet[lIndexNo];
		return NULL;
	}
	char	*GetIdxStkCode(long lIndexNo)
	{
		if(lIndexNo>=0&&lIndexNo<GetIdxNum())
			return m_pLcIdxSet[lIndexNo].Code;
		return "";
	}
	//
	BOOL	TestIndexExist(char *pIdxCode,long lIdxNo = -1);
	BOOL	TestLCIndex(char *pIdxCode,long &lIdxNo);
	//
	void	CheckReCalc(CWnd *pView,char *pIdxCode);
	void	SetNeedReCalc();
	
	//
	long	GetLcNum();
protected:
	//
	BOOL	ReLoadExtern(long lExtNo);
	long	GetDate(char *buf,long lDateFormat);
	//
	BOOL	ReCalcIndex(long lIndexNo);
	BOOL	TQData(LPLCIDXSET pLcIdxSet,MemStkInfo *pCurStock,LPLCIDXDAY pLcStkDay,long lLcStkDayNum,LPCWDATA pCWData,long lCWDataNum);
};

extern class LcIdxMng *g_pLcIdxMng;

#endif

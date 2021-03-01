#ifndef _INTERFACE_H
#define _INTERFACE_H

#ifdef	CALCDLL_EXPORT
	#define CALCDLL	_declspec(dllexport)
#else
	#define CALCDLL	_declspec(dllimport)
#endif

#define MAX_LINEOUT		50
#define MAX_PARAMNUM	16
#define MAX_PRDNUM		12

#define VAR_LEN			16
#define ZBCODE_LEN		14
#define ZBNAME_LEN		50
#define PASSWORD_LEN	20

#include "OutStruct.h"

extern  BYTE TMEANLESS_DATA[8];

#define TMEANLESS_INT				87654321
#define TSET_MEANLESS(a)	        memset(&(a),0xF8,sizeof(a))
#define TSET_NULL(a,from,num)		memset(a+(from),0xF8,sizeof(float)*(num))
#define TBE_MEANLESS(a)				memcmp(&(a),TMEANLESS_DATA,sizeof(a))

#pragma pack(push,1)

#define OUT_RESERVE	2

const	int		MAX_FA = 6;	

#define		DRAW_KLINE		1
#define		DRAW_STICKLINE  2
#define		DRAW_ICON		3
#define		DRAW_TEXT		4
#define		DRAW_BAND		5

//函数类别描述信息表
typedef struct tagTYPEX
{
	int		nTypeID;						//类别ID
	int		nRootType;						//根类别
	int		nFatherType;					//父类型
	char	lpszName[32];					//类别中文名
	BOOL 	bEnable;						//FALSE不显示,TRUE显示,该项具继承性,RESERVED
}TYPEX, *LPTYPEX;

//Index结构改进：
typedef struct tag_ParaInfo   //参数信息的结构定义
{
	char		acParaName[VAR_LEN];//参数的名称
	float		nMin;				//参数最小取值范围
	float		nMax;				//参数最大取值范围
	float		nStep;				//参数测试步长
	float		nDefault;			//系统推荐的缺省值
	float		nValue[MAX_PRDNUM]; //用户定义的值,enum Period
}TPARAINFO;

enum Period  //12个周期对应的下标SubScript
{
	MIN1_ST		,
	MIN5_ST		,
	MIN15_ST	,
	MIN30_ST	,
	HOUR_ST		,
	DAY_ST		,
	WEEK_ST		,
	MONTH_ST	,
	MINN_ST		,
	DAYN_ST		,
	SEASON_ST	,
	YEAR_ST		,
};

typedef struct tag_LineInfo   //画线方法信息的结构定义
{
	char		acLineName[VAR_LEN];   	//输出线的名称，缺省用公式前变量名
	int 		nType;					//本输出线的绘制方法(<10为线形,>10为系统定义颜色)
	DWORD		nColor;					//自定颜色(-1则取nType)
	int			nWidth;					//线的宽度
}TLINEINFO;

typedef struct tag_INDEXINFO
{  
	short		nIndexNo;			//数组的偏移
	BYTE	 	nSet;				//属于哪个集合 0 指标 1 条件 2 专家 3 K线 4 自定义函数(返回最后一行.)
	char		acCode[ZBCODE_LEN]; //指标英文简称
	char		acName[ZBNAME_LEN]; //指标中文描述
	BYTE		nType;       		//指标类型
	DWORD		nDrawMode;			//画线方式 enum DRAWMODE
	BYTE		nParaNum;    		//参数个数
	TPARAINFO	aPara[MAX_PARAMNUM];//参数信息,6参数
	BYTE		nLineNum;			//输出线个数
	TLINEINFO	aLine[MAX_LINEOUT];	//保存输出线的性质
	BYTE		nAxisType;   		//Y坐标绘制类型,0为自动，否则使用fAxisValue
	BYTE		nAddXAxisNum;		//需要额外增加的Y轴重要分界线个数
	float		afAddXAxis[4];		//额外增加的Y轴重要分界线的分界数值
	float		fAxisValue[7];		//自定义的Y坐标值
	char		lpszPassword[PASSWORD_LEN];	//密码内容,为空表示不需加密
	char    *	lpszDefTxt;      	//定义指标的演算公式信息(无内容则为NULL)
	char    *	lpszParamGuy;		//参数精灵描述
	char    *	lpszCompliedInfo; 	//经过编译以后的指标演算公式信息
	char    *	lpszHelp;			//指标说明
	DWORD		nDelStatus;         //enum  STATUS
}TINDEXINFO ;

////指标输出
enum LINETYPE
{
	DEFAULT		= 0,
	VOLSTICK	= 1,	//成交量线
	COLORSTICK	= 2,	//森林线
	CIRCLEDOT	= 3,	//圈叉图 
	STICK       = 4,
	LINESTICK	= 5,
	ZBCIRCLEDOT = 6,
	ZBCROSSDOT  = 7,
	POINTDOT	= 8,
	NODRAW		= 255,	//不画出来
};

enum STATUS
{
	SYSTEM_INDEX=  0x00000001,
	USER_INDEX	=  0x00000002,	//此处是设计漏洞
	SECRET_INDEX=  0x00000004,
	BIN_INDEX	=  0x00000008,
	TMP_INDEX	=  0x00000010,
	DEL_INDEX	=  0x00000020,
	ERR_INDEX	=  0x00000040,
	HIDE_INDEX	=  0x00000080,
	UNSTABLE_INDEX= 0x00000100,
	EXPIRABLE_INDEX=0x00000200,
};

//输出时，浮点数组对应的循序
enum SYSTEM
{
	ENTERLONG	= 0,		//{多头买入}		
	EXITLONG    = 1,		//{多头卖出} 
	ENTERSHORT  = 2,		//{空头买入} 
	EXITSHORT	= 3,		//{空头卖出} 
};

//属于哪个集合
enum INDEX_SET
{
	ZB_TYPE = 0,
	TJ_TYPE = 1,
	EXP_TYPE= 2,
	KL_TYPE = 3,
	FUNC_TYPEX = 4,
	UF_TYPE = 5,
	SEPCLINE_TYPE = 6,
};

// 指标类型 BYTE
enum INDEX_LX
{
	INDEX_OBOS     ,  //超买超卖型*
	INDEX_TREND    ,  //趋势型*
	INDEX_ENERG    ,  //能量型*
	INDEX_VOL      ,  //成交量型*
	INDEX_MAV      ,  //均线型*
	INDEX_STYLE    ,  //图表型*
	INDEX_PATH     ,  //路径型*
	INDEX_STOP     ,  //停损型*
	INDEX_TRADE    ,  //交易型X
	INDEX_OTHER	   ,  //其他
	INDEX_ADDBASE = 100,//用户添加的,在INDEX_ADDBASE之后
};

enum	TJXG_LX
{
	TJ_ZB,
	TJ_JBM,
	TJ_JSPZ,
	TJ_ZSTZ,
	TJ_XTTZ,
	TJ_OTHER,
	TJ_ADDBASE = 100,//用户添加的,在TJ_ADDBASE之后
};

// 画线方式 BYTE
enum DRAWMODE
{
	//主图叠加,必须叠加到主图，变化坐标;可删除,不可独立存在
	//以TAB方式,在主图上切换.
	OnlyMain		=	0x00000001,
	//可在主图,如果再叠加到主图,不变坐标.其他的一定在附图.
	//如果叠加的是K线等,坐标可自行调整.
	MainAble		=   0x00000002,			
	//叠加时,以原指标为主,且必须非OnlyMain.
	//如果有叠加,就不能拖动.  
	//变化坐标
	KOverlay		=	0x00000004,	//K线	
	UOverlay		=   0x00000008,	//美国线
	COverlay		=   0x00000010,	//收盘站线
	/////////////////////////
	PUCULINE		=	0x00000020,
	SARLINE			=	0x00000040,
	BASEZERO		=   0x00000080,
};

//五彩K线 16色
#define COLORRED                    0 
#define COLORBLUE                   1  
#define COLORGREEN                  2  
#define COLORCYAN                   3 
#define COLORBLACK                  4 
#define COLORMAGENTA                5  
#define COLORBROWN                  6  
#define COLORLIGHTGRAY              7  
#define COLORGRAY                   8  
#define COLORLIGHTBLUE              9  
#define COLORLIGHTGREEN				10  
#define COLORLIGHTCYAN				11  
#define COLORLIGHTRED				12  
#define COLORLIGHTMAGENTA			13  
#define COLORYELLOW					14  
#define COLORWHITE					15  
#define DEFAULTKCOLOR				16

#define	MAXKCOLOR					17

//Calc类的错误宏
#define CALC_OK			0
#define CALC_SKIP		1
#define CALC_ERROR		-1
#define ASK_ALL			-1

class  CALCDLL CalcInterface
{
public:
	CalcInterface();
	virtual ~CalcInterface();
public:
	BOOL			Init(char *PubDir=".\\",char *PriDir=".\\",char *IniFile=".\\Tdxw.ini",char *RegStr="");
	void			SetSysLimited(BOOL bLimitExport,BOOL bHasCW,long lOpenRq,long Reserved,char *OtherTypeName,char *OtherXGName);
	BOOL			CloseAll();
	short			RegisterDataIO(PDATAIOFUNC pfn);
	long			InputData(char * Code,short nSetCode,short DataType,void * pData,short nDataNum,BYTE nTQ=0,unsigned long unused = 0);
	//nDataNum = ASK_ALL 表示全部
	long			InputDataInfo(char * Code,short nSetCode,short DataType,short nDataNum = ASK_ALL,BYTE nTQ=0,unsigned long unused = 0);
	long			InputDataInfo(char * Code,short nSetCode,short DataType,NTime time1=NOTIME,NTime time2=NOTIME,BYTE nTQ=0,unsigned long unused = 0);
	//最多只有MAX_LINEOUT 个输出,不够的置为NULL
	//如果成功，返回CALC_OK；否则返回CALC_ERROR!
	short			Calc(BYTE nSet,long nIndexNo,float * fPut[MAX_LINEOUT]);
	float			Calc(char *strFormula, short nSetCode, char *Code);
	long			GetTreeInfo(void *ackData, int nAckType, int nSubType=-1);
	char *			GetLastError();	
	long			GetIndexNum(BYTE nSet)	const;					//得到个数
	//如果失败，返回NULL
	TINDEXINFO	*	GetIndexInfo(BYTE nSet,long nIndexNo)	const;	//画线时，直接指定位置
	TINDEXINFO	*   GetIndexInfo(BYTE nSet,char * acCode)	const;			//名称检索,返回为空不画分析图
	////////////////////
	long			GetIndexNo(BYTE nSet,char *acCode)    const;
	char		*   GetIndexCode(BYTE nSet,long nIndexNo)	const;	
	TINDEXINFO	*   operator[](BYTE nSet) const;
	///////////////////
	//从零开始
	long			GetTypeNum() const;
	char		*   GetTypeName(long No)   const;
	BOOL			GetTypeInfo(LPTYPEX pTypeInfo, long No);
	////工具函数
	BOOL			CopyIndexInfo(TINDEXINFO * pDes,TINDEXINFO * pSrc);
	float		*   Allocfloat(short nLen);					//一次声请一个，长度为nLen
	void		    Freefloat(float * p);					//释放一个
	//////返回数据指针
	void		*   GetData()	const	{	return m_pData;		};
	//修改成功与否
	BOOL			PopupDlg(short Subscript = DAY_ST);
	BOOL			PopupDlg(BYTE nSet,long nIndexNo,short Subscript = DAY_ST);
private:
	void			ResetSystem(BYTE nSet=255);
	BOOL			LoadIndex();
	BOOL			SaveIndex();
private:
	static          NTime   NOTIME;
	static			long	m_ObjCalcer;				//每生成一个对象+1，析构一个-1，最后析构的负责存盘
	void		*   m_pData;
	short			m_nDataType,m_nDataNum;
	NTime			m_time[2];
	char			m_strErrorInfo[256],m_Code[7];
	BOOL			m_bInited;
};


#pragma pack(pop)
#endif

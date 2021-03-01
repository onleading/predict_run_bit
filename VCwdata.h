#ifndef VCWDATA_H_
#define VCWDATA_H_

#ifdef	TCWDATA_EXPORT
	#define TCWDATADLL_API extern "C" __declspec(dllexport)
#else
	#define TCWDATADLL_API extern "C" __declspec(dllimport)
#endif

#pragma pack(1)

#define CW_NAME_LEN			60
#define CW_COMMAND_LEN		8*1024

#define CW_SECURITY_NUM		8000
#define CW_CONST_NUM		2000
#define CW_CONSTVALUE_NUM	3000	
#define CW_TABLE_NUM		2000
#define CW_FIELD_NUM		200
#define CW_SIGCOND_NUM		10
#define CW_FIELD_MAXLEN		500

#define C_CW_CATEGORY		1177
#define C_CW_MARKET			1491
#define C_CW_LISTEDSECTOR	1495
#define C_CW_LISTEDSTATE	1176
#define C_CW_TJJHY			1601
#define C_CW_ZJHHY			1602
#define C_CW_TDXHY			1603

enum C_DataType
{
	C_Short = 3,			//len=2
	C_Int = 4,				//len=4 
	C_LongLong = 5,			//len=8 
	C_Char = 6,				//len=1
	C_Float = 7,			//len=4
	C_Double = 8,			//len=8
	C_Date = 9,				//len=4 (YYYYMMDD)int
	C_DateTime = 10,		//len=8 ((YYYYMMDD)int (HHMMSS)int)
	C_Text = 11,			//文本信息
	C_EnumFirst = 1000,		//枚举
	C_EnumEnd = 79999,		
	C_DecimalFirst = 80000,	//
	C_DecimalEnd = 89999,
	C_StrFirst = 90000,		//9****,len=****
	C_StrEnd = 98000,		
};

//内存证券主表结构
typedef struct tagMemSecurityInfo
{
	long	m_nInnerCode;		//证券内部编码
	long	m_nComCode;			//公司代码	
	char	m_szSymbol[10+1];	//证券代码
	char	m_szSecuAbbr[20+1];	//证券简称
	long	m_nSecuMarket;		//证券市场
	long	m_nSecuCategory;	//证券类别
	long	m_nListedDate;		//上市日期
	long	m_nListedSector;	//上市板块
	long	m_nListedState;		//上市状态
	long	m_nTJJHY;			//统计局行业
	long	m_nZJHHY;			//证监会行业
	long	m_nTDXHY;			//通达信行业
}MemSecurityInfo;

//内存常量结构
typedef struct tagMemConstInfo
{
	long	m_lCID;						//子常量ID
	char	m_szCName[CW_NAME_LEN+1];	//子常量名称
}MemConstInfo;

//内存表结构
typedef struct tagMemTableInfo
{
	long	lTableId;	
	char	szTableExp[CW_NAME_LEN+1];	// 表名(中文)
}MemTableInfo;

//内存字段结构
typedef struct tagMemFieldInfo
{
	long	nId;					// 字段ID(其字段名即为T???)
	long	nType;					// 字段类型
	char	szName[CW_NAME_LEN+1];	// 字段描述
	long	nPos;					// 字段记录中的位置
	long	nWid;					// 字段宽度
}MemFieldInfo;

//数据回调函数的方式
typedef long(CALLBACK * PDATAIOFUNC)(char * Code,short nSetCode,short DataType,void * pData/*内部申请*/,short nDataNum,NTime,NTime,BYTE nTQ,unsigned long);
//实现功能切换的回调函数
typedef long(CALLBACK * PSWITCHINFOFUNC)(char * Code,short nSetCode,char *buf,long buflen,short FuncType,short Option,unsigned long);


#pragma pack()
#endif

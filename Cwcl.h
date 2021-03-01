#ifndef CWCL_H_
#define CWCL_H_

#include "blowfish.h"
#include "dbf.h"

//B01:红利
//B02:配股价
//B03:送股
//B04:配股
#define CWCL_TYPE_DR			1		//除权除息
//
//B01:上市前流通股本（万股）
//B02:上市前总股本（万股）
//B03:上市后流通股本（万股）
//B04:上市后总股本（万股）
#define CWCL_TYPE_SEND2OUT		2		//送股上市(股本变动)
//
//B01:上市前流通股本（万股）
//B02:上市前总股本（万股）
//B03:上市后流通股本（万股）
//B04:上市后总股本（万股）
#define CWCL_TYPE_IN2OUT		3		//内部股上市(股本变动)
//
//B01:上市前流通股本（万股）
//B02:上市前总股本（万股）
//B03:上市后流通股本（万股）
//B04:上市后总股本（万股）
#define CWCL_TYPE_OTHERCAPCHG	5		//其他股本变化(股本变动)
//
//B02:新股价
//B03:新股
#define CWCL_TYPE_NEW			6		//增发新股
//
//B01:上市前流通股本（万股）
//B02:上市前总股本（万股）
//B03:上市后流通股本（万股）
//B04:上市后总股本（万股）
#define CWCL_TYPE_BUYBACK		7		//回购(股本变动)
//
//B01:上市前流通股本（万股）
//B02:上市前总股本（万股）
//B03:上市后流通股本（万股）
//B04:上市后总股本（万股）
#define CWCL_TYPE_NEW2OUT		8		//新股上市(股本变动)
//
//B01:上市前流通股本（万股）
//B02:上市前总股本（万股）
//B03:上市后流通股本（万股）
//B04:上市后总股本（万股）
#define CWCL_TYPE_RESERVES2OUT	9		//转增股本上市(股本变动)
//
//B01:上市前流通股本（万股）
//B02:上市前总股本（万股）
//B03:上市后流通股本（万股）
//B04:上市后总股本（万股）
#define CWCL_TYPE_CONVERT2OUT	10		//可转债上市(股本变动)
//
//B03:扩股
#define CWCL_TYPE_INPROCHG		11		//流通股扩股

typedef struct
{
	char	Setcode;	//新版数据才有此字段
	char 	Code[7];
	long    Date;
	char    Type;
	float   B01;
	float   B02;
	float   B03;
	float   B04;
}CWDATA,*LPCWDATA;

typedef struct //relation.dat
{
	short   SetCode;    //市场
	char    Code[11];   //代码
	char    Name[19];   //名称
	char	Typestr[11];//类型串;
	DWORD   dwUnUsed;   //UnUsed;
}STKRELATION;

////profile.dat
typedef struct 
{
	char SetCode;
	char Code[7];
	char Name[9];
	long Date;      //此日期前
	char Reason[43];//原因 用43是为了让整个结构大小为 64
}TDXSTKOLDNAME;

class Cwcl
{
public:
	Cwcl();
	~Cwcl();
	void	ReadARecord(short RecordNum,CWDATA *CWdata_obj);
	void	Set_Password(BYTE * Password,short len);
	void	CWData_Encrypt(CWDATA *CWdata_obj,short len);
	void	CWdata_Decrypt(CWDATA *CWdata_obj,short len);
	void	ReadLTGB_ZGB(struct MemStkInfo * scode,long * pDate, float * fpLTGB,float * fpZGB,short ItemNum,float fDefaultLTGB,float fDefaultZGB);
	void	ReadLTGB_ZGB_OneDate(struct MemStkInfo * scode,long Date,float * LTGB,float * ZGB,float fDefaultLTGB,float fDefaultZGB);
	void	GetGBBQ_Type(struct MemStkInfo * scode,struct AnalyData * AnalyDatap, char * fpBQtype,short ItemNum);
	void	TQData(struct MemStkInfo * scode,struct AnalyData * AnalyDatap,short nFrom,short nEnd,short nDataNum,short nTQFlag,short nPeriod);
	void	MinTQData(struct MemStkInfo * scode,struct AnalyData * AnalyDatap,short nFrom,short nEnd,short nDataNum,short nTQFlag,short nYearFlag=0);
	short	GetTQInfo(struct MemStkInfo * scode,CWDATA *pCWdata,short nMaxNum,BOOL bOnlyQk=FALSE);
	short	GetRecentGbbqInfo(CWDATA *pOutData,short nMaxNum);

	short	Init_ZGB_LTGB();
	CWDATA * GetGpCwDataStart(char *GpCode,short setcode,short &DataNum); //找到某股票的财务缓冲区入口
	unsigned long Get_RecordNum(char *File);//得到加密文件的记录个数
	unsigned long Get_RecordNum();

	void	ResetRLTData(void);
	short	ReadRelationInfo(struct MemStkInfo * scode, STKRELATION *RLTData, short nMaxCount);

	void	ResetFaceData(void);
	long	ReadAllFaceInfo(TDXSTKOLDNAME* &FaceData);
	short	ReadFaceInfo(struct MemStkInfo * scode, TDXSTKOLDNAME *FaceData, short nMaxCount);

private:
	DBH_Head * 		m_CWBB;			//DBF  头结构
	char 			FileName[256];  //数据库文件名仅在加密过程中使用
	CBlowfish  		* Fish;

	CWDATA			*All_CwData;
	unsigned long   All_DataNum;

	//关联
	STKRELATION		*ALL_RLTData;
	short			RLTDataSet;
	short			*RLTSetInfo;
	long			All_RLTDataNum;

	//FaceOff
	TDXSTKOLDNAME	*ALL_FaceData;
	long			ALL_FaceNum;
};

#endif

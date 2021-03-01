//�ײ����ݴ�������
#ifndef  StockDataIo_H
#define  StockDataIo_H

#include "struct.h"
#include "cwcl.h"
#include "protocol3.h"

struct LocalIndexSet;

class StockDataIo
{
public:
	StockDataIo();
	~StockDataIo();
	BOOL		TcpipInit(BOOL bOffLineMode);
	struct		MemStkInfo * operator [] (short i);
	MemStkInfo *Get_MemStkInfo(const char * code );					//���ݺ���
	MemStkInfo *Get_MemStkInfo(const char * Code,short setcode);
	MemStkInfo *Get_MemStkInfo(short SetCode, short xuhao);
	MemStkInfo *Get_MemStkInfoFromHqxuhao(short set_code, short Hqxuhao);
	MemStkInfo *Get_MemStkInfo_FromMultiBuf(const char * pBuff);
	short		GetIndex_FromMultiBuf(const char * pBuff);

	short		GetIndex( const char * code );						//���ݺ���
	short		GetIndex(const char * code,short setcode);
	short		GetIndex( short SetCode, short xuhao );
	char*		GetNameFromIndex( short index );
	char*		GetMarketInfo(struct MemStkInfo *tmpscode);
	short		GetDomain( MemStkInfo * scode );
	//
	short		GetStockNum(int flag);
	float		GetStockClose(struct MemStkInfo *tmpscode);
	float		GetStockNow(struct MemStkInfo *tmpscode);		
	short		FiltCode( short SetCode, union flunion fltype, short * p );	
	//�ı䵱ǰ����
	void		SetCurrStockData( short SetCode, short xuhao, char * the_hq,char * the_hq_l2=NULL );
	void		SetCurrQueue( short tmpsetcode, short Index, char * the_queue);
	void		SetCurrMarkInfo( short tmpsetcode, short Index, char * the_mrk );
	void		SetStatData( short tmpsetcode, short Index, char * the_imgstat, char * the_tckstat);
	void		CalcDDEReport();
	//������
	BOOL		ReadCurrStockData(struct MemStkInfo *tmpscode,struct CurrStockData * hqp,struct CurrStockData_L2 * hqp_l2=NULL,struct ExCurrData *hqexp=NULL);
	BOOL		ReadCurrStockDataForCode(struct CurrStockData * pTmp,const char * code,short setcode=-1,struct CurrStockData_L2 * hqp_l2=NULL,struct ExCurrData *hqexp=NULL);	//���ݺ���	
	BOOL		ReadCurrQueue(struct MemStkInfo *tmpscode,struct CurrQueue * pQueue);
	BOOL		ReadCurrMarkInfo( struct MemStkInfo *tmpscode, struct MarkInfo * mrk_tmp );
	BOOL		ReadCurrStatData( struct MemStkInfo *tmpscode, struct  SumImgStat * Img_tmp,struct SumTckStat * Tck_tmp);
	LPDDEREPORT ReadDDEReport(short set_code,short Index);
	//����������
	long		GetAnalyDataNum(struct MemStkInfo * scode,short Type,short mulnum=1);
    short		ReadAnalyDataAndCurrHQ( struct MemStkInfo * scode, struct AnalyData * AnalyDatap, short ItemNum ,short Type,struct CurrStockData * pTmp,int & nTodayDataNum,short mulnum=1,short dataflag=1000);
    short		ReadVipDocData(struct MemStkInfo * scode, struct AnalyData * AnalyDatap, short ItemNum ,short Type,short mulnum=1,short dataflag=1000);
	short		ReadAnalyDataOffset(struct MemStkInfo * scode,struct AnalyData * AnalyDatap, short ItemNum ,short Type, long offset,short mulnum=1);
	short		ReadAnalyDataFromTime(struct MemStkInfo * scode,struct AnalyData * AnalyDatap, short ItemNum,long StartDate,long EndDate,short Type,short mulnum=1);
	//����չ����
	long		GetExDayDataNum(struct MemStkInfo * scode);
	short		ReadExDayData(struct MemStkInfo * scode,LPEXDAYINFO pExdayInfo,short ItemNum);
	//����ͳ�Ȩ����
	void		GetGBBQ_Type(struct MemStkInfo * scode,struct AnalyData * AnalyDatap, char * fpBQtype,short ItemNum);
	CWDATA	*	GetGpCwDataStart(char *GpCode,short setcode,short &DataNum); //�ҵ�ĳ��Ʊ�Ĳ��񻺳������
	void		ReadLTGB(struct MemStkInfo * scode,struct AnalyData * AnalyDatap, float * fpLTGB,short ItemNum,float fDefaultLTGB);
	void		ReadLTGB_ZGB(struct MemStkInfo * scode,long *pDate, float * fpLTGB, float *fpZGB,short ItemNum,float fDefaultLTGB,float fDefaultZGB);
	void		ReadLTGB_ZGB_OneDate(struct MemStkInfo * scode,long Date, float * fpLTGB, float *fpZGB,float fDefaultLTGB,float fDefaultZGB);
	void		ReadRealTGB_ZGB(struct MemStkInfo * scode,long Date, long *pDate, float * fpLTGB, float *fpZGB,short ItemNum, float &fRealLTGB, float &fRealZGB);
	void		FixTQDataFromDay(AnalyData * AnalyDatap,short nDataNum,AnalyData * pTempData,short nReadNum,short nPeriod,BOOL bDataValid,int nStkType);
	short		TQData(MemStkInfo * scode,AnalyData * AnalyDatap,short nFrom,short nEnd,short nDataNum,short nTQFlag,short nPeriod);
	short		GetTQInfo(struct MemStkInfo * scode,CWDATA *pOutData,short nMaxNum,BOOL bOnlyQk=FALSE);
	short		GetRecentGbbqInfo(CWDATA *pOutData,short nMaxNum);
	//������Ϣ
	short		GetRLTData(struct MemStkInfo * scode, STKRELATION *pOutData, short nMaxNum);
	long		GetAllFaceData(TDXSTKOLDNAME* &pOutData);
	short		GetFaceData(struct MemStkInfo * scode, TDXSTKOLDNAME *pOutData, short nMaxNum);	
		
	//�������г��ĺ���
	BOOL		DSInit();
	void		AfterDSLoginSuccess();
	PTIMEINF	DSGetTimeInf(short TimeID);
	BOOL		BeDSStock(MemStkInfo *pStkInfo);
	BOOL		BeDSStock(BYTE Market,const char Code[CODE_SIZE]);
	BOOL		BeDSStock(long nIDIndex);
	MemStkInfo*	GetDSStkInfo(long nIDIndex);
	MemStkInfo*	GetDSStkInfo(BYTE Market,const char Code[CODE_SIZE]);
	short		DSFiltCode(int nWhichXh,short*pIdx,short IdxSize);

	BOOL		DSSetCurrData(MemStkInfo *pStkInfo, PCURRDATA pData,PEXCURRDATA pDataEx=NULL);
	void		DSGetCurrData(MemStkInfo *pStkInfo,PCURRDATA pData,PEXCURRDATA pDataEx=NULL);

	void		SaveDSCurrData();
	void		LoadDSCurrData();

	void		LoadIndentInfo();

	void		ReAddLc(short setcode);
	BOOL		CanMakeOneTick(MemStkInfo *pStkInfo,CurrStockData & Hq,BOOL bDs,BOOL bZs,float fLastPrice,long lLastVolume);
public:
    char		pStockPy[MAXGPNUM][6];
	long		m_nDSNum;					//�������������
private:
    struct  CurrStockData	*	hqhead[2];
	struct  CurrStockData_L2*	hqhead_l2[2];
	struct  CurrQueue		*	hqqueue[2];
	struct  MarkInfo		*	hqmark[2];
	struct  SumImgStat		*	hqImgStat[2];
	struct  SumTckStat		*	hqTckStat[2];
	LPDDEREPORT					hqDDEReport[2];
	Cwcl	*m_pCwcl;
    short   IndexNum[2], StockNum[2], HostStkNum[2];

	map<CODEMAPINFO, long, CodeStrCmp> m_code2IndexMap;
	TIMEINF	m_aTimerSet[MAX_DS_TIMERNUM];
	vector<MemStkInfo> m_aDSStkInfo;
	PCURRDATA	m_aICList;	//	��������
	PEXCURRDATA	m_aICListEx;	
private:	
	short		DayTranWeekOrMonOrDayN(struct AnalyData * TempAnalyDatap,struct AnalyData * AnalyDatap,short ItemNum,short ReadedDayNum,short type,char *code,short setcode,short mulnum=1);
};

extern class StockDataIo * g_pStockIO;
#endif

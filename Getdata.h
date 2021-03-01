#ifndef GETDATA_H
#define GETDATA_H

#include "Remote.h"
#include "protocol3.h"

#define ADDEDGBBQ_NUM	200

class CGetData
{
public:
  CGetData ( void );
  ~CGetData ( void );
  BOOL   Create ( void );

public:		//非阻塞式GetData函数及其ACK
  BOOL  GetStockNum ( short setcode, long rq );
  short GetStockNumACK();

  BOOL  GetRXxmCfg ( void );	// 取远程消息面配置
  BOOL  GetRXxmCfgACK();		

  BOOL  GetUrgentCfg(const char *username);	// 取广告和紧急公告
  BOOL  GetUrgentCfgACK();

  BOOL  GetURLUrgentCfg( void );// 取URL化广告和紧急公告
  BOOL  GetURLUrgentCfgACK();

  BOOL	HqAlive();				//行情存活包
  BOOL  HqAliveACK(DWORD & nMaxTJResult,DWORD & nADGUID);
  
  BOOL  Inmode2(const char *username,const char *nickname);
  BOOL  Inmode2ACK();
  BOOL  NeedAuth(const char *username,const char *password);
  BOOL  NeedAuthACK(char &sucflag,char *errmsg,char &need_auth);

  BOOL  GetHostMoreInfo();	//主站扩充信息
  BOOL  GetHostMoreInfoACK(long &szinfdate,long &shinfdate,long &szinfhms,long &shinfhms,char &errflag);

  BOOL  GetPassPort(char *QsId,float clientver);	//主站通行证
  BOOL  GetPassPortACK(char *errmsg,BOOL bDiscardAutoupInfo);

  BOOL	GetNewGPInf(short setcode,short startxh);
  short GetNewGPInfACK(struct remote_TmpStkInfo3* pTmp);

  BOOL  GetBaseInf(short stknum,char *buf);
  short GetBaseInfACK(short szstknum,short shstknum,struct MemStkInfo *pszMem,struct MemStkInfo *pshMem);

  BOOL  GetGbbqInf(short stknum,char *buf);
  BOOL  GetGbbqInfACK(short &retnum,short szstknum,short shstknum,struct MemStkInfo *pSZMem,struct MemStkInfo *pSHMem);
  //行情数据请求
  BOOL  GetCurrStockData(struct MemStkInfo *scode);
  BOOL  GetCurrStockDataACK(struct MemStkInfo *scode,struct CurrStockData * hqp,struct CurrStockData_L2 *hqp_l2=NULL);
  BOOL  GetMultiCurrStockData ( char * codehead, short num);
  short GetMultiCurrStockDataACK (struct CurrStockData_Code * pTmp,int maxnum);
  BOOL  GetMultiCurrStockDataEx ( short setDomain, short coltype, short startxh, short wantnum, short sorttype);
  BOOL  GetMultiCurrStockDataEx ( char * codehead, short num);
  short GetMultiCurrStockDataExACK (struct CurrStockDataEx * pExTmp,int maxnum);
  BOOL  GetG2MultiCurrStockData ( char * codehead, short num);
  short GetG2MultiCurrStockDataACK (struct CurrStockData_Code * pTmp,int maxnum);  
  BOOL  GetL2MultiCurrQueue ( char * codehead, short num);
  short GetL2MultiCurrQueueACK (struct CurrQueue_Code * pTmp,int maxnum);
  BOOL	GetStatData( char * codehead, short num,BOOL bImg );
  short GetStatDataAck(BOOL bImg,short *lpnStkIndex=NULL,int nCnt=0);
  //分时数据请求	
  BOOL  GetMinuteData(struct MemStkInfo * scode,BOOL bHq,short &nNTMinute,struct MinuteData * MinuteDatap=NULL,short * TimeSec=NULL);
  short GetMinuteDataACK(struct MemStkInfo * scode, struct MinuteData * MinuteDatap, struct CurrStockData * pTmp);
  BOOL  GetMmld(struct MemStkInfo * scode);
  BOOL  GetMmldACK(short getnums, struct MinuteData * MinuteDatap);
  BOOL  GetLead(struct MemStkInfo * scode);
  BOOL  GetLeadACK(struct MemStkInfo * scode,short &getnums, float * fLBData);
  BOOL  GetAuction(struct MemStkInfo * scode,long nReqNo,long nReqNum);
  BOOL  GetAuctionACK(short &getnums,LPVTLDATA pAuction);
  BOOL  GetZmzm(struct MemStkInfo * scode);
  BOOL  GetZmzmACK(short &getnums, LPNORMALSTAT MinuteDatap);
  BOOL  GetJgjc(struct MemStkInfo * scode);
  BOOL  GetJgjcACK(short &getnums, LPNORMALSTAT MinuteDatap);
  BOOL  GetCjbs(struct MemStkInfo * scode);
  BOOL  GetCjbsACK(short &getnums, LPTICKSTAT MinuteDatap);
  BOOL  GetCjtj(struct MemStkInfo * scode);
  BOOL  GetCjtjACK(short &getnums, LPTICKSTAT MinuteDatap);
  BOOL	GetTotalTickStat(struct MemStkInfo * scode);
  BOOL	GetTotalTickStatAck(LPTICKSTAT MinuteDatap);
  //历史分时数据请求
  BOOL  GetHisMinuteData(struct MemStkInfo * scode,long lDate);
  short GetHisMinuteDataACK(struct MemStkInfo * scode, struct MinuteData * MinuteDatap, float *pClose,long date=0);
  //CDP数据
  BOOL  GetCDP(struct MemStkInfo * scode);
  BOOL  GetCDPACK(struct MemStkInfo * scode);
  //分价表
  BOOL  GetPriceVolTable(struct MemStkInfo * scode);
  short GetPriceVolTableACK(struct MemStkInfo * scode,struct PriceVolTable * pPriceVolTable,short MaxItem);
  //历史分价表
  BOOL  GetHisPriceVolTable(struct MemStkInfo * scode,long lDate);
  short GetHisPriceVolTableACK(struct MemStkInfo * scode,struct PriceVolTable * pPriceVolTable,short MaxItem,float *pClose);
  //Tick数据
  BOOL  GetTickData(struct MemStkInfo * scode,short startxh, short num,BOOL bReverse=FALSE);
  short GetTickDataACK(struct MemStkInfo *scode,struct TickData *indap,short num,BOOL bFirstWant=FALSE);
  //历史Tick数据
  BOOL  GetHisTickData(struct MemStkInfo * scode,long lDate,short startxh, short num);
  short GetHisTickDataACK(struct MemStkInfo *scode,struct TickData *indap,short num,float *pClose);
  //BSP数据
  BOOL  GetBsp1Data(struct MemStkInfo * scode,short startxh, short num);
  short GetBsp1DataACK(struct MemStkInfo *scode,TICK_MMP *indap,short num,BOOL bFirstWant=FALSE);
  //历史BSP数据
  BOOL  GetHisBsp1Data(struct MemStkInfo * scode,long lDate,short startxh, short num);
  short GetHisBsp1DataACK(struct MemStkInfo *scode,TICK_MMP *indap,short num,float *pClose);
  //分析数据请求
  short GetAnalyData(struct MemStkInfo * scode,short offset,short ItemNum,short Type,short mulnum=1,struct AnalyData * AnalyDatap=NULL);
  short GetAnalyDataACK( struct MemStkInfo * scode,struct AnalyData * AnalyDatap,short Type=-1,short oldnum=-1);
  //全景排行
  BOOL	GetZHSort(int GpType,int Num=6);
  BOOL  GetZHSortACK(ZHRESULT Buf[9]);
  //宽带行情
  BOOL  GetSimHq(char * codehead, short num);
  void  GetSimHqACK();
  //下载数据
  BOOL  GetFxFile(MemStkInfo *pStkInfo,long startdate,long enddate,short type=PER_DAY);
  BOOL  GetFxFileACK(MemStkInfo *pStkInfo);
  //
  BOOL  GetDateDayFile(long lDate,long lPos,long lReqLen,short type=EXDAYSZ_FILE);
  BOOL  GetDateDayFileACK(short &setcode,long &filelen,long &filercvlen,char *prcvbuf,int nMaxLen);
  //逐笔成交数据
  BOOL  GetTickNumL2(struct MemStkInfo * scode,DWORD localSeq);
  short GetTickNumL2ACK(struct MemStkInfo *scode,DWORD &MaxTicSeq);
  BOOL  GetTickDataL2(struct MemStkInfo * scode,DWORD startxh, short num,BOOL bReverse=TRUE);
  short GetTickDataL2ACK(struct MemStkInfo *scode,struct TickData_L2 *indap,short num,DWORD &MaxTicSeq);
  //其它Level2相关数据
  BOOL  GetWTRank(short setcode,short fltype);
  short GetWTRankACK(struct l2_WTRankings *Rankings);
  BOOL  GetSCancelRank(short setcode,short fltype);
  short GetSCancelRankACK(struct l2_SingleCancelRankings *Rankings);
  BOOL  GetTCancelRank(short setcode,short fltype);
  short GetTCancelRankACK(struct l2_TotalCancelRankings *Rankings);
  BOOL  GetHYRank(short setcode);
  short GetHYRankACK(float &MarketTotalValueTraded,struct l2_HYRankings *HYRankings,struct l2_HYGPRankings *HYGPRankings);
  BOOL	GetG2DeepWarn(short setcode,long nReqWarnNo,long nReqWarnNum);
  BOOL	GetG2DeepWarnACK(LPWARNATOM pWarnAtom,long nWarnAtom);
  //数据挖掘请求
  BOOL  SendDataDigReq();
  BOOL  DataDigACK(long &clientnum,long &usedclientnum,long &seconds,char *HostName,short &attention_num,short &resonate_num,short &no_attention_num,short &no_resonate_num,short *attention_index,short *resonate_index,short *no_attention_index,short *no_resonate_index,unsigned long *attention_rate,unsigned long *resonate_rate,unsigned long *no_attention_rate,unsigned long *no_resonate_rate,long contentlen,char **pContent);
  //主力监控数据
  BOOL  GetBigVolData(long getpos=-1);
  short GetBigVolDataACK(struct BigVolTick *pTick,long wantnum,long & lastpos);
  //基本面和消息面等其它请求
  BOOL  GetFileLen( char * filename );
  BOOL  GetXxmFileLen( short xxmxh, short xh );    // 得到指定公告信息文件长度
  BOOL  GetJbmFileLen( const char * code,short setcode );         // 得到指定股票基本资料文件长度
  long  GetFileLenACK(char *usemdstr=NULL,char *mdstr=NULL);

  BOOL  GetXxmItem( short xxmxh, short startxh);	//得到公告信息标题
  short GetXxmItemACK(char * buffer);

  BOOL  GetText(short xxmxh, short xh, DWORD pos);	//得到指定公告信息文件内容
  BOOL  GetJbmText(const char * code, short setcode, DWORD pos,BOOL bBigTxt=FALSE);	//得到指定股票基本资料文件内容
  BOOL  GetAllJbmText(const char * code, short setcode);
  short GetTextACK(char * buffer );
  
  //信息港
  BOOL	GetPathFile (const char *filepath, int nPos);
  long  GetPathFileACK(char * buffer);

  BOOL  GetJbmCfg (const char * code, short setcode);
  int	GetJbmCfgACK();

  //资讯工场数据
  BOOL  GetIwTitle(char search_type,long from_order,short wantnum,short	setcode,char *code,long	hy_id,long type_id);
  short GetIwTitleACK(struct iw_client_title *pList,long wantnum,long &totalnum);  

  //新的基本资料协议
  BOOL GetNewJbmCfg (const char * code, short setcode, short whichjbm=0);
  int  GetNewJbmCfgACK ();
  BOOL GetNewJbmText (const char * code, short setcode,short which,char *sFilePath,long nOffset,long nLength,short whichjbm=0);
  short GetNewJbmTextACK(char * buffer );

  //第三方市场
  BOOL	DSGetSrvInfo(void);								//主站信息
  BOOL	DSGetSrvInfoACK(SRVINFOANS *pInfo);

  BOOL	DSLogin();
  BOOL  DSLoginACK(char *errmsg);

  BOOL	DSGetMktInfo(void);			//	市场属性请求
  BOOL	DSGetMktInfoACK(void);
  BOOL	DSGetStkInf(long Offset,short StkNum);			//商品码表信息
  short	DSGetStkInfACK(PSTKINFO pStkInf);
  
  BOOL	DSGetMarketTimer(long Offset,short TimerNum);	//时间属性设置
  short	DSGetMarketTimerACK(PTIMEINF pTimer);
  
  BOOL	DSGetCurrData(MemStkInfo *pStkInfo);		//最新单独行情请求
  void	DSGetCurrDataACK(PCURRDATA pCurrData,EXCURRDATA * hqpex=NULL);
  
  BOOL	DSGetMulCurrData(PMULIDX pMulInx,short num);	//最新多个行情请求
  BOOL  DSGetMulData(BYTE Market,short SortID,WORD From,WORD SortNum,WORD SortType);
  short DSGetMulCurrDataACK(PMULIDX pMulIdx,int MulNum);

  BOOL	DSGetTick(MemStkInfo *pStkInfo,DWORD Offset,WORD TickNum);
  WORD	DSGetTickACK(PTICKDATA pTick);

  BOOL	DSGetHisTick(MemStkInfo *pStkInfo,long nDate,DWORD Offset,WORD TickNum);
  WORD	DSGetHisTickACK(PTICKDATA pTick);
  
  BOOL	DSGetHisTick2(MemStkInfo *pStkInfo,long nDate,DWORD Offset,WORD TickNum);
  WORD	DSGetHisTick2ACK(PTICKDATA pTick, float *fClose, long *Volin);
  
  BOOL	DSGetMinData(MemStkInfo *pStkInfo);
  WORD	DSGetMinDataACK(PMINDATA pMinData);
  
  BOOL	DSGetHisMinData(MemStkInfo *pStkInfo, long nDate);
  WORD	DSGetHisMinDataACK(PMINDATA pMinData, float *fClose);
  
  int	DSGetHisData(MemStkInfo *pStkInfo,WORD Period,WORD MulNum,DWORD Offset,WORD DataNum,struct AnalyData * AnalyDatap);
  WORD	DSGetHisDataACK( struct MemStkInfo * scode,struct AnalyData * AnalyDatap,short Type,short oldnum);
  
  BOOL	DSGetFJBData(MemStkInfo *pStkInfo);
  BOOL	DSGetFJBDataACK(PriceVolTable *pPrcVolTab);

  BOOL	GetSeatInfo(struct MemStkInfo * scode,char BSFlag,short WantNum);
  short	GetSeatInfoACK(struct MemStkInfo * scode,struct Seat_Content * pSeat,struct Seat_Content * pSeatS,short MaxItem);
  
  BOOL	GetOneSeatQuery(short nSeatID);
  short GetSeatQueryACK(SEATQUERYINFO *pSqi, short MaxItem);
  
  BOOL	DSGetFile( char * filename, DWORD pos, DWORD flag);
  short DSGetFileACK(char * buffer, DWORD *pFlag);
  
  //新的基本资料协议
  BOOL  DSGetNewJbmCfg (const char * code, short setcode, short whichjbm=0);
  int	DSGetNewJbmCfgACK ();
  BOOL	DSGetNewJbmText (const char * code, short setcode,short which,char *sFilePath,long nOffset,long nLength,short whichjbm=0);
  short DSGetNewJbmTextACK(char * buffer );

  BOOL	DSGetZHSort(int GpType,int Num);
  BOOL	DSGetZHSortACK(ZHRESULT Buf[9]);

  //公告
  BOOL	GetTimerMoreInfo();
  BOOL	GetTimerMoreInfoACK();
  BOOL	GetInfoUpload(BYTE InfoType, time_t TimeTag, char *Title, char *Content);

private:
  char * pTransfer;
  char	 SingleCodeHead[10*1024];
};

extern class CGetData * g_pGetData;
#endif

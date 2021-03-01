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

public:		//������ʽGetData��������ACK
  BOOL  GetStockNum ( short setcode, long rq );
  short GetStockNumACK();

  BOOL  GetRXxmCfg ( void );	// ȡԶ����Ϣ������
  BOOL  GetRXxmCfgACK();		

  BOOL  GetUrgentCfg(const char *username);	// ȡ���ͽ�������
  BOOL  GetUrgentCfgACK();

  BOOL  GetURLUrgentCfg( void );// ȡURL�����ͽ�������
  BOOL  GetURLUrgentCfgACK();

  BOOL	HqAlive();				//�������
  BOOL  HqAliveACK(DWORD & nMaxTJResult,DWORD & nADGUID);
  
  BOOL  Inmode2(const char *username,const char *nickname);
  BOOL  Inmode2ACK();
  BOOL  NeedAuth(const char *username,const char *password);
  BOOL  NeedAuthACK(char &sucflag,char *errmsg,char &need_auth);

  BOOL  GetHostMoreInfo();	//��վ������Ϣ
  BOOL  GetHostMoreInfoACK(long &szinfdate,long &shinfdate,long &szinfhms,long &shinfhms,char &errflag);

  BOOL  GetPassPort(char *QsId,float clientver);	//��վͨ��֤
  BOOL  GetPassPortACK(char *errmsg,BOOL bDiscardAutoupInfo);

  BOOL	GetNewGPInf(short setcode,short startxh);
  short GetNewGPInfACK(struct remote_TmpStkInfo3* pTmp);

  BOOL  GetBaseInf(short stknum,char *buf);
  short GetBaseInfACK(short szstknum,short shstknum,struct MemStkInfo *pszMem,struct MemStkInfo *pshMem);

  BOOL  GetGbbqInf(short stknum,char *buf);
  BOOL  GetGbbqInfACK(short &retnum,short szstknum,short shstknum,struct MemStkInfo *pSZMem,struct MemStkInfo *pSHMem);
  //������������
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
  //��ʱ��������	
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
  //��ʷ��ʱ��������
  BOOL  GetHisMinuteData(struct MemStkInfo * scode,long lDate);
  short GetHisMinuteDataACK(struct MemStkInfo * scode, struct MinuteData * MinuteDatap, float *pClose,long date=0);
  //CDP����
  BOOL  GetCDP(struct MemStkInfo * scode);
  BOOL  GetCDPACK(struct MemStkInfo * scode);
  //�ּ۱�
  BOOL  GetPriceVolTable(struct MemStkInfo * scode);
  short GetPriceVolTableACK(struct MemStkInfo * scode,struct PriceVolTable * pPriceVolTable,short MaxItem);
  //��ʷ�ּ۱�
  BOOL  GetHisPriceVolTable(struct MemStkInfo * scode,long lDate);
  short GetHisPriceVolTableACK(struct MemStkInfo * scode,struct PriceVolTable * pPriceVolTable,short MaxItem,float *pClose);
  //Tick����
  BOOL  GetTickData(struct MemStkInfo * scode,short startxh, short num,BOOL bReverse=FALSE);
  short GetTickDataACK(struct MemStkInfo *scode,struct TickData *indap,short num,BOOL bFirstWant=FALSE);
  //��ʷTick����
  BOOL  GetHisTickData(struct MemStkInfo * scode,long lDate,short startxh, short num);
  short GetHisTickDataACK(struct MemStkInfo *scode,struct TickData *indap,short num,float *pClose);
  //BSP����
  BOOL  GetBsp1Data(struct MemStkInfo * scode,short startxh, short num);
  short GetBsp1DataACK(struct MemStkInfo *scode,TICK_MMP *indap,short num,BOOL bFirstWant=FALSE);
  //��ʷBSP����
  BOOL  GetHisBsp1Data(struct MemStkInfo * scode,long lDate,short startxh, short num);
  short GetHisBsp1DataACK(struct MemStkInfo *scode,TICK_MMP *indap,short num,float *pClose);
  //������������
  short GetAnalyData(struct MemStkInfo * scode,short offset,short ItemNum,short Type,short mulnum=1,struct AnalyData * AnalyDatap=NULL);
  short GetAnalyDataACK( struct MemStkInfo * scode,struct AnalyData * AnalyDatap,short Type=-1,short oldnum=-1);
  //ȫ������
  BOOL	GetZHSort(int GpType,int Num=6);
  BOOL  GetZHSortACK(ZHRESULT Buf[9]);
  //�������
  BOOL  GetSimHq(char * codehead, short num);
  void  GetSimHqACK();
  //��������
  BOOL  GetFxFile(MemStkInfo *pStkInfo,long startdate,long enddate,short type=PER_DAY);
  BOOL  GetFxFileACK(MemStkInfo *pStkInfo);
  //
  BOOL  GetDateDayFile(long lDate,long lPos,long lReqLen,short type=EXDAYSZ_FILE);
  BOOL  GetDateDayFileACK(short &setcode,long &filelen,long &filercvlen,char *prcvbuf,int nMaxLen);
  //��ʳɽ�����
  BOOL  GetTickNumL2(struct MemStkInfo * scode,DWORD localSeq);
  short GetTickNumL2ACK(struct MemStkInfo *scode,DWORD &MaxTicSeq);
  BOOL  GetTickDataL2(struct MemStkInfo * scode,DWORD startxh, short num,BOOL bReverse=TRUE);
  short GetTickDataL2ACK(struct MemStkInfo *scode,struct TickData_L2 *indap,short num,DWORD &MaxTicSeq);
  //����Level2�������
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
  //�����ھ�����
  BOOL  SendDataDigReq();
  BOOL  DataDigACK(long &clientnum,long &usedclientnum,long &seconds,char *HostName,short &attention_num,short &resonate_num,short &no_attention_num,short &no_resonate_num,short *attention_index,short *resonate_index,short *no_attention_index,short *no_resonate_index,unsigned long *attention_rate,unsigned long *resonate_rate,unsigned long *no_attention_rate,unsigned long *no_resonate_rate,long contentlen,char **pContent);
  //�����������
  BOOL  GetBigVolData(long getpos=-1);
  short GetBigVolDataACK(struct BigVolTick *pTick,long wantnum,long & lastpos);
  //���������Ϣ�����������
  BOOL  GetFileLen( char * filename );
  BOOL  GetXxmFileLen( short xxmxh, short xh );    // �õ�ָ��������Ϣ�ļ�����
  BOOL  GetJbmFileLen( const char * code,short setcode );         // �õ�ָ����Ʊ���������ļ�����
  long  GetFileLenACK(char *usemdstr=NULL,char *mdstr=NULL);

  BOOL  GetXxmItem( short xxmxh, short startxh);	//�õ�������Ϣ����
  short GetXxmItemACK(char * buffer);

  BOOL  GetText(short xxmxh, short xh, DWORD pos);	//�õ�ָ��������Ϣ�ļ�����
  BOOL  GetJbmText(const char * code, short setcode, DWORD pos,BOOL bBigTxt=FALSE);	//�õ�ָ����Ʊ���������ļ�����
  BOOL  GetAllJbmText(const char * code, short setcode);
  short GetTextACK(char * buffer );
  
  //��Ϣ��
  BOOL	GetPathFile (const char *filepath, int nPos);
  long  GetPathFileACK(char * buffer);

  BOOL  GetJbmCfg (const char * code, short setcode);
  int	GetJbmCfgACK();

  //��Ѷ��������
  BOOL  GetIwTitle(char search_type,long from_order,short wantnum,short	setcode,char *code,long	hy_id,long type_id);
  short GetIwTitleACK(struct iw_client_title *pList,long wantnum,long &totalnum);  

  //�µĻ�������Э��
  BOOL GetNewJbmCfg (const char * code, short setcode, short whichjbm=0);
  int  GetNewJbmCfgACK ();
  BOOL GetNewJbmText (const char * code, short setcode,short which,char *sFilePath,long nOffset,long nLength,short whichjbm=0);
  short GetNewJbmTextACK(char * buffer );

  //�������г�
  BOOL	DSGetSrvInfo(void);								//��վ��Ϣ
  BOOL	DSGetSrvInfoACK(SRVINFOANS *pInfo);

  BOOL	DSLogin();
  BOOL  DSLoginACK(char *errmsg);

  BOOL	DSGetMktInfo(void);			//	�г���������
  BOOL	DSGetMktInfoACK(void);
  BOOL	DSGetStkInf(long Offset,short StkNum);			//��Ʒ�����Ϣ
  short	DSGetStkInfACK(PSTKINFO pStkInf);
  
  BOOL	DSGetMarketTimer(long Offset,short TimerNum);	//ʱ����������
  short	DSGetMarketTimerACK(PTIMEINF pTimer);
  
  BOOL	DSGetCurrData(MemStkInfo *pStkInfo);		//���µ�����������
  void	DSGetCurrDataACK(PCURRDATA pCurrData,EXCURRDATA * hqpex=NULL);
  
  BOOL	DSGetMulCurrData(PMULIDX pMulInx,short num);	//���¶����������
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
  
  //�µĻ�������Э��
  BOOL  DSGetNewJbmCfg (const char * code, short setcode, short whichjbm=0);
  int	DSGetNewJbmCfgACK ();
  BOOL	DSGetNewJbmText (const char * code, short setcode,short which,char *sFilePath,long nOffset,long nLength,short whichjbm=0);
  short DSGetNewJbmTextACK(char * buffer );

  BOOL	DSGetZHSort(int GpType,int Num);
  BOOL	DSGetZHSortACK(ZHRESULT Buf[9]);

  //����
  BOOL	GetTimerMoreInfo();
  BOOL	GetTimerMoreInfoACK();
  BOOL	GetInfoUpload(BYTE InfoType, time_t TimeTag, char *Title, char *Content);

private:
  char * pTransfer;
  char	 SingleCodeHead[10*1024];
};

extern class CGetData * g_pGetData;
#endif

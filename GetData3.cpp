#include "stdafx.h"
#include "HQComm.h"
#include "MainFrm.h"

/////////////////////////////////////////////////////////////////////////////
//登录协议
BOOL	CGetData::DSLogin()
{
	struct ds_login_req_t *reqp=(struct ds_login_req_t *)pTransfer;
	memset(reqp,0,sizeof(struct ds_login_req_t));
	reqp->req = DS_LOGIN_REQ;
	//三方行情模块的版本号
	reqp->clientver=atof(g_szDSExeVersion);
	//输入高级行情用户名和密码
	char LoginName[50]={0},Password[50]={0};
	strcpy(LoginName,g_strLoginName);
	strcpy(Password,g_strPassword);
	strcpy(reqp->user,LoginName);
	strncpy(reqp->pwd,Password,19);

	//是否是通达信的版本的客户端
	reqp->istdxuser=1;

	//对密码部分加密
	CBlowfish fish((BYTE *)"SECURE20031107_TDXDS",20);////解密
	fish.Encrypt((BYTE *)((BYTE *)reqp+sizeof(short)),(sizeof(struct ds_login_req_t)-sizeof(short)));	
	return(g_pComm3->SendIt(pTransfer,sizeof(struct ds_login_req_t))==0);
}

BOOL	CGetData::DSLoginACK(char *errmsg)
{
	struct ds_login_ans *ansp=(struct ds_login_ans *)g_pReceive3;
	if(ansp->Sucflag==0)
	{
		strcpy(errmsg,ansp->errmsg);
		return FALSE;
	}
	else
	{
		//对时
		SYSTEMTIME time,ltime;
		GetLocalTime( &ltime );
		long LoginDSLocalSeconds = ltime.wHour*3600+ltime.wMinute*60+ltime.wSecond;	//对时时的本地时间

		time.wYear = (ansp->date).da_year;
		time.wMonth = (ansp->date).da_mon;
		time.wDay = (ansp->date).da_day;
		time.wHour = (ansp->time).ti_hour;
		time.wMinute = (ansp->time).ti_min;
		time.wSecond = (ansp->time).ti_sec;
		time.wMilliseconds = 0.1*(ansp->time).ti_hund;
		long LoginDSServerSeconds = time.wHour*3600+time.wMinute*60+time.wSecond;	//对时时的服务器时间
		g_LoginDSServerSubLocal = LoginDSServerSeconds - LoginDSLocalSeconds;						//第三方对时差值
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//取主站信息
BOOL	CGetData::DSGetSrvInfo(void)
{
	PREQNULL pReq = (PREQNULL)pTransfer;
	pReq->ReqID = SERVER_INFO;
	return(g_pComm3->SendIt(pTransfer,sizeof(REQNULL))==0);
}

BOOL	CGetData::DSGetSrvInfoACK(SRVINFOANS *pInfo)
{
	if(pInfo)
		memcpy(pInfo,g_pReceive3,sizeof(SRVINFOANS));
	return(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL	CGetData::DSGetMktInfo(void)	//	市场属性
{
	PREQNULL pReq = (PREQNULL)pTransfer;
	pReq->ReqID = MARKET_INFO;
	return(g_pComm3->SendIt(pTransfer,sizeof(REQNULL))==0);
}

BOOL	CGetData::DSGetMktInfoACK(void)
{
	PMKTINFANS	pAns = (PMKTINFANS)g_pReceive3;
	if(pAns->MktNum>0)
	{
		pAns->MktNum = min(MAX_MKT_NUM,pAns->MktNum);
		DSSaveMarketInfo(pAns->MktInf,pAns->MktNum);
		return(TRUE);
	}
	return(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
//取码表
BOOL	CGetData::DSGetStkInf(long Offset,short StkNum)
{
	PSTKINFREQ	pReq = (PSTKINFREQ)pTransfer;
	pReq->ReqID = CODE_LIST_INFO_5;
	pReq->Offset = Offset;
	pReq->StkNum = StkNum;
	return(g_pComm3->SendIt(pTransfer,sizeof(STKINFREQ))==0);
}

short	CGetData::DSGetStkInfACK(PSTKINFO pStkInf)
{
	PSTKINFANS pAns = (PSTKINFANS)g_pReceive3;
	memcpy(pStkInf,pAns->StkInfo,sizeof(STKINFO)*pAns->StkNum);
	return(pAns->StkNum);
}
/////////////////////////////////////////////////////////////////////////////
//时间属性设置
BOOL	CGetData::DSGetMarketTimer(long Offset,short TimerNum)
{
	PTIMEINFREQ	pReq = (PTIMEINFREQ)pTransfer;
	pReq->ReqID = TIME_INFO;
	pReq->Offset = Offset;
	pReq->TimeNum = TimerNum;
	return(g_pComm3->SendIt(pTransfer,sizeof(TIMEINFREQ))==0);
}

short	CGetData::DSGetMarketTimerACK(PTIMEINF pTimer)
{
	PTIMEINFANS pAns = (PTIMEINFANS)g_pReceive3;
	memcpy(pTimer,pAns->TimeInf,sizeof(TIMEINF)*pAns->TimeNum);
	return(pAns->TimeNum);
}

/////////////////////////////////////////////////////////////////////////////
//	最新单独行情请求 
BOOL	CGetData::DSGetCurrData(MemStkInfo *pStkInfo)
{
	if(pStkInfo==NULL) return(FALSE);
	PICDREQ	pReq = (PICDREQ)pTransfer;
	pReq->ReqID = NOW_CURR_DATA;
	pReq->Market = pStkInfo->setcode;
	lstrcpy(pReq->Code,pStkInfo->Code);
	return(g_pComm3->SendIt(pTransfer,sizeof(ICDREQ))==0);
}

void	CGetData::DSGetCurrDataACK(CURRDATA * hqp,EXCURRDATA * hqpex)
{
	PICDEXANS	pAns = (PICDEXANS)g_pReceive3;
	BYTE Market; 
	char Code[CODE_SIZE+1]={0};
	memcpy(&Market,pAns->buf,1);
	memcpy(Code,pAns->buf+1,CODE_SIZE);
	memcpy(hqp,pAns->buf+1+CODE_SIZE,sizeof(CURRDATA));
	struct MemStkInfo *pInfo=g_pStockIO->GetDSStkInfo(Market, Code);
	if(!pInfo) return;
	if(hqpex)
		memset(hqpex,0,sizeof(EXCURRDATA));
	if(hqp->Reserved)
	{
		g_pStockIO->DSSetCurrData(pInfo,hqp,(PEXCURRDATA)(pAns->buf+1+CODE_SIZE+sizeof(CURRDATA)));
		if(hqpex)
		{
			memcpy(hqpex,pAns->buf+1+CODE_SIZE+sizeof(CURRDATA),sizeof(EXCURRDATA));
			pInfo->zangsu = hqpex->fZhangsu;
			pInfo->VolBase = hqpex->VolBase;
		}
	}
	else
		g_pStockIO->DSSetCurrData(pInfo,hqp,NULL);
	if(pInfo->setcode==g_ET.GPSetCode && strcmp(g_ET.GPCode,pInfo->Code)==0)
	{
		if(g_ET.HWnd) //向调用者的窗口发送数据消息
			ETSendCopyDataMessage(g_ET.HWnd,pInfo,hqp);
		g_ET.Ready = 1;
	}
	if(InStatusBarGP(pInfo))
		((CMainFrame *)AfxGetMainWnd())->RefreshStatusBar();
}

/////////////////////////////////////////////////////////////////////////////
//	最新多个行情请求 
BOOL	CGetData::DSGetMulCurrData(PMULIDX pMulInx,short num)
{
	InterlockedExchange(&g_nDSAutoStopTimer, 0);
	PMULDATAREQ	pReq = (PMULDATAREQ)pTransfer;
	pReq->ReqID = MUL_CURR_DATA;
	pReq->StkNum = num;
	memcpy(pReq->MulIdx,pMulInx,num*sizeof(MULIDX));
	short  SendSize = sizeof(MULDATAREQ);
	if(num>1) SendSize += (sizeof(MULIDX)*(num-1));
	return(g_pComm3->SendIt(pTransfer,SendSize)==0);
}

BOOL  CGetData::DSGetMulData(BYTE Market,short SortID,WORD From,WORD SortNum,WORD SortType)
{
	InterlockedExchange(&g_nDSAutoStopTimer, 0);
	PSORTREQ pReq = (PSORTREQ)pTransfer;
	pReq->ReqID = PUSH_HQB_DATA;
	pReq->Market = Market;
	pReq->SortID = SortID;
	pReq->From = From;
	pReq->SortNum = SortNum;
	pReq->SortType = SortType;
	return(g_pComm3->SendIt(pTransfer,sizeof(SORTREQ))==0);
}

short CGetData::DSGetMulCurrDataACK(PMULIDX pMulIdx,int MulNum)
{
	PMULDATAEXANS	pAns = (PMULDATAEXANS)g_pReceive3;
	struct MemStkInfo *pInfo=NULL;
	PCURRDATA	hqp = NULL; 
	PEXCURRDATA hqexp = NULL;
	BYTE Market; 
	char Code[CODE_SIZE+1]={0};
	int pos=0,nCnt=0;
	BOOL bStatusGP=FALSE;
	for(WORD i=0;i<pAns->StkNum;++i)
	{
		memcpy(&Market,pAns->buf+pos,1);
		memcpy(Code,pAns->buf+pos+1,CODE_SIZE);
		if(nCnt < MulNum)
		{
			pMulIdx[nCnt].Market=Market;
			memcpy(pMulIdx[nCnt].Code,Code,CODE_SIZE);
			nCnt++;
		}
		hqp = (PCURRDATA)(pAns->buf+pos+1+CODE_SIZE);
		hqexp = NULL;	//必须要赋空
		if(hqp->Reserved) 
			hqexp = (PEXCURRDATA)(pAns->buf+pos+1+CODE_SIZE+sizeof(CURRDATA));
		pInfo=g_pStockIO->GetDSStkInfo(Market,Code);
		if(pInfo==NULL) continue;
		if(hqexp)
		{
			g_pStockIO->DSSetCurrData(pInfo,hqp,hqexp);
			pos+=1+CODE_SIZE+sizeof(CURRDATA)+sizeof(EXCURRDATA);
			pInfo->zangsu = hqexp->fZhangsu;
			pInfo->VolBase = hqexp->VolBase;
		}
		else
		{
			g_pStockIO->DSSetCurrData(pInfo,hqp,NULL);
			pos+=1+CODE_SIZE+sizeof(CURRDATA);
		}
		if(pInfo->setcode==g_ET.GPSetCode && strcmp(g_ET.GPCode,pInfo->Code)==0)
		{
			if(g_ET.HWnd) //向调用者的窗口发送数据消息
				ETSendCopyDataMessage(g_ET.HWnd,pInfo,hqp);
			g_ET.Ready = 1;
		}
		if(!bStatusGP)
			bStatusGP = InStatusBarGP(pInfo);
	}
	if(bStatusGP)
		((CMainFrame *)AfxGetMainWnd())->RefreshStatusBar();
	return nCnt;
}
/////////////////////////////////////////////////////////////////////////////
//分笔成交
BOOL	CGetData::DSGetTick(MemStkInfo *pStkInfo,DWORD Offset,WORD TickNum)
{
	PTICKREQ pReq = (PTICKREQ)pTransfer;
	pReq->ReqID = TICK_DATA;
	pReq->Market = pStkInfo->setcode;
	pReq->Offset = Offset;
	pReq->TickNum = TickNum;
	lstrcpy(pReq->Code,pStkInfo->Code);
	return(g_pComm3->SendIt(pTransfer,sizeof(TICKREQ))==0);
}

WORD	CGetData::DSGetTickACK(PTICKDATA pTick)
{
	PTICKANS pAns = (PTICKANS)g_pReceive3;
	BYTE Market; char Code[CODE_SIZE];
	Market = pAns->Market;
	lstrcpyn(Code,pAns->Code,CODE_SIZE);
	IMemcpy(pTick,pAns->iTick,sizeof(TICKDATA)*pAns->TickNum);
	return(pAns->TickNum);
}

BOOL	CGetData::DSGetHisTick(MemStkInfo *pStkInfo,long nDate,DWORD Offset,WORD TickNum)
{
	PHISTICKREQ pReq = (PHISTICKREQ)pTransfer;
	pReq->ReqID = HISTICK_DATA;
	pReq->nDate = nDate;
	pReq->Market = pStkInfo->setcode;
	pReq->Offset = Offset;
	pReq->TickNum = TickNum;
	lstrcpy(pReq->Code,pStkInfo->Code);
	return(g_pComm3->SendIt(pTransfer,sizeof(HISTICKREQ))==0);
}

WORD	CGetData::DSGetHisTickACK(PTICKDATA pTick)
{
	PTICKANS pAns = (PTICKANS)g_pReceive3;
	BYTE Market; char Code[CODE_SIZE];
	Market = pAns->Market;
	lstrcpyn(Code,pAns->Code,CODE_SIZE);
	IMemcpy(pTick,pAns->iTick,sizeof(TICKDATA)*pAns->TickNum);
	return(pAns->TickNum);
}

BOOL	CGetData::DSGetHisTick2(MemStkInfo *pStkInfo,long nDate,DWORD Offset,WORD TickNum)
{
	PHISTICKREQ pReq = (PHISTICKREQ)pTransfer;
	pReq->ReqID = HISTICK2_DATA;
	pReq->nDate = nDate;
	pReq->Market = pStkInfo->setcode;
	pReq->Offset = Offset;
	pReq->TickNum = TickNum;
	lstrcpy(pReq->Code,pStkInfo->Code);
	return(g_pComm3->SendIt(pTransfer,sizeof(HISTICKREQ))==0);
}

WORD	CGetData::DSGetHisTick2ACK(PTICKDATA pTick, float *fClose, long *Volin)
{
	PTICK2ANS pAns = (PTICK2ANS)g_pReceive3;
	BYTE Market; char Code[CODE_SIZE];
	Market = pAns->Market;
	lstrcpyn(Code,pAns->Code,CODE_SIZE);
	memcpy(fClose, &pAns->fClose, sizeof(float));
	memcpy(Volin, &pAns->Volin, sizeof(long));
	IMemcpy(pTick,pAns->iTick,sizeof(TICKDATA)*pAns->TickNum);
	return(pAns->TickNum);
}
/////////////////////////////////////////////////////////////////////////////
//分时数据
BOOL	CGetData::DSGetMinData(MemStkInfo *pStkInfo)
{
	PMINDATAREQ pReq = (PMINDATAREQ)pTransfer;
	pReq->ReqID = MIN_DATA;
	pReq->Market = pStkInfo->setcode;
	lstrcpy(pReq->Code,pStkInfo->Code);
	return(g_pComm3->SendIt(pTransfer,sizeof(MINDATAREQ))==0);
}

WORD	CGetData::DSGetMinDataACK(PMINDATA pMinData)
{
	PMINDATAANS pAns = (PMINDATAANS)g_pReceive3;
	IMemcpy(pMinData,pAns->iData,sizeof(MINDATA)*pAns->DataNum);
	return(pAns->DataNum);
}

BOOL	CGetData::DSGetHisMinData(MemStkInfo *pStkInfo, long nDate)
{
	PHISMINDATAREQ pReq = (PHISMINDATAREQ)pTransfer;
	pReq->ReqID = HISMIN_DATA;
	pReq->Market = pStkInfo->setcode;
	lstrcpy(pReq->Code,pStkInfo->Code);
	pReq->nDate = nDate;
	return(g_pComm3->SendIt(pTransfer,sizeof(HISMINDATAREQ))==0);
}

WORD	CGetData::DSGetHisMinDataACK(PMINDATA pMinData, float *fClose)
{
	PHISMINDATAANS pAns = (PHISMINDATAANS)g_pReceive3;
	memcpy(fClose, &pAns->fClose, sizeof(float));
	IMemcpy(pMinData,pAns->iData,sizeof(MINDATA)*pAns->DataNum);
	return(pAns->DataNum);
}
/////////////////////////////////////////////////////////////////////////////
//K线数据
int	CGetData::DSGetHisData(MemStkInfo *pStkInfo,WORD Period,WORD MulNum,DWORD Offset,WORD DataNum,struct AnalyData * AnalyDatap)
{
	PHISDATAREQ pReq = (PHISDATAREQ)pTransfer;
	pReq->ReqID = HIS_DATA;
	pReq->Market = pStkInfo->setcode;
	pReq->Period = Period;
	pReq->MulNum = MulNum;
	pReq->Offset = Offset;
	pReq->DataNum = DataNum;
	lstrcpy(pReq->Code,pStkInfo->Code);
	if(g_pComm3->SendIt(pTransfer,sizeof(HISDATAREQ))!=0)
		return -1;
	return 0;
}

WORD	CGetData::DSGetHisDataACK( struct MemStkInfo * scode,struct AnalyData * AnalyDatap,short Type,short oldnum)
{
	PHISDATAANS pAns = (PHISDATAANS)g_pReceive3;
	IMemcpy(AnalyDatap,pAns->iData,sizeof(ANALYDATA)*pAns->DataNum);
	for(int i=0;i<pAns->DataNum;i++)
	{
		if(i>1 && (AnalyDatap[i].Open<COMPPREC || AnalyDatap[i].Close<COMPPREC || AnalyDatap[i].High<COMPPREC || AnalyDatap[i].Low<COMPPREC))
		{
			memset(AnalyDatap+i,0,sizeof(struct AnalyData));
			AnalyDatap[i].Open=AnalyDatap[i-1].Close;
			AnalyDatap[i].Close=AnalyDatap[i-1].Close;
			AnalyDatap[i].High=AnalyDatap[i-1].Close;
			AnalyDatap[i].Low=AnalyDatap[i-1].Close;
		}
		//对最高和最低进行容错
		if(AnalyDatap[i].Close>AnalyDatap[i].High+COMPPREC)
			AnalyDatap[i].High=AnalyDatap[i].Close;
		if(AnalyDatap[i].Low>AnalyDatap[i].Close+COMPPREC)
			AnalyDatap[i].Low=AnalyDatap[i].Close;
	}
	//写入Cache
//	if( oldnum>=0 && Type==PER_DAY )
//		WriteCacheData(scode,scode->setcode,AnalyDatap,pAns->DataNum,Type);
	//写入下载数据区
	if(Type==PER_DAY && g_bDay_WriteDisk)
	{
		long lNowDate = GetDSCorrespondingDate(g_pStockIO->DSGetTimeInf(scode->TimerID));
		if(AnalyDatap[pAns->DataNum-1].Time.Date==lNowDate)
			AddToVipdocDayData(scode,AnalyDatap,pAns->DataNum-1,Type);
		else if(AnalyDatap[pAns->DataNum-1].Time.Date<lNowDate)
			AddToVipdocDayData(scode,AnalyDatap,pAns->DataNum,Type);
	}
	return(pAns->DataNum);
}
//////////////////////////////////////////////////////////////////////////
BOOL	CGetData::DSGetFJBData(MemStkInfo *pStkInfo)	//分价表数据
{
	PFJBDATAREQ pReq = (PFJBDATAREQ)pTransfer;
	pReq->req = FJB_DATA;
	pReq->Market = pStkInfo->setcode;
	lstrcpy(pReq->Code, pStkInfo->Code);
	return(g_pComm3->SendIt(pTransfer,sizeof(FJBDATAREQ))==0);
}

BOOL	CGetData::DSGetFJBDataACK(PriceVolTable *pPrcVolTab)
{
	PFJBDATAANS pAns = (PFJBDATAANS)g_pReceive3;
	IMemcpy(pPrcVolTab, pAns->idx, sizeof(PriceVolTable)*pAns->num);
	return(pAns->num);
}

//买卖队列信息
BOOL CGetData::GetSeatInfo(struct MemStkInfo * scode,char BSFlag,short WantNum)
{
	struct Seat_Req * pReq = (struct Seat_Req *)pTransfer;
	pReq->ReqID = SEAT_DATA;
	pReq->BSFlag = BSFlag;
	pReq->Market = scode->setcode;
	pReq->WantNum = WantNum;
	lstrcpy(pReq->Code, scode->Code);
	return(g_pComm3->SendIt(pTransfer,sizeof(struct Seat_Req))==0);	
}

short CGetData::GetSeatInfoACK(struct MemStkInfo * scode,struct Seat_Content * pSeatB,struct Seat_Content * pSeatS,short MaxItem)
{
	struct Seat_Ans *pAns = (struct Seat_Ans *)g_pReceive3;
	int GetNumB = max(pAns->NumB,0);
	int GetNumS = max(pAns->NumS,0);
	GetNumB = min(GetNumB,MaxItem);
	GetNumS = min(GetNumS,MaxItem);
	int GetNum = GetNumB+GetNumS;
	IMemcpy(pSeatB, pAns->List, sizeof(struct Seat_Content)*GetNumB);
	IMemcpy(pSeatS, &pAns->List[GetNumB], sizeof(struct Seat_Content)*GetNumS);
	return GetNum;
}

//席位查询协议
BOOL CGetData::GetOneSeatQuery(short nSeatID)
{
	GETSEATQUERY_REQ * pReq = (GETSEATQUERY_REQ *)pTransfer;
	pReq->iRetCode = DS_SEATQUERY_REQ;
	pReq->nReqNum = 1;
	pReq->nSeatItems[0] = nSeatID;
	return(g_pComm3->SendIt(pTransfer,sizeof(GETSEATQUERY_REQ))==0);	
}

short CGetData::GetSeatQueryACK(SEATQUERYINFO *pSqi, short MaxItem)
{
	GETSEATQUERY_ANS *pAns = (GETSEATQUERY_ANS *)g_pReceive3;
	int GetNum = max(pAns->nInfoNum,0);
	GetNum = min(GetNum,MaxItem);
	IMemcpy(pSqi, &pAns->cSeatItem[0], sizeof(SEATQUERYINFO)*GetNum);
	return GetNum;
}

//三方F10协议
BOOL CGetData::DSGetNewJbmCfg (const char * code, short setcode,short whichjbm)
{
	struct ds_f10cfg_req * reqp;
	reqp = ( struct ds_f10cfg_req * ) pTransfer;
	reqp->req = DS_F10CFG_REQ;
	reqp->setcode = setcode;
	memmove ( reqp->code, code, SH_CODE_LEN );
	reqp->whichjbm = whichjbm;
	reqp->reserved = 0;
	if(g_pComm3->SendIt( pTransfer,sizeof ( struct ds_f10cfg_req ) ) != 0)
		return FALSE;
	return TRUE;
}

int CGetData::DSGetNewJbmCfgACK ()
{
	struct f10cfg_ans  * ansp;
	ansp = ( struct f10cfg_ans * ) g_pReceive3;
	g_F10CaptionNum = ansp->titlenum;
	g_F10CaptionNum = max(0,g_F10CaptionNum);
	g_F10CaptionNum = min(MAX_F10CAPTION,g_F10CaptionNum);
	memcpy(g_NewF10Caption,ansp->list,g_F10CaptionNum*sizeof(struct ds_f10cfg_title));
	for(int i=0;i < g_F10CaptionNum;i++)
		_FL(g_NewF10Caption[i].sTitle);
	return g_F10CaptionNum;
}

BOOL CGetData::DSGetNewJbmText (const char * code, short setcode,short which,char *sFilePath,long nOffset,long nLength,short whichjbm)
{
	struct ds_f10txt_req * wzreqp;
	wzreqp = ( struct ds_f10txt_req * ) pTransfer;
	wzreqp->req = DS_F10TXT_REQ;
	wzreqp->setcode = setcode;
	memmove ( wzreqp->code, code, SH_CODE_LEN );
	wzreqp->which = which;
	strncpy(wzreqp->sFilePath,sFilePath,78);
	wzreqp->nOffset = nOffset;
	wzreqp->nLength = nLength;
	wzreqp->whichjbm = whichjbm;
	wzreqp->reserved = 0;
	if(g_pComm3->SendIt( pTransfer,sizeof ( struct ds_f10txt_req ) ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::DSGetNewJbmTextACK(char * buffer )
{
	struct ds_f10txt_ans    * wzansp;
	wzansp = ( struct ds_f10txt_ans * ) g_pReceive3;
	if(wzansp->num<=0 || wzansp->num>30*1024) 
		return 0;
	memcpy( buffer, wzansp->buf, wzansp->num);
	buffer[wzansp->num] = 0;
	_FL(buffer);
	return wzansp->num;
}
//////////////////////////////////////////////////////////////////////////
BOOL CGetData::DSGetZHSort(int GpType,int Num)
{
	struct zhsort_req *zhsortreqp;
	zhsortreqp = (struct zhsort_req *)pTransfer;
	zhsortreqp->req = ZHSORT_DS_NREQ;
	zhsortreqp->domain = GpType;
	zhsortreqp->num = Num;
	if(g_pComm3->SendIt( pTransfer, sizeof ( struct zhsort_req )) != 0)
		return FALSE;
	return TRUE;
}

BOOL CGetData::DSGetZHSortACK(ZHRESULT Buf[9])
{
	MemStkInfo *pStkInfo=NULL;
	CurrStockData tmpHQ;
	struct zhsort_ans * zhsortansp;
	zhsortansp = ( struct zhsort_ans * )g_pReceive3;
	int tmpSize = 1+SH_CODE_LEN+2*sizeof(float),pos,which;
	for(int i=0;i < 9;i++)
	{
		which = 0;
		for(int j=0;j < zhsortansp->num;j++)
		{
			pos=i*zhsortansp->num+j;
			memcpy(&(Buf[i].SetCode[which]),zhsortansp->buf+pos*tmpSize,1);	//Protocol Change,增加SetCode
			memcpy(Buf[i].Code[which],zhsortansp->buf+pos*tmpSize+1,SH_CODE_LEN);
			Buf[i].Code[which][SH_CODE_LEN] = 0;
			pStkInfo=g_pStockIO->Get_MemStkInfo(Buf[i].Code[which],Buf[i].SetCode[which]);
			g_pStockIO->ReadCurrStockData(pStkInfo,&tmpHQ);
			if(!pStkInfo)
				continue;
			memcpy(&(Buf[i].Now[which]),zhsortansp->buf+pos*tmpSize+1+SH_CODE_LEN,sizeof(float));
			memcpy(&(Buf[i].Other[which]),zhsortansp->buf+pos*tmpSize+1+SH_CODE_LEN+sizeof(float),sizeof(float));
			which++;
		}
		Buf[i].Num = which;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
//下载文件
BOOL CGetData::DSGetFile( char * filename, DWORD pos, DWORD flag)
{
	struct ds_file_req * filereqp;

	filereqp = ( struct ds_file_req * ) pTransfer;
	filereqp->req = DS_FILE_REQ;
	filereqp->flag = flag;
	filereqp->pos = pos;
	strcpy ( filereqp->fname, filename );
	if(g_pComm3->SendIt( pTransfer, sizeof ( struct ds_file_req )) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::DSGetFileACK(char * buffer, DWORD *pFlag)
{
	struct ds_wz_ans         * wzansp;
	wzansp = ( struct ds_wz_ans * ) g_pReceive3;
	memmove( buffer, wzansp->buf, wzansp->num );
	buffer[wzansp->num] = 0;
	if(pFlag) memcpy(pFlag, &wzansp->flag, sizeof(DWORD));
	return wzansp->num;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

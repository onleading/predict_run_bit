#include "stdafx.h"
#include "HQComm.h"
#include "getdata.h"
#include "BlowFish.h"
#include "AutoupInfo.h"
#include "MainFrm.h"

CGetData::CGetData ( void )
{
}

CGetData::~CGetData ( void )
{
	if ( pTransfer ) delete pTransfer;
}

BOOL CGetData::Create ( void )
{
	if ( !g_pComm ) return FALSE;
	pTransfer = NULL;
	pTransfer = new char[REQBUFFER_LEN+20];
	if ( !pTransfer)
	{
		TDX_MessageBox(NULL,"致命错误,内存不足!",MB_OK|MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}

/****************************************************************/
/*				非阻塞式GetData函数及其ACK						*/	
/****************************************************************/

BOOL CGetData::GetStockNum ( short setcode, long rq )
{
	struct stknum_req * stknumreqp;

	stknumreqp = ( struct stknum_req * ) pTransfer;
	stknumreqp->req = STOCKNUM_NREQ;
	stknumreqp->setcode = setcode;
	stknumreqp->rq = rq;

	if ( g_pComm->SendIt( pTransfer,sizeof( struct stknum_req ) ) !=0 )
		return FALSE;
	return TRUE;
}

short CGetData::GetStockNumACK ()
{
	short stknum;
	memmove ( &stknum, g_pReceive, sizeof ( short ) );
	return stknum;
}

BOOL CGetData::GetRXxmCfg ( void )   // 取远程消息面配置
{
	short req;
	req = XXMCFG_REQ;
	memmove ( pTransfer, &req, sizeof ( short ) );
	if(g_pComm->SendIt( pTransfer,sizeof(short) ) != 0)
		return FALSE;
	return TRUE;
}

BOOL CGetData::GetRXxmCfgACK()   // 取远程消息面配置
{
	short i, num, type;
	memmove ( ( char * )&num, g_pReceive, sizeof ( short ) );
	XXMNum = 0;
	for ( i = 0; i < num; i++ )
	{
		memmove ( ( char * )&type, g_pReceive + sizeof ( short ) + num * 22 + i * sizeof ( short ), sizeof ( short ) );
		if ( type != TEXTXX ) type = YCQSXX;
		XXMType[i] = type;
		XXMTopic[i] = g_pReceive+2+i*22;
		XXMTopic[i] = XXMTopic[i].Left(17);
		XXMTopic[i].TrimRight();
		XXMNum++;
		if(XXMNum >= 12) break;
	}
	return TRUE;
}

BOOL CGetData::HqAlive( void )
{
	short req;
	req = ALIVE_REQ;
	memmove ( pTransfer, &req, sizeof ( short ) );
	if(g_pComm->SendIt( pTransfer,sizeof(short) ) != 0)
		return FALSE;
	return TRUE;
}

BOOL CGetData::HqAliveACK (DWORD & nMaxTJResult,DWORD & nADGUID)
{
	short urgentnum;
	memcpy(&urgentnum,g_pReceive,sizeof(short));
	memcpy(&nMaxTJResult,g_pReceive+sizeof(short),sizeof(DWORD));
	memcpy(&nADGUID,g_pReceive+sizeof(short)+sizeof(DWORD),sizeof(DWORD));
	if(g_nUrgentNum < urgentnum)
	{
		g_nUrgentNum = urgentnum;
		return TRUE;
	}
	return FALSE;
}

BOOL CGetData::GetUrgentCfg( const char *username )   // 取广告和紧急公告
{
	short req;
	if(!g_bCanUserUrgent)
	{		
		req = URGENT_REQ;
		memmove ( pTransfer, &req, sizeof ( short ) );
		if(g_pComm->SendIt( pTransfer,sizeof(short) ) != 0)
			return FALSE;
		return TRUE;
	}
	else
	{
		struct user_urgent_req *reqp = (struct user_urgent_req *)pTransfer;
		memset(reqp,0,sizeof(struct user_urgent_req));
		reqp->req = USER_URGENT_REQ;
		if(g_OEM.bFormatAndUseYYBID)
		{
			CString tmpStr;
			tmpStr.Format("%04d",g_JyInfo.yybid);
			strcpy(reqp->username,tmpStr);
		}
		else if(strlen(username)>0)
			strcpy(reqp->username,username);
		if(g_pComm->SendIt( pTransfer,sizeof(struct user_urgent_req) ) != 0)
			return FALSE;
		return TRUE;
	}
}

BOOL CGetData::GetUrgentCfgACK()   // 取通告附加句和紧急公告
{
	struct urgent_ans *urgentansp = (struct urgent_ans *)g_pReceive;
	struct Cfg_Struct CfgStruct={0};
	g_ManyHostInfo.NotifyStr[0] = 0;  //通告附加句
	if(g_GC.bNoUrgent)
		return TRUE;
	CFile UrgentFile;
	if(UrgentFile.Open(g_WSXHStr+"tmp\\urgent.dat",CFile::modeCreate|CFile::modeWrite))
	{
		if(1 == urgentansp->valid)
		{
			CfgStruct.EndTime = urgentansp->ldate;
			if(urgentansp->advertlen>0)
				memcpy(CfgStruct.NotifyStr,urgentansp->buf,urgentansp->advertlen);
			CfgStruct.NotifyStr[90] = 0;
			strcpy(g_ManyHostInfo.NotifyStr,CfgStruct.NotifyStr);
			if(urgentansp->sourcelen>0)
				memcpy(CfgStruct.UrgentSource,urgentansp->buf+urgentansp->advertlen,urgentansp->sourcelen);
			CfgStruct.UrgentSource[30]=0;
			if(urgentansp->contentlen>0)
				memcpy(CfgStruct.UrgentContent,urgentansp->buf+urgentansp->advertlen+urgentansp->sourcelen,min(2048,urgentansp->contentlen));
			CfgStruct.UrgentContent[2048] = 0;
			UrgentFile.Write(&CfgStruct,sizeof(struct Cfg_Struct));
		}
		UrgentFile.Close();
	}
	return TRUE;
}

BOOL CGetData::GetURLUrgentCfg( void )   // 取广告和紧急公告
{
	short req;
	req = GET_URLURGENT_NREQ;
	memmove ( pTransfer, &req, sizeof ( short ) );
	if(g_pComm->SendIt( pTransfer,sizeof(short) ) != 0)
		return FALSE;
	return TRUE;
}

BOOL CGetData::GetURLUrgentCfgACK()   // 取广告和紧急公告
{
	struct get_urlurgent_ans *urgentansp = (struct get_urlurgent_ans *)g_pReceive;
	g_URLEndTime = urgentansp->URLEndTime;
	strcpy(g_URLTitle,urgentansp->URLTitle);
	_FL(g_URLTitle);
	strcpy(g_URL,urgentansp->URL);
	CTime NowTime = CTime::GetCurrentTime();
	long lNowTime = TimeToLong(NowTime);
	if(g_URLEndTime < lNowTime)	g_URLEndTime = 0;
	else if(strlen(g_URLTitle) < 3)	g_URLEndTime = 0;
	return TRUE;
}

BOOL CGetData::GetHostMoreInfo()
{
    struct hostmore_req * morereqp;
	morereqp = ( struct hostmore_req * ) pTransfer;
	morereqp->req = HOSTMORE_EREQ;
	//verflag含义 0:测试版 1:正式版 20:Level2版的magicchar1
	morereqp->verflag = 1;

	if(g_pComm->SendIt( pTransfer,sizeof(struct hostmore_req) ) != 0)
		return FALSE;
	return TRUE;
}

BOOL CGetData::GetHostMoreInfoACK(long &szinfdate,long &shinfdate,long &szinfhms,long &shinfhms,char &errflag)
{
	struct hostmore_ans *moreansp = (struct hostmore_ans *)g_pReceive;
	int i;
	//对时
	SYSTEMTIME time,ltime;
	GetLocalTime( &ltime );
	g_LoginLocalSeconds = ltime.wHour*3600+ltime.wMinute*60+ltime.wSecond;	//对时时的本地时间
	time.wYear = (moreansp->date).da_year;
	time.wMonth = (moreansp->date).da_mon;
	time.wDay = (moreansp->date).da_day;
	time.wHour = (moreansp->time).ti_hour;
	time.wMinute = (moreansp->time).ti_min;
	time.wSecond = (moreansp->time).ti_sec;
	time.wMilliseconds = 0.1*(moreansp->time).ti_hund;
	g_LoginServerSeconds = time.wHour*3600+time.wMinute*60+time.wSecond;	//对时时的服务器时间
	g_LoginServerDate = time.wYear*10000+time.wMonth*100+time.wDay;			//对时时的服务器日期
	/*
	//对于个人版,判断是否到期
	if(g_bUserHostEverConnect && g_pConnectWnd)
	{
		g_AntiHack.bJudgeExpireDate = TRUE;
		long expiredate = g_ExpireDate;
		if(g_LoginServerDate>expiredate)
		{
			TDX_MessageBox(NULL,"当前高级行情帐号已过期,请联系软件提供商进行续期!\r\n如需继续使用,可以切换到传统/免费行情方式进入",MB_OK|MB_ICONERROR);
			errflag = 0;
			return FALSE;
		}
		else
		{
			expiredate=min(expiredate,20300101);
			CTime keyTime=LongToTime(expiredate),serverTime=LongToTime(g_LoginServerDate);
			CTimeSpan span=keyTime-serverTime;
			if(span.GetDays()<10) //提前10天报过期
			{
				CString tmpHint;
				tmpHint.Format("高级行情帐号的到期时间是%d,再过%d天就要过期了.\r\n请联系软件提供商进行续期!\r\n点击[确定]继续运行.",expiredate,span.GetDays());
				TDX_MessageBox(g_pConnectWnd->m_hWnd,tmpHint,MB_OK|MB_ICONEXCLAMATION);
			}
		}
	}
#ifdef LEVEL2
	if(g_nCurLevel2Tab==1)
	{
		g_GC.bHasGPGrade=FALSE;
		g_GC.bHasTend=FALSE;
		g_GC.bNoAutoup=TRUE;
	}
	else
	{
		g_GC.bHasGPGrade=TRUE;
		g_GC.bHasTend=TRUE;
		g_GC.bNoAutoup=FALSE;
	}
#else
	if(g_GC.bHasLevel2Engine && g_bUserHostEverConnect && g_GC.bHasTdxInfo)
	{
		g_GC.bHasGPGrade=TRUE;
		g_GC.bHasTend=TRUE;		
		g_strTendName="tend_std";
		g_strTendCfg="tend_std.cfg";
	}
	else if(g_OEM.bHasTdxTend)
		g_GC.bHasTend=TRUE;			
#endif
#ifdef BIG5
	g_GC.bHasGPGrade = FALSE;
#endif
	*/
	//对时后，重新得到当前时间
	CTime tmpTime(time);
	g_tNowTime = tmpTime;
	g_iToday=g_tNowTime.GetYear()*10000L+100L*g_tNowTime.GetMonth()+g_tNowTime.GetDay();

	//开收市时间
	for(i = 0;i < 8;i++)
	{   
		sz[i] = moreansp->sz[i];
 		sh[i] = moreansp->sh[i];
	}
	//主站信息
	g_bYesterdayFlag = moreansp->byesterday;
	szinfdate = moreansp->szinfdate;
	shinfdate = moreansp->shinfdate;
	szinfhms = moreansp->szinfhms;
	shinfhms = moreansp->shinfhms;

	errflag = moreansp->errflag;
	/*
	g_bHasZXQY = moreansp->bserv1 & 0x02;
	g_bNo003InAG = moreansp->bserv1 & 0x04;
	g_bSupportETF = moreansp->bserv1 & 0x08;
	g_bSupportDownMin5 = moreansp->bserv1 & 0x40;
	if(!g_bSupportDownMin5)
		g_nZFFzNum=5;
	g_bG2DT = moreansp->bserv1 & 0x80;
	g_bG2DT = 0;
	g_bSupportLevel2 = moreansp->bserv2 & 0x01;
	g_bSupportDDE = moreansp->bserv2 & 0x04;
	//如果支持L2的话，g_bG2DT肯定为TRUE
	g_bHasF10Cfg = moreansp->bserv3 & 0x01;
	g_bUseSpecVolumeUnit = moreansp->bserv3 & 0x02;
	if(g_bG2DT)
		g_bUseSpecVolumeUnit = TRUE;
	g_nUrgentNum = moreansp->nUrgentNum;
	errflag = moreansp->errflag;
	g_bUseBigJBM = moreansp->bbigjbm;
	//
	g_TdxCfg.JbmNum=1;
	if(moreansp->jbmnum>=1 && moreansp->jbmnum<=3)
		g_TdxCfg.JbmNum=moreansp->jbmnum;
	//
	g_bCanUserUrgent = moreansp->bcanuserurgent;
	g_bHasBakHostFile = moreansp->hasBakHostFile;
	strcpy(g_ManyHostInfo.strHostName,moreansp->hostname);
	_FL(g_ManyHostInfo.strHostName);
	for(i=0;i<g_TdxCfg.JbmNum;i++)
	{
		if(moreansp->Jbmname[i][0]=='#')
		{
			strncpy(g_TdxCfg.Jbmname[i],moreansp->Jbmname[i]+1,9);
			g_TdxCfg.Jbmname[i][9]=0;
		}
	}
	g_ManyHostInfo.bWebPage = moreansp->webpage;
	if(g_ManyHostInfo.bWebPage)	strcpy(g_ManyHostInfo.strURL,moreansp->url);
	if(g_ManyHostInfo.bWebPage)
		ShellExecute(NULL, _T("open"), g_ManyHostInfo.strURL, NULL, NULL, SW_SHOW);
	*/

	return TRUE;
}

BOOL  CGetData::GetPassPort(char *QsId,float clientver)
{
    struct get_passport_req * reqp;
	reqp = ( struct get_passport_req * ) pTransfer;
	memset(reqp,0,sizeof(struct get_passport_req));
	reqp->req = GET_PASSPORT_NREQ;
	reqp->clientver = clientver;
	reqp->AutoupOrder = g_GC.WhichAutoUpInfo-1;	//升级标识,所有的版本都根据此标识,繁体版亦如此
	strcpy(reqp->QsId,QsId);
	if(g_bHasAdvHQVersion)
	{	
	#ifdef OEM_NEWJY
		if(g_GC.bHasLevel2Engine)		
			reqp->AutoupOrder = 4; //对于插件版，升级包固定为4(超级终端版)
	#endif
		reqp->ProductIDchar = 'T';
		char cMagic1 = 0,cMagic2  = 0;
		reqp->magicchar2 = cMagic2;
		strcpy(reqp->ProductID,g_GC.Level2_AutoupId);
	}
	else
	{
	#ifdef ZSZQJZ
		strcpy(reqp->ProductID,"招商金钻");
	#endif
	}
	reqp->valid_level=2; //5.62内核有效等级是1,早期的是0
	if(g_pComm->SendIt( pTransfer,sizeof(struct get_passport_req) ) != 0)
		return FALSE;
	return TRUE;	
}

BOOL  CGetData::GetPassPortACK(char *errmsg,BOOL bDiscardAutoupInfo)
{
	struct get_passport_ans *ansp = (struct get_passport_ans *)g_pReceive;	
	memset(&g_NeedAutoupInfo,0,sizeof(struct NeedAutoupInfo));
	if(ansp->Sucflag==0)
		strcpy(errmsg,ansp->errmsg);
	/*
	else if(!bDiscardAutoupInfo)
	{		
		g_NeedAutoupInfo.bNeedAutoup=ansp->bNeedAutoup;
		g_NeedAutoupInfo.ThisVer=ansp->ThisVer;
		if(ansp->ThisVer<0.001)	//如果版本不支持,或者没有配置,则不提示自动升级
			g_NeedAutoupInfo.bNeedAutoup=0;		
		strcpy(g_NeedAutoupInfo.AutoupURL,ansp->AutoupURL);		
		strncpy(g_NeedAutoupInfo.Descripton,ansp->Descripton,2*1024-2);		
		g_NeedAutoupInfo.Descripton[2*1024-2]=0;
		g_NeedAutoupInfo.bAutoupFromHQHost=ansp->bAutoupFromHQHost;
	}
	WriteAutoupInfo(&g_NeedAutoupInfo);
	*/
	return (ansp->Sucflag)?TRUE:FALSE;
}

//(再次INMODE拨入请求)
BOOL CGetData::Inmode2(const char *username,const char *nickname)
{
    struct inmode2_req * inmode2reqp;
	inmode2reqp = ( struct inmode2_req * ) pTransfer;
	memset(inmode2reqp,0,sizeof(inmode2_req));

	inmode2reqp->req = INMODE2_EREQ;
	inmode2reqp->type = HQ_SERTYPE;
	inmode2reqp->inmode = TDXW_INMODE;
	strncpy(inmode2reqp->username,username,12);
	strncpy(inmode2reqp->nickname,nickname,20);
	if(g_pComm->SendIt(pTransfer,sizeof ( inmode2_req )) != 0)
		return FALSE;
	return TRUE;
}

BOOL CGetData::Inmode2ACK()
{
	struct inmode2_ans *inmode2ansp;
	inmode2ansp = (struct inmode2_ans *)g_pReceive;
	if(inmode2ansp->ch != 0) return FALSE;
	return TRUE;
}

//(再次INMODE拨入请求)
BOOL CGetData::NeedAuth(const char *username,const char *password)
{
    struct need_auth_req * reqp;
	reqp = ( struct need_auth_req * ) pTransfer;
	memset(reqp,0,sizeof(need_auth_req));

	reqp->req = NEED_AUTHP_REQ;
	strncpy(reqp->username,username,12);
	strncpy(reqp->password,password,12);
	if(g_pComm->SendIt(pTransfer,sizeof ( need_auth_req )) != 0)
		return FALSE;
	return TRUE;
}

BOOL CGetData::NeedAuthACK(char &sucflag,char *errmsg,char &need_auth)
{
	struct need_auth_ans *ansp;
	ansp = (struct need_auth_ans *)g_pReceive;
	
	sucflag = ansp->sucflag;
	strncpy(errmsg,ansp->errmsg,48);
	errmsg[48] = 0;
	need_auth = ansp->need_auth;

	TDEL(g_WTVerifyIP);
	g_WtVerifyIPNum = 0;
	if(need_auth==1)
	{
		g_WtVerifyIPNum = ansp->ipnum;
		g_WTVerifyIP = new struct IpInfo[g_WtVerifyIPNum];
		memcpy(g_WTVerifyIP,ansp->Info,g_WtVerifyIPNum*sizeof(struct IpInfo));
	}	
	return TRUE;
}
/************************************************************************************
						分析等数据异步请求
*************************************************************************************/

//行情数据请求
BOOL CGetData::GetCurrStockData(struct MemStkInfo *scode)
{ 
	if ( !scode ) return FALSE;

	if(g_bG2DT)
	{
		struct CurrStockData tmpHq;
		g_pStockIO->ReadCurrStockData(scode,&tmpHq);
		memcpy(SingleCodeHead,&scode->setcode,1);
		memcpy(SingleCodeHead+1,scode->Code,SH_CODE_LEN);
		memcpy(SingleCodeHead+1+SH_CODE_LEN,&tmpHq.RefreshNum,sizeof(DWORD));
		return GetG2MultiCurrStockData(SingleCodeHead,1);
	}
	struct gghq_req * gghqreqp;
	gghqreqp = ( struct gghq_req * ) pTransfer;
	gghqreqp->req = g_bSupportETF?GGHQ2_5MMP_NREQ:GGHQ_5MMP_NREQ;
	gghqreqp->setcode = scode->setcode;
	memmove ( gghqreqp->code, scode->Code, SH_CODE_LEN );
	if(g_pComm->SendIt ( pTransfer,  sizeof ( struct gghq_req ) ) != 0)
		return FALSE;
	return TRUE;
}

BOOL CGetData::GetCurrStockDataACK(struct MemStkInfo *scode,struct CurrStockData * hqp,struct CurrStockData_L2 *hqp_l2)
{
	if(g_bG2DT)
	{
		struct CurrStockData_Code tmpDataCode;
		GetG2MultiCurrStockDataACK (&tmpDataCode,1);
		g_pStockIO->ReadCurrStockData(scode,hqp,hqp_l2);
		return TRUE;
	}
	memset ( hqp, 0, sizeof ( struct CurrStockData ) );
	hqp->Close = COMPPREC;
	if ( !scode ) return FALSE;
	short	len = 0;
	CurrStockData_rtn ( g_pReceive + len, hqp, len, TRUE,g_bSupportETF);
	g_pStockIO->SetCurrStockData(scode->setcode,scode->Index,(char *)hqp);
	if(scode->setcode==g_ET.GPSetCode && strcmp(g_ET.GPCode,scode->Code)==0)
	{
		if(g_ET.HWnd) //向调用者的窗口发送数据消息
			ETSendCopyDataMessage(g_ET.HWnd,scode,hqp);
		g_ET.Ready = 1;
	}
	else
		g_ET.HWnd = NULL;
	if(InStatusBarGP(scode))
		((CMainFrame *)AfxGetMainWnd())->RefreshStatusBar();
	return TRUE;
}

BOOL CGetData::GetMultiCurrStockData ( char * codehead, short num)
{
	if(g_bG2DT)
	{
		MemStkInfo *tmpInfo;
		char setcode;
		char Code[SH_CODE_LEN+1];
		struct CurrStockData tmpHq;
		int tmpCodeLen=1+SH_CODE_LEN+sizeof(DWORD);
		for(int i=0;i<num;i++)
		{
			memcpy(&setcode,codehead+i*(1+SH_CODE_LEN),1);
			memcpy(Code,codehead+i*(1+SH_CODE_LEN)+1,SH_CODE_LEN);
			Code[SH_CODE_LEN]=0;
			tmpInfo=g_pStockIO->Get_MemStkInfo(Code,setcode);
			g_pStockIO->ReadCurrStockData(tmpInfo,&tmpHq);
			memcpy(SingleCodeHead+i*tmpCodeLen,codehead+i*(1+SH_CODE_LEN),(1+SH_CODE_LEN));
			memcpy(SingleCodeHead+i*tmpCodeLen+(1+SH_CODE_LEN),&tmpHq.RefreshNum,sizeof(DWORD));
		}
		return GetG2MultiCurrStockData(SingleCodeHead,num);
	}
	struct combhq_req * hqreqp;
	hqreqp = ( struct combhq_req * ) pTransfer;
	hqreqp->req = g_bSupportETF?COMBHQ_2_5MMP_NREQ:COMBHQ_5MMP_NREQ;
	hqreqp->num       = num;
	memmove ( hqreqp->codehead, codehead, num * (1+SH_CODE_LEN) );
	if(g_pComm->SendIt ( pTransfer,sizeof ( struct combhq_req ) + num * (1+SH_CODE_LEN) - 1 ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetMultiCurrStockDataACK (struct CurrStockData_Code * pTmp,int maxnum)
{
	if(g_bG2DT)
		return GetG2MultiCurrStockDataACK(pTmp,maxnum);
	short i,len,num;
	memmove ( &num, g_pReceive+sizeof ( short ), sizeof ( short ) );
	num = min(num,maxnum);
	len = 2 * sizeof ( short );
	MemStkInfo *tmpscode;
	BOOL bStatusGP=FALSE;
	for ( i = 0; i < num; i++ )
	{
		CurrStockData_Code_rtn ( g_pReceive + len, pTmp + i, len,TRUE,g_bSupportETF);
		tmpscode = g_pStockIO->Get_MemStkInfo(pTmp[i].Code,pTmp[i].SetCode);
		if(!tmpscode) continue;
		g_pStockIO->SetCurrStockData(tmpscode->setcode,tmpscode->Index,(char *)(&(pTmp[i].hq)));
		if(tmpscode->setcode==g_ET.GPSetCode && strcmp(g_ET.GPCode,tmpscode->Code)==0)
		{
			if(g_ET.HWnd) //向调用者的窗口发送数据消息
				ETSendCopyDataMessage(g_ET.HWnd,tmpscode,&pTmp[i].hq);
			g_ET.Ready = 1;
		}
		if(!bStatusGP)
			bStatusGP = InStatusBarGP(tmpscode);
	}
	if(bStatusGP)
		((CMainFrame *)AfxGetMainWnd())->RefreshStatusBar();	
	return num;
}

BOOL CGetData::GetMultiCurrStockDataEx (short setDomain, short coltype, short startxh, short wantnum, short sorttype)
{
	if(g_bSupportDownMin5)
	{
		struct multihq_new_req * hqreqp;
		hqreqp = ( struct multihq_new_req * ) pTransfer;
		memset(hqreqp,0,sizeof(struct multihq_new_req));
		hqreqp->req = MULTIHQ_NEW_NREQ;
		hqreqp->setDomain = setDomain;
		hqreqp->coltype   = coltype;
		hqreqp->startxh   = startxh;
		hqreqp->wantnum   = wantnum;
		hqreqp->sorttype  = sorttype;
		hqreqp->zfmin	  = g_nZFFzNum;
		if ( g_pComm->SendIt( pTransfer,sizeof( struct multihq_new_req ) ) !=0 )
			return FALSE;
		return TRUE;
	}
	struct multihq_req * hqreqp;
	hqreqp = ( struct multihq_req * ) pTransfer;
	hqreqp->req = g_bNo003InAG?MULTIHQ3_5MMP_NREQ:MULTIHQ2_5MMP_NREQ;
	hqreqp->setDomain = setDomain;
	hqreqp->coltype   = coltype;
	hqreqp->startxh   = startxh;
	hqreqp->wantnum   = wantnum;
	hqreqp->sorttype  = sorttype;
	if ( g_pComm->SendIt( pTransfer,sizeof( struct multihq_req ) ) !=0 )
		return FALSE;
	return TRUE;
}

BOOL CGetData::GetMultiCurrStockDataEx ( char * codehead, short num)
{  
	if(g_bSupportDownMin5)
	{
		struct combhq_new_req  * hqreqp;
		hqreqp = ( struct combhq_new_req  * ) pTransfer;
		memset(hqreqp,0,sizeof(struct combhq_new_req));
		hqreqp->req		= COMBHQ_NEW_NREQ;
		hqreqp->zfmin	= g_nZFFzNum;
		hqreqp->num		= num;
		memmove ( hqreqp->codehead, codehead, num * (1+SH_CODE_LEN) );
		if(g_pComm->SendIt ( pTransfer,sizeof ( struct combhq_new_req  ) + num * (1+SH_CODE_LEN) - 1 ) != 0)
			return FALSE;
		return TRUE;
	}
	struct combhq_req * hqreqp;
	hqreqp = ( struct combhq_req * ) pTransfer;
	hqreqp->req = COMBHQ2_5MMP_NREQ;
	hqreqp->num       = num;
	memmove ( hqreqp->codehead, codehead, num * (1+SH_CODE_LEN) );
	if(g_pComm->SendIt ( pTransfer,sizeof ( struct combhq_req ) + num * (1+SH_CODE_LEN) - 1 ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetMultiCurrStockDataExACK (struct CurrStockDataEx * pExTmp,int maxnum)
{
	short i,len,num;
	memmove ( &num, g_pReceive+sizeof ( short ), sizeof ( short ) );
	num = min(num,maxnum);
	len = 2 * sizeof ( short );
	MemStkInfo *tmpscode;
	for ( i = 0; i < num; i++ )
	{
		CurrStockDataEx_rtn ( g_pReceive + len, pExTmp + i, len ,TRUE);
		tmpscode = g_pStockIO->Get_MemStkInfo(pExTmp[i].Code,pExTmp[i].SetCode);
		if(!tmpscode) 
			continue;
		g_pStockIO->SetCurrStockData(tmpscode->setcode,tmpscode->Index,(char *)(&(pExTmp[i].hq)));
		tmpscode->dkflag = pExTmp[i].dkflag;
		tmpscode->tbp = pExTmp[i].tbp;
		tmpscode->stoplost = pExTmp[i].stoplost;
		tmpscode->leave = pExTmp[i].leave;
		tmpscode->zangsu = pExTmp[i].zangsu;
		tmpscode->nWarnCount = pExTmp[i].nWarnCount;
	}
	return num;
}

//请求Level2的数据
BOOL CGetData::GetG2MultiCurrStockData ( char * codehead, short num)
{
	struct combhq_req * hqreqp;
	hqreqp = ( struct combhq_req * ) pTransfer;
	if(CanLevel2() && g_bSupportDDE)
		hqreqp->req	= G2_COMBHQ2_NREQ;
	else
		hqreqp->req	= G2_COMBHQ_NREQ;
	hqreqp->num = num;
	memmove ( hqreqp->codehead, codehead, num * (1+SH_CODE_LEN+sizeof(DWORD)) );
	if(g_pComm->SendIt ( pTransfer,sizeof ( struct combhq_req ) + num * (1+SH_CODE_LEN+sizeof(DWORD)) - 1 ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetG2MultiCurrStockDataACK (struct CurrStockData_Code * pTmp,int maxnum)
{
	short len,num;
	for(int i=0;i<g_AnsHeader.RawLen;i++)
	{
		if(CanLevel2() && g_bSupportDDE)
			g_pReceive[i]^=g_AntiHack.cLevel2XORChar_ComboHQ2;
		else
			g_pReceive[i]^=g_AntiHack.cLevel2XORChar_ComboHQ;		
	}
	memmove ( &num, g_pReceive, sizeof ( short ) );
	num = min(num,maxnum);
	len = sizeof ( short );
	MemStkInfo *tmpscode;
	BOOL bStatusGP=FALSE;
	for ( i = 0; i < min(maxnum,num); i++ )
	{
		CurrStockDataG2_Code_rtn ( g_pReceive + len, pTmp + i, len);
		tmpscode = g_pStockIO->Get_MemStkInfo(pTmp[i].Code,pTmp[i].SetCode);
		if(!tmpscode) continue;
		g_pStockIO->SetCurrStockData(tmpscode->setcode,tmpscode->Index,(char *)(&(pTmp[i].hq)),(char *)(&(pTmp[i].hql2)));
		if(tmpscode->setcode==g_ET.GPSetCode && strcmp(g_ET.GPCode,tmpscode->Code)==0)
		{
			if(g_ET.HWnd) //向调用者的窗口发送数据消息
				ETSendCopyDataMessage(g_ET.HWnd,tmpscode,&pTmp[i].hq);
			g_ET.Ready = 1;
		}
		if(!bStatusGP)
			bStatusGP = InStatusBarGP(tmpscode);
	}
	if(bStatusGP)
		((CMainFrame *)AfxGetMainWnd())->RefreshStatusBar();
	return num;
}

//请求Level2的数据
BOOL CGetData::GetL2MultiCurrQueue ( char * codehead, short num)
{
	if(!CanLevel2()) return FALSE;	
	struct combhq_req * hqreqp;
	hqreqp = ( struct combhq_req * ) pTransfer;
	hqreqp->req	= L2_COMBQUEUE_NREQ;
	hqreqp->num = num;
	memmove ( hqreqp->codehead, codehead, num * (1+SH_CODE_LEN+sizeof(DWORD)) );
	if(g_pComm->SendIt ( pTransfer,sizeof ( struct combhq_req ) + num * (1+SH_CODE_LEN+sizeof(DWORD)) - 1 ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetL2MultiCurrQueueACK (struct CurrQueue_Code * pTmp,int maxnum)
{
	short len,num;
	for(int i=0;i<g_AnsHeader.RawLen;i++)
		g_pReceive[i]^=g_AntiHack.cLevel2XORChar_Queue;
	memmove ( &num, g_pReceive, sizeof ( short ) );
	len = sizeof ( short );
	MemStkInfo *tmpscode;
	for ( i = 0; i < min(maxnum,num); i++ )
	{
		CurrQueueL2_Code_rtn ( g_pReceive + len, pTmp + i, len);
		tmpscode = g_pStockIO->Get_MemStkInfo(pTmp[i].Code,pTmp[i].SetCode);
		if(!tmpscode) continue;
		g_pStockIO->SetCurrQueue(tmpscode->setcode,tmpscode->Index,(char *)(&(pTmp[i].queue)));
	}
	return num;
}

BOOL CGetData::GetStatData( char * codehead, short num,BOOL bImg )
{
	if(!g_bSupportDDE) 
		return FALSE;	
	struct combhq_req * hqreqp;
	hqreqp = ( struct combhq_req * ) pTransfer;
	if(bImg)
		hqreqp->req	= L2_IMGSTAT_NREQ;
	else
		hqreqp->req	= L2_NEWTCKSTAT_NREQ;
	hqreqp->num = num;
	memmove ( hqreqp->codehead, codehead, num * (1+SH_CODE_LEN+sizeof(DWORD)) );
	if(g_pComm->SendIt ( pTransfer,sizeof ( struct combhq_req ) + num * (1+SH_CODE_LEN+sizeof(DWORD)) - 1 ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetStatDataAck(BOOL bImg,short *lpnStkIndex,int nCnt)
{
	short len,num;
	for(int i=0;i<g_AnsHeader.RawLen;i++)
		g_pReceive[i]^=0x77;
	memmove ( &num, g_pReceive, sizeof ( short ) );
	len = sizeof ( short );

	MemStkInfo *tmpscode;
	char		setcode;
	char		code[SH_CODE_LEN];
	int			n = 0,iGPNo = -1;
	SUMIMGSTAT	ImgStat;
	SUMTCKSTAT	TckStat;
	int			nNowSeonds = GetSeconds();
	for ( i = 0; i < num; i++ )
	{
		memcpy(&setcode,g_pReceive+len,1);
		memcpy(code,g_pReceive+len+1,SH_CODE_LEN);
		code[SH_CODE_LEN] = 0;
		len+=1+SH_CODE_LEN;
		tmpscode = g_pStockIO->Get_MemStkInfo(code,setcode);
		if(bImg)
		{
			memcpy(&ImgStat,g_pReceive+len,sizeof(SUMIMGSTAT));
			len+=sizeof(SUMIMGSTAT);
			if(tmpscode)
			{
				tmpscode->nImgStatLastSeconds=max(0,nNowSeonds-6*3600);
				g_pStockIO->SetStatData(tmpscode->setcode,tmpscode->Index,(char *)&ImgStat,NULL);
			}
		}
		else
		{
			memcpy(&TckStat,g_pReceive+len,sizeof(SUMTCKSTAT));
			len+=sizeof(SUMTCKSTAT);
			if(tmpscode)
			{
				tmpscode->nTckStatLastSeconds=max(0,nNowSeonds-6*3600);
				g_pStockIO->SetStatData(tmpscode->setcode,tmpscode->Index,NULL,(char *)&TckStat);
			}
		}
		if(lpnStkIndex && n<nCnt)
		{
			iGPNo = g_pStockIO->GetIndex(code,setcode);
			if(iGPNo>=0)
			{
				lpnStkIndex[n]=iGPNo;
				n++;
			}
		}
	}
	if(lpnStkIndex)
		return n;
	return num;
}

//分时数据请求	
BOOL CGetData::GetMinuteData(struct MemStkInfo * scode,BOOL bHq,short &nNTMinute,struct MinuteData * MinuteDatap,short * TimeSec)
{
	if (!scode)   return FALSE;
	//读缓存
	if(MinuteDatap&&nNTMinute>0&&TimeSec)
	{
		MinuteData TmpMinuteData[1500]={0};
		if(GetCahceMinuteData(scode, TmpMinuteData)&&TmpMinuteData[0].Minute>0)
		{
			int nsiwu = GetMinuteFromFZ(0,1,TimeSec)-TmpMinuteData[0].Minute;
			if(TmpMinuteData[nNTMinute-1].Minute==GetMinuteFromFZ(nNTMinute-1-nsiwu,1,TimeSec)&&
				TmpMinuteData[nNTMinute-1].Now>COMPPREC)
			{
				memcpy(MinuteDatap, TmpMinuteData, nNTMinute*sizeof(MinuteData));
				return TRUE;
			}
		}
	}
	nNTMinute = -1;

	struct combzst_req * zstreqp;
	zstreqp = ( struct combzst_req * )pTransfer;
	zstreqp->req     = MinuteTick_SpecialData(scode)?COMBFST2_NREQ:COMBFST_NREQ;
	zstreqp->setcode = scode->setcode;
	memmove ( zstreqp->code, scode->Code, SH_CODE_LEN );
	zstreqp->hqflag  = bHq?1:0;
	zstreqp->ticknum = 0;
	if(g_pComm->SendIt ( pTransfer,sizeof ( struct combzst_req ) ) != 0)
		return FALSE;
	return TRUE;
}

//注意此函数还没有解决自动开收市时间变更问题
short CGetData::GetMinuteDataACK(struct MemStkInfo * scode, struct MinuteData * MinuteDatap, struct CurrStockData * pTmp)
{
	short			getnums;
	short           i, len;
	char            precise;
	if(!scode)	return 0; 
	memmove (&getnums, g_pReceive, sizeof(short));
	len = 2 * sizeof (short);
	if(pTmp != NULL)
	{
		CurrStockData_rtn ( g_pReceive + len, pTmp, len);
		g_pStockIO->SetCurrStockData(scode->setcode,scode->Index,(char *)pTmp);
	}
	precise = GetPrecise ( scode->Code,scode->setcode );
	MinuteData_rtn ( g_pReceive + len, MinuteDatap, precise, scode->Code,scode->setcode,getnums, len ,MinuteTick_SpecialData(scode));
	(MinuteDatap)->Minute = 570;
	for(i = 1;i < 120;i++)
		(MinuteDatap+i)->Minute = (MinuteDatap+i -1)->Minute +1 ;
	(MinuteDatap+120)->Minute = 780;
	for(i = 121;i < 240;i++)
		(MinuteDatap+i)->Minute = (MinuteDatap+i -1)->Minute +1 ;
	//分时存盘
	CString tempFileName;
	tempFileName.Format("%sLData\\cache\\%s.tfz",g_WSXHStr,GetSetStr(scode->setcode));
	FILE *fpt = fopen( tempFileName, "r+b");
	if(fpt)
	{
		struct MinCacheHead tmpHead;
		long filelen;
		fseek(fpt, scode->Index*sizeof(struct MinCacheHead),SEEK_SET);
		fread(&tmpHead,1,sizeof(struct MinCacheHead),fpt);
		if(tmpHead.suc==1)
		{
			fseek(fpt,tmpHead.pos,SEEK_SET);
			fwrite(MinuteDatap,1,240*sizeof(struct MinuteData),fpt);
		}
		else
		{
			fseek(fpt,0,SEEK_END);
			filelen = on_filelength(fpt);
			fwrite(MinuteDatap,1,240*sizeof(struct MinuteData),fpt);
			fseek(fpt, scode->Index*sizeof(struct MinCacheHead),SEEK_SET);
			tmpHead.suc = 1;
			tmpHead.pos = filelen;
			fwrite(&tmpHead,1,sizeof(struct MinCacheHead),fpt);
		}
		fclose(fpt);
	}
	return getnums;
}

//分时数据请求
BOOL CGetData::GetHisMinuteData(struct MemStkInfo * scode,long lDate)
{
	if (!scode)   return FALSE; 

	struct combzst_his_req * zstreqp;
	zstreqp = ( struct combzst_his_req * )pTransfer;
	zstreqp->req     = MinuteTick_SpecialData(scode)?COMBFST2_HIS_NREQ:COMBFST_HIS_NREQ;
	zstreqp->ldate	 = lDate;
	zstreqp->setcode = scode->setcode;
	memmove ( zstreqp->code, scode->Code, SH_CODE_LEN );
	if(g_pComm->SendIt ( pTransfer,sizeof ( struct combzst_his_req ) ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetHisMinuteDataACK(struct MemStkInfo * scode, struct MinuteData * MinuteDatap, float *pClose,long date)
{
	short			getnums;
	short           i, len;
	char            precise;
	if(!scode)	return 0; 
	short setcode = g_pStockIO->GetDomain(scode);
	memmove (&getnums, g_pReceive, sizeof(short));
	len = sizeof (short);
	memcpy(pClose,g_pReceive+len,sizeof(float));
	len +=sizeof (float);
	precise = GetPrecise ( scode->Code,scode->setcode );
	MinuteData_rtn ( g_pReceive + len, MinuteDatap, precise, scode->Code,scode->setcode,getnums, len ,MinuteTick_SpecialData(scode));
	(MinuteDatap)->Minute = 570;
	for(i = 1;i < 120;i++)
		(MinuteDatap+i)->Minute = (MinuteDatap+i -1)->Minute +1 ;
	(MinuteDatap+120)->Minute = 780;
	for(i = 121;i < 240;i++)
		(MinuteDatap+i)->Minute = (MinuteDatap+i -1)->Minute +1 ;
	
	//如果有date参数,则顺便写临时文件
	if(date > 0)
	{
		long filelen,minNum;
		BOOL bFind = FALSE;
		char string[256];
		sprintf(string,"%szst_cache\\%s%s.zst",g_WSXHStr,GetSetStr(setcode),scode->Code);
		FILE * tmpfp = _fsopen( string, "r+b",SH_DENYNO);
		if (!tmpfp)	tmpfp = _fsopen( string,"wb",SH_DENYNO);
		if (tmpfp)
		{
			filelen = on_filelength(tmpfp);
			minNum = filelen/sizeof(CACHEHISZST);
			if(minNum > 0)
			{
				CACHEHISZST *pTmpCacheZst = new CACHEHISZST[minNum];
				fread(pTmpCacheZst,sizeof(CACHEHISZST),minNum,tmpfp);
				for(i = 0;i < minNum;i++)
				{
					if(pTmpCacheZst[i].date==date)
						break;
				}
				if(i < minNum) bFind = TRUE;
				delete pTmpCacheZst;
			}
			if(!bFind)
			{
				CACHEHISZST tmpCacheZst={0};
				tmpCacheZst.date = date;
				tmpCacheZst.Close = *pClose;
				memcpy(tmpCacheZst.mindata,MinuteDatap,240*sizeof(struct MinuteData));
				fseek(tmpfp,0,SEEK_END);
				fwrite(&tmpCacheZst,sizeof(CACHEHISZST),1,tmpfp);
			}
			fclose(tmpfp);
		}
	}
	return getnums;
}

BOOL CGetData::GetMmld(struct MemStkInfo * scode)
{ 
	if (!scode)   return FALSE; 

	struct mmld_req * mmldreqp;
	mmldreqp = ( struct mmld_req * ) pTransfer;
	mmldreqp->req = MMLD_NREQ;  
	mmldreqp->setcode = scode->setcode;
	memmove ( mmldreqp->code, scode->Code, SH_CODE_LEN );
	if ( g_pComm->SendIt ( pTransfer,sizeof( struct mmld_req ) ) != 0 )
		return FALSE;
	return TRUE;
}

BOOL CGetData::GetMmldACK(short getnums, struct MinuteData * MinuteDatap)
{
	short len,itemnum;
	memmove ( &itemnum, g_pReceive, sizeof ( short ) );
	len = sizeof ( short );
	if ( itemnum > getnums ) itemnum = getnums;
	MmldStruc_rtn ( g_pReceive + len, MinuteDatap, itemnum, len );
	return TRUE;
}

BOOL CGetData::GetLead(struct MemStkInfo * scode)
{ 
	if (!scode)   return FALSE; 

	struct ccl_req * cclreqp;
	cclreqp = ( struct ccl_req * ) pTransfer;
	cclreqp->req = CCL_NREQ;
	cclreqp->setcode = scode->setcode;
	memmove ( cclreqp->code, scode->Code, SH_CODE_LEN );
	if ( g_pComm->SendIt(pTransfer,sizeof( struct ccl_req ) ) != 0)
		return FALSE;
	return TRUE;
}

BOOL CGetData::GetLeadACK(struct MemStkInfo * scode,short &getnums, float * fLBData)
{
	if(!scode)
	{
		getnums = 0;
		return FALSE;
	}
	short itemnum,len;
    memmove ( &itemnum, g_pReceive, sizeof ( short ) );
	if ( itemnum > getnums ) itemnum = getnums;
    len = sizeof ( short );
    VolInStock_rtn ( g_pReceive + len, fLBData, scode->Code, scode->setcode,itemnum, len );
	getnums = itemnum;
	return TRUE;
}

BOOL CGetData::GetAuction(struct MemStkInfo * scode,long nReqNo,long nReqNum)
{

	if (!scode)   return FALSE;

	struct l2_auction_req * reqp = ( struct l2_auction_req * ) pTransfer;
	reqp->req		= L2_AUCTION_NREQ;
	reqp->setcode = scode->setcode;
	memmove ( reqp->code, scode->Code, SH_CODE_LEN );
	reqp->ItemPos	= nReqNo;
	reqp->ItemNum	= nReqNum;
	if(g_pComm->SendIt(pTransfer,sizeof(l2_auction_req)) != 0)
		return FALSE;
	return TRUE;
}

BOOL CGetData::GetAuctionACK(short &getnums,LPVTLDATA pAuction)
{
	short len,itemnum;
	memmove(&itemnum, g_pReceive, sizeof(short));
	len = sizeof(short);
	itemnum = min(itemnum,MAX_DEEPWARN);
	itemnum = max(itemnum,0);
	if(itemnum>0) memcpy(pAuction,g_pReceive+len,itemnum*sizeof(VTLDATA));
	getnums = itemnum;
	return TRUE;
}

BOOL CGetData::GetZmzm(struct MemStkInfo * scode)
{
	if (!scode)   return FALSE; 

	struct l2_zmzm_req * zmzmreqp;
	zmzmreqp = ( struct l2_zmzm_req * ) pTransfer;
	zmzmreqp->req = L2_ZMZM_NREQ;  
	zmzmreqp->setcode = scode->setcode;
	memmove ( zmzmreqp->code, scode->Code, SH_CODE_LEN );
	if ( g_pComm->SendIt ( pTransfer,sizeof( struct l2_zmzm_req ) ) != 0 )	return FALSE;
	return TRUE;
}

BOOL CGetData::GetZmzmACK(short &getnums, LPNORMALSTAT MinuteDatap)
{
	short len,itemnum;
	memmove ( &itemnum, g_pReceive, sizeof ( short ) );
	len = sizeof ( short );
	if ( itemnum > getnums ) itemnum = getnums;
	ZmzmStruc_rtn ( g_pReceive + len, MinuteDatap, itemnum, len );
	return TRUE;
}

BOOL CGetData::GetJgjc(struct MemStkInfo * scode)
{
	if (!scode)   return FALSE; 

	struct l2_jgjc_req * jgjcreqp;
	jgjcreqp = ( struct l2_jgjc_req * ) pTransfer;
	jgjcreqp->req = L2_JGJC_NREQ;  
	jgjcreqp->setcode = scode->setcode;
	memmove ( jgjcreqp->code, scode->Code, SH_CODE_LEN );
	if ( g_pComm->SendIt ( pTransfer,sizeof( struct l2_jgjc_req ) ) != 0 )	return FALSE;
	return TRUE;
}

BOOL CGetData::GetJgjcACK(short &getnums, LPNORMALSTAT MinuteDatap)
{
	short len,itemnum;
	memmove ( &itemnum, g_pReceive, sizeof ( short ) );
	len = sizeof ( short );
	if ( itemnum > getnums ) itemnum = getnums;
	JgjcStruc_rtn ( g_pReceive + len, MinuteDatap, itemnum, len );
	return TRUE;
}

BOOL CGetData::GetCjbs(struct MemStkInfo * scode)
{
	if (!scode)   return FALSE; 

	struct l2_newcjbs_req * cjbsreqp;
	cjbsreqp = ( struct l2_newcjbs_req * ) pTransfer;
	cjbsreqp->req = L2_NEWCJBS_NREQ;  
	cjbsreqp->setcode = scode->setcode;
	memmove ( cjbsreqp->code, scode->Code, SH_CODE_LEN );
	if ( g_pComm->SendIt ( pTransfer,sizeof( struct l2_newcjbs_req ) ) != 0 )	return FALSE;
	return TRUE;
}

BOOL CGetData::GetCjbsACK(short &getnums, LPTICKSTAT MinuteDatap)
{
	short len,itemnum;
	memmove ( &itemnum, g_pReceive, sizeof ( short ) );
	len = sizeof ( short );
	if ( itemnum > getnums ) itemnum = getnums;
	CjbsStruc_rtn ( g_pReceive + len, MinuteDatap, itemnum, len );
	return TRUE;
}

BOOL CGetData::GetCjtj(struct MemStkInfo * scode)
{
	if (!scode)   return FALSE; 

	struct l2_newvolume_req * cjtjreqp;
	cjtjreqp = ( struct l2_newvolume_req * ) pTransfer;
	cjtjreqp->req = L2_NEWVOLUME_NREQ;  
	cjtjreqp->setcode = scode->setcode;
	memmove ( cjtjreqp->code, scode->Code, SH_CODE_LEN );
	if ( g_pComm->SendIt ( pTransfer,sizeof( struct l2_newvolume_req ) ) != 0 )	return FALSE;
	return TRUE;
}

BOOL CGetData::GetCjtjACK(short &getnums, LPTICKSTAT MinuteDatap)
{
	short len,itemnum;
	memmove ( &itemnum, g_pReceive, sizeof ( short ) );
	len = sizeof ( short );
	if ( itemnum > getnums ) itemnum = getnums;
	CjtjStruc_rtn ( g_pReceive + len, MinuteDatap, itemnum, len );
	return TRUE;
}

BOOL CGetData::GetTotalTickStat(struct MemStkInfo * scode)
{
	if (!scode)   return FALSE; 

	struct l2_newtickstat_req * tickstatreqp;
	tickstatreqp = ( struct l2_newtickstat_req * ) pTransfer;
	tickstatreqp->req = L2_NEWTICKSTAT_NREQ;  
	tickstatreqp->setcode = scode->setcode;
	memmove ( tickstatreqp->code, scode->Code, SH_CODE_LEN );
	if ( g_pComm->SendIt ( pTransfer,sizeof( struct l2_newtickstat_req ) ) != 0 )	return FALSE;
	return TRUE;
}

BOOL CGetData::GetTotalTickStatAck(LPTICKSTAT MinuteDatap)
{
	short len = 0;
	TickStatStruc_rtn(g_pReceive+len,MinuteDatap,1,len);
	return TRUE;
}

BOOL CGetData::GetCDP(struct MemStkInfo * scode)
{
	if (!scode)   return FALSE;
	struct cdp_req * cdpreqp;
	cdpreqp = ( struct cdp_req * ) pTransfer;
	cdpreqp->req = CDP_NREQ;
	cdpreqp->setcode = scode->setcode;
	memmove ( cdpreqp->code, scode->Code, SH_CODE_LEN );
	if(g_pComm->SendIt (  pTransfer, sizeof ( struct cdp_req ) ) != 0)
		return FALSE;
	return TRUE;	
}

BOOL CGetData::GetCDPACK(struct MemStkInfo * scode)
{
	if(!scode) return FALSE;
	struct remote_cdpInfo cdpinfo;
    CdpStruc_rtn ( g_pReceive, scode,&cdpinfo);
	scode->cdp = cdpinfo.cdp;
	scode->nh = cdpinfo.nh;
	scode->nl = cdpinfo.nl;
	scode->ah = cdpinfo.ah;
	scode->al = cdpinfo.al;
	scode->dkflag = cdpinfo.dkflag;
	scode->tbp = cdpinfo.tbp;
	scode->stoplost = cdpinfo.stoplost;
	scode->leave = cdpinfo.leave;
	return TRUE;
}

BOOL CGetData::GetPriceVolTable(struct MemStkInfo * scode)
{
	if (!scode)   return FALSE;

	struct fjb_req * fjbreqp;
	fjbreqp = ( struct fjb_req * ) pTransfer;
	fjbreqp->req = FJB_NREQ;
	fjbreqp->setcode = scode->setcode;
	memmove ( fjbreqp->code, scode->Code, SH_CODE_LEN );
	if(g_pComm->SendIt (  pTransfer, sizeof ( struct fjb_req ) ) != 0)
		return FALSE;
	return TRUE;	
}

short CGetData::GetPriceVolTableACK(struct MemStkInfo * scode,struct PriceVolTable * pPriceVolTable,short MaxItem)
{
	if(!scode) return FALSE;
	short itemnum,len;
	memcpy(&itemnum,g_pReceive,sizeof(short));
	len = sizeof(short);
	itemnum = min(MaxItem,itemnum);
	if(itemnum > 0)
	{
		struct CurrStockData tmpHq;
		CurrStockData_rtn(g_pReceive+len,&tmpHq,len);
		PriceVolTable_rtn(scode,g_pReceive+len,pPriceVolTable,itemnum,len);
	}
	return max(0,itemnum);
}

//读历史分价表
BOOL CGetData::GetHisPriceVolTable(struct MemStkInfo * scode,long lDate)
{
	if (!scode)   return FALSE;

	struct fjb_his_req * fjbreqp;
	fjbreqp = ( struct fjb_his_req * ) pTransfer;
	fjbreqp->req = FJB_HIS_NREQ;
	fjbreqp->ldate = lDate;
	fjbreqp->setcode = scode->setcode;
	memmove ( fjbreqp->code, scode->Code, SH_CODE_LEN );
	if(g_pComm->SendIt (  pTransfer, sizeof ( struct fjb_his_req ) ) != 0)
		return FALSE;
	return TRUE;	
}

short CGetData::GetHisPriceVolTableACK(struct MemStkInfo * scode,struct PriceVolTable * pPriceVolTable,short MaxItem,float *pClose)
{
	if(!scode) return FALSE;
	short itemnum,len;
	memcpy(&itemnum,g_pReceive,sizeof(short));
	len = sizeof(short);
	memcpy(pClose,g_pReceive+len,sizeof(float));
	len += sizeof(float);
	itemnum = min(MaxItem,itemnum);
	if(itemnum > 0)
		PriceVolTable_rtn(scode,g_pReceive+len,pPriceVolTable,itemnum,len);
	return max(0,itemnum);
}

//Tick数据请求
BOOL CGetData::GetTickData(struct MemStkInfo * scode,short startxh, short num,BOOL bReverse)
{
	if (!scode)   return FALSE; 

	struct fbb_req * fbbreqp;
	fbbreqp = ( struct fbb_req * ) pTransfer;
	if(bReverse) fbbreqp->req = REVFBB2_NREQ;
	else		 fbbreqp->req = FBB2_NREQ;
	fbbreqp->setcode = scode->setcode;
	memmove ( fbbreqp->code, scode->Code, SH_CODE_LEN );
	fbbreqp->startxh = startxh;
	fbbreqp->num = num;
	if(g_pComm->SendIt (  pTransfer, sizeof ( struct fbb_req ) ) != 0)
		return FALSE;
	return TRUE;
}

void	SaveTickData(BYTE Market,LPCSTR Code,DWORD nIDIndex,DWORD TickNum,TickData*indap)
{
	CString tempFileName;
	tempFileName.Format("%sLData\\cache\\%s.tik",g_WSXHStr,GetSetStr(Market));
	FILE *fpt = fopen( tempFileName, "r+b");
	if(fpt)
	{
		struct MinCacheHead tmpHead;
		long filelen;
		TICKDATA tmpTickData[WANT_TICKS]; //用一临时缓冲区代替，因为num不一定等于WANT_TICKS
		memset(tmpTickData,0,sizeof(TICKDATA)*WANT_TICKS);
		memcpy(tmpTickData,indap,sizeof(TICKDATA)*min(TickNum,WANT_TICKS));
		fseek(fpt,nIDIndex*sizeof(struct MinCacheHead),SEEK_SET);
		fread(&tmpHead,1,sizeof(struct MinCacheHead),fpt);
		if(tmpHead.suc==1)
		{
			fseek(fpt,tmpHead.pos,SEEK_SET);
			fwrite(tmpTickData,1,WANT_TICKS*sizeof(TICKDATA),fpt);
		}
		else
		{
			fseek(fpt,0,SEEK_END);
			filelen = on_filelength(fpt);
			fwrite(tmpTickData,1,WANT_TICKS*sizeof(TICKDATA),fpt);
			fseek(fpt, nIDIndex*sizeof(struct MinCacheHead),SEEK_SET);
			tmpHead.suc = 1;
			tmpHead.pos = filelen;
			fwrite(&tmpHead,1,sizeof(struct MinCacheHead),fpt);
		}
		fclose(fpt);
	}
}

short CGetData::GetTickDataACK(struct MemStkInfo *scode,struct TickData *indap,short num,BOOL bFirstWant)
{
	if(!scode) return 0;
	short len,getnum=0;
	char precise;
	BOOL bZs=testzs(scode);
	memset ( indap, 0, num * sizeof ( struct TickData ) );
	memmove(&getnum,g_pReceive,sizeof(short));
	getnum=max(0,getnum);
	getnum=min(num,getnum);
	len = sizeof(short);
	precise = GetPrecise( scode->Code,scode->setcode );
	TickData_rtn ( g_pReceive+len , indap ,precise,getnum,len,TRUE,bZs);
	if(bFirstWant&&getnum) //如果是第一次请求,则缓存
		SaveTickData(scode->setcode,scode->Code,scode->Index,getnum,indap);
	return getnum;
}

//历史Tick数据
BOOL CGetData::GetHisTickData(struct MemStkInfo * scode,long lDate,short startxh, short num)
{
	if (!scode)   return FALSE; 

	struct fbb_his_req * fbbreqp;
	fbbreqp = ( struct fbb_his_req * ) pTransfer;
	fbbreqp->req = MinuteTick_SpecialData(scode)?REVFBB2_HIS_NREQ:REVFBB_HIS_NREQ;
	fbbreqp->ldate = lDate;
	fbbreqp->setcode = scode->setcode;
	memmove ( fbbreqp->code, scode->Code, SH_CODE_LEN );
	fbbreqp->startxh = startxh;
	fbbreqp->num = num;
	if(g_pComm->SendIt (  pTransfer, sizeof ( struct fbb_his_req ) ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetHisTickDataACK(struct MemStkInfo *scode,struct TickData *indap,short num,float *pClose)
{
	if(!scode) return 0;
	short len,getnum=0;
	char precise;
	BOOL bZs=testzs(scode);
	memset ( indap, 0, num * sizeof ( struct TickData ) );
	memmove(&getnum,g_pReceive,sizeof(short));
	getnum=max(0,getnum);
	getnum=min(num,getnum);
	len = sizeof(short);
	memcpy(pClose,g_pReceive+len,sizeof(float));
	len += sizeof(float);
	precise = GetPrecise( scode->Code,scode->setcode );
	TickData_rtn ( g_pReceive+len , indap ,precise,getnum,len,MinuteTick_SpecialData(scode),bZs);
	return getnum;
}


//买卖盘数据(精简)
BOOL CGetData::GetBsp1Data(struct MemStkInfo * scode,short startxh, short num)
{
	if (!scode)   return FALSE; 

	struct bsp1_req * bspreqp;
	bspreqp = ( struct bsp1_req * ) pTransfer;
	bspreqp->req = REVBSP1_NREQ;
	bspreqp->setcode = scode->setcode;
	memmove ( bspreqp->code, scode->Code, SH_CODE_LEN );
	bspreqp->startxh = startxh;
	bspreqp->num = num;
	if(g_pComm->SendIt (  pTransfer, sizeof ( struct bsp1_req ) ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetBsp1DataACK(struct MemStkInfo *scode,TICK_MMP *indap,short num,BOOL bFirstWant)
{
	if(!scode) return 0;

	short len;
	memset ( indap, 0, num * sizeof(TICK_MMP) );
	memmove(&num,g_pReceive,sizeof(short));
	len = sizeof(short);
	Bsp1Data_rtn ( g_pReceive+len , indap ,scode,num,len);
	return num;
}

//历史买卖盘数据(精简)
BOOL CGetData::GetHisBsp1Data(struct MemStkInfo * scode,long lDate,short startxh, short num)
{
	if (!scode)   return FALSE; 

	struct bsp1_his_req * bspreqp;
	bspreqp = ( struct bsp1_his_req * ) pTransfer;
	bspreqp->req = REVBSP1_HIS_NREQ;
	bspreqp->ldate =  lDate;
	bspreqp->setcode = scode->setcode;
	memmove ( bspreqp->code, scode->Code, SH_CODE_LEN );
	bspreqp->startxh = startxh;
	bspreqp->num = num;
	if(g_pComm->SendIt (  pTransfer, sizeof ( struct bsp1_his_req ) ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetHisBsp1DataACK(struct MemStkInfo *scode,TICK_MMP *indap,short num,float *pClose)
{
	if(!scode) return 0;

	short len;
	memset ( indap, 0, num * sizeof(TICK_MMP) );
	memmove(&num,g_pReceive,sizeof(short));
	len = sizeof(short);
	memmove(pClose,g_pReceive+len,sizeof(short));
	len += sizeof(float);
	Bsp1Data_rtn ( g_pReceive+len , indap ,scode,num,len);
	return num;
}

//分析数据请求,-1:发送失败 0:发送成功 >0:缓存得到的实际个数
short CGetData::GetAnalyData(struct MemStkInfo * scode,short offset,short ItemNum,short Type,short mulnum,struct AnalyData * AnalyDatap)
{
	if (g_bDebugMode && g_bChangeDebugCode) //更换代码后的调试模式不请求日线数据
		return 0;
	if (!scode)   
		return 0;
	//非二代转码机不支持直接取日线文件协议
	BOOL bLCPt = IsLCPt(scode);	
	if(bLCPt && (!g_bG2DT || Type!=PER_DAY))
		return 0;
	if(AnalyDatap && Type==PER_DAY)
	{	
		long VipDocLastDate=GetVipDocLastDate();
		if(g_bAllLocalData&&(VipDocLastDate==g_lOpenRq||GetForwardDate(VipDocLastDate)==g_lOpenRq))
		{
			short nDiskNum = g_pStockIO->ReadAnalyDataOffset(scode,AnalyDatap,ItemNum,Type,offset,GetMulNum(Type));
			if(nDiskNum>100)
			{
				//如果读日线缓存成功，则将最后行情更新
				struct CurrStockData tmpHq;
				g_pStockIO->ReadCurrStockData(scode,&tmpHq);
				if(AnalyDatap[nDiskNum-1].Time.Date==g_lOpenRq)
				{
					float ftmp = tmpHq.Now;
					if(ftmp < UPDATE_COMPPREC) 
					{
						ftmp = tmpHq.Close;
						tmpHq.Now = tmpHq.Close;
					}
					tmpHq.Open   = tmpHq.Open<UPDATE_COMPPREC?ftmp:tmpHq.Open;
					tmpHq.Max   = tmpHq.Max<UPDATE_COMPPREC?ftmp:tmpHq.Max;
					tmpHq.Min   = tmpHq.Min<UPDATE_COMPPREC?ftmp:tmpHq.Min;
					//
					AnalyDatap[nDiskNum-1].Open   = tmpHq.Open;
					AnalyDatap[nDiskNum-1].High   = tmpHq.Max ;
					AnalyDatap[nDiskNum-1].Low    = tmpHq.Min ;
					AnalyDatap[nDiskNum-1].Close  = tmpHq.Now ;
					if(!testzs(scode) && need_justcjl(scode->setcode,scode->Code))
						AnalyDatap[nDiskNum-1].Volume = scode->Unit*tmpHq.Volume;
					else
						AnalyDatap[nDiskNum-1].Volume = tmpHq.Volume;
					AnalyDatap[nDiskNum-1].Amount = tmpHq.Amount;
					if(testzs(scode))
					{
						AnalyDatap[nDiskNum-1].zd.up   = tmpHq.Buyv[0];
						AnalyDatap[nDiskNum-1].zd.down = tmpHq.Sellv[0];
					}
					else
						AnalyDatap[nDiskNum-1].YClose = 0.0;
				}
				
				return nDiskNum;
			}
			else memset(AnalyDatap, 0, ItemNum*sizeof(AnalyData));
		}
	}

	if(!g_bSupportETF && (Type==PER_SEASON || Type==PER_YEAR))
		return 0;
	if(bLCPt)
	{
		LPLCPTSET pPtSet = NULL;
		if(g_pLcIdxMng)
			pPtSet = g_pLcIdxMng->GetLcPTSet(scode->Code);
		if(!pPtSet) return 0;
		struct codefx_req * fxreqp;
		fxreqp = ( struct codefx_req * ) pTransfer;
		fxreqp->req      = CODEFX_NREQ;
		fxreqp->offset   = offset;
		fxreqp->itemnum  = ItemNum;
		fxreqp->setcode  = pPtSet->BaseSetcode;
		memmove ( fxreqp->code, pPtSet->BaseCode, SH_CODE_LEN );
		if(g_pComm->SendIt( pTransfer,sizeof ( struct codefx_req ) ) != 0)
			return -1;
	}
	else
	{
		struct combfx2_req * fxreqp;
		fxreqp = ( struct combfx2_req * ) pTransfer;
		fxreqp->req      = AnalyData_SpecialData(scode)?COMBFX4_NREQ:COMBFX3_NREQ;
		fxreqp->offset   = offset;
		fxreqp->linetype = Type;
		fxreqp->mulnum   = mulnum;
		fxreqp->itemnum  = ItemNum;
		fxreqp->setcode  = scode->setcode;
		memmove ( fxreqp->code, scode->Code, SH_CODE_LEN );
		if(g_pComm->SendIt( pTransfer,sizeof ( struct combfx2_req ) ) != 0)
			return -1;
	}
	return 0;
}

short CGetData::GetAnalyDataACK( struct MemStkInfo * scode,struct AnalyData * AnalyDatap,short Type,short oldnum)
{
	if(!scode)	return 0; 

	short ItemNum;
	memmove( &ItemNum,g_pReceive,sizeof( short ) );
	if(ItemNum <=0 ) return 0;
	short len = sizeof( short );
	if(AnalyData_SpecialData(scode))
		AnalyData_rtn4 (scode,g_pReceive+len,AnalyDatap,ItemNum,len);
	else
		AnalyData_rtn3 (scode,g_pReceive+len,AnalyDatap,ItemNum,len);
	if( oldnum>=0 && (Type==PER_DAY || Type==PER_WEEK || Type==PER_MONTH) )
		WriteCacheData(scode,scode->setcode,AnalyDatap,ItemNum,Type);
	return ItemNum;
}

BOOL CGetData::GetZHSort(int GpType,int Num)
{
	if(g_bSupportDownMin5)
	{
		struct zhsort_new_req *zhsortreqp;
		zhsortreqp = (struct zhsort_new_req *)pTransfer;
		memset(zhsortreqp,0,sizeof(struct zhsort_new_req));
		zhsortreqp->req = ZHSORT_NEW_NREQ;
		zhsortreqp->domain = GpType;
		zhsortreqp->zfmin = g_nZFFzNum;
		zhsortreqp->num = Num;
		if(g_bSupportDDE)
			zhsortreqp->zhtype = 1;
		if(g_pComm->SendIt( pTransfer, sizeof ( struct zhsort_new_req )) != 0)
			return FALSE;
		return TRUE;
	}
	struct zhsort_req *zhsortreqp;
	zhsortreqp = (struct zhsort_req *)pTransfer;
	zhsortreqp->req = ZHSORT_NREQ;
	zhsortreqp->domain = GpType;
	zhsortreqp->num = Num;
	if(g_pComm->SendIt( pTransfer, sizeof ( struct zhsort_req )) != 0)
		return FALSE;
	return TRUE;
}

BOOL CGetData::GetZHSortACK(ZHRESULT Buf[9])
{
	MemStkInfo *pStkInfo=NULL;
	CurrStockData tmpHQ;
	struct zhsort_ans * zhsortansp;
	zhsortansp = ( struct zhsort_ans * )g_pReceive;
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
			if(!pStkInfo || pStkInfo->Close<COMPPREC)
				continue;
			memcpy(&(Buf[i].Now[which]),zhsortansp->buf+pos*tmpSize+1+SH_CODE_LEN,sizeof(float));
			memcpy(&(Buf[i].Other[which]),zhsortansp->buf+pos*tmpSize+1+SH_CODE_LEN+sizeof(float),sizeof(float));
			which++;
		}
		Buf[i].Num = which;
	}
	return TRUE;
}

BOOL CGetData::GetFxFile(MemStkInfo *pStkInfo,long startdate,long enddate,short type)
{
	struct fxfile_req *fxfilereqp;
	fxfilereqp = (struct fxfile_req *)pTransfer;
	fxfilereqp->req = FXFILE_NREQ;
	fxfilereqp->setcode = pStkInfo->setcode;
	memcpy(fxfilereqp->code,pStkInfo->Code,SH_CODE_LEN);
	fxfilereqp->startdate = startdate;
	fxfilereqp->enddate = enddate;
	fxfilereqp->type = type;
	if(g_pComm->SendIt( pTransfer, sizeof ( struct fxfile_req )) != 0)
		return FALSE;
	return TRUE;	
}

BOOL CGetData::GetFxFileACK(MemStkInfo *pStkInfo)
{
	if(!pStkInfo) return TRUE;

	struct fxfile_ans *fxfileansp;
	fxfileansp = (struct fxfile_ans *)g_pReceive;
    long buflen = fxfileansp->filelen;
	int	nData=0;
	if(fxfileansp->type==PER_DAY)			//如果是日线分析数据
	{
		nData = buflen/sizeof(ANALYDATA);
		if(nData>=1)
			return AddToVipdocDayData(pStkInfo,(struct AnalyData *)fxfileansp->buf,nData,fxfileansp->type);
	}
	else if(fxfileansp->type==PER_MIN5)		//如果是5分钟分析数据
	{
		nData = buflen/sizeof(ANALYDATA);
		if(nData>=1)
			return AddToVipdocFxMinData(pStkInfo,(struct AnalyData *)fxfileansp->buf,nData,fxfileansp->type);
	}
	return TRUE;
}

BOOL CGetData::GetDateDayFile(long lDate,long lPos,long lReqLen,short type)
{
	struct dayfile_req *dayfilereqp;
	dayfilereqp = (struct dayfile_req *)pTransfer;
	memset(dayfilereqp,0,sizeof(struct dayfile_req));
	dayfilereqp->req = EXDAYFILE_NREQ;
	dayfilereqp->ldate = lDate;
	dayfilereqp->type = type;
	dayfilereqp->lpos = lPos;
	dayfilereqp->reqlen = lReqLen;
	if(g_pComm->SendIt( pTransfer, sizeof ( struct dayfile_req )) != 0)
		return FALSE;
	return TRUE;
}

BOOL CGetData::GetDateDayFileACK(short &setcode,long &filelen,long &filercvlen,char *prcvbuf,int nMaxLen)
{
	struct dayfile_ans *dayfileansp;
	dayfileansp = (struct dayfile_ans *)g_pReceive;
    filelen		= dayfileansp->filelen;
	filercvlen	= dayfileansp->sendlen;
	int	nData=0;
	if(dayfileansp->type==EXDAYSZ_FILE)
	{
		if(prcvbuf==NULL || filercvlen<0 || filercvlen>nMaxLen) return FALSE;
		if(filercvlen>0)
			memcpy(prcvbuf,dayfileansp->buf,filercvlen*sizeof(char));
		setcode = SZ;
	}
	else if(dayfileansp->type==EXDAYSH_FILE)
	{
		if(prcvbuf==NULL || filercvlen<0 || filercvlen>nMaxLen) return FALSE;
		if(filercvlen>0)
			memcpy(prcvbuf,dayfileansp->buf,filercvlen*sizeof(char));
		setcode = SH;
	}
	else return FALSE;
	return TRUE;
}

//用于办公网协议
BOOL CGetData::GetSimHq(char * codehead, short num)								//多股简单行情
{
	struct simhq_req * hqreqp;
	hqreqp = ( struct simhq_req * ) pTransfer;
	hqreqp->req = SIMHQ_NREQ;
	hqreqp->num = num;
	memmove ( hqreqp->buf, codehead, num * (1+SH_CODE_LEN) );
	if(g_pComm->SendIt( pTransfer,sizeof(struct simhq_req)-1+num*(1+SH_CODE_LEN) ) != 0)
		return FALSE;
	return TRUE;
}

void CGetData::GetSimHqACK()
{
	short i,len,num;
	char Code[SH_CODE_LEN+1];
	char setcode;
	struct simhq_Info simhq;
	struct CurrStockData tmphq;
	MemStkInfo *tmpscode;

	memmove ( &num, g_pReceive, sizeof ( short ) );
	len = sizeof ( short );
	for ( i = 0; i < num; i++ )
	{
		simhq_rtn(g_pReceive + len,setcode,Code,&simhq,len);
		tmpscode = g_pStockIO->Get_MemStkInfo(Code,setcode);
		if(!tmpscode) continue;
		//得到行情后，重新DataIO中的股票行情
		g_pStockIO->ReadCurrStockData(tmpscode,&tmphq);
		tmphq.Now = simhq.Now;
		tmphq.Open = simhq.Open;
		tmphq.Max = simhq.Max;
		tmphq.Min = simhq.Min;
		tmphq.Volume = simhq.Volume;
		tmphq.Amount = simhq.Amount;
		g_pStockIO->SetCurrStockData(tmpscode->setcode,tmpscode->Index,(char*)&tmphq);
	}
}

//基本面和消息面等其它请求
BOOL CGetData::GetFileLen ( char * filename )
{
	struct filelen_req * filelenreqp;

	filelenreqp = ( struct filelen_req * ) pTransfer;
	filelenreqp->req = FILELEN_REQ;
	strcpy ( filelenreqp->fname, filename );
	if(g_pComm->SendIt( pTransfer,sizeof ( struct filelen_req )) != 0)
		return FALSE;
	return TRUE;
}

BOOL CGetData::GetXxmFileLen ( short xxmxh, short xh )
{
	struct xxm_len_req * xxmlenreqp;
	xxmlenreqp  = ( struct xxm_len_req * ) pTransfer;
	xxmlenreqp->req = XXM_LEN_REQ;
	xxmlenreqp->xxmxh = xxmxh;
	xxmlenreqp->xh = xh;
	if(g_pComm->SendIt(pTransfer,sizeof ( xxm_len_req )) != 0)
		return FALSE;
	return TRUE;
}

BOOL CGetData::GetJbmFileLen ( const char * code,short setcode)
{
	struct jbm_len_req * jbmlenreqp;
	jbmlenreqp  = ( struct jbm_len_req * ) pTransfer;
	jbmlenreqp->req = JBM_LEN_REQ;
	jbmlenreqp->setcode = setcode;
	memmove ( jbmlenreqp->code, code, SH_CODE_LEN );
	if(g_pComm->SendIt(pTransfer,sizeof ( jbm_len_req )) != 0)
		return FALSE;
	return TRUE;
}

long CGetData::GetFileLenACK(char *usemdstr,char *mdstr)
{
	struct filelen_ans * filelenansp;
	filelenansp = ( struct filelen_ans * ) g_pReceive;
	if(usemdstr)
		*usemdstr = filelenansp->usemdstr;
	if(mdstr)
	{
		mdstr[0]=0;
		if(usemdstr && *usemdstr==1)
		{
			strncpy(mdstr,filelenansp->mdstr,32);
			mdstr[32]=0;
		}
	}
	return filelenansp->filelen;
}

BOOL CGetData::GetXxmItem ( short xxmxh, short startxh)
{
	struct wz_req * wzreqp;
	wzreqp = ( struct wz_req * ) pTransfer;
	wzreqp->req     = XXMITEM_REQ;
	wzreqp->htype   = xxmxh;
	wzreqp->startxh = startxh;
	if(g_pComm->SendIt( pTransfer,sizeof ( struct wz_req ) ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetXxmItemACK(char * buffer)
{
	struct wz_ans * wzansp;
	short           reclen;

	wzansp = ( struct wz_ans * )g_pReceive;
	reclen = g_AnsHeader.RawLen;
	memmove( buffer, wzansp->buf, reclen - sizeof ( wzansp->num ) );
	buffer[reclen-sizeof(wzansp->num)] = 0;
	return wzansp->num;
}

BOOL CGetData::GetText ( short xxmxh, short xh, DWORD pos)
{
	struct xxmwz_long_req * wzreqp;
	wzreqp = ( struct xxmwz_long_req * ) pTransfer;
	wzreqp->req   = XXMWZ_LONG_REQ;
	wzreqp->xxmxh = xxmxh;
	wzreqp->xh    = xh;
	wzreqp->pos   = pos;
	if(g_pComm->SendIt( pTransfer,sizeof ( struct xxmwz_long_req ) ) != 0)
		return FALSE;
	return TRUE;
}

BOOL CGetData::GetPathFile(const char *filepath, int nPos)
{
	struct pathfile_req * xxgreqp;
	xxgreqp = ( struct pathfile_req * ) pTransfer;
	xxgreqp->req = PATHFILE_REQ;
	xxgreqp->pos = nPos;
	xxgreqp->len = 30000;
	strncpy(xxgreqp->pathfile,filepath, 99);
	if(g_pComm->SendIt( pTransfer,sizeof ( struct pathfile_req ) ) != 0)
		return FALSE;
	return TRUE;
}

long CGetData::GetPathFileACK(char * buffer)
{
	struct pathfile_ans * wzansp;
	long           reclen;

	wzansp = ( struct pathfile_ans * )g_pReceive;
	reclen = g_AnsHeader.RawLen;
	memmove( buffer, wzansp->buf, reclen - sizeof ( wzansp->len ) );
	buffer[reclen-sizeof(wzansp->len)] = 0;
	return wzansp->len;
}

BOOL CGetData::GetJbmText (const char * code, short setcode,DWORD pos,BOOL bBigTxt)
{
	struct jbmwz_long_req * wzreqp;
	wzreqp = ( struct jbmwz_long_req * ) pTransfer;
	wzreqp->req = bBigTxt?JBMWZ_LONG2_REQ:JBMWZ_LONG_REQ;
	wzreqp->setcode = setcode;
	wzreqp->pos = pos;
	memmove ( wzreqp->code, code, SH_CODE_LEN );
	if(g_pComm->SendIt( pTransfer,sizeof ( struct jbmwz_long_req ) ) != 0)
		return FALSE;
	return TRUE;
}

BOOL CGetData::GetAllJbmText (const char * code, short setcode)
{
	struct jbmwz_all_req * wzreqp;
	wzreqp = ( struct jbmwz_all_req * ) pTransfer;
	wzreqp->req = JBMWZ_ALL_REQ;
	wzreqp->setcode = setcode;
	memmove ( wzreqp->code, code, SH_CODE_LEN );
	if(g_pComm->SendIt( pTransfer,sizeof ( struct jbmwz_all_req ) ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetTextACK(char * buffer )
{
	struct wz_ans         * wzansp;
	short                   reclen;
	wzansp = ( struct wz_ans * ) g_pReceive;
	reclen = g_AnsHeader.RawLen;
	memmove( buffer, wzansp->buf, reclen - sizeof ( wzansp->num ) );
	buffer[reclen-sizeof(wzansp->num)] = 0;
	_FL(buffer);
	return wzansp->num;
}

BOOL CGetData::GetJbmCfg (const char * code, short setcode)
{
	struct f10cfg_req * reqp;
	reqp = ( struct f10cfg_req * ) pTransfer;
	reqp->req = F10CFG_REQ;
	reqp->setcode = setcode;
	memmove ( reqp->code, code, SH_CODE_LEN );
	if(g_pComm->SendIt( pTransfer,sizeof ( struct f10cfg_req ) ) != 0)
		return FALSE;
	return TRUE;
}

int CGetData::GetJbmCfgACK()
{
	struct f10cfg_ans  * ansp;
	ansp = ( struct f10cfg_ans * ) g_pReceive;
	g_F10CaptionNum = ansp->titlenum;
	g_F10CaptionNum = max(0,g_F10CaptionNum);
	g_F10CaptionNum = min(MAX_F10CAPTION,g_F10CaptionNum);
	memcpy(g_F10Caption,ansp->list,g_F10CaptionNum*sizeof(struct f10cfg_title));
	for(int i=0;i < g_F10CaptionNum;i++)
		_FL(g_F10Caption[i].title);
	return g_F10CaptionNum;
}

BOOL CGetData::GetNewJbmCfg (const char * code, short setcode,short whichjbm)
{
	struct new_f10cfg_req * reqp;
	reqp = ( struct new_f10cfg_req * ) pTransfer;
	reqp->req = NEW_F10CFG_REQ;
	reqp->setcode = setcode;
	memmove ( reqp->code, code, SH_CODE_LEN );
	reqp->whichjbm = whichjbm;
	reqp->reserved = 0;
	if(g_pComm->SendIt( pTransfer,sizeof ( struct new_f10cfg_req ) ) != 0)
		return FALSE;
	return TRUE;
}

int CGetData::GetNewJbmCfgACK ()
{
	struct new_f10cfg_ans  * ansp;
	ansp = ( struct new_f10cfg_ans * ) g_pReceive;
	g_F10CaptionNum = ansp->titlenum;
	g_F10CaptionNum = max(0,g_F10CaptionNum);
	g_F10CaptionNum = min(MAX_F10CAPTION,g_F10CaptionNum);
	memcpy(g_NewF10Caption,ansp->list,g_F10CaptionNum*sizeof(struct new_f10cfg_title));
	for(int i=0;i < g_F10CaptionNum;i++)
		_FL(g_NewF10Caption[i].sTitle);
	return g_F10CaptionNum;
}

BOOL CGetData::GetNewJbmText (const char * code, short setcode,short which,char *sFilePath,long nOffset,long nLength,short whichjbm)
{
	struct new_f10txt_req * wzreqp;
	wzreqp = ( struct new_f10txt_req * ) pTransfer;
	wzreqp->req = NEW_F10TXT_REQ;
	wzreqp->setcode = setcode;
	memmove ( wzreqp->code, code, SH_CODE_LEN );
	wzreqp->which = which;
	strncpy(wzreqp->sFilePath,sFilePath,78);
	wzreqp->nOffset = nOffset;
	wzreqp->nLength = nLength;
	wzreqp->whichjbm = whichjbm;
	wzreqp->reserved = 0;
	if(g_pComm->SendIt( pTransfer,sizeof ( struct new_f10txt_req ) ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetNewJbmTextACK(char * buffer )
{
	struct new_f10txt_ans    * wzansp;
	wzansp = ( struct new_f10txt_ans * ) g_pReceive;
	if(wzansp->num<=0 || wzansp->num>30*1024) 
		return 0;
	memcpy( buffer, wzansp->buf, wzansp->num);
	buffer[wzansp->num] = 0;
	_FL(buffer);
	return wzansp->num;
}

BOOL  CGetData::GetIwTitle(char search_type,long from_order,short wantnum,short	setcode,char *code,long	hy_id,long type_id)
{
	struct iw_title_req *preq = (struct iw_title_req *)pTransfer;
	memset(preq,0,sizeof(struct iw_title_req));
	
	preq->req = GET_IW_TITLE;
	preq->search_type = search_type;
	preq->from_order = from_order;
	preq->wantnum = wantnum;
	preq->setcode = setcode;
	strncpy(preq->code,code,10);
	preq->hy_id = hy_id;
	preq->type_id = type_id;
	if(g_pComm->SendIt(pTransfer, sizeof(struct iw_title_req)) != 0)
		return FALSE;
	return TRUE;
}

int IwTitleSort( const void * a, const void * b )
{
	struct iw_client_title *p1,*p2;
	p1 = (struct iw_client_title*) a;
	p2 = (struct iw_client_title*) b;
	if(p1->time_ymd > p2->time_ymd) return -1;
	if(p1->time_ymd < p2->time_ymd) return 1;
	if(p1->time_hms > p2->time_hms) return -1;
	if(p1->time_hms < p2->time_hms) return 1;
	return 0;
}

short CGetData::GetIwTitleACK(struct iw_client_title *pList,long wantnum,long &totalnum)
{
	struct iw_title_ans *pans = (struct iw_title_ans *)g_pReceive;
	totalnum = pans->totalnum;
	long tmpNum = min(pans->titlenum,wantnum);
	if(tmpNum > 0)
	{
		memcpy(pList,pans->list,tmpNum*sizeof(struct iw_client_title));
		qsort((void *)pList, tmpNum, sizeof(struct iw_client_title), IwTitleSort);
	}
	return tmpNum;
}


BOOL CGetData::GetNewGPInf(short setcode,short startxh)
{
	struct cod_req           * codreqp;
	codreqp = ( struct cod_req * )     pTransfer;
	codreqp->req = CODE3_NREQ;
	codreqp->setcode = setcode;
	codreqp->startxh = startxh;
	if ( g_pComm->SendIt( pTransfer,sizeof( struct cod_req ) ) !=0 )
		return FALSE;
	return TRUE;
}

short CGetData::GetNewGPInfACK(struct remote_TmpStkInfo3* pTmp)
{
	struct cod_new_ans3 * codansp;
	codansp = ( struct cod_new_ans3 * ) g_pReceive;
	memmove ( pTmp, codansp->headp, codansp->stknum * sizeof ( struct remote_TmpStkInfo3 ) );
	return codansp->stknum;
}

BOOL CGetData::GetBaseInf(short stknum,char *buf)
{
	struct autobase_req	* basereqp;
	basereqp = ( struct autobase_req * )     pTransfer;
	basereqp->req = AUTOBASE2_EREQ;
	basereqp->stknum = stknum;
	memcpy(basereqp->buf,buf,stknum*(1+SH_CODE_LEN));
	short tmplen = 2*sizeof(short)+stknum*(1+SH_CODE_LEN);
	if ( g_pComm->SendIt( pTransfer,tmplen ) !=0 )
		return FALSE;
	return TRUE;
}

short CGetData::GetBaseInfACK(short szstknum,short shstknum,struct MemStkInfo *pszMem,struct MemStkInfo *pshMem)
{
	struct autobase2_ans	* baseansp;
	baseansp = ( struct autobase2_ans * ) g_pReceive;
	struct remote_BaseInfo2 *pbase;
	int i,j;
	char gpcode[SH_CODE_LEN+1];
	for(i=0;i < baseansp->stknum;i++)
	{
		pbase = baseansp->basep+i;
		memcpy(gpcode,pbase->Code,SH_CODE_LEN);
		gpcode[SH_CODE_LEN] = 0;
		if(pbase->setcode==SZ)
		{
			for(j=0;j < szstknum;j++)
				if(strcmp(pszMem[j].Code,gpcode)==0) break;
			if(j < szstknum)
			{
				pszMem[j].ActiveCapital = pbase->ActiveCapital;
				memcpy(&pszMem[j].J_addr,&pbase->J_addr,sizeof(struct remote_BaseInfo2)-sizeof(float)-SH_CODE_LEN-1);
			}
		}
		else
		{
			for(j=0;j < shstknum;j++)
				if(strcmp(pshMem[j].Code,gpcode)==0)	break;
			if(j < shstknum)
			{
				pshMem[j].ActiveCapital = pbase->ActiveCapital;
				memcpy(&pshMem[j].J_addr,&pbase->J_addr,sizeof(struct remote_BaseInfo2)-sizeof(float)-SH_CODE_LEN-1);
			}
		}
	}			
	return baseansp->stknum;
}

BOOL CGetData::GetGbbqInf(short stknum,char *buf)
{
	struct autogbbq_req	* gbbqreqp;
	gbbqreqp = ( struct autogbbq_req * )     pTransfer;
	gbbqreqp->req = AUTOGBBQ_EREQ;
	gbbqreqp->stknum = stknum;
	memcpy(gbbqreqp->buf,buf,stknum*(1+SH_CODE_LEN));
	short tmplen = 2*sizeof(short)+stknum*(1+SH_CODE_LEN);
	if ( g_pComm->SendIt( pTransfer,tmplen ) !=0 )
		return FALSE;
	return TRUE;
}

CWDATA *GetCwclPos(char *GpCode,short setcode,unsigned long TotalNum,CWDATA *CwData,short &DataNum)
{
	CWDATA *pStart = NULL;
	long start = 0;
	DataNum = 0;
	for(int i=0; i<TotalNum; i++)
	{
		if(CompareCwcl(CwData+i,GpCode,setcode))
		{
			if(!pStart)
			{
				pStart = CwData+i;
				start = i;
			}
		}
		else if(pStart)
		{
			DataNum = i-start;
			break;
		}
	}
	if(pStart && i >= TotalNum)
		DataNum = i-start;
	return pStart;
}

BOOL CGetData::GetGbbqInfACK(short &retnum,short szstknum,short shstknum,struct MemStkInfo *pSZMem,struct MemStkInfo *pSHMem) //只针对AUTOGBBQ2_EREQ
{
	retnum = 0;
	//先打开GBBQ文件
	int i,k;
	unsigned long Record_Num = 0;
	CBlowfish  *p_fish = new CBlowfish;
	char *Password = "770303770222";
	p_fish->InitializeBlowfish((BYTE *)Password,12);
	int cwbuflen = GP_MAXGBBQNUM;	//多申请GP_MAXGBBQNUM个数据空间
	CWDATA *All_CwData = NULL;
	FILE *fp_in = _fsopen(g_WSXHStr+"LData\\cache\\gbbq","rb",_SH_DENYNO);
	if (fp_in)
	{
		long filelen = on_filelength(fp_in);
		fread(&Record_Num,sizeof(unsigned long),1,fp_in);
		if(filelen-sizeof(unsigned long) != Record_Num*sizeof(CWDATA))
			Record_Num=0;
		cwbuflen += Record_Num;
		All_CwData = new CWDATA[cwbuflen];
		if (All_CwData == NULL)	{ delete p_fish;fclose(fp_in);return FALSE; }

		fread(All_CwData,sizeof(CWDATA)*Record_Num ,1, fp_in);
		for (i = 0;i<Record_Num;i++)
			p_fish->Decrypt((BYTE *)(&All_CwData[i]),24);

		fclose(fp_in);
	}
	else
	{
		All_CwData = new CWDATA[cwbuflen];
		if (All_CwData == NULL)	
		{
			delete p_fish;
			return FALSE;
		}
	}
	//处理应答包
	struct autogbbq_ans	* gbbqansp;
	gbbqansp = ( struct autogbbq_ans * ) g_pReceive;
	char *pChar = gbbqansp->buf;
	CWDATA *pTmpPos=NULL;
	char gpcode[SH_CODE_LEN+1];
	char setcode;
	short freshnum = 0,oldfreshnum = 0;
	for(i=0;i < gbbqansp->stknum;i++)
	{
		memcpy(&setcode,pChar,1); //得到setcode
		memcpy(gpcode,pChar+1,SH_CODE_LEN);
		gpcode[SH_CODE_LEN] = 0;
		pChar+=SH_CODE_LEN+1;

		memcpy(&freshnum,pChar,sizeof(short));
		pChar+=sizeof(short);
		pTmpPos = GetCwclPos(gpcode,setcode,Record_Num,All_CwData,oldfreshnum);
		if(!pTmpPos) pTmpPos = All_CwData+Record_Num;
		memmove(pTmpPos,pTmpPos+oldfreshnum,sizeof(CWDATA)*(Record_Num-(pTmpPos-All_CwData)));
		memmove(pTmpPos+freshnum,pTmpPos,sizeof(CWDATA)*(Record_Num-(pTmpPos-All_CwData)));
		memcpy(pTmpPos,pChar,freshnum*sizeof(CWDATA));
		pChar+=freshnum*sizeof(CWDATA);
		Record_Num += freshnum-oldfreshnum;
		if(Record_Num > cwbuflen-GP_MAXGBBQNUM) //假定一支股票不可能有ADDEDGBBQ_NUM次权息变动
		{
			CWDATA *tmpBuf = new CWDATA[cwbuflen+GP_MAXGBBQNUM];
			memcpy(tmpBuf,All_CwData,cwbuflen*sizeof(CWDATA));
			cwbuflen+=GP_MAXGBBQNUM;
			TDEL(All_CwData);
			All_CwData = tmpBuf;
		}
		if(setcode==SZ)
		{
			for(k=0;k<szstknum;k++)
			{
				if(memicmp(pSZMem[k].Code,gpcode,SH_CODE_LEN)==0)
				{
					pSZMem[k].GbbqFreshCount = -pSZMem[k].GbbqFreshCount;
					break;
				}
			}
		}
		else
		{
			for(k=0;k<shstknum;k++)
			{
				if(memicmp(pSHMem[k].Code,gpcode,SH_CODE_LEN)==0)
				{
					pSHMem[k].GbbqFreshCount = -pSHMem[k].GbbqFreshCount;
					break;
				}
			}
		}
	}
	//加密并存盘
	FILE *fp_out = _fsopen(g_WSXHStr+"LData\\cache\\gbbq","wb",_SH_DENYNO);
	if (fp_out == NULL)  
	{
		TDEL(p_fish);
		TDEL(All_CwData);
		return FALSE;
	}
	fwrite(&Record_Num,sizeof(unsigned long),1,fp_out);

	for (i = 0;i<Record_Num;i++)
		p_fish->Encrypt((BYTE *)(&All_CwData[i]),24);
	fwrite(All_CwData,Record_Num*sizeof(CWDATA),1,fp_out);

	TDEL(p_fish);	
	fclose(fp_out);
	TDEL(All_CwData);

	retnum = gbbqansp->stknum;
	return TRUE;
}

BOOL CGetData::GetTickNumL2(struct MemStkInfo * scode,DWORD localSeq)
{
	if(!CanLevel2()) return FALSE;	
	if (!scode)	return FALSE; 

	struct l2ticnum_req * reqp = ( struct l2ticnum_req * ) pTransfer;
	reqp->req = L2_TICNUM_NREQ;
	reqp->setcode = scode->setcode;
	memmove ( reqp->code, scode->Code, SH_CODE_LEN );
	reqp->localSeq = localSeq;
	reqp->reserved = 0;
	if(g_pComm->SendIt (  pTransfer, sizeof ( struct l2ticnum_req ) ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetTickNumL2ACK(struct MemStkInfo *scode,DWORD &MaxTicSeq)
{
	if(!scode) return 0;
	struct l2ticnum_ans * ansp = ( struct l2ticnum_ans * ) g_pReceive;
	if(ansp->setcode!=scode->setcode || memcmp(ansp->code,scode->Code,SH_CODE_LEN))
		return 0;
	MaxTicSeq = ansp->MaxSeq;
	return 1;
}

BOOL CGetData::GetTickDataL2(struct MemStkInfo * scode,DWORD startxh, short num, BOOL bReverse)
{
	if(!CanLevel2()) return FALSE;
	if (!scode)   return FALSE; 

	struct l2tic_req * reqp;
	reqp = ( struct l2tic_req * ) pTransfer;
	if(bReverse)
		reqp->req = L2_REVTIC_NREQ;
	else
		reqp->req = L2_TIC_NREQ;
	reqp->setcode = scode->setcode;
	memmove ( reqp->code, scode->Code, SH_CODE_LEN );
	reqp->startxh = startxh;
	reqp->num = num;
	if(g_pComm->SendIt (  pTransfer, sizeof ( struct l2tic_req ) ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetTickDataL2ACK(struct MemStkInfo *scode,struct TickData_L2 *indap,short num,DWORD &MaxTicSeq)
{
	if(!scode) return 0;
	for(int i=0;i<g_AnsHeader.RawLen;i++)
		g_pReceive[i]^=g_AntiHack.cLevel2XORChar_TickL2;
	short len,getnum=0;
	char precise;
	memset ( indap, 0, num * sizeof ( struct TickData_L2 ) );
	memmove(&getnum,g_pReceive,sizeof(short));
	getnum=max(0,getnum);
	getnum=min(num,getnum);
	memmove(&MaxTicSeq,g_pReceive+sizeof(short),sizeof(DWORD));
	len = sizeof(short)+sizeof(DWORD);
	precise = GetPrecise( scode->Code,scode->setcode );
	TickDataL2_rtn ( g_pReceive+len,indap,getnum,len);
	return getnum;
}

BOOL CGetData::GetWTRank(short setcode,short fltype)
{
	if(!CanLevel2()) return FALSE;

	struct l2_wt_rank_req * reqp = ( struct l2_wt_rank_req * ) pTransfer;
	reqp->req = L2_WT_RANK_NREQ;
	reqp->setcode = setcode;
	reqp->fltype = fltype;
	reqp->reserved = 0;
	if(g_pComm->SendIt (  pTransfer, sizeof ( struct l2_wt_rank_req ) ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetWTRankACK(struct l2_WTRankings *Rankings)
{
	struct l2_wt_rank_ans * ansp = ( struct l2_wt_rank_ans * ) g_pReceive;
	memcpy(Rankings,ansp->list,20*sizeof(struct l2_WTRankings));
	return 1;
}

BOOL CGetData::GetSCancelRank(short setcode,short fltype)
{
	if(!CanLevel2()) return FALSE;
	
	struct l2_singlecancel_rank_req * reqp = ( struct l2_singlecancel_rank_req * ) pTransfer;
	reqp->req = L2_SINGLECANCEL_RANK_NREQ;
	reqp->setcode = setcode;
	reqp->fltype = fltype;
	reqp->reserved = 0;
	if(g_pComm->SendIt (  pTransfer, sizeof ( struct l2_singlecancel_rank_req ) ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetSCancelRankACK(struct l2_SingleCancelRankings *Rankings)
{
	struct l2_singlecancel_rank_ans * ansp = ( struct l2_singlecancel_rank_ans * ) g_pReceive;
	memcpy(Rankings,ansp->list,20*sizeof(struct l2_SingleCancelRankings));
	return 1;
}

BOOL CGetData::GetTCancelRank(short setcode,short fltype)
{
	if(!CanLevel2()) return FALSE;

	struct l2_totalcancel_rank_req * reqp = ( struct l2_totalcancel_rank_req * ) pTransfer;
	reqp->req = L2_TOTALCANCEL_RANK_NREQ;
	reqp->setcode = setcode;
	reqp->fltype = fltype;
	reqp->reserved = 0;
	if(g_pComm->SendIt (  pTransfer, sizeof ( struct l2_totalcancel_rank_req ) ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetTCancelRankACK(struct l2_TotalCancelRankings *Rankings)
{
	struct l2_totalcancel_rank_ans * ansp = ( struct l2_totalcancel_rank_ans * ) g_pReceive;
	memcpy(Rankings,ansp->list,20*sizeof(struct l2_TotalCancelRankings));
	return 1;
}

BOOL CGetData::GetHYRank(short setcode)
{
	if(!CanLevel2()) return FALSE;

	struct l2_hy_rank_req * reqp = ( struct l2_hy_rank_req * ) pTransfer;
	reqp->req = L2_HY_RANK_NREQ;
	reqp->setcode = setcode;
	reqp->reserved = 0;
	if(g_pComm->SendIt (  pTransfer, sizeof ( struct l2_hy_rank_req ) ) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetHYRankACK(float &MarketTotalValueTraded,struct l2_HYRankings *HYRankings,struct l2_HYGPRankings *HYGPRankings)
{
	struct l2_hy_rank_ans * ansp = ( struct l2_hy_rank_ans * ) g_pReceive;
	MarketTotalValueTraded = ansp->MarketTotalValueTraded;
	memcpy(HYRankings,ansp->HYlist,10*sizeof(struct l2_HYRankings));
	memcpy(HYGPRankings,ansp->HYGPlist,10*5*sizeof(struct l2_HYGPRankings));
	return 1;
}

BOOL CGetData::GetG2DeepWarn(short setcode,long nReqWarnNo,long nReqWarnNum)
{
	struct deepwarn_req * reqp = ( struct deepwarn_req * ) pTransfer;
	reqp->req		= G2_DEEPWARN_NREQ;
	reqp->setcode	= setcode;
	reqp->ItemPos	= nReqWarnNo;
	reqp->ItemNum	= nReqWarnNum;
	if(g_pComm->SendIt( pTransfer,sizeof(deepwarn_req) ) != 0)
		return FALSE;
	return TRUE;
}

int CGetData::GetG2DeepWarnACK(LPWARNATOM pWarnAtom,long nWarnAtom)
{
	long lGetNum = long(*((short *)g_pReceive));
	if(pWarnAtom==NULL) 
		return lGetNum;
	long len = sizeof(short);
	nWarnAtom = min(nWarnAtom,MAX_DEEPWARN);
	nWarnAtom = max(0,min(lGetNum,nWarnAtom));
	if(nWarnAtom>0) 
		memcpy(pWarnAtom,g_pReceive+len,nWarnAtom*sizeof(WARNATOM));
	return nWarnAtom;
}

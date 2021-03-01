#include "stdafx.h"
#include <sys\stat.h>
#include "ProgressWnd.h"
#include "HQComm.h"
#include "getdata.h"
#include "BlowFish.h"

///////////////////////////////////////////////////////////////////
//							扩充请求GetData函数
///////////////////////////////////////////////////////////////////

BOOL CGetData::SendDataDigReq()
{
	struct datadig_req *preq;
	preq = (struct datadig_req *)pTransfer;
	preq->req = DATADIG_EXT_NREQ;
	preq->reserved = 0;
	if(g_pComm->SendIt(pTransfer, sizeof(struct datadig_req)) != 0)
		return FALSE;
	return TRUE;
}

BOOL CGetData::DataDigACK(long &clientnum,long &usedclientnum,long &seconds,char *HostName,short &attention_num,short &resonate_num,short &no_attention_num,short &no_resonate_num,short *attention_index,short *resonate_index,short *no_attention_index,short *no_resonate_index,unsigned long *attention_rate,unsigned long *resonate_rate,unsigned long *no_attention_rate,unsigned long *no_resonate_rate,long contentlen,char **pContent)
{
	struct datadig_ext_ans *pans;
	pans = (struct datadig_ext_ans *)g_pReceive;

	clientnum = pans->clientnum;
	usedclientnum = pans->usedclientnum;
	seconds = pans->seconds;
	strcpy(HostName,pans->HostName);

	attention_num = pans->attention_num;
	resonate_num = pans->resonate_num;
	no_attention_num = pans->no_attention_num;
	no_resonate_num = pans->no_resonate_num;

	contentlen = pans->contentlen;
	(*pContent) = new char[contentlen+1];
	memcpy(*pContent,pans->content,contentlen);
	(*pContent)[contentlen] = 0;

	char tmpCode[SH_CODE_LEN+1];
	int i;
	for(i=0;i < attention_num;i++)
	{
		memcpy(tmpCode,pans->attention[i].stockcode,SH_CODE_LEN);
		tmpCode[SH_CODE_LEN] = 0;
		attention_index[i] = g_pStockIO->GetIndex(tmpCode,pans->attention[i].setcode);		
		if(attention_index[i]==-1) 
		{
			attention_num = 0;
			return FALSE;
		}
		attention_rate[i] = pans->attention[i].rate;
	}
	for(i=0;i < resonate_num;i++)
	{
		memcpy(tmpCode,pans->resonate[i].stockcode,SH_CODE_LEN);
		tmpCode[SH_CODE_LEN] = 0;
		resonate_index[i] = g_pStockIO->GetIndex(tmpCode,pans->resonate[i].setcode);		
		if(resonate_index[i]==-1) 
		{
			resonate_num = 0;
			return FALSE;
		}
		resonate_rate[i] = pans->resonate[i].rate;
	}
	for(i=0;i < no_attention_num;i++)
	{
		memcpy(tmpCode,pans->no_attention[i].stockcode,SH_CODE_LEN);
		tmpCode[SH_CODE_LEN] = 0;
		no_attention_index[i] = g_pStockIO->GetIndex(tmpCode,pans->no_attention[i].setcode);		
		if(no_attention_index[i]==-1) 
		{
			no_attention_num = 0;
			return FALSE;
		}
		no_attention_rate[i] = pans->no_attention[i].rate;
	}
	for(i=0;i < no_resonate_num;i++)
	{
		memcpy(tmpCode,pans->no_resonate[i].stockcode,SH_CODE_LEN);
		tmpCode[SH_CODE_LEN] = 0;
		no_resonate_index[i] = g_pStockIO->GetIndex(tmpCode,pans->no_resonate[i].setcode);		
		if(no_resonate_index[i]==-1) 
		{
			no_resonate_num = 0;
			return FALSE;
		}
		no_resonate_rate[i] = pans->no_resonate[i].rate;
	}
	return TRUE;
}

BOOL  CGetData::GetBigVolData(long getpos)
{
	struct bigvoltick_req *preq;
	preq = (struct bigvoltick_req *)pTransfer;
	preq->req = BIGVOLTICK_NREQ;
	preq->reserved = 0;
	preq->getpos = getpos;
	preq->avail_vol = 500;
	if(g_pComm->SendIt(pTransfer, sizeof(struct bigvoltick_req)) != 0)
		return FALSE;
	return TRUE;
}

short CGetData::GetBigVolDataACK(struct BigVolTick *pTick,long wantnum,long & lastpos)
{
	struct bigvoltick_ans *pans;
	pans = (struct bigvoltick_ans *)g_pReceive;
	long tmpNum = min(pans->ticknum,wantnum);
	if(tmpNum > 0)
	{
		memcpy(pTick,pans->list,tmpNum*sizeof(struct BigVolTick));
		lastpos = pans->lastpos;
	}
	return tmpNum;
}

//定时请求信息
BOOL	CGetData::GetTimerMoreInfo()
{
	REQINFONULL *reqp;
	reqp = (REQINFONULL*)pTransfer;
	reqp->ReqID = ACTIVEINFO_NREQ;
	reqp->TimeTag = g_LastTimeTag;
	if(g_pComm->SendIt( pTransfer, sizeof ( REQINFONULL )) != 0)
		return FALSE;
	return TRUE;	
}

BOOL	CGetData::GetTimerMoreInfoACK()
{
	REQINFOTIMER *ansp;
	ansp = (PREQINFOTIMER)g_pReceive;
	if(ansp->MoreInfo==URGENT_TYPE)
	{
		URGENTINFO uti={0};
		uti.Type = 1;
		uti.TimeTag = ansp->TimeTag;
		uti.TitleLen = strlen(ansp->Info);
		char *strContent = &ansp->Info[uti.TitleLen+1];
		uti.ContentLen = strlen(strContent);
		uti.Info = new char[uti.TitleLen+uti.ContentLen+2];
		memcpy(uti.Info, ansp->Info, uti.TitleLen+uti.ContentLen+2);

		g_aUrgentInfo.push_back(uti);
		InterlockedExchange(&g_LastTimeTag, uti.TimeTag);
		return TRUE;
	}
	return FALSE;
}

//公告发布
BOOL	CGetData::GetInfoUpload(BYTE InfoType, time_t TimeTag, char *Title, char *Content)
{
	REQINFOUPLOAD *reqp;
	reqp = (REQINFOUPLOAD*)pTransfer;
	reqp->ReqID = INFOUPLOAD_NREQ;
	reqp->Type = InfoType;
	reqp->TimeTag = TimeTag;
	reqp->InfoLen = strlen(Title)+strlen(Content)+2;
	memcpy(reqp->Info, Title, strlen(Title));
	reqp->Info[strlen(Title)] = 0;
	memcpy(&reqp->Info[strlen(Title)+1], Content, strlen(Content));
	reqp->Info[strlen(Title)+1+strlen(Content)] = 0;
	reqp->ReqUpLoadLen = sizeof(REQINFOUPLOAD)-1+reqp->InfoLen;
	if(g_pComm->SendIt( pTransfer, reqp->ReqUpLoadLen) != 0)
		return FALSE;
	return TRUE;	
}

// HKNewsGet.cpp: implementation of the CHKNewsGet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tdxw.h"
#include "HKNewsGet.h"
#include "zlib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define CLOSE_SOCKET(hSocket) {if(hSocket!=INVALID_SOCKET){closesocket(hSocket);hSocket=INVALID_SOCKET;}}

short				g_wNewsPort=7711;
char				g_NewsMainUrl[50]={0};
CCriticalSection	g_NewsComSec;

BOOL	SendData(SOCKET hSocket,LPSTR SendBuf,long DataSize)
{
	g_NewsComSec.Lock();
	for(long Offset=0,SendSize=0;DataSize>0;)
	{
		SendSize = send(hSocket,SendBuf+Offset,DataSize,0);
		if(SendSize==SOCKET_ERROR)
		{
			if(WSAGetLastError()==WSAEWOULDBLOCK)
			{
				continue;
			}
			g_NewsComSec.Unlock();
			return(FALSE);
		}
		DataSize -= SendSize;
		Offset += SendSize;
	}
	g_NewsComSec.Unlock();
	return(TRUE);
}

BOOL	RecvData(SOCKET hSocket,LPSTR RecvBuf,long NeedRcvSize)
{
	g_NewsComSec.Lock();
	int nZeroToo = 0;
	for(long Offset=0,RcvSize=0;NeedRcvSize>0;)
	{
		RcvSize = recv(hSocket,RecvBuf+Offset,NeedRcvSize,0);
		if(RcvSize==SOCKET_ERROR)
		{
			if(WSAGetLastError()==WSAEWOULDBLOCK)
			{
				continue;
			}
			g_NewsComSec.Unlock();
			return(FALSE);
		}
		Offset += RcvSize;
		NeedRcvSize -= RcvSize;
		if(RcvSize==0) nZeroToo++;
		else nZeroToo = 0;
		if(nZeroToo>3) break;
	}
	g_NewsComSec.Unlock();
	return(TRUE);
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHKNewsGet::CHKNewsGet()
{
	m_hSocket = INVALID_SOCKET;
	InterlockedExchange(&m_bConnected2News, 0);
	memset(m_pTransfer, 0, sizeof(m_pTransfer));
	memset(m_pTcpBuf, 0, sizeof(m_pTcpBuf));
	InitNewsThread();
}

CHKNewsGet::~CHKNewsGet()
{
	UninitNewsThread();
}
//////////////////////////////////////////////////////////////////////////
BOOL	CHKNewsGet::InitNewsThread()
{
	UninitNewsThread();
	
	char errMsg[128]={0};
	if (!StartSocket(errMsg)) return FALSE;
	int nWhich=g_DSHost.PrimaryHost;
	if(nWhich<0||nWhich>=g_DSHost.HostNum) nWhich=0;
	if (!ConnectWithNews(errMsg, g_DSHost.IPAddress[nWhich], g_DSHost.Port[nWhich]))
		return FALSE;
	InterlockedExchange(&m_bConnected2News, 1);
	return TRUE;
}

void	CHKNewsGet::UninitNewsThread()
{
	if(m_hSocket!=INVALID_SOCKET)
	{
		WSAAsyncSelect(m_hSocket,0,0,0);
		CLOSE_SOCKET(m_hSocket);
	}
	InterlockedExchange(&m_bConnected2News, 0);
}

BOOL	CHKNewsGet::StartSocket(LPSTR strError)
{
	if(m_hSocket!=INVALID_SOCKET)
	{
		WSAAsyncSelect(m_hSocket,0,0,0);
		CLOSE_SOCKET(m_hSocket);
	}
	
	m_hSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(m_hSocket==INVALID_SOCKET)
	{
		lstrcpy(strError,"创建套口字失败");
		return(FALSE);
	}

	return TRUE;
}

BOOL	CHKNewsGet::ConnectWithNews(LPSTR strError,const char *hostname,UINT hostport)
{
	if(m_bConnected2News) return TRUE;
	long tick1 = GetTickCount();
	if(!DirectConnect(m_hSocket,g_ProxyInfo,hostname,hostport,tick1,strError))
		return FALSE;	
	InterlockedExchange(&m_bConnected2News, 1);
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
//发送
BOOL	CHKNewsGet::SendIt(void *lpBuf, int nBufLen)
{
	if(!m_bConnected2News)
		InitNewsThread();
	
	REQHEADER	Header={0};
	Header.Info.Version	= 1;					//扩展行情协议版本是1
	Header.Info.Compressed	= 0;
	Header.Info.Encrypted	= 0;
	Header.Info.Reserved	= 0;
	Header.MainID			= 0;
	Header.AssisID			= 0;
	Header.Priority			= 0;
	Header.PacketLen		= nBufLen;
	Header.RawLen			= nBufLen;
	
	REQBUFFER	Buffer = {0};
	memcpy(&Buffer.ReqHeader, &Header, sizeof(REQHEADER));
	memcpy(&Buffer.ReqBuffer, lpBuf, nBufLen);
	
	m_bConnected2News=SendData(m_hSocket,(char*)&Buffer, sizeof(REQHEADER)+nBufLen);
	//重连一次,一般在此判断连接状况
	if(!m_bConnected2News&&InitNewsThread())
		m_bConnected2News=SendData(m_hSocket,(char*)&Buffer, sizeof(REQHEADER)+nBufLen);
	return m_bConnected2News;
}

BOOL	CHKNewsGet::ReqNewsConfig()
{
	GETMAININFO_REQ	*pReq = (GETMAININFO_REQ*)m_pTransfer;
	memset(pReq,0,sizeof(GETMAININFO_REQ));
	pReq->iRetCode = DS_MAININFO_REQ;
	
	return SendIt(m_pTransfer,sizeof(GETMAININFO_REQ));
}

BOOL	CHKNewsGet::ReqNewsList(char *OuterID, int iOffSet)
{
	GETTITLEINFO_REQ	*pReq = (GETTITLEINFO_REQ*)m_pTransfer;
	memset(pReq,0,sizeof(GETTITLEINFO_REQ));
	pReq->iRetCode = DS_TITLEINFO_REQ;
	strncpy(pReq->OuterID, OuterID, 63);
	pReq->iOffSet = iOffSet;

	return SendIt(m_pTransfer,sizeof(GETTITLEINFO_REQ));
}

BOOL	CHKNewsGet::ReqNewsContent(MENUINFOSTRUCT_PLUS *pMis, short iOffset)
{
	memset(m_pTransfer, 0, sizeof(m_pTransfer));
	GETONEINFO_REQ	*pReq = (GETONEINFO_REQ*)m_pTransfer;
	pReq->iRetCode = DS_ONEINFO_REQ;
	strncpy(pReq->iFatherOuterID, pMis->mis.iFatherOuterID, 63);
	pReq->iInnerID = pMis->mis.iInnerID;
	strncpy(pReq->OuterID, pMis->mis.OuterID, 63);
	pReq->iOffSet = iOffset;
	return SendIt(m_pTransfer,sizeof(GETONEINFO_REQ));
}
//////////////////////////////////////////////////////////////////////////
//接收
DWORD	CHKNewsGet::OnRecvHead(ANSHEADER *pHeader)		//阻塞
{
	BOOL bRcvHeadOK = RecvData(m_hSocket,(char*)pHeader,sizeof(ANSHEADER));
	if(!bRcvHeadOK) return(0);
	
	if(pHeader->Separator!=SEPARATOR_NUM) return(0);
	if(pHeader->PacketLen>ANSBUFFER_LEN||pHeader->PacketLen<2)	return(0);
	if(pHeader->RawLen>ANSBUFFER_LEN||pHeader->RawLen<2)	return(0);
	return(pHeader->PacketLen);
}

BOOL	CHKNewsGet::OnRecvConfig(ANSHEADER *pHeader, vector<MENUINFOSTRUCT_PLUS> &config)
{
	config.clear();
	
	DWORD PacketLen = pHeader->PacketLen;
	char *pData = new char[PacketLen+1024];
	memset(pData,0,PacketLen+1024);
	BOOL bRcvDataOK = RecvData(m_hSocket,(char*)pData,PacketLen);
	if(!bRcvDataOK) 
	{
		TDELA(pData);
		return FALSE;
	}
	
	DWORD nRawLen = pHeader->RawLen;
	char  * pRawBuf = new char[nRawLen+1024];
	memset(pRawBuf, 0, nRawLen+1024);
	//解压
	if (pHeader->Info.Compressed)
	{
		if(!uncompress((BYTE *)pRawBuf,&nRawLen,(BYTE *)pData,PacketLen))
		{
			if(pHeader->RawLen != nRawLen)
			{
				TDELA(pRawBuf);
				TDELA(pData);
				return FALSE;
			}
		}
	}
	else 
	{
		memcpy(pRawBuf, pData, nRawLen);
	}
	TDELA(pData);

	switch(pHeader->req)
	{
	case DS_MAININFO_REQ:
		{
			GETMAININFO_ANS *pAns = (GETMAININFO_ANS*)pRawBuf;
			int nPos = sizeof(int);
			for(int i=0;i<pAns->iInfoNum;i++)
			{
				MENUINFOSTRUCT *pmis = (MENUINFOSTRUCT *)&pRawBuf[nPos];
				MENUINFOSTRUCT_PLUS misp;
				memcpy(&misp.mis, pmis, sizeof(MENUINFOSTRUCT));
				misp.strShowName = (CString)(LPCSTR)pmis->chTitle;
				config.push_back(misp);
				nPos += (sizeof(MENUINFOSTRUCT)+pmis->uiTitleLen);
			}
		}
		break;
	}
	TDELA(pRawBuf);
	return TRUE;
}

BOOL	CHKNewsGet::OnRecvTitle(ANSHEADER *pHeader, vector<MENUINFOSTRUCT_PLUS> &list)	//阻塞
{	
	DWORD PacketLen = pHeader->PacketLen;
	char *pData = new char[PacketLen+1024];
	memset(pData,0,PacketLen+1024);
	BOOL bRcvDataOK = RecvData(m_hSocket,(char*)pData,PacketLen);
	if(!bRcvDataOK) 
	{
		TDELA(pData);
		return FALSE;
	}
	
	DWORD nRawLen = pHeader->RawLen;
	char  * pRawBuf = new char[nRawLen+1024];
	memset(pRawBuf, 0, nRawLen+1024);
	//解压
	if (pHeader->Info.Compressed)
	{
		if(!uncompress((BYTE *)pRawBuf,&nRawLen,(BYTE *)pData,PacketLen))
		{
			if(pHeader->RawLen != nRawLen)
			{
				TDELA(pRawBuf);
				TDELA(pData);
				return FALSE;
			}
		}
	}
	else 
		memcpy(pRawBuf, pData, nRawLen);
	TDELA(pData);

	switch(pHeader->req)
	{
	case DS_TITLEINFO_REQ:
		{
			GETTITLEINFO_ANS *pAns = (GETTITLEINFO_ANS*)pRawBuf;
			if(pAns->iInfoNum<1) 
			{
				TDELA(pRawBuf);
				return FALSE;
			}

			int nPos = sizeof(GETTITLEINFO_ANS)-sizeof(MENUINFOSTRUCT);
			for(int i=0;i<pAns->iInfoNum;i++)
			{
				MENUINFOSTRUCT *pmis = (MENUINFOSTRUCT *)&pRawBuf[nPos];
				MENUINFOSTRUCT_PLUS misp;
				memcpy(&misp.mis, pmis, sizeof(MENUINFOSTRUCT));
				misp.strShowName = (CString)(LPCSTR)pmis->chTitle;
				list.push_back(misp);
				nPos += (sizeof(MENUINFOSTRUCT)+pmis->uiTitleLen);
			}
		}
		break;
	}
	TDELA(pRawBuf);
	return TRUE;
}

BOOL	CHKNewsGet::OnRecvContent(ANSHEADER *pHeader, CString &Content)	//阻塞
{	
	DWORD PacketLen = pHeader->PacketLen;
	char *pData = new char[PacketLen+1024];
	memset(pData,0,PacketLen+1024);
	BOOL bRcvDataOK = RecvData(m_hSocket,(char*)pData,PacketLen);
	if(!bRcvDataOK) 
	{
		TDELA(pData);
		return FALSE;
	}
	
	DWORD nRawLen = pHeader->RawLen;
	char  * pRawBuf = new char[nRawLen+1024];
	memset(pRawBuf, 0, nRawLen+1024);
	//解压
	if (pHeader->Info.Compressed)
	{
		if(!uncompress((BYTE *)pRawBuf,&nRawLen,(BYTE *)pData,PacketLen))
		{
			if(pHeader->RawLen != nRawLen)
			{
				TDELA(pRawBuf);
				TDELA(pData);
				return FALSE;
			}
		}
	}
	else 
		memcpy(pRawBuf, pData, nRawLen);
	TDELA(pData);

	switch(pHeader->req)
	{
	case DS_ONEINFO_REQ:
		{
			GETONEINFO_ANS *pAns = (GETONEINFO_ANS*)pRawBuf;
			if(pAns->iInfoLen<1||pAns->iInfoLen>=nRawLen)
			{
				TDELA(pRawBuf);
				return FALSE;
			}
			Content += (CString)pAns->chInfo;
		}
		break;
	}
	TDELA(pRawBuf);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
BOOL	CHKNewsGet::GetNewsConfig(vector<MENUINFOSTRUCT_PLUS> &vNewsConfigList)
{
	if(!ReqNewsConfig()) return FALSE;
	ANSHEADER Header={0};
	if(!OnRecvHead(&Header)) return FALSE;
	if(!OnRecvConfig(&Header, vNewsConfigList)) return FALSE;
	return TRUE;
}

BOOL	CHKNewsGet::GetTitleList(vector<MENUINFOSTRUCT_PLUS> &vTitleList, MENUINFOSTRUCT_PLUS *pInfo_title, char setcode, char  *code)
{
	if(!ReqNewsList(pInfo_title->mis.OuterID, vTitleList.size())) return FALSE;
	ANSHEADER Header={0};
	if(!OnRecvHead(&Header)) return FALSE;
	if(!OnRecvTitle(&Header, vTitleList)) return FALSE;
	return TRUE;
}

BOOL	CHKNewsGet::GetContent(CString &Content, MENUINFOSTRUCT_PLUS *pInfo_title, char setcode, char  *code)
{
	Content.Empty();
	while(TRUE)
	{
		if(!ReqNewsContent(pInfo_title, Content.GetLength())) 
			return FALSE;
		ANSHEADER Header={0};
		if(!OnRecvHead(&Header)) return FALSE;
		if(!OnRecvContent(&Header, Content)) return FALSE;
	}
	return TRUE;
}

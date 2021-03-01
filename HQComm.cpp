#include "stdafx.h"
#include "HQComm.h"
#include "zlib.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
//
//		HQComm类
//
/////////////////////////////////////////////////////////////////////////////////////////////////

CHQComm::CHQComm(int commtype)
{
	AfxSocketInit();
	m_nPort=7709;
	m_bConnected=FALSE;
	m_sError = _T("就绪");

	m_pThread = NULL;
	m_SendTop = 0;

	m_bBuffEven = TRUE;	
	m_bDirectEven = TRUE;	
	m_bDirectSendNum = 0;	
	m_HQBufFlag = 0;
	m_pBlowfish = NULL;

	m_nTimeCnt = 0;

	m_nCommType = commtype;
}

CHQComm::~CHQComm()
{
	if(m_bConnected)
		Disconnect();
}

void	CHQComm::SetEncryptKey(char *KeyStr)
{
	if(KeyStr[0]!=0)
	{
		m_pBlowfish = new CBlowfish;
		m_pBlowfish->InitializeBlowfish((BYTE *)KeyStr,strlen(KeyStr));
	}
	else
	{
		TDEL(m_pBlowfish);
	}
}

void	CHQComm::DecryptBuf(char *buf,int nBufLen)
{
	if(m_pBlowfish)
		m_pBlowfish->Decrypt((BYTE *)buf,((nBufLen-1)/8)*8);		
}

void CHQComm::SetServerProperties( LPCTSTR sServerHostName, UINT nPort)
{
	if(!sServerHostName)	return;
	m_sHostName	  = sServerHostName;
	m_nPort		  = nPort;
}

CString CHQComm::GetServerHostName()
{
	return m_sHostName;
}

UINT CHQComm::GetPort()
{
	return m_nPort;
}

CString CHQComm::GetErrorStr()
{
	return m_sError;
}

BOOL CHQComm::Connect()
{
	if( m_bConnected ) return TRUE;
	m_bBuffEven = TRUE;
	m_bDirectEven = TRUE;
	m_bDirectSendNum = 0;
	SetSendTimeCnt(0);
	if( !ClientSock.Create() )
	{
		m_sError = _T( "不能创建套接字!" );
		return FALSE;
	}
	ClientSock.InitIt(this,m_nCommType);
	ClientSock.m_bStartAsyn = FALSE;
	if(!g_ProxyInfo.m_bProxy)				//无代理连接
	{
		ClientSock.m_bStartAsyn = TRUE;
		//ISA代理
		if(g_ProxyInfo.bTdxProxy)
		{
			if(!ClientSock.Connect(g_ProxyInfo.tdxip[g_ProxyInfo.tdxprimary], g_ProxyInfo.tdxport[g_ProxyInfo.tdxprimary]))
			{
				int	nResult = WSAGetLastError();
				if( nResult != WSAEWOULDBLOCK )
				{
					SafeCloseSocket();
					return FALSE;
				}
			}
		}
		else if( !ClientSock.Connect( GetServerHostName(), GetPort() ) )
		{
			int	nResult = WSAGetLastError();
			if( nResult != WSAEWOULDBLOCK )
			{
				SafeCloseSocket();
				return FALSE;
			}
		}
		return TRUE;
	}
	else
	{
		if( !ClientSock.Connect( g_ProxyInfo.m_strProxyIP,g_ProxyInfo.m_nProxyPort) )
		{
			int	nResult = WSAGetLastError();
			if( nResult != WSAEWOULDBLOCK )
			{
				SafeCloseSocket();
				return FALSE;
			}
		}
		return TRUE;
	}
	return TRUE;	
}

BOOL CHQComm::StartThread()
{
	TDEL(m_pThread);
	m_pThread = new CReqThread;
	if (!m_pThread)	return FALSE;
	m_pThread->InitIt(this);
	if (!m_pThread->CreateThread())		//执行该线程
	{
		TDEL(m_pThread);
		return FALSE;
	}
	if(!ClientSock.m_pThread)
		ClientSock.StartThread();
	m_bConnected = TRUE;
	return TRUE;
}

BOOL CHQComm::AfterConnectProxy()
{
	if(g_ProxyInfo.m_nSockType==0)		//SOCK4代理
	{
		char buff[100];
		memset(buff,0,100);
		struct sock4req1 *m_proxyreq;
		m_proxyreq = (struct sock4req1 *)buff;
		m_proxyreq->VN = 4;
		m_proxyreq->CD = 1;
		if(g_ProxyInfo.bTdxProxy)
		{
			m_proxyreq->Port = ntohs(g_ProxyInfo.tdxport[g_ProxyInfo.tdxprimary]);
			m_proxyreq->IPAddr = inet_addr(g_ProxyInfo.tdxip[g_ProxyInfo.tdxprimary]);
		}
		else 
		{
			m_proxyreq->Port = ntohs(GetPort());
			m_proxyreq->IPAddr = inet_addr(GetServerHostName());
		}
		if(m_proxyreq->IPAddr == INADDR_NONE) //如果是域名
		{
			LPHOSTENT lphost;
			if(g_ProxyInfo.bTdxProxy)
				lphost = gethostbyname(g_ProxyInfo.tdxip[g_ProxyInfo.tdxprimary]);
			else
				lphost = gethostbyname(GetServerHostName());
			if(lphost)
				m_proxyreq->IPAddr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
			else
			{
				m_sError = _T("代理失败!");
				SafeCloseSocket();
				return FALSE;
			}
		}
		ClientSock.Send(buff,9);
		struct sock4ans1 *m_proxyans;
		m_proxyans = (struct sock4ans1 *)buff;
		memset(buff,0,100);
		if(!ReceiveBytes(&ClientSock,buff,sizeof(struct sock4ans1))) 
		{
			m_sError = _T("代理服务器超时!");
			SafeCloseSocket();
			return FALSE;
		}
		if(m_proxyans->VN != 0 || m_proxyans->CD != 90)
		{
			m_sError = _T("通过代理连接行情主站不成功!");
			SafeCloseSocket();
			return FALSE;
		}
		ClientSock.m_bStartAsyn = TRUE;
	}
	else if(g_ProxyInfo.m_nSockType==1)	//SOCK5代理
	{
		char buff[600];
		struct sock5req1 *m_proxyreq1;
		m_proxyreq1 = (struct sock5req1 *)buff;
		m_proxyreq1->Ver = 5;
		m_proxyreq1->nMethods = 2;
		m_proxyreq1->Methods[0] = 0;
		m_proxyreq1->Methods[1] = 2;
		ClientSock.Send(buff,4);
		struct sock5ans1 *m_proxyans1;
		m_proxyans1 = (struct sock5ans1 *)buff;
		memset(buff,0,600);
		if(!ReceiveBytes(&ClientSock,buff,sizeof(struct sock5ans1))) 
		{
			m_sError = _T("代理服务器超时!");
			SafeCloseSocket();
			return FALSE;
		}
		if(m_proxyans1->Ver != 5 || (m_proxyans1->Method!=0 && m_proxyans1->Method!=2))
		{
			m_sError = _T("通过代理连接行情主站不成功!");
			SafeCloseSocket();
			return FALSE;
		}
		if(m_proxyans1->Method == 2)
		{
			// SOCKS5的验证请求封包
			// 封包格式为:0x01|用户名长度|用户名|密码长度|密码内容
			int nUserLen = strlen(g_ProxyInfo.m_strProxyUser);
			int nPassLen = strlen(g_ProxyInfo.m_strProxyPass);
			char *tmpBuf=(char *)buff;
			tmpBuf[0]=1;			
			tmpBuf[1]=nUserLen;
			memcpy(tmpBuf+2,g_ProxyInfo.m_strProxyUser,nUserLen);			
			tmpBuf[2+nUserLen]=nPassLen;
			memcpy(tmpBuf+2+nUserLen+1,g_ProxyInfo.m_strProxyPass,nPassLen);
			int nReqLen=2+nUserLen+1+nPassLen;
			ClientSock.Send(buff,nReqLen);
			struct authans *m_authans = (struct authans *)buff;
			memset(buff,0,600);
			if(!ReceiveBytes(&ClientSock,buff,sizeof(struct authans))) 
			{
				m_sError = _T("代理服务器超时!");
				SafeCloseSocket();
				return FALSE;
			}
			if(m_authans->Ver != 1 || m_authans->Status != 0)
			{
				m_sError = _T("代理服务器用户验证不成功!");
				SafeCloseSocket();
				return FALSE;
			}
		}
		struct sock5req2 *m_proxyreq2;
		m_proxyreq2 = (struct sock5req2 *)buff;
		m_proxyreq2->Ver = 5;
		m_proxyreq2->Cmd = 1;
		m_proxyreq2->Rsv = 0;
		unsigned long tmpLong = 0;
		unsigned short port = 0;
		BOOL bHostName=FALSE,otherlen=0;		
		if(g_ProxyInfo.bTdxProxy)
		{
			tmpLong=inet_addr(g_ProxyInfo.tdxip[g_ProxyInfo.tdxprimary]);
			port=ntohs(g_ProxyInfo.tdxport[g_ProxyInfo.tdxprimary]);
		}
		else
		{
			tmpLong = inet_addr(GetServerHostName());
			if(tmpLong == INADDR_NONE) //如果是域名
				bHostName=TRUE;
			port=ntohs(GetPort());
		}
		if(!bHostName)
		{
			m_proxyreq2->Atyp = 1;//如ATYPE为ATYP_IPV4ADDR,则地址为4字节,网络字节序
			memcpy(m_proxyreq2->other,&tmpLong,4);
			memcpy(m_proxyreq2->other+4,&port,2);
			otherlen=6;
		}
		else
		{
			m_proxyreq2->Atyp = 3;//如ATYPE为ATYP_HOSTNAME,则地址为[长度+目标地址]
			m_proxyreq2->other[0]=strlen(GetServerHostName());
			memcpy(m_proxyreq2->other+1,GetServerHostName(),strlen(GetServerHostName()));
			memcpy(m_proxyreq2->other+1+strlen(GetServerHostName()),&port,2);
			otherlen=strlen(GetServerHostName())+1+2;
		}
		ClientSock.Send(buff,sizeof(struct sock5req2)-1+otherlen);

		struct sock5ans2 *m_proxyans2;
		memset(buff,0,600);
		m_proxyans2 = (struct sock5ans2 *)buff;
		if(!ReceiveBytes(&ClientSock,buff,sizeof(struct sock5ans2))) 
		{
			m_sError = _T("代理服务器超时!");
			SafeCloseSocket();
			return FALSE;
		}
		if(m_proxyans2->Ver != 5 || m_proxyans2->Rep != 0)
		{
			m_sError = _T("通过代理连接行情主站不成功!");
			SafeCloseSocket();
			return FALSE;
		}
		ClientSock.m_bStartAsyn = TRUE;
	}
	else						//HTTP代理
	{
		char buff[600];
		memset(buff,0,600*sizeof(char));
		if(g_ProxyInfo.m_strProxyUser[0]==0)
		{
			if(!g_ProxyInfo.bTdxProxy)
				sprintf(buff, "%s%s:%d%s","CONNECT ",GetServerHostName(),GetPort()," HTTP/1.0\r\nUser-Agent: Mozilla/4.0\r\nConnection: Keep-Alive\r\n\r\n");
			else 
				sprintf(buff, "%s%s:%d%s","CONNECT ",g_ProxyInfo.tdxip[g_ProxyInfo.tdxprimary],g_ProxyInfo.tdxport[g_ProxyInfo.tdxprimary]," HTTP/1.0\r\nUser-Agent: tdxproxy/1.0\r\n\r\n");
		}
		else
		{
			CString str;
			str.Format("CONNECT %s:%d HTTP/1.0\r\n",g_ProxyInfo.bTdxProxy?g_ProxyInfo.tdxip[g_ProxyInfo.tdxprimary]:GetServerHostName(),g_ProxyInfo.bTdxProxy?g_ProxyInfo.tdxport[g_ProxyInfo.tdxprimary]:GetPort());
			CString userpass=g_ProxyInfo.m_strProxyUser+(CString)":"+g_ProxyInfo.m_strProxyPass;
			CString base64str;
			if(!Base64Encode(userpass,base64str))
			{
				m_sError = _T("代理Base64编码错误!");
				SafeCloseSocket();
				return FALSE;
			}
			str+="Proxy-Authorization:Basic "+base64str+"\r\n\r\n";
			strcpy(buff,str);			
		}
		ClientSock.Send(buff,strlen(buff));	//发送请求,不能加1,因为对于Squid代理,很严格
		memset(buff,0,600);
		if(!ReceiveBytes(&ClientSock,buff,36))
		{
			if(g_ProxyInfo.bTdxProxy)
				m_sError = _T("绿色通道超时!");
			else
				m_sError = _T("代理服务器超时!");
			SafeCloseSocket();
			return FALSE;
		}
		if(strstr(buff, "Connection established") == NULL)  //连接不成功
		{
			if(g_ProxyInfo.bTdxProxy)
				m_sError = _T("通过绿色通道连接主站不成功!");
			else
				m_sError = _T("通过代理连接行情主站不成功!");
			SafeCloseSocket();
			return FALSE;
		}
		ClientSock.Receive(buff,500); //再收取多余的费数据
		ClientSock.m_bStartAsyn = TRUE;
	}
	return TRUE;
}

void CHQComm::SafeCloseSocket()
{
	CAsyncSocket* pSocket = CAsyncSocket::LookupHandle((SOCKET)ClientSock.m_hSocket, TRUE);
	if(pSocket)
		CAsyncSocket::KillSocket(ClientSock.m_hSocket,pSocket);
	closesocket(ClientSock.m_hSocket);
	ClientSock.m_hSocket=INVALID_SOCKET;
}

BOOL CHQComm::Disconnect()
{
//	if(g_pHqSrc) 
//		g_pHqSrc->KillAllHqThread();

	if(m_pThread)
	{
		SetEvent( m_pThread->m_hEndEvent );
		SetEvent( m_pThread->m_hStartEvent );
		if (::WaitForSingleObject(m_pThread->m_hThread,1000)!=WAIT_OBJECT_0 )
			::TerminateThread(m_pThread->m_hThread,0);
		TDEL(m_pThread);
	}
	SafeCloseSocket();
	if(ClientSock.m_pThread)
	{
		SetEvent((ClientSock.m_pThread)->m_hEndEvent);
		SetEvent((ClientSock.m_pThread)->m_hStartEvent);
		if (::WaitForSingleObject((ClientSock.m_pThread)->m_hThread,1000)!=WAIT_OBJECT_0)
			::TerminateThread((ClientSock.m_pThread)->m_hThread,0);
		TDEL(ClientSock.m_pThread);
	}
	m_bConnected = FALSE;
	if(LayoutFlag && m_nCommType==0)
		AfxGetMainWnd()->SendMessage(COMM_DISCONNECT);
	TDEL(m_pBlowfish);
	return TRUE;
}

int CHQComm::SendIt(void *lpBuf, int nBufLen)
{
	if(!m_bConnected)	return -1;

	REQHEADER	Header;
	if(m_nCommType==1)
		Header.Info.Version	= 1;
	else
		Header.Info.Version	= NOW_VERSION;
	Header.Info.Compressed	= 0;
	Header.Info.Encrypted	= 0;
	Header.Info.Reserved	= 0;
	Header.MainID			= g_nMainID;
	Header.AssisID			= g_nAssisID;
	Header.Priority			= g_nPriority;
	Header.PacketLen		= nBufLen;
	Header.RawLen			= nBufLen;

	m_TopSection.Lock();
	switch(GetReqType(g_nAssisID))
	{
	case BUFFER_REQTYPE:	//请求包缓冲
		if ( m_SendTop>=MAX_SENDBUF_NUM )
		{
			m_TopSection.Unlock();
			return 0;
		}
		m_SendBuf[m_SendTop].ReqHeader = Header;
		memcpy(&(m_SendBuf[m_SendTop].ReqBuffer),lpBuf,nBufLen);
		m_SendTop ++ ;
		break;
	case HQ_REQTYPE:		//行情版面请求包
		m_HQSendBuf.ReqHeader = Header;
		memcpy(&(m_HQSendBuf.ReqBuffer),lpBuf,nBufLen);
		if(g_nPriority == 2)//如果是刷新行情请求，则不缓冲
		{
			if(!m_bDirectEven)
			{
				m_TopSection.Unlock();
				return -1;
			}
			if(SendOneBuf((char *)&m_HQSendBuf,sizeof(REQHEADER)+nBufLen) == -1)
			{
				m_TopSection.Unlock();
				return -1;
			}
			m_bDirectSendNum += 1;
			m_bDirectEven = FALSE;
		}
		else
		{
			//如果发送还未响应的请求太多，则丢弃
			if(m_bDirectSendNum >= MAX_HQ_PILE_NUM)
			{
				m_TopSection.Unlock();
				return -1;
			}
			if(m_HQBufFlag == 0)
			{
				if(SendOneBuf((char *)&m_HQSendBuf,sizeof(REQHEADER)+nBufLen) == -1)
				{
					m_TopSection.Unlock();
					return -1;
				}
				m_bDirectSendNum += 1;
				m_bDirectEven = FALSE;
				m_HQBufFlag = 1;
			}
			else if(m_HQBufFlag == 1)
				m_HQBufFlag = 2;
		}
		break;
	case DIRECT_REQTYPE:	//直接发送
	default:				//前一个请求应答后才发送
		{
			/*
			if(g_nPriority == 2 && !m_bDirectEven)
			{
				m_TopSection.Unlock();
				return -1;
			}
			*/
			//如果发送还未响应的请求太多，则丢弃
			if(m_bDirectSendNum >= MAX_DIRECT_PILE_NUM)
			{
				m_TopSection.Unlock();
				return -1;
			}
			REQBUFFER SendBuf;
			SendBuf.ReqHeader = Header;
			memcpy(&SendBuf.ReqBuffer,lpBuf,nBufLen);
			if(SendOneBuf((char *)&SendBuf,sizeof(REQHEADER)+nBufLen) == -1)
			{
				m_TopSection.Unlock();
				return -1;
			}
			m_bDirectSendNum += 1;
			m_bDirectEven = FALSE;
		}
		break;
	}
	m_TopSection.Unlock();
	InterlockedExchangeAdd(&g_nNetOutCount, nBufLen);
	return 0;
}

int CHQComm::SendOneBuf(char *buf,int len)
{
	return SendToSocket(&ClientSock,buf,len);
}

//发送线程调用的函数
int CHQComm::SendBuf()
{
	if(!m_bConnected)	return -1;

	m_TopSection.Lock();
	if ( m_SendTop>0 )
	{
		m_EvenSection.Lock();
		if(SendOneBuf((char *)&m_SendBuf[0],sizeof(REQHEADER)+m_SendBuf[0].ReqHeader.PacketLen) == -1)
		{
			m_EvenSection.Unlock();
			m_TopSection.Unlock();
			return -1;
		}
		//组内按顺序发送请求,由上一次回来的包进行推出,而不是由定时器驱动
		if(GetReqType(m_SendBuf[0].ReqHeader.AssisID) == BUFFER_REQTYPE)
			m_bBuffEven = FALSE;
		m_EvenSection.Unlock();
		memmove(m_SendBuf,m_SendBuf+1,(m_SendTop-1)*sizeof(REQBUFFER));
		m_SendTop--;
	}
	m_TopSection.Unlock();
	return 0;
}

void CHQComm::SetSendTop(int value)
{
	m_TopSection.Lock();
	m_SendTop = value;
	m_TopSection.Unlock();
}

int CHQComm::GetSendTop()
{
	return m_SendTop;
}

void CHQComm::SetSendTimeCnt(int nTimeCnt)
{
	m_nTimeCnt = nTimeCnt;
}

DWORD CHQComm::GetSendTimeCnt()
{
	return m_nTimeCnt;
}

BOOL CHQComm::ConnectTdxProxy()
{
	ClientSock.m_bStartAsyn = FALSE;
	char buff[600];
	memset(buff,0,600*sizeof(char));
	sprintf(buff, "%s%s:%d%s","CONNECT ",GetServerHostName(),GetPort()," HTTP/1.0\r\nUser-Agent: NodeW/0.1\r\n\r\n");
	ClientSock.Send(buff,strlen(buff)+1);	//发送请求
	memset(buff,0,600);
	if(!ReceiveBytes(&ClientSock,buff,39))
	{
		m_sError = _T("绿色通道超时!");
		SafeCloseSocket();
		return FALSE;
	}
	if(strstr(buff, "Connection established") == NULL)  //连接不成功
	{
		m_sError = _T("通过绿色通道连接主站不成功!");
		SafeCloseSocket();
		return FALSE;
	} 
	ClientSock.Receive(buff,500); //再收取多余的费数据
	ClientSock.m_bStartAsyn = TRUE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
//		HQComm_NLB类 (全为静态成员)
//
/////////////////////////////////////////////////////////////////////////////////////////////////

char CHQComm_NLB::NLB_ErrStr[100] = {0};
SOCKET	CHQComm_NLB::NLB_allsock[150] = {INVALID_SOCKET};

int CHQComm_NLB::NLB_ReqInfo(const char *hostname,UINT hostport,struct NLBInfo *pNLBInfo,int nIndex,BOOL bDS)
{
	if(nIndex<0||nIndex>=150) return 0;
	long tick1 = GetTickCount();
	SOCKET		&m_sock = NLB_allsock[nIndex];
	if(!DirectConnect(m_sock,g_ProxyInfo,hostname,hostport,tick1,NLB_ErrStr))
		return 0;
	tick1 = GetTickCount();
	//开始收发数据
	char sockbuff[ANSBUFFER_LEN+20];
	REQHEADER	Header;
	memset(&Header,0,sizeof(REQHEADER));
	Header.Info.Version		= NOW_VERSION;
	Header.Priority			= 0;
	Header.PacketLen = Header.RawLen = sizeof(short);
	short req = OTHERINFO_REQ;
	if(bDS)
	{
		Header.Info.Version = 1;
		req = DS_OTHERINFO_REQ;
	}
	memcpy(sockbuff,&Header,sizeof(REQHEADER));
	memcpy(sockbuff+sizeof(REQHEADER),&req,sizeof(short));
	if(SendToSocket(m_sock,sockbuff,sizeof(REQHEADER)+sizeof(short))==-1)
	{
		strcpy(NLB_ErrStr,"发送数据失败!");
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		Sleep(1000);
		return 0;
	}
	if(bDS)
	{
		if(sizeof(ds_otherinfo_ans)!=ReceiveNLBData(m_sock,sockbuff))
		{
			strcpy(NLB_ErrStr,"接收数据失败!");
			closesocket(m_sock);
			m_sock = INVALID_SOCKET;
			Sleep(1000);
			return 0;
		}
		struct ds_otherinfo_ans *pAnsBody = (struct ds_otherinfo_ans *)sockbuff;
		pNLBInfo->ClientNum = pAnsBody->ClientNum;
		pNLBInfo->MaxConnectNum = pAnsBody->MaxConnectNum;
		pNLBInfo->Diff_TickCnt = GetTickCount()-tick1;
	}
	else
	{
		if(sizeof(otherinfo_ans)!=ReceiveNLBData(m_sock,sockbuff))
		{
			strcpy(NLB_ErrStr,"接收数据失败!");
			closesocket(m_sock);
			m_sock = INVALID_SOCKET;
			Sleep(1000);
			return 0;
		}
		struct otherinfo_ans *pAnsBody = (struct otherinfo_ans *)sockbuff;
		pNLBInfo->ClientNum = pAnsBody->ClientNum;
		pNLBInfo->MaxConnectNum = pAnsBody->MaxConnectNum;
		pNLBInfo->Diff_TickCnt = GetTickCount()-tick1;
	}
	strcpy(NLB_ErrStr,"处理成功!");

	closesocket(m_sock);
	m_sock = INVALID_SOCKET;
	return 1;
}

DWORD	WINAPI	AfxNLBThread(LPVOID lParam)
{
	int nThread = (int)(lParam);
	return CHQComm_NLB::NLB_ReqInfo(g_HQHost.IPAddress[nThread],g_HQHost.Port[nThread],g_NLBInfo+nThread,nThread,FALSE);
}

DWORD	WINAPI	AfxVipNLBThread(LPVOID lParam)
{
	int nThread = (int)(lParam);
	return CHQComm_NLB::NLB_ReqInfo(g_VipHQHost.IPAddress[nThread],g_VipHQHost.Port[nThread],g_NLBInfo+nThread,nThread,FALSE);
}

DWORD	WINAPI	AfxLevel2NLBThread(LPVOID lParam)
{
	int nThread = (int)(lParam);
	return CHQComm_NLB::NLB_ReqInfo(g_L2HQHost.IPAddress[nThread],g_L2HQHost.Port[nThread],g_NLBInfo+nThread,nThread,FALSE);
}

DWORD	WINAPI	AfxDSNLBThread(LPVOID lParam)
{
	int nThread = (int)(lParam);
	return CHQComm_NLB::NLB_ReqInfo(g_DSHost.IPAddress[nThread],g_DSHost.Port[nThread],g_NLBInfo+nThread,nThread,TRUE);
}

DWORD	WINAPI	AfxSSDSNLBThread(LPVOID lParam)
{
	int nThread = (int)(lParam);
	return CHQComm_NLB::NLB_ReqInfo(g_SSDSHost.IPAddress[nThread],g_SSDSHost.Port[nThread],g_NLBInfo+nThread,nThread,TRUE);
}

DWORD	WINAPI	AfxWTNLBThread(LPVOID lParam)
{
	int nThread = (int)(lParam);
	return CHQComm_NLB::NLB_WTReqInfo(g_WTHost.IPAddress[nThread],g_WTHost.Port[nThread],g_NLBInfo+nThread,nThread);
}

int CHQComm_NLB::NLB_GetMinweight(int flag)
{
	TDXWHOST *pHost=&g_HQHost;
	if(flag == L2HQ_NLB)
		pHost=&g_L2HQHost;
	else if(flag == DSHQ_NLB)
		pHost=&g_DSHost;
	else if(flag == VIPHQ_NLB)
		pHost=&g_VipHQHost;
	else if(flag == SSDSHQ_NLB)
		pHost=&g_SSDSHost;
	int i,j,nRet = 0;
	short *pCanFlag = new short[pHost->HostNum];
	memset(pCanFlag, 0, pHost->HostNum*sizeof(short));
	HANDLE m_hNLBThreads[150]={NULL};
	DWORD	TmpThreadID;
	for(i=0;i<pHost->HostNum;i++)
	{
		g_NLBInfo[i].ClientNum=-1;
		if(flag==VIPHQ_NLB)
			m_hNLBThreads[i] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)AfxVipNLBThread,(LPVOID)i,0,&TmpThreadID);
		else if(flag==DSHQ_NLB)
			m_hNLBThreads[i] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)AfxDSNLBThread,(LPVOID)i,0,&TmpThreadID);
		else if(flag==L2HQ_NLB)
			m_hNLBThreads[i] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)AfxLevel2NLBThread,(LPVOID)i,0,&TmpThreadID);
		else if(flag==SSDSHQ_NLB)
			m_hNLBThreads[i] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)AfxSSDSNLBThread,(LPVOID)i,0,&TmpThreadID);
		else
			m_hNLBThreads[i] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)AfxNLBThread,(LPVOID)i,0,&TmpThreadID);
		Sleep(1);
	}
	vector<HANDLE> vWait;
	for(i=0;i<pHost->HostNum;i++)
	{
		if(m_hNLBThreads[i]!=NULL)
			vWait.push_back(m_hNLBThreads[i]);
	}
	int nNLBAck = 0;
	int waitAck=min(5,(pHost->HostNum+1)/2);
	while (vWait.size()>0&&nNLBAck<waitAck)
	{
		DWORD nRet = WaitForMultipleObjects(vWait.size(), &vWait[0], FALSE, 6000);			//每次等6s超时
		if(nRet==WAIT_TIMEOUT||nRet<WAIT_OBJECT_0||nRet>WAIT_OBJECT_0+pHost->HostNum-1) break;
		int nWaitXh = nRet - WAIT_OBJECT_0;
		int nThreadXh = -1;
		for(i=0;i<150;i++)
		{
			if(vWait[nWaitXh]&&vWait[nWaitXh]==m_hNLBThreads[i])
			{
				nThreadXh = i;
				break;
			}
		}
		if(nThreadXh<0) 
		{
			++nNLBAck;
			continue;
		}
		DWORD nExitCode;
		BOOL bGetCode = GetExitCodeThread(m_hNLBThreads[nThreadXh], &nExitCode);
		if(bGetCode&&STILL_ACTIVE!=nExitCode)
		{
			if(1!=nExitCode||g_NLBInfo[nThreadXh].ClientNum<0)
				pCanFlag[nThreadXh]=0;
			else pCanFlag[nThreadXh]=1;
			
			CloseHandle(m_hNLBThreads[nThreadXh]);
			m_hNLBThreads[nThreadXh] = NULL;

			vWait.clear();
			for(i=0;i<pHost->HostNum;i++)
			{
				if(m_hNLBThreads[i]!=NULL)
					vWait.push_back(m_hNLBThreads[i]);
			}
		}
		++nNLBAck;
	}
	for(i=0;i<pHost->HostNum;i++)
	{
		CHQComm_NLB::CloseThreadSocket(i);
		if(m_hNLBThreads[i]==NULL) continue;
		DWORD nExitCode;
		BOOL bGetCode = GetExitCodeThread(m_hNLBThreads[i], &nExitCode);
		if(!bGetCode||STILL_ACTIVE==nExitCode)
		{
			if (::WaitForSingleObject(m_hNLBThreads[i],300)!=WAIT_OBJECT_0 )
			{
				//不能强制去掉线程，以免造成半连接(三次握手没有完成)
				//::TerminateThread(m_hNLBThreads[i], 0);
			}
		}
		else if(bGetCode)
		{
			if(1!=nExitCode||g_NLBInfo[i].ClientNum<0)
				pCanFlag[i]=0;
			else pCanFlag[i]=1;
		}
		CloseHandle(m_hNLBThreads[i]);
		m_hNLBThreads[i] = NULL;
	}
	//找出负载最轻的主站
	long tmpWeight=0,tmpClientRatio=0,tmpZongHe=0;
	long Which_Minweight=-1,MinZongHe=99999999;
	double tmpqz=0;
	for(j=0;j < pHost->HostNum;j++)
	{
		if(pCanFlag[j]==0) continue;
		tmpWeight = g_NLBInfo[j].Diff_TickCnt;
		tmpClientRatio = g_NLBInfo[j].ClientNum*100/g_NLBInfo[j].MaxConnectNum;
		if(tmpClientRatio>90)
			tmpZongHe = long(tmpWeight/10.0*7+tmpClientRatio*3);
		else if(tmpClientRatio>80)
			tmpZongHe = long(tmpWeight/10.0*8+tmpClientRatio*2);
		else
			tmpZongHe = long(tmpWeight/10.0*9+tmpClientRatio);
		tmpqz=(tmpZongHe*(100-pHost->weight[j]))/100.0;
		tmpZongHe=(long)tmpqz;
		if(tmpZongHe < MinZongHe)
		{								
			MinZongHe = tmpZongHe;
			Which_Minweight = j;
		}
	}
	TDEL(pCanFlag);
	return Which_Minweight;
}

void CHQComm_NLB::CloseThreadSocket(int nIndex)
{
	if(NLB_allsock[nIndex]!=INVALID_SOCKET)
	{
		closesocket(NLB_allsock[nIndex]);
		NLB_allsock[nIndex] = INVALID_SOCKET;
	}
}

int CHQComm_NLB::ReceiveNLBData(SOCKET m_sock,char *pReceive)
{
	ANSHEADER	Header;
	int		nBytesRead;
	DWORD	nBytes = 0;
	short	ansheadlen = sizeof(ANSHEADER);

	fd_set fdR;
	struct timeval timeout;
	timeout.tv_sec=5;
	timeout.tv_usec=0;
	
	while(1)
	{	
		//采用select方式来判断超时
		FD_ZERO(&fdR); 
		FD_SET(m_sock,&fdR);
		int nSelRet=select(m_sock+1,&fdR,NULL,NULL,&timeout);
		if(nSelRet!=1||!FD_ISSET(m_sock,&fdR))
			return 0;		
		nBytesRead = recv(m_sock,pReceive+nBytes,ANSBUFFER_LEN+20-nBytes,0);
		if(nBytesRead<=0)
			return 0;
		nBytes+=nBytesRead;
		if(nBytes < ansheadlen)//一直读完
			continue;
		memcpy(&Header,pReceive,ansheadlen);
		if(nBytes < ansheadlen+Header.PacketLen)
			continue;
		memcpy(pReceive,pReceive+ansheadlen,Header.PacketLen);
		//解压
		if (Header.Info.Compressed)
		{
			DWORD nRawLen = Header.RawLen;
			char  * pRawBuf = new char[nRawLen];
			if (!pRawBuf)	return 0;
			if(!uncompress((BYTE *)pRawBuf,&nRawLen,(BYTE *)pReceive,Header.PacketLen))
			{
				memcpy(pReceive,pRawBuf,nRawLen);
				Header.RawLen = nRawLen;
			}
			delete pRawBuf;
		}
		return Header.RawLen;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//	委托相关的测试
//////////////////////////////////////////////////////////////////////////

typedef struct tagWTREQHEADER
{	struct tagINFO
	{	unsigned char Version:4;				// 协议版本
		unsigned char Compressed:1;				// 是否压缩
		unsigned char EncryptType:3;			// 加密类型		
	} Info;
	unsigned long  TransKey;					// 请求唯一标识
	unsigned char  PlusHeaderLen;				// 扩充请求包头长度(老协议为优先级字段,恒为0)
	unsigned short PacketLen;					// 包长度(数据体,不包含包头和可扩展包头)
	unsigned short RawLen;						// 包的原始长度(数据体,不包含包头和可扩展包头)
	unsigned short ReqNo;						// 请求序号
} WTREQHEADER;

// 应答头信息,标准应答信息
typedef struct tagANSHEADER
{	long Separator;								// 分割数字,用于拆分应答包处理,正常包头必须是SEPARATOR_NUM
	struct tagINFO
	{	unsigned char Version:4;				// 协议版本
		unsigned char Compressed:1;				// 是否压缩
		unsigned char EncryptType:3;			// 加密类型
	} Info;
	unsigned long  TransKey;					// 请求唯一标识
	unsigned char  SeparatorOn:1;				// 保留字节(恒为0,原优先级字段)
	unsigned char  Reserved:7;					// 保留位,全空
	unsigned short ReqNo;						// 请求号
	unsigned short PacketLen;					// 包长度(数据体,不包含包头和可扩展包头)
	unsigned short RawLen;						// 原始包长度(数据体,不包含包头和可扩展包头)
} WTANSHEADER;

struct tc50_get_loader_req
{	WORD		m_wReserved;						// (废弃,必须为空)通达信营业部ID
};													// 5.10sp7前的版本未检查客户端入参会导致崩溃,干脆送空

struct tc50_get_loader_ans
{	DWORD		m_dwNowConnectNum;					// 当前在线人数(含管理和监控连接)
	DWORD		m_dwMaxConnectNum;					// 最大允许在线人数(含管理和监控连接)
	DWORD		m_dwReserved;						// (废弃,为空),负载(如果送入的通达信营业部ID非空则返回对应营业部的负载)
};

const int  TC50_GET_LOADER			= 3000;			// 读取服务器负载状况

int CHQComm_NLB::ReceiveWTNLBData(SOCKET m_sock,char *pReceive)
{
	WTANSHEADER	Header;
	int		nBytesRead;
	DWORD	nBytes = 0;
	short	ansheadlen = sizeof(WTANSHEADER);

	fd_set fdR;
	struct timeval timeout;
	timeout.tv_sec=5;
	timeout.tv_usec=0;
	
	time_t   t; 
	t = time ( NULL );
	while(1)
	{	
		if(time ( NULL ) - t >= 10)
			return 0;

		//采用select方式来判断超时
		FD_ZERO(&fdR); 
		FD_SET(m_sock,&fdR);
		if(select(m_sock+1,&fdR,NULL,NULL,&timeout)!=1)
			return 0;
		
		nBytesRead = recv(m_sock,pReceive+nBytes,ANSBUFFER_LEN+20-nBytes,0);
		if(nBytesRead<=0)
			return 0;
		nBytes+=nBytesRead;
		if(nBytes < ansheadlen)//一直读完
			continue;
		memcpy(&Header,pReceive,ansheadlen);
		if(nBytes < ansheadlen+Header.PacketLen)
			continue;
		memcpy(pReceive,pReceive+ansheadlen,Header.PacketLen);
		//解压
		if (Header.Info.Compressed)
		{
			DWORD nRawLen = Header.RawLen;
			char  * pRawBuf = new char[nRawLen];
			if (!pRawBuf)	return 0;
			if(!uncompress((BYTE *)pRawBuf,&nRawLen,(BYTE *)pReceive,Header.PacketLen))
			{
				memcpy(pReceive,pRawBuf,nRawLen);
				Header.RawLen = nRawLen;
			}
			delete pRawBuf;
		}
		return Header.RawLen;
	}
	return 0;
}

int CHQComm_NLB::NLB_WTReqInfo(const char *hostname,UINT hostport,struct NLBInfo *pNLBInfo,int nIndex)
{
	if(nIndex<0||nIndex>=150) return 0;
	long tick1 = GetTickCount();
	SOCKET		&m_sock = NLB_allsock[nIndex];
	if(!DirectConnect(m_sock,g_ProxyInfo,hostname,hostport,tick1,NLB_ErrStr))
		return 0;
	tick1 = GetTickCount();
	//开始收发数据
	char sockbuff[ANSBUFFER_LEN+20];
	WTREQHEADER	Header;
	memset(&Header,0,sizeof(WTREQHEADER));
	Header.Info.Version	= 14;
	Header.ReqNo = TC50_GET_LOADER;
	Header.PacketLen = Header.RawLen = sizeof(WORD);
	memcpy(sockbuff,&Header,sizeof(WTREQHEADER));
	struct tc50_get_loader_req	*pReq = (struct tc50_get_loader_req *)(sockbuff+sizeof(WTREQHEADER));
	pReq->m_wReserved=0;
	int reqlen=sizeof(struct tc50_get_loader_req);
	if(SendToSocket(m_sock,sockbuff,sizeof(WTREQHEADER)+reqlen)==-1)
	{
		strcpy(NLB_ErrStr,"发送数据失败!");
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		Sleep(1000);
		return 0;
	}
	if(sizeof(tc50_get_loader_ans)!=ReceiveWTNLBData(m_sock,sockbuff))
	{
		strcpy(NLB_ErrStr,"接收数据失败!");
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
		Sleep(1000);
		return 0;
	}
	struct tc50_get_loader_ans		  *pAnsBody  = (struct tc50_get_loader_ans *)sockbuff;
	pNLBInfo->ClientNum = pAnsBody->m_dwNowConnectNum;
	pNLBInfo->MaxConnectNum = pAnsBody->m_dwMaxConnectNum;
	pNLBInfo->Diff_TickCnt = GetTickCount()-tick1;
	strcpy(NLB_ErrStr,"处理成功!");

	closesocket(m_sock);
	m_sock = INVALID_SOCKET;
	return 1;
}

//////////////////////////////////////////////////////////////////////////

char* CHQComm_NLB::NLB_GetLastErr()
{
	return NLB_ErrStr;
}

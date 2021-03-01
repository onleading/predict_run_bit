// HqSource.cpp: implementation of the CHqSource class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CBase64Coding.h"
#include "IDefine.h"
#include "HqSource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHqSource::CHqSource()
{
	m_hSocket = INVALID_SOCKET;
	m_bConnected = FALSE;
	m_lTestAlive = 0;
	m_iNo = -1;
}

CHqSource::~CHqSource()
{
	CLOSE_SOCKET(m_hSocket);
}
//////////////////////////////////////////////////////////////////////////
short	CHqSource::Connect2Host(LPSTR strError,const char *hostname,UINT hostport)
{
	if(m_bConnected) return TRUE;

	int nRcv=0;
	m_hSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(m_hSocket==INVALID_SOCKET) return 0;
	//设置无延迟属性
	BOOL bNoDelay = TRUE;
	setsockopt(m_hSocket,IPPROTO_TCP,TCP_NODELAY,(char *)&bNoDelay,sizeof(BOOL));
	//设置超时
	int nTimeOut = 8000;
	setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&nTimeOut, sizeof(int));
	setsockopt(m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&nTimeOut, sizeof(int));
	//开始连接
	struct	sockaddr_in	m_remote;
	m_remote.sin_family = AF_INET;
	if(!g_ProxyInfo.m_bProxy)				//无代理
	{
		m_remote.sin_port = htons(hostport);
		m_remote.sin_addr.s_addr = inet_addr(hostname);
		if(m_remote.sin_addr.s_addr == INADDR_NONE)	//如果是域名
		{
			LPHOSTENT lphost;
			lphost = gethostbyname(hostname);
			if(lphost)
				m_remote.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		}
	//	if(-1==connect(m_hSocket,(sockaddr*)&m_remote,sizeof(m_remote)))
	//		return 0;
		//开始异步连接
		BOOL bConnectSuc=FALSE;
		fd_set fs;
		timeval tv;
		u_long on=1;
		int maxfileh=0;
		if(ioctlsocket(m_hSocket,FIONBIO,&on)!=-1)
		{
			int nConRet = connect(m_hSocket,(sockaddr*)&m_remote,sizeof(m_remote));
			if(nConRet==-1 && WSAGetLastError()==WSAEWOULDBLOCK)
			{
				FD_ZERO(&fs);
				FD_SET(m_hSocket,&fs);
				tv.tv_sec=3; //3秒钟超时
				tv.tv_usec=0;
				maxfileh=m_hSocket;
				if(select(maxfileh+1,0,&fs,0,&tv)==1)
				{
					on = 0;
					int optvalue=0;
					int optlen=sizeof(int);
					getsockopt(m_hSocket,SOL_SOCKET,SO_ERROR,(char *)&optvalue,&optlen);
					if(optvalue==0 && FD_ISSET(m_hSocket,&fs)==1 && ioctlsocket(m_hSocket,FIONBIO,&on)!=-1)
						bConnectSuc=TRUE;
				}
			}
		}
		if(!bConnectSuc)
		{
			strcpy(strError,"不能连接到主站!");
			return 0;
		}
	}
	else if(g_ProxyInfo.m_nSockType == 0)	//SOCK4代理
	{
		m_remote.sin_port = htons(g_ProxyInfo.m_nProxyPort);//代理端口
		m_remote.sin_addr.s_addr = inet_addr(g_ProxyInfo.m_strProxyIP);
		if(m_remote.sin_addr.s_addr == INADDR_NONE)	//如果是域名
		{
			LPHOSTENT lphost;
			lphost = gethostbyname(g_ProxyInfo.m_strProxyIP);
			if(lphost)
				m_remote.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		}
		if(connect(m_hSocket,(sockaddr*)&m_remote,sizeof(m_remote)) == SOCKET_ERROR)
		{
			strcpy(strError,"不能连接到代理服务器!");
			return 0;
		}
		char buff[100];
		memset(buff,0,100);
		struct sock4req1 *m_proxyreq;
		m_proxyreq = (struct sock4req1 *)buff;//分配内存
		m_proxyreq->VN = 4;//SOCK4
		m_proxyreq->CD = 1;
		m_proxyreq->Port = ntohs(hostport);
		m_proxyreq->IPAddr = inet_addr(hostname);
		if(m_proxyreq->IPAddr == INADDR_NONE) //如果是域名
		{
			LPHOSTENT lphost;
			lphost = gethostbyname(hostname);
			if(lphost)
				m_proxyreq->IPAddr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
			else
			{
				strcpy(strError,"代理失败!");
				return 0;
			}
		}
		send(m_hSocket,buff,9,0);
		struct sock4ans1 *m_proxyans;
		m_proxyans = (struct sock4ans1 *)buff;//分配内存
		memset(buff,0,100);
		nRcv = recv(m_hSocket,buff,100,0);
		if(nRcv < sizeof(struct sock4ans1))
		{
			strcpy(strError,"代理服务器超时!");
			return 0;
		}
		if(m_proxyans->VN != 0 || m_proxyans->CD != 90)
		{
			strcpy(strError,"通过代理连接主站不成功!");
			closesocket(m_hSocket);
			return 0;
		}
	}
	else if(g_ProxyInfo.m_nSockType == 1)	//SOCK5代理
	{
		m_remote.sin_port = htons(g_ProxyInfo.m_nProxyPort);
		m_remote.sin_addr.s_addr = inet_addr(g_ProxyInfo.m_strProxyIP);
		if(m_remote.sin_addr.s_addr == INADDR_NONE)	//如果是域名
		{
			LPHOSTENT lphost;
			lphost = gethostbyname(g_ProxyInfo.m_strProxyIP);
			if(lphost)
				m_remote.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		}
		if(connect(m_hSocket,(sockaddr*)&m_remote,sizeof(m_remote)) == SOCKET_ERROR)
		{
			strcpy(strError,"不能连接到代理服务器!");
			return 0;
		}
		char buff[600];
		struct sock5req1 *m_proxyreq1;
		m_proxyreq1 = (struct sock5req1 *)buff;//分配内存
		m_proxyreq1->Ver = 5;//SOCK5
		m_proxyreq1->nMethods = 2;
		m_proxyreq1->Methods[0] = 0;
		m_proxyreq1->Methods[1] = 2;
		send(m_hSocket,buff,4,0);
		struct sock5ans1 *m_proxyans1;
		m_proxyans1 = (struct sock5ans1 *)buff;//分配内存
		memset(buff,0,600);
		nRcv = recv(m_hSocket,buff,600,0);
		if(nRcv < sizeof(struct sock5ans1))
		{
			strcpy(strError,"代理服务器超时!");
			return 0;
		}
		if(m_proxyans1->Ver != 5 || (m_proxyans1->Method!=0 && m_proxyans1->Method!=2))
		{
			strcpy(strError,"通过代理连接主站不成功!");
			closesocket(m_hSocket);
			return 0;
		}
		if(m_proxyans1->Method == 2)//校验用户
		{
			int nUserLen = strlen(g_ProxyInfo.m_strProxyUser);
			int nPassLen = strlen(g_ProxyInfo.m_strProxyPass);
			struct authreq *m_authreq;
			m_authreq = (struct authreq *)buff;
			m_authreq->Ver = 1;
			m_authreq->Ulen = nUserLen;
			strcpy(m_authreq->Name,g_ProxyInfo.m_strProxyUser);
			m_authreq->PLen = nPassLen;
			strcpy(m_authreq->Pass,g_ProxyInfo.m_strProxyPass);
			send(m_hSocket,buff,513,0);
			struct authans *m_authans;
			m_authans = (struct authans *)buff;
			memset(buff,0,600);
			nRcv = recv(m_hSocket,buff,600,0);
			if(nRcv < sizeof(struct authans))
			{
				strcpy(strError,"代理服务器超时!");
				return 0;
			}
			if(m_authans->Ver != 1 || m_authans->Status != 0)
			{
				strcpy(strError,"通过代理连接主站不成功!");
				closesocket(m_hSocket);
				return 0;
			}
		}
		struct sock5req2 *m_proxyreq2;
		m_proxyreq2 = (struct sock5req2 *)buff;
		m_proxyreq2->Ver = 5;
		m_proxyreq2->Cmd = 1;
		m_proxyreq2->Rsv = 0;
		m_proxyreq2->Atyp = 1;
		unsigned long tmpLong = inet_addr(hostname);
		if(tmpLong == INADDR_NONE) //如果是域名
		{
			LPHOSTENT lphost;
			lphost = gethostbyname(hostname);
			if(lphost)
				tmpLong = ((LPIN_ADDR)lphost->h_addr)->s_addr;
			else
			{
				strcpy(strError,"代理失败!");
				return 0;
			}
		}
		unsigned short port = ntohs(hostport);
		memcpy(m_proxyreq2->other,&tmpLong,4);
		memcpy(m_proxyreq2->other+4,&port,2);
		send(m_hSocket,buff,sizeof(struct sock5req2)+5,0);

		struct sock5ans2 *m_proxyans2;
		memset(buff,0,600);
		m_proxyans2 = (struct sock5ans2 *)buff;
		nRcv = recv(m_hSocket,buff,600,0);
		if(nRcv < sizeof(struct sock5ans2))
		{
			strcpy(strError,"代理服务器超时!");
			return 0;
		}
		if(m_proxyans2->Ver != 5 || m_proxyans2->Rep != 0)
		{
			strcpy(strError,"通过代理连接主站不成功!");
			closesocket(m_hSocket);
			return 0;
		}
	}
	else						//Http代理					
	{
		m_remote.sin_port = htons(g_ProxyInfo.m_nProxyPort);
		m_remote.sin_addr.s_addr = inet_addr(g_ProxyInfo.m_strProxyIP);
		if(m_remote.sin_addr.s_addr == INADDR_NONE)	//如果是域名
		{
			LPHOSTENT lphost;
			lphost = gethostbyname(g_ProxyInfo.m_strProxyIP);
			if(lphost)
				m_remote.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		}
		if(connect(m_hSocket,(sockaddr*)&m_remote,sizeof(m_remote)) == SOCKET_ERROR)
		{
			strcpy(strError,"不能连接到代理服务器!");
			return 0;
		}
		char buff[600];
		if(g_ProxyInfo.m_strProxyUser[0]==0)			
			sprintf( buff, "%s%s:%d%s","CONNECT ",g_ProxyInfo.tdxip[g_ProxyInfo.tdxprimary],g_ProxyInfo.tdxport[g_ProxyInfo.tdxprimary]," HTTP/1.1\r\nUser-Agent: Mozilla/4.0\r\n\r\n");
		else
		{
			CString str;
			str.Format("CONNECT %s:%d HTTP/1.0\r\n",g_ProxyInfo.bTdxProxy?g_ProxyInfo.tdxip[g_ProxyInfo.tdxprimary]:g_pComm->GetServerHostName(),g_ProxyInfo.bTdxProxy?g_ProxyInfo.tdxport[g_ProxyInfo.tdxprimary]:g_pComm->GetPort());
			CString userpass=g_ProxyInfo.m_strProxyUser+(CString)":"+g_ProxyInfo.m_strProxyPass;
			CString base64str;
			CByteArray array;
			for (int i=0;i<userpass.GetLength();i++)
				array.Add(userpass[i]);
			
			CBase64Coding base64coding;
			if (!base64coding.Encode(array,base64str))
			{
				strcpy(strError,"代理Base64编码错误!");
				return 0;
			}
			str+="Proxy-Authorization:Basic "+base64str+"\r\n\r\n";
			strcpy(buff,str);			
		}
		int iii = send(m_hSocket,buff,strlen(buff),0);	//发送请求
		memset(buff,0,600);
		int nRcv = recv(m_hSocket,buff,600,0);
		if(nRcv < 36)
		{
			strcpy(strError,"代理服务器超时!");
			return 0;
		}
		if(strstr(buff, "Connection established") == NULL)  //连接不成功
		{
			strcpy(strError,"通过代理连接主站不成功!");
			closesocket(m_hSocket);
			return 0;
		}
	}
	
	m_bConnected = TRUE;
	return 1;
}

void	CHqSource::NetRegister(HWND hMsgWnd, UINT MsgID, long lEvent, int iNo)
{	
	WSAAsyncSelect(m_hSocket,hMsgWnd,MsgID,lEvent);
	m_iNo = iNo;
}

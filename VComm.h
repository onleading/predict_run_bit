#ifndef VCOMM_H_
#define VCOMM_H_

#ifdef	TCOMM_EXPORT
	#define TCOMMDLL_API extern "C" __declspec(dllexport)
#else
	#define TCOMMDLL_API extern "C" __declspec(dllimport)
#endif

typedef struct
{
	BOOL	m_bProxy;
	char	m_nSockType;
	UINT	m_nProxyPort;
	char	m_strProxyIP[100];
	char	m_strProxyPass[100];
	char	m_strProxyUser[100];

	BOOL	bTdxProxy;
	char	tdxproxynum;
	char	tdxprimary;
	char	tdxdesc[10][50];
	char	tdxip[10][50];
	UINT	tdxport[10];
}PROXYINFO;


struct sock4req1
{
	char VN;
	char CD;
	unsigned short Port;
	unsigned long IPAddr;
	char other[1];
};

struct sock4ans1
{
	char VN;
	char CD;
};

struct sock5req1
{
	char Ver;
	char nMethods;
	char Methods[255];
};

struct sock5ans1
{
	char Ver;
	char Method;
};

struct sock5req2
{
	char Ver;
	char Cmd;
	char Rsv;
	char Atyp;
	char other[1];
};

struct sock5ans2
{
	char Ver;
	char Rep;
	char Rsv;
	char Atyp;
	char other[1];
};

struct authreq
{
	char Ver;
	char Ulen;
	char Name[255];
	char PLen;
	char Pass[255];
};

struct authans
{
	char Ver;
	char Status;
};

TCOMMDLL_API int SendToSocket(SOCKET sock,char *buf,int len);
TCOMMDLL_API BOOL ReceiveBytes(SOCKET sock,char *buff,short len,int timeout=15);
TCOMMDLL_API BOOL DirectConnect(SOCKET &m_sock,PROXYINFO proxy,const char *hostname,UINT hostport,long &tick1,char *err_msg/*255*/);

#endif
#ifndef VFCLIENT_H_
#define VFCLIENT_H_
/*****************************************************************
	通达信快速行情数据接口模块

	2008.04.09	支持取证券品种的基本行情信息
******************************************************************/
#pragma pack(push,1) //请务必一字节对齐

typedef struct tagFastHQ
{
	float		Close;						//	4B		前收盘价
	float		Open;						//	4B		今开盘价
	float		Max;						//	4B		最高价
	float		Min;						//	4B		最低价
	float		Now;						//	4B		现价
	DWORD		Volume;						//	4B		总手
	float		Buyp[5];					//	20B		叫买价
	DWORD		Buyv[5];					//	20B		对应叫买价的买盘	  
	float		Sellp[5];					//	20B		叫卖价
	DWORD		Sellv[5];					//	20B		对应叫卖价的卖盘
}FASTHQ;

typedef struct tagFastHQ10
{
	float		Close;						//	4B		前收盘价
	float		Open;						//	4B		今开盘价
	float		Max;						//	4B		最高价
	float		Min;						//	4B		最低价
	float		Now;						//	4B		现价
	DWORD		Volume;						//	4B		总手
	float		Buyp[10];					//	40B		叫买价
	DWORD		Buyv[10];					//	40B		对应叫买价的买盘	  
	float		Sellp[10];					//	40B		叫卖价
	DWORD		Sellv[10];					//	40B		对应叫卖价的卖盘
}FASTHQ_10;

typedef struct
{
	BOOL	bProxy;				//是否启用代理
	char	nSockType;			//代理类型:0:SOCK4代理 1:SOCK5代理 2:Http代理
	UINT	nProxyPort;			//代理服务器的端口
	char	strProxyIP[100];	//代理服务器的地址
	char	strProxyPass[100];	//代理验证用户名
	char	strProxyUser[100];	//代理验证密码
}FPROXYINFO;

typedef struct 
{	
	UINT	nPort;				//行情主站的端口
	char	strIPAddress[100];	//行情主站的地址	
}FHOSTINFO;

#define		SST_5BS			0
#define		SST_10BS		1

#pragma pack(pop)
#endif
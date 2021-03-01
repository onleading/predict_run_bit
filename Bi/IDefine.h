#ifndef	_PUBLIC_DEFINE_DATA_STRUCT_
#define	_PUBLIC_DEFINE_DATA_STRUCT_
/////////////////////////////////////////////////////////////////////////////
#define	DELETE_PTRA(Ptr)	{if(Ptr!=NULL){delete[] Ptr;Ptr=NULL;}}
#define	DELETE_PTR(Ptr)		{if(Ptr!=NULL){delete Ptr;Ptr=NULL;}}
#define	RETURN_FALSE(bRight){if(bRight)	return(FALSE);}
#define	RETURN_TRUE(bRight)	{if(bRight)	return(TRUE);}
#define	RETURN_NULL(bRight)	{if(bRight)	return(NULL);}
#define	RETURN_VALUE(bRight,Value) {if(bRight) return(Value);}
#define	CLOSE_HANDLE(hHandle) {if(hHandle!=NULL){CloseHandle(hHandle);hHandle=NULL;}}
#define	CLOSE_FILE(hFile) {if(hFile!=INVALID_HANDLE_VALUE){CloseHandle(hFile);hFile=INVALID_HANDLE_VALUE;}}
#define	CLOSE_SOCKET(hSocket)	{if(hSocket!=INVALID_SOCKET){closesocket(hSocket);hSocket=INVALID_SOCKET;}}
//#define COMPPREC		0.00001f   //Added by Tian 精度宏
//////////////////////////////////////////////////////////////////////////
#define	DATA_TDXSTD			0x0001		//通达信通用行情
#define DATA_TWIN			0x0002		//同类主站行情源
#define DATA_GZQH			0x0004		//中金所标准期货股指数据接口
#define DATA_KSQH			0x0008		//金仕达期货股指数据接口
#define	DATA_EXCW			0x0010		//财华港股
#define DATA_MDF			0x0020		//MDF港股行情
#define DATA_HSQH			0x0040		//恒生期货
#define DATA_GFQH			0x0080		//广发期货
#define	DATA_GDSY			0x0100		//固定收益
#define DATA_HOSTA			0x0200		//A股主站数据
#define	DATA_LAST			0x8000		
//////////////////////////////////////////////////////////////////////////
#define MAX_CODELEN			7
#define MAX_NAMELEN			13
#define CALC_PERIODA		PER_MIN30
#define CALC_PERIODB		PER_MIN1
/////////////////////////////////////////////////////////////////////////////
const	MAX_VIEWABLE_NUM = 35;
const	MAX_MARKET_NUM = 64;
const	MAX_STOCK_NUM = 10000;
const	MAX_MINUTE_NUM = 4000*250;
const	BASE_PACK_SIZE = 256*1024;
const	MAX_TCP_SIZE = 8*1024;
const	MAX_UDP_SIZE = 4*1024;
const	MAX_PROC_SIZE = 8*1024;
const	MAX_COOP_NUM = 256;
const	MAX_CACHE_NUM= 4*1024;
/////////////////////////////////////////////////////////////////////////////
const	g_iVersion		=	0x0001;
const	MAX_THREAD		=	256L;
const	MAX_BUF_SIZE	=	0x100000;
const	MAX_PUSH_PERCLIE=	16;
const	MAX_PUSH_PERSTK	=	3000;
/////////////////////////////////////////////////////////////////////////////
const	MAX_CHAR_PACK = 21;
const	SPELL_SIZE	=	9;
/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
struct	PackBuf
{
	long nSpaceLen;
	long nBufLen;
	BYTE *pPackBuf;
};
#define UM_ITEMEDITDONE			WM_USER+2004
/////////////////////////////////////////////////////////////////////////////
typedef	struct	tagConfig
{
	//数据源
	char	HomePath[MAX_PATH];		//	工作目录
	char	RegedSource[MAX_PATH];	//	注册的源
	//发布相关
	WORD	ListenPort;				//	监听端口
	long	SockHeap;				//	网络堆大小
	BYTE	SrvDirect;				//	客户服务类型,0只是提供应答,1只提供推送，2都提供
	WORD	ThreadNum;				//	线程数目
	WORD	MaxClient;				//	最大允许连接数
	long	CheckTimer;				//	存活包校验时间[秒]
	WORD	MaxSameIP;				//	同一IP最大允许连接数

	BYTE	bCompress;				//	行情数据是否压缩
	char	EncryptKey[40];			//	行情加密Key

	long	CriticalMin;			//  关键时间点
	
	BOOL	bDataLog;				//	对收到的数据详细记录
}CONFIG,*PCONFIG;
/////////////////////////////////////////////////////////////////////////////
typedef	struct	tagIClient
{
	DWORD		hClientID;		//	客户号
	SOCKET		hSocket;
	DWORD		IPAddr;			//	IP地址
	WORD		IPPort;			//	IP端口
	char		UserRight;		//	用户权限
	char		UserType;		//	用户类型
	char		username[50];	//	用户名
	char		userpass[20];	//	密码
	time_t		loginTime;		//	登陆时间
	long		nPacketNum;		//	发送的包数
}ICLIENT,*LPICLIENT;

/////////////////////////////////////////////////////////////////////////////
#define MAX_USERINFO	8000
#define MAX_VIPUSER		1000
#define MAX_SEND_BUFFER	16*1024
#define MAX_BUFFER		256*1024

typedef struct tagLoginUserInfo
{
	char	username[50];	//	用户名
	short	loginCnt;
}LOGINUSERINFO,*LPLOGINUSERINFO;

typedef struct tagVipUserInfo
{
	char	username[50];	//	用户名
	char	userpass[20];
	char	UserRight;
	char	UserType;
	short	permitCnt;
}VIPUSERINFO,*LPVIPUSERINFO;
//////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
enum
{
	UM_RECV_DATA = WM_USER+0x1101,
	UM_KILLTHREAD,
	UM_RECV_CODELIST_DATA,
	RE_SHOW_DATA,
	UM_RECV_TDXHQ,
	UM_RECV_TWINHQ,
	UM_RECV_TDXSTDATA,
	UM_RECV_EXICW_DATA,
	UM_RECV_GFQH_DATA,
	UM_RECV_HSQH_DATA,
	UM_RECV_TDXA_DATA,
	UM_RECV_HOSTDATA,
	UM_RECV_MDF_DATA,
	UM_ASYN_SQL_MSG,
};
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	char code[CODE_SIZE];
	char name[NAME_SIZE];
}Code2CodePack;

struct cmpstrs
{
  bool operator()(const Code2CodePack s1, const Code2CodePack s2) const
  {
    return strcmp(s1.code, s2.code) < 0;
  }
};

typedef	struct	tagCoopUrl
{
	char	strUrl[32];
	WORD	wPort;
}COOPURL,*PCOOPURL;

struct  ProcCacheHead
{
	char sCode[CODE_SIZE];
	char sName[NAME_SIZE];
	char sMarket[8];
	DWORD Date;
	DWORD Time;
	DWORD	Reserved[1];
};
/////////////////////////////////////////////////////////////////////////////
struct LogoReqFile
{
	char Desc[40];
	char ReqFileName[40];
	char LocalSaveFileName[40];
};

//////////////////////////////////////////////////////////////////////////
#endif

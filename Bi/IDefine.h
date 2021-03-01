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
//#define COMPPREC		0.00001f   //Added by Tian ���Ⱥ�
//////////////////////////////////////////////////////////////////////////
#define	DATA_TDXSTD			0x0001		//ͨ����ͨ������
#define DATA_TWIN			0x0002		//ͬ����վ����Դ
#define DATA_GZQH			0x0004		//�н�����׼�ڻ���ָ���ݽӿ�
#define DATA_KSQH			0x0008		//���˴��ڻ���ָ���ݽӿ�
#define	DATA_EXCW			0x0010		//�ƻ��۹�
#define DATA_MDF			0x0020		//MDF�۹�����
#define DATA_HSQH			0x0040		//�����ڻ�
#define DATA_GFQH			0x0080		//�㷢�ڻ�
#define	DATA_GDSY			0x0100		//�̶�����
#define DATA_HOSTA			0x0200		//A����վ����
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
	//����Դ
	char	HomePath[MAX_PATH];		//	����Ŀ¼
	char	RegedSource[MAX_PATH];	//	ע���Դ
	//�������
	WORD	ListenPort;				//	�����˿�
	long	SockHeap;				//	����Ѵ�С
	BYTE	SrvDirect;				//	�ͻ���������,0ֻ���ṩӦ��,1ֻ�ṩ���ͣ�2���ṩ
	WORD	ThreadNum;				//	�߳���Ŀ
	WORD	MaxClient;				//	�������������
	long	CheckTimer;				//	����У��ʱ��[��]
	WORD	MaxSameIP;				//	ͬһIP�������������

	BYTE	bCompress;				//	���������Ƿ�ѹ��
	char	EncryptKey[40];			//	�������Key

	long	CriticalMin;			//  �ؼ�ʱ���
	
	BOOL	bDataLog;				//	���յ���������ϸ��¼
}CONFIG,*PCONFIG;
/////////////////////////////////////////////////////////////////////////////
typedef	struct	tagIClient
{
	DWORD		hClientID;		//	�ͻ���
	SOCKET		hSocket;
	DWORD		IPAddr;			//	IP��ַ
	WORD		IPPort;			//	IP�˿�
	char		UserRight;		//	�û�Ȩ��
	char		UserType;		//	�û�����
	char		username[50];	//	�û���
	char		userpass[20];	//	����
	time_t		loginTime;		//	��½ʱ��
	long		nPacketNum;		//	���͵İ���
}ICLIENT,*LPICLIENT;

/////////////////////////////////////////////////////////////////////////////
#define MAX_USERINFO	8000
#define MAX_VIPUSER		1000
#define MAX_SEND_BUFFER	16*1024
#define MAX_BUFFER		256*1024

typedef struct tagLoginUserInfo
{
	char	username[50];	//	�û���
	short	loginCnt;
}LOGINUSERINFO,*LPLOGINUSERINFO;

typedef struct tagVipUserInfo
{
	char	username[50];	//	�û���
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

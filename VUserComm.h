#ifndef VUSERCOMM_H_
#define VUSERCOMM_H_
#pragma pack(1)

#ifdef VUSERCOMM_EXPORT 
	#define VUSERCOMM_PREFIX extern "C" _declspec(dllexport)
#else
	#define VUSERCOMM_PREFIX extern "C" _declspec(dllimport)
#endif

struct WriteUrgentInfo
{
	char  	userright;
	char  	username[20];
	long 	ldate;
	char 	source[31];
	char 	buf[6006];	
};

//紧急公告
struct Cfg_Struct
{
	char NotifyStr[91];
	char UrgentSource[31];
	long EndTime;
	char UrgentContent[2049];
};

#define MAX_SENDFILELEN 5800
#define MAX_LISTBUFLEN	6003

class VUserComm
{
public:
	virtual void	InitUserComm(BOOL* pbUserHostEverConnect,BOOL *pbJRSConnected,char *chMngRight,char *chUserType,
					char* strUserHostQSID,char* strUserType,char* strUserCharacter,char* strUserLimits,BOOL bEmbedWTVersion) = 0;
	virtual void    SetServerProperties(PROXYINFO* lpProxy, LPCTSTR sServerHostName, UINT nPort, LPCTSTR sBakServerHostName, UINT nBakPort,LPCTSTR sBakServerHostName2, UINT nBakPort2) = 0;
		
	virtual void	Disconnect() = 0;
	virtual BOOL	Connect(LPCTSTR sServerHostName, UINT nPort) = 0;
	virtual BOOL	ConnectUserHost() = 0;
	
	virtual void	SetProductInfo(char productnbbs[10],BOOL bIsSHLevel2,BOOL bIsSZLevel2) = 0;
	virtual int		RegUser_Login(const char *username,const char *password,const char* szMainExeVersion,int &RetOK,long &lExpireDate,long &nRepeat,BOOL bSpec = FALSE) = 0;
	virtual int		TestZjzh2(const char *yybinfo,long wttype,const char *zh,const char *password,int &RetOK,char *zjzh) = 0;
	virtual int		JyUser_Login(const char *yybinfo,long wttype,const char *zh,const char *password,int &RetOK) = 0;
	virtual int		RegisterUser(short registertype,const char *yybinfo,const char *zjzh,const char *username,const char *password,char ForceReg,int &RetOK) = 0;
	virtual int		SelUser(const char *yybinfo,const char *zjzh,const char *username,char *RetPwd,int &RetOK) = 0;
	virtual int		ChangePwd(BOOL bDelMySelf,const char *username,const char *oldpass,const char *newpass,int &RetOK) = 0;
	virtual BOOL	LivePacket(long &urgentnum,long &unused1,long &nRepeat) = 0;
	
	virtual void	GetLoginAndLiveSccessNum(long &LoginCnt,long &LiveCnt) = 0;
	virtual void	GetLoginMagicChar(char &char1,char &char2) = 0;
	virtual void	GetLoginMagicShort(short &short1,short &short2) = 0;
	virtual void	GetRelaInfo(char *str_RelaNo,char *str_RelaPwd,char *str_RelaOther) = 0;

	virtual BOOL	SendUrgent(const char* user,const char* pwd,struct WriteUrgentInfo *pInfo) = 0;
	virtual BOOL	GetUrgent(char userright,char *username,CString csWSXHStr,long urgentnum) = 0;
	
	virtual int		GetFileList(const char* user,const char * pwd,char* filelist) = 0;

	//返回发送的数据长度，如果为－1，出错
	virtual int		SendFile(const char* user,const char* pwd,const char *itemname,const char* filepath,const char *filename) = 0;
	//返回发送的数据长度，参数nlen为此次发送之前已经发送的所有数据长度
	virtual int		SendFile(const char* user,const char *itemname,const char* filepath,int nlen) = 0;
	//外部确定路径filepath有效
	virtual int		GetFile(const char* user,const char* pwd,const char* filename,const char* filepath,int nlen = 0) = 0;
	
public:
	virtual CString GetErrorStr() = 0;
	virtual void	SetErrorStr(CString strError) = 0;
	virtual int		GetUserNum() = 0;
	
	char		m_User[5][50];	
	char		m_Zjzh[20];

};

VUSERCOMM_PREFIX VUserComm* NewUserCommModule();
VUSERCOMM_PREFIX void DelUserCommModule(VUserComm * p);
#pragma pack()
#endif
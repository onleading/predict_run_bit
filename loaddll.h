#ifndef LOADDLL_H_
#define LOADDLL_H_

#include "jy.h"
#include "levin.h"
#include "VFClient.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////
//金融工具的使用的接口
typedef long(CALLBACK * PGPINFOHQ)(const char * Code,short setcode,char *Name,float *fNow,float *fClose);

typedef BOOL (* CALCTOOL_ShowFunc)(HWND);
typedef void (* CALCTOOL_RegisterCallBack)(PGPINFOHQ);

extern CALCTOOL_ShowFunc			CALCTOOL_ShowFunc_;
extern CALCTOOL_RegisterCallBack	CALCTOOL_RegisterCallBack_;

//////////////////////////////////////////////////////////////////////////////////////////////////////
//嵌入式委托

typedef struct tag_ClientInfo
{
	char	szOem[256];
	char	szYyb[256];
	char	szKhmc[64];
}ClientInfo;

enum DLGID
{
	DLG_CA	= 1,			//CA证书管理
	DLG_CFG	= 2,			//弹出交易设置
	DLG_REGUSER = 3,		//弹出注册用户
	DLG_ACTIVEUSER = 4,		//弹出激活注册用户
	DLG_UPGRADEUSER = 5,	//用户等级升级
	DLG_LEAVEWORD = 6,		//交易的留言

	DLG_SMS	= 20,			//短信口令
	DLG_TOKENCARD = 21,		//令牌卡
	DLG_UKEY = 22			//UKey
};

extern long CALLBACK CalcCallBackFunc(char * Code,short nSetCode,short DataType,void * pData/*内部申请*/,short nDataNum,NTime tFrom,NTime tEnd,BYTE nTQ,unsigned long nReserved);
extern long CALLBACK SwitchCallBackFunc(char * Code,short nSetCode,char *buf,long buflen,short FuncType,short Option,unsigned long nReserved);

typedef BOOL  (* TC_Init_Environment)(char QsId[11],const char *PubDir,const char *PriDir,const char *CfgFile,long Reserved);
typedef BOOL  (* TC_Login)(short ZhType,short ZhNum,InputZhInfo *pZhInfo,long Reserved);
typedef void  (* TC_GetLoginRet)(char *LoginRet/*255*/);
typedef BOOL  (* TC_DoLevinJy)(short bsflag,short which,short setcode,const char *code,const char *name,const char *price,const char *volume,int faceflag);
typedef BOOL  (* TC_CreateAll)(HWND parent,long Reserved);
typedef void  (* TC_ArrangeWnd)(long flag);	
typedef void  (* TC_RegisterCallBack)(PDATAIOFUNC pDatafn,PSWITCHINFOFUNC pSwitchfn);
typedef BOOL  (* TC_PreTransMsg)(MSG *pMsg);
typedef DWORD (* TC_GetJyStatus)(char *hostip/*255*/,UINT *port,char *statusmsg/*255*/);
typedef void  (* TC_BeforeHideWT)();
typedef void  (* TC_GetRightInfo)(DWORD &CategoryInfo,char &Right,char *RightName/*255*/);
typedef void  (* TC_GetVersion)(float &fVersion,char *BuildStr);
typedef void  (* TC_GetClientInfo)(ClientInfo *clientInfo);
typedef void  (* TC_GetDlg)(UINT nDlgID);
typedef BOOL  (* TC_Uninit)();
//扩展功能
typedef BOOL  (* TC_LevinGN)(int GNType,void *pInData,int InByte,void *pOutData,int &OutByte,long Reserved);
typedef void  (* TC_GetExternLevinCfg)(struct Extern_LevinCfg *pCfg);
typedef void  (* TC_SetFeedBackMsg)(HWND hWnd,UINT FeedBackMsg);
typedef int	  (* TC_GetYmdData)(MaiDan *pMainDan,int WantNum);
typedef void  (* TC_GetL2Info)(char *username,char *pass);

extern TC_Init_Environment	TC_Init_Environment_;
extern TC_Login				TC_Login_;
extern TC_GetLoginRet		TC_GetLoginRet_;
extern TC_DoLevinJy			TC_DoLevinJy_;
extern TC_CreateAll			TC_CreateAll_;
extern TC_ArrangeWnd		TC_ArrangeWnd_;
extern TC_RegisterCallBack	TC_RegisterCallBack_;
extern TC_PreTransMsg		TC_PreTransMsg_;
extern TC_GetJyStatus		TC_GetJyStatus_;
extern TC_BeforeHideWT		TC_BeforeHideWT_;
extern TC_GetRightInfo		TC_GetRightInfo_;
extern TC_GetVersion		TC_GetVersion_;
extern TC_GetClientInfo		TC_GetClientInfo_;
extern TC_GetDlg			TC_GetDlg_;
extern TC_Uninit			TC_Uninit_;
extern TC_LevinGN			TC_LevinGN_;
extern TC_GetExternLevinCfg	TC_GetExternLevinCfg_;
extern TC_SetFeedBackMsg	TC_SetFeedBackMsg_;
extern TC_GetYmdData		TC_GetYmdData_;
extern TC_GetL2Info			TC_GetL2Info_;

extern HMODULE				tc_mod;

extern BOOL InitTC(char QsId[11],const char *PubDir,const char *PriDir,const char *CfgFile);
extern void UninitTC();

////////////////////////////////////////////////////////////////////////////////////////
//闪电手模块使用的接口		
typedef void (* FClient_Start)(FHOSTINFO,FPROXYINFO,HWND,UINT);
typedef BOOL (* FClient_SubScrible)(char,char *,char,char,char *,long);
typedef BOOL (* FClient_DataACK)(char,char *,FASTHQ *);
typedef BOOL (* FClient_10_DataACK)(char,char *,FASTHQ_10 *);
typedef char*(* FClient_GetLastErr)();
typedef void (* FClient_End)();

extern FClient_Start			FClient_Start_;
extern FClient_SubScrible		FClient_SubScrible_;
extern FClient_DataACK			FClient_DataACK_;
extern FClient_10_DataACK		FClient_10_DataACK_;
extern FClient_GetLastErr		FClient_GetLastErr_;
extern FClient_End				FClient_End_;

extern HMODULE					fclient_mod;

extern BOOL LoadFClientDLL();
extern void UnLoadFClientDLL();

/////////////////////////////////////////////////////////////////////////
//外部数据DLL

typedef char * (_stdcall * getEncryptParameter)(char *yybdm,char *zjzh,char *yhjb);
typedef const char* (WINAPI * JGXEncrypt)(const char* src);
typedef const char* (WINAPI * JGXDecrypt)(const char* src);

extern getEncryptParameter	getEncryptParameter_;
extern JGXEncrypt			JGXEncrypt_;
extern JGXDecrypt			JGXDecrypt_;

extern HMODULE				otherdll_mod;

extern BOOL LoadOtherDLL();
extern void UnLoadOtherDLL();

/////////////////////////////////////////////////////////////////////////
//通达信加密DLL

typedef BOOL (* QS_Init)(const char *PubDir,const char *PriDir,const char *CfgFile,long reserved);
typedef void (* QS_RegisterCallBack)(PDATAIOFUNC pDatafn,PSWITCHINFOFUNC pSwitchfn);
typedef void (* QS_MainFrameCreated)(HWND hWnd,long Reserved);
typedef void (* QS_BeforeLoginJY)(long Reserved);
typedef void (* QS_AfterLoginJY)(long Reserved);
typedef void (* QS_TransferURL)(char *url,short setcode,char *code,long Reserved);
typedef void (* QS_Option)(int nType);
typedef void (* QS_GetCustomInfo)(char *CustomName,float &fVersion,char *BuildStr);
typedef void (* QS_MainFrameClosed)(long Reserved);
typedef void (* QS_Uninit)();

extern QS_Init				QS_Init_;
extern QS_RegisterCallBack	QS_RegisterCallBack_;
extern QS_MainFrameCreated	QS_MainFrameCreated_;
extern QS_BeforeLoginJY		QS_BeforeLoginJY_;
extern QS_AfterLoginJY		QS_AfterLoginJY_;
extern QS_TransferURL		QS_TransferURL_;
extern QS_Option			QS_Option_;
extern QS_GetCustomInfo		QS_GetCustomInfo_;
extern QS_MainFrameClosed	QS_MainFrameClosed_;
extern QS_Uninit			QS_Uninit_;

extern HMODULE			QSRun_mod;

extern BOOL LoadQSRunDLL();
extern void UnLoadQSRunDLL();

#endif

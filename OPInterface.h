#ifndef OPINTERFACE_H_
#define OPINTERFACE_H_

#include "OutStruct.h"
#include "OPAddition.h"	//与接口相关的结构和宏定义等

//数据回调函数的方式
//除传统方式后，还支持异步数据请求(精简行情、分析图等,AssisID)、分类股票、板块股票
typedef long(CALLBACK * PDATAIOFUNC)(char * Code,short nSetCode,short DataType,void * pData/*内部申请*/,short nDataNum,NTime,NTime,BYTE nTQ,unsigned long);
//实现功能切换的回调函数
typedef long(CALLBACK * PSWITCHINFOFUNC)(char * Code,short nSetCode,char *buf,long buflen,short FuncType,short Option,unsigned long);

//资讯模块使用的接口
BOOL	INFO_Init(HWND hMainWnd,long DataRq,const char *PubDir=".\\",const char *PriDir=".\\",const char *IniFile=".\\100nian.dat",const char *RegStr="",UINT MainWndFeedBackMsg=0,long oemversion=0);
BOOL	INFO_Connect(BOOL bDirect);
void	INFO_SetLoginName(char Name_YYB[50],DWORD Right,char UserType,char QsId[11]);
void	INFO_SetOtherinfo(long DataRq,DWORD unused1,char unused2[50]);
void	INFO_DisConnect();
BOOL	INFO_IsConnect();
void	INFO_GetStatus(char *bConnected,char *hostip/*255*/,unsigned short *port,char *statusmsg/*255*/);
void	INFO_Scheme(SCHEME_COLOR color,SCHEME_FONT font,SCHEME_MENU menu);
void	INFO_RegisterCallBack(PDATAIOFUNC pDatafn,PSWITCHINFOFUNC pSwitchfn);	
int 	INFO_ProcessMsg(short gn_type,HWND hWnd,UINT message, WPARAM wParam = 0, LPARAM lParam = 0);

int 	INFO_AskMineTitle(short setcode,char * code,short is_hsmine,long time_ymd[2],long time_hms[2]);
int 	INFO_GetMineNum();
int 	INFO_GetMineTitle(short ItemNum,ext_info_title	*p);

int 	INFO_AskCjzxTitle(long itemdata);
int 	INFO_GetCjzxNum();
int 	INFO_GetCjzxTitle(short ItemNum,ext_info_title	*p);
int		INFO_ShowCjzx2Content(HWND hWnd,ext_info_title *pTitle);
int		INFO_GetRollTitle(short iNo,short ItemNum,ext_info_title *p,BOOL & bUrgentRollInfo);
int 	INFO_GetGGBWTitle(short setcode,char * code,struct ext_ggbw_title *pBuf,short maxnum);
int 	INFO_ShowFunc(HWND hWnd,short gn_type,short is_hsmine,long itemdata,short setcode,char * code,short items,struct ext_info_title * p,struct ext_ggbw_title *p2,short focusItem,short daycnt);
int 	INFO_AskHqMineInfo(int count,setcode_and_stockcode *psas);
int 	INFO_GetHqMineInfo(stock_mine_info **psmi);

int		INFO_Uninit();

#endif

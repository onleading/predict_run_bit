// ExtenGNCtrl.h: interface for the CExtenGNCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTENGNCTRL_H__348F158B_BA98_45F3_82E7_0382FC745700__INCLUDED_)
#define AFX_EXTENGNCTRL_H__348F158B_BA98_45F3_82E7_0382FC745700__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "loaddll.h"

#define MAX_SUBMODULE_NUM		100
#define MAX_REGEDMESSAGE_NUM	20
#define MAX_ACCEPTABLE_NUM		20

struct ModuleMessageInfo 
{
	UINT	uRegedDataMsg;										//系统定义的消息和数据类型
	UINT	uRegMsgID;											//统一由框架注册
	char	strRegMesage[21];									//注册的消息名称
	char	strAcceptableSourceModule[MAX_ACCEPTABLE_NUM][11];	//可接受消息的来源
};

struct SubModuleInfo
{
	UINT 	nSubModuleType;										//子模块的类型，0-无系统给的父窗口 1-有系统給定的窗口
	char	strSubModuleID[11];									//识别名称
	char 	strSubModuleName[41];								//模块名,作为功能树的树枝名和菜单名
	ModuleMessageInfo	ModuleMsgInfo[MAX_REGEDMESSAGE_NUM];	//子模块注册的消息
};

struct ModuleInfo
{
	UINT 	nModuleType;		//即MODULE_STOCK, MODULE_ETF, MODULE_FUTURES等宏
	char	strModuleID[11];	//识别名称
	char 	strModuleName[41];	//模块名,作为功能树的树枝名和菜单名
	char 	strProvider[41];	//模块版权信息
	float 	fVersion;			//此模块的版本号
	UINT 	nSubModuleNum;		//子模块的个数
	SubModuleInfo	SubModules[MAX_SUBMODULE_NUM];	//子模块
	char	strSerialNo[33];	//序列号
	char	unUsed[222];
};

//取模块信息
typedef void	(*_TDXWEXTGN_GETMODULEINFO)(struct ModuleInfo *pInfo);
//告诉模块回调函数的地址
typedef void	(*_TDXWEXTGN_REGISTERCALLBACK)(PDATAIOFUNC pDataFn,PSWITCHINFOFUNC pFuncFn);
//初始化模块
typedef BOOL	(*_TDXWEXTGN_INITMODULE)(const char *PubDir=".\\",const char *PriDir=".\\", DWORD unUsed=0);
//配色方案
typedef	void	(*_TDXWEXTGN_Scheme)(SCHEME_COLOR color, SCHEME_FONT font, SCHEME_MENU menu);
//在框架容器中显示子模块的界面,nWhichSub为子模块编号
typedef BOOL	(*_TDXWEXTGN_SHOWMODULE)(UINT nUnitID,UINT nWhichSub, HWND hVessel,char *lpAppName,char *lpFileName,char *errMsg,DWORD unUsed=0);
//大小的变化的响应
typedef void 	(*_TDXWEXTGN_ARRANGEWND)(UINT nUnitID,UINT nWhichSub,UINT cx,UINT cy);
//数据和消息传递
typedef BOOL	(* _TDXWEXTGN_SETVALUE)(int nUnitID,UINT nWhichSub,int DataType,int DataLen,void *pData);
//退出模块
typedef void	(*_TDXWEXTGN_EXITMODLE)(UINT nUnitID,UINT nWhichSub,char *lpAppName,char *lpFileName);			//-1表示全部子功能退出

///////////////////////////////////////////////////////////////////////////////////////////

typedef	struct	tagModuleIndex						//	模块加载和使用信息
{
	HMODULE					hModule;				//	动态连接库句柄
	ModuleInfo				iModuleInfo;			//	模块信息
	_TDXWEXTGN_INITMODULE	ModuleInit;				
	_TDXWEXTGN_SHOWMODULE	ModuleShow;
	_TDXWEXTGN_Scheme		ModuleScheme;
	_TDXWEXTGN_ARRANGEWND	ModuleResize;
	_TDXWEXTGN_SETVALUE		ModuleDataTrans;
	_TDXWEXTGN_EXITMODLE	ModuleExit;
	short					StatusFlag;				//	是否完成初始化
	int						nSubStartID;			//	子模块ID，对应UnitType
	//
	HWND					ParentWnd[MAX_SUBMODULE_NUM];
	short					SubStatusFlag[MAX_SUBMODULE_NUM];
}IMODULE;

extern DWORD l_GlobalUnitID;

class CNewMenu;
//////////////////////////////////////////////////////////////////////////
class CExtenGNCtrl  
{
public:
	CExtenGNCtrl();
	virtual ~CExtenGNCtrl();

	long	LoadExternModule(void);
	BOOL	ShowThis(HWND hParentWnd, long nNo, char *errMsg);
	void	ResizeThis(CWnd *pWnd,UINT cx,UINT cy);
	void	ExitThis(CWnd *pWnd);

	short	GetModuleNum(void)
	{
		return m_ModuleEntries.size();
	};
	BOOL	GetModuleInfo(short nModule, ModuleInfo &iModuleInfo)
	{
		if(nModule<0||nModule>=m_ModuleEntries.size()) return FALSE;
		memcpy(&iModuleInfo, &m_ModuleEntries[nModule].iModuleInfo, sizeof(ModuleInfo));
		return m_ModuleEntries[nModule].hModule!=NULL;
	};

	BOOL	LoadModule(int nUnitStyle, int &nSubModule, IMODULE* &pModule);

private:
	BOOL	LoadModuleDll(LPCSTR  DllName, int nModule, IMODULE *iModule);

private:
	vector<IMODULE> m_ModuleEntries;
};

#endif // !defined(AFX_EXTENGNCTRL_H__348F158B_BA98_45F3_82E7_0382FC745700__INCLUDED_)

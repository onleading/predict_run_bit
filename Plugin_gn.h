#ifndef _PLUGIN_GN_H_
#define _PLUGIN_GN_H_

#include "OutStruct.h"

/////////////////////////////////////////////////////////////////////////////
//					外部功能插件接口
/////////////////////////////////////////////////////////////////////////////

#define MAX_GNPLUGIN_NUM	100

#define GNPLUG_TYPE_EMBED	0			//Unit级组件
#define GNPLUG_TYPE_DIALOG	1			//对话框级组件
#define GNPLUG_TYPE_VIEW	2			//View级组件

//重要自定义消息接口
#define UM_UNIT_BROADCAST			WM_USER+8000	//广播
#define UM_EXOPENBLOCKHQ			WM_USER+8001	//打开板块

struct GNPluginInfo
{
	UINT 	nModuleType;		//Unit级组件,对话框级组件,View级组件
	char 	strModuleCode[41];	//模块内部名
	char 	strModuleDesc[41];	//模块描述
	char 	strProvider[41];	//模块版权
	char	strVersion[41];		//版本信息
	short	nUnitID;			//如果是Unit级组件的话,UnitID在0-99之间
	short	nGNLevel;			//功能级别 1-2
	char	bFirstRefreshAllHQ;	//进入前是否要刷新所有行情
	short	nViewID;			//如果是View级组件的话,nViewID在0-99之间
	char	bbPlugin_GPRel;		//该组件是否个股相关
	char	bOnlyInCom;			//仅仅支持在特定版面中显示(不列在定制版面组件中,对于Unit级组件有效)
	char	RightChar;			//权限字符串
	char	unUsed[167];		//保留
};

//取模块信息
typedef void	(*_TE_GetModuleInfo)(struct GNPluginInfo *pInfo);
//告诉模块回调函数的地址
typedef void	(*_TE_RegisterCallBack)(PDATAIOFUNC pDataFn,PSWITCHINFOFUNC pFuncFn);
//设置配色和字体等
typedef void	(*_TE_SetScheme)(SCHEME_COLOR color,SCHEME_FONT font,BOOL bRedraw);
//初始化模块
typedef BOOL	(*_TE_InitModule)(const char *PubDir=".\\",const char *PriDir=".\\", DWORD unUsed=0); 

//-----以下可用于Unit级组件和View级组件
//显示某种功能
typedef BOOL	(*_TE_ShowFunc)(CWnd *pParent,UINT nFuncType,UINT nFuncOption,long reserved);

//-----以下只用于Unit级组件
//注意nCtrlId,可能有多个,0表示所有,单个Ctrl从1开始
//在框架容器中显示模块的界面
typedef BOOL	(*_TE_ShowModule)(HWND hVessel,char *errMsg,UINT nCtrlId);
//大小的变化的响应
typedef void 	(*_TE_ArrangeWnd)(UINT cx,UINT cy,UINT nCtrlId);
//退出单个功能
typedef void	(*_TE_ExitModule)(UINT nCtrlId);

//-----以下可用于Unit级组件和View级组件
//响应消息
typedef int		(*_TE_ProcMsg)(UINT message, WPARAM wParam, LPARAM lParam,UINT nCtrlId);
//得到当前分析的证券信息
typedef void	(*_TE_GetCurrentGPInfo)(short & setcode,char *strCode,UINT nCtrlId);
//设置当前分析的证券信息
typedef BOOL	(*_TE_SetCurrentGPCode)(short setcode,const char *strCode,UINT nCtrlId);

//退出模块
typedef void	(*_TE_UnInitModule)(DWORD unUsed=0); 

typedef	struct	tagGNPluginIndex
{
	HMODULE					hModule;			//	动态连接库句柄
	GNPluginInfo			iInfo;				//	插件信息

	_TE_RegisterCallBack	RigisterCallBack;
	_TE_SetScheme			SetScheme;
	_TE_InitModule			InitModule;

	_TE_ShowFunc			ShowFunc;
	_TE_ShowModule			ShowModule;
	_TE_ArrangeWnd			ArrangeWnd;
	_TE_ProcMsg				ProcMsg;
	_TE_ExitModule			ExitModule;
	_TE_GetCurrentGPInfo	GetCurrentGPInfo;
	_TE_SetCurrentGPCode	SetCurrentGPCode;

	_TE_UnInitModule		UnInitModule;
}IGNPLUGIN;


extern char g_GNPlug_TypeStr[MAX_GNPLUGIN_NUM][41];

/////////////////////////////////////////////////////////////////////////////
//					CGNPlugin 外部接口处理类
/////////////////////////////////////////////////////////////////////////////

class CGNPlugin
{
public:
	CGNPlugin();
	virtual ~	CGNPlugin();

	long		LoadGNPlugin();
	short		GetPluginNum()	{ return m_Modules.size(); }
	BOOL		GetOnePluginInfo(int iNo, GNPluginInfo * pInfo);
	IGNPLUGIN*	GetPluginFromModuleCode(char *ModuleCode);
	char*		GetModuleCodeFromUnitID(UINT nUnitID);

	IGNPLUGIN*	GetPluginFromTypeOrder(int iOrder,int nType);
	IGNPLUGIN*	GetPlugin(int iNo);

private:
	long		GetGNPluginNum(LPCSTR lpFormat);
	BOOL		AdjustPlugin(GNPluginInfo *pInfo);
	BOOL		LoadPluginDll(LPCSTR  DllName, IGNPLUGIN *iPlugin);

private:
	vector<IGNPLUGIN> m_Modules;

};

#endif

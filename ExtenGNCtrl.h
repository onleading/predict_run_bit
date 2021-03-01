#ifndef EXTERNGNCTRL_H_
#define EXTERNGNCTRL_H_

struct ModuleInfo
{
	UINT 	nModuleType;		//即MODULE_STOCK, MODULE_ETF, MODULE_FUTURES等宏
	char 	strModuleName[41];	//模块名,作为功能树的树枝名
	char 	strProvider[41];	//模块版权
	float 	fVersion;			//此模块的版本号
	char	strSerialNo[33];	//序列号
	char	unUsed[222];
};

//取模块信息
typedef void	(*_TE_GetModuleInfo)(struct ModuleInfo *pInfo);
//告诉模块回调函数的地址
typedef void	(*_TE_RegisterCallBack)(PDATAIOFUNC pDataFn,PSWITCHINFOFUNC pFuncFn);
//初始化模块
typedef BOOL	(*_TE_InitModule)(const char *PubDir=".\\",const char *PriDir=".\\", DWORD unUsed=0); 
//在框架容器中显示模块的界面
typedef BOOL	(*_TE_ShowModule)(HWND hVessel,char *errMsg,UINT nCtrlId);
//大小的变化的响应
typedef void 	(*_TE_ArrangeWnd)(UINT cx,UINT cy,UINT nCtrlId);
//响应消息
typedef int		(*_TE_ProcMsg)(UINT message, WPARAM wParam, LPARAM lParam,UINT nCtrlId);
//退出模块
typedef void	(*_TE_ExitModule)(UINT nCtrlId);

typedef	struct	tagModuleIndex
{
	HMODULE					hModule;				//	动态连接库句柄
	ModuleInfo				iModuleInfo;			//	模块信息

	_TE_InitModule			ModuleInit;
	_TE_ShowModule			ModuleShow;
	_TE_ArrangeWnd			ModuleArrangeWnd;
	_TE_ProcMsg				ModuleProcMsg;
	_TE_ExitModule			ModuleExit;
}IMODULE;

class CExtenGNCtrl
{
public:
	CExtenGNCtrl();
	virtual ~CExtenGNCtrl();

	long	LoadExternModule(void);
	BOOL	ShowThis(HWND hParentWnd, long nNo, char *errMsg);
	void	ResizeThis(CWnd *pWnd,UINT cx,UINT cy);
	void	ExitThis(CWnd *pWnd);

	short	CExtenGNCtrl::GetModuleNum(void)
	{
		return m_ModuleEntries.size();
	};

	BOOL	CExtenGNCtrl::GetModuleInfo(short nModule, ModuleInfo &iModuleInfo)
	{
		if(nModule<0||nModule>=m_ModuleEntries.size()) return FALSE;
		memcpy(&iModuleInfo, &m_ModuleEntries[nModule].iModuleInfo, sizeof(ModuleInfo));
		return m_ModuleEntries[nModule].hModule!=NULL;
	};

	IMODULE*	GetModuleFromCode(char *Name);

private:
	BOOL		LoadModuleDll(LPCSTR  DllName, int nModule, IMODULE *iModule);
private:
	vector<IMODULE> m_ModuleEntries;
	long			m_nActiveModule;
	long			m_nWhichSub;
};

#endif

#ifndef VINFO_H_
#define VINFO_H_

#ifdef	TINFO_EXPORT
	#define TINFODLL_API extern "C" __declspec(dllexport)
#else
	#define TINFODLL_API extern "C" __declspec(dllimport)
#endif

#pragma pack(1)

struct PopupWnd_Info
{
	COLORREF rgb1;
	COLORREF rgb2;
	COLORREF txtbk;
	COLORREF txtcolor;
	short	 cx;
	short	 cy;
	short	 stayseconds;
	char	 wndtype;
	char	 hasverifybtn;
	char	 verifyinfo[51];
	char	 unused[51];
};

#define UM_VERIFY_MESSAGE		WM_USER+0x2421	//弹出验证的消息
#define UM_SEARCH_GPGRADE		WM_USER+5018	//搜索某只股票的评级

//数据回调函数的方式
typedef long(CALLBACK * PDATAIOFUNC)(char * Code,short nSetCode,short DataType,void * pData/*内部申请*/,short nDataNum,NTime,NTime,BYTE nTQ,unsigned long);
//实现功能切换的回调函数
typedef long(CALLBACK * PSWITCHINFOFUNC)(char * Code,short nSetCode,char *buf,long buflen,short FuncType,short Option,unsigned long);

//弹出信息框(独立使用)
TINFODLL_API void	I_PopupWnd(CWnd *pMainWnd,struct PopupWnd_Info Info,const char *Title,const char *Txt,const char *Url);
//初始化环境
TINFODLL_API BOOL	I_Init_Environment(const char *PubDir,const char *PriDir,const char *CfgFile,long reserved);
//注册主程序的两个回调函数
TINFODLL_API void	I_RegisterCallBack(PDATAIOFUNC pDatafn,PSWITCHINFOFUNC pSwitchfn);
//判断是否有机构评级或机构诊断数据
TINFODLL_API BOOL	I_HasJGData(const char *Code,short setcode,long reserved);
//弹出某只股票的机构数据窗口
TINFODLL_API void	I_ShowJGDialog(long nGPID,long reserved);
//弹出功能窗口
TINFODLL_API void	I_ShowFunc(long nType,long reserved);
//传入消息
TINFODLL_API int	I_ProcessMsg(HWND hWnd,UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
//弹出时调用
TINFODLL_API int	I_Exit();

#pragma pack()
#endif

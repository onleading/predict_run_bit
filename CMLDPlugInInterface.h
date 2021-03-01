/********************************************************
*														*
*			主力雷达插件示意程序						*
*														*
*			上海鼎新软件								*
*														*
*********************************************************/

#ifndef __CMLD_PLUGIN_INTERFACE__
#define __CMLD_PLUGIN_INTERFACE__

#pragma pack( push, 1 )
//	日线数据
typedef struct	tagLINEDAYITEM
{
	DWORD	dwYYYYMMDD;
	float	open;
	float	high;
	float	low;
	float	close;
	float	volume;
	float	amount;				//	其他数据还需要补充
}LINEDAYITEM;
#pragma pack(pop)

// 证券市场
#define		SH_MARKET_EX			'HS'		// 上海
#define		SZ_MARKET_EX			'ZS'		// 深圳

typedef struct tagONE_STK_LABEL
{
	WORD	wMarket;			//	市场类型
	char	szLabel[10];		//	股票代码
}ONE_STK_LABEL;

#define	MAX_PARAM	6
typedef struct tagTDX_CALCINFO
{
	const DWORD m_dwSize;		//结构大小
	const DWORD m_dwVersion;	//调用软件版本(V2.10 : 0x210)
	const DWORD m_dwSerial;		//调用软件序列号
	const ONE_STK_LABEL m_strStkLabel; //股票代码
	const BOOL m_bIndex;		//大盘

	const int m_nNumData;		//数据数量(pData,pDataEx,pResultBuf数据数量)
	const LINEDAYITEM* m_pData; //常规数据,注意:当m_nNumData==0时可能为 NULL

	// 每个m_pfParam都是数组，个数为m_nNumData，和m_pResultBuf一样
	// 比如这样的公式
	// A:=MA(CLOSE,10);
	// B:=MA(CLOSE,20);
	// C:PLUGFUNC_X(A,B);
	// A和B就是两个m_pfParam数组
	const float* m_pfParam[MAX_PARAM];	//调用参数

	float* m_pResultBuf; //结果缓冲区
}TDX_CALCINFO;

//////////////////////////////////////////////////////////////////////////
interface ICMLDPlugIn_Client;

//-------------------------------------------------------------------
//	系统提供的函数给插件使用，插件应该保留该接口指针，以备使用
interface ICMLDPlugIn_Server
{	
	// 【保留】，获取服务器能力；保留将来使用
	virtual DWORD  GetCapability( int nIndex ) = 0;

	// 获取用户帐户
	virtual LPCSTR GetUserAccount() = 0;
	// 获取用户登录密码，也可以是处理后的用户密码，但能唯一表示用于原来的密码
	virtual LPCSTR GetUserPassword() = 0;
	
	//	切换到行情浏览窗，并移动到pLabelID指定的股票上
	virtual	void	SwitchToReportView( ONE_STK_LABEL * pLabelID, ONE_STK_LABEL * pOrderList, int nCount )=0; 
	//	切换到pLabelID指定股票的K线窗口，
	//	pOrderList和nCount为可选参数，当 pOrderList 不为 NULL 时，要求日线显示窗口按照指定的顺序切换股票（如按 PageDown 下翻股票时)
	virtual	void	SwitchToKlineView( ONE_STK_LABEL * pLabelID, ONE_STK_LABEL * pOrderList, int nCount)=0;
	//	切换到pLabelID指定股票的F10资料
	virtual	void	SwitchToF10View( ONE_STK_LABEL * pLabelID)=0;
	//	执行快捷键，如“03”、“04”等
	virtual	void	OnKeyboardGeniusCmd( LPCSTR lpszCmd )=0;
	//	获取pLabelID指定的股票名称，返回NULL表示失败
	virtual	LPCSTR	GetStkName( ONE_STK_LABEL * pLabelID )=0;

	// 获取指定股票日线个数；
	// 用于分配内存
	virtual int		GetLineDayDataSize( ONE_STK_LABEL * pLabelID ) = 0;
	//	获取 pLabelID 指定的股票，从 nStartDate到nEndDate 之间的日线数据，
	//	pnOutDayCount 输出日线数据个数
	//	bDoPowerSplit 指定输出的数据是否需要除权, TRUE 表示需要除权
	//  返回 日线数据数组，【该变量将通过 FreeMemory 函数释放】
	virtual LINEDAYITEM * GetLineDayData( ONE_STK_LABEL * pLabelID, int nStartDate, int nEndDate, int * pnOutDayCount, BOOL bDoPowerSplit ) = 0;

	// 获取股票列表
	// wMarketType			市场类型，SH_MARKET_EX or SZ_MARKET_EX
	//						0 表示全部市场
	// pnCount				输出股票数量
	// 返回：
	//		股票代码数组；【该内存将通过 FreeMemory 释放】
	virtual ONE_STK_LABEL * GetStockLabelList( WORD wMarketType, int * pnCount ) = 0;

	//	注册插件菜单，菜单ID由系统自动分配。
	//	pClientObj		插件对象
	//	lpszMenuItem	主菜单项中的菜单名字
	//	alpszSubMenu[0] ~ alpszSubMenu[nSubMenuCount-1]	为子菜单项。如果 alpszSubMenu[i] = "-" 则表示为分割条
	//	nSubMenuCount	为菜单项个数
	//	ahMenuBmp		为与菜单相对应的图形，若 ahMenuBmp[i] = NULL，表示对应 alpszMenu[i]没有图形
	//					系统需要复制相应的 Bitmap，当该函数返回后， ahMenuBmp 中的 bitmap 可能被释放
	virtual BOOL	RegisterMenu( ICMLDPlugIn_Client * pClientObj, LPCSTR lpszMenuItem, LPCSTR alpszSubMenu[], int nSubMenuCount, HBITMAP ahMenuBmp[] ) = 0;

	// 释放由服务器分配的内存
	virtual void FreeMemory( void * pBuf ) = 0;	
	
	// 【保留】，将来扩充接口	
	virtual HRESULT QueryInterface( REFIID iid, void **ppvObject ) = 0;	
};

//--------------------------------------------------------
//	客户端接口，服务器调用插件；服务器会保留该接口以备使用
interface ICMLDPlugIn_Client
{
	// 初始化插件
	virtual void Init( HWND hMainWnd, ICMLDPlugIn_Server * pServer ) = 0;
	// 服务器退出前要求插件清除资源
	virtual void Exit() = 0;

	//	响应菜单事件
	virtual void OnMenuCommand( LPCSTR lpszMenuString ) = 0;
	//	下载数据，返回 TRUE,用户按下取消按钮；FALSE 用户没有按取消按钮
	virtual BOOL OnDownloadData() = 0;
	
	// 释放由插件分配的内存
	virtual void FreeMemory( void * pBuf ) = 0;
	// 获取接口
	virtual HRESULT QueryInterface( REFIID iid, void **ppvObject ) = 0;		// 保留，将来扩充接口	
};


//--------------------------------------------------------
//	函数接口
extern "C" {

	//	获取插件接口，以备服务器调用WINAPI	
	__declspec(dllexport) ICMLDPlugIn_Client * GetClientInterface();
};

//指标插件的函数若干
#define PLUGIN_CMLD __declspec(dllexport)

extern "C" PLUGIN_CMLD BOOL	PLUGINFUNC_A(TDX_CALCINFO*pCalcItem);


#endif // __CMLD_PLUGIN_INTERFACE__

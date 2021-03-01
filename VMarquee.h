#ifndef VMARQUEE_H_
#define VMARQUEE_H_

#ifdef	TMARQUEE_EXPORT
	#define TMARQUEEDLL_API extern "C" __declspec(dllexport)
#else
	#define TMARQUEEDLL_API extern "C" __declspec(dllimport)
#endif
 
typedef enum 
{
	SM_HSCROLL		= 1, //横向滚动
	SM_VSCROLL		= 2, //纵向滚动
}SCROLLMODE;

typedef enum
{
	DT_INFO			= 1, //信息
	DT_CUSTOMSTOCK	= 2, //股票
}DISPLAYTYPE;

typedef struct tagPos
{
	int head;
	int tail;
}VIEWPORT;

typedef enum
{
	IT_LINK = 1,//链接
	IT_INFO = 2,//信息
	IT_GP	= 3,//股票
}INFO_TYPE;

typedef struct tagMarqueeInfo
{
	INFO_TYPE	infotype;		//1 链接 2信息 3股票
	DWORD		nInfoID;		//信息ID
	char		content[256];	//系统消息的内容，最长255个字符
	char		url[256];		//如果指向某一个链接，填接地址，否则url[0]=='\0';
	int			weight;			//信息的权重
	COLORREF	color;			//信息的颜色
}MARQUEEINFO,LPMARQUEEINFO;

typedef struct tagMarqueeStock
{
	short	setcode;
	char	Code[20];
	char	Name[20];	//股票名
	long	nGPIndex;
}MARQUEESTOCK,*LPMARQUEESTOCK;

struct MQ_HQInfo
{
	short	xsFlag;		//小数位数
	float	fNow;		//现价
	float	fClose;		//收盘价
	float	fVol;		//成交量
	float	fHsl;		//换手率
	float	fLB;		//量比
};

typedef long (CALLBACK * PMQ_HQINFO)(const char * Code,short setcode,MQ_HQInfo *pHQInfo);

class VMarquee : public CWnd
{
public:
	//启用圆弧立体风格边缘
	virtual void EnableArcEdgeStyle(BOOL bFlag) = 0;
	//设置窗口大小
	virtual void ResizeWindowPos(int x,int y,int cx,int cy) = 0;
	//设置反射消息
	virtual void SetFeedBackMsg(HWND hMsgWnd,UINT uMsgID,DWORD Reserved=0) = 0;
	//设置股票数据回调函数
	virtual void SetHQCallBack(PMQ_HQINFO pfn) = 0;
	//设置成随机模式
	virtual void SetRandom(BOOL bFlag) = 0;
	//设置跑马灯背景色
	virtual void SetBackColor(COLORREF crBack) = 0;
	//设置自选股文字颜色
	virtual void SetCustomStockTxtColor(COLORREF cr1,COLORREF cr2,COLORREF cr3,COLORREF cr4,COLORREF cr5,COLORREF cr6) = 0;
	//设置系统消息文字颜色
	virtual void SetSysInfoTextColor(const COLORREF crTxt) = 0;
	//设置显示模式:系统消息 or 自选股
	virtual void SetDisplayType(DISPLAYTYPE nDt) = 0;
	//设置滚动模式: 横向 or 垂直
	virtual void SetScrollMode(SCROLLMODE nMode) = 0;
	//设置一屏暂停时长
	virtual void SetStopTime(int nCount) = 0;
	//设置跑马灯速度
	virtual void SetMoveSpeed(const UINT& nSpd) = 0;
	//此接口修改为滚动风格 0 不滚动 1 滚动 2 切换整屏 
	virtual void SetMoveStyle(int nStyle) = 0;

	//清空系统消息
	virtual void ClearSysInfo() = 0;
	//清空自选股
	virtual void ClearCustomStock() = 0;
	//添加系统消息
	virtual void AddSysInfo(const MARQUEEINFO &si) = 0;
	//添加自选股信息
	virtual void AddCustomStock(const MARQUEESTOCK& si) = 0;
	//初始化
	virtual void InitPos() = 0;
	//停止
	virtual void Stop(void) = 0;
	//开始 bSwitch参数控制是否整屏显示
	virtual void StartScroll(UINT nEventID,UINT nInterval) = 0;	
	//设置关闭按钮
	virtual BOOL SetCloseMsg(HWND hMsgWnd,UINT uMsgID,BOOL bEnable = FALSE) = 0;
};

TMARQUEEDLL_API VMarquee* NewMarquee(CWnd *pParent,UINT nID);
TMARQUEEDLL_API void DelMarquee(VMarquee * p);

#endif

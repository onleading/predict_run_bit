#ifndef MARQUEE_H_
#define MARQUEE_H_

typedef enum
{
	MARQUEE_POPINFO = 1,
	MARQUEE_SHOWGP	= 2,
}MARQUEE_MSG;

typedef enum 
{
	SM_HSCROLL	= 1, //横向滚动
	SM_VSCROLL	= 2, //纵向滚动
}SCROLLMODE;

typedef enum
{
	DT_SYSINFO		= 1, //系统消息
	DT_CUSTOMSTOCK	= 2, //股票
}DISPLAYTYPE;

typedef struct tagMarqueeStock
{
	CString	sStkName;		//股票名称
	UINT	nStkCode;		//股票代码
	FLOAT	fStkCurrPrice;	//现价
	FLOAT	fStkClsPrice;	//收盘价
	CString	sStkVolumn;		//成交量
	FLOAT	fStkTR;			//换手率
	FLOAT	fStkVR;			//量比
}MARQUEESTOCK,*LPMARQUEESTOCK;

typedef struct tagPos
{
	int head;
	int tail;
}VIEWPORT;

typedef enum
{
	IT_LINK = 1,	//链接
	IT_POP	= 2,	//弹出窗口
}INFO_TYPE;

typedef struct tagMarqueeInfo
{
	char		content[256];	//系统消息的内容，最长255个字符
	char		url[256];		//如果指向某一个链接，填接地址，否则url[0]=='\0';
	INFO_TYPE	flag;			//1 打开连接 2弹出窗口
	int			weight;			//信息的权重
}MARQUEEINFO,LPMARQUEEINFO;

class CMarquee : public CWnd
{
public:
	CMarquee();
	virtual ~CMarquee();
protected:
	BOOL			m_bEnableStop;			//是否可以暂停
	int				m_nDisplayType;			//显示系统消息还是自股票
	int				m_nScrollMode;			//横向滚动还是纵向滚动
	COLORREF		m_crBackColor;			//控件背景色
	COLORREF		m_crSysInfoTxt;			//系统消息颜色
	COLORREF		m_crCustomStockTxt[6];	//自选股颜色
	CString			m_strText;				//
	UINT			m_nEventID;				//TimerID
	VIEWPORT		m_stViewport;			//视口
	UINT			m_nMoveSpd;				//时钟间隔
	BOOL			m_bTimerFlag;			//

	std::vector<MARQUEESTOCK>	m_vecCustomStk; //自选股票
	std::vector<VIEWPORT>		m_vecCustomPos;
	std::vector<MARQUEEINFO>	m_vecSysInfo;	//系统消息
	std::vector<VIEWPORT>		m_vecSysInfoPos;
	int							m_nLength;

	//{{AFX_MSG(CMarquee)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//设置反射消息
	void SetFeedBackMsg(HWND hMsgWnd,UINT uMsgID,DWORD Reserved=0);
	//设置成随机模式
	void SetRandom(BOOL bFlag);
	//设置跑马灯背景色
	void SetBackColor(COLORREF crBack);
	//设置自选股文字颜色
	void SetCustomStockTxtColor(COLORREF cr1,COLORREF cr2,COLORREF cr3,COLORREF cr4,COLORREF cr5,COLORREF cr6);
	//设置系统消息文字颜色
	void SetSysInfoTextColor(const COLORREF crTxt);
	//设置显示模式:系统消息 or 自选股
	void SetDisplayType(DISPLAYTYPE nDt);
	//设置滚动模式: 横向 or 垂直
	void SetScrollMode(SCROLLMODE nMode);
	//设置一屏暂停时长
	void SetStopTime(int nCount);
	//设置跑马灯速度
	void SetMoveSpeed(const UINT& nSpd);
	//是否开启一屏暂停
	void EnableStop(BOOL bFlag);

	//清空系统消息
	void ClearSysInfo();
	//清空自选股
	void ClearCustomStock();
	//添加系统消息
	void AddSysInfo(const MARQUEEINFO &si);
	//添加自选股信息
	void AddCustomStock(const MARQUEESTOCK& si);
	//初始化
	void InitPos();
	//停止
	void Stop(void);
	//开始
	void StartScroll(UINT nEventID,UINT nInterval);	
	//Size消息
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	HWND	m_hMsgWnd;		//反馈消息的窗口
	UINT	m_uMsgID;		//反馈的消息

	BOOL	m_bRandom;		//是否是随机状态

	CRect	m_rcBtn;		//右侧小按钮形状
	CRect	m_rcClient;

	int		m_nStopTime;
	int		m_nStopCount; 
private:
	BOOL RegisterWindowClass(HINSTANCE hInstance,LPCTSTR lpszClassName);
	void DrawIt();
};

#endif

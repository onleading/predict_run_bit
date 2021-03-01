#ifndef UBASE_H_
#define UBASE_H_

#include "struct.h"
#include "custompad.h"

#define CREATE_MEMDC(theRc)	\
	CDC		MemDC;		\
	CBitmap Bitmap,*OldBitmap;	\
	MemDC.CreateCompatibleDC(pDC);	\
	Bitmap.CreateCompatibleBitmap(pDC,theRc##.Width(),theRc##.Height());	\
	OldBitmap = MemDC.SelectObject(&Bitmap);		\
	MemDC.FillSolidRect(0,0,theRc##.Width(),theRc##.Height(),VipColor.BACKCOLOR);	\
	CFont *oldfont= g_d.SelectFont(&MemDC,GRAPH_FONT);		\
	MemDC.SetBkMode(TRANSPARENT);

#define DELETE_MEMDC		\
	g_d.RestoreFont(&MemDC,oldfont);	\
	MemDC.SelectObject(OldBitmap);		\
	Bitmap.DeleteObject();			\
	MemDC.DeleteDC();

class UBase : public CRect
{
protected:
	CWnd	*	m_pView;
	DWORD		m_nInitPad;
	int			m_nWhichUnit;
	LPUNITS		m_pUnits;
	int			m_nInCom2;
	int			m_nInHisDlg;
	int			m_nInMultiPeriod;

	int			m_nType;
	int			m_nStyle;
	CRect		m_rcIn;

	short		m_nCurGP;	//该品种序号 小于10000表示是基本股票 大于10000是三方品种
	MemStkInfo *m_pStkInfo;	//该股票的MemStkInfo

	short		m_Domain;	//该股票的市场
	int			m_XsFlag;	//小数位数
	BOOL		m_bLcIdx;	//自定组合品种
	BOOL		m_bLcExt;	//自定外部品种
	BOOL		m_bLcPt;
	BOOL		m_bZs;
	BOOL		m_bHKZs;
	BOOL		m_bGz;		//是否是国债
	BOOL		m_bQyzq;	//是否是企债
	BOOL		m_bQz;		//是否是权证
	BOOL		m_bABG;		//是否是AB股
	BOOL		m_bSZSHZS;	//是否是深沪统一指数
	BOOL		m_bETF;		//是否是ETF
	BOOL		m_bSZKFSJJ;	//是否是深圳开放式
	BOOL		m_bSHKFSJJ;	//是否是上海开放式
	BOOL		m_bCloseJJ;	//是否是封闭式基金
	long		m_lDate;
	BOOL		m_bIPO;		//是否是第一天上市
	
	BOOL		m_bMinuteTick_Special,m_bAnalyData_Special;
	
	BOOL		m_bDSFlag;		//	三方市场商品标志
	short		TimeSec[8];		//	时间段信息
	PTIMEINF	m_pTimer;		//	时间属性

	int			m_nGxdFlag;
	BOOL		m_bLevel2Property;
public:
	UBase(LPCOMINFO pComInfo);
	virtual ~UBase();

	int		GetMyType() { return m_nType; }

	virtual int ProcessMsg(UINT message, WPARAM wParam = 0, LPARAM lParam = 0);
};

#endif

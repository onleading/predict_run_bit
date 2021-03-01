#if !defined(AFX_COMVIEW_H__65DE8561_CB93_11D4_BB9D_00E04C66B1EB__INCLUDED_)
#define AFX_COMVIEW_H__65DE8561_CB93_11D4_BB9D_00E04C66B1EB__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "macro.h"
#include "struct.h"
#include "globalvar.h"
#include "custompad.h"
#include "UFxt.h"
#include "UZst.h"
#include "UHq.h"
#include "UFjb.h"
#include "UTick.h"
#include "UCdp.h"
#include "UMmp.h"
#include "UCb.h"
#include "UBigVol.h"
#include "UWeb.h"
#include "UGPWeb.h"
#include "UKList.h"
#include "USeat.h"
#include "UHq10.h"
#include "UTick2.h"
#include "UQueue.h"
#include "UTickStat.h"
#include "UWTFB.h"
#include "UWTRank.h"
#include "USCancel.h"
#include "UTCancel.h"
#include "UHycj.h"
#include "UTVGraph.h"
#include "UTickZst.h"
#include "UGlHq.h"
#include "UBaseInner.h"

#include "SwitchEx.h"
#include "UnitView.h"
/////////////////////////////////////////////////////////////////////////////

#define ZST_SWITCH3_HEIGHT	18
#define ZST_SWITCH4_WID		54
#define ZST_SWITCHV_WID		17

#define ZST_MOREUNIT_HEIGHT	200

#define CORNER_SQUARE		30

class CComView : public CUnitView
{
protected:
	CComView();
	DECLARE_DYNCREATE(CComView)
private:
	BOOL	m_bFxtMove;

	DWORD	m_nNowPad;		//当前的版面类型
	DWORD	m_nLastPad;		//上次的版面类型
	DWORD	m_nMainPad;		//主版面,FXT和ZST等
	DWORD	m_nAssisPad1;	//辅助版面1,右边部分
	DWORD	m_nAssisPad2;	//辅助版面2,右边部分

	BOOL	m_bLastShow;	//上次版面是否有右边
	BOOL	m_bNowShow;

	short	m_nCurGP;		//当前所分析的第一支股票

	short	m_nDComCurGP;
	short	m_nDComUnit;

	short	m_nPageGPIndex[MAXGPNUM];
	int		m_nPageGPNum;	//如果为-1，则表示在所有股票换页
	int		m_nCurPageGP;	//当前是该PageGP中的第几个
	short	m_nMulGpIndex[MULHQ_WANTNUM];
	short	m_nMulGpNum;
	short	m_nCurMulGp;

	short	m_nCurSwitch1,m_nCurSwitch2;

	BOOL	m_bSplitting1,m_bSplitting2;
	int		m_nOldSplitX;

	int		m_nTranZstStyle;	//是哪一种走势图模式

	//用于判断多股进入的返回
	BOOL	m_bMult_In;			//从多股进入的
	BOOL	m_bNoChange_MultIn;	//不改变m_bMult_In方式

	LinkedSetInfo m_LSetInfo;
	
	vector<short>	m_GoCache;
	BOOL	m_bInGoCache;
	int		m_nGoCacheCursor;

	CNewZfjsTab	*m_pSwitchBar1,*m_pSwitchBar2,*m_pSwitchBar3;
	
	BOOL	m_bShowAssisPad2;

	BOOL	m_bMultiPeriod;
	int		m_nFirstPeriod;
	int		m_nCurPeriodOff;

	int		m_nMore_Sel;
	BOOL	m_bZstMore_Show;
	BOOL	m_bFxtMore_Show;

	int		m_nSwitchUnitNum;
	
	BOOL	m_bShowSwitch3;
protected:
	void	CreateUnit(int which,int initGPNo,int type,int style=0);//构造一个Unit
	BOOL	ChangeUnitGP(int which,int iNo);

	inline	BOOL	TestUnitProperty(int iUnit, UINT nFlag);
	void	ShowHideDynBtn();
	void	SetComTitle();

	void	RefreshCurrPADHQ();

	void	ConstructPad(DWORD tmpPad,short iNo=-1);
	void	ConstructPad(int bFirst=FALSE);			//构造版面
	void	ConstructNewPad();				

	void	SetUnitReadAndPaint(int i,CRect rc,int flag);
	void	ResizePad(int flag=COM_OnlyDraw,int nNewGP=-1);		//排列版面
	void	SwitchPad(DWORD tmpNowPad,BOOL bLowHQPart=FALSE,BOOL bLevel2=FALSE);	//切换版面

	int		GetNextPageGP(short *lpnIndex,short num,BOOL bNext,int &tmpNo);
	int		GetOne2MulGp(short *lpnIndex,short num,BOOL ToMul);

	int		HitTest(CPoint point);
	int		OnMulUnitClick(CPoint point);
	BOOL	ZoomToOne(int nWhich,BOOL bZoom=TRUE);

	void	SetCurGPAndPad(const char *Code,short setcode,DWORD Pad=0);
	DWORD	GetPadFromMessageCode(int MessageCode,const char *Code=NULL,short setcode=0);

	void	ReadAndPaint();
	void	OnReadAndPaint(WPARAM wParam,LPARAM lParam);
	BOOL	Proc_Special_MessageCode(int tmpMessageCode);

	int		GetCurSwitch(DWORD AssisPad);
	DWORD	GetAssis1FromSwitch(int nWhich);
	DWORD	GetAssis2FromSwitch(int nWhich);
	void	AdjustLevel2Pad(DWORD &tmpPad,BOOL bProperty);
	BOOL	IsSpecAssisPad1(DWORD AssisPad1);

	BOOL	InSplitPos1(CPoint point);
	BOOL	InSplitPos2(CPoint point);

	int		GetZstWidth();
	int		GetZstHeight();
	void	SetSwitchPos();

	void	GetMainArea(int nFixWid,CRect &rc,CRect &DComRc,CRect &SeatRc,BOOL bShowBSUnit,CRect &MoreRc,BOOL bShowMoreUnit);
	void	GetMainAreaRect(CRect &rc,CRect &DComRc,CRect &SeatRc,CRect &MoreRc);
	void	GetUnit1Area(BOOL bProperty,CRect &rc);
	void	GetCornerUnitArea(BOOL bProperty,CRect &rc);
	void	GetAssis2UnitArea(CRect &rc,CRect &rc2);
	void	GetVArea(int nFixWid,CRect &rc);
public:
	int		GetCurrentGPIndex();
	short	GetCurrentGPSet(short *pPageGPIndex);
	int		GetGP_NeedRefreshStatHQ(short *lpnStkIndex,int nMaxNum,int nType);
	DWORD	GetNowPad() { return m_nNowPad; }
	BOOL	GetLastShow(){ return m_bLastShow; }
	int		GetCurrPeriod() { return m_nFxPeriod; }
	void	SetCurrPeriod(int nFxPeriod) { m_nFxPeriod = nFxPeriod; }
	void	SetPageGP(short *lpnIndex,short nTotalGP,short nCurGP,LinkedSetInfo *pLSetInfo=NULL);
	BOOL	IsInPageGP(short setcode,const char *code);
	int		EscHandle();
	
	BOOL	ChangeMultiGP(const char* Code,short setcode, int nXH=-1);

	BOOL	IsHQUnitShow() { return m_bNowShow;}
	short	GetDComGP() {return m_nDComCurGP;}
	DWORD   GetMainPad() { return m_nMainPad; }

	void	JumpToZB(char *ZBCode, int nTarRegion=-1);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CComView();
protected:
	afx_msg void OnWritetotxtfile();
	afx_msg void OnMulUnit();
	afx_msg void OnUpdateMulUnit(CCmdUI *pCmdUI);
	afx_msg void OnMulPeriod();
	afx_msg void OnUpdateMulPeriod(CCmdUI *pCmdUI);

	afx_msg void OnFxtMove();
	afx_msg void OnUpdateFxtMove(CCmdUI* pCmdUI);

	afx_msg void OnZstPlay();
	afx_msg void OnUpdateZstPlay(CCmdUI* pCmdUI);

	afx_msg void OnPlayIt();
	afx_msg void OnUpdatePlayIt(CCmdUI *pCmdUI);

	afx_msg void OnCornerClick(UINT);
	afx_msg void OnCorner2Click(UINT);
	afx_msg void OnCorner3Click(UINT);
	afx_msg void OnCornerVClick(UINT);
	afx_msg void OnMoreShow();

	afx_msg void OnZoomCorner();
	afx_msg void OnZoomToCorner();

	afx_msg void OnGoBack();
	afx_msg void OnUpDateGoBack(CCmdUI *pCmdUI);
	afx_msg void OnGoNext();
	afx_msg void OnUpDateGoNext(CCmdUI *pCmdUI);

	afx_msg void OnVScrollMessage(WPARAM wParam,LPARAM lParam);

	afx_msg void OnUpdateOtherFx(CCmdUI* pCmdUI);
	
	afx_msg	void OnLevinMenu(UINT nID);

	//{{AFX_MSG(CComView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHideright();
	afx_msg void OnZSTLevel2Face();
	afx_msg void OnUpdateZSTLevel2Face(CCmdUI *pCmdUI);
	afx_msg void OnFXTLevel2Face();
	afx_msg void OnUpdateFXTLevel2Face(CCmdUI *pCmdUI);
	afx_msg void OnTick2Priority();
	afx_msg void OnUpdateTick2Priority(CCmdUI *pCmdUI);	
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSwitchlbmmld();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPageup();
	afx_msg void OnPagedown();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDcomlayer();
	afx_msg void OnUpdateDcomlayer(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_COMVIEW_H__65DE8561_CB93_11D4_BB9D_00E04C66B1EB__INCLUDED_)

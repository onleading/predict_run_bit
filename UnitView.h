#ifndef _UnitView_h
#define _UnitView_h

#include "macro.h"
#include "struct.h"
#include "globalvar.h"
#include "ComDoc.h"
#include "custompad.h"
#include "UFxt.h"
#include "UZst.h"
#include "UHq.h"
#include "UFjb.h"
#include "UFsb.h"
#include "UTick.h"
#include "UCdp.h"
#include "UMmp.h"
#include "UCb.h"
#include "UBigVol.h"
#include "PopPreViewTool.h"
/////////////////////////////////////////////////////////////////////////////

enum
{
	U_SHOW,
	U_HIDE,
};

enum
{
	COM_OnlyDraw,
	COM_ReadDraw,
	COM_OnlyRead,
	COM_OnlySize,
};

#define FocusExistAndShow (m_nF>=0 && m_Units.pUnit[m_nF] && m_Units.nStatus[m_nF]==U_SHOW)

class CUnitView : public CView
{
protected:
	CUnitView(BOOL bIsComView2=FALSE);

	virtual ~CUnitView();

	BOOL	m_nTQState;		//复权状态
	UNITS	m_Units;		//存放各Units信息的结构

	int		m_nFxPeriod;	//分析图的周期
	int		m_nOften301;	//分析图的常用22种分析指标

	CRect	m_Rect;			//本ComView的区域

	short	WindowMacro;	//处理请求时的窗口宏
	short	m_nMainID;		//远程使用
	struct	CurrStockData_Code m_MulHQData[MULHQ_WANTNUM];

	BOOL	m_bAutoPage;	//是否自动换页
	BOOL	m_bQuitting;	//退出标志
	BOOL	m_bLayoutFlag;	//窗口是否就绪
	BOOL	KeyGuyFlag;

	CPopPreViewTool m_PreviewTool;

	//打印
	CDC * m_pMemDC;  //A memory device context compatible with our printer DC.
	CRect m_PrintRect;    //To hold the dimensions of our printing area while scaling.
	CBitmap * m_pBm; //Capture the screen image as a Bitmap

public:
	virtual int	GetCurrentGPIndex() { return -1; }
	void	SetMainID();
	void	DestroyPad();
	void	ShowPopPreviewTool(int nUnitType, int nUnitStyle, CRect &rc, MemStkInfo *pStock);
	void	HidePopPreviewTool()
	{
		m_PreviewTool.Pop();
	}
	BOOL	m_bIsComView2;
	int		m_nF;			//当前焦点状态
protected:
	void	GetOverlapCodeHead(int nWhich,char *CodeHead,int &nCount,BOOL bDSCode=FALSE);
	void	GetQZRelationCodeHead(int nGP,char *CodeHead,int &nCount);
public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	afx_msg void OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel);
	DECLARE_EVENTSINK_MAP()
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual	BOOL	TestUnitProperty(int iUnit, UINT nFlag) = 0;

protected:
	afx_msg void OnChooseFP(UINT nID);
	afx_msg void OnUpdateChooseFP(CCmdUI *pCmdUI);
	afx_msg void OnAddFmlPack();
	afx_msg void OnFmlPack2Stk();
	afx_msg void OnFmlPackUnlock();
	afx_msg void OnUpdateFmlPackUnlock(CCmdUI *pCmdUI);
	afx_msg void OnFmlPackMng();
	
	afx_msg void OnHistoryDrawType(UINT nID);
	afx_msg void OnUpdateHistoryDrawType(CCmdUI *pCmdUI);
	
	afx_msg void OnTpAxis();
	afx_msg void OnUpdateTpAxis(CCmdUI *pCmdUI);
	afx_msg void OnTabKey();

	afx_msg void OnDrawLineTools(UINT nID);
	afx_msg void OnUpdateDrawLineTools(CCmdUI* pCmdUI);
	
	afx_msg void OnShowPeriod();
	afx_msg void OnUpdateShowPeriod(CCmdUI *pCmdUI);
	afx_msg void OnShowWinNum();
	afx_msg void OnShowTQ();
	afx_msg void OnUpdateShowTQ(CCmdUI *pCmdUI);
	afx_msg void OnManyDayZstMenu();
	afx_msg void OnUpdateManyDayZstMenu(CCmdUI *pCmdUI);

	afx_msg void OnInBlock();
	afx_msg void OnRightAddBlock();
	afx_msg void OnRightAddZxg();
	afx_msg void OnDelFromBlock();
	afx_msg void OnGPCoolInfo();
	afx_msg void OnTQ(UINT nID);
	afx_msg void OnUpdateTQ(CCmdUI* pCmdUI);
	afx_msg void OnPeriod(UINT nID);
	afx_msg void OnUpdatePeriod(CCmdUI* pCmdUI);
	afx_msg void OnSpecPeriod(UINT nID);
	afx_msg void OnUpdateSpecPeriod(CCmdUI* pCmdUI);

	afx_msg void OnWindowNum(UINT nID) ;
	afx_msg void OnUpdateWindowNum(CCmdUI* pCmdUI) ;
	afx_msg void OnFxtAxis(UINT nID) ;
	afx_msg void OnUpdateFxtAxis(CCmdUI* pCmdUI) ;
	afx_msg void OnShow(UINT nID) ;
	afx_msg void OnUpdateShow(CCmdUI* pCmdUI) ;
	afx_msg void OnOverlap() ;
	afx_msg void OnUnOverlap(); 
	afx_msg void OnZbOverlap() ;
	afx_msg void OnChangeindexparam() ;
	afx_msg void OnMenuZb(UINT nID) ;
	afx_msg void OnUpdateMenuZb(CCmdUI* pCmdUI);
	afx_msg void OnHxZbDlg();
	afx_msg void OnColor() ;
	afx_msg void OnChangeParam(WPARAM wParam, LPARAM lParam);
	afx_msg void OnModifyFormula() ;
	afx_msg void OnDelzb() ;
	afx_msg void OnIndexhelp(); 
	afx_msg void OnUpdateDeleteshow(CCmdUI* pCmdUI) ;
	afx_msg void OnDeleteshow() ;
	afx_msg void OnHisDlg();
	afx_msg void OnUpdateHisDlg(CCmdUI *pCmdUI);
	afx_msg void OnQjStat();
	afx_msg void OnAllQjStat();
	afx_msg void OnManyDayZst(UINT nID);
	afx_msg void OnUpdateManyDayZst(CCmdUI *pCmdUI);
	afx_msg void OnManyDayTickZst(UINT nID);
	afx_msg void OnUpdateManyDayTickZst(CCmdUI *pCmdUI);
	afx_msg void OnTickZstUndo();
	afx_msg void OnTickZst2Zst();
	afx_msg void OnDeloverlapgp();
	afx_msg void OnDeleteInfoDlg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnAutopage();
	afx_msg void OnUpdateAutopage(CCmdUI* pCmdUI);

	afx_msg void OnFilePrintPreview();
	
	afx_msg void OnAutoOverlap(UINT nID);
	afx_msg void OnUpdateAutoOverlap(CCmdUI* pCmdUI);

	afx_msg void OnGlHqSwitch(UINT nID);
	
	afx_msg void OnCancelMark(void);
	afx_msg void OnMarkStockFlag(UINT nID);
	afx_msg void OnUpdateMarkStockFlag(CCmdUI* pCmdUI);

	afx_msg void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	afx_msg void GetMainID(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
};

#endif

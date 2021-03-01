
// MainFrm.h : interface of the CMainFrame class
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__ADD98CEC_82F1_11D1_98A0_0088CC032229__INCLUDED_)
#define AFX_MAINFRM_H__ADD98CEC_82F1_11D1_98A0_0088CC032229__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyStatusBar.h"
#include "TrueColorToolBar.h"
#include "ConditionGPDlg.h"
#include "ChoiceBlockDlg.h"
#include "SimChoiceDlg.h"
#include "CleChoiceDlg.h"
#include "MySelGPDlg.h"
#include "PluginDlg.h"
#include "ParamTestDlg.h"
#include "TestZBDlg.h"
#include "DSZHSortDlg.h"
#include "blockcalcdlg.h"
#include "DataDigDlg.h"
#include "InBlockDlg.h"
#include "GpGxdDlg.h"
#include "CMYTabCtrlBar.h"
#include "FluxCountDlg.h"
#include "ShowURL.h"
#include "MyCaptionBar.h"
#include "DeepWarnDlg.h"
#include "WarnManager.h"
#include "KQSDlg.h"
#include "KQS2Dlg.h"
#include "SeatQueryDlg.h"
#include "UrgentListDlg.h"

struct NCButtonInfo
{
	int		nMinBoxLeft;
	BOOL	bMinDown;
	BOOL	bRestoreDown;
	BOOL	bCloseDown;
	int		bVitalDown;
	CRect	NcRect;
	BOOL	HasNcRect;
	int		nFirstVitalLeft;
	int		bAdvButtonDown;
};

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	void	EnableDocking(DWORD dwDockStyle,BOOL bMyDock=FALSE);
	void	RecalcLayout(BOOL bNotify);
	//实现全屏的函数
	BOOL	GetFullScreenSize(CWnd* pWnd, POINT& pt) const;
	void	EscapeFullScreen();
	////
	BOOL	OnCommCommand(int WindowType,int MessageNo);
	void	OnOPSYS(int treetype,int force_showtree=1);
	void	SwitchToRestSpace(int resttype);
	void	SwitchToWorkSpace(int treetype);
	void	FillGnTree();
	void	FillGPTree();
	void	FillZBTree();
	void	RefreshStatusBar();
	CWnd *  GetRestSpaceWnd(int gn_type);

	void	ShowTabCtrlBar(int nFlag,BOOL bShow);
	CMDIChildWnd* GetNextMDIChildWnd();
	CWnd*	GetNextMDIChildWnd_All();
	void	BringMDI2Top(char *SubTitleString);
	int GetCountCMDIChildWnds();

	void	JumpToZBIndex(char *ZBCode, short nSetcode, char *GPCode, BOOL bIsKLine=FALSE);

	HMENU	NewMenu(UINT MenuID);
	CMenu	m_Menu;

	CDSZHSortDlg	*m_pZHSortDlg;
	CSeatQueryDlg	*m_pHKSeatQueryDlg;
	//飞沫对话框
	CUrgentListDlg *m_pUrgentPopDlg;
// Attributes
public:
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CTrueColorToolBar	m_wndToolBar, m_PerToolBar, m_DrawToolsBar;
	CMyStatusBar		m_wndStatusBar;
	CMyTabCtrlBar		m_wndWorkspace,m_wndRestspace;
#ifdef OEM_NEWJY
	CMyCaptionBar	m_wndCaptionBar;
#endif

protected:
	struct NCButtonInfo m_NCInfo;

	short	m_nHqMinePos;	//行情雷请求的序号位置
	short   m_nHqMinePrenum;
	
	int		m_nFirstOptionPage;

	CImageList	  m_ImageList;

	static const DWORD dwDockBarMap[4][2];

	//用于实现全屏显示的成员变量
	CWnd*			m_pWndFullScreen;
	WINDOWPLACEMENT m_PreviousPlace;
	WINDOWPLACEMENT m_MainPreviousPlace;
	CRect			m_FullScreenPlace;
	BOOL			m_bHadToolBar,m_bHadStatusBar,m_bHadDialogBar;

	CWnd			m_wndMDIClient;
	CWnd*			m_pWndCurrentChild;

	CWarnGPDlg		*m_pWarnGPDlg, *m_pTJGPDlg;	//用来指向预警对话框的指针(非模态)
	CDeepWarnDlg	*m_pDeepWarnDlg;
	CMySelGpDlg		*m_pSelGP;
	CConditionGPDlg *m_pChoiceGPDlg;
	CChoiceBlockDlg	*m_pChoiceBKDlg;
	CSimChoiceDlg	*m_pSimChoiceDlg;
	CCleChoiceDlg	*m_pCleChoiceDlg;
	CPluginDlg		*m_pPluginDlg;
	CParamTestDlg	*m_pTJTestDlg;
	CParamTestDlg	*m_pJYTestDlg;
	CTestZBDlg		*m_pTestZBDlg;
	CDataDigDlg		*m_pDataDigDlg;
	CInBlockDlg		*m_pInBlockDlg;
	CGpGxdDlg		*m_pGpGxdDlg;
	CBlockCalcDlg	*m_pBlockCalcDlg;
	CFluxCountDlg	*m_pFluxCalcDlg;
	CShowURL		*m_pVideoDlg;
	CShowURL		*m_pWebWinDlg;
	CShowURL		*m_pIWContentDlg;
	CShowURL		*m_pPopMyIEDlg;

	CWarnManager	m_WarnManager;
	short			m_nMainID;

	int				m_nInfoReconnect;

	HWND			m_hWndNextViewer;		//保存下一个剪贴板监控窗口
	BOOL			m_bClipSpy;

	int				m_nWarnReq;
	int				m_nAdPos;
			
	HICON			m_hIcon;

	CNewMenu		*m_pTabViewMenu;
	
	BOOL			m_bJyVerify_Success;
	BOOL			m_bAutoDS;
	
	BOOL			m_bStaticToolBar;

	BOOL			m_bTopMost;

	HANDLE			m_hAutoCalcBkThread;
	
public:
	BOOL			m_bPadMenuShow;
////
private:
	void	InitCreateInfo();
	void	TestAutoDS();
	BOOL	bCustDrawCloseBtn();
	void	DockControlBarLeftOf(CToolBar* Bar, CToolBar* LeftOf);
	void	CreateQsDetectThread();
	void	ConstructCustomPad(int padtype);
	void	OnMainWndMessage(int MessageNo);
	void	OnF5();

	BOOL	DownOrUpIt(int which,CPoint point,BOOL bDown);
	void	LogoDlg_ReConnect();
	void	CloseMDIWindow(BOOL bKeepSpec=TRUE);
	short	GetCfgWTPath(CString cfgPath,char *WTPath, char *pDefPath);
	void	NotifyMessage(DWORD dwMessage,HICON hIcon);
	void	SetAllButtons();
	void	LoginEmbedWT();
	void	SwitchToFirstView();
	//画框架元素
	void	_OnNcPaint();
	CRect	_AdvButtonOffset(CRect rect,int x_right);

	void	ConnectHqHost(BOOL bDisableNLB=FALSE, BOOL bReInit=FALSE);
	void	RecalcLayoutStatusRoll();
public:
	void	ProcessNewUser(char *newuser);
	void	CancelModalWindows();	
	void	CancelAllCaptionStatus();

	void	NormalWt();
	void	NonNormalWt();
	
	void	AfterLoadZXG();
// Generated message map functions
protected:
	afx_msg void OnCorpWeb();
	afx_msg void OnWebPageMenu(UINT nID);
	afx_msg void OnWebPageMenu2(UINT nID);
	afx_msg void OnETF();
	afx_msg void OnETFCommand(UINT nID);

	afx_msg void OnConnectInfo();
	afx_msg void OnUpdateConnectInfo(CCmdUI* pCmdUI);
	afx_msg void OnDisConnectInfo();
	afx_msg void OnUpdateDisConnectInfo(CCmdUI* pCmdUI);
	afx_msg void OnSilentDisconnect();
	afx_msg void OnTreeViewMessage(WPARAM wParam, LPARAM lParam);

	afx_msg void OnBigFontGrid();
	afx_msg void OnLockRight();
	afx_msg void On6787();
	afx_msg void OnZxg();
	afx_msg void OnDpFx(UINT nID);
	afx_msg void OnOtherFx(UINT nID);
	afx_msg void OnTdxJBZL();
	afx_msg void OnChangeReport(UINT nID);
	afx_msg void OnBJFX(UINT nID);
	afx_msg void OnMenuBlockClick(UINT nID);
	afx_msg void OnFyReport(UINT nID);
	afx_msg void OnRank(UINT nID);
	afx_msg void OnMulGpType(UINT nID);

	afx_msg void OnZXCommand(UINT nID);
	afx_msg void OnCustomPad(UINT nID);
	afx_msg void OnRecentPad();
	afx_msg void OnQSCusWebPage();
	afx_msg void OnCusWebPage(UINT nID);
	afx_msg void OnHKeyProgram();
	afx_msg void OnF11WebPage();
	afx_msg void OnDownLoad();
	afx_msg void OnUpdateDownLoad(CCmdUI *pCmdUI);
	afx_msg void OnNodeTool();
	afx_msg void OnUpdateNodeTool(CCmdUI *pCmdUI);
	afx_msg void OnDayWriteDisk();
	afx_msg void OnUpdateDayWriteDisk(CCmdUI *pCmdUI);
	afx_msg void OnRightClickLeft(UINT nID);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetDataAck(WPARAM wParam,LPARAM lParam);
	afx_msg void OnCommDisConnect(WPARAM wParam,LPARAM lParam);
	afx_msg void OnHqConnectTimerMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnNcPaint ();
	afx_msg void OnNcLButtonDown( UINT nHitTest, CPoint point );
	afx_msg void OnClose();
	afx_msg void CloseallAndKeepSpec();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void SendStatusReq(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnHotKey(WPARAM wParam,LPARAM lParam);
	afx_msg void OnHqTabOption();
	afx_msg void OnBlockOption();
	afx_msg void OnJyOption();
	afx_msg void OnUpdateJyOption(CCmdUI *pCmdUI);
	afx_msg void OnOption();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnFullscreen();
	afx_msg void OnUpdateFullscreen(CCmdUI* pCmdUI);
	afx_msg void OnTopMost();
	afx_msg void OnUpdateTopMost(CCmdUI* pCmdUI);
	afx_msg void OnCoolinfo();
	afx_msg void OnNews();
	afx_msg void OnEscape();
	afx_msg void OnAllWarngp();
	afx_msg void OnTjWarngp();
	afx_msg void OnDeepWarn();
	afx_msg void OnUpdateDeepWarn(CCmdUI *pCmdUI);
	afx_msg void OnAllWarnSwitch();
	afx_msg void OnAllWarncfg();
	afx_msg void OnUpdateWarnSwitch(CCmdUI* pCmdUI);
	afx_msg void OnTjWarnSwitch();
	afx_msg void OnTjWarncfg();
	afx_msg void OnUpdateTjWarnSwitch(CCmdUI* pCmdUI);
	afx_msg void OnConnectHQ();
	afx_msg void OnFuncWt();
	afx_msg void OnFuncEmbWT();
	afx_msg void OnFuncAddedWt1();
	afx_msg void OnFuncAddedWt2();
	afx_msg void OnFuncHxInfo();
	afx_msg void OnFuncZxjtInfo();
	afx_msg void OnUpdateExternInfo(CCmdUI *pCmdUI);
	afx_msg void OnExternInfo();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void CancelNoModal(WPARAM wParam,LPARAM lParam);
	afx_msg void OnGetgp();
	//与专家系统有关的函数
	afx_msg void OnTJTest();
	afx_msg void OnJYTest();
	afx_msg void OnTestindex();
	afx_msg void OnConditionChoice();
	afx_msg void OnUpdateChoicegp(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGPGradeDlg(CCmdUI *pCmdUI);
	afx_msg void OnTdxReport();
	afx_msg void OnGuba();
	afx_msg void OnUpdateTdxReport(CCmdUI *pCmdUI);
	afx_msg void OnClechoice();
	afx_msg void OnUpdateClechoice(CCmdUI* pCmdUI);
	afx_msg void OnSimchoice();
	afx_msg void OnSelGP();
	afx_msg void OnUpdateSelGP(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSimchoice(CCmdUI* pCmdUI);
	afx_msg void OnFormula();
	afx_msg void OnUpdateFormula(CCmdUI* pCmdUI);
	
	afx_msg void OnZXGOpr();
	afx_msg void OnTdxChoiceGp();
	afx_msg void OnGPFL();
	afx_msg void OnGPSort();
	afx_msg void OnGPZhSort();

	afx_msg void OnPlugin();
	afx_msg void OnUpdatePlugin(CCmdUI *pCmdUI);
	//
	afx_msg void OnStarGraph();
	
	afx_msg void OnHideShowButtonMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnToolbarDropDown(NMTOOLBAR* pnmh, LRESULT* plRes);
	afx_msg void OnFuncMock();
	afx_msg void OnUpdateFuncMock(CCmdUI *pCmdUI);

	afx_msg void OnWideDataHQ();
	afx_msg void OnWideDataSort(WPARAM wParam,LPARAM lParam);
	afx_msg void OnWideDataStatHQ(WPARAM wParam,LPARAM lParam);
	
	afx_msg void OnSyncData();

	afx_msg void OnConnect();
	afx_msg void OnBGConnect();
	afx_msg void OnReInitConnect();
	afx_msg void OnUpdateConnect(CCmdUI* pCmdUI);
	afx_msg void OnDisconnect();
	afx_msg void OnUpdateDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnConnectCfg();

	afx_msg void OnHelpFinder();
	afx_msg void OnWebInfo();
	afx_msg void OnUpdateWebInfo(CCmdUI *pCmdUI);
	afx_msg void OnAdditonZXURL();
	afx_msg void OnUrgentDlg();	
	afx_msg void OnVideo();

	afx_msg void OnAutoConnectMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnConnectResult(WPARAM wParam,LPARAM lParam);

	afx_msg void OnF9();
	afx_msg void OnGgzl();
	afx_msg void OnJbfx();
	afx_msg void OnGPCoolInfo();
	afx_msg void OnJiangEn();
	afx_msg void OnMulUnit();
	afx_msg void OnMSSR();
	afx_msg void OnBigVol();
	afx_msg void OnBuyDS();
	afx_msg void OnGotojbzl();
	afx_msg void OnDSMarket(UINT nID);
	afx_msg void OnFYGrid(UINT nID);
	afx_msg void OnWorkBarCheck(UINT nID);
	afx_msg void OnRestBarCheck(UINT nID);
	afx_msg void OnStatusBarCheck(UINT nID);
	
	afx_msg void OnClipSpy();
	afx_msg void OnUpdateClipSpy(CCmdUI* pCmdUI);
	
	afx_msg void OnShowMessage(WPARAM wParam,LPARAM lParam);
	afx_msg void OnRollInfoPopMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnSlowRollInfoPopMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnCloseRollMsg(WPARAM wParam,LPARAM lParam);
	afx_msg void OnCloseSlowRollMsg(WPARAM wParam,LPARAM lParam);

	afx_msg void OnSwitchPadMenu(UINT nID);

	afx_msg void OnOpenWebWin(WPARAM wParam,LPARAM lParam);	
	afx_msg	void OnLineEnterMsg(WPARAM wParam,LPARAM lParam);
	afx_msg	void OnProcessAutoupInfo(WPARAM wParam,LPARAM lParam);

	afx_msg void OnPopUrgent();
	afx_msg void SendMoreInfoReq(WPARAM wParam,LPARAM lParam);

	afx_msg void OnOpenIWContent(WPARAM wParam,LPARAM lParam);
	afx_msg void OnPopMyIE(WPARAM wParam,LPARAM lParam);

	afx_msg	void OnAutoCalcTjBlock();
	afx_msg	void OnCLXG();

	afx_msg void OnPopMsg(WPARAM wParam,LPARAM lParam);
	//{{AFX_MSG(CMainFrame)
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnSysPad();
	afx_msg void OnUpdateSysPad(CCmdUI *pCmdUI);
	afx_msg void OnShowhq();
	afx_msg void OnShowzst();
	afx_msg void OnShowvip();
	afx_msg void OnIdsTipoftheday();
	afx_msg void OnMinimize();
	afx_msg void OnRestore();
	afx_msg void OnNcRButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnHKSeatQuery();
	afx_msg void OnZHSort(UINT nID);
	afx_msg void OnUpdateZHSort(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZHSortStyle(CCmdUI* pCmdUI);
	afx_msg void OnFuncBlockCalc();
	afx_msg void OnDataDig();
	afx_msg void SearchBlock(WPARAM wParam,LPARAM lParam);
	afx_msg void SearchGpGxd(WPARAM wParam,LPARAM lParam);
	afx_msg void ProcessSwitchCallBack(WPARAM wParam,LPARAM lParam);
	afx_msg void OnWebUserCommand(UINT nID);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnAutoup();
	afx_msg void OnSpeedTest();
	afx_msg void OnForum();	
	afx_msg void OnShowInfo();
	afx_msg void OnRollInfoClose();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnNewpad();
	afx_msg void OnLocalIndex();
	afx_msg void OnLocalExtern();
	afx_msg void OnLocalPtStk();
	afx_msg void OnInitMenuPopup(CNewMenu *pPopupMenu,UINT nIndex,BOOL bSysMenu);
	afx_msg void OnSetdefcuspad();
	afx_msg void OnUpdateSetdefcuspad(CCmdUI* pCmdUI);
	afx_msg void OnShowCustomPadSwitch();
	afx_msg void OnUpdateShowCustomPadSwitch(CCmdUI* pCmdUI);	
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnUpdateBigFontGrid(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLockRight(CCmdUI* pCmdUI);
	afx_msg void OnHszs();
	afx_msg void OnShgzzs();
	afx_msg void OnNetfluxcount();
	afx_msg void OnHkJyTime();
	//}}AFX_MSG
	afx_msg void OnBuy(UINT nID);
	afx_msg void OnSell(UINT nID);
	afx_msg void OnLevinBuy(UINT nID);
	afx_msg void OnLevinSell(UINT nID);
	afx_msg void OnWTCancel();
	afx_msg void OnUserCenter();
	afx_msg void OnQzSDS();
	afx_msg void OnRelativeInfo();
	afx_msg void OnPriceWarn();
	afx_msg void OnConInfo();
	afx_msg void OnMarkMgn();

	afx_msg void OnFundHold();
	afx_msg void OnWriteUrgent();
	afx_msg void OnShowUrgentMsg(WPARAM wParam,LPARAM lParam);
	
	afx_msg void OnAutoConnectDS(WPARAM wParam,LPARAM lParam);
	afx_msg void OnInitDS();
	afx_msg void OnUpdateInitDS(CCmdUI *pCmdUI);
	afx_msg void OnUnInitDS();
	afx_msg void OnUpdateUnInitDS(CCmdUI *pCmdUI);

	afx_msg void OnSpecailZX(WPARAM wParam,LPARAM lParam);
	afx_msg void SearchHKSeat(WPARAM wParam,LPARAM lParam);
	
	afx_msg void OnPadList();
	afx_msg void OnPageup();
	afx_msg void OnPagedown();

	afx_msg void OnJCTZBSetChange(UINT nID);
	afx_msg void OnZFJS_Cmd(UINT nID);
	afx_msg void OnUpdate_ZFJS_Cmd(CCmdUI* pCmdUI);	

	afx_msg void OnDefTool_Cmd(UINT nID);
	afx_msg void OnUpdate_DefTool_Cmd(CCmdUI* pCmdUI);	

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__ADD98CEC_82F1_11D1_98A0_0088CC032229__INCLUDED_)

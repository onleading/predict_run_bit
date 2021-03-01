#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "resource.h"
#include "ViewManager.h"
#include "ToolTipMark.h"
#include "Splash.h"
#include "cf.h"
#include "Plugin_gn.h"
#include "BkData.h"
#include ".\Bi\HqSrcMgn.h"

extern HINSTANCE g_resModule;

extern	CGNPlugin	g_GNPlug;

extern char		g_szReleaseVersion[10];
extern char		g_szMainExeVersion[10];
extern char		g_szDSExeVersion[10];

extern	UINT	UM_MINEMSG;
extern	UINT	UM_GGBWMSG;
extern	UINT	UM_FLZLMSG;
extern	UINT    UM_HQ_MINE_MSG;
extern	UINT	UM_CJZXMSG;

extern	const TCHAR szMainFrameClass[];

#include "HQComm.h"
#include "VUserComm.h"
#include "getdata.h"
extern class CHQComm	  * g_pComm, * g_pComm3;
extern class CGetData * g_pGetData;

extern int  RECENTNUM;
extern int	g_RMSp;
extern int	g_nLinkage;

extern BOOL	g_DDEDataNeedLoad;

//控制版本，主要改下列几个全局变量
extern int		g_nSysMode;
extern BOOL		g_bDemoVip;

extern BOOL		g_bHasDS;
extern BOOL		g_bZBEditable;

extern long		g_nDSAutoStopMinute;
extern long		g_nDSAutoStopTimer;
extern long		g_nDSCheckHostMinute;

extern BOOL g_bLongDSName;
extern BOOL OnLine;
extern BOOL	DSOnLine;
extern BOOL	OffLineMode;

extern BOOL   g_bAllWarnSwitch, g_bTjWarnSwitch;
extern BOOL   g_bNotifyDownload;
extern BOOL	  g_bDay_WriteDisk;	//日线自动写盘

extern BOOL  g_bDrawBlue;

extern long		g_nDefUserLevel;	//用户等级
extern CString g_strPreName;	//产品名称前缀
extern CString g_strTitleName;	//产品名称
extern CString g_strAdName;
extern CString g_strMoving;

extern BOOL	  g_bInsideOpenPage;//在程序内部界面打开链接
extern BOOL   g_bExtern_Info;	//是不是外挂资讯
extern BOOL   g_bExtern_Invest;	//是不是外挂个人理财

extern short  g_nSwitchMainID; //用于Unit得到ComView或HisDlg的主ID

extern char  g_strMovingZXG[MAXGPNUM][SH_CODE_LEN+10];
extern short g_nMovingZXG;

extern BOOL  LayoutFlag;
extern int	 g_nMinnNum,g_nDaynNum;
extern NTime g_NoTime; 

extern struct AnalyData g_SyncAnalyData;
extern float  g_SyncClose;
extern int	  g_nGzPerMin;

extern int	 g_nWhichHisSwitch;
extern BOOL  g_bVolTQ;

class  DTopViewCalc;
extern DTopViewCalc	* g_pCalc, *g_pExtCalc;

extern UINT  g_WTMsgID,g_WTMsgID2;
extern UINT  g_uRefreshAllHQ;
extern UINT  g_uCloseMsg;
extern BOOL  g_bSpecClose;		//是不是由外部程序引起的
extern BOOL  g_bDirectQuit;
extern BOOL  g_bReLoginStatus;
extern BOOL  g_nRollInfoHeight;	//滚动资讯的高度

extern long   g_lOpenRq;

extern BOOL   g_bDonotClose;
class CZnxg;
extern CZnxg *g_pZnxg;
extern CLESIGN g_pXGSign;

extern BOOL		g_bSepEsc;
extern BOOL		g_bOldGridDomain;

//涨速周期
extern int g_nZFFzNum;
//本机网卡地址
extern CString g_NetCardAddress;
//用于板块股
extern  BlockInfo_Struct g_BlockInfo[USERBLOCK_NUM];
extern  int		g_nBlockNum;
//用于概念板块股
extern  MEM_BLOCK g_ConceptBlock[100];
extern  int		  g_ConceptBlockNum;
//用于组合板块股
extern  ZHB_BlockInfo_Struct *g_ZHBBlock;
extern  int					 g_ZHBBlockNum;
//板块绑定颜色
extern  char		g_ColorBlockGp[8][MAXGPNUM][SH_CODE_LEN+10];
extern  int			g_ColorBlockGpNum[8];
//
extern  BLOCKCOLOR	g_BlockColor[8];

//用于大市场选择
extern  short   fztb[4];

extern int ScreenCx,ScreenCy;
extern char *DpGpCode[];

extern  class StockDataIo	*g_pStockIO;
extern  class LcIdxMng		*g_pLcIdxMng;
extern  class ExdayMng		*g_pExdayMng;
extern  class ConstMng		*g_pConstMng;

extern CFGINFO  g_TdxCfg;
extern short	g_WSXH;
extern CString	g_WSXHStr;
extern CTime	g_tNowTime;
extern long		g_iToday;
extern int      g_bYesterdayFlag;

extern short anFZRatio[10];
extern short sz[8];
extern short sh[8];
extern char  lpszTimeStr[12][6];

extern char g_DefCusPadName[];	
extern vector<DynaCmdTable> g_vDynaCmdTab;
extern BOOL	g_bFirstCusPad;
extern struct CmdTable g_pCmdTable[];
extern int g_nCmdTable;

extern WORD	  FuncMessageCode[][2];
extern struct FirstLevel FuncLevel1[];
extern char   *FuncLevel2[];

extern int		lpnSysDomainType[];
extern short	lpsSysClassType_new[];
extern char		*GPType[];

extern char		lpszRMItemName_All[][20];
extern float	lpnRMItemWidth_All[];
extern float	lpnRMItemWidth_Long_All[];
extern int		nColToXh_Hq[10];
extern int		nColToXh_Zj[10];

extern char		lpszZBItemName[][20];  
extern float	lpnZBItemWidth[];

extern char		lpszQRItemName[][20];
extern float	lpnQRItemWidth[];	
extern char		lpszQJItemName[][20];
extern float	lpnQJItemWidth[];

extern int		naPriceExtent[];	//涨跌幅度
extern int		naPopPoint[];		//换手率排名
extern int		naVolumeExtent[];	//量变幅度
extern int		naDiffExtent[];		//震荡幅度

extern char		*QJTypeStr[];
extern char		*AmiComboStr[8];

extern BOOL bStartTips;
extern int nTipFilePos;

extern HWND FirstWindow,BigMenuWindow;

extern CViewManager theViewManager;

extern struct GRIDCOLOR GridColor;
extern struct VIPCOLOR	VipColor;
extern struct TXTCOLOR  TxtColor;
extern struct TREECOLOR	TreeColor;
extern struct SYSTEMCOLOR  SystemColor;

extern struct MENUCOLOR	MenuColor;

extern int		CurrentScheme;

extern CString HomePath;

extern int AutoPageTime;
extern int HQRefreshTime;
extern int StatusRefreshTime;

extern BOOL g_bAutoConnectInfo;	//启动后自动登录资讯
extern BOOL g_bAutoLogo;		//启动后自动登录行情
extern BOOL g_bAutoDS;			//启动后自动登录扩展行情

extern BOOL g_bUseAutoConnect;	//是否使用自动重连
extern int	g_nAutoConnectFlag;
extern int	g_nDSAutoConnectFlag;
extern int	g_nAutoConnectNum;	//自动重连的次数
extern BOOL g_bHasTdxProxy;		//是否有ISA代理

extern BOOL bTreeShow;
extern BOOL bRestShow;
extern int	nLastTreeSel;
extern int	nLastRestSel;
extern BOOL bToolBarShow;
extern BOOL bStatusBarShow;
extern BOOL g_bRightShow;

extern CString FirstBlockName;
extern int FirstGPType;
extern int FirstBlockFlag;
extern CString FirstHisBlockName;
extern int FirstHisGPType;
extern int FirstHisBlockFlag;

extern int g_CurrentJBM;	//当前用户最喜欢的基本资料

extern CString TxtCurGPCode;
extern short   TxtCurSetCode;
extern CString ComCurGPCode;
extern short   ComCurSetCode;
extern CString WebInfoCurGPCode;
extern short   WebInfoCurSetCode;
extern CString WebURL;
extern CString CjzxCurGPCode;
extern short   CjzxCurSetCode;
extern CString JbfxCurGPCode;
extern short   JbfxCurSetCode;


extern int	   HisCurType;;

extern int XXMNum;
extern int XXMType[20];
extern CString XXMTopic[20];

extern struct SYSTEMCOLOR Scheme[];
extern struct SYSTEMCOLOR Scheme_Hold;
extern CString SchemeStr[];
extern int SchemeNum;

extern struct SchemeItemContent theItemContent[];
extern char *g_CalcPeriodStr[];

extern int	IndexSortNum;					
extern int CurIndexSort;
extern struct IndexSortInfo_Struct IndexSortInfo[4];

extern int QRParam[5];

extern WARN_STRU	g_WarnBuffer[WARN_BUFFER_SIZE], g_TJWarnBuffer[WARN_BUFFER_SIZE];
extern int			g_nWarnNum, g_nTJWarnNum;
extern int			g_nWarnRangeMinute;
extern deque<WARN_STRU> g_TimeRangeWarnBuffer;
extern int				g_nWarnHQPos;

extern WARNGLOBALPARAM	g_WarnParam;
extern WARNGLOBALPARAM2	g_WarnParam2;
extern TJ_WARN			g_aWarnTjEx[MAX_TJWARN];
extern short			g_aWarnGpSet[MAXGPNUM];
extern int				g_nWarnGpNum;
extern PWARNINGCND		*g_pAbsCondition;
extern int	g_pAbsConditionNum;
extern long	g_nUnitRadarFlag;
extern int  g_nWarnRefresh;
extern long	g_nUnitDeepWarnFlag;

//用来实现拖放的光标
extern HCURSOR		m_hCanPressB,m_hCanPressS,m_hCanPress,m_hCanDrop,m_hCanDrop_Overlap,m_hCannotDrop,m_hZoomCur,m_hZoomCur2,m_hZoomCur3,m_hVerSplit,m_hHorSplit,m_hMove,m_hCursor2,m_hDraw,m_hDrawMove,m_hDrawTurn,m_hFxtMove,m_hToBig,m_hToSmall;
extern HICON		m_hExpUpShow,m_hExpDownShow,m_hKLineShow,m_hExpUpShow_Sel,m_hExpDownShow_Sel,m_hKLineShow_Sel,m_hGBBQShow,m_hGGBWShow,m_hMoreThunder,m_hOneThunder,m_hMyThunder[6],m_hIcon_Pu[8],m_hFCIcon,m_hZstFlushIcon;
extern HICON		m_hRedUP,m_hRedDown,m_hGreenUP,m_hGreenDown,m_hIconUp,m_hIconDown,m_hIconPBuy[3],m_hIconPSell[3];

extern char g_cwAddrTopic[][10];
extern char g_pDyBlockPy[TOTAL_DY_NUM][5];
extern char g_cwHYstr[][9];
extern char g_pHyBlockPy[TOTAL_HY_NUM][5];

extern CLE_CONTABLE g_pCleKLine1[];
extern CLE_CONTABLE g_pCleKLine2[];
extern CLE_CONTABLE g_pCleKLine3[];
extern CLE_CONTABLE g_pCleKLine4[];
extern short TrendCodeTable[];
extern CLE_CONTABLE g_pCleJBM[];
extern CLE_CONTABLE g_pCleJSM[];

extern CUSTXG_CONTABLE g_nCustCon[];
extern char *BjCmpStr[];

extern float g_my_rate,g_gy_rate;

extern int ComCurMessageCode;

extern char	  WeekInfo[][10];
extern char * strWeekInfo[8];

extern int	g_InfoDlgPosx,g_InfoDlgPosy;
extern BOOL g_bInfoMinimize;

/////////////////////////////////////////////////////////////////////
#include "DDrawLineTool.h"
extern DDrawLineTool	g_DrawLineTool;
extern int			g_nTxtFontSize;
extern COLORREF		g_nTxtColor;

extern CustomFunc	g_CF;
extern FMLPACK		g_FmlPack[MAX_FMLPACK_NUM];
extern int			g_nFmlPack;
extern FML2STK		g_Fml2Stk[MAX_BANDSTOCK_NUM];
extern int			g_nFml2Stk;

extern Gdi		g_d;

extern int		g_nMulNum,g_nNewMulNum;
extern BOOL		g_bDynaZoom;
extern BOOL		g_bZBudFlag;
extern BOOL		g_bTPAxis;
extern BOOL		g_bLockRight;

extern INFODLG	g_DlgInfo;

extern CBINFO	g_CbInfo;

extern UINT		g_uBigToolBarBtn[100], g_uBigToolBarBtn_I[], g_uBigToolBarBtn_II[], g_uBigToolBarBtn_III[], g_uBigToolBarBtn_IIII[];
extern int		g_nBigToolBarBtn, g_nBigToolBarBtn_I, g_nBigToolBarBtn_II, g_nBigToolBarBtn_III, g_nBigToolBarBtn_IIII;

extern UINT	g_uFixedBtn[];
extern int	g_nFixedBtn;
extern UINT	g_uTotalDynBtn[];
extern int	g_nTotalDynBtn;
extern UINT	g_uDynBtn[][30];
extern int	g_nDynBtn[];

extern CString	g_TxtFindStr[MAX_ANDSTR];
extern int		g_nTxtFindNum;
extern CString	g_NewsFindStr[MAX_ANDSTR];
extern int		g_nNewsFindNum;
extern int		g_nTxtFindNo;
extern int		g_nNewsFindNo;
extern BOOL		g_nFindRadio;
extern int		g_nFixedWidth;
extern int		g_nFixedWidth2;

extern float	g_fProfitFee;

extern char		g_strOftenUseZb[][ZBCODE_LEN];
extern int		g_nOftenUseZb;
extern char		g_strFavUseZb[][ZBCODE_LEN];
extern char		g_strZbMain[ZBCODE_LEN];
extern char		g_strLastExp[ZBCODE_LEN];
extern char		g_strLastKLine[ZBCODE_LEN];
extern char		g_strLastZBSort[ZBCODE_LEN];

extern int		g_nMainZbFlag;
extern BOOL		g_bHasXHidden,g_bHasYHidden;
extern long		g_nHligtNowVol;

extern char		g_strFontName[11][40];
extern int		g_nFontSize[11];
extern int		g_nFontWeigth[11];
extern BOOL		g_bBigFontGrid;
extern CPoint	g_ZbDragPoint;

extern TINDEXINFO	g_SpeclIndex[];
extern int	g_nSpeclIndexNum;
////////////////////////////////////////////////////////////////////////
//					远程变量
////////////////////////////////////////////////////////////////////////
extern struct ManyHostInfo g_ManyHostInfo;
extern struct CurrStockData_Code g_ZSHQData[10];

extern BOOL		 g_bRiskPost;
extern BOOL		 g_bRestRightBtn;
extern BOOL		 g_bEveryFirst;	//每次重进系统选择第一个主站
extern BOOL		 g_bIsGuest;
extern char		 g_strLoginName[50];
extern char		 g_strPassword[50];
extern char		 g_L2UserName[50];
extern char		 g_L2Password[50];
extern int		 g_nL2Type;
extern BOOL		 g_bSaveUser, g_bSavePass;
extern TDXWHOST	 g_HQHost;
extern TDXWHOST	 g_VipHQHost,g_L2HQHost;
extern TDXWHOST	 g_WTHost;	
extern TDXWHOST	 g_DSHost,g_SSDSHost;
extern PROXYINFO g_ProxyInfo;
extern CString		g_HQIPStr,g_HQNameStr;
extern unsigned short g_HQPort;
extern CString		g_DSHQIPStr,g_DSHQNameStr;
extern unsigned short g_DSHQPort;
extern CString		g_HQTryOutIP;
extern unsigned short g_HQTryOutPort;

extern short g_nMainID;
extern short g_nAssisID;
extern char	g_nPriority;
extern char	g_pReceive[ANSBUFFER_LEN+20],g_pReceive3[ANSBUFFER_LEN+20];
extern ANSHEADER g_AnsHeader,g_AnsHeader3;

extern short	g_nWideDataMainID;

extern CSplashWnd * g_pSplashWnd;
extern CWnd *  g_pConnectWnd;
extern CWnd *  g_pMainWin;
extern CWnd *  g_pHisDlg;
extern CWnd *  g_pWideDataDlg;
extern CWnd *  g_pDownLoadDlg;
extern CWnd *  g_pTickZstjDlg;
extern CWnd *  g_pDataSyncDlg;

extern class CBkData * g_pBkData;

extern int		g_nDataSyncType;
extern CString	g_strDataSyncFile;
extern CWnd	*	g_pDSConnectWnd;
extern long		g_nLastQuerySeatID;

extern BOOL			g_bFirstStep;
extern int			g_nZBWinNum;
extern char			g_nUrgentNum;
extern struct HostList g_HqHostList;
extern char *LoginTypeStr[3];

extern BOOL	 g_bHasInfo;

extern long  g_lRefreshHQSeconds;
extern long  g_lRefreshStatHQSeconds;
extern BOOL  g_bDowning;
extern BOOL  g_bSelftPHing;
extern BOOL  g_bZnxg_Down;

extern long	 g_lQJBgnDate,g_lQJEndDate;
extern long	 g_nCurQJType;
extern BOOL	 g_bHisViewTQ;
extern int	 g_nToolBarPos;
extern short *g_Simhq2StkIndex;	//用于交换(请求完全行情的序号缓冲区)
extern short g_nSimhq2Stk;
extern BOOL	 g_bBlockModified;
extern short g_nMineNum[MAXGPNUM];		//雷的数目，按股票序号存放

extern BOOL  g_bNo003InAG;
extern BOOL  g_bSupportETF;
extern BOOL	 g_bSupportDownMin5;
extern BOOL  g_bSupportLevel2;
extern BOOL  g_bG2DT;
extern BOOL  g_bUseSpecVolumeUnit;
extern BOOL	 g_bShowHqMine;		//是否在行情表中显示信息雷
extern BOOL	 g_bLongHighLine;	//是否是长的高亮线

extern int	 g_nNetCondition;	//上网环境
//////////////////////////传统版///////////////////////////
extern BOOL	 g_bHasCaption;
extern int	 g_nMenuPeriod;
extern int	 g_nMenuSortType;
extern int	 g_nMul_WhichGPType;
extern int	 g_nMul_nBlockFlag;
extern int	 g_nMul_CurGP;

extern int	g_nMenuStack[20];
extern int	g_nMenuStackTop;

extern short g_awMenuStartNum[];
extern char g_cwMenuTopic[][80];
extern MENUITEMTDX  g_pMenu[];

extern int	g_nOverlapNum;					//走势图叠加的个数
extern MemStkInfo * g_pOverlapStkInfo[3];	//最多允许叠加三只股票
extern BOOL g_bManyDayZST,g_bHisManyDayZST;
extern int	g_nZSTManyDay,g_nHisZSTManyDay;	//多日走势图的天数
extern BOOL g_nTmpDirectConnectHq;
extern BOOL g_bShowVersoin;
extern char g_strLogoAdvert[200];
extern BOOL g_bDiffVolMode;

extern int  g_nTQFlag;
extern long g_nPartTq;
extern long g_nTqStart;
extern int	g_nMainAxis;
//
extern	const MAINLINE g_MainLineName[];
extern	const int g_MainLineNum;

extern WORD    g_virtkey;
extern WORD    g_comkey;
extern BOOL    g_hideinfo;
extern BOOL	   g_bBossKey;

extern int		g_nMenuMode;
extern BOOL		g_bExtendXPStyle; //是否是增强型的XP风格 
extern BOOL		g_bEnableClassXP;
extern BOOL		g_bClassXP;	//

extern BOOL		g_bMin3Tray;		//最小化到托盘

extern long  g_URLEndTime;			//URL结束时间
extern char  g_URLTitle[81];		//URL串的标题
extern char  g_URL[81];				//URL

extern float g_fJiangEnStep;
extern int   g_nJiangEnMode;		//0:阻力位 1:支撑位

extern BOOL  g_nCBSwitch;			//移动筹码分析的类型

extern unsigned long  g_LoginServerSeconds,g_LoginLocalSeconds,g_LoginServerDate;
extern long  g_LoginDSServerSubLocal;

extern int	 g_nWebPageNum;
extern char  g_strWebPageStr[40][50];
extern char  g_strWebPageURL[40][150];
extern char  g_strMainWebTitle[100];

extern int	 g_nWebPageNum2;
extern char  g_strWebPageStr2[20][50];
extern char  g_strWebPageURL2[20][150];
extern char  g_strMainWebTitle2[100];

extern struct tag_VitalBtn g_VitalBtn;
extern BOOL		g_bHasUserHost;
extern CString	g_strCheckUserURL;

extern BOOL		g_bHasWebInfo;
extern BOOL		g_bUserHostEverConnect;
extern BOOL		g_bHasAdvHQVersion;
extern int		g_nPersonalVersion;
extern BOOL		g_bJRSConnected;
extern BOOL		g_bHasTICQ;
extern BOOL		g_bAutoConnectTICQ;
extern BOOL		g_bTQAsBroker;

//////////////////////商业智能平台的变量/////////////////////////////////

extern BOOL  g_bHasF10Cfg;

extern BOOL g_bConnectInfoStatus;

extern char *SwitchUnit_ZstZS[3];
extern char *SwitchUnit_ZstGP[4];
extern char *SwitchUnit_ZstL2[11];

extern char *SwitchUnitStr[4];
extern char *SwitchUnitStr_Tips[4];
extern int	SwitchUnitNum;
extern char *SwitchUnitStr_Mid[7];
extern char *SwitchUnitStr_Mid_Tips[7];
extern int	SwitchUnitNum_Mid;

extern int   RsaBitsInt[5];
extern int	 KeyLenInt[5];

//多功能F10的分类信息
extern struct f10cfg_title g_F10Caption[MAX_F10CAPTION];
extern struct new_f10cfg_title g_NewF10Caption[MAX_F10CAPTION];
extern char	g_F10CaptionNum;

extern LOGININFO g_LoginInfo;

//外挂的行情接口信息
extern struct ETInfo g_ET;

////////////////////集成平台////////////////////////////////

extern CHATAREA g_ChatArea;

extern long	g_nTreePos[20];
extern long	g_nRestPos[20];

extern char g_UserHostIP[50],g_BakUserHostIP[50],g_BakUserHostIP2[50];
extern unsigned short g_nUserHostPort,g_nBakUserHostPort,g_nBakUserHostPort2;

extern char g_FHostIP[50];
extern unsigned short g_nFHostPort;

extern char		g_UserHostQSID[11];		//	券商ID(用户管理服务器返回)
extern CString	g_strTdxVerifyFileName;
extern char		g_MngRight;				//	管理权限
extern char		g_UserType;				//	用户类型
extern char		g_strUserType[9];		//  用户类型串
extern char		g_strUserCharacter[9];	//  用户性质串
extern char		g_strUserLimits[20];	//  用户权限串

extern DWORD	g_CategoryInfo;			//	单独的信息类型
extern char		g_RightStr[255];
extern long		g_ExpireDate;
extern long		g_UrgentNum;

extern CString  SwitchCurGPCode;
extern short	SwitchCurSetCode;

extern BOOL	g_bActTogether;
extern BOOL	g_bFxtShowInfoDlg;
extern BOOL	g_bFxtShowTab;
extern BOOL	g_bTranparentTab;
extern BOOL	g_bHasEmbWT;

extern BOOL	g_bNLB;
extern BOOL	g_bNLBOnLogo;
extern struct NLBInfo g_NLBInfo[150];

extern BOOL	g_bWtUserVerifyStyle;
extern BOOL	g_bHexinWtOnlyTXMM;
extern short g_WtVerifyIPNum;
extern struct IpInfo *g_WTVerifyIP;
extern struct VipVerify_Info g_VipVerify_Info;

extern BOOL g_bAuthSuccess;
extern BOOL g_bWTEverLogin;
extern BOOL g_bHintSwitch;
extern BOOL g_bRandSwitch;
extern BOOL g_bNetErrorReLogin;

extern struct Global_Habit		g_GH;
extern struct Global_Cfg		g_GC;
extern struct Global_EmbOem		g_OEM;
extern struct Global_Url		g_Url;
extern struct Global_SysInfo	g_SysInfo;
extern struct Global_DSCfg		g_DS;
extern struct JyCookie_Info		g_JyInfo;
extern struct AntiHack_Cfg		g_AntiHack;
extern struct NetParam_Cfg		g_NetParam;

extern BOOL g_bForbidOtherHost;
extern BOOL g_bUseKList;
extern CString	g_PersonalPath;

extern	int l_YYBNum;
extern	int l_PrimaryYYB;
struct tag_YYBInfo
{
	char YYBInfo_ID[12];
	char YYBInfo_Str[50];
};
extern struct tag_YYBInfo *l_pYYBInfo;

extern  BOOL g_b2000XPSys;
extern	BOOL g_bUseBigJBM;
extern	BOOL g_bCanUserUrgent;
extern	BOOL g_bHasBakHostFile;
extern	BOOL g_bHasZXQY;
extern	BOOL g_bCanDown;
extern  BOOL g_bLastRqDown;
extern  BOOL g_nReConnectNum; //重连接
extern  int  g_nLogoTimeOut;  //登录过程中的超时秒数
extern	int  g_nCusWebPageNum;
extern	CUSWEBPAGE g_CusWebPage[MAX_CUSWEB_NUM];
extern	CUSWEBPAGE g_F11WebPage;
extern	float	l_aRatio[RATIONNUM];

extern char			g_pNbbs_DS[10];
extern SRVINFOANS	g_iDsSrvInfo;
extern BOOL			g_bDSNeedIdx;
extern BOOL			g_bDSNeedTimer;
extern int			g_GuiMarketNum;
extern int			g_LocalGuiMarketNum;
extern GUIMARKET	g_GuiMarketSet[MAX_MKTGUI_NUM];
extern short		g_nDSMarketNum;
extern MKTINFO		g_pDSMarket[MAX_MKT_NUM];

class	KeyWordSeach;
extern	KeyWordSeach* g_pKeyWordSearch;

extern	GRIDCATITEM		g_SysDefGridCat[12], g_SysDefGridCat_Com2[12];
extern	GRIDCATITEM		g_SysDefGridCat_All[16];
extern	long			g_nSysDefGridCatNum,g_nSysDefGridCat_All;

extern  vector<GRIDCATITEM>	g_CustomGridCat;

extern	SPECMENUCOLOR	g_SpecMenuColor[];
extern	int				g_nSpecMenuColor;

extern	struct ETF_Info g_ETFInfo[];
extern	int g_nETFInfoNum;


#define HQT_FACE	0
#define ZST_FACE	1
#define FXT_FACE	2
#define F10_FACE	3
#define IE_FACE		4
#define CJZX_FACE	5

extern Status_GP g_StatusGP[3];

extern long	g_nNetInCount;
extern long	g_nNetOutCount;

extern vector<PWARNINGCND> g_aSelfWarnGp;

extern	short g_nHZQZFlag;

extern	int		g_nZHStyle;
extern	int		g_nZBReportSortIndex;

class	CMyToolBar;
extern	BOOL	g_bAutoHideBar;
extern  CMyToolBar	*g_pMyToolBar;
extern BOOL	g_bShowBigToolBar;

extern	char		*g_strGroup[MAX_GROUPMARK];
extern	long    g_nCJZX2TitleNum;

extern vector<FY_GROUP_INFO> g_vfyGroup;
extern BOOL g_bFYDataAlwaysSync;
extern struct FY_LOCATION g_FyLocation;

extern BOOL g_bTopTabWorkSpace;
extern BOOL g_bEmbWTCreated;
extern struct ADVERT_INFO g_AdvertInfo;
extern struct NewJy_CFG g_NewJyCfg;

extern struct Server_MarketInfo g_Mtl;

extern CString	g_strTendName;
extern CString  g_strTendCfg;
extern CString  g_strNeedAdName;
extern CString  g_strTdxInfoIniName;

extern BOOL g_bSaveZH,g_bProtectZH;
extern HWND	g_hSoftKeyWnd;
extern HWND	g_hHXInfoWnd;
extern OverlapInfo	g_Overlap[2];

//调试状态
extern BOOL	g_bDebugMode;
extern BOOL	g_bChangeDebugCode;

extern HisTickCacheHead	g_HisTickCacheHdr[MAX_HISCACHE];
extern int	g_nNextHTCHPos;

extern CToolTipMark		*	g_pToolTipMark;
extern BOOL					g_bFixedXAxis;
extern short				g_nFixedXAxis;
extern int					g_nDefaultZxWidth;
extern int					g_nDefaultRestHeight;

extern KeyGuyCNInfo g_KeyGuyCnInfo;
//BI SERVER配置
extern char	g_BIHostIP[50],g_BakBIHostIP[50];
extern unsigned short g_nBIHostPort,g_nBakBIHostPort;

extern int	g_nFlushRealInfo;
extern int	g_nCurLevel2Tab;
extern BOOL	g_bNewJyUseAdvHQHost;
extern vector<FL_CodeName>	g_SectorCN;

//////////////////////////////////////////////////////////////////////////
extern char		lpszJJItemName[][20];
extern float	lpnOJJItemWidth[];
extern float	lpnCJJItemWidth[];

extern char		lpszhbItemName[][20];
extern float    lpnhbItemWidth[];

extern char		lpszJJStockName[][20];
extern float	lpnJJStockWidth[];

extern pFUNDBASE		g_pFundBase;
extern int				g_nFundNum;
extern pFUNDHOLDSTOCK	g_pFundHoldStock;
extern int				g_nHoldNum;
extern BOOL				g_bFundDataAlwaysSync;
//////////////////////////////////////////////////////////////////////////

extern struct WriteUrgentInfo *g_pWriteUrgent;
//////////////////////////////////////////////////////////////////////////

extern DWARNINF	g_DWarnInf;

//和Comview2及其子类
extern char **	g_ppHqStyleStr;
extern char*	nullPadName;
extern time_t	tmp_SetGPtime;

extern map<char*, int, cmpstr>			g_mUnitType;
extern map<char*, int, cmpstr>			g_mUnitStyle;
extern map<UINT, int, greater<UINT> >	g_mUnitMenu;

extern char			g_LayerPath[MAX_PATH], g_ContentPath[MAX_PATH];
extern BOOL			g_bSupportDDE;

//////////////////////////////////////////////
// 系统所用的图像按钮
enum
{
	BTN_REPORT,
	BTN_LIGHT,
	BTN_IPO,
	BTN_GUBA
};	
extern CImageList		g_BtnImage,g_GridImage;

extern BOOL				g_bHasTopView;
extern BOOL				g_bTopViewStatus;
extern char				g_TVHostIP[50];
extern unsigned short	g_nTVHostPort;
extern short			g_nMyProvineID;
extern char			*	g_pProvinceStr[35];

extern map<long, Seat_Code2Com, greater<long> > g_mSeat2Com;
extern map<long, Seat_Com2Name, greater<long> > g_mSeatCode2Name;

extern BOOL				g_bTickTStatus;
extern BOOL				g_bShowCustomPadSwitch;
extern BOOL				g_bHasHomePage;
extern CString			g_InitDefHomePage;

extern int				g_nTopDockTop;

extern map<CString, ShortCut_Cfg, cmpstr > g_mShortCut;

extern char				g_strYxhjCfg[MAX_PATH];
extern char				g_strUserCfg[MAX_PATH];
extern char				g_strBlockNew[MAX_PATH];

//定时请求的信息
extern	time_t	g_LastTimeTag;
extern	vector<URGENTINFO>	g_aUrgentInfo;

extern long					g_nAutoCalcLock;
extern BOOL					g_bShow3DKLine;

extern short				g_nUseDNSUrl;
extern char					*g_strDNSUrl[2];
extern char					*g_strTwinserverUrl;

class CPreviewUnitDlg;
extern CPreviewUnitDlg *g_pPreviewUnitDlg;

extern CHqSrcMgn *g_pHqSrc;
extern vector<MemStkInfo>			head[2];

extern map<long, long, greater<long> > g_DBIndexMap;
extern CDBFile *g_pHQDBFile;

extern long		g_nCountCalcBlockTJ;
extern long		g_nCalcBlockIndex;

extern long		g_nLastValidDate;
extern short		g_nLastValidValue;
extern long		g_nPopInfoLastTime;

extern Scheme_King		g_Scheme_King;

extern char *g_strCalc1, *g_strCalc2, *g_strCalc3;

extern CCriticalSection	g_DXIndexCalc;
extern CCriticalSection	g_HisDataReader;
extern BOOL	g_bAllLocalData;
extern BOOL	g_bTickStatReaded;

extern CCriticalSection	g_CalcIniLock, g_PoloReqSec;
extern map<long, float, greater<long> > g_mTolStat;

#endif

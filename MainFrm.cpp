//MainFrm.cpp : implementation of the CMainFrame class
#include "stdafx.h"
#include "TdxW.h"
#include "MainFrm.h"
#include "ViewManager.h"
#include "GridView.h"
#include "TxtView.h"
#include "NewsView.h"
#include "BigMenuView.h"
#include "HomePageView.h"
#include "ComView.h"
#include "ComView2.h"
#include "IEBrowserView.h"
#include "JbfxView.h"
#include "AdvHqView.h"
#include "JJView.h"
#include "RssView.h"
#include "GnTree.h"
#include "GpTree.h"
#include "ZbTree.h"
#include "WebTree.h"
#include "FindExeDlg.h"
#include "ColorPage.h"
#include "BlockPage.h"
#include "BlockColorPage.h"
#include "OtherPage.h"
#include "Other2Page.h"
#include "Other3Page.h"
#include "Other4Page.h"
#include "ShortCutPage.h"
#include "ExternPage.h"
#include "Splash.h"
#include "KeyGuy.h"
#include "dbf.h"  //dbf动态连接库的头文件
#include "Znxg.h"
#include "InfoDlg.h"
#include "LogoDlg.h"
#include "ConnectCfgDlg.h"
#include "InvestExp.h"
#include "DownLoadDlg.h"
#include "HintDlg.h"
#include "AutoupInfo.h"
#include "LeadUp.h"
#include "TipDlg.h"
#include "GetBlockDlg.h"
#include "ustart.h"
#include "u_changepwd.h"
#include "batchdlg.h"
#include "KeyWordSeach.h"
#include "WTPathSelDlg.h"
#include "TabSetPage.h"
#include "MyDockBar.h"
#include "AdvDlg.h"
#include "verify.h"
#include "Ad.h"
#include "FundHoldStockDlg.h"
#include "SendRecFileDlg.h"
#include "HQControl.h"
#include "PreviewUnitDlg.h"

extern void GetFYInfo();
extern void GetNeedNoteInfo();
extern void	ShowUrgentDlg(int flag,int whichurgent);
extern int	GetTotalMulNumFromIndex(int i);
extern int	nGetRollBarHeight();
extern void	GetOpenStatusFromRollShow();
extern void	GetRollShowFromOpenStatus();

UINT UM_INFOOFFLINE = RegisterWindowMessage("InfoDisconnect"); 
UINT UM_FIRSTSTEP_FAIL = RegisterWindowMessage("FirstStepFail");
UINT UM_CLEARCACHETITLE = RegisterWindowMessage("CleanCacheMineTitle");
UINT UM_ROLLINFOTITLEOK = RegisterWindowMessage("RollInfoTitleOK");
//外挂取资金帐号的消息
UINT QryZJZHMsg = RegisterWindowMessage("Query_Account");
UINT RetZJZHMsg = RegisterWindowMessage("Return_Account");
UINT ExtMsg = RegisterWindowMessage("Exit_Getmsg");
UINT QryZJZHMsg_My = 0;
UINT RetZJZHMsg_My = 0;
UINT ExtMsg_My = 0;
//上次选中的委托程序
long l_nSelExeIndex=0;
//用于行情统计数据的缓冲区
short HQStat_lpnStkIndex[MAXSTAT_MAXNUM] = {0};

// dwDockBarMap
const DWORD CMainFrame::dwDockBarMap[4][2] =
{
	{ AFX_IDW_DOCKBAR_TOP,      CBRS_TOP    },
	{ AFX_IDW_DOCKBAR_BOTTOM,   CBRS_BOTTOM },
	{ AFX_IDW_DOCKBAR_LEFT,     CBRS_LEFT   },
	{ AFX_IDW_DOCKBAR_RIGHT,    CBRS_RIGHT  },
};

char* GetNowVersion()
{
	return g_szReleaseVersion;
}

//////////////////////////////////////////////////////////////////////////
extern void SaveBlockCfg();
DTopViewCalc * m_pCalcOnce = NULL;
DWORD	WINAPI	_AfxAutoCalcBlkThread(LPVOID lParam)
{
	struct tm t;            //定义tm时间结构，用来存储时间格式的数据信息
	time_t t_of_day;     //定义time_t时间结构
	t.tm_year=g_lOpenRq/10000-1900;//以1900年为标准计算时间
	t.tm_mon=(g_lOpenRq/100)%100;                 //为结构体成员赋值
	t.tm_mday=g_lOpenRq%100;
	t.tm_hour=15;
	t.tm_min=10;
	t.tm_sec=0;
	t.tm_isdst=0;
	t_of_day=mktime(&t);
	time_t t_of_data = GetPrivateProfileInt("OTHER","LocalTjFreshTime",0,g_WSXHStr+g_strUserCfg);
	if(t_of_data>t_of_day)
		return 0;

	if(g_nAutoCalcLock>0)
		return 0;

	int ConditionNum  = 0;
	CONDITIONINFO ConditionInfo[MAX_TJCOMB_NUM]={0};
	FILEHEAD FileHead={0};
	FileHead.nRefreshTime = -1;
	int i,j,k;
	//装入条件
	CString tempFileName = g_WSXHStr+"conblk.cos";
	CFile m_File;
	if( m_File.Open(tempFileName,CFile::modeRead|CFile::shareDenyNone) )
	{
		//读入文件头
		m_File.Read(&FileHead,sizeof(FILEHEAD));
		ConditionNum = FileHead.ConditionNum;
		CString tempStr;
		for(int i = 0; i < ConditionNum ;i++)
			//读入组合条件
			m_File.Read(&ConditionInfo[i],sizeof(CONDITIONINFO));
		m_File.Close();
	}

	int		nCurrPeriod = DAY_ST;
	if(ConditionNum < 1)
	{
		//清空条件板块
		vector<BlockInfo_Struct> tmpBlockInfo;
		for(i=0;i<g_nBlockNum;i++)
		{
			if(strstr(g_BlockInfo[i].KeyGuyCode, "@ConBlk")==NULL)
				tmpBlockInfo.push_back(g_BlockInfo[i]);
		}
		memset(g_BlockInfo, 0, sizeof(g_BlockInfo));
		g_nBlockNum = tmpBlockInfo.size();
		for(i=0;i<g_nBlockNum;i++)
			g_BlockInfo[i] = tmpBlockInfo[i];
		SaveBlockCfg();
		
		return 0;
	}

	char acCode[ZBCODE_LEN+1];
	short TjIndexNo[MAX_TJCOMB_NUM];
	memset(TjIndexNo,0,MAX_TJCOMB_NUM*sizeof(short));
	InitCalcInterface(m_pCalcOnce);
	for (i = 0; i < ConditionNum; i++ )
	{
		strcpy(acCode, ConditionInfo[i].acCode);
		nCurrPeriod = ConditionInfo[i].CalcCycle;
		TINDEXINFO	* pTJIndex = m_pCalcOnce->_CallDTopViewCalc_8(TJ_TYPE,(long)0);
		for(j = 0;j < m_pCalcOnce->_CallDTopViewCalc_7(TJ_TYPE);j++)//如果是从存盘文件中取得的条件，则应检查有否此条件
		{
			if ( strcmp(acCode,pTJIndex[j].acCode) == 0 )
			{	
				TjIndexNo[i] = j;
				for ( k = 0; k < min(pTJIndex[TjIndexNo[i]].nParaNum,ConditionInfo[i].ParamNum); k++)
					pTJIndex[TjIndexNo[i]].aPara[k].nValue[nCurrPeriod] = ConditionInfo[i].ParamValue[k];
				break;
			}
		}
	}

	//以下是取出所有符合条件的股票
	GpDomain_Struct m_GpDomain[100]={0}; 
	m_GpDomain[0].flag = DOMAIN_TYPE;
	m_GpDomain[0].which = TYPE_SHAG;
	strcpy(m_GpDomain[0].Name,GPType[TYPE_SHAG]);
	m_GpDomain[1].flag = DOMAIN_TYPE;
	m_GpDomain[1].which = TYPE_SZAG;
	strcpy(m_GpDomain[1].Name,GPType[TYPE_SZAG]);
	int m_nSelDomain = 2;
	short  awUsedStkp[MAXGPNUM]={-1};
	vector<short> resStkp[MAX_TJCOMB_NUM];
	int TotalStockNum = FiltMulGpDomain(m_GpDomain,m_nSelDomain,awUsedStkp);
	if(TotalStockNum <= 0)
	{
		//清空条件板块
		vector<BlockInfo_Struct> tmpBlockInfo;
		for(i=0;i<g_nBlockNum;i++)
		{
			if(strstr(g_BlockInfo[i].KeyGuyCode, "@ConBlk")==NULL)
				tmpBlockInfo.push_back(g_BlockInfo[i]);
		}
		memset(g_BlockInfo, 0, sizeof(g_BlockInfo));
		g_nBlockNum = tmpBlockInfo.size();
		for(i=0;i<g_nBlockNum;i++)
			g_BlockInfo[i] = tmpBlockInfo[i];
		SaveBlockCfg();

		return 0;
	}

//	RefreshAllHq(5*60);
	if(g_nAutoCalcLock>0)
		return 0;

	int SuccessNum = 0,ReadNum;
	int SuccessFlag;					//0:待定 1:成功	-1:失败 -2:阶段满足
	CString tempStr;

	float * fPut[MAX_LINEOUT]={0};
	long time2=FileHead.nDate;
	MemStkInfo *pStkInfo=NULL;
	for ( i = 0; i < TotalStockNum; i++ )
	{
		if(g_nAutoCalcLock>0)
			return 0;

		PeekMsg();

		pStkInfo = (*g_pStockIO)[awUsedStkp[i]];
		SuccessFlag = 0;
		int nCalcPer = 0,nPer;
		CurrStockData hqtmp={0};
		while(pStkInfo && pStkInfo->Close>COMPPREC && nCalcPer<ConditionNum)
		{
			if(g_nAutoCalcLock>0)
				return 0;

			nPer = ConditionInfo[nCalcPer].CalcCycle;
			int nPerCalc = CalcPeriodToTdxPeriod(nPer);
			
			ReadNum = m_pCalcOnce->_CallDTopViewCalc_4(pStkInfo->Code,pStkInfo->setcode,nPerCalc,ASK_ALL,1);	
			if(ReadNum>0)
			{
				for ( j = nCalcPer; j < ConditionNum && ConditionInfo[j].CalcCycle==nPer; j++ )
				{
					TINDEXINFO	* pTJIndex = m_pCalcOnce->_CallDTopViewCalc_8(TJ_TYPE,TjIndexNo[j]);
					if(!pTJIndex) continue;
					nCurrPeriod = nPer;
					m_pCalcOnce->_CallDTopViewCalc_14(fPut[0]);
					memset(fPut,0,MAX_LINEOUT*sizeof(float*));
					fPut[0]  = m_pCalcOnce->_CallDTopViewCalc_13(ReadNum);
					for(k=0;k<min(ConditionInfo[j].ParamNum,pTJIndex->nParaNum);k++)
						pTJIndex->aPara[k].nValue[nCurrPeriod] = ConditionInfo[j].ParamValue[k];
					if ( -1  != m_pCalcOnce->_CallDTopViewCalc_5(TJ_TYPE,TjIndexNo[j],fPut) )
					{
						if((TBE_MEANLESS(fPut[0][ReadNum-1]) != 0) && fabs(fPut[0][ReadNum-1]) > 0.1)
							resStkp[j].push_back(awUsedStkp[i]);
					}
					PeekMsg();
				}
				nCalcPer = j;
			}
			else 
			{
				++nCalcPer;
				continue;
			}
		}
	}
	m_pCalcOnce->_CallDTopViewCalc_14(fPut[0]);

	//清空条件板块
	vector<BlockInfo_Struct> tmpBlockInfo;
	for(i=0;i<g_nBlockNum;i++)
	{
		if(strstr(g_BlockInfo[i].KeyGuyCode, "@ConBlk")==NULL)
			tmpBlockInfo.push_back(g_BlockInfo[i]);
	}
	memset(g_BlockInfo, 0, sizeof(g_BlockInfo));
	g_nBlockNum = tmpBlockInfo.size();
	for(i=0;i<g_nBlockNum;i++)
		g_BlockInfo[i] = tmpBlockInfo[i];

	for(i=0;i<ConditionNum;i++)
	{
		TINDEXINFO	*pTJIndex = m_pCalcOnce->_CallDTopViewCalc_8(TJ_TYPE,ConditionInfo[i].acCode);
		//构造列表项
		CString tempStr = (CString)(pTJIndex?pTJIndex->acName:ConditionInfo[i].acCode) + " ";
		if(ConditionInfo[i].ParamNum != 0)
			tempStr = tempStr + "(";
		for (int j =0 ;j < ConditionInfo[i].ParamNum; j++)
		{
			if( j == ConditionInfo[i].ParamNum-1 )
				tempStr = tempStr + IntToStr(ConditionInfo[i].ParamValue [j]);
			else
				tempStr = tempStr + IntToStr(ConditionInfo[i].ParamValue [j])+",";
		}
		if(ConditionInfo[i].ParamNum != 0)
			tempStr = tempStr + ") ";
		tempStr = tempStr + _F(g_CalcPeriodStr[ConditionInfo[i].CalcCycle]);
		
		char Code[100]={0};
		sprintf(Code, "@ConBlk%d", i);
		SeveToNewBlock(resStkp[i], Code, (char*)(LPCSTR)tempStr,i);
	}
	SaveBlockCfg();
	InterlockedIncrement(&g_nCountCalcBlockTJ);
	g_bDonotClose = FALSE;

	//记录刷新时间
	struct tm t1;            //定义tm时间结构，用来存储时间格式的数据信息
	time_t t_of_data1;     //定义time_t时间结构
	CTime ct = CTime::GetCurrentTime();
	t1.tm_year=ct.GetYear()-1900;//以1900年为标准计算时间
	t1.tm_mon=ct.GetMonth();                 //为结构体成员赋值
	t1.tm_mday=ct.GetDay();
	t1.tm_hour=ct.GetHour();
	t1.tm_min=ct.GetMinute();
	t1.tm_sec=ct.GetSecond();
	t1.tm_isdst=0;
	t_of_data1=mktime(&t1);		
	char ltm[20]={0};
	sprintf(ltm, "%d", t_of_data1);
	WritePrivateProfileString("OTHER","LocalTjFreshTime",ltm,g_WSXHStr+g_strUserCfg);

	return 1;
}

/****************************************************************************************************/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_INITMENUPOPUP()

	ON_UPDATE_COMMAND_UI(CG_ID_VIEW_WORKSPACE, OnUpdateControlBarMenu)
	ON_COMMAND_EX(CG_ID_VIEW_WORKSPACE, OnWorkBarCheck)
	ON_UPDATE_COMMAND_UI(CG_ID_VIEW_RESTSPACE, OnUpdateControlBarMenu)
	ON_COMMAND_EX(CG_ID_VIEW_RESTSPACE, OnRestBarCheck)
	ON_UPDATE_COMMAND_UI(ID_TOOLS, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_TOOLS, OnBarCheck)
	ON_UPDATE_COMMAND_UI(AFX_IDW_STATUS_BAR , OnUpdateControlBarMenu)
	ON_COMMAND_EX(AFX_IDW_STATUS_BAR ,OnStatusBarCheck)

	ON_COMMAND_RANGE(ID_RIGHT_HELP,ID_RIGHT_HELPWEB,OnRightClickLeft)
	ON_COMMAND(ID_CORP_WEB,OnCorpWeb)
	ON_COMMAND(ID_HELP_FINDER, OnHelpFinder)
	
	ON_COMMAND(ID_HQTABOPTION,OnHqTabOption)
	ON_COMMAND(ID_ETF,OnETF)
	ON_COMMAND_RANGE(ID_ETFSTART,ID_ETFEND,OnETFCommand)

	ON_MESSAGE(UM_TREEVIEW_MESSAGE,OnTreeViewMessage)
	ON_COMMAND_RANGE(ID_ZX_FAVOR,ID_ZX_QS_SORT,OnZXCommand)
	ON_COMMAND_RANGE(IDC_CUSTOMPAD1,IDC_CUSTOMPAD100,OnCustomPad)
	ON_COMMAND(IDC_RECENTPAD,OnRecentPad)
	ON_MESSAGE(UM_SHOWURGENT_MESSAGE,OnShowUrgentMsg)

	ON_COMMAND(IDC_QSCUSWEBPAGE,OnQSCusWebPage)
	ON_COMMAND_RANGE(IDC_CUSWEBPAGE_BASE,IDC_CUSWEBPAGE_LAST,OnCusWebPage)
	ON_COMMAND(IDC_F11WEBPAGE,OnF11WebPage)
	ON_COMMAND(IDC_HKEYPROGRAM,OnHKeyProgram)
	ON_COMMAND_RANGE(ID_101,ID_119,OnDpFx)
	ON_COMMAND_RANGE(ID_300,ID_508,OnOtherFx)
	ON_COMMAND_RANGE(ID_RMREPORT,ID_QJTYPE4, OnChangeReport)
	ON_COMMAND(ID_TDX_JBZL,OnTdxJBZL)
	ON_COMMAND_RANGE(ID_RIGHT_TYPE0,ID_RIGHT_TYPE20,OnBJFX)
	ON_COMMAND_RANGE(ID_BLOCK_SYNC/*ID_BLOCK_MENU_1*/,ID_BLOCK_MENU_400,OnMenuBlockClick)
	ON_COMMAND_RANGE(ID_RANK_1,ID_RANK_26,OnRank)
	ON_COMMAND_RANGE(ID_MULGP_TYPE0,ID_MULGP_TYPE17,OnMulGpType)

	ON_COMMAND_RANGE(ID_RIGHT_HQBASE,ID_RIGHT_FYBASE_MAX,OnFyReport)
	ON_COMMAND(ID_BIGFONTGRID,OnBigFontGrid)
	ON_COMMAND(ID_LOCKRIGHT,OnLockRight)

	ON_COMMAND(ID_DOWNLOAD,OnDownLoad)
	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD,OnUpdateDownLoad)

	ON_COMMAND(ID_NODETOOL,OnNodeTool)
	ON_UPDATE_COMMAND_UI(ID_NODETOOL,OnUpdateNodeTool)

	ON_COMMAND(ID_DAY_WRITEDISK,OnDayWriteDisk)
	ON_UPDATE_COMMAND_UI(ID_DAY_WRITEDISK,OnUpdateDayWriteDisk)

	ON_COMMAND(ID_WIDEDATA_HQ,OnWideDataHQ)
	ON_MESSAGE(UM_WIDEDATA_SORT, OnWideDataSort)
	ON_MESSAGE(UM_WIDEDATA_HQSTAT, OnWideDataStatHQ)
	ON_COMMAND(ID_DATASYNC,OnSyncData)

	ON_COMMAND(IDC_ZXG,OnZxg)
	ON_COMMAND(IDC_6787,On6787)
	ON_MESSAGE(UM_STATUS_REQ,SendStatusReq)
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	ON_MESSAGE(COMM_DISCONNECT,OnCommDisConnect)
	ON_MESSAGE(UM_HQCONNECTTIMER_MESSAGE,OnHqConnectTimerMessage)

	ON_MESSAGE(WM_HOTKEY,OnHotKey)

	ON_MESSAGE(WM_CANCELPRESSED,CancelNoModal)

	ON_MESSAGE(UM_SEARCH_BLOCK,SearchBlock)
	ON_MESSAGE(UM_SEARCH_GPGXD,SearchGpGxd)
	ON_MESSAGE(UM_SEARCH_HKSEAT,SearchHKSeat)

	ON_MESSAGE(UM_SWITCHCALLBACK,ProcessSwitchCallBack)

	ON_COMMAND(ID_CONNECT, OnConnect)
	ON_COMMAND(ID_BGCONNECT,OnBGConnect)
	ON_COMMAND(ID_REINITCONNECT,OnReInitConnect)
	ON_UPDATE_COMMAND_UI(ID_CONNECT, OnUpdateConnect)
	ON_COMMAND(ID_DISCONNECT, OnDisconnect)
	ON_UPDATE_COMMAND_UI(ID_DISCONNECT, OnUpdateDisconnect)
	ON_COMMAND(ID_CONNECTCFG, OnConnectCfg)
	ON_COMMAND(ID_SILENTDISCONNECT,OnSilentDisconnect)
	ON_MESSAGE(UM_HIDESHOWBUTTON,OnHideShowButtonMsg)

	ON_MESSAGE(UM_ONLINEENTERMSG,OnLineEnterMsg)
	ON_MESSAGE(UM_PROCESSAUTOUPINFO,OnProcessAutoupInfo)

	ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnToolbarDropDown)

	ON_COMMAND(ID_NETFLUXCOUNT, OnNetfluxcount)
	//与专家系统有关的函数
	ON_COMMAND(ID_TJTEST, OnTJTest)
	ON_COMMAND(ID_JYTEST, OnJYTest)
	ON_COMMAND(ID_TESTINDEX, OnTestindex)
	ON_COMMAND(ID_CONDITIONCHOICE, OnConditionChoice)
	ON_UPDATE_COMMAND_UI(ID_CONDITIONCHOICE, OnUpdateChoicegp)
	ON_COMMAND(ID_CLECHOICE, OnClechoice)
	ON_UPDATE_COMMAND_UI(ID_CLECHOICE, OnUpdateClechoice)
	ON_COMMAND(ID_SIMCHOICE, OnSimchoice)
	ON_COMMAND(ID_SELGP, OnSelGP)
	ON_UPDATE_COMMAND_UI(ID_SELGP, OnUpdateSelGP)	
	ON_UPDATE_COMMAND_UI(ID_SIMCHOICE, OnUpdateSimchoice)
	ON_COMMAND(ID_FORMULA, OnFormula)
	ON_UPDATE_COMMAND_UI(ID_FORMULA, OnUpdateFormula)

	ON_COMMAND(IDC_ZXG_OPR,OnZXGOpr)
	ON_COMMAND(ID_TDXCHOICEGP,OnTdxChoiceGp)
	ON_COMMAND(ID_GPFL,OnGPFL)
	ON_COMMAND(ID_GPSORT,OnGPSort)
	ON_COMMAND(ID_GPZHSORT,OnGPZhSort)

	ON_COMMAND(ID_PLUGIN,OnPlugin)
	ON_UPDATE_COMMAND_UI(ID_PLUGIN, OnUpdatePlugin)
	//
	ON_COMMAND(ID_STAR_GRAPH,OnStarGraph)
	//
	ON_COMMAND(ID_WEBINFO,OnWebInfo)
	ON_COMMAND(ID_URGENT,OnUrgentDlg)
	ON_COMMAND(ID_RIGHT_ADDZX,OnAdditonZXURL)
	ON_COMMAND(ID_VIDEO,OnVideo)
	ON_UPDATE_COMMAND_UI(ID_WEBINFO,OnUpdateWebInfo)

	ON_MESSAGE(UM_AUTOCONNECT,OnAutoConnectMsg)
	ON_MESSAGE(UM_CONNECT_SUCCESS,OnConnectResult)

	ON_COMMAND_RANGE(ID_WEB_MENU_1,ID_WEB_MENU_40,OnWebPageMenu)
	ON_COMMAND_RANGE(ID_WEB2_MENU_1,ID_WEB2_MENU_40,OnWebPageMenu2)

	ON_COMMAND(ID_F9,OnF9)
	ON_COMMAND(IDC_GGZL, OnGgzl)
	ON_COMMAND(IDC_JBFX, OnJbfx)
	ON_COMMAND(ID_GPCOOLINFO,OnGPCoolInfo)
	ON_COMMAND(ID_JIANGEN,OnJiangEn)
	ON_COMMAND(ID_MULUNIT,OnMulUnit)
	ON_COMMAND(ID_MSSR, OnMSSR)
	ON_COMMAND(ID_BIGVOL, OnBigVol)
	ON_COMMAND(ID_GOTOJBZL, OnGotojbzl)
	ON_COMMAND(IDM_BUYDS, OnBuyDS)
	ON_COMMAND_RANGE(IDM_DS1,IDM_DS1+MAX_MKT_NUM,OnDSMarket)
	ON_COMMAND_RANGE(ID_FYFLAG_0,ID_FYFLAG_END,OnFYGrid)
	ON_COMMAND_RANGE(IDC_WEBUSER_1001,IDC_WEBUSER_1003,OnWebUserCommand)
	
	ON_COMMAND_RANGE(IDC_LEVINBUY0,IDC_LEVINBUY20,OnLevinBuy)
	ON_COMMAND_RANGE(IDC_LEVINSELL0,IDC_LEVINSELL20,OnLevinSell)
	ON_COMMAND_RANGE(IDC_BUY0,IDC_BUY20,OnBuy)
	ON_COMMAND_RANGE(IDC_SELL0,IDC_SELL20,OnSell)
	ON_COMMAND(IDC_WTCANCEL,OnWTCancel)
	ON_COMMAND(IDC_QZSDS,OnQzSDS)
	ON_COMMAND(IDC_USERCENTER,OnUserCenter)
	ON_COMMAND(IDC_RELATIVEINFO,OnRelativeInfo)	
	ON_COMMAND(IDC_PRICEWARN,OnPriceWarn)

	ON_COMMAND(ID_CLIPSPY, OnClipSpy)
	ON_UPDATE_COMMAND_UI(ID_CLIPSPY, OnUpdateClipSpy)
	
	ON_MESSAGE(UM_TDXWSHOW,OnShowMessage)

	ON_MESSAGE(UM_CLOSEROLL_MSG,OnCloseRollMsg)
	ON_MESSAGE(UM_CLOSESLOWROLL_MSG,OnCloseSlowRollMsg)

	ON_COMMAND(ID_CONINFO, OnConInfo)

	ON_COMMAND(ID_MARKMNG, OnMarkMgn)

	ON_COMMAND_RANGE(ID_PADSWITCH1, ID_PADSWITCH20, OnSwitchPadMenu)

	ON_UPDATE_COMMAND_UI(ID_GPGRADE_DLG,OnUpdateGPGradeDlg)
	ON_COMMAND(ID_TDXREPORT,OnTdxReport)
	ON_COMMAND(ID_GUBA,OnGuba)
	ON_UPDATE_COMMAND_UI(ID_TDXREPORT,OnUpdateTdxReport)

	ON_COMMAND(ID_WRITEURGENT,OnWriteUrgent)

	ON_MESSAGE(UM_OPENWEBWIN,OnOpenWebWin)
	ON_MESSAGE(UM_AUTOCONNECTDS,OnAutoConnectDS)
	ON_MESSAGE(UM_SPECIALZX_MSG,OnSpecailZX)

	ON_MESSAGE(UM_OPENIWCONTENT,OnOpenIWContent)
	ON_MESSAGE(UM_POPMYIE,OnPopMyIE)

	ON_COMMAND(ID_DSINIT,OnInitDS)
	ON_UPDATE_COMMAND_UI(ID_DSINIT,OnUpdateInitDS)
	ON_COMMAND(ID_UNINITDS,OnUnInitDS)
	ON_UPDATE_COMMAND_UI(ID_UNINITDS,OnUpdateUnInitDS)

	ON_COMMAND(ID_POPMOREDLG, OnPopUrgent)
	ON_MESSAGE(UM_TIMERMORE_REQ,SendMoreInfoReq)

	ON_MESSAGE(UM_AUTOCALCTJBLOCK, OnAutoCalcTjBlock)
	ON_COMMAND(IDC_CLXG, OnCLXG)

	ON_COMMAND_RANGE(IDC_JCTZBSET1,IDC_JCTZBSET5,OnJCTZBSetChange)
	
	ON_COMMAND_RANGE(IDC_ZFJS_PAD0,IDC_ZJFS_LAST,OnZFJS_Cmd)
	ON_UPDATE_COMMAND_UI_RANGE(IDC_ZFJS_PAD0,IDC_ZJFS_LAST,OnUpdate_ZFJS_Cmd)

	ON_COMMAND_RANGE(IDC_DEFTOOLGN1,IDC_NOTSUPPORT,OnDefTool_Cmd)
	ON_UPDATE_COMMAND_UI_RANGE(IDC_DEFTOOLGN1,IDC_NOTSUPPORT,OnUpdate_DefTool_Cmd)

	ON_MESSAGE(UM_POPMSG,OnPopMsg)

	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_NCHITTEST()
	ON_COMMAND(ID_SYSPAD,OnSysPad)
	ON_UPDATE_COMMAND_UI(ID_SYSPAD,OnUpdateSysPad)
	ON_COMMAND(ID_SHOWHQ, OnShowhq)
	ON_COMMAND(ID_SHOWZST, OnShowzst)
	ON_COMMAND(ID_SHOWVIP, OnShowvip)
	ON_COMMAND(CG_IDS_TIPOFTHEDAY, OnIdsTipoftheday)
	ON_COMMAND(ID_MINIMIZE, OnMinimize)
	ON_COMMAND(ID_RESTORE,OnRestore)
	ON_WM_NCRBUTTONUP()
	ON_COMMAND_RANGE(ID_ZHSORT_TYPE0,ID_ZHSORT_TYPE11,OnZHSort)
	ON_COMMAND(ID_HKSEATQUERY,OnHKSeatQuery)
	ON_COMMAND(ID_FUNC_BLOCKCALC, OnFuncBlockCalc)
	ON_COMMAND(ID_DATADIG, OnDataDig)
	ON_WM_NCMOUSEMOVE()
	ON_COMMAND(ID_AUTOUP, OnAutoup)
	ON_COMMAND(ID_SPEEDTEST, OnSpeedTest)
	ON_COMMAND(ID_FORUM,OnForum)
	ON_COMMAND(ID_SHOWINFO,OnShowInfo)
	ON_WM_SYSCOMMAND()
	ON_WM_ACTIVATEAPP()
	ON_COMMAND(ID_NEWPAD, OnNewpad)
	ON_COMMAND(ID_LCINDEX, OnLocalIndex)
	ON_COMMAND(ID_LCEXTERN,OnLocalExtern)
	ON_COMMAND(ID_LCPTSTK,OnLocalPtStk)
	ON_COMMAND(ID_SETDEFCUSPAD, OnSetdefcuspad)
	ON_UPDATE_COMMAND_UI(ID_SETDEFCUSPAD, OnUpdateSetdefcuspad)
	ON_COMMAND(ID_SHOW_CUSTOMPADSWITCH,OnShowCustomPadSwitch)
	ON_UPDATE_COMMAND_UI(ID_SHOW_CUSTOMPADSWITCH,OnUpdateShowCustomPadSwitch)
	ON_WM_COPYDATA()
	ON_UPDATE_COMMAND_UI(ID_BIGFONTGRID,OnUpdateBigFontGrid)
	ON_UPDATE_COMMAND_UI(ID_LOCKRIGHT,OnUpdateLockRight)
	ON_COMMAND(ID_HSZS, OnHszs)
	ON_COMMAND(ID_SHGZZS,OnShgzzs)
	ON_COMMAND(ID_HKJYTIME,OnHkJyTime)
	ON_COMMAND(ID_WARNCFG, OnAllWarncfg)
	ON_COMMAND(ID_WARNSWITCH,OnAllWarnSwitch)
	ON_UPDATE_COMMAND_UI(ID_WARNSWITCH,OnUpdateWarnSwitch)
	ON_COMMAND(ID_TJWARNCFG, OnTjWarncfg)
	ON_COMMAND(ID_TJWARNSWITCH,OnTjWarnSwitch)
	ON_UPDATE_COMMAND_UI(ID_TJWARNSWITCH,OnUpdateTjWarnSwitch)
	ON_COMMAND(ID_GETGP, OnGetgp)
	ON_COMMAND(ID_CLOSEALL, CloseallAndKeepSpec)
	ON_WM_ACTIVATE()
	ON_WM_NCLBUTTONUP()
	ON_COMMAND(ID_WARNGP, OnAllWarngp)
	ON_COMMAND(ID_TJWARNGP, OnTjWarngp)
	ON_COMMAND(ID_DEEPWARN, OnDeepWarn)
	ON_UPDATE_COMMAND_UI(ID_DEEPWARN, OnUpdateDeepWarn)
	ON_WM_CREATE()
	ON_WM_NCPAINT()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(ID_OPTION, OnOption)
	ON_COMMAND(ID_JYOPTION, OnJyOption)
	ON_COMMAND(ID_BLOCKOPTION, OnBlockOption)
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_FULLSCREEN, OnFullscreen)
	ON_UPDATE_COMMAND_UI(ID_FULLSCREEN, OnUpdateFullscreen)
	ON_COMMAND(ID_TOPMOST,OnTopMost)
	ON_UPDATE_COMMAND_UI(ID_TOPMOST,OnUpdateTopMost)
	ON_COMMAND(ID_COOLINFO, OnCoolinfo)
	ON_COMMAND(ID_NEWS, OnNews)
	ON_COMMAND(ID_ESCAPE,OnEscape)
	ON_COMMAND(ID_FUNC_WT, OnFuncWt)
	ON_COMMAND(ID_MOREHEXIN_MSG, OnFuncWt)
	ON_COMMAND(ID_FUNC_EMBWT, OnFuncEmbWT)
	ON_COMMAND(ID_FUNC_ADDEDWT1, OnFuncAddedWt1)
	ON_COMMAND(ID_FUNC_ADDEDWT2, OnFuncAddedWt2)
	ON_COMMAND(ID_FUNC_HXINFO, OnFuncHxInfo)
	ON_COMMAND(ID_FUNC_ZXJTINFO, OnFuncZxjtInfo)
	ON_COMMAND(IDC_CONNECTHQ, OnConnectHQ)
	ON_COMMAND(ID_EXTERN_INFO,OnExternInfo)
	ON_UPDATE_COMMAND_UI(ID_EXTERN_INFO,OnUpdateExternInfo)
	ON_UPDATE_COMMAND_UI(ID_JYOPTION, OnUpdateJyOption)
	ON_COMMAND(ID_FUNDHOLD,OnFundHold)
	ON_COMMAND(ID_PADLIST, OnPadList)
	ON_COMMAND(ID_PAGEUP, OnPageup)
	ON_COMMAND(ID_PAGEDOWN, OnPagedown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//状态栏方格
static UINT indicators[] =
{
	ID_DRAWSPACE1,
};

CMainFrame::CMainFrame()
{
	m_pChoiceGPDlg = NULL;
	m_pChoiceBKDlg = NULL;
	m_pSimChoiceDlg = NULL;
	m_pCleChoiceDlg = NULL;
	m_pPluginDlg = NULL;
	m_pSelGP = NULL;
	m_pTJTestDlg = NULL;
	m_pJYTestDlg = NULL;
	m_pTestZBDlg = NULL;
	m_pZHSortDlg = NULL;
	m_pHKSeatQueryDlg = NULL;
	m_pDataDigDlg = NULL;
	m_pInBlockDlg = NULL;
	m_pGpGxdDlg	  = NULL;
	m_pBlockCalcDlg = NULL;
	m_pFluxCalcDlg = NULL;
	m_pVideoDlg = NULL;
	m_pWebWinDlg = NULL;
	LayoutFlag = FALSE;
	m_pDeepWarnDlg = NULL;
	m_pUrgentPopDlg = NULL;
	m_pIWContentDlg = NULL;
	m_pPopMyIEDlg = NULL;

	m_NCInfo.bMinDown = FALSE;
	m_NCInfo.bRestoreDown = FALSE;
	m_NCInfo.bCloseDown = FALSE;
	m_NCInfo.bVitalDown = -1; //表示一个都没有被按下
	m_NCInfo.bAdvButtonDown = -1;
	m_NCInfo.NcRect.SetRect(0,0,0,0);
	m_NCInfo.HasNcRect = TRUE;

	m_pWndFullScreen = NULL;
	m_pWndCurrentChild = NULL;

	m_nMainID	= 0;
	g_bDirectQuit = TRUE;

	m_nFirstOptionPage = 0;

	m_nInfoReconnect = 100;

	m_hWndNextViewer = NULL;
	m_bClipSpy = FALSE;

	m_nWarnReq = 0;
	m_nAdPos = -1;
	m_nHqMinePos = 0;
	m_nHqMinePrenum = 80;

	m_pTabViewMenu = NULL;
	m_bPadMenuShow = FALSE;
	m_bJyVerify_Success = FALSE;
	m_bAutoDS = FALSE;
	m_bStaticToolBar = FALSE;

	m_bTopMost = FALSE;
	
	m_hAutoCalcBkThread = NULL;
}

CMainFrame::~CMainFrame()
{
	TDEL(m_pChoiceGPDlg);
	TDEL(m_pChoiceBKDlg);
	TDEL(m_pSimChoiceDlg);
	TDEL(m_pCleChoiceDlg);
	TDEL(m_pPluginDlg);
	TDEL(m_pSelGP);
	TDEL(m_pTJTestDlg);
	TDEL(m_pJYTestDlg);
	TDEL(m_pTestZBDlg);
	TDEL(m_pZHSortDlg);
	TDEL(m_pHKSeatQueryDlg);
	TDEL(m_pDataDigDlg);
	TDEL(m_pInBlockDlg);
	TDEL(m_pGpGxdDlg);
	TDEL(m_pBlockCalcDlg);
	TDEL(m_pFluxCalcDlg);
	TDEL(m_pVideoDlg);
	TDEL(m_pWebWinDlg);
	TDEL(g_pToolTipMark);
	TDEL(m_pDeepWarnDlg);
	TDEL(m_pUrgentPopDlg);
	TDEL(m_pIWContentDlg);
	TDEL(m_pPopMyIEDlg);
	TDEL(g_pBkData);
	if(m_pTabViewMenu)
	{
		if(m_pTabViewMenu->m_hMenu) m_pTabViewMenu->DestroyMenu();
		TDEL(m_pTabViewMenu);
		UnregisterHotKey(m_hWnd, 1002);
	}
	CLOSE_HANDLE(m_hAutoCalcBkThread);
	m_wndMDIClient.Detach();
}

//画线工具的ID数组
static UINT  drawtools[] =
{
	ID_READY,
	ID_LDXD,
	ID_LDZX,
	ID_ARROW,
	ID_RAY,
	ID_PXX,
	ID_PXZX,
	ID_YHX,
	ID_GOLD, 
	ID_DFBX, 
	ID_BDX, 
	ID_ZX,
	ID_XXHGD,
	ID_XXHGC,
	ID_XXHG,
	ID_ZQX, 
	ID_FBLQ,  
	ID_ZSX, 
	ID_SSGS,
	ID_UPFLAG,
	ID_DOWNFLAG,  
	ID_TXTTOOLS,
	ID_DEL,
	ID_HIDESELFLINE,
	ID_SHOWALLLINE
};

static UINT  drawtools_more[] =
{
	ID_READY,
	ID_LDXD,
	ID_LDZX,
	ID_ARROW,
	ID_RAY,
	ID_PXX,
	ID_PXZX,
	ID_YHX,
	ID_GOLDPRICE,
	ID_GOLDAIM,
	ID_GOLD, 
	ID_DFBX, 
	ID_BDX, 
	ID_XXHGD,
	ID_XXHGC,
	ID_XXHG,
	ID_ZQX, 
	ID_FBLQ,  
	ID_GANNTIME,
	ID_ZSX, 
	ID_SSGS,
	ID_ZX,
	ID_UPFLAG,
	ID_DOWNFLAG,  
	ID_TXTTOOLS,
	ID_DEL,
	ID_HIDESELFLINE,
	ID_SHOWALLLINE
};

void CMainFrame::InitCreateInfo()
{
	//创建 外挂取资金帐号的消息
	CString tmpMutexStr;
	//
	tmpMutexStr=HomePath+"Query_Account";
	tmpMutexStr.Replace("\\"," ");
	QryZJZHMsg_My = RegisterWindowMessage(tmpMutexStr);
	//
	tmpMutexStr=HomePath+"Return_Account";
	tmpMutexStr.Replace("\\"," ");
	RetZJZHMsg_My = RegisterWindowMessage(tmpMutexStr);
	//
	tmpMutexStr=HomePath+"Exit_Getmsg";
	tmpMutexStr.Replace("\\"," ");
	ExtMsg_My = RegisterWindowMessage(tmpMutexStr);
}

void CMainFrame::TestAutoDS()
{
	if(g_GC.bIsSSDS)
	{
		g_nDSAutoStopMinute = 0;
		g_bHasDS = TRUE;
	}
	//对于合一版,交易用户才能使延时扩展行情(DSMustJy开关打开的话)
#ifdef OEM_NEWJY
	if(!g_GC.bIsSSDS && g_OEM.bDSMustJy)
	{
		DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
		if(!(dwStatus&JY_LOGINED))
			g_bHasDS = FALSE;
	}
#endif	
	m_bAutoDS = (g_bAutoDS || g_GC.bIsSSDS);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CNewMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (m_wndMDIClient.Attach(m_hWndMDIClient) == 0)
    {
        TRACE0("Failed to attach MDIClient.\n");
        return -1;      // fail to create
    }
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//填充CmdTable的Often指标
	Refresh_OftenUseZb_CmdTable();

	//初始化进入的一些变量
	InitCreateInfo();

	//判断是否是实时扩展行情
	TestAutoDS();	
	//
	if(g_bHasCaption) g_bDynaZoom = 0;
	if(g_bHasDS && g_nFixedWidth<198) g_nFixedWidth=198;
	if(g_bShowVersoin)
		SetWindowText(_F(g_strTitleName+" V"+g_szReleaseVersion));
	else
		SetWindowText(_F(g_strTitleName));
	//////////////////////
	//工具栏
	//!!!
	g_bShowBigToolBar = TRUE;
	UINT nToolBarIconSize = 39;
	UINT nToolBarID = g_Scheme_King.ToolBarID;
	UINT nToolBarID_Hot = g_Scheme_King.ToolBarID_Hot;
	UINT nToolBarID_Diable = g_Scheme_King.ToolBarID_Disable;
	DWORD tmpStyle;
	tmpStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP |CBRS_BORDER_3D| CBRS_TOOLTIPS;
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT|TBSTYLE_TRANSPARENT|TBSTYLE_TOOLTIPS,tmpStyle) ||
		!m_wndToolBar.LoadToolBar(nToolBarID))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;    
	}
	//加载真m_wndToolBar彩浮动工具栏
	m_wndToolBar.LoadTrueColorToolBar(nToolBarIconSize, nToolBarID,nToolBarID_Hot,nToolBarID_Diable);
	m_bStaticToolBar = TRUE;
	if(m_wndToolBar.m_hWnd) g_pMyToolBar = &m_wndToolBar;
	//////////////////////
	//状态栏	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;
	}
	//设置状态栏的高度
	int nHeight=17;
	if(g_GH.bBigStatusBar)	nHeight=20;
	if(g_GH.Roll.bShow && !g_GH.Roll.bFloat)
		nHeight+=nGetRollBarHeight();
	m_wndStatusBar.GetStatusBarCtrl().SetMinHeight(nHeight);
	//
	UINT nID, nStyle;
	int cxWidth;	
	m_wndStatusBar.GetPaneInfo(0, nID, nStyle, cxWidth);
	m_wndStatusBar.SetPaneInfo(0, nID, nStyle|SBPS_STRETCH,0);
	//////////////////////
	//定时器
	SetTimer(1,StatusRefreshTime*1000,NULL);	//设置状态栏刷新的定时器
	SetTimer(2,4*1000,NULL);					//主力监控的定时器
	SetTimer(3,3000,NULL);						//设置动态维护
	
	SetTimer(4,100,NULL);						//定时检查g_pComm是否激活
	SetTimer(5,DETECT_SECOND*1000,NULL);		//定时向资讯服务器发送存活包,自动探测三方行情,尝试自动重连

	SetTimer(6,12*1000,NULL);					//自选股的行情刷新
	SetTimer(7,g_nWarnRefresh*1000,NULL);		//预警行情刷新
	SetTimer(8,1000,NULL);						//判断g_pComm通讯超时
	SetTimer(9,6*1000,NULL);					//日线统计数据的刷新
	SetTimer(10,8*1000,NULL);					//日线统计数据的刷新
//	SetTimer(13, 30*1000, NULL);
//	SetTimer(14, 10*1000, NULL);
	
	if(g_AdHead.m_nADNum>0)
		SetTimer(12,g_AdHead.m_nADMinRandPopTime*60*1000,NULL);	//处理广告消息的定时器
	SetTimer(27, 200, NULL);
	m_bJyVerify_Success = LocalJyVerify();
	//如果没有校验,就不自动弹出资讯
	if(!m_bJyVerify_Success && g_VipVerify_Info.DenyInfo)
		g_bAutoConnectInfo = FALSE;
	//////////////////////
	m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
	EnableDocking(CBRS_ALIGN_TOP,FALSE);
	DockControlBar(&m_wndToolBar);
	ShowControlBar(&m_wndToolBar,g_Scheme_King.bShowToolBar,FALSE);
	//////////////////////
	//画线工具栏
	EnableDocking(CBRS_ALIGN_ANY);
	BOOL bUseAdvDrawTools = FALSE;
#if defined(LEVEL2)
	bUseAdvDrawTools = TRUE;
#elif defined(PERSONAL)
	if(g_nPersonalVersion==PERSONAL_JJTZEX)
		bUseAdvDrawTools = TRUE;
#endif
	if(bUseAdvDrawTools)
	{
		if (!m_DrawToolsBar.CreateEx(this,TBSTYLE_AUTOSIZE|TBSTYLE_FLAT|TBSTYLE_TRANSPARENT,WS_CHILD | WS_VISIBLE | CBRS_SIZE_FIXED |CBRS_TOP | CBRS_TOOLTIPS,CRect(0,0,0,0),ID_TOOLS) ||
			!m_DrawToolsBar.LoadToolBar(IDR_LINETOOL_MORE))
		{
			TRACE0("Failed to create toolbar\n");
			return -1;    
		}
		m_DrawToolsBar.LoadTrueColorToolBar(16, IDR_LINETOOL_MORE,IDR_LINETOOL_MORE,IDR_LINETOOL_MORE);
	}
	else
	{
		if (!m_DrawToolsBar.CreateEx(this,TBSTYLE_AUTOSIZE|TBSTYLE_FLAT|TBSTYLE_TRANSPARENT,WS_CHILD | WS_VISIBLE | CBRS_SIZE_FIXED |CBRS_TOP | CBRS_TOOLTIPS,CRect(0,0,0,0),ID_TOOLS) ||
			!m_DrawToolsBar.LoadToolBar(IDR_LINETOOLS))
		{
			TRACE0("Failed to create toolbar\n");
			return -1;    
		}
		m_DrawToolsBar.LoadTrueColorToolBar(16, IDR_LINETOOLS,IDR_LINETOOLS,IDR_LINETOOLS);
	}
	ShowControlBar(&m_DrawToolsBar,FALSE,FALSE);
	m_DrawToolsBar.SetWindowText(_F("画线分析"));
	m_DrawToolsBar.EnableDocking(0);
	m_DrawToolsBar.SetColumns(14);
	CPoint pt(GetSystemMetrics(SM_CXSCREEN)-100,GetSystemMetrics(SM_CYSCREEN)/10);
	FloatControlBar(&m_DrawToolsBar, pt);

	//////////////////////////////////////////////////////////////////////////
	//周期工具栏
	if (!m_PerToolBar.CreateEx(this, TBSTYLE_FLAT|TBSTYLE_TRANSPARENT,WS_CHILD | WS_VISIBLE | CBRS_RIGHT |CBRS_BORDER_3D| CBRS_TOOLTIPS) ||
		!m_PerToolBar.LoadToolBar(IDR_PERTOOL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;    
	}
	m_PerToolBar.LoadTrueColorToolBar(16, IDR_PERTOOL,IDR_PERTOOL,IDR_PERTOOLD);

	m_PerToolBar.EnableDocking(CBRS_ALIGN_RIGHT);
//	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_PerToolBar);
	ShowControlBar(&m_PerToolBar,TRUE,FALSE);

	if(g_bShowBigToolBar)
		m_bStaticToolBar = TRUE;

	if(m_bStaticToolBar)
		SetAllButtons();
	else
		SendMessage(UM_HIDESHOWBUTTON,HideShowBtn_Homepage,1);
	//////////////////////
	//功能树
	m_ImageList.Create(IDB_IMAGELIST_TREE, 13, 1, RGB(0,255,0));
	m_wndWorkspace.SetBtnImageList (&m_ImageList);
	m_wndWorkspace.SetControlBarStyle(TRUE,FALSE);
	if (!m_wndWorkspace.Create(this, CG_ID_VIEW_WORKSPACE,
		_F("功能树"), CSize(g_nDefaultZxWidth,0)))
	{
		TRACE0("Failed to create dialog bar m_wndWorkspace\n");
		return -1;
	}
	memset(g_nTreePos,-1,sizeof(long)*20);//全部赋初值
	int nPos=0;
	m_wndWorkspace.m_bAutoSize=TRUE;
#ifdef OEM_NEWJY
	m_wndWorkspace.m_bAutoSize=TRUE;
	if(g_OEM.bCustomQsZXTree)
	{
		nLastTreeSel=nPos;
		if(!g_OEM.bCustomQsZxPosSecond)
		{
			if(g_OEM.bHasMetaCJZX)
			{
				m_wndWorkspace.AddWnd(_F(g_OEM.CustomQsZxTreeName),RUNTIME_CLASS(CZx3Tree));
				g_nTreePos[ZX3_TREETYPE] = nPos++;
			}
			else
			{
				m_wndWorkspace.AddWnd(_F(g_OEM.CustomQsZxTreeName),RUNTIME_CLASS(CZx2Tree));
				g_nTreePos[ZX2_TREETYPE] = nPos++;
			}
			g_GC.bHasZxTree23=TRUE;
			m_wndWorkspace.AddWnd(_F("市场信息"),RUNTIME_CLASS(CZxTree));
			g_nTreePos[ZX_TREETYPE] = nPos++;
		}
		else
		{
			m_wndWorkspace.AddWnd(_F("市场信息"),RUNTIME_CLASS(CZxTree));
			g_nTreePos[ZX_TREETYPE] = nPos++;
			m_wndWorkspace.AddWnd(_F(g_OEM.CustomQsZxTreeName),RUNTIME_CLASS(CZx2Tree));
			g_nTreePos[ZX2_TREETYPE] = nPos++;
			g_GC.bHasZxTree23=TRUE;
		}
		m_wndWorkspace.AddWnd(_F("技术指标"),RUNTIME_CLASS(CZbTree));
		g_nTreePos[ZB_TREETYPE] = nPos++;			
	}
	else
	{
		m_wndWorkspace.AddWnd(_F("功能"),RUNTIME_CLASS(CGnTree));
		g_nTreePos[GN_TREETYPE] = nPos++;
		m_wndWorkspace.AddWnd(_F("资讯"),RUNTIME_CLASS(CZxTree));
		g_nTreePos[ZX_TREETYPE] = nPos++;
		m_wndWorkspace.AddWnd(_F("股票"),RUNTIME_CLASS(CGpTree));
		g_nTreePos[GP_TREETYPE] = nPos++;
		m_wndWorkspace.AddWnd(_F("指标"),RUNTIME_CLASS(CZbTree));
		g_nTreePos[ZB_TREETYPE] = nPos++;
	}
#else
	if(g_bHasWebInfo)
	{
		m_wndWorkspace.AddWnd(_F("服务"),RUNTIME_CLASS(CWebTree));
		g_nTreePos[WEB_TREETYPE] = nPos++;
	}
	m_wndWorkspace.AddWnd(_F("指标公式"),RUNTIME_CLASS(CZbTree));
	g_nTreePos[ZB_TREETYPE] = nPos++;
	m_wndWorkspace.AddWnd(_F("板块分类"),RUNTIME_CLASS(CGpTree));
	g_nTreePos[GP_TREETYPE] = nPos++;
#endif
	m_wndWorkspace.SetBarStyle(m_wndWorkspace.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndWorkspace.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndWorkspace, AFX_IDW_DOCKBAR_LEFT);
	if(!bTreeShow)
		ShowControlBar(&m_wndWorkspace,FALSE,FALSE);
	m_wndWorkspace.SetActiveWnd(nLastTreeSel);
	//////////////////////
	//辅助区
	m_wndRestspace.SetBtnImageList (&m_ImageList);
	m_wndRestspace.SetControlBarStyle(TRUE,TRUE);
	m_wndRestspace.SetRightBtn(g_bRestRightBtn);
	if (!m_wndRestspace.Create(this, CG_ID_VIEW_RESTSPACE,
		_F("辅助区"), CSize(220,g_nDefaultRestHeight)))
	{
		TRACE0("Failed to create dialog bar m_wndWorkspace\n");
		return -1;
	}
#ifdef OEM_NEWJY
	m_wndRestspace.SetGripper(FALSE);
#endif
	memset(g_nRestPos,-1,sizeof(long)*20);//全部赋初值
	BOOL bHasWeb=FALSE;
	if(!bHasWeb) m_wndRestspace.m_bShowTab=FALSE;
	m_wndRestspace.SetBarStyle(m_wndRestspace.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndRestspace.EnableDocking(CBRS_ALIGN_BOTTOM);

	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndRestspace, AFX_IDW_DOCKBAR_BOTTOM);		
	if(!bRestShow)
		ShowControlBar(&m_wndRestspace,FALSE,FALSE);

	m_wndRestspace.SetActiveWnd(nLastRestSel);

#ifdef OEM_NEWJY
	if(g_nEmbedTabSel==LOGIN_ONLYWT)
	{
		m_wndRestspace.SwitchToFullSize(TRUE);
		SwitchToRestSpace(AREA_WT);
		ShowControlBar(&m_wndRestspace,TRUE,FALSE);
		bRestShow=TRUE;
	}
	else
	{
		m_wndRestspace.ResetSize();
		if(g_nEmbedTabSel==LOGIN_HQWT && TC_GetJyStatus_(NULL,NULL,NULL)&JY_LOGINED && !g_OEM.bFirstHideWTFace)
		{
			SwitchToRestSpace(AREA_WT);
			ShowControlBar(&m_wndRestspace,TRUE,FALSE);
			bRestShow=TRUE;
		}
		else
		{
			ShowControlBar(&m_wndRestspace,FALSE,FALSE);
			bRestShow=FALSE;
		}
	}
	if(TC_GetExternLevinCfg_)
		TC_GetExternLevinCfg_(&g_ExternLevinCfg);
	if(TC_SetFeedBackMsg_)
		TC_SetFeedBackMsg_(m_hWnd,UM_LEVIN_FEEDBACK_MSG);
	SetTimer(30, 5000, NULL); //买卖跟随和止损止赢的功能实现
#else
	m_wndRestspace.ResetSize();
#endif
	///////////////////
	LayoutFlag = TRUE;

	//处理刚进入时的一些消息
	PostMessage(UM_ONLINEENTERMSG,0,0);
	
	g_pMainWin = this;

	//处理后台数据类
	g_pBkData = new CBkData;
//	g_pBkData->Create(this);

	//初始化文字标识模块
	g_pToolTipMark = new CToolTipMark(this);
	if(!g_pToolTipMark || !g_pToolTipMark->InitTool())
	{
		TDEL(g_pToolTipMark);
	}
	//初始化预警管理器
	m_WarnManager.InitWarnManager(this);
	//先得到状态栏
	SendMessage(UM_STATUS_REQ,0); 
	SendMessage(UM_STATUS_REQ,1); 
	//连接资讯主站

	//连接扩展行情主站
	if(g_bHasDS && m_bAutoDS && OnLine)
		PostMessage(UM_AUTOCONNECTDS,0,0);
	//滚动资讯的处理
	if(g_GH.Roll.bShow && g_GH.Roll.bFloat)
		PostMessage(WM_COMMAND,IDM_MOVINGINFO);
	//设置快速隐藏DLL
	if(g_bBossKey)
	{
		WORD tmpComKey=0;
		if(g_comkey&HOTKEYF_ALT)
			tmpComKey|=MOD_ALT;
		if(g_comkey&HOTKEYF_CONTROL)
			tmpComKey|=MOD_CONTROL;
		if(g_comkey&HOTKEYF_SHIFT)
			tmpComKey|=MOD_SHIFT;	
		RegisterHotKey(m_hWnd,1001,tmpComKey,g_virtkey);
	}
	//
	Set_OP_Scheme();
	//
	//clean
	DeleteDirInfo(g_WSXHStr+"diary");	RemoveDirectory(g_WSXHStr+"diary");
	DeleteDirInfo(g_WSXHStr+"info_cache");	RemoveDirectory(g_WSXHStr+"info_cache");
	DeleteDirInfo(g_WSXHStr+"tmp");	RemoveDirectory(g_WSXHStr+"tmp");
	unlink(g_WSXHStr+"user.ini");
	//对于外部执行代码，进行try.catch处理
	try
	{
		//如果有随机启动的设置,自动启动
		if(strlen(g_GC.AlongPathFile))
		{
			if(g_OEM.bZxjtEmbedWTVersion)
				PostMessage(WM_COMMAND,ID_FUNC_ZXJTINFO);
			else
				ExecExtern(g_GC.AlongPathFile,m_hWnd,NULL);
		}
		//针对华林版的特别处理
		if(g_OEM.bHlzqEmbedWTVersion)
		{
			DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
			if(dwStatus&JY_LOGINED)
			{
				char tmpPath[MAX_PATH],tmpArg[MAX_PATH];
				sprintf(tmpPath,"%sHLIQ\\TQ.exe",HomePath);
				char szEnAccount[256]={0};
				Hlzq_Encrypt(g_JyInfo.zjzh,szEnAccount);
				sprintf(tmpArg,"TYPE:2,STRID:Z*%s,MYPASS:000000",szEnAccount);
				ExecExtern(tmpPath,m_hWnd,tmpArg);
			}		
		}
		//是否一进入就打开委托界面
	#ifndef OEM_NEWJY
		if(g_GC.bAlongOpenWT)
			PostMessage(WM_COMMAND,ID_FUNC_WT);
	#endif
		//是否一进入就打开核新的资讯中心
		if(g_GC.bAlongOpenHXInfo)
			PostMessage(WM_COMMAND,ID_FUNC_HXINFO);
		//启动通达信TQ系统(安静地运行,不提示)
		if(g_bHasTICQ && g_bAutoConnectTICQ)
		{
			char tmpPath[MAX_PATH];
			sprintf(tmpPath,"%sTQClient.exe",HomePath);
			if(g_bTQAsBroker)
				ExecExtern(tmpPath,m_hWnd,"silent|broker");
			else
				ExecExtern(tmpPath,m_hWnd,"silent|custom");
		}
	}
	catch(...)
	{
	}
	return 0;
}

void CMainFrame::EnableDocking(DWORD dwDockStyle,BOOL bMyDock)
{
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);
	m_pFloatingFrameClass = RUNTIME_CLASS(CMiniDockFrameWnd);
	for (int i = 0; i < 4; i++)
	{
		if (dwDockBarMap[i][1] & dwDockStyle & CBRS_ALIGN_ANY)
		{
			CDockBar* pDock = (CDockBar*)GetControlBar(dwDockBarMap[i][0]);
			if (pDock == NULL)
			{
				if(bMyDock)	pDock = new CMyDockBar;
				else		pDock = new CDockBar;
				if (!pDock->Create(this,WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE|dwDockBarMap[i][1], dwDockBarMap[i][0]))
					AfxThrowResourceException();
			}
		}
	}
}

void CMainFrame::RecalcLayout(BOOL bNotify) 
{	
	CNewMDIFrameWnd::RecalcLayout(bNotify);
	if(!g_bAutoHideBar) return;
	if(m_wndToolBar.m_hWnd&&m_wndToolBar.m_pDockBar)
		m_wndToolBar.m_pDockBar->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
}

void CMainFrame::DockControlBarLeftOf(CToolBar* Bar, CToolBar* LeftOf)
{    
    // 让 MFC 调整所有浮动工具条的尺寸，以便GetWindowRect的到准确结果
    RecalcLayout(TRUE);
    CRect rect;    
    LeftOf->GetWindowRect(&rect);
    rect.OffsetRect(1,0);
    DWORD dw=LeftOf->GetBarStyle();
    UINT  n = 0;
    n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
    n = (dw&CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
    n = (dw&CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
    n = (dw&CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;    
    // 当矩形使用默认参数时，DockControlBar 将在单独的一行里浮动工具条，
    // 通过计算矩形的大小来模拟工具条拖动到指定位置实现浮动。
    DockControlBar(Bar,n,&rect);
}  

void CMainFrame::CreateQsDetectThread()
{
	DWORD	QsDetectThreadID;
	CMainFrame *pMainWnd = this;
	QsDetectThreadHandle = ::CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)QsDetectThread,(LPVOID)pMainWnd,0,&QsDetectThreadID);
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CNewMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CNewMDIFrameWnd::Dump(dc);
}

#endif

void CMainFrame::RefreshStatusBar()
{
	m_wndStatusBar.RedrawStatusWnd();
}

//是否需要在菜单上自画按钮
BOOL CMainFrame::bCustDrawCloseBtn()
{
#ifdef OEM_STAR_HM
	return TRUE;
#endif
	return !g_bHasCaption;
}

//画标题栏
extern BOOL l_bCXMode;
void CMainFrame::OnNcPaint()
{
	CNewMDIFrameWnd::OnNcPaint();
	if(!bCustDrawCloseBtn()) return;
	_OnNcPaint();
}

CRect CMainFrame::_AdvButtonOffset(CRect rect,int x_right)
{
	rect.OffsetRect(-rect.left+x_right-rect.Width(),0);
	return rect;
}

void CMainFrame::_OnNcPaint()
{
	return;

	CNewMenu *pTopMenu = (CNewMenu *)GetMenu();						//得到系统菜单
	if(!pTopMenu) return;
	int i,nItemNum = pTopMenu->GetMenuItemCount();					//得到菜单个数
	CRect ItemRect;
	GetMenuItemRect(m_hWnd,pTopMenu->m_hMenu,nItemNum-1,&ItemRect); //最后一个菜单项的Rect
	CRect TotalRc;
	GetWindowRect(&TotalRc);
	ItemRect.right-=TotalRc.left;
	ItemRect.top  -=TotalRc.top;
	int startx = ItemRect.right+3,Height = 21;
	int Width = TotalRc.Width()-55-startx;
	if(Width<=0 || ItemRect.top>15)
	{
		m_NCInfo.HasNcRect = FALSE;
		return;
	}
	m_NCInfo.HasNcRect = TRUE;
	m_NCInfo.NcRect.SetRect(startx,2,Width+startx,Height);

	CDC *pDC = GetWindowDC();
	CDC memDC;			//创建兼容性的DC
	CBitmap tempBitmap;
	memDC.CreateCompatibleDC (pDC);
	tempBitmap.CreateCompatibleBitmap (pDC,Width,Height);
	CBitmap *pOldBitmap=memDC.SelectObject (&tempBitmap);
//	memDC.FillSolidRect(0,0,m_NCInfo.NcRect.Width(),m_NCInfo.NcRect.Height(),GetSysColor(COLOR_BTNFACE));
	g_d.Draw3DBar3(&memDC, CRect(0,0,m_NCInfo.NcRect.Width(),m_NCInfo.NcRect.Height()), VipColor.BIGTITLECOLOR, FALSE);
	memDC.SetBkMode (TRANSPARENT);
	CString SmallTitle;
	GetActiveFrame()->GetWindowText(SmallTitle);
	if((i=SmallTitle.Find("-")) != -1)
		SmallTitle = SmallTitle.Mid(i+1);
	TrimString(SmallTitle);
	if(SmallTitle.GetLength()>0) 
		SmallTitle = "["+SmallTitle+"]";
	int tmpDiff = max(0,Width-g_VitalBtn.BtnNum*60-2);
	m_NCInfo.nFirstVitalLeft = startx+tmpDiff;
	CRect tmpRc;
	memDC.SetTextColor(RGB(64,64,64));
	for(i=0;i < g_VitalBtn.BtnNum;i++)
	{
		tmpRc.SetRect(tmpDiff+i*60,3,tmpDiff+i*60+57,19);
		g_GUI.DrawVitalCoolBtn(&memDC,tmpRc,g_VitalBtn.BtnStr[i],0);
	}
	int x_right=tmpDiff-2;
	int tempLen3=0;
	for(i=0;i<g_AdvertInfo.nAdvBt;i++)
	{
		g_AdvertInfo.AdvButton[i].rect.top=3;
		g_AdvertInfo.AdvButton[i].rect.bottom=19;
		tmpRc=_AdvButtonOffset(g_AdvertInfo.AdvButton[i].rect,x_right);
		g_GUI.DrawVitalCoolBtn(&memDC,tmpRc,g_AdvertInfo.AdvButton[i].btTxt,0,g_AdvertInfo.AdvButton[i].txtCor);
		x_right-=tmpRc.Width()+2;
		tempLen3+=tmpRc.Width()+2;
	}
	CFont NewFont2;
	NewFont2.CreateFont(14,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,SONG_FONT);
	CFont *OldFont = memDC.SelectObject(&NewFont2);
	int tempLen1 = (memDC.GetTextExtent(g_strPreName)).cx;
	int tempLen2 = (memDC.GetTextExtent(SmallTitle)).cx;

	if(tempLen1+tempLen2 < tmpDiff-tempLen3)
	{
		memDC.SetTextColor (RGB(255,0,0));
		memDC.TextOut((tmpDiff-tempLen2-tempLen1-tempLen3)/2,4,_F(g_strPreName));
		memDC.SetTextColor (RGB(0,0,255));
		memDC.TextOut((tmpDiff-tempLen2-tempLen1-tempLen3)/2+tempLen1,4,SmallTitle);
	}
	memDC.SelectObject(OldFont);
	NewFont2.DeleteObject();
	//从内存DC拷贝到屏幕DC
	pDC->BitBlt (m_NCInfo.NcRect.left,m_NCInfo.NcRect.top,m_NCInfo.NcRect.Width(),m_NCInfo.NcRect.Height(),&memDC,0,0,SRCCOPY);
	memDC.SelectObject(pOldBitmap);
	tempBitmap.DeleteObject();
	memDC.DeleteDC();
	
	if(g_VitalBtn.BtnNum > 0)
	{
		m_NCInfo.NcRect.left = startx;
		m_NCInfo.NcRect.right = startx+tmpDiff;
	}
	//绘制最小化和关闭按钮位图
	CBitmap pBitmap1,pBitmap2,pBitmap3;
	CDC hMem1,hMem2,hMem3;
	pBitmap1.LoadBitmap(IDB_BITMAP2U);
	hMem1.CreateCompatibleDC(pDC);
	CBitmap *oldbmp1 = hMem1.SelectObject(&pBitmap1);
	m_NCInfo.nMinBoxLeft = TotalRc.Width()-55;
	pDC->BitBlt(m_NCInfo.nMinBoxLeft,5,16,16,&hMem1,0,0,SRCCOPY);
	if(IsZoomed())	pBitmap3.LoadBitmap(IDB_BITMAP3U);
	else			pBitmap3.LoadBitmap(IDB_BITMAP3UZ);
	hMem3.CreateCompatibleDC(pDC);
	CBitmap *oldbmp3 = hMem3.SelectObject(&pBitmap3);	
	pDC->BitBlt(TotalRc.Width()-38,5,16,16,&hMem3,0,0,SRCCOPY);

	pBitmap2.LoadBitmap(IDB_BITMAP1U);
	hMem2.CreateCompatibleDC(pDC);
	CBitmap *oldbmp2 = hMem2.SelectObject(&pBitmap2);
	pDC->BitBlt(TotalRc.Width()-21,5,16,16,&hMem2,0,0,SRCCOPY);			

	hMem1.SelectObject(oldbmp1);
	pBitmap1.DeleteObject();
	hMem1.DeleteDC();
	hMem2.SelectObject(oldbmp2);
	pBitmap2.DeleteObject();
	hMem2.DeleteDC();
	hMem3.SelectObject(oldbmp3);
	pBitmap3.DeleteObject();
	hMem3.DeleteDC();

	ReleaseDC(pDC);
}

BOOL CMainFrame::DownOrUpIt(int which,CPoint point,BOOL bDown)
{
	if(which >= 10 && which < g_VitalBtn.BtnNum+10)
	{
		int left=m_NCInfo.nFirstVitalLeft+(which-10)*60;
		CRect rect,TotalRc;
		GetWindowRect(&TotalRc);
		point.x-=TotalRc.left;
		point.y-=TotalRc.top;
		rect.SetRect(left,5,left+57,21);
 		if(rect.PtInRect (point))  //如果点在某个自画按钮上
		{
 			CDC *hDC;
 			hDC=GetWindowDC();
			hDC->SetBkMode (TRANSPARENT);
			g_GUI.DrawVitalCoolBtn(hDC,rect,g_VitalBtn.BtnStr[which-10],bDown?1:0);
 			ReleaseDC(hDC);			
			return TRUE;
		}
		return FALSE;
	}
	if(which>=100 && which<g_AdvertInfo.nAdvBt+100)
	{
		int nIndex=which-100;
		CRect rect,TotalRc;
		GetWindowRect(&TotalRc);
		point.x-=TotalRc.left;
		point.y-=TotalRc.top;
		rect=g_AdvertInfo.AdvButton[nIndex].rect;
		int left=m_NCInfo.nFirstVitalLeft;
		for(int i=0;i<=nIndex;i++)
			left-=g_AdvertInfo.AdvButton[i].rect.Width()+2;
		rect.OffsetRect(-rect.left+left,-rect.top+5);
		rect.bottom=21;
		if(rect.PtInRect(point))
		{
			CDC *hDC;
			hDC=GetWindowDC();
			hDC->SetBkMode(TRANSPARENT);
			g_GUI.DrawVitalCoolBtn(hDC,rect,g_AdvertInfo.AdvButton[nIndex].btTxt,bDown?1:0,g_AdvertInfo.AdvButton[nIndex].txtCor);
			ReleaseDC(hDC);
			return TRUE;
		}
		return FALSE;
	}
	int left=m_NCInfo.nMinBoxLeft+which*17;
	UINT BitmapID;
	if(bDown)
		BitmapID = (which==0)?IDB_BITMAP2D:(which==1?IDB_BITMAP3D:IDB_BITMAP1D);
	else
		BitmapID = (which==0)?IDB_BITMAP2U:(which==1?IDB_BITMAP3U:IDB_BITMAP1U);

	CRect rect,TotalRc;
	GetWindowRect(&TotalRc);
	point.x-=TotalRc.left;
	point.y-=TotalRc.top;
	rect.SetRect(left,5,left+16,19);
 	if(rect.PtInRect (point))  //如果点在某个自画按钮上
	{
 		CDC *hDC;
 		hDC=GetWindowDC();
 		CBitmap pBitmap,*pOldbm;
		if(which==1 && !IsZoomed())	
			pBitmap.LoadBitmap(bDown?IDB_BITMAP3DZ:IDB_BITMAP3UZ);
		else
			pBitmap.LoadBitmap(BitmapID);
 		CDC hMem;
 		hMem.CreateCompatibleDC(hDC);
 		pOldbm=hMem.SelectObject(&pBitmap);
		hDC->BitBlt(left,5,16,16,&hMem,0,0,SRCCOPY);
		hMem.SelectObject(pOldbm);
		pBitmap.DeleteObject();
		hMem.DeleteDC();
 		ReleaseDC(hDC);
		return TRUE;
 	}
	return FALSE;
}

void CMainFrame::OnNcMouseMove(UINT nHitTest, CPoint point) 
{
	CNewMDIFrameWnd::OnNcMouseMove(nHitTest, point);
	if(!bCustDrawCloseBtn()) return;
	if(m_NCInfo.bMinDown || m_NCInfo.bRestoreDown || m_NCInfo.bCloseDown || m_NCInfo.bVitalDown>=0 || m_NCInfo.bAdvButtonDown>=0)
	{
		if(m_NCInfo.bVitalDown >= 0)
		{
			int left=m_NCInfo.nFirstVitalLeft+m_NCInfo.bVitalDown*60;
			CRect rect,TotalRc;
			GetWindowRect(&TotalRc);
			point.x-=TotalRc.left;
			point.y-=TotalRc.top;
			rect.SetRect(left,5,left+57,21);
 			if(!rect.PtInRect (point))  //如果在某按钮上按住鼠标并移出了按钮范围
			{
 				CDC *hDC;
 				hDC=GetWindowDC();
				hDC->SetBkMode (TRANSPARENT);
				g_GUI.DrawVitalCoolBtn(hDC,rect,g_VitalBtn.BtnStr[m_NCInfo.bVitalDown],0);
	 			ReleaseDC(hDC);			
				m_NCInfo.bMinDown = FALSE;
				m_NCInfo.bRestoreDown = FALSE;
				m_NCInfo.bCloseDown = FALSE;
				m_NCInfo.bVitalDown = -1;
				m_NCInfo.bAdvButtonDown = -1;
			}
			return;
		}
		if(m_NCInfo.bAdvButtonDown>=0)
		{
			CRect rect,TotalRc;
			int nIndex=m_NCInfo.bAdvButtonDown;
			GetWindowRect(&TotalRc);
			point.x-=TotalRc.left;
			point.y-=TotalRc.top;
			rect=g_AdvertInfo.AdvButton[nIndex].rect;
			rect.top=5;
			rect.bottom=21;
			int left=m_NCInfo.nFirstVitalLeft;
			for(int i=0;i<=nIndex;i++)
				left-=g_AdvertInfo.AdvButton[i].rect.Width()+2;
			rect.OffsetRect(left,0);
			if(!rect.PtInRect(point))//如果在某按钮上按住鼠标并移出了按钮范围
			{
				CDC *hDC;
				hDC=GetWindowDC();
				hDC->SetBkMode(TRANSPARENT);
				g_GUI.DrawVitalCoolBtn(hDC,rect,g_AdvertInfo.AdvButton[nIndex].btTxt,0,g_AdvertInfo.AdvButton[nIndex].txtCor);
				ReleaseDC(hDC);
				m_NCInfo.bMinDown = FALSE;
				m_NCInfo.bRestoreDown = FALSE;
				m_NCInfo.bCloseDown = FALSE;
				m_NCInfo.bVitalDown = -1;
				m_NCInfo.bAdvButtonDown = -1;
			}
			return;
		}
		int which;
		if(m_NCInfo.bMinDown) which = 0;
		if(m_NCInfo.bRestoreDown) which = 1;
		if(m_NCInfo.bCloseDown) which = 2;
		int left=m_NCInfo.nMinBoxLeft+which*17;
		CRect rect,TotalRc;
		GetWindowRect(&TotalRc);
		point.x-=TotalRc.left;
		point.y-=TotalRc.top;
		rect.SetRect(left,5,left+16,19);
 		if(!rect.PtInRect(point))  //如果点在某个自画按钮上
		{
			UINT BitmapID = (which==0)?IDB_BITMAP2U:(which==1?IDB_BITMAP3U:IDB_BITMAP1U);
 			CDC *hDC=GetWindowDC();
 			CBitmap pBitmap,*pOldbm;
			if(which==1 && !IsZoomed())	
				pBitmap.LoadBitmap(IDB_BITMAP3UZ);
			else
				pBitmap.LoadBitmap(BitmapID);
 			CDC hMem;
 			hMem.CreateCompatibleDC(hDC);
 			pOldbm=hMem.SelectObject(&pBitmap);
			hDC->BitBlt(left,5,16,16,&hMem,0,0,SRCCOPY);
			hMem.SelectObject(pOldbm);
			pBitmap.DeleteObject();
			hMem.DeleteDC();
 			ReleaseDC(hDC);			
			m_NCInfo.bMinDown = FALSE;
			m_NCInfo.bRestoreDown = FALSE;
			m_NCInfo.bCloseDown = FALSE;
			m_NCInfo.bVitalDown = -1;
			m_NCInfo.bAdvButtonDown = -1;
		}
	}
}

//控制最小化按钮和关闭按钮
void CMainFrame::OnNcLButtonDown( UINT nHitTest, CPoint point)
{
	CNewMDIFrameWnd::OnNcLButtonDown(nHitTest,point);
	if(!bCustDrawCloseBtn()) return;
	if(m_NCInfo.HasNcRect)
	{
		for(int i=0;i < g_VitalBtn.BtnNum;i++)
		{
			if(DownOrUpIt(10+i,point,TRUE)) 
			{
				m_NCInfo.bVitalDown = i;
				return;
			}
		}
		for(i=0;i<g_AdvertInfo.nAdvBt;i++)
		{
			if(DownOrUpIt(100+i,point,TRUE))
			{
				m_NCInfo.bAdvButtonDown=i;
				return;
			}
		}
		if(DownOrUpIt(0,point,TRUE))	m_NCInfo.bMinDown	= TRUE;
		if(DownOrUpIt(1,point,TRUE))	m_NCInfo.bRestoreDown = TRUE;
		if(DownOrUpIt(2,point,TRUE))	m_NCInfo.bCloseDown = TRUE;
	}
}

void CMainFrame::OnNcLButtonUp(UINT nHitTest, CPoint point) 
{
	CNewMDIFrameWnd::OnNcLButtonUp(nHitTest, point);
	if(!bCustDrawCloseBtn()) return;
	if(m_NCInfo.HasNcRect)
	{
		for(int i=0;i < g_VitalBtn.BtnNum;i++)
		{
			if(DownOrUpIt(10+i,point,FALSE)) 
			{
				m_NCInfo.bVitalDown = -1;
				m_NCInfo.bAdvButtonDown = -1;
				if(g_VitalBtn.BtnType[i]==0)
				{
					if(strnicmp(g_VitalBtn.BtnExecFile[i],"http",4)) //如果是路径
					{
						char execfile[MAX_PATH];
						sprintf(execfile,"%s%s",HomePath,g_VitalBtn.BtnExecFile[i]);
						ExecExtern(execfile,m_hWnd);
					}
					else											 //如果是链接
						OpenCusWebPage(this,g_VitalBtn.BtnExecFile[i],g_OEM.bMainWebPageInsideOpen);
				}
				else
					PostMessage(WM_COMMAND,g_VitalBtn.BtnID[i]);
				break;
			}
		}
		for(i=0;i<g_AdvertInfo.nAdvBt;i++)
		{
			if(DownOrUpIt(100+i,point,FALSE))
			{
				m_NCInfo.bVitalDown = -1;
				m_NCInfo.bAdvButtonDown=-1;
				if(strlen(g_AdvertInfo.AdvButton[i].url)>0)
				{
					CString tempStr=g_AdvertInfo.AdvButton[i].url;
					if(tempStr.Find("http://")!=-1)
						ShellExecute(NULL, _T("open"),g_AdvertInfo.AdvButton[i].url,NULL,NULL,SW_SHOW);
					else
					{
						CAdvDlg dlg;
						dlg.SetAdvIndex(i);
						dlg.DoModal();
					}
				}
			}
		}
		if(DownOrUpIt(0,point,FALSE))
		{
			m_NCInfo.bMinDown	= FALSE;
			OnMinimize();
		}
		if(DownOrUpIt(1,point,FALSE))
		{
			m_NCInfo.bRestoreDown	= FALSE;
			OnRestore();
		}
		if(DownOrUpIt(2,point,FALSE))
		{
			m_NCInfo.bCloseDown	= FALSE;
			PostMessage(WM_CLOSE);
		}
	}
}

//外部函数
extern void WriteToUserINI();
extern void WriteToOther();
#include "DlgLogout.h"
void CMainFrame::OnClose() 
{
   	if(g_bDonotClose)
	{
		g_bSpecClose = FALSE;
		return;
	}
	if(!g_bDirectQuit && !g_bSpecClose)	//如果是程序内部退出，则不提示退出
	{
	#ifdef OEM_NEWJY
 		CDlgLogout dlg;
		dlg.DoModal();
		int nExitCode=dlg.m_nExitCode;
		if(nExitCode==0) return;
		if(nExitCode==2)
			g_bReLoginStatus = TRUE;
		else
			g_bReLoginStatus = FALSE;
	#else
		if(TDX_MessageBox (m_hWnd,"您确定退出本系统吗？",MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL)
			return; 		
	#endif
	}
	//提示盘后下载数据
	if(!g_bReLoginStatus && !g_bDirectQuit && !g_bSpecClose && g_bCanDown)
	{
		SYSTEMTIME  nowtime;
		GetCorrespondingTime( &nowtime );
		int m = nowtime.wHour*60+nowtime.wMinute;  //分钟
		if(g_bNotifyDownload && ( (m>940 && !g_bYesterdayFlag) || (m>630 && g_bYesterdayFlag) ) && GetVipDocLastDate()<g_lOpenRq)
			DownDayData_AtOnce(this,"退出前,您是否要下载补全日线数据?\n\n(选股,报表分析,脱机浏览等功能需要日线数据,如果不使用这些功能,不必下载)");
	}

	//向系统广播退出消息 退出,WPARAM参数定为1
	::PostMessage(HWND_BROADCAST,ExtMsg,1,0);
	::PostMessage(HWND_BROADCAST,ExtMsg_My,1,0);
	//针对华林证券TQ的处理
	if(g_OEM.bHlzqEmbedWTVersion)
	{
		char tmpPath[MAX_PATH];
		sprintf(tmpPath,"%sHLIQ\\TQ.exe",HomePath);
		ExecExtern(tmpPath,m_hWnd,"TYPE:11");
	}

	g_pMainWin = NULL;
	LayoutFlag = FALSE;
	EnableWindow(FALSE);
	
	if(IsWindowVisible())
	{
		//消去全屏状态,以便得到功能树等的状态
		if(m_pWndFullScreen != NULL)	EscapeFullScreen();
		
		//得到关闭时功能树的状态
		bTreeShow		= m_wndWorkspace.IsWindowVisible(); 
		bRestShow		= m_wndRestspace.IsWindowVisible(); 
		if(!g_bAutoHideBar)
			bToolBarShow = m_wndToolBar.IsWindowVisible();
		bStatusBarShow	= m_wndStatusBar.IsWindowVisible();
		nLastTreeSel	= m_wndWorkspace.GetCurWnd();
		nLastRestSel	= m_wndRestspace.GetCurWnd();
	}
	
	if(g_bBossKey)
		UnregisterHotKey(m_hWnd,1001); //关闭快速隐藏DLL
	try
	{
		if(g_OEM.bHasQSRun)
			QS_MainFrameClosed_(0);
		CloseMDIWindow(FALSE); //先关闭各子窗口	
		theViewManager.RemoveAll();
		theViewManager.bClosing = TRUE;
		//必须在OnClose后才存INI，因为在各子窗口关闭时，保存了FirstGPType后信息
		WriteToUserINI();
		WriteToOther();

		//从Clipboard Viewer Chain注销本窗口 	
		if(m_bClipSpy)
		{
			if(m_hWndNextViewer)
				::ChangeClipboardChain(m_hWnd, m_hWndNextViewer);
			m_hWndNextViewer=NULL;
			m_bClipSpy = FALSE;
		}
		TDEL(g_pKeyWordSearch);
		if(g_bMin3Tray)
			NotifyMessage(NIM_DELETE,m_hIcon);
		TDEL(g_pWriteUrgent);

		InterlockedExchange(&g_nAutoCalcLock, 1);
		if(m_hAutoCalcBkThread!=NULL&&WaitForSingleObject(m_hAutoCalcBkThread,1000)!=WAIT_OBJECT_0)
			TerminateThread(m_hAutoCalcBkThread,0);

		SkinRemove();
	}
	catch(...)
	{
	}
	CNewMDIFrameWnd::OnClose();	//现在才调用基类函数OnClose();
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CNewMDIFrameWnd::OnSize(nType, cx, cy);
	if(LayoutFlag)
	{
		m_wndWorkspace.OnMainFrameSize();
		m_wndRestspace.OnMainFrameSize();	
	}
}

char OtherZsStr[6][7]={SZ_DPCODE,SZ_ADPCODE,SZ_BDPCODE,SH_DPCODE,SH_ADPCODE,SH_BDPCODE};

void CMainFrame::SendStatusReq(WPARAM wParam,LPARAM lParam) //wParam,0:状态栏 1:统计数据
{
	char *CodeHead = new char[200];
	char  setcode;
	int i,j,nCnt=0;
	if(wParam==0)
	{
		MULIDX	MulIdx[3]={0};
		int		nDSNum=0;
		for(i = 0;i < 3;i++)
		{
			if(g_StatusGP[i].setcode!=SZ && g_StatusGP[i].setcode!=SH)
			{
				MulIdx[nDSNum].Market = g_StatusGP[i].setcode;
				memcpy(MulIdx[nDSNum].Code,g_StatusGP[i].Code,CODE_SIZE);
				nDSNum++;
			}
			else
			{
				memcpy(CodeHead+nCnt*(1+SH_CODE_LEN),&g_StatusGP[i].setcode,1);
				memcpy(CodeHead+nCnt*(1+SH_CODE_LEN)+1,g_StatusGP[i].Code,SH_CODE_LEN);
				nCnt++;
			}
		}
		g_nMainID = MainIDPlus(m_nMainID,MAIN_WIN);
		g_nAssisID = STATUSZSHQ;
		if(nDSNum>0)
			g_pGetData->DSGetMulCurrData(MulIdx,nDSNum);
		if(nCnt>0)
			g_pGetData->GetMultiCurrStockData(CodeHead,nCnt);
	}
	else if(wParam==1)
	{
		for(i = 0;i < 6;i++)
		{
			if(i < 3)	setcode = SZ;
			else		setcode = SH;
			for(j=0;j<3;j++)
			{
				if( setcode==g_StatusGP[j].setcode && (memcmp(g_StatusGP[j].Code,OtherZsStr[i],SH_CODE_LEN)==0) )
					break;
			}
			if(j>=3)
			{
				memcpy(CodeHead+nCnt*(1+SH_CODE_LEN),&setcode,1);
				memcpy(CodeHead+nCnt*(1+SH_CODE_LEN)+1,OtherZsStr[i],SH_CODE_LEN);
				nCnt++;
			}
		}
		g_nMainID = MainIDPlus(m_nMainID,MAIN_WIN);
		g_nAssisID = STATUSZSHQ;
		g_pGetData->GetMultiCurrStockData(CodeHead,nCnt);
	}
	delete CodeHead;
}

void CMainFrame::AfterLoadZXG()
{
	try
	{
		if((g_bAllWarnSwitch||g_nUnitRadarFlag>0) && g_WarnParam2.nWarnFlag==1)
			m_WarnManager.GetWarnGpSet();
	}
	catch(...)
	{
	}
}

//定时器处理函数
void CMainFrame::OnTimer(UINT nIDEvent)
{
	static unsigned short oldmin = 0;
	static int AliveCInt = 0;
	static int QzHqCInt = 0;
	static int DeepWarnInt = 0;
	static int DetetectCInt = 0;

	BOOL bIdle = IsIdleStatus();
	short CtrlDown=::GetKeyState(VK_CONTROL);
	if(CtrlDown>=0&&m_pTabViewMenu)
	{
		keybd_event(VK_RETURN,0,KEYEVENTF_EXTENDEDKEY,0);
		keybd_event(VK_RETURN,0,KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP,0);
		if(m_pTabViewMenu->m_hMenu) m_pTabViewMenu->DestroyMenu();
		TDEL(m_pTabViewMenu);
		UnregisterHotKey(m_hWnd, 1002);
	}
	if(CtrlDown>=0)
		m_bPadMenuShow = FALSE;

	if(nIDEvent == 1)		//刷新状态栏
	{
		if(OnLine)
		{
		//	SendMessage(UM_TIMERMORE_REQ);
			if(!g_bDowning)
			{
				AliveCInt = (AliveCInt+1)%30000;
				if(AliveCInt%6==1 && !bIdle)
					SendMessage(UM_STATUS_REQ,1);
				else if(AliveCInt%6!=2 && !bIdle)
					SendMessage(UM_STATUS_REQ,0);
				else
				{
					g_nMainID = MainIDPlus(m_nMainID,MAIN_WIN);
					g_nAssisID = HQALIVE_ASSISID;
					g_nPriority = 2;
					g_pGetData->HqAlive();
					g_nPriority = 1;
				}
			}
			if(g_bDowning || bIdle)
				RefreshStatusBar();
		}
	}
	else if(nIDEvent == 2)//刷新主力监控
	{
		if((m_pDeepWarnDlg || g_nUnitDeepWarnFlag>0) && !g_bDowning && OnLine)
		{
			DeepWarnInt = (DeepWarnInt+1)%30000;
			g_nMainID = MainIDPlus(m_nMainID,MAIN_WIN);
			if(DeepWarnInt%2==0)
			{
				g_nAssisID = SZ_DWARN_ASSISID;
				g_pGetData->GetG2DeepWarn(SZ,g_DWarnInf.nSZIDPos,MAX_DEEPWARN);
			}
			else
			{
				g_nAssisID = SH_DWARN_ASSISID;
				g_pGetData->GetG2DeepWarn(SH,g_DWarnInf.nSHIDPos,MAX_DEEPWARN);
			}
		}
	}
	else if(FALSE&&nIDEvent == 3)//设置动态维护
	{
		SYSTEMTIME  nowtime;
		GetLocalTime( &nowtime );
		unsigned short nowmin=nowtime.wHour*60+nowtime.wMinute;
		//如果过零点,则重新进入主站
		if(nowmin < oldmin && OnLine)
		{
			//先主站断开连接
			g_pComm->Disconnect();
			g_nDSAutoConnectFlag = g_nAutoConnectFlag = AUTOCONNECT_RESET;
			//再重新连接进入
			LogoDlg_ReConnect();
		}
		oldmin = nowmin;
	}
	else if(nIDEvent == 4)//定时检查通讯是否激活
	{		
		if(OnLine && g_pComm)
		{
			g_pComm->m_EvenSection.Lock();
			if(g_pComm->m_bBuffEven && g_pComm->m_pThread)
				SetEvent(g_pComm->m_pThread->m_hStartEvent);
			g_pComm->m_EvenSection.Unlock();
		}
		if(DSOnLine && g_pComm3)
		{
			g_pComm3->m_EvenSection.Lock();
			if(g_pComm3->m_bBuffEven && g_pComm3->m_pThread)
				SetEvent(g_pComm3->m_pThread->m_hStartEvent);
			g_pComm3->m_EvenSection.Unlock();
		}
	}
	else if(nIDEvent == 5)	//发送资讯存活包或行情报价雷数目包,尝试自动重连
	{
		DetetectCInt = (DetetectCInt+1)%30000;
		if(DetectHackProc())
		{
			exit(0);
			return;
		}
		else if(DetetectCInt%3==0)
		{			
			//防止外面通过杀进程的方式来实现一帐号多次登录
			if(OnLine && g_bUserHostEverConnect && !g_pWriteUrgent && !g_AntiHack.bUserHostEverBOOL)
				SendMessage(WM_COMMAND,ID_SILENTDISCONNECT);
			g_AntiHack.bUserHostEverBOOL = FALSE;
		}
		if(DSOnLine)
		{
			InterlockedIncrement(&g_nDSAutoStopTimer);
			if(g_nDSAutoStopMinute>0 && g_nDSAutoStopTimer>g_nDSAutoStopMinute)
			{
				g_pComm3->Disconnect();
				DSOnLine = FALSE;
				g_nDSAutoConnectFlag = AUTOCONNECT_RESET;
			}
		}
		if(DSOnLine && g_pGetData) //第三市场存活包,暂用
		{
			g_nMainID = MainIDPlus(m_nMainID,MAIN_WIN);
			g_nAssisID = DS_HQALIVE_ASSISID;
			g_pGetData->DSGetSrvInfo();
		}
		if(!OnLine && g_nAutoConnectFlag==AUTOCONNECT_RECONNECT) //定时尝试自动重连
		{
			PostMessage(UM_AUTOCONNECT,0,0);
			return;
		}
		if(!OnLine && g_nAutoConnectFlag==AUTOCONNECT_PENDING)
		{
			g_pComm->Disconnect();	//断开连接,如果在一段时间内还没有自动连接成功,则断开此连接,重新开始
			g_nAutoConnectFlag = AUTOCONNECT_RECONNECT;
			return;
		}
		if(!DSOnLine && g_nDSAutoConnectFlag==AUTOCONNECT_RECONNECT)
		{
			PostMessage(UM_AUTOCONNECT,0,1);
			return;
		}
 		if(!DSOnLine && g_nDSAutoConnectFlag==AUTOCONNECT_PENDING)
		{
			g_pComm3->Disconnect();	//断开连接,如果在一段时间内还没有自动连接成功,则断开此连接,重新开始
			g_nDSAutoConnectFlag = AUTOCONNECT_RECONNECT;
			return;
		}
	}
	else if(nIDEvent == 6 && !g_bDowning && !bIdle)
	{
		if(g_GH.Roll.bShow && (g_GH.Roll.nInfoType[0]==1 || g_GH.Roll.nInfoType[1]==1) && OnLine && g_nMovingZXG > 0)
		{
			//仍然当作预警股票请求
			char *CodeHead = new char[(1+SH_CODE_LEN)*MULHQ_WANTNUM];
			char *CodeHead3 = new char[sizeof(MULIDX)*MULHQ_WANTNUM];
			memset(CodeHead,0,(1+SH_CODE_LEN)*MULHQ_WANTNUM);
			memset(CodeHead3,0,sizeof(MULIDX)*MULHQ_WANTNUM);

			int nTotalZXG = min(MULHQ_WANTNUM,g_nMovingZXG);
			if(nTotalZXG<0) nTotalZXG=0;
			int nCount = 0, nCount3 = 0;
			MemStkInfo *pStkInfo;
			for(int i=0; i<nTotalZXG; i++)
			{
				pStkInfo=g_pStockIO->Get_MemStkInfo_FromMultiBuf(g_strMovingZXG[i]);
				if(!pStkInfo) continue;
				if(pStkInfo->setcode==SZ || pStkInfo->setcode==SH)
				{
					memcpy(CodeHead+nCount*(1+SH_CODE_LEN),&(pStkInfo->setcode),1);
					memcpy(CodeHead+nCount*(1+SH_CODE_LEN)+1,pStkInfo->Code,SH_CODE_LEN);
					nCount++;
				}
				else
				{
					memcpy(CodeHead3+nCount3*sizeof(MULIDX),&pStkInfo->setcode,1);
					memcpy(CodeHead3+nCount3*sizeof(MULIDX)+1,pStkInfo->Code,CODE_SIZE);
					nCount3++;
				}
			}
			g_nMainID = MainIDPlus(m_nMainID,MAIN_WIN);
			g_nAssisID = REFRESHHQ_ASSISID;
			g_nPriority = 2;
			if(nCount>0 && OnLine) 
				g_pGetData->GetMultiCurrStockData(CodeHead,nCount);
			if(nCount3>0 && DSOnLine) 
				g_pGetData->DSGetMulCurrData((PMULIDX)CodeHead3,nCount3);
			g_nPriority = 1;

			delete CodeHead;
			delete CodeHead3;
		}
	}
	else if(nIDEvent == 7 && (g_bAllWarnSwitch||g_nUnitRadarFlag>0||g_bTjWarnSwitch) && OnLine && !g_bDowning && !bIdle)	//预警刷新
	{		
		if(m_nWarnReq<1) 
			m_WarnManager.GetWarnGpSet();
		if((Is_Jy_Time()||m_nWarnReq<2) && g_nWarnGpNum>0) //发送预警行情请求
		{
			char *CodeHead = new char[(1+SH_CODE_LEN)*GRIDHQ_MAXNUM];
			char *CodeHead3 = new char[sizeof(MULIDX)*GRIDHQ_MAXNUM];
			memset(CodeHead,0,(1+SH_CODE_LEN)*GRIDHQ_MAXNUM);
			memset(CodeHead3,0,sizeof(MULIDX)*GRIDHQ_MAXNUM);
			int nCount = 0, nCount3 = 0;
			for(int i=g_nWarnHQPos;i<g_nWarnGpNum && nCount<GRIDHQ_MAXNUM && nCount3<GRIDHQ_MAXNUM;i++)
			{
				if(g_aWarnGpSet[i]<DS_FROM)
				{
					MemStkInfo *tempStkInfo=(*g_pStockIO)[g_aWarnGpSet[i]];
					if(!tempStkInfo) continue;
					memcpy(CodeHead+nCount*(1+SH_CODE_LEN),&(tempStkInfo->setcode),1);
					memcpy(CodeHead+nCount*(1+SH_CODE_LEN)+1,tempStkInfo->Code,SH_CODE_LEN);
					nCount++;
				}
				else
				{
					MemStkInfo *tmpInfo3 = (*g_pStockIO)[g_aWarnGpSet[i]];
					if(!tmpInfo3) continue;
					memcpy(CodeHead3+nCount3*sizeof(MULIDX),&tmpInfo3->setcode,1);
					memcpy(CodeHead3+nCount3*sizeof(MULIDX)+1,tmpInfo3->Code,CODE_SIZE);
					nCount3++;
				}
			}
			g_nMainID = MainIDPlus(m_nMainID,MAIN_WIN);
			g_nAssisID = WARNGPHQ_ASSISID;
			g_nPriority = 2;
			if(nCount>0 && OnLine) 
				g_pGetData->GetMultiCurrStockDataEx(CodeHead,nCount);
			if(nCount3>0 && DSOnLine) 
				g_pGetData->DSGetMulCurrData((PMULIDX)CodeHead3,nCount3);
			g_nPriority = 1;
		
			if(i>=g_nWarnGpNum) 
			{
				g_nWarnHQPos = 0;
				m_nWarnReq++;
			}
			else		 
				g_nWarnHQPos=i;
		
			delete []CodeHead; CodeHead = NULL;
			delete []CodeHead3; CodeHead3 = NULL;
		}
	}
	else if(nIDEvent == 8)	//判断g_pComm通讯超时
	{
		int nTimeCnt = 0;
		if(OnLine && !g_bDowning && g_pComm)
		{
			nTimeCnt = g_pComm->GetSendTimeCnt();
			if(nTimeCnt>MAX_TIMECNT)
			{
				g_pComm->Disconnect();
				AfterSocketClose(0);
			}
			nTimeCnt++;
			g_pComm->SetSendTimeCnt(nTimeCnt);
		}
		if(DSOnLine && !g_bDowning && g_pComm3)
		{
			nTimeCnt = g_pComm3->GetSendTimeCnt();
			nTimeCnt++;
			g_pComm3->SetSendTimeCnt(nTimeCnt);
			if(nTimeCnt>MAX_TIMECNT)
			{
				g_pComm3->Disconnect();
				AfterSocketClose(1);
			}
		}
	}
	else if(nIDEvent == 9 && OnLine && g_bSupportDDE && theViewManager.pActiveView && theViewManager.GetWindowNum()>1)//日线统计数据的刷新
	{
		int nCnt = 0;
		if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)))
			nCnt = ((CComView *)theViewManager.pActiveView)->GetGP_NeedRefreshStatHQ(HQStat_lpnStkIndex,MAXSTAT_MAXNUM,IMG_STAT_HQ);
		else if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))
			nCnt = ((CComView2 *)theViewManager.pActiveView)->GetGP_NeedRefreshStatHQ(HQStat_lpnStkIndex,MAXSTAT_MAXNUM,IMG_STAT_HQ);
		if(nCnt>0)
		{
			g_nMainID = MainIDPlus(m_nMainID,MAIN_WIN);
			g_nAssisID = IMGSTATHQ_ASSISID;
			g_nPriority = 2;
			SendHQStatReq(HQStat_lpnStkIndex,nCnt,IMG_STAT_HQ);
			g_nPriority = 1;
		}
	}
	else if(nIDEvent == 10 && OnLine && g_bSupportDDE && theViewManager.pActiveView && theViewManager.GetWindowNum()>1)//日线统计数据的刷新
	{
		int nCnt = 0;
		if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)))
			nCnt = ((CComView *)theViewManager.pActiveView)->GetGP_NeedRefreshStatHQ(HQStat_lpnStkIndex,MAXSTAT_MAXNUM,TCK_STAT_HQ);
		else if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))
			nCnt = ((CComView2 *)theViewManager.pActiveView)->GetGP_NeedRefreshStatHQ(HQStat_lpnStkIndex,MAXSTAT_MAXNUM,TCK_STAT_HQ);
		if(nCnt>0)
		{
			g_nMainID = MainIDPlus(m_nMainID,MAIN_WIN);
			g_nAssisID = TCKSTATHQ_ASSISID;
			g_nPriority = 2;
			SendHQStatReq(HQStat_lpnStkIndex,nCnt,TCK_STAT_HQ);
			g_nPriority = 1;
		}
	}
	else if(nIDEvent == 11)	//早上初始化只需等5分钟,就直接进入,只使用一次
	{
		KillTimer(11);
		LogoDlg_ReConnect();
	}
	else if(nIDEvent == 13)	//动态刷新条件选股
	{
		//自动条件选股线程
		KillTimer(13);
		if(g_GH.bDynaTJBlock&&(m_hAutoCalcBkThread==NULL||WaitForSingleObject(m_hAutoCalcBkThread,0)==WAIT_OBJECT_0))
			m_hAutoCalcBkThread = CreateThread(NULL,0,_AfxAutoCalcBlkThread,NULL,0,NULL);
	//	SetTimer(13, 5*60*1000, NULL);
	}
	else if(nIDEvent == 14)
	{
		if(WebContentDownload(g_Scheme_King.PopFileUrl, (char*)(LPCSTR)(g_WSXHStr+(CString)"pop.ini")))
			SendMessage(UM_POPMYIE,1);
	}
	else if(nIDEvent==27&&g_bAutoHideBar&&m_wndToolBar.m_pDockBar)			//AutoHideBar
	{
		BOOL  bToolShow= m_wndToolBar.IsVisible();
		BOOL  bNowFloat = m_wndToolBar.IsFloating();
		static BOOL bFloatBar = bNowFloat;
		if(!bFloatBar&&bNowFloat)
			ShowControlBar(&m_wndToolBar, TRUE, FALSE);
		bFloatBar = bNowFloat;
		if(bNowFloat)
		{
			CNewMDIFrameWnd::OnTimer(nIDEvent);
			return;
		}
		//
		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		CRect rc, frc;
		m_wndToolBar.m_pDockBar->GetWindowRect(&rc);
		ScreenToClient(&rc);
		BOOL nTopDock = 10;
	#ifdef OEM_NEWJY
		nTopDock+=g_nTopDockTop;
	#endif
		GetWindowRect(&frc);
		ScreenToClient(&frc);
		if(!frc.PtInRect(pt))
		{
			CNewMDIFrameWnd::OnTimer(nIDEvent);
			return;
		}
		//
		BOOL bTopDock = rc.Width()>rc.Height()&&rc.top<nTopDock;
		BOOL bLeftDock = rc.Width()<rc.Height()&&rc.left<10;
		BOOL bRightDock = rc.Width()<rc.Height()&&rc.right>frc.right-10;
		if(!bToolShow)
		{
			if(bTopDock&&pt.y<=rc.bottom-16)
				ShowControlBar(&m_wndToolBar, TRUE, FALSE);
			if(bLeftDock&&pt.x<=rc.right+6)
				ShowControlBar(&m_wndToolBar, TRUE, FALSE);
			if(bRightDock&&pt.x>=rc.left-6)
				ShowControlBar(&m_wndToolBar, TRUE, FALSE);
		}
		else
		{
			if(bTopDock&&pt.y>rc.bottom)
				ShowControlBar(&m_wndToolBar, FALSE, FALSE);
			if(bLeftDock&&pt.x>rc.right+4)
				ShowControlBar(&m_wndToolBar, FALSE, FALSE);
			if(bRightDock&&pt.x<rc.left-4)
				ShowControlBar(&m_wndToolBar, FALSE, FALSE);
		}
	}
	else if(nIDEvent==30)	//权证的盘面下单
	{
		//如果支持止损止赢单(仅适用于权证),则将所有的权证的行情不断请求
		QzHqCInt = (QzHqCInt+1)%30000;
		if(QzHqCInt%2==0 && OnLine && g_bHasAdvJy && g_ExternLevinCfg.bStop)
		{
			char *CodeHead = new char[MULHQ_WANTNUM*(1+SH_CODE_LEN)];
			MemStkInfo *pStkInfo;
			int i,nType,nCnt=0;
			for(i=0;i<g_pStockIO->GetStockNum(ALL_ALL) && nCnt<MULHQ_WANTNUM;i++)
			{
				pStkInfo = (*g_pStockIO)[i];
				if(!pStkInfo) continue;
				nType = GetStockType(pStkInfo->setcode,pStkInfo->Code);
				if((nType==CODE_SHQZ) || (nType==CODE_SZQZ))
				{
					if(pStkInfo->Name[0]!='E')	//除去功能代码
					{
						memcpy(CodeHead+nCnt*(1+SH_CODE_LEN),&(pStkInfo->setcode),1);
						memcpy(CodeHead+nCnt*(1+SH_CODE_LEN)+1,pStkInfo->Code,SH_CODE_LEN);
						nCnt++;
					}
				}
			}
			if(nCnt>0)
			{
				g_nMainID = MainIDPlus(m_nMainID,MAIN_WIN);
				g_nAssisID = REFRESHHQ_ASSISID;
				g_nPriority = 2;
				g_pGetData->GetMultiCurrStockData(CodeHead,nCnt);
				g_nPriority = 1;
			}
			delete CodeHead;
		}
		DetectLevinCJ(this);
	}
	else if(nIDEvent==100&&g_KeyGuyCnInfo.nKeyGuyCNing==1&&g_KeyGuyCnInfo.pOwner)	//键盘精灵汉字输入
	{
		KillTimer(100);

		CKeyGuy dlg;
		CRect rect;
		g_KeyGuyCnInfo.pOwner->GetWindowRect(&rect);
		dlg.RightBottomPoint = CPoint(rect.right,rect.bottom);	
		dlg.bHasCmd = TRUE;
		dlg.m_edit = (CString)g_KeyGuyCnInfo.strKeyGuyCN;

		memset(&g_KeyGuyCnInfo, 0, sizeof(g_KeyGuyCnInfo));
		
		if( dlg.DoModal() == IDOK && dlg.ReturnType != -1)
			CKeyGuy::OnCommonProcess(dlg.ReturnType,dlg.ReturnValue);
	}
	CNewMDIFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::CancelModalWindows()
{
	CWnd *pActiveWnd = NULL;
	if(g_pMainWin)	g_pMainWin->GetLastActivePopup(); //关闭最近一次激活的窗口
	if(!pActiveWnd) pActiveWnd = g_pMainWin;
	g_bDowning = FALSE;
	if(pActiveWnd)
		pActiveWnd->SendMessage(WM_COMMAND,IDCANCEL,0); //将此模态对话框消掉
	if(g_pHisDlg)
		g_pHisDlg->SendMessage(WM_COMMAND,IDCANCEL,0);

}

void CMainFrame::LogoDlg_ReConnect()	//弹出对话框重新连接
{
	if(!OnLine && g_pMainWin && LayoutFlag)
	{
		//先去掉前面的模态对话框
		CancelModalWindows();
		
		srand(GetTickCount());
		Sleep((rand()%4+1)*500);
		PostMessage(WM_COMMAND,ID_CONNECT);
		g_nTmpDirectConnectHq = TRUE;
	}
}

void CMainFrame::OnUpdateExternInfo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(g_bExtern_Info);
}

void CMainFrame::OnExternInfo()
{
	if(g_bExtern_Info)
	{
		char path[MAX_PATH];
		BOOL bDone = FALSE;
		do
		{
			GetPrivateProfileString("EXTERN","INFOPATH","*****",path,255,g_WSXHStr+g_strYxhjCfg);
			if(ExecExtern(path,m_hWnd))
				bDone = TRUE;
			else
			{
				if(g_GC.nShareMode) //如果是共享模式,不让用户设置交易委托的路经
				{
					TDX_MessageBox(m_hWnd,"执行外挂资讯程序失败!",MB_OK|MB_ICONERROR);
					return;
				}				
				CFindExeDlg dlg;
				dlg.m_strExeInfo = _F("请输入外挂资讯程序的路径:");
				dlg.m_strExePath = path;
				if(dlg.DoModal() == IDOK)
				{
					strcpy(path,dlg.m_strExePath);
					WritePrivateProfileString("EXTERN","INFOPATH",path,g_WSXHStr+g_strYxhjCfg);
					continue;
				}
				bDone = TRUE;
			}
		}while(!bDone);
	}
}

extern void SaveBlockCfg();
extern void LoadBlockColorCfg(LPBLOCKCOLOR pBlockColor);
extern void SaveBlockColorCfg(LPBLOCKCOLOR pBlockColor);
extern void SaveScheme();
void CMainFrame::OnHqTabOption()
{
	m_nFirstOptionPage = 5;
	OnOption();
	m_nFirstOptionPage = 0;
}

void CMainFrame::OnBlockOption()
{
	CPropertySheet OptionSheet("");
	OptionSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CBlockPage		BlockPage;
	CBlockColorPage	BlockColorPage;

	OptionSheet.AddPage (&BlockPage);
//	OptionSheet.AddPage (&BlockColorPage);
	OptionSheet.SetTitle (_F("用户板块设置"));
	OptionSheet.SetActivePage(&BlockPage);
	if(OptionSheet.DoModal() == IDOK)
	{
		//如果板块股有变化
		if( BlockPage.bBlockGPModify)
		{
			CString tempFileName,tempLine;
			DWORD	EndPosition;
			tempFileName = (CString)g_strBlockNew+GetBlockFileName(BlockPage.nCurBlockIndex)+(CString)".blk";

			CStdioFile m_File;
			if( m_File.Open(HomePath+tempFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite|CFile::shareDenyNone) )
			{
				MemStkInfo *tempStkInfo;
				for(int i = 0;i < BlockPage.AddedGPNum;i++)
				{
					tempStkInfo=(*g_pStockIO)[BlockPage.AddedGPIndex[i]];
					BOOL bDSStock = g_pStockIO->BeDSStock(tempStkInfo);
					tempLine.Format(bDSStock?"%d#%s":"%d%s",tempStkInfo->setcode,tempStkInfo->Code);
					m_File.WriteString((CString)"\n"+tempLine);
				}
				EndPosition = m_File.GetPosition();
				m_File.SetLength(EndPosition);
				m_File.Close();
			}
			else
				TDX_MessageBox(m_hWnd,"不能打开板块股文件!",MB_ICONEXCLAMATION+MB_OK);
			g_bBlockModified = TRUE;
			theViewManager.pActiveView->PostMessage(UM_BLOCKMODIFIED,0,0);//在GridView中马上出现
		}
	}
//	SaveBlockColorCfg(g_BlockColor);
	LoadColorfulBlockGP();

	if( BlockPage.bBlockModify)
	{
		//将板块配置文件存盘
		SaveBlockCfg();
		FillGPTree();
		RefreshCusGridTab(); //重新刷新自定义标签
		for(int i = 0;i<theViewManager.GetWindowNum();i++)
		{
			CView *tempView = (CView *)theViewManager.arViews[i];
			if( tempView->IsKindOf(RUNTIME_CLASS(CGridView)) )
				((CGridView *)tempView)->SendMessage(UM_RESETGRIDTAB);
			else if(tempView->IsKindOf(RUNTIME_CLASS(CComView2)))
				((CComView2 *)tempView)->SendMessage(UM_RESETGRIDTAB);
		}
	}
}

#include "SimOptDlg.h"
//系统设置的属性页
void CMainFrame::OnOption()
{
	CSimOptDlg dlg;
	if(dlg.DoModal()==IDOK)
	{
		if(CurrentScheme != dlg.nCurSelScheme)
		{
			CurrentScheme = dlg.nCurSelScheme ;
			SystemColor = Scheme[CurrentScheme];
			GridColor = SystemColor.GridColor;
			VipColor = SystemColor.VipColor;
			TreeColor = SystemColor.TreeColor;
			TxtColor = SystemColor.TxtColor;
			MenuColor.MenuBackColor = VipColor.BACKCOLOR;
			MenuColor.MenuTitleColor = VipColor.KNCOLOR;
			MenuColor.MenuColor1 = VipColor.BIGTITLECOLOR;
			MenuColor.MenuColor2 = VipColor.LEVELCOLOR;
			MenuColor.MenuColor3 = VipColor.ZBCOLOR[2];
			MenuColor.MenuColor4 = VipColor.ZBCOLOR[3];
			g_d.ReCreateGdiObject();
			m_WarnManager.UpdateWarnGP();
			m_WarnManager.UpdateTJGP();
			for(int i = 0;i<theViewManager.GetWindowNum();i++)
			{
				CView *tempView = (CView *)theViewManager.arViews[i];
				if( tempView->IsKindOf(RUNTIME_CLASS(CGridView)) )
					((CGridView *)tempView)->SpecialReDraw(DRAW_ALL);
				else if(tempView->IsKindOf(RUNTIME_CLASS(CComView)) || tempView->IsKindOf(RUNTIME_CLASS(CComView2)))
					tempView->SendMessage(UM_REPAINTCOM_MESSAGE);
				else if(tempView->IsKindOf(RUNTIME_CLASS(CBigMenuView)) )
					((CBigMenuView *)tempView)->ChangeMenuColor();
				else if(tempView->IsKindOf(RUNTIME_CLASS(CJJView)) )
					((CJJView *)tempView)->SpecialReDraw(DRAW_ALL);
				else if( tempView->IsKindOf(RUNTIME_CLASS(CNewsView)) || tempView->IsKindOf(RUNTIME_CLASS(CTxtView)))
				{
					CloseView("Txt");
					CloseView("News");
				}
			}
			//
			CWnd *pTmpWnd = NULL;
			for(i=0;i < m_wndWorkspace.GetWndNum();i++)
			{
				pTmpWnd = m_wndWorkspace.GetWnd(i);
				if(!pTmpWnd) continue;
				if(pTmpWnd->IsKindOf(RUNTIME_CLASS(CZbTree))) 
					((CZbTree *)pTmpWnd)->SetAppearance(TreeColor.TreeBackColor,TreeColor.TreeForeColor);
				else if(pTmpWnd->IsKindOf(RUNTIME_CLASS(CGpTree))) 
					((CGpTree *)pTmpWnd)->SetAppearance(TreeColor.TreeBackColor,TreeColor.TreeForeColor);
				else if(pTmpWnd->IsKindOf(RUNTIME_CLASS(CGnTree))) 
					((CGnTree *)pTmpWnd)->SetAppearance(TreeColor.TreeBackColor,TreeColor.TreeForeColor);
				else if(pTmpWnd->IsKindOf(RUNTIME_CLASS(CWebTree))) 
					((CWebTree *)pTmpWnd)->SetAppearance(TreeColor.TreeBackColor,TreeColor.TreeForeColor);
			}			
			//如果当前有全景排行对话框，则重画之
			if(m_pZHSortDlg)	
				m_pZHSortDlg->Invalidate(FALSE);
			SaveScheme();
		}
		g_nMinnNum = dlg.m_nMinn;
		g_nDaynNum = dlg.m_nDayn;
	}
	return;

	CPropertySheet OptionSheet("");
	OptionSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CColorPage		ColorPage;
	COtherPage		OtherPage;
	COther2Page		Other2Page;
	COther3Page	    Other3Page;
	COther4Page	    Other4Page;
	CTabSetPage		TabSetPage;
	CShortCutPage	ShortCutPage;
	CExternPage		ExternPage;
	OptionSheet.AddPage (&ColorPage);
	OptionSheet.AddPage (&OtherPage);
	OptionSheet.AddPage (&Other2Page);
	OptionSheet.AddPage (&Other3Page);
	OptionSheet.AddPage (&Other4Page);
	OptionSheet.AddPage (&TabSetPage);
	OptionSheet.AddPage (&ShortCutPage);
	OptionSheet.AddPage (&ExternPage);
	OptionSheet.SetTitle (_F("系统设置"));
	switch(m_nFirstOptionPage)
	{
	case 5:
		OptionSheet.SetActivePage(&TabSetPage);break;
	}
	if(OptionSheet.DoModal () == IDOK)
	{		
		BOOL bLastBold = g_GH.bBoldZSTLine;
		BOOL bLastHQInfo = g_GH.bLeftHQInfo;
		g_GH.bLockHQSort = OtherPage.m_bLockHQSort;

		g_GH.bBoldZSTLine = Other4Page.m_bBoldZSTLine;
		g_GH.bLeftHQInfo = Other4Page.m_bLeftHQInfo;
		g_GH.bDiffVolColor = Other4Page.m_bDiffVolColor;
		g_GH.bEmptyZSTRight = Other4Page.m_bEmptyZstRight;
		g_GH.bZsShowAverage = Other4Page.m_bZsShowAverage;
		g_GH.bZsShowLX = Other4Page.m_bZsShowLX;
		g_GH.bDenseZSTAxis = Other4Page.m_bDenseZSTAxis;
		g_GH.bHq10Bottom = Other4Page.m_bHq10Bottom;
		g_GH.bShowBS=Other4Page.m_bShowBS;
		g_GH.bPinAssistPad = Other4Page.m_bPinAssistPad;
		g_GH.bShowTPPriceInZST = Other4Page.m_bShowTPPriceInZST;

		Refresh_OftenUseZb_Menu();
		Refresh_OftenUseZb_CmdTable();
		if( ColorPage.bModify || CurrentScheme != ColorPage.nCurSelScheme || bLastBold!=g_GH.bBoldZSTLine )
		{
			if(ColorPage.nCurSelScheme == 0)
				Scheme[0] = ColorPage.UI;
			CurrentScheme = ColorPage.nCurSelScheme ;
			SystemColor = Scheme[CurrentScheme];
			GridColor = SystemColor.GridColor;
			VipColor = SystemColor.VipColor;
			TreeColor = SystemColor.TreeColor;
			TxtColor = SystemColor.TxtColor;
			MenuColor.MenuBackColor = VipColor.BACKCOLOR;
			MenuColor.MenuTitleColor = VipColor.KNCOLOR;
			MenuColor.MenuColor1 = VipColor.BIGTITLECOLOR;
			MenuColor.MenuColor2 = VipColor.LEVELCOLOR;
			MenuColor.MenuColor3 = VipColor.ZBCOLOR[2];
			MenuColor.MenuColor4 = VipColor.ZBCOLOR[3];
			g_d.ReCreateGdiObject();
			m_WarnManager.UpdateWarnGP();
			m_WarnManager.UpdateTJGP();
			for(int i = 0;i<theViewManager.GetWindowNum();i++)
			{
				CView *tempView = (CView *)theViewManager.arViews[i];
				if( tempView->IsKindOf(RUNTIME_CLASS(CGridView)) )
					((CGridView *)tempView)->SpecialReDraw(DRAW_ALL);
				else if(tempView->IsKindOf(RUNTIME_CLASS(CComView)) || tempView->IsKindOf(RUNTIME_CLASS(CComView2)))
					tempView->SendMessage(UM_REPAINTCOM_MESSAGE);
				else if(tempView->IsKindOf(RUNTIME_CLASS(CBigMenuView)) )
					((CBigMenuView *)tempView)->ChangeMenuColor();
				else if(tempView->IsKindOf(RUNTIME_CLASS(CJJView)) )
					((CJJView *)tempView)->SpecialReDraw(DRAW_ALL);
				else if( tempView->IsKindOf(RUNTIME_CLASS(CNewsView)) || tempView->IsKindOf(RUNTIME_CLASS(CTxtView)))
				{
					CloseView("Txt");
					CloseView("News");
				}
			}
			//
			CWnd *pTmpWnd = NULL;
			for(i=0;i < m_wndWorkspace.GetWndNum();i++)
			{
				pTmpWnd = m_wndWorkspace.GetWnd(i);
				if(!pTmpWnd) continue;
				if(pTmpWnd->IsKindOf(RUNTIME_CLASS(CZbTree))) 
					((CZbTree *)pTmpWnd)->SetAppearance(TreeColor.TreeBackColor,TreeColor.TreeForeColor);
				else if(pTmpWnd->IsKindOf(RUNTIME_CLASS(CGpTree))) 
					((CGpTree *)pTmpWnd)->SetAppearance(TreeColor.TreeBackColor,TreeColor.TreeForeColor);
				else if(pTmpWnd->IsKindOf(RUNTIME_CLASS(CGnTree))) 
					((CGnTree *)pTmpWnd)->SetAppearance(TreeColor.TreeBackColor,TreeColor.TreeForeColor);
				else if(pTmpWnd->IsKindOf(RUNTIME_CLASS(CWebTree))) 
					((CWebTree *)pTmpWnd)->SetAppearance(TreeColor.TreeBackColor,TreeColor.TreeForeColor);
			}			
			//如果当前有全景排行对话框，则重画之
			if(m_pZHSortDlg)	
				m_pZHSortDlg->Invalidate(FALSE);
			SaveScheme();
		}
		//
		Set_OP_Scheme();	//改变风格
		//处理其它设置的内容
		if( AutoPageTime != OtherPage.m_AutoPageTime)
		{
			AutoPageTime = OtherPage.m_AutoPageTime;
			CView *tempView;
			for(int i = 0;i<theViewManager.GetWindowNum();i++)
			{
				tempView = (CView *)theViewManager.arViews[i];
				if( tempView->IsKindOf(RUNTIME_CLASS(CGridView)) )
				{
					(CGridView *)tempView->KillTimer(2);
					(CGridView *)tempView->SetTimer(2,AutoPageTime*1000,NULL);
				}
				if( tempView->IsKindOf(RUNTIME_CLASS(CComView)) )
				{
					//重新设置定时器
					(CComView *)tempView->KillTimer(0x2222);
					(CComView *)tempView->SetTimer(0x2222,AutoPageTime*1000,NULL);
				}
			}
		}
		g_bHasXHidden = OtherPage.m_bXaxis;
		g_bHasYHidden = OtherPage.m_bYaxis;
		g_GH.bOnlyDownAGData =  OtherPage.m_bOnlyDownAGData;
		g_GH.bHlight_HighLow = OtherPage.m_bHlight_HighLow;
		g_bLongHighLine = !OtherPage.m_bLongSquare;
		g_nNewMulNum = GetTotalMulNumFromIndex(OtherPage.m_nChartNum);
		g_nHligtNowVol = OtherPage.m_nHligtNowVol;
		g_nZFFzNum = OtherPage.m_nZFMin;
		g_nMinnNum = OtherPage.m_nMinn;
		g_nDaynNum = OtherPage.m_nDayn;
		g_bVolTQ = OtherPage.m_bVolTQ;
		g_bHisViewTQ = OtherPage.m_bHisViewTQ;
		g_bNotifyDownload = OtherPage.m_bNotifyDownload;
		g_bShowHqMine = OtherPage.m_bShowHqMine;
		g_GH.bCodeCanSort = OtherPage.m_bCodeCanSort;

		g_GH.nInitKNum = -1;
		if(Other4Page.m_bHasInitKNum)
			g_GH.nInitKNum = Other4Page.m_nInitKNum;
		g_GH.nInitKNum2 = -1;
		if(Other4Page.m_bHasInitKNum2)
			g_GH.nInitKNum2 = Other4Page.m_nInitKNum2;
		g_GH.bLocalData = Other4Page.m_bLocalData;
		g_GH.bShowKGap = Other4Page.m_bShowKGap;
		g_GH.bShowMmpChg = Other4Page.m_bShowMmpChg;
		g_GH.nShowJUNJ = Other4Page.m_nShowJunj;
		g_GH.bPillarBSChart = Other4Page.m_bPillarBSChart;
		g_GH.bBigHq = Other4Page.m_bBigHq;
		g_bDrawBlue = OtherPage.m_bDrawBlue;
		g_GH.NoXHCol = OtherPage.m_bNoXHCol;
		g_GH.NoVScroll = OtherPage.m_bNoVScroll;
		g_bFixedXAxis = OtherPage.m_bFixedXAxis;
		g_nFixedXAxis = OtherPage.m_nFixedXAxis;
		if(OtherPage.m_nToolBarPos != g_nToolBarPos || OtherPage.m_bAutoHideBar != g_bAutoHideBar)
		{
			if(OtherPage.m_bAutoHideBar != g_bAutoHideBar)
				TDX_MessageBox(m_hWnd,"工具栏自动隐藏需要重新进入系统才能生效!",MB_OK|MB_ICONINFORMATION);
			g_nToolBarPos = OtherPage.m_nToolBarPos;
			g_bAutoHideBar = OtherPage.m_bAutoHideBar;
			if(g_bAutoHideBar || g_nToolBarPos==2)
				m_wndToolBar.SetBarStyle(CBRS_ALIGN_TOP);
			else if(g_nToolBarPos==1)
				m_wndToolBar.SetBarStyle(CBRS_ALIGN_RIGHT);				
			else if(g_nToolBarPos==3)
				m_wndToolBar.SetBarStyle(CBRS_ALIGN_BOTTOM);
			else
				m_wndToolBar.SetBarStyle(CBRS_ALIGN_LEFT);			
			if(g_bAutoHideBar)
				m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
			else
				m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
			EnableDocking(CBRS_ALIGN_ANY,g_bAutoHideBar);
			DockControlBar(&m_wndToolBar);
			if(!g_bAutoHideBar)
				ShowControlBar(&m_wndToolBar,TRUE,FALSE);
		}
		if(g_bBossKey)
		{
			WORD tmpComKey=0;
			if(g_comkey&HOTKEYF_ALT)
				tmpComKey|=MOD_ALT;
			if(g_comkey&HOTKEYF_CONTROL)
				tmpComKey|=MOD_CONTROL;
			if(g_comkey&HOTKEYF_SHIFT)
				tmpComKey|=MOD_SHIFT;	
			RegisterHotKey(m_hWnd,1001,tmpComKey,g_virtkey);
		}
		else
			UnregisterHotKey(m_hWnd,1001); //关闭快速隐藏DLL
		//如果出现行情信息位置变化了,或指标有变化,则全部相应的窗口
		if(bLastHQInfo!=g_GH.bLeftHQInfo || Other2Page.m_bChangeZB&&g_bFxtShowTab)
		{
			CloseView("Com");
			if(!g_bFirstCusPad)
				CloseView("Com2");
		}
		FillGnTree();
	}
}

//交易设置的属性页
void CMainFrame::OnJyOption()
{
	if(TC_GetDlg_)
		TC_GetDlg_(DLG_CFG);
}

void CMainFrame::OnUpdateJyOption(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(g_OEM.bEmbedWTVersion);
}

void CMainFrame::ProcessNewUser(char *newuser)
{
	//将下次进入的用户名和进入方式设置好
	CString	Ini = g_WSXHStr+g_strYxhjCfg;
	WritePrivateProfileString("USER","UserName",newuser,Ini);
	Ini = g_WSXHStr+g_strUserCfg;
	WritePrivateProfileString("Other","LastLoginType",IntToStr(REGISTER_LOGIN),Ini);
	if(TDX_MessageBox(m_hWnd,"注册用户成功,\n是否重新进入系统,以新注册的用户名登录?",MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL)
		return;
	//关闭本系统,直接退出前先去掉前面的模态对话框
	g_bDirectQuit = TRUE;
	CancelModalWindows();
	PostMessage(WM_CLOSE);
	//重新执行此程序
	char path[256];
	sprintf(path,"%TopView.exe",HomePath);
	ExecExtern(path,m_hWnd,"nodetect");
}

void CMainFrame::OnWebUserCommand(UINT nID)
{
	SendMessage(UM_TREEVIEW_MESSAGE,WEB_TREETYPE,nID-IDC_WEBUSER_1001+1001);
}

void CMainFrame::OnTreeViewMessage(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case GN_TREETYPE:	//功能
		{
			WORD LoWord = LOWORD(lParam);
			WORD HiWord = HIWORD(lParam);
			OnCommCommand(HiWord,LoWord);
			break;
		}
	case GP_TREETYPE:	//股票
		{
			if(lParam == 10000) break; //如果是个母枝，则退出
			//一般情况下，向行情窗口发消息
			g_bOldGridDomain=FALSE;
			CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
			if(tempWnd)
				tempWnd->SendMessage(WM_TREE_MESSAGE,lParam);
			g_bOldGridDomain=TRUE;
			break;
		}
	case ZB_TREETYPE:	//指标
		{
			if(lParam == 65535) break; //如果母枝没有子枝，则退出
			WORD LoWord = LOWORD(lParam);
			WORD HiWord = HIWORD(lParam);
			int MessageNo;
			if(HiWord == 1 || HiWord == 2 || HiWord == 3)
			{
				int type=0,index=0;	//0:删除指示 1:交易系统指示 2:五彩K线 3:一般指标 4:特特画线指标
				if(HiWord == 1) type = 1;
				if(HiWord == 2) type = 2;
				index = LoWord;
				if(theViewManager.pActiveView ->IsKindOf(RUNTIME_CLASS(CComView)) ||
					theViewManager.pActiveView ->IsKindOf(RUNTIME_CLASS(CComView2)) )
					theViewManager.pActiveView->SendMessage(WM_TREEZS_MESSAGE,type,index);
			}
			else
			{
				if(HiWord == 0)
					MessageNo = LoWord + KEYGUY_ZB;		//指标
				else
					MessageNo = LoWord + KEYGUY_SEPCZB; //特殊指标
				OpenComView(1,NULL,0,MessageNo);
			}
			break;
		}
	case ZX_TREETYPE:
		{
			OnOPSYS(ZX_TREETYPE);
			theViewManager.pActiveView->SendMessage(UM_CJZX_MESSAGE,lParam,0);
		}
		break;
	case ZX2_TREETYPE:
		{
			OnOPSYS(ZX2_TREETYPE);
			theViewManager.pActiveView->SendMessage(UM_CJZX_MESSAGE,lParam,0);
		}
		break;
	case ZX3_TREETYPE:
		{
			OnOPSYS(ZX3_TREETYPE);
			theViewManager.pActiveView->SendMessage(UM_CJZX_MESSAGE,lParam,0);
		}
		break;
	case JB_TREETYPE:
		{
			OnOPSYS(JB_TREETYPE);
			theViewManager.pActiveView->SendMessage(UM_JBFX_MESSAGE,lParam,0);
		}
		break;
	case WEB_TREETYPE:
		{
			if(lParam>=1001)
			{
				switch(lParam)
				{
				case 1001:
					{
						char tmpPath[MAX_PATH];
						sprintf(tmpPath,"%sTQClient.exe",HomePath);
						if(g_bTQAsBroker)
							ExecExtern(tmpPath,m_hWnd,"broker");
						else
							ExecExtern(tmpPath,m_hWnd,"custom");
					}
					break;
				case 1002:
					if(g_LoginInfo.bDialogRegister)
					{
						CU_ChangePwd dlg;
						dlg.m_bDelMySelf=FALSE;
						dlg.DoModal();
					}
					break;
				case 1003:
					if(g_LoginInfo.bDialogRegister && g_LoginInfo.bCanDelMySelf)
					{
						CU_ChangePwd dlg;
						dlg.m_bDelMySelf=TRUE;
						dlg.DoModal();
					}
					break;
				}
				break;
			}
			if(lParam>=0)
				OnOPSYS(WEB_TREETYPE);
			else
				OnOPSYS(WEB_TREETYPE,0);
			theViewManager.pActiveView->SendMessage(WM_WEBINFO_MESSAGE);
		}
		break;
	}
}

void CMainFrame::OnBigFontGrid()
{
	g_bBigFontGrid=!g_bBigFontGrid;
	g_d.ReCreateGdiObject();
	OnShowhq();
	if( theViewManager.pActiveView && theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CGridView)) )
		((CGridView *)theViewManager.pActiveView)->SpecialReDraw(DRAW_ALL);
}

void CMainFrame::OnUpdateBigFontGrid(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(g_bBigFontGrid);
}

void CMainFrame::OnLockRight()
{
	g_bLockRight=!g_bLockRight;
}

void CMainFrame::OnUpdateLockRight(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(g_bLockRight);
}

void CMainFrame::OnDpFx(UINT nID)
{
	OnCommCommand(CMD_COM,nID-ID_101+101);
}

void CMainFrame::OnOtherFx(UINT nID)
{
	if(nID == ID_300)
		OnCommCommand(CMD_COM,300);
	else
		OnCommCommand(CMD_COM,nID-ID_501+501);
}

void CMainFrame::OnChangeReport(UINT nID)
{
	OnCommCommand(CMD_HIS,400+nID-ID_RMREPORT);
}

void CMainFrame::OnTdxJBZL()
{
	OnCommCommand(CMD_JBM,802);
}

void CMainFrame::OnBJFX(UINT nID)
{
	g_bOldGridDomain=FALSE;
	CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
	if(tempWnd)
		tempWnd->SendMessage(WM_COMMAND,nID);
	g_bOldGridDomain=TRUE;
}

void CMainFrame::OnMenuBlockClick(UINT nID)
{
	g_bOldGridDomain=FALSE;
	CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
	if(tempWnd)
		tempWnd->SendMessage(WM_COMMAND,nID);
	g_bOldGridDomain=TRUE;
}

void CMainFrame::OnFyReport(UINT nID)
{
	g_bOldGridDomain=FALSE;
	CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
	if(tempWnd)
		tempWnd->SendMessage(WM_COMMAND,nID);
	g_bOldGridDomain=TRUE;
}

void CMainFrame::OnRank(UINT nID)
{
	g_bOldGridDomain=FALSE;
	CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
	if(tempWnd)
		tempWnd->SendMessage(WM_COMMAND,nID);
	g_bOldGridDomain=TRUE;
}

void CMainFrame::OnMulGpType(UINT nID)
{
	g_nMul_WhichGPType = nID-ID_MULGP_TYPE0;
	g_nMul_nBlockFlag = -1;
	OnCommCommand(CMD_COM,900);
}

void CMainFrame::OnETF()
{	
	CNewMenu ETFMenu;
	ETFMenu.CreatePopupMenu();
	for(int i=0;i<g_nETFInfoNum;i++)
		ETFMenu.AppendMenu(MF_STRING, ID_ETFSTART+i,_F(g_ETFInfo[i].Desc));
	CPoint pt;
	GetCursorPos(&pt);
	ETFMenu.TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,AfxGetMainWnd());
}


void CMainFrame::OnETFCommand(UINT nID)
{
	int iNo=nID-ID_ETFSTART;
	if(iNo<0 || iNo>=g_nETFInfoNum) return;
	MemStkInfo *tmpInfo=g_pStockIO->Get_MemStkInfo(g_ETFInfo[iNo].Code,g_ETFInfo[iNo].setcode);
	if(tmpInfo)	ToZst(g_ETFInfo[iNo].Code,g_ETFInfo[iNo].setcode,TRUE);	
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_SYSCOMMAND && pMsg->wParam == SC_SCREENSAVE)
		return TRUE;//禁止屏幕保护程序的运行
	if(pMsg->message == WM_LBUTTONDOWN||pMsg->message == WM_NCLBUTTONDOWN)
	{
		CPoint tempPoint(LOWORD(pMsg->lParam),HIWORD(pMsg->lParam));
		if(tempPoint.x<2&&tempPoint.y>45)
			SendMessage(WM_COMMAND, CG_ID_VIEW_WORKSPACE);
	}
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_ESCAPE && !g_pHisDlg && theViewManager.pActiveView)
		{
			if(m_pZHSortDlg)
			{
				SendMessage (WM_CANCELPRESSED,(WPARAM)m_pZHSortDlg,0);
				return TRUE;
			}
			CWnd *pWnd = CWnd::GetFocus(); //当焦点在View上时，才能使用VK_ESCAPE键
			if(pWnd && ((pWnd==theViewManager.pActiveView) || theViewManager.pActiveView->IsChild(pWnd)) )
			{
				if( !theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CBigMenuView)) && !theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CHomePageView)) )
				{
					//如果当前窗口为ComView，则先让它处理消息
					if( theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)) )
					{
						CComView *tempView = (CComView *)theViewManager.pActiveView;
						if(tempView->EscHandle() == 1)
							return TRUE;
					}
					//如果当前窗口为ComView，则先让它处理消息
					if( theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)) )
					{
						CComView2 *tempView = (CComView2 *)theViewManager.pActiveView;
						int nDefNo=GetCusPadTypeParam(g_DefCusPadName);
						if(nDefNo>=0)
						{
							tempView->SendMessage(UM_CONSTRUCTPAD_MESSAGE, nDefNo);
							if(!g_bHasCaption)
								return TRUE;
						}
						if(tempView->EscHandle() == 1)
							return TRUE;
					}
					//TxtView较特殊一些，相当于对话框
					if( theViewManager.pOldActiveView && (theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CTxtView)) || theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CIEBrowserView))) )
					{
						if(theViewManager.pOldActiveView != theViewManager.pActiveView)
						{
							CFrameWnd *tempFrameWnd = (CFrameWnd *)(theViewManager.pOldActiveView)->GetParent();
							tempFrameWnd->ActivateFrame();
							return TRUE;
						}
					}
					//如果是由高级行情,基本分析和经纪人视图进入分析图的,则直接返回
					if( theViewManager.pOldActiveView && (theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)) ||
						theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)) ))
					{
						if(theViewManager.pOldActiveView->IsKindOf(RUNTIME_CLASS(CAdvHqView)) || 
						   theViewManager.pOldActiveView->IsKindOf(RUNTIME_CLASS(CJbfxView)) )
						{
							CFrameWnd *tempFrameWnd = (CFrameWnd *)(theViewManager.pOldActiveView)->GetParent();
							tempFrameWnd->ActivateFrame();
							return TRUE;
						}
					}
					if(theViewManager.pOldActiveView2 && theViewManager.pOldActiveView2 != theViewManager.pActiveView)
					{
						CFrameWnd *tempFrameWnd = (CFrameWnd *)(theViewManager.pOldActiveView2)->GetParent ();
						tempFrameWnd->ActivateFrame();
						return TRUE;
					}
					SwitchToFirstView();
				}
			}
		}
		short CtrlDown,ShiftDown;
		switch(pMsg->wParam)  //截获系统键F2--F10
		{
		case VK_TAB:
			{
				CtrlDown=::GetKeyState(VK_CONTROL);
				if(FALSE&&CtrlDown<0)
				{
					if(!m_pTabViewMenu)
					{
						m_pTabViewMenu = new CNewMenu;
						m_pTabViewMenu->CreatePopupMenu();
//						m_pTabViewMenu->SetIconSize(24,24);
						
						CImageList padIconList;
						padIconList.Create(16, 16, ILC_COLORDDB|ILC_MASK, 20, 1);
						
						HICON bitmap;
						GetResPadBit(theViewManager.pActiveView, bitmap);
						padIconList.Add(bitmap);
						for(int i=0;i<theViewManager.GetWindowNum();i++)
						{
							if((CView *)theViewManager.arViews[i]==theViewManager.pActiveView) continue;
							HICON bitmap1;
							GetResPadBit((CView*)theViewManager.arViews[i], bitmap1);
							padIconList.Add(bitmap1);
						}

						int xh = 0;
						CString padTitle="";
						theViewManager.pActiveView->GetParent()->GetWindowText(padTitle);
					//	m_pTabViewMenu->AppendMenu(MF_STRING, ID_PADSWITCH1+xh, padTitle, &padIconList, xh);
						m_pTabViewMenu->AppendMenu(MF_STRING, ID_PADSWITCH1+xh, padTitle);
						xh++;
						for(i=0;i<theViewManager.GetWindowNum();i++)
						{
							if((CView *)theViewManager.arViews[i]==theViewManager.pActiveView) continue;
							CString padTitle1="";
							((CView*)theViewManager.arViews[i])->GetParent()->GetWindowText(padTitle1);
						//	m_pTabViewMenu->AppendMenu(MF_STRING, ID_PADSWITCH1+xh, padTitle1, &padIconList, xh);
							m_pTabViewMenu->AppendMenu(MF_STRING, ID_PADSWITCH1+xh, padTitle1);
							xh++;
						}

						keybd_event(VK_DOWN,0,KEYEVENTF_EXTENDEDKEY,0); 
						keybd_event(VK_DOWN,0,KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP,0);
						keybd_event(VK_DOWN,0,KEYEVENTF_EXTENDEDKEY,0); 
						keybd_event(VK_DOWN,0,KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP,0);

						RegisterHotKey(m_hWnd, 1002, MOD_CONTROL, VK_TAB);
						
						CRect rc;
						GetWindowRect(&rc);
						m_pTabViewMenu->TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,(rc.left+rc.right)/2-62,(rc.top+rc.bottom)/2-62,this);

						m_bPadMenuShow = TRUE;
						UnregisterHotKey(m_hWnd, 1002);
						TDEL(m_pTabViewMenu);
						
						return 0;
					}
				}
			}
			break;
		case VK_F1:
		//	OnCommCommand(CMD_COM,502);
			return TRUE;
		case VK_F2:
		//	OnCommCommand(CMD_COM,503);
			return TRUE;
		case VK_F3:
			if(HQControl::IsValidProgram2())
			{
				CtrlDown=::GetKeyState(VK_CONTROL);
				if(CtrlDown >= 0)	//避免截获Ctrl+F3,Ctrl+F3用于显示走势图
				{
					OnCommCommand(CMD_COM,102);
					return TRUE;
				}
			}
			break;
		case VK_F4:
			if(HQControl::IsValidProgram2())
			{
				CtrlDown=::GetKeyState(VK_CONTROL);
				if(CtrlDown >= 0)	//避免截获Ctrl+F4系统键
				{
					OnCommCommand(CMD_COM,108);
					return TRUE;
				}
			}
			break;
		case VK_F5:
			if(HQControl::IsValidProgram2())
				OnF5();
			return TRUE;
		case VK_F6:
			if(HQControl::IsValidProgram2())
			{
				CtrlDown=::GetKeyState(VK_CONTROL);
				if(CtrlDown >= 0)	//避免截获Ctrl+F6系统键
				{
					ShiftDown=::GetKeyState(VK_SHIFT);
					if(ShiftDown >= 0)
					{
						OnCommCommand(CMD_HQ,TYPE_ZXG+1);
						return TRUE;
					}
				}
			}
			break;
		case VK_F9:
			OnF9();
			return TRUE;
		default:
			break;
		}
	}
	return CNewMDIFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnZxg()
{
	OnCommCommand(CMD_HQ,TYPE_ZXG+1);
}

void CMainFrame::On6787()
{
	OnCommCommand(CMD_HQ,67);
	OnCommCommand(CMD_MAIN,87);
}

void CMainFrame::OnF5()
{
	if(!theViewManager.pActiveView) return;
	if( theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)) )
	{
		CComView2 *pView = (CComView2 *)theViewManager.pActiveView;
		pView->OnF5();
	}
	else if( theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)) )
	{
		CComView *pView = (CComView *)theViewManager.pActiveView;
		UINT tmpPad = pView->GetNowPad();
		UINT tmpMainPad = tmpPad & 0xFF0000;
		if(tmpMainPad == ZST_PAD || tmpMainPad == MULZST_PAD)
			OnCommCommand(CMD_COM,300);
		else
			OnCommCommand(CMD_COM,(g_bTickTStatus?508:501));
	}
	else
		OnCommCommand(CMD_COM,(g_bTickTStatus?508:501));
}

void CMainFrame::OnCorpWeb()
{
	ShellExecute(NULL, _T("open"), (CString)"http://"+g_Url.CorpWeb, NULL, NULL, SW_SHOW);
}

void CMainFrame::OnWebPageMenu(UINT nID)
{
	OpenCusWebPage(this,g_strWebPageURL[nID-ID_WEB_MENU_1],g_OEM.bMainWebPageInsideOpen);
}

void CMainFrame::OnWebPageMenu2(UINT nID)
{
	OpenCusWebPage(this,g_strWebPageURL2[nID-ID_WEB2_MENU_1],g_OEM.bMainWebPageInsideOpen);
}

//在系统动画区按鼠标时弹出菜单的处理
void CMainFrame::OnRightClickLeft(UINT nID)
{
	switch(nID)
	{
	case ID_RIGHT_HELP:
		{
			OnHelpFinder();	
			break;
		}
	case ID_RIGHT_EMAIL:
		{
			if(strlen(g_Url.EmailURL)>0)
				ShellExecute(NULL,_T("open"),g_Url.EmailURL,NULL,NULL, SW_SHOW);
			break;
		}
	case ID_RIGHT_HELPWEB:
		{
			if(strlen(g_Url.HelpWebURL)>0)
				ShellExecute(NULL,_T("open"),g_Url.HelpWebURL,NULL,NULL, SW_SHOW);
			break;
		}
	}
}

BOOL GetTrayWndShow()
{
	HWND hWnd=FindWindow("shell_TrayWnd",NULL);
	if(!hWnd || !::IsWindow(hWnd))
		return FALSE;
	CRect rc;
	GetWindowRect(hWnd,&rc);
	return (GetSystemMetrics(SM_CYSCREEN)-rc.top)>5;
}

RECT GetSysMaxWorkRect()
{
	RECT rcTW,rcMax;
	::SystemParametersInfo(SPI_GETWORKAREA,0,&rcMax,0);
	::GetWindowRect(::FindWindow("Shell_TrayWnd",NULL),&rcTW);	
	
	int nMaxX = GetSystemMetrics(SM_CXSCREEN);   
	int nMaxY = GetSystemMetrics(SM_CYSCREEN);
	if (rcTW.left<=0 && rcTW.right>=0 && rcTW.top<=0 && rcTW.bottom>=nMaxY)//左侧
	{
		if (abs(rcTW.right)<5)
			rcMax.left += 3;
	}
	else if (rcTW.left<=nMaxX && rcTW.right>=nMaxX && rcTW.top<=0 && rcTW.bottom>=nMaxY)//右侧
	{
		if (abs(rcTW.left-nMaxX)<5)
			rcMax.right -= 3;
	}
	else if (rcTW.top<=0 && rcTW.bottom>=0 && rcTW.left<=0 && rcTW.right>=nMaxX)//上侧
	{
		if (abs(rcTW.bottom)<5)
			rcMax.top += 3;
	}
	else if (rcTW.top<=nMaxY && rcTW.bottom>=nMaxY && rcTW.left<=0 && rcTW.right>=nMaxX)//下侧
	{
		if (abs(rcTW.top-nMaxY)<5)
			rcMax.bottom -= 3;
	}
	return rcMax;
}

//重载此函数，得到主框架窗口的最大最小信息，用于全屏显示中
void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	//第一次要去掉标题栏
	static BOOL s_bModifyStyle=FALSE;
	if(!s_bModifyStyle)
	{
		if(!g_bHasCaption)
			ModifyStyle(WS_CAPTION,NULL,0);
#if defined(OEM_STAR_HM)
		ModifyStyle(WS_CAPTION,NULL,0);
#endif
		s_bModifyStyle = TRUE;
	}
	//
	if (m_pWndFullScreen != NULL)
	{			
		lpMMI->ptMaxTrackSize.y = m_FullScreenPlace.Height();
		lpMMI->ptMaxTrackSize.x = m_FullScreenPlace.Width();
	}
	else
	{
		//由于去掉了WS_CAPTION属性,MFC最大化逻辑会出现问题
		//首先判断是否在另一个虚拟的屏，如果是的话，不进行处理
		//否则进行处理，注意判断了
		CRect MainRect;
		GetWindowRect(MainRect);
		if(MainRect.right<GetSystemMetrics(SM_CXSCREEN)+100 && MainRect.bottom<GetSystemMetrics(SM_CYSCREEN)+100)
		{
			RECT rc = GetSysMaxWorkRect();
			lpMMI->ptMaxPosition.x	= rc.left-3; 
			lpMMI->ptMaxPosition.y	= rc.top-3; 
			lpMMI->ptMaxSize.x		= rc.right-rc.left+6; 
			lpMMI->ptMaxSize.y		= rc.bottom-rc.top+4; 
		}
		lpMMI->ptMaxSize.x+=1;
		lpMMI->ptMinTrackSize.x=640;
		lpMMI->ptMinTrackSize.y=426;
		CNewMDIFrameWnd::OnGetMinMaxInfo(lpMMI);
	}
}


void CMainFrame::EscapeFullScreen()
{
	if (m_pWndFullScreen != NULL)
		OnFullscreen();
}

BOOL CMainFrame::GetFullScreenSize(CWnd* pWnd, POINT& pt) const
{
	//应该对任何窗口都实用,不应限制只对激活窗口
	if (m_pWndFullScreen != NULL && !m_FullScreenPlace.IsRectNull())
	{
		pt.x = m_FullScreenPlace.Width();
		pt.y = m_FullScreenPlace.Height();
		return TRUE;
	}
	return FALSE;
}

void CMainFrame::OnFullscreen() 
{
#if defined(OEM_NEWJY)
	return;
#endif
	if (m_pWndFullScreen != NULL)	//消去全屏状态
	{
		ShowControlBar(&m_wndWorkspace,m_bHadDialogBar,FALSE);
		ShowControlBar(&m_wndToolBar, m_bHadToolBar, FALSE);
		ShowControlBar(&m_wndStatusBar, m_bHadStatusBar, FALSE);
		
		m_pWndFullScreen = NULL;  //要在ShowWindow之前设为0,因为OnGetMinMaxInfo中会处理

		LockWindowUpdate();
		SetWindowPlacement(&m_PreviousPlace);
		UnlockWindowUpdate();
	}
	else	//全屏显示
	{
		m_FullScreenPlace.SetRect(0,0,0,0);

		if(g_GH.bFullScreenNotify)
		{
			CHintDlg dlg;
			dlg.m_bAlwaysHint = g_GH.bFullScreenNotify;
			dlg.m_strHint = "切换成全屏模式后,按Ctrl+P键恢复到正常模式";
			dlg.m_strCheck = _F("总是进行提示");
			dlg.DoModal();
			g_GH.bFullScreenNotify = dlg.m_bAlwaysHint;
		}
		CloseallAndKeepSpec(); //关闭所有窗口,才有全屏功能

		m_pWndFullScreen = MDIGetActive();	//得到激活的窗口
		ASSERT(m_pWndFullScreen != NULL);

		GetWindowPlacement(&m_PreviousPlace);

		m_bHadDialogBar = m_wndWorkspace.IsWindowVisible();
		m_bHadToolBar = m_wndToolBar.IsWindowVisible();
		m_bHadStatusBar = m_wndStatusBar.IsWindowVisible();

		ShowControlBar(&m_wndWorkspace,FALSE,FALSE);
		ShowControlBar(&m_wndToolBar, FALSE, FALSE);
		ShowControlBar(&m_wndStatusBar,FALSE, FALSE);

		WINDOWPLACEMENT m_tmpPreviousPlace;
		m_pWndFullScreen->GetWindowPlacement(&m_tmpPreviousPlace);
		GetWindowPlacement(&m_MainPreviousPlace);

		CRect rectDesktop;
		//得到桌面大小
		if (!::SystemParametersInfo(SPI_GETWORKAREA, 0, &rectDesktop, 0))
	 		::GetWindowRect(::GetDesktopWindow(), &rectDesktop);
		else
		{
			rectDesktop.right = ::GetSystemMetrics(SM_CXSCREEN);
			rectDesktop.bottom = ::GetSystemMetrics(SM_CYSCREEN);
			rectDesktop.right -= rectDesktop.left;
			rectDesktop.bottom -= rectDesktop.top;
			rectDesktop.left = -rectDesktop.left;
			rectDesktop.top = -rectDesktop.top;
		}

		CRect rectDesired = rectDesktop;
		::AdjustWindowRect(&rectDesktop, GetStyle(), TRUE);

		rectDesktop.bottom += ::GetSystemMetrics(SM_CYBORDER);
		m_FullScreenPlace = rectDesktop;

		WINDOWPLACEMENT wpNew = m_tmpPreviousPlace;
		wpNew.showCmd =  SW_SHOWNORMAL;
		wpNew.rcNormalPosition = rectDesktop;

		LockWindowUpdate();
		SetWindowPlacement(&wpNew);
		UnlockWindowUpdate();

		wpNew.rcNormalPosition = rectDesired;
		::AdjustWindowRect(&(wpNew.rcNormalPosition), m_pWndFullScreen->GetStyle(), FALSE);

		m_pWndFullScreen->LockWindowUpdate();
		m_pWndFullScreen->SetWindowPlacement(&wpNew);
		m_pWndFullScreen->UnlockWindowUpdate();
	}
}

void CMainFrame::OnUpdateFullscreen(CCmdUI* pCmdUI) 
{
	CNewMDIChildWnd* pWnd = (CNewMDIChildWnd*)MDIGetActive();
	pCmdUI->Enable(pWnd != NULL);
	pCmdUI->SetCheck(m_pWndFullScreen != NULL);
}

void CMainFrame::OnTopMost()
{
	m_bTopMost=!m_bTopMost;
	if(m_bTopMost)
		SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	else
		SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
}

void CMainFrame::OnUpdateTopMost(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bTopMost);
}

void CMainFrame::OnCoolinfo() 
{
	//如果没有备忘录文件，则创建之。
	CFile theFile;
	if( theFile.Open(g_WSXHStr+"Coolinfo.Txt",CFile::modeCreate|CFile::modeNoTruncate) )
		theFile.Close();
	::ShellExecute(NULL,NULL,_T(g_WSXHStr+"Coolinfo.txt"),NULL,NULL,SW_SHOW); 
}

void CMainFrame::OnNews()
{
	SwitchToView("News");
}

void CMainFrame::OnEscape()
{
	PostMessage(WM_KEYDOWN,VK_ESCAPE);
}

void CMainFrame::OnAllWarngp() 
{
	m_WarnManager.OnAllWarngp();
}

void CMainFrame::OnTjWarngp() 
{
	m_WarnManager.OnTjWarngp();
}

void CMainFrame::OnDeepWarn()
{
	if(!g_bG2DT)
		return;
	if(m_pDeepWarnDlg != NULL) 
	{
		if (m_pDeepWarnDlg->IsIconic()) 
			m_pDeepWarnDlg->ShowWindow(SW_RESTORE); 
		return;
	}
	m_pDeepWarnDlg = new CDeepWarnDlg(this);
	m_pDeepWarnDlg->Create(IDD_DEEPWARN_DIALOG);
	m_pDeepWarnDlg->ShowWindow (SW_SHOW);
}

void CMainFrame::OnUpdateDeepWarn(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(g_bG2DT);
}

BOOL CMainFrame::OnCommCommand(int WindowType,int MessageNo)
{
	switch(WindowType)
	{
	case CMD_MAIN:		//如果是主窗口
		{
			OnMainWndMessage(MessageNo);
			break;
		}
	case CMD_HQ:		//如果窗口类型为行情
		{
			BOOL bToGrid=TRUE;
			if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))
			{
				CComView2 *tempWnd = (CComView2 *)theViewManager.pActiveView;
				if(tempWnd->HasNoFixedHQGrid())
				{
					tempWnd->SendMessage(WM_KEYGUY_MESSAGE,MessageNo);
					bToGrid=FALSE;
				}
			}
			if(bToGrid)
			{
				g_bOldGridDomain=FALSE;
				CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
				if(tempWnd)
					tempWnd->SendMessage(WM_KEYGUY_MESSAGE,MessageNo);
				g_bOldGridDomain=TRUE;
			}
			return TRUE;
		}
	case CMD_COM:		//如果窗口类型为组合图
		{
			if(MessageNo==350)
			{
				OnShowvip();
				return TRUE;
			}
			if(MessageNo==351)
			{
				if( theViewManager.pActiveView && theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)) )
				{	//如果当前是日线图
					CComView *tempWnd = (CComView *)theViewManager.pActiveView;
					UINT tmpPadType=tempWnd->GetNowPad();
					UINT tmpMainPad=tmpPadType & 0xFF0000;
					if(tmpMainPad==FXT_PAD)
					{
						tempWnd->SendMessage(WM_COMMAND, ID_EXPSYSSHOW);
						return TRUE;
					}
				}
			}
			if(MessageNo>=150&&MessageNo<200)		//自选股
			{
				short setcode;
				char  gpcode[SH_CODE_LEN+1];
				int k = MessageNo - 150;
				setcode = (g_strMovingZXG[k][0]=='0')?0:1;
				memcpy(gpcode,g_strMovingZXG[k]+1,SH_CODE_LEN);
				gpcode[SH_CODE_LEN] = 0;
				MemStkInfo *pStkInfo = g_pStockIO->Get_MemStkInfo(gpcode,setcode);
				if(!pStkInfo) return TRUE;
				ToZst(gpcode, setcode);
			}
			if(MessageNo==502)
			{
				if( theViewManager.pActiveView && theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)) )
				{	//如果当前是日线图,则进入日线报表
					CComView *tempWnd = (CComView *)theViewManager.pActiveView;
					UINT tmpPadType=tempWnd->GetNowPad();
					UINT tmpMainPad=tmpPadType & 0xFF0000;
					if(tmpMainPad==FXT_PAD)
						g_bUseKList=TRUE;
				}
			}
			MemStkInfo *tmpInfo=NULL;
			int tempCurGPNo = GetSYSCurrentGPIndex();
			if(tempCurGPNo != -1) tmpInfo = (*g_pStockIO)[tempCurGPNo];
			if(!tmpInfo || (theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView))))
				OpenComView(1,NULL,0,MessageNo);
			else if(MessageNo == 8 || (MessageNo >= 801 && MessageNo <= 812))
				OpenComView(1,NULL,0,MessageNo);
			else if(MessageNo!=16&&MessageNo!=17&&MessageNo!=30)
				OpenComView(2,tmpInfo->Code,tmpInfo->setcode,MessageNo);
			break;
		}
	case CMD_JBM:			//如果是基本资料视图
		{
			if(MessageNo == 801 || MessageNo == 802)
			{
				int tempCurrentGPNo = GetSYSCurrentGPIndex();
				if(tempCurrentGPNo >= 0)
				{
					//如果是实时港股，则打开财华(或其它)相应的股票和权证网页
					MemStkInfo * pInfo = (*g_pStockIO)[tempCurrentGPNo];
					if(pInfo)
					{
						if(pInfo->bigmarket==HKGP_TYPE && g_GC.bIsSSDS)
						{
							CString tmpInfoURL=HK_GP_F10_WEBPAGE;
							if(testhkwarrant(pInfo))
								tmpInfoURL=HK_QZ_F10_WEBPAGE;
							OpenCusWebPage(this,tmpInfoURL,FALSE);
							break;
						}
						else
						{
							//!!!
						//	CString tmpInfoURL=HEXUN_WEBPAGE;
						//	if(!testzs(pInfo))
						//		OpenCusWebPage(this,tmpInfoURL,TRUE);
						//	break;
							TxtCurGPCode = pInfo->Code;
							TxtCurSetCode = pInfo->setcode;
						}
					}
				}
				else
				{
					TxtCurGPCode = ComCurGPCode;
					TxtCurSetCode = ComCurSetCode;
				}
				//!!!
			//	break;
				//得到集合
				short nPageGPIndex[MAXGPNUM] = {-1};
				short nTotalGP = GetSYSCurrentGPSet(nPageGPIndex);
				LinkedSetInfo *pLSetInfo = NULL;
				GetSYSCurrentLSetInfo(pLSetInfo);
				if(MessageNo == 802)	
					g_CurrentJBM = JBM_TDX;
				else if(g_CurrentJBM==JBM_TDX)	
					g_CurrentJBM = 0;
				CTxtView *tempWnd = (CTxtView *)SwitchToView("Txt");
				if(tempWnd)
				{
					if(nTotalGP>0&&pLSetInfo)
					{
						tempWnd->SetPageGP(nPageGPIndex, nTotalGP, tempCurrentGPNo, pLSetInfo);
					}
					else theViewManager.pActiveView->SendMessage(WM_RICHEDIT_MESSAGE);
				}
			}
			break;
		}
	case CMD_NEWS:	//如果窗口类型为公告消息
		{
			CNewsView *tempWnd = (CNewsView *)SwitchToView("News");
			if(tempWnd)
				tempWnd->SendMessage(WM_KEYGUY_MESSAGE,MessageNo);
			return TRUE;
		}
	case CMD_HIS:	//如果窗口类型为历史分析
		{
			//如果找到则转到它，否则生成一个
			CView *tmpView=FindViewThenActive("His");
			if(tmpView)
				theViewManager.pActiveView->SendMessage(WM_KEYGUY_MESSAGE,MessageNo);
			else
			{
				if(MessageNo > 400)
				{
					if(!CanProcesLocalData(this, 0))
						break;
				}
				HisCurType = MessageNo - 400; //改写HisCurType，用来传递首先打开时的CurType
				if(HisCurType < 0 && HisCurType > 6) HisCurType = 0;
				if(HisCurType > 2) //如果是区间统计排列，请需要知道是那种排行
				{
					g_nCurQJType = HisCurType-3;
					HisCurType = 3;
				}
				OpenNewView("His");
				break;
			}
			return TRUE;
		}
	case CMD_ADVHQ:
		{
			if(IsBig5Version()) return TRUE;
			RefreshAllHq();		
			CAdvHqView *tempWnd = (CAdvHqView *)SwitchToView("AdvHq");
			if(tempWnd)
				tempWnd->SendMessage(WM_KEYGUY_MESSAGE,MessageNo,0);
			return TRUE;
		}
	case CMD_JBFX:
		return TRUE;
	case CMD_JJ:
		{
			CView *tmpView=FindViewThenActive("Jj");
			if(tmpView)
				theViewManager.pActiveView->SendMessage(WM_KEYGUY_MESSAGE,MessageNo);
			else
				OpenNewView("Jj");
			return TRUE;
		}
	}
	return FALSE;
}

void CMainFrame::OnZXCommand(UINT nID)
{
	OnOPSYS(ZX_TREETYPE,0);
	switch(nID)
	{
		case ID_ZX_FAVOR:
			theViewManager.pActiveView->SendMessage(UM_CJZX_MESSAGE,973,0);
			break;
		case ID_ZX_CFG:
			theViewManager.pActiveView->SendMessage(UM_CJZX_MESSAGE,974,0);
			break;
		case ID_ZX_ADD:
			theViewManager.pActiveView->SendMessage(UM_CJZX_MESSAGE,972,0);
			break;
		case ID_ZX_SEARCH:
			theViewManager.pActiveView->SendMessage(UM_CJZX_MESSAGE,971,0);
			break;
		case ID_ZX_RECENTSEARCH:
			theViewManager.pActiveView->SendMessage(UM_CJZX_MESSAGE,989,0);
			break;
		case ID_ZX_QS:
			theViewManager.pActiveView->SendMessage(UM_CJZX_MESSAGE,100,0);
			break;
		case ID_ZX_QS_SORT:
			theViewManager.pActiveView->SendMessage(UM_CJZX_MESSAGE,200,0);
			break;
	}
}

void CMainFrame::OnCustomPad(UINT nID)
{
	ConstructCustomPad(nID-IDC_CUSTOMPAD1);
}

void CMainFrame::OnRecentPad()
{
	ShowControlBar(&m_wndWorkspace,FALSE,FALSE);
	if(TC_BeforeHideWT_) TC_BeforeHideWT_();
	ShowControlBar(&m_wndRestspace,FALSE,FALSE);
	CView *tmpView=FindViewThenActive("Com2");
	if(!tmpView)
		PostMessage(WM_COMMAND,IDC_CUSTOMPAD1);
}

void CMainFrame::OnQSCusWebPage()
{
	OpenCusWebPage(this,g_NewJyCfg.QSRelativeInfo,FALSE);
}

void CMainFrame::OnCusWebPage(UINT nID)
{
	OpenCusWebPage(this,g_CusWebPage[nID-IDC_CUSWEBPAGE_BASE].PageUrl,g_bInsideOpenPage);
}

void CMainFrame::OnF11WebPage()
{
	OpenCusWebPage(this,g_F11WebPage.PageUrl,TRUE);
}

void CMainFrame::OnHKeyProgram()
{
	CString tmpExecFile="";
	GetExecPathFromRegister(g_GC.HKey_Path,g_GC.HKey_KeyString,tmpExecFile);
	BOOL bRet = FALSE;
	if(strlen(g_GC.HKey_RunParam))
		bRet = ExecExtern(tmpExecFile.GetBuffer(0),m_hWnd,g_GC.HKey_RunParam);
	else
		bRet = ExecExtern(tmpExecFile.GetBuffer(0),m_hWnd);
	if(!bRet)
	{
		CString tmpHintStr;
		tmpHintStr.Format("运行[%s]出错,\r\n请先安装此程序模块!\r\n%s",g_GC.HKey_MenuName,g_GC.HKey_ErrorInfo);
		TDX_MessageBox(m_hWnd,tmpHintStr,MB_OK|MB_ICONEXCLAMATION);
	}
}

void CMainFrame::ConstructCustomPad(int padtype)
{
	CComView2 *tempWnd = (CComView2 *)SwitchToView("Com2");
	if(tempWnd)
		tempWnd->SendMessage(UM_CONSTRUCTPAD_MESSAGE,padtype);
}

void CMainFrame::OnMainWndMessage(int MessageNo)
{
	if(MessageNo == 0)
	{
		if(!g_bHasCaption)
		{
			if(m_wndWorkspace.IsWindowVisible())
				ShowControlBar(&m_wndWorkspace,FALSE,FALSE);
			else
				ShowControlBar(&m_wndWorkspace,TRUE,FALSE);
		}
		else
		{
			CWnd *wnd = CWnd::FromHandle (BigMenuWindow);
			if(wnd)
			{
				((CFrameWnd *)wnd)->ActivateFrame();
				((CFrameWnd *)wnd)->GetActiveView()->SendMessage(UM_MENU_INITPOS,0,0);
			}
			else
			{
				if(m_wndWorkspace.IsWindowVisible())
					ShowControlBar(&m_wndWorkspace,FALSE,FALSE);
				else
					ShowControlBar(&m_wndWorkspace,TRUE,FALSE);
			}
		}
		return;
	}
	if(MessageNo >= 81 && MessageNo <= 88)
	{
		OnZHSort(MessageNo - 81 + ID_ZHSORT_TYPE0);
		return;
	}
	if(MessageNo==89)
	{
		OnZHSort(ID_ZHSORT_TYPE9);
		return;
	}
	if(MessageNo == 811)
	{
		OnZHSort(ID_ZHSORT_TYPE10);
		return;
	}
	if(MessageNo == 812)
	{
		OnZHSort(ID_ZHSORT_TYPE11);
		return;
	}
	if(MessageNo >= 2000)
	{
		ConstructCustomPad(MessageNo-2000);
		return;
	}
	switch(MessageNo)
	{
	case 5:		//类型切换
		OnF5();
		break;
	case 1000:	//交易委托
		OnFuncWt();
		break;
	case 1001:	//闪电买入
		OnLevinBuy(IDC_LEVINBUY11);
		break;
	case 1002:	//撤单查询
		OnWTCancel();
		break;
	case 1003:	//闪电卖出
		OnLevinSell(IDC_LEVINSELL1);
		break;
	case 1005:	//闪电手
		OnQzSDS();
		break;
	case 1006:	//普通买入
		OnBuy(IDC_BUY11);
		break;
	case 1007:	//普通卖出
		OnSell(IDC_SELL1);
		break;
	case 1011:	//卖?价买入
	case 1012:
	case 1013:
	case 1014:
	case 1015:
		OnLevinBuy(IDC_LEVINBUY11+MessageNo-1011);
		break;
	case 1016:	//买?价买入
	case 1017:
	case 1018:
	case 1019:
	case 1020:
		OnLevinBuy(IDC_LEVINBUY1+MessageNo-1016);
		break;
	case 1021:	//买?价卖出
	case 1022:
	case 1023:
	case 1024:
	case 1025:
		OnLevinSell(IDC_LEVINSELL1+MessageNo-1021);
		break;
	case 1026:	//卖?价卖出
	case 1027:
	case 1028:
	case 1029:
	case 1030:
		OnLevinSell(IDC_LEVINSELL11+MessageNo-1026);
		break;
	case 1080:	//研究报告平台
		OnTdxReport();
		break;
	case 1100:	//基本资料
		OnGgzl();
		break;
	case 901://公式管理器
		OnFormula();
		break;
	case 902://条件选股测试
		OnTJTest();
		break;
	case 903://交易系统测试
		OnJYTest();
		break;
	case 904://探索最佳指标
		OnTestindex();
		break;
	case 905://条件选股
		OnConditionChoice();
		break;
	case 906://定制选股
		OnSimchoice();
		break;
	case 907://智能选股
		OnClechoice();
		break;
	case 908://插件选股
		OnPlugin();
		break;
	case 909://综合选股
		OnSelGP();
		break;
	case 910://
		OnAllWarncfg();
		break;
	case 911://
		OnAllWarngp();
		break;
	case 913:	//股票组合计算
		OnFuncBlockCalc();
		break;
	case 914:	//数据挖掘参考
		OnDataDig();
		break;
	case 917:
		OnDataDig();
		break;
	case 918://
		OnTjWarncfg();
		break;
	case 919://
		OnTjWarngp();
		break;	
	case 920:
		if(theViewManager.pActiveView)
			theViewManager.pActiveView->PostMessage(WM_COMMAND,ID_WRITETOTXTFILE);
		break;
	case 924:
		OnPadList();//版面管理器
		break;
	case 925:
		OnShowCustomPadSwitch();//显隐版面切换区
		break;
	case 930:	//系统设置
		OnOption();
		break;
	case 931:	//设置用户板块
		OnBlockOption();
		break;
	case 932:	//通讯设置
		OnConnectCfg();
		break;
	case 933:	//数据下载
		OnDownLoad();
		break;
	case 934:	//数据维护工具
		OnNodeTool();
		break;
	case 935:	//自动升级
		OnAutoup();
		break;
	case 936:	//帮助主题
		OnHelpFinder();
		break;
	case 937:	//用户论坛
		OnForum();
		break;
	default:
		break;
	}
	return;
}

void CMainFrame::LoginEmbedWT()
{
	if(g_OEM.bEmbedWTVersion)
	{
		DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
		if(!(dwStatus&JY_LOGINED))
		{
			BOOL bOK=FALSE;
			g_pConnectWnd = new CLogoDlg;
			((CLogoDlg *)g_pConnectWnd)->m_bOnlyLoginWT = TRUE;
			CWnd *tmpMainWnd = g_pMainWin;	//保存原来的接收窗口
			g_pMainWin = g_pConnectWnd;
			if(((CDialog *)g_pConnectWnd)->DoModal() == IDOK)
				bOK = TRUE;
			g_pMainWin = tmpMainWnd;
			TDEL(g_pConnectWnd);
			if(!bOK) return;
			//启动交易跟随程序
			if(g_GC.bAlongJy && strlen(g_GC.AlongPathFile))
				ExecExtern(g_GC.AlongPathFile,m_hWnd);			
			//中途登录交易,需重新取配置
			if(TC_GetExternLevinCfg_)
				TC_GetExternLevinCfg_(&g_ExternLevinCfg);			
		}
		if(m_wndRestspace.GetHeight()<100)
			m_wndRestspace.ResetSize();
		SwitchToRestSpace(AREA_WT);
		CRect rc;
		m_wndRestspace.GetWindowRect(&rc);
		if(m_wndRestspace.IsFloating() && (rc.bottom<50 || rc.top>GetSystemMetrics(SM_CYSCREEN)-50))
			m_wndRestspace.PostMessage(WM_COMMAND,IDC_BUTTON_MINI);
	}
}

void CMainFrame::NonNormalWt()
{
	char path[256]={0};
	GetPrivateProfileString("EXTERN","F9WTPATH","",path,255,g_WSXHStr+g_strYxhjCfg);
	if(strlen(path)>0)
	{
		if(!ExecExtern(path,m_hWnd,NULL,TRUE))
		{
			CFindExeDlg dlg;
			dlg.m_strCaption = _F("外部交易委托程序");
			dlg.m_strExeInfo = _F("请输入外部交易委托程序的路径:");
			dlg.m_strExePath = path;
			if(dlg.DoModal() == IDOK)
			{
				char path[256]={0};
				strcpy(path,dlg.m_strExePath);
				WritePrivateProfileString("EXTERN","F9WTPATH",path,g_WSXHStr+g_strYxhjCfg);
			}
		}
	}
	else
		NormalWt();
}

#include "verify.h"
void CMainFrame::NormalWt()
{
	if(g_OEM.bEmbedWTVersion)
	{
		OnFuncEmbWT();
		return;
	}
	char path[256]={0};
	BOOL bDone = FALSE;
	do
	{
		short nGetPathOK = GetCfgWTPath(g_WSXHStr+g_strYxhjCfg,path,"*****");
		if(nGetPathOK==0) break;
		if(ExecExtern(path,m_hWnd,NULL,TRUE))
			bDone = TRUE;
		else if(nGetPathOK==1)
		{
			if(g_GC.nShareMode) //如果是共享模式,不让用户设置交易委托的路经
			{
				TDX_MessageBox(m_hWnd,"执行交易委托程序失败!",MB_OK|MB_ICONERROR);
				return;
			}
			CFindExeDlg dlg;
			dlg.m_strCaption = _F("外部交易委托程序");
			dlg.m_strExeInfo = _F("请输入外部交易委托程序的路径:");
			dlg.m_strExePath = path;
			if(dlg.DoModal() == IDOK)
			{
				strcpy(path,dlg.m_strExePath);
				WritePrivateProfileString("EXTERN","WTPATH",path,g_WSXHStr+g_strYxhjCfg);
				continue;
			}
			bDone = TRUE;
		}
	}while(!bDone);
}

#include "TestDlg.h"
void CMainFrame::OnF9()
{
	return;
	if(g_bDebugMode)
	{
		CTestDlg dlg;
		dlg.DoModal();
		return;
	}
	if(g_OEM.bF9WTUseF12Key)
		NormalWt();	
	else
		NonNormalWt();	
}

void CMainFrame::OnFuncWt()
{
	if(g_OEM.bF12SelectWT && g_OEM.bEmbedWTVersion)
	{
		//如果内嵌交易已经登录,则直接处理
		DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
		if((dwStatus&JY_LOGINED))
			OnFuncEmbWT();
		else
		{
			//对委托进行选择
			CWTPathSelDlg dlg(this);
			dlg.m_bFromFileCfg = FALSE;
			UINT nCmdID[3]={0};
			int i=0;
			if(strlen(g_NewJyCfg.AddedWT1_Name)>0)
			{
				strncpy(dlg.m_strWTName[i],g_NewJyCfg.AddedWT1_Name,19);
				dlg.m_strWTName[i][19]=0;
				nCmdID[i]=ID_FUNC_ADDEDWT1;
				i++;
			}
			if(strlen(g_NewJyCfg.AddedWT2_Name)>0)
			{
				strncpy(dlg.m_strWTName[i],g_NewJyCfg.AddedWT2_Name,19);
				dlg.m_strWTName[i][19]=0;
				nCmdID[i]=ID_FUNC_ADDEDWT2;
				i++;
			}
			if(strlen(g_NewJyCfg.AddedWT_Name)>0)
			{
				strncpy(dlg.m_strWTName[i],g_NewJyCfg.AddedWT_Name,19);
				dlg.m_strWTName[i][19]=0;
				nCmdID[i]=ID_FUNC_EMBWT;
				i++;
			}
			dlg.m_nWTNum=i;
			dlg.m_nSelExeIndex=l_nSelExeIndex;
			if(dlg.DoModal()==IDOK)
			{
				l_nSelExeIndex = dlg.m_nSelExeIndex;
				if(l_nSelExeIndex>=0 && l_nSelExeIndex<dlg.m_nWTNum)
					PostMessage(WM_COMMAND,nCmdID[l_nSelExeIndex]);
			}
		}
		return;
	}
	if(g_OEM.bF9WTUseF12Key)
		NonNormalWt();
	else
		NormalWt();
}

void CMainFrame::OnFuncEmbWT()
{
	if(!g_OEM.bEmbedWTVersion) return;
	if(!g_bHasEmbWT)
	{
		TDX_MessageBox(m_hWnd,"系统设置文件有错误,不能使用交易!",MB_ICONERROR);
		return;
	}
	int i=m_wndRestspace.GetCurWnd();
	if(i<0) return;
	if(m_wndRestspace.IsWindowVisible() && g_nRestPos[AREA_WT]==i)
	{
		if(m_wndRestspace.IsFullSize()) return;
		if(TC_BeforeHideWT_) TC_BeforeHideWT_();
		ShowControlBar(&m_wndRestspace,FALSE,FALSE);
		SetFocus();
	}
	else
		LoginEmbedWT();
}

void CMainFrame::OnFuncAddedWt1()
{
	if(strnicmp(g_NewJyCfg.AddedWT1_Path,"http",4)==0)
		OpenCusWebPage(this,g_NewJyCfg.AddedWT1_Path,FALSE);
	else
		ExecExtern(g_NewJyCfg.AddedWT1_Path,m_hWnd,NULL,TRUE);
}

void CMainFrame::OnFuncAddedWt2()
{
	if(strnicmp(g_NewJyCfg.AddedWT2_Path,"http",4)==0)
		OpenCusWebPage(this,g_NewJyCfg.AddedWT2_Path,FALSE);
	else
		ExecExtern(g_NewJyCfg.AddedWT2_Path,m_hWnd,NULL,TRUE);
}

void CMainFrame::OnFuncHxInfo()
{
	//打开核新的独立资讯程序
	//仅在认证时使用-V选项，菜单调用不传递-V
	char path[256],param[256];
	sprintf(path,"%sinfocenter\\InfoLogin.exe",HomePath);
	sprintf(param,"%X",m_hWnd);
	ExecExtern(path,m_hWnd,param,FALSE);
}

void CMainFrame::OnFuncZxjtInfo()
{
	char *	pParam = NULL;
	char	cParam[MAX_PATH]={0};
	if(g_OEM.bZxjtEmbedWTVersion)
	{
		DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
		if(dwStatus&JY_LOGINED)
		{
			CString strParam;
			Zxjt_Base64Info(strParam);
			strcpy(cParam,strParam);
			pParam=cParam;
		}
	}
	ExecExtern(g_GC.AlongPathFile,m_hWnd,pParam);
}

void CMainFrame::OnConnectHQ()
{
	if(g_OEM.bEmbedWTVersion && m_wndRestspace.IsWindowVisible())
	{
		if(TC_BeforeHideWT_) TC_BeforeHideWT_();
		ShowControlBar(&m_wndRestspace,FALSE,FALSE);
	}
	ShowControlBar(&m_wndWorkspace,FALSE,FALSE);
	if(theViewManager.pActiveView&&(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CIEBrowserView)) || theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CTxtView)) || theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CNewsView))))	
		SwitchToFirstView();	
	if(!OnLine)
	{
		g_nTmpDirectConnectHq = TRUE;
		SendMessage(WM_COMMAND,ID_CONNECT);
	}
	SetFocus();
}

//1-得到配置路径，可以执行，0-不执行，掠过操作，-1-自己设置路径
short CMainFrame::GetCfgWTPath(CString cfgPath,char *WTPath, char *pDefPath)
{
	char path[256]={0};
	DWORD pathres = GetPrivateProfileString("EXTERN","WTPATH",pDefPath,WTPath,255,cfgPath);
	if(pathres>0)
	{
		pathres = GetPrivateProfileString("EXTERN","WTPATH1","",path,255,cfgPath);
		if(pathres>0)
		{
			CWTPathSelDlg dlg(this);
			dlg.m_nSelExeIndex=l_nSelExeIndex;
			if(dlg.DoModal()==IDOK)
			{
				l_nSelExeIndex = dlg.m_nSelExeIndex;
				char wtpath[64]={0};
				if(l_nSelExeIndex>0)
					sprintf(wtpath,"WTPATH%d", l_nSelExeIndex);
				else 
					strcpy(wtpath,"WTPATH");
				GetPrivateProfileString("EXTERN",wtpath,pDefPath,WTPath,255,cfgPath);
				return l_nSelExeIndex+1;
			}
			return 0;
		}
	}
	return 1;
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CNewMDIFrameWnd::OnActivate(nState, pWndOther, bMinimized);
	if(bCustDrawCloseBtn())
		OnNcPaint();
}

void CMainFrame::OnConditionChoice() 
{
	/*
	if(m_pChoiceGPDlg)
	{
		if (m_pChoiceGPDlg->IsIconic()) 
			m_pChoiceGPDlg->ShowWindow(SW_RESTORE); 
		return;
	}
	//创建有模式对话框，一定要用指针动态创建
	m_pChoiceGPDlg = new CConditionGPDlg(this);
	m_pChoiceGPDlg->Create(IDD_CHOICEGP_DIALOG);
	m_pChoiceGPDlg->ShowWindow (SW_SHOW);
	*/
	InterlockedExchange(&g_nAutoCalcLock, 1);
	KillTimer(13);
	if(m_pChoiceBKDlg)
	{
		if (m_pChoiceBKDlg->IsIconic()) 
			m_pChoiceBKDlg->ShowWindow(SW_RESTORE); 
		m_pChoiceBKDlg->ShowWindow(SW_SHOW);
		m_pChoiceBKDlg->CenterWindow();
		return;
	}
	//创建有模式对话框，一定要用指针动态创建
	m_pChoiceBKDlg = new CChoiceBlockDlg(this);
	m_pChoiceBKDlg->Create(IDD_CHOICEBLOCKDLG);
	m_pChoiceBKDlg->ShowWindow (SW_SHOW);
	m_pChoiceBKDlg->CenterWindow();
}

void	CMainFrame::OnAutoCalcTjBlock()
{
	KillTimer(13);
	if(!m_pChoiceBKDlg)
	{
		m_pChoiceBKDlg = new CChoiceBlockDlg(this);
		m_pChoiceBKDlg->Create(IDD_CHOICEBLOCKDLG);
	}
	m_pChoiceBKDlg->ShowWindow (SW_HIDE);
	m_pChoiceBKDlg->SendMessage(WM_COMMAND, IDOK);
//	SetTimer(13, 30*1000, NULL);
}

#include "MySelGpDlg.h"
void CMainFrame::OnSelGP()
{
	if(m_pSelGP)
	{
		if (m_pSelGP->IsIconic()) 
			m_pSelGP->ShowWindow(SW_RESTORE); 
		return;
	}
	//创建有模式对话框，一定要用指针动态创建
	m_pSelGP = new CMySelGpDlg(this);
	m_pSelGP->Create(IDD_SELGP_DIALOG);
	m_pSelGP->ShowWindow (SW_SHOW);
}

void CMainFrame::OnUpdateSelGP(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pChoiceGPDlg==NULL && m_pPluginDlg==NULL && m_pCleChoiceDlg==NULL && m_pChoiceBKDlg==NULL);
}

#include "SimChoiceDlg.h"
void CMainFrame::OnSimchoice() 
{
	if(m_pSimChoiceDlg)
	{
		if (m_pSimChoiceDlg->IsIconic()) 
			m_pSimChoiceDlg->ShowWindow(SW_RESTORE); 
		return;
	}
	m_pSimChoiceDlg = new CSimChoiceDlg(this);
	m_pSimChoiceDlg->Create(IDD_SIMCHOICE_DIALOG);
	m_pSimChoiceDlg->ShowWindow (SW_SHOW);
}

void CMainFrame::OnUpdateSimchoice(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pSelGP==NULL && m_pChoiceGPDlg==NULL && m_pPluginDlg==NULL && m_pCleChoiceDlg==NULL && m_pChoiceBKDlg==NULL);
}

#include "DataDigDlg.h"
void CMainFrame::OnDataDig()
{
	if(m_pDataDigDlg)
	{
		if (m_pDataDigDlg->IsIconic()) 
			m_pDataDigDlg->ShowWindow(SW_RESTORE); 
		return;
	}
	if(OnLine)
	{
		m_pDataDigDlg = new CDataDigDlg(this);
		m_pDataDigDlg->Create(IDD_DATADIG_DIALOG);
		m_pDataDigDlg->ShowWindow (SW_SHOW);
	}
}

void CMainFrame::SearchBlock(WPARAM wParam,LPARAM lParam)
{
//	return;
	if(m_pInBlockDlg)
	{
		if (m_pInBlockDlg->IsIconic()) 
			m_pInBlockDlg->ShowWindow(SW_RESTORE); 
		m_pInBlockDlg->SendMessage(UM_SEARCH_BLOCK,wParam,lParam);
		return;
	}
	else
	{
		m_pInBlockDlg = new CInBlockDlg(this);
		m_pInBlockDlg->Create(IDD_INBLOCK_DIALOG);
		m_pInBlockDlg->ShowWindow (SW_SHOW);
		m_pInBlockDlg->SendMessage(UM_SEARCH_BLOCK,wParam,lParam);
	}
}

void CMainFrame::SearchGpGxd(WPARAM wParam,LPARAM lParam)
{
	if(m_pGpGxdDlg)
	{
		if (m_pGpGxdDlg->IsIconic()) 
			m_pGpGxdDlg->ShowWindow(SW_RESTORE); 
		m_pGpGxdDlg->SendMessage(UM_SEARCH_GPGXD,wParam,0);
		return;
	}
	else
	{
		m_pGpGxdDlg = new CGpGxdDlg(this);
		m_pGpGxdDlg->Create(IDD_GPGXD);
		m_pGpGxdDlg->ShowWindow (SW_SHOW);
		m_pGpGxdDlg->SendMessage(UM_SEARCH_GPGXD,wParam,1);
	}
}

void CMainFrame::OnUpdateGPGradeDlg(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(g_GC.bHasGPGrade);
}

void CMainFrame::OnTdxReport()
{
	if(g_GC.bHasGPGrade)
		OpenCusWebPage(this,REPORT_WEBPAGE,TRUE);
	else
		TDX_MessageBox(m_hWnd,"当前版本设置不支持此功能!",MB_OK|MB_ICONEXCLAMATION);
}

void CMainFrame::OnGuba()
{
	OpenCusWebPage(this,GUBA_WEBPAGE,TRUE);
}

void CMainFrame::OnUpdateTdxReport(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(g_GC.bHasGPGrade);
}

void CMainFrame::OnClechoice() 
{
	if(m_pCleChoiceDlg)
	{
		if (m_pCleChoiceDlg->IsIconic()) 
			m_pCleChoiceDlg->ShowWindow(SW_RESTORE); 
		return;
	}
	g_pZnxg = new CZnxg(this);
	if(!g_pZnxg->CalcAllStockStyle()) 
	{
		TDEL(g_pZnxg);
		return;
	}
	m_pCleChoiceDlg = new CCleChoiceDlg(this);
	m_pCleChoiceDlg->Create(IDD_CLECHOICE_DIALOG);
	m_pCleChoiceDlg->ShowWindow (SW_SHOW);
}

void CMainFrame::OnUpdateClechoice(CCmdUI* pCmdUI) 
{	
	pCmdUI->Enable(m_pSelGP==NULL && m_pSimChoiceDlg==NULL && m_pChoiceGPDlg==NULL && m_pPluginDlg==NULL && m_pChoiceBKDlg==NULL);
}

void CMainFrame::CancelNoModal(WPARAM wParam,LPARAM lParam)
{
	CDialog *tempDialog = (CDialog *)wParam;
	if(!tempDialog) return;
	tempDialog->DestroyWindow();
	if( tempDialog == (CDialog *)m_pChoiceGPDlg )
		m_pChoiceGPDlg = NULL;
	if( tempDialog == (CDialog *)m_pChoiceBKDlg )
	{
		m_pChoiceBKDlg = NULL;
	//	SetTimer(13, 30*1000, NULL);
		InterlockedExchange(&g_nAutoCalcLock, 0);
	}
	if( tempDialog == (CDialog *)m_pSimChoiceDlg)
		m_pSimChoiceDlg = NULL;	
	if( tempDialog == (CDialog *)m_pCleChoiceDlg )
		m_pCleChoiceDlg = NULL;
	if( tempDialog == (CDialog *)m_pPluginDlg)
		m_pPluginDlg = NULL;
	if( tempDialog == (CDialog *)m_pSelGP)
		m_pSelGP = NULL;	
	if( tempDialog == (CDialog *)m_pTJTestDlg )
		m_pTJTestDlg = NULL;
	if( tempDialog == (CDialog *)m_pJYTestDlg )
		m_pJYTestDlg = NULL;
	if( tempDialog == (CDialog *)m_pTestZBDlg )
		m_pTestZBDlg = NULL;
	if( tempDialog == (CDialog *)m_pZHSortDlg )
		m_pZHSortDlg = NULL;
	if( tempDialog == (CDialog *)m_pHKSeatQueryDlg )
		m_pHKSeatQueryDlg = NULL;
	if( tempDialog == (CDialog *)m_pInBlockDlg)
		m_pInBlockDlg = NULL;
	if( tempDialog == (CDialog *)m_pGpGxdDlg)
		m_pGpGxdDlg = NULL;
	if( tempDialog == (CDialog *)m_pDataDigDlg)
		m_pDataDigDlg = NULL;
	if( tempDialog == (CDialog *)m_pBlockCalcDlg )
		m_pBlockCalcDlg = NULL;
	if( tempDialog == (CDialog *)m_pFluxCalcDlg )
		m_pFluxCalcDlg = NULL;
	if( tempDialog == (CDialog *)m_pVideoDlg )
		m_pVideoDlg = NULL;
	if( tempDialog == (CDialog *)m_pWebWinDlg )
		m_pWebWinDlg = NULL;
	if( tempDialog == (CDialog *)m_pDeepWarnDlg )
		m_pDeepWarnDlg = NULL;
	if( tempDialog == (CDialog *)m_pUrgentPopDlg )
		m_pUrgentPopDlg = NULL;
	if( tempDialog == (CDialog *)m_pIWContentDlg )
		m_pIWContentDlg = NULL;
	if( tempDialog == (CDialog *)m_pPopMyIEDlg )
		m_pPopMyIEDlg = NULL;
	m_WarnManager.DeleteDlg(tempDialog);
	TDEL(tempDialog);
}

#include "UrgentListDlg.h"
void CMainFrame::OnPopUrgent()
{
	if(m_pUrgentPopDlg)
	{
		if (m_pUrgentPopDlg->IsIconic()) 
			m_pUrgentPopDlg->ShowWindow(SW_RESTORE);
		m_pUrgentPopDlg->FillList();
		return;
	}
	m_pUrgentPopDlg = new CUrgentListDlg(this);
	m_pUrgentPopDlg->Create(IDD_URGENTLIST);
	m_pUrgentPopDlg->ShowWindow (SW_SHOW);
	m_pUrgentPopDlg->FillList();
}

void CMainFrame::OnFormula() 
{
	if(IsBig5Version()) return;
	if(g_bDonotClose) return;

	if(g_pCalc->_CallDTopViewCalc_16())
	{
		for(int i = 0;i<theViewManager.GetWindowNum();i++)
		{
			if( ((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CComView)) ||
				((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CComView2)) )
				((CView *)theViewManager.arViews[i])->SendMessage(UM_AFTERFORMULA);
		}
		FillZBTree(); //重新排列TreeView
	}
}

void CMainFrame::OnTJTest() 
{
	if(m_pTJTestDlg)
	{
		if (m_pTJTestDlg->IsIconic()) 
			m_pTJTestDlg->ShowWindow(SW_RESTORE); 
		return;
	}
	m_pTJTestDlg = new CParamTestDlg(this);
	m_pTJTestDlg->m_nTestType=TJ_TYPE;
	m_pTJTestDlg->Create(IDD_TESTPARAMDLG);
	m_pTJTestDlg->ShowWindow(SW_SHOW);
}

void CMainFrame::OnJYTest() 
{
	if(m_pJYTestDlg)
	{
		if (m_pJYTestDlg->IsIconic()) 
			m_pJYTestDlg->ShowWindow(SW_RESTORE); 
		return;
	}
	m_pJYTestDlg = new CParamTestDlg(this);
	m_pJYTestDlg->m_nTestType=EXP_TYPE;
	m_pJYTestDlg->Create(IDD_TESTPARAMDLG);
	m_pJYTestDlg->ShowWindow(SW_SHOW);
}

void CMainFrame::OnTestindex() 
{
	if(m_pTestZBDlg)
	{
		if (m_pTestZBDlg->IsIconic()) 
			m_pTestZBDlg->ShowWindow(SW_RESTORE); 
		return;
	}
	m_pTestZBDlg = new CTestZBDlg(this);  
	m_pTestZBDlg->Create(IDD_TESTZBDLG);
	m_pTestZBDlg->ShowWindow (SW_SHOW);
}

void CMainFrame::FillGnTree()
{
	CWnd *pTmpWnd = NULL;
	for(int i=0;i < m_wndWorkspace.GetWndNum();i++)
	{
		pTmpWnd = m_wndWorkspace.GetWnd(i);
		if(pTmpWnd && pTmpWnd->IsKindOf(RUNTIME_CLASS(CGnTree))) 
			((CGnTree *)pTmpWnd)->FillTree();
	}			
}

void CMainFrame::FillGPTree()
{
	CWnd *pTmpWnd = NULL;
	for(int i=0;i < m_wndWorkspace.GetWndNum();i++)
	{
		pTmpWnd = m_wndWorkspace.GetWnd(i);
		if(pTmpWnd && pTmpWnd->IsKindOf(RUNTIME_CLASS(CGpTree))) 
			((CGpTree *)pTmpWnd)->FillTree();
	}			
}

void CMainFrame::FillZBTree()
{
	CWnd *pTmpWnd = NULL;
	for(int i=0;i < m_wndWorkspace.GetWndNum();i++)
	{
		pTmpWnd = m_wndWorkspace.GetWnd(i);
		if(pTmpWnd && pTmpWnd->IsKindOf(RUNTIME_CLASS(CZbTree))) 
			((CZbTree *)pTmpWnd)->FillTree();
	}			
}

#include "SelectGPDlg.h"
void CMainFrame::OnGetgp() 
{
	CSelectGPDlg dlg;
	dlg.bSingleSelect = TRUE;
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.SelectedGPNum > 0)
		{
			CString tmpValue;
			if(dlg.SelectedGPIndex[0]<DS_FROM)
			{
				MemStkInfo *tmpInfo = (*g_pStockIO)[dlg.SelectedGPIndex[0]];
				tmpValue.Format("%d%s",tmpInfo->setcode,tmpInfo->Code);
				CKeyGuy::OnCommonProcess(0,tmpValue);
			}
			else
			{
				MemStkInfo *pStkInfo = (*g_pStockIO)[dlg.SelectedGPIndex[0]];
				tmpValue.Format("%d%s",pStkInfo->setcode,pStkInfo->Code);
				CKeyGuy::OnCommonProcess(pStkInfo->setcode+20,tmpValue);
			}
		}
	}
}

void CMainFrame::OnUpdateFormula(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!g_bDonotClose);
}

void CMainFrame::CloseMDIWindow(BOOL bKeepSpec)
{
	CView *tempView;
	CFrameWnd *tempFrameWnd;
	for(int i = theViewManager.GetWindowNum()-1;i >= 0;i--)
	{
		tempView = (CView *)theViewManager.arViews[i];
		if(!tempView) continue;
		tempFrameWnd = (CFrameWnd *)(tempView->GetParent());
		if(!tempFrameWnd || !(::IsWindow(tempFrameWnd->m_hWnd))) continue;
		if( !bKeepSpec || (tempFrameWnd->m_hWnd != FirstWindow && !tempView->IsKindOf(RUNTIME_CLASS(CBigMenuView)) && !tempView->IsKindOf(RUNTIME_CLASS(CIEBrowserView))) )
			tempFrameWnd->SendMessage(WM_CLOSE);
	}
}

void CMainFrame::CloseallAndKeepSpec() 
{
	CloseMDIWindow(TRUE);
	if(LayoutFlag)
		SendMessage(UM_HIDESHOWBUTTON,(g_bFirstCusPad||g_bHasCaption)?HideShowBtn_Homepage:HideShowBtn_Hq);
}

void CMainFrame::OnSysPad()
{
	if(g_bFirstCusPad)
	{
		CFrameWnd *tempFrameWnd = (CFrameWnd *)(CWnd::FromHandle (FirstWindow));
		if(!tempFrameWnd) return;
		tempFrameWnd->ActivateFrame();
		CComView2 *tempView = (CComView2 *)theViewManager.pActiveView;
		int nDefNo=GetCusPadTypeParam(g_DefCusPadName);
		if(nDefNo>=0)
			tempView->SendMessage(UM_CONSTRUCTPAD_MESSAGE, nDefNo);
	}
	else
	{
		TDX_MessageBox(m_hWnd,"没有设置某定制版面作为初始版面!",MB_OK|MB_ICONINFORMATION);
	}
}

void CMainFrame::OnUpdateSysPad(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnShowhq()
{
	if(!(theViewManager.pActiveView)->IsKindOf(RUNTIME_CLASS(CGridView)))
		SwitchToView("Grid");
}

void CMainFrame::OnShowzst() 
{
	OnCommCommand(CMD_COM,501);	
}

void CMainFrame::OnShowvip() 
{
	OnCommCommand(CMD_COM,300);
}

void CMainFrame::OnUpdateChoicegp(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pSimChoiceDlg==NULL && m_pSelGP==NULL && m_pPluginDlg==NULL && m_pCleChoiceDlg==NULL);
}

void CMainFrame::OnIdsTipoftheday() 
{
	CTipDlg dlg;
	dlg.DoModal();	
}

void CMainFrame::OnMinimize() 
{
	WINDOWPLACEMENT m_windowplacement;
	GetWindowPlacement (&m_windowplacement);
	m_windowplacement.showCmd=SW_MINIMIZE;
	SetWindowPlacement (&m_windowplacement);
	
	if(g_bMin3Tray)
	{
		ShowWindow(SW_HIDE);
		NotifyMessage(NIM_ADD,m_hIcon);
	}
}

void CMainFrame::OnRestore() 
{
	WINDOWPLACEMENT m_windowplacement;
	GetWindowPlacement (&m_windowplacement);
	if(IsZoomed())
		m_windowplacement.showCmd=SW_RESTORE;
	else
		m_windowplacement.showCmd=SW_SHOWMAXIMIZED;
	SetWindowPlacement (&m_windowplacement);

	if(g_bMin3Tray)
	{
		SetForegroundWindow();
		NotifyMessage(NIM_DELETE,m_hIcon);
	}
}

#include "AbsWarnPage.h"
#include "OtherWarnPage.h"
#include "OtherWarn2Page.h"
#include "warninfopage.h"

void CMainFrame::OnAllWarncfg() 
{
	CPropertySheet WarnCfgSheet("");
	COtherWarnPage	OtherPage;
	CWarnInfoPage   WarnInfoPage;
	WarnCfgSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	WarnCfgSheet.AddPage (&OtherPage);
	WarnCfgSheet.AddPage (&WarnInfoPage);
	WarnCfgSheet.SetTitle (_F("市场雷达设置"));

	if(WarnCfgSheet.DoModal ()==IDOK)
	{
		if((!g_bAllWarnSwitch && g_nUnitRadarFlag==0) && OnLine)
		{
			if(TDX_MessageBox(m_hWnd,"市场雷达还没有打开,您是否想打开?",MB_YESNO|MB_ICONINFORMATION ) == IDYES)
			{
				g_bAllWarnSwitch = TRUE;
				m_WarnManager.UpdateWarnGP();
			}
		}
		m_WarnManager.GetWarnGpSet();
		m_nWarnReq = 0;
	}
}

void CMainFrame::OnAllWarnSwitch()
{
	g_bAllWarnSwitch = !g_bAllWarnSwitch;
	//重新设置预警集合
	m_WarnManager.GetWarnGpSet();
}

void CMainFrame::OnUpdateWarnSwitch(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(g_bAllWarnSwitch);
}

void CMainFrame::OnTjWarncfg() 
{
	CPropertySheet WarnCfgSheet("");
	CAbsWarnPage	AbsPageGp;
	AbsPageGp.m_psp.dwFlags |= PSP_USETITLE;
	AbsPageGp.m_psp.pszTitle=_F("预警股票");
	AbsPageGp.m_nListFlag=GPLIST;
	CAbsWarnPage	AbsPageTJ;
	AbsPageTJ.m_psp.dwFlags |= PSP_USETITLE;
	AbsPageTJ.m_psp.pszTitle=_F("公式设置");
	AbsPageTJ.m_nListFlag=TJLIST;
	COtherWarn2Page OtherWarn2;

	WarnCfgSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	WarnCfgSheet.AddPage (&AbsPageGp);
	WarnCfgSheet.AddPage (&AbsPageTJ);
	WarnCfgSheet.AddPage (&OtherWarn2);
	WarnCfgSheet.SetTitle (_F("条件预警设置"));

	if(WarnCfgSheet.DoModal ()==IDOK)
	{
		if(g_pAbsConditionNum<1)
			TDX_MessageBox(m_hWnd,"您没有指定股票,条件预警将不会生效!",MB_OK|MB_ICONINFORMATION);
		else if(!g_bTjWarnSwitch && OnLine)
		{
			if(TDX_MessageBox(m_hWnd,"条件预警还没有打开,您是否想打开?",MB_YESNO|MB_ICONINFORMATION ) == IDYES)
			{
				g_bTjWarnSwitch = TRUE;
				m_WarnManager.UpdateTJGP();
			}
		}
		m_WarnManager.GetWarnGpSet();	
		if(theViewManager.pActiveView && theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)))
			theViewManager.pActiveView->SendMessage(UM_CALCANDPAINT);
	}
}

void CMainFrame::OnTjWarnSwitch()
{
	g_bTjWarnSwitch = !g_bTjWarnSwitch;
	//重新设置预警集合
	m_WarnManager.GetWarnGpSet();
}

void CMainFrame::OnUpdateTjWarnSwitch(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(g_bTjWarnSwitch);
}

void CMainFrame::OnNcRButtonUp(UINT nHitTest, CPoint point) 
{
	if(!g_bHasCaption)
	{
		CRect TotalRc;
		GetWindowRect(&TotalRc);
		CRgn m_rgn;
		m_rgn.CreateRectRgn (TotalRc.left,TotalRc.top,TotalRc.right,TotalRc.top+20);
		if(m_rgn.PtInRegion (point))
		{
			CNewMenu menu;
			menu.LoadMenu (IDR_RIGHTMENU);
			CNewMenu *pPopup = (CNewMenu *)menu.GetSubMenu(7);
			pPopup->TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,this);
		}
	}	
	CNewMDIFrameWnd::OnNcRButtonUp(nHitTest, point);
}

void CMainFrame::OnHKSeatQuery()
{
	PostMessage(UM_SEARCH_HKSEAT,-1,0);
}

void CMainFrame::SearchHKSeat(WPARAM wParam,LPARAM lParam)
{
	if(!DSOnLine)
	{
		CString tmpStr;
		tmpStr.Format("没有连接%s，请先连接!",g_DS.DSOEMName);
		TDX_MessageBox(m_hWnd,tmpStr,MB_ICONINFORMATION|MB_OK);
		return;
	}
	if(!g_GC.bIsSSDS)
	{
		TDX_MessageBox(m_hWnd,"实时港股才有经纪席位追踪功能!",MB_ICONINFORMATION|MB_OK);
		return;
	}
	if(m_pHKSeatQueryDlg)
	{
		((CSeatQueryDlg*)m_pHKSeatQueryDlg)->m_nInitSeatID = wParam;
		m_pHKSeatQueryDlg->DoSearchForInitSeatID();
		if(m_pHKSeatQueryDlg->IsIconic()) 
			m_pHKSeatQueryDlg->ShowWindow(SW_RESTORE); 
		return;
	}
	m_pHKSeatQueryDlg = new CSeatQueryDlg(this);
	m_pHKSeatQueryDlg->Create(IDD_SEATQUERY);
	m_pHKSeatQueryDlg->m_nInitSeatID = wParam;
	m_pHKSeatQueryDlg->DoSearchForInitSeatID();
	m_pHKSeatQueryDlg->ShowWindow(SW_SHOW);
}

#include "DSZHSortDlg.h"
void CMainFrame::OnZHSort(UINT nID)
{
	int nTmpType=nID-ID_ZHSORT_TYPE0;
	if(nID==ID_ZHSORT_TYPE11)
		nTmpType=TYPE_CYB;
	else if(nID==ID_ZHSORT_TYPE10)
		nTmpType=TYPE_QZ;
	else if(nID==ID_ZHSORT_TYPE9)
		nTmpType=TYPE_ZXB;
	if(m_pZHSortDlg)
	{
		if (m_pZHSortDlg->IsIconic()) 
			m_pZHSortDlg->ShowWindow(SW_RESTORE); 
		m_pZHSortDlg->SendMessage(WM_COMMAND,ID_SCSTART+nTmpType);
		return;
	}
	m_pZHSortDlg = new CDSZHSortDlg(this);
	m_pZHSortDlg->Create(IDD_ZHSORT_DIALOG);
	m_pZHSortDlg->ShowWindow (SW_SHOW);
	m_pZHSortDlg->SendMessage(WM_COMMAND,ID_SCSTART+nTmpType);
}

void CMainFrame::OnUpdateZHSort(CCmdUI* pCmdUI)
{
	BOOL bRet = FALSE;
	if(m_pZHSortDlg)
	{
		if(pCmdUI->m_nID==ID_ZHSORT_TYPE11)
			bRet = m_pZHSortDlg->GetWhichType()==TYPE_CYB;
		else if(pCmdUI->m_nID==ID_ZHSORT_TYPE10)
			bRet = m_pZHSortDlg->GetWhichType()==TYPE_QZ;
		else if(pCmdUI->m_nID==ID_ZHSORT_TYPE9)
			bRet = m_pZHSortDlg->GetWhichType()==TYPE_ZXB;
		else
			bRet = m_pZHSortDlg->GetWhichType()==pCmdUI->m_nID-ID_ZHSORT_TYPE0;
	}
	pCmdUI->SetCheck(bRet);		
}

void CMainFrame::OnUpdateZHSortStyle(CCmdUI* pCmdUI)
{
	if(pCmdUI->m_nID-ID_ZHSORT_STYLE0==g_nZHStyle)
		pCmdUI->SetRadio(TRUE);
	else pCmdUI->SetRadio(FALSE);
}

void CMainFrame::OnFuncBlockCalc()
{
	if(m_pBlockCalcDlg)
	{
		if (m_pBlockCalcDlg->IsIconic()) 
			m_pBlockCalcDlg->ShowWindow(SW_RESTORE); 
		return;
	}
	m_pBlockCalcDlg = new CBlockCalcDlg(this);
	m_pBlockCalcDlg->Create(IDD_BLOCKCALC);
	m_pBlockCalcDlg->ShowWindow (SW_SHOW);
}

UINT CMainFrame::OnNcHitTest(CPoint point) 
{
	UINT tempint = CNewMDIFrameWnd::OnNcHitTest(point);
	CRect rc;
	GetWindowRect(&rc);
	point.x-=rc.left;
	point.y-=rc.top;
	if(m_NCInfo.NcRect.PtInRect(point))
		return (tempint==HTMENU)?HTCAPTION:tempint;
	else
		return tempint;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style&=~FWS_ADDTOTITLE;
	cs.cx = 795;
	cs.cy = 526;
	BOOL bRes = CNewMDIFrameWnd::PreCreateWindow(cs);
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();
	// see if the class already exists
	if (!::GetClassInfo(hInst, szMainFrameClass, &wndcls))
	{
		// get default stuff
		::GetClassInfo(hInst, cs.lpszClass, &wndcls);
		// register a new class
		wndcls.lpszClassName = szMainFrameClass;
		wndcls.hIcon = ::LoadIcon(hInst, MAKEINTRESOURCE(IDR_MAINFRAME));
		if (!AfxRegisterClass(&wndcls))
			AfxThrowResourceException();
	}
	cs.lpszClass = szMainFrameClass;
	return bRes;
}

void  CMainFrame::SwitchToRestSpace(int resttype)
{	
	if(!bRestShow)
	{
		if(resttype==AREA_WT)
			m_wndRestspace.PostMessage(UM_STARTFUNC_MESSAGE,0,0);
		else
			m_wndRestspace.SendMessage(UM_STARTFUNC_MESSAGE,0,0);
	}
	if(!m_wndRestspace.IsWindowVisible())
		ShowControlBar(&m_wndRestspace,TRUE,TRUE);
	if(g_nRestPos[resttype] >= 0)
		m_wndRestspace.SetActiveWnd(g_nRestPos[resttype]);
	if(resttype==AREA_WT)
		TC_ArrangeWnd_(0);
}

void  CMainFrame::SwitchToWorkSpace(int treetype)
{
	m_wndWorkspace.SetActiveWnd(g_nTreePos[treetype]); //设置网站树
	ShowControlBar(&m_wndWorkspace,TRUE,TRUE);		
}

//6XXXXXX 深圳股票 7XXXXXX 上海股票 8XXXXXX 不好确定
LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL l_bProcessClip=FALSE;
	static int  l_nTotalConnectInfoNum=0;
#ifdef OEM_NEWJY
	if(message==WM_EXITMENULOOP && m_wndCaptionBar.m_hmenu)
		::PostMessage(m_wndCaptionBar.m_hWnd,UM_EXITMENULOOP,0,0);
#endif
	//如果是通知关闭
	if(message == g_uCloseMsg)
	{
		//关闭本系统,直接退出前先去掉前面的模态对话框
		g_bDirectQuit = TRUE;
		CancelModalWindows();
		PostMessage(WM_CLOSE);
	}
	//切换行情
	if(message == g_WTMsgID || message == g_WTMsgID2)
	{
		long CodeValue = (long)wParam;
		memset(&g_ET.HQ,0,sizeof(struct ExternHQ));
		g_ET.HWnd=(HWND)lParam;
		g_ET.GPSetCode = 0;
		g_ET.GPCode[0]=0;
		g_ET.Ready=1;
		g_ET.IsTdxJyHQ=0;
		//
		CString tmpCode;
		tmpCode.Format("%ld",CodeValue);
		int setcode = (tmpCode[0]=='6')?SZ:((tmpCode[0]=='7')?SH:-1);
		if(tmpCode[0]<'3') //期货品种
		{
			int nZmCnt=tmpCode[0]-'0';
			int nTmpLen=tmpCode.GetLength();
			if(nZmCnt<0 || 2*nZmCnt>nTmpLen) nZmCnt=0;
			tmpCode = tmpCode.Mid(1);
			char tmpString[20]={0},tmpNumString[20]={0};
			int i,k=0;
			for(i=0;i<nZmCnt;i++)
			{
				strncpy(tmpNumString,tmpCode.Mid(i*2),2);
				tmpNumString[2]=0;
				tmpString[k++]=atol(tmpNumString)+'A';
			}
			for(i=nZmCnt*2;i<nTmpLen;i++)
				tmpString[k++]=tmpCode[i];
			tmpCode=tmpString;
		}
		else
			tmpCode = tmpCode.Mid(1);
		MemStkInfo *pStkInfo = g_pStockIO->Get_MemStkInfo(tmpCode,setcode);
		if(pStkInfo)
		{
			if(OnLine)
			{
				g_ET.GPSetCode=pStkInfo->setcode;
				strcpy(g_ET.GPCode,pStkInfo->Code);
				g_ET.Ready=0;
			}
			ToZst(pStkInfo->Code,pStkInfo->setcode,FALSE);
			if(g_ET.HWnd && g_ET.Ready) //向调用者的窗口发送数据消息
			{
				struct CurrStockData tmphq;
				g_pStockIO->ReadCurrStockData(pStkInfo,&tmphq);
				ETSendCopyDataMessage(g_ET.HWnd,pStkInfo,&tmphq);
			}
		}
	}
	//刷新所有行情
	if(message == g_uRefreshAllHQ)
	{
		RefreshAllHq(10);
	}
	//取资金帐号信息
#ifdef OEM_NEWJY
	if(message == QryZJZHMsg)
	{
		//如果获取资金帐户,其中wParam定为'1'
		if(wParam==1 && strlen(g_JyInfo.zjzh)>0)
		{
			DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
			if(dwStatus&JY_LOGINED)
				::PostMessage(HWND_BROADCAST,RetZJZHMsg,atol(g_JyInfo.zjzh),0);
		}
	}
	if(message == QryZJZHMsg_My)
	{
		//如果获取资金帐户,其中wParam定为'1'
		if(wParam==1 && strlen(g_JyInfo.zjzh)>0)
		{
			DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
			if(dwStatus&JY_LOGINED)
				::PostMessage(HWND_BROADCAST,RetZJZHMsg_My,atol(g_JyInfo.zjzh),0);
		}
	}
#endif

	if( WM_CHANGECBCHAIN == message)		//Clipboard Viewer注销 
	{
		//如果注销的Clipboard Viewer窗口是本窗口的下一个窗口， 
		//则修改本窗口保存的下一个窗口句柄, 	
		//否则将该消息传递到Clipboard Viewer Chain的下一个窗口 
		if ((HWND) wParam == m_hWndNextViewer) 
			m_hWndNextViewer = (HWND) lParam; 
		else if (m_hWndNextViewer != NULL) 
			::SendMessage(m_hWndNextViewer, message, wParam, lParam); 
	}
	
	if(WM_DRAWCLIPBOARD == message && m_bClipSpy && g_pKeyWordSearch && !l_bProcessClip)		//剪贴板内容变化 
	{
		l_bProcessClip=TRUE;
		::SendMessage(m_hWndNextViewer, message, wParam, lParam);
		//动作
		unsigned int anFormats[] = {CF_TEXT}; 
		unsigned int nFormat;
		nFormat = GetPriorityClipboardFormat(anFormats,sizeof(anFormats));
		if(nFormat == CF_TEXT) 			//搜索代码	
		{ 	
			::OpenClipboard(m_hWnd);
			HGLOBAL hMem = GetClipboardData(nFormat); 
			DWORD nLen = GlobalSize(hMem);
			char *lpstr = (char*)GlobalLock(hMem); 
			char *lpstrend = &lpstr[nLen-1]; 
			
			//关键字查找
			g_pKeyWordSearch->InitResult();
			while (lpstr<lpstrend)
				lpstr = g_pKeyWordSearch->SearchKeyWord(STK_NUMCODE_KEY|STK_NAME_KEY, lpstr, lpstrend);
			if(!g_pKeyWordSearch->m_KeyWordOK.empty())
			{
				CBatchDlg dlg;
				dlg.m_bTopMost=TRUE;//提到最屏幕前面
				dlg.m_nOwnerType = 1;
				int i=0, j=0 , stknum = g_pKeyWordSearch->m_KeyWordOK.size();
				for(i=0;i < stknum;i++)
					dlg.m_nStkIndex[j++] = g_pKeyWordSearch->m_KeyWordOK[i].nIDIndex;
				dlg.m_nStkNum = stknum;
				dlg.m_bDisableDelete = TRUE;
				dlg.DoModal();
			}
			
			GlobalUnlock(hMem); 
			CloseClipboard(); 
		}
		l_bProcessClip=FALSE;
	}
	//交易模块的消息
	if(UM_LEVIN_FEEDBACK_MSG == message)
	{
		if(wParam==FEEDBACK_CFGCHANGE)
			TC_GetExternLevinCfg_(&g_ExternLevinCfg);
		else
		{
			for(int j = 0;j < theViewManager.GetWindowNum();j++)
			{
				if( ((CView *)theViewManager.arViews[j])->IsKindOf(RUNTIME_CLASS(CComView)) )
					((CView *)theViewManager.arViews[j])->SendMessage(UM_LEVIN_FEEDBACK_MSG,wParam,lParam);
			}
		}		
	}
	
	return CNewMDIFrameWnd::WindowProc(message, wParam, lParam);
}
void CMainFrame::OnAutoup() 
{
	char cmdline[256];
	CString tmpStr; //对于个人版,升级不通过主站

	char strAutoUpZip[MAX_PATH]={0};
	CString	Ini = g_WSXHStr+(CString)g_strYxhjCfg;
	GetPrivateProfileString("Other","AutoUpZip","",strAutoUpZip,254,Ini);
	if(g_nUseDNSUrl==1)
	{
		CString tmpURL(strAutoUpZip);
		tmpURL.Replace(g_strDNSUrl[0], g_strDNSUrl[1]);
		strncpy(strAutoUpZip, (char*)(LPCSTR)tmpURL, MAX_PATH-2);
	}
	tmpStr.Format("webautoup %s.zip&%s&%s",strAutoUpZip,g_Scheme_King.YxhjPath,GetNowVersion());
	strcpy(cmdline,tmpStr);

	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartupInfo;
	memset(&siStartupInfo,0,sizeof(STARTUPINFO));
	siStartupInfo.cb = sizeof(STARTUPINFO);
	CString path;
	path.Format("%swebautoup.exe",HomePath);
	CreateProcess(path,cmdline,NULL, NULL, FALSE, 0,NULL, HomePath,&siStartupInfo, &piProcInfo);
}
/*
void CMainFrame::OnAutoup() 
{
	char cmdline[256];
	CString tmpStr; //对于个人版,升级不通过主站
	if(g_GC.bNoAutoup)
	{
		tmpStr.Format("autoupex http://www.tdx.com.cn/products/autoup/%s.zip&tdxw&%s",g_strAutoUpZip,g_szReleaseVersion);
		strcpy(cmdline,tmpStr);
	}
	else
	{
		struct NeedAutoupInfo l_NeedAutoupInfo;
		BOOL bRet=GetAutoupInfo(&l_NeedAutoupInfo);
		if(!bRet)
		{
			TDX_MessageBox(m_hWnd,"无相关升级信息.请连接行情主站!",MB_OK|MB_ICONINFORMATION);
			return;
		}
		if(l_NeedAutoupInfo.bNeedAutoup==0)
		{
			TDX_MessageBox(m_hWnd,"当前程序已是最新的,不需要升级!",MB_OK|MB_ICONINFORMATION);
			return;
		}
		l_NeedAutoupInfo.AutoupURL[118]=0;
		CString tmpURL=l_NeedAutoupInfo.AutoupURL;
		if(l_NeedAutoupInfo.bAutoupFromHQHost)
		{
			CString tmpURL2=tmpURL;
			int j=tmpURL2.ReverseFind('/');
			if(j>=0)
			{
				tmpURL2=tmpURL2.Right(tmpURL2.GetLength()-j-1);	
				j=tmpURL2.ReverseFind('.');
				if(j>=0)
				{
					tmpURL2=tmpURL2.Left(j);	
					tmpURL="update/"+tmpURL2+".zip";
				}
			}
		}
		//对于V1.05以上的自动升级程序，支持后台升级
		if(g_NeedAutoupInfo.bNeedAutoup>1)
			sprintf(cmdline,"autoupex %s&tdxw&%s&1",tmpURL,g_szReleaseVersion);
		else
			sprintf(cmdline,"autoupex %s&tdxw&%s&0",tmpURL,g_szReleaseVersion);
	}
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartupInfo;
	memset(&siStartupInfo,0,sizeof(STARTUPINFO));
	siStartupInfo.cb = sizeof(STARTUPINFO);
	CString path;
	path.Format("%sautoupex.exe",HomePath);
	CreateProcess(path,cmdline,NULL, NULL, FALSE, 0,NULL, HomePath,&siStartupInfo, &piProcInfo);
}
*/

#include "SpeedTestDlg.h"
void CMainFrame::OnSpeedTest()
{
	CSpeedTestDlg dlg;
	if(dlg.DoModal()==IDOK)
	{
		if(!g_bNewJyUseAdvHQHost)
		{
			g_HQHost.PrimaryHost=dlg.m_Return;
			WritePrivateProfileString("HQHOST","PrimaryHost",IntToStr(g_HQHost.PrimaryHost),g_WSXHStr+g_strYxhjCfg);
		}
		else
			WritePrivateProfileString("OTHERHOST","L2PrimaryHost",IntToStr(dlg.m_Return),g_WSXHStr+g_strYxhjCfg);	
		//
		if(OnLine)
		{
			g_pComm->Disconnect();
			g_nDSAutoConnectFlag = g_nAutoConnectFlag = AUTOCONNECT_RESET;
		}
		g_nTmpDirectConnectHq = TRUE;
		ConnectHqHost(TRUE);
	}
}

void CMainFrame::OnForum()
{
	CString tmpStr=(CString)"http://"+g_Url.ForumURL;
	OpenCusWebPage(this,tmpStr,FALSE);
}

#include "ShowInfoDlg.h"
void CMainFrame::OnShowInfo()
{
	if(!HQControl::IsValidProgram2())
		return;
	CShowInfoDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnToolbarDropDown(NMTOOLBAR* pnmtb, LRESULT *plr)
{
	CWnd *pWnd;
	UINT nID;
	int nWhich=0;
	switch (pnmtb->iItem)
	{
	case IDC_ZXG_OPR:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 28;
		break;
	case ID_TDXCHOICEGP:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 0;
		break;
	case ID_QJREPORT:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 1;
		break;
	case ID_OTHERJBM:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 2;
		break;
	case ID_GPFL:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 3;
		break;
	case ID_GPSORT:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 4;
		break;
	case ID_GPZHSORT:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 5;
		break;
	case ID_SHOWPERIOD:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 12;
		break;
	case ID_SHOWWINNUM:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 14;
		break;
	case ID_SHOWTQ:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 13;
		break;
	case ID_MANYDAYZST:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 18;
		break;

	case IDC_ZFJS_PAD1:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 29;
		break;
	case IDC_ZFJS_PAD2:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 30;
		break;
	case IDC_ZFJS_PAD3:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 31;
		break;
	case IDC_ZFJS_PAD4:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 32;
		break;
	case IDC_ZFJS_PAD5:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 33;
		break;
	case IDC_ZFJS_PAD6:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 34;
		break;
	case IDC_ZFJS_PAD7:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 35;
		break;
	case IDC_ZFJS_PAD8:
		pWnd = &m_wndToolBar;
		nID  = IDR_LONELY_MENU;
		nWhich = 36;
		break;
	default:
		return;
	}	
	CNewMenu menu;
	menu.LoadMenu(nID);
	CNewMenu* pPopup = (CNewMenu *)menu.GetSubMenu(nWhich);
	if(!pPopup) return;
	if(pnmtb->iItem == ID_OTHERJBM)
	{
		if(g_TdxCfg.JbmNum > 0)
			pPopup->ModifyMenu(0,MF_BYPOSITION,ID_JBM_MENU1,_F(g_TdxCfg.Jbmname[0]));
		for(int i = 1;i < g_TdxCfg.JbmNum;i++)
			pPopup->AppendMenu (MF_STRING,ID_JBM_MENU1+i,_F(g_TdxCfg.Jbmname[i]));
	}
	CRect rc;
	pWnd->SendMessage(TB_GETRECT, pnmtb->iItem, (LPARAM)&rc);
	pWnd->ClientToScreen(&rc);
	if(g_bShowBigToolBar)
		pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,rc.left,rc.bottom,this);
	else
		pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,rc.right+1,rc.top,this);
}

void CMainFrame::OnZXGOpr()
{

}

void CMainFrame::OnTdxChoiceGp()
{
}

void CMainFrame::OnGPFL()
{
}

void CMainFrame::OnGPSort()
{
}

void CMainFrame::OnGPZhSort()
{
}

void CMainFrame::SetAllButtons()
{
	if(!g_bShowBigToolBar) return;
	int nSep = 0,i;
	//清空所有Button
	m_wndToolBar.SetButtons(NULL,g_nBigToolBarBtn);
	for(i=0;i < g_nBigToolBarBtn;i++)
	{
		if(g_uBigToolBarBtn[i] == ID_SEPARATOR)
		{
			m_wndToolBar.SetButtonInfo(i,g_uBigToolBarBtn[i],TBBS_SEPARATOR,0);
			nSep+=1;
		}
		else
		{
			if(g_uBigToolBarBtn[i]==0)
				m_wndToolBar.SetButtonInfo(i,g_uBigToolBarBtn[i],TBBS_BUTTON|TBSTYLE_DROPDOWN,i-nSep);
			else
				m_wndToolBar.SetButtonInfo(i,g_uBigToolBarBtn[i],TBBS_BUTTON,i-nSep);
		}
	}
	LockWindowUpdate();
	CNewMDIFrameWnd::RecalcLayout();
	UnlockWindowUpdate();
}

void CMainFrame::OnHideShowButtonMsg(WPARAM wParam,LPARAM lParam)
{
	//如果是静态方式,则不进行切换
	if(m_bStaticToolBar) return;

	static int l_lastToolBarType=-1;
	int nTmp = (int)wParam;
	if(nTmp==l_lastToolBarType || nTmp==-1) return; //避免重复构造工具栏
	l_lastToolBarType = nTmp;
	int nSep = 0,i,j,nIndex;
	try //catch由于工具栏造成的错误
	{
		//清空所有Button
		m_wndToolBar.SetButtons(NULL,g_nFixedBtn+g_nDynBtn[nTmp]);
		for(i=0;i < g_nFixedBtn;i++)
		{
			if(g_uFixedBtn[i] == ID_SEPARATOR)
			{
				m_wndToolBar.SetButtonInfo(i,g_uFixedBtn[i],TBBS_SEPARATOR,0);
				nSep+=1;
			}
			else
			{
				if(g_uFixedBtn[i] == ID_TDXCHOICEGP)
					m_wndToolBar.SetButtonInfo(i,g_uFixedBtn[i],TBBS_BUTTON|TBSTYLE_DROPDOWN,i-nSep);
				else
					m_wndToolBar.SetButtonInfo(i,g_uFixedBtn[i],TBBS_BUTTON,i-nSep);
			}
		}
		//画动态Button
		for(i=0;i < g_nDynBtn[nTmp];i++)
		{
			nIndex = 0;
			for(j=0;j<g_nTotalDynBtn;j++)
			{
				if(g_uTotalDynBtn[j]==g_uDynBtn[nTmp][i])
				{	nIndex = j;	break;	}
			}
			if(g_uDynBtn[nTmp][i]==ID_QJREPORT||g_uDynBtn[nTmp][i]==ID_GPFL
			 ||g_uDynBtn[nTmp][i]==ID_GPSORT||g_uDynBtn[nTmp][i]==ID_GPZHSORT
			 ||g_uDynBtn[nTmp][i]==ID_SHOWPERIOD||g_uDynBtn[nTmp][i]==ID_SHOWTQ||g_uDynBtn[nTmp][i]==ID_SHOWWINNUM||g_uDynBtn[nTmp][i]==ID_MANYDAYZST
			 ||(g_uDynBtn[nTmp][i]==ID_OTHERJBM && g_TdxCfg.JbmNum>1))
				m_wndToolBar.SetButtonInfo(g_nFixedBtn+i,g_uDynBtn[nTmp][i],TBBS_BUTTON|TBSTYLE_DROPDOWN,g_nFixedBtn-nSep+nIndex);
			else if(g_uDynBtn[nTmp][i] == ID_SEPARATOR)
				m_wndToolBar.SetButtonInfo(g_nFixedBtn+i,g_uDynBtn[nTmp][i],TBBS_SEPARATOR,0);
			else
				m_wndToolBar.SetButtonInfo(g_nFixedBtn+i,g_uDynBtn[nTmp][i],TBBS_BUTTON,g_nFixedBtn-nSep+nIndex);
		}
		//锁定重画
		if(lParam!=1)
		{
			LockWindowUpdate();
			CNewMDIFrameWnd::RecalcLayout();
			UnlockWindowUpdate();
		}
	}
	catch(...)
	{
	}
}

#include "PluginDlg.h"
void CMainFrame::OnPlugin()
{
	if(m_pPluginDlg)
	{
		if (m_pPluginDlg->IsIconic()) 
			m_pPluginDlg->ShowWindow(SW_RESTORE); 
		return;
	}
	//创建有模式对话框，一定要用指针动态创建
	m_pPluginDlg = new CPluginDlg(this);
	m_pPluginDlg->Create(IDD_PLUGIN_DIALOG);
	m_pPluginDlg->ShowWindow (SW_SHOW);
}

void CMainFrame::OnUpdatePlugin(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_pSimChoiceDlg==NULL && m_pChoiceGPDlg==NULL && m_pCleChoiceDlg==NULL&&m_pChoiceBKDlg==NULL);
}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//如果在辅助区全屏的情况下,有来自菜单的消息或标题区内的,则退出全屏状态
	if(m_wndRestspace.IsFullSize() && LOWORD(wParam)>100) //为防止ESCAPE等消息,要求大于100
	{
		for(int i=0;i < g_VitalBtn.BtnNum;i++)
		{
			if(g_VitalBtn.BtnID[i] == LOWORD(wParam))
				break;
		}
		if(i < g_VitalBtn.BtnNum || HIWORD(wParam)==0)
		{
			if(OnLine)
				m_wndRestspace.SwitchToFullSize(FALSE);
		}
	}

	if(wParam==ID_WINDOW_TILE_HORZ||wParam==ID_WINDOW_TILE_VERT||wParam==ID_WINDOW_ARRANGE||wParam==ID_WINDOW_CASCADE)
		return TRUE;
	return CNewMDIFrameWnd::OnCommand(wParam, lParam);
}

void CMainFrame::SendMoreInfoReq(WPARAM wParam,LPARAM lParam)
{
	g_nMainID = MainIDPlus(m_nMainID,MAIN_WIN);
	g_nAssisID = MOREINFO;
//	g_nPriority = 2;
	g_pGetData->GetTimerMoreInfo();
//	g_nPriority = 1;		
}

//远程请求
void CMainFrame::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	if(theViewManager.bClosing)
		return;
	if(wParam==1)
	{
		if(g_AnsHeader3.AssisID==JUSTCURRHQ)
		{
			for(int j = 0;j < theViewManager.GetWindowNum();j++)
			{
				if( ((CView *)theViewManager.arViews[j])->IsKindOf(RUNTIME_CLASS(CComView)) )
					((CView *)theViewManager.arViews[j])->SendMessage(UM_GETDATA_ACK,wParam);
				if( ((CView *)theViewManager.arViews[j])->IsKindOf(RUNTIME_CLASS(CComView2)) )
					((CView *)theViewManager.arViews[j])->SendMessage(UM_GETDATA_ACK,wParam);
				//支持行情表的推送，注意，只有当前是GridView，才将消息发过去
				if( ((CGridView *)theViewManager.arViews[j])->IsKindOf(RUNTIME_CLASS(CGridView)) )
				{
					if(theViewManager.arViews[j]==theViewManager.pActiveView)
						theViewManager.pActiveView->SendMessage(UM_GETDATA_ACK,wParam);
				}
			}
			return;
		}
		switch(GetWinFromMainID(g_AnsHeader3.MainID))
		{
		case COMVIEW_WIN:	//ComView在版面级维护行情(ZST,TICK,FSB,FXT,MMP,HQ等)
			{
				for(int j = 0;j < theViewManager.GetWindowNum();j++)
				{
					if( ((CView *)theViewManager.arViews[j])->IsKindOf(RUNTIME_CLASS(CComView)) )
						((CView *)theViewManager.arViews[j])->SendMessage(UM_GETDATA_ACK,wParam);
				}
			}
			break;
		case COMVIEW2_WIN:
			{
				for(int j = 0;j < theViewManager.GetWindowNum();j++)
				{
					if( ((CView *)theViewManager.arViews[j])->IsKindOf(RUNTIME_CLASS(CComView2)) )
						((CView *)theViewManager.arViews[j])->SendMessage(UM_GETDATA_ACK,wParam);
				}
			}
			break;
		case	GRIDVIEW_WIN:
		case	HISVIEW_WIN:
		case	TXTVIEW_WIN:
		case	NEWSVIEW_WIN:
			if(LayoutFlag)	
				(theViewManager.pActiveView)->SendMessage(UM_GETDATA_ACK,wParam,lParam);
			break;
		case HISFX_WIN:
			if(g_pHisDlg)		
				g_pHisDlg->SendMessage(UM_GETDATA_ACK,wParam);
			break;
		case PREVIEWUNIT_WIN:
			if(g_pPreviewUnitDlg)
				g_pPreviewUnitDlg->SendMessage(UM_GETDATA_ACK,wParam);
			break;
		case ZHSORT_WIN:
			if(m_pZHSortDlg)
				m_pZHSortDlg->SendMessage(UM_GETDATA_ACK,wParam);
			break;
		case WIDEDATA_WIN:
			if(g_pWideDataDlg)  g_pWideDataDlg->SendMessage(UM_GETDATA_ACK,wParam);
			break;
		case BKDATA_WIN:
			if(g_pBKDataDlg)  g_pBKDataDlg->SendMessage(UM_GETDATA_ACK,wParam);
			break;
		case HKSEATQUERY_WIN:
			if(m_pHKSeatQueryDlg) m_pHKSeatQueryDlg->SendMessage(UM_GETDATA_ACK,wParam);
			break;
		case	MAIN_WIN:
			switch(g_AnsHeader3.AssisID)
			{
			case STATUSZSHQ:		//状态栏请求
				{
					MULIDX  MulIdx[3]={0};
					g_pGetData->DSGetMulCurrDataACK(MulIdx,3);
					RefreshStatusBar();
				}
				break;
			case	DS_HQALIVE_ASSISID:
				{
					g_pGetData->DSGetSrvInfoACK(&g_iDsSrvInfo);
					CTime ct = CTime::GetCurrentTime();
					if(ct.GetMinute()-g_nDSCheckHostMinute>=DS_DETECTCODE_MIN) //DS_DETECTCODE_MIN分钟探测有没有新的代码链
					{
						DSCheckSrvInfo();
						if(g_bDSNeedIdx||g_bDSNeedTimer)
						{
							g_pComm3->Disconnect();
							DSOnLine = FALSE;
							g_nDSAutoConnectFlag = AUTOCONNECT_RESET;
							DSConnectWhenNeedHq(TRUE);
						}
						g_nDSCheckHostMinute = ct.GetMinute();
					}
				}
				break;
			case	DS_LOGIN_ASSISID:
				{
					char errmsg[100];
					if(!g_pGetData->DSLoginACK(errmsg))
					{
						g_pComm3->Disconnect();	//断开连接
						DSOnLine = FALSE;
						g_nDSAutoConnectFlag = AUTOCONNECT_RESET;
					}
					else if(theViewManager.pActiveView)
					{
						if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)) || theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))
							theViewManager.pActiveView->PostMessage(UM_READANDPAINT,0,0);
					}
				}
				break;
			case	WARNGPHQ_ASSISID:
				{
					MULIDX MulHQData[GRIDHQ_MAXNUM]={0};
					short nGetWarnData = g_pGetData->DSGetMulCurrDataACK(MulHQData, GRIDHQ_MAXNUM);
					int i,xh;
					if(g_bTjWarnSwitch)
					{
						for(i=0; i<nGetWarnData; i++)
						{
							if(!LayoutFlag) break;
							xh = g_pStockIO->GetIndex(MulHQData[i].Code,MulHQData[i].Market);
							if(xh >= 0)	m_WarnManager.StartTJWatch((*g_pStockIO)[xh]);
						}
					}
				}
				break;
			case	REFRESHHQ_ASSISID:
				{
					MULIDX MulHQData[MULHQ_WANTNUM]={0};
					g_pGetData->DSGetMulCurrDataACK(MulHQData, MULHQ_WANTNUM);
				}
				break;
			}
			break;
		case ZSTICKDATA_WIN:
			if(g_pTickZstjDlg)  
				g_pTickZstjDlg->SendMessage(UM_GETDATA_ACK,wParam);
			break;
		}
		return;
	}
	switch(GetWinFromMainID(g_AnsHeader.MainID))
	{
	case MAIN_WIN:
		switch(g_AnsHeader.AssisID)
		{
		case STATUSZSHQ:		//状态栏请求
			g_pGetData->GetMultiCurrStockDataACK(g_ZSHQData,10);
			RefreshStatusBar();
			break;
		case MOREINFO:
			if(g_pGetData->GetTimerMoreInfoACK())
			{
				SendMessage(WM_COMMAND, ID_POPMOREDLG);
			}
			break;
		case WARNGPHQ_ASSISID:
			{
				struct CurrStockDataEx *pTmpHq = new struct CurrStockDataEx[GRIDHQ_MAXNUM];
				int nGetWarnData = g_pGetData->GetMultiCurrStockDataExACK(pTmpHq,GRIDHQ_MAXNUM);
				int i,xh;
				if(g_bAllWarnSwitch||g_nUnitRadarFlag>0)
				{
					for(i=0; i<nGetWarnData; i++)
					{
						if(!LayoutFlag) break;
						xh = g_pStockIO->GetIndex(pTmpHq[i].Code,pTmpHq[i].SetCode);
						if(xh >= 0)	m_WarnManager.StartAllWarn((*g_pStockIO)[xh]);
					}
				}
				if(g_bTjWarnSwitch)
				{
					for(i=0; i<nGetWarnData; i++)
					{
						if(!LayoutFlag) break;
						xh = g_pStockIO->GetIndex(pTmpHq[i].Code,pTmpHq[i].SetCode);
						if(xh >= 0)	m_WarnManager.StartTJWatch((*g_pStockIO)[xh]);
					}
				}
				delete pTmpHq;
			}
			break;
		case REFRESHHQ_ASSISID:
			{
				struct CurrStockData_Code *pTmpHq = new struct CurrStockData_Code[MULHQ_WANTNUM];
				g_pGetData->GetMultiCurrStockDataACK(pTmpHq,MULHQ_WANTNUM);
				delete pTmpHq;
			}
			break;
		case HQALIVE_ASSISID:	//行情存活，及时得到通告
			{
				DWORD nMaxTJResult=0,nADGUID=0;
				if(g_pGetData->HqAliveACK(nMaxTJResult,nADGUID))
				{
					g_nMainID = MainIDPlus(m_nMainID,MAIN_WIN);
					g_nAssisID = URGENT_ASSISID;
					g_pGetData->GetUrgentCfg(g_strLoginName);
				}
				else if(nADGUID!=g_AdHead.m_nADGUID)
				{
					g_nMainID = MainIDPlus(m_nMainID,MAIN_WIN);
					g_nAssisID = AD_ASSISID;
					g_pGetData->GetPathFile(g_strNeedAdName,0);
				}
				break;
			}
		case URGENT_ASSISID:	//及时得到通告
			g_pGetData->GetUrgentCfgACK();
			ShowUrgentDlg(URGENT_DURING,URGENT_NORMAL);
			break;
		case AD_ASSISID:		//及时得到广告
			{
				//先删除原来的广告文件
				unlink(g_WSXHStr+"LData\\cache\\"+g_strNeedAdName);
				//生成广告文件
				char *buffer = new char[ANSBUFFER_LEN];
				int tmpLen=g_pGetData->GetPathFileACK(buffer);
				if(tmpLen>0)
				{
					CFile otherfile;
					if(otherfile.Open(g_WSXHStr+"LData\\cache\\"+g_strNeedAdName,CFile::modeCreate|CFile::modeWrite))
					{
						otherfile.Write(buffer,tmpLen);
						otherfile.Close();
					}					
				}
				delete buffer;
				//重新读取广告文件并设置定时器
				GetNeedAdInfo();
				if(g_AdHead.m_nADNum>0)
				{
					KillTimer(12);
					SetTimer(12,5*1000,NULL);	//处理广告消息的定时器,尽快弹出
				}
			}
			break;
		case SZ_DWARN_ASSISID:
		case SH_DWARN_ASSISID:
			{
				int nGetNum = g_pGetData->GetG2DeepWarnACK(NULL,0);
				if(nGetNum<=0) 
					break;
				if(nGetNum+g_DWarnInf.nWarnNum > g_DWarnInf.nWarnSpace)
				{
					LPWARNATOM tmpWarn = new WARNATOM[nGetNum+g_DWarnInf.nWarnNum+DWARN_SPACE];
					LPWARNATOM SwapWarn= g_DWarnInf.pWarnAtom;
					if(tmpWarn == NULL) 
						break;
					memset(tmpWarn,0,(nGetNum+g_DWarnInf.nWarnNum+DWARN_SPACE)*sizeof(WARNATOM));
					if(g_DWarnInf.nWarnSpace>0 && g_DWarnInf.pWarnAtom!=NULL)
						memcpy(tmpWarn,g_DWarnInf.pWarnAtom,g_DWarnInf.nWarnSpace*sizeof(WARNATOM));
					g_DWarnInf.pWarnAtom = tmpWarn;
					g_DWarnInf.nWarnSpace= nGetNum+g_DWarnInf.nWarnNum+DWARN_SPACE;
					TDELA(SwapWarn);
				}
				int nRealGet = g_pGetData->GetG2DeepWarnACK(g_DWarnInf.pWarnAtom+g_DWarnInf.nWarnNum,nGetNum);
				if(nRealGet<=0) break;
				g_DWarnInf.nWarnNum += nRealGet;
				if(g_AnsHeader.AssisID==SZ_DWARN_ASSISID)
					g_DWarnInf.nSZIDPos = g_DWarnInf.pWarnAtom[g_DWarnInf.nWarnNum-1].WarnID+1;
				else
					g_DWarnInf.nSHIDPos = g_DWarnInf.pWarnAtom[g_DWarnInf.nWarnNum-1].WarnID+1;
				//
				if(m_pDeepWarnDlg)
					m_pDeepWarnDlg->PostMessage(UPDATE_WARNGP);
				if(g_nUnitDeepWarnFlag>0)
				{
					for(int j = 0;j < theViewManager.GetWindowNum();j++)
					{
						if( ((CView *)theViewManager.arViews[j])->IsKindOf(RUNTIME_CLASS(CComView)) || ((CView *)theViewManager.arViews[j])->IsKindOf(RUNTIME_CLASS(CComView2)) )
							((CView *)theViewManager.arViews[j])->PostMessage(UPDATE_WARNGP);
					}
				}
			}
			break;
		case IMGSTATHQ_ASSISID:
		case TCKSTATHQ_ASSISID:
			{
				int nNum = g_pGetData->GetStatDataAck(g_AnsHeader.AssisID==IMGSTATHQ_ASSISID,HQStat_lpnStkIndex,MAXSTAT_MAXNUM);
				for(int i = 0;i < nNum;i++)
				{
					for(int j = 0;j < theViewManager.GetWindowNum();j++)
					{
						if( ((CView *)theViewManager.arViews[j])->IsKindOf(RUNTIME_CLASS(CComView)) || ((CView *)theViewManager.arViews[j])->IsKindOf(RUNTIME_CLASS(CComView2)) )
							((CView *)theViewManager.arViews[j])->PostMessage(ASYN_STATHQ_MESSAGE,HQStat_lpnStkIndex[i]);
					}
				}					
			}
			break;
		default:				//其它分到各个视图
			break;
		}
		break;
	case COMVIEW_WIN:	//ComView在版面级维护行情(ZST,TICK,FSB,FXT,MMP,HQ等)
		{
			for(int j = 0;j < theViewManager.GetWindowNum();j++)
			{
				if( ((CView *)theViewManager.arViews[j])->IsKindOf(RUNTIME_CLASS(CComView)) )
					((CView *)theViewManager.arViews[j])->SendMessage(UM_GETDATA_ACK,wParam);
			}
		}
		break;
	case COMVIEW2_WIN:	//ComView在版面级维护行情(ZST,TICK,FSB,FXT,MMP,HQ等)
		{
			for(int j = 0;j < theViewManager.GetWindowNum();j++)
			{
				if( ((CView *)theViewManager.arViews[j])->IsKindOf(RUNTIME_CLASS(CComView2)) )
					((CView *)theViewManager.arViews[j])->SendMessage(UM_GETDATA_ACK,wParam);
			}
		}
		break;
	case GRIDVIEW_WIN:
	case HISVIEW_WIN:
	case TXTVIEW_WIN:
	case NEWSVIEW_WIN:
		if(LayoutFlag)	(theViewManager.pActiveView)->SendMessage(UM_GETDATA_ACK,wParam);
		break;
	case PREVIEWUNIT_WIN:
		if(g_pPreviewUnitDlg)
			g_pPreviewUnitDlg->SendMessage(UM_GETDATA_ACK,wParam);
		break;
	case ZHSORT_WIN:
		if(m_pZHSortDlg)	m_pZHSortDlg->SendMessage(UM_GETDATA_ACK,wParam);
		break;
	case HISFX_WIN:
		if(g_pHisDlg)		g_pHisDlg->SendMessage(UM_GETDATA_ACK,wParam);
		break;
	case WIDEDATA_WIN:
		if(g_pWideDataDlg)  g_pWideDataDlg->SendMessage(UM_GETDATA_ACK,wParam);
		break;
	case BKDATA_WIN:
		if(g_pBKDataDlg)	g_pBKDataDlg->SendMessage(UM_GETDATA_ACK,wParam);
		break;
	case DOWNLOAD_WIN:
		if(g_pDownLoadDlg)  g_pDownLoadDlg->SendMessage(UM_GETDATA_ACK,wParam);
		break;
	case DATADIG_WIN:
		if(m_pDataDigDlg)  m_pDataDigDlg->SendMessage(UM_GETDATA_ACK,wParam);
		break;
	case SYNCDATA_WIN:
		if(g_pDataSyncDlg) g_pDataSyncDlg->SendMessage(UM_GETDATA_ACK,wParam);
		break;
	case ZSTICKDATA_WIN:
		if(g_pTickZstjDlg)  g_pTickZstjDlg->SendMessage(UM_GETDATA_ACK,wParam);
		break;
	case INFOUPLOAD_WIN:
		break;
	default:
		break;
	}
}

void CMainFrame::OnCommDisConnect(WPARAM wParam,LPARAM lParam)
{
	OnLine = FALSE;
	if(DSOnLine && g_pComm3)
		g_pComm3->Disconnect();
	DSOnLine = FALSE;	
	g_nDSAutoConnectFlag = AUTOCONNECT_RESET;
	RefreshStatusBar();
}

void CMainFrame::OnHqConnectTimerMessage(WPARAM wParam,LPARAM lParam)
{
	SetTimer(11,5*1000*60,NULL); //早上初始化只需等5分钟,就直接进入
}

void CMainFrame::ConnectHqHost(BOOL bDisableNLB, BOOL bReInit)
{
	if(OnLine)
	{
		if(!bReInit)
			return;
		g_pComm->Disconnect();
	}
	if(DSOnLine)
	{
		if(!bReInit)
			return;
		g_pComm3->Disconnect();
	}
	//容错,LogoDlg的ContinueModal错误
	BOOL bOK=FALSE;
	try
	{
		g_pConnectWnd = new CLogoDlg;
		((CLogoDlg *)g_pConnectWnd)->m_bHasOffMode = FALSE;
		((CLogoDlg *)g_pConnectWnd)->m_bDisableNLB = bDisableNLB;
		if(bReInit)
			((CLogoDlg *)g_pConnectWnd)->m_bDirectConnect = TRUE;
		CWnd *tmpMainWnd = g_pMainWin;	//保存原来的接收窗口
		g_pMainWin = g_pConnectWnd;
		if(((CDialog *)g_pConnectWnd)->DoModal() == IDOK)
		{
			bOK = TRUE;
			OnLine = TRUE;
		}
		g_pMainWin = tmpMainWnd;
		TDEL(g_pConnectWnd);
	}
	catch(...)
	{
		TDX_MessageBox(m_hWnd,"未知错误，请重新连接!",MB_OK|MB_ICONEXCLAMATION);
		OnLine = FALSE;
		return;
	}
	if(OnLine && bOK)
	{
		InterlockedExchange(&g_nAutoCalcLock, 1);
		if(m_hAutoCalcBkThread!=NULL&&WaitForSingleObject(m_hAutoCalcBkThread,1000)!=WAIT_OBJECT_0)
			TerminateThread(m_hAutoCalcBkThread,0);

	//	CloseallAndKeepSpec(); //重连之前，关掉所有窗口，除行情窗口
		TDEL(g_pStockIO);
		//在g_pStockIO->TcpipInit之前load,以便于加入到代码链
		TDEL(g_pLcIdxMng);
		g_pLcIdxMng	= new LcIdxMng;
		if(!g_pLcIdxMng)
		{
			TDX_MessageBox(m_hWnd,"内存申请出错!",MB_OK|MB_ICONERROR);
			g_bSpecClose = TRUE;
			PostMessage(WM_CLOSE); //直接退出
			return;
		}
		g_pLcIdxMng->LoadAll();
		//
		TDEL(g_pExdayMng);
		g_pExdayMng = new ExdayMng;
		//构造全局实例
		g_pStockIO = (StockDataIo * ) new StockDataIo;
		if (!g_pStockIO->TcpipInit(OffLineMode))
		{
			TDX_MessageBox(m_hWnd,"数据初始化错误，程序退出!",MB_OK|MB_ICONERROR);
			g_bSpecClose = TRUE;
			PostMessage(WM_CLOSE); //直接退出
			return;
		}
		if(DSOnLine)
			g_pStockIO->AfterDSLoginSuccess();
		try
		{
			g_nOverlapNum = 0; //因为DataIO重新初始化了，故g_nOverlapNum要复位
			////
			memset(g_nMineNum,0,MAXGPNUM*sizeof(short));	//将雷的数目清空
			m_nHqMinePos = 0;
			m_nHqMinePrenum = 80;
			//将一些全局变量重置
			TxtCurGPCode = "";
			TxtCurSetCode = -1;
			ComCurGPCode = "600839";
			ComCurSetCode = SH;
			g_nWarnNum = 0;
			g_nTJWarnNum =0;
			//重新得到柔性平台设置信息
			GetFYInfo();
			g_bFYDataAlwaysSync=FALSE;
			g_DDEDataNeedLoad=TRUE;
			g_lRefreshHQSeconds = 0;
			g_lRefreshStatHQSeconds = 0;
			//进行其它全局函数的清理工作
			TDELA(g_DWarnInf.pWarnAtom);
			memset(&g_DWarnInf,0,sizeof(DWARNINF));
			g_DWarnInf.nSZIDPos=-1;
			g_DWarnInf.nSHIDPos=-1;
			CWnd *pFirstWnd = CWnd::FromHandle (FirstWindow);
			if(pFirstWnd)	
			{
				CView *tmpView = ((CFrameWnd *)pFirstWnd)->GetActiveView();
				if(tmpView)
					tmpView->SendMessage(UM_GRIDREINIT,0,0);
			}
			//
			RefreshStatusBar();
			//判断是否是实时扩展行情
			TestAutoDS();
			//连接扩展行情主站
			if(g_bHasDS && m_bAutoDS)
				PostMessage(UM_AUTOCONNECTDS,0,0);
			//重开常与用户管理服务器的常连接线程
			LayoutFlag = FALSE;
			if(UserThreadHandle)
				WaitForSingleObject(UserThreadHandle,5000);

			LayoutFlag = TRUE;
		}
		catch(...)
		{
		}
	}
}

void CMainFrame::OnConnect()
{
	ConnectHqHost();
}

void CMainFrame::OnBGConnect()
{
	//先去掉前面的模态对话框
	CancelModalWindows();
	PostMessage(WM_COMMAND,ID_CONNECT);
}

void CMainFrame::OnReInitConnect()
{
	ConnectHqHost(FALSE, TRUE);
}

void CMainFrame::OnUpdateConnect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!OnLine && g_nAutoConnectFlag!=AUTOCONNECT_PENDING);
}

void CMainFrame::OnDisconnect()
{
	if(OnLine)
	{
		g_pComm->Disconnect();
		g_nDSAutoConnectFlag = g_nAutoConnectFlag = AUTOCONNECT_RESET;
		if(g_bUserHostEverConnect && !g_bJRSConnected) 
			return;
		TDX_MessageBox(m_hWnd,"行情连接被断开!",MB_ICONINFORMATION|MB_OK);
	}
}

void CMainFrame::OnUpdateDisconnect(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(OnLine);
}

void CMainFrame::OnSilentDisconnect()
{
	if(OnLine)
	{
		g_pComm->Disconnect();
		g_nDSAutoConnectFlag = g_nAutoConnectFlag = AUTOCONNECT_RESET;
	}
}

void CMainFrame::OnConnectCfg()
{
	CConnectCfgDlg dlg;
	if(g_OEM.bEmbedWTVersion)
		dlg.m_bHasWTSet=TRUE;
	dlg.DoModal();
}

void CMainFrame::OnDownLoad()
{
	if(OnLine)
	{
		g_pDownLoadDlg = new CDownloadDlg;
		((CDownloadDlg *)g_pDownLoadDlg)->m_bModal = FALSE;
		((CDownloadDlg *)g_pDownLoadDlg)->m_bDown_AtOnce = FALSE;
		((CDownloadDlg *)g_pDownLoadDlg)->Create(IDD_DOWNLOAD_DIALOG);
		g_pDownLoadDlg->ShowWindow (SW_SHOW);
		g_pDownLoadDlg->CenterWindow();
	}
	else
	{
		TDX_MessageBox(m_hWnd,"请先连接到行情主站!",MB_ICONINFORMATION|MB_OK);
		return;
	}
}

void CMainFrame::OnUpdateDownLoad(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(g_bCanDown);
}

void CMainFrame::OnNodeTool()
{
	if(IsBig5Version()) return;
	char tmpPath[MAX_PATH];
	strcpy(tmpPath,HomePath);
	strcat(tmpPath,"NodeTool.exe");
	ExecExtern(tmpPath,m_hWnd);
}

void CMainFrame::OnUpdateNodeTool(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(g_bCanDown);
}

void CMainFrame::OnDayWriteDisk()
{
	if(!g_bCanDown) return; //如果是本地数据,不允许写盘
	g_bDay_WriteDisk = !g_bDay_WriteDisk;
}

void CMainFrame::OnUpdateDayWriteDisk(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( g_bCanDown );
	pCmdUI->SetCheck(g_bDay_WriteDisk);
}

//关于主菜单有时失控问题的修改
void CMainFrame::CancelAllCaptionStatus()
{
#ifdef OEM_NEWJY
	if(m_wndCaptionBar.m_hmenu)
		::PostMessage(m_wndCaptionBar.m_hWnd,UM_EXITMENULOOP,0,0);
#endif
}

#include "WideDataDlg.h"
void CMainFrame::OnWideDataHQ()
{
	if(OnLine)
	{
		CancelAllCaptionStatus();

		g_pWideDataDlg = new CWideDataDlg;
		((CWideDataDlg *)g_pWideDataDlg)->m_nType = WIDEDATA_HQ;
		((CWideDataDlg *)g_pWideDataDlg)->m_nTotalRow = min(g_nSimhq2Stk,MAXGPNUM);
		memcpy(((CWideDataDlg *)g_pWideDataDlg)->lpnUsedStkIndex,g_Simhq2StkIndex,sizeof(short)*min(g_nSimhq2Stk,MAXGPNUM));
		((CWideDataDlg *)g_pWideDataDlg)->DoModal();
		TDEL(g_pWideDataDlg);
	}
}

void CMainFrame::OnWideDataSort(WPARAM wParam,LPARAM lParam)
{
	LinkedSetInfo *pLsetInfo = (LinkedSetInfo*)wParam;
	if(OnLine&&pLsetInfo->bUsed)
	{
		CancelAllCaptionStatus();

		g_pWideDataDlg = new CWideDataDlg;
		((CWideDataDlg *)g_pWideDataDlg)->m_nType = WIDEDATA_SORT;
		((CWideDataDlg *)g_pWideDataDlg)->m_nTotalRow = pLsetInfo->nValidRow;
		((CWideDataDlg *)g_pWideDataDlg)->m_pLSetInfo = pLsetInfo; 
		((CWideDataDlg *)g_pWideDataDlg)->DoModal();
		TDEL(g_pWideDataDlg);
	}
}

void CMainFrame::OnWideDataStatHQ(WPARAM wParam,LPARAM lParam)
{
	if(OnLine)
	{
		CancelAllCaptionStatus();
		
		g_pWideDataDlg = new CWideDataDlg;
		((CWideDataDlg *)g_pWideDataDlg)->m_nType = (wParam==0)?WIDEDATA_IMG_STAT:WIDEDATA_TCK_STAT;
		((CWideDataDlg *)g_pWideDataDlg)->m_nTotalRow = min(g_nSimhq2Stk,MAXGPNUM);
		memcpy(((CWideDataDlg *)g_pWideDataDlg)->lpnUsedStkIndex,g_Simhq2StkIndex,sizeof(short)*min(g_nSimhq2Stk,MAXGPNUM));
		((CWideDataDlg *)g_pWideDataDlg)->DoModal();
		TDEL(g_pWideDataDlg);
	}
}

#include "DataSyncDlg.h"
void CMainFrame::OnSyncData()
{
	if(OnLine)
	{
		try
		{
			g_pDataSyncDlg = new CDataSyncDlg;
			((CDataSyncDlg *)g_pDataSyncDlg)->m_nType = g_nDataSyncType;
			((CDataSyncDlg *)g_pDataSyncDlg)->m_strDataSyncFile = g_strDataSyncFile;
			((CDataSyncDlg *)g_pDataSyncDlg)->DoModal();
			TDEL(g_pDataSyncDlg);
		}
		catch(...)
		{
		}
	}
}

void CMainFrame::OnStarGraph()
{
	OnCommCommand(CMD_ADVHQ,0);
}

void CMainFrame::OnMulUnit()
{
	if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)))
	{
		theViewManager.pActiveView->SendMessage(WM_COMMAND,ID_MULUNIT);
		return;
	}
	int WhichGPType = 6,nBlockFlag = -1;
	if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CGridView)))
		((CGridView *)theViewManager.pActiveView)->GetCurrentDomain(WhichGPType,nBlockFlag);
	g_nMul_WhichGPType = WhichGPType;
	g_nMul_nBlockFlag = nBlockFlag;
	g_nMul_CurGP = GetSYSCurrentGPIndex();
	OnCommCommand(CMD_COM,900);	
}

void CMainFrame::OnMSSR()
{
	OnCommCommand(CMD_COM,901);
}

void CMainFrame::OnBigVol()
{
	OnCommCommand(CMD_COM,902);
}

void CMainFrame::OnOpenWebWin(WPARAM wParam,LPARAM lParam)
{
	if(m_pWebWinDlg)
	{
		if (m_pWebWinDlg->IsIconic()) 
			m_pWebWinDlg->ShowWindow(SW_RESTORE); 
	}
	else
	{
		int iNo = GetSYSCurrentGPIndex();
		if(iNo<0) return;
		//创建有模式对话框，一定要用指针动态创建
		m_pWebWinDlg = new CShowURL(this,TRUE);
		m_pWebWinDlg->m_strTitle = _F("深度查询");
		m_pWebWinDlg->SetInfo(0,0,g_NewJyCfg.RelativeInfo,iNo);
		m_pWebWinDlg->Create(IDD_SHOWURL);
		m_pWebWinDlg->ShowWindow(SW_SHOW);
	}
	if(!(theViewManager.pActiveView)->IsKindOf(RUNTIME_CLASS(CComView)) && wParam==1)
		SwitchToView("Com");
	SetForegroundWindow();
}

void CMainFrame::OnGotojbzl()
{
	CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
	if(tempWnd)
		tempWnd->SendMessage(WM_COMMAND,ID_GOTOJBZL);
}

void CMainFrame::OnBuyDS()
{
	ShellExecute(NULL, _T("open"), BUYDS_WEBPAGE, NULL, NULL, SW_SHOW);	
}

void CMainFrame::OnDSMarket(UINT nID)
{
	g_bOldGridDomain=FALSE;
	CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
	if(tempWnd)
		tempWnd->SendMessage(WM_COMMAND,nID);
	g_bOldGridDomain=TRUE;
}

void CMainFrame::OnFYGrid(UINT nID)
{
	g_bOldGridDomain=FALSE;
	CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
	if(tempWnd)
		tempWnd->SendMessage(WM_COMMAND,nID);
	g_bOldGridDomain=TRUE;
}

void CMainFrame::OnWebInfo()
{
	if(!g_bHasWebInfo) 
	{
		if(strlen(g_Url.WebInfoURL)>0)
			ShellExecute(NULL, _T("open"), g_Url.WebInfoURL, NULL, NULL, SW_SHOW);
		return;
	}

	int i=m_wndWorkspace.GetCurWnd();
	if(i < 0) return;
	if(m_wndWorkspace.IsWindowVisible() && g_nTreePos[WEB_TREETYPE]==i)
	{
		ShowControlBar(&m_wndWorkspace,FALSE,FALSE);
		SwitchToFirstView();
	}
	else
		SwitchToWorkSpace(WEB_TREETYPE);		
}

void CMainFrame::OnUpdateWebInfo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(g_bHasWebInfo||strlen(g_Url.WebInfoURL)>0);
}

void CMainFrame::OnAdditonZXURL()
{
	if(strlen(g_Url.AddZxURL)>0)
		ShellExecute(NULL, _T("open"), g_Url.AddZxURL, NULL, NULL, SW_SHOW);
	return;
}

void CMainFrame::OnVideo()
{
	if(g_GC.Video.nVideoFlag==1)	//直接网页格式
	{
		ShellExecute(NULL, _T("open"), g_GC.Video.strVideoURL, NULL, NULL, SW_SHOW);
	}
	else if(g_GC.Video.nVideoFlag==2)	//内嵌窗口格式
	{
		if(m_pVideoDlg)
		{
			if (m_pVideoDlg->IsIconic()) 
				m_pVideoDlg->ShowWindow(SW_RESTORE); 
			return;
		}
		//创建有模式对话框，一定要用指针动态创建
		m_pVideoDlg = new CShowURL(this);
		m_pVideoDlg->m_strTitle = _F("财经视频");
		m_pVideoDlg->SetInfo(g_GC.Video.nWndcx,g_GC.Video.nWndcy,g_GC.Video.strVideoURL);
		m_pVideoDlg->Create(IDD_SHOWURL);
		m_pVideoDlg->ShowWindow (SW_SHOW);
	}
	else if(g_GC.Video.nVideoFlag==3)	//KooPlayer P2P播放格式
	{
		char tmpPath[MAX_PATH];
		sprintf(tmpPath,"%sPlayer.exe",HomePath);
		ExecExtern(tmpPath,m_hWnd);
	}
}

#include "NewUrgentDlg.h"
void CMainFrame::OnHelpFinder()
{
	GoToHelp();
//	CNewUrgentDlg dlg;
//	dlg.DoModal();
}

//设置状态栏的高度并重新设置滚动栏
void CMainFrame::RecalcLayoutStatusRoll()
{
	int nHeight=17;
	if(g_GH.bBigStatusBar)	nHeight=20;
	if(g_GH.Roll.bShow)
		nHeight+=nGetRollBarHeight();
	m_wndStatusBar.GetStatusBarCtrl().SetMinHeight(nHeight);
	RecalcLayout(TRUE);
	m_wndStatusBar.PostMessage(UM_ROLLINFOCFG_MSG,0,0);
}

void CMainFrame::OnCloseRollMsg(WPARAM wParam,LPARAM lParam)
{
	if(!g_GH.Roll.bFloat)
	{
		g_GH.Roll.bRollOpen[0]=FALSE;
		GetRollShowFromOpenStatus();
		RecalcLayoutStatusRoll();
	}
}

void CMainFrame::OnCloseSlowRollMsg(WPARAM wParam,LPARAM lParam)
{
	if(!g_GH.Roll.bFloat)
	{
		g_GH.Roll.bRollOpen[1]=FALSE;
		GetRollShowFromOpenStatus();
		RecalcLayoutStatusRoll();
	}
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{
	 if(nID==SC_MINIMIZE) //使用我自己的最小化函数
	 {
		OnMinimize();
		return;
	 }
	 CNewMDIFrameWnd::OnSysCommand(nID, lParam);
}

int g_Hookcmd = SW_SHOW;
HWND g_HidedWnd[1000];
BOOL g_nHideWnd = 0;
BOOL CALLBACK EnumWindowProcAndShowHide(HWND hwnd,LPARAM lParam)
{
	if(hwnd == 0) 
		return FALSE;
	HWND tmphWnd = NULL;
	tmphWnd = GetParent(hwnd);
	while(tmphWnd)
	{
		if(g_pMainWin->m_hWnd == tmphWnd)
		{
			if(g_Hookcmd==SW_SHOW) //如果是隐藏对话框
			{
				for(int i=0;i < g_nHideWnd;i++)
					if(g_HidedWnd[i]==hwnd) break;
				if(i >= g_nHideWnd)
					ShowWindow(hwnd,g_Hookcmd);
			}
			else
			{
				if(IsWindowVisible(hwnd))
					ShowWindow(hwnd,g_Hookcmd);
				else if(g_nHideWnd < 1000)//如果本来就是隐藏的,不打扰它
					g_HidedWnd[g_nHideWnd++] = hwnd;
			}
			break;
		}
		tmphWnd = GetParent(tmphWnd);
	}
	return TRUE;
}

LRESULT CMainFrame::OnHotKey(WPARAM wParam,LPARAM lParam)
{
	static BOOL m_Hideflag = TRUE;
	static BOOL MsgBoxflag = FALSE;
	static time_t   l_LastT = 0;

	if(wParam==1002)				//CTL_TAB切换菜单热键
	{
		if(m_pTabViewMenu)
		{
			keybd_event(VK_DOWN,0,KEYEVENTF_EXTENDEDKEY,0); 
			keybd_event(VK_DOWN,0,KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP,0);
		}
	}

	if(MsgBoxflag) return 0;

	if(wParam==1001)
	{
		if(!m_Hideflag)
			g_Hookcmd=SW_SHOW;
		else
		{
			if(!g_hideinfo)
			{
				MsgBoxflag = TRUE;
				CString str;
				str.Format("此操作将隐藏 [%s]  \n再按同样的键恢复\n\n确认隐藏吗?",g_strPreName);
				if(TDX_MessageBox(m_hWnd,str,MB_OKCANCEL|MB_ICONQUESTION)==IDCANCEL)
				{
					MsgBoxflag = FALSE;
					return 0;
				}
				MsgBoxflag = FALSE;
			}
			g_Hookcmd=SW_HIDE;
			g_nHideWnd=0;
		}
		ShowWindow(g_Hookcmd);
		EnumWindows((WNDENUMPROC)EnumWindowProcAndShowHide,0);
		if(g_Hookcmd==SW_SHOW)
			SetForegroundWindow();
		m_Hideflag=!m_Hideflag;
	}
	return 0;
}

HMENU CMainFrame::NewMenu(UINT MenuID)
{
	m_Menu.LoadMenu(MenuID);
	return(m_Menu.Detach());
}

void AppendCustomPadMenu(CNewMenu* pPadMenu)
{
	int i;
	CString tmpMenuStr;
	for (i=0;i<g_vDynaCmdTab.size();i++)
	{
		tmpMenuStr = g_vDynaCmdTab[i].ChName;
		if(strcmp(g_DefCusPadName, tmpMenuStr)==0)
			tmpMenuStr+="(初始)";
		pPadMenu->AppendMenu(MF_STRING, IDC_CUSTOMPAD1+i, _F(tmpMenuStr));	
		if( i != 0 && i % 28 == 0 )
			SetMenuBreak(i,pPadMenu);			
	}
}

void AppendDSMenu(CNewMenu* pDSMenu)
{
	for(int iPos = pDSMenu->GetMenuItemCount()-1; iPos > 1; iPos--)
		pDSMenu->DeleteMenu(iPos, MF_BYPOSITION);
	//如果是免费港股，则加入购买港股的链接
	if(g_DS.HasHKMarket && !g_GC.bIsSSDS && !g_OEM.bNoAdvert)
		pDSMenu->AppendMenu(MF_STRING,IDM_BUYDS,_F("购买实时港股"));
	if(g_LocalGuiMarketNum==0)
		pDSMenu->AppendMenu(MF_SEPARATOR);
	CString tmpMenuStr;
	int k,kk;
	for(k=0;k<g_GuiMarketNum;k++)
	{
		BOOL bFirst=TRUE;
		for(kk=0;kk < MAX_MKT_NUM;kk++)
		{
			int nDSMarketID = g_GuiMarketSet[k].MarketSet[kk];
			if(nDSMarketID==0) continue;
			if(bFirst && g_LocalGuiMarketNum>0)
				pDSMenu->AppendMenu(MF_SEPARATOR);
			bFirst=FALSE;
			tmpMenuStr.Format("%s\t.%d",g_GuiMarketSet[k].MarketName[kk],600+nDSMarketID);
			pDSMenu->AppendMenu(MF_STRING, IDM_DS1+nDSMarketID,_F(tmpMenuStr));
		}					
	}
	if(g_DS.HasHKMarket)
	{
		if(g_GuiMarketNum>0)
			pDSMenu->AppendMenu(MF_SEPARATOR);
		if(g_GC.bIsSSDS)
			pDSMenu->AppendMenu(MF_STRING,ID_HKSEATQUERY,_F("经纪席位追踪"));
		pDSMenu->AppendMenu(MF_STRING,ID_HKJYTIME,_F("港交所休市日"));
	}
}

void CMainFrame::OnInitMenuPopup(CNewMenu *pPopupMenu,UINT nIndex,BOOL bSysMenu)
{
	CNewMDIFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	///////////////////////////////////////////////
	if(!bSysMenu && pPopupMenu)
	{
		int nPos = pPopupMenu->GetMenuItemCount();
		if(g_nCusWebPageNum>0 && (pPopupMenu->GetMenuItemID(nPos-1)==ID_DELFROMBLOCK || pPopupMenu->GetMenuItemID(nPos-1)==ID_GPCOOLINFO))
		{
			pPopupMenu->AppendMenu(MF_SEPARATOR);
			static CNewMenu WebPopupMenu;
			WebPopupMenu.DestroyMenu();
			WebPopupMenu.CreatePopupMenu();
			for(int i=0;i<g_nCusWebPageNum;i++)
				WebPopupMenu.AppendMenu(MF_STRING, IDC_CUSWEBPAGE_BASE+i, _F((LPCSTR)g_CusWebPage[i].PageName));
			pPopupMenu->AppendMenu(MF_POPUP,(UINT)(WebPopupMenu.m_hMenu),_F("个股关联网站"));
		}
	}
	//////////////////////////////////////////////////////////////////////////
#if defined(OEM_NEWJY)
	return;
#endif
	CNewMenu *pPadMenu = NULL, *pTCmdMenu = NULL;
	CNewMenu *pTopMenu = (CNewMenu*)GetMenu();
	if(!pTopMenu) return;

	//定制版面的菜单
	if(!g_bHasCaption)
	{
		CNewMenu* pGnMenu = (CNewMenu*)pTopMenu->GetSubMenu(1);
		if(!pGnMenu) return;
		pPadMenu = (CNewMenu*)pGnMenu->GetSubMenu(8);
	}
	else
	{
		int nPos=11;
	#ifdef OEM_STAR_HM
		nPos=8;
	#endif
		if(g_bHasDS) nPos+=1;
		if(g_nWebPageNum>0 && g_nWebPageNum2>0)
			pPadMenu = (CNewMenu*)pTopMenu->GetSubMenu(nPos);
		else if(g_nWebPageNum>0)
			pPadMenu = (CNewMenu*)pTopMenu->GetSubMenu(nPos-1);
		else
			pPadMenu =(CNewMenu*)pTopMenu->GetSubMenu(nPos-2);
	}
	if(!pPadMenu) return;
	if(FALSE&&pPopupMenu == pPadMenu)
	{
		int iPos,iFix=8;
		for (iPos = pPadMenu->GetMenuItemCount()-1; iPos > iFix; iPos--)
			pPadMenu->DeleteMenu(iPos, MF_BYPOSITION);
		AppendCustomPadMenu(pPadMenu);
	}

	if(g_bHasUserHost || g_bHasTICQ)
	{
		CNewMenu* pSysMenu = (CNewMenu*)pTopMenu->GetSubMenu(0);
		if(!pSysMenu) return;
		if(pPopupMenu==pSysMenu)
		{
			pPopupMenu->RemoveMenu(IDC_WEBUSER_1001,MF_BYCOMMAND);
			pPopupMenu->RemoveMenu(IDC_WEBUSER_1002,MF_BYCOMMAND);
			pPopupMenu->RemoveMenu(IDC_WEBUSER_1003,MF_BYCOMMAND);
			if(g_bUserHostEverConnect && OnLine)
			{
			#ifdef PERSONAL
				pPopupMenu->RemoveMenu(ID_WRITEURGENT,MF_BYCOMMAND);
				if(g_MngRight==5)
					pPopupMenu->InsertMenu(ID_CORP_WEB,MF_BYCOMMAND,ID_WRITEURGENT,_F("发布通告"));	
			#endif
				pPopupMenu->RemoveMenu(ID_SEND_FILE,MF_BYCOMMAND);
				pPopupMenu->RemoveMenu(ID_REC_FILE,MF_BYCOMMAND);
				pPopupMenu->InsertMenu(ID_CORP_WEB,MF_BYCOMMAND,ID_SEND_FILE,_F("上传个性数据"));	
				pPopupMenu->InsertMenu(ID_CORP_WEB,MF_BYCOMMAND,ID_REC_FILE,_F("下载个性数据"));	
			}		
			if(g_bHasUserHost)
			{
				if(!g_bIsGuest && g_LoginInfo.bDialogRegister && g_nLastLoginType!=JY_LOGIN) //如果是交易登录,不能修改密码
				{
					pPopupMenu->InsertMenu(ID_CORP_WEB, MF_BYCOMMAND,IDC_WEBUSER_1002, _F("修改密码"));	
					if(g_LoginInfo.bCanDelMySelf)
						pPopupMenu->InsertMenu(ID_CORP_WEB, MF_BYCOMMAND,IDC_WEBUSER_1003, _F("删除用户"));	
				}
			}
			if(g_bHasTICQ)
				pPopupMenu->InsertMenu(ID_CORP_WEB, MF_BYCOMMAND,IDC_WEBUSER_1001, _F("TQ"));	
		}
	}
	if(FALSE&&g_bHasDS)	//如果有三方行情,将重新排列报价菜单
	{
		CNewMenu* pDSMenu = (CNewMenu*) pTopMenu->GetSubMenu(5);		
		if(pDSMenu && pPopupMenu==pDSMenu)
			AppendDSMenu(pDSMenu);
	}
	//资讯菜单的处理
	CNewMenu* pZXMenu=NULL;
	if(g_bHasCaption==0)
		pZXMenu = (CNewMenu*)pTopMenu->GetSubMenu(pTopMenu->GetMenuItemCount()-g_nWebPageNum?4:3);
	else
		pZXMenu = (CNewMenu*)pTopMenu->GetSubMenu(pTopMenu->GetMenuItemCount()-5);
	if(!pZXMenu) return;
	if(pPopupMenu == pZXMenu)
	{
		//F11资讯项
		if(strlen(g_GC.HKey_Path) && strlen(g_GC.HKey_KeyString))
		{
			pPopupMenu->RemoveMenu(IDC_HKEYPROGRAM,MF_BYCOMMAND);
			pPopupMenu->InsertMenu(ID_TDX_JBZL, MF_BYCOMMAND,IDC_HKEYPROGRAM, _F(g_GC.HKey_MenuName+CString("\tF11")));
		}
		else if(strlen(g_F11WebPage.PageName) != 0)
		{
			pPopupMenu->RemoveMenu(IDC_F11WEBPAGE,MF_BYCOMMAND);
			pPopupMenu->InsertMenu(ID_TDX_JBZL, MF_BYCOMMAND,IDC_F11WEBPAGE, _F(g_F11WebPage.PageName+CString("\tF11")));
		}
		//
		if(strlen(g_Url.AddZxStr) != 0)
		{
			pPopupMenu->RemoveMenu(ID_RIGHT_ADDZX,MF_BYCOMMAND);
			pPopupMenu->InsertMenu(ID_NEWS, MF_BYCOMMAND,ID_RIGHT_ADDZX, _F(g_Url.AddZxStr));	
		}
		if(!g_bHasWebInfo && strlen(g_Url.WebInfoURL)==0)
			pPopupMenu->RemoveMenu(ID_WEBINFO,MF_BYCOMMAND);
		if(g_GC.Video.nVideoFlag==0)
			pPopupMenu->RemoveMenu(ID_VIDEO,MF_BYCOMMAND);
	}
	//帮助菜单的处理
	CNewMenu* pHelpMenu = (CNewMenu*)pTopMenu->GetSubMenu(pTopMenu->GetMenuItemCount()-1);
	if(!pHelpMenu) return;
	if(pPopupMenu == pHelpMenu)
	{
		if(strlen(g_Url.EmailStr) != 0)
		{
			pPopupMenu->RemoveMenu(ID_RIGHT_EMAIL,MF_BYCOMMAND);
			pPopupMenu->InsertMenu(ID_FORUM, MF_BYCOMMAND,ID_RIGHT_EMAIL, _F(g_Url.EmailStr));	
		}
		if(strlen(g_Url.HelpWebStr) != 0)
		{
			pPopupMenu->RemoveMenu(ID_RIGHT_HELPWEB,MF_BYCOMMAND);
			pPopupMenu->InsertMenu(ID_FORUM, MF_BYCOMMAND,ID_RIGHT_HELPWEB, _F(g_Url.HelpWebStr));	
		}
	}
}
//
void CMainFrame::OnAutoConnectMsg(WPARAM wParam,LPARAM lParam)
{
	if(lParam==1 && g_AntiHack.bDSLogo)
	{
		if(g_bHasDS && !DSOnLine)
		{
			g_nDSAutoConnectFlag = AUTOCONNECT_PENDING;
			if(g_pComm3->Connect()) return;
		}
	}
	else if(lParam==0)
	{
		g_nAutoConnectNum++;
		if(g_nAutoConnectNum==5 && !g_bHintSwitch) //如果重连后也不通,则弹出连接对话框切换进入
		{
			g_nAutoConnectFlag = AUTOCONNECT_RESET;
			LogoDlg_ReConnect();
			return;
		}
		if(g_nAutoConnectNum == 5) //自动重连3次，不通则报错
		{
			//重新查找最快主站
			long Which_Minweight=CHQComm_NLB::NLB_GetMinweight(HQ_NLB);
			if(Which_Minweight >= 0)
			{
				g_HQHost.PrimaryHost = Which_Minweight;
				g_HQIPStr = g_HQHost.IPAddress[g_HQHost.PrimaryHost];
				g_HQNameStr = g_HQHost.HostName[g_HQHost.PrimaryHost];
				g_HQPort = g_HQHost.Port[g_HQHost.PrimaryHost];
				g_pComm->SetServerProperties(g_HQIPStr,g_HQPort);
			}
			else
			{
				g_nAutoConnectFlag = AUTOCONNECT_RESET;
				TDX_MessageBox(m_hWnd,"行情连接被断开,进入脱机运行状态!",MB_OK|MB_ICONQUESTION);
				return;
			}
		//	g_nAutoConnectFlag = AUTOCONNECT_RESET;
		//	TDX_MessageBox(m_hWnd,"行情连接被断开,进入脱机运行状态!",MB_OK|MB_ICONQUESTION);
		//	return;
		}
		g_nAutoConnectFlag = AUTOCONNECT_PENDING;
		if(!g_pComm->Connect())
		{
			g_nAutoConnectFlag = AUTOCONNECT_RECONNECT;
			return;
		}
	}
}

void CMainFrame::OnConnectResult(WPARAM wParam,LPARAM lParam)
{
	BOOL bIsComm3=(lParam==1);
	if(!AsynConnectResult(bIsComm3,wParam))
	{
		if(bIsComm3)
		{
			g_pComm3->Disconnect();	//断开连接
			g_nDSAutoConnectFlag = AUTOCONNECT_RECONNECT;
		}
		else
		{
			g_pComm->Disconnect();	//断开连接
			g_nAutoConnectFlag = AUTOCONNECT_RECONNECT;
		}
		return;
	}
	else
	{
		if(bIsComm3)
		{
			g_pComm3->StartThread();
			g_SysInfo.ConnectDSHQ_LastYmd=GetYMD();
			g_SysInfo.ConnectDSHQ_LastHms=GetHMS();

			DSOnLine = TRUE;
			g_pComm3->SetSendTop(0);	//状态复位
			//开始请求数据
			g_nMainID = MainIDPlus(m_nMainID,MAIN_WIN);
			g_nAssisID= DS_LOGIN_ASSISID;
			g_pGetData->DSLogin();
		}
		else
		{
			if(g_bNetErrorReLogin && HQControl::TestUnStableNetWorkStatus())
			{
				//先设置重连标识,弹出提示对话框后,不让再次进行断开逻辑
				g_bReLoginStatus = TRUE;
				//
				TDX_MessageBox(m_hWnd,"检测到本地网络连接不稳定或本程序运行不正常，\r\n\r\n点确定按钮后系统将退出重新进入!",MB_OK|MB_ICONEXCLAMATION);
				//
				g_bDirectQuit = TRUE;
				//关闭本系统,直接退出前先去掉前面的模态对话框
				CancelModalWindows();
				PostMessage(WM_CLOSE);
				return;
			}

			g_pComm->StartThread();
			g_SysInfo.ConnectHQ_LastYmd=GetYMD();
			g_SysInfo.ConnectHQ_LastHms=GetHMS();

			OnLine = TRUE;
			g_pComm->SetSendTop(0);	//状态复位
			//自动重连后进行一些操作
			RefreshStatusBar();
			//先去掉前面的模态对话框
			CancelModalWindows();

			if(theViewManager.pActiveView)
			{
				if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)) || theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))
					theViewManager.pActiveView->PostMessage(UM_READANDPAINT,0,0);
			}
		}
	}
}

void CMainFrame::OnOPSYS(int treetype,int force_showtree)
{
	//先得到当前股票
	int tempCurrentGPNo = GetSYSCurrentGPIndex();
	if(tempCurrentGPNo != -1)
	{
		if(treetype==WEB_TREETYPE)
		{
			WebInfoCurGPCode = (*g_pStockIO)[tempCurrentGPNo]->Code;
			WebInfoCurSetCode = (*g_pStockIO)[tempCurrentGPNo]->setcode;
		}
		else if(treetype==ZX_TREETYPE || treetype==ZX2_TREETYPE || treetype==ZX3_TREETYPE)
		{
			CjzxCurGPCode = (*g_pStockIO)[tempCurrentGPNo]->Code;
			CjzxCurSetCode = (*g_pStockIO)[tempCurrentGPNo]->setcode;
		}
		else if(treetype==JB_TREETYPE)
		{
			JbfxCurGPCode = (*g_pStockIO)[tempCurrentGPNo]->Code;
			JbfxCurSetCode = (*g_pStockIO)[tempCurrentGPNo]->setcode;
		}
	}
	switch(treetype)
	{
	case WEB_TREETYPE:
		SwitchToView("WebInfo");break;
	case ZX_TREETYPE:
	case ZX2_TREETYPE:
	case ZX3_TREETYPE:
		SwitchToView("Cjzx");break;
	case JB_TREETYPE:
		SwitchToView("Jbfx");break;
	}
	if(force_showtree)
	{
		m_wndWorkspace.SetActiveWnd(g_nTreePos[treetype]); //设置网站树
		ShowControlBar(&m_wndWorkspace,TRUE,TRUE);		
	}
}

void CMainFrame::OnJbfx()
{
	//如果设置了外挂的注册表项,按F11则直接打开
	if(strlen(g_GC.HKey_Path) && strlen(g_GC.HKey_KeyString))
		OnHKeyProgram();
	//如果设置了F11网页，也直接打开网页
	else if(strlen(g_F11WebPage.PageName) != 0)
		OnF11WebPage();
}

void CMainFrame::OnGgzl()
{
	OnCommCommand(CMD_JBM,801);
}

void CMainFrame::OnGPCoolInfo()
{
	SwitchToRestSpace(AREA_GGBW);
}

#include "JiangDlg.h"
void CMainFrame::OnJiangEn()
{
	struct CurrStockData tmpHq;
	memset(&tmpHq,0,sizeof(struct CurrStockData));
	int tempCurrentNo = GetSYSCurrentGPIndex();
	if(tempCurrentNo >= 0)
	{
		MemStkInfo *pStkInfo = (*g_pStockIO)[tempCurrentNo];
		if(pStkInfo)
			g_pStockIO->ReadCurrStockData(pStkInfo,&tmpHq);
	}
	CJiangDlg dlg;
	if(tmpHq.Now > COMPPREC)
		dlg.m_fStartP = tmpHq.Now;
	else
		dlg.m_fStartP = tmpHq.Close;
	dlg.DoModal();
}

void CMainFrame::OnWorkBarCheck(UINT nID)
{
	OnBarCheck(nID);
	if(!m_wndWorkspace.IsWindowVisible())
	{
		m_wndWorkspace.SetActiveWnd(0);
		theViewManager.pActiveView->SetFocus();
	}
}

void CMainFrame::OnRestBarCheck(UINT nID)
{
	if(g_OEM.bEmbedWTVersion && !m_wndRestspace.IsWindowVisible())
	{
		int i=m_wndRestspace.GetCurWnd();
		if(g_nRestPos[AREA_WT]==i)
		{
			DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
			if(!(dwStatus&JY_LOGINED))
			{
				LoginEmbedWT();
				return;
			}
		}
	}
	OnBarCheck(nID);
	if(!bRestShow)
		m_wndRestspace.SendMessage(UM_STARTFUNC_MESSAGE,0,0);
	if(!m_wndRestspace.IsWindowVisible())
		theViewManager.pActiveView->SetFocus();
	else if(g_bActTogether)
	{
		SendMessage(UM_MINEMSG,0,0);		
		SendMessage(UM_FLZLMSG,0,0);
		SendMessage(UM_GGBWMSG,0,0);
	}
}

void CMainFrame::OnStatusBarCheck(UINT nID)
{
	OnBarCheck(nID);
	bStatusBarShow = m_wndStatusBar.IsWindowVisible();
}

void CMainFrame::SwitchToFirstView()
{
	CWnd *wnd;
	if(TRUE/*!g_bHasCaption*/ || g_bFirstCusPad)
		wnd = CWnd::FromHandle (FirstWindow);
	else
		wnd = CWnd::FromHandle (BigMenuWindow);
	if(wnd)
		((CFrameWnd *)wnd)->ActivateFrame();
}

void CMainFrame::ProcessSwitchCallBack(WPARAM wParam,LPARAM lParam)
{
	short FuncType = LOWORD(wParam);
	switch(FuncType)
	{
	case SWITCH_TOZSGPC:	//切换到股票池
		{
			CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
			if(tempWnd)
			{
				tempWnd->m_pSwitchBar->SetCurLabel(1);
				tempWnd->SendMessage(WM_COMMAND,ID_RIGHT_FYGROUP);
			}
		}
		break;
	case SWITCH_TOWARNWT:	//切换到预警委托
		PostMessage(WM_COMMAND,IDC_PRICEWARN);
		break;		
	case SWITCH_TOZST:		//切换到走势图
		ToZst(SwitchCurGPCode.GetBuffer(0),SwitchCurSetCode,TRUE);
		break;
	case SWITCH_TOVIP:		//切换到分析图
		ToZst(SwitchCurGPCode.GetBuffer(0),SwitchCurSetCode,FALSE);
		break;	
	case SWITCH_REFRESHALLHQ://刷新所有行情,lParam为最小的刷新间隔数,如为0,则为10秒
		if(lParam!=0)
			RefreshAllHq(lParam);
		else
			RefreshAllHq(10);
		break;
	case SWITCH_ADDBLOCK:	//加入到板块
		{
			CGetBlockDlg dlg;
			dlg.m_bGetBlock = FALSE;
			dlg.m_CurGPCode=SwitchCurGPCode;
			dlg.m_CurSetCode=SwitchCurSetCode;
			dlg.DoModal ();
			break;
		}
		break;		
	case SWITCH_SAVENEWBLOCK://存为新板块
		{
			short StkNum = HIWORD(wParam);
			short *pIndex = (short *)lParam;
			if(!pIndex || StkNum < 0 || StkNum >= MAXGPNUM) break;
			CGetBlockDlg dlg;
			if(dlg.DoModal() == IDOK)
			{
				if(HasGPInBlockAndContinue(dlg.nSelBlock))
					MakeBlock(pIndex,StkNum,dlg.nSelBlock,FALSE);
			}
		}
		break;
	case SWITCH_SETKEY:
		PostMessage(WM_KEYDOWN,lParam,0);
		break;
	case SWITCH_SHIFTCJZX:
		PostMessage(WM_COMMAND,IDC_ZXTREE);
		break;
	case SWITCH_CLOSECJZX:
		{
			if( theViewManager.pOldActiveView && !(theViewManager.pOldActiveView->IsKindOf(RUNTIME_CLASS(CIEBrowserView))) )
			{
				if(theViewManager.pOldActiveView != theViewManager.pActiveView)
				{
					CFrameWnd *tempFrameWnd = (CFrameWnd *)(theViewManager.pOldActiveView)->GetParent();
					tempFrameWnd->ActivateFrame();
					return;
				}
			}
			SwitchToFirstView();
		}
		break;
	case SWITCH_CLOSEASSISWND:
		if(g_nEmbedTabSel==LOGIN_ONLYWT && !OnLine && m_wndRestspace.IsFullSize()) break;
		ShowControlBar(&m_wndRestspace,FALSE,FALSE);
		if(theViewManager.pActiveView)
			theViewManager.pActiveView->SetFocus();
		break;
	case SWITCH_MAXASSISWND:
		if(g_nEmbedTabSel==LOGIN_ONLYWT && !OnLine) break;
		if(m_wndRestspace.IsFullSize()) 
			m_wndRestspace.SwitchToFullSize(FALSE);
		else
			m_wndRestspace.SwitchToFullSize(TRUE);
		break;
	case SWITCH_TOGGLEASSISWND:
		m_wndRestspace.PostMessage(WM_COMMAND,IDC_BUTTON_MINI);
		break;
	case SWITCH_OPENEMBWT:
		SwitchToRestSpace(AREA_WT);
		break;
	case SWITCH_REFRESHTITLE:
		OnNcPaint();
		break;
	case SWITCH_LOGINEMBWT:
		LoginEmbedWT();
		break;
	case SWITCH_QUITEMBWT:
		if(g_nEmbedTabSel==LOGIN_ONLYWT && !OnLine && m_wndRestspace.IsFullSize())
		{
			//关闭本系统,直接退出前先去掉前面的模态对话框
			g_bDirectQuit=TRUE;
			CancelModalWindows();
			PostMessage(WM_CLOSE);
			break;
		}
		ProcessSwitchCallBack(SWITCH_CLOSEASSISWND,0);
		g_bEmbWTCreated=FALSE;
		break;
	case SWITCH_JYGETHQ:	//交易得到行情
		{
			memset(&g_ET.JyHQ,0,sizeof(struct TdxJyHQ));
			g_ET.HWnd=(HWND)lParam;
			g_ET.GPSetCode = 0;
			g_ET.GPCode[0]=0;
			g_ET.Ready=0;
			g_ET.IsTdxJyHQ=1;
			if(SwitchCurSetCode!=SZ && SwitchCurSetCode!=SH)
				SwitchCurSetCode=-1;
			//
			MemStkInfo *pStkInfo = g_pStockIO->Get_MemStkInfo(SwitchCurGPCode,SwitchCurSetCode);
			MemStkInfo *pCurrStkInfo = NULL;
			if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)))
			{
				int tempCurrentNo = GetSYSCurrentGPIndex();
				pCurrStkInfo = (*g_pStockIO)[tempCurrentNo];
			}
			if(pStkInfo)
			{
				if(OnLine)
				{
					g_ET.GPSetCode=pStkInfo->setcode;
					strcpy(g_ET.GPCode,pStkInfo->Code);
				}
				if(pCurrStkInfo!=pStkInfo)
					ToZst(pStkInfo->Code,pStkInfo->setcode,FALSE);
				else
					g_ET.Ready=1;
				if(g_ET.HWnd && g_ET.Ready) //向调用者的窗口发送数据消息
				{
					struct CurrStockData tmphq;
					g_pStockIO->ReadCurrStockData(pStkInfo,&tmphq);
					ETSendCopyDataMessage(g_ET.HWnd,pStkInfo,&tmphq);
				}
			}
			break;
		}
	}
}

CWnd *CMainFrame::GetRestSpaceWnd(int gn_type)
{
	switch(gn_type)
	{
	case IGN_MINE:
		return m_wndRestspace.GetWnd(g_nRestPos[AREA_MINE]);
	case IGN_GGBW:
		return m_wndRestspace.GetWnd(g_nRestPos[AREA_GGBW]);
	case IGN_FLZL:
		return m_wndRestspace.GetWnd(g_nRestPos[AREA_FLZL]);
	}
	return NULL;
}

void CMainFrame::OnNewpad() 
{
	CString tmpPadIni; CFile tmpIni;
	tmpPadIni.Format("%ssheet\\tmp.spo",g_WSXHStr);
	if(!tmpIni.Open(tmpPadIni, CFile::modeCreate|CFile::modeWrite))	return;
	tmpIni.Close();
	WritePrivateProfileString("DEAFULTGP", "GPINDEX_VAR1", IntToStr(51), tmpPadIni);
	WritePrivateProfileString("DEAFULTGP", "UnitNum", IntToStr(1), tmpPadIni);
	
	ConstructCustomPad(1208);
	
	TDX_MessageBox (m_hWnd,"提示:新建版面时,请点右键进行操作,\r\n新建完毕后,选择\'退出设置版面\'",MB_ICONINFORMATION|MB_OK);	
}

#include "SetDefPadDlg.h"
void CMainFrame::OnSetdefcuspad() 
{
	if(g_vDynaCmdTab.empty())
	{
		TDX_MessageBox (m_hWnd,"请先定义一些定制版面,才能设置初始版面!",MB_ICONINFORMATION|MB_OK);
		return;
	}
	CSetDefPadDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnUpdateSetdefcuspad(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!g_vDynaCmdTab.empty());
}

void CMainFrame::OnShowCustomPadSwitch()
{
	g_bShowCustomPadSwitch=!g_bShowCustomPadSwitch;
	for(int i = 0;i<theViewManager.GetWindowNum();i++)
	{
		CView *tempView = (CView *)theViewManager.arViews[i];
		if(tempView->IsKindOf(RUNTIME_CLASS(CComView2)))
		{
			((CComView2 *)tempView)->SetControlSize();
			((CComView2 *)tempView)->SendMessage(UM_REFRESHCUSTOMBAR);
		}
	}
}

void CMainFrame::OnUpdateShowCustomPadSwitch(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_bShowCustomPadSwitch);
}

BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	if(pCopyDataStruct && pCopyDataStruct->lpData && pCopyDataStruct->cbData>40 && (memcmp((char *)pCopyDataStruct->lpData+7,"Command=ChangeCode",18)==0) )
	{
		char *pTmp=new char[200];
		memcpy(pTmp,pCopyDataStruct->lpData,98);
		pTmp[98]=0;
		char *pPos=strstr(pTmp,"CodeList=");
		if(pPos)
		{
			pPos+=10;
			pPos[6]=0;
			MemStkInfo *pStkInfo = g_pStockIO->Get_MemStkInfo(pPos,-1);
			if(pStkInfo) ToZst(pStkInfo->Code,pStkInfo->setcode,FALSE);
		}
		delete pTmp;
		return TRUE;
	}
	if(g_GC.bAlongOpenHXInfo && pCopyDataStruct && (pCopyDataStruct->dwData & 0xFF)==7 && g_hHXInfoWnd && ::IsWindow(g_hHXInfoWnd))
	{
		::SendMessage(g_hHXInfoWnd, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)pCopyDataStruct);
		return TRUE;
	}	
	if(g_bWtUserVerifyStyle==6 && pCopyDataStruct && pCopyDataStruct->dwData==1)
	{
		MakeJyVerifyFile(""); //生成验证Cookie文件
		m_bJyVerify_Success = LocalJyVerify();
		TDX_MessageBox (m_hWnd,"委托认证通过!",MB_ICONINFORMATION|MB_OK);
		return TRUE;
	}
	return CNewMDIFrameWnd::OnCopyData(pWnd, pCopyDataStruct);
}

void CMainFrame::OnClipSpy()
{
	if(!m_bClipSpy)
	{
		m_hWndNextViewer = ::SetClipboardViewer(m_hWnd);
		m_bClipSpy = TRUE;
		if(!g_pKeyWordSearch)
		{
			g_pKeyWordSearch = new KeyWordSeach;
			int gpnum = g_pStockIO->GetStockNum(ALL_ALL);
			for(int i=0;i<gpnum;i++)
				g_pKeyWordSearch->RegiseterStk(i,TRUE,TRUE);
		}
	}
	else
	{
		::ChangeClipboardChain(m_hWnd, m_hWndNextViewer);
		m_hWndNextViewer=NULL;
		m_bClipSpy = FALSE;
	}
}

void CMainFrame::OnUpdateClipSpy(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bClipSpy);
}

void CMainFrame::NotifyMessage(DWORD dwMessage,HICON hIcon)
{
	NOTIFYICONDATA noti;
	memset(&noti,0,sizeof(NOTIFYICONDATA));
	noti.cbSize=sizeof(NOTIFYICONDATA);
	noti.hIcon=hIcon;
	noti.hWnd=m_hWnd;
	strcpy(noti.szTip,(char*)(LPCSTR)g_strTitleName);
	noti.uCallbackMessage=UM_TDXWSHOW;
	noti.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
	noti.uID=0;
	Shell_NotifyIcon(dwMessage,&noti);
}

void CMainFrame::OnShowMessage(WPARAM wParam,LPARAM lParam)
{
	switch(lParam) 
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		SendMessage(WM_COMMAND, ID_RESTORE);
		break;
	case WM_RBUTTONDOWN:
		{
			CNewMenu menu;
			menu.LoadMenu (IDR_RIGHTMENU);
			CNewMenu *pPopup = (CNewMenu *)menu.GetSubMenu(7);
			CPoint point;
			GetCursorPos(&point);
			SetForegroundWindow();
			pPopup->TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,this);
		}
		break;
	}
}

void CMainFrame::OnHszs() 
{
	ToZst(".HSI",27,TRUE);	
}

void CMainFrame::OnShgzzs()
{
	ToZst("000012",SH,TRUE);	
}

void CMainFrame::OnNetfluxcount() 
{
	if(m_pFluxCalcDlg)
	{
		m_pFluxCalcDlg->ShowWindow(SW_SHOW);
		return;
	}
	m_pFluxCalcDlg = new CFluxCountDlg(this);
	m_pFluxCalcDlg->Create(IDD_FLUXCOUNT);
	m_pFluxCalcDlg->ShowWindow (SW_SHOW);
}

void CMainFrame::OnBuy(UINT nID)
{
	if(!CanUseEmbedWTFunc(m_hWnd)) return;
	SwitchToRestSpace(AREA_WT);
	int tempCurrentNo = GetSYSCurrentGPIndex();
	if(nID>=IDC_BUY0 && nID<=IDC_BUY20 && tempCurrentNo>=0)
		m_wndRestspace.PostMessage(UM_DOLEVINBUY_MESSAGE,nID-IDC_BUY0,tempCurrentNo);	
}

void CMainFrame::OnSell(UINT nID)
{
	if(!CanUseEmbedWTFunc(m_hWnd)) return;
	SwitchToRestSpace(AREA_WT);
	int tempCurrentNo = GetSYSCurrentGPIndex();
	if(nID>=IDC_SELL0 && nID<=IDC_SELL20 && tempCurrentNo>=0)
		m_wndRestspace.PostMessage(UM_DOLEVINSELL_MESSAGE,nID-IDC_SELL0,tempCurrentNo);	
}

void CMainFrame::OnLevinBuy(UINT nID)
{
	if(!CanUseEmbedWTFunc(m_hWnd)) return;
	int tempCurrentNo = GetSYSCurrentGPIndex();
	if(nID>=IDC_LEVINBUY0 && nID<=IDC_LEVINBUY20 && tempCurrentNo>=0)
		m_wndRestspace.PostMessage(UM_DOLEVINBUY_MESSAGE,nID-IDC_LEVINBUY0,-tempCurrentNo);	
}

void CMainFrame::OnLevinSell(UINT nID)
{
	if(!CanUseEmbedWTFunc(m_hWnd)) return;
	int tempCurrentNo = GetSYSCurrentGPIndex();
	if(nID>=IDC_LEVINSELL0 && nID<=IDC_LEVINSELL20 && tempCurrentNo>=0)
		m_wndRestspace.PostMessage(UM_DOLEVINSELL_MESSAGE,nID-IDC_LEVINSELL0,-tempCurrentNo);
}

void CMainFrame::OnWTCancel()
{
	if(!CanUseEmbedWTFunc(m_hWnd)) return;
	int tempCurrentNo = GetSYSCurrentGPIndex();
	SwitchToRestSpace(AREA_WT);
	m_wndRestspace.PostMessage(UM_DOWTCANCEL_MESSAGE,tempCurrentNo,0);	
}

void CMainFrame::OnQzSDS()
{
	if(!g_GC.bHasQzSDS) return;
	if(!CanUseEmbedWTFunc(m_hWnd)) return;
	//对于闪电手交易功能,如果当前画面不是ComView,则切换到分时图
	if(!theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)))
	{
		int tempCurrentNo = GetSYSCurrentGPIndex();
		if(tempCurrentNo >= 0)
		{
			MemStkInfo *pStkInfo = (*g_pStockIO)[tempCurrentNo];
			if(pStkInfo)
				ToZst(pStkInfo->Code,pStkInfo->setcode,FALSE);
		}
	}
	//如果当前权证是某只权证,则将交易模块用到的最新权证代码更换掉
	int tempCurrentNo = GetSYSCurrentGPIndex();
	if(tempCurrentNo>=0)
	{
		MemStkInfo *pInfo = (*g_pStockIO)[tempCurrentNo];
		if(pInfo)
		{
			int nType = GetStockType(pInfo->setcode,pInfo->Code);
			if((nType==CODE_SHQZ) || (nType==CODE_SZQZ))
				WritePrivateProfileString("sds","lastzqdm",pInfo->Code,g_WSXHStr+"newmodem.ini"); 
		}
	}
	m_wndRestspace.PostMessage(UM_DOQZSDS_MESSAGE,0,0);	
}

void CMainFrame::OnUserCenter()
{
	OpenCusWebPage(this,USERCENTER_WEBPAGE,FALSE);
}

void CMainFrame::OnRelativeInfo()
{
	OpenCusWebPage(this,g_NewJyCfg.RelativeInfo,TRUE);
}

#include "ModifyAbsDlg.h"
void CMainFrame::OnPriceWarn()
{
	if(!OnLine)
	{
		TDX_MessageBox(m_hWnd,"没有连接行情,不能使用此功能!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}	
	int iNo=GetSYSCurrentGPIndex();
	if(iNo<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[iNo];
	if(!tempStkInfo) return;
	int i,j;
	BOOL bExist = FALSE;
	for(i = 0;i < g_pAbsConditionNum;i++)//看是否已经存在这只股票
	{
		if(tempStkInfo->setcode==g_pAbsCondition[i].setcode && strcmp(tempStkInfo->Code,g_pAbsCondition[i].code)==0)
		{	bExist = TRUE;break; }
	}
	if(!bExist)
	{	
		if(g_pAbsConditionNum>=MAX_ABSWARNMAXNUM)
		{
			TDX_MessageBox(m_hWnd,"条件预警的股票太多,不能再加入!",MB_OK|MB_ICONEXCLAMATION);
			return;
		}
		memset(g_pAbsCondition+g_pAbsConditionNum,0,sizeof(PWARNINGCND));
		strcpy(g_pAbsCondition[g_pAbsConditionNum].code,tempStkInfo->Code);
		g_pAbsCondition[g_pAbsConditionNum].setcode = tempStkInfo->setcode;
		g_pAbsCondition[g_pAbsConditionNum].fLowPrice = 0.0f;
		g_pAbsCondition[g_pAbsConditionNum].fHighPrice = 0.0f;
		g_pAbsCondition[g_pAbsConditionNum].ETFAPRate = 0;
		g_pAbsCondition[g_pAbsConditionNum].TurnOverRate = 0;
		g_pAbsConditionNum++;
		i=g_pAbsConditionNum-1;
	}
	BOOL bLastWarnOn = g_bTjWarnSwitch; 
	g_bTjWarnSwitch = FALSE;
	CModifyAbsDlg dlg;
	dlg.m_bValidPrice = TRUE;	//必须有合法的价格
	dlg.m_bHasDeleteBtn = TRUE;	//有删除按钮
	dlg.m_fUpBreak = g_pAbsCondition[i].fHighPrice;
	dlg.m_fDownBreak = g_pAbsCondition[i].fLowPrice;
	dlg.m_nCurSel1 = g_pAbsCondition[i].bDoLevin1;
	dlg.m_nCurSel2 = g_pAbsCondition[i].bDoLevin2;
	dlg.m_fPriceDiff	= g_pAbsCondition[i].ETFAPRate*0.01;
	dlg.m_fChangeHand	= g_pAbsCondition[i].TurnOverRate*0.01;
	dlg.nStockNo = iNo;
	UINT nRet = dlg.DoModal();
	g_bTjWarnSwitch = bLastWarnOn;
	if(nRet== IDOK)
	{
		g_pAbsCondition[i].fHighPrice = dlg.m_fUpBreak;
		g_pAbsCondition[i].fLowPrice = dlg.m_fDownBreak;
		g_pAbsCondition[i].bDoLevin1 = dlg.m_nCurSel1;
		g_pAbsCondition[i].bDoLevin2 = dlg.m_nCurSel2;
		g_pAbsCondition[i].ETFAPRate = (unsigned short)(dlg.m_fPriceDiff*100+0.5);
		g_pAbsCondition[i].TurnOverRate = (unsigned short)(dlg.m_fChangeHand*100+0.5);
		if(!g_bTjWarnSwitch && OnLine)
		{
			if(TDX_MessageBox(m_hWnd,"条件预警还没有打开,您是否想打开?",MB_YESNO|MB_ICONINFORMATION ) == IDYES)
				g_bTjWarnSwitch = TRUE;
		}
		if(theViewManager.pActiveView && theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)))
			theViewManager.pActiveView->SendMessage(UM_CALCANDPAINT);
	}
	else
	{
		if(!bExist)	g_pAbsConditionNum--;
		else if(dlg.m_bDeleted)
		{
			strcpy(g_pAbsCondition[i].code,"-1");
			for(i = g_pAbsConditionNum - 1;i >= 0;i--)
			{
				if(strcmp(g_pAbsCondition[i].code,"-1") == 0)
				{
					for(j = i+1;j < g_pAbsConditionNum;j++)
						g_pAbsCondition[j-1] = g_pAbsCondition[j];
					g_pAbsConditionNum--;
				}
			}
		}
	}
}

#include "ShowInfoDlg.h"
void	CMainFrame::OnConInfo()
{
	if(!HQControl::IsValidProgram2())
		return;
	CShowInfoDlg dlg;
	dlg.DoModal();
}

#include "MarkMgn.h"
void	CMainFrame::OnMarkMgn()
{
	CMarkMgn dlg;
	dlg.DoModal();
}

void	CMainFrame::OnSwitchPadMenu(UINT nID)
{
	if(nID-ID_PADSWITCH1==0)				//当前不用换
		return;

	for(int i=0;i<theViewManager.GetWindowNum();i++)
	{
		if((CView *)theViewManager.arViews[i]==theViewManager.pActiveView)
			break;
	}
	int nNewXh = nID-ID_PADSWITCH1;
	if(nID-ID_PADSWITCH1<=i) nNewXh -= 1;

	CView *pNewActPadView = (CView*)theViewManager.arViews[nNewXh];
	if(pNewActPadView != theViewManager.pActiveView)
	{
		CFrameWnd *tempFrameWnd = (CFrameWnd *)(pNewActPadView)->GetParent();
		tempFrameWnd->ActivateFrame();
	}
}

void	CMainFrame::OnLineEnterMsg(WPARAM wParam,LPARAM lParam)
{
	//关闭欢迎页
//	CSplashWnd::HideSplashScreen();
	//
	if(bStartTips)
		OnIdsTipoftheday();
	if(!OffLineMode)
	{
		//通告消息
		if(!g_GC.bNoUrgent)
			PostMessage(UM_SHOWURGENT_MESSAGE,URGENT_NORMAL,1);
		//对于高速行情免费登录版,弹出通达信的通告
	#ifdef LEVEL2
		if(g_nCurLevel2Tab==1)
			PostMessage(UM_SHOWURGENT_MESSAGE,URGENT_TDXFREE,0);
	#endif
		if(g_bUserHostEverConnect && g_GC.bAutoupFromLevel1)
			;
		else
			SendMessage(UM_PROCESSAUTOUPINFO,0,0);
	}
	if(OnLine && g_GH.bLocalData && GetVipDocLastDate()<g_lOpenRq)
	{
		SYSTEMTIME  nowtime;
		GetCorrespondingTime(&nowtime);
		int m = nowtime.wHour*60+nowtime.wMinute;  //分钟
		long lLastData = GetLastDate(g_lOpenRq);
		while(GetWeek(lLastData)==0 || GetWeek(lLastData)==6) lLastData = GetLastDate(lLastData);
		if(g_bYesterdayFlag || m>940 || GetVipDocLastDate()<lLastData)
			DownDayData_AtOnce(this,"您选择了本地数据模式,经检测,本地日线数据可能不全,\r\n\r\n现在是否要下载补全?\n\n");
	}
#ifdef OEM_NEWJY
	if(g_GC.bHasLevel2Engine && g_bUserHostEverConnect)
		CreateQsDetectThread();
	if(g_OEM.bHasQSRun)
		QS_MainFrameCreated_(m_hWnd,0);
#endif	
}

BOOL QueryAutoUpVersion(char *strUrl, float &fVersion)
{	
	char *cFileName = "netver.dat";
	CInternetSession *m_pInetSession = NULL;
	if(!m_pInetSession)
	{
		//建立用户验证的Session
		m_pInetSession = new CInternetSession(AfxGetAppName(),1,PRE_CONFIG_INTERNET_ACCESS);
		m_pInetSession->EnableStatusCallback();
		m_pInetSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT,15*1000);
		m_pInetSession->SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 1000);
		m_pInetSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);
	}
	if(!m_pInetSession) return FALSE;
	CHttpFile* File=NULL;
	CHttpConnection* pConnection=NULL;
	try
	{
		CString szHeaders = _T("Accept: text/*\r\nUser-Agent: Mozilla/4.0\r\n");
		CString strTempURL(strUrl);
		CString ServerName;
		CString Object;
		INTERNET_PORT Port;
		DWORD ServiceType;
		AfxParseURL( strTempURL, ServiceType, ServerName, Object, Port );
		pConnection = m_pInetSession->GetHttpConnection( ServerName,Port );
		if(!pConnection) return FALSE;

		File = pConnection->OpenRequest( CHttpConnection::HTTP_VERB_GET, Object,
		NULL, 1, NULL, NULL,
		INTERNET_FLAG_RELOAD |
		INTERNET_FLAG_NO_CACHE_WRITE);
		if(!File) 
		{
			pConnection->Close();
			delete pConnection;
			return FALSE;
		}
		BOOL result = File->AddRequestHeaders(szHeaders);
		if(result)
			result = File->SendRequest();
		if(result)
		{
			DWORD dwRet;
			File->QueryInfoStatusCode(dwRet);
			if(dwRet==HTTP_STATUS_OK)
			{
				CStdioFile contentFile;
				if(contentFile.Open(cFileName, CStdioFile::modeCreate|CStdioFile::modeWrite|CStdioFile::shareExclusive))
				{
					char Buffer[512]={0};
					int numread = 0;
					do 
					{
						numread = File->Read(Buffer,500);
						if(numread>0) contentFile.Write(Buffer, numread);
					} while(numread>0);

					contentFile.Close();
				}	
				if(File)
				{
					File->Close();
					delete File;
					File = NULL;
				}
				if(pConnection)
				{
					pConnection->Close();
					delete pConnection;
					pConnection = NULL;
				}
			}
		}
		if(File)
		{
			File->Close();
			delete File;
			File = NULL;
		}
		if(pConnection)
		{
			pConnection->Close();
			delete pConnection;
			pConnection = NULL;
		}
	}
	catch(...)
	{
		if(File)
		{
			File->Close();
			delete File;
			File = NULL;
		}
		if(pConnection)
		{
			pConnection->Close();
			delete pConnection;
			pConnection = NULL;
		}
		return FALSE;
	}

	CStdioFile contentFile;
	if(contentFile.Open(cFileName, CStdioFile::modeRead))
	{
		char Buffer[512]={0};
		if(contentFile.Read(Buffer,500)>0)
			fVersion = atof(Buffer);
		contentFile.Close();
	}

	return TRUE;
}

void	CMainFrame::OnProcessAutoupInfo(WPARAM wParam,LPARAM lParam)
{
	float fWebVersion = 0.0f;
	char strAutoUpZip[MAX_PATH]={0};
	CString	Ini = g_WSXHStr+(CString)g_strYxhjCfg;
	GetPrivateProfileString("Other","AutoUpZip","",strAutoUpZip,254,Ini);
	char uptxt[MAX_PATH]={0};
	sprintf(uptxt, "%s.txt", strAutoUpZip);
	if(g_nUseDNSUrl==1)
	{
		CString tmpURL(strAutoUpZip);
		tmpURL.Replace(g_strDNSUrl[0], g_strDNSUrl[1]);
		strncpy(strAutoUpZip, (char*)(LPCSTR)tmpURL, MAX_PATH-2);
	}
	if(QueryAutoUpVersion(uptxt, fWebVersion)&&fWebVersion>atof(GetNowVersion())+COMPPREC)
	{
		AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_AUTOUP);
	}
}

/*
void	CMainFrame::OnProcessAutoupInfo(WPARAM wParam,LPARAM lParam)
{
	BOOL bWriteAutoupTime=FALSE;
	long nTime = GetYMD();
	 //自动升级信息
	if(!g_GC.bNoAutoup)
	{
		bWriteAutoupTime=ShowAutoupInfo();
		if(bWriteAutoupTime)
			g_GH.nLastHintAutoupTime = nTime;
	}
	else if(g_GC.nShareMode || g_GC.bLocalData)
	{
		//对于这三种情况,不能进行升级
	}
	else if(g_GH.nLastHintAutoupTime>0)
	{
		if(nTime-g_GH.nLastHintAutoupTime>=100)
		{
			CLeadUp dlg;
			if(dlg.DoModal()==IDOK)
			{
				g_GH.nLastHintAutoupTime = nTime;
				AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_AUTOUP);
			}
			else if(dlg.m_bAsk)		
				g_GH.nLastHintAutoupTime = nTime;
			bWriteAutoupTime=TRUE;
		}
	}
	if(bWriteAutoupTime)
		WritePrivateProfileString("LevelUp","LastTime",IntToStr(g_GH.nLastHintAutoupTime),g_WSXHStr+g_strUserCfg);
}
*/

void CMainFrame::OnFundHold()
{
	int tempCurrentNo = GetSYSCurrentGPIndex();
	if(tempCurrentNo == -1)	return ;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[tempCurrentNo];
	if(!tempStkInfo) return;

	if(!g_bFundDataAlwaysSync && !g_bDebugMode)
	{
		g_nDataSyncType=DATASYNC_JJ;
		AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_DATASYNC);
	}
	CFundHoldStockDlg dlg;
	strcpy(dlg.m_lpszSymbol,tempStkInfo->Code);
	dlg.m_strTitle.Format("%s主要被下列基金所持有",tempStkInfo->Code);
	dlg.DoModal();
}

#include "WriteUrgent.h"
void CMainFrame::OnWriteUrgent()
{
	if(!OnLine)
	{
		TDX_MessageBox(m_hWnd,"脱机状态下不能发布!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	if(g_pWriteUrgent) 
	{
		TDX_MessageBox(m_hWnd,"通告正在发布中,请稍侯再试!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	CWriteUrgent dlg;
	dlg.DoModal();
}

void CMainFrame::OnUrgentDlg()
{
//	ShowUrgentDlg(URGENT_DURING,URGENT_NORMAL);
	SendMessage(WM_COMMAND, ID_POPMOREDLG);
}

//lParam为0表示是中途弹出 为1表示是刚进入时弹出
void CMainFrame::OnShowUrgentMsg(WPARAM wParam,LPARAM lParam)
{
	if(lParam==0)
		ShowUrgentDlg(URGENT_DURING,wParam);
	else
		ShowUrgentDlg(URGENT_INIT,wParam);
}

void CMainFrame::OnAutoConnectDS(WPARAM wParam,LPARAM lParam)
{
	DSConnectWhenNeedHq(TRUE);
}

void CMainFrame::OnInitDS()
{
	if(!OnLine) 
	{
		TDX_MessageBox(m_hWnd,"请先连接到行情主站!",MB_ICONINFORMATION|MB_OK);
		return;
	}
	DSConnectWhenNeedHq(FALSE);
}

void CMainFrame::OnUpdateInitDS(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(g_bHasDS && !DSOnLine);
}

void CMainFrame::OnUnInitDS()
{
	if(g_pComm3) 
		g_pComm3->Disconnect();
	DSOnLine = FALSE;
	g_nDSAutoConnectFlag = AUTOCONNECT_RESET;

	CString tmpHintStr;
	tmpHintStr.Format("%s连接被断开!",g_DS.DSOEMName);
	TDX_MessageBox(m_hWnd,tmpHintStr,MB_ICONINFORMATION|MB_OK);
}

void CMainFrame::OnUpdateUnInitDS(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(g_bHasDS&&DSOnLine);
}

#include "LocalIndexMngDlg.h"
#include "LocalExternMngDlg.h"
#include "LocalPtStkMngDlg.h"
void CMainFrame::OnLocalIndex()
{
	CPropertySheet OptionSheet("");
	OptionSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CLocalIndexMngDlg	Page1;
	CLocalExternMngDlg	Page2;
	CLocalPtStkMngDlg	Page3;
	OptionSheet.AddPage(&Page1);
	OptionSheet.AddPage(&Page2);
	OptionSheet.AddPage(&Page3);
	OptionSheet.SetTitle (_F("定制品种管理"));
	OptionSheet.SetActivePage(&Page1);
	OptionSheet.DoModal();
	if(theViewManager.pActiveView)
		theViewManager.pActiveView->PostMessage(UM_BLOCKMODIFIED,0,0);
	LoadAndResetGridTab();
	GetZHBBlockNum();
}

void CMainFrame::OnLocalExtern()
{
	CPropertySheet OptionSheet("");
	OptionSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CLocalIndexMngDlg	Page1;
	CLocalExternMngDlg	Page2;
	CLocalPtStkMngDlg	Page3;
	OptionSheet.AddPage(&Page1);
	OptionSheet.AddPage(&Page2);
	OptionSheet.AddPage(&Page3);
	OptionSheet.SetTitle (_F("定制品种管理"));
	OptionSheet.SetActivePage(&Page2);
	OptionSheet.DoModal();
	if(theViewManager.pActiveView)
		theViewManager.pActiveView->PostMessage(UM_BLOCKMODIFIED,0,0);
	LoadAndResetGridTab();
	GetZHBBlockNum();
}

void CMainFrame::OnLocalPtStk()
{
	CPropertySheet OptionSheet("");
	OptionSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	CLocalIndexMngDlg	Page1;
	CLocalExternMngDlg	Page2;
	CLocalPtStkMngDlg	Page3;
	OptionSheet.AddPage(&Page1);
	OptionSheet.AddPage(&Page2);
	OptionSheet.AddPage(&Page3);
	OptionSheet.SetTitle (_F("定制品种管理"));
	OptionSheet.SetActivePage(&Page3);
	OptionSheet.DoModal();
	if(theViewManager.pActiveView)
		theViewManager.pActiveView->PostMessage(UM_BLOCKMODIFIED,0,0);
	LoadAndResetGridTab();
	GetZHBBlockNum();
}

//港交所休市日
void CMainFrame::OnHkJyTime()
{
	CString tmpURL = HK_JYTIME_WEBPAGE;
	CTime ct = CTime::GetCurrentTime();
	CString tmpReplaceYear;
	tmpReplaceYear.Format("%02d",ct.GetYear()%100);
	tmpURL.Replace("#ShortYear#",tmpReplaceYear);
	ShellExecute(NULL, _T("open"), tmpURL, NULL, NULL, SW_SHOW);
}

#include "PadListDlg.h"
void CMainFrame::OnPadList()
{
	CPadListDlg dlg;
	dlg.DoModal();
	if(dlg.m_bModified)
		FillGnTree();
	if(dlg.m_bModified || dlg.m_bSwitchShow)
	{
		for(int i = 0;i<theViewManager.GetWindowNum();i++)
		{
			CView *tempView = (CView *)theViewManager.arViews[i];
			if(tempView->IsKindOf(RUNTIME_CLASS(CComView2)))
				((CComView2 *)tempView)->SendMessage(UM_REFRESHCUSTOMBAR);
		}
	}
}

void CMainFrame::ShowTabCtrlBar(int nFlag,BOOL bShow)
{
	if(nFlag==0)
		ShowControlBar(&m_wndWorkspace,bShow,FALSE);
	else if(nFlag==1)
		ShowControlBar(&m_wndRestspace,bShow,FALSE);
}

void CMainFrame::OnPageup() 
{
	if(theViewManager.pActiveView)
		theViewManager.pActiveView->PostMessage(WM_COMMAND,ID_PAGEUP);
}

void CMainFrame::OnPagedown() 
{
	if(theViewManager.pActiveView)
		theViewManager.pActiveView->PostMessage(WM_COMMAND,ID_PAGEDOWN);
}

#include "GSDlg.h"
#include "DownHttpFileDlg.h"
extern BOOL ProcessVsatLocalFile(BOOL bBG,BYTE *ContentBuf,DWORD nLen,const char *strDestPath);
extern BOOL DownHttpFile(const char *url,BYTE* & ContentBuf,DWORD & nLen);

void ProcessVsatUrl(BOOL bBG,CWnd *pWnd,CString tmpURL)
{
	BYTE *	pBuf = NULL;
	DWORD	nLen = 0;
	if(!DownHttpFile(tmpURL,pBuf,nLen))
	{
		TDX_MessageBox(pWnd->m_hWnd,"取数据失败!",MB_OK|MB_ICONERROR);
		return;
	}
	if(pBuf)
	{
		ProcessVsatLocalFile(bBG,pBuf,nLen,g_WSXHStr);
		TDEL(pBuf);
		OpenCusWebPage(pWnd,g_WSXHStr+"meta_vsat.htm",TRUE);
	}
}

void CMainFrame::OnSpecailZX(WPARAM wParam,LPARAM lParam)
{
	struct CcCache_Info pCcInfo[100];
	int nCCNum = ReadCCCookie(pCcInfo,100);
	//
	BYTE ZXSecurePackage[1024]={0};
	ReadZXCookie(NULL,MAX_ZXCOOKIE_NUM,ZXSecurePackage);
	struct VSatZx_Info *pInfo = (struct VSatZx_Info *)ZXSecurePackage;
	
	if(pInfo->m_lYBGPnum<1)		pInfo->m_lYBGPnum=5;
	if(pInfo->m_lYBZXnum<1)		pInfo->m_lYBZXnum=5;
	if(pInfo->m_lCCGPnum<1)		pInfo->m_lCCGPnum=5;
	if(pInfo->m_lCCZXnum<1)		pInfo->m_lCCZXnum=5;
	if(pInfo->m_lXXSDGPnum<1)	pInfo->m_lXXSDGPnum=5;
	if(pInfo->m_lXXSDZXnum<1)	pInfo->m_lXXSDZXnum=5;
	switch(wParam)
	{
	case 0:		//如果维赛特股搜的话
		{
			CGSDlg Gsdlg;
			if(Gsdlg.DoModal()==IDOK)
			{
				if(strlen(Gsdlg.m_strGPCode)==0) break;
				CString tmpURL;
				tmpURL.Format("%s/YCPJ/YJBGByCodes.aspx?codes=%s&topcount=%d",g_OEM.strVsatHttp,Gsdlg.m_strGPCode,pInfo->m_lYBZXnum);
				ProcessVsatUrl(TRUE,this,tmpURL);
			}
		}
		break;
	case 1:		//如果维赛特持仓报告的话
	case 2:		//如果维赛特信息速递的话
		{
			int GPnum = (wParam==1)?pInfo->m_lCCGPnum:pInfo->m_lXXSDGPnum;
			int ZXNum = (wParam==1)?pInfo->m_lCCZXnum:pInfo->m_lXXSDZXnum;
			CString strCodes="";
			for(int k=0;k<min(GPnum,nCCNum);k++)
			{
				if(k>0) strCodes+=",";
				strCodes+=pCcInfo[k].m_szZZQDM;
			}
			CString tmpURL;			
			if(wParam==1)
			{
				tmpURL.Format("%s/YCPJ/YJBGByCodes.aspx?codes=%s&topcount=%d",g_OEM.strVsatHttp,strCodes,ZXNum);
				ProcessVsatUrl(TRUE,this,tmpURL);
			}
			else
			{
				tmpURL.Format("%s/YCPJ/GSGGByCodes.aspx?codes=%s&topcount=%d",g_OEM.strVsatHttp,strCodes,ZXNum);
				ProcessVsatUrl(FALSE,this,tmpURL);
			}
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
CMDIChildWnd* CMainFrame::GetNextMDIChildWnd()
{
	if (!m_pWndCurrentChild)
	{
		// Get the first child window.
		m_pWndCurrentChild = m_wndMDIClient.GetWindow(GW_CHILD);
	}
	else
	{
		// Get the next child window in the list.
        m_pWndCurrentChild=
			(CMDIChildWnd*)m_pWndCurrentChild->GetWindow(GW_HWNDNEXT);
	}
	
	if (!m_pWndCurrentChild)
	{
		// No child windows exist in the MDIClient,
		// or you are at the end of the list. This check
		// will terminate any recursion.
		return NULL;
	}
	
	// Check the kind of window
    if (!m_pWndCurrentChild->GetWindow(GW_OWNER))
	{
        if (m_pWndCurrentChild->
			IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
		{
			// CMDIChildWnd or a derived class.
			return (CMDIChildWnd*)m_pWndCurrentChild;
		}
        else
		{
			CString Title; 
			m_pWndCurrentChild->GetWindowText(Title);
		//	if(Title.Find("股东人数")>=0)
		//		m_pWndCurrentChild->BringWindowToTop();
			// Window is foreign to the MFC framework.
			// Check the next window in the list recursively.
			return GetNextMDIChildWnd();
		}
	}
    else
	{
		// Title window associated with an iconized child window.
		// Recurse over the window manager's list of windows.
		return GetNextMDIChildWnd();
	}
}

CWnd* CMainFrame::GetNextMDIChildWnd_All()
{
	if (!m_pWndCurrentChild)
	{
		// Get the first child window.
		m_pWndCurrentChild = m_wndMDIClient.GetWindow(GW_CHILD);
	}
	else
	{
		// Get the next child window in the list.
        m_pWndCurrentChild=
			(CMDIChildWnd*)m_pWndCurrentChild->GetWindow(GW_HWNDNEXT);
	}
	
	if (!m_pWndCurrentChild)
	{
		// No child windows exist in the MDIClient,
		// or you are at the end of the list. This check
		// will terminate any recursion.
		return NULL;
	}
	
	// Check the kind of window
    if (!m_pWndCurrentChild->GetWindow(GW_OWNER))
	{
        return m_pWndCurrentChild;
	}
    else
	{
		// Title window associated with an iconized child window.
		// Recurse over the window manager's list of windows.
		return GetNextMDIChildWnd();
	}
}

void CMainFrame::BringMDI2Top(char *SubTitleString)
{
	if (!m_pWndCurrentChild)
	{
		// Get the first child window.
		m_pWndCurrentChild = m_wndMDIClient.GetWindow(GW_CHILD);
	}
	else
	{
		// Get the next child window in the list.
        m_pWndCurrentChild=
			(CMDIChildWnd*)m_pWndCurrentChild->GetWindow(GW_HWNDNEXT);
	}
	
	if (!m_pWndCurrentChild)
	{
		// No child windows exist in the MDIClient,
		// or you are at the end of the list. This check
		// will terminate any recursion.
		return;
	}
	
	// Check the kind of window
    if (!m_pWndCurrentChild->GetWindow(GW_OWNER))
	{
        if (m_pWndCurrentChild->
			IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
		{
			CString Title; 
			m_pWndCurrentChild->GetWindowText(Title);
			if(Title.Find(SubTitleString)>=0)
			{
				m_pWndCurrentChild->BringWindowToTop();
				m_pWndCurrentChild = NULL;
				return;
			}
			// CMDIChildWnd or a derived class.
			BringMDI2Top(SubTitleString);
		}
        else
		{
			CString Title; 
			m_pWndCurrentChild->GetWindowText(Title);
			if(Title.Find(SubTitleString)>=0)
			{
				m_pWndCurrentChild->BringWindowToTop();
				m_pWndCurrentChild = NULL;
				return;
			}
			// Window is foreign to the MFC framework.
			// Check the next window in the list recursively.
			BringMDI2Top(SubTitleString);
		}
	}
    else
	{
		// Title window associated with an iconized child window.
		// Recurse over the window manager's list of windows.
		BringMDI2Top(SubTitleString);
	}
}

int CMainFrame::GetCountCMDIChildWnds()
{
	int count = 0;
	
	CMDIChildWnd* pChild = GetNextMDIChildWnd();
	while (pChild)
	{
		count++;
		pChild = GetNextMDIChildWnd();
	}
	return count;
}

//////////////////////////////////////////////////////////////////////////
#include "QuickFlushTjDlg.h"
char *Tscp_21_Zb[21] = {"大盘风向","趋势逆转","操盘高手","红绿信号","超级波段","大师解盘","发现庄迹","主力追踪","资金动向","底部建仓","机构控盘","金鹰踏庄","底部进攻","追踪黑马","顺势而为","金鹰猎庄","涨停追踪","金玉满堂","机构吸筹","捉庄散户","资金博弈"};
char *Info_5_Url[5] = {"111101azx.rjglpt.com/class.asp?id=47","111101azx.rjglpt.com/class.asp?id=48","111101azx.rjglpt.com/class.asp?id=49","111101azx.rjglpt.com/class.asp?id=50","111101azx.rjglpt.com/class.asp?id=51"};
char *Plugin_6_Url[6] = {"bainian.yhglpt.com/heju/hxnc.asp", "bainian.yhglpt.com/ggtx.asp", "bainian.yhglpt.com/jglhb.asp", "bainian.yhglpt.com/jggpc.asp", "bainian.yhglpt.com/dtzjfx.asp","juyingzx.kfglpt.com/jjzn.asp"};
char *NewPlugA_Url[8] = {"bainian.yhglpt.com/etz/dp1.asp", "bainian.yhglpt.com/etz/dp2.asp", "bainian.yhglpt.com/etz/dp3.asp", "bainian.yhglpt.com/etz/bk1.asp", "bainian.yhglpt.com/etz/bk2.asp", "bainian.yhglpt.com/etz/xg1.asp", "bainian.yhglpt.com/etz/xg2.asp", "bainian.yhglpt.com/etz/xg3.asp"};

char *JTC_ZBSet[3] = {"波段密码策略", "短线密码策略", "主力动向解密"};
void CMainFrame::OnJCTZBSetChange(UINT nID)
{
	int nZBSet = nID-IDC_JCTZBSET1;
	((CMainFrame*)AfxGetMainWnd())->JumpToZBIndex(JTC_ZBSet[nZBSet], -1, NULL);
}

void CMainFrame::OnZFJS_Cmd(UINT nMenuID)
{
	switch (nMenuID)
	{
	case IDC_ZFJS_PAD0:
		{
			CView *tmpView = FindViewThenActive("Grid");
			if(!tmpView)
				OpenNewView("Grid");
		}
		break;
	case IDC_ZFJS_PAD9:
		{
			PROCESS_INFORMATION piProcInfo;
			STARTUPINFO siStartupInfo;
			memset(&siStartupInfo,0,sizeof(STARTUPINFO));
			siStartupInfo.cb = sizeof(STARTUPINFO);
			CString path;
			path.Format("UFReport.exe",HomePath);
			CreateProcess(path,NULL,NULL,NULL,FALSE,0,NULL,HomePath,&siStartupInfo, &piProcInfo);
		}
		break;

	case IDC_ZFJS_10:
		{
			g_pMainWin->PostMessage(WM_COMMAND,ID_501);
		}
		break;
	case IDC_ZFJS_11:
		{
			g_pMainWin->PostMessage(WM_COMMAND,ID_300);
		}
		break;

	case IDC_ZFJS_20:
		{
			if(Tscp_21_Zb[nMenuID-IDC_ZFJS_20][0]==0) 
				break;
			JumpToZBIndex(Tscp_21_Zb[nMenuID-IDC_ZFJS_20],SH,SH_DPCODE);
		}
		break;
	case IDC_ZFJS_111:
	case IDC_ZFJS_112:
	case IDC_ZFJS_113:
	case IDC_ZFJS_114:
	case IDC_ZFJS_115:
	case IDC_ZFJS_116:
	case IDC_ZFJS_117:
	case IDC_ZFJS_118:
	case IDC_ZFJS_119:
	case IDC_ZFJS_120:
	case IDC_ZFJS_121:
	case IDC_ZFJS_122:
	case IDC_ZFJS_123:
	case IDC_ZFJS_124:
	case IDC_ZFJS_125:
	case IDC_ZFJS_126:
	case IDC_ZFJS_127:
	case IDC_ZFJS_128:
	case IDC_ZFJS_129:
	case IDC_ZFJS_130:
	{
			if(Tscp_21_Zb[nMenuID-IDC_ZFJS_110][0]==0) 
				break;

			long nIDIndex = GetSYSCurrentGPIndex();
			MemStkInfo *pStock = (*g_pStockIO)[nIDIndex];
			if(pStock)
				JumpToZBIndex(Tscp_21_Zb[nMenuID-IDC_ZFJS_110],pStock->setcode,pStock->Code);
			else 
				JumpToZBIndex(Tscp_21_Zb[nMenuID-IDC_ZFJS_110],0,"000001");
		}
		break;

	/*
	case IDC_ZFJS_30:
	case IDC_ZFJS_31:
	case IDC_ZFJS_32:
	case IDC_ZFJS_33:
	case IDC_ZFJS_34:
	case IDC_ZFJS_35:
	case IDC_ZFJS_36:
	case IDC_ZFJS_37:
		{
			if(Gghb_30_37[nMenuID-IDC_ZFJS_30][0]==0) 
				break;

			long nIDIndex = GetSYSCurrentGPIndex();
			MemStkInfo *pStock = (*g_pStockIO)[nIDIndex];
			if(testzs(pStock))
				JumpToZBIndex(Gghb_30_37[nMenuID-IDC_ZFJS_30],0,"000001");
			else
				JumpToZBIndex(Gghb_30_37[nMenuID-IDC_ZFJS_30],pStock->setcode,pStock->Code);
		}
		break;
	*/

	case IDC_ZFJS_40:
	case IDC_ZFJS_41:
	case IDC_ZFJS_42:
		{
			for(int i=0;i<g_vDynaCmdTab.size();i++)
			{
				if(g_vDynaCmdTab[i].DefPadSet==IDC_ZFJS_PAD4&&g_vDynaCmdTab[i].DefPadCmdID==nMenuID)
				{
					/*
					CQuickFlushTjDlg *pFlushDlg = new CQuickFlushTjDlg;
					pFlushDlg->m_nCalcType = CALC_ZSBLOCK;
					pFlushDlg->Create(IDD_FLUSHTJBLOCK_DIALOG);
					pFlushDlg->ShowWindow(SW_SHOW);
					pFlushDlg->CenterWindow();
					pFlushDlg->SendMessage(WM_COMMAND, IDOK);
					TDEL(pFlushDlg);
					*/
					/*
					CQuickFlushTjDlg FlushDlg;
					FlushDlg.m_nCalcType = CALC_ZSBLOCK;
					if(FlushDlg.DoModal()==IDOK)
						ConstructCustomPad(i);
					*/
					RefreshAllHq(5*60);
					ConstructCustomPad(i);
					break;
				}
			}
		}
		break;
	case IDC_ZFJS_50:
	case IDC_ZFJS_51:
	case IDC_ZFJS_52:
		{
			for(int i=0;i<g_vDynaCmdTab.size();i++)
			{
				if(g_vDynaCmdTab[i].DefPadSet==IDC_ZFJS_PAD5&&g_vDynaCmdTab[i].DefPadCmdID==nMenuID)
				{
					ConstructCustomPad(i);
					break;
				}
			}
		}
		break;

	case IDC_ZFJS_72:			//板块领航的位置改名为多空趋势，功能主体是原牛熊归栏功能
		{
			for(int i=0;i<g_vDynaCmdTab.size();i++)
			{
				if(g_vDynaCmdTab[i].DefPadCmdID==nMenuID)
				{
					CQuickFlushTjDlg FlushDlg;
					FlushDlg.m_nCalcType = CALC_DXINDEX1;
					if(FlushDlg.DoModal()==IDOK)
						ConstructCustomPad(i);
					break;
				}
			}
		}
		break;
	case IDC_ZFJS_70:
	case IDC_ZFJS_71:
	case IDC_ZFJS_73:
	case IDC_ZFJS_74:
		{
			for(int i=0;i<g_vDynaCmdTab.size();i++)
			{
				if(g_vDynaCmdTab[i].DefPadSet==IDC_ZFJS_PAD7&&g_vDynaCmdTab[i].DefPadCmdID==nMenuID)
				{
					ConstructCustomPad(i);
					break;
				}
			}
		}
		break;

	case IDC_ZFJS_60:
	case IDC_ZFJS_61:
	case IDC_ZFJS_62:
	case IDC_ZFJS_63:
	case IDC_ZFJS_64:
		{
			for(int i=0;i<g_vDynaCmdTab.size();i++)
			{
				if(g_vDynaCmdTab[i].DefPadSet==IDC_ZFJS_PAD6&&g_vDynaCmdTab[i].DefPadCmdID==nMenuID)
				{
					/*
					CQuickFlushTjDlg *pFlushDlg = new CQuickFlushTjDlg;
					pFlushDlg->m_nCalcType = CALC_DXINDEX1;
					pFlushDlg->Create(IDD_FLUSHTJBLOCK_DIALOG);
					pFlushDlg->ShowWindow(SW_SHOW);
					pFlushDlg->CenterWindow();
					pFlushDlg->SendMessage(WM_COMMAND, IDOK);
					TDEL(pFlushDlg);
					*/
					CQuickFlushTjDlg FlushDlg;
					FlushDlg.m_nCalcType = CALC_DXINDEX1;
					if(FlushDlg.DoModal()==IDOK)
						ConstructCustomPad(i);
					break;
				}
			}
		}
		break;

	case IDC_ZFJS_80:
	case IDC_ZFJS_81:
	case IDC_ZFJS_82:
	case IDC_ZFJS_83:
	case IDC_ZFJS_84:
		SendMessage(WM_COMMAND, IDC_ZFJS_20);
		break;
		
	case IDC_ZFJS_85:
	case IDC_ZFJS_86:
		{
			for(int i=0;i<g_vDynaCmdTab.size();i++)
			{
				if(g_vDynaCmdTab[i].DefPadCmdID==nMenuID)
				{
					CQuickFlushTjDlg FlushDlg;
					FlushDlg.m_nCalcType = CALC_DXINDEX2;
					if(FlushDlg.DoModal()==IDOK)
						ConstructCustomPad(i);
					break;
				}
			}
		}
		break;
	case IDC_ZFJS_90:
		{
			g_Scheme_King.HomePage.LoadString(IDS_NCURL0);
			SwitchToView("HomePage");
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CHomePageView)))
				theViewManager.pActiveView->SendMessage(UM_RELOAD_MSG);
		}
		break;
	case IDC_ZFJS_91:
	case IDC_ZFJS_92:
	case IDC_ZFJS_93:
	case IDC_ZFJS_94:
	case IDC_ZFJS_95:
	case IDC_ZFJS_96:
	case IDC_ZFJS_97:
	case IDC_ZFJS_98:
		{
			g_Scheme_King.HomePage.Format("bainian.yhglpt.com/ry/gn%d.asp", nMenuID-IDC_ZFJS_90);
			SwitchToView("HomePage");
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CHomePageView)))
				theViewManager.pActiveView->SendMessage(UM_RELOAD_MSG);
		}
		break;
	case IDC_ZFJS_99:
	case IDC_ZFJS_100:
	case IDC_ZFJS_101:
	case IDC_ZFJS_102:
	case IDC_ZFJS_103:
		{
			g_Scheme_King.HomePage.Format("%s", Plugin_6_Url[nMenuID-IDC_ZFJS_98]);
			SwitchToView("HomePage");
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CHomePageView)))
				theViewManager.pActiveView->SendMessage(UM_RELOAD_MSG);
		}
		break;
	case IDC_ZFJS_150:
	case IDC_ZFJS_151:
	case IDC_ZFJS_152:
	case IDC_ZFJS_153:
	case IDC_ZFJS_154:
	case IDC_ZFJS_155:
	case IDC_ZFJS_156:
	case IDC_ZFJS_157:
		{
			g_Scheme_King.HomePage.Format("%s", NewPlugA_Url[nMenuID-IDC_ZFJS_150]);
			SwitchToView("HomePage");
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CHomePageView)))
				theViewManager.pActiveView->SendMessage(UM_RELOAD_MSG);
		}
		break;
	case IDC_ZFJS_161:
	case IDC_ZFJS_162:
	case IDC_ZFJS_163:
	case IDC_ZFJS_164:
	case IDC_ZFJS_169:
	case IDC_ZFJS_170:
	case IDC_ZFJS_171:
	case IDC_ZFJS_172:
	case IDC_ZFJS_173:
	case IDC_ZFJS_174:
	case IDC_ZFJS_175:
	case IDC_ZFJS_176:
	case IDC_ZFJS_177:
	case IDC_ZFJS_178:
		{
			g_Scheme_King.HomePage.Format("bainian.yhglpt.com/etz/zx%d.asp", nMenuID-IDC_ZFJS_160);
			SwitchToView("HomePage");
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CHomePageView)))
				theViewManager.pActiveView->SendMessage(UM_RELOAD_MSG);
		}
		break;
	case IDC_ZFJS_179:
		{
			g_Scheme_King.HomePage.Format("bainian.yhglpt.com/zch/zxnc%d.asp",4-g_Scheme_King.nVersion);
			SwitchToView("HomePage");
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CHomePageView)))
				theViewManager.pActiveView->SendMessage(UM_RELOAD_MSG);
		}
		break;
	}
}

void CMainFrame::OnUpdate_ZFJS_Cmd(CCmdUI* pCmdUI)
{
	switch(pCmdUI->m_nID)
	{
	case IDC_ZFJS_PAD0:
		{
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CGridView)))
			{
				pCmdUI->SetCheck(1);
				return;
			}
			pCmdUI->SetCheck(0);
		}
		break;
	case IDC_ZFJS_PAD1:
		{
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CComView)))
			{
				pCmdUI->SetCheck(1);
				return;
			}
			pCmdUI->SetCheck(0);
		}
		break;
	case IDC_ZFJS_PAD4:
	case IDC_ZFJS_PAD5:
	case IDC_ZFJS_PAD6:
	case IDC_ZFJS_PAD7:
	case IDC_ZFJS_PAD8:
		{
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CComView2)))
			{
				CComView2 *pComView2 =  (CComView2*)theViewManager.pActiveView;
				if(pComView2->GetNowPadSet()==pCmdUI->m_nID)
				{
					pCmdUI->SetCheck(1);
					return;
				}
			}
			pCmdUI->SetCheck(0);
		}
		break;

	case IDC_ZFJS_10: 
		{
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CComView)))
			{
				CComView *pComView = (CComView*)theViewManager.pActiveView;
				pCmdUI->SetRadio(pComView->GetMainPad()==ZST_PAD);
			}
		}
		break;
	case IDC_ZFJS_11: 
		{
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CComView)))
			{
				CComView *pComView = (CComView*)theViewManager.pActiveView;
				pCmdUI->SetRadio(pComView->GetMainPad()==FXT_PAD); 
			}
		}
		break;
	case IDC_ZFJS_40:
	case IDC_ZFJS_41:
	case IDC_ZFJS_42:
		{
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CComView2)))
			{
				CComView2 *pComView2 = (CComView2*)theViewManager.pActiveView;
				pCmdUI->SetRadio(pComView2->GetNowPadSet()==IDC_ZFJS_PAD4&&pComView2->GetNowPadCmdID()==pCmdUI->m_nID);
			}
		}
		break;
	case IDC_ZFJS_50:
	case IDC_ZFJS_51:
	case IDC_ZFJS_52:
		{
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CComView2)))
			{
				CComView2 *pComView2 = (CComView2*)theViewManager.pActiveView;
				pCmdUI->SetRadio(pComView2->GetNowPadSet()==IDC_ZFJS_PAD5&&pComView2->GetNowPadCmdID()==pCmdUI->m_nID);
			}
		}
		break;
	case IDC_ZFJS_60:
	case IDC_ZFJS_61:
	case IDC_ZFJS_62:
	case IDC_ZFJS_63:
	case IDC_ZFJS_64:
	case IDC_ZFJS_65:
		{
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CComView2)))
			{
				CComView2 *pComView2 = (CComView2*)theViewManager.pActiveView;
				pCmdUI->SetRadio(pComView2->GetNowPadSet()==IDC_ZFJS_PAD6&&pComView2->GetNowPadCmdID()==pCmdUI->m_nID);
			}
		}
		break;
	case IDC_ZFJS_70:
	case IDC_ZFJS_71:
	case IDC_ZFJS_72:
	case IDC_ZFJS_73:
	case IDC_ZFJS_74:
		{
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CComView2)))
			{
				CComView2 *pComView2 = (CComView2*)theViewManager.pActiveView;
				pCmdUI->SetRadio(pComView2->GetNowPadCmdID()==pCmdUI->m_nID);
			}
		}
		break;
	case IDC_ZFJS_80:
	case IDC_ZFJS_81:
	case IDC_ZFJS_82:
	case IDC_ZFJS_83:
	case IDC_ZFJS_84:
		{
			//先计算大盘状态
			MemStkInfo *pDPInfo = g_pStockIO->Get_MemStkInfo("000001", SH);
			int nSignalFlag = -1;
			//先看是不是暴跌
			CURRDATA iDPData={0};
			g_pStockIO->ReadCurrStockData(pDPInfo, &iDPData);
			if(100.0f*(iDPData.Now-iDPData.Close)/iDPData.Close>2.0f)
				nSignalFlag = 5;
			else
			{
				g_DXIndexCalc.Lock();
				CalcDXIndex3(pDPInfo);
				g_DXIndexCalc.Unlock();
				nSignalFlag = pDPInfo->CalcRes3.fOut[0]+0.5f;
			}
			pCmdUI->Enable(nSignalFlag==pCmdUI->m_nID-IDC_ZFJS_80+1||IDC_ZFJS_85==pCmdUI->m_nID||IDC_ZFJS_86==pCmdUI->m_nID);

			//改版面文件
			if(nSignalFlag==pCmdUI->m_nID-IDC_ZFJS_80+1)
			{
				for(int i=0;i<g_vDynaCmdTab.size();i++)
				{
					if(g_vDynaCmdTab[i].DefPadSet==IDC_ZFJS_PAD8)			//特别处理
						break;
				}
				g_vDynaCmdTab[i].DefPadCmdID = pCmdUI->m_nID;
				CString padini = g_WSXHStr+(CString)"sheet\\"+g_vDynaCmdTab[i].EnName+".sp";
				char *PadName[5] = {"大盘上升策略", "大盘震荡策略", "大盘调整策略", "大盘下跌策略", "大盘行情策略"};
				WritePrivateProfileString("DEAFULTGP", "Name", PadName[nSignalFlag-1], padini);
				WritePrivateProfileString("DEAFULTGP", "DefPadCmdID", IntToStr(pCmdUI->m_nID), padini);
				WritePrivateProfileString("STEP0", "ConFlag", IntToStr(10+nSignalFlag-1), padini);
				strcpy(g_vDynaCmdTab[i].ChName, PadName[nSignalFlag-1]);
			}
			
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CComView2)))
			{
				CComView2 *pComView2 = (CComView2*)theViewManager.pActiveView;
				pCmdUI->SetRadio(pComView2->GetNowPadSet()==IDC_ZFJS_PAD8&&pComView2->GetNowPadCmdID()==pCmdUI->m_nID);
			}
		}
		break;
	case IDC_ZFJS_85:
	case IDC_ZFJS_86:
		{
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CComView2)))
			{
				CComView2 *pComView2 = (CComView2*)theViewManager.pActiveView;
				pCmdUI->SetRadio(pComView2->GetNowPadCmdID()==pCmdUI->m_nID);
			}
		}
		break;
	case IDC_ZFJS_90:
	case IDC_ZFJS_91:
	case IDC_ZFJS_92:
	case IDC_ZFJS_93:
	case IDC_ZFJS_94:
		{

		}
		break;
	default:
		pCmdUI->SetCheck(0);
		break;
	}
}

void CMainFrame::OnDefTool_Cmd(UINT nID)
{
	switch (nID)
	{
	case IDC_DEFTOOLGN1:
		g_Scheme_King.HomePage = g_InitDefHomePage;
		SwitchToView("HomePage");
		if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CHomePageView)))
			theViewManager.pActiveView->SendMessage(UM_RELOAD_MSG);
		break;
	case IDC_DEFTOOLGN2:
		SendMessage(WM_COMMAND, ID_RIGHT_TYPE15);
		break;
	case IDC_DEFTOOLGN3:
		SendMessage(WM_COMMAND, ID_SHOWVIP);
		break;
	case IDC_DEFTOOLGN4:
		SendMessage(WM_COMMAND, ID_SHOWZST);
		break;
	case IDC_DEFTOOLGN5:
		SendMessage(WM_COMMAND, ID_TOOLS);
		break;
	case IDC_DEFTOOLGN6:
		SendMessage(WM_COMMAND, ID_RIGHT_TYPE6);
		break;
	case IDC_DEFTOOLGN7:
		SendMessage(WM_COMMAND, ID_MULUNIT);
		break;
	case IDC_DEFTOOLGN8:
		SendMessage(WM_COMMAND, ID_DOWNLOAD);
		break;
	case IDC_DEFTOOLGN9:
		SendMessage(WM_COMMAND, IDC_ZFJS_74);
		break;
	case IDC_DEFTOOLGN10:
		SendMessage(WM_COMMAND, IDC_ZFJS_72);
		break;
		
	case IDC_DEFTOOLGN11:
		SendMessage(WM_COMMAND, IDC_JCTZBSET1);
		break;
	case IDC_DEFTOOLGN12:
		SendMessage(WM_COMMAND, IDC_JCTZBSET2);
		break;
	case IDC_DEFTOOLGN13:
		SendMessage(WM_COMMAND, IDC_JCTZBSET3);
		break;
	case IDC_DEFTOOLGN14:
		SendMessage(WM_COMMAND, IDC_JCTZBSET4);
		break;
	case IDC_DEFTOOLGN27:
		{
			g_Scheme_King.HomePage.Format("bainian.yhglpt.com/etz/xg2.asp");
			SwitchToView("HomePage");
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CHomePageView)))
				theViewManager.pActiveView->SendMessage(UM_RELOAD_MSG);
		}
		break;
	case IDC_DEFTOOLGN28:
		{
			g_Scheme_King.HomePage.Format("bainian.yhglpt.com/etz/xg3.asp");
			SwitchToView("HomePage");
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CHomePageView)))
				theViewManager.pActiveView->SendMessage(UM_RELOAD_MSG);
		}
		break;

	case IDC_DEFTOOLGN15:
		SendMessage(WM_COMMAND, IDC_ZFJS_176);
		break;
	case IDC_DEFTOOLGN16:
		SendMessage(WM_COMMAND, IDC_ZFJS_164);
		break;
	case IDC_DEFTOOLGN17:
		SendMessage(WM_COMMAND, IDC_ZFJS_172);
		break;
	case IDC_DEFTOOLGN18:
		SendMessage(WM_COMMAND, IDC_ZFJS_177);
		break;
	case IDC_DEFTOOLGN25:
		SendMessage(WM_COMMAND, IDC_ZFJS_173);
		break;
	case IDC_DEFTOOLGN19:
		SendMessage(WM_COMMAND, IDC_ZFJS_170);
		break;
	case IDC_DEFTOOLGN20:
		SendMessage(WM_COMMAND, IDC_ZFJS_171);
		break;
	case IDC_DEFTOOLGN21:
		SendMessage(WM_COMMAND, IDC_ZFJS_162);
		break;
	case IDC_DEFTOOLGN22:
		SendMessage(WM_COMMAND, IDC_ZFJS_161);
		break;
	case IDC_DEFTOOLGN23:
		SendMessage(WM_COMMAND, IDC_ZFJS_163);
		break;
	case IDC_DEFTOOLGN24:
		SendMessage(WM_COMMAND, IDC_ZFJS_152);
		break;

	case IDC_NOTSUPPORT:
		{
			TDX_MessageBox(GetSafeHwnd(), _T("您还没有开通该权限，权限开通请联系客服！"), MB_OK);
		}
		break;
	}
}

void CMainFrame::OnUpdate_DefTool_Cmd(CCmdUI* pCmdUI)
{
	return;
	switch(pCmdUI->m_nID)
	{
	case IDC_DEFTOOLGN3:
		if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CComView)))
		{
			CComView *pComView = (CComView*)theViewManager.pActiveView;
			pCmdUI->SetCheck(pComView->GetMainPad()==FXT_PAD); 
		}
		else 
			pCmdUI->SetCheck(0);
		break;
	case IDC_DEFTOOLGN4:
		if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CComView)))
		{
			CComView *pComView = (CComView*)theViewManager.pActiveView;
			pCmdUI->SetCheck(pComView->GetMainPad()==ZST_PAD);
		}
		else 
			pCmdUI->SetCheck(0);
		break;
	case IDC_DEFTOOLGN6:
		if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CGridView)))
		{
			CComView *pComView = (CComView*)theViewManager.pActiveView;
			pCmdUI->SetCheck(1);
		}
		else 
			pCmdUI->SetCheck(0);
		break;
	case IDC_DEFTOOLGN9:
	case IDC_DEFTOOLGN10:
		if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CComView2)))
		{
			UINT uTrueCmdID = 0;
			if(pCmdUI->m_nID==IDC_DEFTOOLGN2) uTrueCmdID=IDC_ZFJS_74;
			else if(pCmdUI->m_nID==IDC_DEFTOOLGN6) uTrueCmdID=IDC_ZFJS_72;
			
			CComView2 *pComView2 = (CComView2*)theViewManager.pActiveView;
			pCmdUI->SetCheck(pComView2->GetNowPadSet()==IDC_ZFJS_PAD7&&pComView2->GetNowPadCmdID()==uTrueCmdID);
		}
		else 
			pCmdUI->SetCheck(0);
		break;
	}
}

void CMainFrame::OnOpenIWContent(WPARAM wParam,LPARAM lParam)
{
	if(m_pIWContentDlg)
	{
		if (m_pIWContentDlg->IsIconic()) 
			m_pIWContentDlg->ShowWindow(SW_RESTORE); 
		if(wParam==1) //修改URL
		{
			m_pIWContentDlg->SetTitle(_F(g_strIWTitle));
			m_pIWContentDlg->SetInfo(0,0,g_strIWURL);
			m_pIWContentDlg->StartNavigate();
		}
	}
	else
	{
		//创建有模式对话框，一定要用指针动态创建
		m_pIWContentDlg = new CShowURL(this, TRUE);
		m_pIWContentDlg->m_strTitle = _F(g_strIWTitle);
		m_pIWContentDlg->SetInfo(0,0,g_strIWURL);
		m_pIWContentDlg->Create(IDD_SHOWURL);
		m_pIWContentDlg->ShowWindow(SW_SHOW);
	}	
}

void CMainFrame::OnCLXG()
{
	//先计算大盘状态
	MemStkInfo *pDPInfo = g_pStockIO->Get_MemStkInfo("000001", SH);
	int nSignalFlag = -1;
	//先看是不是暴跌
	CURRDATA iDPData={0};
	g_pStockIO->ReadCurrStockData(pDPInfo, &iDPData);
	if(100.0f*(iDPData.Now-iDPData.Close)/iDPData.Close>2.0f)
		nSignalFlag = 5;
	else
	{
		g_DXIndexCalc.Lock();
		CalcDXIndex3(pDPInfo);
		g_DXIndexCalc.Unlock();
		nSignalFlag = pDPInfo->CalcRes3.fOut[0]+0.5f;
	}

	UINT nID = nSignalFlag+IDC_ZFJS_80-1;

	for(int i=0;i<g_vDynaCmdTab.size();i++)
	{
		if(g_vDynaCmdTab[i].DefPadSet==IDC_ZFJS_PAD8)
			break;
	}
	CString padini = g_WSXHStr+(CString)"sheet\\"+g_vDynaCmdTab[i].EnName+".sp";
	char *PadName[5] = {"大盘上升策略", "大盘震荡策略", "大盘调整策略", "大盘下跌策略", "大盘行情策略"};
	WritePrivateProfileString("DEAFULTGP", "Name", PadName[nSignalFlag-1], padini);
	WritePrivateProfileString("DEAFULTGP", "DefPadCmdID", IntToStr(nID), padini);
	WritePrivateProfileString("STEP0", "ConFlag", IntToStr(10+nSignalFlag-1), padini);

	SendMessage(WM_COMMAND, nID);
}

//////////////////////////////////////////////////////////////////////////
//指标跳转
void CMainFrame::JumpToZBIndex(char *ZBCode, short nSetcode, char *GPCode, BOOL bIsKLine)
{
	if(GPCode==NULL)
	{
		if(bIsKLine) return;
		int MessageNo = GetMessageNoFromZB(ZBCode,FALSE);
		OpenComView(1,NULL,0,MessageNo);
		//特殊处理中金的模板
		if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)))	
		{
			CComView *pView = (CComView *)theViewManager.pActiveView;
			if(strcmp("波段密码策略", ZBCode)==0)
			{
				pView->SendMessage(WM_COMMAND, ID_DAY);
				pView->JumpToZB("", 3);
				pView->JumpToZB("波段密码", 0);
				pView->JumpToZB("VOL", 1);
				pView->JumpToZB("MACD", 2);
			}
			else if(strcmp("短线密码策略", ZBCode)==0)
			{
				pView->SendMessage(WM_COMMAND, ID_MIN15);
				pView->JumpToZB("", 3);
				pView->JumpToZB("波段密码", 0);
				pView->JumpToZB("VOL", 1);
				pView->JumpToZB("MACD", 2);
			}
			else if(strcmp("主力动向解密", ZBCode)==0)
			{
				pView->SendMessage(WM_COMMAND, ID_DAY);
				pView->JumpToZB("", 3);
				pView->JumpToZB("波段密码", 0);
				pView->JumpToZB("ZLJK", 1);
				pView->JumpToZB("SHJK", 2);
			}
			pView->Invalidate();
		}
		return;
	}
	MemStkInfo *pStock = g_pStockIO->Get_MemStkInfo(GPCode, nSetcode);
	if(!pStock)
	{
		long nIDIndex = GetSYSCurrentGPIndex();
		pStock	= (*g_pStockIO)[nIDIndex];
		if(!pStock) return;
	}
	OpenComView(2,pStock->Code, pStock->setcode,300,0,NULL,-1,-1,NULL);
	if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)))	
	{
		CComView *pView = (CComView *)theViewManager.pActiveView;
		if(strcmp("波段密码策略", ZBCode)==0)
		{
			pView->SendMessage(WM_COMMAND, ID_DAY);
			pView->JumpToZB("", 3);
			pView->JumpToZB("波段密码", 0);
			pView->JumpToZB("VOL", 1);
			pView->JumpToZB("MACD", 2);
		}
		else if(strcmp("短线密码策略", ZBCode)==0)
		{
			pView->SendMessage(WM_COMMAND, ID_MIN15);
			pView->JumpToZB("", 3);
			pView->JumpToZB("波段密码", 0);
			pView->JumpToZB("VOL", 1);
			pView->JumpToZB("MACD", 2);
		}
		else if(strcmp("主力动向解密", ZBCode)==0)
		{
			pView->SendMessage(WM_COMMAND, ID_MIN15);
			pView->JumpToZB("", 3);
			pView->JumpToZB("波段密码", 0);
			pView->JumpToZB("ZLJK", 1);
			pView->JumpToZB("SHJK", 2);
		}
		else 
			pView->JumpToZB(ZBCode);
		pView->Invalidate();
	}
}

void CMainFrame::OnPopMsg(WPARAM wParam,LPARAM lParam)
{
	if(wParam==0)
	{
		if(OnLine)
		{
			g_pComm->Disconnect();
			OnLine = FALSE;
			TDX_MessageBox(m_hWnd,(char*)(lParam),MB_ICONINFORMATION|MB_OK);
		}		
	}
}

void CMainFrame::OnPopMyIE(WPARAM wParam,LPARAM lParam)
{
	CString Ini = g_WSXHStr+"pop.ini";
	
	//判断时间
	CTime ct = CTime::GetCurrentTime();
	int nDate = GetPrivateProfileInt("POP","Date",0,Ini);
	int nTime = GetPrivateProfileInt("POP","Time",0,Ini);
	if(nDate<1&&nTime<1) return;
	if(nDate>0&&nDate!=10000*ct.GetYear()+100*ct.GetMonth()+ct.GetDay()) return;
	if(nTime>0)
	{
		int nNewPopMin = (60*nTime/10000+(nTime/100)%100);
		int nNowMin = 60*ct.GetHour()+ct.GetMinute();
		int nValidTime = GetPrivateProfileInt("POP","ValidTime",10,Ini);
		if(g_nPopInfoLastTime>=nTime||nNowMin-nNewPopMin>nValidTime) 
		{
			g_nPopInfoLastTime=nTime;
			return;
		}
	}
	/*
	//判断权限
	char strAuthoritys[MAX_PATH]={0};
	GetPrivateProfileString("POOL","AUTHORITY","",strAuthoritys,MAX_PATH-1,Ini);
	char sa[3]={0};
	sprintf(sa, "%01d", g_nDefUserLevel);
	if(strstr(strAuthoritys, sa)==NULL)
		return;
	*/
	
	int nHeight = GetPrivateProfileInt("POP","Height",240,Ini);
	int nWidth = GetPrivateProfileInt("POP","Width",360,Ini);
	char Title[MAX_PATH]={0}, Url[MAX_PATH]={0};
	GetPrivateProfileString("POP","Title","",Title,MAX_PATH-1,Ini);
	GetPrivateProfileString("POP","Url","",Url,MAX_PATH-1,Ini);
	CRect rc;
	GetWindowRect(&rc);
	
	if(m_pPopMyIEDlg)
	{
		if (m_pPopMyIEDlg->IsIconic()) 
			m_pPopMyIEDlg->ShowWindow(SW_RESTORE);
		m_pPopMyIEDlg->SetWindowPos(this, rc.right-nWidth, rc.bottom-nHeight, nWidth, nHeight, SWP_SHOWWINDOW);
		m_pPopMyIEDlg->SetTitle(_F(Title));
		m_pPopMyIEDlg->SetInfo(0,0,Url);
		m_pPopMyIEDlg->StartNavigate(Url);
	}
	else
	{
		//创建有模式对话框，一定要用指针动态创建
		m_pPopMyIEDlg = new CShowURL(this,FALSE,TRUE);
		m_pPopMyIEDlg->m_strTitle = _F(Title);
		m_pPopMyIEDlg->SetInfo(0,0,Url);
		m_pPopMyIEDlg->Create(IDD_SHOWURL);
		MakeWindowTransparent(m_pPopMyIEDlg->GetSafeHwnd(), 0.8f*MWT_MAX_FACTOR);
		m_pPopMyIEDlg->SetWindowPos(this, rc.right-nWidth, rc.bottom-nHeight, nWidth, nHeight, SWP_SHOWWINDOW);
		m_pPopMyIEDlg->StartNavigate(Url);
	}
	
	g_nPopInfoLastTime=nTime;
}

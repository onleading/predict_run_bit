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
#include "dbf.h"  //dbf��̬���ӿ��ͷ�ļ�
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
//���ȡ�ʽ��ʺŵ���Ϣ
UINT QryZJZHMsg = RegisterWindowMessage("Query_Account");
UINT RetZJZHMsg = RegisterWindowMessage("Return_Account");
UINT ExtMsg = RegisterWindowMessage("Exit_Getmsg");
UINT QryZJZHMsg_My = 0;
UINT RetZJZHMsg_My = 0;
UINT ExtMsg_My = 0;
//�ϴ�ѡ�е�ί�г���
long l_nSelExeIndex=0;
//��������ͳ�����ݵĻ�����
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
	struct tm t;            //����tmʱ��ṹ�������洢ʱ���ʽ��������Ϣ
	time_t t_of_day;     //����time_tʱ��ṹ
	t.tm_year=g_lOpenRq/10000-1900;//��1900��Ϊ��׼����ʱ��
	t.tm_mon=(g_lOpenRq/100)%100;                 //Ϊ�ṹ���Ա��ֵ
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
	//װ������
	CString tempFileName = g_WSXHStr+"conblk.cos";
	CFile m_File;
	if( m_File.Open(tempFileName,CFile::modeRead|CFile::shareDenyNone) )
	{
		//�����ļ�ͷ
		m_File.Read(&FileHead,sizeof(FILEHEAD));
		ConditionNum = FileHead.ConditionNum;
		CString tempStr;
		for(int i = 0; i < ConditionNum ;i++)
			//�����������
			m_File.Read(&ConditionInfo[i],sizeof(CONDITIONINFO));
		m_File.Close();
	}

	int		nCurrPeriod = DAY_ST;
	if(ConditionNum < 1)
	{
		//����������
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
		for(j = 0;j < m_pCalcOnce->_CallDTopViewCalc_7(TJ_TYPE);j++)//����ǴӴ����ļ���ȡ�õ���������Ӧ����з������
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

	//������ȡ�����з��������Ĺ�Ʊ
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
		//����������
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
	int SuccessFlag;					//0:���� 1:�ɹ�	-1:ʧ�� -2:�׶�����
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

	//����������
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
		//�����б���
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

	//��¼ˢ��ʱ��
	struct tm t1;            //����tmʱ��ṹ�������洢ʱ���ʽ��������Ϣ
	time_t t_of_data1;     //����time_tʱ��ṹ
	CTime ct = CTime::GetCurrentTime();
	t1.tm_year=ct.GetYear()-1900;//��1900��Ϊ��׼����ʱ��
	t1.tm_mon=ct.GetMonth();                 //Ϊ�ṹ���Ա��ֵ
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
	//��ר��ϵͳ�йصĺ���
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

//״̬������
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
	m_NCInfo.bVitalDown = -1; //��ʾһ����û�б�����
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

//���߹��ߵ�ID����
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
	//���� ���ȡ�ʽ��ʺŵ���Ϣ
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
	//���ں�һ��,�����û�����ʹ��ʱ��չ����(DSMustJy���ش򿪵Ļ�)
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
	//���CmdTable��Oftenָ��
	Refresh_OftenUseZb_CmdTable();

	//��ʼ�������һЩ����
	InitCreateInfo();

	//�ж��Ƿ���ʵʱ��չ����
	TestAutoDS();	
	//
	if(g_bHasCaption) g_bDynaZoom = 0;
	if(g_bHasDS && g_nFixedWidth<198) g_nFixedWidth=198;
	if(g_bShowVersoin)
		SetWindowText(_F(g_strTitleName+" V"+g_szReleaseVersion));
	else
		SetWindowText(_F(g_strTitleName));
	//////////////////////
	//������
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
	//������m_wndToolBar�ʸ���������
	m_wndToolBar.LoadTrueColorToolBar(nToolBarIconSize, nToolBarID,nToolBarID_Hot,nToolBarID_Diable);
	m_bStaticToolBar = TRUE;
	if(m_wndToolBar.m_hWnd) g_pMyToolBar = &m_wndToolBar;
	//////////////////////
	//״̬��	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;
	}
	//����״̬���ĸ߶�
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
	//��ʱ��
	SetTimer(1,StatusRefreshTime*1000,NULL);	//����״̬��ˢ�µĶ�ʱ��
	SetTimer(2,4*1000,NULL);					//������صĶ�ʱ��
	SetTimer(3,3000,NULL);						//���ö�̬ά��
	
	SetTimer(4,100,NULL);						//��ʱ���g_pComm�Ƿ񼤻�
	SetTimer(5,DETECT_SECOND*1000,NULL);		//��ʱ����Ѷ���������ʹ���,�Զ�̽����������,�����Զ�����

	SetTimer(6,12*1000,NULL);					//��ѡ�ɵ�����ˢ��
	SetTimer(7,g_nWarnRefresh*1000,NULL);		//Ԥ������ˢ��
	SetTimer(8,1000,NULL);						//�ж�g_pCommͨѶ��ʱ
	SetTimer(9,6*1000,NULL);					//����ͳ�����ݵ�ˢ��
	SetTimer(10,8*1000,NULL);					//����ͳ�����ݵ�ˢ��
//	SetTimer(13, 30*1000, NULL);
//	SetTimer(14, 10*1000, NULL);
	
	if(g_AdHead.m_nADNum>0)
		SetTimer(12,g_AdHead.m_nADMinRandPopTime*60*1000,NULL);	//��������Ϣ�Ķ�ʱ��
	SetTimer(27, 200, NULL);
	m_bJyVerify_Success = LocalJyVerify();
	//���û��У��,�Ͳ��Զ�������Ѷ
	if(!m_bJyVerify_Success && g_VipVerify_Info.DenyInfo)
		g_bAutoConnectInfo = FALSE;
	//////////////////////
	m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
	EnableDocking(CBRS_ALIGN_TOP,FALSE);
	DockControlBar(&m_wndToolBar);
	ShowControlBar(&m_wndToolBar,g_Scheme_King.bShowToolBar,FALSE);
	//////////////////////
	//���߹�����
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
	m_DrawToolsBar.SetWindowText(_F("���߷���"));
	m_DrawToolsBar.EnableDocking(0);
	m_DrawToolsBar.SetColumns(14);
	CPoint pt(GetSystemMetrics(SM_CXSCREEN)-100,GetSystemMetrics(SM_CYSCREEN)/10);
	FloatControlBar(&m_DrawToolsBar, pt);

	//////////////////////////////////////////////////////////////////////////
	//���ڹ�����
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
	//������
	m_ImageList.Create(IDB_IMAGELIST_TREE, 13, 1, RGB(0,255,0));
	m_wndWorkspace.SetBtnImageList (&m_ImageList);
	m_wndWorkspace.SetControlBarStyle(TRUE,FALSE);
	if (!m_wndWorkspace.Create(this, CG_ID_VIEW_WORKSPACE,
		_F("������"), CSize(g_nDefaultZxWidth,0)))
	{
		TRACE0("Failed to create dialog bar m_wndWorkspace\n");
		return -1;
	}
	memset(g_nTreePos,-1,sizeof(long)*20);//ȫ������ֵ
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
			m_wndWorkspace.AddWnd(_F("�г���Ϣ"),RUNTIME_CLASS(CZxTree));
			g_nTreePos[ZX_TREETYPE] = nPos++;
		}
		else
		{
			m_wndWorkspace.AddWnd(_F("�г���Ϣ"),RUNTIME_CLASS(CZxTree));
			g_nTreePos[ZX_TREETYPE] = nPos++;
			m_wndWorkspace.AddWnd(_F(g_OEM.CustomQsZxTreeName),RUNTIME_CLASS(CZx2Tree));
			g_nTreePos[ZX2_TREETYPE] = nPos++;
			g_GC.bHasZxTree23=TRUE;
		}
		m_wndWorkspace.AddWnd(_F("����ָ��"),RUNTIME_CLASS(CZbTree));
		g_nTreePos[ZB_TREETYPE] = nPos++;			
	}
	else
	{
		m_wndWorkspace.AddWnd(_F("����"),RUNTIME_CLASS(CGnTree));
		g_nTreePos[GN_TREETYPE] = nPos++;
		m_wndWorkspace.AddWnd(_F("��Ѷ"),RUNTIME_CLASS(CZxTree));
		g_nTreePos[ZX_TREETYPE] = nPos++;
		m_wndWorkspace.AddWnd(_F("��Ʊ"),RUNTIME_CLASS(CGpTree));
		g_nTreePos[GP_TREETYPE] = nPos++;
		m_wndWorkspace.AddWnd(_F("ָ��"),RUNTIME_CLASS(CZbTree));
		g_nTreePos[ZB_TREETYPE] = nPos++;
	}
#else
	if(g_bHasWebInfo)
	{
		m_wndWorkspace.AddWnd(_F("����"),RUNTIME_CLASS(CWebTree));
		g_nTreePos[WEB_TREETYPE] = nPos++;
	}
	m_wndWorkspace.AddWnd(_F("ָ�깫ʽ"),RUNTIME_CLASS(CZbTree));
	g_nTreePos[ZB_TREETYPE] = nPos++;
	m_wndWorkspace.AddWnd(_F("������"),RUNTIME_CLASS(CGpTree));
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
	//������
	m_wndRestspace.SetBtnImageList (&m_ImageList);
	m_wndRestspace.SetControlBarStyle(TRUE,TRUE);
	m_wndRestspace.SetRightBtn(g_bRestRightBtn);
	if (!m_wndRestspace.Create(this, CG_ID_VIEW_RESTSPACE,
		_F("������"), CSize(220,g_nDefaultRestHeight)))
	{
		TRACE0("Failed to create dialog bar m_wndWorkspace\n");
		return -1;
	}
#ifdef OEM_NEWJY
	m_wndRestspace.SetGripper(FALSE);
#endif
	memset(g_nRestPos,-1,sizeof(long)*20);//ȫ������ֵ
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
	SetTimer(30, 5000, NULL); //���������ֹ��ֹӮ�Ĺ���ʵ��
#else
	m_wndRestspace.ResetSize();
#endif
	///////////////////
	LayoutFlag = TRUE;

	//����ս���ʱ��һЩ��Ϣ
	PostMessage(UM_ONLINEENTERMSG,0,0);
	
	g_pMainWin = this;

	//�����̨������
	g_pBkData = new CBkData;
//	g_pBkData->Create(this);

	//��ʼ�����ֱ�ʶģ��
	g_pToolTipMark = new CToolTipMark(this);
	if(!g_pToolTipMark || !g_pToolTipMark->InitTool())
	{
		TDEL(g_pToolTipMark);
	}
	//��ʼ��Ԥ��������
	m_WarnManager.InitWarnManager(this);
	//�ȵõ�״̬��
	SendMessage(UM_STATUS_REQ,0); 
	SendMessage(UM_STATUS_REQ,1); 
	//������Ѷ��վ

	//������չ������վ
	if(g_bHasDS && m_bAutoDS && OnLine)
		PostMessage(UM_AUTOCONNECTDS,0,0);
	//������Ѷ�Ĵ���
	if(g_GH.Roll.bShow && g_GH.Roll.bFloat)
		PostMessage(WM_COMMAND,IDM_MOVINGINFO);
	//���ÿ�������DLL
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
	//�����ⲿִ�д��룬����try.catch����
	try
	{
		//������������������,�Զ�����
		if(strlen(g_GC.AlongPathFile))
		{
			if(g_OEM.bZxjtEmbedWTVersion)
				PostMessage(WM_COMMAND,ID_FUNC_ZXJTINFO);
			else
				ExecExtern(g_GC.AlongPathFile,m_hWnd,NULL);
		}
		//��Ի��ְ���ر���
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
		//�Ƿ�һ����ʹ�ί�н���
	#ifndef OEM_NEWJY
		if(g_GC.bAlongOpenWT)
			PostMessage(WM_COMMAND,ID_FUNC_WT);
	#endif
		//�Ƿ�һ����ʹ򿪺��µ���Ѷ����
		if(g_GC.bAlongOpenHXInfo)
			PostMessage(WM_COMMAND,ID_FUNC_HXINFO);
		//����ͨ����TQϵͳ(����������,����ʾ)
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
    // �� MFC �������и����������ĳߴ磬�Ա�GetWindowRect�ĵ�׼ȷ���
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
    // ������ʹ��Ĭ�ϲ���ʱ��DockControlBar ���ڵ�����һ���︡����������
    // ͨ��������εĴ�С��ģ�⹤�����϶���ָ��λ��ʵ�ָ�����
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

//�Ƿ���Ҫ�ڲ˵����Ի���ť
BOOL CMainFrame::bCustDrawCloseBtn()
{
#ifdef OEM_STAR_HM
	return TRUE;
#endif
	return !g_bHasCaption;
}

//��������
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

	CNewMenu *pTopMenu = (CNewMenu *)GetMenu();						//�õ�ϵͳ�˵�
	if(!pTopMenu) return;
	int i,nItemNum = pTopMenu->GetMenuItemCount();					//�õ��˵�����
	CRect ItemRect;
	GetMenuItemRect(m_hWnd,pTopMenu->m_hMenu,nItemNum-1,&ItemRect); //���һ���˵����Rect
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
	CDC memDC;			//���������Ե�DC
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
	//���ڴ�DC��������ĻDC
	pDC->BitBlt (m_NCInfo.NcRect.left,m_NCInfo.NcRect.top,m_NCInfo.NcRect.Width(),m_NCInfo.NcRect.Height(),&memDC,0,0,SRCCOPY);
	memDC.SelectObject(pOldBitmap);
	tempBitmap.DeleteObject();
	memDC.DeleteDC();
	
	if(g_VitalBtn.BtnNum > 0)
	{
		m_NCInfo.NcRect.left = startx;
		m_NCInfo.NcRect.right = startx+tmpDiff;
	}
	//������С���͹رհ�ťλͼ
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
 		if(rect.PtInRect (point))  //�������ĳ���Ի���ť��
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
 	if(rect.PtInRect (point))  //�������ĳ���Ի���ť��
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
 			if(!rect.PtInRect (point))  //�����ĳ��ť�ϰ�ס��겢�Ƴ��˰�ť��Χ
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
			if(!rect.PtInRect(point))//�����ĳ��ť�ϰ�ס��겢�Ƴ��˰�ť��Χ
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
 		if(!rect.PtInRect(point))  //�������ĳ���Ի���ť��
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

//������С����ť�͹رհ�ť
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
					if(strnicmp(g_VitalBtn.BtnExecFile[i],"http",4)) //�����·��
					{
						char execfile[MAX_PATH];
						sprintf(execfile,"%s%s",HomePath,g_VitalBtn.BtnExecFile[i]);
						ExecExtern(execfile,m_hWnd);
					}
					else											 //���������
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

//�ⲿ����
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
	if(!g_bDirectQuit && !g_bSpecClose)	//����ǳ����ڲ��˳�������ʾ�˳�
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
		if(TDX_MessageBox (m_hWnd,"��ȷ���˳���ϵͳ��",MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL)
			return; 		
	#endif
	}
	//��ʾ�̺���������
	if(!g_bReLoginStatus && !g_bDirectQuit && !g_bSpecClose && g_bCanDown)
	{
		SYSTEMTIME  nowtime;
		GetCorrespondingTime( &nowtime );
		int m = nowtime.wHour*60+nowtime.wMinute;  //����
		if(g_bNotifyDownload && ( (m>940 && !g_bYesterdayFlag) || (m>630 && g_bYesterdayFlag) ) && GetVipDocLastDate()<g_lOpenRq)
			DownDayData_AtOnce(this,"�˳�ǰ,���Ƿ�Ҫ���ز�ȫ��������?\n\n(ѡ��,�������,�ѻ�����ȹ�����Ҫ��������,�����ʹ����Щ����,��������)");
	}

	//��ϵͳ�㲥�˳���Ϣ �˳�,WPARAM������Ϊ1
	::PostMessage(HWND_BROADCAST,ExtMsg,1,0);
	::PostMessage(HWND_BROADCAST,ExtMsg_My,1,0);
	//��Ի���֤ȯTQ�Ĵ���
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
		//��ȥȫ��״̬,�Ա�õ��������ȵ�״̬
		if(m_pWndFullScreen != NULL)	EscapeFullScreen();
		
		//�õ��ر�ʱ��������״̬
		bTreeShow		= m_wndWorkspace.IsWindowVisible(); 
		bRestShow		= m_wndRestspace.IsWindowVisible(); 
		if(!g_bAutoHideBar)
			bToolBarShow = m_wndToolBar.IsWindowVisible();
		bStatusBarShow	= m_wndStatusBar.IsWindowVisible();
		nLastTreeSel	= m_wndWorkspace.GetCurWnd();
		nLastRestSel	= m_wndRestspace.GetCurWnd();
	}
	
	if(g_bBossKey)
		UnregisterHotKey(m_hWnd,1001); //�رտ�������DLL
	try
	{
		if(g_OEM.bHasQSRun)
			QS_MainFrameClosed_(0);
		CloseMDIWindow(FALSE); //�ȹرո��Ӵ���	
		theViewManager.RemoveAll();
		theViewManager.bClosing = TRUE;
		//������OnClose��Ŵ�INI����Ϊ�ڸ��Ӵ��ڹر�ʱ��������FirstGPType����Ϣ
		WriteToUserINI();
		WriteToOther();

		//��Clipboard Viewer Chainע�������� 	
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
	CNewMDIFrameWnd::OnClose();	//���ڲŵ��û��ຯ��OnClose();
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

void CMainFrame::SendStatusReq(WPARAM wParam,LPARAM lParam) //wParam,0:״̬�� 1:ͳ������
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

//��ʱ��������
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

	if(nIDEvent == 1)		//ˢ��״̬��
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
	else if(nIDEvent == 2)//ˢ���������
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
	else if(FALSE&&nIDEvent == 3)//���ö�̬ά��
	{
		SYSTEMTIME  nowtime;
		GetLocalTime( &nowtime );
		unsigned short nowmin=nowtime.wHour*60+nowtime.wMinute;
		//��������,�����½�����վ
		if(nowmin < oldmin && OnLine)
		{
			//����վ�Ͽ�����
			g_pComm->Disconnect();
			g_nDSAutoConnectFlag = g_nAutoConnectFlag = AUTOCONNECT_RESET;
			//���������ӽ���
			LogoDlg_ReConnect();
		}
		oldmin = nowmin;
	}
	else if(nIDEvent == 4)//��ʱ���ͨѶ�Ƿ񼤻�
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
	else if(nIDEvent == 5)	//������Ѷ���������鱨������Ŀ��,�����Զ�����
	{
		DetetectCInt = (DetetectCInt+1)%30000;
		if(DetectHackProc())
		{
			exit(0);
			return;
		}
		else if(DetetectCInt%3==0)
		{			
			//��ֹ����ͨ��ɱ���̵ķ�ʽ��ʵ��һ�ʺŶ�ε�¼
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
		if(DSOnLine && g_pGetData) //�����г�����,����
		{
			g_nMainID = MainIDPlus(m_nMainID,MAIN_WIN);
			g_nAssisID = DS_HQALIVE_ASSISID;
			g_pGetData->DSGetSrvInfo();
		}
		if(!OnLine && g_nAutoConnectFlag==AUTOCONNECT_RECONNECT) //��ʱ�����Զ�����
		{
			PostMessage(UM_AUTOCONNECT,0,0);
			return;
		}
		if(!OnLine && g_nAutoConnectFlag==AUTOCONNECT_PENDING)
		{
			g_pComm->Disconnect();	//�Ͽ�����,�����һ��ʱ���ڻ�û���Զ����ӳɹ�,��Ͽ�������,���¿�ʼ
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
			g_pComm3->Disconnect();	//�Ͽ�����,�����һ��ʱ���ڻ�û���Զ����ӳɹ�,��Ͽ�������,���¿�ʼ
			g_nDSAutoConnectFlag = AUTOCONNECT_RECONNECT;
			return;
		}
	}
	else if(nIDEvent == 6 && !g_bDowning && !bIdle)
	{
		if(g_GH.Roll.bShow && (g_GH.Roll.nInfoType[0]==1 || g_GH.Roll.nInfoType[1]==1) && OnLine && g_nMovingZXG > 0)
		{
			//��Ȼ����Ԥ����Ʊ����
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
	else if(nIDEvent == 7 && (g_bAllWarnSwitch||g_nUnitRadarFlag>0||g_bTjWarnSwitch) && OnLine && !g_bDowning && !bIdle)	//Ԥ��ˢ��
	{		
		if(m_nWarnReq<1) 
			m_WarnManager.GetWarnGpSet();
		if((Is_Jy_Time()||m_nWarnReq<2) && g_nWarnGpNum>0) //����Ԥ����������
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
	else if(nIDEvent == 8)	//�ж�g_pCommͨѶ��ʱ
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
	else if(nIDEvent == 9 && OnLine && g_bSupportDDE && theViewManager.pActiveView && theViewManager.GetWindowNum()>1)//����ͳ�����ݵ�ˢ��
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
	else if(nIDEvent == 10 && OnLine && g_bSupportDDE && theViewManager.pActiveView && theViewManager.GetWindowNum()>1)//����ͳ�����ݵ�ˢ��
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
	else if(nIDEvent == 11)	//���ϳ�ʼ��ֻ���5����,��ֱ�ӽ���,ֻʹ��һ��
	{
		KillTimer(11);
		LogoDlg_ReConnect();
	}
	else if(nIDEvent == 13)	//��̬ˢ������ѡ��
	{
		//�Զ�����ѡ���߳�
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
	else if(nIDEvent==30)	//Ȩ֤�������µ�
	{
		//���֧��ֹ��ֹӮ��(��������Ȩ֤),�����е�Ȩ֤�����鲻������
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
					if(pStkInfo->Name[0]!='E')	//��ȥ���ܴ���
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
	else if(nIDEvent==100&&g_KeyGuyCnInfo.nKeyGuyCNing==1&&g_KeyGuyCnInfo.pOwner)	//���̾��麺������
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
	if(g_pMainWin)	g_pMainWin->GetLastActivePopup(); //�ر����һ�μ���Ĵ���
	if(!pActiveWnd) pActiveWnd = g_pMainWin;
	g_bDowning = FALSE;
	if(pActiveWnd)
		pActiveWnd->SendMessage(WM_COMMAND,IDCANCEL,0); //����ģ̬�Ի�������
	if(g_pHisDlg)
		g_pHisDlg->SendMessage(WM_COMMAND,IDCANCEL,0);

}

void CMainFrame::LogoDlg_ReConnect()	//�����Ի�����������
{
	if(!OnLine && g_pMainWin && LayoutFlag)
	{
		//��ȥ��ǰ���ģ̬�Ի���
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
				if(g_GC.nShareMode) //����ǹ���ģʽ,�����û����ý���ί�е�·��
				{
					TDX_MessageBox(m_hWnd,"ִ�������Ѷ����ʧ��!",MB_OK|MB_ICONERROR);
					return;
				}				
				CFindExeDlg dlg;
				dlg.m_strExeInfo = _F("�����������Ѷ�����·��:");
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
	OptionSheet.SetTitle (_F("�û��������"));
	OptionSheet.SetActivePage(&BlockPage);
	if(OptionSheet.DoModal() == IDOK)
	{
		//��������б仯
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
				TDX_MessageBox(m_hWnd,"���ܴ򿪰����ļ�!",MB_ICONEXCLAMATION+MB_OK);
			g_bBlockModified = TRUE;
			theViewManager.pActiveView->PostMessage(UM_BLOCKMODIFIED,0,0);//��GridView�����ϳ���
		}
	}
//	SaveBlockColorCfg(g_BlockColor);
	LoadColorfulBlockGP();

	if( BlockPage.bBlockModify)
	{
		//����������ļ�����
		SaveBlockCfg();
		FillGPTree();
		RefreshCusGridTab(); //����ˢ���Զ����ǩ
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
//ϵͳ���õ�����ҳ
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
			//�����ǰ��ȫ�����жԻ������ػ�֮
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
	OptionSheet.SetTitle (_F("ϵͳ����"));
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
			//�����ǰ��ȫ�����жԻ������ػ�֮
			if(m_pZHSortDlg)	
				m_pZHSortDlg->Invalidate(FALSE);
			SaveScheme();
		}
		//
		Set_OP_Scheme();	//�ı���
		//�����������õ�����
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
					//�������ö�ʱ��
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
				TDX_MessageBox(m_hWnd,"�������Զ�������Ҫ���½���ϵͳ������Ч!",MB_OK|MB_ICONINFORMATION);
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
			UnregisterHotKey(m_hWnd,1001); //�رտ�������DLL
		//�������������Ϣλ�ñ仯��,��ָ���б仯,��ȫ����Ӧ�Ĵ���
		if(bLastHQInfo!=g_GH.bLeftHQInfo || Other2Page.m_bChangeZB&&g_bFxtShowTab)
		{
			CloseView("Com");
			if(!g_bFirstCusPad)
				CloseView("Com2");
		}
		FillGnTree();
	}
}

//�������õ�����ҳ
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
	//���´ν�����û����ͽ��뷽ʽ���ú�
	CString	Ini = g_WSXHStr+g_strYxhjCfg;
	WritePrivateProfileString("USER","UserName",newuser,Ini);
	Ini = g_WSXHStr+g_strUserCfg;
	WritePrivateProfileString("Other","LastLoginType",IntToStr(REGISTER_LOGIN),Ini);
	if(TDX_MessageBox(m_hWnd,"ע���û��ɹ�,\n�Ƿ����½���ϵͳ,����ע����û�����¼?",MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL)
		return;
	//�رձ�ϵͳ,ֱ���˳�ǰ��ȥ��ǰ���ģ̬�Ի���
	g_bDirectQuit = TRUE;
	CancelModalWindows();
	PostMessage(WM_CLOSE);
	//����ִ�д˳���
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
	case GN_TREETYPE:	//����
		{
			WORD LoWord = LOWORD(lParam);
			WORD HiWord = HIWORD(lParam);
			OnCommCommand(HiWord,LoWord);
			break;
		}
	case GP_TREETYPE:	//��Ʊ
		{
			if(lParam == 10000) break; //����Ǹ�ĸ֦�����˳�
			//һ������£������鴰�ڷ���Ϣ
			g_bOldGridDomain=FALSE;
			CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
			if(tempWnd)
				tempWnd->SendMessage(WM_TREE_MESSAGE,lParam);
			g_bOldGridDomain=TRUE;
			break;
		}
	case ZB_TREETYPE:	//ָ��
		{
			if(lParam == 65535) break; //���ĸ֦û����֦�����˳�
			WORD LoWord = LOWORD(lParam);
			WORD HiWord = HIWORD(lParam);
			int MessageNo;
			if(HiWord == 1 || HiWord == 2 || HiWord == 3)
			{
				int type=0,index=0;	//0:ɾ��ָʾ 1:����ϵͳָʾ 2:���K�� 3:һ��ָ�� 4:���ػ���ָ��
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
					MessageNo = LoWord + KEYGUY_ZB;		//ָ��
				else
					MessageNo = LoWord + KEYGUY_SEPCZB; //����ָ��
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
		return TRUE;//��ֹ��Ļ�������������
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
			CWnd *pWnd = CWnd::GetFocus(); //��������View��ʱ������ʹ��VK_ESCAPE��
			if(pWnd && ((pWnd==theViewManager.pActiveView) || theViewManager.pActiveView->IsChild(pWnd)) )
			{
				if( !theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CBigMenuView)) && !theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CHomePageView)) )
				{
					//�����ǰ����ΪComView����������������Ϣ
					if( theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)) )
					{
						CComView *tempView = (CComView *)theViewManager.pActiveView;
						if(tempView->EscHandle() == 1)
							return TRUE;
					}
					//�����ǰ����ΪComView����������������Ϣ
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
					//TxtView������һЩ���൱�ڶԻ���
					if( theViewManager.pOldActiveView && (theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CTxtView)) || theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CIEBrowserView))) )
					{
						if(theViewManager.pOldActiveView != theViewManager.pActiveView)
						{
							CFrameWnd *tempFrameWnd = (CFrameWnd *)(theViewManager.pOldActiveView)->GetParent();
							tempFrameWnd->ActivateFrame();
							return TRUE;
						}
					}
					//������ɸ߼�����,���������;�������ͼ�������ͼ��,��ֱ�ӷ���
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
		switch(pMsg->wParam)  //�ػ�ϵͳ��F2--F10
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
				if(CtrlDown >= 0)	//����ػ�Ctrl+F3,Ctrl+F3������ʾ����ͼ
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
				if(CtrlDown >= 0)	//����ػ�Ctrl+F4ϵͳ��
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
				if(CtrlDown >= 0)	//����ػ�Ctrl+F6ϵͳ��
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

//��ϵͳ�����������ʱ�����˵��Ĵ���
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
	if (rcTW.left<=0 && rcTW.right>=0 && rcTW.top<=0 && rcTW.bottom>=nMaxY)//���
	{
		if (abs(rcTW.right)<5)
			rcMax.left += 3;
	}
	else if (rcTW.left<=nMaxX && rcTW.right>=nMaxX && rcTW.top<=0 && rcTW.bottom>=nMaxY)//�Ҳ�
	{
		if (abs(rcTW.left-nMaxX)<5)
			rcMax.right -= 3;
	}
	else if (rcTW.top<=0 && rcTW.bottom>=0 && rcTW.left<=0 && rcTW.right>=nMaxX)//�ϲ�
	{
		if (abs(rcTW.bottom)<5)
			rcMax.top += 3;
	}
	else if (rcTW.top<=nMaxY && rcTW.bottom>=nMaxY && rcTW.left<=0 && rcTW.right>=nMaxX)//�²�
	{
		if (abs(rcTW.top-nMaxY)<5)
			rcMax.bottom -= 3;
	}
	return rcMax;
}

//���ش˺������õ�����ܴ��ڵ������С��Ϣ������ȫ����ʾ��
void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	//��һ��Ҫȥ��������
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
		//����ȥ����WS_CAPTION����,MFC����߼����������
		//�����ж��Ƿ�����һ���������������ǵĻ��������д���
		//������д���ע���ж���
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
	//Ӧ�ö��κδ��ڶ�ʵ��,��Ӧ����ֻ�Լ����
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
	if (m_pWndFullScreen != NULL)	//��ȥȫ��״̬
	{
		ShowControlBar(&m_wndWorkspace,m_bHadDialogBar,FALSE);
		ShowControlBar(&m_wndToolBar, m_bHadToolBar, FALSE);
		ShowControlBar(&m_wndStatusBar, m_bHadStatusBar, FALSE);
		
		m_pWndFullScreen = NULL;  //Ҫ��ShowWindow֮ǰ��Ϊ0,��ΪOnGetMinMaxInfo�лᴦ��

		LockWindowUpdate();
		SetWindowPlacement(&m_PreviousPlace);
		UnlockWindowUpdate();
	}
	else	//ȫ����ʾ
	{
		m_FullScreenPlace.SetRect(0,0,0,0);

		if(g_GH.bFullScreenNotify)
		{
			CHintDlg dlg;
			dlg.m_bAlwaysHint = g_GH.bFullScreenNotify;
			dlg.m_strHint = "�л���ȫ��ģʽ��,��Ctrl+P���ָ�������ģʽ";
			dlg.m_strCheck = _F("���ǽ�����ʾ");
			dlg.DoModal();
			g_GH.bFullScreenNotify = dlg.m_bAlwaysHint;
		}
		CloseallAndKeepSpec(); //�ر����д���,����ȫ������

		m_pWndFullScreen = MDIGetActive();	//�õ�����Ĵ���
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
		//�õ������С
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
	//���û�б���¼�ļ����򴴽�֮��
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
	case CMD_MAIN:		//�����������
		{
			OnMainWndMessage(MessageNo);
			break;
		}
	case CMD_HQ:		//�����������Ϊ����
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
	case CMD_COM:		//�����������Ϊ���ͼ
		{
			if(MessageNo==350)
			{
				OnShowvip();
				return TRUE;
			}
			if(MessageNo==351)
			{
				if( theViewManager.pActiveView && theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)) )
				{	//�����ǰ������ͼ
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
			if(MessageNo>=150&&MessageNo<200)		//��ѡ��
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
				{	//�����ǰ������ͼ,��������߱���
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
	case CMD_JBM:			//����ǻ���������ͼ
		{
			if(MessageNo == 801 || MessageNo == 802)
			{
				int tempCurrentGPNo = GetSYSCurrentGPIndex();
				if(tempCurrentGPNo >= 0)
				{
					//�����ʵʱ�۹ɣ���򿪲ƻ�(������)��Ӧ�Ĺ�Ʊ��Ȩ֤��ҳ
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
				//�õ�����
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
	case CMD_NEWS:	//�����������Ϊ������Ϣ
		{
			CNewsView *tempWnd = (CNewsView *)SwitchToView("News");
			if(tempWnd)
				tempWnd->SendMessage(WM_KEYGUY_MESSAGE,MessageNo);
			return TRUE;
		}
	case CMD_HIS:	//�����������Ϊ��ʷ����
		{
			//����ҵ���ת��������������һ��
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
				HisCurType = MessageNo - 400; //��дHisCurType�������������ȴ�ʱ��CurType
				if(HisCurType < 0 && HisCurType > 6) HisCurType = 0;
				if(HisCurType > 2) //���������ͳ�����У�����Ҫ֪������������
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
		tmpHintStr.Format("����[%s]����,\r\n���Ȱ�װ�˳���ģ��!\r\n%s",g_GC.HKey_MenuName,g_GC.HKey_ErrorInfo);
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
	case 5:		//�����л�
		OnF5();
		break;
	case 1000:	//����ί��
		OnFuncWt();
		break;
	case 1001:	//��������
		OnLevinBuy(IDC_LEVINBUY11);
		break;
	case 1002:	//������ѯ
		OnWTCancel();
		break;
	case 1003:	//��������
		OnLevinSell(IDC_LEVINSELL1);
		break;
	case 1005:	//������
		OnQzSDS();
		break;
	case 1006:	//��ͨ����
		OnBuy(IDC_BUY11);
		break;
	case 1007:	//��ͨ����
		OnSell(IDC_SELL1);
		break;
	case 1011:	//��?������
	case 1012:
	case 1013:
	case 1014:
	case 1015:
		OnLevinBuy(IDC_LEVINBUY11+MessageNo-1011);
		break;
	case 1016:	//��?������
	case 1017:
	case 1018:
	case 1019:
	case 1020:
		OnLevinBuy(IDC_LEVINBUY1+MessageNo-1016);
		break;
	case 1021:	//��?������
	case 1022:
	case 1023:
	case 1024:
	case 1025:
		OnLevinSell(IDC_LEVINSELL1+MessageNo-1021);
		break;
	case 1026:	//��?������
	case 1027:
	case 1028:
	case 1029:
	case 1030:
		OnLevinSell(IDC_LEVINSELL11+MessageNo-1026);
		break;
	case 1080:	//�о�����ƽ̨
		OnTdxReport();
		break;
	case 1100:	//��������
		OnGgzl();
		break;
	case 901://��ʽ������
		OnFormula();
		break;
	case 902://����ѡ�ɲ���
		OnTJTest();
		break;
	case 903://����ϵͳ����
		OnJYTest();
		break;
	case 904://̽�����ָ��
		OnTestindex();
		break;
	case 905://����ѡ��
		OnConditionChoice();
		break;
	case 906://����ѡ��
		OnSimchoice();
		break;
	case 907://����ѡ��
		OnClechoice();
		break;
	case 908://���ѡ��
		OnPlugin();
		break;
	case 909://�ۺ�ѡ��
		OnSelGP();
		break;
	case 910://
		OnAllWarncfg();
		break;
	case 911://
		OnAllWarngp();
		break;
	case 913:	//��Ʊ��ϼ���
		OnFuncBlockCalc();
		break;
	case 914:	//�����ھ�ο�
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
		OnPadList();//���������
		break;
	case 925:
		OnShowCustomPadSwitch();//���������л���
		break;
	case 930:	//ϵͳ����
		OnOption();
		break;
	case 931:	//�����û����
		OnBlockOption();
		break;
	case 932:	//ͨѶ����
		OnConnectCfg();
		break;
	case 933:	//��������
		OnDownLoad();
		break;
	case 934:	//����ά������
		OnNodeTool();
		break;
	case 935:	//�Զ�����
		OnAutoup();
		break;
	case 936:	//��������
		OnHelpFinder();
		break;
	case 937:	//�û���̳
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
			CWnd *tmpMainWnd = g_pMainWin;	//����ԭ���Ľ��մ���
			g_pMainWin = g_pConnectWnd;
			if(((CDialog *)g_pConnectWnd)->DoModal() == IDOK)
				bOK = TRUE;
			g_pMainWin = tmpMainWnd;
			TDEL(g_pConnectWnd);
			if(!bOK) return;
			//�������׸������
			if(g_GC.bAlongJy && strlen(g_GC.AlongPathFile))
				ExecExtern(g_GC.AlongPathFile,m_hWnd);			
			//��;��¼����,������ȡ����
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
			dlg.m_strCaption = _F("�ⲿ����ί�г���");
			dlg.m_strExeInfo = _F("�������ⲿ����ί�г����·��:");
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
			if(g_GC.nShareMode) //����ǹ���ģʽ,�����û����ý���ί�е�·��
			{
				TDX_MessageBox(m_hWnd,"ִ�н���ί�г���ʧ��!",MB_OK|MB_ICONERROR);
				return;
			}
			CFindExeDlg dlg;
			dlg.m_strCaption = _F("�ⲿ����ί�г���");
			dlg.m_strExeInfo = _F("�������ⲿ����ί�г����·��:");
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
		//�����Ƕ�����Ѿ���¼,��ֱ�Ӵ���
		DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
		if((dwStatus&JY_LOGINED))
			OnFuncEmbWT();
		else
		{
			//��ί�н���ѡ��
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
		TDX_MessageBox(m_hWnd,"ϵͳ�����ļ��д���,����ʹ�ý���!",MB_ICONERROR);
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
	//�򿪺��µĶ�����Ѷ����
	//������֤ʱʹ��-Vѡ��˵����ò�����-V
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

//1-�õ�����·��������ִ�У�0-��ִ�У��ӹ�������-1-�Լ�����·��
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
	//������ģʽ�Ի���һ��Ҫ��ָ�붯̬����
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
	//������ģʽ�Ի���һ��Ҫ��ָ�붯̬����
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
	//������ģʽ�Ի���һ��Ҫ��ָ�붯̬����
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
		TDX_MessageBox(m_hWnd,"��ǰ�汾���ò�֧�ִ˹���!",MB_OK|MB_ICONEXCLAMATION);
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
		FillZBTree(); //��������TreeView
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
		TDX_MessageBox(m_hWnd,"û������ĳ���ư�����Ϊ��ʼ����!",MB_OK|MB_ICONINFORMATION);
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
	WarnCfgSheet.SetTitle (_F("�г��״�����"));

	if(WarnCfgSheet.DoModal ()==IDOK)
	{
		if((!g_bAllWarnSwitch && g_nUnitRadarFlag==0) && OnLine)
		{
			if(TDX_MessageBox(m_hWnd,"�г��״ﻹû�д�,���Ƿ����?",MB_YESNO|MB_ICONINFORMATION ) == IDYES)
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
	//��������Ԥ������
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
	AbsPageGp.m_psp.pszTitle=_F("Ԥ����Ʊ");
	AbsPageGp.m_nListFlag=GPLIST;
	CAbsWarnPage	AbsPageTJ;
	AbsPageTJ.m_psp.dwFlags |= PSP_USETITLE;
	AbsPageTJ.m_psp.pszTitle=_F("��ʽ����");
	AbsPageTJ.m_nListFlag=TJLIST;
	COtherWarn2Page OtherWarn2;

	WarnCfgSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	WarnCfgSheet.AddPage (&AbsPageGp);
	WarnCfgSheet.AddPage (&AbsPageTJ);
	WarnCfgSheet.AddPage (&OtherWarn2);
	WarnCfgSheet.SetTitle (_F("����Ԥ������"));

	if(WarnCfgSheet.DoModal ()==IDOK)
	{
		if(g_pAbsConditionNum<1)
			TDX_MessageBox(m_hWnd,"��û��ָ����Ʊ,����Ԥ����������Ч!",MB_OK|MB_ICONINFORMATION);
		else if(!g_bTjWarnSwitch && OnLine)
		{
			if(TDX_MessageBox(m_hWnd,"����Ԥ����û�д�,���Ƿ����?",MB_YESNO|MB_ICONINFORMATION ) == IDYES)
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
	//��������Ԥ������
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
		tmpStr.Format("û������%s����������!",g_DS.DSOEMName);
		TDX_MessageBox(m_hWnd,tmpStr,MB_ICONINFORMATION|MB_OK);
		return;
	}
	if(!g_GC.bIsSSDS)
	{
		TDX_MessageBox(m_hWnd,"ʵʱ�۹ɲ��о���ϯλ׷�ٹ���!",MB_ICONINFORMATION|MB_OK);
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
	m_wndWorkspace.SetActiveWnd(g_nTreePos[treetype]); //������վ��
	ShowControlBar(&m_wndWorkspace,TRUE,TRUE);		
}

//6XXXXXX ���ڹ�Ʊ 7XXXXXX �Ϻ���Ʊ 8XXXXXX ����ȷ��
LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	static BOOL l_bProcessClip=FALSE;
	static int  l_nTotalConnectInfoNum=0;
#ifdef OEM_NEWJY
	if(message==WM_EXITMENULOOP && m_wndCaptionBar.m_hmenu)
		::PostMessage(m_wndCaptionBar.m_hWnd,UM_EXITMENULOOP,0,0);
#endif
	//�����֪ͨ�ر�
	if(message == g_uCloseMsg)
	{
		//�رձ�ϵͳ,ֱ���˳�ǰ��ȥ��ǰ���ģ̬�Ի���
		g_bDirectQuit = TRUE;
		CancelModalWindows();
		PostMessage(WM_CLOSE);
	}
	//�л�����
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
		if(tmpCode[0]<'3') //�ڻ�Ʒ��
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
			if(g_ET.HWnd && g_ET.Ready) //������ߵĴ��ڷ���������Ϣ
			{
				struct CurrStockData tmphq;
				g_pStockIO->ReadCurrStockData(pStkInfo,&tmphq);
				ETSendCopyDataMessage(g_ET.HWnd,pStkInfo,&tmphq);
			}
		}
	}
	//ˢ����������
	if(message == g_uRefreshAllHQ)
	{
		RefreshAllHq(10);
	}
	//ȡ�ʽ��ʺ���Ϣ
#ifdef OEM_NEWJY
	if(message == QryZJZHMsg)
	{
		//�����ȡ�ʽ��ʻ�,����wParam��Ϊ'1'
		if(wParam==1 && strlen(g_JyInfo.zjzh)>0)
		{
			DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
			if(dwStatus&JY_LOGINED)
				::PostMessage(HWND_BROADCAST,RetZJZHMsg,atol(g_JyInfo.zjzh),0);
		}
	}
	if(message == QryZJZHMsg_My)
	{
		//�����ȡ�ʽ��ʻ�,����wParam��Ϊ'1'
		if(wParam==1 && strlen(g_JyInfo.zjzh)>0)
		{
			DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
			if(dwStatus&JY_LOGINED)
				::PostMessage(HWND_BROADCAST,RetZJZHMsg_My,atol(g_JyInfo.zjzh),0);
		}
	}
#endif

	if( WM_CHANGECBCHAIN == message)		//Clipboard Viewerע�� 
	{
		//���ע����Clipboard Viewer�����Ǳ����ڵ���һ�����ڣ� 
		//���޸ı����ڱ������һ�����ھ��, 	
		//���򽫸���Ϣ���ݵ�Clipboard Viewer Chain����һ������ 
		if ((HWND) wParam == m_hWndNextViewer) 
			m_hWndNextViewer = (HWND) lParam; 
		else if (m_hWndNextViewer != NULL) 
			::SendMessage(m_hWndNextViewer, message, wParam, lParam); 
	}
	
	if(WM_DRAWCLIPBOARD == message && m_bClipSpy && g_pKeyWordSearch && !l_bProcessClip)		//���������ݱ仯 
	{
		l_bProcessClip=TRUE;
		::SendMessage(m_hWndNextViewer, message, wParam, lParam);
		//����
		unsigned int anFormats[] = {CF_TEXT}; 
		unsigned int nFormat;
		nFormat = GetPriorityClipboardFormat(anFormats,sizeof(anFormats));
		if(nFormat == CF_TEXT) 			//��������	
		{ 	
			::OpenClipboard(m_hWnd);
			HGLOBAL hMem = GetClipboardData(nFormat); 
			DWORD nLen = GlobalSize(hMem);
			char *lpstr = (char*)GlobalLock(hMem); 
			char *lpstrend = &lpstr[nLen-1]; 
			
			//�ؼ��ֲ���
			g_pKeyWordSearch->InitResult();
			while (lpstr<lpstrend)
				lpstr = g_pKeyWordSearch->SearchKeyWord(STK_NUMCODE_KEY|STK_NAME_KEY, lpstr, lpstrend);
			if(!g_pKeyWordSearch->m_KeyWordOK.empty())
			{
				CBatchDlg dlg;
				dlg.m_bTopMost=TRUE;//�ᵽ����Ļǰ��
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
	//����ģ�����Ϣ
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
	CString tmpStr; //���ڸ��˰�,������ͨ����վ

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
	CString tmpStr; //���ڸ��˰�,������ͨ����վ
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
			TDX_MessageBox(m_hWnd,"�����������Ϣ.������������վ!",MB_OK|MB_ICONINFORMATION);
			return;
		}
		if(l_NeedAutoupInfo.bNeedAutoup==0)
		{
			TDX_MessageBox(m_hWnd,"��ǰ�����������µ�,����Ҫ����!",MB_OK|MB_ICONINFORMATION);
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
		//����V1.05���ϵ��Զ���������֧�ֺ�̨����
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
	//�������Button
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
	//����Ǿ�̬��ʽ,�򲻽����л�
	if(m_bStaticToolBar) return;

	static int l_lastToolBarType=-1;
	int nTmp = (int)wParam;
	if(nTmp==l_lastToolBarType || nTmp==-1) return; //�����ظ����칤����
	l_lastToolBarType = nTmp;
	int nSep = 0,i,j,nIndex;
	try //catch���ڹ�������ɵĴ���
	{
		//�������Button
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
		//����̬Button
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
		//�����ػ�
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
	//������ģʽ�Ի���һ��Ҫ��ָ�붯̬����
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
	//����ڸ�����ȫ���������,�����Բ˵�����Ϣ��������ڵ�,���˳�ȫ��״̬
	if(m_wndRestspace.IsFullSize() && LOWORD(wParam)>100) //Ϊ��ֹESCAPE����Ϣ,Ҫ�����100
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

//Զ������
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
				//֧�����������ͣ�ע�⣬ֻ�е�ǰ��GridView���Ž���Ϣ����ȥ
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
		case COMVIEW_WIN:	//ComView�ڰ��漶ά������(ZST,TICK,FSB,FXT,MMP,HQ��)
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
			case STATUSZSHQ:		//״̬������
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
					if(ct.GetMinute()-g_nDSCheckHostMinute>=DS_DETECTCODE_MIN) //DS_DETECTCODE_MIN����̽����û���µĴ�����
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
						g_pComm3->Disconnect();	//�Ͽ�����
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
		case STATUSZSHQ:		//״̬������
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
		case HQALIVE_ASSISID:	//�������ʱ�õ�ͨ��
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
		case URGENT_ASSISID:	//��ʱ�õ�ͨ��
			g_pGetData->GetUrgentCfgACK();
			ShowUrgentDlg(URGENT_DURING,URGENT_NORMAL);
			break;
		case AD_ASSISID:		//��ʱ�õ����
			{
				//��ɾ��ԭ���Ĺ���ļ�
				unlink(g_WSXHStr+"LData\\cache\\"+g_strNeedAdName);
				//���ɹ���ļ�
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
				//���¶�ȡ����ļ������ö�ʱ��
				GetNeedAdInfo();
				if(g_AdHead.m_nADNum>0)
				{
					KillTimer(12);
					SetTimer(12,5*1000,NULL);	//��������Ϣ�Ķ�ʱ��,���쵯��
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
		default:				//�����ֵ�������ͼ
			break;
		}
		break;
	case COMVIEW_WIN:	//ComView�ڰ��漶ά������(ZST,TICK,FSB,FXT,MMP,HQ��)
		{
			for(int j = 0;j < theViewManager.GetWindowNum();j++)
			{
				if( ((CView *)theViewManager.arViews[j])->IsKindOf(RUNTIME_CLASS(CComView)) )
					((CView *)theViewManager.arViews[j])->SendMessage(UM_GETDATA_ACK,wParam);
			}
		}
		break;
	case COMVIEW2_WIN:	//ComView�ڰ��漶ά������(ZST,TICK,FSB,FXT,MMP,HQ��)
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
	SetTimer(11,5*1000*60,NULL); //���ϳ�ʼ��ֻ���5����,��ֱ�ӽ���
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
	//�ݴ�,LogoDlg��ContinueModal����
	BOOL bOK=FALSE;
	try
	{
		g_pConnectWnd = new CLogoDlg;
		((CLogoDlg *)g_pConnectWnd)->m_bHasOffMode = FALSE;
		((CLogoDlg *)g_pConnectWnd)->m_bDisableNLB = bDisableNLB;
		if(bReInit)
			((CLogoDlg *)g_pConnectWnd)->m_bDirectConnect = TRUE;
		CWnd *tmpMainWnd = g_pMainWin;	//����ԭ���Ľ��մ���
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
		TDX_MessageBox(m_hWnd,"δ֪��������������!",MB_OK|MB_ICONEXCLAMATION);
		OnLine = FALSE;
		return;
	}
	if(OnLine && bOK)
	{
		InterlockedExchange(&g_nAutoCalcLock, 1);
		if(m_hAutoCalcBkThread!=NULL&&WaitForSingleObject(m_hAutoCalcBkThread,1000)!=WAIT_OBJECT_0)
			TerminateThread(m_hAutoCalcBkThread,0);

	//	CloseallAndKeepSpec(); //����֮ǰ���ص����д��ڣ������鴰��
		TDEL(g_pStockIO);
		//��g_pStockIO->TcpipInit֮ǰload,�Ա��ڼ��뵽������
		TDEL(g_pLcIdxMng);
		g_pLcIdxMng	= new LcIdxMng;
		if(!g_pLcIdxMng)
		{
			TDX_MessageBox(m_hWnd,"�ڴ��������!",MB_OK|MB_ICONERROR);
			g_bSpecClose = TRUE;
			PostMessage(WM_CLOSE); //ֱ���˳�
			return;
		}
		g_pLcIdxMng->LoadAll();
		//
		TDEL(g_pExdayMng);
		g_pExdayMng = new ExdayMng;
		//����ȫ��ʵ��
		g_pStockIO = (StockDataIo * ) new StockDataIo;
		if (!g_pStockIO->TcpipInit(OffLineMode))
		{
			TDX_MessageBox(m_hWnd,"���ݳ�ʼ�����󣬳����˳�!",MB_OK|MB_ICONERROR);
			g_bSpecClose = TRUE;
			PostMessage(WM_CLOSE); //ֱ���˳�
			return;
		}
		if(DSOnLine)
			g_pStockIO->AfterDSLoginSuccess();
		try
		{
			g_nOverlapNum = 0; //��ΪDataIO���³�ʼ���ˣ���g_nOverlapNumҪ��λ
			////
			memset(g_nMineNum,0,MAXGPNUM*sizeof(short));	//���׵���Ŀ���
			m_nHqMinePos = 0;
			m_nHqMinePrenum = 80;
			//��һЩȫ�ֱ�������
			TxtCurGPCode = "";
			TxtCurSetCode = -1;
			ComCurGPCode = "600839";
			ComCurSetCode = SH;
			g_nWarnNum = 0;
			g_nTJWarnNum =0;
			//���µõ�����ƽ̨������Ϣ
			GetFYInfo();
			g_bFYDataAlwaysSync=FALSE;
			g_DDEDataNeedLoad=TRUE;
			g_lRefreshHQSeconds = 0;
			g_lRefreshStatHQSeconds = 0;
			//��������ȫ�ֺ�����������
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
			//�ж��Ƿ���ʵʱ��չ����
			TestAutoDS();
			//������չ������վ
			if(g_bHasDS && m_bAutoDS)
				PostMessage(UM_AUTOCONNECTDS,0,0);
			//�ؿ������û�����������ĳ������߳�
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
	//��ȥ��ǰ���ģ̬�Ի���
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
		TDX_MessageBox(m_hWnd,"�������ӱ��Ͽ�!",MB_ICONINFORMATION|MB_OK);
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
		TDX_MessageBox(m_hWnd,"�������ӵ�������վ!",MB_ICONINFORMATION|MB_OK);
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
	if(!g_bCanDown) return; //����Ǳ�������,������д��
	g_bDay_WriteDisk = !g_bDay_WriteDisk;
}

void CMainFrame::OnUpdateDayWriteDisk(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( g_bCanDown );
	pCmdUI->SetCheck(g_bDay_WriteDisk);
}

//�������˵���ʱʧ��������޸�
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
		//������ģʽ�Ի���һ��Ҫ��ָ�붯̬����
		m_pWebWinDlg = new CShowURL(this,TRUE);
		m_pWebWinDlg->m_strTitle = _F("��Ȳ�ѯ");
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
	if(g_GC.Video.nVideoFlag==1)	//ֱ����ҳ��ʽ
	{
		ShellExecute(NULL, _T("open"), g_GC.Video.strVideoURL, NULL, NULL, SW_SHOW);
	}
	else if(g_GC.Video.nVideoFlag==2)	//��Ƕ���ڸ�ʽ
	{
		if(m_pVideoDlg)
		{
			if (m_pVideoDlg->IsIconic()) 
				m_pVideoDlg->ShowWindow(SW_RESTORE); 
			return;
		}
		//������ģʽ�Ի���һ��Ҫ��ָ�붯̬����
		m_pVideoDlg = new CShowURL(this);
		m_pVideoDlg->m_strTitle = _F("�ƾ���Ƶ");
		m_pVideoDlg->SetInfo(g_GC.Video.nWndcx,g_GC.Video.nWndcy,g_GC.Video.strVideoURL);
		m_pVideoDlg->Create(IDD_SHOWURL);
		m_pVideoDlg->ShowWindow (SW_SHOW);
	}
	else if(g_GC.Video.nVideoFlag==3)	//KooPlayer P2P���Ÿ�ʽ
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

//����״̬���ĸ߶Ȳ��������ù�����
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
	 if(nID==SC_MINIMIZE) //ʹ�����Լ�����С������
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
			if(g_Hookcmd==SW_SHOW) //��������ضԻ���
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
				else if(g_nHideWnd < 1000)//��������������ص�,��������
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

	if(wParam==1002)				//CTL_TAB�л��˵��ȼ�
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
				str.Format("�˲��������� [%s]  \n�ٰ�ͬ���ļ��ָ�\n\nȷ��������?",g_strPreName);
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
			tmpMenuStr+="(��ʼ)";
		pPadMenu->AppendMenu(MF_STRING, IDC_CUSTOMPAD1+i, _F(tmpMenuStr));	
		if( i != 0 && i % 28 == 0 )
			SetMenuBreak(i,pPadMenu);			
	}
}

void AppendDSMenu(CNewMenu* pDSMenu)
{
	for(int iPos = pDSMenu->GetMenuItemCount()-1; iPos > 1; iPos--)
		pDSMenu->DeleteMenu(iPos, MF_BYPOSITION);
	//�������Ѹ۹ɣ�����빺��۹ɵ�����
	if(g_DS.HasHKMarket && !g_GC.bIsSSDS && !g_OEM.bNoAdvert)
		pDSMenu->AppendMenu(MF_STRING,IDM_BUYDS,_F("����ʵʱ�۹�"));
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
			pDSMenu->AppendMenu(MF_STRING,ID_HKSEATQUERY,_F("����ϯλ׷��"));
		pDSMenu->AppendMenu(MF_STRING,ID_HKJYTIME,_F("�۽���������"));
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
			pPopupMenu->AppendMenu(MF_POPUP,(UINT)(WebPopupMenu.m_hMenu),_F("���ɹ�����վ"));
		}
	}
	//////////////////////////////////////////////////////////////////////////
#if defined(OEM_NEWJY)
	return;
#endif
	CNewMenu *pPadMenu = NULL, *pTCmdMenu = NULL;
	CNewMenu *pTopMenu = (CNewMenu*)GetMenu();
	if(!pTopMenu) return;

	//���ư���Ĳ˵�
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
					pPopupMenu->InsertMenu(ID_CORP_WEB,MF_BYCOMMAND,ID_WRITEURGENT,_F("����ͨ��"));	
			#endif
				pPopupMenu->RemoveMenu(ID_SEND_FILE,MF_BYCOMMAND);
				pPopupMenu->RemoveMenu(ID_REC_FILE,MF_BYCOMMAND);
				pPopupMenu->InsertMenu(ID_CORP_WEB,MF_BYCOMMAND,ID_SEND_FILE,_F("�ϴ���������"));	
				pPopupMenu->InsertMenu(ID_CORP_WEB,MF_BYCOMMAND,ID_REC_FILE,_F("���ظ�������"));	
			}		
			if(g_bHasUserHost)
			{
				if(!g_bIsGuest && g_LoginInfo.bDialogRegister && g_nLastLoginType!=JY_LOGIN) //����ǽ��׵�¼,�����޸�����
				{
					pPopupMenu->InsertMenu(ID_CORP_WEB, MF_BYCOMMAND,IDC_WEBUSER_1002, _F("�޸�����"));	
					if(g_LoginInfo.bCanDelMySelf)
						pPopupMenu->InsertMenu(ID_CORP_WEB, MF_BYCOMMAND,IDC_WEBUSER_1003, _F("ɾ���û�"));	
				}
			}
			if(g_bHasTICQ)
				pPopupMenu->InsertMenu(ID_CORP_WEB, MF_BYCOMMAND,IDC_WEBUSER_1001, _F("TQ"));	
		}
	}
	if(FALSE&&g_bHasDS)	//�������������,���������б��۲˵�
	{
		CNewMenu* pDSMenu = (CNewMenu*) pTopMenu->GetSubMenu(5);		
		if(pDSMenu && pPopupMenu==pDSMenu)
			AppendDSMenu(pDSMenu);
	}
	//��Ѷ�˵��Ĵ���
	CNewMenu* pZXMenu=NULL;
	if(g_bHasCaption==0)
		pZXMenu = (CNewMenu*)pTopMenu->GetSubMenu(pTopMenu->GetMenuItemCount()-g_nWebPageNum?4:3);
	else
		pZXMenu = (CNewMenu*)pTopMenu->GetSubMenu(pTopMenu->GetMenuItemCount()-5);
	if(!pZXMenu) return;
	if(pPopupMenu == pZXMenu)
	{
		//F11��Ѷ��
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
	//�����˵��Ĵ���
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
		if(g_nAutoConnectNum==5 && !g_bHintSwitch) //���������Ҳ��ͨ,�򵯳����ӶԻ����л�����
		{
			g_nAutoConnectFlag = AUTOCONNECT_RESET;
			LogoDlg_ReConnect();
			return;
		}
		if(g_nAutoConnectNum == 5) //�Զ�����3�Σ���ͨ�򱨴�
		{
			//���²��������վ
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
				TDX_MessageBox(m_hWnd,"�������ӱ��Ͽ�,�����ѻ�����״̬!",MB_OK|MB_ICONQUESTION);
				return;
			}
		//	g_nAutoConnectFlag = AUTOCONNECT_RESET;
		//	TDX_MessageBox(m_hWnd,"�������ӱ��Ͽ�,�����ѻ�����״̬!",MB_OK|MB_ICONQUESTION);
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
			g_pComm3->Disconnect();	//�Ͽ�����
			g_nDSAutoConnectFlag = AUTOCONNECT_RECONNECT;
		}
		else
		{
			g_pComm->Disconnect();	//�Ͽ�����
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
			g_pComm3->SetSendTop(0);	//״̬��λ
			//��ʼ��������
			g_nMainID = MainIDPlus(m_nMainID,MAIN_WIN);
			g_nAssisID= DS_LOGIN_ASSISID;
			g_pGetData->DSLogin();
		}
		else
		{
			if(g_bNetErrorReLogin && HQControl::TestUnStableNetWorkStatus())
			{
				//������������ʶ,������ʾ�Ի����,�����ٴν��жϿ��߼�
				g_bReLoginStatus = TRUE;
				//
				TDX_MessageBox(m_hWnd,"��⵽�����������Ӳ��ȶ��򱾳������в�������\r\n\r\n��ȷ����ť��ϵͳ���˳����½���!",MB_OK|MB_ICONEXCLAMATION);
				//
				g_bDirectQuit = TRUE;
				//�رձ�ϵͳ,ֱ���˳�ǰ��ȥ��ǰ���ģ̬�Ի���
				CancelModalWindows();
				PostMessage(WM_CLOSE);
				return;
			}

			g_pComm->StartThread();
			g_SysInfo.ConnectHQ_LastYmd=GetYMD();
			g_SysInfo.ConnectHQ_LastHms=GetHMS();

			OnLine = TRUE;
			g_pComm->SetSendTop(0);	//״̬��λ
			//�Զ����������һЩ����
			RefreshStatusBar();
			//��ȥ��ǰ���ģ̬�Ի���
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
	//�ȵõ���ǰ��Ʊ
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
		m_wndWorkspace.SetActiveWnd(g_nTreePos[treetype]); //������վ��
		ShowControlBar(&m_wndWorkspace,TRUE,TRUE);		
	}
}

void CMainFrame::OnJbfx()
{
	//�����������ҵ�ע�����,��F11��ֱ�Ӵ�
	if(strlen(g_GC.HKey_Path) && strlen(g_GC.HKey_KeyString))
		OnHKeyProgram();
	//���������F11��ҳ��Ҳֱ�Ӵ���ҳ
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
	case SWITCH_TOZSGPC:	//�л�����Ʊ��
		{
			CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
			if(tempWnd)
			{
				tempWnd->m_pSwitchBar->SetCurLabel(1);
				tempWnd->SendMessage(WM_COMMAND,ID_RIGHT_FYGROUP);
			}
		}
		break;
	case SWITCH_TOWARNWT:	//�л���Ԥ��ί��
		PostMessage(WM_COMMAND,IDC_PRICEWARN);
		break;		
	case SWITCH_TOZST:		//�л�������ͼ
		ToZst(SwitchCurGPCode.GetBuffer(0),SwitchCurSetCode,TRUE);
		break;
	case SWITCH_TOVIP:		//�л�������ͼ
		ToZst(SwitchCurGPCode.GetBuffer(0),SwitchCurSetCode,FALSE);
		break;	
	case SWITCH_REFRESHALLHQ://ˢ����������,lParamΪ��С��ˢ�¼����,��Ϊ0,��Ϊ10��
		if(lParam!=0)
			RefreshAllHq(lParam);
		else
			RefreshAllHq(10);
		break;
	case SWITCH_ADDBLOCK:	//���뵽���
		{
			CGetBlockDlg dlg;
			dlg.m_bGetBlock = FALSE;
			dlg.m_CurGPCode=SwitchCurGPCode;
			dlg.m_CurSetCode=SwitchCurSetCode;
			dlg.DoModal ();
			break;
		}
		break;		
	case SWITCH_SAVENEWBLOCK://��Ϊ�°��
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
			//�رձ�ϵͳ,ֱ���˳�ǰ��ȥ��ǰ���ģ̬�Ի���
			g_bDirectQuit=TRUE;
			CancelModalWindows();
			PostMessage(WM_CLOSE);
			break;
		}
		ProcessSwitchCallBack(SWITCH_CLOSEASSISWND,0);
		g_bEmbWTCreated=FALSE;
		break;
	case SWITCH_JYGETHQ:	//���׵õ�����
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
				if(g_ET.HWnd && g_ET.Ready) //������ߵĴ��ڷ���������Ϣ
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
	
	TDX_MessageBox (m_hWnd,"��ʾ:�½�����ʱ,����Ҽ����в���,\r\n�½���Ϻ�,ѡ��\'�˳����ð���\'",MB_ICONINFORMATION|MB_OK);	
}

#include "SetDefPadDlg.h"
void CMainFrame::OnSetdefcuspad() 
{
	if(g_vDynaCmdTab.empty())
	{
		TDX_MessageBox (m_hWnd,"���ȶ���һЩ���ư���,�������ó�ʼ����!",MB_ICONINFORMATION|MB_OK);
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
		MakeJyVerifyFile(""); //������֤Cookie�ļ�
		m_bJyVerify_Success = LocalJyVerify();
		TDX_MessageBox (m_hWnd,"ί����֤ͨ��!",MB_ICONINFORMATION|MB_OK);
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
	//���������ֽ��׹���,�����ǰ���治��ComView,���л�����ʱͼ
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
	//�����ǰȨ֤��ĳֻȨ֤,�򽫽���ģ���õ�������Ȩ֤���������
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
		TDX_MessageBox(m_hWnd,"û����������,����ʹ�ô˹���!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}	
	int iNo=GetSYSCurrentGPIndex();
	if(iNo<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[iNo];
	if(!tempStkInfo) return;
	int i,j;
	BOOL bExist = FALSE;
	for(i = 0;i < g_pAbsConditionNum;i++)//���Ƿ��Ѿ�������ֻ��Ʊ
	{
		if(tempStkInfo->setcode==g_pAbsCondition[i].setcode && strcmp(tempStkInfo->Code,g_pAbsCondition[i].code)==0)
		{	bExist = TRUE;break; }
	}
	if(!bExist)
	{	
		if(g_pAbsConditionNum>=MAX_ABSWARNMAXNUM)
		{
			TDX_MessageBox(m_hWnd,"����Ԥ���Ĺ�Ʊ̫��,�����ټ���!",MB_OK|MB_ICONEXCLAMATION);
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
	dlg.m_bValidPrice = TRUE;	//�����кϷ��ļ۸�
	dlg.m_bHasDeleteBtn = TRUE;	//��ɾ����ť
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
			if(TDX_MessageBox(m_hWnd,"����Ԥ����û�д�,���Ƿ����?",MB_YESNO|MB_ICONINFORMATION ) == IDYES)
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
	if(nID-ID_PADSWITCH1==0)				//��ǰ���û�
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
	//�رջ�ӭҳ
//	CSplashWnd::HideSplashScreen();
	//
	if(bStartTips)
		OnIdsTipoftheday();
	if(!OffLineMode)
	{
		//ͨ����Ϣ
		if(!g_GC.bNoUrgent)
			PostMessage(UM_SHOWURGENT_MESSAGE,URGENT_NORMAL,1);
		//���ڸ���������ѵ�¼��,����ͨ���ŵ�ͨ��
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
		int m = nowtime.wHour*60+nowtime.wMinute;  //����
		long lLastData = GetLastDate(g_lOpenRq);
		while(GetWeek(lLastData)==0 || GetWeek(lLastData)==6) lLastData = GetLastDate(lLastData);
		if(g_bYesterdayFlag || m>940 || GetVipDocLastDate()<lLastData)
			DownDayData_AtOnce(this,"��ѡ���˱�������ģʽ,�����,�����������ݿ��ܲ�ȫ,\r\n\r\n�����Ƿ�Ҫ���ز�ȫ?\n\n");
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
		//�����û���֤��Session
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
	 //�Զ�������Ϣ
	if(!g_GC.bNoAutoup)
	{
		bWriteAutoupTime=ShowAutoupInfo();
		if(bWriteAutoupTime)
			g_GH.nLastHintAutoupTime = nTime;
	}
	else if(g_GC.nShareMode || g_GC.bLocalData)
	{
		//�������������,���ܽ�������
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
	dlg.m_strTitle.Format("%s��Ҫ�����л���������",tempStkInfo->Code);
	dlg.DoModal();
}

#include "WriteUrgent.h"
void CMainFrame::OnWriteUrgent()
{
	if(!OnLine)
	{
		TDX_MessageBox(m_hWnd,"�ѻ�״̬�²��ܷ���!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	if(g_pWriteUrgent) 
	{
		TDX_MessageBox(m_hWnd,"ͨ�����ڷ�����,���Ժ�����!",MB_OK|MB_ICONEXCLAMATION);
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

//lParamΪ0��ʾ����;���� Ϊ1��ʾ�Ǹս���ʱ����
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
		TDX_MessageBox(m_hWnd,"�������ӵ�������վ!",MB_ICONINFORMATION|MB_OK);
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
	tmpHintStr.Format("%s���ӱ��Ͽ�!",g_DS.DSOEMName);
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
	OptionSheet.SetTitle (_F("����Ʒ�ֹ���"));
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
	OptionSheet.SetTitle (_F("����Ʒ�ֹ���"));
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
	OptionSheet.SetTitle (_F("����Ʒ�ֹ���"));
	OptionSheet.SetActivePage(&Page3);
	OptionSheet.DoModal();
	if(theViewManager.pActiveView)
		theViewManager.pActiveView->PostMessage(UM_BLOCKMODIFIED,0,0);
	LoadAndResetGridTab();
	GetZHBBlockNum();
}

//�۽���������
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
		TDX_MessageBox(pWnd->m_hWnd,"ȡ����ʧ��!",MB_OK|MB_ICONERROR);
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
	case 0:		//���ά���ع��ѵĻ�
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
	case 1:		//���ά���سֱֲ���Ļ�
	case 2:		//���ά������Ϣ�ٵݵĻ�
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
		//	if(Title.Find("�ɶ�����")>=0)
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
char *Tscp_21_Zb[21] = {"���̷���","������ת","���̸���","�����ź�","��������","��ʦ����","����ׯ��","����׷��","�ʽ���","�ײ�����","��������","��ӥ̤ׯ","�ײ�����","׷�ٺ���","˳�ƶ�Ϊ","��ӥ��ׯ","��ͣ׷��","��������","��������","׽ׯɢ��","�ʽ���"};
char *Info_5_Url[5] = {"111101azx.rjglpt.com/class.asp?id=47","111101azx.rjglpt.com/class.asp?id=48","111101azx.rjglpt.com/class.asp?id=49","111101azx.rjglpt.com/class.asp?id=50","111101azx.rjglpt.com/class.asp?id=51"};
char *Plugin_6_Url[6] = {"bainian.yhglpt.com/heju/hxnc.asp", "bainian.yhglpt.com/ggtx.asp", "bainian.yhglpt.com/jglhb.asp", "bainian.yhglpt.com/jggpc.asp", "bainian.yhglpt.com/dtzjfx.asp","juyingzx.kfglpt.com/jjzn.asp"};
char *NewPlugA_Url[8] = {"bainian.yhglpt.com/etz/dp1.asp", "bainian.yhglpt.com/etz/dp2.asp", "bainian.yhglpt.com/etz/dp3.asp", "bainian.yhglpt.com/etz/bk1.asp", "bainian.yhglpt.com/etz/bk2.asp", "bainian.yhglpt.com/etz/xg1.asp", "bainian.yhglpt.com/etz/xg2.asp", "bainian.yhglpt.com/etz/xg3.asp"};

char *JTC_ZBSet[3] = {"�����������", "�����������", "�����������"};
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

	case IDC_ZFJS_72:			//����캽��λ�ø���Ϊ������ƣ�����������ԭţ�ܹ�������
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
			//�ȼ������״̬
			MemStkInfo *pDPInfo = g_pStockIO->Get_MemStkInfo("000001", SH);
			int nSignalFlag = -1;
			//�ȿ��ǲ��Ǳ���
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

			//�İ����ļ�
			if(nSignalFlag==pCmdUI->m_nID-IDC_ZFJS_80+1)
			{
				for(int i=0;i<g_vDynaCmdTab.size();i++)
				{
					if(g_vDynaCmdTab[i].DefPadSet==IDC_ZFJS_PAD8)			//�ر���
						break;
				}
				g_vDynaCmdTab[i].DefPadCmdID = pCmdUI->m_nID;
				CString padini = g_WSXHStr+(CString)"sheet\\"+g_vDynaCmdTab[i].EnName+".sp";
				char *PadName[5] = {"������������", "�����𵴲���", "���̵�������", "�����µ�����", "�����������"};
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
			TDX_MessageBox(GetSafeHwnd(), _T("����û�п�ͨ��Ȩ�ޣ�Ȩ�޿�ͨ����ϵ�ͷ���"), MB_OK);
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
		if(wParam==1) //�޸�URL
		{
			m_pIWContentDlg->SetTitle(_F(g_strIWTitle));
			m_pIWContentDlg->SetInfo(0,0,g_strIWURL);
			m_pIWContentDlg->StartNavigate();
		}
	}
	else
	{
		//������ģʽ�Ի���һ��Ҫ��ָ�붯̬����
		m_pIWContentDlg = new CShowURL(this, TRUE);
		m_pIWContentDlg->m_strTitle = _F(g_strIWTitle);
		m_pIWContentDlg->SetInfo(0,0,g_strIWURL);
		m_pIWContentDlg->Create(IDD_SHOWURL);
		m_pIWContentDlg->ShowWindow(SW_SHOW);
	}	
}

void CMainFrame::OnCLXG()
{
	//�ȼ������״̬
	MemStkInfo *pDPInfo = g_pStockIO->Get_MemStkInfo("000001", SH);
	int nSignalFlag = -1;
	//�ȿ��ǲ��Ǳ���
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
	char *PadName[5] = {"������������", "�����𵴲���", "���̵�������", "�����µ�����", "�����������"};
	WritePrivateProfileString("DEAFULTGP", "Name", PadName[nSignalFlag-1], padini);
	WritePrivateProfileString("DEAFULTGP", "DefPadCmdID", IntToStr(nID), padini);
	WritePrivateProfileString("STEP0", "ConFlag", IntToStr(10+nSignalFlag-1), padini);

	SendMessage(WM_COMMAND, nID);
}

//////////////////////////////////////////////////////////////////////////
//ָ����ת
void CMainFrame::JumpToZBIndex(char *ZBCode, short nSetcode, char *GPCode, BOOL bIsKLine)
{
	if(GPCode==NULL)
	{
		if(bIsKLine) return;
		int MessageNo = GetMessageNoFromZB(ZBCode,FALSE);
		OpenComView(1,NULL,0,MessageNo);
		//���⴦���н��ģ��
		if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)))	
		{
			CComView *pView = (CComView *)theViewManager.pActiveView;
			if(strcmp("�����������", ZBCode)==0)
			{
				pView->SendMessage(WM_COMMAND, ID_DAY);
				pView->JumpToZB("", 3);
				pView->JumpToZB("��������", 0);
				pView->JumpToZB("VOL", 1);
				pView->JumpToZB("MACD", 2);
			}
			else if(strcmp("�����������", ZBCode)==0)
			{
				pView->SendMessage(WM_COMMAND, ID_MIN15);
				pView->JumpToZB("", 3);
				pView->JumpToZB("��������", 0);
				pView->JumpToZB("VOL", 1);
				pView->JumpToZB("MACD", 2);
			}
			else if(strcmp("�����������", ZBCode)==0)
			{
				pView->SendMessage(WM_COMMAND, ID_DAY);
				pView->JumpToZB("", 3);
				pView->JumpToZB("��������", 0);
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
		if(strcmp("�����������", ZBCode)==0)
		{
			pView->SendMessage(WM_COMMAND, ID_DAY);
			pView->JumpToZB("", 3);
			pView->JumpToZB("��������", 0);
			pView->JumpToZB("VOL", 1);
			pView->JumpToZB("MACD", 2);
		}
		else if(strcmp("�����������", ZBCode)==0)
		{
			pView->SendMessage(WM_COMMAND, ID_MIN15);
			pView->JumpToZB("", 3);
			pView->JumpToZB("��������", 0);
			pView->JumpToZB("VOL", 1);
			pView->JumpToZB("MACD", 2);
		}
		else if(strcmp("�����������", ZBCode)==0)
		{
			pView->SendMessage(WM_COMMAND, ID_MIN15);
			pView->JumpToZB("", 3);
			pView->JumpToZB("��������", 0);
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
	
	//�ж�ʱ��
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
	//�ж�Ȩ��
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
		//������ģʽ�Ի���һ��Ҫ��ָ�붯̬����
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

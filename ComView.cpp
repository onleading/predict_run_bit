// ComView.cpp : implementation file
#include "stdafx.h"
#include "tdxw.h"
#include "ComView.h"
#include "InfoDlg.h"
#include "MainFrm.h"
#include "UBaseInner.h"
#include "Plugin_gn.h"
#include "ComView_sub.h"

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CComView, CView)

BEGIN_MESSAGE_MAP(CComView, CUnitView)
	ON_COMMAND(ID_WRITETOTXTFILE, OnWritetotxtfile)

	ON_COMMAND_RANGE(ID_CORNER_1,ID_CORNER_10,OnCornerClick)
	ON_COMMAND_RANGE(ID_CORNER2_1,ID_CORNER2_7,OnCorner2Click)
	ON_COMMAND_RANGE(ID_CORNER3_1,ID_CORNER3_15,OnCorner3Click)
	ON_COMMAND(ID_MORE_SHOW,OnMoreShow)

	ON_COMMAND(ID_ZOOMCORNER,OnZoomCorner)
	ON_COMMAND(ID_ZOOMTOCORNER,OnZoomToCorner)

	ON_COMMAND(ID_MULUNIT,OnMulUnit)
	ON_UPDATE_COMMAND_UI(ID_MULUNIT,OnUpdateMulUnit)
	ON_COMMAND(ID_MULPERIOD,OnMulPeriod)
	ON_UPDATE_COMMAND_UI(ID_MULPERIOD,OnUpdateMulPeriod)
	ON_COMMAND(ID_ZSTPLAY,OnZstPlay)
	ON_UPDATE_COMMAND_UI(ID_ZSTPLAY,OnUpdateZstPlay)
	ON_COMMAND(ID_FXT_MOVE,OnFxtMove)
	ON_UPDATE_COMMAND_UI(ID_FXT_MOVE,OnUpdateFxtMove)

	ON_UPDATE_COMMAND_UI_RANGE(ID_300,ID_508,OnUpdateOtherFx)	

	ON_MESSAGE(UM_READANDPAINT,OnReadAndPaint)
	ON_MESSAGE(UM_VSCROLLBAR,OnVScrollMessage)

	ON_COMMAND_RANGE(ID_LEVIN_SENDYMD,ID_LEVIN_CANCEL,OnLevinMenu)

	ON_COMMAND(IDC_COMGOBACK, OnGoBack)
	ON_UPDATE_COMMAND_UI(IDC_COMGOBACK, OnUpDateGoBack)
	ON_COMMAND(IDC_COMGONEXT, OnGoNext)
	ON_UPDATE_COMMAND_UI(IDC_COMGONEXT, OnUpDateGoNext)
	//{{AFX_MSG_MAP(CComView)
	ON_WM_SIZE()
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_HIDERIGHT, OnHideright)
	ON_COMMAND(ID_ZST_LEVEL2FACE,OnZSTLevel2Face)
	ON_UPDATE_COMMAND_UI(ID_ZST_LEVEL2FACE,OnUpdateZSTLevel2Face)
	ON_COMMAND(ID_FXT_LEVEL2FACE,OnFXTLevel2Face)
	ON_UPDATE_COMMAND_UI(ID_FXT_LEVEL2FACE,OnUpdateFXTLevel2Face)
	ON_COMMAND(ID_TICK2PRIORITY,OnTick2Priority)
	ON_UPDATE_COMMAND_UI(ID_TICK2PRIORITY,OnUpdateTick2Priority)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_SWITCHLBMMLD, OnSwitchlbmmld)
	ON_WM_CHAR()
	ON_COMMAND(ID_PAGEUP, OnPageup)
	ON_COMMAND(ID_PAGEDOWN, OnPagedown)
	ON_WM_CREATE()
	ON_COMMAND(ID_DCOMLAYER, OnDcomlayer)
	ON_UPDATE_COMMAND_UI(ID_DCOMLAYER, OnUpdateDcomlayer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

UINT l_nMulPeriodOrder[8]={PER_MIN1,PER_MIN5,PER_MIN15,PER_MIN30,PER_HOUR,PER_DAY,PER_WEEK,PER_MONTH};

#define V_NORMAL		0
#define V_F10			1
#define V_CWCT			2

int g_nVStatus = V_NORMAL;

/////////////////////////////////////////////////////////////////////////////

int CComView::GetCurSwitch(DWORD AssisPad)
{
	switch(AssisPad)
	{
	case ZJ_PAD:			return 0;
	case HQTICK_PAD:		return 1;
	case YD_PAD:			return 2;
	case HQFJB_PAD:			return 3;

	case HQ10QUEUE_PAD:		return 0;
	case HQ10WTFB_PAD:		return 1;
	case HQ10TICKSTAT_PAD:	return 2;
	case HQ10SCANCEL_PAD:	return 4;
	case HQ10TCANCEL_PAD:	return 5;
	case HQ10WTRANK_PAD:	return 6;
	case HQ10HYCJ_PAD:		return 7;
	}
	return 0;
}

DWORD CComView::GetAssis1FromSwitch(int nWhich)
{
	switch(nWhich)
	{
	case 0:
		return ZJ_PAD;
	case 1:
		return HQTICK_PAD;
	case 2:
		return YD_PAD;
	case 3:
		return HQFJB_PAD;
	}
	return 0;
}

DWORD CComView::GetAssis2FromSwitch(int nWhich)
{
	switch(nWhich)
	{
	case 0:
		return HQ10QUEUE_PAD;
	case 1:
		return HQ10WTFB_PAD;
	case 2:
		return HQ10TICKSTAT_PAD;
	case 4:
		return HQ10SCANCEL_PAD;
	case 5:
		return HQ10TCANCEL_PAD;
	case 6:
		return HQ10WTRANK_PAD;
	case 7:
		return HQ10HYCJ_PAD;
	}
	return 0;
}

void CComView::AdjustLevel2Pad(DWORD &tmpPad,BOOL bProperty)
{
	DWORD tmpMainPad   = tmpPad & 0xFF0000;
	DWORD tmpAssisPad1 = tmpPad & 0x00FF00;
	DWORD tmpAssisPad2 = tmpPad & 0x0000FF;
	if(tmpMainPad!=ZST_PAD && tmpMainPad!=FXT_PAD) return;
	if(bProperty)
		tmpPad=tmpMainPad+((tmpAssisPad1==HQTICK_PAD&&g_GH.bTick2Priority)?HQTICK2_PAD:tmpAssisPad1)+tmpAssisPad2;
	else
		tmpPad=tmpMainPad+(tmpAssisPad1==HQTICK2_PAD?HQTICK_PAD:tmpAssisPad1)+tmpAssisPad2;
}

BOOL CComView::IsSpecAssisPad1(DWORD AssisPad1)
{
	return (AssisPad1==CB_PAD||AssisPad1==BIGVOL_PAD);
}

CComView::CComView()
{
	WindowMacro = COMVIEW_WIN;
	m_bFxtMove  = FALSE;
	//翻页信息
	m_nPageGPNum = -1;
	m_nCurPageGP = -1;
	m_nMulGpNum = 1;
	m_nCurMulGp = 0;
	//远程使用
	m_nMainID = 0;
	//处理多股同列时使用
	m_bMult_In = FALSE;
	m_bNoChange_MultIn = FALSE;
	memset(&m_LSetInfo, 0, sizeof(LinkedSetInfo));
	//
	m_bSplitting1 = FALSE;
	m_bSplitting2 = FALSE;
	m_nOldSplitX = -10;	//置为无效
	m_nTranZstStyle = ZST_BIG;
	//
	m_nCurGP = g_pStockIO->GetIndex(ComCurGPCode,ComCurSetCode);
	if(m_nCurGP < 0) m_nCurGP = 14;
	m_nNowPad = 0;
	m_nAssisPad1 = 0;	//初值设为0,GetPadFromMessageCode中要用到
	m_nAssisPad2 = 0;	//初值设为0,GetPadFromMessageCode中要用到
	if(ComCurMessageCode != -1)
	{
		m_nNowPad = GetPadFromMessageCode(ComCurMessageCode);
		Proc_Special_MessageCode(ComCurMessageCode);
		ComCurMessageCode = -1;
	}
	if(m_nNowPad==0)
		m_nNowPad = FXT_PAD+(g_GH.bPinAssistPad?HQTICK_PAD:HQZST_PAD)+HQ10QUEUE_PAD;
	BOOL bProperty=IsLevel2Property((*g_pStockIO)[m_nCurGP]);
	AdjustLevel2Pad(m_nNowPad,bProperty);
	//
	m_nLastPad =  m_nNowPad;
	m_bNowShow = m_bLastShow = g_bRightShow;
	//
	m_nMainPad   = m_nNowPad & 0xFF0000;
	m_nAssisPad1 = m_nNowPad & 0x00FF00;
	m_nAssisPad2 = m_nNowPad & 0x0000FF;
	m_nCurSwitch1 = GetCurSwitch(m_nAssisPad1);
	m_nCurSwitch2 = GetCurSwitch(m_nAssisPad2);
	m_pSwitchBar1 = NULL;
	m_pSwitchBar2 = NULL;
	m_pSwitchBar3 = NULL;

	m_GoCache.clear();
	m_nGoCacheCursor = -1;
	m_bInGoCache = FALSE;

	m_nDComCurGP = -1;
	m_nDComUnit = -1;

	m_bShowAssisPad2 = FALSE;

	m_bMultiPeriod	= FALSE;
	m_nFirstPeriod	= 0;
	m_nCurPeriodOff	= 0;

	m_bShowSwitch3 = TRUE;
	
	m_nMore_Sel = 0;
	m_bZstMore_Show = FALSE;
	m_bFxtMore_Show = FALSE;

	m_nSwitchUnitNum = SwitchUnitNum;
}

CComView::~CComView()
{
	TDEL(m_pSwitchBar1);
	TDEL(m_pSwitchBar2);
	TDEL(m_pSwitchBar3);
	theViewManager.RemoveView(this);
	DestroyPad();
	if(m_nMainPad==ZST_PAD || m_nMainPad==FXT_PAD)
		g_bRightShow = m_bNowShow;
}

int CComView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CUnitView::OnCreate(lpCreateStruct) == -1)
		return -1;

	//创建Assis1区切换按钮
	int i;
	m_pSwitchBar1 = new CNewZfjsTab(m_hWnd);
	m_pSwitchBar1->SetStyle(SwitchStyle_samebutton);
	if(!m_pSwitchBar1 || !m_pSwitchBar1->Create(_T("NewZfjsTab"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_COMSWITCHBAR1))
		return -1;
	for(i=0;i<m_nSwitchUnitNum;i++)
		m_pSwitchBar1->InsertSwitchLabel(i,_F(SwitchUnitStr[i]),_F(SwitchUnitStr_Tips[i]),WM_COMMAND,ID_CORNER_1+i);
	//创建Assis2区切换按钮
	m_pSwitchBar2 = new CNewZfjsTab(m_hWnd);
	m_pSwitchBar2->SetStyle(SwitchStyle_samebutton);
	if(!m_pSwitchBar2 || !m_pSwitchBar2->Create(_T("NewZfjsTab"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_COMSWITCHBAR2))
		return -1;
	for(i=0;i<SwitchUnitNum_Mid;i++)
		m_pSwitchBar2->InsertSwitchLabel(i,_F(SwitchUnitStr_Mid[i]),_F(SwitchUnitStr_Mid_Tips[i]),WM_COMMAND,ID_CORNER2_1+i);
	//创建分时图MoreUnit区切换按钮
	m_pSwitchBar3 = new CNewZfjsTab(m_hWnd);
	m_pSwitchBar3->SetStyle(SwitchStyle_switchbutton);
	if(!m_pSwitchBar3 || !m_pSwitchBar3->Create(_T("NewZfjsTab"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_COMSWITCHBAR3))
		return -1;
	int nFrom = 0;
	m_pSwitchBar3->InsertSwitchLabel(nFrom++,_F("扩展区"),_F("关闭或打开扩展分析区,`键(ESC下面的那个键)可以开关扩展区"),WM_COMMAND,ID_MORE_SHOW,0,0,1);
	for(i = 0;i < g_SwitchMoreNum;i++)
	{
		m_pSwitchBar3->InsertSwitchLabel(nFrom,_F(g_SwitchMore[i].pStr),_F(g_SwitchMore[i].pTipStr),WM_COMMAND,ID_CORNER3_1+i,0,0,1);
		if(g_SwitchMore[i].nFlag==1)
			m_pSwitchBar3->SetSpecialTextColor(nFrom,VipColor.VOLCOLOR);
		nFrom++;
	}

	return 0;
}

void CComView::OnInitialUpdate()
{
	CUnitView::OnInitialUpdate();
	theViewManager.AddView("行情分析", this);	
	ConstructPad(TRUE);
}

//此函数仅对远程请求有效，主要为了保证先读行情，再读Zst,Tick等
void CComView::ReadAndPaint()
{
	int i;
	if(m_nMainPad == MULZST_PAD || m_nMainPad == MULFXT_PAD)
	{
		FOR_UNIT(i)
			m_Units.pUnit[i]->ProcessMsg(WM_PAINT,1);
	}
	else
	{
		if(m_Units.Num == 1)
			m_Units.pUnit[0]->ProcessMsg(WM_PAINT,1);
		else if(m_Units.Num > 1)
		{
			m_Units.pUnit[1]->ProcessMsg(WM_PAINT,1);
			for(i=0;i < m_Units.Num && m_Units.pUnit[i];i++)
			{
				if(i == 1) continue;
				m_Units.pUnit[i]->ProcessMsg(WM_PAINT,1);
			}
		}
	}
}

void CComView::OnReadAndPaint(WPARAM wParam,LPARAM lParam)
{
	if(g_pComm)
	{
		g_pComm->m_bBuffEven = TRUE;
		g_pComm->m_bDirectEven = TRUE;
	}
	if(g_pComm3)
	{
		g_pComm3->m_bBuffEven = TRUE;
		g_pComm3->m_bDirectEven = TRUE;
	}
	SetMainID();
	ReadAndPaint();	
}

//处理一些特别消息，如果返回TRUE的话，则需要重新构造整体Unit
BOOL CComView::Proc_Special_MessageCode(int tmpMessageCode)
{
	if(tmpMessageCode==506)
	{
		m_nTranZstStyle=ZST_LB;
		return TRUE;
	}
	else if(tmpMessageCode==507)
	{
		m_nTranZstStyle=ZST_MMLD;
		return TRUE;
	}
	else if(tmpMessageCode==508)
	{
		BOOL OldTickTStatus=g_bTickTStatus;
		g_bTickTStatus=TRUE;
		m_nTranZstStyle=ZST_BIG;
		return (OldTickTStatus!=g_bTickTStatus);
	}
	else if(tmpMessageCode==501)
	{
		BOOL OldTickTStatus=g_bTickTStatus;
		g_bTickTStatus=FALSE;
		return (OldTickTStatus!=g_bTickTStatus);
	}
	if(tmpMessageCode>=101 && tmpMessageCode<=119) //如果是开始进入是大盘，则重新得到m_nCurGP;
	{
		if(tmpMessageCode <= 106)
			m_nCurGP = g_pStockIO->GetIndex(DpGpCode[tmpMessageCode-101],SH); 
		else if(tmpMessageCode <= 113 || tmpMessageCode == 116 || tmpMessageCode == 117 || tmpMessageCode == 119)
			m_nCurGP = g_pStockIO->GetIndex(DpGpCode[tmpMessageCode-101],SZ); 
		else
			m_nCurGP = g_pStockIO->GetIndex(DpGpCode[tmpMessageCode-101],SH); 
		if(tmpMessageCode==105 || tmpMessageCode==112)
			m_nTranZstStyle = ZST_MMLD;
		else if(tmpMessageCode==106 || tmpMessageCode==113)
			m_nTranZstStyle = ZST_LB;
		if(m_nCurGP >= 0)
		{
			ComCurGPCode = (*g_pStockIO)[m_nCurGP]->Code;
			ComCurSetCode = (*g_pStockIO)[m_nCurGP]->setcode;
		}
		return TRUE;
	}
	if(tmpMessageCode >= 301 && tmpMessageCode <= 300+g_nOftenUseZb) //指标命令
	{
		m_nOften301 = tmpMessageCode;
		m_nFxPeriod = g_nMenuPeriod;
		return TRUE;
	}
	if(tmpMessageCode == 900)
	{
		short   indexbuffer[MAXGPNUM];
		int totalnum = GetGpIndex(g_nMul_nBlockFlag,g_nMul_WhichGPType,indexbuffer);
		if(g_nMul_CurGP < 0)
			SetPageGP(indexbuffer,totalnum,indexbuffer[0]);
		else
		{
			LinkedSetInfo *pLSetInfo = NULL;
			GetSYSCurrentLSetInfo(pLSetInfo);
			SetPageGP(indexbuffer,totalnum,g_nMul_CurGP,pLSetInfo);
		}
		m_nCurMulGp = 0;
		g_nMulNum = g_nNewMulNum;
		m_nMulGpNum = GetOne2MulGp(m_nMulGpIndex,g_nMulNum,TRUE);
		if(m_nCurGP >= 0)
		{
			MemStkInfo *tmpStkInfo = (*g_pStockIO)[m_nCurGP]; //如果是多股同列,重新计算ComCurGPCode等
			if(tmpStkInfo)
			{
				ComCurGPCode = tmpStkInfo->Code;
				ComCurSetCode = tmpStkInfo->setcode;
			}
		}
		return TRUE;
	}
	return FALSE;
}

//创建一个Unit
void CComView::CreateUnit(int which,int initGPNo,int type,int style)
{
	TDEL(m_Units.pUnit[which]);	//先删除原来的Unit
	
	m_Units.Type[which] = type;
	m_Units.Style[which] = style;
	m_Units.nStatus[which] = U_SHOW;
	if(which==1 && (type==HQ_UNIT||type==LOWHQ_UNIT||type==CB_UNIT||type==BIGVOL_UNIT||type==WEB_UNIT))
		m_Units.nStatus[which] = m_bLastShow?U_SHOW:U_HIDE;
	COMINFO ComInfo;
	ComInfo.pWnd		= this;
	ComInfo.nNowPad		= m_nNowPad;
	ComInfo.pUnits		= &m_Units;
	ComInfo.nWhichUnit	= which;
	ComInfo.nUnitType	= type;
	ComInfo.nUnitStyle	= style;
	ComInfo.nInCom2		= 0;
	ComInfo.nInDlg		= 0;
	ComInfo.nInMultiPeriod = 0;
	if(m_bMultiPeriod)
		ComInfo.nInMultiPeriod = 1;
	ComInfo.bPlugin = 0;
	ComInfo.bPlugin_GPRel = 0;
	switch(type)
	{
	case ZST_UNIT:	//走势图组件
		m_Units.pUnit[which] = new UZst(&ComInfo);
		break;
	case FXT_UNIT:	//分析图组件
		m_Units.pUnit[which] = new UFxt(&ComInfo);
		break;
	case HQ_UNIT:	//行情图组件
		m_Units.pUnit[which] = new UHq(&ComInfo);
		break;
	case LOWHQ_UNIT://小行情图组件
		m_Units.pUnit[which] = new UHq(&ComInfo,TRUE);
		break;
	case TICK_UNIT:	//分时成交组件
		if(g_bUseKList)
		{
			m_Units.Type[which] = KLIST_UNIT;
			ComInfo.nUnitType = KLIST_UNIT;
			m_Units.pUnit[which] = new UKList(&ComInfo);
			g_bUseKList = FALSE;
		}
		else
			m_Units.pUnit[which] = new UTick(&ComInfo);
		break;
	case FJB_UNIT:	//分价表组件
		m_Units.pUnit[which] = new UFjb(&ComInfo);
		break;
	case CDP_UNIT:	//相关数值组件
		m_Units.pUnit[which] = new UCdp(&ComInfo);
		break;
	case MMP_UNIT:	//详细买卖盘组件
		m_Units.pUnit[which] = new UMmp(&ComInfo);
		break;
	case CB_UNIT:	//移动筹码组件
		m_Units.pUnit[which] = new UCb(&ComInfo);
		break;
	case BIGVOL_UNIT://主力监控精灵组件
		m_Units.pUnit[which] = new UBigVol(&ComInfo);
		break;
	case WEB_UNIT:	//通用浏览组件
		m_Units.pUnit[which] = new UWeb(&ComInfo);
		break;
	case SEAT_UNIT://买家经纪排位
		m_Units.pUnit[which] = new USeat(&ComInfo);
		break;
	case HQ10_UNIT:	//十档买卖盘组件
		m_Units.pUnit[which] = new UHq10(&ComInfo);
		break;
	case TICK2_UNIT://逐笔成交组件
		m_Units.pUnit[which] = new UTick2(&ComInfo);
		break;
	case QUEUE_UNIT://买卖队列组件
		m_Units.pUnit[which] = new UQueue(&ComInfo);
		break;
	case TICKSTAT_UNIT:	//龙虎看盘组件
		m_Units.pUnit[which] = new UTickStat(&ComInfo);
		break;
	case WTFB_UNIT:		//委托分布图
		m_Units.pUnit[which] = new UWtfb(&ComInfo);
		break;
	case WTRANK_UNIT:	//委托排名组件
		m_Units.pUnit[which] = new UWTRank(&ComInfo);
		break;
	case SCANCEL_UNIT:	//即时撤单排名
		m_Units.pUnit[which] = new USCancel(&ComInfo);
		break;
	case TCANCEL_UNIT:	//累计撤单排名
		m_Units.pUnit[which] = new UTCancel(&ComInfo);
		break;
	case HYCJ_UNIT:		//行业成交排名
		m_Units.pUnit[which] = new UHycj(&ComInfo);
		break;
	case TV_UNIT:		//TopView组件
		m_Units.pUnit[which] = new UTVGraph(&ComInfo);
		break;
	case TICKZST_UNIT:	//闪电图
		m_Units.pUnit[which] = new UTickZst(&ComInfo);
		break;
	case F10_UNIT:		//F10资料
		m_Units.pUnit[which] = new UBaseInner(&ComInfo,"");
		break;
	case MORE_UNIT:	//分时图分析图下部组件群
		{
			int nMoreValue = MORE_GLHQ;
			if(m_nMore_Sel >= 0 && m_nMore_Sel<g_SwitchMoreNum)
				nMoreValue = g_SwitchMore[m_nMore_Sel].nMoreValue;
			switch(nMoreValue)
			{
			case MORE_GPPAGE:	//个股综合资讯
				{
					m_Units.Type[which] = GPPAGE_UNIT;
					ComInfo.nUnitType = GPPAGE_UNIT;
					m_Units.pUnit[which] = new UGPWeb(&ComInfo);
					((UWeb*)m_Units.pUnit[which])->SetURL("http://gppage",FALSE,TRUE);
					((UWeb*)m_Units.pUnit[which])->SetSimpleWeb(FALSE);
				}
				break;
			case MORE_META_RP:	//META:个股研究报告
				{
					m_Units.Type[which] = GPPAGE_UNIT;
					ComInfo.nUnitType = GPPAGE_UNIT;
					m_Units.pUnit[which] = new UGPWeb(&ComInfo);
					((UGPWeb*)m_Units.pUnit[which])->SetMode(GPWEB_VIEWTHEM,GPWEB_VIEWTHEM_RP);
					CString tmpURL;
					tmpURL.Format("%swebs\\web_cache\\rpxxxxxx.htm",HomePath);
					((UWeb*)m_Units.pUnit[which])->SetURL(tmpURL,FALSE,TRUE);
					((UWeb*)m_Units.pUnit[which])->SetSimpleWeb(FALSE);
				}
				break;
			case MORE_META_PJ:	//META:个股评级
				{
					m_Units.Type[which] = GPPAGE_UNIT;
					ComInfo.nUnitType = GPPAGE_UNIT;
					m_Units.pUnit[which] = new UGPWeb(&ComInfo);
					((UGPWeb*)m_Units.pUnit[which])->SetMode(GPWEB_VIEWTHEM,GPWEB_VIEWTHEM_PJ);
					CString tmpURL;
					tmpURL.Format("%swebs\\web_cache\\pjxxxxxx.htm",HomePath);
					((UWeb*)m_Units.pUnit[which])->SetURL(tmpURL,FALSE,TRUE);
					((UWeb*)m_Units.pUnit[which])->SetSimpleWeb(FALSE);
				}
				break;
			case MORE_META_HY:	//META:行业资讯
				{
					m_Units.Type[which] = GPPAGE_UNIT;
					ComInfo.nUnitType = GPPAGE_UNIT;
					m_Units.pUnit[which] = new UGPWeb(&ComInfo);
					((UGPWeb*)m_Units.pUnit[which])->SetMode(GPWEB_VIEWTHEM,GPWEB_VIEWTHEM_HY);
					CString tmpURL;
					tmpURL.Format("%swebs\\web_cache\\hyxxxxxx.htm",HomePath);
					((UWeb*)m_Units.pUnit[which])->SetURL(tmpURL,FALSE,TRUE);
					((UWeb*)m_Units.pUnit[which])->SetSimpleWeb(FALSE);
				}
				break;
			case MORE_META_GG:	//META:公司公告
				{
					m_Units.Type[which] = GPPAGE_UNIT;
					ComInfo.nUnitType = GPPAGE_UNIT;
					m_Units.pUnit[which] = new UGPWeb(&ComInfo);
					((UGPWeb*)m_Units.pUnit[which])->SetMode(GPWEB_VIEWTHEM,GPWEB_VIEWTHEM_GG);
					CString tmpURL;
					tmpURL.Format("%swebs\\web_cache\\ggxxxxxx.htm",HomePath);
					((UWeb*)m_Units.pUnit[which])->SetURL(tmpURL,FALSE,TRUE);
					((UWeb*)m_Units.pUnit[which])->SetSimpleWeb(FALSE);
				}
				break;				
			case MORE_META_XW:	//META:公司新闻
				{
					m_Units.Type[which] = GPPAGE_UNIT;
					ComInfo.nUnitType = GPPAGE_UNIT;
					m_Units.pUnit[which] = new UGPWeb(&ComInfo);
					((UGPWeb*)m_Units.pUnit[which])->SetMode(GPWEB_VIEWTHEM,GPWEB_VIEWTHEM_XW);
					CString tmpURL;
					tmpURL.Format("%swebs\\web_cache\\xwxxxxxx.htm",HomePath);
					((UWeb*)m_Units.pUnit[which])->SetURL(tmpURL,FALSE,TRUE);
					((UWeb*)m_Units.pUnit[which])->SetSimpleWeb(FALSE);
				}
				break;		
			case MORE_META_GW:	//META:告和新闻合并
				{
					m_Units.Type[which] = GPPAGE_UNIT;
					ComInfo.nUnitType = GPPAGE_UNIT;
					m_Units.pUnit[which] = new UGPWeb(&ComInfo);
					((UGPWeb*)m_Units.pUnit[which])->SetMode(GPWEB_VIEWTHEM,GPWEB_VIEWTHEM_GW);
					CString tmpURL;
					tmpURL.Format("%swebs\\web_cache\\gwxxxxxx.htm",HomePath);
					((UWeb*)m_Units.pUnit[which])->SetURL(tmpURL,FALSE,TRUE);
					((UWeb*)m_Units.pUnit[which])->SetSimpleWeb(FALSE);
				}
				break;
			case MORE_ZHSORT:
				{
					m_Units.Type[which] = ZHSORT_UNIT;
					ComInfo.nUnitType = ZHSORT_UNIT;
					m_Units.pUnit[which] = new UZhSort(&ComInfo);
					UZhSort *pUZhSort = (UZhSort*)m_Units.pUnit[which];
					pUZhSort->SetStyle(STYLE_1X5);
					pUZhSort->WhichType = 0;
					pUZhSort->ProcessMsg(WM_CHANGESCTYPE);
					break;
				}
				break;
			default:			//关联分析组件
				m_Units.Type[which] = GLHQ_UNIT;
				ComInfo.nUnitType = GLHQ_UNIT;
				m_Units.pUnit[which] = new UGlHq(&ComInfo);
				((UGlHq *)m_Units.pUnit[which])->Create(this,GLHQ_BLOCK_GP);
				break;
			}
		}
		break;
	}
	ChangeUnitGP(which,initGPNo);
	if(type == FXT_UNIT)
	{
		if(m_nFxPeriod != -1)
			m_Units.pUnit[which]->ProcessMsg(UM_SET_FXPERIOD,m_nFxPeriod);
		if(m_nOften301  != -1)
		{
			long iNo = g_pCalc->_CallDTopViewCalc_9(ZB_TYPE,g_strOftenUseZb[m_nOften301-301]);
			if(iNo >= 0)
				m_Units.pUnit[0]->ProcessMsg(UM_SET_OFTENZB,iNo);
		}
		m_nOften301 = -1;
	}
}

//改变某组件的当前股票
BOOL CComView::ChangeUnitGP(int which,int iNo)
{
	if(m_Units.pUnit[which]) 
		m_Units.pUnit[which]->ProcessMsg(UM_SET_STOCK,iNo);
	if(!m_bNoChange_MultIn)	
		m_bMult_In = FALSE;
	return TRUE;
}

//设置窗口标题
void CComView::SetComTitle()
{
	char *tmpCode = g_pStockIO->GetNameFromIndex(m_nCurGP);
	theViewManager.SetViewName(tmpCode, this);
//	GetParent()->SetWindowText((CString)_F("行情分析-")+tmpCode);
	AfxGetMainWnd()->SetWindowText(g_strTitleName);
		
	//do GoCache
	if(!m_bInGoCache&&!m_bMult_In)
	{
		while(m_nGoCacheCursor<m_GoCache.size()-1) m_GoCache.pop_back();

		short nLastGp = -1, nCurorGp = -1;
		if(!m_GoCache.empty())
		{
			int nLast = m_GoCache.size()-1;
			nLastGp = m_GoCache[nLast];
			if(m_nGoCacheCursor>=0&&m_nGoCacheCursor<m_GoCache.size())
				nCurorGp = m_GoCache[m_nGoCacheCursor];
		}
		if(nLastGp!=m_nCurGP&&nCurorGp!=m_nCurGP) 
		{
			m_GoCache.push_back(m_nCurGP);
			m_nGoCacheCursor = m_GoCache.size()-1;
		}
	}
	m_bInGoCache = FALSE;
}

//重新构造版面
void CComView::ConstructPad(BOOL bFirst)
{
	DestroyPad();//先删除版面
	BOOL bProperty=IsLevel2Property((*g_pStockIO)[m_nCurGP]);
	if(m_nMainPad==ZST_PAD)
		m_bShowAssisPad2 = (bProperty && g_GH.bZSTLevel2Face);
	else
		m_bShowAssisPad2 = (bProperty && g_GH.bFXTLevel2Face);
	int count = 0,i;
	switch(m_nMainPad)
	{
	case TICK_PAD:
		CreateUnit(count++,m_nCurGP,TICK_UNIT,TICK_BIG);
		break;
	case FJB_PAD:
		CreateUnit(count++,m_nCurGP,FJB_UNIT,FJB_BIG);
		break;
	case TICK2_PAD:
		CreateUnit(count++,m_nCurGP,TICK2_UNIT,TICK_BIG);
		break;
	case MMP_PAD:
		CreateUnit(count++,m_nCurGP,MMP_UNIT,MMP_BIG);
		break;
	case MULZST_PAD:
	case MULFXT_PAD:
		if(m_bMultiPeriod && m_nMainPad==MULFXT_PAD)
		{
			for(i = 0;i < 6;i++)
			{
				CreateUnit(count++,m_nCurGP,FXT_UNIT,FXT_MULTI);
				m_Units.pUnit[i]->ProcessMsg(UM_SET_FXPERIOD,l_nMulPeriodOrder[(i+m_nFirstPeriod)%8]);
				m_Units.pUnit[i]->ProcessMsg(UM_SETSEL,m_nCurPeriodOff==i);
			}
		}
		else
		{
			for(i = 0;i < g_nMulNum;i++)
			{
				CreateUnit(count++,m_nMulGpIndex[i],m_nMainPad==MULZST_PAD?ZST_UNIT:FXT_UNIT,m_nMainPad==MULZST_PAD?ZST_MULTI:FXT_MULTI);
				m_Units.pUnit[i]->ProcessMsg(UM_SETSEL,i==m_nCurMulGp);
			}
		}
		m_bMult_In = TRUE;
		m_bNoChange_MultIn = TRUE;
		break;
	case ZST_PAD:
	case FXT_PAD:
		if(m_nMainPad==ZST_PAD)
		{
			if(!g_bTickTStatus) 
				CreateUnit(count++,m_nCurGP,ZST_UNIT,m_nTranZstStyle);
			else
				CreateUnit(count++,m_nCurGP,TICKZST_UNIT,m_nTranZstStyle);
		}
		else
			CreateUnit(count++,m_nCurGP,FXT_UNIT,FXT_BIG);
		if(!m_bShowAssisPad2)	CreateUnit(count++,m_nCurGP,HQ_UNIT);
		else					CreateUnit(count++,m_nCurGP,LOWHQ_UNIT);
		switch(m_nAssisPad1)
		{
		case HQTICK_PAD:
			CreateUnit(count++,m_nCurGP,TICK_UNIT,TICK_SMALL);
			break;
		case HQFJB_PAD:
			CreateUnit(count++,m_nCurGP,FJB_UNIT,FJB_SMALL);
			break;
		case HQTICK2_PAD:
			CreateUnit(count++,m_nCurGP,TICK2_UNIT,TICK_SMALL);
			break;
		case HQMMP_PAD:
			CreateUnit(count++,m_nCurGP,MMP_UNIT,MMP_SMALL);
			break;
		case HQZST_PAD:
			CreateUnit(count++,m_nCurGP,ZST_UNIT,ZST_SMALL);
			break;
		case HQZS_PAD:
			CreateUnit(count++,GetGPZS(m_nCurGP),ZST_UNIT,ZST_GPZS);
			break;
		case HQCDP_PAD:
			CreateUnit(count++,m_nCurGP,CDP_UNIT);
			break;
		case CB_PAD:
			CreateUnit(count++,m_nCurGP,CB_UNIT);
			break;
		case BIGVOL_PAD:
			CreateUnit(count++,m_nCurGP,BIGVOL_UNIT);
			break;
		case ZJ_PAD:
			CreateUnit(count++,m_nCurGP,TICKSTAT_UNIT);
			break;
		case YD_PAD:
			CreateUnit(count++,m_nCurGP,BIGVOL_UNIT);
			break;
		}
		switch(m_nAssisPad2)
		{
		case HQ10QUEUE_PAD:
			CreateUnit(count++,m_nCurGP,HQ10_UNIT);
			CreateUnit(count++,m_nCurGP,QUEUE_UNIT);
			break;
		case HQ10WTFB_PAD:
			CreateUnit(count++,m_nCurGP,HQ10_UNIT);
			CreateUnit(count++,m_nCurGP,WTFB_UNIT);
			break;
		case HQ10TICKSTAT_PAD:
			CreateUnit(count++,m_nCurGP,HQ10_UNIT);
			CreateUnit(count++,m_nCurGP,TICKSTAT_UNIT);
			break;
		case HQ10WTRANK_PAD:
			CreateUnit(count++,m_nCurGP,HQ10_UNIT);
			CreateUnit(count++,m_nCurGP,WTRANK_UNIT);
			break;
		case HQ10SCANCEL_PAD:
			CreateUnit(count++,m_nCurGP,HQ10_UNIT);
			CreateUnit(count++,m_nCurGP,SCANCEL_UNIT);
			break;
		case HQ10TCANCEL_PAD:
			CreateUnit(count++,m_nCurGP,HQ10_UNIT);
			CreateUnit(count++,m_nCurGP,TCANCEL_UNIT);
			break;
		case HQ10HYCJ_PAD:
			CreateUnit(count++,m_nCurGP,HQ10_UNIT);
			CreateUnit(count++,m_nCurGP,HYCJ_UNIT);
			break;
		}
		if(g_nVStatus==V_F10)
			CreateUnit(count++,m_nCurGP,F10_UNIT);
		else
			CreateUnit(count++,m_nCurGP,TV_UNIT);
		CreateUnit(count++,m_nCurGP,SEAT_UNIT);
		CreateUnit(count++,m_nCurGP,MORE_UNIT);
	}
	m_nDComUnit = count;
	if(m_nMainPad==ZST_PAD)
	{
		if(!g_bTickTStatus) 
			CreateUnit(count++,m_nDComCurGP,ZST_UNIT,m_nTranZstStyle);
		else
			CreateUnit(count++,m_nDComCurGP,TICKZST_UNIT,m_nTranZstStyle);
	}
	else if(m_nMainPad==FXT_PAD)
		CreateUnit(count++,m_nDComCurGP,FXT_UNIT,FXT_BIG);
	m_Units.Num = count;
	//先将m_bNowShow赋值,再ResizePad(可能会用到这种状态)
	m_bNowShow = FALSE;
	if(m_Units.pUnit[1] && (m_nMainPad==ZST_PAD || m_nMainPad==FXT_PAD))
		m_bNowShow = (m_Units.nStatus[1]==U_SHOW);
	if(bFirst)
		ResizePad(COM_OnlyRead);	//第一次只读数据
	else
		ResizePad(COM_ReadDraw);
	SetComTitle();
}

void CComView::ConstructPad(DWORD tmpPad,short iNo)
{
	m_nNowPad = tmpPad;
	m_nMainPad   = m_nNowPad & 0xFF0000;
	m_nAssisPad1 = m_nNowPad & 0x00FF00;
	m_nAssisPad2 = m_nNowPad & 0x0000FF;
	m_nCurSwitch1 = GetCurSwitch(m_nAssisPad1);
	m_nCurSwitch2 = GetCurSwitch(m_nAssisPad2);   
	if(iNo!=-1)	m_nCurGP = iNo;
	ConstructNewPad();
}

void CComView::ConstructNewPad()
{
	if(!m_bLayoutFlag) 
		return;
	SetMainID();
	ConstructPad();
	ShowHideDynBtn();
}

//改变尺寸,画图或读数据 flag
//		=COM_OnlyDraw,画图,
//		=COM_ReadDraw,读数据并画图,
//		=COM_OnlyRead,仅读数据,
//		=COM_OnlySize,不读不画
void CComView::SetUnitReadAndPaint(int i,CRect rc,int flag)
{
	if(!m_Units.pUnit[i]) return;
	m_Units.Rect[i] = rc;
 	long wParam = MAKELONG(rc.left,rc.top);
 	long lParam = MAKELONG(rc.Width(),rc.Height());
	m_Units.pUnit[i]->ProcessMsg(WM_SIZE,wParam,lParam);
	if(flag==COM_OnlyRead)
		m_Units.pUnit[i]->ProcessMsg(UM_READDATA);
	if(flag==COM_ReadDraw || flag==COM_OnlyDraw)
		m_Units.pUnit[i]->ProcessMsg(WM_PAINT,flag);
}

void CComView::GetMainArea(int nFixWid,CRect &rc,CRect &DComRc,CRect &SeatRc,BOOL bShowBSUnit,CRect &MoreRc,BOOL bShowMoreUnit)
{
	SeatRc.SetRect(0,0,0,0);
	MoreRc.SetRect(0,0,0,0);
	if(!g_GH.bLeftHQInfo)
	{
		rc.left=0;
		rc.right=max(0,m_Rect.Width()-nFixWid);
	}
	else
	{
		rc.left=nFixWid;
		rc.right=m_Rect.Width();
	}
	CRect tmpRc=rc;
	rc.top=0;
	int totalheight=m_Rect.Height();	
	if((m_nMainPad==ZST_PAD || m_nMainPad==FXT_PAD) && m_bShowSwitch3)
		totalheight-=ZST_SWITCH3_HEIGHT;
	if(m_nDComCurGP<0) 
	{
		int BSUnit_Height=100;
		if(bShowBSUnit)
		{
			if(m_Rect.Width()<1000)
				BSUnit_Height=136;
			totalheight-=BSUnit_Height;
		}
		int MoreUnit_Height=max(0.382f*totalheight, ZST_MOREUNIT_HEIGHT);
		if(bShowMoreUnit)
		{
			totalheight-=MoreUnit_Height;
			totalheight=max(0,totalheight);
		}
		int nTmpTop=totalheight;
		if(bShowBSUnit)
		{
			SeatRc=rc;
			SeatRc.top=nTmpTop;
			SeatRc.bottom=nTmpTop+BSUnit_Height;
			nTmpTop+=BSUnit_Height;
		}
		if(bShowMoreUnit)
		{
			MoreRc=tmpRc;
			MoreRc.top=nTmpTop;
			MoreRc.bottom=nTmpTop+MoreUnit_Height;
			nTmpTop+=MoreUnit_Height;
		}
		rc.bottom=totalheight;
	}
	else 
		rc.bottom=totalheight/2;

	DComRc=rc;
	if(m_nDComCurGP<0) 
		DComRc.SetRect(0,0,0,0);
	else
	{
		DComRc.top=totalheight/2;
		DComRc.bottom=totalheight;
	}
}

void CComView::GetMainAreaRect(CRect &rc,CRect &DComRc,CRect &SeatRc,CRect &MoreRc)
{
	BOOL	bShowBSUnit = FALSE, bShowMore = FALSE;
	if(m_nMainPad==ZST_PAD && IsSeatProperty((*g_pStockIO)[m_nCurGP]))
		bShowBSUnit = TRUE;
	if(m_nMainPad==ZST_PAD && m_bZstMore_Show)
		bShowMore = TRUE;
	else if(m_nMainPad==FXT_PAD && m_bFxtMore_Show)
		bShowMore = TRUE;
	if(m_Units.nStatus[1] == U_SHOW)
	{
		if(m_bShowAssisPad2)
			GetMainArea(g_nFixedWidth+g_nFixedWidth2,rc,DComRc,SeatRc,bShowBSUnit,MoreRc,bShowMore);
		else
			GetMainArea(g_nFixedWidth,rc,DComRc,SeatRc,bShowBSUnit,MoreRc,bShowMore);
	}
	else
		GetMainArea(0,rc,DComRc,SeatRc,bShowBSUnit,MoreRc,bShowMore);
}

void CComView::GetUnit1Area(BOOL bProperty,CRect &rc)
{
	if(!g_GH.bLeftHQInfo)
	{
		rc.left=max(0,m_Rect.Width()-g_nFixedWidth);
		rc.right=m_Rect.Width();
	}
	else
	{
		rc.left=0;
		rc.right=g_nFixedWidth;
	}
	rc.top=0;
	if(IsSpecAssisPad1(m_nAssisPad1))
	{
		rc.bottom=0;
		rc.left=rc.right=0;
	}
 	else
 	{
		if(bProperty)		
			rc.bottom=min(m_Rect.Height(),FIXED_LOWHEIGHT);
		else				
			rc.bottom=min(m_Rect.Height(),FIXED_HEIGHT);
	}
}

void CComView::GetCornerUnitArea(BOOL bProperty,CRect &rc)
{
	if(!g_GH.bLeftHQInfo)
	{
		rc.left=max(0,m_Rect.Width()-g_nFixedWidth);
		rc.right=m_Rect.Width();
	}
	else
	{
		rc.left=0;
		rc.right=g_nFixedWidth;
	}
	if(IsSpecAssisPad1(m_nAssisPad1))
	{
		rc.top=0;
		rc.bottom=max(0,m_Rect.Height()-SWITCH_HEIGHT2);
	}
	else
	{
		int nFixHeight=FIXED_HEIGHT;
		if(bProperty)
			nFixHeight=FIXED_LOWHEIGHT;
		if(m_Rect.Height() > nFixHeight)
		{
			rc.top=nFixHeight;
			rc.bottom=max(nFixHeight,m_Rect.Height()-SWITCH_HEIGHT2);
		}
		else
			rc.top=rc.bottom=m_Rect.Height();
	}
}

void CComView::GetAssis2UnitArea(CRect &rc,CRect &rc2)
{
	if(!g_GH.bLeftHQInfo)
	{
		rc.left=max(0,m_Rect.Width()-g_nFixedWidth-g_nFixedWidth2);
		rc.right=m_Rect.Width()-g_nFixedWidth;
	}
	else
	{
		rc.left=g_nFixedWidth;
		rc.right=g_nFixedWidth+g_nFixedWidth2;
	}
	rc.top=0;
	rc.bottom=min(FIXED_HEIGHT,m_Rect.Height());

	rc2=rc;
	if(m_Rect.Height() > FIXED_HEIGHT)
	{
		rc2.top=FIXED_HEIGHT;
		rc2.bottom=max(FIXED_HEIGHT,m_Rect.Height()-SWITCH_HEIGHT2);
	}
	else
		rc2.top=rc2.bottom=m_Rect.Height();

	if(g_GH.bHq10Bottom)
	{
		rc.top=max(0,m_Rect.Height()-FIXED_HEIGHT-SWITCH_HEIGHT2-2);
		rc.bottom=max(0,m_Rect.Height()-SWITCH_HEIGHT2);

		rc2.top=0;
		rc2.bottom=max(0,m_Rect.Height()-FIXED_HEIGHT-SWITCH_HEIGHT2-2);
	}
}

void CComView::GetVArea(int nFixWid,CRect &rc)
{
	if(!g_GH.bLeftHQInfo)
	{
		rc.left=0;
		rc.right=max(0,m_Rect.Width()-nFixWid);
	}
	else
	{
		rc.left=nFixWid;
		rc.right=m_Rect.Width();
	}
	rc.top=0;
	int nZstHeight=m_Rect.Height();
	if(m_bShowSwitch3)
	{
		nZstHeight-=ZST_SWITCH3_HEIGHT;
		BOOL bShowMore=FALSE;
		if(m_nMainPad==ZST_PAD && m_bZstMore_Show)
			bShowMore = TRUE;
		else if(m_nMainPad==FXT_PAD && m_bFxtMore_Show)
			bShowMore = TRUE;
		int MoreUnit_Height=max(0.382f*nZstHeight, ZST_MOREUNIT_HEIGHT);
		if(bShowMore)
		{
			nZstHeight-=MoreUnit_Height;
			nZstHeight=max(nZstHeight,0);
		}
	}
	rc.bottom=nZstHeight;
}

void GetMulQuat(int & wquat,int & hquat)
{
	wquat=2,hquat=2;
	switch(g_nMulNum)
	{
	case 9:		wquat = 3;	hquat = 3;	break;
	case 16:	wquat = 4;	hquat = 4;	break;
	case 25:	wquat = 5;	hquat = 5;	break;
	case 6:		wquat = 3;	hquat = 2;	break;
	case 12:	wquat = 4;	hquat = 3;	break;
	case 20:	wquat = 5;	hquat = 4;	break;
	}
}

void CComView::ResizePad(int flag,int nNewGP)
{
	BOOL bProperty=FALSE;
	BOOL bHasSeat=FALSE;
	if(nNewGP>=0)
	{
		bProperty=IsLevel2Property((*g_pStockIO)[nNewGP]);
		bHasSeat=IsSeatProperty((*g_pStockIO)[nNewGP]);
	}
	else
	{
		bProperty=IsLevel2Property((*g_pStockIO)[m_nCurGP]);
		bHasSeat=IsSeatProperty((*g_pStockIO)[m_nCurGP]);
	}
	CRect	rc,rc2,DComRc,SeatRc,MoreRc;
	int		UnitNo;
	BOOL	bShowBSUnit = FALSE, bShowMore = FALSE;
	if(m_nMainPad==ZST_PAD && bHasSeat)
		bShowBSUnit = TRUE;
	if(m_nMainPad==ZST_PAD && m_bZstMore_Show)
		bShowMore = TRUE;
	else if(m_nMainPad==FXT_PAD && m_bFxtMore_Show)
		bShowMore = TRUE;
	switch(m_nMainPad)
	{
	case TICK_PAD:
	case FJB_PAD:
	case TICK2_PAD:
	case MMP_PAD:
		rc.SetRect(0,0,m_Rect.Width(),m_Rect.Height());
		SetUnitReadAndPaint(0,rc,flag);
		break;
	case MULZST_PAD:
	case MULFXT_PAD:
		{
			if(m_nMainPad==MULFXT_PAD && m_bMultiPeriod)
			{
				int wid = m_Rect.Width()/3;
				int hei = m_Rect.Height()/2;
				int i = 0;
				int tmpwid,tmphei;
				for(i=0;i<6;i++)
				{
					tmpwid = wid; 
					tmphei = hei;
					if(i%3==2) tmpwid = m_Rect.Width()-2*wid;
					if(i/3==1) tmphei = m_Rect.Height()-hei;
					rc = CRect((i%3)*wid,(i/3)*hei,(i%3)*wid+tmpwid,(i/3)*hei+tmphei);
					SetUnitReadAndPaint(i,rc,flag);
				}
			}
			else
			{
				int i,j,tmpwid,tmphei;
				int wquat=2,hquat=2;
				GetMulQuat(wquat,hquat);
				int wid = m_Rect.Width()/wquat,hei = m_Rect.Height()/hquat;

				for(i=0;i<hquat;i++)
				for(j=0;j<wquat;j++)
				{
					rc.left=j*wid;
					rc.top=i*hei;
					tmpwid = wid; 
					tmphei = hei;
					if(j == wquat-1)	tmpwid = m_Rect.Width()-(wquat-1)*wid;
					if(i == hquat-1)	tmphei = m_Rect.Height()-(hquat-1)*hei;
					rc.right=j*wid+tmpwid;
					rc.bottom=i*hei+tmphei;
					SetUnitReadAndPaint(i*wquat+j,rc,flag);
				}
			}
		}
		break;
	case ZST_PAD:
	case FXT_PAD:
		if(m_Units.nStatus[1] == U_SHOW)
		{
			//先请求行情信息
			GetUnit1Area(m_bShowAssisPad2,rc);
			SetUnitReadAndPaint(1,rc,flag);
			//再请求主体区
			if(m_bShowAssisPad2)
				GetMainArea(g_nFixedWidth+g_nFixedWidth2,rc,DComRc,SeatRc,bShowBSUnit,MoreRc,bShowMore);
			else
				GetMainArea(g_nFixedWidth,rc,DComRc,SeatRc,bShowBSUnit,MoreRc,bShowMore);
			if(g_nVStatus>V_NORMAL)
				rc.SetRectEmpty();
			SetUnitReadAndPaint(0,rc,flag);

			UnitNo=2;
			//再请求右下角的数据
			GetCornerUnitArea(m_bShowAssisPad2,rc);
			SetUnitReadAndPaint(UnitNo++,rc,flag);
			//再请求Assis2的数据
			if(m_bShowAssisPad2)
			{
				GetAssis2UnitArea(rc,rc2);
				SetUnitReadAndPaint(UnitNo++,rc,flag);
				SetUnitReadAndPaint(UnitNo++,rc2,flag);
			}
			else
			{
				rc.SetRectEmpty();
				SetUnitReadAndPaint(UnitNo++,rc,flag);
				SetUnitReadAndPaint(UnitNo++,rc,flag);
			}
			//请求TV的数据
			if(g_nVStatus>V_NORMAL)
			{
				if(m_bShowAssisPad2)
					GetVArea(g_nFixedWidth+g_nFixedWidth2,rc);
				else
					GetVArea(g_nFixedWidth,rc);
			}
			else
				rc.SetRectEmpty();
			SetUnitReadAndPaint(UnitNo++,rc,flag);
			//分时图下面的UNIT
			SetUnitReadAndPaint(UnitNo++,SeatRc,flag);
			//再请求更多数据
			SetUnitReadAndPaint(UnitNo++,MoreRc,flag);
			//请求DCOM的数据
			SetUnitReadAndPaint(m_nDComUnit,DComRc,DComRc.Height()<1?COM_OnlySize:flag);
		}
		else	//如果右边被隐藏
		{
			rc.SetRectEmpty();
			SetUnitReadAndPaint(1,rc,flag);
			
			GetMainArea(0,rc,DComRc,SeatRc,bShowBSUnit,MoreRc,bShowMore);
			if(g_nVStatus>V_NORMAL)
				rc.SetRectEmpty();
			SetUnitReadAndPaint(0,rc,flag);

			UnitNo=2;
			rc.SetRectEmpty();
			SetUnitReadAndPaint(UnitNo++,rc,flag);
			SetUnitReadAndPaint(UnitNo++,rc,flag);
			SetUnitReadAndPaint(UnitNo++,rc,flag);
			if(g_nVStatus>V_NORMAL)
				GetVArea(0,rc);
			SetUnitReadAndPaint(UnitNo++,rc,flag);
			//分时图下面的UNIT
			SetUnitReadAndPaint(UnitNo++,SeatRc,flag);
			//再请求更多数据
			SetUnitReadAndPaint(UnitNo++,MoreRc,flag);
			//请求DCOM的数据
			SetUnitReadAndPaint(m_nDComUnit,DComRc,DComRc.Height()<1?COM_OnlySize:flag);
		}
	}
	SetSwitchPos();
}

void CComView::ShowHideDynBtn()
{
	int ntmpShowType;
	if(m_nMainPad == ZST_PAD)
		ntmpShowType = HideShowBtn_Zst;
	else if(m_nMainPad == FXT_PAD)
		ntmpShowType = HideShowBtn_Fxt;
	else if(m_nMainPad == MULZST_PAD || (m_nMainPad == MULFXT_PAD && !m_bMultiPeriod))
		ntmpShowType = HideShowBtn_Mul;
	else 
		ntmpShowType = HideShowBtn_Other;
	AfxGetMainWnd()->SendMessage(UM_HIDESHOWBUTTON,ntmpShowType);
}

void CComView::OnDraw(CDC* pDC)
{
	if(g_pToolTipMark) 
		g_pToolTipMark->ClearToolTipInfo(this);
	int i;
	FOR_UNIT(i)
		m_Units.pUnit[i]->ProcessMsg(WM_PAINT);
}

void CComView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	theViewManager.OnActivateView(bActivate, this);
	g_bSepEsc = FALSE;
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if(!bActivate)
	{
		m_bMult_In = FALSE;
		m_bNoChange_MultIn = FALSE;
		::ClipCursor(NULL);
		::ReleaseCapture();
		PostMessage(UM_DELETE_INFODLG,1);
	}
	if(!m_bQuitting && bActivate)
	{
		ShowHideDynBtn();
	}
}

int CComView::GetZstWidth()
{
	int nZstWidth=m_Rect.Width()-g_nFixedWidth;
	if(m_bShowAssisPad2)
		nZstWidth-=g_nFixedWidth2;
	if(!m_bNowShow)
		nZstWidth=m_Rect.Width();
	return nZstWidth;
}

int CComView::GetZstHeight()
{
	int nZstHeight=m_Rect.Height();
	if(m_bShowSwitch3)
	{
		nZstHeight-=ZST_SWITCH3_HEIGHT;
		BOOL bShowMore=FALSE;
		if(m_nMainPad==ZST_PAD && m_bZstMore_Show)
			bShowMore = TRUE;
		else if(m_nMainPad==FXT_PAD && m_bFxtMore_Show)
			bShowMore = TRUE;
		int MoreUnit_Height=max(0.382f*nZstHeight, ZST_MOREUNIT_HEIGHT);;
		if(bShowMore)
		{
			nZstHeight-=MoreUnit_Height;
			nZstHeight=max(0,nZstHeight);
		}
	}
	return nZstHeight;
}

void CComView::SetSwitchPos()
{
	if(!m_pSwitchBar1 || !m_pSwitchBar2 || !m_pSwitchBar3) return;
	CRect rc;
	GetClientRect(&rc);
	//先处理m_pSwitchBar3
	if((m_nMainPad!=ZST_PAD && m_nMainPad!=FXT_PAD) || !m_bShowSwitch3)
	{
		m_pSwitchBar3->SetWindowPos(NULL,0,0,0,0,SWP_SHOWWINDOW);
	}
	else
	{
		int nZstWidth = GetZstWidth();
		int nTopShift=0;
		if(m_nMainPad==ZST_PAD && !m_bZstMore_Show)
			nTopShift=1;
		else if(m_nMainPad==FXT_PAD && !m_bFxtMore_Show)
			nTopShift=1;
		int nRightLeave = 0;
		if(!g_GH.bLeftHQInfo)
			m_pSwitchBar3->SetWindowPos(NULL,0,m_Rect.Height()-ZST_SWITCH3_HEIGHT-nTopShift,nZstWidth+1-nRightLeave,ZST_SWITCH3_HEIGHT+nTopShift,SWP_SHOWWINDOW);
		else
			m_pSwitchBar3->SetWindowPos(NULL,m_Rect.Width()-nZstWidth,m_Rect.Height()-ZST_SWITCH3_HEIGHT-nTopShift,nZstWidth-nRightLeave,ZST_SWITCH3_HEIGHT+nTopShift,SWP_SHOWWINDOW);
		if((m_nMainPad==ZST_PAD && m_bZstMore_Show) || (m_nMainPad==FXT_PAD && m_bFxtMore_Show))
			m_pSwitchBar3->SetCurLabel(m_nMore_Sel+1);
		else
			m_pSwitchBar3->SetCurLabel(-999);
		m_pSwitchBar3->SetFontColorSchema(VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.AXISCOLOR,VipColor.TXTCOLOR,VipColor.KNCOLOR,VipColor.LEVELCOLOR,RGB(128,0,0),12,SONG_FONT);
		//
	}
	//再处理m_pSwitchBar1和m_pSwitchBar2
	if((m_nMainPad!=ZST_PAD&&m_nMainPad!=FXT_PAD) || m_Units.nStatus[1]==U_HIDE || rc.Height()<FIXED_LOWHEIGHT) 
	{
		m_pSwitchBar1->SetWindowPos(NULL,0,0,0,0,SWP_SHOWWINDOW);
		m_pSwitchBar2->SetWindowPos(NULL,0,0,0,0,SWP_SHOWWINDOW);
		return;
	}
	if(!g_GH.bLeftHQInfo)
		m_pSwitchBar1->SetWindowPos(NULL,m_Rect.Width()-g_nFixedWidth,m_Rect.Height()-SWITCH_HEIGHT2,g_nFixedWidth,SWITCH_HEIGHT2,SWP_SHOWWINDOW);
	else
		m_pSwitchBar1->SetWindowPos(NULL,0,m_Rect.Height()-SWITCH_HEIGHT2,g_nFixedWidth,SWITCH_HEIGHT2,SWP_SHOWWINDOW);
	if(m_bShowAssisPad2)
	{
		if(!g_GH.bLeftHQInfo)
			m_pSwitchBar2->SetWindowPos(NULL,m_Rect.Width()-g_nFixedWidth-g_nFixedWidth2,m_Rect.Height()-SWITCH_HEIGHT2,g_nFixedWidth2,SWITCH_HEIGHT2,SWP_SHOWWINDOW);
		else
			m_pSwitchBar2->SetWindowPos(NULL,g_nFixedWidth,m_Rect.Height()-SWITCH_HEIGHT2,g_nFixedWidth2,SWITCH_HEIGHT2,SWP_SHOWWINDOW);
	}
	else
		m_pSwitchBar2->SetWindowPos(NULL,0,0,0,0,SWP_SHOWWINDOW);
	if(m_nMainPad==ZST_PAD)
		m_pSwitchBar1->ModifySwitchLabel(7,_F("主"),_F("主力监控精灵"));
	else if(m_nMainPad==FXT_PAD)
		m_pSwitchBar1->ModifySwitchLabel(7,_F("筹"),_F("移动筹码分布"));
	m_pSwitchBar1->SetCurLabel(m_nCurSwitch1);
	m_pSwitchBar2->SetCurLabel(m_nCurSwitch2);
	m_pSwitchBar1->SetFontColorSchema(VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.AXISCOLOR,VipColor.TXTCOLOR,VipColor.KNCOLOR,VipColor.LEVELCOLOR,RGB(128,0,0),12,SONG_FONT);
	m_pSwitchBar2->SetFontColorSchema(VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.BACKCOLOR,VipColor.AXISCOLOR,VipColor.TXTCOLOR,VipColor.KNCOLOR,VipColor.LEVELCOLOR,RGB(128,0,0),12,SONG_FONT);
}

void CComView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	m_Rect.SetRect(0,0,cx,cy);
	if(cx>0&&cy>0) 
		ResizePad(COM_OnlySize);
	SetSwitchPos();
}

//设置翻页集合
void CComView::SetPageGP(short *lpnIndex,short nTotalGP,short nCurGP,LinkedSetInfo *pLSetInfo)
{
	if(nTotalGP == -1)
	{	
		m_nPageGPNum = -1;
		return;
	}
	
	int nStartRow = 0;
	if(pLSetInfo) 
	{
		memcpy(&m_LSetInfo, pLSetInfo, sizeof(LinkedSetInfo));
		m_LSetInfo.pMsgRtnWnd = this;
		nStartRow = pLSetInfo->nStartRow;
		memcpy(lpnIndex, pLSetInfo->lpnUsedStkIndex, pLSetInfo->nTolRow*sizeof(short));
	}
	else memset(&m_LSetInfo, 0, sizeof(LinkedSetInfo));

	for(int i = nStartRow;i < nTotalGP+nStartRow;i++)
	{	
		if(lpnIndex[i%nTotalGP] == nCurGP)	break;
	}
	memcpy(m_nPageGPIndex,lpnIndex,nTotalGP*sizeof(short));
	m_nPageGPNum = nTotalGP;
	m_nCurPageGP = i;
}

//是否在翻页集合中
BOOL CComView::IsInPageGP(short setcode,const char *code)
{
	if(m_nPageGPNum==-1) return TRUE;
	short index=g_pStockIO->GetIndex(code,setcode);
	if(index>=0)
	{
		for(int i=0;i<m_nPageGPNum;i++)
			if(m_nPageGPIndex[i]==index) break;
		if(i<m_nPageGPNum) 
		{
			m_nCurPageGP=i;
			return TRUE;
		}
	}
	return FALSE;
}

//得到某一个翻页股票
int CComView::GetNextPageGP(short *lpnIndex,short num,BOOL bNext,int &tmpNo)
{
	if(num < 0 || num > MULHQ_WANTNUM) return 0;
	int i,n=0,tmpIndex;
	memset(lpnIndex,-1,num*sizeof(short));
	if(m_nPageGPNum != -1)	//如果有股票集合
	{
		if(!bNext)
		{	
			int nPreFirst = m_nCurPageGP-num;
			if(m_LSetInfo.bUsed&&nPreFirst<m_LSetInfo.nStartRow)
			{
				m_LSetInfo.nStartRow = (m_nCurPageGP-m_LSetInfo.nValidRow+m_nPageGPNum)%m_nPageGPNum;
				if(m_LSetInfo.nStartRow+m_LSetInfo.nValidRow>m_nPageGPNum)
					m_LSetInfo.nStartRow = 0;
				memcpy(m_LSetInfo.lpnUsedStkIndex, m_nPageGPIndex, MAXGPNUM*sizeof(short));
				if(IsRemoteGet(&m_LSetInfo))
					AfxGetMainWnd()->SendMessage(UM_WIDEDATA_SORT, (WPARAM)&m_LSetInfo, 0);
				
				m_nPageGPNum = m_LSetInfo.nTolRow;
				if(m_nCurPageGP-num<0)	m_nCurPageGP = max(0,m_nPageGPNum-max(1,m_nPageGPNum%num));
				else	m_nCurPageGP = m_nCurPageGP-num;
				memcpy(m_nPageGPIndex, m_LSetInfo.lpnUsedStkIndex, MAXGPNUM*sizeof(short));
			}
			else if(m_nCurPageGP-num<0)				
				m_nCurPageGP = max(0,m_nPageGPNum-max(1,m_nPageGPNum%num));
			else								
				m_nCurPageGP = m_nCurPageGP-num;
		}
		else
		{
			int nNextLast = m_nCurPageGP+2*num-1;
			if(m_LSetInfo.bUsed&&nNextLast>=min(m_LSetInfo.nStartRow+m_LSetInfo.nVisualRow, m_LSetInfo.nTolRow))
			{
				if(m_nCurPageGP+num>=m_nPageGPNum) m_LSetInfo.nStartRow = 0;
				else m_LSetInfo.nStartRow = m_nCurPageGP+num;
				memcpy(m_LSetInfo.lpnUsedStkIndex, m_nPageGPIndex, MAXGPNUM*sizeof(short));
				if(IsRemoteGet(&m_LSetInfo))
					AfxGetMainWnd()->SendMessage(UM_WIDEDATA_SORT, (WPARAM)&m_LSetInfo, 0);
				
				m_nPageGPNum = m_LSetInfo.nTolRow;
				m_nCurPageGP = m_LSetInfo.nStartRow;
				memcpy(m_nPageGPIndex, m_LSetInfo.lpnUsedStkIndex, MAXGPNUM*sizeof(short));
			}
			else if(m_nCurPageGP+num>=m_nPageGPNum)	
				m_nCurPageGP = 0;
			else								
				m_nCurPageGP = m_nCurPageGP+num;
		}
		for(i=m_nCurPageGP,n=0;i<m_nPageGPNum && n<num;i++,n++)
		{
			tmpIndex = m_nPageGPIndex[i];
			lpnIndex[n] = tmpIndex;
		}
		tmpNo = lpnIndex[0];
	}
	else					//如果是所有股票集合
	{
		int allgp = 0; 
		int nBasegp = 0;
		if(m_nCurGP<DS_FROM) 
			allgp = g_pStockIO->GetStockNum(ALL_ALL);
		else if(g_bHasDS) 
		{
			nBasegp = DS_FROM;
			allgp = g_pStockIO->m_nDSNum;
		}
		if(!bNext)
		{
			if((m_nCurGP-nBasegp)-num < 0)		tmpNo = nBasegp + max(0,allgp-max(1,allgp%num));
			else								tmpNo = m_nCurGP-num;
		}
		else
		{
			if((m_nCurGP-nBasegp)+num >= allgp)	tmpNo = nBasegp;
			else								tmpNo = m_nCurGP+num;
		}
		for(i=m_nCurGP-nBasegp,n=0;i<allgp && n<num;i++,n++)
			lpnIndex[n] = nBasegp + i;
	}
	m_nCurMulGp = 0;
	SetMainID();
	return n;
}

//得到多股的股票集
int CComView::GetOne2MulGp(short *lpnIndex,short num,BOOL ToMul)
{
	if(num < 0 || num > MULHQ_WANTNUM) return 0;
	if(!ToMul && num != 1) return 0;
	int i,n=0,tmpIndex,which=0;
	memset(lpnIndex,-1,num*sizeof(short));
	if(m_nPageGPNum != -1)	//如果有股票集合
	{
		for(int i=0;i < m_nPageGPNum;i++)
		{
			if(m_nPageGPIndex[i] == m_nCurGP)
			{
				m_nCurPageGP = i;
				break;
			}
		}
		which = m_nCurPageGP%num;
		m_nCurPageGP = m_nCurPageGP-m_nCurPageGP%num;
		for(i=m_nCurPageGP,n=0;i<m_nPageGPNum && n<num;i++,n++)
		{
			tmpIndex = m_nPageGPIndex[i];
			lpnIndex[n] = tmpIndex;
		}
		m_nCurGP = lpnIndex[0];
	}
	else					//如果是所有股票集合
	{
		int allgp = 0;
		int nBasegp=0;
		if(m_nCurGP<DS_FROM) 
			allgp = g_pStockIO->GetStockNum(ALL_ALL);
		else if(g_bHasDS) 
		{
			nBasegp = DS_FROM;
			allgp = g_pStockIO->m_nDSNum;
		}
		which = m_nCurGP%num;
		m_nCurGP = m_nCurGP-m_nCurGP%num;
		for(i=m_nCurGP,n=0;i<allgp+nBasegp && n<num;i++,n++)
			lpnIndex[n] = i;
	}
	m_nCurMulGp = which;
	return n;
}

//切换版面类型,所有的版面变换必须经过此函数
void CComView::SwitchPad(DWORD tmpNowPad,BOOL bLowHQPart,BOOL bProperty)
{	
	if(tmpNowPad == m_nNowPad && !bLowHQPart) 
		return;
	if(bLowHQPart)
	{
		if(m_nMainPad==ZST_PAD)
			m_bShowAssisPad2 = (bProperty && g_GH.bZSTLevel2Face);
		else
			m_bShowAssisPad2 = (bProperty && g_GH.bFXTLevel2Face);
	}
	//重新得到MainPad和AssisPad
	m_nLastPad = m_nNowPad;
	if(m_Units.pUnit[1] && (m_nMainPad==ZST_PAD || m_nMainPad==FXT_PAD))
		m_bLastShow = (m_Units.nStatus[1]==U_SHOW);
	m_bNowShow	= m_bLastShow;
	m_nNowPad	= tmpNowPad;

	m_nMainPad   = m_nNowPad & 0xFF0000;
	m_nAssisPad1 = m_nNowPad & 0x00FF00;
	m_nAssisPad2 = m_nNowPad & 0x0000FF;
	m_nCurSwitch1 = GetCurSwitch(m_nAssisPad1);
	m_nCurSwitch2 = GetCurSwitch(m_nAssisPad2);
	
	DWORD uLastMain = m_nLastPad & 0xFF0000;
	DWORD uLastAssis1 = m_nLastPad & 0x00FF00;
	DWORD uLastAssis2 = m_nLastPad & 0x0000FF;
	if(m_nMainPad != uLastMain && m_nAssisPad1==uLastAssis1 &&
		 ((m_nMainPad==ZST_PAD&&uLastMain==FXT_PAD)||(m_nMainPad==FXT_PAD&&uLastMain==ZST_PAD)))
	{
		if(g_nVStatus>V_NORMAL)
			return;
		//分析图与走势图的置换
		if(m_nMainPad==ZST_PAD)
		{
			if(!g_bTickTStatus)
			{
				CreateUnit(0,m_nCurGP,ZST_UNIT,m_nTranZstStyle);
				if(m_nDComCurGP>=0 && m_nDComUnit>=0)
					CreateUnit(m_nDComUnit,m_nDComCurGP,ZST_UNIT,m_nTranZstStyle);
			}
			else
			{
				CreateUnit(0,m_nCurGP,TICKZST_UNIT,m_nTranZstStyle);
				if(m_nDComCurGP>=0 && m_nDComUnit>=0)
					CreateUnit(m_nDComUnit,m_nDComCurGP,TICKZST_UNIT,m_nTranZstStyle);
			}
		}
		else
		{
			CreateUnit(0,m_nCurGP,FXT_UNIT,FXT_BIG);
			if(m_nDComCurGP>=0 && m_nDComUnit>=0)
				CreateUnit(m_nDComUnit,m_nDComCurGP,FXT_UNIT,m_nTranZstStyle);
		}
		//得到几个Unit的位置
		CRect	rc,DComRc,SeatRc,MoreRc;
		GetMainAreaRect(rc,DComRc,SeatRc,MoreRc);
		//
		SetUnitReadAndPaint(0,rc,COM_ReadDraw);
		if(m_nDComUnit>1)
		{
			SetUnitReadAndPaint(m_nDComUnit-1,MoreRc,COM_ReadDraw);
			SetUnitReadAndPaint(m_nDComUnit-2,SeatRc,COM_ReadDraw);
		}
		if(m_nDComCurGP>=0 && m_nDComUnit>=0)
			SetUnitReadAndPaint(m_nDComUnit,DComRc,COM_ReadDraw);
		//
		SetSwitchPos();
		ShowHideDynBtn();
	}
	else if(m_nMainPad != uLastMain)
		ConstructNewPad();
	else if(m_nAssisPad1!=uLastAssis1 || bLowHQPart)
	{
		SetMainID();
		if(bLowHQPart)
		{
			if(!m_bShowAssisPad2)	CreateUnit(1,m_nCurGP,HQ_UNIT);
			else					CreateUnit(1,m_nCurGP,LOWHQ_UNIT);
		}
		switch(m_nAssisPad1)
		{
		case HQZST_PAD:
			CreateUnit(2,m_nCurGP,ZST_UNIT,ZST_SMALL);
			break;
		case HQTICK_PAD:
			CreateUnit(2,m_nCurGP,TICK_UNIT,TICK_SMALL);
			break;
		case HQFJB_PAD:
			CreateUnit(2,m_nCurGP,FJB_UNIT,FJB_SMALL);
			break;
		case HQTICK2_PAD:
			CreateUnit(2,m_nCurGP,TICK2_UNIT,TICK_SMALL);
			break;
		case HQMMP_PAD:
			CreateUnit(2,m_nCurGP,MMP_UNIT,MMP_SMALL);
			break;
		case HQCDP_PAD:
			CreateUnit(2,m_nCurGP,CDP_UNIT);
			break;
		case HQZS_PAD:
			CreateUnit(2,GetGPZS(m_nCurGP),ZST_UNIT,ZST_GPZS);
			break;
		case CB_PAD:
			CreateUnit(2,m_nCurGP,CB_UNIT);
			break;		
		case BIGVOL_PAD:
			CreateUnit(2,m_nCurGP,BIGVOL_UNIT);
			break;
		case ZJ_PAD:
			CreateUnit(2,m_nCurGP,TICKSTAT_UNIT);
			break;
		case YD_PAD:
			CreateUnit(2,m_nCurGP,BIGVOL_UNIT);
			break;
		}
		if(!bLowHQPart)
		{
			if(m_Units.pUnit[2])
				m_Units.pUnit[2]->ProcessMsg(UM_READDATA);
			ResizePad(COM_OnlyDraw);
		}
	}
	else if(m_nAssisPad2!=uLastAssis2)
	{
		SetMainID();
		switch(m_nAssisPad2)
		{
		case HQ10QUEUE_PAD:
			CreateUnit(4,m_nCurGP,QUEUE_UNIT);
			break;
		case HQ10WTFB_PAD:
			CreateUnit(4,m_nCurGP,WTFB_UNIT);
			break;
		case HQ10TICKSTAT_PAD:
			CreateUnit(4,m_nCurGP,TICKSTAT_UNIT);
			break;
		case HQ10SCANCEL_PAD:
			CreateUnit(4,m_nCurGP,SCANCEL_UNIT);
			break;
		case HQ10TCANCEL_PAD:
			CreateUnit(4,m_nCurGP,TCANCEL_UNIT);
			break;
		case HQ10WTRANK_PAD:
			CreateUnit(4,m_nCurGP,WTRANK_UNIT);
			break;
		case HQ10HYCJ_PAD:
			CreateUnit(4,m_nCurGP,HYCJ_UNIT);
			break;
		}
		if(!bLowHQPart)
		{
			ResizePad(COM_OnlyDraw);
			if(m_Units.pUnit[4])
				m_Units.pUnit[4]->ProcessMsg(UM_READDATA);
		}
	}
	SetCursor(LoadCursor(NULL,IDC_ARROW));
}

void CComView::SetCurGPAndPad(const char *Code,short setcode,DWORD Pad)
{
	int i,iNo = g_pStockIO->GetIndex(Code,setcode);
	if(iNo < 0) return;
	BOOL bZS = testzs((*g_pStockIO)[iNo]);
	BOOL bChgSwitch = FALSE;
	
	if(m_nAssisPad1==0) 
		m_nAssisPad1=HQTICK_PAD;
	if(bZS&&m_nAssisPad1!=YD_PAD) 
	{
		bChgSwitch = TRUE;
		m_nAssisPad1=YD_PAD;
	}
	if(!bZS&&m_nAssisPad1!=HQTICK_PAD) 
	{
		bChgSwitch = TRUE;
		m_nAssisPad1=HQTICK_PAD;
	}
	if(m_nAssisPad2==0) 
		m_nAssisPad2=HQ10QUEUE_PAD;
	
	if(Pad)		//如果又要换类型，又要换股票，则重新ConstructPad
	{
		DWORD tmpPad=Pad;
		BOOL bProperty=IsLevel2Property((*g_pStockIO)[iNo]);
		AdjustLevel2Pad(tmpPad,bProperty);
		ConstructPad(tmpPad,iNo);
	}
	else		//仅仅改变股票(多股要换成单股)
	{
		BOOL bProperty=IsLevel2Property((*g_pStockIO)[iNo]);		
		BOOL bHasSeat=IsSeatProperty((*g_pStockIO)[iNo]);
		if(m_nMainPad == MULZST_PAD || m_nMainPad == MULFXT_PAD)
		{	//如果是多股版面
			DWORD tmpPad = ZST_PAD+HQTICK_PAD+m_nAssisPad2;
			if(m_nMainPad == MULFXT_PAD)
				tmpPad = FXT_PAD+(g_GH.bPinAssistPad?m_nAssisPad1:HQZST_PAD)+m_nAssisPad2;
			AdjustLevel2Pad(tmpPad,bProperty);
			ConstructPad(tmpPad,iNo);
			return;
		}
		else
		{	
			if(bProperty!=IsLevel2Property((*g_pStockIO)[m_nCurGP]) || bHasSeat!=IsSeatProperty((*g_pStockIO)[m_nCurGP]))
			{
				DWORD tmpPad=m_nNowPad;
				AdjustLevel2Pad(tmpPad,bProperty);
				SwitchPad(tmpPad,TRUE,bProperty);
				ResizePad(COM_OnlySize,iNo);
			}
			if(bChgSwitch)
				SwitchPad(m_nMainPad+m_nAssisPad1+m_nAssisPad2);

			//如果是单股版面
			m_nCurGP = iNo;
			SetMainID();
			FOR_UNIT(i)
			{
				if(i==m_nDComUnit) continue;
				m_bNoChange_MultIn = FALSE;
				if(m_Units.Style[i] == ZST_GPZS)
					ChangeUnitGP(i,GetGPZS(m_nCurGP));
				else
					ChangeUnitGP(i,m_nCurGP);
			}
			ReadAndPaint();
			SetComTitle();
		}
	}
}

DWORD CComView::GetPadFromMessageCode(int MessageCode,const char *Code,short setcode)
{
	int iNo=-1;
	if(Code) iNo = g_pStockIO->GetIndex(Code,setcode);
	BOOL bZS = FALSE;
	if(iNo>=0) m_nCurGP=iNo;
	bZS = testzs((*g_pStockIO)[m_nCurGP]);

	if(m_nAssisPad1==0) 
		m_nAssisPad1=HQTICK_PAD;
	if(bZS) 
		m_nAssisPad1=YD_PAD;
	if(m_nAssisPad2==0) 
		m_nAssisPad2=HQ10QUEUE_PAD;
	DWORD tmpPad=0,tmpMainPad=0;
	if(MessageCode>=101 && MessageCode<=119)
		return ZST_PAD+(g_GH.bPinAssistPad?m_nAssisPad1:HQTICK_PAD)+m_nAssisPad2;
	if(MessageCode>=301 && MessageCode<=322)
		return FXT_PAD+(g_GH.bPinAssistPad?m_nAssisPad1:HQZST_PAD)+m_nAssisPad2;
	switch(MessageCode)
	{
	case 501: //普通分时图
	case 506: //量比(多空)指标
	case 507: //买卖力道(涨跌率)
	case 508:
		if(g_GH.bPinAssistPad&&m_nAssisPad1==CB_PAD) 
			m_nAssisPad1 = HQTICK_PAD;
		tmpPad = ZST_PAD+(g_GH.bPinAssistPad?m_nAssisPad1:HQTICK_PAD)+m_nAssisPad2; 
		break;
	case 502: tmpPad = TICK_PAD; break;
	case 503: tmpPad = FJB_PAD; break;
	case 504: tmpPad = TICK2_PAD; break;
	case 505: tmpPad = MMP_PAD; break;
	case 300: 
		m_nFxPeriod = g_GH.nDefPeriod;
		if(g_GH.bPinAssistPad&&m_nAssisPad1==BIGVOL_PAD) 
			m_nAssisPad1 = HQTICK_PAD;
		tmpPad = FXT_PAD+(g_GH.bPinAssistPad?m_nAssisPad1:HQZST_PAD)+m_nAssisPad2; break;
	case 900: //多股同列
		tmpPad = MULZST_PAD; 
		break;
	case 901: 
		if((m_nNowPad&0xFF0000)==FXT_PAD && (m_nNowPad&0x00FF00)==CB_PAD)		//如果已经是筹码分布
			tmpPad = FXT_PAD+(g_GH.bPinAssistPad?HQTICK_PAD:HQZST_PAD)+m_nAssisPad2;
		else
			tmpPad = FXT_PAD+CB_PAD+m_nAssisPad2;
		break;
	case 902: 
		if((m_nNowPad&0xFF0000)==ZST_PAD && (m_nNowPad&0x00FF00)==BIGVOL_PAD)	//如果已经是主力监控
			tmpPad = ZST_PAD+HQTICK_PAD+m_nAssisPad2;
		else
			tmpPad = ZST_PAD+BIGVOL_PAD+m_nAssisPad2;
		break;
	case 903:
		tmpMainPad = (m_nNowPad==0)?ZST_PAD:(m_nNowPad&0xFF0000);
		break;
	}
	//是否要改变AssisPad2模式
	BOOL bProperty=IsLevel2Property((*g_pStockIO)[m_nCurGP]);
	if(bProperty && !g_GH.bZSTLevel2Face==g_GH.bFXTLevel2Face)
	{
		AdjustLevel2Pad(tmpPad,bProperty);
		ConstructPad(tmpPad);
	}
	return tmpPad;
}

LRESULT CComView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//远程雷和个股资讯到来的消息
	if ( UM_MINEMSG == message || UM_GGBWMSG == message)
	{
		if(m_Units.pUnit[0])
			m_Units.pUnit[0]->ProcessMsg(message,wParam,lParam);
	}
	switch(message)
	{
	case WM_CHANGEGP_MESSAGE:
		if(m_nMainPad==TICK_PAD)
		{
			PostMessage(WM_CHANGEALL_MESSAGE,(g_bTickTStatus?508:501));
			break;
		}
		SetCurGPAndPad(ComCurGPCode,ComCurSetCode);
		break;
	case WM_CHANGEALL_MESSAGE:
		{
			int MessageCode = (UINT)wParam;
			DWORD tmpPad = GetPadFromMessageCode(MessageCode,ComCurGPCode,ComCurSetCode);
			if(tmpPad == 0) break;
			Proc_Special_MessageCode(MessageCode);
			SetCurGPAndPad(ComCurGPCode,ComCurSetCode,tmpPad);
		}
		break;
	case WM_KEYGUY_MESSAGE:
		{
			int MessageCode = (int)wParam;
			DWORD tmpPad;
			if(MessageCode == 8)								//F8切换周期
			{
				if (m_nMainPad != FXT_PAD)
				{
					if(m_nAssisPad1==0) 
						m_nAssisPad1=HQTICK_PAD;
					if(m_nAssisPad2==0) 
						m_nAssisPad2=HQ10QUEUE_PAD;
					tmpPad = FXT_PAD+(g_GH.bPinAssistPad?m_nAssisPad1:HQZST_PAD)+m_nAssisPad2;
					SwitchPad(tmpPad);
				}
				if( m_nMainPad == FXT_PAD && m_Units.pUnit[0] )
					((UFxt*)m_Units.pUnit[0])->ProcessMsg(WM_KEYDOWN,VK_F8);
				break;
			}
			if( (MessageCode==16||MessageCode==17||MessageCode==30) && (m_nMainPad==FXT_PAD||m_nMainPad==ZST_PAD) && 
				m_Units.pUnit[0]&&m_Units.pUnit[1] )
			{
				if(MessageCode==16)					//信息地雷
					m_Units.pUnit[0]->ProcessMsg(UM_POPINFODLG,0);
				if(MessageCode==17 && g_pMainWin)	//投资日记
					g_pMainWin->SendMessage(WM_COMMAND,ID_GPCOOLINFO);
				if(MessageCode==30)					//切换关联
					m_Units.pUnit[1]->ProcessMsg(UM_SWITCHLINK);
				break;
			}
			else if(MessageCode >= 801 && MessageCode <= 812)	//切换周期
			{
				if (m_nMainPad != FXT_PAD)
				{
					if(m_nAssisPad1==0) 
						m_nAssisPad1=HQTICK_PAD;
					if(m_nAssisPad2==0) 
						m_nAssisPad2=HQ10QUEUE_PAD;
					m_nFxPeriod = KeyOrderToTdxPeriod(MessageCode-801);
					tmpPad = FXT_PAD+(g_GH.bPinAssistPad?m_nAssisPad1:HQZST_PAD)+m_nAssisPad2;
					//是否要改变AssisPad2模式
					BOOL bProperty=IsLevel2Property((*g_pStockIO)[m_nCurGP]);
					if(bProperty && !g_GH.bZSTLevel2Face==g_GH.bFXTLevel2Face)
					{
						AdjustLevel2Pad(tmpPad,bProperty);
						ConstructPad(tmpPad);
					}
					else
						SwitchPad(tmpPad);
				}
				else
					OnPeriod(MessageCode-801+ID_MIN1);
				break;
			}
			else if(MessageCode >= KEYGUY_SEPCZB)						//特殊指标
			{
				if (m_nMainPad != FXT_PAD)
				{
					if(m_nAssisPad1==0) 
						m_nAssisPad1=HQTICK_PAD;
					if(m_nAssisPad2==0) 
						m_nAssisPad2=HQ10QUEUE_PAD;
					tmpPad = FXT_PAD+(g_GH.bPinAssistPad?m_nAssisPad1:HQZST_PAD)+m_nAssisPad2;
					SwitchPad(tmpPad);
				}
				if(m_Units.pUnit[0])
					m_Units.pUnit[0]->ProcessMsg(UM_CHANGESPEZB,MessageCode-KEYGUY_SEPCZB);
				break;
			}
			else if(MessageCode >= KEYGUY_MAINLINE && MessageCode < KEYGUY_BLOCK) //主图类型
			{
				if (m_nMainPad != FXT_PAD)
				{
					if(m_nAssisPad1==0) 
						m_nAssisPad1=HQTICK_PAD;
					if(m_nAssisPad2==0) 
						m_nAssisPad2=HQ10QUEUE_PAD;
					tmpPad = FXT_PAD+(g_GH.bPinAssistPad?m_nAssisPad1:HQZST_PAD)+m_nAssisPad2;
					SwitchPad(tmpPad);
				}
				if(m_Units.pUnit[0])
					m_Units.pUnit[0]->ProcessMsg(WM_PAINT);
				break;
			}
			else if(MessageCode >= KEYGUY_ZB) //指标
			{
				if (m_nMainPad != FXT_PAD)
				{
					if(m_nAssisPad1==0) 
						m_nAssisPad1=HQTICK_PAD;
					if(m_nAssisPad2==0) 
						m_nAssisPad2=HQ10QUEUE_PAD;
					tmpPad = FXT_PAD+(g_GH.bPinAssistPad?m_nAssisPad1:HQZST_PAD)+m_nAssisPad2;
					SwitchPad(tmpPad);
				}
				if(m_Units.pUnit[0])
					m_Units.pUnit[0]->ProcessMsg(UM_CHANGEZB,MessageCode-KEYGUY_ZB);
				break;
			}
			tmpPad = GetPadFromMessageCode(MessageCode);
			if(tmpPad == 0) break;
			//判断是否是多股同列
			if(m_nNowPad==MULZST_PAD && MessageCode==300)
			{
				tmpPad = MULFXT_PAD;
				m_bMultiPeriod = FALSE;
				SwitchPad(tmpPad);
			}
			else if(m_nNowPad==MULFXT_PAD && MessageCode==501 && !m_bMultiPeriod)
			{
				tmpPad = MULZST_PAD;
				SwitchPad(tmpPad);
			}
			else if(Proc_Special_MessageCode(MessageCode))
			{
				if(m_nCurGP >= 0)
				{
					ComCurGPCode = (*g_pStockIO)[m_nCurGP]->Code;
					ComCurSetCode = (*g_pStockIO)[m_nCurGP]->setcode;
				}
				SetCurGPAndPad(ComCurGPCode,ComCurSetCode,tmpPad);
			}
			else
				SwitchPad(tmpPad);
		}
		break;
	case UM_GET_FXTDATA:	//得到分析周期
		if( m_nMainPad == FXT_PAD && m_Units.pUnit[0] )
			((UFxt*)m_Units.pUnit[0])->ProcessMsg(UM_GET_FXTDATA);
		break;
	case UM_SET_CUR_DATE:	//设置当前日期
		if(m_Units.pUnit[1])
		{
			m_Units.pUnit[1]->ProcessMsg(UM_SET_DATE,wParam);
			m_Units.pUnit[1]->ProcessMsg(WM_PAINT,1);
		}
		if(m_Units.pUnit[2])
		{
			m_Units.pUnit[2]->ProcessMsg(UM_SET_DATE,wParam);
			m_Units.pUnit[2]->ProcessMsg(WM_PAINT,1);
		}
		break;
	case UM_REFLECT:		//消息反射
		switch(wParam)
		{
		case Reflect_ToZstime:		//切换到分时图上的某个时间点
			if(m_Units.pUnit[2])
				m_Units.pUnit[2]->ProcessMsg(UM_TOZSTIME,lParam);
			break;
		case Reflect_ZstSynch:		//切换到分时图上的相应时该
			if(m_Units.pUnit[1])
				m_Units.pUnit[1]->ProcessMsg(UM_ZSTSYNCH,lParam);
			if(m_Units.pUnit[2])
				m_Units.pUnit[2]->ProcessMsg(UM_ZSTSYNCH,lParam);
			break;
		case Reflect_Cb_FxtStart:	//移动筹码分布的起始时间
			if(m_Units.pUnit[2])
				m_Units.pUnit[2]->ProcessMsg(UM_CB_FXTSTART);
			break;
		case Reflect_Cb_FxtEnd:		//移动筹码分布的终止时间
			if(m_Units.pUnit[2])
				m_Units.pUnit[2]->ProcessMsg(UM_CB_FXTEND);
			break;
		case Reflect_Cb_Date:
			if(!g_bLockRight && m_Units.pUnit[2])	//移动筹码分布的时间
				m_Units.pUnit[2]->ProcessMsg(UM_CB_DATE,lParam);
			break;
		case Reflect_Tv_Date:		//TopView日期
			if(!g_bLockRight)
			{
				int i;
				FOR_UNIT(i)
					if(m_Units.pUnit[i]) m_Units.pUnit[i]->ProcessMsg(UM_TV_DATE);
			}
			break;
		case Reflect_Tv_Period:		//TopView周期
			if(!g_bLockRight)
			{
				int i;
				FOR_UNIT(i)
					if(m_Units.pUnit[i]) m_Units.pUnit[i]->ProcessMsg(UM_TV_PERIOD);
			}
			break;
		default:
			break;
		}
		break;
	case UM_CONTINUEPLAY:		//继续播报
		if(m_nMainPad==ZST_PAD)
			m_Units.pUnit[0]->ProcessMsg(UM_CONTINUEPLAY,wParam,lParam);
		break;
	case UPDATE_WARNGP:			//主力监控
		{
			int i;
			FOR_UNIT(i)
				m_Units.pUnit[i]->ProcessMsg(UPDATE_WARNGP,0,0);
		}
		break;
	case UM_LEVIN_FEEDBACK_MSG:	//盘面下单的消息
		{
			if(m_nMainPad==ZST_PAD||m_nMainPad==FXT_PAD)
				m_Units.pUnit[0]->ProcessMsg(UM_LEVIN_FEEDBACK_MSG,wParam,lParam);
			int i;
			FOR_UNIT(i)
			{
				if(m_Units.Type[i]==HQ10_UNIT||m_Units.Type[i]==HQ_UNIT||m_Units.Type[i]==GLHQ_UNIT)
					m_Units.pUnit[i]->ProcessMsg(UM_LEVIN_FEEDBACK_MSG,wParam,lParam);
			}
		}
		break;
	case UM_REPAINTCOM_MESSAGE:
		{
			int i;
			FOR_UNIT(i)
				if(m_Units.pUnit[i] && m_Units.nStatus[i]==U_SHOW) m_Units.pUnit[i]->ProcessMsg(WM_PAINT);
			SetSwitchPos();
		}
		break;
	default:
		break;
	}
	return CUnitView::WindowProc(message, wParam, lParam);
}

int CComView::EscHandle()
{
	int nRet = 0;
	if((m_nMainPad==ZST_PAD||m_nMainPad==FXT_PAD) && g_nVStatus>V_NORMAL)
	{
		g_nVStatus = V_NORMAL;
		m_nNowPad = m_nMainPad+m_nAssisPad1+m_nAssisPad2;
		ConstructNewPad();
		return 1;
	}
	if(m_nDComCurGP<0 && m_Units.pUnit[0])
	{
		nRet = m_Units.pUnit[0]->ProcessMsg(WM_KEYDOWN,VK_ESCAPE);
		if(nRet == 0)
		{
			//如果是多股进入的，直接返回多股同列版面
			if(m_bMult_In && m_nMainPad!=MULZST_PAD && m_nMainPad!=MULFXT_PAD)
			{
				ZoomToOne(-1,FALSE);
				nRet = 1;
			}
		}
	}
	else if(m_nDComUnit>=0 && 0==m_nF && m_Units.pUnit[0])
	{
		nRet = m_Units.pUnit[0]->ProcessMsg(WM_KEYDOWN,VK_ESCAPE);
		if(nRet == 0)
		{
			//如果是多股进入的，直接返回多股同列版面
			if(m_bMult_In && m_nMainPad!=MULZST_PAD && m_nMainPad!=MULFXT_PAD)
			{
				ZoomToOne(-1,FALSE);
				nRet = 1;
			}
		}
	}
	else if(m_nDComUnit>=0 && m_nF==m_nDComUnit && m_Units.pUnit[m_nDComUnit])
		nRet = m_Units.pUnit[m_nDComUnit]->ProcessMsg(WM_KEYDOWN,VK_ESCAPE);
	return nRet;
}

void CComView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(!OnLine)  //此函数远程专用
	{
		CView::OnKeyUp(nChar, nRepCnt, nFlags);
		return;
	}
	if(m_nMainPad == MULZST_PAD || m_nMainPad == MULFXT_PAD)//对于多股版面，消息另外走
	{
		switch(nChar)
		{
		case VK_NEXT:
		case VK_PRIOR:
			ReadAndPaint();
			SetComTitle();
			return;
		}
	}
	switch(nChar)
	{
	case VK_ADD:
		{
			if(g_pComm->GetSendTop() != 0) break;
			if(m_nMainPad!=ZST_PAD && m_nMainPad!=FXT_PAD)
				break;
			SHORT ShiftDown=::GetKeyState(VK_SHIFT);
			if(ShiftDown < 0)
			{
				m_nCurSwitch1--;
				if(m_nCurSwitch1<0) m_nCurSwitch1 = m_nSwitchUnitNum-1;
			}
			else
				m_nCurSwitch1=(m_nCurSwitch1+1)%m_nSwitchUnitNum;
			SwitchPad(m_nMainPad+GetAssis1FromSwitch(m_nCurSwitch1)+m_nAssisPad2);
		}
		break;
	case VK_NEXT:
	case VK_PRIOR:
		{
			int i;
			if(m_nMainPad==TICK_PAD || m_nMainPad==FJB_PAD || m_nMainPad==TICK2_PAD || m_nMainPad==MMP_PAD)
			{
				FOR_UNIT(i)
					m_Units.pUnit[i]->ProcessMsg(WM_KEYDOWN,nChar,nRepCnt);
				break;
			}
			ReadAndPaint();
			SetComTitle();
		}
		break;
	}
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CComView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	//对于多股版面，消息另外走
	if(m_nMainPad == MULFXT_PAD && m_bMultiPeriod)
	{
		int i;
		switch(nChar) //在多股中,用TAB,左右箭头在股票间跳动,上下箭头自己使用
		{
		case VK_TAB:
		case VK_LEFT:
		case VK_RIGHT:
			{
				if(nChar == VK_LEFT) 
					m_nCurPeriodOff = (m_nCurPeriodOff+5)%6;
				else 
					m_nCurPeriodOff = (m_nCurPeriodOff+1)%6;
				FOR_UNIT(i)
				{
					m_Units.pUnit[i]->ProcessMsg(UM_SETSEL,m_nCurPeriodOff==i);
					m_Units.pUnit[i]->ProcessMsg(WM_PAINT);
				}
			}
			return;
		case VK_NEXT:
		case VK_PRIOR:
			{
				int tmpNo;
				m_nMulGpNum = GetNextPageGP(m_nMulGpIndex,1,nChar==VK_NEXT,tmpNo);
				m_nCurGP=tmpNo;
				FOR_UNIT(i)
				{
					m_bNoChange_MultIn = TRUE;
					ChangeUnitGP(i,m_nCurGP);
					m_Units.pUnit[i]->ProcessMsg(UM_SETSEL,m_nCurPeriodOff==i);
					m_Units.pUnit[i]->ProcessMsg(WM_PAINT,1,1);
					m_Units.pUnit[i]->ProcessMsg(WM_KEYDOWN,nChar);
				}
				SetComTitle();
			}
			return;
		case VK_RETURN:
			m_nFxPeriod = l_nMulPeriodOrder[(m_nFirstPeriod+m_nCurPeriodOff)%8];
			ZoomToOne(m_nCurMulGp,TRUE); //版面的放大
			return;
		}
	}
	if(m_nMainPad == MULZST_PAD || m_nMainPad == MULFXT_PAD)
	{
		int i;
		switch(nChar) //在多股中,用TAB,左右箭头在股票间跳动,上下箭头自己使用
		{
		case VK_TAB:
		case VK_LEFT:
		case VK_RIGHT:
			{
				if(nChar == VK_LEFT)
				{
					if(m_nCurMulGp==0)
						m_nCurMulGp = m_nMulGpNum-1;
					else
						m_nCurMulGp = max(0,m_nCurMulGp-1);
				}
				else
				{
					if(m_nCurMulGp==m_nMulGpNum-1)
						m_nCurMulGp = 0;
					else
						m_nCurMulGp = min(m_nMulGpNum-1,m_nCurMulGp+1);
				}
				FOR_UNIT(i)
				{
					m_Units.pUnit[i]->ProcessMsg(UM_SETSEL,i==m_nCurMulGp);
					m_Units.pUnit[i]->ProcessMsg(WM_PAINT);
				}
			}
			return;
		case VK_NEXT:
		case VK_PRIOR:
			{
				int tmpNo;
				m_nMulGpNum = GetNextPageGP(m_nMulGpIndex,g_nMulNum,nChar==VK_NEXT,tmpNo);
				m_nCurGP=tmpNo;
				FOR_UNIT(i)
				{
					m_bNoChange_MultIn = TRUE;
					ChangeUnitGP(i,m_nMulGpIndex[i]);
					m_Units.pUnit[i]->ProcessMsg(UM_SETSEL,i==m_nCurMulGp);
					m_Units.pUnit[i]->ProcessMsg(WM_PAINT,1,1);//lParm为1,表示只画框架
					m_Units.pUnit[i]->ProcessMsg(WM_KEYDOWN,nChar);
				}
				SetComTitle();
			}
			return;
		case VK_RETURN:
			ZoomToOne(m_nCurMulGp,TRUE); //版面的放大
			return;
		}
	}
	switch(nChar)
	{
	case VK_LEFT:
	case VK_RIGHT:
	case VK_UP:
	case VK_DOWN:
		if(m_nMainPad == FXT_PAD) 
		{
			m_Units.pUnit[0]->ProcessMsg(WM_KEYDOWN,nChar,nRepCnt);
			if(m_nDComCurGP>=0 && m_nDComUnit>=0 && (nChar==VK_UP||nChar==VK_DOWN))
				m_Units.pUnit[m_nDComUnit]->ProcessMsg(WM_KEYDOWN,nChar,nRepCnt);
		}
		else
		{
			int i;
			FOR_UNIT(i)
			{
				if(i==m_nDComUnit) continue;
				m_Units.pUnit[i]->ProcessMsg(WM_KEYDOWN,nChar,nRepCnt);
			}
		}
		break;
	case VK_NEXT:
	case VK_PRIOR:
		{
			int i;
			if(m_nMainPad==TICK_PAD || m_nMainPad==FJB_PAD || m_nMainPad==TICK2_PAD || m_nMainPad==MMP_PAD)
				break;
			int tmpNo=0;
			m_nMulGpNum = GetNextPageGP(m_nMulGpIndex,1,nChar==VK_NEXT,tmpNo);
			BOOL bProperty=IsLevel2Property((*g_pStockIO)[tmpNo]);
			BOOL bHasSeat=IsSeatProperty((*g_pStockIO)[tmpNo]);
			if(bProperty!=IsLevel2Property((*g_pStockIO)[m_nCurGP]) || bHasSeat!=IsSeatProperty((*g_pStockIO)[m_nCurGP]))
			{
				DWORD tmpPad=m_nNowPad;
				AdjustLevel2Pad(tmpPad,bProperty);
				SwitchPad(tmpPad,TRUE,bProperty);
				ResizePad(COM_OnlySize,tmpNo);
			}
			m_nCurGP=tmpNo;
			FOR_UNIT(i)
			{
				if(i==m_nDComUnit) continue;
				m_bNoChange_MultIn = TRUE;
				if(m_Units.Style[i] == ZST_GPZS) //如果是股票指数Unit，则其股票为GetGPZS()
					ChangeUnitGP(i,GetGPZS(m_nCurGP));
				else
					ChangeUnitGP(i,m_nCurGP);
				m_Units.pUnit[i]->ProcessMsg(WM_PAINT,1,1); //lParm为1,表示只画框架
				m_Units.pUnit[i]->ProcessMsg(WM_KEYDOWN,nChar);
			}
			SetComTitle();
		}
		break;
	case VK_INSERT:
		PostMessage(WM_COMMAND,ID_DCOMLAYER);
		break;
	default:
		{
			int i;
			FOR_UNIT(i)
			{
				if(i==m_nDComUnit) continue;
				m_Units.pUnit[i]->ProcessMsg(WM_KEYDOWN,nChar,nRepCnt);
			}
		}
		break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CComView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int i;
	FOR_UNIT(i)
		m_Units.pUnit[i]->ProcessMsg(WM_CHAR,nChar,nRepCnt);
	if(m_nMainPad==ZST_PAD && (nChar=='/' || nChar=='*'))
		m_nTranZstStyle=((UZst *)m_Units.pUnit[0])->GetStyle();
	CView::OnChar(nChar, nRepCnt, nFlags);
}

int	CComView::GetCurrentGPIndex()
{
	if(m_nMainPad==MULFXT_PAD && m_bMultiPeriod)
		return m_nCurGP;
	else if(m_nMainPad==MULZST_PAD || m_nMainPad==MULFXT_PAD)
		return m_nMulGpIndex[m_nCurMulGp];
	else if(m_nDComCurGP>=0 && m_nF==m_nDComUnit)
		return m_nDComCurGP;
	return m_nCurGP;
}

short CComView::GetCurrentGPSet(short *pPageGPIndex)
{
	if(m_nPageGPNum<1)	
		return(0);
	memcpy(pPageGPIndex, m_nPageGPIndex, m_nPageGPNum*sizeof(short));
	return m_nPageGPNum;
}

void CComView::OnHideright() 
{
	if(m_Units.pUnit[1] && (m_nMainPad==ZST_PAD||m_nMainPad==FXT_PAD))
	{
		m_Units.nStatus[1] = (m_Units.nStatus[1]==U_SHOW)?U_HIDE:U_SHOW;
		ResizePad();
		m_bLastShow = (m_Units.nStatus[1]==U_SHOW);
		m_bNowShow = m_bLastShow;
		SetSwitchPos();
	}
}

void CComView::OnZSTLevel2Face()
{
	g_GH.bZSTLevel2Face=!g_GH.bZSTLevel2Face;
	BOOL bProperty=IsLevel2Property((*g_pStockIO)[m_nCurGP]);		
	if(bProperty)
	{
		AdjustLevel2Pad(m_nNowPad,bProperty);
		SwitchPad(m_nNowPad,TRUE,bProperty);
		ResizePad(COM_ReadDraw);
	}
}

void CComView::OnUpdateZSTLevel2Face(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CanLevel2());
	pCmdUI->SetCheck(g_GH.bZSTLevel2Face);
}

void CComView::OnFXTLevel2Face()
{
	g_GH.bFXTLevel2Face=!g_GH.bFXTLevel2Face;
	BOOL bProperty=IsLevel2Property((*g_pStockIO)[m_nCurGP]);		
	if(bProperty)
	{
		AdjustLevel2Pad(m_nNowPad,bProperty);
		SwitchPad(m_nNowPad,TRUE,bProperty);
		ResizePad(COM_ReadDraw);
	}
}

void CComView::OnUpdateFXTLevel2Face(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CanLevel2());
	pCmdUI->SetCheck(g_GH.bFXTLevel2Face);
}

void CComView::OnTick2Priority()
{
	g_GH.bTick2Priority=!g_GH.bTick2Priority;
	BOOL bProperty=IsLevel2Property((*g_pStockIO)[m_nCurGP]);		
	if(bProperty)
	{
		AdjustLevel2Pad(m_nNowPad,bProperty);
		SwitchPad(m_nNowPad);
	}
}

void CComView::OnUpdateTick2Priority(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(CanLevel2());
	pCmdUI->SetCheck(g_GH.bTick2Priority);
}

void CComView::RefreshCurrPADHQ()
{
	if(!OnLine)		return;
	if(g_bDowning)	return;
	if(!g_pStockIO) return;

	//减少叠加品种和关联品种的刷新次数,主股票刷新10次他才刷新一次
	static int s_ComView_OtherCodeHead = 0;

	char *CodeHead = new char[(1+SH_CODE_LEN)*MULHQ_WANTNUM];
	char *CodeHead3 = new char[sizeof(MULIDX)*MULHQ_WANTNUM];
	memset(CodeHead,0,(1+SH_CODE_LEN)*MULHQ_WANTNUM);
	memset(CodeHead3,0,sizeof(MULIDX)*MULHQ_WANTNUM);
	struct MemStkInfo *pStkInfo, *tmpInfo, *tmpInfo3,*tmpInfo3_Zs;
	int nCount = 0, nCount3 = 0,i;
	if(m_nMainPad==MULZST_PAD || (m_nMainPad==MULFXT_PAD && !m_bMultiPeriod))
	{
		for(i = 0;i < m_nMulGpNum;i++)
		{
			if(m_nMulGpIndex[i]<DS_FROM)
			{
				tmpInfo = (*g_pStockIO)[m_nMulGpIndex[i]];
				if(!tmpInfo) continue;
				if(IsLocalStock(tmpInfo)) continue;
				memcpy(CodeHead+nCount*(1+SH_CODE_LEN),&tmpInfo->setcode,1);
				memcpy(CodeHead+nCount*(1+SH_CODE_LEN)+1,tmpInfo->Code,SH_CODE_LEN);
				nCount++;
			}
			if(m_nMulGpIndex[i]>=DS_FROM)
			{
				tmpInfo3 = (*g_pStockIO)[m_nMulGpIndex[i]];
				if(tmpInfo3)
				{
					memcpy(CodeHead3+nCount3*sizeof(MULIDX),&tmpInfo3->setcode,1);
					memcpy(CodeHead3+nCount3*sizeof(MULIDX)+1,tmpInfo3->Code,CODE_SIZE);
					nCount3++;
				}
			}
		}
	}
	else
	{
		if(m_nCurGP<DS_FROM)
		{
			pStkInfo = (*g_pStockIO)[m_nCurGP];
			if(pStkInfo && !IsLocalStock(pStkInfo) && pStkInfo->setcode<2)
			{
				memcpy(CodeHead,&pStkInfo->setcode,1);
				memcpy(CodeHead+1,pStkInfo->Code,SH_CODE_LEN);
				nCount++;
				if(s_ComView_OtherCodeHead%10==0)
				{
					GetOverlapCodeHead(0,CodeHead,nCount);
					GetQZRelationCodeHead(m_nCurGP,CodeHead,nCount);
				}
			}
		}
		else if(m_nCurGP>=DS_FROM)
		{
			tmpInfo3 = (*g_pStockIO)[m_nCurGP];
			if(tmpInfo3)
			{
				memcpy(CodeHead3+nCount3*sizeof(MULIDX),&tmpInfo3->setcode,1);
				memcpy(CodeHead3+nCount3*sizeof(MULIDX)+1,tmpInfo3->Code,CODE_SIZE);
				nCount3++;
				if(tmpInfo3->bigmarket==HKGP_TYPE && m_nAssisPad1==HQZS_PAD)
				{
					tmpInfo3_Zs = g_pStockIO->Get_MemStkInfo(HK_DPCODE,HK_DPSETCODE);
					if(tmpInfo3_Zs)
					{
						memcpy(CodeHead3+nCount3*sizeof(MULIDX),&tmpInfo3_Zs->setcode,1);
						memcpy(CodeHead3+nCount3*sizeof(MULIDX)+1,tmpInfo3_Zs->Code,CODE_SIZE);
						nCount3++;
					}
				}
				if(s_ComView_OtherCodeHead%10==0)
					GetOverlapCodeHead(0,CodeHead3,nCount3,TRUE);
			}
		}
	}
	if(nCount<MULHQ_WANTNUM && m_nDComCurGP>=0 && m_nDComUnit>=0)
	{
		tmpInfo = (*g_pStockIO)[m_nDComCurGP];
		if(tmpInfo && !IsLocalStock(tmpInfo) && tmpInfo->setcode<2)
		{
			memcpy(CodeHead+nCount*(1+SH_CODE_LEN),&tmpInfo->setcode,1);
			memcpy(CodeHead+nCount*(1+SH_CODE_LEN)+1,tmpInfo->Code,SH_CODE_LEN);
			nCount++;
		}
		else if(tmpInfo&&m_nDComCurGP>=DS_FROM)
		{
			memcpy(CodeHead3+nCount3*sizeof(MULIDX),&tmpInfo->setcode,1);
			memcpy(CodeHead3+nCount3*sizeof(MULIDX)+1,tmpInfo->Code,CODE_SIZE);
			nCount3++;
		}
		if(s_ComView_OtherCodeHead%10==0)
		{
			GetOverlapCodeHead(m_nDComUnit,CodeHead,nCount);
			GetOverlapCodeHead(m_nDComUnit,CodeHead3,nCount3,TRUE);
			GetQZRelationCodeHead(m_nDComCurGP,CodeHead,nCount);
		}
	}
	if(nCount > 0 || nCount3 > 0)
	{
		SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
		g_nAssisID = JUSTCURRHQ;
		if(CanLevel2())				//Level2方式下数据包不能丢失
			g_nPriority = 1;
		else
			g_nPriority = 2;
		if(nCount>0 && OnLine) 
			g_pGetData->GetMultiCurrStockData(CodeHead,nCount);
		if(nCount3>0 && DSOnLine) 
			g_pGetData->DSGetMulCurrData((PMULIDX)CodeHead3,nCount3);
		g_nPriority = 1;
	}
	delete []CodeHead; CodeHead = NULL;
	delete []CodeHead3; CodeHead3 = NULL;

	s_ComView_OtherCodeHead++;
}

void AddGPToStatHQ(int iGPNo,int &nCnt,short *lpnStkIndex,int nMaxNum,int nType)
{
	if(!g_pStockIO) return;
	int nLastSeconds = 0;
	int nNowSeonds = GetSeconds();
	MemStkInfo *tmpInfo = (*g_pStockIO)[iGPNo];
	if(!tmpInfo) return;
	if(nType==IMG_STAT_HQ)	
		nLastSeconds = tmpInfo->nImgStatLastSeconds;
	else					
		nLastSeconds = tmpInfo->nTckStatLastSeconds;
	if(iGPNo<DS_FROM && !IsLocalStock(tmpInfo) && nNowSeonds-nLastSeconds>HQSTAT_SEC+6*3600)
	{
		if(nCnt<nMaxNum)
		{
			lpnStkIndex[nCnt]=iGPNo;
			nCnt++;
		}
	}	
}

int CComView::GetGP_NeedRefreshStatHQ(short *lpnStkIndex,int nMaxNum,int nType)
{
	int i,nCnt = 0;
	if( m_nMainPad==MULZST_PAD || (m_nMainPad==MULFXT_PAD && !m_bMultiPeriod) )
	{
		for(i = 0;i < m_nMulGpNum;i++)
			AddGPToStatHQ(m_nMulGpIndex[i],nCnt,lpnStkIndex,nMaxNum,nType);
	}
	else
		AddGPToStatHQ(m_nCurGP,nCnt,lpnStkIndex,nMaxNum,nType);
	if(m_nDComCurGP>=0 && m_nDComUnit>=0)
		AddGPToStatHQ(m_nDComCurGP,nCnt,lpnStkIndex,nMaxNum,nType);
	return nCnt;
}

void CComView::OnTimer(UINT nIDEvent) 
{
	BOOL bIdle = IsIdleStatus();
	if(nIDEvent==0x1111||nIDEvent==0x1112||nIDEvent==0x1113||nIDEvent==0x1114||nIDEvent==0x1115)	
	{	//各组件的定时器
		if(g_bDowning)	return;
		int i;
		FOR_UNIT(i)
			m_Units.pUnit[i]->ProcessMsg(WM_TIMER,nIDEvent);
	}
	if(nIDEvent==0x2222 && m_bAutoPage)			//自动翻页定时器
	{
		if(g_bDowning)	return;
		if(theViewManager.pActiveView!=this)
		{ 
			m_bAutoPage = FALSE; 
			return; 
		}
		OnKeyDown(VK_NEXT,1,0);
		if(OnLine)
			OnKeyUp(VK_NEXT,1,0);
	}
	if(nIDEvent==0x3333 && !bIdle)				//刷新当前PAD
		RefreshCurrPADHQ();
	if(nIDEvent==0x4444 && g_bHasInfo && !bIdle)//分时图的特别定时器
	{
		if(g_bDowning)	return;
		if(m_Units.pUnit[0] && m_nMainPad == ZST_PAD)
			m_Units.pUnit[0]->ProcessMsg(WM_TIMER,0x4444);
	}
	if(nIDEvent==0x4445)						//分时图的即时播报闪烁定时器
	{
		if(m_Units.pUnit[0] && m_nMainPad == ZST_PAD)
			m_Units.pUnit[0]->ProcessMsg(WM_TIMER,0x4445);
	}
	if(nIDEvent==0x6666 && !bIdle)											//全景排行的刷新
	{
		if(g_bDowning)	return;
		int i;
		FOR_UNIT(i)
		{
			if(m_Units.pUnit[i] && m_Units.Type[i] == ZHSORT_UNIT)
			{
				m_Units.pUnit[i]->ProcessMsg(WM_TIMER,0x6666);
				break;
			}
		}
	}
	if(nIDEvent==0x7777  && !bIdle)				//关联行情的定时器
	{
		int i;
		FOR_UNIT(i)
		{
			if(!m_Units.pUnit[i]) continue;
			if(m_Units.nStatus[i]==U_HIDE||m_Units.Type[i] != GLHQ_UNIT) continue;
			m_Units.pUnit[i]->ProcessMsg(WM_TIMER,0x7777);
		}
	}	
	CView::OnTimer(nIDEvent);
}

void CComView::OnMulUnit()
{
	m_bMultiPeriod = FALSE;
	if(m_nMainPad==MULZST_PAD || m_nMainPad==MULFXT_PAD)
		ZoomToOne(m_nCurMulGp,TRUE); //版面的放大
	else
		ZoomToOne(-1,FALSE);
}

void CComView::OnUpdateMulUnit(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_nMainPad==MULZST_PAD || m_nMainPad==MULFXT_PAD);
	
	pCmdUI->Enable(!(m_bMultiPeriod && m_nMainPad==MULFXT_PAD));
}

void CComView::OnMulPeriod()
{
	if(m_bMultiPeriod && m_nMainPad==MULFXT_PAD)
	{
		if(m_nCurPeriodOff>=0 && m_nCurPeriodOff<=5)
		{
			if(ZoomToOne(m_nCurPeriodOff,TRUE))
			{
				m_bMultiPeriod = FALSE;
				return;
			}
		}
	}
	else if(m_nMainPad==FXT_PAD)
	{
		m_bMultiPeriod	= TRUE;
		if(m_nFxPeriod<0 || m_nFxPeriod>=12)
			m_nFxPeriod = PER_MIN1;
		ZoomToOne(-1,FALSE);
	}
}

void CComView::OnUpdateMulPeriod(CCmdUI *pCmdUI) 
{
	pCmdUI->SetCheck(m_bMultiPeriod && m_nMainPad==MULFXT_PAD);
	pCmdUI->Enable(m_nMainPad==FXT_PAD || (m_bMultiPeriod && m_nMainPad==MULFXT_PAD));
}

void CComView::OnFxtMove()
{
	if(m_nMainPad==FXT_PAD)
	{
		((UFxt*)m_Units.pUnit[0])->SetXPanState(!((UFxt*)m_Units.pUnit[0])->GetXPanState());
		m_bFxtMove = ((UFxt*)m_Units.pUnit[0])->GetXPanState();
	}
}

void CComView::OnUpdateFxtMove(CCmdUI* pCmdUI) 
{
	if(m_nMainPad==FXT_PAD)
		pCmdUI->SetCheck(m_bFxtMove);
}

void CComView::OnUpdateOtherFx(CCmdUI* pCmdUI)
{
	switch(pCmdUI->m_nID)
	{
	case ID_300: pCmdUI->SetRadio(m_nMainPad==FXT_PAD); break;
	case ID_501: pCmdUI->SetRadio(m_nMainPad==ZST_PAD && m_nTranZstStyle==ZST_BIG && !g_bTickTStatus); break;
	case ID_502: pCmdUI->SetRadio(m_nMainPad==TICK_PAD); break;
	case ID_503: pCmdUI->SetRadio(m_nMainPad==FJB_PAD); break;
	case ID_504: pCmdUI->SetRadio(m_nMainPad==TICK2_PAD); break;
	case ID_505: pCmdUI->SetRadio(m_nMainPad==MMP_PAD); break;
	case ID_506: pCmdUI->SetRadio(m_nMainPad==ZST_PAD && m_nTranZstStyle==ZST_LB); break;
	case ID_507: pCmdUI->SetRadio(m_nMainPad==ZST_PAD && m_nTranZstStyle==ZST_MMLD); break;
	case ID_508: pCmdUI->SetRadio(m_nMainPad==ZST_PAD && g_bTickTStatus); break;
	}
}

BOOL CComView::ZoomToOne(int nWhich,BOOL bZoom)
{
	DWORD m_nTmpMnd=m_nLastPad&0xFF0000,m_nTmpAssis1=m_nLastPad&0x00FF00,m_nTmpAssis2=m_nLastPad&0x0000FF,tmpNowPad;
	m_nCurGP = GetCurrentGPIndex();
	BOOL bProperty=IsLevel2Property((*g_pStockIO)[m_nCurGP]);
	if(bZoom) //放大
	{
		return FALSE;
		m_bNoChange_MultIn = TRUE;
		if(m_nMainPad==MULZST_PAD||m_nMainPad==MULFXT_PAD)
		{
			if(m_nTmpMnd==ZST_PAD || m_nTmpMnd==FXT_PAD)
			{
				if(m_nTmpMnd==ZST_PAD && m_nTmpAssis1==CB_PAD) //如果上次是主力监控,则回到HQTICK_PAD
					tmpNowPad = ZST_PAD+HQTICK_PAD;
				else
				{
					if(m_nMainPad==MULZST_PAD)
						tmpNowPad=ZST_PAD+(g_GH.bPinAssistPad?m_nTmpAssis1:HQTICK_PAD);
					else
						tmpNowPad=FXT_PAD+(g_GH.bPinAssistPad?m_nTmpAssis1:HQZST_PAD);
				}
			}
			else if(m_nTmpMnd==MULZST_PAD || m_nTmpMnd==MULFXT_PAD) //如果是多股情况，则不能用以前的版面类型
				tmpNowPad = (m_nMainPad==MULZST_PAD)?(ZST_PAD+HQTICK_PAD):(FXT_PAD+HQZST_PAD);
			else
				tmpNowPad = ZST_PAD+HQTICK_PAD;
			tmpNowPad+=HQ10QUEUE_PAD;
			AdjustLevel2Pad(tmpNowPad,bProperty);
			DynaZoom(this,m_Units.Rect[m_nCurMulGp],m_Rect);//在GetOne2MulGp前
			m_nMulGpNum = GetOne2MulGp(m_nMulGpIndex,1,FALSE);
			SwitchPad(tmpNowPad);
			m_bMult_In = TRUE;
			return TRUE;
		}
		else if(nWhich==2 && (m_nAssisPad1==HQTICK_PAD||m_nAssisPad1==HQFJB_PAD||m_nAssisPad1==HQTICK2_PAD||m_nAssisPad1==HQMMP_PAD) )
		{
			if(m_nAssisPad1==HQTICK_PAD)		tmpNowPad = TICK_PAD;
			else if(m_nAssisPad1==HQFJB_PAD)	tmpNowPad = FJB_PAD;
			else if(m_nAssisPad1==HQTICK2_PAD)	tmpNowPad = TICK2_PAD;
			else								tmpNowPad = MMP_PAD;
			AdjustLevel2Pad(tmpNowPad,bProperty);
			DynaZoom(this,m_Units.Rect[2],m_Rect);//在GetOne2MulGp前
			SwitchPad(tmpNowPad);
			return TRUE;
		}
	}
	else
	{
		if(m_bMultiPeriod && m_nMainPad==FXT_PAD)
		{
			int wid = m_Rect.Width()/3;
			int hei = m_Rect.Height()/2;

			CRect tmpRect = CRect((m_nCurPeriodOff%3)*wid,(m_nCurPeriodOff/3)*hei,(m_nCurPeriodOff%3+1)*wid,(m_nCurPeriodOff/3+1)*hei);

			DynaZoom(this,m_Rect,tmpRect);
			SwitchPad(MULFXT_PAD);
			return TRUE;
		}
		else if(m_nMainPad<TICK_PAD)
		{
			g_nMulNum = g_nNewMulNum;
			tmpNowPad = (m_nMainPad==FXT_PAD)?MULFXT_PAD:MULZST_PAD;
			m_nMulGpNum = GetOne2MulGp(m_nMulGpIndex,g_nMulNum,TRUE);
		
			int wquat=2,hquat=2;
			GetMulQuat(wquat,hquat);

			int wid = m_Rect.Width()/wquat,hei = m_Rect.Height()/hquat;
			int i = m_nCurMulGp/wquat,j = m_nCurMulGp%wquat;
			CRect tmpRect = CRect(j*wid,i*hei,(j+1)*wid,(i+1)*hei);

			DynaZoom(this,m_Rect,tmpRect); //在GetOne2MulGp后
			SwitchPad(tmpNowPad);
			return TRUE;
		}
		else if(m_nMainPad <= MMP_PAD)
		{
			int assisPad1;
			if(m_nMainPad==TICK_PAD)		assisPad1 = HQTICK_PAD;
			else if(m_nMainPad==FJB_PAD)	assisPad1 = HQFJB_PAD;
			else if(m_nMainPad==TICK2_PAD)	assisPad1 = HQTICK2_PAD;
			else							assisPad1 = HQMMP_PAD;
			if(m_nTmpMnd!=ZST_PAD && m_nTmpMnd!=FXT_PAD)
				m_nTmpMnd = ZST_PAD;
			tmpNowPad = m_nTmpMnd+assisPad1+HQ10QUEUE_PAD;
			DynaZoom(this,m_Rect,CRect(m_Rect.Width()-g_nFixedWidth,FIXED_HEIGHT,m_Rect.Width(),m_Rect.Height()-SWITCH_HEIGHT2));
			SwitchPad(tmpNowPad);
			return TRUE;
		}
	}
	return FALSE;
}

//为-1,没有点到任何Unit
int	CComView::HitTest(CPoint point)
{
	int i,nRet = -1;
	FOR_UNIT(i)
	{
		if((m_Units.Rect[i]).PtInRect(point))
		{
			nRet = i;
			break;
		}
	}
	if(m_nDComCurGP>=0 && m_nDComUnit>=0 && m_nDComUnit==nRet)
		m_nF = nRet;
	else
		m_nF = 0;
	return nRet;
}

BOOL CComView::InSplitPos1(CPoint point)
{
	if( (!g_GH.bLeftHQInfo && point.x > m_Rect.right-g_nFixedWidth-5 && point.x<m_Rect.right-g_nFixedWidth+5) ||
		(g_GH.bLeftHQInfo && point.x > g_nFixedWidth-5 && point.x<g_nFixedWidth+5)
	  )
		return TRUE;
	return FALSE;
}

BOOL CComView::InSplitPos2(CPoint point)
{
	if( (!g_GH.bLeftHQInfo && point.x > m_Rect.right-g_nFixedWidth-g_nFixedWidth2-5 && point.x<m_Rect.right-g_nFixedWidth-g_nFixedWidth2+5) ||
		(g_GH.bLeftHQInfo && point.x > g_nFixedWidth+g_nFixedWidth2-5 && point.x<g_nFixedWidth+g_nFixedWidth2+5)
	  )
		return TRUE;
	return FALSE;
}

void CComView::OnMouseMove(UINT nFlags, CPoint point) 
{
	//显示分割窗口光标
	if(m_bNowShow && (InSplitPos1(point) || (m_bShowAssisPad2 && InSplitPos2(point))))
	{
		SetCursor(m_hVerSplit);
		return;
	}
	//分割窗口过程中
	if(m_bSplitting1 || m_bSplitting2)
	{
		CDC *pDC = GetDC();		
		int oldrop = pDC->SetROP2(R2_XORPEN);
		g_d.DrawLine(pDC,m_nOldSplitX,m_Rect.top,m_nOldSplitX,m_Rect.bottom,VipColor.LEVELCOLOR^VipColor.BACKCOLOR);
		g_d.DrawLine(pDC,point.x,m_Rect.top,point.x,m_Rect.bottom,VipColor.LEVELCOLOR^VipColor.BACKCOLOR);
		m_nOldSplitX = point.x;
		pDC->SetROP2(oldrop);
		ReleaseDC(pDC);
		return;
	}
	if(!(m_nMainPad==FXT_PAD && m_Units.Rect[0].PtInRect(point)) && !(m_nMainPad==ZST_PAD && m_Units.Rect[0].PtInRect(point)) )
		SetCursor(LoadCursor(NULL,IDC_ARROW));
	if(m_nMainPad<TICK_PAD)		//ZST,FXT等版面的Zoom鼠标
	{
		CRect Corner1(m_Rect.Width()-CORNER_SQUARE,0,m_Rect.Width(),CORNER_SQUARE);
		CRect Corner2(0,0,0,0);
		if(!g_GH.bLeftHQInfo)
		{
			if(m_bShowAssisPad2)
				Corner1.SetRect(m_Rect.Width()-g_nFixedWidth-g_nFixedWidth2-CORNER_SQUARE,0,m_Rect.Width()-g_nFixedWidth-g_nFixedWidth2,CORNER_SQUARE);
			else
				Corner1.SetRect(m_Rect.Width()-g_nFixedWidth-CORNER_SQUARE,0,m_Rect.Width()-g_nFixedWidth,CORNER_SQUARE);
			Corner2.SetRect(m_Rect.left,0,m_Rect.left+CORNER_SQUARE,CORNER_SQUARE);
		}
		if(Corner1.PtInRect(point))	
		{
			SetCursor(m_hZoomCur);
			return;
		}
		if(m_nMainPad==FXT_PAD && Corner2.PtInRect(point))	
		{
			SetCursor(m_hZoomCur3);
			return;
		}
	}
	else if(m_nMainPad<=MMP_PAD)//TICK,FJB,TICK2,MMP等版面的Zoom鼠标
	{
		CRect Corner(0,0,CORNER_SQUARE,CORNER_SQUARE);
		if(Corner.PtInRect(point))	
		{
			SetCursor(m_hZoomCur2);
			return;
		}
	}
	int iHit = HitTest(point);
	if(iHit != -1) m_Units.pUnit[iHit]->ProcessMsg(WM_MOUSEMOVE,nFlags,MAKELONG(point.x,point.y));
}

int CComView::OnMulUnitClick(CPoint point)
{
	int i,nWhich = HitTest(point);
	if(m_bMultiPeriod && m_nMainPad==MULFXT_PAD)
	{
		if(nWhich>=0 && nWhich<6)
		{
			m_nCurPeriodOff = nWhich;
			m_nFxPeriod = l_nMulPeriodOrder[(m_nFirstPeriod+nWhich)%8];
			FOR_UNIT(i)
			{
				m_Units.pUnit[i]->ProcessMsg(UM_SETSEL,m_nCurPeriodOff==i);
				m_Units.pUnit[i]->ProcessMsg(WM_PAINT);
			}
		}
	}
	else
	{
		if(nWhich!=-1 && (m_nMainPad==MULZST_PAD||m_nMainPad==MULFXT_PAD))
		{
			if(nWhich >= m_nMulGpNum) return -1;
			m_nCurMulGp = nWhich;
			FOR_UNIT(i)
			{
				m_Units.pUnit[i]->ProcessMsg(UM_SETSEL,i==m_nCurMulGp);
				m_Units.pUnit[i]->ProcessMsg(WM_PAINT);
			}
		}
	}
	return nWhich;
}

void CComView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//开始分割
	if(m_bNowShow  )
	{
		if(!m_bSplitting1 && InSplitPos1(point))
		{
			m_bSplitting1 = TRUE;
			SetCapture();
			m_nOldSplitX = -10; //置为无效
			return;
		}
		if(m_bShowAssisPad2 && !m_bSplitting2 && InSplitPos2(point))
		{
			m_bSplitting2 = TRUE;
			SetCapture();
			m_nOldSplitX = -10; //置为无效
			return;
		}
	}
	int i = OnMulUnitClick(point);
	if(i == -1) return;		//在多股情况下没有选中一个
	if(m_nMainPad<TICK_PAD)	//ZST,FXT等版面的缩小
	{
		CRect Corner1(m_Rect.Width()-CORNER_SQUARE,0,m_Rect.Width(),CORNER_SQUARE);
		CRect Corner2(0,0,0,0);
		if(!g_GH.bLeftHQInfo)
		{
			if(m_bShowAssisPad2)
				Corner1.SetRect(m_Rect.Width()-g_nFixedWidth-g_nFixedWidth2-CORNER_SQUARE,0,m_Rect.Width()-g_nFixedWidth-g_nFixedWidth2,CORNER_SQUARE);
			else
				Corner1.SetRect(m_Rect.Width()-g_nFixedWidth-CORNER_SQUARE,0,m_Rect.Width()-g_nFixedWidth,CORNER_SQUARE);
			Corner2.SetRect(m_Rect.left,0,m_Rect.left+CORNER_SQUARE,CORNER_SQUARE);
		}
		if(Corner1.PtInRect(point))	
		{
			m_bMultiPeriod = FALSE;
			ZoomToOne(-1,FALSE);
			return;
		}
		if(m_nMainPad==FXT_PAD && Corner2.PtInRect(point))	
		{
			m_bMultiPeriod	= TRUE;
			if(m_nFxPeriod<0 || m_nFxPeriod>=12)
				m_nFxPeriod = PER_MIN1;
			ZoomToOne(-1,FALSE);
			return;
		}
	}
	else if(m_nMainPad<=MMP_PAD)//TICK,FJB,TICK2,MMP等版面的缩小
	{
		CRect Corner(0,0,CORNER_SQUARE,CORNER_SQUARE);
		if(Corner.PtInRect(point))	
		{	ZoomToOne(-1,FALSE);  return;  }
	}
	m_Units.pUnit[i]->ProcessMsg(WM_LBUTTONDOWN,nFlags,MAKELONG(point.x,point.y));
}

void CComView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//停止分割
	if(m_bSplitting1)
	{
		m_bSplitting1 = FALSE;
		ReleaseCapture();
		SetCursor(LoadCursor(NULL,IDC_ARROW));
		if(!g_GH.bLeftHQInfo)
			g_nFixedWidth = max(198,m_Rect.right-point.x);
		else
			g_nFixedWidth = max(198,point.x);
		g_nFixedWidth = min(320,g_nFixedWidth);
		if(g_bHasDS && g_nFixedWidth<198) g_nFixedWidth=198;
		ResizePad();
		SetCursor(m_hVerSplit);
		return;
	}
	if(m_bSplitting2)
	{
		m_bSplitting2 = FALSE;
		ReleaseCapture();
		SetCursor(LoadCursor(NULL,IDC_ARROW));
		if(!g_GH.bLeftHQInfo)
			g_nFixedWidth2 = max(214,m_Rect.right-point.x-g_nFixedWidth);
		else
			g_nFixedWidth2 = max(214,point.x-g_nFixedWidth);
		g_nFixedWidth2 = min(320,g_nFixedWidth2);
		ResizePad();
		SetCursor(m_hVerSplit);
		return;
	}
	int i = HitTest(point);
	if(i != -1) m_Units.pUnit[i]->ProcessMsg(WM_LBUTTONUP,nFlags,MAKELONG(point.x,point.y));
}

void CComView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{	
	int i = OnMulUnitClick(point);
	if(i == -1) return;			//在多股情况下没有选中一个
	if(ZoomToOne(i,TRUE))		//多股版面或右下角下图的放大
		return;
	else if(m_nMainPad==TICK_PAD || m_nMainPad==FJB_PAD || m_nMainPad==TICK2_PAD || m_nMainPad==MMP_PAD)
	{
		if(ZoomToOne(i,FALSE)) 
			return;				//Tick等大版面的缩小
	}
	m_Units.pUnit[i]->ProcessMsg(WM_LBUTTONDBLCLK,nFlags,MAKELONG(point.x,point.y));
}

void CComView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	int i = HitTest(point);
	if( (i==2&&m_nMainPad!=MULZST_PAD&&m_nMainPad!=MULFXT_PAD) || 
	    (i==0&&m_nMainPad>=TICK_PAD && m_nMainPad<=MMP_PAD) )
	{
		CNewMenu menu,*pPopup;
		menu.LoadMenu (IDR_LONELY_MENU);
		if(i==2 && m_nAssisPad1<=YD_PAD)
		//	pPopup = (CNewMenu *)menu.GetSubMenu(9);
			return;
		else if(i==2 || i==1)
		{
			if(m_nAssisPad1==HQZST_PAD||m_nAssisPad1==HQZS_PAD) 
			{
				if(m_Units.Type[i]==ZST_UNIT)
				{
					UZst *pZst = (UZst*)m_Units.pUnit[i];
					pZst->m_bBeginRectangle = FALSE;
					pZst->ProcessMsg(WM_PAINT);
				}
			}
			pPopup = (CNewMenu *)menu.GetSubMenu(8);
		}
		else
			pPopup = (CNewMenu *)menu.GetSubMenu(10);
		ClientToScreen (&point);
		pPopup->TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,AfxGetMainWnd());
		return;
	}
	if(i != -1) m_Units.pUnit[i]->ProcessMsg(WM_RBUTTONUP,nFlags,MAKELONG(point.x,point.y));
}

void CComView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	int i = HitTest(point);
	if(i != -1) m_Units.pUnit[i]->ProcessMsg(WM_RBUTTONDOWN,nFlags,MAKELONG(point.x,point.y));
}

BOOL CComView::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_LBUTTONDOWN||pMsg->message==WM_RBUTTONDOWN||
		pMsg->message==WM_LBUTTONUP||pMsg->message==WM_RBUTTONUP)
	{
		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		int iUnit = HitTest(pt);
		if(iUnit>=0&&iUnit<=m_Units.Num&&m_nF!=iUnit)
			m_nF = iUnit;
	}
	if(m_nMainPad==MULFXT_PAD)
	{
		if((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F8)
			||(pMsg->message == WM_CHAR && (pMsg->wParam == ']'||pMsg->wParam == '[')))
		{
			int i;
			FOR_UNIT(i)
			{
				if(m_Units.pUnit[i] && m_Units.Type[i]==FXT_UNIT)
				{
					if(((UFxt *)(m_Units.pUnit[i]))->CheckReqing())
						return TRUE;
				}
			}
			if(m_bMultiPeriod)
			{
				m_nFirstPeriod = (m_nFirstPeriod+1)%8;
				FOR_UNIT(i)
				{
					if(m_Units.pUnit[i])
					{
						m_Units.pUnit[i]->ProcessMsg(UM_SET_FXPERIOD,l_nMulPeriodOrder[(i+m_nFirstPeriod)%8]);
						m_Units.pUnit[i]->ProcessMsg(UM_SETSEL,m_nCurPeriodOff==i);
						m_Units.pUnit[i]->ProcessMsg(WM_PAINT,1);
					}
				}
			}
			else
			{
				//如果是[,则相当于反向F8
				LPARAM lParam=0;
				if(pMsg->message == WM_CHAR && pMsg->wParam == '[')
					lParam=1;
				FOR_UNIT(i)
				{
					if(m_Units.pUnit[i])
						((UFxt*)m_Units.pUnit[i])->ProcessMsg(WM_KEYDOWN,VK_F8,lParam);
				}
			}
			return TRUE;
		}
	}
	if(m_nMainPad==FXT_PAD || m_nMainPad==ZST_PAD )
	{
		//'键显隐右边信息区
		if(pMsg->message == WM_CHAR && pMsg->wParam == '\'')
		{
			PostMessage(WM_COMMAND,ID_HIDERIGHT);
			return TRUE;
		}
		//`键(ESC下面的那个键)开关扩展区(如果有的话)
		if(m_bShowSwitch3 && pMsg->message == WM_CHAR && pMsg->wParam == '`')
		{
			OnMoreShow();
			return TRUE;
		}
	}
	return CUnitView::PreTranslateMessage(pMsg);
}

void CComView::OnSwitchlbmmld() 
{
	if(m_nMainPad==ZST_PAD)
	{
		m_Units.pUnit[0]->ProcessMsg(UM_SWITCHSTYLE);
		m_nTranZstStyle=((UZst *)m_Units.pUnit[0])->GetStyle();
	}
}

void CComView::OnPageup() 
{
	OnKeyDown(VK_PRIOR,1,0);
	if(OnLine)
		OnKeyUp(VK_PRIOR,1,0);
}

void CComView::OnPagedown() 
{
	OnKeyDown(VK_NEXT,1,0);
	if(OnLine)
		OnKeyUp(VK_NEXT,1,0);
}

#include "ZstPlayDlg.h"
void CComView::OnZstPlay()
{
	if(m_nMainPad==ZST_PAD)
	{
		int bsync = m_Units.pUnit[0]->ProcessMsg(UM_GET_PLAY); //如果有同步，则取消之，否则，激活之
		int nRet = m_Units.pUnit[0]->ProcessMsg(UM_SET_PLAY,(bsync==1)?0:1);
		if(nRet==0) 
		{
			TDX_MessageBox(m_hWnd,"数据不全，不能进行分时重播!",MB_OK|MB_ICONEXCLAMATION);
			return;
		}
		if(m_Units.pUnit[2])
			m_Units.pUnit[2]->ProcessMsg(UM_SET_PLAY,(bsync==1)?0:1);
		if(m_Units.pUnit[1])
			m_Units.pUnit[1]->ProcessMsg(UM_SET_PLAY,(bsync==1)?0:1);
		if(!bsync)
		{
			m_bAutoPage = FALSE;
			SendMessage(UM_REFLECT,Reflect_ToZstime,9*60);
			CZstPlayDlg pSyncDlg;
			pSyncDlg.DoModal();
		}
	}
}

void CComView::OnUpdateZstPlay(CCmdUI* pCmdUI)
{
	if(m_nMainPad==ZST_PAD && m_Units.pUnit[0])
		pCmdUI->SetCheck((m_Units.pUnit[0]->ProcessMsg(UM_GET_PLAY))==1);
}

#include "ExportDlg.h"
void CComView::OnWritetotxtfile()
{
	BOOL	bExportTick = FALSE;
	int		iIndex = GetCurrentGPIndex();
	MemStkInfo *tmpInfo=(*g_pStockIO)[iIndex];
	if(!tmpInfo) return;
	CExportDlg dlg;
	dlg.m_strInitFileName = tmpInfo->Code;
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.m_nTabSel == 0 || dlg.m_nTabSel == 1)
		{
			if(m_nMainPad==FXT_PAD)
				((UFxt*)m_Units.pUnit[0])->WriteToTxtFile(dlg.m_strFileName,dlg.m_nTabSel);
			else if(m_nMainPad==TICK_PAD && m_Units.Type[0]==TICK_UNIT)
			{
			}
			else
			{
				TDX_MessageBox(m_hWnd,"仅仅行情、分时成交明细、报表和分析版面能导出文本数据!",MB_OK|MB_ICONEXCLAMATION);
				return;
			}
			if(m_nMainPad==TICK_PAD && m_Units.Type[0]==TICK_UNIT)
			{
				((UTick*)m_Units.pUnit[0])->WriteToTxtFile(dlg.m_strFileName,dlg.m_nTabSel,9*60);
				bExportTick = TRUE;
			}
		}
		CString tmpMessage;
		tmpMessage="导出成功,是否打开此文件?";
		if(bExportTick)
			tmpMessage="请等待分笔请求数据停止后再打开导出文件。\r\n\r\n如果分笔请求停止了，请点击[确定]查看导出文件。";
		if(TDX_MessageBox (m_hWnd,tmpMessage,MB_ICONQUESTION|MB_OKCANCEL)==IDOK)
		{
			CString tmpStr;
			tmpStr.Format("\"%s\"",dlg.m_strFileName);
			::ShellExecute(NULL,NULL,tmpStr,NULL,NULL,SW_SHOW);
		}		
	}
}

void CComView::OnZoomCorner()
{
	ZoomToOne(2,TRUE);
}

void CComView::OnZoomToCorner()
{
	ZoomToOne(0,FALSE);
}

void CComView::OnCornerClick(UINT nID)
{
	if(m_nMainPad==ZST_PAD || m_nMainPad==FXT_PAD)
		SwitchPad(m_nMainPad+GetAssis1FromSwitch(nID-ID_CORNER_1)+m_nAssisPad2);
}

void CComView::OnCorner2Click(UINT nID)
{
	if(m_nMainPad==ZST_PAD || m_nMainPad==FXT_PAD)
		SwitchPad(m_nMainPad+m_nAssisPad1+GetAssis2FromSwitch(nID-ID_CORNER2_1));
}

void CComView::OnCorner3Click(UINT nID)
{
	if(m_nMainPad==ZST_PAD || m_nMainPad==FXT_PAD)
	{
		BOOL bLastShow = FALSE;
		if(m_nMainPad==ZST_PAD)
		{
			if(m_bZstMore_Show)
				bLastShow = TRUE;
		}
		else 
		{
			if(m_bFxtMore_Show)
				bLastShow = TRUE;
		}
		if(bLastShow && m_nMore_Sel==(nID-ID_CORNER3_1))
		{
			OnMoreShow();
			return;
		}
		if(m_nMainPad==ZST_PAD)
			m_bZstMore_Show = TRUE;
		else
			m_bFxtMore_Show = TRUE;
		m_nMore_Sel = nID-ID_CORNER3_1;
		m_pSwitchBar3->SetCurLabel(m_nMore_Sel+1);
		//得到几个Unit的位置
		CRect	rc,DComRc,SeatRc,MoreRc;
		GetMainAreaRect(rc,DComRc,SeatRc,MoreRc);
		if(m_nDComUnit>1)
			CreateUnit(m_nDComUnit-1,m_nCurGP,MORE_UNIT,m_nTranZstStyle);		
		if(!bLastShow)
			ResizePad(COM_OnlySize);
		if(m_nDComUnit>1)
			SetUnitReadAndPaint(m_nDComUnit-1,MoreRc,COM_ReadDraw);	
	}
}
void CComView::OnMoreShow()
{
	if(m_nMainPad==ZST_PAD)
	{
		m_bZstMore_Show = !m_bZstMore_Show;
		ResizePad(COM_ReadDraw);
	}
	else if(m_nMainPad==FXT_PAD)
	{
		m_bFxtMore_Show = !m_bFxtMore_Show;
		ResizePad(COM_ReadDraw);
	}
}

BOOL CComView::TestUnitProperty(int iUnit, UINT nFlag)
{
	return TRUE;
}

void CComView::OnVScrollMessage(WPARAM wParam,LPARAM lParam)
{
	int i;
	FOR_UNIT(i)
		m_Units.pUnit[i]->ProcessMsg(UM_VSCROLLBAR,wParam,lParam);	
}

BOOL CComView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(zDelta < 0)	PostMessage(WM_COMMAND,ID_PAGEDOWN);
	else			PostMessage(WM_COMMAND,ID_PAGEUP);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

#include "KeyGuy.h"
void CComView::OnGoBack()
{
	if(m_bMult_In||m_GoCache.empty()) return;

	--m_nGoCacheCursor;
	m_nGoCacheCursor = max(0, m_nGoCacheCursor);
	MemStkInfo *pInfo = (*g_pStockIO)[m_GoCache[m_nGoCacheCursor]];
	if(pInfo)
	{
		m_bInGoCache = TRUE;
		char strKeyVal[64]={0};
		sprintf(strKeyVal, "%d%s", pInfo->setcode, pInfo->Code);
		CKeyGuy::OnCommonProcess(0, strKeyVal);
	}
}

void CComView::OnUpDateGoBack(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_GoCache.size()>1&&m_nGoCacheCursor>0&&!m_bMult_In);
}

void CComView::OnGoNext()
{
	if(m_bMult_In||m_GoCache.empty()) return;

	++m_nGoCacheCursor;
	m_nGoCacheCursor = min(m_GoCache.size()-1, m_nGoCacheCursor);
	MemStkInfo *pInfo = (*g_pStockIO)[m_GoCache[m_nGoCacheCursor]];
	if(pInfo)
	{
		m_bInGoCache = TRUE;
		char strKeyVal[64]={0};
		sprintf(strKeyVal, "%d%s", pInfo->setcode, pInfo->Code);
		CKeyGuy::OnCommonProcess(0, strKeyVal);
	}
}

void CComView::OnUpDateGoNext(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_GoCache.size()>1&&m_nGoCacheCursor<m_GoCache.size()-1&&!m_bMult_In);
}

BOOL CComView::ChangeMultiGP(const char* Code,short setcode, int nXH)
{
	if(m_nMainPad!=MULFXT_PAD&&m_nMainPad!=MULZST_PAD) return FALSE;

	if(m_nMainPad==MULFXT_PAD && m_bMultiPeriod)
	{
		long nIDIndex = g_pStockIO->GetIndex(Code, setcode);
		if(nIDIndex<0) return FALSE;
		m_nCurGP = nIDIndex;
		int i = 0;
		FOR_UNIT(i)
		{
			m_bNoChange_MultIn = TRUE;
			ChangeUnitGP(i,m_nCurGP);
			m_Units.pUnit[i]->ProcessMsg(UM_SETSEL,m_nCurPeriodOff==i);
			m_Units.pUnit[i]->ProcessMsg(WM_PAINT,1);
		}
		SetComTitle();
	}
	else if(m_nPageGPNum!=-1) //有特定的股票集合情况下才能更新多股同列中的股票
	{
		if(nXH<0||nXH>=g_nMulNum) nXH = min(m_nCurMulGp, g_nMulNum-1);

		long nIDIndex = g_pStockIO->GetIndex(Code, setcode);
		if(nIDIndex<0) return FALSE;
		
		m_nPageGPIndex[m_nCurPageGP+nXH] = nIDIndex;
		
		m_nMulGpNum = GetOne2MulGp(m_nMulGpIndex,g_nMulNum,TRUE);
		m_Units.pUnit[nXH]->ProcessMsg(UM_SET_STOCK, nIDIndex);
		m_Units.pUnit[nXH]->ProcessMsg(WM_PAINT, 1);
		m_nCurMulGp = nXH;	//GetOne2MulGp可能会改变m_nCurMulGp,所以要重新赋值
	}
	return TRUE;
}

#include "SelectGPDlg.h"
void CComView::OnDcomlayer() 
{
	if(g_bTickTStatus)
		return;
	if(m_nDComCurGP>=0)
	{
		short nOldGP = m_nDComCurGP;
		m_nDComCurGP = -1;
		if(nOldGP!=m_nDComCurGP) ResizePad(COM_ReadDraw);
		m_nF = 0;
	}
	else
	{
		int iIndex=GetCurrentGPIndex();
		MemStkInfo *tmpInfo=(*g_pStockIO)[iIndex];
		if(!tmpInfo) return;
		//打开选择股票对话框
		CSelectGPDlg dlg;
		dlg.m_pStkInfo=tmpInfo;
		dlg.bSingleSelect = TRUE;
		dlg.bOverlapGpType = 3;
		dlg.bCurDomain = tmpInfo->setcode;		
		if(dlg.DoModal() == IDOK)
		{
			if(dlg.SelectedGPNum > 0)
			{
				m_nDComCurGP = dlg.SelectedGPIndex[0];
				ChangeUnitGP(m_nDComUnit,m_nDComCurGP);
				ResizePad(COM_ReadDraw);
			}
		}
	}
}

void CComView::OnUpdateDcomlayer(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nDComCurGP>=0);
}

void CComView::OnLevinMenu(UINT nID)
{
	if(m_nMainPad==ZST_PAD)
		m_Units.pUnit[0]->ProcessMsg(WM_COMMAND,nID);
}

void CComView::JumpToZB(char *ZBCode,int nTarRegion)
{
	if( m_nMainPad == FXT_PAD && m_Units.pUnit[0] )
	{
		UFxt *pUfxt = (UFxt*)m_Units.pUnit[0];
		pUfxt->JumpToZB(ZBCode,nTarRegion);
	}
}
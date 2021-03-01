// UnitView.cpp : implementation file
#include "stdafx.h"
#include "tdxw.h"
#include "HqGrid.h"
#include "UnitView.h"
#include "comview.h"
#include "comview2.h"
#include "MainFrm.h"
#include "FormulaPackDlg.h"

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CUnitView, CView)
	ON_COMMAND_RANGE(ID_IMAGEK, ID_TOWLINE, OnHistoryDrawType)
	ON_UPDATE_COMMAND_UI_RANGE(ID_IMAGEK, ID_TOWLINE, OnUpdateHistoryDrawType)
	
	ON_COMMAND_RANGE(ID_FMLPACK00,ID_FMLPACK19,OnChooseFP)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FMLPACK00,ID_FMLPACK19,OnUpdateChooseFP)
	ON_COMMAND(ID_ADDFMLPACK,OnAddFmlPack)
	ON_COMMAND(ID_FMLPACK2STK,OnFmlPack2Stk)
	ON_COMMAND(ID_FMLPACKUNLOCK,OnFmlPackUnlock)
	ON_UPDATE_COMMAND_UI(ID_FMLPACKUNLOCK,OnUpdateFmlPackUnlock)
	ON_COMMAND(ID_FMLPACKMNG,OnFmlPackMng)
	
	ON_COMMAND(ID_INBLOCK,OnInBlock)
	ON_COMMAND(ID_RIGHT_ADDBLOCK,OnRightAddBlock)
	ON_COMMAND(ID_RIGHT_ADDZXG,OnRightAddZxg)
	ON_COMMAND(ID_DELFROMBLOCK,OnDelFromBlock)
	ON_COMMAND(ID_GPCOOLINFO,OnGPCoolInfo)

	ON_COMMAND_RANGE(ID_NOTQ,ID_PARTTQ,OnTQ)
	ON_UPDATE_COMMAND_UI_RANGE(ID_NOTQ,ID_PARTTQ,OnUpdateTQ)
	ON_COMMAND_RANGE(CHART_1, CHART_10, OnWindowNum)
	ON_UPDATE_COMMAND_UI_RANGE(CHART_1, CHART_10, OnUpdateWindowNum)
	ON_COMMAND_RANGE(ID_NORMAL, ID_GOLDEN, OnFxtAxis)
	ON_UPDATE_COMMAND_UI_RANGE(ID_NORMAL, ID_GOLDEN, OnUpdateFxtAxis)
	ON_COMMAND_RANGE(ID_MIN1, ID_YEAR, OnPeriod)
	ON_COMMAND_RANGE(ID_FXTSPEC0,ID_FXTSPEC2,OnSpecPeriod)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MIN1, ID_YEAR, OnUpdatePeriod)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FXTSPEC0, ID_FXTSPEC2, OnUpdateSpecPeriod)

	ON_COMMAND(ID_HISZS,OnHisDlg)
	ON_UPDATE_COMMAND_UI(ID_HISZS,OnUpdateHisDlg)

	ON_COMMAND_RANGE(ID_MENU_ZB1, ID_MOREINDEX, OnMenuZb)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MENU_ZB1, ID_MOREINDEX, OnUpdateMenuZb)

	ON_COMMAND_RANGE(ID_READY,ID_HIDESELFLINE,OnDrawLineTools)
	ON_UPDATE_COMMAND_UI_RANGE(ID_READY,ID_HIDESELFLINE,OnUpdateDrawLineTools)

	ON_MESSAGE(UM_CHANGEPARAM, OnChangeParam)
	ON_COMMAND(ID_SHOWPERIOD,OnShowPeriod)
	ON_UPDATE_COMMAND_UI(ID_SHOWPERIOD,OnUpdateShowPeriod)
	ON_COMMAND(ID_SHOWWINNUM,OnShowWinNum)
	ON_COMMAND(ID_SHOWTQ,OnShowTQ)
	ON_UPDATE_COMMAND_UI(ID_SHOWTQ,OnUpdateShowTQ)
	ON_COMMAND(ID_MANYDAYZST,OnManyDayZstMenu)
	ON_UPDATE_COMMAND_UI(ID_MANYDAYZST,OnUpdateManyDayZstMenu)

	ON_COMMAND(ID_HXZBDLG,OnHxZbDlg)

	ON_COMMAND(ID_MODIFY_FORMULA, OnModifyFormula)
	ON_COMMAND(ID_DELZB, OnDelzb)
	ON_COMMAND(ID_INDEXHELP, OnIndexhelp)
	ON_UPDATE_COMMAND_UI(ID_DELETESHOW, OnUpdateDeleteshow)
	ON_COMMAND(ID_DELETESHOW, OnDeleteshow)

	ON_COMMAND(ID_QJSTAT,OnQjStat)
	ON_COMMAND(ID_ALLQJSTAT,OnAllQjStat)
	ON_MESSAGE(UM_DELETE_INFODLG,OnDeleteInfoDlg)
	
	ON_COMMAND_RANGE(ID_MANYDAYZST0,ID_MANYDAYZST9,OnManyDayZst)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MANYDAYZST0,ID_MANYDAYZST9,OnUpdateManyDayZst)

	ON_COMMAND_RANGE(ID_MANYDAYTICKZST0,ID_MANYDAYTICKZST9,OnManyDayTickZst)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MANYDAYTICKZST0,ID_MANYDAYTICKZST9,OnUpdateManyDayTickZst)
	ON_COMMAND(ID_TICKZSTUNDO,OnTickZstUndo)
	ON_COMMAND(ID_TICKZST2ZST, OnTickZst2Zst)

	ON_COMMAND_RANGE(ID_EXPSYSSHOW, ID_KLINESHOW, OnShow)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EXPSYSSHOW, ID_KLINESHOW, OnUpdateShow)
	ON_COMMAND(ID_OVERLAP, OnOverlap)
	ON_COMMAND_RANGE(ID_AUTOVERIDX, ID_AUTOVERLAST, OnAutoOverlap)
	ON_UPDATE_COMMAND_UI_RANGE(ID_AUTOVERIDX, ID_AUTOVERLAST, OnUpdateAutoOverlap)
	ON_COMMAND(ID_UNOVERLAP, OnUnOverlap)
	ON_COMMAND(ID_ZB_OVERLAP, OnZbOverlap)
	ON_COMMAND(ID_CHANGEINDEXPARAM, OnChangeindexparam)
	ON_COMMAND(ID_COLOR, OnColor)
	ON_COMMAND(ID_TPAXIS,OnTpAxis)
	ON_UPDATE_COMMAND_UI(ID_TPAXIS,OnUpdateTpAxis)
	ON_COMMAND(ID_DELOVERLAPGP, OnDeloverlapgp)

	ON_COMMAND(ID_AUTOPAGE, OnAutopage)
	ON_UPDATE_COMMAND_UI(ID_AUTOPAGE, OnUpdateAutopage)

	ON_COMMAND(ID_TABKEY,OnTabKey)
	
	ON_COMMAND(ID_MARK_CANCEL, OnCancelMark)
	ON_COMMAND_RANGE(ID_MARK_ADD1, ID_MARK_ADD10, OnMarkStockFlag)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MARK_ADD1, ID_MARK_ADD10, OnUpdateMarkStockFlag)

	ON_COMMAND_RANGE(ID_GLHQ_SWITCH1,ID_GLHQ_SWITCH100,OnGlHqSwitch)

	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	ON_MESSAGE(UM_GETMAINID,GetMainID)
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()

	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

CUnitView::CUnitView(BOOL bIsComView2)
{
	m_bIsComView2 = bIsComView2;

	m_nF = 0;
	m_nTQState = g_nTQFlag;
	m_bAutoPage = FALSE;

	m_bQuitting = FALSE;
	m_bLayoutFlag	= FALSE;
	KeyGuyFlag  = FALSE;
	memset(&m_Units,0,sizeof(UNITS));

	m_nFxPeriod = -1;		//为-1表示使用分析图中上次保留的周期
	m_nOften301 = -1;		//为-1表示使用分析图中上次保留的指标
	m_Rect.SetRect(0,0,0,0);
	WindowMacro = COMVIEW_WIN;
	m_nMainID = 0;

	m_pMemDC = new CDC ;
	m_pBm = new CBitmap;
}

CUnitView::~CUnitView()
{
	delete m_pMemDC;
	delete	m_pBm;
}

void CUnitView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	m_bLayoutFlag = TRUE;
	Modify_CorpWebMenu();

	if(g_pToolTipMark) 
		g_pToolTipMark->AddTool(this, "");

	m_PreviewTool.InitPreviewTool(this);
		
	//用于定时更新行情的定时器
	SetTimer(1,GetListRefreshTime(),NULL);
	//用于自动翻页的定时器
	SetTimer(2,AutoPageTime*1000,NULL);
	//用于定时刷新行情列表的定时器,缺省是3秒
	SetTimer(3,3000,NULL);	
	//1分钟,分时,5分钟线的刷新更新时间
	SetTimer(0x1111,ELAPSE_SECOND*1000,NULL);
	SetTimer(0x1112,TICK2_SECOND*1000,NULL);
	SetTimer(0x1114,(QUEUE_SECOND*1000+500),NULL);
	//市场信息的更新定时器
	SetTimer(0x1113,MARKETINFO_SECOND*1000,NULL);
	//用于自动翻页的定时器
	SetTimer(0x2222,AutoPageTime*1000,NULL);
	//远程行情更新时间为行情表的一半
	SetTimer(0x3333,GetGPRefreshTime(),NULL);	
	//走势图的雷定时更新
	SetTimer(0x4444,50*1000,NULL);
	//走势图的即时播报标记的闪烁
	SetTimer(0x4445,1*1000,NULL);
	//用于定时全景排行的定时器
	SetTimer(0x6666,(HQRefreshTime+1)*1000,NULL);
}

//删除版面
void CUnitView::DestroyPad()
{
	int i;
	FOR_UNIT(i)
		TDEL(m_Units.pUnit[i]);
	memset(&m_Units,0,sizeof(UNITS));
}

void CUnitView::OnHistoryDrawType(UINT nID)
{
	int nPos = NORMAL_AXIS;
	BOOL bReal = g_GH.bRealUPK;
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
	{
		switch (nID)
		{
		case ID_IMAGEK:			nPos = 0;	bReal = FALSE;	break;
		case ID_REALK:			nPos = 0;	bReal = TRUE;	break;
		case ID_AMERICA:		nPos = 1;					break;
		case ID_CLOSELINE:		nPos = 2;					break;
		case ID_CLOSESTATION:	nPos = 3;					break;
		case ID_TOWLINE:		nPos = 4;					break;
		}
		g_nMainZbFlag = nPos;
		g_GH.bRealUPK = bReal;
		m_Units.pUnit[m_nF]->ProcessMsg(WM_PAINT);
	}	
}

void CUnitView::OnUpdateHistoryDrawType(CCmdUI *pCmdUI)
{
	int nPos = ID_IMAGEK;
	if(m_Units.Type[m_nF]==FXT_UNIT && m_Units.pUnit[m_nF])
	{
		switch (g_nMainZbFlag)
		{
		case 0:	
			if(g_GH.bRealUPK)	nPos = ID_REALK;
			else				nPos = ID_IMAGEK;
			break;
		case 1:		nPos = ID_AMERICA;			break;
		case 2:		nPos = ID_CLOSELINE;		break;
		case 3:		nPos = ID_CLOSESTATION;		break;
		case 4:		nPos = ID_TOWLINE;			break;
		}
		pCmdUI->SetCheck(pCmdUI->m_nID == nPos);
	}
	else
		pCmdUI->Enable(FALSE);
}

void CUnitView::OnTpAxis()
{
	g_bTPAxis = !g_bTPAxis;
	for(int i = 0;i<theViewManager.GetWindowNum();i++)
	{
		if( ((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CComView)) || ((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CComView2)) )
			((CView *)theViewManager.arViews[i])->SendMessage(UM_CALCANDPAINT);
	}
}

void CUnitView::OnUpdateTpAxis(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_Units.Type[m_nF]==ZST_UNIT);
	pCmdUI->SetCheck(g_bTPAxis);
}

void CUnitView::OnTabKey()
{
	if(FocusExistAndShow &&  (m_Units.Type[m_nF]==FXT_UNIT || m_Units.Type[m_nF]==ZST_UNIT))
		m_Units.pUnit[m_nF]->ProcessMsg(WM_KEYDOWN,VK_TAB);
}

void CUnitView::OnDrawLineTools(UINT nID)
{
	if(m_Units.Type[m_nF]==FXT_UNIT && m_Units.pUnit[m_nF])
		((UFxt*)m_Units.pUnit[m_nF])->SetSelfLine(nID);
	else if(m_Units.Type[m_nF]==ZST_UNIT && m_Units.pUnit[m_nF])
		((UZst*)m_Units.pUnit[m_nF])->SetSelfLine(nID);
}

void CUnitView::OnUpdateDrawLineTools(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_Units.Type[m_nF]==FXT_UNIT || m_Units.Type[m_nF]==ZST_UNIT);
	if(m_Units.Type[m_nF]==FXT_UNIT && m_Units.pUnit[m_nF])
	{
		UINT nID = ((UFxt*)m_Units.pUnit[m_nF])->GetSelfLine();
		if(pCmdUI->m_nID==ID_HIDESELFLINE) 
			pCmdUI->SetCheck(!g_DrawLineTool.GetShowFlag());
		else if(nID == pCmdUI->m_nID) 
			pCmdUI->SetCheck(TRUE);
		else 
			pCmdUI->SetCheck(FALSE);
 	}
	else if(m_Units.Type[m_nF]==ZST_UNIT && m_Units.pUnit[m_nF])
	{
		UINT nID = ((UZst*)m_Units.pUnit[m_nF])->GetSelfLine();
		if(pCmdUI->m_nID==ID_HIDESELFLINE) 
			pCmdUI->SetCheck(!g_DrawLineTool.GetShowFlag());
		else if(nID == pCmdUI->m_nID) 
			pCmdUI->SetCheck(TRUE);
		else 
			pCmdUI->SetCheck(FALSE);
 	}
}

void CUnitView::OnShowPeriod()
{
}

void CUnitView::OnUpdateShowPeriod(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT);
}

void CUnitView::OnShowWinNum()
{
}

void CUnitView::OnShowTQ()
{
}

void CUnitView::OnUpdateShowTQ(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT);
}

void CUnitView::OnManyDayZstMenu()
{
}

void CUnitView::OnUpdateManyDayZstMenu(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FocusExistAndShow && m_Units.Type[m_nF]==ZST_UNIT);
}

void CUnitView::OnInBlock()
{
	int tempCurrentNo = GetCurrentGPIndex();
	if(tempCurrentNo < 0)	return;
	AfxGetMainWnd()->SendMessage(UM_SEARCH_BLOCK,tempCurrentNo);
}

void CUnitView::OnGPCoolInfo()
{
	if(FocusExistAndShow && (m_Units.Type[m_nF]==FXT_UNIT || m_Units.Type[m_nF]==ZST_UNIT))
		m_Units.pUnit[m_nF]->ProcessMsg(UM_GGBWMSG,1,0);
	else if(g_pMainWin)
		((CMainFrame *)g_pMainWin)->SwitchToRestSpace(AREA_GGBW);
}

void CUnitView::OnRightAddBlock()
{
	Func_RightAddblock(GetCurrentGPIndex(),TRUE);
}

void CUnitView::OnRightAddZxg()
{
	Func_RightAddZxg(GetCurrentGPIndex(),TRUE);
	PostMessage(WM_PAINT);
}

void CUnitView::OnDelFromBlock()
{
	Func_DelFromBlock(GetCurrentGPIndex(),TRUE);
	PostMessage(WM_PAINT);
}

#include "SetTqTimeDlg.h"
void CUnitView::OnTQ(UINT nID)
{
	switch(nID)
	{
	case ID_NOTQ:
		m_nTQState = 0;
		break;
	case ID_LEFTTQ:
		if (m_nTQState == 2)	m_nTQState = 0;
		else					m_nTQState = 2;
		break;
	case ID_RIGHTTQ:
		if (m_nTQState == 1)	m_nTQState = 0;
		else					m_nTQState = 1;
		break;
	case ID_PARTTQ:
		if(m_nTQState==3)
			m_nTQState = 0;
		else
		{
			CSetTqTimeDlg dlg;
			if(dlg.DoModal()==IDOK)
				m_nTQState=3;
			else return;
		}
		break;
	}
	if(m_nTQState==2 || (m_nTQState==3 && g_nPartTq==1))
		TDX_MessageBox(m_hWnd,"注意：当前操作将进行后复权(推荐使用前复权方式).\r\n\r\n后复权时,最后的分析数据不进行盘中动态维护.",MB_OK|MB_ICONINFORMATION);
	
	int tmpTQFlag=g_nTQFlag;
	///////////////////////////
	for(int i=0; i < m_Units.Num;i++)
	{
		if(m_Units.Type[i]==FXT_UNIT && m_Units.pUnit[i] && m_Units.nStatus[i]==U_SHOW && 
			m_Units.Rect[i].Height()>0 && m_Units.Rect[i].Width()>0)
		{
			g_nTQFlag=tmpTQFlag;
			switch(nID)
			{
			case ID_NOTQ:
				m_Units.pUnit[i]->ProcessMsg(UM_TQ,0);
				break;
			case ID_LEFTTQ:
				m_Units.pUnit[i]->ProcessMsg(UM_TQ,2);
				break;
			case ID_RIGHTTQ:
				m_Units.pUnit[i]->ProcessMsg(UM_TQ,1);
				break;
			case ID_PARTTQ:
				m_Units.pUnit[i]->ProcessMsg(UM_TQ,3);
				break;
			}
		}
	}
}

void CUnitView::OnUpdateTQ(CCmdUI* pCmdUI) 
{
	BOOL OK = FALSE;
	if(m_Units.Type[m_nF]==FXT_UNIT && m_Units.pUnit[m_nF])
		OK = TRUE;
	if (!OK)
		pCmdUI->Enable(FALSE);
	else
	{
		switch(pCmdUI->m_nID)
		{
		case ID_NOTQ:
			pCmdUI->SetCheck(m_nTQState==0);
			break;
		case ID_LEFTTQ:
			pCmdUI->SetCheck(m_nTQState==2);
			break;
		case ID_RIGHTTQ:
			pCmdUI->SetCheck(m_nTQState==1);
			break;
		case ID_PARTTQ:
			pCmdUI->SetCheck(m_nTQState==3);
			break;
		}
	}
}

void CUnitView::OnPeriod(UINT nID) 
{
	int nPos = PER_DAY;
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
	{
		int nPeriod = KeyOrderToTdxPeriod(nID-ID_MIN1);
		m_Units.pUnit[m_nF]->ProcessMsg(UM_SETPERIOD, nPeriod);
	}
}

void CUnitView::OnUpdatePeriod(CCmdUI* pCmdUI) 
{
	if(m_nF>=0 && m_Units.Type[m_nF]==FXT_UNIT && m_Units.pUnit[m_nF])
	{
		int nPeriod = ((UFxt*)m_Units.pUnit[m_nF])->GetCurrPeriod();
		int nPos = ID_MIN1+TdxPeriodToKeyOrder(nPeriod);
		pCmdUI->SetCheck(pCmdUI->m_nID == nPos);
	}
	else
		pCmdUI->Enable(FALSE);
}

void CUnitView::OnSpecPeriod(UINT nID) 
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
	{
		((UFxt*)m_Units.pUnit[m_nF])->SetSpecProcDraw(nID-ID_FXTSPEC0);
		((UFxt*)m_Units.pUnit[m_nF])->ProcessMsg(WM_PAINT,1);
	}
}

void CUnitView::OnUpdateSpecPeriod(CCmdUI* pCmdUI) 
{
	if(m_Units.Type[m_nF]==FXT_UNIT && m_Units.pUnit[m_nF])
	{
		int nSpecType = ((UFxt*)m_Units.pUnit[m_nF])->GetSpecProcDraw();
		pCmdUI->SetRadio(pCmdUI->m_nID == nSpecType+ID_FXTSPEC0);
	}
	else
		pCmdUI->Enable(FALSE);
}

void CUnitView::OnWindowNum(UINT nID) 
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
	{
		int No=3;
		switch(nID)
		{
		case CHART_1:	No=1;	break;
		case CHART_2:	No=2;	break;
		case CHART_3:	No=3;	break;
		case CHART_4:	No=4;	break;
		case CHART_5:	No=5;	break;
		case CHART_6:	No=6;	break;
		case CHART_7:	No=7;	break;
		case CHART_8:	No=8;	break;
		case CHART_9:	No=9;	break;
		case CHART_10:	No=10;	break;
		}
		m_Units.pUnit[m_nF]->ProcessMsg(UM_SELNUM,No);
	}
}

void CUnitView::OnUpdateWindowNum(CCmdUI* pCmdUI) 
{
	if(m_nF>=0 && m_Units.Type[m_nF]==FXT_UNIT && m_Units.pUnit[m_nF])
	{
		int nPos=CHART_3;		
		int nCount = ((UFxt*)m_Units.pUnit[m_nF])->GetWindowNum();
		switch (nCount)
		{
		case 1:	nPos = CHART_1; break;
		case 2:	nPos = CHART_2;	break;
		case 3:	nPos = CHART_3;	break;
		case 4:	nPos = CHART_4;	break;
		case 5:	nPos = CHART_5;	break;
		case 6:	nPos = CHART_6;	break;
		case 7:	nPos = CHART_7;break;
		case 8:	nPos = CHART_8;break;
		case 9:	nPos = CHART_9;break;
		case 10:nPos = CHART_10;break;
		}
		pCmdUI->SetCheck(pCmdUI->m_nID == nPos);
	}
	else
		pCmdUI->Enable(FALSE);
}

void CUnitView::OnFxtAxis(UINT nID) 
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
	{
		int nPos = NORMAL_AXIS;
		switch (nID)
		{
		case ID_NORMAL:		nPos = NORMAL_AXIS;		break;
		case ID_SAMESCALE:	nPos = SAMESCALE_AXIS;	break;
		case ID_SAMESECT:	nPos = SAMESECT_AXIS;	break;
		case ID_BFB:		nPos = R100_AXIS;		break;
		case ID_LOG:		nPos = LOG10_AXIS;		break;
		case ID_GOLDEN:		nPos = GOLDEN_AXIS;		break;
		}
		m_Units.pUnit[m_nF]->ProcessMsg(UM_SETAXIS, nPos);
		g_nMainAxis=nPos;
	}	
}

void CUnitView::OnUpdateFxtAxis(CCmdUI* pCmdUI) 
{
	if(m_nF>=0 && m_Units.Type[m_nF]==FXT_UNIT && m_Units.pUnit[m_nF])
	{
		int nPos = ID_NORMAL;	
		int nCount = ((UFxt*)m_Units.pUnit[m_nF])->GetAsixType();
		switch (nCount)
		{
		case NORMAL_AXIS:	nPos = ID_NORMAL;break;
		case SAMESCALE_AXIS:nPos = ID_SAMESCALE;break;
		case SAMESECT_AXIS:	nPos = ID_SAMESECT;break;
		case R100_AXIS:		nPos = ID_BFB;   break;
		case LOG10_AXIS:	nPos = ID_LOG;   break;
		case GOLDEN_AXIS:	nPos = ID_GOLDEN;   break;
		}
		pCmdUI->SetCheck(pCmdUI->m_nID == nPos);
	}
	else
		pCmdUI->Enable(FALSE);
}

void CUnitView::OnShow(UINT nID) 
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		m_Units.pUnit[m_nF]->ProcessMsg(UM_SELECTMANY, nID);
}

void CUnitView::OnUpdateShow(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_nF>=0 && m_Units.Type[m_nF]==FXT_UNIT);	
}

void CUnitView::OnOverlap() 
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		m_Units.pUnit[m_nF]->ProcessMsg(UM_OVERLAP);
	if(FocusExistAndShow && m_Units.Type[m_nF]==ZST_UNIT)
		m_Units.pUnit[m_nF]->ProcessMsg(UM_OVERLAP);
}

void CUnitView::OnAutoOverlap(UINT nID)
{
	int nXH = -1;
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		nXH = 0;
	if(FocusExistAndShow && m_Units.Type[m_nF]==ZST_UNIT)
		nXH = 1;
	UINT nPreID = g_Overlap[nXH].nAutoType;
	if(nXH>=0)
	{
		if(nID==ID_AUTOVERIDX&&!(g_Overlap[nXH].nAutoType&AUTO_OVERLAP_IDX)) 
			g_Overlap[nXH].nAutoType |= AUTO_OVERLAP_IDX;
		else if(nID==ID_AUTOVERIDX&&(g_Overlap[nXH].nAutoType&AUTO_OVERLAP_IDX)) 
			g_Overlap[nXH].nAutoType ^= AUTO_OVERLAP_IDX;
		if(nID==ID_AUTOVERMG&&!(g_Overlap[nXH].nAutoType&AUTO_OVERLAP_MG)) 
			g_Overlap[nXH].nAutoType |= AUTO_OVERLAP_MG;
		else if(nID==ID_AUTOVERMG&&(g_Overlap[nXH].nAutoType&AUTO_OVERLAP_MG)) 
			g_Overlap[nXH].nAutoType ^= AUTO_OVERLAP_MG;

		if(nXH==0&&nPreID!=0&&g_Overlap[nXH].nAutoType==0)		//去掉了全部自动叠加
			m_Units.pUnit[m_nF]->ProcessMsg(UM_UNOVERLAP);
		else m_Units.pUnit[m_nF]->ProcessMsg(UM_AUTOVERLAP);
	}
}

void CUnitView::OnUpdateAutoOverlap(CCmdUI* pCmdUI)
{
	int nXH = -1;
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		nXH = 0;
	if(FocusExistAndShow && m_Units.Type[m_nF]==ZST_UNIT)
		nXH = 1;
	if(nXH>=0&&pCmdUI->m_nID==ID_AUTOVERIDX) pCmdUI->SetCheck(g_Overlap[nXH].nAutoType&AUTO_OVERLAP_IDX);
	if(nXH>=0&&pCmdUI->m_nID==ID_AUTOVERMG) pCmdUI->SetCheck(g_Overlap[nXH].nAutoType&AUTO_OVERLAP_MG);
}

void CUnitView::OnUnOverlap() 
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		m_Units.pUnit[m_nF]->ProcessMsg(UM_UNOVERLAP);
	if(FocusExistAndShow && m_Units.Type[m_nF]==ZST_UNIT)
		m_Units.pUnit[m_nF]->ProcessMsg(UM_UNOVERLAP);
}

void CUnitView::OnZbOverlap() 
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		m_Units.pUnit[m_nF]->ProcessMsg(UM_SELECTMANY, ID_ZB_OVERLAP);
}

void CUnitView::OnChangeindexparam() 
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		m_Units.pUnit[m_nF]->ProcessMsg(UM_ADJUSTPARAM);
}

void CUnitView::OnMenuZb(UINT nID) 
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		m_Units.pUnit[m_nF]->ProcessMsg(UM_MENUZB, nID);
}

void CUnitView::OnUpdateMenuZb(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nF>=0 && m_Units.Type[m_nF]==FXT_UNIT);
}

void CUnitView::OnHxZbDlg()
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		m_Units.pUnit[m_nF]->ProcessMsg(UM_HXZBDLG);
}

void CUnitView::OnColor() 
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		m_Units.pUnit[m_nF]->ProcessMsg(UM_CHANGECOLOR);
}

void CUnitView::OnChangeParam(WPARAM wParam, LPARAM lParam)
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		m_Units.pUnit[m_nF]->ProcessMsg(UM_CHANGEPARAM);
}

void CUnitView::OnModifyFormula() 
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		m_Units.pUnit[m_nF]->ProcessMsg(UM_MODIFYFORMULA);
}

void CUnitView::OnDelzb() 
{
	SendMessage(WM_KEYDOWN, VK_DELETE, 0);
}

void CUnitView::OnIndexhelp() 
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		m_Units.pUnit[m_nF]->ProcessMsg(UM_INDEXHELP);
}

void CUnitView::OnUpdateDeleteshow(CCmdUI* pCmdUI) 
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		pCmdUI->Enable(((UFxt*)m_Units.pUnit[m_nF])->ExistZS());
	else
		pCmdUI->Enable(FALSE);
}

void CUnitView::OnDeleteshow() 
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		m_Units.pUnit[m_nF]->ProcessMsg(UM_DELZS);
}

void CUnitView::OnHisDlg()
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
	{
		m_bAutoPage = FALSE;
		m_Units.pUnit[m_nF]->ProcessMsg(UM_HISDLG,1); //强制选中一K线
	}
}

void CUnitView::OnUpdateHisDlg(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_nF>=0 && m_Units.Type[m_nF]==FXT_UNIT);
}

void CUnitView::OnQjStat()
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT) 
		m_Units.pUnit[m_nF]->ProcessMsg(UM_QJSTAT);
	else if(FocusExistAndShow && m_Units.Type[m_nF]==ZST_UNIT) 
		m_Units.pUnit[m_nF]->ProcessMsg(UM_POPZSTICKDLG);
}

void CUnitView::OnAllQjStat()
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		m_Units.pUnit[m_nF]->ProcessMsg(UM_QJSTAT,1);
	else if(FocusExistAndShow && m_Units.Type[m_nF]==ZST_UNIT) 
		m_Units.pUnit[m_nF]->ProcessMsg(UM_POPZSTICKDLG,1);
}

void CUnitView::OnManyDayZst(UINT nID)
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==ZST_UNIT)
	{
		//在数据没有到来之前,不要再进行多日分时,否则容易出现问题
		if(!OnLine || ((UZst *)(m_Units.pUnit[m_nF]))->GetDataReady()) 
		{
			SetMainID();
			if(nID==ID_MANYDAYZST0)
			{
				g_bManyDayZST = FALSE;
				g_nZSTManyDay = 1;
			}
			else
			{
				g_bManyDayZST = TRUE;
				g_nZSTManyDay = nID-ID_MANYDAYZST0;
			}
			m_Units.pUnit[m_nF]->ProcessMsg(UM_MANYDAYZST);
		}
	}
}

void CUnitView::OnUpdateManyDayZst(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FocusExistAndShow && m_Units.Type[m_nF]==ZST_UNIT);
	if(pCmdUI->m_nID == ID_MANYDAYZST0)
		pCmdUI->SetCheck(!g_bManyDayZST);
	else
		pCmdUI->SetCheck(g_bManyDayZST && g_nZSTManyDay==pCmdUI->m_nID-ID_MANYDAYZST0);
}

void CUnitView::OnManyDayTickZst(UINT nID)
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==TICKZST_UNIT)
	{	
		if(nID==ID_MANYDAYTICKZST0)
		{
			g_bManyDayZST = FALSE;
			g_nZSTManyDay = 1;
		}
		else
		{
			g_bManyDayZST = TRUE;
			g_nZSTManyDay = nID-ID_MANYDAYTICKZST0;
		}
		m_Units.pUnit[m_nF]->ProcessMsg(UM_MANYDAYZST);
	}
}

void CUnitView::OnUpdateManyDayTickZst(CCmdUI *pCmdUI)
{
	if(pCmdUI->m_nID == ID_MANYDAYTICKZST0)
		pCmdUI->SetCheck(!g_bManyDayZST);
	else
		pCmdUI->SetCheck(g_bManyDayZST && g_nZSTManyDay==pCmdUI->m_nID-ID_MANYDAYTICKZST0);
}

void CUnitView::OnTickZstUndo()
{
	if(FocusExistAndShow && m_Units.Type[m_nF]==TICKZST_UNIT)
		m_Units.pUnit[m_nF]->ProcessMsg(UM_TICKZSTUNDO);
}

void CUnitView::OnTickZst2Zst()
{
	((CMainFrame *)(AfxGetApp()->m_pMainWnd))->OnCommCommand(CMD_COM,(g_bTickTStatus?501:508));
}

void CUnitView::OnDeloverlapgp()
{
	if(m_nF>=0 && m_Units.Type[m_nF]==FXT_UNIT && m_Units.pUnit[m_nF])
		SendMessage(WM_KEYDOWN, VK_DELETE, 0);
}

void CUnitView::OnAutopage() 
{
	if(!m_bAutoPage && !SureAutoPage(this))
		return;		
	m_bAutoPage = !m_bAutoPage;
	int i;
	FOR_UNIT(i)
	{
		if(m_Units.Type[i]!=HQGRID_UNIT||
			!TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT))
			continue;
		HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[i];
		pHQGrid->ProcessMsg(WM_COMMAND,ID_AUTOPAGE);
	}
}

void CUnitView::OnUpdateAutopage(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bAutoPage);
}

void CUnitView::OnKillFocus(CWnd* pNewWnd) 
{
	CView::OnKillFocus(pNewWnd);
}

void CUnitView::OnDeleteInfoDlg(WPARAM wParam,LPARAM lParam)
{
	if(lParam>0) m_nF = lParam-1;
	if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
  		m_Units.pUnit[m_nF]->ProcessMsg(UM_DELETE_INFODLG,wParam,0);
}

void CUnitView::SetMainID()
{
	if(OnLine)
	{
		g_pComm->SetSendTop(0);
		MainIDPlus(m_nMainID,WindowMacro);		
		KillTimer(0x3333);//换版面,重设定时器
		SetTimer(0x3333,GetGPRefreshTime(),NULL);
	}
}

void CUnitView::GetMainID(WPARAM wParam,LPARAM lParam)	//采用消息的方式，直接填写g_nMainID
{
	if(m_nMainID == 0)	SetMainID();
	g_nMainID = m_nMainID;		
}

void CUnitView::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	if(wParam==1)
	{
		if(JUSTCURRHQ==g_AnsHeader3.AssisID)
		{
			MULIDX MulHQData[MULHQ_WANTNUM]={0};
			short hqnum = g_pGetData->DSGetMulCurrDataACK(MulHQData, MULHQ_WANTNUM);
			int i,k;
			for(k = 0;k < hqnum;k++)
			{
				MemStkInfo *pStkInfo = g_pStockIO->GetDSStkInfo(MulHQData[k].Market,MulHQData[k].Code);
				if(!pStkInfo) continue;
				for(i = 0;i<theViewManager.GetWindowNum();i++)
				{
					CView *tempView = (CView *)theViewManager.arViews[i];
					if(tempView->IsKindOf(RUNTIME_CLASS(CComView)) || tempView->IsKindOf(RUNTIME_CLASS(CComView2)))
						tempView->SendMessage(ASYN_HQ_MESSAGE,pStkInfo->Index);
				}
			}
		}
		else if(m_nMainID==g_AnsHeader3.MainID)
		{
			short whichunit,whichreq;
			GetComAssisID(g_AnsHeader3.AssisID,whichunit,whichreq);
			if(whichunit >= 0 && whichunit < m_Units.Num && m_Units.nStatus[whichunit]==U_SHOW && m_Units.pUnit[whichunit])
			{
				if(m_Units.Type[whichunit]==HQGRID_UNIT) 
					return;
				m_Units.pUnit[whichunit]->ProcessMsg(UM_GETDATA_ACK,whichreq);
			}
		}
	}
	else
	{
		if(JUSTCURRHQ==g_AnsHeader.AssisID)
		{
			memset(m_MulHQData,0,MULHQ_WANTNUM*sizeof(CurrStockData_Code));
			short hqnum = g_pGetData->GetMultiCurrStockDataACK(m_MulHQData,MULHQ_WANTNUM);
			int i,k,iIndex;
			for(k = 0;k < hqnum;k++)
			{
				iIndex = g_pStockIO->GetIndex(m_MulHQData[k].Code,m_MulHQData[k].SetCode);
				for(i = 0;i<theViewManager.GetWindowNum();i++)
				{
					CView *tempView = (CView *)theViewManager.arViews[i];
					if(tempView->IsKindOf(RUNTIME_CLASS(CComView)) || tempView->IsKindOf(RUNTIME_CLASS(CComView2)))
						tempView->SendMessage(ASYN_HQ_MESSAGE,iIndex);
				}
			}
		}
		else if (m_nMainID==g_AnsHeader.MainID)
		{
			short whichunit,whichreq;
			GetComAssisID(g_AnsHeader.AssisID,whichunit,whichreq);
			if(whichunit >= 0 && whichunit < m_Units.Num && m_Units.nStatus[whichunit]==U_SHOW && m_Units.pUnit[whichunit])
			{
				if(m_Units.Type[whichunit]==HQGRID_UNIT) 
					return;
				m_Units.pUnit[whichunit]->ProcessMsg(UM_GETDATA_ACK,whichreq);
			}
		}
	}
}

void CUnitView::OnDestroy()
{
	m_bQuitting	= TRUE;
	CView::OnDestroy();
}

BOOL CUnitView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

BOOL CUnitView::PreCreateWindow(CREATESTRUCT& cs) 
{
	LPCTSTR wndcls = AfxRegisterWndClass(CS_VREDRAW|CS_HREDRAW|CS_BYTEALIGNCLIENT|CS_BYTEALIGNWINDOW|CS_DBLCLKS);
	cs.lpszClass = wndcls;
	return CView::PreCreateWindow(cs);
}

LRESULT CUnitView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	int i;
	switch(message)
	{
	case UM_TABSWITCHCHG:
		if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		{
			m_Units.pUnit[m_nF]->ProcessMsg(message,wParam,lParam);
			break;
		}
		break;
	case UM_CHANGEZB:
	case UM_CHANGESPEZB:
		if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		{
			m_Units.pUnit[m_nF]->ProcessMsg(message,wParam,lParam);
			break;
		}
		FOR_UNIT(i)
		{
			if(m_Units.nStatus[i]==U_HIDE) continue;
			if(m_Units.Type[i]==FXT_UNIT&&m_Units.pUnit[i])
				m_Units.pUnit[i]->ProcessMsg(message,wParam,lParam);
		}
		break;
	case UM_DYNCHGPARAM:
		if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		{
			m_Units.pUnit[m_nF]->ProcessMsg(UM_DYNCHGPARAM);
			break;
		}
		FOR_UNIT(i)
		{
			if(m_Units.nStatus[i]==U_HIDE) continue;
			if(m_Units.Type[i]==FXT_UNIT&&m_Units.pUnit[i])
				m_Units.pUnit[i]->ProcessMsg(UM_DYNCHGPARAM);
		}
		break;
	case UM_AFTERFORMULA:
		if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		{
			m_Units.pUnit[m_nF]->ProcessMsg(UM_AFTERFORMULA);
			break;
		}
		FOR_UNIT(i)
		{
			if(m_Units.nStatus[i]==U_HIDE) continue;
			if(m_Units.Type[i]==FXT_UNIT&&m_Units.pUnit[i])
				m_Units.pUnit[i]->ProcessMsg(UM_AFTERFORMULA);
		}
		break;
	case UM_DRAGZBTREE:
		if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		{
			m_Units.pUnit[m_nF]->ProcessMsg(UM_DRAGZBTREE,wParam,lParam);
			break;
		}
		FOR_UNIT(i)
		{
			if(m_Units.nStatus[i]==U_HIDE) continue;
			if(m_Units.Type[i]==FXT_UNIT&&m_Units.pUnit[i])
				m_Units.pUnit[i]->ProcessMsg(UM_DRAGZBTREE,wParam,lParam);
		}
		break;
	case WM_TREEZS_MESSAGE:
		if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		{
			m_Units.pUnit[m_nF]->ProcessMsg(UM_CHANGEZS,wParam,lParam);
			break;
		}
		FOR_UNIT(i)
		{
			if(m_Units.nStatus[i]==U_HIDE) continue;
			if(m_Units.Type[i]==FXT_UNIT&&m_Units.pUnit[i])
				m_Units.pUnit[i]->ProcessMsg(UM_CHANGEZS,wParam,lParam);
		}
		break;
	case UM_GET_FXTDATE:
		if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
			return m_Units.pUnit[m_nF]->ProcessMsg(UM_GET_FXTDATE,wParam,lParam);
		return 0;
	case UM_CALCANDPAINT:
		FOR_UNIT(i)
			if(m_Units.pUnit[i] && m_Units.nStatus[i]==U_SHOW) m_Units.pUnit[i]->ProcessMsg(UM_CALCANDPAINT);
		break;
	case ASYN_HQ_MESSAGE:
		FOR_UNIT(i)
			if(m_Units.pUnit[i]) m_Units.pUnit[i]->ProcessMsg(ASYN_HQ_MESSAGE,wParam);
		break;
	case ASYN_STATHQ_MESSAGE:
		FOR_UNIT(i)
			if(m_Units.pUnit[i]) m_Units.pUnit[i]->ProcessMsg(ASYN_STATHQ_MESSAGE,wParam);
		break;
	case UM_HSCROLLBAR:
	case UM_VSCROLLBAR:
		if(lParam>=0 && lParam<m_Units.Num)
			m_Units.pUnit[lParam]->ProcessMsg(message,wParam,lParam);
	default:
		break;
	}
	return CView::WindowProc(message, wParam, lParam);
}

BOOL CUnitView::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F8 )
	{
		if(FocusExistAndShow && m_Units.Type[m_nF]==FXT_UNIT)
		{
			((UFxt*)m_Units.pUnit[m_nF])->ProcessMsg(WM_KEYDOWN,VK_F8);
			return TRUE;
		}	
	}
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam==VK_BACK)
	{
		PostMessage(WM_COMMAND,IDC_COMGOBACK);
		return 1;
	}
	if(pMsg->message == WM_SYSKEYDOWN && ((pMsg->wParam>=0x31 && pMsg->wParam<=0x39) || pMsg->wParam==0x30) )
	{
		if(m_Units.Type[m_nF]==ZST_UNIT && m_Units.pUnit[m_nF])
		{
			if(pMsg->wParam==0x30)
				PostMessage(WM_COMMAND,ID_MANYDAYZST9);		
			else
				PostMessage(WM_COMMAND,ID_MANYDAYZST0+pMsg->wParam-0x31);		
			return TRUE;
		}
		else if(m_Units.Type[m_nF]==TICKZST_UNIT && m_Units.pUnit[m_nF])
		{
			if(pMsg->wParam==0x30)
				PostMessage(WM_COMMAND,ID_MANYDAYTICKZST9);		
			else
				PostMessage(WM_COMMAND,ID_MANYDAYTICKZST0+pMsg->wParam-0x31);
			return TRUE;
		}
	}
	if (pMsg->message == WM_CHAR && pMsg->wParam == '-')
	{
		OnAutopage();
		return TRUE;
	}
	if (pMsg->message == WM_CHAR && KeyGuyFlag == FALSE )
	{
		if(ProcessKeyGuy(pMsg,this,KeyGuyFlag))
			return TRUE;
	}
	if(pMsg->message==WM_KEYDOWN && (pMsg->wParam=='z'||pMsg->wParam=='Z'))
	{
		SHORT CtrlDown=::GetKeyState(VK_CONTROL);
		if(CtrlDown<0)
		{
			if(::GetKeyState(VK_SHIFT)<0)
				PostMessage(WM_COMMAND,ID_DELFROMBLOCK);
			else
				PostMessage(WM_COMMAND,ID_RIGHT_ADDBLOCK);
			return TRUE;
		}
	}
	if(pMsg->message==WM_KEYDOWN && (pMsg->wParam=='v'||pMsg->wParam=='V'))
	{
		SHORT CtrlDown=::GetKeyState(VK_CONTROL);
		if(CtrlDown<0)
		{
			PostMessage(WM_COMMAND,ID_RIGHTTQ);
			return TRUE;
		}
	}
	if (pMsg->message>=UM_BK_START && pMsg->message<=UM_BK_END)
	{
		int i;
		FOR_UNIT(i)
		{
			if( (m_Units.Type[i]!=HQGRID_UNIT && m_Units.Type[i]!=RMREPORT_UNIT && m_Units.Type[i]!=GLHQ_UNIT && m_Units.Type[i]!=GPPAGE_UNIT) ||
				!TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT))
				continue;
			if(m_Units.Type[i]==RMREPORT_UNIT && pMsg->message==UM_BK_RMHQ_OK)
			{
				UBaseInner* pInner = (UBaseInner*)m_Units.pUnit[i];
				pInner->ProcessMsg(pMsg->message,pMsg->wParam,pMsg->lParam);
			}
			else if(m_Units.Type[i]==GLHQ_UNIT && pMsg->message==UM_BK_GLHQ_OK)
			{
				UGlHq* pGlHq = (UGlHq*)m_Units.pUnit[i];
				pGlHq->ProcessMsg(pMsg->message,pMsg->wParam,pMsg->lParam);
			}
			else if(m_Units.Type[i]==HQGRID_UNIT)
			{
				HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[i];
				pHQGrid->ProcessMsg(pMsg->message,pMsg->wParam,pMsg->lParam);
			}
			else if(m_Units.Type[i]==GPPAGE_UNIT)
			{
				UWeb* pWeb = (UWeb*)m_Units.pUnit[i];
				pWeb->ProcessMsg(pMsg->message,pMsg->wParam,pMsg->lParam);
			}
		}
		return TRUE;
	}
	if(g_pToolTipMark) 
		g_pToolTipMark->RelayEvent(pMsg);
	if(m_PreviewTool.m_hWnd!=NULL)  
		m_PreviewTool.RelayEvent(pMsg);
	int i = 0;
	FOR_UNIT(i)
	{	
		if(m_Units.pUnit[i] && m_Units.Type[i]==FXT_UNIT)
			((UFxt*)m_Units.pUnit[i])->m_ToolTip.SendMessage(TTM_RELAYEVENT, 0, (LPARAM)pMsg);
		else if(m_Units.pUnit[i] && m_Units.Type[i]==ZST_UNIT)
		{
			((UZst*)m_Units.pUnit[i])->m_ToolTip.SendMessage(TTM_RELAYEVENT, 0, (LPARAM)pMsg);
			((UZst*)m_Units.pUnit[i])->m_LevinToolTip.SendMessage(TTM_RELAYEVENT, 0, (LPARAM)pMsg);
		}
		else if(m_Units.pUnit[i] && m_Units.Type[i]==SEAT_UNIT)
			((USeat*)m_Units.pUnit[i])->m_ToolTip.SendMessage(TTM_RELAYEVENT, 0, (LPARAM)pMsg);
		else if(m_Units.pUnit[i] && m_Units.Type[i]==HQ_UNIT || m_Units.pUnit[i] && m_Units.Type[i]==LOWHQ_UNIT)
			((UHq*)m_Units.pUnit[i])->m_ToolTip.SendMessage(TTM_RELAYEVENT, 0, (LPARAM)pMsg);		
	}
	return CView::PreTranslateMessage(pMsg);
}

//////////////////////////////////////////////////////////////////////////
//打印
BOOL CUnitView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	if(g_d.IsBlackColorScheme(VipColor.BACKCOLOR))
	{
		if(TDX_MessageBox (m_hWnd,"背景太暗,建议将系统背景色调节白底再打印,要强制打印吗?",MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL)
			return FALSE;
	}
	PRINTDLG pd; //该结构包含打印对话框中的所有信息 
	LPDEVMODE lpDevMode; 
	if(AfxGetApp()->GetPrinterDeviceDefaults(&pd)) //获得默认的打印机的信息 
	{ 
		lpDevMode=(LPDEVMODE)GlobalLock(pd.hDevMode); 
		if(lpDevMode) 
			lpDevMode->dmOrientation=DMORIENT_LANDSCAPE; //将打印机设置为横向打印。 
		GlobalUnlock(pd.hDevMode); 
	}
	return DoPreparePrinting(pInfo);
}

void CUnitView::OnFilePrintPreview()
{
	UpdateWindow();	
	CView::OnFilePrintPreview();
}

void CUnitView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{	
	CRect rect;
	GetClientRect(rect);
	CClientDC dc(this);
	SendMessage(UM_REPAINTCOM_MESSAGE);
	if (m_pMemDC->GetSafeHdc()) m_pMemDC->DeleteDC();
	m_pMemDC->CreateCompatibleDC(&dc);	
	m_pMemDC->SetMapMode(MM_ANISOTROPIC);
	m_pMemDC->SetWindowExt(dc.GetDeviceCaps(HORZRES),dc.GetDeviceCaps(VERTRES));
	m_pMemDC->SetViewportExt(m_pMemDC->GetDeviceCaps(HORZRES),m_pMemDC->GetDeviceCaps(VERTRES));

	if (m_pBm->GetSafeHandle()) m_pBm->DeleteObject();
	m_pBm->CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());
	m_pMemDC->SelectObject(m_pBm);
	dc.DPtoLP(rect);	//Convert to Logical Coordinates
	m_PrintRect= rect;  //Save Logical Coordinates
	m_pMemDC->BitBlt(0,0,rect.Width(),rect.Height(),&dc,0,0,SRCCOPY);
	g_d.SelectPen(m_pMemDC,LEVEL_PEN);
	g_d.DrawRect(m_pMemDC,1,1,rect.Width()-1,rect.Height()-1);
}

void CUnitView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	int nPrintCx = pDC->GetDeviceCaps(HORZRES);
	int nPrintCy = pDC->GetDeviceCaps(VERTRES);
	CClientDC dc(this);
	int nScreenCx = dc.GetDeviceCaps(HORZRES);
	int nScreenCy = dc.GetDeviceCaps(VERTRES);
	float fRatio = (float)nPrintCy/(float)nPrintCx;
	nScreenCx = nScreenCy/fRatio;
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(nScreenCx, nScreenCy);
	pDC->SetViewportExt(nPrintCx, nPrintCy);	
	pDC->StretchBlt(0,0,nScreenCx,nScreenCy,m_pMemDC,0,0,m_PrintRect.Width(),m_PrintRect.Height(),SRCCOPY);
}

void CUnitView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{	
	CView::OnEndPrinting(pDC, pInfo);
}

void CUnitView::OnCancelMark()
{
	int nCurGpIndex = GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	MarkStockFlag(tempStkInfo, 0);
}

#include "TipMarkDlg.h"
void CUnitView::OnMarkStockFlag(UINT nID)
{
	int i, nFlag = nID - ID_MARK_ADD1+1;
	if(nFlag<=0||nFlag>10) return;
	int nCurGpIndex = GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	
	if(nFlag==MAX_GROUPMARK)
	{
		CTipMarkDlg dlg;
		dlg.m_pStock = tempStkInfo;
		if(dlg.DoModal()==IDOK)
			MarkStockTipFlag(tempStkInfo, dlg.m_strTip);
	}
	else MarkStockFlag(tempStkInfo, (char)nFlag);
	
	FOR_UNIT(i)
	{
		if(m_Units.Type[i]==HQ_UNIT) 
			m_Units.pUnit[i]->ProcessMsg(WM_PAINT);
	}
}

void CUnitView::OnUpdateMarkStockFlag(CCmdUI* pCmdUI)
{
	char nFlag = pCmdUI->m_nID - ID_MARK_ADD1+1;
	if(nFlag<=0||nFlag>10) return;
	int nCurGpIndex = GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	pCmdUI->SetCheck(nFlag==tempStkInfo->fl_flag);
}

void CUnitView::GetOverlapCodeHead(int nWhich,char *CodeHead,int &nCount,BOOL bDSCode)
{	
	//叠加的股票
	int nCodeSize = bDSCode?CODE_SIZE:SH_CODE_LEN;
	if(m_Units.Type[nWhich]==ZST_UNIT)
	{
		UZst *pZst = (UZst*)m_Units.pUnit[nWhich];
		for(int i=0;pZst && i<pZst->GetOverlapNum();i++)
		{
			if(nCount>=MULHQ_WANTNUM) break;
			MemStkInfo *tmpInfo = pZst->GetOverlapMemStkInfo(i);
			if(!tmpInfo) 
				continue;
			if(bDSCode&&(tmpInfo->setcode==SH||tmpInfo->setcode==SZ)) 
				continue;
			else if(!bDSCode&&tmpInfo->setcode!=SH&&tmpInfo->setcode!=SZ) 
				continue;
			memcpy(CodeHead+nCount*(1+nCodeSize),&tmpInfo->setcode,1);
			memcpy(CodeHead+nCount*(1+nCodeSize)+1,tmpInfo->Code,nCodeSize);
			nCount++;
		}
	}
}

void CUnitView::OnGlHqSwitch(UINT nID)
{
	int iNo = nID - ID_GLHQ_SWITCH1;
	if(m_nF>=0 && m_Units.Type[m_nF]==GLHQ_UNIT && m_Units.pUnit[m_nF])
		m_Units.pUnit[m_nF]->ProcessMsg(UM_GLHQ_SWITCH,iNo);
}

void CUnitView::GetQZRelationCodeHead(int nGP,char *CodeHead,int &nCount)
{	
	struct MemStkInfo *pStkInfo = (*g_pStockIO)[nGP];
	if(!pStkInfo || pStkInfo->setcode>=2)
		return;
	//权证关联股行情
	int Type=GetStockType(pStkInfo->setcode,pStkInfo->Code);
	if(Type==CODE_SZQZ || Type==CODE_SHQZ)
	{
		STKRELATION  LinkData[100] = {0};
		short nLinkNum = g_pStockIO->GetRLTData(pStkInfo, LinkData, 100);
		for(int i=0;i<nLinkNum;i++)
		{
			if(nCount>=MULHQ_WANTNUM) break;
			if(strlen(LinkData[i].Code)<1) continue;
			MemStkInfo *pInfo0 = g_pStockIO->Get_MemStkInfo(LinkData[i].Code, LinkData[i].SetCode);
			if(!pInfo0) continue;
			Type=GetStockType(pInfo0->setcode,pInfo0->Code);
			if(Type==CODE_SHAG||Type==CODE_SZAG||Type==CODE_ZXB||Type==CODE_CYB)
			{
				memcpy(CodeHead+nCount*(1+SH_CODE_LEN),&pInfo0->setcode,1);
				memcpy(CodeHead+nCount*(1+SH_CODE_LEN)+1,pInfo0->Code,SH_CODE_LEN);
				nCount++;
				break;
			}
		}
	}
}

void CUnitView::OnChooseFP(UINT nID)
{
	int nFlag = nID - ID_FMLPACK00;
	if(nFlag<0||nFlag>=g_nFmlPack) return;
	if(m_nF>=0 && m_Units.Type[m_nF]==FXT_UNIT && m_Units.pUnit[m_nF])
	{
		m_Units.pUnit[m_nF]->ProcessMsg(UM_USEFORMULAPACK,nFlag,1);
		m_Units.pUnit[m_nF]->ProcessMsg(WM_PAINT);
	}
}

void CUnitView::OnUpdateChooseFP(CCmdUI *pCmdUI)
{
}

void CUnitView::OnAddFmlPack()
{
	if(m_nF>=0 && m_Units.Type[m_nF]==FXT_UNIT && m_Units.pUnit[m_nF])
		m_Units.pUnit[m_nF]-> ProcessMsg(UM_ADDFORMULAPACK);
}

void CUnitView::OnFmlPack2Stk()
{
	int i=0;
	OnFmlPackUnlock();
	if(g_nFml2Stk>=MAX_BANDSTOCK_NUM)
	{
		CString tmpInfo;
		tmpInfo.Format("最多允许将%d只股票绑定到模板!\r\n请使用模板管理功能整理!",MAX_BANDSTOCK_NUM);
		TDX_MessageBox(m_hWnd,tmpInfo,MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	int nCurGpIndex = GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	CFormulaPackDlg dlg;
	dlg.SetMode(FALSE,TRUE);
	dlg.SetStock(tempStkInfo);
	if(dlg.DoModal()==IDOK)
	{
		for(i=0;i<g_nFmlPack;i++)
		{
			if(stricmp(g_Fml2Stk[g_nFml2Stk-1].aPackName,g_FmlPack[i].aPackName)==0)
				break;
		}
		if(i<g_nFmlPack && m_nF>=0 && m_Units.Type[m_nF]==FXT_UNIT && m_Units.pUnit[m_nF])
		{
			m_Units.pUnit[m_nF]->ProcessMsg(UM_USEFORMULAPACK,i);
			m_Units.pUnit[m_nF]->ProcessMsg(WM_PAINT);
		}
	}
}

void CUnitView::OnFmlPackUnlock()
{
	int i = 0;
	int nFml2Stk = 0;
	int nCurGpIndex = GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	for(i=0;i<g_nFml2Stk;i++)
	{
		if(tempStkInfo->setcode==g_Fml2Stk[i].setcode
			&& strcmp(tempStkInfo->Code,g_Fml2Stk[i].code)==0) continue;
		if(nFml2Stk<i) memcpy(&(g_Fml2Stk[nFml2Stk]),&(g_Fml2Stk[i]),sizeof(FML2STK));
		nFml2Stk++;
	}
	g_nFml2Stk = nFml2Stk;
	if(g_nFml2Stk<MAX_BANDSTOCK_NUM) 
		memset(&(g_Fml2Stk[g_nFml2Stk]),0,(MAX_BANDSTOCK_NUM-g_nFml2Stk)*sizeof(FML2STK));
}

void CUnitView::OnUpdateFmlPackUnlock(CCmdUI *pCmdUI)
{
	int i = 0,j = 0;
	int nCurGpIndex = GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	for(i=0;i<g_nFml2Stk;i++)
	{
		if(tempStkInfo->setcode==g_Fml2Stk[i].setcode
			&& strcmp(tempStkInfo->Code,g_Fml2Stk[i].code)==0)
		{
			for(j=0;j<g_nFmlPack;j++)
			{
				if(strcmp(g_Fml2Stk[i].aPackName,g_FmlPack[j].aPackName)==0) break;
			}
			if(j<g_nFmlPack) break;
		}
	}
	pCmdUI->Enable(i<g_nFml2Stk);
}

void CUnitView::OnFmlPackMng()
{
	CFormulaPackDlg dlg;
	dlg.SetMode(FALSE,FALSE);
	dlg.DoModal();
}

void	CUnitView::ShowPopPreviewTool(int nUnitType, int nUnitStyle, CRect &rc, MemStkInfo *pStock)
{
	CString tmpChar;
	tmpChar.Format("%d-",nUnitType);
	for(int k=0;k<10;k++)
		tmpChar += "000000";
	tmpChar += "\r\n";
	CString Style;
	Style.Format("%d",nUnitStyle);
	tmpChar += Style;
	tmpChar += "\r\n";
	CString GPCode;
	GPCode.Format("%d%s",pStock->setcode,pStock->Code);
	tmpChar += GPCode;
	for(k=0;k<16;k++)
	{
		CString strLine;
		strLine.Format("\r\n%d", k);
		tmpChar += strLine;
	}
	m_PreviewTool.AddPreviewTool(this, _F(tmpChar), rc, TOOLTIP_HQ);
}


BEGIN_EVENTSINK_MAP(CUnitView, CView)
    //{{AFX_EVENTSINK_MAP(CAssisWnd)
	ON_EVENT(CUnitView, IDC_IEUNIT, 250 /* BeforeNavigate2 */, OnBeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CUnitView::OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel) 
{	
	USES_CONVERSION;
	
	char url[501]={0};
	strncpy(url,W2A(URL->bstrVal),500);
	if(BeforeNavigate_Process(url))
		*Cancel = TRUE;
}

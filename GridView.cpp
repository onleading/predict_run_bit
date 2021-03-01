// GridView.cpp : implementation of the CGridView class
#include "stdafx.h"
#include "TdxW.h"
#include "GridView.h"
#include "ViewManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FYGROUP_SWITCH_HEIGHT	19
/////////////////////////////////////////////////////////////////////////////
// CGridView

IMPLEMENT_DYNCREATE(CGridView, CView)

BEGIN_MESSAGE_MAP(CGridView, CView)
	/////////////////////
	ON_COMMAND(ID_INBLOCK,OnInBlock)
	ON_COMMAND(ID_BATCH,OnBatch)
	ON_COMMAND(ID_GOTOJBZL, OnGotojbzl)
	ON_COMMAND(ID_CHANGEGPTYPE, OnChangeGPType)
	ON_COMMAND(ID_AUTOPAGE,OnAutoPage)
	ON_COMMAND_RANGE(ID_RIGHT_OPEN,ID_RIGHT_DELETEFROMBLOCK,OnHQGridCommand)
	ON_COMMAND_RANGE(ID_RIGHT_2,ID_RIGHT_49,OnHQGridCommand)
	ON_COMMAND_RANGE(ID_QHRIGHT_2,ID_QHRIGHT_23,OnHQGridCommand)
	ON_COMMAND_RANGE(ID_RIGHT_TYPE0,ID_RIGHT_TYPE20,OnHQGridCommand)
	ON_COMMAND_RANGE(ID_RIGHT_HQBASE,ID_RIGHT_FYGROUP_MAX,OnFyReport)
	ON_COMMAND_RANGE(ID_RIGHT_FYBASE1,ID_RIGHT_FYBASE_MAX,OnFyReport)

	ON_COMMAND_RANGE(ID_DY1,ID_DY32,OnHQGridCommand)
	ON_COMMAND_RANGE(ID_HY1,ID_HY52,OnHQGridCommand)
	ON_COMMAND_RANGE(ID_CONCEPT_MENU_1,ID_CONCEPT_MENU_100,OnHQGridCommand)
	ON_COMMAND_RANGE(ID_ZJHHY_MENU_1,ID_ZJHHY_MENU_200,OnHQGridCommand)
	ON_COMMAND_RANGE(ID_ZHB_MENU_1,ID_ZHB_MENU_100,OnHQGridCommand)
	ON_COMMAND_RANGE(ID_BLOCK_MENU_1,ID_BLOCK_MENU_400,OnHQGridCommand)

	ON_COMMAND_RANGE(IDM_DS1,IDM_DS1+MAX_MKT_NUM,OnDSMarket)
	ON_COMMAND_RANGE(ID_FYFLAG_0,ID_FYFLAG_END,OnHQGridCommand)
	ON_COMMAND_RANGE(ID_RANK_1,ID_RANK_26,OnHQGridCommand)
	ON_COMMAND_RANGE(ID_GRIDHZ_AVER,ID_GRIDHZ_STD,OnGridHZ)
	ON_COMMAND(ID_GRIDHZ_OPT,OnGridHZOpt)
	/////////////////////
	ON_UPDATE_COMMAND_UI_RANGE(ID_DY1,ID_DY32,OnUpdateDYBlock)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_DS1,IDM_DS1+MAX_MKT_NUM,OnUpdateDSMarket)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FYFLAG_0,ID_FYFLAG_END,OnUpdateFYGrid)
	ON_UPDATE_COMMAND_UI_RANGE(ID_HY1,ID_HY52,OnUpdateHYBlock)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CONCEPT_MENU_1,ID_CONCEPT_MENU_100,OnUpdateConceptClick)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ZJHHY_MENU_1,ID_ZJHHY_MENU_200,OnUpdateZJHHYClick)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ZHB_MENU_1,ID_ZHB_MENU_100,OnUpdateZHBClick)
	
	ON_UPDATE_COMMAND_UI_RANGE(ID_RANK_1,ID_RANK_26,OnUpdateRank)
	ON_UPDATE_COMMAND_UI_RANGE(ID_RIGHT_2,ID_RIGHT_49,OnUpdateRightClickHead)
	ON_UPDATE_COMMAND_UI_RANGE(ID_QHRIGHT_2,ID_QHRIGHT_23,OnUpdateRightClickHead)
	ON_UPDATE_COMMAND_UI_RANGE(ID_RIGHT_TYPE0,ID_RIGHT_TYPE20,OnUpdateClickCorner)
	ON_UPDATE_COMMAND_UI_RANGE(ID_RIGHT_OPEN,ID_RIGHT_DELETEFROMBLOCK,OnUpdateRightClickRow)
	ON_UPDATE_COMMAND_UI_RANGE(ID_BLOCK_MENU_1,ID_BLOCK_MENU_400,OnUpdateMenuBlockClick)
	ON_UPDATE_COMMAND_UI(ID_AUTOPAGE, OnUpdateAutopage)
	ON_UPDATE_COMMAND_UI_RANGE(ID_GRIDHZ_AVER,ID_GRIDHZ_STD,OnUpdateGridHZ)
	//////
	ON_MESSAGE(UM_GETMAINID,GetMainID)
	ON_MESSAGE(UM_RESETFYSWITCHBAR,OnResetFySwitchBar)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_WRITETOTXTFILE, OnWritetotxtfile)
	ON_COMMAND(ID_PAGEUP, OnPageup)
	ON_COMMAND(ID_PAGEDOWN, OnPagedown)
	ON_MESSAGE(UM_GRIDSETTITLE,OnGridSetTitle)
	ON_COMMAND_RANGE(ID_MDFHK_HY_1,ID_MDFHK_HY_300,OnDSMarket)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MDFHK_HY_1,ID_MDFHK_HY_300,OnUpdateMDFHkHyClick)
	
	ON_COMMAND(ID_MARK_CANCEL, OnCancelMark)
	ON_COMMAND_RANGE(ID_MARK_ADD1, ID_MARK_ADD10, OnMarkStockFlag)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MARK_ADD1, ID_MARK_ADD10, OnUpdateMarkStockFlag)
//{{AFX_MSG_MAP(CGridView)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()

	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridView construction/destruction

CGridView::CGridView()
{
	m_bEditShow = FALSE;

	KeyGuyFlag = FALSE;
	m_bQuitting = FALSE;

	m_nMainID = 0;
	m_bLayoutFlag = FALSE;

	m_pHQGrid = new HQGrid(NULL);
	m_pSwitchBar = NULL;
	m_bHasTendBar = FALSE;
}

CGridView::~CGridView()
{
	TDEL(m_pSwitchBar);
	TDEL(m_pHQGrid);
	theViewManager.RemoveView(this);
}

/////////////////////////////////////////////////////////////////////////////
// CGridView drawing

void CGridView::OnDraw(CDC* pDC)
{
	if(g_pToolTipMark) 
		g_pToolTipMark->ClearToolTipInfo(this);
	if(m_bHasTendBar)
	{
		CRect rcWnd,rcSwitch;
		GetClientRect(rcWnd);
		rcWnd.OffsetRect(-rcWnd.left,-rcWnd.top);
		if(m_pSwitchBar && ::IsWindow(m_pSwitchBar->m_hWnd))
		{
			m_pSwitchBar->GetWindowRect(rcSwitch);
			ScreenToClient(rcSwitch);
		}
		rcWnd.left=rcSwitch.right;
		rcWnd.bottom=rcSwitch.bottom+1;

		g_GUI.DrawGradient(pDC,rcWnd,GridColor.GridBackColor,RGB(128,128,128),TRUE);
		CPen  *oldpen = g_d.SelectPen(pDC,AXIS_PEN);
		g_d.DrawLine(pDC,0,rcWnd.bottom-1,rcWnd.right,rcWnd.bottom-1);
		g_d.RestorePen(pDC,oldpen);
		if(strlen(m_strFYTitle)>0)
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(VipColor.BIGTITLECOLOR);
			CFont *pOld=g_d.SelectFont(pDC,SMALL_FONT);
			CSize tmpsz=pDC->GetTextExtent(m_strFYTitle);
			int rx=m_rcFYTitle.Width()-tmpsz.cx;
			int x=(rx>=0?rx:0)/2+m_rcFYTitle.left;
			CRect tmprc=m_rcFYTitle;
			tmprc.left=x;
			pDC->DrawText(m_strFYTitle,tmprc,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
			g_d.RestoreFont(pDC,pOld);
		}
	}
	if(!m_bEditShow)
		m_pHQGrid->ProcessMsg(WM_PAINT);
}

/////////////////////////////////////////////////////////////////////////////
// CGridView diagnostics

#ifdef _DEBUG
void CGridView::AssertValid() const
{
	CView::AssertValid();
}

void CGridView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG

////////////////////////////////////////////////////////////

void CGridView::SetFySwitch()
{
	if(!m_pSwitchBar) return;
	m_strFYTitle="";
	m_pSwitchBar->ClearAllLabel();
	m_pSwitchBar->SetFontColorSchema(GridColor.GridBackColor,GridColor.GridBackColor,RGB(212,208,200),0,RGB(160,160,160),RGB(0,0,0),RGB(150,150,150),0,VipColor.AXISCOLOR,12,SONG_FONT);
	m_bHasTendBar = (!g_GC.bOnlyHK) && (g_vfyGroup.size() || g_bSupportDDE);
	if(m_bHasTendBar)
	{
		m_pSwitchBar->InsertSwitchLabel(0,_F("行情报价"),WM_COMMAND,ID_RIGHT_HQBASE);
		int n=1;
		if(g_bSupportDDE)
			m_pSwitchBar->InsertSwitchLabel(n++,_F("DDE决策"),WM_COMMAND,ID_RIGHT_HQDDE);
		if(g_bHasDS)
			m_pSwitchBar->InsertSwitchLabel(n++,_F("A+H行情对照"),WM_COMMAND,ID_RIGHT_HQAJH);
		for(int i=0;i<g_vfyGroup.size();i++)
		{
			if(g_vfyGroup[i].belong<0)
			{
				if(g_vfyGroup[i].subGroup.size()>0)
					m_pSwitchBar->InsertSwitchLabel(n,_F(g_vfyGroup[i].strGroupName),WM_COMMAND,IDR_LONELY_MENU,-200-i,TRUE);
				else
					m_pSwitchBar->InsertSwitchLabel(n,_F(g_vfyGroup[i].strGroupName),WM_COMMAND,ID_RIGHT_FYGROUP+i);
				COLORREF cor;
				if(GetSpecialTextColor(g_vfyGroup[i].groupAttribute,cor))
					m_pSwitchBar->SetSpecialTextColor(n,cor);
				g_vfyGroup[i].nIndex=n++;
			}
		}
		CRect rc;
		GetClientRect(rc);
		int cx=rc.Width();
		int cy=rc.Height();
		if(cx!=0 && cy!=0)
		{
			int cx2=m_pSwitchBar->GetNeedWidth();
			cx2=min(cx2,ScreenCx-200);
			m_pSwitchBar->SetWindowPos(NULL,0,0,cx2,FYGROUP_SWITCH_HEIGHT-1,SWP_SHOWWINDOW);
			CSize sz=g_GUI.GetBitmapSize(IDB_INFOBT_N);
			m_rcFYTitle.SetRect(cx2,0,cx-sz.cx-30,FYGROUP_SWITCH_HEIGHT-1);
			m_RichEdit.MoveWindow(0,FYGROUP_SWITCH_HEIGHT,cx,cy-FYGROUP_SWITCH_HEIGHT);
			if(!m_bEditShow)
			{
				WPARAM wParam = MAKELONG(0,FYGROUP_SWITCH_HEIGHT);
				LPARAM lParam = MAKELONG(cx,cy-FYGROUP_SWITCH_HEIGHT);
				m_pHQGrid->ProcessMsg(WM_SIZE,wParam,lParam);
			}
		}
	}
	Invalidate(FALSE);
}

int CGridView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_RichEdit.Create(WS_CHILD|ES_AUTOVSCROLL|ES_READONLY|WS_VSCROLL|WS_TABSTOP|ES_MULTILINE,CRect(0,0,0,0),this,TXT_RICHEDIT);
	m_RichEdit.SetMenuIndex(-1);
	m_RichEdit.m_bUseTxtColor=FALSE;
	m_RichEdit.ApplyTxtSize();
	m_RichEdit.ShowWindow(SW_HIDE);

	//创建风云切换区
	m_pSwitchBar = new CNewZfjsTab(m_hWnd);
	m_pSwitchBar->SetStyle(SwitchStyle_custom1);
	if(!m_pSwitchBar || !m_pSwitchBar->Create(_T("NewZfjsTab"),"",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,ID_HQSWITCHBAR))
		return -1;
	m_pSwitchBar->SetMenuCallBack((PMENUCALLBACK)SwitchMenuCallBack);
//	SetFySwitch();
	m_pHQGrid->Create(this,CRect(0,0,0,0));

	m_bLayoutFlag = TRUE;
	//用于定时更新行情的定时器
	SetTimer(1,GetListRefreshTime(),NULL);
	//用于自动翻页的定时器
	SetTimer(2,AutoPageTime*1000,NULL);
	//用于定时刷新行情列表的定时器,缺省是3秒
	SetTimer(3,3000,NULL);

	return 0;
}

extern void GetInitDomain(int &WhichGPType,int &BlockFlag,int FirstType,int FirstBlock);

void CGridView::OnInitialUpdate() 
{
 	CView::OnInitialUpdate();
	Modify_CorpWebMenu();

	theViewManager.AddView("市场报价", this);
	
	if(g_pToolTipMark) 
		g_pToolTipMark->AddTool(this, ""); 

	//赋初始股票集合
	int BlockFlag,WhichGPType;
	GetInitDomain(WhichGPType,BlockFlag,FirstGPType,FirstBlockFlag);
	m_pHQGrid->SetDomain(BlockFlag,WhichGPType,TRUE,ZAF,TRUE);
	theViewManager.pOldActiveView=this;
}

void CGridView::OnTimer(UINT nIDEvent)
{
	if(theViewManager.pActiveView != this)
		return;
	m_pHQGrid->ProcessMsg(WM_TIMER,nIDEvent);
	CView::OnTimer(nIDEvent);
}

void CGridView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	theViewManager.OnActivateView(bActivate, this);
	//在历史分析和行情图中，当激活之时，自动保存旧的激活视图
	if(!g_bSepEsc)	//如果是特殊ESC逻辑,则不用行情
		theViewManager.pOldActiveView2 = theViewManager.pActiveView;
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if(!m_bQuitting && bActivate)
	{
		AfxGetMainWnd()->SendMessage(UM_HIDESHOWBUTTON,HideShowBtn_Hq);
	}
	if(bActivate && theViewManager.pOldActiveView!=this)
	{
		g_pComm->m_HQBufFlag = 0;
		g_pComm3->m_HQBufFlag = 0;
		if(m_pHQGrid && g_bOldGridDomain) 
		{
			m_pHQGrid->m_bRefreshDraw=TRUE;
			m_pHQGrid->ProcessHQRefresh();
			m_pHQGrid->m_bRefreshDraw=FALSE;
		}
	}
	PostMessage(UM_BLOCKMODIFIED,0,0);
}

void CGridView::ResizeControls()
{
	CRect rc,rc2;
	GetWindowRect(rc);
	GetClientRect(rc2);
	int cx=rc2.Width(),cy=rc2.Height();
	if(m_bLayoutFlag)
	{
		long wParam,lParam;
		if(m_bHasTendBar)
		{
			m_pSwitchBar->SetRect(0,0,0,FYGROUP_SWITCH_HEIGHT-1);
			int cx2=m_pSwitchBar->GetNeedWidth();
			cx2=min(cx2,ScreenCx-200);
			m_pSwitchBar->SetWindowPos(NULL,0,0,cx2,FYGROUP_SWITCH_HEIGHT-1,SWP_SHOWWINDOW);
			CSize sz=g_GUI.GetBitmapSize(IDB_INFOBT_N);
			m_rcFYTitle.SetRect(cx2,0,cx-sz.cx-30,FYGROUP_SWITCH_HEIGHT-1);
			m_RichEdit.MoveWindow(0,FYGROUP_SWITCH_HEIGHT,cx,cy-FYGROUP_SWITCH_HEIGHT);
			if(!m_bEditShow)
			{
				wParam = MAKELONG(0,FYGROUP_SWITCH_HEIGHT);
				lParam = MAKELONG(cx,cy-FYGROUP_SWITCH_HEIGHT);
				m_pHQGrid->ProcessMsg(WM_SIZE,wParam,lParam);
			}
		}
		else
		{
 			wParam = MAKELONG(0,0);
 			lParam = MAKELONG(cx,cy);
			m_pHQGrid->ProcessMsg(WM_SIZE,wParam,lParam);
			m_pSwitchBar->SetWindowPos(NULL,0,0,0,0,SWP_SHOWWINDOW);
			m_RichEdit.MoveWindow(0,0,0,0);
		}
	}
}

void CGridView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	ResizeControls();
}

BOOL CGridView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_CHAR && KeyGuyFlag == FALSE )
	{
		if(ProcessKeyGuy(pMsg,this,KeyGuyFlag))
			return TRUE;
		if(pMsg->wParam == '-' || pMsg->wParam == ']' || pMsg->wParam == '[' || pMsg->wParam == VK_TAB || pMsg->wParam == VK_SPACE || pMsg->wParam == '*')
		{
			m_pHQGrid->ProcessMsg(WM_CHAR,pMsg->wParam,pMsg->lParam);
			return TRUE;
		}
	}
	if(g_pToolTipMark) 
		g_pToolTipMark->RelayEvent(pMsg);
	return CView::PreTranslateMessage(pMsg);
}


void CGridView::GetMainID(WPARAM wParam,LPARAM lParam)
{
	g_nMainID = MainIDPlus(m_nMainID,GRIDVIEW_WIN);
	g_nAssisID = HQ_ASSISID+wParam;
}

void CGridView::SwitchEdit(int nWhichGroup)
{
	if(!m_bEditShow)
	{
		CRect rcWnd;
		GetClientRect(rcWnd);
		WPARAM w = MAKELONG(0,FYGROUP_SWITCH_HEIGHT);
		LPARAM l = MAKELONG(rcWnd.Width(),rcWnd.Height()-FYGROUP_SWITCH_HEIGHT);
		m_pHQGrid->ProcessMsg(WM_SIZE,w,l);
		m_RichEdit.ShowWindow(SW_HIDE);
	}
	else
	{
		m_pHQGrid->SetRect(CRect(0,0,0,0));
		CString strInfo;
		CString infoFile;
		infoFile.Format("%sLData\\cache\\%s%02d.tat",g_WSXHStr,g_strTendName,g_vfyGroup[nWhichGroup].nIndex_file);
		m_pHQGrid->GetFYTxt(infoFile,0,strInfo);
		m_RichEdit.SetWindowText(_F(strInfo));
		m_RichEdit.ShowWindow(SW_SHOW);
	}
}

LRESULT CGridView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==UM_GETDATA_ACK)
		m_pHQGrid->OnGetDataACK(m_nMainID,wParam==1);
	else if(message==UM_RESETGRIDTAB)
	{
		m_pHQGrid->InsertSwitch(0);
		m_pHQGrid->SetRect(CRect(0,0,0,0), TRUE);
	}	
	else if(message==UM_GRIDREINIT)
	{
		ResizeControls();
		SetFySwitch();				
		m_pHQGrid->ProcessMsg(message,wParam,lParam);
		return 1;
	}
	else if(message==UM_BLOCKMODIFIED && g_bBlockModified)
	{
		g_bBlockModified = FALSE;
		m_pHQGrid->ProcessMsg(message,wParam,lParam);
	}
	else if(message==WM_TREE_MESSAGE ||	//自定义消息
		    message==WM_KEYGUY_MESSAGE || 
			message==UM_TBBD_MESSAGE || 
			message==UM_CHANGEGRIDWIDTH ||
			message==UM_HSCROLLBAR ||
			message==UM_VSCROLLBAR )
	{
		m_pHQGrid->ProcessMsg(message,wParam,lParam);
		return 1;
	}
	else if(message==WM_CHANGEBLOCK)
	{
		int topGroup=0;
		if(wParam==-3)	//风云板块
		{
			int whichGPType=g_FyLocation.nGroup;
			SetFYGroupActive(whichGPType);
			if(g_vfyGroup[whichGPType].belong<0)
				topGroup=g_vfyGroup[whichGPType].nIndex;
			else
				topGroup=g_vfyGroup[g_vfyGroup[whichGPType].belong].nIndex;
		}
		if(m_pSwitchBar && ::IsWindow(m_pSwitchBar->m_hWnd))
			m_pSwitchBar->SetCurLabel(topGroup);
		m_pHQGrid->ProcessMsg(message,wParam,lParam);
		return 1;
	}
	return CView::WindowProc(message, wParam, lParam);
}

void CGridView::OnDestroy() 
{
	m_bQuitting = TRUE;
	CView::OnDestroy();	
}

short CGridView::GetCurrentGPIndex()
{
	return m_pHQGrid->GetCurrentGPIndex();
}

short CGridView::GetCurrentGPSet(short *pPageGPIndex)
{
	return m_pHQGrid->GetCurrentGPSet(pPageGPIndex);
}

void CGridView::GetCurrentDomain(int & WhichGPType,int & nBlockFlag)
{
	WhichGPType = m_pHQGrid->GetGPType();
	nBlockFlag = m_pHQGrid->GetBlockFlag();
}

/////////////////////////消息传递///////////////////////////////

void CGridView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar==VK_ESCAPE && m_pHQGrid->m_nGridType==FY_TYPE)
	{
		SendMessage(WM_COMMAND,ID_RIGHT_HQBASE);
		return;
	}
	m_pHQGrid->OnKeyDown(nChar, nRepCnt, nFlags);
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CGridView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	m_pHQGrid->OnKeyUp(nChar, nRepCnt, nFlags);
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CGridView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect;
	m_pHQGrid->GetClientRect(rect);
	if(rect.PtInRect(point))
		m_pHQGrid->ProcessMsg(WM_LBUTTONDOWN,nFlags,MAKELONG(point.x,point.y));
	CView::OnLButtonDown(nFlags, point);
}

void CGridView::OnRButtonUp(UINT nFlags, CPoint point)
{
	CRect rect;
	m_pHQGrid->GetClientRect(rect);
	if(rect.PtInRect(point))
		m_pHQGrid->ProcessMsg(WM_RBUTTONUP,nFlags,MAKELONG(point.x,point.y));
	CView::OnRButtonUp(nFlags, point);
}

void CGridView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CRect rect;
	m_pHQGrid->GetClientRect(rect);
	if(rect.PtInRect(point))
		m_pHQGrid->ProcessMsg(WM_LBUTTONDBLCLK,nFlags,MAKELONG(point.x,point.y));
	CView::OnLButtonDblClk(nFlags, point);
}

void CGridView::OnMouseMove(UINT nFlags, CPoint point)
{
	m_pHQGrid->ProcessMsg(WM_MOUSEMOVE,nFlags,MAKELONG(point.x,point.y));
	CView::OnMouseMove(nFlags, point);
}

void CGridView::OnLButtonUp(UINT nFlags, CPoint point)
{	
	CRect rect;
	m_pHQGrid->GetClientRect(rect);
	if(rect.PtInRect(point))
		m_pHQGrid->ProcessMsg(WM_LBUTTONUP,nFlags,MAKELONG(point.x,point.y));
	CView::OnLButtonUp(nFlags, point);
}

BOOL CGridView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(zDelta < 0)
	{
		OnKeyDown(VK_NEXT,1,0);
		OnKeyUp(VK_NEXT,1,0);
	}
	else
	{
		OnKeyDown(VK_PRIOR,1,0);
		OnKeyUp(VK_PRIOR,1,0);
	}
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CGridView::OnGridSetTitle(WPARAM wParam,LPARAM lParam)
{
	CString strTitle;
	m_pHQGrid->GetGridTitle(strTitle);
	//设置窗口标题
	theViewManager.SetViewName(strTitle, this);
//	GetParent()->SetWindowText (_F("市场报价-"+strTitle));
	AfxGetMainWnd()->SetWindowText(g_strTitleName);
}

void CGridView::OnUpdateRightClickHead(CCmdUI* pCmdUI)
{
	m_pHQGrid->OnUpdateRightClickHead(pCmdUI);
}

void CGridView::OnUpdateDYBlock(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID-ID_DY1 == m_pHQGrid->GetBlockFlag()-DY_START);
}

void CGridView::OnUpdateFYGrid(CCmdUI* pCmdUI)
{
	int nFY = pCmdUI->m_nID-ID_FYFLAG_0;
	int nSelType = FY_START+nFY/MAX_FYNUM_IN_GROUP;
	int nSelFlag = nFY%MAX_FYNUM_IN_GROUP;
	pCmdUI->SetCheck(m_pHQGrid->GetGPType()==nSelType&&m_pHQGrid->GetBlockFlag()==nSelFlag);
}

void CGridView::OnUpdateHYBlock(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID-ID_HY1 == m_pHQGrid->GetBlockFlag()-HY_START);
}

void CGridView::OnUpdateConceptClick(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID-ID_CONCEPT_MENU_1 == m_pHQGrid->GetBlockFlag()-CON_START);
}

void CGridView::OnUpdateZHBClick(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID-ID_ZHB_MENU_1 == m_pHQGrid->GetBlockFlag()-ZHB_START);
}

void CGridView::OnUpdateZJHHYClick(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID-ID_ZJHHY_MENU_1 == m_pHQGrid->GetBlockFlag()-ZJHHY_START);
}

void CGridView::OnUpdateClickCorner(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID-ID_RIGHT_TYPE0 == m_pHQGrid->GetGPType());
}

void CGridView::OnUpdateRightClickRow(CCmdUI* pCmdUI)
{
	switch(pCmdUI->m_nID)
	{
	case ID_RIGHT_OPEN:
	case ID_DELFROMBLOCK:
	case ID_RIGHT_ADDBLOCK:
	case ID_RIGHT_ADDZXG:
		pCmdUI->Enable(m_pHQGrid->GetTotalRow() > 0);
		break;
	case ID_RIGHT_NEWWINDOW:
		pCmdUI->Enable(FALSE);
		break;
	case ID_RIGHT_DELETEFROMBLOCK:
		{
			int tmpBlockFlag = m_pHQGrid->GetBlockFlag();
			int tmpGPType = m_pHQGrid->GetGPType();
			if(!BeUserBlock(tmpGPType,tmpBlockFlag))
				pCmdUI->Enable(FALSE);
			else
				pCmdUI->Enable(m_pHQGrid->GetTotalRow() > 0);
		}
		break;
	}
}

void CGridView::OnUpdateRank(CCmdUI *pCmdUI)
{
	m_pHQGrid->OnUpdateRank(pCmdUI);
}

void CGridView::OnUpdateMenuBlockClick(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID-ID_BLOCK_MENU_1 == m_pHQGrid->GetBlockFlag());
}

void CGridView::OnUpdateAutopage(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_pHQGrid && m_pHQGrid->GetAutoPage());
}

void CGridView::OnUpdateGridHZ(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_pHQGrid->IsHZTypeNow(pCmdUI->m_nID-ID_GRIDHZ_AVER));
}
////////////////////
void CGridView::OnInBlock()
{
	m_pHQGrid->ProcessMsg(WM_COMMAND,ID_INBLOCK);
}

void CGridView::OnBatch()
{
	m_pHQGrid->ProcessMsg(WM_COMMAND,ID_BATCH);
}

void CGridView::OnGotojbzl()
{
	m_pHQGrid->ProcessMsg(WM_COMMAND,ID_GOTOJBZL);
}

void CGridView::OnChangeGPType()
{
	m_pHQGrid->ProcessMsg(WM_COMMAND,ID_CHANGEGPTYPE);
}

void CGridView::OnAutoPage()
{
	m_pHQGrid->ProcessMsg(WM_COMMAND,ID_AUTOPAGE);
}

void CGridView::OnResetFySwitchBar(WPARAM wParam,LPARAM lParam)
{
	m_strFYTitle="";
	Invalidate(FALSE);
	if(m_pSwitchBar && ::IsWindow(m_pSwitchBar->m_hWnd)) 
		m_pSwitchBar->SetCurLabel(wParam);
	if(m_bEditShow)
	{
		m_bEditShow=FALSE;
		SwitchEdit(0);
	}
}

void CGridView::OnFyReport(UINT nID)
{
	BOOL bProcessMsg=FALSE;
	if(nID==ID_RIGHT_HQBASE || nID==ID_RIGHT_HQDDE || nID==ID_RIGHT_HQAJH)
	{
		SetFYGroupActive(-1);
		m_strFYTitle="";
		Invalidate(FALSE);
		if(m_bEditShow)
		{
			m_bEditShow=FALSE;
			SwitchEdit(0);
		}
	}
	if(nID>=ID_RIGHT_FYGROUP && nID<=ID_RIGHT_FYGROUP_MAX)
	{
		if(g_vfyGroup.size()<1) return;
		SetFYGroupActive(nID-ID_RIGHT_FYGROUP);
		if(g_vfyGroup[nID-ID_RIGHT_FYGROUP].belong>=0)
			m_pSwitchBar->SetCurLabel(g_vfyGroup[g_vfyGroup[nID-ID_RIGHT_FYGROUP].belong].nIndex);
		m_bEditShow=FALSE;
		if(g_vfyGroup[nID-ID_RIGHT_FYGROUP].nType==1)
			m_bEditShow=TRUE;
		if(m_bEditShow)
		{
			m_pHQGrid->ProcessMsg(WM_COMMAND,nID);
			bProcessMsg=TRUE;
		}
		SwitchEdit(nID-ID_RIGHT_FYGROUP);	
	}	
	if(!bProcessMsg)
		m_pHQGrid->ProcessMsg(WM_COMMAND,nID);
}

void CGridView::OnHQGridCommand(UINT nID)
{
	m_pHQGrid->ProcessMsg(WM_COMMAND,nID);	
}

BOOL ConnectDSAtOnce(BOOL &bOK,HWND hWnd)
{
	bOK=FALSE;
	if(g_nDSAutoStopMinute>0)
		bOK = DSConnectWhenNeedHq(TRUE);
	else if(!DSOnLine)
	{
		if(!OnLine) 
		{
			TDX_MessageBox(hWnd,"请先连接到行情主站!",MB_ICONINFORMATION|MB_OK);
			return FALSE;
		}
		if(!DSConnectWhenNeedHq(FALSE))
			return FALSE;
		bOK=TRUE;
	}	
	return TRUE;
}

void CGridView::OnDSMarket(UINT nID)
{
	BOOL bOK=FALSE;
	if(!ConnectDSAtOnce(bOK,m_hWnd))
		return;
	if(bOK)
		SendMessage(UM_GRIDREINIT);
	m_pHQGrid->ProcessMsg(WM_COMMAND,nID);
}

void CGridView::OnUpdateMDFHkHyClick(CCmdUI* pCmdUI)
{
	int nBlockFlag = m_pHQGrid->GetBlockFlag();
	pCmdUI->SetCheck(pCmdUI->m_nID-ID_MDFHK_HY_1==nBlockFlag-HKHY_START);
}

void CGridView::OnUpdateDSMarket(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(DSGetMarketXH(pCmdUI->m_nID-IDM_DS1)==m_pHQGrid->GetGPType()-DS_START);
}

void CGridView::OnGridHZ(UINT nID)
{
	int nHZNum0 = m_pHQGrid->GetStaticItemCount();
	m_pHQGrid->SetHZType(nID-ID_GRIDHZ_AVER);
	int nHZNum = m_pHQGrid->GetStaticItemCount();
	if(nHZNum-nHZNum0>0) RefreshAllHq();
	m_pHQGrid->ProcessMsg(WM_PAINT);
}

#include "GrigHzParaDlg.h"
void CGridView::OnGridHZOpt()
{
	short nTmpFlag = g_nHZQZFlag;
	CGrigHzParaDlg dlg;
	if(dlg.DoModal()==IDOK)
	{
		if(nTmpFlag!=g_nHZQZFlag)
			m_pHQGrid->ProcessMsg(WM_PAINT);
	}

}
////////////////////

BOOL CGridView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

BOOL CGridView::ProcessGPCode(const char *code,short setcode)
{
	return FALSE;
}

void CGridView::SpecialReDraw(int Flag)
{
	Invalidate();
	m_pSwitchBar->SetFontColorSchema(GridColor.GridBackColor,GridColor.GridBackColor,RGB(212,208,200),0,RGB(160,160,160),RGB(0,0,0),RGB(150,150,150),0,VipColor.AXISCOLOR,12,SONG_FONT);
	m_pHQGrid->SpecialReDraw(Flag);
}

void CGridView::OnWritetotxtfile() 
{
	m_pHQGrid->OnWritetotxtfile();
}

void CGridView::OnPageup() 
{
	m_pHQGrid->OnKeyDown(VK_PRIOR,1,0);
	if(OnLine)
		m_pHQGrid->OnKeyUp(VK_PRIOR,1,0);
}

void CGridView::OnPagedown() 
{
	m_pHQGrid->OnKeyDown(VK_NEXT,1,0);
	if(OnLine)
		m_pHQGrid->OnKeyUp(VK_NEXT,1,0);
}

void CGridView::OnCancelMark()
{
	int nCurGpIndex = GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	MarkStockFlag(tempStkInfo, 0);
}

#include "TipMarkDlg.h"
void CGridView::OnMarkStockFlag(UINT nID)
{
	int nFlag = nID - ID_MARK_ADD1+1;
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
	m_pHQGrid->ProcessMsg(WM_PAINT);
}

void CGridView::OnUpdateMarkStockFlag(CCmdUI* pCmdUI)
{
	char nFlag = pCmdUI->m_nID - ID_MARK_ADD1+1;
	if(nFlag<=0||nFlag>10) return;
	int nCurGpIndex = GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	pCmdUI->SetCheck(nFlag==tempStkInfo->fl_flag);
}
//////////////////////////////////////////////////////////////////////////
//打印相关
BOOL CGridView::OnPreparePrinting(CPrintInfo* pInfo) 
{
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

void CGridView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
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

	m_pHQGrid->SetPrintRect(CRect(0,0,nScreenCx, nScreenCy));
	m_pHQGrid->CalcPrintPage(pDC, pInfo);

	g_d.SelectPen(pDC,LEVEL_PEN);
	g_d.DrawRect(pDC,1,1,nScreenCx-1,nScreenCy-1);
}

void CGridView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
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

	m_pHQGrid->SetPrintRect(CRect(0,0,nScreenCx, nScreenCy));
	m_pHQGrid->PrintGrid(pDC, pInfo);		
}

void CGridView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CView::OnEndPrinting(pDC, pInfo);
}

void CGridView::SetFYTitle(LPCSTR str)
{
	m_strFYTitle=str;
	InvalidateRect(m_rcFYTitle);
}

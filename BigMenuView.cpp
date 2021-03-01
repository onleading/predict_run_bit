 // BigMenuView.cpp : implementation of the CBigMenuView class
#include "stdafx.h"
#include "TdxW.h"
#include "BigMenuView.h"
#include "GridView.h"
#include "ComView.h"
#include "TxtView.h"
#include "GetBlockDlg.h"
#include "ViewManager.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CBigMenuView

IMPLEMENT_DYNCREATE(CBigMenuView, CView)

BEGIN_MESSAGE_MAP(CBigMenuView, CView)
//{{AFX_MSG_MAP(CBigMenuView)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBigMenuView construction/destruction

CBigMenuView::CBigMenuView()
{
	KeyGuyFlag = FALSE;
	m_bQuitting = FALSE;
	m_bLayoutFlag = FALSE;

	pMenuWin   = NULL;
	pBlockWin  = NULL;
}

CBigMenuView::~CBigMenuView()
{
	TDEL(pMenuWin);
	TDEL(pBlockWin);
	theViewManager.RemoveView(this);
}

/////////////////////////////////////////////////////////////////////////////
// CBigMenuView drawing

void CBigMenuView::OnDraw(CDC* pDC)
{
	pActiveWin->Handle (WM_PAINT,0,0);
}

/////////////////////////////////////////////////////////////////////////////
// CBigMenuView diagnostics

#ifdef _DEBUG
void CBigMenuView::AssertValid() const
{
	CView::AssertValid();
}

void CBigMenuView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG

void CBigMenuView::OnInitialUpdate() 
{
 	CView::OnInitialUpdate();
	Modify_CorpWebMenu();
		
	theViewManager.AddView("大字菜单", this);
	m_bLayoutFlag = TRUE;

	Refresh_OftenUseZb_Menu();
	Refresh_XXM_Menu();
}

int CBigMenuView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

    pMenuWin  = new CMenuWin(this);         //用于菜单的显示          0
	pBlockWin = new CBlockWin(this);        //用于选择板块            1	

	//设置初始窗口
	pActiveWin	= pMenuWin;
	pDeactiveWin= pMenuWin;
	nActiveWin = 0;
	nDeactiveWin = 0;
	SendMessage(UM_SWAP_MENU,MENU_TOP,0);

	return 0;
}

void CBigMenuView::ChangeMenuColor()
{
	if(pActiveWin)
		pActiveWin->Handle(UM_CHANGE_MENU_COLOR,0,0);
}

void CBigMenuView::OnDestroy() 
{
	m_bQuitting = TRUE;
	CView::OnDestroy();	
}

void CBigMenuView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	theViewManager.OnActivateView(bActivate, this);
	if(!g_bSepEsc)	//如果是特殊ESC逻辑,则不用行情
		theViewManager.pOldActiveView2 = theViewManager.pActiveView;
	g_bSepEsc = FALSE;
	//在历史分析和行情图中，当激活之时，自动保存旧的激活视图
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if(!m_bQuitting && bActivate)
	{
		AfxGetMainWnd()->SendMessage(UM_HIDESHOWBUTTON,HideShowBtn_Homepage);
	}
}

void CBigMenuView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	pActiveWin->Handle (WM_SIZE,MENU_DRAW_PUTBITMAP,0);
}

void CBigMenuView::OnMouseMove(UINT nFlags, CPoint point) 
{
	DWORD wParam=MAKELONG(point.x,point.y);
	pActiveWin->Handle (WM_MOUSEMOVE,wParam,0);
	CView::OnMouseMove(nFlags, point);
}

void CBigMenuView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	DWORD wParam=MAKELONG(point.x,point.y);
	pActiveWin->Handle (WM_LBUTTONDOWN,wParam,0);
	CView::OnLButtonDown(nFlags, point);
}

void CBigMenuView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	PostMessage(WM_KEYDOWN,VK_ESCAPE);
}

void CBigMenuView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	DWORD wParam=MAKELONG(point.x,point.y);
	pActiveWin->Handle (WM_LBUTTONDBLCLK,wParam,0);
	CView::OnLButtonDblClk(nFlags, point);
}

void CBigMenuView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(pActiveWin->Handle (WM_KEYDOWN,nChar,nRepCnt) != -1) return;
	switch(nChar)
	{
	case VK_ESCAPE:
		if(g_nMenuStackTop==0) break;
		pMenuWin->SetMenuID(g_nMenuStack[g_nMenuStackTop-2]);
		pMenuWin->SetNow(g_nMenuStack[g_nMenuStackTop-1]);
		g_nMenuStackTop-=2;
		ChangeToPad(0);
		pActiveWin->Handle(UM_SHOW_PAD,1,0);
		break;
	default:
		break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CBigMenuView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	DWORD dwtmp = MAKELONG(nSBCode,nPos);
	pActiveWin->Handle(WM_HSCROLL,dwtmp, (LPARAM)pScrollBar);
	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CBigMenuView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	DWORD dwtmp = MAKELONG(nSBCode,nPos);
	pActiveWin->Handle(WM_VSCROLL,dwtmp, (LPARAM)pScrollBar);
	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

LRESULT CBigMenuView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	//先处理用户消息
	OnUserMessage(message,wParam,lParam);
	return CView::WindowProc(message, wParam, lParam);
}

void CBigMenuView::ChangeToPad(int which)
{
	pDeactiveWin = pActiveWin;
	nDeactiveWin = nActiveWin;
	switch(which)
	{
	case 0:
		pActiveWin = pMenuWin;
		nActiveWin = 0;
		break;
	case 1:
		pActiveWin = pBlockWin;
		nActiveWin = 1;
		break;
	}
	pDeactiveWin->Handle(UM_HIDE_PAD,0,0);
}

void CBigMenuView::OnUserMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
	switch(message)
	{
	case UM_SWAP_MENU:
		switch (wParam)
		{
			case MENU_LEAF:
				if(lParam >= L_DPFX_BASE+101 && lParam <= L_DPFX_BASE+119)
				{	//大盘分析
					pMainWnd->OnCommCommand(CMD_COM,lParam-L_DPFX_BASE);
					break;
				}
				else if(lParam >= L_INDEX_BASE+301 && lParam <= L_INDEX_BASE+322) 
				{	//技术分析
					pMainWnd->OnCommCommand(CMD_COM,lParam-L_INDEX_BASE);
					break;
				}
				else if(lParam >= L_XXM_BASE && lParam <= L_XXM_BASE+19)
				{	//公告消息
					int pos=lParam-L_XXM_BASE;
					if(pos<XXMNum)
						pMainWnd->OnCommCommand(CMD_NEWS,lParam-L_XXM_BASE+701);
					else
					{
						if(pos==XXMNum)
						{
							if(g_bHasInfo)
								pMainWnd->SendMessage(WM_COMMAND,IDC_ZXTREE);
							else if(g_bHasWebInfo||strlen(g_Url.WebInfoURL)>0)
								pMainWnd->SendMessage(WM_COMMAND,ID_WEBINFO);
							else if(g_bExtern_Info)
								pMainWnd->SendMessage(WM_COMMAND,ID_EXTERN_INFO);
						}
						else if(pos==XXMNum+1)
						{
							if(g_bHasWebInfo||strlen(g_Url.WebInfoURL)>0)
								pMainWnd->SendMessage(WM_COMMAND,ID_WEBINFO);
							else if(g_bExtern_Info)
								pMainWnd->SendMessage(WM_COMMAND,ID_EXTERN_INFO);							
						}
						else if(pos==XXMNum+2)
						{
							if(g_bExtern_Info)
								pMainWnd->SendMessage(WM_COMMAND,ID_EXTERN_INFO);							
						}
					}
					break;
				}
				else if(lParam >= L_BJFX_BASE && lParam <= L_BJFX_BASE+17)
				{	//报价分析
					pMainWnd->OnCommCommand(CMD_HQ,lParam-L_BJFX_BASE+1);
					break;
				}
				else if(lParam >= L_MULGP_BASE && lParam <= L_MULGP_BASE+17)
				{	//多股同列
					g_nMul_WhichGPType = lParam-L_MULGP_BASE;
					g_nMul_nBlockFlag = -1;
					pMainWnd->OnCommCommand(CMD_COM,900);
					break;
				}
				else if(lParam >= L_TBBD_BASE && lParam <= L_TBBD_BASE+8) 
				{	//特别报道
					if(g_nMenuSortType==228)
						pMainWnd->OnCommCommand(CMD_MAIN,81+lParam-L_TBBD_BASE);
					else
					{
						g_bOldGridDomain=FALSE;
						CGridView *tempWnd = (CGridView *)SwitchToView("Grid");						
						if(tempWnd)
							tempWnd->SendMessage(UM_TBBD_MESSAGE,g_nMenuSortType,lParam-L_TBBD_BASE);
						g_bOldGridDomain=TRUE;
					}
					break;
				}
				switch(lParam)
				{
				case L_JSFX_ZST:
				case L_JSFX_TICK:
				case L_JSFX_FJB:
				case L_JSFX_FSB:
				case L_JSFX_MMP:
				case L_JSFX_LB:
				case L_JSFX_MMLD:
				case L_JSFX_TICKT:
					pMainWnd->OnCommCommand(CMD_COM,lParam-L_JSFX_ZST+501);
					break;
				case L_WEBINFO:
					pMainWnd->SendMessage(WM_COMMAND,ID_WEBINFO);
					break;
				case L_WT:
					pMainWnd->SendMessage(WM_COMMAND,ID_FUNC_WT);
					break;
				case L_QUIT:
					pMainWnd->PostMessage(WM_CLOSE); //只能用PostMessage
					break;
				case L_RMREPORT:
					pMainWnd->OnCommCommand(CMD_HIS,400);
					break;
				case L_QJREPORT:
					pMainWnd->SendMessage(WM_COMMAND,ID_PHASESORT);
					break;
				case L_ZBREPORT:
					pMainWnd->SendMessage(WM_COMMAND,ID_ZBREPORT);
					break;
				case L_QRREPORT:
					pMainWnd->SendMessage(WM_COMMAND,ID_QRREPORT);
					break;
				case L_QJTYPE1:
				case L_QJTYPE2:
				case L_QJTYPE3:
				case L_QJTYPE4:
					pMainWnd->SendMessage(WM_COMMAND,ID_QJTYPE1+lParam-L_QJTYPE1);
					break;
				case L_FORMULA:
					pMainWnd->SendMessage(WM_COMMAND,ID_FORMULA);
					break;
				case L_TJTEST:
					pMainWnd->SendMessage(WM_COMMAND,ID_TJTEST);
					break;
				case L_JYTEST:
					pMainWnd->SendMessage(WM_COMMAND,ID_JYTEST);
					break;
				case L_TESTINDEX:
					pMainWnd->SendMessage(WM_COMMAND,ID_TESTINDEX);
					break;
				case L_CONDITIONCHOICE:
					pMainWnd->SendMessage(WM_COMMAND,ID_CONDITIONCHOICE);
					break;
				case L_CLECHOICE:
					pMainWnd->SendMessage(WM_COMMAND,ID_CLECHOICE);
					break;
				case L_PLUGIN:
					pMainWnd->SendMessage(WM_COMMAND,ID_PLUGIN);
					break;
				case L_SELGP:
					pMainWnd->SendMessage(WM_COMMAND,ID_SELGP);
					break;
				case L_SIM_CHOICE:
					pMainWnd->SendMessage(WM_COMMAND,ID_SIMCHOICE);
					break;
				case L_ALLWARNGP:
					pMainWnd->SendMessage(WM_COMMAND,ID_WARNGP);
					break;
				case L_ALLWARNCFG:
					pMainWnd->SendMessage(WM_COMMAND,ID_WARNCFG);
					break;
				case L_TJWARNGP:
					pMainWnd->SendMessage(WM_COMMAND,ID_TJWARNGP);
					break;
				case L_TJWARNCFG:
					pMainWnd->SendMessage(WM_COMMAND,ID_TJWARNCFG);
					break;					
				case L_DATADIG:
					pMainWnd->SendMessage(WM_COMMAND,ID_DATADIG);
					break;
				case L_BLOCKOPTION:
					pMainWnd->SendMessage(WM_COMMAND,ID_BLOCKOPTION);
					break;
				case L_OPTION:
					pMainWnd->SendMessage(WM_COMMAND,ID_OPTION);
					break;
				case L_INVEST:
					pMainWnd->SendMessage(WM_COMMAND,IDC_BROKER);
					break;
				case L_PROFIT:
					pMainWnd->SendMessage(WM_COMMAND,ID_FUNC_PROFIT);
					break;
				case L_BLOCKCALC:
					pMainWnd->SendMessage(WM_COMMAND,ID_FUNC_BLOCKCALC);
					break;
				case L_AUTOUP:
					pMainWnd->SendMessage(WM_COMMAND,ID_AUTOUP);
					break;
				case L_CONNECTCFG:
					pMainWnd->SendMessage(WM_COMMAND,ID_CONNECTCFG);
					break;
				case L_CONNECT:
					pMainWnd->PostMessage(WM_COMMAND,ID_CONNECT);
					break;
				case L_DISCONNECT:
					pMainWnd->SendMessage(WM_COMMAND,ID_DISCONNECT);
					break;
				case L_CONNECTINFO:
					pMainWnd->SendMessage(WM_COMMAND,ID_CONNECTINFO);
					break;
				case L_DISCONNECTINFO:
					pMainWnd->SendMessage(WM_COMMAND,ID_DISCONNECTINFO);
					break;
				case L_DOWNLOAD:
					pMainWnd->SendMessage(WM_COMMAND,ID_DOWNLOAD);
					break;
				case L_NODETOOL:
					pMainWnd->SendMessage(WM_COMMAND,ID_NODETOOL);
					break;
				case L_FORUM:
					pMainWnd->SendMessage(WM_COMMAND,ID_FORUM);
					break;
				case L_COOLINFO:
					pMainWnd->SendMessage(WM_COMMAND,ID_COOLINFO);
					break;
				case L_HELP_FINDER:
					pMainWnd->SendMessage(WM_COMMAND,ID_HELP_FINDER);
					break;
				}
				break;
			case MENU_BJFX_SELBLOCK:
				//进入报价分析的选择板块
				ChangeToPad(1);
				pBlockWin->SetType(VAL_BJFX_BLOCK);
				pActiveWin->Handle(UM_SHOW_PAD,1,0);
				break;
			case MENU_MULGP_SELBLOCK:
				//进入多股同列的选择板块
				ChangeToPad(1);
				pBlockWin->SetType(VAL_MULGP_BLOCK);
				pActiveWin->Handle(UM_SHOW_PAD,1,0);
				break;
			default:
				if(wParam==MENU_INDEX)
					g_nMenuPeriod = lParam;		 //保存中间菜单的周期
				else if(wParam==MENU_TBBD_LB)
					g_nMenuSortType = 201+lParam;//保存中间菜单的排列方法
				pMenuWin->SetMenuID(wParam);
				pMenuWin->SetNow(0);
				ChangeToPad(0);
				pActiveWin->Handle(UM_SHOW_PAD,1,0);
				break;
		}
		break;
	case UM_MENU_SETBLOCK:
		if(wParam==VAL_BJFX_BLOCK)
		{
			g_bOldGridDomain=FALSE;
			CGridView *tempWnd = (CGridView *)SwitchToView("Grid");						
			if(tempWnd)
				tempWnd->SendMessage(WM_CHANGEBLOCK,lParam);
			g_bOldGridDomain=TRUE;
		}
		else if(wParam==VAL_MULGP_BLOCK)
		{
			g_nMul_WhichGPType = -1;
			g_nMul_nBlockFlag = lParam;
			pMainWnd->OnCommCommand(CMD_COM,900);
		}
		break;
	case UM_MENU_INITPOS:
		pMenuWin->SetMenuID(0);
		pMenuWin->SetNow(0);
		g_nMenuStackTop=0;
		ChangeToPad(0);
		pActiveWin->Handle(UM_SHOW_PAD,0,0);
		break;
	default:
		break;
	}
}

BOOL CBigMenuView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_CHAR && KeyGuyFlag == FALSE )
	{
		if(ProcessKeyGuy(pMsg,this,KeyGuyFlag))
			return TRUE;
	}
	return CView::PreTranslateMessage(pMsg);
}

BOOL CBigMenuView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

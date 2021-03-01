// JJView.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "JJView.h"
#include "ViewManager.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJJView

IMPLEMENT_DYNCREATE(CJJView, CView)

CJJView::CJJView()
{
	KeyGuyFlag = FALSE;
	m_bQuitting = FALSE;
	m_pMemDC = new CDC ;
	m_pBm = new CBitmap;
}

CJJView::~CJJView()
{
	theViewManager.RemoveView(this);
	delete m_pMemDC;
	delete	m_pBm;
}


BEGIN_MESSAGE_MAP(CJJView, CView)
	//{{AFX_MSG_MAP(CJJView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(WM_KEYGUY_MESSAGE,OnKeyGuyMessage)
	ON_MESSAGE(UM_HSCROLLBAR,OnHScrollMessage)
	ON_MESSAGE(UM_VSCROLLBAR,OnVScrollMessage)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND_RANGE(ID_OPENFUND,ID_FUNDHOLDSTOCK,OnSelFundType)
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_RIGHT_OPEN,OnRightOpen)
	ON_COMMAND(ID_TOPSTOCK,OnTopStock)
	ON_COMMAND(ID_TOP10STOCK,OnTop10stock)
	ON_COMMAND(ID_TOPFUND,OnTopFund)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJJView drawing
//特殊的Invalidate方法,可设置OnDraw方式
//DRAW_ALL 1 DRAW_NOREADDATA 2
void CJJView::SpecialReDraw(int Flag)
{
	m_JJGrid.RefreshScrollBar();
	m_JJGrid.DrawFlag = Flag;
	Invalidate(FALSE);
}

void CJJView::OnDraw(CDC* pDC)
{
	if(g_pToolTipMark) 
		g_pToolTipMark->ClearToolTipInfo(this);
	m_JJGrid.DrawGrid(pDC);
}

void CJJView::OnDestroy() 
{
	m_bQuitting = TRUE;
	CView::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// CJJView diagnostics

#ifdef _DEBUG
void CJJView::AssertValid() const
{
	CView::AssertValid();
}

void CJJView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CJJView message handlers

BOOL CJJView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CJJView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	theViewManager.OnActivateView(bActivate, this);
	theViewManager.pOldActiveView2 = theViewManager.pActiveView;
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if(!m_bQuitting && bActivate)
	{
		AfxGetMainWnd()->SendMessage(UM_HIDESHOWBUTTON,HideShowBtn_Homepage);
	}
}

void CJJView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	Modify_CorpWebMenu();
	
	if(g_pToolTipMark) 
		g_pToolTipMark->AddTool(this, ""); 

	CString title = "基金分析";
	theViewManager.AddView(title, this);
	
	RefreshAllHq(120);

	m_JJGrid.RefreshScrollBar();
	m_JJGrid.InitGrid(0);
	m_JJGrid.SetCurLabel(0);
}

int CJJView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_JJGrid.Create(this,CRect(0,0,0,0));
	return 0;
}

void CJJView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	m_JJGrid.OnSize(nType,cx,cy);
}

void CJJView::OnKeyGuyMessage(WPARAM wParam,LPARAM lParam)
{
	if(wParam==0)
		m_JJGrid.InitGrid();
}

BOOL CJJView::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_CHAR && KeyGuyFlag == FALSE )
	{
		if(ProcessKeyGuy(pMsg,this,KeyGuyFlag))	//键盘精灵
			return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN) 
		{
			PostMessage(WM_LBUTTONDBLCLK);
		}
		else  if(pMsg->wParam == VK_TAB)
		{
			if(FiltTabKeyCom()) return TRUE;
			SHORT CtrlDown=::GetKeyState(VK_CONTROL);
			if(CtrlDown >= 0)
				m_JJGrid.ProcessMsg(WM_KEYDOWN,VK_TAB,0);
		}
	}
	if(g_pToolTipMark) 
		g_pToolTipMark->RelayEvent(pMsg);	
	return CView::PreTranslateMessage(pMsg);
}

void CJJView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CRect rect;
	GetClientRect(rect);
	CClientDC dc(this);
	SpecialReDraw(DRAW_NOREADDATA);
	
	if (m_pMemDC->GetSafeHdc()) m_pMemDC->DeleteDC();
	m_pMemDC->CreateCompatibleDC(&dc);	
	m_pMemDC->SetMapMode(MM_ANISOTROPIC);
	m_pMemDC->SetWindowExt(dc.GetDeviceCaps(HORZRES),dc.GetDeviceCaps(VERTRES));
	m_pMemDC->SetViewportExt(m_pMemDC->GetDeviceCaps(HORZRES),m_pMemDC->GetDeviceCaps(VERTRES));
	
	if (m_pBm->GetSafeHandle()) m_pBm->DeleteObject();
	m_pBm->CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());
	m_pMemDC->SelectObject(m_pBm);
	dc.DPtoLP(rect);					//Convert to Logical Coordinates
	m_PrintRect= rect;					//Save Logical Coordinates
	m_pMemDC->BitBlt(0,0,rect.Width(),rect.Height(),&dc,0,0,SRCCOPY);
	g_d.SelectPen(m_pMemDC,LEVEL_PEN);
	g_d.DrawRect(m_pMemDC,1,1,rect.Width()-1,rect.Height()-1);
}

void CJJView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
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

void CJJView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CView::OnEndPrinting(pDC, pInfo);
}

void CJJView::OnHScrollMessage(WPARAM wParam,LPARAM lParam)
{
	m_JJGrid.OnHScrollMessage(wParam,lParam);
}

void CJJView::OnVScrollMessage(WPARAM wParam,LPARAM lParam)
{
	m_JJGrid.OnVScrollMessage(wParam,lParam);
}

void CJJView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	m_JJGrid.OnKeyDown(nChar,nRepCnt,nFlags);
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CJJView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_JJGrid.OnLButtonDown(nFlags,point);
	CView::OnLButtonDown(nFlags, point);
}

void CJJView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CClientDC dc(this);
	//如果在行的范围中，则弹出行的右键菜单.
	m_JJGrid.OnClick(&dc,point,TRUE);
	CView::OnRButtonUp(nFlags, point);
}

BOOL CJJView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	if(g_d.IsBlackColorScheme(VipColor.BACKCOLOR))
	{
		if(TDX_MessageBox (m_hWnd,"背景太暗,建议将系统背景色调节白底再打印,要强制打印吗?",MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL)
			return FALSE;
	}
	
	PRINTDLG pd;									//该结构包含打印对话框中的所有信息 
	LPDEVMODE lpDevMode; 
	if(AfxGetApp()->GetPrinterDeviceDefaults(&pd)) //获得默认的打印机的信息 
	{ 
		lpDevMode=(LPDEVMODE)GlobalLock(pd.hDevMode); 
		if(lpDevMode) 
			lpDevMode->dmOrientation=DMORIENT_LANDSCAPE; //将打印机设置为横向打印。 
		GlobalUnlock(pd.hDevMode); 
	}
	
	return CView::OnPreparePrinting(pInfo);
}

void CJJView::OnFilePrintPreview() 
{
	UpdateWindow();	
	CView::OnFilePrintPreview();
}


BOOL CJJView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(zDelta < 0)
		OnKeyDown(VK_DOWN,1,0);
	else
		OnKeyDown(VK_UP,1,0);
	
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CJJView::OnSelFundType(UINT nID)
{
	m_JJGrid.InitGrid(nID-ID_OPENFUND);
	m_JJGrid.SetCurLabel(nID-ID_OPENFUND);
}

void CJJView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{	
	m_JJGrid.OnLButtonDbClick(nFlags,point);
	CView::OnLButtonDblClk(nFlags, point);
}

void CJJView::SelectSomeRow(const char* lpszSymbol)
{
	m_JJGrid.SelectSomeRow(lpszSymbol);
}

short CJJView::GetCurrentGPIndex()
{
	short setcode;
	const char* lpszCode = m_JJGrid.GetStockCode(setcode);
	if(NULL == lpszCode) return -1;
	return g_pStockIO->GetIndex(lpszCode,setcode);	
}

void CJJView::OnRightOpen()
{
	m_JJGrid.ProcessMsg(WM_COMMAND,ID_RIGHT_OPEN);
}

void CJJView::OnTopStock()
{
	m_JJGrid.ProcessMsg(WM_COMMAND,ID_TOPSTOCK);
}

void CJJView::OnTop10stock()
{
	m_JJGrid.ProcessMsg(WM_COMMAND,ID_TOP10STOCK);
}

void CJJView::OnTopFund()
{
	m_JJGrid.ProcessMsg(WM_COMMAND,ID_TOPFUND);
}

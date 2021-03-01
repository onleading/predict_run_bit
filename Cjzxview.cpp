// HisView.cpp : implementation file

#include "stdafx.h"
#include "tdxw.h"
#include "CjzxView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_CJZX_DIRECT_SEP		100000000L

/////////////////////////////////////////////////////////////////////////////
// CCjzxView

IMPLEMENT_DYNCREATE(CCjzxView, CView)

CCjzxView::CCjzxView()
{
	KeyGuyFlag = FALSE;
	m_bQuitting = FALSE;
}

CCjzxView::~CCjzxView()
{
	theViewManager.RemoveView(this);
}

BEGIN_MESSAGE_MAP(CCjzxView, CView)
	//{{AFX_MSG_MAP(CCjzxView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_KEYGUY_MESSAGE,OnKeyGuyMessage)
	ON_MESSAGE(UM_CJZX_MESSAGE,OnCjzxMessage)
	ON_MESSAGE(UM_CJZX_DAY_MESSAGE,OnCjzxDayMessage)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCjzxView drawing

/////////////////////////////////////////////////////////////////////////////
// CCjzxView diagnostics

#ifdef _DEBUG
void CCjzxView::AssertValid() const
{
	CView::AssertValid();
}

void CCjzxView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCjzxView message handlers

BOOL CCjzxView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CCjzxView::OnInitialUpdate()
{
	CView::OnInitialUpdate();	
	Modify_CorpWebMenu();
		
	CString title = "财经资讯";
	theViewManager.AddView(title, this);
	GetParent()->SetWindowText (_F("财经资讯"));
	INFO_ShowFunc(m_hWnd,IGN_CJZX,FALSE,1,0,NULL,0,NULL,NULL,0,0); //缺省打开券商发表
}

void CCjzxView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	theViewManager.OnActivateView(bActivate, this);
	g_bSepEsc = FALSE;
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if(!m_bQuitting && bActivate)
		AfxGetMainWnd()->SendMessage(UM_HIDESHOWBUTTON,HideShowBtn_Homepage);
}

BOOL CCjzxView::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_CHAR && KeyGuyFlag == FALSE )
	{
		if(ProcessKeyGuy(pMsg,this,KeyGuyFlag))
			return TRUE;
	}
	return CView::PreTranslateMessage(pMsg);
}

int CCjzxView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CCjzxView::OnDestroy() 
{
	m_bQuitting = TRUE;
	INFO_ProcessMsg(IGN_CJZX,NULL,WM_DESTROY,0,0);
	CView::OnDestroy();
}

void CCjzxView::OnKeyGuyMessage(WPARAM wParam,LPARAM lParam)
{
}

void CCjzxView::OnCjzxMessage(WPARAM wParam,LPARAM lParam)
{
	if(wParam>=960 && wParam<=969) //如果是公告消息,转到NewsView窗口
	{
		CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
		pMainWnd->OnCommCommand(CMD_NEWS,wParam-960+701);	
		return;
	}
	if(wParam>=MAX_CJZX_DIRECT_SEP)
	{
		if(wParam<MAX_CJZX_DIRECT_SEP+MAX_INFO_TITLE)
			INFO_ShowCjzx2Content(m_hWnd,g_pCJZX2Title+(wParam-MAX_CJZX_DIRECT_SEP));
	}
	else
		INFO_ShowFunc(m_hWnd,IGN_CJZX,FALSE,wParam,0,NULL,0,NULL,NULL,0,0);
}

void CCjzxView::OnCjzxDayMessage(WPARAM wParam,LPARAM lParam)
{
	INFO_ShowFunc(m_hWnd,IGN_CJZX,FALSE,wParam,0,NULL,0,NULL,NULL,0,lParam);
}

void CCjzxView::OnDraw(CDC* pDC) 
{	
}

LRESULT CCjzxView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	return CView::WindowProc(message, wParam, lParam);
}

short CCjzxView::GetCurrentGPIndex()
{
	return g_pStockIO->GetIndex(CjzxCurGPCode,CjzxCurSetCode);
}

void CCjzxView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	const MSG *pMsg=CWnd::GetCurrentMessage();
	if(pMsg->hwnd==m_hWnd)
	{
		INFO_ProcessMsg(IGN_CJZX,pMsg->hwnd,pMsg->message,pMsg->wParam,pMsg->lParam);
	}
}

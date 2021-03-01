// HomePageView.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "HomePageView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHomePageView

IMPLEMENT_DYNCREATE(CHomePageView, CHtmlView)

CHomePageView::CHomePageView()
{
	KeyGuyFlag = FALSE;
	m_bQuitting = FALSE;	
}

CHomePageView::~CHomePageView()
{
	theViewManager.RemoveView(this);
}

void CHomePageView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHomePageView)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CHomePageView, CHtmlView)
	//{{AFX_MSG_MAP(CHomePageView)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_MESSAGE(UM_RELOAD_MSG,OnReloadMsg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CHomePageView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CHtmlView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CHomePageView::OnInitialUpdate() 
{
	CHtmlView::OnInitialUpdate();

	theViewManager.AddView("首页", this);
	theViewManager.SetViewName("首页", this);
//	GetParent()->SetWindowText(_F("首页"));
	AfxGetMainWnd()->SetWindowText(g_strTitleName);

	PostMessage(UM_RELOAD_MSG,0,0);
//	SetTimer(1, 60*1000, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CHomePageView diagnostics

#ifdef _DEBUG
void CHomePageView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CHomePageView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHomePageView message handlers

void CHomePageView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	theViewManager.OnActivateView(bActivate, this);
	if(!g_bSepEsc)	//如果是特殊ESC逻辑,则不用行情
		theViewManager.pOldActiveView2 = theViewManager.pActiveView;
	g_bSepEsc = FALSE;
	//在历史分析和行情图中，当激活之时，自动保存旧的激活视图
	CHtmlView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if(!m_bQuitting && bActivate)
	{
		AfxGetMainWnd()->SendMessage(UM_HIDESHOWBUTTON,HideShowBtn_Homepage);
	}
}

void CHomePageView::OnDestroy() 
{
	m_bQuitting = TRUE;
	KillTimer(1);
	CHtmlView::OnDestroy();	
}

BOOL CHomePageView::OnEraseBkgnd(CDC* pDC) 
{
    return TRUE;	
}

void CHomePageView::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==1)
		PostMessage(UM_RELOAD_MSG,0,0);
	
	CView::OnTimer(nIDEvent);
}

//判断IE上是否能输入字符等,通过查询两个接口
BOOL CanInput(CHtmlView *pHtmlView)
{
	CComPtr<IHTMLDocument2> pDoc2;
	CComPtr<IDispatch> spDisp = pHtmlView->GetHtmlDocument();
	if(!spDisp) return FALSE;
	HRESULT hr=spDisp->QueryInterface(&pDoc2);
	if(FAILED(hr) || !pDoc2) return FALSE;
	CComPtr<IHTMLElement> pElement;
	hr=pDoc2->get_activeElement(&pElement);
	if(FAILED(hr) || !pElement) return FALSE;

	VARIANT_BOOL bText; 
	pElement->get_isTextEdit(&bText); 
	return (bText == VARIANT_TRUE);
}

BOOL CHomePageView::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_RBUTTONUP||pMsg->message==WM_RBUTTONDOWN)
		return TRUE;
	if(pMsg->message==WM_MBUTTONUP||pMsg->message==WM_MBUTTONDOWN)   
		return TRUE;   
	if(pMsg->message == WM_CHAR && KeyGuyFlag == FALSE)
	{
		if(/*!CanInput(this) && */ProcessKeyGuy(pMsg,this,KeyGuyFlag))
			return TRUE;
	}
	return CHtmlView::PreTranslateMessage(pMsg);
}

short CHomePageView::GetCurrentGPIndex()
{
	return g_pStockIO->GetIndex(ComCurGPCode,ComCurSetCode);
}	

void CHomePageView::OnReloadMsg(WPARAM wParam,LPARAM lParam)
{
	try
	{
		SetSilent(TRUE);
		Navigate(g_Scheme_King.HomePage);
	}
	catch(...)
	{
	}
}

BEGIN_EVENTSINK_MAP(CHomePageView, CHtmlView)
    //{{AFX_EVENTSINK_MAP(CHomePageView)
	ON_EVENT(CHomePageView, AFX_IDW_PANE_FIRST, 263 /* WindowClosing */, OnWindowClosingExplorer1, VTS_BOOL VTS_PBOOL)
	ON_EVENT(CHomePageView, AFX_IDW_PANE_FIRST, 250 /* BeforeNavigate2 */, OnBeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CHomePageView::OnWindowClosingExplorer1(BOOL IsChildWindow, BOOL FAR* Cancel) 
{
	//不能用户在网页中关闭
	*Cancel=TRUE;
}

void CHomePageView::OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel) 
{	
	USES_CONVERSION;

	char url[501]={0};
	strncpy(url,W2A(URL->bstrVal),500);
	if(BeforeNavigate_Process(url))
		*Cancel = TRUE;
}


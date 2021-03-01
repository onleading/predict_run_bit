// IEBrowserView.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "IEBrowserView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIEBrowserView

IMPLEMENT_DYNCREATE(CIEBrowserView, CHtmlView)

CIEBrowserView::CIEBrowserView()
{
	theViewManager.AddView("F10资讯", this);
	
	m_bQuitting = FALSE;

	m_strNeedUnlinkFile = "";
}

CIEBrowserView::~CIEBrowserView()
{
	theViewManager.RemoveView(this);
	if(m_strNeedUnlinkFile.GetLength()>0)	
	{
		unlink(m_strNeedUnlinkFile);
		m_strNeedUnlinkFile="";
	}	
}

void CIEBrowserView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIEBrowserView)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CIEBrowserView, CHtmlView)
	//{{AFX_MSG_MAP(CIEBrowserView)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_COMMAND(ID_WEB_BACK,OnWebBack)
	ON_COMMAND(ID_WEB_FORWARD,OnWebForward)
	ON_COMMAND(ID_WEB_STOP,OnWebStop)
	ON_COMMAND(ID_WEB_REFRESH,OnWebRefresh)
	ON_COMMAND(ID_WEB_CLOSE,OnWebClose)
	ON_MESSAGE(WM_WEBINFO_MESSAGE,OnWebInfo_Message)
	ON_WM_TIMER()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CIEBrowserView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CHtmlView::OnCreate(lpCreateStruct) == -1)
		return -1;

    // 创建工具栏
    CMDIChildWnd * pFrame = (CMDIChildWnd*)GetParentFrame();
	if (!m_wndIEToolBar.CreateEx(pFrame, TBSTYLE_FLAT|TBSTYLE_TRANSPARENT,WS_CHILD | WS_VISIBLE | CBRS_TOP |CBRS_BORDER_3D | CBRS_TOOLTIPS,CRect(0,0,0,0),ID_IETOOLS) ||
		!m_wndIEToolBar.LoadToolBar(IDR_IE))
    {
        TRACE0("Failed to create doc view ");
        return -1;      // fail to create
    }
	CImageList	imageList;
	CBitmap		bitmap;
	bitmap.LoadBitmap(IDR_IE);
	imageList.Create(16, 16, ILC_COLORDDB|ILC_MASK, 71, 1);
	imageList.Add(&bitmap, SRGB(192));
	m_wndIEToolBar.SendMessage(TB_SETIMAGELIST, 0, (LPARAM)imageList.m_hImageList);
	imageList.Detach();
	bitmap.Detach();
    // 停靠设置
    m_wndIEToolBar.EnableDocking(CBRS_ALIGN_ANY);
    pFrame->EnableDocking(CBRS_ALIGN_ANY);
    pFrame->DockControlBar(&m_wndIEToolBar);
	
	return 0;
}

void CIEBrowserView::OnInitialUpdate() 
{
	CHtmlView::OnInitialUpdate();
	Modify_CorpWebMenu();
		
	SetToolBar(TRUE);
	SetStatusBar(TRUE);
	GetParent()->SetWindowText (_F("基本信息"));
}

void CIEBrowserView::OnWebBack()
{
	if(m_strNeedUnlinkFile.GetLength()>0)
		return;

	try	//不让其出现错误对话框
	{
		GoBack();
	}
	catch(...)
	{
	}
}

void CIEBrowserView::OnWebForward()
{
	if(m_strNeedUnlinkFile.GetLength()>0)
		return;

	try	//不让其出现错误对话框
	{
		GoForward();
	}
	catch(...)
	{
	}
}

void CIEBrowserView::OnWebStop()
{
	if(m_strNeedUnlinkFile.GetLength()>0)
		return;

	try	//不让其出现错误对话框
	{
		Stop();
	}
	catch(...)
	{
	}
}

void CIEBrowserView::OnWebRefresh()
{
	if(m_strNeedUnlinkFile.GetLength()>0)
		return;

	try	//不让其出现错误对话框
	{
		Refresh();
	}
	catch(...)
	{
	}	
}

void CIEBrowserView::OnWebClose()
{
//	CFrameWnd *tempFrameWnd = (CFrameWnd *)(GetParent());
//	if(tempFrameWnd)
//		tempFrameWnd->SendMessage(WM_CLOSE);	
	AfxGetMainWnd()->PostMessage(WM_KEYDOWN, VK_ESCAPE);
}

/////////////////////////////////////////////////////////////////////////////
// CIEBrowserView diagnostics

#ifdef _DEBUG
void CIEBrowserView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CIEBrowserView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIEBrowserView message handlers

void CIEBrowserView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	theViewManager.OnActivateView(bActivate, this);
	g_bSepEsc = FALSE;
	CHtmlView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if(!m_bQuitting && bActivate)
		AfxGetMainWnd()->SendMessage(UM_HIDESHOWBUTTON,HideShowBtn_WebInfo);
}

void CIEBrowserView::OnDestroy() 
{
	m_bQuitting = TRUE;
	CHtmlView::OnDestroy();	
}

BOOL CIEBrowserView::OnEraseBkgnd(CDC* pDC) 
{
    return TRUE;	
}

void CIEBrowserView::OnWebInfo_Message(WPARAM wParam,LPARAM lParam)
{
	try	//不让其出现错误对话框
	{
		//SetSilent(TRUE); //此处不能SetSilent,否则很多弹出的对话框不能显示 
		Navigate(WebURL);
		SetTimer(0x10,500,NULL);//抢回焦点的定时器
		if(WebURL.Find("zzc.html")>=0 && WebURL.Find("wt_cache")>=0)
		{
			m_strNeedUnlinkFile = WebURL;
			SetTimer(0x100,5*1000,NULL);//定时删除敏感的临时文件
		}
	}
	catch(...)
	{
	}
}

BOOL CIEBrowserView::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_RBUTTONUP||pMsg->message==WM_RBUTTONDOWN)
		return TRUE;
	if(pMsg->message==WM_MBUTTONUP||pMsg->message==WM_MBUTTONDOWN)   
		return TRUE;   
	return CHtmlView::PreTranslateMessage(pMsg);
}

short CIEBrowserView::GetCurrentGPIndex()
{
	return g_pStockIO->GetIndex(WebInfoCurGPCode,WebInfoCurSetCode);
}	

void CIEBrowserView::OnTimer(UINT nIDEvent) 
{
	//抢回焦点的定时器
	if(nIDEvent==0x10)	
	{
		CFrameWnd *tempFrameWnd = (CFrameWnd *)GetParent();
		tempFrameWnd->SetFocus();
		KillTimer(0x10);
	}
	//定时删除敏感的临时文件
	if(nIDEvent==0x100 && m_strNeedUnlinkFile.GetLength()>0)	
	{
		unlink(m_strNeedUnlinkFile);
		m_strNeedUnlinkFile="";
		KillTimer(0x100);
	}	
	CHtmlView::OnTimer(nIDEvent);
}

BEGIN_EVENTSINK_MAP(CIEBrowserView, CHtmlView)
    //{{AFX_EVENTSINK_MAP(CIEBrowserView)
	ON_EVENT(CIEBrowserView, AFX_IDW_PANE_FIRST, 263 /* WindowClosing */, OnWindowClosingExplorer1, VTS_BOOL VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CIEBrowserView::OnWindowClosingExplorer1(BOOL IsChildWindow, BOOL FAR* Cancel) 
{
	//不能用户在网页中关闭
	*Cancel=TRUE;
}


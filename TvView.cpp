// TvView.cpp : implementation file

#include "stdafx.h"
#include "tdxw.h"
#include "TvView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTvView

IMPLEMENT_DYNCREATE(CTvView, CView)

CTvView::CTvView()
{
	KeyGuyFlag = FALSE;
	m_bQuitting = FALSE;
}

CTvView::~CTvView()
{
	theViewManager.RemoveView(this);
}

BEGIN_MESSAGE_MAP(CTvView, CView)
	//{{AFX_MSG_MAP(CTvView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_KEYGUY_MESSAGE,OnKeyGuyMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTvView drawing

/////////////////////////////////////////////////////////////////////////////
// CTvView diagnostics

#ifdef _DEBUG
void CTvView::AssertValid() const
{
	CView::AssertValid();
}

void CTvView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTvView message handlers

BOOL CTvView::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect (&rect,GridColor.GridBackColor);
	return TRUE;
}

void CTvView::OnInitialUpdate()
{
	CView::OnInitialUpdate();	
	Modify_CorpWebMenu();

	CString title = "TV功能";
	theViewManager.AddView(title, this);
	GetParent()->SetWindowText (_F("TV功能"));
}

void CTvView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	theViewManager.OnActivateView(bActivate, this);
	if(!g_bSepEsc)	//如果是特殊ESC逻辑,则不用行情
		theViewManager.pOldActiveView2 = theViewManager.pActiveView;	
	g_bSepEsc = FALSE;
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if(!m_bQuitting && bActivate)
	{
		AfxGetMainWnd()->SendMessage(UM_HIDESHOWBUTTON,HideShowBtn_Homepage);
	}
}

BOOL CTvView::ProcessGPCode(const char *code)
{
	return FALSE;
}

short CTvView::GetCurrentGPIndex()
{
	return -1;
}

BOOL CTvView::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_CHAR && KeyGuyFlag == FALSE )
	{
		if(ProcessKeyGuy(pMsg,this,KeyGuyFlag))
			return TRUE;
	}
	return CView::PreTranslateMessage(pMsg);
}

int CTvView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CTvView::OnDestroy() 
{
	m_bQuitting = TRUE;
	CView::OnDestroy();
}

void CTvView::OnKeyGuyMessage(WPARAM wParam,LPARAM lParam)
{
}

void CTvView::OnDraw(CDC* pDC) 
{	
}

LRESULT CTvView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	return CView::WindowProc(message, wParam, lParam);
}

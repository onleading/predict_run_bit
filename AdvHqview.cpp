// HisView.cpp : implementation file

#include "stdafx.h"
#include "tdxw.h"
#include "AdvHqView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAdvHqView

IMPLEMENT_DYNCREATE(CAdvHqView, CView)

CAdvHqView::CAdvHqView()
{
	KeyGuyFlag = FALSE;
	m_bQuitting = FALSE;
}

CAdvHqView::~CAdvHqView()
{
	theViewManager.RemoveView(this);
}

BEGIN_MESSAGE_MAP(CAdvHqView, CView)
	//{{AFX_MSG_MAP(CAdvHqView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_KEYGUY_MESSAGE,OnKeyGuyMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAdvHqView drawing

/////////////////////////////////////////////////////////////////////////////
// CAdvHqView diagnostics

#ifdef _DEBUG
void CAdvHqView::AssertValid() const
{
	CView::AssertValid();
}

void CAdvHqView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAdvHqView message handlers

BOOL CAdvHqView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CAdvHqView::OnInitialUpdate()
{
	CView::OnInitialUpdate();	
	Modify_CorpWebMenu();

	CString title = "高级行情功能";
	theViewManager.AddView(title, this);
	GetParent()->SetWindowText (_F("高级行情功能"));
}

void CAdvHqView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
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

BOOL CAdvHqView::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_CHAR && KeyGuyFlag == FALSE )
	{
		if(ProcessKeyGuy(pMsg,this,KeyGuyFlag))
			return TRUE;
	}
	return CView::PreTranslateMessage(pMsg);
}

int CAdvHqView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CAdvHqView::OnDestroy() 
{
	m_bQuitting = TRUE;
	CView::OnDestroy();
}

void CAdvHqView::OnKeyGuyMessage(WPARAM wParam,LPARAM lParam)
{
}

BOOL CAdvHqView::ProcessGPCode(const char *code,short setcode)
{
	return TRUE;
}

void CAdvHqView::OnDraw(CDC* pDC) 
{	

}

LRESULT CAdvHqView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==WM_DRAWITEM || message==WM_MEASUREITEM)
		return TRUE;
	return CView::WindowProc(message, wParam, lParam);
}

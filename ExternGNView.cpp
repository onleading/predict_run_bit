// ExternGNView.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "ExternGNView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExternGNView

IMPLEMENT_DYNCREATE(CExternGNView, CView)

CExternGNView::CExternGNView()
{
}

CExternGNView::~CExternGNView()
{
	theViewManager.RemoveView(this);
}


BEGIN_MESSAGE_MAP(CExternGNView, CView)
	//{{AFX_MSG_MAP(CExternGNView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_KEYGUY_MESSAGE,OnKeyGuyMessage)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExternGNView drawing

void CExternGNView::OnDraw(CDC* pDC)
{
}

/////////////////////////////////////////////////////////////////////////////
// CExternGNView diagnostics

#ifdef _DEBUG
void CExternGNView::AssertValid() const
{
	CView::AssertValid();
}

void CExternGNView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CExternGNView message handlers

void CExternGNView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	theViewManager.OnActivateView(bActivate, this);
	
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CExternGNView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	Modify_CorpWebMenu();
		
	CString title = "外部高级功能";
	theViewManager.AddView(title, this);
	GetParent()->SetWindowText (_F("外部高级功能"));
}

BOOL CExternGNView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

int CExternGNView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
		
	return 0;
}

void CExternGNView::OnDestroy() 
{
	g_ExternGNCtrl.ExitThis(this);

	CView::OnDestroy();
}

void CExternGNView::OnKeyGuyMessage(WPARAM wParam,LPARAM lParam)
{
	char errMsg[128]={0};
	g_ExternGNCtrl.ShowThis((HWND)wParam, (long)lParam, errMsg);
	CRect rc;
	GetClientRect(&rc);
	g_ExternGNCtrl.ResizeThis(this, rc.Width(), rc.Height());
}

void CExternGNView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	g_ExternGNCtrl.ResizeThis(this, cx, cy);	
}

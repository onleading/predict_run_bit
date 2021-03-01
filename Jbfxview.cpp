#include "stdafx.h"
#include "tdxw.h"
#include "JbfxView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJbfxView

IMPLEMENT_DYNCREATE(CJbfxView, CComView2)

CJbfxView::CJbfxView()
{
	KeyGuyFlag = FALSE;
	m_bQuitting = FALSE;
	m_bLayoutFlag = FALSE;
}

CJbfxView::~CJbfxView()
{
	theViewManager.RemoveView(this);
}

BEGIN_MESSAGE_MAP(CJbfxView, CComView2)
	//{{AFX_MSG_MAP(CJbfxView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJbfxView drawing

/////////////////////////////////////////////////////////////////////////////
// CJbfxView diagnostics

#ifdef _DEBUG
void CJbfxView::AssertValid() const
{
	CComView2::AssertValid();
}

void CJbfxView::Dump(CDumpContext& dc) const
{
	CComView2::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CJbfxView message handlers


int CJbfxView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComView2::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_bLayoutFlag = TRUE;

	return 0;
}

void CJbfxView::OnInitialUpdate()
{
	Modify_CorpWebMenu();
		
	CString title = "基本分析";
	theViewManager.AddView(title, this);
	GetParent()->SetWindowText (_F("基本分析"));
}

void CJbfxView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	theViewManager.OnActivateView(bActivate, this);
	g_bSepEsc = FALSE;
	CComView2::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if(!m_bQuitting && bActivate)
		AfxGetMainWnd()->SendMessage(UM_HIDESHOWBUTTON,HideShowBtn_Homepage);
}


BOOL CJbfxView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

BOOL CJbfxView::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_CHAR && KeyGuyFlag == FALSE )
	{
		if(ProcessKeyGuy(pMsg,this,KeyGuyFlag))
			return TRUE;
	}
	return CComView2::PreTranslateMessage(pMsg);
}

void CJbfxView::OnDestroy() 
{
	m_bQuitting = TRUE;
	CView::OnDestroy();
}

//////////////////////////////////////////////////////////////////////////
//针对继承的
void CJbfxView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
}

// CG: This file was added by the Splash Screen component.
// Splash.cpp : implementation file

#include "stdafx.h"
#include "resource.h"

#include "Splash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class
CSplashWnd::CSplashWnd()
{
	Create();
}

CSplashWnd::~CSplashWnd()
{

}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// 创建不可见框架窗口：设置 WS_EX_TOOLWINDOW 式样
BOOL CSplashWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// 设置 WS_EX_TOOLWINDOW 扩展式样
	if (CWnd::PreCreateWindow(cs)) {
		cs.dwExStyle |= WS_EX_TOOLWINDOW;
		return TRUE;
	}
	return FALSE;
}

BOOL CSplashWnd::Create(CWnd* pParentWnd /*= NULL*/)
{
	CString bmpfile = HomePath+"splash.bmp";
	hBitmap = LoadImage(NULL,bmpfile,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	if(hBitmap == NULL)
		return FALSE;
	m_bitmap.Attach((HBITMAP)hBitmap);

	BITMAP bm;
	m_bitmap.GetBitmap(&bm);

	BOOL bCreateSuc = CreateEx(0,
		AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_POPUP | WS_VISIBLE, 0, 0, bm.bmWidth, bm.bmHeight, pParentWnd->GetSafeHwnd(), NULL);

	if(bCreateSuc)
		return TRUE;

	return FALSE;
}

void CSplashWnd::OnClose()
{
	AnimateWindow(GetSafeHwnd(), 3000, AW_BLEND | AW_HIDE);

	CWnd::OnClose();
}

int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CenterWindow();
	AnimateWindow(GetSafeHwnd(), 3000, AW_BLEND);

	return TRUE;
}

BOOL CSplashWnd::OnEraseBkgnd(CDC *pDC)
{
	CDC dcImage;
	if (!dcImage.CreateCompatibleDC(pDC))
		return FALSE;
	
	BITMAP bm;
	m_bitmap.GetBitmap(&bm);
	
	// Paint the image.
	CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmap);
	pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
	dcImage.SelectObject(pOldBitmap);
	
	return TRUE;
}
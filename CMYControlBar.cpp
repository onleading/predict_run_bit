// CMYControlBar.cpp : implementation file
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CMYControlBar.h"
#include <afxpriv.h>    // for CDockContext
#include "resource.h"
#include "MyInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyControlBar

CMyControlBar::CMyControlBar()
{
	// 注册当前类
	RegisterWindowClass(NULL,_T("MyControlBar"));

    m_sizeMin		= CSize(32, 32);
    m_sizeHorz		= CSize(200, 200);
    m_sizeVert		= CSize(200, 200);
#ifdef OEM_NEWJY
	if(g_nDefaultRestHeight>400)
		m_sizeFloat		= CSize(980, g_nDefaultRestHeight);
	else
		m_sizeFloat		= CSize(832, g_nDefaultRestHeight);
#else
    m_sizeFloat		= CSize(760, 386);
#endif
    m_bTracking		= FALSE;
    m_bInRecalcNC	= FALSE;
    m_cxEdge		= 3;
	m_menuID		= 0;
	m_bGripper		= TRUE;
	m_bButtons		= TRUE;

	m_bFullSize		= FALSE;

	m_bFlatBtn		= TRUE;
	m_bHasSizeBtn	= FALSE;
	m_bRightBtn		= TRUE;

	m_pCloseBt		= NULL;
}

BOOL CMyControlBar::RegisterWindowClass(HINSTANCE hInstance,LPCTSTR lpszClassName)
{
	WNDCLASS WndClass={0};
	ZeroMemory(&WndClass,sizeof(WndClass));
	WndClass.style=CS_PARENTDC|CS_DBLCLKS;
	WndClass.lpfnWndProc=::DefWindowProc;
	WndClass.hInstance=hInstance?hInstance:(AfxGetModuleState()->m_hCurrentInstanceHandle);
	WndClass.hCursor=AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	WndClass.lpszClassName=lpszClassName;
	WndClass.hIcon=NULL;
	return AfxRegisterClass(&WndClass);
}

CMyControlBar::~CMyControlBar()
{
	TDEL(m_pCloseBt);
}

IMPLEMENT_DYNAMIC(CMyControlBar, CControlBar)

BEGIN_MESSAGE_MAP(CMyControlBar, CControlBar)
	//{{AFX_MSG_MAP(CMyControlBar)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_NCPAINT()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCHITTEST()
	ON_WM_NCCALCSIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_CAPTURECHANGED()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(IDC_BUTTON_HIDE, OnButtonClose)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_HIDE, OnUpdateButtonClose)
	ON_COMMAND(IDC_BUTTON_MINI, OnButtonMinimize)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_MINI, OnUpdateButtonMinimize)
	ON_COMMAND(IDC_BUTTON_13SIZE, OnButton13Size)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_13SIZE, OnUpdateButton13Size)
	ON_COMMAND(IDC_BUTTON_23SIZE, OnButton23Size)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_23SIZE, OnUpdateButton23Size)
	ON_COMMAND(IDC_BUTTON_FULLSIZE, OnButtonFullSize)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_FULLSIZE, OnUpdateButtonFullSize)	
	ON_WM_CONTEXTMENU()
	ON_MESSAGE(UM_MYBITMAPBUTTON,OnBitmapBt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyControlBar message handlers

void CMyControlBar::OnUpdateCmdUI(CFrameWnd * pTarget, BOOL bDisableIfNoHndler)
{
    UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

void CMyControlBar::SetGripper(BOOL bHasGripper)
{
	m_bGripper = bHasGripper;
}

void CMyControlBar::SetRightBtn(BOOL bRightBtn)
{
	m_bRightBtn = bRightBtn;	
}

BOOL CMyControlBar::Create(CWnd * pParentWnd, UINT nID, LPCTSTR lpszWindowName, CSize sizeDefault, DWORD dwStyle)
{
    ASSERT_VALID(pParentWnd);   // must have a parent
    ASSERT (((dwStyle & CBRS_SIZE_FIXED)   != CBRS_SIZE_FIXED) && 
	    	((dwStyle & CBRS_SIZE_DYNAMIC) != CBRS_SIZE_DYNAMIC));

    // save the style
    SetBarStyle(dwStyle & CBRS_ALL);

    CString wndclass = ::AfxRegisterWndClass(CS_DBLCLKS,
        ::LoadCursor(NULL, IDC_ARROW),
        ::GetSysColorBrush(COLOR_BTNFACE), 0);

    m_sizeHorz = m_sizeVert = sizeDefault;

    dwStyle &= ~CBRS_ALL;
    dwStyle &= WS_VISIBLE | WS_CHILD;
    
	return CWnd::Create(wndclass, lpszWindowName, dwStyle,
		CRect(0,0,0,0), pParentWnd, nID);
}

//flag=0表示不支持左右停靠 =1表示不支持上下停靠
void CMyControlBar::DisableDockPos(int flag)
{
	DWORD dwStyle=GetBarStyle();
	if(flag==0)
	{
		dwStyle &= ~CBRS_ALIGN_LEFT;
		dwStyle &= ~CBRS_ALIGN_RIGHT;
	}
	else
	{
		dwStyle &= ~CBRS_ALIGN_TOP;
		dwStyle &= ~CBRS_ALIGN_BOTTOM;
	}
	SetBarStyle(dwStyle);
}

CSize CMyControlBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
    CRect rc;

    m_pDockSite->GetControlBar(AFX_IDW_DOCKBAR_TOP)->GetWindowRect(rc);
    int nHorzDockBarWidth = bStretch ? 32767 : rc.Width() + 4;
    m_pDockSite->GetControlBar(AFX_IDW_DOCKBAR_LEFT)->GetWindowRect(rc);
    int nVertDockBarHeight = bStretch ? 32767 : rc.Height() + 4;

    if (bHorz)
        return CSize(nHorzDockBarWidth, m_sizeHorz.cy);
    else
        return CSize(m_sizeVert.cx, nVertDockBarHeight);
}

CSize CMyControlBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
    if (dwMode & (LM_HORZDOCK | LM_VERTDOCK))
    {
        if (nLength == -1)
            GetDockingFrame()->DelayRecalcLayout();
        return CControlBar::CalcDynamicLayout(nLength,dwMode);
    }

    if (dwMode & LM_MRUWIDTH)
        return m_sizeFloat;

    if (dwMode & LM_COMMIT)
    {
        m_sizeFloat.cx = nLength;
        return m_sizeFloat;
    }

    if (dwMode & LM_LENGTHY)
        return CSize(m_sizeFloat.cx,m_sizeFloat.cy = max(m_sizeMin.cy, nLength));
    else
        return CSize(max(m_sizeMin.cx, nLength), m_sizeFloat.cy);
}

void CMyControlBar::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CControlBar::OnWindowPosChanged(lpwndpos);
	
    // Find on which side are we docked
	m_nDockBarID = GetParent()->GetDlgCtrlID();

    if (m_bInRecalcNC == FALSE)
	{
        m_bInRecalcNC = TRUE;

        // Force recalc the non-client area
        SetWindowPos(NULL, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE |
            SWP_NOACTIVATE | SWP_NOZORDER |
            SWP_FRAMECHANGED);

        m_bInRecalcNC = FALSE;
    }

	if (m_bButtons && m_bGripper)
	{
		ASSERT(m_ImageList);

		if (IsFloating()) 
		{
			m_btnClose.ShowWindow(SW_HIDE);
			m_btnMinim.ShowWindow(SW_HIDE);
			if(m_pCloseBt)
				m_pCloseBt->ShowWindow(SW_HIDE);
			if(m_bHasSizeBtn)
			{
				m_btn1P3Size.ShowWindow(SW_HIDE);
				m_btn2P3Size.ShowWindow(SW_HIDE);				
				m_btnFullSize.ShowWindow(SW_HIDE);				
				m_btnBack.ShowWindow(SW_HIDE);				
				m_btnForward.ShowWindow(SW_HIDE);				
				m_btnMainPage.ShowWindow(SW_HIDE);				
			}
			return;
		}
		else 
		{
			if(g_bTopTabWorkSpace)			
				m_pCloseBt->ShowWindow(SW_SHOW);
			else
				m_btnClose.ShowWindow(SW_SHOW);
			if(IsHorzDocked())
				m_btnMinim.ShowWindow(SW_SHOW);
			else
				m_btnMinim.ShowWindow(SW_HIDE);
			if(m_bHasSizeBtn)
			{
				m_btn1P3Size.ShowWindow(SW_SHOW);
				m_btn2P3Size.ShowWindow(SW_SHOW);				
				m_btnFullSize.ShowWindow(SW_SHOW);	
				m_btnBack.ShowWindow(SW_SHOW);				
				m_btnForward.ShowWindow(SW_SHOW);				
				m_btnMainPage.ShowWindow(SW_SHOW);								
			}
		}

		CRect rcClose(GetButtonRect());
		CRect rcMinim(GetButtonRect());

		if (IsHorzDocked()) 
		{
			rcMinim.OffsetRect(0,14);
			m_btnMinim.SetIcon(m_ImageList->ExtractIcon(2),CSize(13,13));
		}
		else 
		{
			if(g_bTopTabWorkSpace)
				m_pCloseBt->MoveWindow(rcClose);
			rcClose.OffsetRect(14,0);
			m_btnMinim.SetIcon(m_ImageList->ExtractIcon(1),CSize(13,13));
		}
		m_btnClose.MoveWindow(rcClose);
		m_btnMinim.MoveWindow(rcMinim);
		if(m_bHasSizeBtn)
		{
			if (IsHorzDocked())			
			{
				rcMinim.OffsetRect(0,14);
				m_btn1P3Size.MoveWindow(rcMinim);
				rcMinim.OffsetRect(0,14);
				m_btn2P3Size.MoveWindow(rcMinim);
				rcMinim.OffsetRect(0,14);
				m_btnFullSize.MoveWindow(rcMinim);

				rcMinim.OffsetRect(0,14);
				m_btnBack.MoveWindow(rcMinim);
				rcMinim.OffsetRect(0,14);
				m_btnForward.MoveWindow(rcMinim);
				rcMinim.OffsetRect(0,14);
				m_btnMainPage.MoveWindow(rcMinim);
			}
			else
			{
				rcClose.OffsetRect(14,0);
				m_btn1P3Size.MoveWindow(rcClose);
				rcClose.OffsetRect(14,0);
				m_btn2P3Size.MoveWindow(rcClose);
				rcClose.OffsetRect(14,0);
				m_btnFullSize.MoveWindow(rcClose);

				rcClose.OffsetRect(14,0);
				m_btnBack.MoveWindow(rcClose);
				rcClose.OffsetRect(14,0);
				m_btnForward.MoveWindow(rcClose);
				rcClose.OffsetRect(14,0);
				m_btnMainPage.MoveWindow(rcClose);

			}
		}
	}
	Invalidate();
}

BOOL CMyControlBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
    if ((nHitTest != HTSIZE) || m_bTracking)
        return CControlBar::OnSetCursor(pWnd, nHitTest, message);

#if defined(MFCXLIB_STATIC) 
	HINSTANCE hInst = AfxFindResourceHandle(
			MAKEINTRESOURCE(AFX_IDC_VSPLITBAR), RT_GROUP_CURSOR);

    if (IsHorzDocked())
        ::SetCursor(::LoadCursor(hInst, MAKEINTRESOURCE(AFX_IDC_VSPLITBAR)));
    else
        ::SetCursor(::LoadCursor(hInst, MAKEINTRESOURCE(AFX_IDC_HSPLITBAR)));
#else
    if (IsHorzDocked())
        ::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
    else
        ::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
#endif
    return TRUE;
}

BOOL CMyControlBar::IsHorzDocked() const
{
    return (m_nDockBarID == AFX_IDW_DOCKBAR_TOP ||
        m_nDockBarID == AFX_IDW_DOCKBAR_BOTTOM);
}

BOOL CMyControlBar::IsVertDocked() const
{
    return (m_nDockBarID == AFX_IDW_DOCKBAR_LEFT ||
        m_nDockBarID == AFX_IDW_DOCKBAR_RIGHT);
}

void CMyControlBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
    // Compute the rectangle of the mobile edge
    GetWindowRect(m_rectBorder);
    m_rectBorder.OffsetRect(-m_rectBorder.left, -m_rectBorder.top);
    m_rectBorder.DeflateRect(1,1);
    
    CRect rcWnd = lpncsp->rgrc[0];
    DWORD dwBorderStyle = m_dwStyle | CBRS_BORDER_ANY;

    switch (m_nDockBarID)
    {
    case AFX_IDW_DOCKBAR_TOP:
		{
			dwBorderStyle &= ~CBRS_BORDER_BOTTOM;
			rcWnd.DeflateRect(1, 1, 1, m_cxEdge + 1);
			m_rectBorder.top = m_rectBorder.bottom - m_cxEdge;
			break;
		}
    case AFX_IDW_DOCKBAR_BOTTOM:
		{
			dwBorderStyle &= ~CBRS_BORDER_TOP;
			rcWnd.DeflateRect(1, m_cxEdge + 1, 1, 1);
			m_rectBorder.bottom = m_rectBorder.top + m_cxEdge;
			lpncsp->rgrc[0].left += 10;
			break;
		}
    case AFX_IDW_DOCKBAR_LEFT:
		{
			dwBorderStyle &= ~CBRS_BORDER_RIGHT;
			rcWnd.DeflateRect(1, 1, m_cxEdge + 1, 1);
			m_rectBorder.left = m_rectBorder.right - m_cxEdge;
			break;
		}
    case AFX_IDW_DOCKBAR_RIGHT:
		{
			dwBorderStyle &= ~CBRS_BORDER_LEFT;
			rcWnd.DeflateRect(m_cxEdge + 1, 1, 1, 1);
			m_rectBorder.right = m_rectBorder.left + m_cxEdge;
			break;
		}
    default:
		{
			m_rectBorder.SetRectEmpty();
			break;
		}
    }

    lpncsp->rgrc[0] = rcWnd;

    SetBarStyle(dwBorderStyle);
}

void CMyControlBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	DrawGripper(&dc);
}

void CMyControlBar::OnNcPaint() 
{
	CDC *pDC=GetWindowDC();

	CRect rectWindow;
	GetWindowRect(rectWindow);
	ScreenToClient(rectWindow);

	CRect rectClient;
	GetClientRect(rectClient);
	rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
	pDC->ExcludeClipRect(rectClient);
	
    // draw borders in non-client area
	rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
	//	DrawBorders(&dc, rectWindow);
	DrawBorders(pDC, rectWindow);
	
    // erase parts not drawn
	pDC->IntersectClipRect(rectWindow);
	
    // erase NC background the hard way
	pDC->FillRect(rectWindow, &CBrush(::GetSysColor(COLOR_BTNFACE)));
	
    // paint the mobile edge
    pDC->Draw3dRect(m_rectBorder, ::GetSysColor(COLOR_BTNHIGHLIGHT),
        ::GetSysColor(COLOR_BTNSHADOW));
	ReleaseDC(pDC);
}

UINT CMyControlBar::OnNcHitTest(CPoint point) 
{
    if (IsFloating())
        return CControlBar::OnNcHitTest(point);

    CRect rc;
    GetWindowRect(rc);
    point.Offset(-rc.left, -rc.top);

    if (m_rectBorder.PtInRect(point))
        return HTSIZE;
    else
        return HTCLIENT;
}

BOOL CMyControlBar::IsFloating()
{
	return (!IsHorzDocked() && !IsVertDocked());
}

void CMyControlBar::StartTracking()
{
    SetCapture();

    // make sure no updates are pending
    RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW);
    m_pDockSite->LockWindowUpdate();

    m_ptOld = m_rectBorder.CenterPoint();
    m_bTracking = TRUE;
    
    m_rectTracker = m_rectBorder;
    if (!IsHorzDocked()) m_rectTracker.bottom -= 4;

    OnInvertTracker(m_rectTracker);
}

void CMyControlBar::StopTracking(BOOL bAccept)
{
    OnInvertTracker(m_rectTracker);
    m_pDockSite->UnlockWindowUpdate();
    m_bTracking = FALSE;
    ReleaseCapture();
    
    if (!bAccept)
		return;

    int maxsize, minsize, newsize;
    CRect rcc;
    m_pDockSite->GetClientRect(rcc);

    newsize = IsHorzDocked() ? m_sizeHorz.cy : m_sizeVert.cx;
    maxsize = newsize + (IsHorzDocked() ? rcc.Height() : rcc.Width());
    minsize = IsHorzDocked() ? m_sizeMin.cy : m_sizeMin.cx;

    CPoint point = m_rectTracker.CenterPoint();
    switch (m_nDockBarID)
    {
    case AFX_IDW_DOCKBAR_TOP:
		{
			newsize += point.y - m_ptOld.y;
			break;
		}
    case AFX_IDW_DOCKBAR_BOTTOM:
		{
			newsize += -point.y + m_ptOld.y;
			break;
		}
    case AFX_IDW_DOCKBAR_LEFT:
		{
			newsize += point.x - m_ptOld.x;
			break;
		}
    case AFX_IDW_DOCKBAR_RIGHT:
		{
			newsize += -point.x + m_ptOld.x;
			break;
		}
    }

    newsize = max(minsize, min(maxsize, newsize));

    if (IsHorzDocked())
        m_sizeHorz.cy = newsize;
    else
        m_sizeVert.cx = newsize;

    m_pDockSite->DelayRecalcLayout();
}

void CMyControlBar::OnInvertTracker(const CRect &rect)
{
    ASSERT_VALID(this);
    ASSERT(!rect.IsRectEmpty());
    ASSERT(m_bTracking);

    CRect rct = rect, rcc, rcf;
    GetWindowRect(rcc);
    m_pDockSite->GetWindowRect(rcf);

    rct.OffsetRect(rcc.left - rcf.left, rcc.top - rcf.top);
    rct.DeflateRect(1, 1);

    CDC *pDC = m_pDockSite->GetDCEx(NULL,
        DCX_WINDOW|DCX_CACHE|DCX_LOCKWINDOWUPDATE);

    CBrush* pBrush = CDC::GetHalftoneBrush();
    HBRUSH hOldBrush = NULL;
    if (pBrush != NULL)
        hOldBrush = (HBRUSH)SelectObject(pDC->m_hDC, pBrush->m_hObject);

    pDC->PatBlt(rct.left, rct.top, rct.Width(), rct.Height(), PATINVERT);

    if (hOldBrush != NULL)
        SelectObject(pDC->m_hDC, hOldBrush);

    m_pDockSite->ReleaseDC(pDC);
}

void CMyControlBar::OnCaptureChanged(CWnd *pWnd) 
{
    if (m_bTracking && pWnd != this)
        StopTracking(FALSE); // cancel tracking
	
	CControlBar::OnCaptureChanged(pWnd);
}

void CMyControlBar::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
    if (m_bTracking)
		return;
    
    if ((nHitTest == HTSIZE) && !IsFloating())
        StartTracking();
    else    
		CControlBar::OnNcLButtonDown(nHitTest, point);
}

void CMyControlBar::OnMouseMove(UINT nFlags, CPoint point) 
{
    if (IsFloating() || !m_bTracking) {
        CControlBar::OnMouseMove(nFlags, point);
        return;
    }

    CPoint cpt = m_rectTracker.CenterPoint();
    ClientToWnd(point);

    if (IsHorzDocked()) {
        if (cpt.y != point.y) {
            OnInvertTracker(m_rectTracker);
            m_rectTracker.OffsetRect(0, point.y - cpt.y);
            OnInvertTracker(m_rectTracker);
        }
    }

    else {
        if (cpt.x != point.x) {
            OnInvertTracker(m_rectTracker);
            m_rectTracker.OffsetRect(point.x - cpt.x, 0);
            OnInvertTracker(m_rectTracker);
        }
    }
}

void CMyControlBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
    if (m_pDockBar != NULL)
    {
        // start the drag
        ASSERT(m_pDockContext != NULL);
        ClientToScreen(&point);
        //m_pDockContext->StartDrag(point);
    }
    else
		CControlBar::OnLButtonDown(nFlags, point);
}

void CMyControlBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
    if (!m_bTracking)
        CControlBar::OnLButtonUp(nFlags, point);
    else
    {
        ClientToWnd(point);
        StopTracking(TRUE);
    }
}

void CMyControlBar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	PostMessage(WM_COMMAND,IDC_BUTTON_MINI);
}

CPoint& CMyControlBar::ClientToWnd(CPoint &point)
{
    point.Offset(1, 1);

    if (m_nDockBarID == AFX_IDW_DOCKBAR_BOTTOM)
        point.y += m_cxEdge;
    else if (m_nDockBarID == AFX_IDW_DOCKBAR_RIGHT)
        point.x += m_cxEdge;

    return point;
}

CRect CMyControlBar::GetButtonRect()
{
	CRect pRect;
	GetClientRect(pRect);
	if(IsHorzDocked()) 
	{
		pRect.top	 += 0;
		pRect.bottom = pRect.top+12;
		if(m_bRightBtn)
			pRect.left  = pRect.right-12;
		else
			pRect.right = pRect.left+12;
	}
	else
	{
		if(g_bTopTabWorkSpace)
		{
			pRect.right -= 1;
			pRect.left   = pRect.right-16;
			pRect.top   += 3;
			pRect.bottom = pRect.top+16;
		}
		else
		{
			pRect.right -= 14;
			pRect.left   = pRect.right-12;
			pRect.top   += 1;
			pRect.bottom = pRect.top+12;
		}
	}
	return pRect;
}

CRect CMyControlBar::GetGripperRect()
{
	CRect pRect;
	GetClientRect(pRect);
	pRect.OffsetRect(-pRect.left,-pRect.top);

	if(IsHorzDocked()) {
		pRect.DeflateRect(1,1);
		if(m_bRightBtn)
		{
			pRect.left = pRect.right-9;
			pRect.right= pRect.left+3;
		}
		else
		{
			pRect.left		+= 1;
			pRect.right		 = pRect.left+3;
		}		
		pRect.bottom	-= 1;
		if(!m_bHasSizeBtn)
			pRect.top		+= m_bButtons?28:1;
		else
			pRect.top		+= m_bButtons?56:1;
	}
	else {
		pRect.DeflateRect(1,1);
		pRect.left		+= 2;	
		pRect.top		+= 2;
		pRect.bottom	 = pRect.top+3;
		if(!m_bHasSizeBtn)
			pRect.right		-= m_bButtons?15:2;
		else
			pRect.right		-= m_bButtons?56:2;
	}

	return pRect;
}

void CMyControlBar::DrawGripper(CDC* pDC)
{
	if(g_bTopTabWorkSpace)
	{
		CRect pRect;
		GetClientRect(pRect);
		pRect.DeflateRect(1,1,0,0);
		pRect.bottom = pRect.top+23;
		pDC->FillSolidRect(pRect.left,pRect.bottom-1,pRect.Width(),1,RGB(180,180,180));
		CRect tmprc=pRect;
		tmprc.DeflateRect(0,0,0,1);
		COLORREF cor0,cor1;
		cor0=g_GUI.GetColorCfgFromResource(g_resModule,"IDR_COLOR","CONTROLBAR_BK",0);
		cor1=g_GUI.GetColorCfgFromResource(g_resModule,"IDR_COLOR","CONTROLBAR_BK",1);
		g_GUI.DrawGradient(pDC,tmprc,cor0,cor1,FALSE);

		GetClientRect(pRect);
		pRect.right=pRect.left+2;
		pDC->FillSolidRect(pRect,::GetSysColor(COLOR_BTNFACE));
	}
	else
	{
		if (IsFloating())
			return;
		if(m_bGripper)
		{
			CRect pRect;
			GetClientRect(pRect);
			pRect.OffsetRect(-pRect.left,-pRect.top);
			if(IsHorzDocked()) 
			{
				if(!m_bRightBtn)			
					pRect.right = pRect.left+14;
				else
					pRect.left = pRect.right-14;
			}
			else
			{
				pRect.bottom = pRect.top+14;
			}
			pDC->FillSolidRect(pRect,::GetSysColor(COLOR_BTNFACE));
		}
		if (m_bGripper && !IsHorzDocked())
		{
			CRect pRect;
			GetClientRect(pRect);
			pRect.OffsetRect(-pRect.left,-pRect.top);
			pRect.bottom = pRect.top+14;
			g_GUI.DrawGradient(pDC,pRect,RGB(220,220,220),RGB(168,168,168),FALSE);		
			GetClientRect(pRect);
			pRect.OffsetRect(-pRect.left,-pRect.top);
			pRect.right=pRect.left+2;
			pDC->FillSolidRect(pRect,::GetSysColor(COLOR_BTNFACE));
		}
	}
	m_pDockSite->RecalcLayout();
}

void CMyControlBar::SetControlBarStyle(BOOL bFlatBtn,BOOL bHasSizeBtn)
{
	m_bFlatBtn = bFlatBtn;
	m_bHasSizeBtn = bHasSizeBtn;
}

int CMyControlBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (m_bButtons)
	{
		ASSERT(m_ImageList);

		if(!m_btnClose.Create(NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP |
			BS_NOTIFY | BS_OWNERDRAW | BS_ICON,
			CRect(0,0,0,0), this, IDC_BUTTON_HIDE ))
		{
			TRACE0("Unable to create CMyControlBar close button\n");
			return -1;
		}
		if(g_bTopTabWorkSpace)
		{
			m_btnClose.ShowWindow(SW_HIDE);
			HINSTANCE tmphi=AfxGetResourceHandle();
			AfxSetResourceHandle(g_resModule);
			LPCTSTR temp[9]={0,0,"IDB_CONTROLBAR_CLOSEBT_N",
							0,0,"IDB_CONTROLBAR_CLOSEBT_D",
							0,0,"IDB_CONTROLBAR_CLOSEBT_O"};		
			m_pCloseBt=new MyBitmapBt(ID_CONTROLBAR_CLOSE,"",NormalBt,0,TRUE);
			m_pCloseBt->SetBK(temp);
			m_pCloseBt->Create(NULL,"",WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,ID_CONTROLBAR_CLOSE);
			AfxSetResourceHandle(tmphi);
		}
		if(!m_btnMinim.Create(NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP |
			BS_NOTIFY | BS_OWNERDRAW | BS_ICON,
			CRect(0,0,0,0), this, IDC_BUTTON_MINI ))
		{
			TRACE0("Unable to create CMyControlBar close button\n");
			return -1;
		}		
		if(m_bHasSizeBtn)
		{
			if(!m_btn1P3Size.Create(NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP |
				BS_NOTIFY | BS_OWNERDRAW | BS_ICON,
				CRect(0,0,0,0), this, IDC_BUTTON_13SIZE ))
			{
				TRACE0("Unable to create CMyControlBar close button\n");
				return -1;
			}
			if(!m_btn2P3Size.Create(NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP |
				BS_NOTIFY | BS_OWNERDRAW | BS_ICON,
				CRect(0,0,0,0), this, IDC_BUTTON_23SIZE ))
			{
				TRACE0("Unable to create CMyControlBar close button\n");
				return -1;
			}
			if(!m_btnFullSize.Create(NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP |
				BS_NOTIFY | BS_OWNERDRAW | BS_ICON,
				CRect(0,0,0,0), this, IDC_BUTTON_FULLSIZE ))
			{
				TRACE0("Unable to create CMyControlBar close button\n");
				return -1;
			}
			if(!m_btnBack.Create(NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP |
				BS_NOTIFY | BS_OWNERDRAW | BS_ICON,
				CRect(0,0,0,0), this, IDC_BUTTON_BACK ))
			{
				TRACE0("Unable to create CMyControlBar close button\n");
				return -1;
			}
			if(!m_btnForward.Create(NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP |
				BS_NOTIFY | BS_OWNERDRAW | BS_ICON,
				CRect(0,0,0,0), this, IDC_BUTTON_FORWARD ))
			{
				TRACE0("Unable to create CMyControlBar close button\n");
				return -1;
			}
			if(!m_btnMainPage.Create(NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP |
				BS_NOTIFY | BS_OWNERDRAW | BS_ICON,
				CRect(0,0,0,0), this, IDC_BUTTON_MAINPAGE ))
			{
				TRACE0("Unable to create CMyControlBar close button\n");
				return -1;
			}
			m_btn1P3Size.SetIcon(m_ImageList->ExtractIcon(3),CSize(13,13));
			m_btn2P3Size.SetIcon(m_ImageList->ExtractIcon(4),CSize(13,13));
			m_btnFullSize.SetIcon(m_ImageList->ExtractIcon(5),CSize(13,13));
			m_btnBack.SetIcon(m_ImageList->ExtractIcon(6),CSize(13,13));
			m_btnForward.SetIcon(m_ImageList->ExtractIcon(7),CSize(13,13));
			m_btnMainPage.SetIcon(m_ImageList->ExtractIcon(8),CSize(13,13));
		}
		if(!m_bFlatBtn)
		{
			m_btnClose.DisableFlatLook();
			m_btnMinim.DisableFlatLook();
		}
		m_btnClose.SetIcon(m_ImageList->ExtractIcon(0),CSize(13,13));
		// Create the ToolTip control.
		m_ToolTip.Create(this);
		m_ToolTip.Activate(TRUE);		
		m_ToolTip.AddTool (&m_btnClose, _T("隐藏"));
		m_ToolTip.AddTool (&m_btnMinim, _T("浮动"));
		if(m_bHasSizeBtn)
		{
			m_ToolTip.AddTool (&m_btn1P3Size, _T("1/3整屏"));
			m_ToolTip.AddTool (&m_btn2P3Size, _T("2/3整屏"));
			m_ToolTip.AddTool (&m_btnFullSize, _T("整屏"));
			m_ToolTip.AddTool (&m_btnBack, _T("后退"));
			m_ToolTip.AddTool (&m_btnForward, _T("前进"));
			m_ToolTip.AddTool (&m_btnMainPage, _T("回到主页"));
		}
	}
	
	return 0;
}

CImageList* CMyControlBar::SetBtnImageList(CImageList *pImageList)
{
	CImageList* pPrevList = m_ImageList;
	m_ImageList = pImageList;
	return pPrevList;
}

void CMyControlBar::OnButtonClose()
{
	GetDockingFrame()->ShowControlBar(this, FALSE, FALSE);
	theViewManager.pActiveView->SetFocus();
}

void CMyControlBar::OnUpdateButtonClose(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}

void CMyControlBar::OnButtonMinimize()
{
	if(!m_bFullSize)
	{
		if(IsFloating())
			m_pDockContext->ToggleDocking();
		else
		{
			CPoint pt;
			pt.x=(ScreenCx-m_sizeFloat.cx)/2;
			pt.y=(ScreenCy-m_sizeFloat.cy)/2;
			m_pDockSite->FloatControlBar(this,pt);
		}
	}
}

void CMyControlBar::OnUpdateButtonMinimize(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}

void CMyControlBar::ResetSize()
{
	PostMessage(WM_COMMAND,IDC_BUTTON_13SIZE);
}

int CMyControlBar::GetHeight()
{
	return m_sizeHorz.cy;
}

void CMyControlBar::OnButton13Size()
{
	m_bFullSize = FALSE;
    CRect rcc;
    m_pDockSite->GetClientRect(rcc);
    if (IsHorzDocked())
		m_sizeHorz.cy = g_nDefaultRestHeight;
    else
        m_sizeVert.cx = (rcc.Width()+30)/3;

    m_pDockSite->DelayRecalcLayout();
}

void CMyControlBar::OnUpdateButton13Size(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}

void CMyControlBar::OnButton23Size()
{
	m_bFullSize = FALSE;
    CRect rcc;
    m_pDockSite->GetClientRect(rcc);
    if (IsHorzDocked())
	{
		if(ScreenCx<1024)
			m_sizeHorz.cy = 305;
		else
			m_sizeHorz.cy = 420;
	}
    else
        m_sizeVert.cx = 2*(rcc.Width()-52)/3; 

    m_pDockSite->DelayRecalcLayout();
}

void CMyControlBar::OnUpdateButton23Size(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}

void CMyControlBar::OnButtonFullSize()
{
	m_bFullSize = TRUE;
    CRect rcc;
    m_pDockSite->GetClientRect(rcc);
    if (IsHorzDocked())
		m_sizeHorz.cy = rcc.Height()-15;
    else
        m_sizeVert.cx = rcc.Width()-15; 

    m_pDockSite->DelayRecalcLayout();
}

void CMyControlBar::OnUpdateButtonFullSize(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(true);
}

//
void CMyControlBar::OnMainFrameSize()
{
	if(m_bFullSize)
		OnButtonFullSize();
}

BOOL CMyControlBar::IsFullSize()
{
	return m_bFullSize;
}

void CMyControlBar::SwitchToFullSize(BOOL bFull)
{
	if(bFull)
		OnButtonFullSize();
	else
		OnButton13Size();
}

//

BOOL CMyControlBar::PreTranslateMessage(MSG* pMsg) 
{
	if (m_bButtons)
		m_ToolTip.RelayEvent(pMsg);
	return CControlBar::PreTranslateMessage(pMsg);
}

void CMyControlBar::SetMenuID(UINT nID)
{
	m_menuID = nID;
}

UINT CMyControlBar::GetMenuID()
{
	return m_menuID;
}

void CMyControlBar::OnContextMenu(CWnd* /*pWnd*/, CPoint point) 
{
	// if no menu, just return.
	if (m_menuID == 0 ) {
		TRACE0("No control bar menu defined.\n");
		return;
	}

	if (point.x == -1 && point.y == -1)
	{
		//keystroke invocation
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);
		
		point = rect.TopLeft();
		point.Offset(5, 5);
	}
	
	CNewMenu menu;
	VERIFY(menu.LoadMenu(m_menuID));
	
	CNewMenu* pPopup =(CNewMenu*) menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;
	
	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();
	
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
		pWndPopupOwner);
}

BOOL CMyControlBar::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CMyControlBar::OnBitmapBt(WPARAM wParam, LPARAM lParam)
{
	switch((UINT)lParam)  
	{
	case ID_CONTROLBAR_CLOSE:
		{
			PostMessage(WM_COMMAND,MAKEWPARAM(IDC_BUTTON_HIDE,BN_CLICKED),(LPARAM)(m_btnClose.m_hWnd));
		}
		break;
	}
}

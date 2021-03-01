// IConButton.cpp : implementation file
//

#include "stdafx.h"
 #include "IConButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIconButton

CIconButton::CIconButton()
{
	m_hIcon=NULL;
	m_bHover=false;
	m_bEnableBorder=true;
	m_clrBkColor=GetSysColor(COLOR_3DFACE);
	m_clrHoverColor=RGB(181,189,214);
    m_clrDisableColor=RGB(132,132,132);
}

CIconButton::~CIconButton()
{
}


BEGIN_MESSAGE_MAP(CIconButton, CButton)
	//{{AFX_MSG_MAP(CIconButton)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_ERASEBKGND()
	ON_WM_KILLFOCUS()


END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIconButton message handlers

void CIconButton::OnKillFocus( CWnd* pNewWnd ) 
{
   m_bHover=false;
   Invalidate();
}


BOOL CIconButton::OnEraseBkgnd( CDC* pDC )
{
	return FALSE;
}



void CIconButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
   
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	CRect rc=lpDrawItemStruct->rcItem;
	dc.FillSolidRect(rc,m_clrBkColor);
	if(m_bHover)
	{
		dc.FillSolidRect(rc,m_clrHoverColor);
	}
	
	if(lpDrawItemStruct->itemState&ODS_DISABLED)
	{
		dc.FillSolidRect(rc,m_clrDisableColor);
	}
	
	   
	
	int x,y;
	x=(rc.Width()-16)/2;
	y=(rc.Height()-16)/2;
	if(m_hIcon)
		::DrawIconEx(dc,x,y,m_hIcon,16,16,0,0,DI_NORMAL);
    if(m_bEnableBorder)
   {
	   dc.MoveTo(rc.left,rc.top);
	   dc.LineTo(rc.left,rc.bottom-1);
	   dc.LineTo(rc.right-1,rc.bottom-1);
	   dc.LineTo(rc.right-1,rc.top);
	   dc.LineTo(rc.left,rc.top);
   }
   else if(m_bHover)
   {
       dc.MoveTo(rc.left,rc.top);
	   dc.LineTo(rc.left,rc.bottom-1);
	   dc.LineTo(rc.right-1,rc.bottom-1);
	   dc.LineTo(rc.right-1,rc.top);
	   dc.LineTo(rc.left,rc.top);
   }

   dc.Detach();
   
}

void CIconButton::SetIcon(HICON hIcon)
{
   m_hIcon=hIcon;
   if(::IsWindow(m_hWnd))
   {
	   Invalidate();
   }
}

void CIconButton::OnMouseMove(UINT nFlags, CPoint point) 
{
 	if (! m_bHover) {
          // draw with button borders
          m_bHover = true;
          Invalidate();
          // remember to remove button borders when we leave
          TRACKMOUSEEVENT trackmouseevent;
          trackmouseevent.cbSize = sizeof(trackmouseevent);
          trackmouseevent.dwFlags = TME_LEAVE;
          trackmouseevent.hwndTrack = GetSafeHwnd();
          trackmouseevent.dwHoverTime = HOVER_DEFAULT;
          _TrackMouseEvent(&trackmouseevent);
     }
	//CButton::OnMouseMove(nFlags, point);
}

LONG CIconButton::OnMouseLeave(WPARAM wParam,LPARAM lParam)
{
	m_bHover=false;
	Invalidate();
     return 0;
}

 

void CIconButton::SetBkColor(COLORREF clr)
{
    m_clrBkColor=clr;
	if(::IsWindow(m_hWnd))
	{
		Invalidate();
	}
}

void CIconButton::SetHoverColor(COLORREF clr)
{
    m_clrHoverColor=clr;
	if(::IsWindow(m_hWnd))
	{
		Invalidate();
	}
}

void CIconButton::SetDisableColor(COLORREF clr)
{ 
	m_clrDisableColor=clr;
	if(::IsWindow(m_hWnd))
	{
		Invalidate();
	}

}

void CIconButton::EnableBorder(bool bEnable)
{
   m_bEnableBorder=bEnable;
   if(::IsWindow(m_hWnd))
	{
		Invalidate();
	}
}

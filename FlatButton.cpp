// FlatButton.cpp : implementation file
//

#include "stdafx.h"
 #include "FlatButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFlatButton

CFlatButton::CFlatButton()
{
	m_bHover=false;
	m_bChecked=false;
	m_clrBkColor=GetSysColor(COLOR_3DFACE);
	m_clrTextColor=RGB(0,0,0);
	m_clrSelColor=RGB(255,255,0);
	m_clrHoverColor=m_clrBkColor;//RGB(190,143,65);
	m_pFont = NULL;
}

CFlatButton::~CFlatButton()
{
}


BEGIN_MESSAGE_MAP(CFlatButton, CButton)
	//{{AFX_MSG_MAP(CFlatButton)
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlatButton message handlers

void CFlatButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    CDC dc,memDC; 
	CRect rect=lpDrawItemStruct->rcItem; 
	CString strCaption; 
	dc.Attach(lpDrawItemStruct->hDC); //得到绘制的设备环境CDC 
	COLORREF colortxt;
	if(m_bChecked)
	{
		colortxt=m_clrSelColor;
	}
	else
	{
	   colortxt=m_clrTextColor;
	}
	
	CPen penBorder(PS_SOLID,1,m_clrTextColor),*pOldPen;
		
	CFont *pFont=NULL,*pOldFont=NULL;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp,*pOldBmp=NULL;
	bmp.CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());
	pOldBmp=memDC.SelectObject(&bmp);
	memDC.SetTextColor(colortxt);
	
	pFont = m_pFont;
	//设成和父窗口一样的字体
	CWnd *pWnd=GetParent();
	if(pWnd)
	{
		if(!pFont) pFont=pWnd->GetFont();
		pOldFont=memDC.SelectObject(pFont);
	}
	
	//缺省显示如下 
	if(!m_bChecked) 
	//	memDC.FillSolidRect(rect,m_clrBkColor);
		g_d.Draw3DBar3(&memDC,rect,RGB(64,0,0),FALSE);
	else 
		g_d.Draw3DBar3(&memDC,rect,RGB(255,0,0),FALSE);
	GetWindowText(strCaption); 
	memDC.SetBkMode(TRANSPARENT); 
	memDC.DrawText(strCaption,&rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE); 
	pOldPen=memDC.SelectObject(&penBorder);
	memDC.MoveTo(rect.left,rect.bottom);
	memDC.LineTo(rect.TopLeft());
	memDC.LineTo(rect.right-1,rect.top);
	memDC.LineTo(rect.right-1,rect.bottom-1);
	memDC.LineTo(rect.left,rect.bottom-1);
	
	if(lpDrawItemStruct->itemState&ODS_DISABLED)//如果被禁用
	{
		memDC.SetTextColor(RGB(128,128,128));
		memDC.DrawText(strCaption,&rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		pOldPen=memDC.SelectObject(&penBorder);
		memDC.MoveTo(rect.left,rect.bottom);
		memDC.LineTo(rect.TopLeft());
		memDC.LineTo(rect.right-1,rect.top);
		memDC.LineTo(rect.right-1,rect.bottom-1);
		memDC.LineTo(rect.left,rect.bottom-1);
		memDC.SelectObject(pOldPen);
	}
		
	if(lpDrawItemStruct->itemState&ODS_SELECTED)
	{		 
		//按下鼠标 
		CRect rc=rect;
		rc.OffsetRect(1,1);
 		memDC.FillSolidRect(rect,m_clrBkColor);
		memDC.DrawText(strCaption,&rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		pOldPen=memDC.SelectObject(&penBorder);
		memDC.MoveTo(rect.left,rect.bottom);
		memDC.LineTo(rect.TopLeft());
		memDC.LineTo(rect.right-1,rect.top);
		memDC.LineTo(rect.right-1,rect.bottom-1);
		memDC.LineTo(rect.left,rect.bottom-1); 
		memDC.SelectObject(pOldPen);
	 
		 
	}
	else if(m_bHover)
	{   //没按鼠标但是鼠标在按钮上移动
		CRect rc=rect;
 	//	memDC.FillSolidRect(rect,m_clrHoverColor);
	//	g_d.Draw3DBar3(&memDC,rect,VipColor.FLOATCURSORCOLOR,FALSE);
		memDC.SetTextColor(RGB(255,255,0));
		memDC.DrawText(strCaption,&rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		pOldPen=memDC.SelectObject(&penBorder);
		memDC.MoveTo(rect.left,rect.bottom);
		memDC.LineTo(rect.TopLeft());
		memDC.LineTo(rect.right-1,rect.top);
		memDC.LineTo(rect.right-1,rect.bottom-1);
		memDC.LineTo(rect.left,rect.bottom-1); 
		memDC.SelectObject(pOldPen);
	}
	dc.BitBlt(0,0,rect.Width(),rect.Height(),&memDC,0,0,SRCCOPY);
	dc.Detach();
	if(pWnd)
	{
		memDC.SelectObject(pOldFont);
	}
	memDC.SelectObject(pOldBmp);
	memDC.DeleteDC();	
}

void CFlatButton::OnMouseMove(UINT nFlags, CPoint point) 
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

LONG CFlatButton::OnMouseLeave(WPARAM wParam,LPARAM lParam)
{
	m_bHover=false;
	Invalidate();
     return 0;
}

void CFlatButton::SetTextColor(COLORREF clr)
{
   m_clrTextColor=clr;
   if(::IsWindow(m_hWnd))
   {
	   Invalidate();
   }
}

void CFlatButton::SetBkColor(COLORREF clr)
{
	m_clrBkColor=clr;
	if(::IsWindow(m_hWnd))
	{
		Invalidate();
	}

}

void CFlatButton::SetHoverColor(COLORREF clr)
{
	m_clrHoverColor=clr;
	 
	if(::IsWindow(m_hWnd))
	{
		Invalidate();
	}
	 
}

void CFlatButton::SetFont(CFont *pFont)
{
	m_pFont = pFont;
}

void CFlatButton::SetSelColor(COLORREF clr)
{
	m_clrSelColor=clr;
	if(::IsWindow(m_hWnd))
	{
		Invalidate();
	}
}
void CFlatButton::CheckButton(bool bCheck)
{
    m_bChecked=bCheck;
	if(::IsWindow(m_hWnd))
	{
		Invalidate();
	}
}

bool CFlatButton::IsBtnChecked()
{
   return m_bChecked;
}

COLORREF CFlatButton::GetBkColor()
{
   return m_clrBkColor;
}

COLORREF CFlatButton::GetTxtColor()
{
   return m_clrTextColor;
}

COLORREF CFlatButton::GetHoverColor()
{
  return m_clrHoverColor;
}

COLORREF CFlatButton::GetSelColor()
{
  return m_clrSelColor;
}

BOOL CFlatButton::OnEraseBkgnd(CDC* pDC) 
{
    return TRUE;	
}

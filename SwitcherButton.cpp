// SwitcherButton.cpp
//
// Programmed by: JIMMY BRUSH (Kathy007@email.msn.com)
// Download by http://www.codefans.net 
// Legal:
//
// THIS CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
// You may use and distribute this code provided that you do not
// remove this title header and that you do not charge money for
// it. If you want to update the code, feel free to do so, as long
// as you *mark your changes* in code AND in the revision log below
// (and send it to me ;)
//
//
//
// Version: 1.0 revision 1
//
// Revision Log:
//
// SUN MAR 14 1999 - JIMMY BRUSH -  Finished Writing version 1.0
// MON MAR 15 1999 - JIMMY BRUSH -  Fixed RemoveButton to correctly decrement selected button
//									Added CMemDC_Ex by Keith Rule
//									Fixed up Animation a bit
//
//
//
// In the next version \ Wish List:
//
// 1. Tool Tips in CSwitcherButton
// 2. Support for more buttons than can display (with the up/down button at the right)

#include "stdafx.h"
#include "SwitcherWnd.h"
#include "SwitcherButton.h"

#include <winuser.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSwitcherButton

CSwitcherButton::CSwitcherButton()
{
	m_nState = SWITCHBUTTON_UP;
	HasCapture = false;
	m_iIcon = NULL;
	m_iID = -1;

	// Win95/98 balks at creating & deleting the font in OnPaint
	// so put it here
	m_fNormal.CreateFont(10,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_QUALITY|FF_DONTCARE,"MS Sans Serif");
	m_fBold.CreateFont(10,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_QUALITY|FF_DONTCARE,"MS Sans Serif");

	m_ToolTip.InitToolTip(this,TTS_WITHTITLE);
	m_ToolTip.AddToolTip(this,"",NULL,TOOLTIP_TV_MSG);
}

CSwitcherButton::~CSwitcherButton()
{
	m_fNormal.DeleteObject();
	m_fBold.DeleteObject();
}


BEGIN_MESSAGE_MAP(CSwitcherButton, CWnd)
	//{{AFX_MSG_MAP(CSwitcherButton)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSwitcherButton message handlers

void CSwitcherButton::OnPaint() 
{
	CPaintDC paintdc(this); // device context for painting
	CRect rect;
	GetClientRect(&rect);

	CMemDC_Ex dc(&paintdc, &rect);
	
	// center the icon
	int icontop = (rect.Height() / 2) - (16 / 2);

	CString text;
	GetWindowText(text);

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(::GetSysColor(COLOR_BTNTEXT));

	CBrush brush;
	if (m_nState == SWITCHBUTTON_UP)
	{
		brush.CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
		dc.FillRect(&rect, &brush);
		dc.Draw3dRect(&rect, ::GetSysColor(COLOR_3DHIGHLIGHT), ::GetSysColor(COLOR_3DDKSHADOW));
		rect.DeflateRect(1, 1);
		dc.Draw3dRect(&rect, ::GetSysColor(COLOR_3DLIGHT), ::GetSysColor(COLOR_3DSHADOW));
	}
	else if (m_nState == SWITCHBUTTON_DOWN)
	{
		brush.CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
		dc.FillRect(&rect, &brush);
		dc.Draw3dRect(&rect, ::GetSysColor(COLOR_3DDKSHADOW), ::GetSysColor(COLOR_3DHIGHLIGHT));
		rect.DeflateRect(1,1);
		dc.Draw3dRect(&rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DLIGHT));
		// make it look pressed
		rect.top += 1;
		icontop += 1;
	}
	else if (m_nState == SWITCHBUTTON_SELECTED)
	{
		brush.CreateSolidBrush(::GetSysColor(COLOR_3DLIGHT));
		dc.FillRect(&rect, &brush);
		dc.Draw3dRect(&rect, ::GetSysColor(COLOR_3DDKSHADOW), ::GetSysColor(COLOR_3DHIGHLIGHT));
		rect.DeflateRect(1,1);
		dc.Draw3dRect(&rect, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DLIGHT));
		// make it look pressed
		rect.top+= 1;
		icontop += 1;
	}

	// dont even bother if no text
	if (text.IsEmpty() == FALSE)
	{
		// dont want text near the border
		rect.DeflateRect(2,2);

		// MS BUG: DT_VCENTER dont work! (or im doing something wrong)
		// so must Vertical Center it ourself
		CSize size;
		GetTextExtentPoint32(dc.GetSafeHdc(), text, text.GetLength(), &size);

		rect.top += (rect.Height() / 2) - (size.cy / 2) + 1;
		rect.left += 20;

		if (m_nState == SWITCHBUTTON_SELECTED)
			dc.SelectObject(&m_fBold);
		else
			dc.SelectObject(&m_fNormal);

		dc.DrawText(text, &rect, DT_END_ELLIPSIS|DT_VCENTER);
	}

	if (m_iIcon != NULL)
		::DrawIconEx(dc.GetSafeHdc(), 4, icontop, m_iIcon, 16, 16, 0, (HBRUSH)brush, DI_IMAGE);

	brush.DeleteObject();
}

void CSwitcherButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDown(nFlags, point);

	if (m_nState != SWITCHBUTTON_SELECTED)
	{
		SetCapture();
		m_nState = SWITCHBUTTON_DOWN;
		HasCapture = true;
		Invalidate();
	}
}

void CSwitcherButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonUp(nFlags, point);

	bool change = true;

	if (m_nState == SWITCHBUTTON_DOWN)
	{
		m_nState = SWITCHBUTTON_SELECTED;
		::SendMessage(m_wndParent->GetSafeHwnd(), SWM_SELCHANGE, (WPARAM)this, 0);
	}
	else
	{
		if (m_nState == SWITCHBUTTON_UP)
			change = false;

	//	if (m_nState == SWITCHBUTTON_SELECTED)
	//		::SendMessage(m_wndParent->GetSafeHwnd(), SWM_UNSELECT, (WPARAM)this, 0);

	//	m_nState = SWITCHBUTTON_UP;
	}

	ReleaseCapture();
	HasCapture = false;

	if (change)
		Invalidate();
}

void CSwitcherButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd::OnMouseMove(nFlags, point);

	if (HasCapture)
	{
		RECT rect;
		GetClientRect(&rect);

		if (point.x > rect.right || point.x < rect.left || point.y < rect.top || point.y > rect.bottom)
		{
			if (m_nState != SWITCHBUTTON_UP)
			{
				m_nState = SWITCHBUTTON_UP;
				Invalidate();
			}
		}
		else
		{
			if (m_nState != SWITCHBUTTON_DOWN)
			{
				m_nState = SWITCHBUTTON_DOWN;
				Invalidate();
			}
		}
	}
	else
	{
		RECT rect;
		GetClientRect(&rect);
		CString title;
		GetWindowText(title);
		m_ToolTip.AddToolTip(this, title, &rect, TOOLTIP_TV_MSG);
	}
}

BOOL CSwitcherButton::PreTranslateMessage(MSG* pMsg) 
{
	m_ToolTip.SendMessage(TTM_RELAYEVENT, 0, (LPARAM)pMsg);
	
	return CWnd::PreTranslateMessage(pMsg);
}

BOOL CSwitcherButton::DoCreate(CWnd *parent, int x, int y, int cx, int cy, CString text)
{
	m_wndParent = parent;

	WNDCLASS myclass;
	myclass.style = CS_HREDRAW|CS_VREDRAW|CS_PARENTDC;
	myclass.lpfnWndProc = AfxWndProc;
	myclass.cbClsExtra = 0;
	myclass.cbWndExtra = 0;
	myclass.hInstance = AfxGetInstanceHandle();
	myclass.hIcon = NULL;
	myclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	myclass.hbrBackground = NULL;
	myclass.lpszMenuName = NULL;
	myclass.lpszClassName = "SwitcherButtonClass";
	AfxRegisterClass(&myclass);

	return Create("SwitcherButtonClass", text, WS_CHILD|WS_VISIBLE, CRect(x,y,cx+x,cy+y), parent, 0);
}

void CSwitcherButton::SetText(CString text)
{
	SetWindowText(text);
	Invalidate();
	return;
}

void CSwitcherButton::ReplaceIcon(HICON icon)
{
	m_iIcon = icon;
	Invalidate();
}

void CSwitcherButton::Refresh()
{
	Invalidate();
}

void CSwitcherButton::Select()
{
	if (HasCapture)
	{
		ReleaseCapture();
		HasCapture = false;
	}

	m_nState = SWITCHBUTTON_SELECTED;
	Invalidate();
}

void CSwitcherButton::Unselect()
{
	if (HasCapture)
	{
		ReleaseCapture();
		HasCapture = false;
	}

	m_nState = SWITCHBUTTON_UP;
	Invalidate();
}

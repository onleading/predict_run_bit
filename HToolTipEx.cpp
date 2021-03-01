#include "stdafx.h"
#include "HToolTipEx.h"

// HToolTipEx
 
HToolTipEx::HToolTipEx()
{
	m_NeedZoom	= FALSE;
	m_Activate	= FALSE;
}
HToolTipEx::HToolTipEx(CWnd *pParent)
{
	m_NeedZoom	= FALSE;
	m_Activate	= FALSE;
	m_pWnd		= pParent;
}

HToolTipEx::~HToolTipEx()
{
}
 

BEGIN_MESSAGE_MAP(HToolTipEx, CToolTipCtrl)
	//{{AFX_MSG_MAP(HToolTipEx)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// HToolTipEx message handlers

COLORREF HToolTipEx::SetBkColor(COLORREF crColor)
{
	COLORREF oldColor;

	oldColor = SendMessage(TTM_GETTIPBKCOLOR, 0, 0);
	SendMessage(TTM_SETTIPBKCOLOR, (WPARAM)(COLORREF)crColor, 0);
	m_text_bkcor=crColor;
	return oldColor;
}

void HToolTipEx::SetTitleBkColor(COLORREF crColor1,COLORREF crColor2)
{
	m_title_bkcor1=crColor1;
	m_title_bkcor2=crColor2;
}

COLORREF HToolTipEx::SetTextColor(COLORREF crColor)
{
	COLORREF oldColor;

	oldColor = SendMessage(TTM_GETTIPTEXTCOLOR, 0, 0);
	SendMessage(TTM_SETTIPTEXTCOLOR, (WPARAM)(COLORREF)crColor, 0);
	m_text_textcor=crColor;
	return oldColor;
}

void HToolTipEx::SetTitleTextColor(COLORREF crColor)
{
	m_title_textcor=crColor;
}

COLORREF HToolTipEx::GetBkColor()
{
	return SendMessage(TTM_GETTIPBKCOLOR, 0, 0);
}

COLORREF HToolTipEx::GetTextColor()
{
	return SendMessage(TTM_GETTIPTEXTCOLOR, 0, 0);
}

int HToolTipEx::SetDelayTime(DWORD dwType, int nTime)
{
	int nDuration;
	switch(dwType)
	{
	case TTDT_AUTOPOP:
	case TTDT_INITIAL:
	case TTDT_RESHOW:
		nDuration = SendMessage(TTM_GETDELAYTIME, (DWORD)dwType, 0);
		SendMessage(TTM_SETDELAYTIME, (WPARAM)(DWORD)dwType, (LPARAM)(INT)MAKELONG(nTime, 0));
		return nDuration;
	}
	return -1;
}

int HToolTipEx::GetDelayTime(DWORD dwType)
{
	switch(dwType)
	{
	case TTDT_AUTOPOP:
	case TTDT_INITIAL:
	case TTDT_RESHOW:
		return SendMessage(TTM_GETDELAYTIME, (DWORD)dwType, 0);
	}
	return -1;
}

void HToolTipEx::GetMargin(LPRECT lpRect)
{
	SendMessage(TTM_GETMARGIN, 0, (LPARAM)(LPRECT)lpRect);
}

int HToolTipEx::GetMaxTipWidth()
{
	return SendMessage(TTM_GETMAXTIPWIDTH, 0, 0);
}

RECT HToolTipEx::SetMargin(LPRECT lpRect)
{
	RECT TempRect;
	GetMargin(&TempRect);
	SendMessage(TTM_SETMARGIN, 0, (LPARAM)(LPRECT)lpRect);

	return TempRect;
}

int HToolTipEx::SetMaxTipWidth(int nWidth)
{
	return SendMessage(TTM_SETMAXTIPWIDTH, 0, (LPARAM)(INT)nWidth);
}

void HToolTipEx::TrackActivate(BOOL bActivate, LPTOOLINFO lpti)
{
	SendMessage(TTM_TRACKACTIVATE, (WPARAM)(BOOL)bActivate,
		(LPARAM)(LPTOOLINFO)lpti);
}

void HToolTipEx::TrackPosition(LPPOINT lppt)
{
	SendMessage(TTM_TRACKPOSITION, 0, (LPARAM)(DWORD)MAKELONG(lppt->x, lppt->y));
}

void HToolTipEx::TrackPosition(int xPos, int yPos)
{
	SendMessage(TTM_TRACKPOSITION, 0, (LPARAM)(DWORD)MAKELONG(xPos, yPos));
}

void HToolTipEx::Activate( BOOL bActivate )
{
	m_Activate=bActivate;
	CToolTipCtrl::Activate(bActivate );
}

BOOL HToolTipEx::IsActive()
{
	return m_Activate;
}

int HToolTipEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolTipCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_style=TTS_NORMAL;
	m_title_bkcor1=RGB(255,128,128);
	m_title_bkcor2=RGB(128,128,128);
	m_text_bkcor=RGB(225,255,255);
	m_title_textcor=RGB(0,0,0);
	m_text_textcor=RGB(0,0,0);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
void HToolTipEx::DealText(LPCSTR lpszText,UINT style)
{
	CString str=lpszText;
	str.Replace("\r","");
	int i;
	switch(style)  
	{
	case TTS_NORMAL:
		{
			m_text_str=str;
		}
		break;
	case TTS_WITHTITLE:
		{
			i=str.Find("\n");
			if(i==-1)
			{
				m_title_str="";
				m_text_str=str;
			}
			else
			{
				m_title_str=str.Left(i);
				m_text_str=str.Mid(i+1);
			}
		}
		break;
	}
}

void HToolTipEx::UpdateTipText(LPCTSTR lpszText, CWnd* pWnd, UINT nIDTool)
{
	CToolTipCtrl::UpdateTipText(lpszText,pWnd,nIDTool);
	DealText(lpszText,m_style);
}

void HToolTipEx::UpdateTipText(UINT nIDText, CWnd* pWnd, UINT nIDTool)
{
	ASSERT(nIDText != 0);
	
	CString str;
	VERIFY(str.LoadString(nIDText));
	UpdateTipText(str, pWnd, nIDTool);
}

LRESULT HToolTipEx::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(message==WM_PAINT)
	{
		LRESULT r=CToolTipCtrl::WindowProc(message, wParam, lParam);
		DrawIt();
		return r;
	}
	return CToolTipCtrl::WindowProc(message, wParam, lParam);
}

void HToolTipEx::DrawIt()
{
	CRect rc;
	GetWindowRect(rc);
	rc.OffsetRect(-rc.left,-rc.top);
	
	switch(m_style)  
	{
	case TTS_NORMAL:
	    {
			m_text_rect=rc;
	    }
		break;
	case TTS_WITHTITLE:
	    {
			CDC *pWndDC=GetWindowDC();
			CDC memdc;
			CBitmap membm;
			memdc.CreateCompatibleDC(pWndDC);
			membm.CreateCompatibleBitmap(pWndDC,rc.Width(),rc.Height());
			CBitmap *poldbm=memdc.SelectObject(&membm);
			CFont *pWndFont=GetFont();
			CFont font;
			LOGFONT lf;
			pWndFont->GetLogFont(&lf);
			font.CreateFontIndirect(&lf);
			CFont *poldfont=memdc.SelectObject(&font);

			m_title_rect=rc;
			TEXTMETRIC tm;
			memdc.GetTextMetrics(&tm);
			CSize size=memdc.GetTextExtent("сю");
			if(m_title_str=="")
			{
				m_title_rect.SetRect(0,0,0,0);
				m_text_rect=rc;
			}
			else
			{
				m_title_rect.bottom=size.cy+2;
				m_text_rect=rc;
				m_text_rect.top=m_title_rect.bottom;
				m_text_rect.bottom-=1;
			}
			g_GUI.DrawGradient(&memdc,m_title_rect,m_title_bkcor1,m_title_bkcor2,FALSE);
			memdc.FillSolidRect(m_text_rect,m_text_bkcor);
			memdc.SetBkMode(TRANSPARENT);
			m_title_rect.left+=1;
			m_text_rect.left+=1;
			m_text_rect.top+=1;

			memdc.SetTextColor(m_title_textcor);
			memdc.DrawText(m_title_str,m_title_rect,DT_VCENTER|DT_LEFT|DT_SINGLELINE);
			
			memdc.SetTextColor(m_text_textcor);
			memdc.DrawText(m_text_str,m_text_rect,DT_LEFT|DT_WORDBREAK);
			
			memdc.SetBkMode(OPAQUE);
			memdc.SelectObject(poldfont);
			pWndDC->BitBlt(0,0,rc.Width(),rc.Height(),&memdc,0,0,SRCCOPY);
			memdc.SelectObject(poldbm);
			memdc.DeleteDC();
			membm.DeleteObject();
			ReleaseDC(pWndDC);
	    }
	    break;
	}
}

void HToolTipEx::SetStyle(UINT nStyle)
{
	switch(nStyle)  
	{
	case TTS_NORMAL:
	case TTS_WITHTITLE:
		m_style=nStyle;
	    break;
	default:
		m_style=TTS_NORMAL;
		break;
	}
}

BOOL HToolTipEx::AddTool(CWnd* pWnd, LPCTSTR lpszText, LPCRECT lpRectTool,
						   UINT nIDTool)
{
	DealText(lpszText,m_style);
	return CToolTipCtrl::AddTool(pWnd,lpszText,lpRectTool,nIDTool);
}

BOOL HToolTipEx::AddTool(CWnd* pWnd, UINT nIDText, LPCRECT lpRectTool,
						   UINT nIDTool)
{
	DealText((LPTSTR)MAKEINTRESOURCE(nIDText),m_style);
	return CToolTipCtrl::AddTool(pWnd,nIDText,lpRectTool,nIDTool);
}

void HToolTipEx::InitToolTip(CWnd *pWnd,UINT nStyle)
{
	Create(pWnd);
	AddTool(pWnd,"");
	SetMaxTipWidth(500);
	SetStyle(nStyle);
	SetDelayTime(TTDT_INITIAL,150);
	Activate(FALSE);
}

void HToolTipEx::UpdateToolTip(CWnd *pWnd,CString strTipInfo)
{
	if(!pWnd || !::IsWindow(pWnd->m_hWnd))
		return;
	UpdateTipText(strTipInfo,pWnd);	
	if(strTipInfo.GetLength()==0)
		Activate(FALSE);
	else
		Activate(TRUE);
}

void HToolTipEx::PopToolTip()
{
	Pop();
}

void HToolTipEx::AddToolTip(CWnd *pWnd,CString strTipInfo,LPCRECT pRect,UINT nID)
{
	if(!pWnd || !::IsWindow(pWnd->m_hWnd))
		return;
	AddTool(pWnd,strTipInfo,pRect,nID);
	Activate(TRUE);
}

void HToolTipEx::DelToolTip(CWnd *pWnd)
{
	if(!pWnd || !::IsWindow(pWnd->m_hWnd))
		return;
	DelTool(pWnd);
	Activate(FALSE);
}


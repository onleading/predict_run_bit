// MyBitmapBt.cpp : implementation file
//

#include "stdafx.h"
#include "MyBitmapBt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
const UINT WM_MOUSEENTER=WM_USER+1000;			//类内部使用,当鼠标进入按钮区域时触发

/////////////////////////////////////////////////////////////////////////////
// MyBitmapBt
MyBitmapBt::MyBitmapBt()
{
	memset(m_TitleBtBK,0,sizeof(m_TitleBtBK));
	m_pBMCenter=NULL;
}
MyBitmapBt::~MyBitmapBt()
{
	for(int i=0;i<9;i++)
	{
		if(m_TitleBtBK[i])
		{
			if((HBITMAP)*m_TitleBtBK[i]) m_TitleBtBK[i]->DeleteObject();
			delete m_TitleBtBK[i];
			m_TitleBtBK[i]=NULL;
		}
	}
	if(m_pBMCenter)
	{
		if((HBITMAP)*m_pBMCenter) m_pBMCenter->DeleteObject();
		delete m_pBMCenter;
		m_pBMCenter=NULL;
	}
	if((HBITMAP)m_bmp) m_bmp.DeleteObject();
}


BEGIN_MESSAGE_MAP(MyBitmapBt, CWnd)
	//{{AFX_MSG_MAP(MyBitmapBt)
	ON_WM_PAINT()
	ON_WM_MOVE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_MESSAGE(WM_MOUSEENTER,OnMouseEnter)
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// MyBitmapBt message handlers
//参数为:按钮ID,文字,类型(类型为TitleBt时调用SetCenterImage设置中心位图ID),文字显示格式,是否采用不规则形状窗口,透明色
MyBitmapBt::MyBitmapBt(UINT id,LPCSTR text,ButtonType bt,UINT nFormat/* =DT_BOTTOM|DT_CENTER */,BOOL useWndRgn/* =FALSE */,COLORREF corMask/* =RGB(0,255,0) */)
{
	m_pBMCenter=NULL;
	memset(m_TitleBtBK,0,sizeof(m_TitleBtBK));
	Init(id,text,bt,nFormat,useWndRgn,corMask);
}

void MyBitmapBt::Init(UINT id,LPCSTR text,ButtonType bt,UINT nFormat/* =DT_BOTTOM|DT_CENTER */,BOOL useWndRgn/* =FALSE */,COLORREF corMask/* =RGB(0,255,0) */)
{
	m_cor_text_n=RGB(0,0,0);
	m_cor_text_d=RGB(0,0,0);
	m_cor_text_o=RGB(0,0,0);
	m_text_style_n=TS_Normal;
	m_text_style_d=TS_Normal;
	m_text_style_o=TS_Normal;
	m_text_height_n=12;
	m_text_height_d=12;
	m_text_height_o=12;
	m_id=id;
	m_wndText=text;
	m_bt=bt;
	m_BtState=Normal;
	m_drawflag=TRUE;
	m_LBtDown=FALSE;
	bFirst=TRUE;
	m_bSel=FALSE;
	m_nFormat=nFormat;
	m_UseWndRgn=useWndRgn;
	m_corMask=corMask;
	m_pfOwnerDraw=NULL;
	m_bFocus=FALSE;
	m_offset[0]=m_offset[1]=m_offset[2]=CPoint(0,0);
	m_cor1_menu=RGB(128,128,128);
	m_cor2_menu=RGB(255,255,255);
}
void MyBitmapBt::SetCenterImage(long ID_CENTER)
{
	if(ID_CENTER)
	{
		if(m_pBMCenter)
		{
			m_pBMCenter->DeleteObject();
			TDEL(m_pBMCenter);
		}	
		if(!m_pBMCenter->LoadBitmap(ID_CENTER))
			TDEL(m_pBMCenter);
	}
}
void MyBitmapBt::SetCenterImage(LPCTSTR resName)
{
	if(resName)
	{
		if(m_pBMCenter)
		{
			m_pBMCenter->DeleteObject();
			TDEL(m_pBMCenter);
		}	
		if(!m_pBMCenter->LoadBitmap(resName))
			TDEL(m_pBMCenter);
	}
}
void MyBitmapBt::SetCenterImage(CBitmap *pbm)
{
	if(pbm)
	{
		if(m_pBMCenter)
		{
			m_pBMCenter->DeleteObject();
			TDEL(m_pBMCenter);
		}		
		m_pBMCenter=new CBitmap;
		BITMAP tempbitmap;
		pbm->GetBitmap(&tempbitmap);
		long tbuffsize=tempbitmap.bmWidthBytes*tempbitmap.bmHeight;
		tempbitmap.bmBits=new BYTE[tbuffsize];
		pbm->GetBitmapBits(tbuffsize,tempbitmap.bmBits);
		m_pBMCenter->CreateBitmapIndirect(&tempbitmap);
		TDEL(tempbitmap.bmBits);
	}
}

void MyBitmapBt::SetCustomFunc(MBB_OwnerDraw pFunc)
{
	m_pfOwnerDraw=pFunc;
}

void MyBitmapBt::_ModifyState(ButtonState &state)
{
	state=m_BtState;
	if(m_bt==SwitchLabelBt /*&& state==Normal*/ && m_bSel)
		state=LMouseDown;
}
void MyBitmapBt::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CDC *pDC=&dc;
	CDC memdc;
	CBitmap *old;
	
	GetWindowRect(m_wndRect);
	ScreenToClient(m_wndRect);
	
	memdc.CreateCompatibleDC(pDC);
	ButtonState tmpState=m_BtState;
	_ModifyState(tmpState);
	if(!m_drawflag)
	{
		memdc.SelectObject(&m_bmp);
		if(m_pfOwnerDraw) m_pfOwnerDraw(this,&memdc,m_wndRect,tmpState,m_bFocus);
		pDC->BitBlt(0,0,m_wndRect.Width(),m_wndRect.Height(),&memdc, 0, 0, SRCCOPY);
	}
	else
	{
		if((HBITMAP)m_bmp) m_bmp.DeleteObject();
		m_bmp.CreateCompatibleBitmap(pDC,m_wndRect.Width(),m_wndRect.Height());
		old=memdc.SelectObject(&m_bmp);
		CBitmap *pleft=NULL,*pright=NULL,*pbackground=NULL;
		CPoint text_offset;
		switch (tmpState)
		{
		case Normal:
			pleft=m_TitleBtBK[0];
			pright=m_TitleBtBK[1];
			pbackground=m_TitleBtBK[2];
			text_offset=m_offset[0];
			break;
		case LMouseDown:
			pleft=m_TitleBtBK[3];
			pright=m_TitleBtBK[4];
			pbackground=m_TitleBtBK[5];
			text_offset=m_offset[1];
			break;
		case MouseOver:
			pleft=m_TitleBtBK[6];
			pright=m_TitleBtBK[7];
			pbackground=m_TitleBtBK[8];
			text_offset=m_offset[2];
			break;
		}
		
		CSize left=g_GUI.GetBitmapSize(pleft);
		CSize right=g_GUI.GetBitmapSize(pright);
		if(left.cx!=0 && left.cy!=0)
			g_GUI.DrawXPBmp(&memdc,pleft,CPoint(0,0),CPoint(left.cx,m_wndRect.Height()));
		if(right.cx!=0 && right.cy!=0)
		{
			CPoint pt1(m_wndRect.Width()-right.cx,0);
			CPoint pt2(right.cx,m_wndRect.Height());
			g_GUI.DrawXPBmp(&memdc,pright,pt1,pt2);
		}
		g_GUI.DrawXPBmp(&memdc,pbackground,CPoint(left.cx,0),CPoint(m_wndRect.Width()-left.cx-right.cx,m_wndRect.Height()));
		memdc.SetBkMode(TRANSPARENT);
		if(m_bt==NormalBt)
		{
			if(m_pfOwnerDraw) m_pfOwnerDraw(this,&memdc,m_wndRect,tmpState,m_bFocus);
			CFont btFont,*poldfont;
			GetTextFont(btFont);
			poldfont=memdc.SelectObject(&btFont);
			CRect tmprc=m_wndRect;
			tmprc.DeflateRect(1,2,0,0);
			_SetTextCor(&memdc);

			tmprc.OffsetRect(text_offset);
			memdc.DrawText(m_wndText,m_wndText.GetLength(),tmprc,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			memdc.SelectObject(poldfont);
			btFont.DeleteObject();
		}else
		if(m_bt==SwitchLabelBt)
		{
			if(m_pfOwnerDraw) m_pfOwnerDraw(this,&memdc,m_wndRect,tmpState,m_bFocus);
			CFont btFont,*poldfont;
			GetTextFont(btFont);
			poldfont=memdc.SelectObject(&btFont);
			CRect tmprc(left.cx+2,2,m_wndRect.Width()-right.cx,m_wndRect.Height()-2);
			_SetTextCor(&memdc);
			
			tmprc.OffsetRect(text_offset);
			CSize sz=memdc.GetTextExtent(m_wndText);
			memdc.DrawText(m_wndText,m_wndText.GetLength(),tmprc,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
			memdc.SelectObject(poldfont);
			btFont.DeleteObject();
		}else
		if(m_bt==MenuBt)
		{
			if(m_BtState==LMouseDown)
				memdc.Draw3dRect(m_wndRect,m_cor1_menu,m_cor2_menu);
			if(m_pfOwnerDraw) m_pfOwnerDraw(this,&memdc,m_wndRect,tmpState,m_bFocus);
			CFont btFont,*poldfont;
			GetTextFont(btFont);
			poldfont=memdc.SelectObject(&btFont);
			_SetTextCor(&memdc);
			CRect tmprc=m_wndRect;
			tmprc.OffsetRect(text_offset);
			memdc.DrawText(m_wndText,m_wndText.GetLength(),tmprc,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			memdc.SelectObject(poldfont);
			btFont.DeleteObject();
		}else
		if(m_bt==TitleBt)
		{
			CSize size=g_GUI.GetBitmapSize(m_pBMCenter);
			//double s=m_wndRect.Width()*6/10.0/size.cx;
			double s=1.0;
			CPoint temp;
			if((m_nFormat&0x03) == DT_RIGHT)
				temp.x=5;
			if((m_nFormat&0x03) == DT_LEFT)
				temp.x=m_wndRect.Width()-5-(long)(size.cx*s+0.5);
			if((m_nFormat&0x03) == DT_CENTER)
				temp.x=(long)((m_wndRect.Width()-size.cx*s)/2.0);
			temp.y=(long)((m_wndRect.Height()-size.cy*s)/2.0+0.5);
			
			if(tmpState==MouseOver) temp.Offset(-1,-1);
			g_GUI.DrawTransparencyBitmap(&memdc,temp,CSize((int)(size.cx*s),(int)(size.cy*s)),
					CSize(m_wndRect.Width(),m_wndRect.Height()),RGB(0,255,0),m_pBMCenter);
			
			CRect temp2=m_wndRect;
			temp2.bottom-=2;
			CFont btFont,*poldfont;
			GetTextFont(btFont);
			poldfont=memdc.SelectObject(&btFont);
			_SetTextCor(&memdc);
			temp2.OffsetRect(text_offset);
			memdc.DrawText(m_wndText,m_wndText.GetLength(),temp2,m_nFormat|DT_SINGLELINE);
			memdc.SelectObject(poldfont);
			btFont.DeleteObject();
		}
		pDC->BitBlt(0,0,m_wndRect.Width(),m_wndRect.Height(),&memdc, 0, 0, SRCCOPY);
		memdc.SelectObject(old);
	}
	memdc.DeleteDC();
}

COLORREF MyBitmapBt::_SetTextCor(CDC *pDC)
{
	COLORREF old;
	ButtonState tmpState;
	_ModifyState(tmpState);
	switch(tmpState)  
	{
	case LMouseDown:
	    {
			old=pDC->SetTextColor(m_cor_text_d);
	    }
		break;
	case MouseOver:
	    {
			old=pDC->SetTextColor(m_cor_text_o);
	    }
	    break;
	case Normal:
	default:
	    {
			old=pDC->SetTextColor(m_cor_text_n);
	    }
	    break;
	}
	return old;
}

void MyBitmapBt::GetTextFont(CFont &font)
{
	TextStyle temp;
	int height;
	ButtonState tmpState;
	_ModifyState(tmpState);
	switch(tmpState)  
	{
	case LMouseDown:
	    {
			temp=m_text_style_d;
			height=m_text_height_d;
	    }
		break;
	case MouseOver:
	    {
			temp=m_text_style_o;
			height=m_text_height_o;
	    }
	    break;
	case Normal:
	default:
	    {
			temp=m_text_style_n;
			height=m_text_height_n;
	    }
	    break;
	}
	int weight;
	BOOL bItalic,bUnderline;
	GetTextStyle(temp,weight,bItalic,bUnderline);
	font.CreateFont(height,0,0,0,weight,bItalic,bUnderline,0,DEFAULT_CHARSET,OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE, SONG_FONT );
}

BOOL MyBitmapBt::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void MyBitmapBt::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (bFirst)
	{
		PostMessage(WM_MOUSEENTER);
		bFirst = FALSE;
		SetTimer(1,70,NULL);
	}
	CWnd::OnMouseMove(nFlags, point);
}

void MyBitmapBt::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_BtState!=LMouseDown && m_bt!=SwitchLabelBt)
	{
		m_BtState=LMouseDown;
		_SetWindowRgn();
	}
	SetCapture();
	m_LBtDown=TRUE;
	CWnd::OnLButtonDown(nFlags, point);
}

void MyBitmapBt::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_LBtDown)
	{
		BOOL bIn=FALSE;
		if(m_UseWndRgn)
		{
			CRgn wndRgn;
			wndRgn.CreateRectRgn(0,0,0,0);
			GetWindowRgn((HRGN)wndRgn);
			bIn=wndRgn.PtInRegion(point);
			wndRgn.DeleteObject();
		}
		else
		{
			CRect wndrc;
			GetClientRect(&wndrc);
			bIn=wndrc.PtInRect(point);
		}

		if(bIn)
		{
			switch(m_bt)  
			{
			case SwitchLabelBt:
			    {
					m_bSel=TRUE;
			    }
				break;
			case MenuBt:
			    {
					m_bSel=TRUE;
					if(m_BtState!=LMouseDown)
					{
						m_BtState=LMouseDown;
						_SetWindowRgn();
					}
			    }
			    break;
			case NormalBt:
			default:
			    {
					if(m_bSel) m_BtState=LMouseDown;
					else m_BtState=MouseOver;
					_SetWindowRgn();
			    }
			    break;
			}
			::PostMessage(GetParent()->m_hWnd,UM_MYBITMAPBUTTON,0,m_id);
		}
		else
		{
			if(m_bSel) m_BtState=LMouseDown;
			else m_BtState=Normal;
			if(m_bt==SwitchLabelBt)
				Invalidate();
			else
			_SetWindowRgn();
		}
		ReleaseCapture();
	}
	m_LBtDown=FALSE;
	CWnd::OnLButtonUp(nFlags, point);
}

void MyBitmapBt::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==1)
	{
		CPoint pt;
		CRect rect;
		GetCursorPos(&pt);
		GetWindowRect(&rect);
		BOOL bIn=TRUE;
		if(m_UseWndRgn)
		{
			CRgn wndRgn;
			wndRgn.CreateRectRgn(0,0,0,0);
			GetWindowRgn((HRGN)wndRgn);
			pt.Offset(-rect.left,-rect.top);
			if (!wndRgn.PtInRegion(pt) && !m_LBtDown)
				bIn=FALSE;
			wndRgn.DeleteObject();
		}
		else
		{
			if(!rect.PtInRect(pt)&& !m_LBtDown) bIn=FALSE;
		}
		
		if (bIn==FALSE)
		{
			KillTimer(1); // kill self.
			PostMessage(WM_MOUSELEAVE);
		}
	}
	if(nIDEvent==2 && m_bSel)
	{
		HWND tmpHwnd=GetParent()->m_hWnd;
		::PostMessage(tmpHwnd,UM_MYBITMAPBUTTON_ONMOUSEMOVE,0,m_id);
	}
	CWnd::OnTimer(nIDEvent);
}

void MyBitmapBt::OnMouseEnter(WPARAM wParam,LPARAM lParam)
{
	m_ToolTip.UpdateTipText(m_strTooltip,this);
	if(m_bt==SwitchLabelBt)
	{
		m_BtState=MouseOver;
		Invalidate();
		return;
	}
	if(m_bSel==TRUE)
		return;
	if(m_BtState!=MouseOver)
	{
		m_BtState=MouseOver;
		_SetWindowRgn();
	}
}

void MyBitmapBt::OnMouseLeave(WPARAM wParam,LPARAM lParam)
{
	BOOL bChanged=FALSE;
	if(m_bSel && m_bt!=SwitchLabelBt)
		{
			if(m_BtState!=LMouseDown)
			{
				m_BtState=LMouseDown;
				bChanged=TRUE;
			}
		}
	else
	{
			if(m_BtState!=Normal)
			{
				m_BtState=Normal;
				bChanged=TRUE;
			}
	}
	if(bChanged)
	{
		if(m_bt==SwitchLabelBt)
			Invalidate();
		else
			_SetWindowRgn();
	}
	bFirst=TRUE;
}

void MyBitmapBt::SetSel(BOOL sel)
{
	m_bSel=sel;
	if(m_bt==SwitchLabelBt)
	{
		Invalidate();
		return;
	}
	
	if(sel) m_BtState=LMouseDown;
	else m_BtState=Normal;
	_SetWindowRgn();
}

BOOL MyBitmapBt::GetSel()
{
	return m_bSel;
}

void MyBitmapBt::SetBK(long bk[9])
{
	for(int i=0;i<9;i++)
	{
		if(m_TitleBtBK[i])
		{
			m_TitleBtBK[i]->DeleteObject();
			TDEL(m_TitleBtBK[i]);
		}
		if(bk[i])
		{
			m_TitleBtBK[i]=new CBitmap;
			if(!m_TitleBtBK[i]->LoadBitmap(bk[i]))
				TDEL(m_TitleBtBK[i]);
		}
	}
}
void MyBitmapBt::SetBK(LPCTSTR bk[9])
{
	for(int i=0;i<9;i++)
	{
		if(m_TitleBtBK[i])
		{
			m_TitleBtBK[i]->DeleteObject();
			TDEL(m_TitleBtBK[i]);
		}
		if(bk[i])
		{
			m_TitleBtBK[i]=new CBitmap;
			if(!m_TitleBtBK[i]->LoadBitmap(bk[i]))
				TDEL(m_TitleBtBK[i]);
		}
	}
}
void MyBitmapBt::SetBK(CBitmap* bk[9])
{
	for(int i=0;i<9;i++)
	{
		if(m_TitleBtBK[i])
		{
			m_TitleBtBK[i]->DeleteObject();
			TDEL(m_TitleBtBK[i]);
		}
		if(!bk[i]) continue;
		m_TitleBtBK[i]=new CBitmap;
		BITMAP tempbitmap;
		bk[i]->GetBitmap(&tempbitmap);
		long tbuffsize=tempbitmap.bmWidthBytes*tempbitmap.bmHeight;
		tempbitmap.bmBits=new BYTE[tbuffsize];
		bk[i]->GetBitmapBits(tbuffsize,tempbitmap.bmBits);
		m_TitleBtBK[i]->CreateBitmapIndirect(&tempbitmap);
		TDEL(tempbitmap.bmBits);
	}
}

void MyBitmapBt::OnMove(int x, int y)
{
	CWnd::OnMove(x,y);
	Invalidate();
}

void MyBitmapBt::SetTextCor(COLORREF cor_text_n,COLORREF cor_text_d,COLORREF cor_text_o)
{
	m_cor_text_n=cor_text_n;
	m_cor_text_d=cor_text_d;
	m_cor_text_o=cor_text_o;
}

void MyBitmapBt::SetTextStyle(TextStyle style_n,TextStyle style_d,TextStyle style_o)
{
	m_text_style_n=style_n;
	m_text_style_d=style_d;
	m_text_style_o=style_o;
}

void MyBitmapBt::GetTextStyle(TextStyle style,int &weight,BOOL &bItalic,BOOL &bUnderline)
{
	weight=(style&TS_Bold)==TS_Bold?600:FW_NORMAL;
	bItalic=(style&TS_Italic)==TS_Italic?TRUE:FALSE;
	bUnderline=(style&TS_Underline)==TS_Underline?TRUE:FALSE;
}

void MyBitmapBt::SetTextHeight(int height_n,int height_d,int height_o)
{
	m_text_height_n=height_n;
	m_text_height_d=(height_d==-1)?height_n:height_d;
	m_text_height_o=(height_o==-1)?height_n:height_o;
}

void MyBitmapBt::_SetWindowRgn()
{
	if(m_bt!=SwitchLabelBt && m_UseWndRgn)
	{
		CRgn wndRgn;
		GetOriginalWndRgn(wndRgn);
		SetWindowRgn((HRGN)wndRgn,FALSE);
	}
	Invalidate();
}

void MyBitmapBt::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	if(cx!=0)
	_SetWindowRgn();
}

int MyBitmapBt::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetTimer(2,50,NULL);

	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);
	m_ToolTip.AddTool(this,"");
	m_ToolTip.SetDelayTime(200);
	
	return 0;
}

void MyBitmapBt::SetTooltip(LPCSTR str)
{
	m_strTooltip=str;
}

BOOL MyBitmapBt::PreTranslateMessage(MSG* pMsg) 
{
	if(m_ToolTip.m_hWnd!=NULL)  
		m_ToolTip.RelayEvent(pMsg);
	return CWnd::PreTranslateMessage(pMsg);
}

void MyBitmapBt::SetTextOffset(int x_n,int y_n,int x_d,int y_d,int x_o,int y_o)
{
	m_offset[0].x=x_n;
	m_offset[0].y=y_n;
	m_offset[1].x=x_d;
	m_offset[1].y=y_d;
	m_offset[2].x=x_o;
	m_offset[2].y=y_o;
}

void MyBitmapBt::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	m_bFocus=TRUE;
	Invalidate();	
}

void MyBitmapBt::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
	m_bFocus=FALSE;
	Invalidate();
}

void MyBitmapBt::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar==' ' && m_bt!=SwitchLabelBt)
	{
		if(m_BtState!=LMouseDown)
		{
			m_BtState=LMouseDown;
			_SetWindowRgn();
		}
	}
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void MyBitmapBt::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar==' ')
	{
		if(GetKeyState(VK_LCONTROL)>=0 && GetKeyState(VK_RCONTROL)>=0 &&
			GetKeyState(VK_LMENU)>=0 && GetKeyState(VK_RMENU)>=0)
		{
			switch(m_bt)  
			{
			case SwitchLabelBt:
				break;
			case MenuBt:
				{
					if(m_BtState!=LMouseDown)
					{
						m_BtState=LMouseDown;
						m_bSel=TRUE;
						_SetWindowRgn();
					}
				}
				break;
			case NormalBt:
			default:
				{
					if(m_bSel) m_BtState=LMouseDown;
					else m_BtState=Normal;
					_SetWindowRgn();
				}
				break;
			}
			::PostMessage(GetParent()->m_hWnd,UM_MYBITMAPBUTTON,0,m_id);
		}
	}
	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void MyBitmapBt::SetFrameCor_MenuStyle(COLORREF cor1,COLORREF cor2)
{
	m_cor1_menu=cor1;
	m_cor2_menu=cor2;
}

void MyBitmapBt::GetOriginalWndRgn(CRgn &rgn)
{
	HRGN tmphrgn=(HRGN)rgn;
	if(tmphrgn)
		rgn.DeleteObject();
	else
		rgn.CreateRectRgn(0,0,0,0);
	CRect rcWnd;
	GetWindowRect(rcWnd);
	int cx=rcWnd.Width();
	int cy=rcWnd.Height();
	if(m_UseWndRgn && cx!=0 && cy!=0)
	{
		CRgn left,center,right;
		CSize s_left,s_center,s_right;
		CBitmap *pleft=NULL,*pright=NULL,*pbackground=NULL;
		ButtonState tmpState;
		_ModifyState(tmpState);
		switch (tmpState)
		{
		case Normal:
			pleft=m_TitleBtBK[0];
			pright=m_TitleBtBK[1];
			pbackground=m_TitleBtBK[2];
			break;
		case LMouseDown:
			pleft=m_TitleBtBK[3];
			pright=m_TitleBtBK[4];
			pbackground=m_TitleBtBK[5];
			break;
		case MouseOver:
			pleft=m_TitleBtBK[6];
			pright=m_TitleBtBK[7];
			pbackground=m_TitleBtBK[8];
			break;
		}
		s_left=g_GUI.GetBitmapRgn(left,pleft,m_corMask);
		s_right=g_GUI.GetBitmapRgn(right,pright,m_corMask);
		right.OffsetRgn(cx-s_right.cx,0);
		s_center.cx=cx-s_left.cx-s_right.cx;
		s_center.cy=cy;
		CSize s_tmp=g_GUI.GetBitmapSize(pbackground);
		if(s_center.cx==s_tmp.cx)
			s_center=g_GUI.GetBitmapRgn(center,pbackground,m_corMask);
		else
			center.CreateRectRgn(0,0,s_center.cx,s_center.cy);
		center.OffsetRgn(s_left.cx,0);
		rgn.CombineRgn(&rgn,&left,RGN_OR);
		rgn.CombineRgn(&rgn,&center,RGN_OR);
		rgn.CombineRgn(&rgn,&right,RGN_OR);
		left.DeleteObject();
		center.DeleteObject();
		right.DeleteObject();
	}
}

void MyBitmapBt::SetText(LPCSTR txt)
{
	m_wndText=txt;
	Invalidate();
}

UINT MyBitmapBt::GetID()
{
	return m_id;
}

//////////////////////////////////////////////////////////////////////////
//class CMyStatic
CMyStatic::CMyStatic()
{
	m_rc.SetRect(0,0,0,0);
	m_str="";
	m_cor_text=0;
	m_pParentWnd=NULL;
}

void CMyStatic::SetParent(CWnd * pParentWnd)
{
	m_pParentWnd=pParentWnd;
}

void CMyStatic::SetWindowText(LPCSTR str)
{
	m_str=str;
	if(m_pParentWnd && ::IsWindow(m_pParentWnd->m_hWnd))
		m_pParentWnd->InvalidateRect(m_rc);
}

void CMyStatic::GetWindowText(CString &str)
{
	str=m_str;
}

void CMyStatic::SetRect(CRect rc)
{
	m_rc=rc;
}

void CMyStatic::GetRect(CRect &rc)
{
	rc=m_rc;
}

void CMyStatic::SetTextColor(COLORREF cor_text)
{
	m_cor_text=cor_text;
}

COLORREF CMyStatic::GetTextColor()
{
	return m_cor_text;
}

void CMyStatic::DrawText(CDC *pDC,UINT nFormat)
{
	COLORREF oldcor=pDC->SetTextColor(m_cor_text);
	pDC->DrawText(m_str,m_rc,nFormat);
	pDC->SetTextColor(oldcor);
}

void CMyStatic::DrawText(CDC *pDC,UINT nFormat,COLORREF cr,UINT size,LPCSTR type)
{
	CFont btFont;
	btFont.CreateFont(size ,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,type);

	CFont *poldfont=pDC->SelectObject(&btFont);
	COLORREF oldcor=pDC->SetTextColor(cr);
	int oldbkmode=pDC->GetBkMode();
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(m_str,m_rc,nFormat);
	pDC->SetBkMode(oldbkmode);
	pDC->SetTextColor(oldcor);
	pDC->SelectObject(poldfont);
}

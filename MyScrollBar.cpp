//MyScrollBar.cpp : implementation file

#include "stdafx.h"
#include "resource.h"
#include "MyScrollBar.h"
#include "MyInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyScrollBar
#define   SJX_LEFT		0
#define   SJX_RIGHT		1
#define   SJX_UP		2
#define   SJX_DOWN		3

CMyScrollBar::CMyScrollBar()
{
	// 注册当前类
	RegisterWindowClass(NULL,_T("MyScrollBar"));

	m_nScrollBarPos	= 0;
	m_nMinPos		= 0;
	m_nMaxPos		= 1;
	m_nScrollBarPos = 0;
	m_nMouseStat	= 1;

	m_nDrawStyle	= 0;	
	m_style=MSBS_DEFAULT;
	
	memset(m_cor,0,10*sizeof(COLORREF));
}

BOOL CMyScrollBar::RegisterWindowClass(HINSTANCE hInstance,LPCTSTR lpszClassName)
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

CMyScrollBar::~CMyScrollBar()
{
}


BEGIN_MESSAGE_MAP(CMyScrollBar, CWnd)
	//{{AFX_MSG_MAP(CMyScrollBar)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyScrollBar message handlers
void CMyScrollBar::Init(int placestyle,HWND hwnd,UINT msg,int whichunit)
{
	m_nDrawStyle	= placestyle;	
	m_MsghWnd		= hwnd; 
	m_Msg			= msg;
	m_WhichUnit		= whichunit;
}

void CMyScrollBar::SetScrollColor(COLORREF cor[10])
{
	memcpy(m_cor,cor,10*sizeof(COLORREF));
}

void CMyScrollBar::SetScrollColor(COLORREF bordercolor,int borderwidth,COLORREF bc, COLORREF fc)
{
	if(g_d.IsBlackColorScheme(bc))
	{
		//暗色调
		if(m_style==MSBS_DEFAULT)
		{
			m_cor[0] = g_GUI.LightenScaleColor(70,bc);
			m_cor[1] = bc;
			m_cor[2] = g_GUI.LightenScaleColor(99,bc);
			m_cor[3] = g_GUI.LightenScaleColor(93,bc);
			m_cor[4] = bc;
			m_cor[5] = g_GUI.LightenScaleColor(76,bc);
			m_cor[6] = g_GUI.LightenScaleColor(41,bc);
			m_cor[7] = g_GUI.LightenScaleColor(140,bc);
			m_cor[8] = RGB(0x8E,0x95,0xA2);
			m_cor[9] = RGB(159,166,176);
		}
	}
	else
	{
		//亮色调
		if(m_style==MSBS_DEFAULT)
		{
			m_cor[0] = RGB(220,220,220);
			m_cor[1] = RGB(236,238,243);
			m_cor[2] = RGB(251,251,254);
			m_cor[3] = RGB(237,237,239);
			m_cor[4] = RGB(190,191,205);
			m_cor[5] = RGB(180,180,189);
			m_cor[6] = RGB(148,149,162);
			m_cor[7] = RGB(240,240,240);
			m_cor[8] = RGB(0x8E,0x95,0xA2);
			m_cor[9] = RGB(255,255,255);
		}
	}
}

BOOL CMyScrollBar::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->hwnd == this->m_hWnd)
	{
		switch( pMsg->message )
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			LButtonDown(pMsg);
			return TRUE;
		case WM_LBUTTONUP:
			LButtonUp(pMsg);
			break;
		case WM_MOUSEMOVE:
			MouseMove(pMsg,0);
			return TRUE;
		}
	}
	return CWnd::PreTranslateMessage(pMsg);
}


//ScrollBar
//***************************************************************************************

void CMyScrollBar::SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw)
{
	m_nMinPos		= nMinPos;
	m_nMaxPos		= nMaxPos+1;
	if ( m_nMaxPos == m_nMinPos )	m_nMaxPos =m_nMinPos+1;
	if(bRedraw)
		DrawScrollBar();
//	TRACE("--1 m_nMaxPos==%d\n",m_nMaxPos);
}

int CMyScrollBar::SetScrollPos(int nPos, BOOL bRedraw)
{	
	m_nScrollBarPos = nPos;
	if (m_nScrollBarPos >= m_nMaxPos   )
		 m_nScrollBarPos = m_nMaxPos-1;
	if(m_nScrollBarPos<m_nMinPos)
		m_nScrollBarPos=m_nMinPos;
	if(bRedraw)
		DrawScrollBar();
	return 1;
}

int	 CMyScrollBar::GetScrollPos()
{
	return m_nScrollBarPos;
}

void CMyScrollBar::DrawButtonBG(CDC *pDC,CRect rc,BOOL bHorizontal)
{
	if(m_style==MSBS_DEFAULT)
	{
	//	pDC->FillSolidRect(rc.left+1,rc.top+1,rc.Width()-2,rc.Height()-2,m_cor[7]);
		g_d.Draw3DBar2(pDC, CRect(rc.left+1,rc.top+1,rc.Width()-2,rc.Height()-2), m_cor[7], FALSE);
		
		pDC->SetPixel(rc.left,rc.top,g_GUI.LightenScaleColor(50,pDC->GetPixel(rc.left,rc.top)));
		pDC->SetPixel(rc.left,rc.bottom-1,g_GUI.LightenScaleColor(50,pDC->GetPixel(rc.left,rc.bottom-1)));
		pDC->SetPixel(rc.right-1,rc.top,g_GUI.LightenScaleColor(50,pDC->GetPixel(rc.right-1,rc.top)));
		pDC->SetPixel(rc.right-1,rc.bottom-1,g_GUI.LightenScaleColor(50,pDC->GetPixel(rc.right-1,rc.bottom-1)));
		
		g_GUI.DrawLine(pDC,rc.left+1,rc.top,rc.right-1,rc.top,m_cor[5]);
		g_GUI.DrawLine(pDC,rc.left,rc.top+1,rc.left,rc.bottom-1,m_cor[5]);
		
		g_GUI.DrawLine(pDC,rc.left+1,rc.bottom-1,rc.right-1,rc.bottom-1,m_cor[6]);
		g_GUI.DrawLine(pDC,rc.right-1,rc.top+1,rc.right-1,rc.bottom-1,m_cor[6]);
		
		rc.DeflateRect(2,2,2,2);
		g_GUI.DrawGradient(pDC,rc,m_cor[3],m_cor[4],!bHorizontal);
	}
	if(m_style==MSBS_SIMPLE)
	{
	//	pDC->FillSolidRect(rc,m_cor[5]);
		g_d.Draw3DBar2(pDC, rc, m_cor[5], FALSE);
		g_GUI.DrawLine(pDC,rc.left,rc.top,rc.right,rc.top,m_cor[1]);
		g_GUI.DrawLine(pDC,rc.left,rc.top,rc.left,rc.bottom,m_cor[1]);
		g_GUI.DrawLine(pDC,rc.left+1,rc.top+1,rc.right,rc.top+1,m_cor[3]);
		g_GUI.DrawLine(pDC,rc.left+1,rc.top+1,rc.left+1,rc.bottom,m_cor[3]);
	}
}

void CMyScrollBar::DrawLeftArrow(CDC *pDC,CRect rc)
{
	if(m_style==MSBS_DEFAULT)
	{
		int xtemp=(rc.Width()+1-6)/2-1;
		int ytemp=(rc.Height()+1-9)/2;
		CPoint basePT(rc.left,rc.top);
		basePT.Offset(xtemp,ytemp);
		g_GUI.DrawLine(pDC,basePT.x+4,basePT.y,basePT.x,basePT.y+4,m_cor[8]);
		g_GUI.DrawLine(pDC,basePT.x,basePT.y+4,basePT.x+5,basePT.y+9,m_cor[8]);
		g_GUI.DrawLine(pDC,basePT.x+4,basePT.y+1,basePT.x+1,basePT.y+4,m_cor[8]);
		g_GUI.DrawLine(pDC,basePT.x+1,basePT.y+4,basePT.x+5,basePT.y+8,m_cor[8]);
		g_GUI.DrawLine(pDC,basePT.x+5,basePT.y+1,basePT.x+2,basePT.y+4,m_cor[8]);
		g_GUI.DrawLine(pDC,basePT.x+2,basePT.y+4,basePT.x+6,basePT.y+8,m_cor[8]);
	}
	if(m_style==MSBS_SIMPLE)
	{
		g_GUI.DrawLine(pDC,rc.right,rc.top,rc.right,rc.bottom,m_cor[0]);
		rc.right-=1;
		int nLayer=rc.Height()-2-1;
		int x=rc.right-(rc.Width()-nLayer)/2;
		int y=rc.top+2;
		for(int i=0;i<nLayer;i++)
			g_GUI.DrawLine(pDC,x-i,y+i/2,x-i,y+nLayer-i/2,m_cor[6]);
	}
}
void CMyScrollBar::DrawRightArrow(CDC *pDC,CRect rc)
{
	if(m_style==MSBS_DEFAULT)
	{
		int xtemp=(rc.Width()+1-6)/2+1;
		int ytemp=(rc.Height()+1-9)/2;
		CPoint basePT(rc.left,rc.top);
		basePT.Offset(xtemp,ytemp);
		CPen Pen(PS_SOLID,1,m_cor[8]),*pOldPen;
		pOldPen=pDC->SelectObject(&Pen);
		pDC->MoveTo(basePT.x+1,basePT.y);
		pDC->LineTo(basePT.x+5,basePT.y+4);
		pDC->LineTo(basePT.x,basePT.y+9);
		pDC->MoveTo(basePT.x+1,basePT.y+1);
		pDC->LineTo(basePT.x+4,basePT.y+4);
		pDC->LineTo(basePT.x,basePT.y+8);
		pDC->MoveTo(basePT.x,basePT.y+1);
		pDC->LineTo(basePT.x+3,basePT.y+4);
		pDC->LineTo(basePT.x-1,basePT.y+8);
		pDC->SelectObject(pOldPen);
		Pen.DeleteObject();
	}
	if(m_style==MSBS_SIMPLE)
	{
		g_GUI.DrawLine(pDC,rc.left,rc.top,rc.left,rc.bottom,m_cor[0]);
		rc.left+=1;
		int nLayer=rc.Height()-2-1;
		int x=rc.left+(rc.Width()-nLayer)/2;
		int y=rc.top+2;
		for(int i=0;i<nLayer;i++)
			g_GUI.DrawLine(pDC,x+i,y+i/2,x+i,y+nLayer-i/2,m_cor[6]);
	}
}
void CMyScrollBar::DrawUpArrow(CDC *pDC,CRect rc)
{
	if(m_style==MSBS_DEFAULT)
	{
		int xtemp=(rc.Width()+1-9)/2;
		int ytemp=(rc.Height()+1-6)/2-1;
		CPoint basePT(rc.left,rc.top);
		basePT.Offset(xtemp,ytemp);
		CPen Pen(PS_SOLID,1,m_cor[8]),*pOldPen;
		pOldPen=pDC->SelectObject(&Pen);
		
		pDC->MoveTo(basePT.x,basePT.y+4);
		pDC->LineTo(basePT.x+4,basePT.y);
		pDC->LineTo(basePT.x+9,basePT.y+5);
		
		pDC->MoveTo(basePT.x+1,basePT.y+4);
		pDC->LineTo(basePT.x+4,basePT.y+1);
		pDC->LineTo(basePT.x+8,basePT.y+5);
		
		pDC->MoveTo(basePT.x+1,basePT.y+5);
		pDC->LineTo(basePT.x+4,basePT.y+2);
		pDC->LineTo(basePT.x+8,basePT.y+6);
		
		pDC->SelectObject(pOldPen);
		Pen.DeleteObject();
	}
	if(m_style==MSBS_SIMPLE)
	{
		g_GUI.DrawLine(pDC,rc.left,rc.bottom-1,rc.right,rc.bottom-1,m_cor[0]);
		rc.bottom-=1;
		int nLayer=rc.Width()-2-1;
		int x=rc.left+2;
		int y=rc.bottom-(rc.Height()-nLayer)/2;
		for(int i=0;i<nLayer;i++)
			g_GUI.DrawLine(pDC,x+i/2,y-i,x+nLayer-i/2,y-i,m_cor[6]);
	}
}
void CMyScrollBar::DrawDownArrow(CDC *pDC,CRect rc)
{
	if(m_style==MSBS_DEFAULT)
	{
		int xtemp=(rc.Width()+1-9)/2;
		int ytemp=(rc.Height()+1-6)/2+1;
		CPoint basePT(rc.left,rc.top);
		basePT.Offset(xtemp,ytemp);
		CPen Pen(PS_SOLID,1,m_cor[8]),*pOldPen;
		pOldPen=pDC->SelectObject(&Pen);
		
		pDC->MoveTo(basePT.x+1,basePT.y);
		pDC->LineTo(basePT.x+4,basePT.y+3);
		pDC->LineTo(basePT.x+8,basePT.y-1);
		
		pDC->MoveTo(basePT.x+1,basePT.y+1);
		pDC->LineTo(basePT.x+4,basePT.y+4);
		pDC->LineTo(basePT.x+8,basePT.y);
		
		pDC->MoveTo(basePT.x,basePT.y+1);
		pDC->LineTo(basePT.x+4,basePT.y+5);
		pDC->LineTo(basePT.x+9,basePT.y);
		
		pDC->SelectObject(pOldPen);
		Pen.DeleteObject();
	}
	if(m_style==MSBS_SIMPLE)
	{
		g_GUI.DrawLine(pDC,rc.left,rc.top,rc.right,rc.top,m_cor[0]);
		rc.top+=1;
		int nLayer=rc.Width()-2-1;
		int x=rc.left+2;
		int y=rc.top+(rc.Height()-nLayer)/2;
		for(int i=0;i<nLayer;i++)
			g_GUI.DrawLine(pDC,x+i/2,y+i,x+nLayer-i/2,y+i,m_cor[6]);
	}
}

void CMyScrollBar::DrawButtonMark(CDC *pDC,CRect rc,BOOL bHorizontal)
{
	if(m_style==MSBS_DEFAULT)
	{
		int xtemp=(int)((rc.Width()-(bHorizontal?8:7))/(float)2+0.5);
		int ytemp=(int)((rc.Height()-(bHorizontal?7:8))/(float)2+0.5);
		CPoint basePT(rc.left,rc.top);
		basePT.Offset(xtemp,ytemp);
		CPen Pen1,Pen2,*pOldPen;
		Pen1.CreatePen(PS_SOLID,1,m_cor[9]);
		Pen2.CreatePen(PS_SOLID,1,g_GUI.DarkenScaleColor(100,m_cor[9]));
		
		pOldPen=pDC->SelectObject(&Pen1);
		if(bHorizontal)
		{
			for(int i=0;i<4;i++)
			{
				pDC->MoveTo(basePT.x+i*2,basePT.y);
				pDC->LineTo(basePT.x+i*2,basePT.y+6);
			}
			pDC->SelectObject(&Pen2);
			for(i=0;i<4;i++)
			{
				pDC->MoveTo(basePT.x+1+i*2,basePT.y+1);
				pDC->LineTo(basePT.x+1+i*2,basePT.y+7);
			}
		}
		else
		{
			for(int i=0;i<4;i++)
			{
				pDC->MoveTo(basePT.x,basePT.y+i*2);
				pDC->LineTo(basePT.x+6,basePT.y+i*2);
			}
			pDC->SelectObject(&Pen2);
			for(i=0;i<4;i++)
			{
				pDC->MoveTo(basePT.x,basePT.y+1+i*2);
				pDC->LineTo(basePT.x+6,basePT.y+1+i*2);
			}
		}
		
		pDC->SelectObject(pOldPen);
		Pen1.DeleteObject();
		Pen2.DeleteObject();
	}
}

void CMyScrollBar::DrawScrollBar()
{
	CPen Pen2,*OldPen2;
	CDC *dc,*desDc; 
	CRect rc;
	float ltmp;
	desDc=GetDC();

	if ( m_nDrawStyle == -1) return;
	GetClientRect(&rc);

	dc=new CDC();
	dc->CreateCompatibleDC(desDc);
	CBitmap membm;
	membm.CreateCompatibleBitmap(desDc,rc.Width(),rc.Height());
	dc->SelectObject(&membm);
	Pen2.CreatePen(PS_SOLID,1,m_cor[0]);
	OldPen2 =dc->SelectObject(&Pen2);

	CRect rtButton;
	CPoint basePT;
	if  (m_nDrawStyle == X_HORIZON )  // X_HORIZON   1 // 水平
	{
		//画边框
		if(m_style==MSBS_DEFAULT)
		{
			dc->FrameRect(rc,&CBrush(m_cor[0]));
			g_GUI.DrawGradient(dc,CRect(1,1,rc.Width()-1,rc.Height()-1),m_cor[1],m_cor[2],FALSE);
		}
		if(m_style==MSBS_SIMPLE)
		{
			dc->FrameRect(rc,&CBrush(m_cor[0]));
			g_d.Draw3DBar2(dc, CRect(1,1,rc.Width()-2,rc.Height()-2), m_cor[7], FALSE);
		//	dc->FillSolidRect(1,1,rc.Width()-2,rc.Height()-2,m_cor[7]);
		}

		//画左右按钮
		if(rc.Width()>rc.Height()*2+2 && m_style==MSBS_DEFAULT)
		{
			//绘画左按钮
			rtButton.SetRect(0,0,rc.Height(),rc.Height()-2);
			rtButton.OffsetRect(1,1);
			DrawButtonBG(dc,rtButton,TRUE);
			DrawLeftArrow(dc,rtButton);
			
			//绘画右按钮
			rtButton.SetRect(0,0,rc.Height(),rc.Height()-2);
			rtButton.OffsetRect(rc.Width()-1-rtButton.Width(),1);
			DrawButtonBG(dc,rtButton,TRUE);
			DrawRightArrow(dc,rtButton);
		}
		if(rc.Width()>rc.Height()*4+2 && m_style==MSBS_SIMPLE)
		{
			//绘画左按钮
			rtButton.SetRect(0,0,rc.Height()*2,rc.Height()-2);
			rtButton.OffsetRect(1,1);
			DrawButtonBG(dc,rtButton,TRUE);
			DrawLeftArrow(dc,rtButton);
			
			//绘画右按钮
			rtButton.SetRect(0,0,rc.Height()*2,rc.Height()-2);
			rtButton.OffsetRect(rc.Width()-1-rtButton.Width(),1);
			DrawButtonBG(dc,rtButton,TRUE);
			DrawRightArrow(dc,rtButton);
		}

		if(m_nMaxPos>1 && rc.Width()>rtButton.Width()*2+1*2+2+40)
		{
			//绘画移动条	
			int moveacer,tmp;
			moveacer = rc.Width()-rtButton.Width()*2-2;
			ltmp = (float)moveacer/m_nMaxPos;
			if( ltmp < 40. ) 
				ltmp=40.;
			m_step	= (float)(moveacer-ltmp) /((float)m_nMaxPos-1);
			m_movebarwidth  = (int)(ltmp+0.5);

			tmp = rtButton.Width()+1+(int)((float)(m_nScrollBarPos)*m_step+0.5);
			m_rcBar.SetRect(tmp,1,tmp+m_movebarwidth,rc.Height()-1);
			if(m_rcBar.left<1+rtButton.Width())
				m_rcBar.OffsetRect(1+rtButton.Width()-m_rcBar.left,0);
			if(m_rcBar.right>rc.Width()-1-rtButton.Width())
				m_rcBar.OffsetRect(rc.Width()-1-rtButton.Width()-m_rcBar.right,0);
			DrawButtonBG(dc,m_rcBar,TRUE);
			DrawButtonMark(dc,m_rcBar,TRUE);
		}
	}
	else   //X_VERTICAL  0 // 竖直
	{
		//画边框
		if(m_style==MSBS_DEFAULT)
		{
			dc->FrameRect(rc,&CBrush(m_cor[0]));
			g_GUI.DrawGradient(dc,CRect(1,1,rc.Width()-1,rc.Height()-1),m_cor[1],m_cor[2],TRUE);
		}
		if(m_style==MSBS_SIMPLE)
		{
			dc->FrameRect(rc,&CBrush(m_cor[0]));
		//	dc->FillSolidRect(1,1,rc.Width()-2,rc.Height()-2,m_cor[7]);
			g_d.Draw3DBar2(dc, CRect(1,1,rc.Width()-2,rc.Height()-2), m_cor[7], FALSE);
		}

		//画上下按钮
		if(rc.Height()>rc.Width()*2+2 && m_style==MSBS_DEFAULT)
		{
			//绘画上按钮
			rtButton.SetRect(0,0,rc.Width()-2,rc.Width());
			rtButton.OffsetRect(1,1);
			DrawButtonBG(dc,rtButton,FALSE);
			DrawUpArrow(dc,rtButton);
			
			//绘画下按钮
			rtButton.SetRect(0,0,rc.Width()-2,rc.Width());
			rtButton.OffsetRect(1,rc.Height()-1-rtButton.Height());
			DrawButtonBG(dc,rtButton,FALSE);
			DrawDownArrow(dc,rtButton);
		}
		if(rc.Height()>rc.Width()*4+2 && m_style==MSBS_SIMPLE)
		{
			//绘画上按钮
			rtButton.SetRect(0,0,rc.Width()-2,rc.Width()*2);
			rtButton.OffsetRect(1,1);
			DrawButtonBG(dc,rtButton,FALSE);
			DrawUpArrow(dc,rtButton);
			
			//绘画下按钮
			rtButton.SetRect(0,0,rc.Width()-2,rc.Width()*2);
			rtButton.OffsetRect(1,rc.Height()-1-rtButton.Height());
			DrawButtonBG(dc,rtButton,FALSE);
			DrawDownArrow(dc,rtButton);
		}

		if(m_nMaxPos>1 && rc.Height()>rtButton.Height()*2+1*2+2+40)
		{
			//绘画移动条
			int moveacer,tmp;
			moveacer = rc.Height()-rtButton.Height()*2-2;
			ltmp = (float)moveacer/m_nMaxPos;
			if( ltmp < 40. ) 
				ltmp=40.;

			m_step	= ((float)(moveacer-ltmp) ) /((float)m_nMaxPos-1);
			m_movebarwidth  = (int)(ltmp+0.5) ;
			
			tmp = rtButton.Height()+1+(int)((float)(m_nScrollBarPos)*m_step+0.5);
			m_rcBar.SetRect(1,tmp,rc.Width()-1,tmp+m_movebarwidth);
			if(m_rcBar.top<1+rtButton.Height())
				m_rcBar.OffsetRect(0,1+rtButton.Height()-m_rcBar.top);
			if(m_rcBar.bottom>rc.Height()-1-rtButton.Height())
				m_rcBar.OffsetRect(0,rc.Height()-1-rtButton.Height()-m_rcBar.bottom);
			DrawButtonBG(dc,m_rcBar,FALSE);
			DrawButtonMark(dc,m_rcBar,FALSE);
		}
	}
	desDc->BitBlt(0,0,rc.Width(),rc.Height(),dc,0,0,SRCCOPY);
	dc->SelectObject(OldPen2);
	Pen2.DeleteObject();
	dc->DeleteDC();
	delete dc;
	membm.DeleteObject();
	ReleaseDC(desDc);
}

//*************************   Mouse Event Process ****************************************************
void CMyScrollBar::MouseMove(MSG *pMsg,int flag)
{
	CRect  rc,rctmp;
	int sjxwidth;
	int tmp;
	int oldpos;
	CPoint point;

	if ( m_nDrawStyle == -1) return ;
	if ( m_nMouseStat == 1 && flag == 0) return ;
	
	GetWindowRect(&rc);

	point.x =(short) LOWORD(pMsg->lParam);
	point.y =(short) HIWORD(pMsg->lParam);
	
	
	if  (m_nDrawStyle == X_HORIZON  )//X_HORIZON   1 // 水平
	{
		if(m_style==MSBS_DEFAULT)	sjxwidth=rc.Height()-2;
		if(m_style==MSBS_SIMPLE)	sjxwidth=rc.Height()*2-2;

		oldpos	= m_nScrollBarPos;
		switch(flag)  
		{
		case 0:
		    {
				tmp=point.x-(m_rcBar.left+m_movebarwidth/2);
				rctmp=m_rcBar;
				if(tmp!=0)
					rctmp.OffsetRect(tmp,0);
				if(rctmp.left<1+sjxwidth)
					rctmp.OffsetRect(1+sjxwidth-rctmp.left,0);
				if(rctmp.right>rc.Width()-1-sjxwidth)
					rctmp.OffsetRect(rc.Width()-1-sjxwidth-rctmp.right,0);
				m_nScrollBarPos=(int)((float)(rctmp.left-1-sjxwidth)/m_step+0.5);
		    }
			break;
		case 1:
		    {
				m_nScrollBarPos--;
		    }
		    break;
		case 2:
			{
				m_nScrollBarPos ++ ;
			}
			break;
		}
		if ( m_nScrollBarPos < m_nMinPos ) m_nScrollBarPos = m_nMinPos;
		if ( m_nScrollBarPos >= m_nMaxPos ) m_nScrollBarPos =  m_nMaxPos-1;
		m_rcBar.left=(int)(1+sjxwidth+m_nScrollBarPos*m_step+0.5);
		m_rcBar.right=m_rcBar.left+m_movebarwidth;
		
		if(oldpos!=m_nScrollBarPos)
		{
			DrawScrollBar();
			//send notif message 
			::SendMessage(m_MsghWnd,m_Msg,m_nScrollBarPos,m_WhichUnit);
		}
	}
	else
	{
		if(m_style==MSBS_DEFAULT)	sjxwidth = rc.Width()-2;
		if(m_style==MSBS_SIMPLE)	sjxwidth = rc.Width()*2-2;

		switch (flag )
		{
		case 0:
			{
				tmp=point.y-(m_rcBar.top+m_movebarwidth/2);
				rctmp=m_rcBar;
				oldpos = m_nScrollBarPos;
				if(tmp!=0)
					rctmp.OffsetRect(0,tmp);
				if(rctmp.top<1+sjxwidth)
					rctmp.OffsetRect(0,1+sjxwidth-rctmp.top);
				if(rctmp.bottom>rc.Height()-1-sjxwidth)
					rctmp.OffsetRect(0,rc.Height()-1-sjxwidth-rctmp.bottom);
				m_nScrollBarPos = (int)((float)(rctmp.top-sjxwidth-1)/ (float)m_step+0.5 ); 
			}
			 break;
		case 1:
			{
				if ( m_nScrollBarPos >0 ) m_nScrollBarPos--;
			}
			 break;
		case 2:
			{
				m_nScrollBarPos ++ ;
			}
			 break;
		}
		if ( m_nScrollBarPos < m_nMinPos ) m_nScrollBarPos = m_nMinPos;
		if ( m_nScrollBarPos >= m_nMaxPos ) m_nScrollBarPos =  m_nMaxPos-1;
		m_rcBar.top=(int)(1+sjxwidth+m_nScrollBarPos*m_step+0.5);
		m_rcBar.bottom=m_rcBar.top+m_movebarwidth;
		
		if(oldpos!=m_nScrollBarPos)
		{
			DrawScrollBar();
			//send notif message 
			::SendMessage(m_MsghWnd,m_Msg,m_nScrollBarPos,m_WhichUnit);
		}
	}
}

void CMyScrollBar::LButtonDown(MSG *pMsg)
{
	CRect  rc;
	int sjxwidth;
	int moveacer,tmp;
	float ltmp;
	
	if ( m_nDrawStyle == -1) return ;
	CPoint point(LOWORD(pMsg->lParam),HIWORD(pMsg->lParam));

	GetWindowRect(&rc);

	if  (m_nDrawStyle == X_HORIZON )  //X_HORIZON   1 // 水平
	{
		if(m_style==MSBS_DEFAULT)	sjxwidth=rc.Height()-2;
		if(m_style==MSBS_SIMPLE)	sjxwidth=rc.Height()*2-2;
		moveacer = rc.Width()-sjxwidth*2-1*2;
		
		ltmp = (float)moveacer/m_nMaxPos;
		if( ltmp < 40. ) 
			ltmp=40.;
		m_step	= (float)(moveacer-ltmp) /((float)m_nMaxPos-1);
		m_movebarwidth  = (int)(ltmp+0.5);
		//in move acer 
		if (  point.x <= sjxwidth +1) 
		{
			MouseMove(pMsg,1);
		}
		else
		{
			tmp = rc.Width()-sjxwidth-1;
			if (  point.x > tmp ) 
			{
				// go right
				MouseMove(pMsg,2);
			}
			else
			{
				m_nMouseStat = 0;
				MouseMove(pMsg,0);
				SetCapture();
			}
		}
	}
	else
	{
		if(m_style==MSBS_DEFAULT)	sjxwidth = rc.Width()-2;
		if(m_style==MSBS_SIMPLE)	sjxwidth = rc.Width()*2-2;
		moveacer = rc.Height()-sjxwidth*2-1*2;
		ltmp = (float)moveacer/m_nMaxPos;
		if( ltmp < 40. ) 
			ltmp=40.;
		m_step	= ((float)(moveacer-ltmp) ) /((float)m_nMaxPos-1);
		m_movebarwidth  = (int)(ltmp+0.5) ;

		//in move acer 
		if (  point.y <= sjxwidth +1) 
		{
			//go left
			MouseMove(pMsg,1);
		}
		else
		{
			tmp = rc.Height()-sjxwidth -1;
			if (  point.y > tmp ) 
			{
				// go right
				MouseMove(pMsg,2);
			}
			else
			{
				m_nMouseStat = 0;
				MouseMove(pMsg,0);
				SetCapture();
			}
		}
	}
}

void CMyScrollBar::LButtonUp(MSG *pMsg)
{
	CPoint point(LOWORD(pMsg->lParam),HIWORD(pMsg->lParam));
	if( m_nMouseStat == 0)
	{
		m_nMouseStat = 1;
		ReleaseCapture ();
	}
	MouseMove(pMsg,0);
}

void CMyScrollBar::OnPaint() 
{
	DrawScrollBar();
	CWnd::OnPaint();
}

BOOL CMyScrollBar::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

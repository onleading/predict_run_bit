// SwitchEx.cpp: implementation of the CNewZfjsTab class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SwitchEx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//构造函数
CNewZfjsTab::CNewZfjsTab(HWND hWnd,CWnd *pMainWnd)
{
	// 注册当前类
	RegisterWindowClass(NULL,_T("NewZfjsTab"));

	m_SwitchStyle	= SwitchStyle_default;
	bFirst			= TRUE;						//鼠标刚进入窗口区域
	m_MsghWnd		= hWnd;
	m_pMainWnd		= pMainWnd;
	m_bhasFLBtn		= FALSE;
	m_bhasDPBtn		= FALSE;
	m_nHotNo		= -1;						//当前选中的标签号
	m_nCurNo		= 0;
	m_nOffset		= 0;
	m_nLeftNo		= 0;
	m_IsMore		= FALSE;
	m_nAngleDx		= 0;
	left = top = right = bottom = 0;
	m_aRgn.clear();
	m_aBackRC[0].clear();
	m_aBackRC[1].clear();

	m_pSwitchMenuCallBack = NULL;
	
	//背景色 选中色 没有选中色 文字色 选中文字色 热感背景色 热感文字色 边框色	
	m_crHot		= RGB(128,128,128);
	m_crBk		= RGB(0,0,0);
	m_crBtnSelBk= RGB(48,48,48);
	m_crMk		= RGB(128,0,0);
	m_crUMk		= RGB(128,128,128);
	m_crTxt		= RGB(0,0,0);
	m_crMTxt	= RGB(192,192,192);
	m_crHTxt	= RGB(0,0,0);
	m_crLine	= m_crBk;
	m_crTipLine	= RGB(128,0,0);

	m_Font.CreateFont(14,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Tahoma");
	m_brBk.CreateSolidBrush(m_crBk);
	m_brHot.CreateSolidBrush(m_crHot);
	m_brLine.CreateSolidBrush(m_crLine);
	m_brUMk.CreateSolidBrush(m_crUMk);
	m_brMk.CreateSolidBrush(m_crMk);
	m_brTipLine.CreateSolidBrush(m_crTipLine);

	m_aTipRgn[0].CreateRectRgn(1,2,3,4);
	m_aTipRgn[1].CreateRectRgn(1,2,3,5);
}

//注册窗口类
BOOL CNewZfjsTab::RegisterWindowClass(HINSTANCE hInstance,LPCTSTR lpszClassName)
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

CNewZfjsTab::~CNewZfjsTab()
{
	FreeGDIObject();
	m_aTipRgn[0].DeleteObject();
	m_aTipRgn[1].DeleteObject();
}

BEGIN_MESSAGE_MAP(CNewZfjsTab, CWnd)
	//{{AFX_MSG_MAP(CNewZfjsTab)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//判断是否是暗色调
BOOL CNewZfjsTab::IsBlackColorScheme(BOOL bBkColor)
{
	DWORD tmpWord=bBkColor;
	return ((tmpWord&0x000000FF)<100 && ((tmpWord&0x0000FF00)>>8)<100 && ((tmpWord&0x00FF0000)>>16)<100);
}

//画线工具
void CNewZfjsTab::DrawLine(CDC *pDC,int x1,int y1,int x2,int y2,COLORREF color,int style,int width)
{
	CPen Pen,*OldPen;
	Pen.CreatePen(style,width,color);
	OldPen =pDC->SelectObject(&Pen);
	pDC->MoveTo(x1,y1);
	pDC->LineTo(x2,y2);
	pDC->SelectObject(OldPen);
	Pen.DeleteObject();
}

void CNewZfjsTab::SetFontColorSchema(COLORREF Background,COLORREF SelectedBackground,
							COLORREF unSelectedBackground,COLORREF HotBackground,
							COLORREF LineColor,COLORREF NormalTxtColor,
							COLORREF SelectTxtColor,COLORREF HotTxtColor,COLORREF TipLineColor,
							int Fontsize,char* FontName)
{
	FreeGDIObject();
	//背景色、选中色、没有选中色、文字色、选中文字色、热感背景色、热感文字色、边框色。
	m_crBk		= Background;
	m_crMk		= SelectedBackground;
	m_crUMk		= unSelectedBackground;
	m_crLine	= LineColor;
	m_crHot		= HotBackground;
	m_crTxt		= NormalTxtColor;
	m_crMTxt	= SelectTxtColor;
	m_crHTxt	= HotTxtColor;
	m_crTipLine = TipLineColor;
	BOOL bDefaultUseBold = TRUE;
	if(IsBlackColorScheme(m_crBk))
	{
		m_crBtnSelBk = RGB(28,28,28);
		if(m_SwitchStyle==SwitchStyle_switchbutton || m_SwitchStyle==SwitchStyle_samebutton || m_SwitchStyle == SwitchStyle_Vswitchbutton ||  m_SwitchStyle==SwitchStyle_Vsamebutton  )
			m_crMTxt = RGB(0,224,224);
		if(m_SwitchStyle==SwitchStyle_switchbutton2)
			m_crBtnSelBk = RGB(80,80,80);
	}
	else
	{
		m_crBtnSelBk = RGB(230,230,230);
		if(m_SwitchStyle==SwitchStyle_switchbutton || m_SwitchStyle==SwitchStyle_samebutton || m_SwitchStyle == SwitchStyle_Vswitchbutton || m_SwitchStyle==SwitchStyle_Vsamebutton )
			m_crMTxt = RGB(250,0,0);
		if(m_SwitchStyle==SwitchStyle_switchbutton2)
			m_crBtnSelBk = RGB(180,180,180);
		bDefaultUseBold = FALSE;
	}
	switch(m_SwitchStyle)  
	{
	case SwitchStyle_samebutton:
	case SwitchStyle_switchbutton:
	case SwitchStyle_Vswitchbutton:
	case SwitchStyle_switchbutton2:
	case SwitchStyle_custom1:
	case SwitchStyle_Vdefault:
	case SwitchStyle_Vsamebutton:
	case SwitchStyle_Vcustom1:
		m_Font.CreateFont(Fontsize,0,0,0,FW_NORMAL,0,0,0,
			DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,FontName);
		break;
	case SwitchStyle_default:

	default:
		m_Font.CreateFont(Fontsize,0,0,0,bDefaultUseBold?FW_BOLD:FW_NORMAL,0,0,0,
			DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY,DEFAULT_PITCH|FF_DONTCARE,FontName);
	    break;
	}
	m_brBk.CreateSolidBrush(m_crBk);
	m_brUMk.CreateSolidBrush(m_crUMk);
	m_brMk.CreateSolidBrush(m_crMk);
	m_brHot.CreateSolidBrush(m_crHot);
	m_brLine.CreateSolidBrush(m_crLine);
	m_brTipLine.CreateSolidBrush(m_crTipLine);

	for(int i=0;i<m_vTextColor.size();i++)
	{
		if(!m_vTextColor[i].bSet)
			m_vTextColor[i].cor=NormalTxtColor;
	}
}

void CNewZfjsTab::ClearAllLabel()
{
	m_aRgn.clear();
	m_aBackRC[0].clear();
	m_aBackRC[1].clear();
	m_vTextColor.clear();
	m_bhasFLBtn = FALSE;
	m_bhasDPBtn = FALSE;
}

void CNewZfjsTab::FreeGDIObject()
{
	m_Font.DeleteObject();
	m_brBk.DeleteObject();
	m_brHot.DeleteObject();
	m_brLine.DeleteObject();
	m_brUMk.DeleteObject();
	m_brMk.DeleteObject();
	m_brTipLine.DeleteObject();
}

LRESULT CNewZfjsTab::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if ( ProcessMsg(message,wParam,lParam) )
		return TRUE;
	return  CWnd::WindowProc(message,wParam,lParam);
}

int	CNewZfjsTab::ProcessMsg(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	int	   ret = 0;
	CPoint pt(LOWORD(lParam),HIWORD(lParam));	
	switch(Msg)
	{
	case WM_MOUSEMOVE:
		{
			ret = 1;
			if (bFirst)
			{
				bFirst = FALSE;
				SetTimer(1,100,NULL);
			}
			BOOL bIn=FALSE;
			for ( int i=0;i<m_aRgn.size();++i )
			{
				CRect tmprc=m_aRgn[i].rcOut;
				switch(m_SwitchStyle)  
				{
				case SwitchStyle_custom1:
					tmprc.DeflateRect(0,0,Height()/2,0);
					break;
				case SwitchStyle_Vcustom1:
					tmprc.DeflateRect(0,0,0,Width()/2);
					break;
				case SwitchStyle_default:
					tmprc.DeflateRect(Height()/4,0,Height()/4,0);
				    break;
				case SwitchStyle_Vdefault:
					tmprc.DeflateRect(0,Width()/4,0,Width()/4);
					break;
				default:
				    break;
				}
				if(tmprc.PtInRect(pt))
				{
					bIn=TRUE;
					if(m_nHotNo!=i && strlen(m_aRgn[i].tooltip)>0)
					{
						m_ToolTip.UpdateTipText(m_aRgn[i].tooltip,this);
						m_nHotNo=i;
						DrawSwitch();
					}
					break;
				}
			}
			if(!bIn) 
				m_ToolTip.UpdateTipText("",this);
		}
		break;
	case WM_LBUTTONDOWN:
		{ 
			ret = 1;
			int  i;
			if (!PtInRect(pt))
				break;
			if ( m_IsMore )
			{
				if ( m_aTipRct[0].PtInRect(pt) && m_nLeftNo > 0 )
				{
					m_nOffset = 0;
					m_nLeftNo = max(0,m_nLeftNo-1);
					for ( i=0;i<m_nLeftNo;++i )
						if (m_SwitchStyle == SwitchStyle_Vdefault || m_SwitchStyle == SwitchStyle_Vswitchbutton || m_SwitchStyle == SwitchStyle_Vsamebutton || m_SwitchStyle == SwitchStyle_Vcustom1)
							m_nOffset += m_aRgn[i].rcIn.Height()+m_nAngleDx;
						else
							m_nOffset += m_aRgn[i].rcIn.Width()+m_nAngleDx;
					for ( i=0;i<m_aRgn.size();++i )
					{
						m_aRgn[i].rcIn = m_aBackRC[0][i];
						m_aRgn[i].rcOut = m_aBackRC[1][i];
						CRect TRect = m_aRgn[i].rcIn;

						if (m_SwitchStyle == SwitchStyle_Vdefault || m_SwitchStyle == SwitchStyle_Vswitchbutton || m_SwitchStyle == SwitchStyle_Vsamebutton || m_SwitchStyle == SwitchStyle_Vcustom1)
						{
							m_aRgn[i].rcIn.OffsetRect(0,-m_nOffset);
							m_aRgn[i].rcOut.OffsetRect(0,-m_nOffset);							
						}
						else
						{
							m_aRgn[i].rcIn.OffsetRect(-m_nOffset,0);
							m_aRgn[i].rcOut.OffsetRect(-m_nOffset,0);							
						}
					}
					DrawSwitch();
				}
				else if ( m_aTipRct[1].PtInRect(pt) && m_aRgn.size()>0 )
				{	//实际坐标。
					if ( m_aRgn[m_aRgn.size()-1].rcOut.right >= right )
					{
						m_nOffset = 0;
						m_nLeftNo = min(m_aRgn.size()-1,m_nLeftNo+1);
						for ( i=0;i<m_nLeftNo;++i )
						{
							if (m_SwitchStyle == SwitchStyle_Vdefault || m_SwitchStyle == SwitchStyle_Vswitchbutton || m_SwitchStyle == SwitchStyle_Vsamebutton || m_SwitchStyle == SwitchStyle_Vcustom1)
								m_nOffset += m_aRgn[i].rcIn.Height()+m_nAngleDx;
							else
								m_nOffset += m_aRgn[i].rcIn.Width()+m_nAngleDx;
						}
						for ( i=0;i<m_aRgn.size();++i )
						{
							m_aRgn[i].rcIn = m_aBackRC[0][i];
							m_aRgn[i].rcOut = m_aBackRC[1][i];
							CRect TRect = m_aRgn[i].rcIn;

							if (m_SwitchStyle == SwitchStyle_Vdefault || m_SwitchStyle == SwitchStyle_Vswitchbutton || m_SwitchStyle == SwitchStyle_Vsamebutton || m_SwitchStyle == SwitchStyle_Vcustom1)
							{
								m_aRgn[i].rcIn.OffsetRect(0,-m_nOffset);
								m_aRgn[i].rcOut.OffsetRect(0,-m_nOffset);
							}
							else
							{
								m_aRgn[i].rcIn.OffsetRect(-m_nOffset,0);
								m_aRgn[i].rcOut.OffsetRect(-m_nOffset,0);
							}
						}
						DrawSwitch();
					}
				}
				//不要再进入下面分支
				if ( m_aTipRct[0].PtInRect(pt) || m_aTipRct[1].PtInRect(pt) )
					break;
			}
			for ( i=0;i<m_aRgn.size();++i )
			{
				if ( m_aRgn[i].rcOut.PtInRect(pt) )
				{
					if (m_aRgn.size() == 1 || m_aRgn[i].everclick || i != m_nCurNo || m_aRgn[i].isMenu )
					{
						SetCurLabel(i);
						SendOutMsg();
						DrawSwitch();
					}
					break;
				}
			}
		}
		break;
	case WM_SIZE:
		{
			int x = LOWORD(wParam),y = HIWORD(wParam);
			int cx = LOWORD(lParam),cy = HIWORD(lParam);
			if (m_SwitchStyle == SwitchStyle_Vdefault || m_SwitchStyle == SwitchStyle_Vswitchbutton || m_SwitchStyle == SwitchStyle_Vsamebutton || m_SwitchStyle == SwitchStyle_Vcustom1)
			{
				SetRectV(x,y,x + cx,y + cy);
			}
			else
			{
				SetRect(x,y,x+cx,y+cy);
			}
		}
		ret = 1;
		break;
	}
	return ret;
}

BOOL CNewZfjsTab::SetWindowPos ( const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags )
{
	if (m_SwitchStyle == SwitchStyle_Vdefault || m_SwitchStyle == SwitchStyle_Vswitchbutton || m_SwitchStyle == SwitchStyle_Vsamebutton || m_SwitchStyle == SwitchStyle_Vcustom1)
		SetRectV(0,0,cx,cy);
	else
		SetRect(0,0,cx,cy);
	return CWnd::SetWindowPos(pWndInsertAfter,x,y,cx,cy,nFlags);
}

void CNewZfjsTab::SetRectV(int x1,int y1,int x2,int y2)
{
	int		of = 3;
	CPoint	aPoint[3];									//三角箭头的三个顶点的坐标
	ZeroMemory(&aPoint,3 * sizeof(CPoint));
	//首先设置
	left = x1; top = y1; right = x2; bottom = y2;
	m_rcWnd.SetRect(left,top,right,bottom);
	////////////////////////////////
	m_aTipRgn[0].DeleteObject();
	m_aTipRgn[1].DeleteObject();
	//集中在一头
	m_aTipRct[0].SetRect(x1,y1,x2,y1+TIP_W);			//调整按键的矩形区域
	m_aTipRct[1].SetRect(x1,y1+TIP_W,x2,y1+2*TIP_W);	//调整按键的矩形区域
	/////////////////////////////////////
	aPoint[0] = m_aTipRct[0].BottomRight() + CPoint(-of,-of);
	aPoint[1] = m_aTipRct[0].TopLeft() + CPoint(of, m_aTipRct[0].Height() - of);
	aPoint[2] = m_aTipRct[0].TopLeft() + CPoint((x2 -x1)/2, of);
	m_aTipRgn[0].CreatePolygonRgn(aPoint,3,ALTERNATE);
	/////////////////////////////////
	aPoint[0] = m_aTipRct[1].TopLeft()+CPoint(m_aTipRct[1].Width()-of,of);
	aPoint[1] = m_aTipRct[1].TopLeft()+CPoint(of,of);
	aPoint[2] = m_aTipRct[1].BottomRight()-CPoint((x2-x1)/2,of);
	m_aTipRgn[1].CreatePolygonRgn(aPoint,3,ALTERNATE);
	////////////////////////////////////
	//总是先假设不必加入端扭
	//在需加入端扭，要重新分布时
	CDC * pDC = GetDC();
	CFont *oldF = pDC->SelectObject(&m_Font);
	int nFullWidth=CalcRgn(pDC);
	if(nFullWidth>bottom && !m_IsMore && bottom-top>0)
	{
		m_IsMore = TRUE;
		CRect tmprc1=m_aRgn[0].rcOut;
		for(int i=0;i<m_aRgn.size();i++)
		{
			m_aRgn[i].rcOut.OffsetRect(0,2*TIP_W);
			m_aRgn[i].rcIn.OffsetRect(0,2*TIP_W);
			m_aBackRC[0][i] = m_aRgn[i].rcIn;
			m_aBackRC[1][i] = m_aRgn[i].rcOut;
		}
		CRect tmprc2=m_aRgn[0].rcOut;
	}
	if(nFullWidth<=bottom && m_IsMore && bottom-top>0)
	{
		m_IsMore=FALSE;
		CRect tmprc1=m_aRgn[0].rcOut;
		for(int i=0;i<m_aRgn.size();i++)
		{
			m_aRgn[i].rcOut.OffsetRect(0,-2*TIP_W);
			m_aRgn[i].rcIn.OffsetRect(0,-2*TIP_W);
			m_aBackRC[0][i] = m_aRgn[i].rcIn;
			m_aBackRC[1][i] = m_aRgn[i].rcOut;
		}
		CRect tmprc2=m_aRgn[0].rcOut;
	}
	pDC->SelectObject(oldF);
	ReleaseDC(pDC);
	//
	if ( m_IsMore )
	{
		if ( m_nLeftNo >0 && m_nLeftNo<m_aRgn.size() )
		{
			int		i;
			m_nOffset = 0;
			for ( i=0;i<m_nLeftNo;++i )
				m_nOffset += m_aRgn[i].rcIn.Height()+m_nAngleDx;
			for ( i=0;i<m_aRgn.size();++i )
			{
				m_aRgn[i].rcIn.OffsetRect(0,-m_nOffset);
				m_aRgn[i].rcOut.OffsetRect(0,-m_nOffset);
			}
		}
	}
	else
	{
		m_nLeftNo = 0;
		m_nOffset = 0;
	}
}


void CNewZfjsTab::SetRect(int x1,int y1,int x2,int y2)
{
	int		of = 3;
	CPoint	aPoint[3];									//三角箭头的三个顶点的坐标
	ZeroMemory(&aPoint,3 * sizeof(CPoint));
	//首先设置
	left = x1; top = y1; right = x2; bottom = y2;
	m_rcWnd.SetRect(left,top,right,bottom);
	////////////////////////////////
	m_aTipRgn[0].DeleteObject();
	m_aTipRgn[1].DeleteObject();
	//集中在一头
	m_aTipRct[0].SetRect(x1,y1,x1+TIP_W,y2);			//调整按键的矩形区域
	m_aTipRct[1].SetRect(x1+TIP_W,y1,x1+2*TIP_W,y2);	//调整按键的矩形区域
	/////////////////////////////////////
	aPoint[0] = m_aTipRct[0].TopLeft() + CPoint(m_aTipRct[0].Width()-of,of);
	aPoint[1] = m_aTipRct[0].BottomRight() - CPoint(of,of);
	aPoint[2] = m_aTipRct[0].TopLeft() + CPoint(of,(y2-y1)/2);
	m_aTipRgn[0].CreatePolygonRgn(aPoint,3,ALTERNATE);
	/////////////////////////////////
	aPoint[0] = m_aTipRct[1].TopLeft()+CPoint(of,of);
	aPoint[1] = m_aTipRct[1].BottomRight()-CPoint(m_aTipRct[1].Width()-of,of);
	aPoint[2] = m_aTipRct[1].BottomRight()-CPoint(of,(y2-y1)/2);
	m_aTipRgn[1].CreatePolygonRgn(aPoint,3,ALTERNATE);
	////////////////////////////////////
	//总是先假设不必加入端扭
	//在需加入端扭，要重新分布时
	CDC * pDC = GetDC();
	CFont *oldF = pDC->SelectObject(&m_Font);
	int nFullWidth=CalcRgn(pDC);
	if(nFullWidth>right && !m_IsMore && right-left>0)
	{
		m_IsMore = TRUE;
		CRect tmprc1=m_aRgn[0].rcOut;
		for(int i=0;i<m_aRgn.size();i++)
		{
			m_aRgn[i].rcOut.OffsetRect(2*TIP_W,0);
			m_aRgn[i].rcIn.OffsetRect(2*TIP_W,0);
			m_aBackRC[0][i] = m_aRgn[i].rcIn;
			m_aBackRC[1][i] = m_aRgn[i].rcOut;
		}
		CRect tmprc2=m_aRgn[0].rcOut;
	}
	if(nFullWidth<=right && m_IsMore && right-left>0)
	{
		m_IsMore=FALSE;
		CRect tmprc1=m_aRgn[0].rcOut;
		for(int i=0;i<m_aRgn.size();i++)
		{
			m_aRgn[i].rcOut.OffsetRect(-2*TIP_W,0);
			m_aRgn[i].rcIn.OffsetRect(-2*TIP_W,0);
			m_aBackRC[0][i] = m_aRgn[i].rcIn;
			m_aBackRC[1][i] = m_aRgn[i].rcOut;
		}
		CRect tmprc2=m_aRgn[0].rcOut;
	}
	pDC->SelectObject(oldF);
	ReleaseDC(pDC);
	//
	if ( m_IsMore )
	{
		if ( m_nLeftNo >0 && m_nLeftNo<m_aRgn.size() )
		{
			int		i;
			m_nOffset = 0;
			for ( i=0;i<m_nLeftNo;++i )
				m_nOffset += m_aRgn[i].rcIn.Width()+m_nAngleDx;
			for ( i=0;i<m_aRgn.size();++i )
			{
				m_aRgn[i].rcIn.OffsetRect(-m_nOffset,0);
				m_aRgn[i].rcOut.OffsetRect(-m_nOffset,0);
			}
		}
	}
	else
	{
		m_nLeftNo = 0;
		m_nOffset = 0;
	}
}

int CNewZfjsTab::GetNeedWidth()
{
	CDC * pDC = GetDC();
	CFont *oldF = pDC->SelectObject(&m_Font);
	int nRet = CalcRgn(pDC);
	if( m_IsMore )
	{
		for(int i=0;i<m_aRgn.size();i++)
		{
			m_aRgn[i].rcOut.OffsetRect(2*TIP_W,0);
			m_aRgn[i].rcIn.OffsetRect(2*TIP_W,0);
			m_aBackRC[0][i] = m_aRgn[i].rcIn;
			m_aBackRC[1][i] = m_aRgn[i].rcOut;
		}
	}
	pDC->SelectObject(oldF);
	ReleaseDC(pDC);
	return nRet;
}

int CNewZfjsTab::CalcRgn(CDC *pDC)
{
	int		i,len;
	int		dx = 0;
	int		s = 0;
	int		dy = 0;
	int		e =0;
	if (m_SwitchStyle == SwitchStyle_Vdefault || m_SwitchStyle == SwitchStyle_Vswitchbutton || m_SwitchStyle == SwitchStyle_Vsamebutton || m_SwitchStyle == SwitchStyle_Vcustom1)
	{
		s = top;
		dy = right - left;
		dx = dy / 2;						//半宽
		e = top + dx;						
		m_nAngleDx = dx;
	}
	else
	{
		s=left;
		dy = bottom - top;
		dx = dy / 2;						//半宽
		e = left + dx;						
		m_nAngleDx = dx;					//高度
	}

	int		n = m_aRgn.size();

	m_aBackRC[0].resize(m_aRgn.size());
	m_aBackRC[1].resize(m_aRgn.size());
	
	switch(m_SwitchStyle)
	{
	case SwitchStyle_custom1:
		{
			for(i=0;i<n;i++)
			{
				len=pDC->GetTextExtent(m_aRgn[i].caption).cx;
				e+=len+dx*2;
				m_aRgn[i].rcOut.SetRect(s,bottom-dy,e,bottom);
				m_aRgn[i].rcIn.SetRect(s+dx,bottom-dy,e-dx*2,bottom);
				m_aBackRC[0][i] = m_aRgn[i].rcIn;
				m_aBackRC[1][i] = m_aRgn[i].rcOut;
				s=e-dx;
			}
		}
		break;

	case SwitchStyle_Vcustom1:
		{
			for(i=0;i<n;i++)
			{
				CString str = m_aRgn[i].caption;
				UINT nLen = str.GetLength();
				UINT nHeight = nLen / 2 * pDC->GetTextExtent(str).cy;	
				len = nHeight;
				e += len + 2 * dx; 
				m_aRgn[i].rcOut.SetRect(right-dy,s,right,e);
				m_aRgn[i].rcIn.SetRect(right-dy,s+dx,right,e-dx*2);
				m_aBackRC[0][i] = m_aRgn[i].rcIn;
				m_aBackRC[1][i] = m_aRgn[i].rcOut;
				s=e-dx;
			}
		}
		break;

	case SwitchStyle_default:
		{
			for ( i=0;i<n;++i )
			{
				if(m_aRgn[i].isMenu)			//如果是带菜单的标签，就在标签的文字后加上“▲”
				{
					if(m_aRgn[i].caption.Find(_F("▲"))==-1)
						m_aRgn[i].caption+=_F("▲");
				}
				len = pDC->GetTextExtent(m_aRgn[i].caption).cx-4;
				e += len + 2*dx; 
				m_aRgn[i].rcOut.SetRect(s,bottom-dy,e,bottom);
				m_aRgn[i].rcIn.SetRect(s+dx,bottom-dy,e-dx,bottom);
				s  = e - dx;
				m_aBackRC[0][i] = m_aRgn[i].rcIn;
				m_aBackRC[1][i] = m_aRgn[i].rcOut;
			}
		}
		break;

	case SwitchStyle_Vdefault:
		{
			for ( i=0;i<n;++i )
			{
				if(m_aRgn[i].isMenu)
				{
					if(m_aRgn[i].caption.Find(_F("▲"))==-1)
						m_aRgn[i].caption+=_F("▲");
				}
				CString str = m_aRgn[i].caption;
				UINT nLen = str.GetLength();
				UINT nHeight = nLen / 2 * pDC->GetTextExtent(str).cy;

				len = nHeight - 4;
				e += len + 2 * dx; 
				m_aRgn[i].rcOut.SetRect(right-dy,s,right,e);
				m_aRgn[i].rcIn.SetRect(right-dy,s+dx,right,e-dx);
				s  = e - dx;
				m_aBackRC[0][i] = m_aRgn[i].rcIn;
				m_aBackRC[1][i] = m_aRgn[i].rcOut;
			}
		}
		break;

	case SwitchStyle_switchbutton:
		{
			s=left+1,e=left;
			for ( i=0;i<n;++i )
			{
				len = pDC->GetTextExtent(m_aRgn[i].caption).cx+8;
				e += len; 
				if(e>right) e=right;
				m_aRgn[i].rcOut.SetRect(s,bottom-dy,e,bottom);
				m_aRgn[i].rcIn=m_aRgn[i].rcOut;
				m_aRgn[i].rcIn.DeflateRect(0,1,1,1);
				s  = e;
				m_aBackRC[0][i] = m_aRgn[i].rcIn;
				m_aBackRC[1][i] = m_aRgn[i].rcOut;
			}
		}
		break;
	case SwitchStyle_Vswitchbutton:
		{
			s = top + 1,e = top;
			for (i = 0;i < n; i++)
			{
				CString str = m_aRgn[i].caption;
				UINT nLen = str.GetLength();
				UINT nHeight = nLen / 2 * pDC->GetTextExtent(str).cy;
				e += (nHeight + 8);
				if (e>bottom)
					e = bottom;
				m_aRgn[i].rcOut.SetRect(right - dy,s,right,e);
				m_aRgn[i].rcIn=m_aRgn[i].rcOut;
				m_aRgn[i].rcIn.DeflateRect(1,0,1,1);
				s = e;
				m_aBackRC[0][i] = m_aRgn[i].rcIn;
				m_aBackRC[1][i] = m_aRgn[i].rcOut;
			}
		}
		break;
	case SwitchStyle_switchbutton2:
		{
			s=left+1,e=left;
			for ( i=0;i<n;++i )
			{
				len = Width()/n;
				e += len; 
				if(i==n-1) e=right;
				m_aRgn[i].rcOut.SetRect(s,bottom-dy,e,bottom);
				m_aRgn[i].rcIn=m_aRgn[i].rcOut;
				m_aRgn[i].rcIn.DeflateRect(0,1,1,1);
				s  = e;
				m_aBackRC[0][i] = m_aRgn[i].rcIn;
				m_aBackRC[1][i] = m_aRgn[i].rcOut;
			}
		}
		break;
	case SwitchStyle_samebutton:
		{
			s=left+1,e=left;
			for ( i=0;i<n;++i )
			{
				len = Width()/n;
				e += len; 
				if(i==n-1) e=right;
				m_aRgn[i].rcOut.SetRect(s,bottom-dy,e,bottom);
				m_aRgn[i].rcIn=m_aRgn[i].rcOut;
				m_aRgn[i].rcIn.DeflateRect(0,1,1,1);
				s  = e;
				m_aBackRC[0][i] = m_aRgn[i].rcIn;
				m_aBackRC[1][i] = m_aRgn[i].rcOut;
			}
		}
		break;

	case SwitchStyle_Vsamebutton:
		{
			s=top+1,e=top;
			for ( i=0;i<n;++i )
			{
				len = Height()/n;
				e += len; 
				if(i==n-1) e=bottom;
				m_aRgn[i].rcOut.SetRect(right - dy,s,right,e);
				m_aRgn[i].rcIn=m_aRgn[i].rcOut;
				m_aRgn[i].rcIn.DeflateRect(1,0,1,1);
				s  = e;
				m_aBackRC[0][i] = m_aRgn[i].rcIn;
				m_aBackRC[1][i] = m_aRgn[i].rcOut;
			}
		}
		break;

	}
	if( m_IsMore )
	{
		for(int i=0;i<n;i++)
		{
			if (m_SwitchStyle == SwitchStyle_Vdefault || m_SwitchStyle == SwitchStyle_Vswitchbutton || m_SwitchStyle == SwitchStyle_Vsamebutton || m_SwitchStyle == SwitchStyle_Vcustom1)
			{
				m_aRgn[i].rcOut.OffsetRect(0,2*TIP_W);
				m_aRgn[i].rcIn.OffsetRect(0,2*TIP_W);

			}
			else
			{
				m_aRgn[i].rcOut.OffsetRect(2*TIP_W,0);
				m_aRgn[i].rcIn.OffsetRect(2*TIP_W,0);

			}
			m_aBackRC[0][i] = m_aRgn[i].rcIn;
			m_aBackRC[1][i] = m_aRgn[i].rcOut;
		}
	}
	if (m_SwitchStyle == SwitchStyle_Vdefault || m_SwitchStyle == SwitchStyle_Vswitchbutton || m_SwitchStyle == SwitchStyle_Vsamebutton || m_SwitchStyle == SwitchStyle_Vcustom1)
	{
		if (e == top + dx)
		{
			return 0;
		}
		return e - top;
	}
	if(e==left+dx)
		return 0;	//没有标签

	return e-left;
}

void CNewZfjsTab::PaintButton()
{
	CDC *pDC = GetDC();
	CDC MyDC;	
	CBitmap MyBitmap; 
	MyDC.CreateCompatibleDC(pDC); 
	MyBitmap.CreateCompatibleBitmap(pDC,Width(),Height()); 

	CBitmap *pOldBitmap = MyDC.SelectObject(&MyBitmap);
	int		mode = MyDC.SetBkMode(TRANSPARENT);
	CFont*	oldF = MyDC.SelectObject(&m_Font);
	////////////////////////////////
	CRect	rects;
	rects = m_rcWnd;
	rects.OffsetRect(-left,-top);
	COLORREF crBk = m_crBk,crUMk = m_crUMk;
	if(m_SwitchStyle==SwitchStyle_switchbutton)
	{
		crBk = m_crBtnSelBk;
		crUMk = m_crBtnSelBk;
	}
//	MyDC.FillSolidRect(&rects,crBk);
	g_d.Draw3DBar3(&MyDC, rects, crBk, FALSE);
	for ( int i=0;i<m_aRgn.size();++i )
	{
		rects = m_aRgn[i].rcOut;
		rects.OffsetRect(-left,-top);
		DrawLine(&MyDC,rects.right-1,rects.top,rects.right-1,rects.bottom,m_crLine);
		rects=m_aRgn[i].rcIn;
		rects.OffsetRect(-left,-top);
		////////////////////////////
		COLORREF crTxt;
		if ( m_nHotNo>=0 && i==m_nHotNo )
			crTxt=m_crHTxt;
		else
			crTxt=m_vTextColor[i].cor;
		MyDC.SetTextColor(crTxt);
	//	MyDC.FillRect(&rects,&CBrush(crUMk));
		g_d.Draw3DBar3(&MyDC, rects, RGB(64,0,0), FALSE);
		//////////////////////////////
		rects.left += 1;
		if(m_SwitchStyle!=SwitchStyle_switchbutton || rects.right<Width()-3)
			MyDC.DrawText(m_aRgn[i].caption,&(rects),DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	}
	if ( m_nCurNo>=0 )
	{
		rects = m_aRgn[m_nCurNo].rcIn;
		rects.OffsetRect(-left,-top);
		MyDC.SetTextColor(m_crMTxt);
		CRect tmprc=rects;
		MyDC.FillRect(tmprc,&m_brBk);
		tmprc.DeflateRect(1,0,1,0);
	//	MyDC.FillRect(tmprc,&CBrush(m_crBtnSelBk));
		g_d.Draw3DBar3(&MyDC, tmprc, RGB(192,0,0), FALSE);
		rects.left+= 1;
		if(m_SwitchStyle!=SwitchStyle_switchbutton || rects.right<Width()-3)
			MyDC.DrawText(m_aRgn[m_nCurNo].caption,&(rects),DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	}
	/////////////////////////
	pDC->BitBlt (left,top,Width(),Height(),&MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
	//////////////////////////
	MyDC.SelectObject(oldF);
	MyDC.SetBkMode(mode);
	MyDC.SelectObject(pOldBitmap); 
	MyBitmap.DeleteObject(); 
	MyDC.DeleteDC(); 
	ReleaseDC(pDC);
}

void CNewZfjsTab::PaintButtonV()
{
	CDC *pDC = GetDC();
	CDC MyDC;	
	CBitmap MyBitmap; 
	MyDC.CreateCompatibleDC(pDC); 
	MyBitmap.CreateCompatibleBitmap(pDC,Width(),Height()); 
	
	CBitmap *pOldBitmap = MyDC.SelectObject(&MyBitmap);
	int		mode = MyDC.SetBkMode(TRANSPARENT);
	CFont*	oldF = MyDC.SelectObject(&m_Font);
	////////////////////////////////
	CRect	rects;
	rects = m_rcWnd;
	rects.OffsetRect(-left,-top);
	COLORREF crBk = m_crBk,crUMk = m_crUMk;
	if(m_SwitchStyle==SwitchStyle_Vswitchbutton)
	{
		crBk = m_crBtnSelBk;
		crUMk = m_crBtnSelBk;
	}
	MyDC.FillSolidRect(&rects,crBk);
	MyDC.FrameRect(rects,&m_brLine);

	for ( int i=0;i<m_aRgn.size();++i )
	{
		rects = m_aRgn[i].rcOut;
		rects.OffsetRect(-left,-top);
		DrawLine(&MyDC,rects.left,rects.bottom - 1,rects.right,rects.bottom - 1,m_crLine,PS_SOLID,1);
		rects=m_aRgn[i].rcIn;
		rects.OffsetRect(-left,-top);
		////////////////////////////
		COLORREF crTxt;
		if ( m_nHotNo>=0 && i==m_nHotNo )
			crTxt=m_crHTxt;
		else
			crTxt=m_vTextColor[i].cor;
		MyDC.SetTextColor(crTxt);
		MyDC.FillRect(&rects,&CBrush(crUMk));
		//////////////////////////////
		rects.top += 1;
		if(m_SwitchStyle!=SwitchStyle_Vswitchbutton || rects.bottom<Height()-3)
		{
			CString str="";
			UINT nCount = m_aRgn[i].caption.GetLength();
			for (int nI = 0; nI < m_aRgn[i].caption.GetLength() - 3; nI+=1)
			{
				CString strTemp = m_aRgn[i].caption.Mid(2*nI,2);
				if (strTemp == "")
					break;
				str += strTemp + "\n\t";
			}

			MyDC.DrawText(str,&(rects),DT_CENTER);
		}
	}

	if ( m_nCurNo>=0 )
	{
		rects = m_aRgn[m_nCurNo].rcIn;
		rects.OffsetRect(-left,-top);
		MyDC.SetTextColor(m_crMTxt);
		CRect tmprc=rects;
		MyDC.FillRect(tmprc,&m_brBk);
		tmprc.DeflateRect(0,1,0,1);
		MyDC.FillRect(tmprc,&CBrush(m_crBtnSelBk));
		rects.top+= 1;
		if(m_SwitchStyle!=SwitchStyle_Vswitchbutton || rects.bottom<Height()-3)
		{
			CString str="";
			UINT nCount = m_aRgn[m_nCurNo].caption.GetLength();
			for (int nI = 0; nI < m_aRgn[m_nCurNo].caption.GetLength() - 3; nI+=1)
			{
				CString strTemp = m_aRgn[m_nCurNo].caption.Mid(2*nI,2);
				if (strTemp == "")
					break;
				str += strTemp + "\n\t";
			}

			MyDC.DrawText(str,&(rects),DT_CENTER);
		}
	}
	/////////////////////////
	pDC->BitBlt (left,top,Width(),Height(),&MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
	//////////////////////////
	MyDC.SelectObject(oldF);
	MyDC.SetBkMode(mode);
	MyDC.SelectObject(pOldBitmap); 
	MyBitmap.DeleteObject(); 
	MyDC.DeleteDC(); 
	ReleaseDC(pDC);
}

void CNewZfjsTab::PaintSwitchV()
{
	CDC *pDC = GetDC();
	CDC MyDC;
	CBitmap MyBitmap;
	MyDC.CreateCompatibleDC(pDC);
	MyBitmap.CreateCompatibleBitmap(pDC,Width(),Height());
	
	CBitmap *pOldBitmap = MyDC.SelectObject(&MyBitmap);
	int		mode = MyDC.SetBkMode(TRANSPARENT);
	CFont * oldF = MyDC.SelectObject(&m_Font);

	CPoint	aPoint[4];						//存放按键外框的四个顶点
	ZeroMemory(&aPoint,4 * sizeof(CPoint));
	CRect	rects;
	CPen	Pen;
	Pen.CreatePen(PS_SOLID,1,m_crMk);
	CPen  *	oldP = MyDC.SelectObject(&Pen);	
	////////////////////////////////////
	rects = m_rcWnd;
	rects.OffsetRect(-left,-top);
	MyDC.FillSolidRect(&rects,m_crBk);
	//////////////////////////////
	for ( int i=0;i<m_aRgn.size();++i )
	{
		if(m_aRgn[i].rcIn.right > right) break;
		CRgn	Rgn;
		aPoint[0] = m_aRgn[i].rcOut.TopLeft();
		aPoint[1] = m_aRgn[i].rcIn.TopLeft() + CPoint(0,m_aRgn[i].rcIn.Height());
		aPoint[2] = m_aRgn[i].rcOut.BottomRight();
		aPoint[3] = m_aRgn[i].rcOut.BottomRight() - CPoint(0,m_aRgn[i].rcOut.Height());
		Rgn.CreatePolygonRgn(aPoint,4,ALTERNATE);
		Rgn.OffsetRgn(-left,-top);	//平移
		CRgn sRgn;					//画圆角
		aPoint[0].Offset(-left,-top);
		sRgn.CreateRectRgn(aPoint[0].x,aPoint[0].y,aPoint[0].x+1,aPoint[0].y+1);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		sRgn.OffsetRgn(0,1);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		sRgn.OffsetRgn(1,-1);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		////////////////////////////
		COLORREF crTxt;
		if ( m_nHotNo>=0 && i==m_nHotNo )
			crTxt=m_crHTxt;
		else
			crTxt=m_vTextColor[i].cor;
		MyDC.SetTextColor(crTxt);
		MyDC.FillRgn(&Rgn,&m_brUMk);
		//////////////////////////////
		MyDC.FrameRgn(&Rgn,&m_brLine,1,1);
		/////////////////////////////
		rects = m_aRgn[i].rcIn;
		rects.right+=1;
		rects.OffsetRect(-left,-top);	//平移
		CString str="";
		UINT nCount = m_aRgn[i].caption.GetLength();
		for (int nI = 0; nI < m_aRgn[i].caption.GetLength() - 3; nI+=1)
		{
			CString strTemp = m_aRgn[i].caption.Mid(2*nI,2);
			if (strTemp == "")
				break;
			str += strTemp + "\n\t";
		}
		
		MyDC.DrawText(str,&(rects),DT_CENTER);
	}
	if ( m_nCurNo>=0 )
	{
		CRgn	Rgn;
		aPoint[0] = m_aRgn[m_nCurNo].rcOut.TopLeft();
		aPoint[1] = m_aRgn[m_nCurNo].rcIn.TopLeft() + CPoint(0,m_aRgn[m_nCurNo].rcIn.Height());
		aPoint[2] = m_aRgn[m_nCurNo].rcOut.BottomRight();
		aPoint[3] = m_aRgn[m_nCurNo].rcOut.BottomRight() - CPoint(0,m_aRgn[m_nCurNo].rcOut.Height());
		Rgn.CreatePolygonRgn(aPoint,4,ALTERNATE);
		Rgn.OffsetRgn(-left,-top);	//平移
		CRgn sRgn;
		aPoint[0].Offset(-left,-top);
		sRgn.CreateRectRgn(aPoint[0].x,aPoint[0].y,aPoint[0].x+1,aPoint[0].y+1);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		sRgn.OffsetRgn(0,1);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		sRgn.OffsetRgn(1,-1);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		//////////////////////////////////
		CBrush *pbrBK;
		if ( m_nHotNo == m_nCurNo )
			pbrBK=&m_brHot;
		else
			pbrBK=&m_brMk;
		MyDC.SetTextColor(m_crMTxt);
		MyDC.FillRgn(&Rgn,pbrBK);
		MyDC.FrameRgn(&Rgn,&m_brLine,1,1);
		//////////////用背景笔擦去最下面线。
		rects = m_aRgn[m_nCurNo].rcOut;
		rects.OffsetRect(-left,-top);	//平移
		MyDC.MoveTo(rects.right - 1,rects.top);
		MyDC.LineTo(rects.right - 1,rects.bottom);
		/////////////////////////////
		rects = m_aRgn[m_nCurNo].rcIn;	//点击按键式的字体平移效果
		rects.left+= 1;
		rects.bottom+=1;
		rects.OffsetRect(-left,-top);	//平移
		CString str="";
		UINT nCount = m_aRgn[m_nCurNo].caption.GetLength();
		for (int nI = 0; nI < m_aRgn[m_nCurNo].caption.GetLength() - 3; nI+=1)
		{
			CString strTemp = m_aRgn[m_nCurNo].caption.Mid(2*nI,2);
			if (strTemp == "")
				break;
			str += strTemp + "\n\t";
		}		
		MyDC.DrawText(str,&(rects),DT_CENTER);
	}
	if ( m_IsMore )
	{
		int	  nSelect = -1;
		rects = m_aTipRct[0];
		rects.bottom = m_aTipRct[1].bottom;
		rects.OffsetRect(-left,-top);
		MyDC.FillSolidRect(&rects,m_crBk);
		MyDC.FrameRect(&rects,&m_brTipLine);
		for ( i=0;i<2;++i )
		{
			m_aTipRgn[i].OffsetRgn(-left,-top);	//平移
			MyDC.FillRgn(&m_aTipRgn[i],&m_brTipLine);
			m_aTipRgn[i].OffsetRgn(left,top);	//平移
		}
	}
	/////////////////////////

	UINT nI = m_aRgn.size();
	if (m_aRgn.size() > 0 && m_aRgn[0].rcOut.Height() > 0)
	{
		UINT nHeight = Height();
		UINT nUnitHeight = m_aRgn[0].rcOut.Height();
		UINT nMidNo = nHeight / nUnitHeight;
		if (nMidNo >= m_aRgn.size() - m_nLeftNo)
		{
			CRect rect = m_aRgn[m_aRgn.size() - 1].rcOut;
			UINT nnn = Height() - rect.bottom;
			DrawLine(&MyDC,right - 1,rect.bottom,right - 1,bottom,m_crLine,PS_SOLID,1);
			
		}
	}
	DrawLine(&MyDC,left,bottom - 1,right,bottom - 1,m_crLine,PS_SOLID,1);

	pDC->BitBlt (left,top,Width(),Height(),&MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
	MyDC.SelectObject(oldP);
	Pen.DeleteObject();
	MyDC.SelectObject(oldF);
	MyDC.SetBkMode(mode);
	MyDC.SelectObject(pOldBitmap);
	MyBitmap.DeleteObject();
	MyDC.DeleteDC();
	ReleaseDC(pDC);
}

void CNewZfjsTab::PaintSpecialSwitch()
{
	CDC *pDC = GetDC();
	CDC MyDC;
	CBitmap MyBitmap;
	MyDC.CreateCompatibleDC(pDC);
	MyBitmap.CreateCompatibleBitmap(pDC,Width(),Height());
	
	CBitmap *pOldBitmap = MyDC.SelectObject(&MyBitmap);
	int		mode = MyDC.SetBkMode(TRANSPARENT);
	CFont * oldF = MyDC.SelectObject(&m_Font);

	CPoint	aPoint[4];
	CRect	rects;
	CPen	Pen;
	Pen.CreatePen(PS_SOLID,1,m_crMk);
	CPen  *	oldP = MyDC.SelectObject(&Pen);	
	////////////////////////////////////
	rects = m_rcWnd;
	rects.OffsetRect(-left,-top);
	MyDC.FillSolidRect(&rects,m_crBk);
	//////////////////////////////
	for ( int i=0;i<m_aRgn.size();++i )
	{
		if(m_aRgn[i].rcIn.right > right) break;
		CRgn	Rgn;
		aPoint[0] = m_aRgn[i].rcOut.TopLeft();
		aPoint[1] = m_aRgn[i].rcIn.TopLeft() + CPoint(m_aRgn[i].rcIn.Width(),0);
		aPoint[2] = m_aRgn[i].rcOut.BottomRight();
		aPoint[3] = m_aRgn[i].rcOut.BottomRight() - CPoint(m_aRgn[i].rcOut.Width(),0);
		Rgn.CreatePolygonRgn(aPoint,4,ALTERNATE);
		Rgn.OffsetRgn(-left,-top);	//平移
		CRgn sRgn;
		aPoint[0].Offset(-left,-top);
		sRgn.CreateRectRgn(aPoint[0].x,aPoint[0].y,aPoint[0].x+1,aPoint[0].y+1);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		sRgn.OffsetRgn(1,0);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		sRgn.OffsetRgn(-1,1);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		////////////////////////////
		COLORREF crTxt;
		if ( m_nHotNo>=0 && i==m_nHotNo )
			crTxt=m_crHTxt;
		else
			crTxt=m_vTextColor[i].cor;
		MyDC.SetTextColor(crTxt);
		MyDC.FillRgn(&Rgn,&m_brUMk);
		//////////////////////////////
		MyDC.FrameRgn(&Rgn,&m_brLine,1,1);
		/////////////////////////////
		rects = m_aRgn[i].rcIn;
		rects.bottom+=1;
		rects.OffsetRect(-left,-top);	//平移
		MyDC.DrawText(m_aRgn[i].caption,&(rects),DT_SINGLELINE|DT_VCENTER|DT_LEFT);
	}
	if ( m_nCurNo>=0 )
	{
		CRgn	Rgn;
		aPoint[0] = m_aRgn[m_nCurNo].rcOut.TopLeft();
		aPoint[1] = m_aRgn[m_nCurNo].rcIn.TopLeft() + CPoint(m_aRgn[m_nCurNo].rcIn.Width(),0);
		aPoint[2] = m_aRgn[m_nCurNo].rcOut.BottomRight();
		aPoint[3] = m_aRgn[m_nCurNo].rcOut.BottomRight() - CPoint(m_aRgn[m_nCurNo].rcOut.Width(),0);
		Rgn.CreatePolygonRgn(aPoint,4,ALTERNATE);
		Rgn.OffsetRgn(-left,-top);	//平移
		CRgn sRgn;
		aPoint[0].Offset(-left,-top);
		sRgn.CreateRectRgn(aPoint[0].x,aPoint[0].y,aPoint[0].x+1,aPoint[0].y+1);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		sRgn.OffsetRgn(1,0);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		sRgn.OffsetRgn(-1,1);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		//////////////////////////////////
		CBrush *pbrBK;
		if ( m_nHotNo == m_nCurNo )
			pbrBK=&m_brHot;
		else
			pbrBK=&m_brMk;
		MyDC.SetTextColor(m_crMTxt);
		MyDC.FillRgn(&Rgn,pbrBK);
		MyDC.FrameRgn(&Rgn,&m_brLine,1,1);
		//////////////用背景笔擦去最下面线。
		rects = m_aRgn[m_nCurNo].rcOut;
		rects.OffsetRect(-left,-top);	//平移
		MyDC.MoveTo(rects.left,rects.bottom-1);
		MyDC.LineTo(rects.right,rects.bottom-1);
		/////////////////////////////
		rects = m_aRgn[m_nCurNo].rcIn;
		rects.left+= 1;
		rects.bottom+=1;
		rects.OffsetRect(-left,-top);	//平移
		MyDC.DrawText(m_aRgn[m_nCurNo].caption,&(rects),DT_SINGLELINE|DT_VCENTER|DT_LEFT);
	}
	if ( m_IsMore )
	{
		int	  nSelect = -1;
		rects = m_aTipRct[0];
		rects.right = m_aTipRct[1].right;
		rects.OffsetRect(-left,-top);
		MyDC.FillSolidRect(&rects,m_crBk);
		MyDC.FrameRect(&rects,&m_brTipLine);
		for ( i=0;i<2;++i )
		{
			m_aTipRgn[i].OffsetRgn(-left,-top);	//平移
			MyDC.FillRgn(&m_aTipRgn[i],&m_brTipLine);
			m_aTipRgn[i].OffsetRgn(left,top);	//平移
		}
	}
	/////////////////////////
	pDC->BitBlt (left,top,Width(),Height(),&MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
	MyDC.SelectObject(oldP);
	Pen.DeleteObject();
	MyDC.SelectObject(oldF);
	MyDC.SetBkMode(mode);
	MyDC.SelectObject(pOldBitmap);
	MyBitmap.DeleteObject();
	MyDC.DeleteDC();
	ReleaseDC(pDC);
}

void CNewZfjsTab::PaintSpecialSwitchV()
{
	CDC *pDC = GetDC();
	CDC MyDC;
	CBitmap MyBitmap;
	MyDC.CreateCompatibleDC(pDC);
	MyBitmap.CreateCompatibleBitmap(pDC,Width(),Height());
	
	CBitmap *pOldBitmap = MyDC.SelectObject(&MyBitmap);
	int		mode = MyDC.SetBkMode(TRANSPARENT);
	CFont * oldF = MyDC.SelectObject(&m_Font);

	CPoint	aPoint[4];
	CRect	rects;
	CPen	Pen;
	Pen.CreatePen(PS_SOLID,1,m_crMk);
	CPen  *	oldP = MyDC.SelectObject(&Pen);	
	////////////////////////////////////
	rects = m_rcWnd;
	rects.OffsetRect(-left,-top);
	MyDC.FillSolidRect(&rects,m_crBk);
	//////////////////////////////
	for ( int i=0;i<m_aRgn.size();++i )
	{
		if(m_aRgn[i].rcIn.right > right) break;
		CRgn	Rgn;
		aPoint[0] = m_aRgn[i].rcOut.TopLeft();
		aPoint[1] = m_aRgn[i].rcIn.TopLeft() + CPoint(0,m_aRgn[i].rcIn.Height());
		aPoint[2] = m_aRgn[i].rcOut.BottomRight();
		aPoint[3] = m_aRgn[i].rcOut.BottomRight() - CPoint(0,m_aRgn[i].rcOut.Height());

		Rgn.CreatePolygonRgn(aPoint,4,ALTERNATE);
		Rgn.OffsetRgn(-left,-top);	//平移
		CRgn sRgn;
		aPoint[0].Offset(-left,-top);
		sRgn.CreateRectRgn(aPoint[0].x,aPoint[0].y,aPoint[0].x+1,aPoint[0].y+1);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		sRgn.OffsetRgn(0,1);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		sRgn.OffsetRgn(1,-1);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		////////////////////////////
		COLORREF crTxt;
		if ( m_nHotNo>=0 && i==m_nHotNo )
			crTxt=m_crHTxt;
		else
			crTxt=m_vTextColor[i].cor;
		MyDC.SetTextColor(crTxt);
		MyDC.FillRgn(&Rgn,&m_brUMk);
		//////////////////////////////
		MyDC.FrameRgn(&Rgn,&m_brLine,1,1);
		/////////////////////////////
		rects = m_aRgn[i].rcIn;
		rects.right+=1;
		rects.OffsetRect(-left,-top);	//平移
		CString str="";
		UINT nCount = m_aRgn[i].caption.GetLength();
		for (int nI = 0; nI < m_aRgn[i].caption.GetLength() - 3; nI+=1)
		{
			CString strTemp = m_aRgn[i].caption.Mid(2*nI,2);
			if (strTemp == "")
				break;
			str += strTemp + "\n\t";
		}

		MyDC.DrawText(str,&(rects),DT_CENTER);
	}
	if ( m_nCurNo>=0 )
	{
		CRgn	Rgn;
		aPoint[0] = m_aRgn[m_nCurNo].rcOut.TopLeft();
		aPoint[1] = m_aRgn[m_nCurNo].rcIn.TopLeft() + CPoint(0,m_aRgn[m_nCurNo].rcIn.Height());
		aPoint[2] = m_aRgn[m_nCurNo].rcOut.BottomRight();
		aPoint[3] = m_aRgn[m_nCurNo].rcOut.BottomRight() - CPoint(0,m_aRgn[m_nCurNo].rcOut.Height());

		Rgn.CreatePolygonRgn(aPoint,4,ALTERNATE);
		Rgn.OffsetRgn(-left,-top);	//平移
		CRgn sRgn;
		aPoint[0].Offset(-left,-top);
		sRgn.CreateRectRgn(aPoint[0].x,aPoint[0].y,aPoint[0].x+1,aPoint[0].y+1);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		sRgn.OffsetRgn(0,1);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		sRgn.OffsetRgn(1,-1);
		Rgn.CombineRgn(&Rgn,&sRgn,RGN_XOR);
		//////////////////////////////////
		CBrush *pbrBK;
		if ( m_nHotNo == m_nCurNo )
			pbrBK=&m_brHot;
		else
			pbrBK=&m_brMk;
		MyDC.SetTextColor(m_crMTxt);
		MyDC.FillRgn(&Rgn,pbrBK);
		MyDC.FrameRgn(&Rgn,&m_brLine,1,1);
		//////////////用背景笔擦去最下面线。
		rects = m_aRgn[m_nCurNo].rcOut;
		rects.OffsetRect(-left,-top);	//平移
		MyDC.MoveTo(rects.right - 1,rects.top);
		MyDC.LineTo(rects.right - 1,rects.bottom);
		/////////////////////////////
		rects = m_aRgn[m_nCurNo].rcIn;
		rects.top+= 1;
		rects.right+=1;
		rects.OffsetRect(-left,-top);	//平移
		CString str="";
		UINT nCount = m_aRgn[m_nCurNo].caption.GetLength();
		for (int nI = 0; nI < m_aRgn[m_nCurNo].caption.GetLength() - 3; nI+=1)
		{
			CString strTemp = m_aRgn[m_nCurNo].caption.Mid(2*nI,2);
			if (strTemp == "")
				break;
			str += strTemp + "\n\t";
		}
		MyDC.DrawText(str,&(rects),DT_CENTER);
	}
	if ( m_IsMore )
	{
		int	  nSelect = -1;
		rects = m_aTipRct[0];
		rects.bottom = m_aTipRct[1].bottom;
		rects.OffsetRect(-left,-top);
		MyDC.FillSolidRect(&rects,m_crBk);
		MyDC.FrameRect(&rects,&m_brTipLine);
		for ( i=0;i<2;++i )
		{
			m_aTipRgn[i].OffsetRgn(-left,-top);	//平移
			MyDC.FillRgn(&m_aTipRgn[i],&m_brTipLine);
			m_aTipRgn[i].OffsetRgn(left,top);	//平移
		}
	}
	/////////////////////////
	pDC->BitBlt (left,top,Width(),Height(),&MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
	MyDC.SelectObject(oldP);
	Pen.DeleteObject();
	MyDC.SelectObject(oldF);
	MyDC.SetBkMode(mode);
	MyDC.SelectObject(pOldBitmap);
	MyBitmap.DeleteObject();
	MyDC.DeleteDC();
	ReleaseDC(pDC);
}

void CNewZfjsTab::PaintSwitch()
{
	CDC *pDC = GetDC();
	CDC MyDC;	
	CBitmap MyBitmap; 
	MyDC.CreateCompatibleDC(pDC); 
	MyBitmap.CreateCompatibleBitmap(pDC,Width(),Height()); 

	CBitmap *pOldBitmap = MyDC.SelectObject(&MyBitmap);
	int		mode = MyDC.SetBkMode(TRANSPARENT);
	CFont *	oldF = MyDC.SelectObject(&m_Font);
	
	CPoint	aPoint[4];
	CRect	rects;
	CPen	Pen;
	Pen.CreatePen(PS_SOLID,1,m_crBk);
	CPen *	oldP = MyDC.SelectObject(&Pen);
	////////////////////////////////////
	rects = m_rcWnd;
	rects.OffsetRect(-left,-top);
	MyDC.FillSolidRect(&rects,m_crBk);
	//////////////////////////////
	COLORREF crTxt;
	for ( int i=0;i<m_aRgn.size();++i )
	{
		if(m_aRgn[i].rcIn.right > right) break;
		CRgn	Rgn;
		aPoint[0] = m_aRgn[i].rcOut.TopLeft();
		aPoint[1] = m_aRgn[i].rcOut.TopLeft() + CPoint(m_aRgn[i].rcOut.Width(),0);
		aPoint[2] = m_aRgn[i].rcIn.BottomRight();
		aPoint[3] = m_aRgn[i].rcIn.BottomRight() - CPoint(m_aRgn[i].rcIn.Width(),0);
		Rgn.CreatePolygonRgn(aPoint,4,ALTERNATE);
		Rgn.OffsetRgn(-left,-top);	//平移
		////////////////////////////
		if ( m_nHotNo>=0 && i==m_nHotNo )
			crTxt=m_crHTxt;
		else
			crTxt=m_vTextColor[i].cor;
		MyDC.SetTextColor(crTxt);
		MyDC.FillRgn(&Rgn,&m_brUMk);
		//////////////////////////////
		MyDC.FrameRgn(&Rgn,&m_brLine,1,1);
		/////////////////////////////
		rects = m_aRgn[i].rcIn;
		rects.left+= 1;
		rects.OffsetRect(-left,-top);	//平移
		MyDC.DrawText(m_aRgn[i].caption,&(rects),DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	}
	if ( m_nCurNo>=0 )
	{
		CRgn	Rgn;
		aPoint[0] = m_aRgn[m_nCurNo].rcOut.TopLeft();
		aPoint[1] = m_aRgn[m_nCurNo].rcOut.TopLeft() + CPoint(m_aRgn[m_nCurNo].rcOut.Width(),0);
		aPoint[2] = m_aRgn[m_nCurNo].rcIn.BottomRight();
		aPoint[3] = m_aRgn[m_nCurNo].rcIn.BottomRight() - CPoint(m_aRgn[m_nCurNo].rcIn.Width(),0);
		Rgn.CreatePolygonRgn(aPoint,4,ALTERNATE);
		Rgn.OffsetRgn(-left,-top);	//平移
		rects = m_aRgn[m_nCurNo].rcOut;
		rects.OffsetRect(-left,-top);
		//////////////////////////////////
		CBrush *pbrbk;
		if ( m_nHotNo == m_nCurNo )
			pbrbk=&m_brHot;
		else
			pbrbk=&m_brMk;
		MyDC.SetTextColor(m_crMTxt);
		MyDC.FillRgn(&Rgn,pbrbk);
		MyDC.FrameRgn(&Rgn,&m_brLine,1,1);
		//////////////兼容老风格,用背景笔擦去最上面线。
		rects = m_aRgn[m_nCurNo].rcOut;
		rects.OffsetRect(-left,-top);	//平移
		MyDC.MoveTo(rects.left,rects.top);
		MyDC.LineTo(rects.right,rects.top);
		/////////////////////////////
		rects = m_aRgn[m_nCurNo].rcIn;
		rects.left+= 1;
		rects.OffsetRect(-left,-top);	//平移
		MyDC.DrawText(m_aRgn[m_nCurNo].caption,&(rects),DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	}
	if ( m_IsMore )
	{
		int	  nSelect = -1;
		rects = m_aTipRct[0];
		rects.right = m_aTipRct[1].right;
		rects.OffsetRect(-left,-top);
		MyDC.FillSolidRect(&rects,m_crBk);
		MyDC.FrameRect(&rects,&m_brTipLine);
		for ( i=0;i<2;++i )
		{
			m_aTipRgn[i].OffsetRgn(-left,-top);	//平移
			MyDC.FillRgn(&m_aTipRgn[i],&m_brTipLine);
			m_aTipRgn[i].OffsetRgn(left,top);	//平移
		}
	}
	/////////////////////////
	pDC->BitBlt (left,top,Width(),Height(),&MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
	MyDC.SelectObject(oldP);
	Pen.DeleteObject();
	MyDC.SelectObject(oldF);
	MyDC.SetBkMode(mode);
	MyDC.SelectObject(pOldBitmap);
	MyBitmap.DeleteObject();
	MyDC.DeleteDC();
	ReleaseDC(pDC);
}

BOOL CNewZfjsTab::PtInRect(POINT point)
{
	CRect	rc(left,top,right,bottom);
	return rc.PtInRect(point);
}

BOOL CNewZfjsTab::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CNewZfjsTab::DrawSwitch()
{
	if(Width()<=0 || Height()<=0 ) 
		return;
	if ( m_aRgn.empty() ) 
		return;
	switch(m_SwitchStyle)  
	{
	case SwitchStyle_custom1:
		PaintSpecialSwitch();
		break;
	case SwitchStyle_default:
		PaintSwitch();
	    break;
	case SwitchStyle_samebutton:
	case SwitchStyle_switchbutton:
	case SwitchStyle_switchbutton2:
		PaintButton();
		break;
	case SwitchStyle_Vdefault:
		PaintSwitchV();
	    break;
	case SwitchStyle_Vswitchbutton:
	case SwitchStyle_Vsamebutton:
		PaintButtonV();
		break;
	case SwitchStyle_Vcustom1:
		PaintSpecialSwitchV();
		break;
	}
}

void CNewZfjsTab::OnPaint()
{
	DrawSwitch();
	CWnd::OnPaint();
}

void CNewZfjsTab::SendOutMsg()
{
	if ( m_aRgn[m_nCurNo].isMenu )
	{
		int OldOld = m_nCurNo; 
		if(!m_pSwitchMenuCallBack)
			return;
		//调用回调函数的处理
		CWnd *ProcWnd = m_pMainWnd;
		if(!ProcWnd)
			ProcWnd = AfxGetMainWnd();
		m_pSwitchMenuCallBack(this,ProcWnd,m_aRgn[m_nCurNo].rcOut.left,m_aRgn[m_nCurNo].rcOut.top,m_aRgn[m_nCurNo].rcOut.right,m_aRgn[m_nCurNo].rcOut.bottom,m_aRgn[m_nCurNo].message,m_aRgn[m_nCurNo].menuid,m_aRgn[m_nCurNo].whichmenu);
		m_nCurNo = OldOld;
	}
	else if ( m_aRgn[m_nCurNo].message>0 )
		::PostMessage(m_MsghWnd,m_aRgn[m_nCurNo].message,m_aRgn[m_nCurNo].wparam,m_aRgn[m_nCurNo].lparam);
}

RgnInfoEx & CNewZfjsTab::aRgnAt(int index)
{
	return m_aRgn[index];
}

void CNewZfjsTab::InsertSwitchLabel(int index,const char *caption,const char *tooltip,UINT msg_menu,WPARAM wparam_menuid,LPARAM lparam_menuno,BOOL isMenu,long everclick)
{
	InsertSwitchLabel(index,caption,msg_menu,wparam_menuid,lparam_menuno,isMenu,everclick);
	m_aRgn[index].tooltip=tooltip;
}

void CNewZfjsTab::InsertSwitchLabel(int index,const char *caption,UINT msg_menu,WPARAM wparam_menuid,LPARAM lparam_menuno,BOOL isMenu,long everclick)
{
	RgnInfoEx tmp;
	m_aRgn.insert(&m_aRgn[0]+index,tmp);
	////////////////////////////////////////////////
	m_aRgn[index].caption	= caption;
	if ( isMenu )
	{
		m_aRgn[index].isMenu	= 1;
		m_aRgn[index].message   = msg_menu;
		m_aRgn[index].menuid	= wparam_menuid;
		m_aRgn[index].whichmenu	= lparam_menuno;
	}
	else
	{
		m_aRgn[index].message	= msg_menu;
		m_aRgn[index].wparam	= wparam_menuid;
		m_aRgn[index].lparam	= lparam_menuno;
	}
	m_aRgn[index].everclick = everclick;
	m_aRgn[index].tooltip="";
	SpecialTextColor sc;
	sc.cor=m_crTxt;
	m_vTextColor.insert(&m_vTextColor[0]+index,sc);
}

void CNewZfjsTab::SetSpecialTextColor(UINT nIndex,COLORREF cor)
{
	if(nIndex+1>m_vTextColor.size()) return;
	m_vTextColor[nIndex].cor=cor;
	m_vTextColor[nIndex].bSet=TRUE;
}

void CNewZfjsTab::SetMenuCallBack(PMENUCALLBACK pFunc)
{
	m_pSwitchMenuCallBack = pFunc;
}

void CNewZfjsTab::ModifySwitchLabel(int iWhich,const char *caption,const char * tooltip)
{
	if(iWhich<0 || iWhich>=m_aRgn.size()) return;
	m_aRgn[iWhich].caption	= caption;
	m_aRgn[iWhich].tooltip	= tooltip;
}

void CNewZfjsTab::SetCurLabel(int nCurNo)
{
	if(nCurNo==-999)
	{
		m_nCurNo = -999;
		DrawSwitch();
		return;
	}
	if(nCurNo>=0) m_nCurNo = nCurNo;
	m_nCurNo = min(m_nCurNo,m_aRgn.size()-1);
	m_nCurNo = max(m_nCurNo, 0);	
	DrawSwitch();
}

void CNewZfjsTab::SetStyle(SwitchStyle ss)
{
	m_SwitchStyle=ss;
}

void CNewZfjsTab::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==1)
	{
		CPoint pt;
		CRect rect;
		GetCursorPos(&pt);
		GetWindowRect(&rect);
		if (!::PtInRect(&rect,pt))
		{
			KillTimer(1);
			PostMessage(WM_MOUSELEAVE);
		}
	}
	
	CWnd::OnTimer(nIDEvent);
}

void CNewZfjsTab::OnMouseLeave(WPARAM wParam,LPARAM lParam)
{
	m_nHotNo = -1;
	bFirst	= TRUE;
	DrawSwitch();
}

int CNewZfjsTab::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);
	m_ToolTip.AddTool(this,"");
	m_ToolTip.SetMaxTipWidth(500);	
	m_ToolTip.SetDelayTime(500);
	return 0;
}

BOOL CNewZfjsTab::PreTranslateMessage(MSG* pMsg)
{
	if(m_ToolTip.m_hWnd!=NULL)  
		m_ToolTip.RelayEvent(pMsg);
	return CWnd::PreTranslateMessage(pMsg);
}

CString	CNewZfjsTab::GetCaption(int nNo) 
{
	if(nNo>=0 && nNo<m_aRgn.size()) 
		return m_aRgn[nNo].caption;
	return "";
};

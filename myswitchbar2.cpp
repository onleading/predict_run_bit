// myswitchbar.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "myswitchbar2.h"
#include "MyInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMySwitchBar2

CMySwitchBar2::CMySwitchBar2()
{
	m_nLabelNum = 0;
	m_nCurLabel = 0;

	m_MsghWnd = NULL;
	m_SelChangeMsg = 0;
	m_nMinWidth = 10;
	m_nMaxWidth = 100;
}

CMySwitchBar2::~CMySwitchBar2()
{
}


BEGIN_MESSAGE_MAP(CMySwitchBar2, CWnd)
	//{{AFX_MSG_MAP(CMySwitchBar2)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMySwitchBar2 message handlers

void CMySwitchBar2::SetMinTabWidth(int minWidth)
{
	m_nMinWidth = minWidth;
}

void CMySwitchBar2::Init(HWND hwnd,UINT SelChangeMsg)
{
	m_MsghWnd		= hwnd; 
	m_SelChangeMsg  = SelChangeMsg;
}

CImageList* CMySwitchBar2::SetImageList (CImageList *pImageList)
{
	return NULL;	//暂不支持
}

void CMySwitchBar2::ClearAllItem()
{
	m_nLabelNum = 0;
}

BOOL CMySwitchBar2::AddItem(char *caption)
{
	if(m_nLabelNum >= MAX_SWTICH_NUM) return FALSE;
	strncpy(m_SwitchLabel[m_nLabelNum].caption,caption,48);
	m_SwitchLabel[m_nLabelNum].caption[48]=0;
	m_nLabelNum++;
	return TRUE;
}

void CMySwitchBar2::SetCurSel(int whichFl)
{
	if(whichFl < 0 || whichFl >= m_nLabelNum)
		whichFl = -1;
	m_nCurLabel = whichFl;
	DrawIt();
}

int CMySwitchBar2::GetCurSel()
{
	return m_nCurLabel;
}

void CMySwitchBar2::DrawInfo(CDC *pDC)
{
	CRect rc,tmpRc;
	GetClientRect(&rc);
	int barlen = rc.Width();
	int barheight = rc.Height();
	g_GUI.DrawGradient(pDC,CRect(0,0,barlen,barheight),RGB(218,218,218),RGB(235,235,235),TRUE);
	g_d.DotLine(pDC,2,0,2,barheight,RGB(64,64,64),2);
	CFont *pOldFont = g_d.SelectFont(pDC, BOLDGRAPH_FONT);
	g_d.DisplayText(pDC,6,2,RGB(0,0,0),_F(g_RightStr));
	g_d.RestoreFont(pDC, pOldFont);
}

void CMySwitchBar2::DrawOEM(CDC *pDC)
{
	int i;
	CRect rc,tmpRc;
	GetClientRect(&rc);
	int barlen = rc.Width();
	int barheight = rc.Height();

	COLORREF s_SwitchColor[7];
	for(i=0;i<7;i++)
		s_SwitchColor[i]=g_GUI.GetColorCfgFromResource(g_resModule,"IDR_COLOR","CMySwitchBar2",i);
	g_GUI.DrawGradient(pDC,CRect(0,0,barlen,barheight),s_SwitchColor[0],s_SwitchColor[1],TRUE);
	if(m_nLabelNum < 1)
		return;
	CFont *font = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	CFont *pOldFont = pDC->SelectObject(font);
	CSize size = pDC->GetTextExtent("上");
	int space=2;
	int width = (barlen-space*(m_nLabelNum-1))/m_nLabelNum;
	width = max(width,m_nMinWidth);
	width = min(width,m_nMaxWidth);
	for(i=0;i < m_nLabelNum;i++)
	{
		if(i==m_nCurLabel)
			pDC->SetTextColor(s_SwitchColor[2]);
		else
			pDC->SetTextColor(s_SwitchColor[3]);
		tmpRc.left=(width+space)*i;
		tmpRc.top=0;
		tmpRc.right=tmpRc.left+width;
		tmpRc.bottom=barheight;

		if(i==m_nCurLabel)
		{
			g_GUI.DrawLine(pDC,tmpRc.left+1,tmpRc.top,tmpRc.right,tmpRc.top,SRGB(132));
			g_GUI.DrawLine(pDC,tmpRc.left+2,tmpRc.top+1,tmpRc.right,tmpRc.top+1,SRGB(141));
			g_GUI.DrawLine(pDC,tmpRc.left,tmpRc.top+1,tmpRc.left,tmpRc.bottom-1,SRGB(132));
			g_GUI.DrawLine(pDC,tmpRc.left+1,tmpRc.top+1,tmpRc.left+1,tmpRc.bottom-1,SRGB(150));
			g_GUI.DrawLine(pDC,tmpRc.left+1,tmpRc.bottom-1,tmpRc.right-1,tmpRc.bottom-1,SRGB(215));
			g_GUI.DrawLine(pDC,tmpRc.left+1,tmpRc.bottom-2,tmpRc.right-2,tmpRc.bottom-2,SRGB(250));
			g_GUI.DrawLine(pDC,tmpRc.right-2,tmpRc.top+2,tmpRc.right-2,tmpRc.bottom-2,SRGB(241));
			g_GUI.DrawLine(pDC,tmpRc.right-1,tmpRc.top,tmpRc.right-1,tmpRc.bottom-1,SRGB(214));
			g_GUI.DrawGradient(pDC,CRect(tmpRc.left+2,tmpRc.top+2,tmpRc.right-2,tmpRc.bottom-2),s_SwitchColor[6],s_SwitchColor[5],FALSE);
			tmpRc.top+=2;
		}
		else
		{
			pDC->FrameRect(tmpRc,&CBrush(s_SwitchColor[4]));
			CRect tmpRc2=tmpRc;
			tmpRc2.DeflateRect(1,1,1,1);
			g_GUI.DrawGradient(pDC,tmpRc2,s_SwitchColor[5],s_SwitchColor[6],TRUE);
		}
		pDC->DrawText(m_SwitchLabel[i].caption,tmpRc,DT_VCENTER|DT_CENTER|DT_SINGLELINE);
	}
	pDC->SelectObject(pOldFont);
	font->DeleteObject();
}

void CMySwitchBar2::DrawNormal(CDC *pDC)
{
	int i;
	CRect rc,tmpRc;
	GetClientRect(&rc);
	int barlen = rc.Width();
	int barheight = rc.Height();

	COLORREF s_SwitchColor[6]={RGB(210,210,210),RGB(220,0,0),RGB(0,0,0),RGB(128,128,128),RGB(165,165,165),RGB(128,128,128)};
	g_d.Draw3DBar2(pDC, CRect(0,0,barlen,barheight), s_SwitchColor[0], FALSE);
//	pDC->FillSolidRect(0,0,barlen,barheight,s_SwitchColor[0]);
	if(m_nLabelNum < 1) 
		return;
	CFont *font = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	CFont *pOldFont = pDC->SelectObject(font);
	CSize size = pDC->GetTextExtent("上");
	int width = barlen/m_nLabelNum;
	width = max(width,m_nMinWidth);
	width = min(width,m_nMaxWidth);
	for(i=0;i < m_nLabelNum;i++)
	{
		if(i==m_nCurLabel)
			pDC->SetTextColor(s_SwitchColor[1]);
		else
			pDC->SetTextColor(s_SwitchColor[2]);
		tmpRc.left=width*i+3;
		tmpRc.top=5;
		tmpRc.right=width*i+width-3;
		tmpRc.bottom=barheight-3;
		if(i!=m_nCurLabel-1) 
			g_GUI.DrawLine(pDC,width*(i+1)-1,1,width*(i+1)-1,rc.Height()-4,s_SwitchColor[3]);
		if(i==m_nCurLabel)
		{
			g_GUI.DrawLine(pDC,width*i+1,0,width*i+1,rc.Height()-5,s_SwitchColor[3]);
			CRect tmpRc2=tmpRc;
			tmpRc2.InflateRect(1,4,3,-1);
			g_GUI.DrawGradient(pDC,tmpRc2,s_SwitchColor[4],s_SwitchColor[5],TRUE);
			g_GUI.DrawLine(pDC,tmpRc2.left,tmpRc2.bottom,tmpRc2.right-1,tmpRc2.bottom,s_SwitchColor[3]);
		}
		pDC->DrawText(m_SwitchLabel[i].caption,tmpRc,DT_TOP|DT_CENTER);
	}
	pDC->SelectObject(pOldFont);
	font->DeleteObject();
}

void CMySwitchBar2::DrawIt()
{
	CRect rc;
	GetClientRect(&rc);
	int barlen = rc.Width();
	int barheight = rc.Height();
	CDC *pDC = GetDC();
	CDC m_MyDC;
	CBitmap m_MyBitmap,*m_pOldBitmap;
	m_MyDC.CreateCompatibleDC(pDC);
	m_MyBitmap.CreateCompatibleBitmap(pDC,barlen,barheight);
	m_pOldBitmap = m_MyDC.SelectObject(&m_MyBitmap);
	m_MyDC.SetBkMode(TRANSPARENT);	
	if(g_OEM.bGdzqEmbedWTVersion)
		DrawInfo(&m_MyDC);
	else if(g_OEM.bEmbedWTVersion)
		DrawOEM(&m_MyDC);
	else
		DrawNormal(&m_MyDC);
	pDC->BitBlt (0,0,barlen,barheight,&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
	m_MyDC.SelectObject(m_pOldBitmap);
	m_MyBitmap.DeleteObject();
	m_MyDC.DeleteDC();

	ReleaseDC(pDC);
}

BOOL CMySwitchBar2::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CMySwitchBar2::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	DrawIt();
}

void CMySwitchBar2::OnPaint() 
{
	DrawIt();		
	CWnd::OnPaint();
}

void CMySwitchBar2::OnClick(CPoint point)
{
	if(g_OEM.bGdzqEmbedWTVersion) 
		return;

	CRect rc;
	GetClientRect(&rc);
	int barlen = rc.Width();
	int width = barlen/m_nLabelNum;
	width = max(width,m_nMinWidth);
	width = min(width,m_nMaxWidth);
	int iNo = point.x/width;
	if(iNo < 0 || iNo >= m_nLabelNum) return;
	if(m_nCurLabel != iNo)
	{
		m_nCurLabel = iNo;
		::SendMessage(m_MsghWnd,m_SelChangeMsg,m_nCurLabel,0);
	}
}

void CMySwitchBar2::OnLButtonDown(UINT nFlags, CPoint point) 
{
	OnClick(point);
}

void CMySwitchBar2::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);	
}

void CMySwitchBar2::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnClick(point);
}

void CMySwitchBar2::OnRButtonDown(UINT nFlags, CPoint point) 
{
	OnClick(point);
}

void CMySwitchBar2::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	OnClick(point);
}

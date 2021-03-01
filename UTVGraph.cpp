#include "stdafx.h"
#include "UTVGraph.h"
#include "ComView.h"
#include "LinkStkDlg.h"

UTVGraph::UTVGraph(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_TVIconRect.SetRectEmpty();

	m_bHasJGData = FALSE;
	m_nLinkGPNum = 0;
}

UTVGraph::~UTVGraph()
{
}

int  UTVGraph::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_nCurGP==-1) //如果Unit为空，则另行处理
	{
		if(message==WM_PAINT)
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
			return 1;
		}
		return UBase::ProcessMsg(message,wParam,lParam);
	}
	switch(message)
	{
	case UM_SET_STOCK:
		UBase::ProcessMsg(message,wParam,lParam);
		ProcessMsg(UM_READDATA);
		return 1;
	case UM_READDATA:
		ReadData();
		return 1;
	case WM_TIMER:
		return 1;
	case WM_PAINT:
		{
			if(wParam == 1)
				ProcessMsg(UM_READDATA,lParam);
			
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			m_MyDC.SetBkMode(TRANSPARENT);

			CFont *oldfont = g_d.SelectFont(&m_MyDC,SMALL2_FONT);
			CPen *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);

			g_d.DrawLine(&m_MyDC,0,0,0,m_rcIn.bottom);
			if(g_GH.bLeftHQInfo && !m_nInCom2)
				g_d.DrawLine(&m_MyDC,m_rcIn.right-1,0,m_rcIn.right-1,m_rcIn.bottom);
			g_d.DrawLine(&m_MyDC,0,m_rcIn.bottom,m_rcIn.right,m_rcIn.bottom);
			DrawIt(&m_MyDC);

			g_d.RestorePen(&m_MyDC,oldpen);
			g_d.RestoreFont(&m_MyDC,oldfont);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
		}
		return 1;
	case WM_MOUSEMOVE:
		{
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			pt.x-=left;
			pt.y-=top;
			if(m_TVIconRect.PtInRect(pt))
			{
				SetCursor(m_hFxtMove);
				return 1;
			}
			else
				SetCursor(LoadCursor(NULL,IDC_ARROW));
		}
		return 1;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void UTVGraph::ReadData()
{
	if(!m_bLevel2Property)
		return;
	//;;
	m_bHasJGData = FALSE;
	m_nLinkGPNum = g_pStockIO->GetRLTData(m_pStkInfo, m_LinkData, 100);
}

void UTVGraph::DrawIt(CDC *pDC)
{
	if(!g_bHasTopView)
		return;
	DrawFrame(pDC);
	//;;
}

void UTVGraph::DrawFrame(CDC *pDC)
{	
	//画标题区
	int XStart=max(0,Width()-g_nFixedWidth);
	if(g_GH.bLeftHQInfo)
		XStart=0;
	if(m_bHasJGData) 
		pDC->DrawIcon(XStart+2,4,m_hIcon_Pu[5]);
	else if(m_nGxdFlag>=0)
		pDC->DrawIcon(XStart+2,4,m_hIcon_Pu[7]);
	if(m_nLinkGPNum>0) 
		pDC->DrawIcon(XStart+18,4,m_hIcon_Pu[4]);
	int nFixedWidth=m_rcIn.right;
	int YFrom=6;
	CFont *oldfont = g_d.SelectFont(pDC,MEDIUM_FONT);	
	CSize tmpSize = g_d.DisplayText(pDC,XStart+36,YFrom,VipColor.BIGTITLECOLOR,"%6s ",m_pStkInfo->Code);
	CSize zs = g_d.DisplaySameWidtchText(pDC,XStart+36+tmpSize.cx,YFrom,GetBlockGPColor(m_pStkInfo,VipColor.BIGTITLECOLOR),"%s",m_pStkInfo->Name);
	DrawMarkFlag(m_pStkInfo,pDC,XStart+36+tmpSize.cx+zs.cx,YFrom-3,left,top,m_pView);

	if(!m_nInCom2)
	{
		g_d.SelectFont(pDC,BOLDGRAPH_FONT);
		tmpSize=g_d.DisplayText(pDC,XStart+g_nFixedWidth-28,YFrom,RGB(192,0,192),_F("返回"));
		m_TVIconRect.SetRect(XStart+g_nFixedWidth-50,YFrom,XStart+g_nFixedWidth,YFrom+20);
	}
	g_d.RestoreFont(pDC,oldfont);
}

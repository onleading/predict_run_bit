#include "stdafx.h"
#include "UTCancel.h"
#include "ComView.h"

UTCancel::UTCancel(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_bDataReady = FALSE;
	m_bEmpty = TRUE;
	m_nZoomFlag = 0;
	m_fltype = 1;
	m_setcode = SH;
	for(int i=0;i<10;i++)
	{
		m_BuyRect[i].SetRectEmpty();
		m_SellRect[i].SetRectEmpty();
	}
}

UTCancel::~UTCancel()
{

}

int  UTCancel::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
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
	case UM_READDATA:
		m_bDataReady = FALSE;
		ReadData(FALSE,wParam==0);
		return 1;
	case UM_GETDATA_ACK://对于远程版,请求和返回分开
		switch(wParam)
		{
		case 0:	
			m_bDataReady = TRUE;
			ReadData(TRUE);
			break;
		}
		ProcessMsg(WM_PAINT);
		return 1;
	case WM_TIMER:
		if(wParam==0x1113)
			ProcessMsg(UM_READDATA);
		return 1;
	case WM_PAINT:
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			m_MyDC.SetBkMode(TRANSPARENT);

			CFont *oldfont = g_d.SelectFont(&m_MyDC,SMALL_FONT);
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
			pt.x-=left;pt.y-=top;
			int n,i=-1,j=-1;
			for(n=0;n<10;n++)
			{
				if(m_SellRect[n].PtInRect(pt))
				{i=n;break;}
				if(m_BuyRect[n].PtInRect(pt))
				{j=n;break;}
			}
			if(i>=0 || j>=0)
			{
				SetCursor(m_hFxtMove);
				return 1;
			}
			else
				SetCursor(LoadCursor(NULL,IDC_ARROW));
		}
		return 1;
	case WM_LBUTTONDOWN:
		{
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			pt.x-=left;pt.y-=top;
			int n,i=-1,j=-1;
			for(n=0;n<10;n++)
			{
				if(m_SellRect[n].PtInRect(pt))
				{i=n;break;}
				if(m_BuyRect[n].PtInRect(pt))
				{j=n;break;}
			}
			MemStkInfo *tmpInfo=NULL;
			if(i>=0)
				tmpInfo=g_pStockIO->Get_MemStkInfo(g_Mtl.TotalCancelRankings[i].Code,m_setcode);
			else if(j>=0)
				tmpInfo=g_pStockIO->Get_MemStkInfo(g_Mtl.TotalCancelRankings[10+j].Code,m_setcode);
			if(tmpInfo)
				ToZst(tmpInfo->Code,tmpInfo->setcode,FALSE);
		}
		return 1;
	case WM_LBUTTONDBLCLK:
		{
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			pt.x-=left;pt.y-=top;
			int n,i=-1,j=-1;
			for(n=0;n<10;n++)
			{
				if(m_SellRect[n].PtInRect(pt))
				{i=n;break;}
				if(m_BuyRect[n].PtInRect(pt))
				{j=n;break;}
			}
			if(i<0 && j<0)
			{
				if(m_nZoomFlag==0)
				{
					if(pt.y<m_rcIn.Height()/2)	m_nZoomFlag=1;
					else						m_nZoomFlag=2;
				}
				else
					m_nZoomFlag=0;
			}
			ProcessMsg(WM_PAINT);
		}
		return 1;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void UTCancel::ReadData(BOOL bDataReady,BOOL bReqData)
{
	if(!OnLine || !CanLevel2())
		return;
	if(!bDataReady)
	{
		if(bReqData)
		{
			m_pView->SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
			g_nAssisID = SetComAssisID(m_nWhichUnit,0);
			g_pGetData->GetTCancelRank(m_setcode,m_fltype);
		}
	}
	else
		g_pGetData->GetTCancelRankACK(g_Mtl.TotalCancelRankings);
}

void UTCancel::DrawPart1(CDC *pDC,int TotalHeight,int TextH,int XWidth,int XFrom[],int YFrom)
{
	if(TotalHeight<2*TextH) return;
	g_d.DisplayText(pDC,XFrom[0],YFrom,VipColor.TXTCOLOR,_F("委卖撤单"));
	g_d.DisplayText(pDC,XFrom[2],YFrom,VipColor.TXTCOLOR,XWidth,_F("撤单量"));	
	YFrom+=TextH;
	g_d.DotLine(pDC,0,YFrom-1,m_rcIn.right,YFrom-1,VipColor.AXISCOLOR,2);
	YFrom+=1;
	int i,row=(TotalHeight-1-TextH)/TextH;
	MemStkInfo *tmpInfo;
	CSize size;
	for(i=0;i<min(10,row);i++)
	{
		tmpInfo=g_pStockIO->Get_MemStkInfo(g_Mtl.TotalCancelRankings[i].Code,m_setcode);
		if(tmpInfo)
		{
			size=g_d.DisplaySameWidtchText(pDC,XFrom[0],YFrom,GetBlockGPColor(tmpInfo,GridColor.Grid_CodeName_Color),tmpInfo->Name);
			m_SellRect[i].SetRect(XFrom[0],YFrom,XFrom[0]+size.cx,YFrom+TextH-2);
			g_d.DisplayText(pDC,XFrom[2],YFrom,VipColor.VOLCOLOR,XWidth,"%.0f",g_Mtl.TotalCancelRankings[i].TotalWithdrawnQty/tmpInfo->Unit);
			YFrom+=TextH;
		}		
	}
}

void UTCancel::DrawPart2(CDC *pDC,int TotalHeight,int TextH,int XWidth,int XFrom[],int YFrom)
{
	if(TotalHeight<2*TextH) return;
	g_d.DisplayText(pDC,XFrom[0],YFrom,VipColor.TXTCOLOR,_F("委买撤单"));
	g_d.DisplayText(pDC,XFrom[2],YFrom,VipColor.TXTCOLOR,XWidth,_F("撤单量"));
	YFrom+=TextH;
	g_d.DotLine(pDC,0,YFrom-1,m_rcIn.right,YFrom-1,VipColor.AXISCOLOR,2);
	YFrom+=1;
	int i,row=(TotalHeight-1-TextH)/TextH;
	MemStkInfo *tmpInfo;
	CSize size;
	for(i=0;i<min(10,row);i++)
	{
		tmpInfo=g_pStockIO->Get_MemStkInfo(g_Mtl.TotalCancelRankings[10+i].Code,m_setcode);
		if(tmpInfo)
		{
			size=g_d.DisplaySameWidtchText(pDC,XFrom[0],YFrom,GetBlockGPColor(tmpInfo,GridColor.Grid_CodeName_Color),tmpInfo->Name);
			m_BuyRect[i].SetRect(XFrom[0],YFrom,XFrom[0]+size.cx,YFrom+TextH-2);
			g_d.DisplayText(pDC,XFrom[2],YFrom,VipColor.VOLCOLOR,XWidth,"%.0f",g_Mtl.TotalCancelRankings[10+i].TotalWithdrawnQty/tmpInfo->Unit);
			YFrom+=TextH;
		}		
	}
}

void UTCancel::DrawIt(CDC *pDC)
{
	if(Height() < 20) return;	//如果空间太小，不画
	if(m_bZs || m_bDSFlag) return;
	for(int i=0;i<10;i++)
	{
		m_BuyRect[i].SetRectEmpty();
		m_SellRect[i].SetRectEmpty();
	}
	int nFixedWidth=m_rcIn.right;
	int TextH=HQ_TEXTH-2,YFrom=2,XFrom[3];
	XFrom[0]=HQ_XSPACE;
	XFrom[1]=(nFixedWidth-2*HQ_XSPACE)/3+HQ_XSPACE;
	XFrom[2]=(nFixedWidth-2*HQ_XSPACE)*2/3+HQ_XSPACE;
	int XWidth=(nFixedWidth-2*HQ_XSPACE)/3;
	if(m_nZoomFlag==1)
		DrawPart1(pDC,m_rcIn.bottom,TextH,XWidth,XFrom,YFrom);
	else if(m_nZoomFlag==2)
		DrawPart2(pDC,m_rcIn.bottom,TextH,XWidth,XFrom,YFrom);
	else
	{
		DrawPart1(pDC,m_rcIn.bottom/2,TextH,XWidth,XFrom,YFrom);
		g_d.DrawLine(pDC,0,m_rcIn.bottom/2,m_rcIn.right,m_rcIn.bottom/2);
		YFrom=m_rcIn.bottom/2+2;
		DrawPart2(pDC,m_rcIn.bottom/2,TextH,XWidth,XFrom,YFrom);
	}
}
